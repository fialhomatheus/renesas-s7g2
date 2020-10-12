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
 * File Name    : hw_agt_private.h
 * Description  : Asynchronous General Purpose Timer (AGT) Module hardware private header file.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @{
 **********************************************************************************************************************/

#ifndef HW_AGT_PRIVATE_H
#define HW_AGT_PRIVATE_H

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_timer_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define FSUB_FREQUENCY_HZ  (32768U)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef enum e_agt_start_status
{
    AGT_STOP  = 0,
    AGT_START = 1,
} agt_start_status_t;

/** Input/Output pins */
typedef enum e_agt_agtioc
{
    AGT_AGTCMA = 0,
    AGT_AGTCMB = 1
} agt_agtioc_t;

/** Timer modes */
typedef enum e_agt_mode
{
    AGT_MODE_TIMER                    = 0,     ///< The count source is counted.
    AGT_MODE_PULSE_OUTPUT             = 1,     ///< The count source is counted and the output is inverted at each
                                               // underflow of the timer.
    AGT_MODE_EVENT_COUNTER            = 2,     ///< An external event is counted.
    AGT_MODE_PULSE_WIDTH_MEASUREMENT  = 3,     ///< An external pulse width is measured.
    AGT_MODE_PULSE_PERIOD_MEASUREMENT = 4,     ///< An external pulse period is measured.
} agt_mode_t;

/** Count source division */
typedef enum e_agt_clock_division
{
    AGT_CKS_DIV_1   = 0,                       ///< Division ratio 1/1
    AGT_CKS_DIV_2   = 1,                       ///< Division ratio 1/2
    AGT_CKS_DIV_4   = 2,                       ///< Division ratio 1/4
    AGT_CKS_DIV_8   = 3,                       ///< Division ratio 1/8
    AGT_CKS_DIV_16  = 4,                       ///< Division ratio 1/16
    AGT_CKS_DIV_32  = 5,                       ///< Division ratio 1/32
    AGT_CKS_DIV_64  = 6,                       ///< Division ratio 1/64
    AGT_CKS_DIV_128 = 7,                       ///< Division ratio 1/128
} agt_clock_division_t;

/**********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common/hw_agt_common.h"



/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER


#endif /* HW_AGT_PRIVATE_H */

/*******************************************************************************************************************//**
 * @}
 **********************************************************************************************************************/
