/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_agt0_adc_trigger;
#ifndef isr_agt0_adc_trigger_callback
void isr_agt0_adc_trigger_callback(timer_callback_args_t *p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc1_ch5_triangular;
#ifndef g_adc1_ch5_triangular_callback
void g_adc1_ch5_triangular_callback(adc_callback_args_t *p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0_ch0_sine;
#ifndef isr_adc0_ch0_sine_callback
void isr_adc0_ch0_sine_callback(adc_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_gpt0_pwm_synth;
#ifndef isr_gpt0_pwm_synth_callback
void isr_gpt0_pwm_synth_callback(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
