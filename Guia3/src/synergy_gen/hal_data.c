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
dac_instance_ctrl_t g_dac0_ctrl;
const dac_extended_cfg_t g_dac0_ex_ctrl =
{ .enable_charge_pump = false, };
const dac_cfg_t g_dac0_cfg =
{ .channel = 1, .ad_da_synchronized = false, .data_format = DAC_DATA_FORMAT_FLUSH_RIGHT, .output_amplifier_enabled =
          true,
  .p_extend = &g_dac0_ex_ctrl };
/* Instance structure to use this module. */
const dac_instance_t g_dac0 =
{ .p_ctrl = &g_dac0_ctrl, .p_cfg = &g_dac0_cfg, .p_api = &g_dac_on_dac };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 0);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 0);
#endif
#endif
adc_instance_ctrl_t g_adc0_ctrl;
const adc_cfg_t g_adc0_cfg =
{ .unit = 0,
  .mode = ADC_MODE_SINGLE_SCAN,
  .resolution = ADC_RESOLUTION_12_BIT,
  .alignment = ADC_ALIGNMENT_RIGHT,
  .add_average_count = ADC_ADD_OFF,
  .clearing = ADC_CLEAR_AFTER_READ_ON,
  .trigger = ADC_TRIGGER_SOFTWARE,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
  .p_callback = isr_adc0_callback,
  .p_context = &g_adc0,
  .scan_end_ipl = (12),
  .scan_end_b_ipl = (BSP_IRQ_DISABLED),
  .calib_adc_skip = false,
  .voltage_ref = ADC_EXTERNAL_VOLTAGE,
  .over_current = OVER_CURRENT_DETECTION_ENABLE,
  .pga0 = PGA_DISABLE,
  .pga1 = PGA_DISABLE,
  .pga2 = PGA_DISABLE, };
const adc_channel_cfg_t g_adc0_channel_cfg =
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
const adc_instance_t g_adc0 =
{ .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg, .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
#if (12) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_timer0) && !defined(SSP_SUPPRESS_ISR_AGT0)
SSP_VECTOR_DEFINE_CHAN(agt_int_isr, AGT, INT, 0);
#endif
#endif
static agt_instance_ctrl_t g_timer0_ctrl;
static const timer_on_agt_cfg_t g_timer0_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto_output_enabled = false,
  .agtio_output_enabled = false,
  .output_inverted = false,
  .agtoa_output_enable = false,
  .agtob_output_enable = false, };
static const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  .period = 5,
  .unit = TIMER_UNIT_PERIOD_MSEC,
  .channel = 0,
  .autostart = true,
  .p_callback = isr_timer0_callback,
  .p_context = &g_timer0,
  .p_extend = &g_timer0_extend,
  .irq_ipl = (12), };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_agt };
void g_hal_init(void)
{
    g_common_init ();
}
