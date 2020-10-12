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
 * File Name    : hw_dac_private.h
 * Description  : Header file for low level driver functions
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @addtogroup DAC
 * @{
 **********************************************************************************************************************/

#ifndef HW_DAC_PRIVATE_H
#define HW_DAC_PRIVATE_H

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_dac.h"

/* Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Map to the DAC registers */
typedef struct
{
    uint16_t  DADR0;
    uint16_t  DADR1;
    uint8_t   DACR;
    uint8_t   DADPR;
    uint8_t   DAADSCR;
    uint8_t   DAVREFCR;
    uint8_t   DAAMPCR;
    uint8_t   DAADUSR;
} hw_dac_ctrl_t;

typedef enum e_hw_dac_control
{
    HW_DAC_CONTROL_DISABLE = (0x0), ///< DAC control disable
    HW_DAC_CONTROL_ENABLE  = (0x1), ///< DAC control enable
    HW_DAC_CONTROL_MAX              ///< DAC control
} hw_dac_control_t;

typedef struct                                      /*!< DAC Structure
                                                     *                                                        */
{
    __I uint32_t   RESERVED[16];

    __IO uint16_t  DADRn[2];                      /*!< D/A Data Register %s (DADPR.DPSEL bit = 0 (right justified
                                                   *format))   */

    union
    {
        __IO uint8_t  DACR;                             /*!< D/A Control Register
                                                         *                                                 */

        struct
        {
            uint8_t             :  5;
            __IO uint8_t  DAE   :  1;                   /*!< D/A Enable
                                                         *                                                           */
            __IO uint8_t  DAOE0 :  1;                   /*!< D/A Output Enable 0
                                                         *                                                  */
            __IO uint8_t  DAOE1 :  1;                   /*!< D/A Output Enable 1
                                                         *                                                  */
        }  DACR_b;                                      /*!< BitSize
                                                         *
                                                         *
                                                         *                                                            */
    };

    union
    {
        __IO uint8_t  DADPR;                            /*!< DADRm Format Select Register
                                                         *                                         */

        struct
        {
            uint8_t             :  7;
            __IO uint8_t  DPSEL :  1;                   /*!< DADRm Format Select
                                                         *                                                  */
        }  DADPR_b;                                     /*!< BitSize
                                                         *
                                                         *
                                                         *                                                            */
    };

    union
    {
        __IO uint8_t  DAADSCR;                          /*!< D/A-A/D Synchronous Start Control Register
                                                         *                           */

        struct
        {
            uint8_t              :  7;
            __IO uint8_t  DAADST :  1;                  /*!< D/A-A/D Synchronous Conversion
                                                         *                                       */
        }  DAADSCR_b;                                   /*!< BitSize
                                                         *
                                                         *
                                                         *                                                            */
    };

    __I uint8_t  RESERVED1;

    union
    {
        __IO uint8_t  DAAMPCR;                          /*!< D/A Output Amplifier Control Register
                                                         *                                */

        struct
        {
            uint8_t              :  6;
            __IO uint8_t  DAAMP0 :  1;                  /*!< Amplifier Control 0
                                                         *                                                  */
            __IO uint8_t  DAAMP1 :  1;                  /*!< Amplifier Control 1
                                                         *                                                  */
        }  DAAMPCR_b;                                   /*!< BitSize
                                                         *
                                                         *
                                                         *                                                            */
    };
} HW_DAC_Type;

/***********************************************************************************************************************
 * Function prototypes
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/** Select implementation based on BSP here */
#include "common/hw_dac_common.h"

/* Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* HW_DAC_PRIVATE_H */

/*******************************************************************************************************************//**
 * @} (end addtogroup DAC)
 **********************************************************************************************************************/
