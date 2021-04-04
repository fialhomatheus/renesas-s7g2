/* HAL-only entry function */
#include <math.h>
#include "hal_data.h"
#include "input_capture.h"

/* GPIO Mapping */
#define GLED    IOPORT_PORT_06_PIN_00   //SK-S7G2 board LED1
#define RLED    IOPORT_PORT_06_PIN_01   //SK-S7G2 board LED2
#define YLED    IOPORT_PORT_06_PIN_02   //SK-S7G2 board LED3
#define BTN5    IOPORT_PORT_00_PIN_05   //SK-S7G2 board S5
#define BUZZER  IOPORT_PORT_06_PIN_03   //Piezoelectric buzzer

/* High part (t_on) of the buzzer waveform. Used to set its period.
 * Its frequency changes to indicate proximity to the sensor. */
uint16_t buzzer_t_on = 100;

input_capture_info_t input_capture_info;
float distance = 0;

/* Function prototypes */
void init_drivers(void);
void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args);


void hal_entry(void)
{
    init_drivers();

    ioport_level_t buzzer_state = IOPORT_LEVEL_LOW;
    uint16_t ton;

    while(1)
    {
        switch ((uint8_t)roundf(distance)){
            case 10:
                g_ioport.p_api->pinWrite(RLED, IOPORT_LEVEL_LOW);
                g_ioport.p_api->pinWrite(YLED, IOPORT_LEVEL_HIGH);
                g_ioport.p_api->pinWrite(GLED, IOPORT_LEVEL_HIGH);
                buzzer_t_on = 100;
                break;
            case 20:
                g_ioport.p_api->pinWrite(RLED, IOPORT_LEVEL_HIGH);
                g_ioport.p_api->pinWrite(YLED, IOPORT_LEVEL_LOW);
                g_ioport.p_api->pinWrite(GLED, IOPORT_LEVEL_HIGH);
                buzzer_t_on = 300;
                break;
            case 30:
                g_ioport.p_api->pinWrite(RLED, IOPORT_LEVEL_HIGH);
                g_ioport.p_api->pinWrite(YLED, IOPORT_LEVEL_HIGH);
                g_ioport.p_api->pinWrite(GLED, IOPORT_LEVEL_LOW);
                buzzer_t_on = 750;
                break;
            default:
                break;
        }

        /* High part of the buzzer waveform */
        for(ton = 0; ton < buzzer_t_on; ton++)
        {
            g_ioport.p_api->pinWrite(BUZZER, buzzer_state);
            buzzer_state = !buzzer_state;
            R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MICROSECONDS);
        }

        /* Low part of the buzzer waveform */
        g_ioport.p_api->pinWrite(BUZZER, IOPORT_LEVEL_LOW);
        R_BSP_SoftwareDelay(buzzer_t_on, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

void init_drivers(void)
{
    /* Initialize GPT Timer Driver */
    g_hcsr04_echo.p_api->open(g_hcsr04_echo.p_ctrl, g_hcsr04_echo.p_cfg);

    /* Initialize Input Capture Driver */
    g_input_capture.p_api->open(g_input_capture.p_ctrl, g_input_capture.p_cfg);

    /* Get the status of the measurement counter */
    g_input_capture.p_api->infoGet(g_input_capture.p_ctrl, &input_capture_info);

    /* Initialize external IRQ from SK-S7G2 S5 */
    g_external_irq10_btn5.p_api->open(g_external_irq10_btn5.p_ctrl, g_external_irq10_btn5.p_cfg);
}


/* 0 - Obstacle nearby
 * 1 - Obstacle in medium range
 * 2 - Obstacle far away
 * */
uint8_t distance_range = 0;

void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    /* Add an anti-bounce filter */
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    /* Cycle through the different distances */
    distance_range++;
    distance_range %= 3;

    switch (distance_range) {
        case 0:
            /* Set pulse width to 580us, which corresponds to 10cm */
            g_hcsr04_echo.p_api->periodSet(g_hcsr04_echo.p_ctrl, 580, TIMER_UNIT_PERIOD_USEC);
            break;
        case 1:
            /* Set pulse width to 1160us, which corresponds to 20cm */
            g_hcsr04_echo.p_api->periodSet(g_hcsr04_echo.p_ctrl, 1160, TIMER_UNIT_PERIOD_USEC);
            break;
        case 2:
            /* Set pulse width to 1740us, which corresponds to 30cm */
            g_hcsr04_echo.p_api->periodSet(g_hcsr04_echo.p_ctrl, 1740, TIMER_UNIT_PERIOD_USEC);
            break;
        default:
            break;
    }
}
