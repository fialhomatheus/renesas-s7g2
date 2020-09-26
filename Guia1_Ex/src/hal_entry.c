#include "hal_data.h"

/* GPIO Mapping:
 *      green_led   -> Pin 0, Port 6
 *      red_led     -> Pin 1, Port 6
 *      yellow_led  -> Pin 2, Port 6
 *      green_btn   -> Pin 6, Port 0
 *      red_btn     -> Pin 5, Port 0
 */
#define LEDS    IOPORT_PORT_06
#define BTNS    IOPORT_PORT_00

void hal_entry(void)
{
    /* GPIO setup is done with Synergy Configuration tool */
    ioport_level_t  green_led = IOPORT_LEVEL_HIGH,
                    red_led = IOPORT_LEVEL_HIGH,
                    yellow_led = IOPORT_LEVEL_HIGH;
    ioport_level_t  green_btn, red_btn;

    ioport_size_t   led_port_state = 0x0007,
                    buttons = 0x0007;

    for (;;)
    {
        /* Read button state and wait 500ms */
        g_ioport.p_api->portRead(BTNS, &buttons);
        green_btn = (ioport_level_t)((buttons & 0x0040) >> 6);
        red_btn = (ioport_level_t)((buttons & 0x0020) >> 5);

        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        /* Toggle led if associated button is pressed */
        green_led = !(green_led ^ green_btn);
        red_led = !(red_led ^ red_btn);

        /* Toggle yellow led every 500ms */
        yellow_led = !yellow_led;

        led_port_state = (ioport_size_t)((green_led << 0) | (red_led << 1) | (yellow_led << 2));
        g_ioport.p_api->portWrite(LEDS, led_port_state, 0x0007);
    }
}
