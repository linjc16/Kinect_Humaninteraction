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

/* [] END OF FILE */

#include <cydevice_trm.h>
#include <CyLib.h>
#include <isr_rx_2.h>
#include "cyapicallbacks.h"
#include <project.h>

extern const uint8 broadcastID;
extern const uint8 motorAmount;
extern const uint8 quesCurrAngle;
extern const uint8 quesCurrVelo;

extern uint8 quesnum;
extern int bufIdx;
extern char buffer[100];
extern char bufferchar[100];
extern int bufferchar_len;
extern int state_onoff;// 0 off, 1 on
extern char temp;
//PSoc向Robot的指令
extern uint8 Cmd2Robot[64];
extern uint8 AnsFromRobot[];
extern unsigned short ID_read;
extern unsigned short Len_read;

extern void limit_ccw(char id, char angle);
extern void limit_cw(char id, char angle);
extern void motion(char id, int angle, int speed);
extern int ParseAnswer();   

uint8 CNM[64];
/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);




