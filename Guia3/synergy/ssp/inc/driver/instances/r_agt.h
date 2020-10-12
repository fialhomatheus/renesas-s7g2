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
 * File Name    : r_agt.h
 * Description  : Asynchronous General Purpose Timer (AGT) Module instance header file.
 **********************************************************************************************************************/



#ifndef R_AGT_H
#define R_AGT_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_agt_cfg.h"
#include "r_timer_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Leading zeroes removed to avoid coding standards violation. */
#define AGT_CODE_VERSION_MAJOR (1U)
#define AGT_CODE_VERSION_MINOR (9U)

/** Maximum number of clock counts in 16 bit timer. */
#define AGT_MAX_CLOCK_COUNTS        (0xFFFFULL)
#define AGT_MILLISECONDS_PER_SECOND (1000ULL)
#define AGT_MICROSECONDS_PER_SECOND (1000000ULL)
#define AGT_NANOSECONDS_PER_SECOND  (1000000000ULL)
#define AGT_KHZ_TO_HZ               (1000ULL)

#define AGT_32BIT_MAX_VALUE         (0xFFFFFFFFULL)

/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @defgroup AGT AGT
 * @brief Driver for the Asynchronous General Purpose Timer (AGT).
 *
 * @section AGT_SUMMARY Summary
 * Extends @ref TIMER_API.
 *
 * HAL High-Level Driver for accessing and configuring AGT timer modes.
 *
 * The AGT timer functions are used by the Timer to provide timer services.
 *
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Count source */
typedef enum e_agt_count_source
{
    /** Counter clock source is PCLKB when AGT_CLOCK_PCLKB, AGT_CLOCK_PCLKB_DIV_2, or AGT_CLOCK_PCLKB_DIV_8 is selected.  
     *  The PCLKB divisor is selected automatically at runtime to the optimal value of PCLKB/1, PCLKB/2, or PCLKB/8.
     *  If the timer_cfg_t::unit is TIMER_UNIT_PERIOD_RAW_COUNTS, the timer_cfg_t::period should be the desired value
     *  in PCLKB counts, even if the value would exceed 16 bits.  For example, if a period of 0x30000 counts is 
     *  requested, a divisor of PCLKB/8 is be selected and the counter underflows after 0x6000 counts. */
    AGT_CLOCK_PCLKB          = 0,
    AGT_CLOCK_PCLKB_DIV_8    = 1,    ///< Superseded: See AGT_CLOCK_PCLKB.
    AGT_CLOCK_PCLKB_DIV_2    = 3,    ///< Superseded: See AGT_CLOCK_PCLKB.
    AGT_CLOCK_LOCO           = 4,    ///< Divided clock LOCO specified y bits CKS[2:0] in the AGTMR2 register
    AGT_CLOCK_AGT0_UNDERFLOW = 5,    ///< Underflow event signal from AGT0
    AGT_CLOCK_FSUB           = 6,    ///< Divided clock fSUB specified by bits CKS[2:0] in the AGTMR2 register
} agt_count_source_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when timer_api_t::open is called. */
typedef struct st_agt_instance_ctrl
{
    /** Callback provided when a timer ISR occurs.  NULL indicates no CPU interrupt. */
    void (* p_callback)(timer_callback_args_t * p_args);

    /** Placeholder for user data.  Passed to the user callback in ::timer_callback_args_t. */
    void const *    p_context;
    void       *    p_reg;        ///< Base register for this channel
    uint32_t        open;         ///< Whether or not channel is open
    uint16_t        period;       ///< Current timer period (counts)
    uint8_t         channel;      ///< Channel number.
    IRQn_Type       irq;          ///< Counter overflow IRQ number
    timer_mode_t    mode;         ///< Timer mode
} agt_instance_ctrl_t;

/** Optional AGT extension data structure.*/
typedef struct st_timer_on_agt_cfg
{
    agt_count_source_t  count_source;  ///< AGT channel clock source. Valid values are: AGT_CLOCK_PCLKB, AGT_CLOCK_LOCO, AGT_CLOCK_FSUB
    bool  agto_output_enabled;         ///< AGTO pin is enabled for output compare (true, false)
    bool  agtio_output_enabled;        ///< AGTIO pin is enabled for output compare (true, false)
    bool  output_inverted;             ///< Output inverted (true, false)
    bool  agtoa_output_enable;         ///< Enable comparator A output pin (true, false)
    bool  agtob_output_enable;         ///< Enable comparator B output pin (true, false)
} timer_on_agt_cfg_t;

/*******************************************************************************************************************//**
 * @} (end defgroup AGT)
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const timer_api_t g_timer_on_agt;
/** @endcond */

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif // R_AGT_H


