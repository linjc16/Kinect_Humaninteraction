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


//PSoc向Robot的指令
uint8 Cmd2Robot[64];
uint8 AnsFromRobot[64];

unsigned short ID_read;
unsigned short Len_read;

const uint8 broadcastID = 0xFE;

volatile uint8 Mode = 0;
struct MotorInfo motorinfo[4];
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
void Reg_Write_angle(uint8 id, uint16 angle)
{
    //第一个ID
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x05; //length
    Cmd2Robot[4] = 0x04; //异步写入
    Cmd2Robot[5] = 0x1E;
    Cmd2Robot[6] = angle & 0xFF;
    Cmd2Robot[7] = (angle & 0xFF00) >> 8;
    Cmd2Robot[8] = ~(Cmd2Robot[2] + Cmd2Robot[3] + Cmd2Robot[4] + Cmd2Robot[5] + Cmd2Robot[6] + Cmd2Robot[7]);
    UART_PutArray(Cmd2Robot,9);
    
    motorinfo[id].dest_angle = angle;
    
}

void WriteSpeed(uint8 id, uint16 speed)
{
    Cmd2Robot[0] = 0xFF;
    Cmd2Robot[1] = 0xFF;
    Cmd2Robot[2] = id;
    Cmd2Robot[3] = 0x05;
    Cmd2Robot[4] = 0x03;
    Cmd2Robot[5] = 0x20;
    Cmd2Robot[6] = speed & 0x00FF;
    Cmd2Robot[7] = (speed & 0xFF00) >> 8;
    
    char temp = 0x00;
    int i = 2;
    for(; i < 8 ; i++)
    {
        temp += Cmd2Robot[i];
    }
    Cmd2Robot[8] = ~temp;

    UART_PutArray(Cmd2Robot,9);
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
    UART_PutArray(Cmd2Robot,8);
    quesnum = 1;
    
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
    UART_PutArray(Cmd2Robot,8);
    quesnum = 2;
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
    
    writeVoltRange(broadcastID,60,120);
	CyDelay(10);
    
    //motion(broadcastID, 0, 0x1FF);
	//CyDelay(5);
    
    acc_ctl(broadcastID,0,0);
	CyDelay(10);
        
    
    //------------------------------motion------------------------------------------
    
     Reg_Write_angle(0,0);
     CyDelay(10);
     Reg_Write_angle(1,0x1ff);
     CyDelay(10);
     Reg_Write_angle(2,0);
     CyDelay(10);
     Reg_Write_angle(3,400);
     CyDelay(10);        
     action();
     CyDelay(10);

}


//----------------------------主函数--------------------------

int main()
{
    UART_Blue_Start();
    UART_Start();
    isr_rx_Start();
    //isr_rx_2_Start();
    UART_Blue_PutString("UART is OK\r\n"); 
    CyDelay(10);
    CYGlobalIntEnable;
    CyDelay(10);
    
    initial_Motor_Info();    
    UART_Blue_PutString("finishing init\n");
    CyDelay(1000);
    
    int flag = 0;
    
    for(;;)
    {
        
        
        if(Mode == 1 && flag == 0) {//掷轮胎模式
            UART_Blue_PutString("Mode1\n");
            // Prep
            CyDelay(1000);
            motion(3,400,0x1FF);
            CyDelay(5000);
            motion(3,580,0x1FF);
            CyDelay(550);
            
            // Cast
            Reg_Write_angle(0, 200);
            CyDelay(50);
            Reg_Write_angle(1, 300);
            CyDelay(50);
            action();
         
            CyDelay(550);
            
            WriteSpeed(2, 0x2FF);
            CyDelay(50);
            WriteSpeed(1, 0x3FF);
            CyDelay(50);
            Reg_Write_angle(1, 800);
            CyDelay(50);
            Reg_Write_angle(2, 300);
            CyDelay(50);
            action();
            CyDelay(150);
            
            motion(3, 400, 0x1FF);
            CyDelay(50);
            
            WriteSpeed(1, 0x1FF);
            CyDelay(50);
            WriteSpeed(2, 0x1FF);
            CyDelay(50);
            WriteSpeed(0, 0x1FF);
            CyDelay(50);
        }
        else if(Mode == 2 && flag == 0){//投篮模式
            UART_Blue_PutString("Mode2\n");
            CyDelay(1000);
            motion(3,400,0x1FF);
            CyDelay(5000);
            motion(3,580,0x1FF);
            CyDelay(550);
            
            // Cast
            Reg_Write_angle(1, 800);
            CyDelay(50);
            Reg_Write_angle(2, 450);
            CyDelay(50);
            action();
            CyDelay(700);
            
            
            
            motion(2, 0, 0x1FF);
            CyDelay(50);
            motion(3, 400, 0x1FF);
            CyDelay(50);
            
            
            
        }
        CyDelay(1000);
        initial_Motor_Info();    
        UART_Blue_PutString("finishing init\n");
        CyDelay(2000);
    }
}

/* [] END OF FILE */