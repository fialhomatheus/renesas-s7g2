/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "r_dac.h"
#include "r_dac_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_agt.h"
#include "r_timer_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq11_btn4;
#ifndef isr_external_irq11_btn4_callback
void isr_external_irq11_btn4_callback(external_irq_callback_args_t *p_args);
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq10_btn5;
#ifndef isr_external_irq10_btn5_callback
void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args);
#endif
/** DAC on DAC Instance. */
extern const dac_instance_t g_dac0;
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;
#ifndef isr_adc0_callback
void isr_adc0_callback(adc_callback_args_t *p_args);
#endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer0;
#ifndef isr_timer0_callback
void isr_timer0_callback(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
