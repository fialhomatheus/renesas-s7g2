/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq11_btn4;
#ifndef isr_irq11_btn4_callback
void isr_irq11_btn4_callback(external_irq_callback_args_t *p_args);
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq10_btn5;
#ifndef isr_irq10_btn5_callback
void isr_irq10_btn5_callback(external_irq_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* HAL_DATA_H_ */
