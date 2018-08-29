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
#include <isr_rx.h>
#include "cyapicallbacks.h"
#include <project.h>


extern int bufIdx;
extern char buffer[100];
extern char bufferchar[100];
extern int bufferchar_len;
extern int state_onoff;// 0 off, 1 on
extern char temp;
extern uint8 blue_char;
extern uint8 ccnm_temp;
//PSoc向Robot的指令
extern uint8 Cmd2Robot[64];
extern uint8 AnsFromRobot[64];
extern unsigned short ID_read;
extern unsigned short Len_read;
extern unsigned short Blue_len_read;

extern void limit_ccw(char id, char angle);
extern void limit_cw(char id, char angle);
extern void motion(char id, int angle, int speed);
extern void read_current_position(uint8 id);
extern void read_current_v(uint8 id);
extern void get_blue_rx();

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


CY_ISR(isr_rx_Interrupt)
{
    #ifdef isr_rx_INTERRUPT_INTERRUPT_CALLBACK
        isr_rx_Interrupt_InterruptCallback();
    #endif /* isr_rx_INTERRUPT_INTERRUPT_CALLBACK */ 

    /*  Place your Interrupt code here. */
    /* `#START isr_rx_Interrupt` */
    
    uint8 comm_from_rx[64];
    bufferchar_len = 0;
    char8 str[100];
    int len = 0;
    int i = 0;
    for(;i<100;i++) str[i] = 0;
    
    Blue_len_read = 0;
    
    while(UART_Blue_ReadRxStatus() == UART_Blue_RX_STS_FIFO_NOTEMPTY) 
    {
        char a;
        //buffer[bufIdx++] = UART_Blue_GetChar();
        //a = buffer[bufIdx - 1];
        //if(bufIdx == 100) bufIdx = 0;
        
        
        a = UART_Blue_GetChar();
        comm_from_rx[Blue_len_read] = a;  
        Blue_len_read ++;
        
        //UART_Blue_PutChar(a);
       /* if(a)
        {
        str[len] = a;
        len++;
        switch(a)
        {
            case 'p':
            {
                if(state_onoff == 0)
                {
                   state_onoff = 1;
                    temp = 'n';
                }
                else
                {
                   state_onoff = 0; 
                    temp = 'f';
                }
                break;   
            }
            case 'm':
                motion(0, 0x1FF, 0x1FF);

                break;
            case 'b':
                motion(0, 0x0, 0x1FF);
                break;
            case 'r':
                read_current_position(0);
                break;
            default:break;
        }
        
        }*/
        
        
    }
            ccnm_temp = comm_from_rx[0];
      //  UART_Blue_PutArray(comm_from_rx,4);
            get_blue_rx();
    
}
