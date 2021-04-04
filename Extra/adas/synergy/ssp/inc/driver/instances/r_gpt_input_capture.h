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
 * File Name    : r_gpt_input_capture.h
 * Description  : Prototypes of GPT functions used to implement the input capture interface.
 **********************************************************************************************************************/

#ifndef R_GPT_INPUT_CAPTURE_H
#define R_GPT_INPUT_CAPTURE_H

/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @defgroup GPT_INPUT_CAPTURE GPT Input Capture
 * @brief Driver for the General PWM Timer (GPT) with Input Capture.
 *
 * @section GPT_INPUT_CAPTURE_SUMMARY Summary
 * Extends @ref INPUT_CAPTURE_API.
 *
 * This module implements the @ref INPUT_CAPTURE_API for the General PWM Timer (GPT) peripherals GPT32EH, GPT32E, GPT32.
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_input_capture_api.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define GPT_INPUT_CAPTURE_CODE_VERSION_MAJOR (1U)
#define GPT_INPUT_CAPTURE_CODE_VERSION_MINOR (12U)

#define GPT_INPUT_CAPTURE_MAX_COUNT          (0xFFFFFFFFUL)  ///< Maximum value of GPT counter

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Input capture signal selection */
typedef enum e_gpt_input_capture_signal
{
    GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCA,    ///< GTIOCxA pin, where x is channel number
    GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCB,    ///< GTIOCxB pin, where x is channel number
} gpt_input_capture_signal_t;

/** Input capture signal noise filter (debounce) setting. Only available for input signals GTIOCxA and GTIOCxB.
 *   The noise filter samples the external signal at intervals of the PCLK divided by one of the values.
 *   When 3 consecutive samples are at the same level (high or low), then that level is passed on as
 *   the observed state of the signal. See "Noise Filter Function" in the hardware manual, GPT section.
 */
typedef enum e_gpt_input_capture_signal_filter
{
    GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE,   ///< NONE - no filtering
    GPT_INPUT_CAPTURE_SIGNAL_FILTER_1,      ///< PCLK/1 - fast sampling
    GPT_INPUT_CAPTURE_SIGNAL_FILTER_4,      ///< PCLK/4
    GPT_INPUT_CAPTURE_SIGNAL_FILTER_16,     ///< PCLK/16
    GPT_INPUT_CAPTURE_SIGNAL_FILTER_64,     ///< PCLK/64 - slow sampling
} gpt_input_capture_signal_filter_t;

/** Input capture PCLK divider. Used to scale the timer counter. */
typedef enum e_gpt_input_capture_clock_divider
{
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_1,      ///< / 1
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_4,      ///< / 4
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_16,     ///< / 16
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_64,     ///< / 64
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_256,    ///< / 256
    GPT_INPUT_CAPTURE_CLOCK_DIVIDER_1024,   ///< / 1024
} gpt_input_capture_clock_divider_t;

/** @brief Extension configuration struct for TU Input Capture.
 *
 * Pointed to by input_capture_cfg_t.p_extend
 */
typedef struct st_gpt_input_capture_extend
{
    gpt_input_capture_signal_t         signal;          ///< One of gpt_input_capture_signal_t
    gpt_input_capture_signal_filter_t  signal_filter;   ///< One of gpt_input_capture_signal_filter_t
    gpt_input_capture_clock_divider_t  clock_divider;   ///< One of gpt_input_capture_clock_divider_t

    /** The unused GTIOCx pin can be used as an enable signal to enable captures. If the Input Capture Signal Pin
     * is GTIOCA, then the enable pin is GTIOCB.  The enable level is set here if used. */
    input_capture_signal_level_t       enable_level;
    gpt_input_capture_signal_filter_t  enable_filter;   ///< One of gpt_input_capture_signal_filter_t
} gpt_input_capture_extend_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when input_capture_api_t::open is called. */
typedef struct st_gpt_input_capture_instance_ctrl
{
    uint32_t                    open;         ///< Whether or not channel is open
    uint8_t                     channel;      ///< The channel in use.
    input_capture_mode_t        mode;         ///< The mode of measurement being performed.
    input_capture_repetition_t  repetition;   ///< One-shot or periodic measurement.
    uint32_t                    capture_count;///< The value of the timer captured at the time of interrupt.
    uint32_t                    overflows_last;///< Overflow count that occurred during last measurement
    uint32_t                    overflows_current;///< Running count of overflows in current measurement
    void (*p_callback) (input_capture_callback_args_t * p_args); ///< Pointer to user callback
    void const                * p_context;    ///< Pointer to user's context data, to be passed to the callback function.
    void                      * p_reg;          ///< GPT base register for this channel
    IRQn_Type                   capture_irq;  ///< Capture IRQ number
    IRQn_Type                   overflow_irq; ///< Overflow IRQ number
    input_capture_variant_t     variant;      ///< Timer variant
    uint32_t                    start_bitmask; ///< Start and Clear bitmask for input capture
    uint32_t                    stop_bitmask;  ///<Stop and capture bitmask for input capture
} gpt_input_capture_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const input_capture_api_t g_input_capture_on_gpt;
/** @endcond */

/*******************************************************************************************************************//**
 * @} (end defgroup GPT_INPUT_CAPTURE)
 **********************************************************************************************************************/

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif // R_GPT_INPUT_CAPTURE_H
