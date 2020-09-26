/* HAL-only entry function */
#include "hal_data.h"

/* GPIO Mapping */
#define GLED    IOPORT_PORT_06_PIN_00   //SK-S7G2 board LED1
#define RLED    IOPORT_PORT_06_PIN_01   //SK-S7G2 board LED2
#define YLED    IOPORT_PORT_06_PIN_02   //SK-S7G2 board LED3

#define BTN4    IOPORT_PORT_00_PIN_06   //SK-S7G2 board S4
#define BTN5    IOPORT_PORT_00_PIN_05   //SK-S7G2 board S5


void hal_entry(void)
{
    /* GPIO setup is done with Synergy Configuration tool */
    ioport_level_t  green_led = IOPORT_LEVEL_HIGH,
                    red_led = IOPORT_LEVEL_HIGH,
                    yellow_led = IOPORT_LEVEL_HIGH;
    ioport_level_t btn_4, btn_5;

    while(1)
    {
        g_ioport.p_api->pinRead(BTN4, &btn_4);
        g_ioport.p_api->pinRead(BTN5, &btn_5);

        /* Toggle LED state if associated button is pressed */
        green_led = !(green_led ^ btn_4);
        red_led = !(red_led ^ btn_5);

        /* Toggle yellow LED every 500ms */
        yellow_led = !yellow_led;
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        /* Update LED states */
        g_ioport.p_api->pinWrite(GLED, green_led);
        g_ioport.p_api->pinWrite(RLED, red_led);
        g_ioport.p_api->pinWrite(YLED, yellow_led);
    }
}
