/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "r_gpt_input_capture.h"
#include "r_input_capture_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq10_btn5;
#ifndef isr_external_irq10_btn5_callback
void isr_external_irq10_btn5_callback(external_irq_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const input_capture_instance_t g_input_capture;
#ifndef input_capture_callback
void input_capture_callback(input_capture_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t g_hcsr04_echo;
#ifndef NULL
void NULL(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
