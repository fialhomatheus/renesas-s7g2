/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : hw_agt_common.h
 * Description  : Asynchronous General Purpose Timer (AGT) Module hardware common header file.
 **********************************************************************************************************************/

#ifndef HW_AGT_COMMON_H
#define HW_AGT_COMMON_H

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** Pointer to AGT channels */

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Set compare match value
 * @param   p_reg        Pointer to input/output control register
 * @param   value        Compare match value to set
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_CompareMatchSet (R_AGT0_Type * p_agt_base, agt_agtioc_t agtio, uint16_t const value)
{
    if (AGT_AGTCMA == agtio)
    {
        /* GTCCRA buffer register. */
        p_agt_base->AGTCMA_b.AGTCMA = value;
    }
    else
    {
        /* GTCCRB buffer register. */
        p_agt_base->AGTCMB_b.AGTCMB = value;
    }
}

/*******************************************************************************************************************//**
 * Returns the value of the start bit, indicating if the counter is counting or not.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @return  Start bit value
 **********************************************************************************************************************/
__STATIC_INLINE agt_start_status_t HW_AGT_CounterStartBitGet (R_AGT0_Type * p_agt_base)
{
    return (agt_start_status_t) p_agt_base->AGTCR_b.TSTART;
}

/*******************************************************************************************************************//**
 * Starts or stops the counter.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   start_stop   Specify start or stop operation
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_CounterStartStop (R_AGT0_Type * p_agt_base, agt_start_status_t start_stop)
{
    p_agt_base->AGTCR = (0xF0U | (uint8_t) start_stop);
}

/*******************************************************************************************************************//**
 * Clears flags in AGT control register.
 * @param   ch           The channel corresponds to the hardware channel number.
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_CounterFlagsClear (R_AGT0_Type * p_agt_base)
{
    p_agt_base->AGTCR &= 0x0FU;
}

/*******************************************************************************************************************//**
 * Sets the counter value.
 * @param  ch           The channel corresponds to the hardware channel number.
 * @param  value        Any number from 0 to 0xFFFF.
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_CounterSet (R_AGT0_Type * p_agt_base, uint32_t value)
{
    /* Set counter value */
    p_agt_base->AGT = (uint16_t)(value & 0x0FFFF);
}

/*******************************************************************************************************************//**
 * Returns the current counter value.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @return  Current counter value in the range 0 to 0xFFFF.
 **********************************************************************************************************************/
__STATIC_INLINE timer_size_t HW_AGT_CounterGet (R_AGT0_Type * p_agt_base)
{
    /* Get counter value */
    return p_agt_base->AGT;
}

/*******************************************************************************************************************//**
 * Reads count status flag.
 * @param   ch           The channel corresponds to the hardware channel number.
 **********************************************************************************************************************/
__STATIC_INLINE uint8_t HW_AGT_CountStatusGet (R_AGT0_Type * p_agt_base)
{
   return  p_agt_base->AGTCR_b.TCSTF;
}

/*******************************************************************************************************************//**
 * Sets the timer count source.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify the count source
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_CountSourceSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTMR1_b.TCK = (uint8_t)(value & 0x07);
}

/*******************************************************************************************************************//**
 * Gets the timer count source.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @return  Current count source
 **********************************************************************************************************************/
__STATIC_INLINE uint8_t HW_AGT_CountSourceGet (R_AGT0_Type * p_agt_base)
{
    return p_agt_base->AGTMR1_b.TCK;
}

/*******************************************************************************************************************//**
 * Sets the division ratio for fSUB/LOCO clock.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify division ratio
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_ClockDivideSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTMR2_b.CKS = (uint8_t)(value & 0x07);
}

/*******************************************************************************************************************//**
 * Gets the division ratio for fSUB/LOCO clock.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @retval  Division ratio
 **********************************************************************************************************************/
__STATIC_INLINE agt_clock_division_t HW_AGT_ClockDivideGet (R_AGT0_Type * p_agt_base)
{
    uint8_t cks = p_agt_base->AGTMR2_b.CKS;
    agt_clock_division_t ret_value = (agt_clock_division_t)cks;
    return (ret_value);
}

/*******************************************************************************************************************//**
 * Sets the low power mode.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify low power mode
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_LowPowerModeSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTMR2_b.LPM = (uint8_t)(value & 0x01);
}

/*******************************************************************************************************************//**
 * Sets the I/O mode.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify TOE mode
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_IOModeSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTIOC_b.TOE = (uint8_t)(value & 0x01);
}

/*******************************************************************************************************************//**
 * Sets the I/O mode.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify TOE mode
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_IOTEdgeSelectSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTIOC_b.TEDGSEL = (uint8_t)(value & 0x01);
}

/*******************************************************************************************************************//**
 * Sets the timer mode.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        Specify mode of operation
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_ModeSet (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTMR1_b.TMOD = (uint8_t)(value & 0x07);
}

/*******************************************************************************************************************//**
 * Sets the I/O pin enable.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @param   value        TOEA/TOEB
 **********************************************************************************************************************/
__STATIC_INLINE void HW_AGT_IOPinEnable (R_AGT0_Type * p_agt_base, uint8_t value)
{
    p_agt_base->AGTCMSR = (uint8_t)(value);
}

/*******************************************************************************************************************//**
 * Return the elc event associated with the AGTI interrupt.
 * @param   ch           The channel corresponds to the hardware channel number.
 * @return  The ELC event associated AGTI for the channel.
 **********************************************************************************************************************/
__STATIC_INLINE elc_event_t HW_AGT_AGTIEventGet (uint8_t const ch)
{
    /* All the current synergy MCUs (S7, S3, S1) have two AGT channels, hence this is just a condition check */
    return (elc_event_t) ((uint32_t) ELC_EVENT_AGT0_INT + 
           (ch * ((uint32_t) ELC_EVENT_AGT1_INT - (uint32_t) ELC_EVENT_AGT0_INT)));
}

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* HW_AGT_COMMON_H */
