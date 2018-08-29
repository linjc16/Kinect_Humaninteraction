/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

// 2018.7.8. 15:09
#include <project.h>
#include "cyapicallbacks.h"



//请求信号
const unsigned char Answer2PC[8]="GG"; 

const float Kp = 0.01f;

int bufIdx = 0;
char buffer[100];

char bufferchar[100];
int bufferchar_len;

int state_onoff = 0;// 0 off, 1 on
char temp;

//PSoc向Robot的指令
uint8 Cmd2Robot[64];
uint8 AnsFromRobot[64];

unsigned short ID_read;
unsigned short Len_read;
unsigned short Blue_len_read;

const uint8 broadcastID = 0xFE;
const uint8 motorAmount = 3;
const uint8 quesCurrAngle = 1;
const uint8 quesCurrVelo = 2;


struct MotorInfo motorinfo[4];

//蓝牙读取
uint8 blue_char[64];  //temp变量
uint8 blue_get_char[64]; //实际得到

//当前问询号
uint8 quesnum = 0;
//修改ID
void change_id(uint8 old_id,uint8 new_id)
{
    Cmd2Robot[0] = 0xFF; 
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = old_id;  //ID
    Cmd2Robot[3] = 0x04;   //有效数据长度
    Cmd2Robot[4] = 0x03;    //WRITE DATA 指令
    Cmd2Robot[5] = 0x03;    //Address
    Cmd2Robot[6] = new_id;
    Cmd2Robot[7] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] + Cmd2Robot[6]);
    Cmd2Robot[8] = 0;
    UART_PutArray(Cmd2Robot,9);
}

//写延迟时间,100us为0x32
void writeDelayTime(uint8 id, uint8 delaytime)
{	
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x04;
    Cmd2Robot[4] = 0x03;
    Cmd2Robot[5] = 0x05;
    Cmd2Robot[6] = delaytime;

    char temp = 0x00;
    int i = 2;
    for(; i < 7 ; i++)
    {
        temp += Cmd2Robot[i];
    }
    Cmd2Robot[7] = ~temp;

    UART_PutArray(Cmd2Robot,8);
	if(id != broadcastID)
		motorinfo[id].delaytime = delaytime;
	else{
		for(i=0;i<4;i++)
			motorinfo[i].delaytime = delaytime;
	}
}


//逆时针角度限制（最大值限制）
void limit_ccw(uint8 id, uint16 angle)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;      //ID
    Cmd2Robot[3] = 0x05;    //Length
    Cmd2Robot[4] = 0x03;    //WRITE DATA
    Cmd2Robot[5] = 0x08;    //Address
    Cmd2Robot[6] = angle & 0xFF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] + Cmd2Robot[6] + Cmd2Robot[7]);
    Cmd2Robot[9] = 0;
    
    UART_PutArray(Cmd2Robot,10);
	if(id != broadcastID)
		motorinfo[id].ccw = angle;
	else{
        int i;
		for(i=0;i<4;i++)
			motorinfo[i].ccw = angle;
	}
}


//顺时针角度限制（最小值限制）
void limit_cw(uint8 id, uint16 angle)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;      //ID
    Cmd2Robot[3] = 0x05;    //Length
    Cmd2Robot[4] = 0x03;
    Cmd2Robot[5] = 0x06;
    Cmd2Robot[6] = angle & 0xFF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] + Cmd2Robot[6] + Cmd2Robot[7]);
    Cmd2Robot[9] = 0;
    
    UART_PutArray(Cmd2Robot,10);
	
	if(id != broadcastID)
		motorinfo[id].cw = angle;
	else{
        int i;
		for(i=0;i<4;i++)
			motorinfo[i].cw = angle;
	}
}

//写电压工作范围,参数为实际的10倍
//若想让低电压为6,则lowV=60
void writeVoltRange(uint8 id, uint8 lowV, uint8 highV)
{

    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x05;
    Cmd2Robot[4] = 0x03;
    Cmd2Robot[5] = 0x0C;
    Cmd2Robot[6] = lowV;
    Cmd2Robot[7] = highV;

    char temp = 0x00;
    int i = 2;
    for(; i < 8 ; i++)
    {
        temp += Cmd2Robot[i];
    }
    Cmd2Robot[8] = ~temp;

    UART_PutArray(Cmd2Robot,9);
}

//舵机运动
void motion(uint8 id, uint16 angle, uint16 speed)
{

    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x07;
    Cmd2Robot[4] = 0x03;
    Cmd2Robot[5] = 0x1E;
    Cmd2Robot[6] = angle & 0x00FF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = speed & 0x00FF;
    Cmd2Robot[9] = (speed & 0xFF00) >> 8;
    char temp = 0x00;
    int i = 2;
    for(; i < 10 ; i++)
    {
        temp += Cmd2Robot[i];
    }
    Cmd2Robot[10] = ~temp;

    UART_PutArray(Cmd2Robot,11);
	
	if(id != broadcastID){
		motorinfo[id].dest_angle = angle;
		motorinfo[id].dest_velo = speed;
	}
	else{
		for(i=0;i<4;i++){
			motorinfo[i].dest_angle = angle;
			motorinfo[i].dest_velo = speed;
		}
	}
}

//复位
void reset(uint8 id)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x02;
    Cmd2Robot[4] = 0x06;
    Cmd2Robot[5] = 0xF7;
    UART_PutArray(Cmd2Robot,6);
}

//同时调整两个舵机分别到某一角度(异步写入）
void Reg_Write_angle(uint8 id, uint16 angle, uint8 isInit)
{
    
    uint16 speed = 0x1FF;
    motorinfo[id].current_angle = motorinfo[id].dest_angle;
    motorinfo[id].dest_angle = angle;
    
    if(!isInit){        
        int16 diff = angle - motorinfo[id].current_angle;
        if(diff < 0) diff = -diff;
        speed += (diff - 256)/2;
        if(speed > 0x2FF) speed = 0x2FF;
    }
    
    
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x07; //length
    Cmd2Robot[4] = 0x04; //异步写入
    Cmd2Robot[5] = 0x1E;
    Cmd2Robot[6] = angle & 0xFF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = speed & 0xFF;
    Cmd2Robot[9] = (speed & 0xFF00) >> 8;
    
    Cmd2Robot[10] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] 
                    + Cmd2Robot[6] + Cmd2Robot[7] + Cmd2Robot[8] + Cmd2Robot[9]);
    /*
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x07; //length
    Cmd2Robot[4] = 0x04; //异步写入
    Cmd2Robot[5] = 0x1E;
    Cmd2Robot[6] = angle & 0xFF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = 0xFF;
    Cmd2Robot[9] = 1;
    
    Cmd2Robot[10] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] 
                    + Cmd2Robot[6] + Cmd2Robot[7] + Cmd2Robot[8] + Cmd2Robot[9]);
    */
    UART_PutArray(Cmd2Robot,11);        
}
//执行异步写指令
void action()
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = 0xFE; //广播
    Cmd2Robot[3] = 0x02;
    Cmd2Robot[4] = 0x05; //指令
    Cmd2Robot[5] = 0xFA;
    UART_PutArray(Cmd2Robot,6);
}

//加速度与减速度控制
void acc_ctl(uint8 id,uint8 p_acc, uint8 m_acc)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x05; //length
    Cmd2Robot[4] = 0x03;//写入指令
    Cmd2Robot[5] = 0x30;
    Cmd2Robot[6] = p_acc;
    Cmd2Robot[7] = m_acc;
    Cmd2Robot[8] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] + Cmd2Robot[6] + Cmd2Robot[7]); 
    UART_PutArray(Cmd2Robot,9);
	
	if(id != broadcastID){
		motorinfo[id].plus_acc = p_acc;
		motorinfo[id].minus_acc = m_acc;
	}
	else{
        int i;
		for(i=0;i<4;i++){
			motorinfo[i].plus_acc = p_acc;
			motorinfo[i].minus_acc = m_acc;
		}
	}
}

//读取当前位置
void read_current_position(uint8 id)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x04;  //length
    Cmd2Robot[4] = 0x02; //读指令
    Cmd2Robot[5] = 0x24;
    Cmd2Robot[6] = 0x02; //读取数据长度
    Cmd2Robot[7] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] +Cmd2Robot[5] + Cmd2Robot[6]);
    quesnum = 1;
    UART_PutArray(Cmd2Robot,8);    
}
//读当前速度
void read_current_v(uint8 id)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x04; //length
    Cmd2Robot[4] = 0x02; //读指令
    Cmd2Robot[5] = 0x26;
    Cmd2Robot[6] = 0x02;
    Cmd2Robot[7] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] +Cmd2Robot[5] + Cmd2Robot[6]);
    quesnum = 2;
    UART_PutArray(Cmd2Robot,8);
}
/*
//REG_WRITE标志
void read_REG_WRITE_FLGA()
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = 

}*/


//向蓝牙打印错误
void printError(uint8 id, uint8 errornum){
    char msg[10] = " error :";
    msg[6] = id+'0';
    UART_Blue_PutString(msg);
    CyDelay(100);
    if(errornum & 1) {
        UART_Blue_PutString(" Volt error");
        CyDelay(100);
    }    
    if(errornum & 2){
        UART_Blue_PutString(" angle error");
        CyDelay(100);
    }
    if(errornum & 4){
        UART_Blue_PutString(" wendu error");
        CyDelay(100);
    }
    if(errornum & 8 || errornum & 0x40){
        UART_Blue_PutString(" instru error");
        CyDelay(100);
    }
    if(errornum & 0x10){
        UART_Blue_PutString(" checknum error");
        CyDelay(100);
    }
    if(errornum & 0x20){
        UART_Blue_PutString(" overload error");
        CyDelay(100);
    }
}

//有用的变量UART传输中断用到
uint8 ccnm;
//蓝牙传输中断用到
uint8 ccnm_temp = 0;




//正常则返回0,否则返回error值
int ParseAnswer(){
    
    uint8 current = 0;
    uint8 len;
    uint8 error;
    
    UART_Blue_PutString(AnsFromRobot);
    
    CyDelay(20);
    
    uint16 temp1;
    while(AnsFromRobot[current] == 0xFF) current++;   
    ID_read = AnsFromRobot[current];
    current ++;
    len = AnsFromRobot[current];
    current ++;
    
    error = AnsFromRobot[current];
    
    if(quesnum == quesCurrAngle){
        current += 2;
        motorinfo[ID_read].current_angle = AnsFromRobot[current];
        motorinfo[ID_read].current_angle = motorinfo[ID_read].current_angle << 8;
        current --;
        motorinfo[ID_read].current_angle += AnsFromRobot[current];
        
        if(motorinfo[ID_read].current_velo > 0x7FFF) motorinfo[ID_read].current_velo = 0;
        else if(motorinfo[ID_read].current_velo > 0x3FF) motorinfo[ID_read].current_velo = 0x3FF;
    }
    
    else if(quesnum == quesCurrVelo){
        current += 2;
        motorinfo[ID_read].current_velo = AnsFromRobot[current];
        motorinfo[ID_read].current_velo = motorinfo[ID_read].current_velo << 8;
        current --;
        motorinfo[ID_read].current_velo += AnsFromRobot[current];
    }    
    

    
    //if(error) printError(ID_read, error);
    return error;
}
int count = 0;
int getall_from_robot = 0 ;
//获取机器发回的编码（草泥马的中断）
uint8 cnmtmp[64];
void get_AnsRromRobot()
{
    cnmtmp[count] = ccnm;
    //UART_Blue_PutChar(ccnm);
    count = (count + 1) % 8;
    if(count == 0)
    {
      
      quesnum = 1; //要不要删--------------------------------------------------------
      getall_from_robot = 1;
     UART_Blue_PutArray(cnmtmp,8);     
     if(quesnum == quesCurrAngle || quesnum == quesCurrVelo)
     ParseAnswer(cnmtmp);   
    }
}


//初始化motorinfo和实际motor参数
//回复延迟时间100us
//0-2号舵机0-0x3FF,3号舵机角度430-600
//目标速度0x1FF,目标角度0x1FF
//加速度减速度都为0
//当前速度0，角度512
void initial_Motor_Info(){
    int id = 0;
    for(;id<4;id++){
		motorinfo[id].Id = id;
		//motorinfo[id].delaytime = 100;
		//motorinfo[id].cw = 0;
		//motorinfo[id].ccw = 0x3FF;
		//motorinfo[id].dest_angle = 0x1FF;
		//motorinfo[id].dest_velo = 0x1FF;
		//motorinfo[id].plus_acc = 0;
		//motorinfo[id].minus_acc = 0;
		
		motorinfo[id].current_angle = 0;
		motorinfo[id].current_velo = 0;
	}
	
 /*   //--------------------------------set_angle_lim-----------------------------
    //0号舵机角度motor0:0-300
    CyDelay(5);
    limit_ccw(0,300);
    CyDelay(5);
    limit_cw(0,0);
    CyDelay(5);
    //1号舵机角度motor1:220-802
    limit_ccw(1,802);
    CyDelay(5);
    limit_cw(1,220);
    CyDelay(5);
    //2号舵机
    limit_ccw(2,480);
    CyDelay(5);
    limit_cw(2,0);
    CyDelay(5);
    */
    //3号舵机角度370-600
	limit_ccw(3, 600);
    CyDelay(5);
	limit_cw(3, 370);
    CyDelay(5); 
    
    
    
    //----------------------------broadcast all---------------------------------------
	//broadcast
    CyDelay(10);
	writeDelayTime(broadcastID, 0x32);
    
    //CyDelay(5);
	//limit_ccw(broadcastID, 0x3FF);
	//CyDelay(5);
    //limit_cw(broadcastID, 0);
    //CyDelay(5);
    
    CyDelay(10);
    writeVoltRange(broadcastID,60,120);
	CyDelay(10);
    
    //motion(broadcastID, 0, 0x1FF);
	//CyDelay(5);
    
    acc_ctl(broadcastID,0,0);
	CyDelay(10);
    
    
    
    
    //------------------------------motion------------------------------------------
     
    CyDelay(20);
    Reg_Write_angle(0,0,1);
    //motion(0,0,0x1ff);
    CyDelay(20);
    Reg_Write_angle(1,0x1ff,1);
    CyDelay(20);
    Reg_Write_angle(2,0,1);
    CyDelay(20);
    Reg_Write_angle(3,400,1);
    CyDelay(50);        
    action();
    CyDelay(50);  

    
}

//--------------------------UART中断---------------------------
CY_ISR(isr_rx_2_Interrupt)
{
    #ifdef isr_rx_2_INTERRUPT_INTERRUPT_CALLBACK
        isr_rx_2_Interrupt_InterruptCallback();
    #endif /* isr_rx_2_INTERRUPT_INTERRUPT_CALLBACK */ 

    /*  Place your Interrupt code here. */
    /* `#START isr_rx_2_Interrupt` */
/*
    Len_read = 0;
    while(UART_ReadRxStatus() == UART_RX_STS_FIFO_NOTEMPTY) 
    {
        //buffer[bufIdx++] = UART_GetChar();
        //if(bufIdx == 100) bufIdx = 0;
        
        char a;
        a = UART_GetChar();
        //UART_PutChar(a);

        AnsFromRobot[Len_read] = a;
        Len_read ++;

       
      //  bufferchar[bufferchar_len] = UART_GetChar();
     //   bufferchar_len++;
        
      //  UART_PutString(bufferchar);
    } 
    AnsFromRobot[Len_read] = 0;
    ccnm = AnsFromRobot[0];
   // UART_Blue_PutArray(AnsFromRobot,Len_read);
   // if(quesnum == quesCurrAngle || quesnum == quesCurrVelo)
   // ParseAnswer(AnsFromRobot);
    get_AnsRromRobot();
*/
}


//-------------------------- kinect 接口--------------------------------------//
uint8 command_from_kinect[64];
uint16 angle_ki[4];

int hands_open = -1; //0为手开着，1为手关着

//Kinect复原reset
void kinect_reset()
{
    int i = 0;
   for( ; i < 64 ; i++)
   {
        command_from_kinect[i] = 0;
    }
}
//解析kinect指令
void Read_from_kinect()
{
    //UART_Blue_PutArray(command_from_kinect,4);
   if(command_from_kinect[0] == 0x1D)
  {
      state_onoff = 1;
      int i = 0;
   /* for (; i < 3 ; i++)
    {
      motion(i,0x1FF,0x1FF);
      CyDelay(1000);
      motion(i,0,0x1FF);
      CyDelay(100);
    }
    if(i == 3)
    {
       motion(3,0x1AE,0x1FF);
       CyDelay(1000);
       motion(i,0x258,0x1FF);
       CyDelay(100);
    }*/
     UART_Blue_PutString(" ON");
     CyDelay(5);
      
  }
  else if(command_from_kinect[0] == 0x1E)
  {
      state_onoff = 0;  
   /* int i = 0;
        for (; i < 3 ; i++)
    {
      motion(i,0x1FF,0x1FF);
      CyDelay(1000);
      motion(i,0,0x1FF);
      CyDelay(100);
    }
    
    if(i == 3)
    {
        
    }*/
     // motion(0, 0x0, 0x1FF);
      UART_Blue_PutString(" OFF");
      CyDelay(5);
      
  }
  //手关着
  else if(command_from_kinect[0] == 0xA0)
  {
     hands_open = 0;
     angle_ki[0] = command_from_kinect[3] * 4;
     angle_ki[1] = command_from_kinect[1] * 4;
    //2号需要做转换
     angle_ki[2] = 614 - command_from_kinect[2] * 4;
     angle_ki[3] = 580;
   //  char temp[64];
    
    // sprintf(temp," angle0:%d, angle1:%d,angle2:%d,angle3:%d",angle_ki[0],angle_ki[1],angle_ki[2],angle_ki[3]);
    // UART_Blue_PutArray(temp,45);
    CyDelay(5);
     //UART_Blue_PutString(" GetA0");
     
  }
  //手开着
  else if(command_from_kinect[0] == 0xA1)
  {
     hands_open = 1;
     angle_ki[0] = command_from_kinect[3] * 4;
     angle_ki[1] = command_from_kinect[1] * 4;
    //2号需要做转换
     angle_ki[2] = 614 - command_from_kinect[2] * 4;
     angle_ki[3] = 400;
   //     sprintf(temp," angle0:%d, angle1:%d,angle2:%d,angle3:%d",angle_ki[0],angle_ki[1],angle_ki[2],angle_ki[3]);
   // UART_Blue_PutArray(temp,45);
     CyDelay(5);
     //UART_Blue_PutString(" GetA1");
  }
  kinect_reset();
}

//用于接收kinect发出指令的临时全局变量
uint8 kinect_command_temp[64];


void get_kinect_comm()
{
   int i = 0;
   for( ; i <64 ; i++)
   {
       command_from_kinect[i] = blue_get_char[i];
   }
}
int count_blue_rx = 0;
int length_blue_rx = 0;


//读取蓝牙收到字符（两个函数共用）
void send_blue_rx(uint8 blue_char[])
{
    int i = 0;
    for(; i < 64 ; i++)
    {
       blue_get_char[i] = blue_char[i];   
    }
    UART_Blue_PutArray(blue_get_char,4); 
    CyDelay(5);
    //---------------------------------------------------
}
void get_blue_rx()
{   
    blue_char[count_blue_rx] = ccnm_temp;
    count_blue_rx = (count_blue_rx + 1) % 4;
    if(count_blue_rx == 0)
    {
      UART_Blue_ClearRxBuffer();
      send_blue_rx(blue_char);
       //返璞归真 
      int i = 0;
      for( ; i <64 ; i++)
      {
         blue_char[i] = 0;  
      }
    }
    
    
}



//----------------------------主函数--------------------------

int main()
{
    UART_Blue_Start();
    UART_Start();
    isr_rx_Start();
    isr_rx_2_Start();
    UART_Blue_PutString("UART is OK\r\n"); 
    CyDelay(10);
    
    //motion(0, angle, speed);
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    initial_Motor_Info();    
    UART_Blue_PutString("finishing init\n");

    CyDelay(1000);
    for(;;)
    {
        if(state_onoff == 0)
        {
         // ccnm_temp = 0;
          while(!ccnm_temp);
          CyDelay(5);
          get_kinect_comm();
          CyDelay(5);
          Read_from_kinect();   
        }
        
        else if(state_onoff == 1)
        {
            /*
            int k = 0;
            for(; k < 3 ; k++)
            {
                //清空缓冲区
                UART_ClearRxBuffer();            
                //读取当前位置
                read_current_position(k);
                CyDelay(60);
                
                while(UART_ReadRxStatus() == UART_RX_STS_FIFO_NOTEMPTY) 
                {                
                    char a;
                    a = UART_GetChar();

                    AnsFromRobot[Len_read] = a;
                    Len_read ++;
                } 
                AnsFromRobot[Len_read] = 0;
                ParseAnswer();
                quesnum = 0;
            }
            

            for( k = 0 ; k < 3 ; k++)
            {
                char temp[32] = {0};
                sprintf(temp," motor%dangle:%d",k,motorinfo[k].current_angle);
                UART_Blue_PutArray(temp,28);
                CyDelay(10);
            }
            */    
        
            ccnm = 0;
            ccnm_temp = 0;
            UART_Blue_PutString("GG");
            //等待输入
             while(!ccnm_temp);
            //PID
            CyDelay(5);
            get_kinect_comm();
            CyDelay(5);
            Read_from_kinect();
             
                         
            CyDelay(10);
            Reg_Write_angle(0,angle_ki[0],0);
            CyDelay(10);
            Reg_Write_angle(1,angle_ki[1],0);
            CyDelay(10);
            Reg_Write_angle(2,angle_ki[2],0);
            CyDelay(10);
            Reg_Write_angle(3,angle_ki[3],0);
            CyDelay(10);        
            action();
            CyDelay(10); 
        }
        CyDelay(50);
        
    }
}

/* [] END OF FILE */
