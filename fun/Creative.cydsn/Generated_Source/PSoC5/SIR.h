/*******************************************************************************
* File Name: SIR.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SIR_H) /* Pins SIR_H */
#define CY_PINS_SIR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SIR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SIR__PORT == 15 && ((SIR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SIR_Write(uint8 value);
void    SIR_SetDriveMode(uint8 mode);
uint8   SIR_ReadDataReg(void);
uint8   SIR_Read(void);
void    SIR_SetInterruptMode(uint16 position, uint16 mode);
uint8   SIR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SIR_SetDriveMode() function.
     *  @{
     */
        #define SIR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SIR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SIR_DM_RES_UP          PIN_DM_RES_UP
        #define SIR_DM_RES_DWN         PIN_DM_RES_DWN
        #define SIR_DM_OD_LO           PIN_DM_OD_LO
        #define SIR_DM_OD_HI           PIN_DM_OD_HI
        #define SIR_DM_STRONG          PIN_DM_STRONG
        #define SIR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SIR_MASK               SIR__MASK
#define SIR_SHIFT              SIR__SHIFT
#define SIR_WIDTH              1u

/* Interrupt constants */
#if defined(SIR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SIR_SetInterruptMode() function.
     *  @{
     */
        #define SIR_INTR_NONE      (uint16)(0x0000u)
        #define SIR_INTR_RISING    (uint16)(0x0001u)
        #define SIR_INTR_FALLING   (uint16)(0x0002u)
        #define SIR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SIR_INTR_MASK      (0x01u) 
#endif /* (SIR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SIR_PS                     (* (reg8 *) SIR__PS)
/* Data Register */
#define SIR_DR                     (* (reg8 *) SIR__DR)
/* Port Number */
#define SIR_PRT_NUM                (* (reg8 *) SIR__PRT) 
/* Connect to Analog Globals */                                                  
#define SIR_AG                     (* (reg8 *) SIR__AG)                       
/* Analog MUX bux enable */
#define SIR_AMUX                   (* (reg8 *) SIR__AMUX) 
/* Bidirectional Enable */                                                        
#define SIR_BIE                    (* (reg8 *) SIR__BIE)
/* Bit-mask for Aliased Register Access */
#define SIR_BIT_MASK               (* (reg8 *) SIR__BIT_MASK)
/* Bypass Enable */
#define SIR_BYP                    (* (reg8 *) SIR__BYP)
/* Port wide control signals */                                                   
#define SIR_CTL                    (* (reg8 *) SIR__CTL)
/* Drive Modes */
#define SIR_DM0                    (* (reg8 *) SIR__DM0) 
#define SIR_DM1                    (* (reg8 *) SIR__DM1)
#define SIR_DM2                    (* (reg8 *) SIR__DM2) 
/* Input Buffer Disable Override */
#define SIR_INP_DIS                (* (reg8 *) SIR__INP_DIS)
/* LCD Common or Segment Drive */
#define SIR_LCD_COM_SEG            (* (reg8 *) SIR__LCD_COM_SEG)
/* Enable Segment LCD */
#define SIR_LCD_EN                 (* (reg8 *) SIR__LCD_EN)
/* Slew Rate Control */
#define SIR_SLW                    (* (reg8 *) SIR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SIR_PRTDSI__CAPS_SEL       (* (reg8 *) SIR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SIR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SIR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SIR_PRTDSI__OE_SEL0        (* (reg8 *) SIR__PRTDSI__OE_SEL0) 
#define SIR_PRTDSI__OE_SEL1        (* (reg8 *) SIR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SIR_PRTDSI__OUT_SEL0       (* (reg8 *) SIR__PRTDSI__OUT_SEL0) 
#define SIR_PRTDSI__OUT_SEL1       (* (reg8 *) SIR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SIR_PRTDSI__SYNC_OUT       (* (reg8 *) SIR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SIR__SIO_CFG)
    #define SIR_SIO_HYST_EN        (* (reg8 *) SIR__SIO_HYST_EN)
    #define SIR_SIO_REG_HIFREQ     (* (reg8 *) SIR__SIO_REG_HIFREQ)
    #define SIR_SIO_CFG            (* (reg8 *) SIR__SIO_CFG)
    #define SIR_SIO_DIFF           (* (reg8 *) SIR__SIO_DIFF)
#endif /* (SIR__SIO_CFG) */

/* Interrupt Registers */
#if defined(SIR__INTSTAT)
    #define SIR_INTSTAT            (* (reg8 *) SIR__INTSTAT)
    #define SIR_SNAP               (* (reg8 *) SIR__SNAP)
    
	#define SIR_0_INTTYPE_REG 		(* (reg8 *) SIR__0__INTTYPE)
#endif /* (SIR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SIR_H */


/* [] END OF FILE */
