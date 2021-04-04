#include <stdio.h>
#include "input_capture.h"

#define CHANNEL_ZERO (0U)
#define TIMER_SECOND (1000000000U)
#define TIMER_MILLISECOND (1000000U)
#define TIMER_MICROSECOND (1000U)
#define BIT_32 (0x100000000U)

static uint64_t capture_overflow = 0;
static uint32_t capture_counter = 0;

void input_capture_callback(input_capture_callback_args_t * p_args)
{
    static uint32_t pclk_freq_hz = 0;
    static uint64_t time_captured = 0;

    if(CHANNEL_ZERO == p_args->channel)
    {
        switch(p_args->event)
        {
            case INPUT_CAPTURE_EVENT_MEASUREMENT:

                /* Get the value of the captured counter and overflows number */
                capture_counter = p_args->counter;

                /* Get the frequency of PCLKD in Hz*/
                g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_PCLKD, &pclk_freq_hz);

                /* Calculate time value of measurement (ns) */
                time_captured = (uint64_t)(((capture_overflow * BIT_32) + (uint64_t)capture_counter) * TIMER_SECOND / pclk_freq_hz);

                /* Calculate distance from the pulse width measured. Refer to the HC-SR04 datasheet */
                distance = (float)time_captured/58000;

                time_captured = 0;
                capture_overflow = 0;
                break;

            case INPUT_CAPTURE_EVENT_OVERFLOW:

                /* Overflows counter add one */
                capture_overflow++;

                break;
            default:
                break;
        }
    }
}

