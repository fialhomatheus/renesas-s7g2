/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (2) != BSP_IRQ_DISABLED
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
  .irq_ipl = (2), };
/* Instance structure to use this module. */
const external_irq_instance_t g_external_irq10_btn5 =
{ .p_ctrl = &g_external_irq10_btn5_ctrl, .p_cfg = &g_external_irq10_btn5_cfg, .p_api = &g_external_irq_on_icu };
#if !defined(SSP_SUPPRESS_ISR_g_input_capture) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_input_capture) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_isr, GPT, CAPTURE_COMPARE_A, 0);
#endif
static gpt_input_capture_instance_ctrl_t g_input_capture_ctrl;
static const gpt_input_capture_extend_t g_input_capture_extend =
{ .signal = GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCA, .signal_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, .clock_divider =
          GPT_INPUT_CAPTURE_CLOCK_DIVIDER_1,
  .enable_level = INPUT_CAPTURE_SIGNAL_LEVEL_NONE, .enable_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, };
static const input_capture_cfg_t g_input_capture_cfg =
{ .channel = 0, .mode = INPUT_CAPTURE_MODE_PULSE_WIDTH, .edge = INPUT_CAPTURE_SIGNAL_EDGE_FALLING, .repetition =
          INPUT_CAPTURE_REPETITION_PERIODIC,
  .autostart = true, .p_callback = input_capture_callback, .p_context = &g_input_capture, .p_extend =
          &g_input_capture_extend,
  .capture_irq_ipl = (3), .overflow_irq_ipl = (3), };
/* Instance structure to use this module. */
const input_capture_instance_t g_input_capture =
{ .p_ctrl = &g_input_capture_ctrl, .p_cfg = &g_input_capture_cfg, .p_api = &g_input_capture_on_gpt };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_hcsr04_echo) && !defined(SSP_SUPPRESS_ISR_GPT2)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 2);
#endif
#endif
static gpt_instance_ctrl_t g_hcsr04_echo_ctrl;
static const timer_on_gpt_cfg_t g_hcsr04_echo_extend =
{ .gtioca =
{ .output_enabled = true, .stop_level = GPT_PIN_LEVEL_HIGH },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_hcsr04_echo_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  .period = 1740,
  .unit = TIMER_UNIT_PERIOD_USEC,
  .duty_cycle = 50,
  .duty_cycle_unit = TIMER_PWM_UNIT_PERCENT,
  .channel = 2,
  .autostart = true,
  .p_callback = NULL,
  .p_context = &g_hcsr04_echo,
  .p_extend = &g_hcsr04_echo_extend,
  .irq_ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const timer_instance_t g_hcsr04_echo =
{ .p_ctrl = &g_hcsr04_echo_ctrl, .p_cfg = &g_hcsr04_echo_cfg, .p_api = &g_timer_on_gpt };
void g_hal_init(void)
{
    g_common_init ();
}
