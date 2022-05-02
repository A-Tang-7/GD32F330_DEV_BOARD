#include "pwm.h"

static void PWM_Init(PWM_InitTypeDef pwm);

static void PWM_Init(PWM_InitTypeDef pwm)
{
    timer_oc_parameter_struct timer_ocintpara;
    Timer_InitTypeDef Timer_InitStructure={ .timer_clk      = pwm.timer_clk,
                                            .timer_periph   = pwm.periph,
                                            .timer_psc      = pwm.psc, 
                                            .timer_arr      = pwm.arr };

    gpio_mode_set(pwm.gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, pwm.gpio_pin);
    gpio_output_options_set(pwm.gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pwm.gpio_pin);
    gpio_af_set(pwm.gpio_port, GPIO_AF_2, pwm.gpio_pin);

    Timer_Init(Timer_InitStructure);

    /* configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(pwm.periph, pwm.channel, &timer_ocintpara);
    timer_channel_output_pulse_value_config(pwm.periph, pwm.channel, 0);
    timer_channel_output_mode_config(pwm.periph, pwm.channel, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(pwm.periph, pwm.channel, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(pwm.periph);
    /* auto-reload preload enable */
    timer_enable(pwm.periph);
}

void Timer1_CH2_PWM_Init(void)
{
    PWM_InitTypeDef Pwm_InitStructure = { .timer_clk = PWM_TIMER_CLK,
                                          .gpio_port = PWM_GPIO_PORT,
                                          .gpio_pin  = PWM_GPIO_PIN,
                                          .periph    = PWM_TIMER_PERIPH,
                                          .channel   = PWM_TIMER_CHANNEL,
                                          .psc       = PWM_TIMER_PSC,
                                          .arr       = PWM_TIMER_ARR };
    PWM_Init(Pwm_InitStructure);
}










