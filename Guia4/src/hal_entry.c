#include "hal_data.h"

/* GPIO Mapping */
#define GLED    IOPORT_PORT_06_PIN_00   //SK-S7G2 board GLED
#define BTN4    IOPORT_PORT_00_PIN_06   //SK-S7G2 board S4
#define BTN5    IOPORT_PORT_00_PIN_05   //SK-S7G2 board S5
#define GTIOCB  1                       //GPT Timer2 PWM output (pin P102)
//P102 and P002 are connected by a jumper (PWM -> IRQ)


/* Function prototypes */
void drivers_setup();
void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args);
void isr_external_irq11_btn4_callback(external_irq_callback_args_t *p_args);
void isr_external_irq8_pwm_callback(external_irq_callback_args_t *p_args);
void isr_timer2_callback(timer_callback_args_t *p_args);


/* Declaration of global variables */
ioport_level_t green_led = IOPORT_LEVEL_HIGH;

external_irq_trigger_t irq_trigger = EXTERNAL_IRQ_TRIG_FALLING;
timer_size_t duty_cycle = 50;


void hal_entry(void)
{
    drivers_setup();

    for(;;)
    {

    }
}

void drivers_setup()
{
    /* GPIO, IRQs and Timer configurations done with Synergy Configuration Tool  */
    g_external_irq10_btn5.p_api->open(g_external_irq10_btn5.p_ctrl, g_external_irq10_btn5.p_cfg);
    g_external_irq11_btn4.p_api->open(g_external_irq11_btn4.p_ctrl, g_external_irq11_btn4.p_cfg);

    g_external_irq8_pwm.p_api->open(g_external_irq8_pwm.p_ctrl, g_external_irq8_pwm.p_cfg);
    g_external_irq8_pwm.p_api->triggerSet(g_external_irq8_pwm.p_ctrl, irq_trigger);

    g_timer2.p_api->open(g_timer2.p_ctrl, g_timer2.p_cfg);
}


void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    /* Decrease PWM duty cycle */
    duty_cycle += 10;
    if (duty_cycle > 100)
        duty_cycle = 100;
}


void isr_external_irq11_btn4_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    /* Increase PWM duty cycle */
    duty_cycle -= 10;
    if (duty_cycle <= 0)
        duty_cycle = 0;
}


void isr_external_irq8_pwm_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    /* Toggle LED, which is activated by negative logic! */
    green_led = !green_led;
    g_ioport.p_api->pinWrite(GLED, green_led);

    /* Change IRQ trigger in order to interrupt both t_on and t_off*/
    irq_trigger = irq_trigger == EXTERNAL_IRQ_TRIG_FALLING ? EXTERNAL_IRQ_TRIG_RISING : EXTERNAL_IRQ_TRIG_FALLING;
    g_external_irq8_pwm.p_api->triggerSet(g_external_irq8_pwm.p_ctrl, irq_trigger);
}


void isr_timer2_callback(timer_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);

    g_timer2.p_api->dutyCycleSet(g_timer2.p_ctrl, duty_cycle, TIMER_PWM_UNIT_PERCENT, GTIOCB);
}
