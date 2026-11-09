#include "bsp_adc.h"


int32_t temperature_value;
/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
       __IO uint32_t idx = 0;
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC2);
    /* reset ADC */
    adc_deinit(ADC2);
    /* ADC clock config */
    adc_clock_config(ADC2, ADC_CLK_SYNC_HCLK_DIV6);
    /* ADC contineous function enable */
    adc_special_function_config(ADC2, ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan mode enable */
    adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
    /* ADC resolution config */
    adc_resolution_config(ADC2, ADC_RESOLUTION_12B);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC2,ADC_INSERTED_CHANNEL,4);

    /* ADC 1/4 voltate of external battery config */
    adc_inserted_channel_config(ADC2,0,ADC_CHANNEL_17,240);
    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(ADC2,1,ADC_CHANNEL_18,240);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(ADC2,2,ADC_CHANNEL_19,240);
    /* ADC high precision temperature sensor channel config */
    adc_inserted_channel_config(ADC2,3,ADC_CHANNEL_20,240);

    /* enable 1/4 voltate of external battery channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VBAT, ENABLE);
    /* enable ADC temperature channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMPSENSOR, ENABLE);
    /* enable internal reference voltage channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VREFINT, ENABLE);
    /* enable high precision temperature sensor channel */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_HP_TEMPSENSOR, ENABLE);

    /* ADC trigger config */
    adc_external_trigger_config(ADC2, ADC_INSERTED_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* enable ADC interface */
    adc_enable(ADC2);
    /* wait for ADC stability */
    for(idx=0; idx<0x1FFF; idx++){
    }
    /* ADC calibration mode config */
    adc_calibration_mode_config(ADC2, ADC_CALIBRATION_OFFSET_MISMATCH);
    /* ADC calibration number config */
    adc_calibration_number(ADC2, ADC_CALIBRATION_NUM1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);
}

float Get_temperature(void)
{
/* ADC software trigger enable */
    adc_software_trigger_enable(ADC2, ADC_INSERTED_CHANNEL);
    /* value convert */
    return (((0.66f - ADC_IDATA1(ADC2)*3.3f/4096)/1.838f*1000) + 10);
}