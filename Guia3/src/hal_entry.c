/* HAL-only entry function */
#include "hal_data.h"
#include "math.h"


/* GPIO Mapping */
#define GLED    IOPORT_PORT_06_PIN_00   //SK-S7G2 board GLED
#define YLED    IOPORT_PORT_06_PIN_02   //SK-S7G2 board YLED

#define BTN4    IOPORT_PORT_00_PIN_06   //SK-S7G2 board S4
#define BTN5    IOPORT_PORT_00_PIN_05   //SK-S7G2 board S5
//P000 and P015 are connected by a jumper (ADC <- DAC)


/* Definition of constants */
#define M_PI        3.14159265358979323846
#define MAX_VALUE   4095
#define MAX_POINTS  1000
#define OMEGA       (2*M_PI/MAX_POINTS) //ω = 2π/T


/* Definition of macros */
#define _ftrans(x, xi, xm, yi, ym)  (float)((float)((float)(x-xi)*(float)(ym-yi))/(float)(xm-xi)+yi)


/* Declaration of global variables */
uint16_t *waveform_ptr;
uint16_t sine_lut[MAX_POINTS];
uint16_t triangular_lut[MAX_POINTS];

float waveform_voltage = 0; //Set to global for visualization in Real-Time Chart in Debug mode
ioport_level_t  green_led = IOPORT_LEVEL_HIGH,
                yellow_led = IOPORT_LEVEL_HIGH;


/* Function prototypes */
void generate_luts();
void isr_timer0_callback(timer_callback_args_t *p_args);
void isr_adc0_callback(adc_callback_args_t *p_args);
void isr_external_irq11_btn4_callback(external_irq_callback_args_t *p_args);
void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args);


void hal_entry(void)
{
    /* Generate Look-up tables and set sine-wave as default */
    generate_luts();
    waveform_ptr = sine_lut;
    
    /* Instantiate DAC, ADC, Timer and Interrupt drivers */
    g_dac0.p_api->open(g_dac0.p_ctrl, g_dac0.p_cfg);
    g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    g_external_irq11_btn4.p_api->open(g_external_irq11_btn4.p_ctrl, g_external_irq11_btn4.p_cfg);
    g_external_irq10_btn5.p_api->open(g_external_irq10_btn5.p_ctrl, g_external_irq10_btn5.p_cfg);

    /* Setup ADC scan */
    g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
    g_adc0.p_api->scanStart(g_adc0.p_ctrl);

    while(1);
}


void generate_luts()
{
    float function_value;
    for (int32_t x = 0; x < MAX_POINTS; x++)
    {
        /* Sine-wave generation using math::sin() */
        function_value = (float)((MAX_VALUE/2)*(sin(OMEGA*x) + 1));
        sine_lut[x] = (uint16_t)round(function_value);

        /* Triangular-wave generation using two linear functions (y = mx + n)
         *  f(x) = (2MAX_VALUE/MAX_POINTS)*x                , if 0 < x < MAX_POINTS/2
         *  f(x) = -(2MAX_VALUE/MAX_POINTS)*x + 2*MAX_VALUE , if MAX_POINTS/2 < x < MAX_POINTS
         */
        if(x < (MAX_POINTS/2))
            function_value = (float)(x*2*MAX_VALUE/MAX_POINTS);
        else
            function_value = (float)(2*MAX_VALUE - x*2*MAX_VALUE/MAX_POINTS);
        triangular_lut[x] = (uint16_t)round(function_value);
    }
}

void isr_timer0_callback(timer_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    static uint16_t plot_point = 0;
    static uint8_t k = 0;

    /* Blink green LED */
    if (++k == 50)
    {
        k = 0;
        green_led = !green_led;
        g_ioport.p_api->pinWrite(GLED, green_led);
    }

    /* Synthesize selected waveform */
    g_dac0.p_api->write(g_dac0.p_ctrl, waveform_ptr[plot_point++]);
    plot_point %= MAX_POINTS;
}

void isr_adc0_callback(adc_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    uint16_t waveform_adc_data = 0;
    static uint32_t k = 0;

    /* Blink yellow LED */
    if (++k == 200000)
    {
        k = 0;
        yellow_led = !yellow_led;
        g_ioport.p_api->pinWrite(YLED, yellow_led);
    }

    /* Read ADC data and setup next scan */
    g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &waveform_adc_data);
    waveform_voltage = _ftrans(waveform_adc_data, 0, MAX_VALUE, 0, 3.3f);
    g_adc0.p_api->scanStart(g_adc0.p_ctrl);
}

void isr_external_irq11_btn4_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    
    /* Select sine wave Look-up Table*/
    waveform_ptr = sine_lut;
}

void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args)
{
    SSP_PARAMETER_NOT_USED(p_args);
    
    /* Select triangular wave Look-up Table*/
    waveform_ptr = triangular_lut;
}
