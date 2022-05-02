#include "led.h"

void Led_Init(void)
{
    rcu_periph_clock_enable(LED0_GPIO_CLK);
    rcu_periph_clock_enable(LED1_GPIO_CLK);

    gpio_mode_set(LED0_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED0_PIN);
    gpio_mode_set(LED1_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_PIN);
    gpio_output_options_set(LED0_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, LED0_PIN);
    gpio_output_options_set(LED1_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, LED1_PIN);
    
    gpio_bit_set(LED0_GPIO_PORT, LED0_PIN);
    gpio_bit_set(LED1_GPIO_PORT, LED1_PIN);
}


