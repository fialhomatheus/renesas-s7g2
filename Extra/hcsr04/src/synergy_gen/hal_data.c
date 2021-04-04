/* generated HAL source file - do not edit */
#include "hal_data.h"
#if !defined(SSP_SUPPRESS_ISR_g_echo_measurement) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_echo_measurement) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_input_capture_isr, GPT, CAPTURE_COMPARE_A, 0);
#endif
static gpt_input_capture_instance_ctrl_t g_echo_measurement_ctrl;
static const gpt_input_capture_extend_t g_echo_measurement_extend =
{ .signal = GPT_INPUT_CAPTURE_SIGNAL_PIN_GTIOCA, .signal_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, .clock_divider =
          GPT_INPUT_CAPTURE_CLOCK_DIVIDER_1,
  .enable_level = INPUT_CAPTURE_SIGNAL_LEVEL_NONE, .enable_filter = GPT_INPUT_CAPTURE_SIGNAL_FILTER_NONE, };
static const input_capture_cfg_t g_echo_measurement_cfg =
{ .channel = 0, .mode = INPUT_CAPTURE_MODE_PULSE_WIDTH, .edge = INPUT_CAPTURE_SIGNAL_EDGE_RISING, .repetition =
          INPUT_CAPTURE_REPETITION_PERIODIC,
  .autostart = true, .p_callback = echo_measurement_callback, .p_context = &g_echo_measurement, .p_extend =
          &g_echo_measurement_extend,
  .capture_irq_ipl = (12), .overflow_irq_ipl = (12), };
/* Instance structure to use this module. */
const input_capture_instance_t g_echo_measurement =
{ .p_ctrl = &g_echo_measurement_ctrl, .p_cfg = &g_echo_measurement_cfg, .p_api = &g_input_capture_on_gpt };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_trigger) && !defined(SSP_SUPPRESS_ISR_GPT11)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 11);
#endif
#endif
static gpt_instance_ctrl_t g_trigger_ctrl;
static const timer_on_gpt_cfg_t g_trigger_extend =
{ .gtioca =
{ .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = false, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_trigger_cfg =
{ .mode = TIMER_MODE_PWM,
  .period = 110,
  .unit = TIMER_UNIT_PERIOD_MSEC,
  .duty_cycle = 9099,
  .duty_cycle_unit = TIMER_PWM_UNIT_PERCENT_X_1000,
  .channel = 11,
  .autostart = true,
  .p_callback = NULL,
  .p_context = &g_trigger,
  .p_extend = &g_trigger_extend,
  .irq_ipl = (BSP_IRQ_DISABLED), };
/* Instance structure to use this module. */
const timer_instance_t g_trigger =
{ .p_ctrl = &g_trigger_ctrl, .p_cfg = &g_trigger_cfg, .p_api = &g_timer_on_gpt };
void g_hal_init(void)
{
    g_common_init ();
}
