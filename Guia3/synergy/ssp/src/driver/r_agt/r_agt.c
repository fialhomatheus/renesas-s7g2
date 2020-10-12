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
 * File Name    : r_agt.c
 * Description  : Driver API for Asynchronous General Purpose Timer
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hw/hw_agt_private.h"
#include "r_agt_private.h"
#include "r_agt_private_api.h"
#include "r_agt.h"
#include "r_fmi.h"
#include "r_cgc.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef AGT_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define AGT_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &s_agt_version)
#endif

/** "AGT" in ASCII, used to determine if channel is open. */
#define AGT_OPEN                (0x00414754ULL)

/** Maximum number of clock counts in 16 bit timer */
#define AGT_MAX_CLOCK_COUNTS_16 (0xFFFFUL)
#define AGT_COMPARATOR_MATCH_A_OUTPUT (0x03U) ///<Enabling AGTOAn pin
#define AGT_COMPARATOR_MATCH_B_OUTPUT (0x30U) ///<Enabling AGTOBn pin

#define AGT_KHZ_PER_HZ                (1000ULL)
#define AGT_NSEC_PER_SEC              (1000000000ULL)
#define AGT_USEC_PER_SEC              (1000000ULL)
#define AGT_MSEC_PER_SEC              (1000ULL)

#define AGT_SOURCE_CLOCK_PCLKB_BITS   (0x3U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static ssp_err_t agt_source_freq_get(R_AGT0_Type * p_agt_regs,
                                     agt_count_source_t count_source,
                                     uint32_t * p_clk_freq_hz);

static ssp_err_t agt_period_to_counts(uint32_t      const clk_freq_hz,
                                      timer_size_t  const period,
                                      timer_unit_t  const unit,
                                      uint64_t    *       p_period_counts);

static void agt_divisor_select(agt_count_source_t   * p_count_source,
                               uint64_t             * p_period_counts,
                               agt_clock_division_t * p_divisor);

static ssp_err_t agt_period_set(R_AGT0_Type         * p_agt_regs,
                                agt_instance_ctrl_t * p_ctrl,
                                timer_size_t    const period,
                                timer_unit_t    const unit);

static ssp_err_t agt_counter_stop_wait(R_AGT0_Type * p_agt_regs);

static ssp_err_t agt_clock_frequency_get(R_AGT0_Type * p_agt_regs, uint32_t * p_clock_frequency);

static ssp_err_t duty_cycle_to_pclk (timer_size_t     const duty_cycle,
                                     timer_pwm_unit_t const unit,
                                     timer_size_t     const period,
                                     uint16_t       *       p_duty_cycle_counts);

static void agt_set_duty_cycle (R_AGT0_Type * p_agt_regs, uint8_t const pin, uint16_t const duty_cycle_counts);

static ssp_err_t agt_open_parameter_check(agt_instance_ctrl_t      * const p_ctrl,
                                          timer_cfg_t        const * const p_cfg,
                                          ssp_feature_t            * const p_feature);

static ssp_err_t agt_interrupt_configure(agt_instance_ctrl_t      * const p_ctrl,
                                         timer_cfg_t        const * const p_cfg);

static void agt_extension_configure(agt_instance_ctrl_t      * const p_ctrl,
                                    timer_cfg_t        const * const p_cfg,
                                    agt_mode_t               * const p_agt_mode);

static ssp_err_t agt_pwm_mode_configure(agt_instance_ctrl_t      * const p_ctrl,
                                        timer_cfg_t        const * const p_cfg);

static ssp_err_t agt_check_count_source(timer_cfg_t        const * const p_cfg);

static void agt_event_configure(agt_instance_ctrl_t      * const p_ctrl,
                                  timer_cfg_t        const * const p_cfg,
                                  ssp_feature_t            * const p_feature);

static void agt_count_set(agt_instance_ctrl_t * p_ctrl, R_AGT0_Type * p_agt_regs);

/* ISRs. */
void agt_int_isr (void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "agt";
#endif

static uint16_t g_agt_channel0_period = 0U;

#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this 
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t s_agt_version =
{
    .api_version_minor  = TIMER_API_VERSION_MINOR,
    .api_version_major  = TIMER_API_VERSION_MAJOR,
    .code_version_minor = AGT_CODE_VERSION_MINOR,
    .code_version_major = AGT_CODE_VERSION_MAJOR,
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/** AGT Implementation of General Timer Driver  */
/*LDRA_NOANALYSIS LDRA_INSPECTED below not working. */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const timer_api_t g_timer_on_agt =
{
    .open         = R_AGT_TimerOpen,
    .stop         = R_AGT_Stop,
    .start        = R_AGT_Start,
    .reset        = R_AGT_Reset,
    .counterGet   = R_AGT_CounterGet,
    .periodSet    = R_AGT_PeriodSet,
    .dutyCycleSet = R_AGT_DutyCycleSet,
    .infoGet      = R_AGT_InfoGet,
    .close        = R_AGT_Close,
    .versionGet   = R_AGT_VersionGet
};
/*LDRA_ANALYSIS */

/** @addtogroup AGT
 * @{*/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Open the AGT channel as a timer, handles required initialization described in hardware manual. Implements
 * timer_api_t::open.
 *
 * The Timer Open function configures a single AGT channel for timer mode with parameters specified in the timer
 * Configuration structure. It also sets up the control block for use with subsequent AGT Timer APIs.
 * 
 * This function must be called once prior to calling any other AGT API functions. After a channel is opened, the Open
 * function should not be called again for the same channel without first calling the associated Close function.
 *
 * The AGT hardware does not support one-shot functionality natively.  The one-shot feature is therefore implemented in
 * the AGT HAL layer. For a timer configured as a one-shot timer, the timer is stopped upon the first timer expiration.
 *
 * The AGT implementation of the general timer can accept an optional ::timer_on_agt_cfg_t extension parameter.  For
 * AGT, the extension specifies the clock to be used as timer source and the output pin configurations.  If the
 * extension parameter is not specified (NULL), the default clock PCLKB is used and the output pins are disabled.
 *
 * The clock divider is selected based on the source clock frequency and the timer period supplied by the caller.
 *
 * @retval SSP_SUCCESS           Initialization was successful and timer has started.
 * @retval SSP_ERR_ASSERTION     One of the following parameters may be NULL: p_cfg, p_ctrl,  or the
 *                               configuration channel ID exceeds AGT_MAX_CH, or the configuration mode is invalid.
 * @retval SSP_ERR_IN_USE        The channel specified has already been opened.
 * @retval SSP_ERR_IRQ_BSP_DISABLED A required interrupt has not been enabled in the BSP.
 * @return                       See @ref Common_Error_Codes or functions called by this function for other possible
 *                               return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *                                   * fmi_api_t::eventInfoGet
 *                                   * cgc_api_t::systemClockFreqGet
 *                                   * cgc_api_t::clockCheck
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/
ssp_err_t R_AGT_TimerOpen(timer_ctrl_t      * const p_api_ctrl,
                          timer_cfg_t const * const p_cfg)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;

    agt_mode_t  agt_mode  = AGT_MODE_TIMER;
    ssp_feature_t feature = {{(ssp_ip_t) 0U}};
    ssp_err_t   error     = agt_open_parameter_check(p_ctrl, p_cfg, &feature);
    AGT_ERROR_RETURN(SSP_SUCCESS == error, error);
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;

    /** Check agt count source to throw an error. */
    error = agt_check_count_source(p_cfg);
    AGT_ERROR_RETURN((SSP_ERR_CLOCK_ACTIVE == error), error);
    agt_event_configure(p_ctrl, p_cfg, &feature);

    /** Verify channel is not already used */
    error = R_BSP_HardwareLock(&feature);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    /** Power on the AGT channel. */
    R_BSP_ModuleStart(&feature);

    /** Wait for counter to stop. */
    error = agt_counter_stop_wait(p_agt_regs);
    if (SSP_SUCCESS == error)
    {
        /** Clear AGTO output */
        HW_AGT_IOModeSet(p_agt_regs, 0);

        /** Clear TEDGSEL bit to normal output. */
        HW_AGT_IOTEdgeSelectSet(p_agt_regs, 0);

        error = agt_interrupt_configure(p_ctrl, p_cfg);
    }

    if (SSP_SUCCESS == error)
    {
        agt_extension_configure(p_ctrl, p_cfg, &agt_mode);

        /** Set the AGT mode based on agt_mode value.  */
        HW_AGT_ModeSet(p_agt_regs, agt_mode);

        /** Make sure period is valid, then set period */
        p_ctrl->channel          = p_cfg->channel;
        error = agt_period_set(p_agt_regs, p_ctrl, p_cfg->period, p_cfg->unit);
    }

    if (SSP_SUCCESS == error)
    {
        error = agt_pwm_mode_configure(p_ctrl, p_cfg);
    }

    if (SSP_SUCCESS != error)
    {
        /* Initialization not successful, power off and release hardware lock. */
        R_BSP_ModuleStop(&feature);
        R_BSP_HardwareUnlock(&feature);
    }
    AGT_ERROR_RETURN(SSP_SUCCESS == error, error);

    p_ctrl->mode = p_cfg->mode;

    /** Start the timer if requested by user */
    if (p_cfg->autostart)
    {
        HW_AGT_CounterStartStop(p_agt_regs, AGT_START);
    }

    p_ctrl->open = AGT_OPEN;

    /** All done.  */
    return SSP_SUCCESS;
} /* End of function R_AGT_TimerOpen */

/*******************************************************************************************************************//**
 * Stops counter, disables interrupts, disables output pins, and clears internal driver data.  Implements
 * timer_api_t::close.
 *
 * @retval     SSP_SUCCESS          The AGT Timer channel is successfully closed.
 * @retval     SSP_ERR_ASSERTION    The parameter p_ctrl is NULL.
 * @retval     SSP_ERR_NOT_OPEN     The AGT channel is not opened.
 **********************************************************************************************************************/

ssp_err_t R_AGT_Close(timer_ctrl_t * const p_api_ctrl)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    /** Cleanup the device: Stop counter, disable interrupts, and power down if no other channels are in use. */
    ssp_err_t error;
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    error = agt_counter_stop_wait(p_agt_regs);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    /** Clear the TOE (output enable) bit */
    HW_AGT_IOModeSet(p_agt_regs, 0);

    /** Clear the TEDGSEL bit */
    HW_AGT_IOTEdgeSelectSet(p_agt_regs, 0);

    if (SSP_INVALID_VECTOR != p_ctrl->irq)
    {
        NVIC_DisableIRQ(p_ctrl->irq);
    }

    ssp_feature_t feature = {{(ssp_ip_t) 0U}};
    feature.id = SSP_IP_AGT;
    feature.channel = p_ctrl->channel;
    feature.unit = 0U;
    R_BSP_ModuleStop(&feature);

    ssp_vector_info_t * p_vector_info;
    if (SSP_INVALID_VECTOR != p_ctrl->irq)
    {
        R_SSP_VectorInfoGet(p_ctrl->irq, &p_vector_info);
        *(p_vector_info->pp_ctrl) = NULL;
    }

    /** Unlock channel */
    R_BSP_HardwareUnlock(&feature);

    p_ctrl->open = 0U;

    return SSP_SUCCESS;
} /* End of function R_AGT_Close */

/*******************************************************************************************************************//**
 * Retrieve and store counter value in provided p_value pointer. Implements timer_api_t::counterGet.
 *
 * @retval SSP_SUCCESS           Counter value read, p_value is valid.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_AGT_CounterGet(timer_ctrl_t * const p_api_ctrl,
                           timer_size_t * const p_value)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_value);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    /** Read counter value */
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;

    /** Increment retrieved counter value by one, as counter counts till zero */
    *p_value = HW_AGT_CounterGet(p_agt_regs);
    if (0xffffU != *p_value)
    {
        *p_value += 1U;
    }
    return SSP_SUCCESS;
} /* End of function R_AGT_CounterGet */

/*******************************************************************************************************************//**
 * Sets period value provided. Implements timer_api_t::periodSet.
 *
 * @retval SSP_SUCCESS           Period value written successfully.
 * @retval SSP_ERR_ASSERTION     The p_ctrl parameter was null.
 * @retval SSP_ERR_INVALID_ARG   One of the following is invalid:
 *                                    - p_period->unit: must be one of the options from timer_size_t::unit
 *                                    - p_period->value: must result in a period supported by the clock source
 *                                      specified during the Open call.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 * @return                       See @ref Common_Error_Codes or functions called by this function for other possible
 *                               return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *                                   * cgc_api_t::systemClockFreqGet
 **********************************************************************************************************************/
ssp_err_t R_AGT_PeriodSet (timer_ctrl_t * const p_api_ctrl,
                           timer_size_t   const period,
                           timer_unit_t   const unit)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    /** Make sure period is valid, then set period */
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    ssp_err_t err = agt_period_set(p_agt_regs, p_ctrl, period, unit);
    AGT_ERROR_RETURN(SSP_SUCCESS == err, err);

    return SSP_SUCCESS;
} /* End of function R_AGT_PeriodSet */

/*******************************************************************************************************************//**
 * Setting duty cycle is not supported by this driver. Implements timer_api_t::dutyCycleSet.
 *
 * @retval SSP_SUCCESS                Once duty cycle set successfully.
 * @retval SSP_ERR_INVALID_ARGUMENT   If any of the argument is invalid
 * @retval SSP_ERR_NOT_OPEN           The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_AGT_DutyCycleSet (timer_ctrl_t * const p_api_ctrl, timer_size_t const duty_cycle, timer_pwm_unit_t const unit,
        uint8_t const pin)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    uint16_t duty_cycle_counts = 0;
    ssp_err_t err = duty_cycle_to_pclk(duty_cycle, unit, p_ctrl->period, &duty_cycle_counts);
    AGT_ERROR_RETURN((SSP_SUCCESS == err), err);

    /** Set duty cycle. */
    agt_set_duty_cycle(p_agt_regs, pin, duty_cycle_counts);
    return SSP_SUCCESS;
} /* End of function R_AGT_DutyCycleSet */

/*******************************************************************************************************************//**
 * Resets the counter value to the period that was originally set. Implements timer_api_t::reset.
 *
 * @retval SSP_SUCCESS           Counter value written successfully.
 * @retval SSP_ERR_ASSERTION     The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_AGT_Reset(timer_ctrl_t * const p_api_ctrl)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    /** Save running status for restart, if already running. */
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    agt_start_status_t status = HW_AGT_CounterStartBitGet(p_agt_regs);

    ssp_err_t error;
    error = agt_counter_stop_wait(p_agt_regs);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    /** Set AGT counter for given period value. */
    agt_count_set(p_ctrl, p_agt_regs);

    /** Restart the AGT channel if it was running. */
    HW_AGT_CounterStartStop(p_agt_regs, status);

    return SSP_SUCCESS;
} /* End of function R_AGT_Reset */

/*******************************************************************************************************************//**
 * Starts timer. Implements timer_api_t::start.
 *
 * @retval SSP_SUCCESS           Timer successfully started.
 * @retval SSP_ERR_ASSERTION     The p_ctrl parameter is null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_AGT_Start(timer_ctrl_t * const p_api_ctrl)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    /** Start timer */
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    HW_AGT_CounterStartStop(p_agt_regs, AGT_START);
    return SSP_SUCCESS;
} /* End of function R_AGT_Start */

/*******************************************************************************************************************//**
* @brief  Get timer information and store it in provided pointer p_info.
* Implements timer_api_t::infoGet.
*
* @retval SSP_SUCCESS           Period, status, count direction, frequency value written to caller's
*                               structure successfully.
* @retval SSP_ERR_ASSERTION     The p_ctrl or p_period_counts parameter was null.
* @retval SSP_ERR_NOT_OPEN      The channel is not opened.
* @retval SSP_ERR_INVALID_HW_CONDITION   Invalid hardware setting is detected.
**********************************************************************************************************************/
ssp_err_t R_AGT_InfoGet (timer_ctrl_t * const p_api_ctrl, timer_info_t * const p_info)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
   /** Make sure parameters are valid. */
   SSP_ASSERT(NULL != p_ctrl);
   SSP_ASSERT(NULL != p_info);
   AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
   SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

   /** Get and store period */
   p_info->period_counts = p_ctrl->period;

   /** Get and store clock frequency */
   R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
   agt_clock_frequency_get(p_agt_regs, &p_info->clock_frequency);
   if (0U == p_info->clock_frequency)
   {
       return SSP_ERR_INVALID_HW_CONDITION;
   }

   /** AGT supports only counting down direction */
   p_info->count_direction = TIMER_DIRECTION_DOWN;

   bool status = HW_AGT_CounterStartBitGet(p_agt_regs);
   if (status)
   {
       p_info->status = TIMER_STATUS_COUNTING;
   }
   else
   {
       p_info->status = TIMER_STATUS_STOPPED;
   }

   p_info->elc_event = HW_AGT_AGTIEventGet(p_ctrl->channel);

   return SSP_SUCCESS;
} /* End of function R_AGT_InfoGet */

/*******************************************************************************************************************//**
 * Stops the AGT channel specified by the handle (control block). This API implements timer_api_t::stop.
 * This API does not reset the channel or power it down.
 *
 * @retval SSP_SUCCESS           Timer successfully stopped.
 * @retval SSP_ERR_ASSERTION   The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_AGT_Stop(timer_ctrl_t * const p_api_ctrl)
{
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) p_api_ctrl;
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    AGT_ERROR_RETURN(AGT_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
    SSP_ASSERT(NULL != p_ctrl->p_reg);
#endif

    ssp_err_t   error;
    R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;
    error = agt_counter_stop_wait(p_agt_regs);
    return error;
} /* End of function R_AGT_Stop */

/*******************************************************************************************************************//**
 * Sets driver version based on compile time macros.  Implements timer_api_t::versionGet.
 *
 * @retval     SSP_SUCCESS          Successful close.
 * @retval     SSP_ERR_ASSERTION    The parameter p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t R_AGT_VersionGet(ssp_version_t * const p_version)
{
#if AGT_CFG_PARAM_CHECKING_ENABLE
    /** Verify parameters are valid */
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = s_agt_version.version_id;

    return SSP_SUCCESS;
} /* End of function R_AGT_VersionGet */


 /** @} (end addtogroup AGT) */


/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Gets AGT source clock frequency (no divisor applied).
 *
 * @param[in]   p_agt_regs                Registers of AGT channel used
 * @param[in]   count_source              AGT source clock
 * @param[out]  p_clk_freq_hz             AGT source clock frequency stored here.
 *
 * @retval      SSP_SUCCESS               Clock frequency set in p_clk_freq_hz.
 * @retval      SSP_ERR_INVALID_ARGUMENT  Count source invalid for channel 0.
 * @return                                See @ref Common_Error_Codes or functions called by this function for other
 *                                        possible return codes. This function calls:
 *                                        * fmi_api_t::productFeatureGet
 *                                        * cgc_api_t::systemClockFreqGet
 **********************************************************************************************************************/
static ssp_err_t agt_source_freq_get(R_AGT0_Type * p_agt_regs,
                                     agt_count_source_t count_source,
                                     uint32_t * p_clk_freq_hz)
{
    /** If Clock source is PCLKB or derived from PCLKB */
    if (0U == ((uint8_t) count_source & (~AGT_SOURCE_CLOCK_PCLKB_BITS)))
    {
        /** Call CGC function to obtain current PCLKB clock frequency. */
        ssp_err_t error = g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_PCLKB, p_clk_freq_hz);
        AGT_ERROR_RETURN((SSP_SUCCESS == error), error);
    }
    else if (AGT_CLOCK_AGT0_UNDERFLOW == count_source)
    {
        /** Count source set to AGT0 Underflow is not valid for channel 0. */
        if(R_AGT0 == p_agt_regs )
        {
            return SSP_ERR_INVALID_ARGUMENT;
        }
        else
        {
            ssp_feature_t feature = {{(ssp_ip_t) 0U}};
            R_AGT0_Type * p_agt0_regs;
            fmi_feature_info_t info = {0U};
            feature.channel = 0U;
            feature.unit = 0U;
            feature.id = SSP_IP_AGT;
            g_fmi_on_fmi.productFeatureGet(&feature, &info);
            p_agt0_regs = (R_AGT0_Type *) info.ptr;

            /** Function agt_source_freq_get is recursively called, to fetch the source clock of AGT0,
                in the cascaded mode */
            agt_count_source_t agt0_count_source = (agt_count_source_t)HW_AGT_CountSourceGet(p_agt0_regs);
            agt_source_freq_get(p_agt0_regs, agt0_count_source, p_clk_freq_hz);
        }
    }
    else
    {
        /** Else either fSUB clock or LOCO clock is used.  The frequency is set to 32Khz  (32768) */
        *p_clk_freq_hz = FSUB_FREQUENCY_HZ;
    }

    return SSP_SUCCESS;
}

/******************************************************************************************************************//***
 * Converts user requested period to AGT clock counts.
 *
 * @param[in]   clk_freq_hz      AGT source clock frequency
 * @param[in]   period           User defined period
 * @param[in]   unit             Unit of user defined period
 * @param[out]  p_period_counts  AGT period in counts stored here
 *
 * @retval     SSP_SUCCESS               Clock frequency set in p_clk_freq_hz.
 * @retval     SSP_ERR_INVALID_ARGUMENT  Invalid unit
 **********************************************************************************************************************/
static ssp_err_t agt_period_to_counts(uint32_t      const clk_freq_hz,
                                      timer_size_t  const period,
                                      timer_unit_t  const unit,
                                      uint64_t    *       p_period_counts)
{
    /** Convert period to input clock counts so it can be set in hardware. */
    ssp_err_t err = SSP_SUCCESS;
    switch (unit)
    {
        case TIMER_UNIT_PERIOD_RAW_COUNTS:
            /** counts = desired_freq (counts) */
            *p_period_counts = period;
            break;
        case TIMER_UNIT_FREQUENCY_KHZ:
            /** counts = src_clock_freq (Hz) / ((1000 kHz/Hz * desired_freq (kHz)) * channel0_period (in counts)) */
            *p_period_counts = (clk_freq_hz * 1ULL) / ((AGT_KHZ_PER_HZ * (uint64_t) period) * g_agt_channel0_period);
            break;
        case TIMER_UNIT_FREQUENCY_HZ:
            /** counts = src_clock_freq (Hz) / (desired_freq (Hz) * channel0_period (in counts)) */
            *p_period_counts = (clk_freq_hz * 1ULL) / (period * g_agt_channel0_period);
            break;
        case TIMER_UNIT_PERIOD_NSEC:
            /** counts = desired_freq (ns) * src_clock_freq (Hz) / (1000000000 ns/s * channel0_period (in counts)) */
            *p_period_counts = (period * (clk_freq_hz * 1ULL)) / (AGT_NSEC_PER_SEC * g_agt_channel0_period);
            break;
        case TIMER_UNIT_PERIOD_USEC:
            /** counts = desired_freq (us) * src_clock_freq (Hz) / (1000000 us/s * channel0_period (in counts)) */
            *p_period_counts = (period * (clk_freq_hz * 1ULL)) / (AGT_USEC_PER_SEC * g_agt_channel0_period);
            break;
        case TIMER_UNIT_PERIOD_MSEC:
            /** counts = desired_freq (ms) * src_clock_freq (Hz) / (1000 ms/s * channel0_period (in counts)) */
            *p_period_counts = (period * (clk_freq_hz * 1ULL)) / (AGT_MSEC_PER_SEC * g_agt_channel0_period);
            break;
        case TIMER_UNIT_PERIOD_SEC:
            /** counts = (desired_freq (s) * src_clock_freq (Hz)) / channel0_period (in counts) */
            *p_period_counts = (period * (clk_freq_hz * 1ULL)) / g_agt_channel0_period;
            break;
        default:
            err = SSP_ERR_INVALID_ARGUMENT;
            break;
    }
    AGT_ERROR_RETURN((SSP_SUCCESS == err), err);
    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Converts user requested period to AGT clock counts.
 *
 * @param[in,out]  p_count_source   AGT source clock stored here
 * @param[in,out]  p_period_counts  AGT period in counts stored here
 * @param[out]     p_divisor        AGT divisor stored here
 **********************************************************************************************************************/
static void agt_divisor_select(agt_count_source_t   * p_count_source,
                               uint64_t             * p_period_counts,
                               agt_clock_division_t * p_divisor)
{
    /** Set divisor according to the selected count source */
    if (0U == ((uint8_t) *p_count_source & (~AGT_SOURCE_CLOCK_PCLKB_BITS)))
    {
        /* PCLKB is used, select available divisor.  It can be one of the three: Div 1, Div 2 and Div 8 */
        *p_count_source = AGT_CLOCK_PCLKB;

        /* Does temp_period exceed AGT_MAX_CLOCK_COUNTS? */
        if (*p_period_counts > AGT_MAX_CLOCK_COUNTS)
        {
            /** *p_period_counts exceeds 16 bits, select divisor 2 (and cut *p_period_counts into half). */
            *p_count_source = AGT_CLOCK_PCLKB_DIV_2;
            *p_period_counts /= 2U;
        }

        /* If *p_period_counts still exceeds AGT_MAX_CLOCK_COUNTS, divide *p_period_counts further by 4, effectively divide the
         * original
         * value by 8.  Select DIV_8. */
        if (AGT_MAX_CLOCK_COUNTS < *p_period_counts)
        {
            /* *p_period_counts still exceeds 16 bits, select divisor 8 */
            *p_count_source = AGT_CLOCK_PCLKB_DIV_8;
            *p_period_counts /= 4U;
        }
    }
    else if (AGT_CLOCK_AGT0_UNDERFLOW == *p_count_source)
    {
        /* Divisor not allowed. */
    }
    else
    {
        /** Sub clock is used, select appropriate sub clock divisor */
        while ((AGT_MAX_CLOCK_COUNTS < *p_period_counts) && (AGT_CKS_DIV_128 > *p_divisor))
        {
            /** Divide period by 2 and increase divisor to next power of 2. */
            *p_period_counts /= 2U;
            (*p_divisor)++;
        }
    }
}
/*******************************************************************************************************************//**
 * Converts user input period and unit values to PCLK counts and with appropriate divisor settings, then sets compare
 * match value and divisor.
 *
 * @param[in]  p_agt_regs   AGT registers for this channel
 * @param[in]  p_ctrl       Control block for this instance
 * @param[in]  period       User defined period
 * @param[in]  unit         Unit of user defined period
 *
 * @retval     SSP_SUCCESS               Delay and divisor were set successfully.
 * @retval     SSP_ERR_INVALID_ARGUMENT  One of the following is invalid:
 *                                         - p_period->unit: must be one of the options from timer_size_t::unit
 *                                         - p_period->value: must result in a period in the following range:
 *                                             - Lower bound: (1 / (PCLK frequency))
 *                                             - Upper bound: (0xFFFFFFFF * 1024 / (PCLK frequency))
 * @return                       See @ref Common_Error_Codes or functions called by this function for other possible
 *                               return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *                                   * cgc_api_t::systemClockFreqGet
 **********************************************************************************************************************/
static ssp_err_t agt_period_set(R_AGT0_Type * p_agt_regs, agt_instance_ctrl_t * p_ctrl, timer_size_t const period, timer_unit_t const unit)
{
#if AGT_CFG_PARAM_CHECKING_ENABLE
    AGT_ERROR_RETURN((0U != period), SSP_ERR_INVALID_ARGUMENT);
#endif

    ssp_err_t               error        = SSP_SUCCESS;
    uint32_t                clk_freq_hz  = 0U;
    uint64_t                temp_period  = 0U;
    agt_clock_division_t    temp_div     = AGT_CKS_DIV_1;

    /* Registers values casted to an enumerated list for readability. */
    agt_count_source_t count_source = (agt_count_source_t) HW_AGT_CountSourceGet(p_agt_regs);

    /** Look up source frequency for period conversion. */
    error = agt_source_freq_get(p_agt_regs, count_source, &clk_freq_hz);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    /** Set the value of the variable g_agt_channel0_period to 1 when not in cascaded mode so that the count value calculation is not affected. */
    if (AGT_CLOCK_AGT0_UNDERFLOW != count_source)
    {
        g_agt_channel0_period = 1U;
    }

    /** Convert period to counts. */
    error = agt_period_to_counts(clk_freq_hz, period, unit, &temp_period);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    /** Value in temp_period now in counts. Could potentially overflow 16 bits.  If so, convert it, set divisor. */
    agt_divisor_select(&count_source, &temp_period, &temp_div);

    /** Validate period parameter.  If period is valid, set it.  Otherwise return error */
    AGT_ERROR_RETURN((temp_period <= AGT_MAX_CLOCK_COUNTS), SSP_ERR_INVALID_ARGUMENT);

    /** Store current status, then stop timer before setting divisor register */
    agt_start_status_t status = HW_AGT_CounterStartBitGet(p_agt_regs);

    error = agt_counter_stop_wait(p_agt_regs);
    AGT_ERROR_RETURN((SSP_SUCCESS == error), error);

    HW_AGT_CountSourceSet(p_agt_regs, count_source);
    HW_AGT_ClockDivideSet(p_agt_regs, temp_div);

    /** Store the counter value so it can be retrieved later. */
    p_ctrl->period = (uint16_t) temp_period;
    if (0U == p_ctrl->channel)
    {
        g_agt_channel0_period = (uint16_t) temp_period;
    }

    /** Set AGT counter for given period value. */
    agt_count_set(p_ctrl, p_agt_regs);

    /** Reset counter in case new cycle is less than current count value, then restore state (counting or stopped). */
    HW_AGT_CounterStartStop(p_agt_regs, status);

    return SSP_SUCCESS;
} /* End of function agt_period_set. */

/*******************************************************************************************************************//**
 * Obtain the clock frequency of AGT, with divisor applied.
 *
 * @param[in]  p_agt_regs         Registers of AGT channel used
 * @param[out] p_clock_frequency  Source clock frequency of AGT returned here (in Hz)
 *
 * @return                        See @ref Common_Error_Codes or functions called by this function for other possible
 *                                return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *                                   * cgc_api_t::systemClockFreqGet
 **********************************************************************************************************************/
static ssp_err_t agt_clock_frequency_get(R_AGT0_Type * p_agt_regs, uint32_t * p_clock_frequency)
{
    *p_clock_frequency = 0U;
    uint32_t clock_frequency  = 0U;

    agt_count_source_t count_source = (agt_count_source_t)HW_AGT_CountSourceGet(p_agt_regs);
    agt_clock_division_t divisor    = HW_AGT_ClockDivideGet(p_agt_regs);

    ssp_err_t err = agt_source_freq_get(p_agt_regs, count_source, &clock_frequency);
    AGT_ERROR_RETURN(SSP_SUCCESS == err, err);

    uint8_t count_source_int = (uint8_t) count_source;
    if (0U == (count_source_int & (~AGT_SOURCE_CLOCK_PCLKB_BITS)))
    {
        /* If Clock source is PCLKB or derived from PCLKB */
        if (AGT_CLOCK_PCLKB_DIV_8 == count_source)
        {
            clock_frequency /= 8U;
        }
        if (AGT_CLOCK_PCLKB_DIV_2 == count_source)
        {
            clock_frequency /= 2U;
        }
    }
    else if (AGT_CLOCK_AGT0_UNDERFLOW == count_source)
    {
        /* No divisor to apply. */
    }
    else
    {
        clock_frequency >>= (uint32_t) divisor;
    }

    *p_clock_frequency = clock_frequency;

    return SSP_SUCCESS;
} /* End of function agt_clock_frequency_get */

/*******************************************************************************************************************//**
 * AGT internal function: Wait for counter to stop.
 *
 * @param[in]   p_agt_regs   AGT registers for this channel
 *
 * @retval      SSP_SUCCESS             Counter stopped before timeout.
 * @retval      SSP_ERR_TIMEOUT         Counter did not stop before timeout.
 **********************************************************************************************************************/
static ssp_err_t agt_counter_stop_wait(R_AGT0_Type * p_agt_regs)
{
    /** Stop timer */
    HW_AGT_CounterStartStop(p_agt_regs, AGT_STOP);

    uint32_t    timeout = AGT_32BIT_MAX_VALUE;
    uint8_t     count_status;

    do
    {
        count_status = HW_AGT_CountStatusGet(p_agt_regs);
        if (0U == count_status)
        {
            return SSP_SUCCESS;
        }
        timeout -= 1U;
    }
    while (timeout > 0U);

    return SSP_ERR_TIMEOUT;
}

/*******************************************************************************************************************//**
 * Converts duty cycle from input value to PCLK counts
 *
 * @param[in]   duty_cycle           User defined duty cycle
 * @param[in]   unit                 Unit of user defined duty cycle
 * @param[in]   period               User defined period
 * @param[out]  p_duty_cycle_counts  AGT duty cycle in counts stored here
 *
 * @retval     SSP_SUCCESS               Successful conversion
 * @retval     SSP_ERR_INVALID_ARGUMENT  One of the following is invalid:
 *                                         - unit: must be one of the options from timer_pwm_unit_t
 *                                         - duty_cycle: must result in a period in the following range:
 *                                             - Lower bound: (1 / (PCLK frequency))
 *                                             - Upper bound: period
 **********************************************************************************************************************/
static ssp_err_t duty_cycle_to_pclk (timer_size_t const duty_cycle, timer_pwm_unit_t const unit,
        timer_size_t const period, uint16_t * p_duty_cycle_counts)
{
    /** Initially set to an invalid value */
    uint16_t temp_duty_cycle = 0xFFFEULL;

    /** Convert duty cycle to PCLK counts so it can be set in hardware. */
    ssp_err_t err = SSP_SUCCESS;
    switch (unit)
    {
        case TIMER_PWM_UNIT_RAW_COUNTS:
            temp_duty_cycle = (uint16_t) duty_cycle;
            break;
        case TIMER_PWM_UNIT_PERCENT:
            temp_duty_cycle = (uint16_t)((period * duty_cycle) / 100U);
            break;
        case TIMER_PWM_UNIT_PERCENT_X_1000:
            temp_duty_cycle = (uint16_t)(((uint32_t) period * duty_cycle) / 100000ULL);
            break;
        default:
            err = SSP_ERR_INVALID_ARGUMENT;
            break;
    }

    AGT_ERROR_RETURN(SSP_SUCCESS == err, err);
    AGT_ERROR_RETURN(temp_duty_cycle <= period, SSP_ERR_INVALID_ARGUMENT);

    *p_duty_cycle_counts = (uint16_t) temp_duty_cycle;

    return SSP_SUCCESS;
} /* End of function duty_cycle_to_pclk */


/*******************************************************************************************************************//**
 * @brief      Set the duty cycle value in the comparator A/B and enable the output pin accordingly
 *
 * @param[in]   p_agt_regs           AGT registers for this channel
 * @param[in]   pin                  Which pin to set duty cycle for
 * @param[in]   duty_cycle_counts    Duty cycle to set (in counts)
 **********************************************************************************************************************/
static void agt_set_duty_cycle (R_AGT0_Type * p_agt_regs, uint8_t const pin, uint16_t const duty_cycle_counts)
{
    if (pin == AGT_AGTCMA)
    {
        HW_AGT_CompareMatchSet(p_agt_regs, AGT_AGTCMA, duty_cycle_counts);
        HW_AGT_IOPinEnable(p_agt_regs,AGT_COMPARATOR_MATCH_A_OUTPUT); ///<Enabling AGTOAn pin
    }
    else
    {
        HW_AGT_CompareMatchSet(p_agt_regs, AGT_AGTCMB, duty_cycle_counts);
        HW_AGT_IOPinEnable(p_agt_regs,AGT_COMPARATOR_MATCH_B_OUTPUT); ///<Enabling AGTOBn pin
    }
}

/*******************************************************************************************************************//**
 * Performs parameter checking for R_AGT_TimerOpen().
 *
 * @param[in]   p_ctrl       AGT instance control block
 * @param[in]   p_cfg        AGT configurations
 * @param[in]   p_feature    AGT feature definition for this channel
 *
 * @retval      SSP_SUCCESS        Pointers are valid and AGT channel exists on this part.
 * @retval      SSP_ERR_ASSERTION  A pointer was NULL.
 * @return                         See @ref Common_Error_Codes or functions called by this function for other possible
 *                                 return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 **********************************************************************************************************************/
static ssp_err_t agt_open_parameter_check(agt_instance_ctrl_t      * const p_ctrl,
                                          timer_cfg_t        const * const p_cfg,
                                          ssp_feature_t            * const p_feature)
{
#if AGT_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_ctrl);
#endif

    p_feature->id = SSP_IP_AGT;
    p_feature->channel = p_cfg->channel;
    p_feature->unit = 0U;
    fmi_feature_info_t info = {0};
    ssp_err_t error = g_fmi_on_fmi.productFeatureGet(p_feature, &info);
    AGT_ERROR_RETURN(SSP_SUCCESS == error, error);
    p_ctrl->p_reg = info.ptr;

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Performs event configure settings for R_AGT_TimerOpen().
 *
 * @param[in]   p_ctrl       AGT instance control block
 * @param[in]   p_cfg        AGT configurations
 * @param[in]   p_feature    AGT feature definition for this channel
 **********************************************************************************************************************/
static void agt_event_configure(agt_instance_ctrl_t      * const p_ctrl,
                                  timer_cfg_t        const * const p_cfg,
                                  ssp_feature_t            * const p_feature)
{
    ssp_vector_info_t * p_vector_info;
    fmi_event_info_t event_info = {(IRQn_Type) 0U};
    g_fmi_on_fmi.eventInfoGet(p_feature, SSP_SIGNAL_AGT_INT, &event_info);
    p_ctrl->irq = event_info.irq;
    if (SSP_INVALID_VECTOR != p_ctrl->irq)
    {
        R_SSP_VectorInfoGet(p_ctrl->irq, &p_vector_info);
        NVIC_SetPriority(p_ctrl->irq, p_cfg->irq_ipl);
        *(p_vector_info->pp_ctrl) = p_ctrl;
    }
}

/*******************************************************************************************************************//**
 * Enables interrupts if a callback is provided or one shot mode is selected.
 *
 * @param[in]   p_ctrl       AGT instance control block
 * @param[in]   p_cfg        AGT configurations
 *
 * @retval      SSP_SUCCESS               AGT interrupts enabled if required.
 * @retval      SSP_ERR_IRQ_BSP_DISABLED  AGT interrupt not in the vector table.
 **********************************************************************************************************************/
static ssp_err_t agt_interrupt_configure(agt_instance_ctrl_t      * const p_ctrl,
                                         timer_cfg_t        const * const p_cfg)
{
    /** Enable IRQ if user supplied a callback function,
     *  or if the timer is a one-shot timer (so the driver is able to
     *  turn off the timer after one period. */
    if ((NULL != p_cfg->p_callback) || (TIMER_MODE_ONE_SHOT == p_cfg->mode))
    {
        /* Return error if IRQ is required and not in the vector table. */
        AGT_ERROR_RETURN(SSP_INVALID_VECTOR != p_ctrl->irq, SSP_ERR_IRQ_BSP_DISABLED);

        /* Clear flags in control register. */
        HW_AGT_CounterFlagsClear(p_ctrl->p_reg);

        /** Clear any pending interrupts, and then enable interrupt. */
        R_BSP_IrqStatusClear(p_ctrl->irq);
        NVIC_ClearPendingIRQ(p_ctrl->irq);
        NVIC_EnableIRQ(p_ctrl->irq);

        /* Record user supplied callback routine. */
        p_ctrl->p_callback = p_cfg->p_callback;
        p_ctrl->p_context  = p_cfg->p_context;
    }

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Configures clock source and output pins.  Updates AGT mode if output pins are used.
 *
 * @param[in]     p_ctrl       AGT instance control block
 * @param[in]     p_cfg        AGT configurations
 * @param[in,out] p_agt_mode   AGT mode
 **********************************************************************************************************************/
static void agt_extension_configure(agt_instance_ctrl_t      * const p_ctrl,
                                    timer_cfg_t        const * const p_cfg,
                                    agt_mode_t               * const p_agt_mode)
{
    /** If AGT timer configuration structure is present, pick up the counter source and output
     *  pin configuration.
     *
     *  AGTO pin and AGTIO pin can be independently enabled for output.  The following describes the mode
     *  the AGT channel is configured to, based on user-specified AGTO and AGTIO pin output.
     *
     * If both AGT0 and AGTIO are disabled, AGT is open in timer mode with TOE bit cleared.
     * If AGT0 is enabled and AGTIO is disabled,  AGT is open in timer mode with TOE bit set.
     * If AGT0 is disabled and AGTIO is enabled, AGT is open in pulse output mode with TOE bit cleared.
     * If both AGT0 and AGTIO are enabled,  AGT is open in pulse output mode, TOE bit set.
     */
    if (NULL != p_cfg->p_extend)
    {
        timer_on_agt_cfg_t const * p_ext = (timer_on_agt_cfg_t const *) p_cfg->p_extend;
        HW_AGT_CountSourceSet(p_ctrl->p_reg, p_ext->count_source);

        if (true == p_ext->agto_output_enabled)
        {
            /* Set the TOE bit if AGTO is enabled.  AGTO can be enabled in any mode. */
            HW_AGT_IOModeSet(p_ctrl->p_reg, 1U);
        }

        if ((true == p_ext->agto_output_enabled) || (true == p_ext->agtio_output_enabled))
        {
            /* If AGTIO output pin is enabled, put the AGT in Pulse Output mode. Otherwise, use Timer Mode. */
            *p_agt_mode = AGT_MODE_PULSE_OUTPUT;

            /* Configure TEDGSEL bit based on user input. */
            HW_AGT_IOTEdgeSelectSet(p_ctrl->p_reg, (uint8_t) p_ext->output_inverted);
        }
    }
    else
    {
        /** If AGT timer configuration structure is not present, use default clock AGT_TCK_PCLKB as the clock
         *  source for the timer mode. */
        HW_AGT_CountSourceSet(p_ctrl->p_reg, AGT_CLOCK_PCLKB);
    }
}

/*******************************************************************************************************************//**
 * Configures duty cycle if AGT is in PWM mode.
 *
 * @param[in]   p_ctrl       AGT instance control block
 * @param[in]   p_cfg        AGT configurations
 *
 * @retval      SSP_SUCCESS               Duty cycle configured if required.
 * @retval      SSP_ERR_INVALID_ARGUMENT  Duty cycle configuration invalid.
 **********************************************************************************************************************/
static ssp_err_t agt_pwm_mode_configure(agt_instance_ctrl_t      * const p_ctrl,
                                        timer_cfg_t        const * const p_cfg)
{
    /** Calculate duty cycle */
    uint16_t duty_cycle_pclk = 0;
    if (TIMER_MODE_PWM == p_cfg->mode)
    {
        ssp_err_t error = duty_cycle_to_pclk (p_cfg->duty_cycle, p_cfg->duty_cycle_unit,
                                    p_ctrl->period, &duty_cycle_pclk);
        AGT_ERROR_RETURN((SSP_SUCCESS == error), error);
    }
    else
    {
        return SSP_SUCCESS;
    }

    timer_on_agt_cfg_t * p_extend = (timer_on_agt_cfg_t *) p_cfg->p_extend;

    /** Set output if requested */
    if (p_extend->agtoa_output_enable)
    {
        agt_set_duty_cycle(p_ctrl->p_reg, AGT_AGTCMA, duty_cycle_pclk);
    }

    if(p_extend->agtob_output_enable)
    {
        agt_set_duty_cycle(p_ctrl->p_reg, AGT_AGTCMB, duty_cycle_pclk);
    }

    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Checks whether clock is Active or not for Fsub and LOCO.
 *
 * @param[in]   p_cfg        AGT configurations
 * @return                       See @ref Common_Error_Codes or functions called by this function for other possible
 *                               return codes. This function calls:
 *                                   * cgc_api_t::clockCheck
 **********************************************************************************************************************/
static ssp_err_t agt_check_count_source(timer_cfg_t const * const p_cfg)
{
    ssp_err_t err = SSP_ERR_CLOCK_ACTIVE;
    const cgc_api_t * pcgc = &g_cgc_on_cgc;
    timer_on_agt_cfg_t const * p_ext = (timer_on_agt_cfg_t const *) p_cfg->p_extend;
    if(NULL != p_ext)
    {
        if (AGT_CLOCK_FSUB == p_ext->count_source)
        {
            err = pcgc->clockCheck(CGC_CLOCK_SUBCLOCK);
        }
        else if (AGT_CLOCK_LOCO == p_ext->count_source)
        {
            err = pcgc->clockCheck(CGC_CLOCK_LOCO);
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  Set the agt counter for the corresponding period value.
 *
 * @param[in]     p_ctrl       AGT instance control block.
 * @param[in]     p_agt_regs   Registers of AGT channel used.
 **********************************************************************************************************************/
static void agt_count_set(agt_instance_ctrl_t * p_ctrl , R_AGT0_Type * p_agt_regs)
{
    uint32_t temp_period = p_ctrl->period;
    /** Decrement period value by one before setting it to counter, as counter counts till zero. */
    if (0U != temp_period)
    {
        temp_period -= 1U;
    }

    HW_AGT_CounterSet(p_agt_regs, temp_period);
}

/*********************************************************************************************************************
 * AGT counter underflow interrupt.
 **********************************************************************************************************************/
void agt_int_isr (void)
{
    /* Save context if RTOS is used */
    SF_CONTEXT_SAVE

    ssp_vector_info_t * p_vector_info = NULL;
    R_SSP_VectorInfoGet(R_SSP_CurrentIrqGet(), &p_vector_info);
    agt_instance_ctrl_t * p_ctrl = (agt_instance_ctrl_t *) *(p_vector_info->pp_ctrl);

    timer_callback_args_t cb_data;

    /** Clear pending IRQ to make sure it doesn't fire again after exiting */
    R_BSP_IrqStatusClear(R_SSP_CurrentIrqGet());

    if ((NULL != p_ctrl) && (NULL != p_ctrl->p_reg))
    {
        R_AGT0_Type * p_agt_regs = (R_AGT0_Type *) p_ctrl->p_reg;

        /** If the channel is configured to be one-shot mode, stop the timer. */
        if (TIMER_MODE_ONE_SHOT == p_ctrl->mode)
        {
            agt_counter_stop_wait(p_agt_regs);
            /** Set AGT counter for given period value. */
            agt_count_set(p_ctrl, p_agt_regs);
        }

        /** Invoke the callback function if it is set. */
        if (NULL != p_ctrl->p_callback)
        {
            /** Setup parameters for the user-supplied callback function. */
            cb_data.p_context = p_ctrl->p_context;
            cb_data.event     = TIMER_EVENT_EXPIRED;
            p_ctrl->p_callback(&cb_data);
        }
    }

    /* Restore context if RTOS is used */
    SF_CONTEXT_RESTORE
}
