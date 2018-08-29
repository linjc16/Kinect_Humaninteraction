using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Ports;
using System.Threading;
using Microsoft.Kinect;

namespace KinectHost
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        SerialDataReceivedEventHandler MySerialRecv;

        private SerialPort myPort;
        private string[] ports;
        private int[] baudrates = { 9600, 19200, 38400, 57600, 115200 };

        private KinectSensor kinectDevice;
        private DepthFrameReader depthFrameReader;
        private FrameDescription depthFrameDescription;
        private BodyFrameReader bodyFrameReader;

        private Joint[] bodyParts;
        private int rightHand = 0;
        private double[] angles;
        private double[] angle0, angle1, angle2;
        private long tmr = 0;
        private long filterLength = 5;

        int status = 0;

        public MainWindow()
        {
            // Init window 
            InitializeComponent();

            // Init Kinect
            kinectDevice = KinectSensor.GetDefault();
            depthFrameReader = kinectDevice.DepthFrameSource.OpenReader();
            depthFrameDescription = kinectDevice.DepthFrameSource.FrameDescription;
            bodyFrameReader = kinectDevice.BodyFrameSource.OpenReader();

            // Init Serialport
            myPort = new SerialPort();
            MySerialRecv = new SerialDataReceivedEventHandler(OnDataReceived);

            // Init detection
            bodyParts = new Joint[5];
            angles = new double[3];
            angle0 = new double[filterLength];
            angle1 = new double[filterLength];
            angle2 = new double[filterLength];

            // Init window widgets
            btnClear.IsEnabled = false;
            btnClosePort.IsEnabled = false;
            btnOpenPort.IsEnabled = false;
            btnSend.IsEnabled = false;
            btnCloseCam.IsEnabled = false;
            lblAng0.Content = "Angle0 = ";
            lblAng1.Content = "Angle1 = ";
            lblAng2.Content = "Angle2 = ";
            lblRHS.Content = "State = ";
            
            // Serialport Options Init
            foreach (int bdr in baudrates)
            {
                cbBRate.Items.Add(bdr.ToString());
            }

            cbBRate.SelectedIndex = 0;

            ports = SerialPort.GetPortNames();

            if (ports.Length > 0)
            {
                cbPortNO.SelectedIndex = 0;
                btnOpenPort.IsEnabled = true;
            }
            else
            {
                cbPortNO.SelectedIndex = -1;
            }

            Array.Sort(ports);

            foreach(string s in ports) cbPortNO.Items.Add(s);
            
        }

        private void OnOpenPortClicked(object sender, RoutedEventArgs e)
        {
            if (!myPort.IsOpen)
            {
                // Set properties
                cbPortNO.IsEnabled = false;
                cbBRate.IsEnabled = false;
                tbReceive.IsEnabled = true;
                tbTransmit.IsEnabled = true;

                btnOpenPort.IsEnabled = false;
                btnClosePort.IsEnabled = true;
                btnSend.IsEnabled = false;
                btnClear.IsEnabled = true;
                
                // Pick port and BdRate
                myPort.PortName = cbPortNO.SelectedItem.ToString();
                myPort.BaudRate = baudrates[cbBRate.SelectedIndex];

                // Open Port
                try
                {
                    myPort.Open();
                    myPort.DataReceived += MySerialRecv;
                }
                catch(Exception)
                {
                    myPort = new SerialPort();
                    lblInfo.Content = "端口打开失败";
                }

                lblInfo.Content = "端口打开成功";
                btnSS.IsEnabled = true;
            }
        }

        private void OnClosePortClicked(object sender, RoutedEventArgs e)
        {
            if (myPort.IsOpen)
            {
                // Set Properties
                cbPortNO.IsEnabled = true;
                cbBRate.IsEnabled = true;
                tbReceive.IsEnabled = false;
                tbTransmit.IsEnabled = false;

                btnClosePort.IsEnabled = false;
                btnSend.IsEnabled = false;
                btnClear.IsEnabled = false;
                btnOpenPort.IsEnabled = true;

                // Close Port
                myPort.Close();
                myPort.DataReceived -= MySerialRecv;

                // Clear UI
                tbTransmit.Clear();
                tbReceive.Clear();

                lblInfo.Content = "端口关闭成功";
            }
        }

        private void OnTransmitTextChanged(object sender, TextChangedEventArgs e)
        {
            // Prohibit null send
            if(tbTransmit.Text == "") btnSend.IsEnabled = false;
            else btnSend.IsEnabled = true;
        }

        private void OnDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // Get count to read
            int count = myPort.BytesToRead;
            byte[] buffer = new byte[count];

            // Read all
            myPort.Read(buffer, 0, count);
            
            // Make string 
            string textApp = Encoding.Default.GetString(buffer);

            // Invoke update UI
            Dispatcher.BeginInvoke(new Action<TextBox, string>(UpdateTb), tbReceive, textApp);

            myPort.DiscardInBuffer();
        }

        private void OnClearClicked(object sender, RoutedEventArgs e)
        {
            // Clear UI
            tbReceive.Text = "";
        }

        private void OnSendClicked(object sender, RoutedEventArgs e)
        {
            // Send the text out 
            try
            {
                myPort.Write(tbTransmit.Text);
            }
            catch(Exception)
            {
                lblInfo.Content = "发送失败";
            }
            lblInfo.Content = "发送成功";
        }

        private void UpdateTb(TextBox tb, string text)
        {
            // Update the receive box
            tb.AppendText(text);

            string recv = tb.Text;

            if (recv.Length <= 2) return;

            // Check "GG" request when start
            if (recv[recv.Length - 1] == 'G' &&
                recv[recv.Length - 2] == 'G' && status == 1)
            {
                byte[] toSend = new byte[4];
                toSend[0] = Convert.ToByte(0xA0 | rightHand);
                toSend[1] = Convert.ToByte(angles[0] * 256 / 300);
                toSend[2] = Convert.ToByte(angles[1] * 256 / 300);
                toSend[3] = Convert.ToByte(angles[2] * 256 / 300);

                myPort.Write(toSend, 0, 4);

                tb.Text = "";
            }
        }

        private void UpdateBodyPart()
        {
            // Show information from kinect
            lblAng0.Content = string.Format("Angle0 = {0:F2}", angles[0]);
            lblAng1.Content = string.Format("Angle1 = {0:F2}", angles[1]);
            lblAng2.Content = string.Format("Angle2 = {0:F2}", angles[2]);
            lblRHS.Content = "State = " + (rightHand == 1 ? "Open" : "Close");
        }

        private void OnBodyFrameArrived(object sender, BodyFrameArrivedEventArgs e)
        {
            using (BodyFrame bf = e.FrameReference.AcquireFrame())
            {
                if (bf != null)
                {
                    Body[] bodies = new Body[bf.BodyCount];
                    bf.GetAndRefreshBodyData(bodies);
                    foreach (Body body in bodies)
                    {
                        if (body.IsTracked && body != null)
                        {
                            // Record the joints needed
                            bodyParts[0] = body.Joints[JointType.ShoulderRight];
                            bodyParts[1] = body.Joints[JointType.ElbowRight];
                            bodyParts[2] = body.Joints[JointType.WristRight];
                            bodyParts[3] = body.Joints[JointType.HandTipRight];
                            bodyParts[4] = body.Joints[JointType.SpineShoulder];

                            if (tmr < filterLength)
                            {
                                // Caculate Angle and modify
                                double tmp0 = CalcAngleVertical(bodyParts[1], bodyParts[0]) / Math.PI * 180;
                                tmp0 = tmp0 > 0 ? 270 - tmp0 : -90 - tmp0;
                                tmp0 += 150;
                                if (tmp0 < 0) tmp0 = 0;

                                double tmp1 = CalcAngle(Subtract(bodyParts[0], bodyParts[1]), Subtract(bodyParts[2], bodyParts[1])) / Math.PI * 180;

                                double tmp2 = CalcAngleLean(bodyParts[1], bodyParts[0]) / Math.PI * 180;
                                tmp2 += 90;
                                if (tmp2 < 0) tmp2 = 0;
                                if (tmp2 > 255) tmp2 = 0;

                                angle0[tmr] = tmp0;
                                angle1[tmr] = tmp1;
                                angle2[tmr] = tmp2;
                                
                                ++tmr;
                            }
                            else
                            {
                                // Averaging
                                angles[0] = 0;
                                angles[1] = 0;
                                angles[2] = 0;
                                angles[3] = 0;

                                for(int i = 0; i < filterLength; ++i)
                                {
                                    angles[0] += angle0[i];
                                    angles[1] += angle1[i];
                                    angles[2] += angle2[i];
                                }

                                angles[0] /= filterLength;
                                angles[1] /= filterLength;
                                angles[2] /= filterLength;

                                // Set hand
                                if (body.HandRightState == HandState.Open && 
                                    body.HandRightConfidence == TrackingConfidence.High) rightHand = 1;

                                if(body.HandRightState == HandState.Closed && body.HandRightConfidence == TrackingConfidence.High)
                                {
                                    rightHand = 0;
                                }

                                Dispatcher.BeginInvoke(new Action(UpdateBodyPart));

                                tmr = 0;
                            }
                        }
                    }
                }
            }
        }

        private void OnOpenCamClicked(object sender, RoutedEventArgs e)
        {
            bodyFrameReader.FrameArrived += OnBodyFrameArrived;

            btnCloseCam.IsEnabled = true;
            btnOpenCam.IsEnabled = false;

            try
            {
                kinectDevice.Open();
            }
            catch(Exception)
            {
                lblInfo.Content = "相机打开失败";
            }
            lblInfo.Content = "相机打开成功";
        }

        private void OnCloseCamClicked(object sender, RoutedEventArgs e)
        {
            // Open kinect
            btnOpenCam.IsEnabled = true;
            btnCloseCam.IsEnabled = false;

            kinectDevice.Close();

            lblAng0.Content = "Angle0 = ";
            lblAng1.Content = "Angle1 = ";
            lblAng2.Content = "Angle2 = ";
            lblRHS.Content = "State = ";   

            lblInfo.Content = "相机关闭成功";
        }

        private double CalcAngle(float []vec1, float[]vec2, int mode = 1)
        {
            // Calculate the angle
            if (vec1.Length == vec2.Length)
            {
                double innerProd = 0.0;
                double sqr1 = 0.0;
                double sqr2 = 0.0;

                for(int i = 0; i < vec1.Length; ++i)
                {
                    innerProd += vec1[i] * vec2[i];
                    sqr1 += vec1[i] * vec1[i];
                    sqr2 += vec2[i] * vec2[i];
                }

                if (Math.Abs(innerProd) < double.Epsilon) return 0;
                int sgn = 0;
                double dominator = Math.Sqrt(sqr1 * sqr2);

                if (innerProd > 0) sgn = 1;
                else sgn = -1;

                if (mode == 1)
                    return Math.Acos(innerProd / dominator);
                else
                    return sgn * Math.Acos(innerProd / dominator);
            }
            else return -2;
        }

        private float[] Subtract(Joint j1, Joint j2)
        {
            // Get vector
            float[] vecRett = new float[3];
            vecRett[0] = j1.Position.X - j2.Position.X;
            vecRett[1] = j1.Position.Y - j2.Position.Y;
            vecRett[2] = j1.Position.Z - j2.Position.Z;
            return vecRett;
        }

        private void OnSSClicked(object sender, RoutedEventArgs e)
        {
            if(status == 0)
            {
                // Open
                if (!myPort.IsOpen || !kinectDevice.IsOpen) return;

                try
                {
                    byte[] toSend = { Convert.ToByte(0x1D),0,0,0 };
                    Thread.Sleep(30);
                    myPort.Write(toSend, 0, 4);
                    lblInfo.Content = "启动成功";
                }
                catch(Exception)
                {
                    lblInfo.Content = "启动失败";
                }

                status = 1;
                btnSS.Content = "停止";
                tbTransmit.IsReadOnly = true;
                btnSend.IsEnabled = false;
                return;
            }
            if(status == 1)
            {
                //Close

                try
                {
                    byte[] toSend = { Convert.ToByte(0x1E),0,0,0 };
                    // Thread.Sleep(100);
                    myPort.Write(toSend, 0, 4);
                    lblInfo.Content = "停止成功";
                }
                catch (Exception)
                {
                    lblInfo.Content = "停止失败";
                }

                status = 0;
                btnSS.Content = "启动";
                tbTransmit.IsReadOnly = false;
                btnSend.IsEnabled = true;
                return;
            }
        }

        private double CalcAngleVertical(Joint j1, Joint j2)
        {
            double dy = j1.Position.Y - j2.Position.Y;
            double dz = j1.Position.Z - j2.Position.Z;
            if (Math.Abs(dy) < 0.1 && Math.Abs(dz) < 0.1) return 0;
            double res = Math.Atan2(dy, dz);
            
            return res;
        }

        private double CalcAngleLean(Joint j1, Joint j2)
        {
            double dy = j1.Position.Y - j2.Position.Y;
            double dx = j1.Position.X - j2.Position.X;
            
            return Math.Atan2(dy, dx);
        }
    }
}
