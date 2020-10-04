/* HAL-only entry function */
#include "hal_data.h"

/* GPIO Mapping */
#define GLED    IOPORT_PORT_06_PIN_00   //SK-S7G2 board LED1
#define RLED    IOPORT_PORT_06_PIN_01   //SK-S7G2 board LED2
#define YLED    IOPORT_PORT_06_PIN_02   //SK-S7G2 board LED3

#define BTN4    IOPORT_PORT_00_PIN_06   //SK-S7G2 board S4
#define BTN5    IOPORT_PORT_00_PIN_05   //SK-S7G2 board S5

/* Function prototypes */
void isr_irq11_btn4_callback(external_irq_callback_args_t *p_args);
void isr_irq10_btn5_callback(external_irq_callback_args_t *p_args);

void hal_entry(void)
{
    /* GPIO and IRQ setup is done with Synergy Configuration tool */
    ioport_level_t  green_led = IOPORT_LEVEL_HIGH,
                    red_led = IOPORT_LEVEL_HIGH,
                    yellow_led = IOPORT_LEVEL_HIGH;

    g_external_irq11_btn4.p_api->open(g_external_irq11_btn4.p_ctrl, g_external_irq11_btn4.p_cfg);
    g_external_irq10_btn5.p_api->open(g_external_irq10_btn5.p_ctrl, g_external_irq10_btn5.p_cfg);

    while(1)
    {
        /* Update LED states */
        g_ioport.p_api->pinWrite(GLED, green_led);
        g_ioport.p_api->pinWrite(RLED, red_led);
        g_ioport.p_api->pinWrite(YLED, yellow_led);

        /* Toggle yellow LED every 500ms */
        yellow_led = !yellow_led;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

    }
}

void isr_irq11_btn4_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    g_ioport.p_api->pinWrite(GLED, IOPORT_LEVEL_LOW);
    g_ioport.p_api->pinWrite(RLED, IOPORT_LEVEL_HIGH);
    g_ioport.p_api->pinWrite(YLED, IOPORT_LEVEL_HIGH);

    R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_SECONDS);
}

void isr_irq10_btn5_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    g_ioport.p_api->pinWrite(GLED, IOPORT_LEVEL_HIGH);
    g_ioport.p_api->pinWrite(RLED, IOPORT_LEVEL_LOW);
    g_ioport.p_api->pinWrite(YLED, IOPORT_LEVEL_HIGH);

    R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_SECONDS);
}



