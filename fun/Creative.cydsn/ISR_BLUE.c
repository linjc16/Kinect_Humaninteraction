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


extern volatile uint8 Mode;

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
    isr_rx_ClearPending();
        char a;        
        a = UART_Blue_GetChar();
        
        UART_Blue_PutChar(a);
            switch(a)
            {
                case '1':
                    Mode = 1;
                    break;   
                case '2':
                    Mode = 2;
                    break;
                default:
                    Mode = 0;
                    break;
            }        
        
}
