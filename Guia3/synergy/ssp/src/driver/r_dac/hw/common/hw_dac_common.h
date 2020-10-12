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
 * File Name    : hw_dac_common.h
 * Description  : Low Level Driver functions
 **********************************************************************************************************************/

#ifndef HW_DAC_COMMON_H
#define HW_DAC_COMMON_H

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define DAC_BASE 0x4005E000UL
#define HW_DAC   ((R_DAC_Type *) DAC_BASE)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef enum e_dac_vref
{
    DAC_VREF_AVCC0_AVSS0 = 1,
    DAC_VREF_INTERNAL    = 3,
    DAC_VREF_VREFH_VREFL = 6,
} dac_vref_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/
__STATIC_INLINE void HW_DAC_Control (R_DAC_Type * p_dac_reg, uint8_t channel, hw_dac_control_t value)
{
    if (0U == channel)
    {
        p_dac_reg->DACR_b.DAOE0 = value;
    }
    else
    {
        p_dac_reg->DACR_b.DAOE1 = value;
    }
}

__STATIC_INLINE void HW_DAC_DADRWrite (R_DAC_Type * p_dac_reg, uint8_t channel, uint16_t value)
{
    p_dac_reg->DADRn[channel] = value;
}

__STATIC_INLINE void HW_DAC_DADRAddressGet (R_DAC_Type * p_dac_reg, uint8_t channel, volatile uint16_t ** p_addr)
{
    *p_addr = &p_dac_reg->DADRn[channel];
}

__STATIC_INLINE void HW_DAC_DADPRCfg (R_DAC_Type * p_dac_reg, uint8_t value)
{
    p_dac_reg->DADPR_b.DPSEL = (0x01U & value);
}

__STATIC_INLINE void HW_DAC_DAADSCRCfg (R_DAC_Type * p_dac_reg, uint8_t value)
{
    p_dac_reg->DAADSCR_b.DAADST = (0x01U & value);
}

__STATIC_INLINE void HW_DAC_DAAMPCRCfg (R_DAC_Type * p_dac_reg, dac_cfg_t const * const p_cfg)
{
    /** Each channel has its own amplifier enable bit. */
    if (0U == p_cfg->channel)
    {
        if (p_cfg->output_amplifier_enabled)
        {
            /** Enable channel 0 amplifier. */
            p_dac_reg->DAAMPCR_b.DAAMP0 = 1;
        }
        else
        {
            /** Disable channel 0 amplifier. */
            p_dac_reg->DAAMPCR_b.DAAMP0 = 0;
        }
    }
    else
    {
        if (p_cfg->output_amplifier_enabled)
        {
            /** Enable channel 1 amplifier. */
            p_dac_reg->DAAMPCR_b.DAAMP1 = 1;
        }
        else
        {
            /** Disable channel 1 amplifier. */
            p_dac_reg->DAAMPCR_b.DAAMP1 = 0;
        }
    }
}

__STATIC_INLINE void HW_DAC_ChargePumpCfg (R_DAC_Type * p_dac_reg, bool enable_charge_pump)
{
    if (enable_charge_pump)
    {
        p_dac_reg->DAPC_b.PUMPEN = 1;
    }
    else
    {
        p_dac_reg->DAPC_b.PUMPEN = 0;
    }
}

__STATIC_INLINE void HW_DAC_VrefSet(R_DAC_Type * p_dac_reg, dac_vref_t vref)
{
    p_dac_reg->DAVREFCR_b.REF = vref;
}

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* HW_DAC_COMMON_H */

