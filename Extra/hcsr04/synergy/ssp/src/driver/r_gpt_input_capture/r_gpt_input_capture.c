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
 * File Name    : r_gpt_input_capture.c
 * Description  :  GPT functions used to implement the input capture interface.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_gpt_input_capture.h"
#include "r_gpt_input_capture_cfg.h"
#include "hw/hw_gpt_private.h"
#include "r_gpt_input_capture_private_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef GPT_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define GPT_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &g_gpt_input_capture_version)
#endif

/** Variant data mask to determine if GPT is 16-bit (0) or 32-bit (1). */
#define GPT_VARIANT_TIMER_SIZE_MASK  (0x4U)

/** "R_GIC" in ASCII, used to determine if channel is open. */
#define R_GIC_OPEN                (0x52474943ULL)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void  gpt_input_capture_hardware_initialize (gpt_input_capture_instance_ctrl_t      * const p_ctrl,
                                                   input_capture_cfg_t const * const p_cfg,
                                                   ssp_feature_t       const * const p_feature);

void gpt_input_capture_isr (void);
void gpt_input_capture_counter_overflow_isr (void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug.  This pragma suppresses the warnings in this 
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t g_gpt_input_capture_version =
{
    .api_version_minor  = INPUT_CAPTURE_API_VERSION_MINOR,
    .api_version_major  = INPUT_CAPTURE_API_VERSION_MAJOR,
    .code_version_major = GPT_INPUT_CAPTURE_CODE_VERSION_MAJOR,
    .code_version_minor = GPT_INPUT_CAPTURE_CODE_VERSION_MINOR,
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "gpt_input_capture";
#endif

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/** GPT Implementation of Input Capture Driver  */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const input_capture_api_t g_input_capture_on_gpt =
{
    .open           = R_GPT_InputCaptureOpen,
    .close          = R_GPT_InputCaptureClose,
    .versionGet     = R_GPT_InputCaptureVersionGet,
    .disable        = R_GPT_InputCaptureDisable,
    .enable         = R_GPT_InputCaptureEnable,
    .infoGet        = R_GPT_InputCaptureInfoGet,
    .lastCaptureGet = R_GPT_InputCaptureLastCaptureGet,
};

/*******************************************************************************************************************//**
 * @addtogroup GPT_INPUT_CAPTURE
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Open a GPT Timer for Input Capture. Implements input_capture_api_t::open.
 *
 * The Open function configures a single GPT channel for input capture and provides a handle for use with the other
 * Input Capture API functions. This function must be called once prior to calling any other Input Capture API function.
 * After a channel is opened, the Open function should not be called again for the same channel without first calling
 * the associated Close function.
 *
 * @retval SSP_SUCCESS              Initialization was successful.
 * @retval SSP_ERR_ASSERTION        One of the parameters is NULL: p_cfg, p_ctrl, p_extend.
 * @retval SSP_ERR_IRQ_BSP_DISABLED A required interrupt does not exist in the vector table.
 * @retval SSP_ERR_IN_USE           The channel specified has already been opened. No configurations were changed. Call
 *                                  the associated Close function or use associated Control commands to reconfigure the
 *                                  channel.
 * @return                          See @ref Common_Error_Codes or functions called by this function for other possible
 *                                  return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *                                   * fmi_api_t::eventInfoGet
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureOpen (input_capture_ctrl_t      * const p_api_ctrl,
                                  input_capture_cfg_t const * const p_cfg)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_cfg->p_extend);
#endif  // GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE

    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = p_cfg->channel;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_GPT;
    fmi_feature_info_t info = {0U};
    ssp_err_t err          = SSP_SUCCESS;
    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    GPT_ERROR_RETURN(SSP_SUCCESS == err, err);

    /** Verify channel is not already used */
    ssp_err_t     bsp_err = R_BSP_HardwareLock(&ssp_feature);
    GPT_ERROR_RETURN((SSP_SUCCESS == bsp_err), SSP_ERR_IN_USE);

    p_ctrl->p_reg = info.ptr;

    ssp_vector_info_t * p_vector_info;
    fmi_event_info_t event_info = {(IRQn_Type) 0U};
    err = g_fmi_on_fmi.eventInfoGet(&ssp_feature, SSP_SIGNAL_GPT_COUNTER_OVERFLOW, &event_info);
    p_ctrl->overflow_irq = event_info.irq;

    if(SSP_INVALID_VECTOR == p_ctrl->overflow_irq)
    {
        /* Release hardware lock. */
        R_BSP_HardwareUnlock(&ssp_feature);
        SSP_ERROR_LOG(SSP_ERR_IRQ_BSP_DISABLED, &g_module_name[0], g_gpt_input_capture_version);
        return SSP_ERR_IRQ_BSP_DISABLED;
    }
    R_SSP_VectorInfoGet(p_ctrl->overflow_irq, &p_vector_info);
    NVIC_SetPriority(p_ctrl->overflow_irq, p_cfg->overflow_irq_ipl);
    *(p_vector_info->pp_ctrl) = p_ctrl;

    err = g_fmi_on_fmi.eventInfoGet(&ssp_feature, SSP_SIGNAL_GPT_CAPTURE_COMPARE_A, &event_info);
    p_ctrl->capture_irq = event_info.irq;

    if(SSP_INVALID_VECTOR == p_ctrl->capture_irq)
    {
        /* Release hardware lock. */
        R_BSP_HardwareUnlock(&ssp_feature);
        SSP_ERROR_LOG(SSP_ERR_IRQ_BSP_DISABLED, &g_module_name[0], g_gpt_input_capture_version);
        return SSP_ERR_IRQ_BSP_DISABLED;
    }
    R_SSP_VectorInfoGet(p_ctrl->capture_irq, &p_vector_info);
    NVIC_SetPriority(p_ctrl->capture_irq, p_cfg->capture_irq_ipl);
    *(p_vector_info->pp_ctrl) = p_ctrl;

    if (0 == (info.variant_data & GPT_VARIANT_TIMER_SIZE_MASK))
    {
        p_ctrl->variant = INPUT_CAPTURE_VARIANT_16_BIT;
    }
    else
    {
        p_ctrl->variant = INPUT_CAPTURE_VARIANT_32_BIT;
    }


    /* Initialize control block. */
    p_ctrl->channel           = p_cfg->channel;
    p_ctrl->mode              = p_cfg->mode;
    p_ctrl->repetition        = p_cfg->repetition;
    p_ctrl->overflows_current = 0U;
    p_ctrl->overflows_last    = 0U;
    p_ctrl->p_callback        = p_cfg->p_callback;
    p_ctrl->p_context         = p_cfg->p_context;

    /* Perform hardware initializations based on configuration. */
    gpt_input_capture_hardware_initialize(p_ctrl, p_cfg, &ssp_feature);

    /* Mark channel as open. */
    p_ctrl->open = R_GIC_OPEN;

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureOpen */

/*******************************************************************************************************************//**
 * @brief  Close a GPT Timer Channel for Input Capture. Implements input_capture_api_t::close.
 *
 * Clears Timer settings, disables interrupts, and clears internal driver data.
 *
 * @retval SSP_SUCCESS          Successful close.
 * @retval SSP_ERR_ASSERTION    The parameter p_ctrl is NULL.
 * @retval SSP_ERR_NOT_OPEN     The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureClose (input_capture_ctrl_t * const p_api_ctrl)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
    ssp_vector_info_t * p_vector_info     = NULL;
    ssp_vector_info_t * p_vector_info_cmp = NULL;

#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Make sure channel is open.  If not open, return. */
    SSP_ASSERT(NULL != p_ctrl);
    GPT_ERROR_RETURN(R_GIC_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    R_SSP_VectorInfoGet(p_ctrl->overflow_irq, &p_vector_info);
    R_SSP_VectorInfoGet(p_ctrl->capture_irq, &p_vector_info_cmp);

    /** Cleanup. Disable interrupts and stop measurements. */
    NVIC_DisableIRQ(p_ctrl->overflow_irq);
    R_BSP_IrqStatusClear(p_ctrl->overflow_irq);
    NVIC_ClearPendingIRQ(p_ctrl->overflow_irq);

    NVIC_DisableIRQ(p_ctrl->capture_irq);
    R_BSP_IrqStatusClear(p_ctrl->capture_irq);
    NVIC_ClearPendingIRQ(p_ctrl->capture_irq);

    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;
    HW_GPT_CounterStartStop(p_gpt_reg, GPT_STOP);
    HW_GPT_ClearSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_StopSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_CaptureASourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_StartSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);

    /** Unlock channel */
    ssp_feature_t ssp_feature  = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = p_ctrl->channel;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_GPT;
    R_BSP_HardwareUnlock(&ssp_feature);

    /** Clear stored internal driver data */
    p_ctrl->p_callback = NULL;
    p_ctrl->p_context  = NULL;
    (*p_vector_info->pp_ctrl) = NULL;
    (*p_vector_info_cmp->pp_ctrl) = NULL;

    p_ctrl->open              = 0U;

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureClose */

/*******************************************************************************************************************//**
 * @brief      Gets driver version based on compile time macros. Implements input_capture_api_t::versionGet.
 *
 * @retval SSP_SUCCESS          Success.
 * @retval SSP_ERR_ASSERTION    The parameter p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureVersionGet (ssp_version_t * const p_version)
{
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Verify parameters are valid */
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_gpt_input_capture_version.version_id;

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureVersionGet */

/*******************************************************************************************************************//**
 * @brief  Disables GPT Input Capture RegA interrupt for specified channel at NVIC. Implements
 * input_capture_api_t::disable.
 *
 * @retval SSP_SUCCESS              Interrupt disabled successfully.
 * @retval SSP_ERR_ASSERTION        The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN         The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureDisable  (input_capture_ctrl_t const * const p_api_ctrl)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    GPT_ERROR_RETURN(R_GIC_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif
    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;

    /** Disable interrupts */
    NVIC_DisableIRQ(p_ctrl->capture_irq);
    NVIC_DisableIRQ(p_ctrl->overflow_irq);

    /** Clearing the input capture source select registers. */
    HW_GPT_CounterStartStop(p_gpt_reg, GPT_STOP);
    HW_GPT_ClearSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_StopSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_CaptureASourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);
    HW_GPT_StartSourceSelect(p_gpt_reg, GPT_TRIGGER_NONE);

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureIrqDisable */

/*******************************************************************************************************************//**
 * @brief  Enables GPT Input Capture RegA interrupt for specified channel at NVIC. Implements
 * input_capture_api_t::enable.
 *
 * @retval SSP_SUCCESS              Interrupt enabled successfully.
 * @retval SSP_ERR_ASSERTION        The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN         The channel is not opened.
  **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureEnable (input_capture_ctrl_t const * const p_api_ctrl)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    GPT_ERROR_RETURN(R_GIC_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif
    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;

    /** Configuring the input capture source select registers. */
    HW_GPT_CounterStartStop(p_gpt_reg, GPT_STOP);
    if (INPUT_CAPTURE_MODE_PERIOD != p_ctrl->mode)
    {
        HW_GPT_StopSourceSelect((GPT_BASE_PTR)p_ctrl->p_reg, (gpt_trigger_t)p_ctrl->stop_bitmask);
    }
    HW_GPT_CaptureASourceSelect(p_gpt_reg, (gpt_trigger_t)p_ctrl->stop_bitmask);
    HW_GPT_ClearSourceSelect(p_gpt_reg, (gpt_trigger_t)p_ctrl->start_bitmask);
    HW_GPT_StartSourceSelect(p_gpt_reg, (gpt_trigger_t)p_ctrl->start_bitmask);

    /** Enabling the overflow and capture registers. */
    R_BSP_IrqStatusClear(p_ctrl->overflow_irq);
    NVIC_ClearPendingIRQ(p_ctrl->overflow_irq);
    NVIC_EnableIRQ(p_ctrl->overflow_irq);

    R_BSP_IrqStatusClear(p_ctrl->capture_irq);
    NVIC_ClearPendingIRQ(p_ctrl->capture_irq);
    NVIC_EnableIRQ(p_ctrl->capture_irq);

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureIrqEnable */

/*******************************************************************************************************************//**
 * @brief  Gets status into provided p_info pointer. Implements input_capture_api_t::infoGet.
 *
 * @retval SSP_SUCCESS              Success.
 * @retval SSP_ERR_ASSERTION        The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN         The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureInfoGet (input_capture_ctrl_t const * const p_api_ctrl,
                                     input_capture_info_t       * const p_info)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Make sure parameters are valid. */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_info);
    GPT_ERROR_RETURN(R_GIC_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;
    bool status = HW_GPT_CounterStartBitGet(p_gpt_reg);
    if (true == status)
    {
        p_info->status = INPUT_CAPTURE_STATUS_CAPTURING;
    }
    else
    {
        p_info->status = INPUT_CAPTURE_STATUS_IDLE;
    }

    p_info->variant = p_ctrl->variant;

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureStatusGet */

/*******************************************************************************************************************//**
 * @brief  Update the last captured value and overflow count, in provided p_capture pointer.
 * Implements input_capture_api_t::lastCaptureGet.
 *
 * @retval SSP_SUCCESS           Period value written successfully.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_GPT_InputCaptureLastCaptureGet  (input_capture_ctrl_t const * const p_api_ctrl,
                                             input_capture_capture_t    * const p_capture)
{
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) p_api_ctrl;
#if GPT_INPUT_CAPTURE_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_capture);
    GPT_ERROR_RETURN(R_GIC_OPEN == p_ctrl->open, SSP_ERR_NOT_OPEN);
#endif

    /** Set capture value */
    p_capture->counter = p_ctrl->capture_count;
    p_capture->overflows = p_ctrl->overflows_last;

    return SSP_SUCCESS;
} /* End of function R_GPT_InputCaptureLastCaptureGet */

/*******************************************************************************************************************//**
 * @} (end addtogroup GPT_INPUT_CAPTURE)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Performs hardware initialization of the GPT for Input Capture.
 * @pre    All parameter checking should be done prior to calling this function.
 *
 * @param[in]  p_cfg  Pointer to configuration struct, which points to extension struct.
 * @param[in]  p_ctrl Pointer to control struct
 * @param[in]  p_feature Features structure
 **********************************************************************************************************************/
static void gpt_input_capture_hardware_initialize (gpt_input_capture_instance_ctrl_t      * const p_ctrl,
                                                   input_capture_cfg_t const * const p_cfg,
                                                   ssp_feature_t       const * const p_feature)
{
    gpt_input_capture_extend_t * p_extend = (gpt_input_capture_extend_t *) p_cfg->p_extend;
    gpt_input_capture_signal_t   signal     = p_extend->signal;
    input_capture_signal_level_t enable     = p_extend->enable_level;

    uint32_t start_bitmask;
    uint32_t stop_bitmask;

    /** Power on GPT before setting any hardware registers. Make sure the counter is stopped before setting registers.
    **/
    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;
    R_BSP_ModuleStart(p_feature);
    HW_GPT_CounterStartStop(p_gpt_reg, GPT_STOP);
    HW_GPT_ModeSet(p_gpt_reg, GPT_MODE_PERIODIC);
    HW_GPT_DirectionSet(p_gpt_reg, GPT_DIR_COUNT_UP);
    HW_GPT_DivisorSet(p_gpt_reg, (gpt_pclk_div_t) p_extend->clock_divider);
    HW_GPT_TimerCycleSet(p_gpt_reg, GPT_INPUT_CAPTURE_MAX_COUNT);
    HW_GPT_CounterSet(p_gpt_reg, 0);

    /** For Pulse Width or Period measurement, the technique is as follows:
     *  Construct the start_bitmask for the signal and edge of the start of the measurement.
     *  Construct the stop_bitmask for the signal and edge of the end of the measurement.
     *  For GTIOCxA and GTIOCxB, also choose the bit(s) based on the enable signal, if any.
     *  Configure the registers as follows:
     *   Start Source Select (GTSSR)    = start_bitmask,    start the counter on start_bitmask signal/edge
     *   Clear Source Select (GTCLR)    = start_bitmask,    start the counter on start_bitmask signal/edge
     *   Stop  Source Select (GTSTP)    = stop_bitmask,     stop the counter on stop_bitmask signal/edge
     *   Capture Source Select (GTICASR)= stop_bitmask,     capture the counter on stop_bitmask signal/edge
     * An interrupt will occur when capture is done, and the capture register can be read to get the pulse width.
     */

    start_bitmask = (uint32_t) GPT_TRIGGER_GTIOCA_RISING_WHILE_GTIOCB_LOW;

    if (GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCB == signal)
    {
        /* switch signals to GTIOCB rising while GTIOCA low*/
        start_bitmask  <<= 4;
    }

    /*initialize stop_bitmask the same as start_bitmask*/
    stop_bitmask = (uint32_t) start_bitmask;

    if (INPUT_CAPTURE_SIGNAL_EDGE_FALLING == p_cfg->edge)
    {
        /* switch the start_bitmask to start on the falling edge both levels */
        start_bitmask  <<= 2;
    }
    else
    {
        /*switch the stop_bitmask to stop on the falling edge (instead of rising)*/
        stop_bitmask <<= 2;
    }


    if (INPUT_CAPTURE_SIGNAL_LEVEL_HIGH == enable)
    {
        /* Switch both start and stop bitmasks to use HIGH enable instead of LOW enable signal */
        start_bitmask <<= 1;
        stop_bitmask  <<= 1;
    }
    else if (INPUT_CAPTURE_SIGNAL_LEVEL_NONE == enable)
    {
        /* Switch both start and stop bitmasks to use both HIGH and LOW enable signal */
        start_bitmask |= (start_bitmask << 1);
        stop_bitmask  |= (stop_bitmask << 1);
    }
    else
    {
        /* Do nothing here as initial enable level was low*/
    }

    if (INPUT_CAPTURE_MODE_PERIOD == p_cfg->mode)
    {
        stop_bitmask  = start_bitmask;
    }

    p_ctrl->start_bitmask = start_bitmask;
    p_ctrl->stop_bitmask = stop_bitmask;

    if (true == p_cfg->autostart)
    {

        /* Set the GPT Source Select registers to start, clear, stop, and capture on the desired signals */
        if (INPUT_CAPTURE_MODE_PERIOD != p_cfg->mode)
        {
            HW_GPT_StopSourceSelect((GPT_BASE_PTR)p_ctrl->p_reg, (gpt_trigger_t) stop_bitmask);
        }
        HW_GPT_CaptureASourceSelect((GPT_BASE_PTR)p_ctrl->p_reg, (gpt_trigger_t) stop_bitmask);
        HW_GPT_ClearSourceSelect((GPT_BASE_PTR)p_ctrl->p_reg, (gpt_trigger_t) start_bitmask);
        HW_GPT_StartSourceSelect((GPT_BASE_PTR)p_ctrl->p_reg, (gpt_trigger_t) start_bitmask);

        /** Enabling the overflow and capture registers. */
        R_BSP_IrqStatusClear(p_ctrl->overflow_irq);
        NVIC_ClearPendingIRQ(p_ctrl->overflow_irq);
        NVIC_EnableIRQ(p_ctrl->overflow_irq);

        R_BSP_IrqStatusClear(p_ctrl->capture_irq);
        NVIC_ClearPendingIRQ(p_ctrl->capture_irq);
        NVIC_EnableIRQ(p_ctrl->capture_irq);
    }
} /* End of function gpt_input_capture_hardware_initialize */

/*******************************************************************************************************************//**
 * @brief  The common code for all GPT input capture interrupt handling.
 *
 * Clears interrupt, disables interrupt if one-shot mode, and calls callback if one was provided in the open function.
 *
 **********************************************************************************************************************/
void gpt_input_capture_isr (void)
{
    /* Save context if RTOS is used */
    SF_CONTEXT_SAVE

    ssp_vector_info_t * p_vector_info = NULL;
    IRQn_Type irq = R_SSP_CurrentIrqGet();
    R_SSP_VectorInfoGet(irq, &p_vector_info);
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) *(p_vector_info->pp_ctrl);
    uint8_t channel = p_ctrl->channel;
    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_reg;

    /* Clear pending IRQ to make sure it doesn't fire again after exiting */
    R_BSP_IrqStatusClear(irq);

    /* Get captured value  */
    uint32_t counter = HW_GPT_RegisterAGet(p_gpt_reg);

    /* This is a stop and capture event (i.e. not a clear and start event), record the number of counter
     * overflows for this capture */
    if(0U != counter)
    {
        p_ctrl->overflows_last = p_ctrl->overflows_current;
        p_ctrl->capture_count = counter;
    }

    /* Clear the current overflow counter, since we are done with a capture or is about to start a new one */
    p_ctrl->overflows_current = 0U;

    /* Further processing is only required if this is a stop and capture event (i.e not a clear and start event) */
    if(0U != counter)
    {
        /* If we captured a one-shot pulse, then turn off future interrupts. */
        if (INPUT_CAPTURE_REPETITION_ONE_SHOT == p_ctrl->repetition)
        {
            NVIC_DisableIRQ(irq);
        }

        /* If a callback is provided, then call it with the captured counter value. */
        if (NULL != p_ctrl->p_callback)
        {
            input_capture_callback_args_t callback_args;    // local storage for callback arguments
            callback_args.channel   = channel;
            callback_args.event     = INPUT_CAPTURE_EVENT_MEASUREMENT;
            callback_args.counter   = p_ctrl->capture_count;
            callback_args.overflows = p_ctrl->overflows_last;
            callback_args.p_context = p_ctrl->p_context;
            p_ctrl->p_callback(&callback_args);         // call user's callback function
        }
    }

    /* Restore context if RTOS is used */
    SF_CONTEXT_RESTORE
} /* End of function gpt_input_capture_interrupt_common */

/*******************************************************************************************************************//**
 * @brief      Counter overflow interrupt procedure.
 **********************************************************************************************************************/
void gpt_input_capture_counter_overflow_isr (void)
{
    /* Save context if RTOS is used */
    SF_CONTEXT_SAVE

    ssp_vector_info_t * p_vector_info = NULL;
    IRQn_Type irq = R_SSP_CurrentIrqGet();
    R_SSP_VectorInfoGet(irq, &p_vector_info);
    gpt_input_capture_instance_ctrl_t * p_ctrl = (gpt_input_capture_instance_ctrl_t *) *(p_vector_info->pp_ctrl);
    uint8_t channel = p_ctrl->channel;

    /** Clear pending IRQ to make sure it doesn't fire again after exiting */
    R_BSP_IrqStatusClear(irq);

    /** Increment the current number of overflows. */
    p_ctrl->overflows_current++;

    if (NULL != p_ctrl->p_callback)
    {
        input_capture_callback_args_t callback_args;    // local storage for callback arguments
        callback_args.channel   = channel;
        callback_args.event     = INPUT_CAPTURE_EVENT_OVERFLOW;
        callback_args.overflows = p_ctrl->overflows_current;
        callback_args.counter   = 0xFFFFFFFFUL;
        callback_args.p_context = p_ctrl->p_context;
        p_ctrl->p_callback(&callback_args);         // call user's callback function
    }

    /* Restore context if RTOS is used */
    SF_CONTEXT_RESTORE
} /* End of function gpt_counter_overflow_common */
