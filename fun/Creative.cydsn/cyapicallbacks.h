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
#ifndef CYAPICALLBACKS_H
#define CYAPICALLBACKS_H
    
    /*Define your macro callbacks here */
    /*For more information, refer to the Macro Callbacks topic in the PSoC Creator Help.*/
    
#endif /* CYAPICALLBACKS_H */   
/* [] */

#include <project.h>    

struct MotorInfo{
    uint8 Id;
    uint8 delaytime;
    uint16 cw;
    uint16 ccw;
    
    uint16 dest_angle;
    uint16 dest_velo;
    uint8 plus_acc;
    uint8 minus_acc;
    
    uint16 current_angle;
    uint16 current_velo;
};