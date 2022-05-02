#include "adc.h"

static uint16_t Get_ADC(void);

void ADC_Init(void)
{
    rcu_periph_clock_enable(RCU_ADC);
    rcu_periph_clock_enable(ADC_GPIO_CLK);
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);

    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);//右对齐
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);//规则组
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);//无外部触发
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    adc_enable();
    delay_1ms(1U);
    adc_calibration_enable();//校准、复位、使能    
}

/*!
    \brief      读取一次ADC数值
    \param[in]  none
    \param[out] none
    \retval     adc_val
*/
static uint16_t Get_ADC(void)
{
    adc_regular_channel_config(0U, ADC_CHANNEL, ADC_SAMPLETIME_239POINT5);
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
    while(!adc_flag_get(ADC_FLAG_EOC));
    adc_flag_clear(ADC_FLAG_EOC);
    return (adc_regular_data_read());
}

/*!
    \brief      读取ADC数值，并通过均值滤波
    \param[in]  times：读取次数，一般取10次
    \param[out] none
    \retval     temp_val/times
*/
uint16_t Get_ADC_Average(const uint8_t times)
{
    uint32_t temp_val=0;
    uint8_t t;
    for(t=0;t<times;t++)
    {
        temp_val += Get_ADC();
        delay_1ms(5);
    }
    return temp_val/times;
}




