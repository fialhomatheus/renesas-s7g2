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
 * File Name    : r_dac.c
 * Description  : DAC HLD implementation
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <string.h>
#include "r_dac.h"
#include "r_dac_private.h"
#include "r_dac_private_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef DAC_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define DAC_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), &g_module_name[0], &g_dac_version)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static ssp_err_t r_dac_open_param_check (dac_instance_ctrl_t * p_ctrl, dac_cfg_t const * const p_cfg);
/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const dac_api_t g_dac_on_dac =
{
    .open            = R_DAC_Open,
    .close           = R_DAC_Close,
    .write           = R_DAC_Write,
    .start           = R_DAC_Start,
    .stop            = R_DAC_Stop,
    .versionGet      = R_DAC_VersionGet,
    .infoGet         = R_DAC_InfoGet
};

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

#if defined(__GNUC__)
/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this
 * structure only.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t g_dac_version =
{
    .api_version_minor  = DAC_API_VERSION_MINOR,
    .api_version_major  = DAC_API_VERSION_MAJOR,
    .code_version_major = DAC_CODE_VERSION_MAJOR,
    .code_version_minor = DAC_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "dac";
#endif

/*******************************************************************************************************************//**
 * @addtogroup DAC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 * @brief Perform required initialization described in hardware manual.  Implements dac_api_t::open.
 * Configures a single DAC channel, starts the channel, and provides a handle for use with the
 * DAC API Write and Close functions.  Must be called once prior to calling any other DAC API
 * functions.  After a channel is opened, Open should not be called again for the same channel
 * without calling Close first.
 *
 * @retval SSP_SUCCESS           The channel was successfully opened.
 * @retval SSP_ERR_ASSERTION     One or both of the following parameters may be NULL: p_api_ctrl or p_cfg
 *                               Channel ID requested in p_cfg may not available on the device selected in
 *                               r_bsp_config.h
 *                               data_format value in p_cfg is out of range.
 *                               ad_da_synchronized value in p_cfg is out of range.
 * @retval  SSP_ERR_IN_USE       DAC resource is locked.
 * @return                       See @ref Common_Error_Codes or functions called by this function for other possible
 *                               return codes. This function calls:
 *                                   * fmi_api_t::productFeatureGet
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_Open (dac_ctrl_t * p_api_ctrl, dac_cfg_t const * const p_cfg)
{
    dac_instance_ctrl_t * p_ctrl = (dac_instance_ctrl_t *) p_api_ctrl;
    ssp_err_t err = SSP_SUCCESS;

    /** Validate the input parameter. */
#if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Check parameters */
    err = r_dac_open_param_check(p_ctrl, p_cfg);

    if (SSP_SUCCESS != err)
    {
        return err;
    }
#endif /* if DAC_CFG_PARAM_CHECKING_ENABLE */

    /** Make sure the peripheral exists. */
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = 0U;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_DAC;
    fmi_feature_info_t info = {0U};

    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    DAC_ERROR_RETURN(SSP_SUCCESS == err, err);


#if DAC_CFG_PARAM_CHECKING_ENABLE
    uint32_t max_channels = (info.variant_data >> 5) & 3U;
    if (0U == max_channels)
    {
        max_channels = 2U;
    }
    DAC_ERROR_RETURN(p_cfg->channel < (uint8_t) max_channels, SSP_ERR_IP_CHANNEL_NOT_PRESENT);
#endif /* if DAC_CFG_PARAM_CHECKING_ENABLE */

    p_ctrl->p_reg = info.ptr;

    /** Lock the DAC Hardware Resource. */
    /* Note: Hardware locks are provided for each channel on the DAC, but the DAC is otherwise viewed as a single
     * peripheral with no channels by the FMI and R_BSP_ModuleStart APIs. */
    ssp_feature.channel = p_cfg->channel;
    err = R_BSP_HardwareLock(&ssp_feature);
    ssp_feature.channel = 0U;
    DAC_ERROR_RETURN((SSP_SUCCESS == err), err);

    /** Power on the DAC device. */
    R_BSP_ModuleStart(&ssp_feature);

    /** Stop the channel. */
    R_DAC_Type * p_dac_reg = (R_DAC_Type *) p_ctrl->p_reg;
    HW_DAC_Control(p_dac_reg, p_cfg->channel, HW_DAC_CONTROL_DISABLE);

    /** Configure data format: left or right justified. */
    HW_DAC_DADPRCfg(p_dac_reg, p_cfg->data_format);

    /** Configure D/A-A/D Synchronous Start Control Register(DAADSCR). */
    HW_DAC_DAADSCRCfg(p_dac_reg, (uint8_t) p_cfg->ad_da_synchronized);

    /** Set output amplifier configuration for the channel. */
    HW_DAC_DAAMPCRCfg(p_dac_reg, p_cfg);
    /** Set the reference voltage. */
    bsp_feature_dac_t dac_feature;
    dac_feature.has_davrefcr = 0U;
    dac_feature.has_chargepump = 0U;
    R_BSP_FeatureDacGet(&dac_feature);

    if (1U == dac_feature.has_davrefcr)
    {
        HW_DAC_VrefSet(p_dac_reg, DAC_VREF_AVCC0_AVSS0);
    }
    bool charge_pump_enabled = false;

    if (1U == dac_feature.has_chargepump)
    {
       if (p_cfg->p_extend)
       {
           const dac_extended_cfg_t *pextend = (dac_extended_cfg_t *) p_cfg->p_extend;
           charge_pump_enabled = pextend->enable_charge_pump;
           /** If charge pump has supported by MCU, configures the register(DAPC) */
           HW_DAC_ChargePumpCfg(p_dac_reg, charge_pump_enabled);
       }
    }

    /** Initialize the channel state information. */
    p_ctrl->channel         = p_cfg->channel;
    p_ctrl->channel_started = false;
    p_ctrl->channel_opened  = true;

    /** All done.  Return. */
    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief    Stop the D/A conversion, stop output, and close the DAC channel.
 * @retval   SSP_SUCCESS           The channel is successfully closed.
 * @retval   SSP_ERR_ASSERTION     p_api_ctrl is NULL.
 * @retval   SSP_ERR_NOT_OPEN      Channel associated with p_ctrl has not been opened.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_Close (dac_ctrl_t * p_api_ctrl)
{
    dac_instance_ctrl_t * p_ctrl = (dac_instance_ctrl_t *) p_api_ctrl;

#if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Validate the handle parameter */
    SSP_ASSERT(NULL != p_ctrl);
#endif
    /** Validate that the channel is opened. */
    DAC_ERROR_RETURN(p_ctrl->channel_opened, SSP_ERR_NOT_OPEN);

    /** Stop the channel */
    R_DAC_Type * p_dac_reg = (R_DAC_Type *) p_ctrl->p_reg;
    HW_DAC_Control(p_dac_reg, p_ctrl->channel, HW_DAC_CONTROL_DISABLE);

    /** Update the channel state information. */
    p_ctrl->channel_opened  = false;
    p_ctrl->channel_started = false;

    /** Unlock the DAC Hardware Resource */
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0U}};
    ssp_feature.channel = p_ctrl->channel;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_DAC;

    /** Power down the DAC device. */
    R_BSP_ModuleStop(&ssp_feature);

    /** Unlock the DAC Hardware Resource */
    R_BSP_HardwareUnlock(&ssp_feature);

    /* All done, return success. */
    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief    Write data to the D/A converter and enable the output if it has not been enabled.
 *
 * @retval   SSP_SUCCESS           Data is successfully written to the D/A Converter.
 * @retval   SSP_ERR_ASSERTION     p_api_ctrl is NULL.
 * @retval   SSP_ERR_NOT_OPEN      Channel associated with p_ctrl has not been opened.
 *
 * @note     Write function automatically starts the D/A conversion after data is successfully written
 *           to the channel.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_Write (dac_ctrl_t * p_api_ctrl, dac_size_t value)
{
    dac_instance_ctrl_t * p_ctrl = (dac_instance_ctrl_t *) p_api_ctrl;

    #if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Validate the handle parameter */
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Validate that the channel is opened. */
    DAC_ERROR_RETURN(p_ctrl->channel_opened, SSP_ERR_NOT_OPEN);

    /** Write the value to D/A converter. */
    R_DAC_Type * p_dac_reg = (R_DAC_Type *) p_ctrl->p_reg;
    HW_DAC_DADRWrite(p_dac_reg, p_ctrl->channel, value);

    /** Start the converter if it has been idle. */
    if (!p_ctrl->channel_started)
    {
        /** Start the channel */
        HW_DAC_Control(p_dac_reg, p_ctrl->channel, HW_DAC_CONTROL_ENABLE);

        p_ctrl->channel_started = true;
    }

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief    Start the D/A conversion output if it has not been started.
 *
 * @retval   SSP_SUCCESS           The channel is started successfully.
 * @retval   SSP_ERR_ASSERTION     p_api_ctrl is NULL.
 * @retval   SSP_ERR_NOT_OPEN      Channel associated with p_ctrl has not been opened.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_Start (dac_ctrl_t * p_api_ctrl)
{
    dac_instance_ctrl_t * p_ctrl = (dac_instance_ctrl_t *) p_api_ctrl;

    #if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Validate the handle parameter */
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Validate that the channel is opened. */
    DAC_ERROR_RETURN(p_ctrl->channel_opened, SSP_ERR_NOT_OPEN);

    /** Enable the output. */
    R_DAC_Type * p_dac_reg = (R_DAC_Type *) p_ctrl->p_reg;
    HW_DAC_Control(p_dac_reg, p_ctrl->channel, HW_DAC_CONTROL_ENABLE);

    /** Update the internal state. */
    p_ctrl->channel_started = true;

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Stop the D/A conversion and disable the output signal.
 *
 * @retval  SSP_SUCCESS           The control is successfully stopped.
 * @retval  SSP_ERR_ASSERTION     p_api_ctrl is NULL.
 * @retval  SSP_ERR_NOT_OPEN      Channel associated with p_ctrl has not been opened.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_Stop (dac_ctrl_t * p_api_ctrl)
{
    dac_instance_ctrl_t * p_ctrl = (dac_instance_ctrl_t *) p_api_ctrl;

    #if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Validate the handle parameter */
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /** Validate that the channel is opened. */
    DAC_ERROR_RETURN(p_ctrl->channel_opened, SSP_ERR_NOT_OPEN);

    /** Disable the output. */
    R_DAC_Type * p_dac_reg = (R_DAC_Type *) p_ctrl->p_reg;
    HW_DAC_Control(p_dac_reg, p_ctrl->channel, HW_DAC_CONTROL_DISABLE);

    /** Mark the internal state. */
    p_ctrl->channel_started = false;

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief  Get version and store it in provided pointer p_version.
 * @retval  SSP_SUCCESS           Successfully retrieved version information.
 * @retval  SSP_ERR_ASSERTION     p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t   R_DAC_VersionGet (ssp_version_t * p_version)
{
#if DAC_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_dac_version.version_id;

    return SSP_SUCCESS;
}
/*******************************************************************************************************************//**
 * @brief  Get information about DAC Resolution and store it in provided pointer p_info.
 *
 * @retval SSP_SUCCESS           Value of DAC resolution written to caller's
 *                               structure successfully.
 * @retval SSP_ERR_ASSERTION     The p_info parameter was null.
 **********************************************************************************************************************/
ssp_err_t R_DAC_InfoGet (dac_info_t    * const p_info)
{
#if DAC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure parameters are valid. */
    SSP_ASSERT(NULL != p_info);
#endif
    /** Assigning DAC bit resolution as 12bit. */
    p_info->bit_width = 12U;

    return SSP_SUCCESS;
} /* End of function R_DAC_InfoGet */


/*******************************************************************************************************************//**
 * @} (end addtogroup DAC)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

#if (DAC_CFG_PARAM_CHECKING_ENABLE)
/*******************************************************************************************************************//**
 * @brief   The parameter checking subroutine for R_DAC_Open API.
 * @param[in]     p_ctrl                     Pointer to the instance control structure for DAC
 * @param[in]     p_cfg                      Pointer to the configuration structure for DAC
 * @retval  SSP_SUCCESS                      No parameter error found
 **********************************************************************************************************************/
static ssp_err_t r_dac_open_param_check(dac_instance_ctrl_t * p_ctrl, dac_cfg_t const * const p_cfg)
{
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT((p_cfg->data_format == DAC_DATA_FORMAT_FLUSH_RIGHT) || (p_cfg->data_format == DAC_DATA_FORMAT_FLUSH_LEFT));
    return SSP_SUCCESS;
}
#endif
