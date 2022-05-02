#include "exti.h"

void Exti_Init(void)
{
    rcu_periph_clock_enable(EXTI_GPIO_CLK);
    rcu_periph_clock_enable(RCU_CFGCMP);//系统配置与比较时钟

    /* configure button pin as input */
    gpio_mode_set(EXTI_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, EXTI_GPIO_PIN);

    /* enable and set key wakeup EXTI interrupt to the higher priority */
    nvic_irq_enable(EXTI_IRQ, 2U, 0U);

    /* connect key wakeup EXTI line to key GPIO pin */
    syscfg_exti_line_config(EXTI_SOURCE_PORT, EXTI_SOURCE_PIN);

    /* configure key wakeup EXTI line */
    exti_init(EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_TYPE);
    exti_interrupt_flag_clear(EXTI_LINE);
}

/*!
    \brief      this function handles external lines 0 to 1 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_1_IRQHandler(void)
{
    /* check the key wakeup is pressed or not */
    if(RESET != exti_interrupt_flag_get(EXTI_LINE)) {
        exti_interrupt_flag_clear(EXTI_LINE);

        LED0_TOGGLE;
        LED1_TOGGLE;
    }
}

