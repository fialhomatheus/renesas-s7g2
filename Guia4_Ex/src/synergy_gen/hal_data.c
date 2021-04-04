/* generated HAL source file - do not edit */
#include "hal_data.h"
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_agt0_adc_trigger) && !defined(SSP_SUPPRESS_ISR_AGT0)
SSP_VECTOR_DEFINE_CHAN(agt_int_isr, AGT, INT, 0);
#endif
#endif
static agt_instance_ctrl_t g_agt0_adc_trigger_ctrl;
static const timer_on_agt_cfg_t g_agt0_adc_trigger_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto_output_enabled = false,
  .agtio_output_enabled = false,
  .output_inverted = false,
  .agtoa_output_enable = false,
  .agtob_output_enable = false, };
static const timer_cfg_t g_agt0_adc_trigger_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  .period = 2,
  .unit = TIMER_UNIT_PERIOD_USEC,
  .channel = 0,
  .autostart = true,
  .p_callback = isr_agt0_adc_trigger_callback,
  .p_context = &g_agt0_adc_trigger,
  .p_extend = &g_agt0_adc_trigger_extend,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const timer_instance_t g_agt0_adc_trigger =
{ .p_ctrl = &g_agt0_adc_trigger_ctrl, .p_cfg = &g_agt0_adc_trigger_cfg, .p_api = &g_timer_on_agt };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc1_ch5_triangular) && !defined(SSP_SUPPRESS_ISR_ADC1)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 1);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc1_ch5_triangular) && !defined(SSP_SUPPRESS_ISR_ADC1)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 1);
#endif
#endif
adc_instance_ctrl_t g_adc1_ch5_triangular_ctrl;
const adc_cfg_t g_adc1_ch5_triangular_cfg =
{ .unit = 1,
  .mode = ADC_MODE_SINGLE_SCAN,
  .resolution = ADC_RESOLUTION_12_BIT,
  .alignment = ADC_ALIGNMENT_RIGHT,
  .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger = ADC_TRIGGER_SOFTWARE,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .p_callback = g_adc1_ch5_triangular_callback,
  .p_context = &g_adc1_ch5_triangular,
  .scan_end_ipl = (12),
  .scan_end_b_ipl = (BSP_IRQ_DISABLED),
  .calib_adc_skip = false,
  .voltage_ref = ADC_EXTERNAL_VOLTAGE,
  .over_current = OVER_CURRENT_DETECTION_ENABLE,
  .pga0 = PGA_DISABLE,
  .pga1 = PGA_DISABLE,
  .pga2 = PGA_DISABLE, };
const adc_channel_cfg_t g_adc1_ch5_triangular_channel_cfg =
{ .scan_mask = (uint32_t) (
        ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) ADC_MASK_CHANNEL_5) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | (0)),
  /** Group B channel mask is right shifted by 32 at the end to form the proper mask */
  .scan_mask_group_b = (uint32_t) (
          (((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) ADC_MASK_CHANNEL_5) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | (0)) >> 32),
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF, .add_mask = (uint32_t) (
          (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
                  | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)),
  .sample_hold_mask = (uint32_t) ((0) | (0) | (0)), .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc1_ch5_triangular =
{ .p_ctrl = &g_adc1_ch5_triangular_ctrl, .p_cfg = &g_adc1_ch5_triangular_cfg, .p_channel_cfg =
          &g_adc1_ch5_triangular_channel_cfg,
  .p_api = &g_adc_on_adc };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0_ch0_sine) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 0);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0_ch0_sine) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 0);
#endif
#endif
adc_instance_ctrl_t g_adc0_ch0_sine_ctrl;
const adc_cfg_t g_adc0_ch0_sine_cfg =
{ .unit = 0,
  .mode = ADC_MODE_SINGLE_SCAN,
  .resolution = ADC_RESOLUTION_12_BIT,
  .alignment = ADC_ALIGNMENT_RIGHT,
  .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger = ADC_TRIGGER_SOFTWARE,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .p_callback = isr_adc0_ch0_sine_callback,
  .p_context = &g_adc0_ch0_sine,
  .scan_end_ipl = (12),
  .scan_end_b_ipl = (BSP_IRQ_DISABLED),
  .calib_adc_skip = false,
  .voltage_ref = ADC_EXTERNAL_VOLTAGE,
  .over_current = OVER_CURRENT_DETECTION_ENABLE,
  .pga0 = PGA_DISABLE,
  .pga1 = PGA_DISABLE,
  .pga2 = PGA_DISABLE, };
const adc_channel_cfg_t g_adc0_ch0_sine_channel_cfg =
{ .scan_mask = (uint32_t) (
        ((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | (0)),
  /** Group B channel mask is right shifted by 32 at the end to form the proper mask */
  .scan_mask_group_b = (uint32_t) (
          (((uint64_t) ADC_MASK_CHANNEL_0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | (0)) >> 32),
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF, .add_mask = (uint32_t) (
          (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
                  | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)),
  .sample_hold_mask = (uint32_t) ((0) | (0) | (0)), .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0_ch0_sine =
{ .p_ctrl = &g_adc0_ch0_sine_ctrl,
  .p_cfg = &g_adc0_ch0_sine_cfg,
  .p_channel_cfg = &g_adc0_ch0_sine_channel_cfg,
  .p_api = &g_adc_on_adc };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_gpt0_pwm_synth) && !defined(SSP_SUPPRESS_ISR_GPT0)
SSP_VECTOR_DEFINE_CHAN(gpt_counter_overflow_isr, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif
static gpt_instance_ctrl_t g_gpt0_pwm_synth_ctrl;
static const timer_on_gpt_cfg_t g_gpt0_pwm_synth_extend =
{ .gtioca =
{ .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
  .gtiocb =
  { .output_enabled = true, .stop_level = GPT_PIN_LEVEL_LOW },
  .shortest_pwm_signal = GPT_SHORTEST_LEVEL_OFF, };
static const timer_cfg_t g_gpt0_pwm_synth_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  .period = 50,
  .unit = TIMER_UNIT_PERIOD_USEC,
  .duty_cycle = 50,
  .duty_cycle_unit = TIMER_PWM_UNIT_PERCENT,
  .channel = 0,
  .autostart = true,
  .p_callback = isr_gpt0_pwm_synth_callback,
  .p_context = &g_gpt0_pwm_synth,
  .p_extend = &g_gpt0_pwm_synth_extend,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const timer_instance_t g_gpt0_pwm_synth =
{ .p_ctrl = &g_gpt0_pwm_synth_ctrl, .p_cfg = &g_gpt0_pwm_synth_cfg, .p_api = &g_timer_on_gpt };
void g_hal_init(void)
{
    g_common_init ();
}
