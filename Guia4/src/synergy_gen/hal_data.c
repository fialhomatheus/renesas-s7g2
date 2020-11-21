/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (11) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_timer2) && !defined(SSP_SUPPRESS_ISR_GPT2)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 2);
#endif
#endif
static gpt_instance_ctrl_t g_timer2_ctrl;
static const timer_on_gpt_cfg_t g_timer2_extend =
{ .gtioca =
{ .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_timer2_cfg =
{ .mode = TIMER_MODE_PWM, .period = 4, .unit = TIMER_UNIT_PERIOD_SEC, .duty_cycle = 50, .duty_cycle_unit =
          TIMER_PWM_UNIT_PERCENT,
  .channel = 2, .autostart = true, .p_callback = isr_timer2_callback, .p_context = &g_timer2, .p_extend =
          &g_timer2_extend,
  .irq_ipl = (11), };
/* Instance structure to use this module. */
const timer_instance_t g_timer2 =
{ .p_ctrl = &g_timer2_ctrl, .p_cfg = &g_timer2_cfg, .p_api = &g_timer_on_gpt };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_external_irq8_pwm) && !defined(SSP_SUPPRESS_ISR_ICU8)
SSP_VECTOR_DEFINE( icu_irq_isr, ICU, IRQ8);
#endif
#endif
static icu_instance_ctrl_t g_external_irq8_pwm_ctrl;
static const external_irq_cfg_t g_external_irq8_pwm_cfg =
{ .channel = 8,
  .trigger = EXTERNAL_IRQ_TRIG_FALLING,
  .filter_enable = false,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .autostart = true,
  .p_callback = isr_external_irq8_pwm_callback,
  .p_context = &g_external_irq8_pwm,
  .p_extend = NULL,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq8_pwm =
{ .p_ctrl = &g_external_irq8_pwm_ctrl, .p_cfg = &g_external_irq8_pwm_cfg, .p_api = &g_external_irq_on_icu };
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
  .p_callback = isr_external_irq11_btn4_callback,
  .p_context = &g_external_irq11_btn4,
  .p_extend = NULL,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq11_btn4 =
{ .p_ctrl = &g_external_irq11_btn4_ctrl, .p_cfg = &g_external_irq11_btn4_cfg, .p_api = &g_external_irq_on_icu };
#if (12) != BSP_IRQ_DISABLED
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
  .p_callback = isr_external_irq10_btn5_callback,
  .p_context = &g_external_irq10_btn5,
  .p_extend = NULL,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq10_btn5 =
{ .p_ctrl = &g_external_irq10_btn5_ctrl, .p_cfg = &g_external_irq10_btn5_cfg, .p_api = &g_external_irq_on_icu };
void g_hal_init(void)
{
    g_common_init ();
}
