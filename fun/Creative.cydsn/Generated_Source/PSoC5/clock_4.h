/*******************************************************************************
* File Name: clock_4.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_clock_4_H)
#define CY_CLOCK_clock_4_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void clock_4_Start(void) ;
void clock_4_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void clock_4_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void clock_4_StandbyPower(uint8 state) ;
void clock_4_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 clock_4_GetDividerRegister(void) ;
void clock_4_SetModeRegister(uint8 modeBitMask) ;
void clock_4_ClearModeRegister(uint8 modeBitMask) ;
uint8 clock_4_GetModeRegister(void) ;
void clock_4_SetSourceRegister(uint8 clkSource) ;
uint8 clock_4_GetSourceRegister(void) ;
#if defined(clock_4__CFG3)
void clock_4_SetPhaseRegister(uint8 clkPhase) ;
uint8 clock_4_GetPhaseRegister(void) ;
#endif /* defined(clock_4__CFG3) */

#define clock_4_Enable()                       clock_4_Start()
#define clock_4_Disable()                      clock_4_Stop()
#define clock_4_SetDivider(clkDivider)         clock_4_SetDividerRegister(clkDivider, 1u)
#define clock_4_SetDividerValue(clkDivider)    clock_4_SetDividerRegister((clkDivider) - 1u, 1u)
#define clock_4_SetMode(clkMode)               clock_4_SetModeRegister(clkMode)
#define clock_4_SetSource(clkSource)           clock_4_SetSourceRegister(clkSource)
#if defined(clock_4__CFG3)
#define clock_4_SetPhase(clkPhase)             clock_4_SetPhaseRegister(clkPhase)
#define clock_4_SetPhaseValue(clkPhase)        clock_4_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(clock_4__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define clock_4_CLKEN              (* (reg8 *) clock_4__PM_ACT_CFG)
#define clock_4_CLKEN_PTR          ((reg8 *) clock_4__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define clock_4_CLKSTBY            (* (reg8 *) clock_4__PM_STBY_CFG)
#define clock_4_CLKSTBY_PTR        ((reg8 *) clock_4__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define clock_4_DIV_LSB            (* (reg8 *) clock_4__CFG0)
#define clock_4_DIV_LSB_PTR        ((reg8 *) clock_4__CFG0)
#define clock_4_DIV_PTR            ((reg16 *) clock_4__CFG0)

/* Clock MSB divider configuration register. */
#define clock_4_DIV_MSB            (* (reg8 *) clock_4__CFG1)
#define clock_4_DIV_MSB_PTR        ((reg8 *) clock_4__CFG1)

/* Mode and source configuration register */
#define clock_4_MOD_SRC            (* (reg8 *) clock_4__CFG2)
#define clock_4_MOD_SRC_PTR        ((reg8 *) clock_4__CFG2)

#if defined(clock_4__CFG3)
/* Analog clock phase configuration register */
#define clock_4_PHASE              (* (reg8 *) clock_4__CFG3)
#define clock_4_PHASE_PTR          ((reg8 *) clock_4__CFG3)
#endif /* defined(clock_4__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define clock_4_CLKEN_MASK         clock_4__PM_ACT_MSK
#define clock_4_CLKSTBY_MASK       clock_4__PM_STBY_MSK

/* CFG2 field masks */
#define clock_4_SRC_SEL_MSK        clock_4__CFG2_SRC_SEL_MASK
#define clock_4_MODE_MASK          (~(clock_4_SRC_SEL_MSK))

#if defined(clock_4__CFG3)
/* CFG3 phase mask */
#define clock_4_PHASE_MASK         clock_4__CFG3_PHASE_DLY_MASK
#endif /* defined(clock_4__CFG3) */

#endif /* CY_CLOCK_clock_4_H */


/* [] END OF FILE */
