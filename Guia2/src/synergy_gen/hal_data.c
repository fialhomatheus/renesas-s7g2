/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_external_irq11_btn4) && !defined(SSP_SUPPRESS_ISR_ICU11)
SSP_VECTOR_DEFINE( icu_irq_isr, ICU, IRQ11);
#endif
#endif
static icu_instance_ctrl_t g_external_irq11_btn4_ctrl;
static const external_irq_cfg_t g_external_irq11_btn4_cfg =
{ .channel = 11,
  .trigger = EXTERNAL_IRQ_TRIG_FALLING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .autostart = true,
  .p_callback = isr_irq11_btn4_callback,
  .p_context = &g_external_irq11_btn4,
  .p_extend = NULL,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq11_btn4 =
{ .p_ctrl = &g_external_irq11_btn4_ctrl, .p_cfg = &g_external_irq11_btn4_cfg, .p_api = &g_external_irq_on_icu };
#if (11) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_external_irq10_btn5) && !defined(SSP_SUPPRESS_ISR_ICU10)
SSP_VECTOR_DEFINE( icu_irq_isr, ICU, IRQ10);
#endif
#endif
static icu_instance_ctrl_t g_external_irq10_btn5_ctrl;
static const external_irq_cfg_t g_external_irq10_btn5_cfg =
{ .channel = 10,
  .trigger = EXTERNAL_IRQ_TRIG_FALLING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .autostart = true,
  .p_callback = isr_irq10_btn5_callback,
  .p_context = &g_external_irq10_btn5,
  .p_extend = NULL,
  .irq_ipl = (11), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq10_btn5 =
{ .p_ctrl = &g_external_irq10_btn5_ctrl, .p_cfg = &g_external_irq10_btn5_cfg, .p_api = &g_external_irq_on_icu };
void g_hal_init(void)
{
    g_common_init ();
}
