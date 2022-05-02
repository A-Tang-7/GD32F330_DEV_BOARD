#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "gd32f3x0.h"
#include "systick.h"

#define ENCODER_A_GPIO_CLK      RCU_GPIOB
#define ENCODER_A_GPIO_PORT     GPIOB
#define ENCODER_A_GPIO_PIN      GPIO_PIN_4
#define ENCODER_A_GPIO_AF       GPIO_AF_1

#define ENCODER_B_GPIO_CLK      RCU_GPIOB
#define ENCODER_B_GPIO_PORT     GPIOB
#define ENCODER_B_GPIO_PIN      GPIO_PIN_5
#define ENCODER_B_GPIO_AF       GPIO_AF_1

#define ENCODER_TIMER_PERIPH    TIMER2
#define ENCODER_TIMER_CLK       RCU_TIMER2
#define ENCODER_A_TIMER_CHANNEL TIMER_CH_0
#define ENCODER_B_TIMER_CHANNEL TIMER_CH_1

#define COUNTER_INIT 32768

typedef struct
{
    rcu_periph_enum a_gipo_clk;
    rcu_periph_enum b_gpio_clk;
    rcu_periph_enum timer_clk;
    uint32_t        a_gpio_port;
    uint32_t        a_gpio_pin;
    uint32_t        a_gpio_af;
    uint32_t        b_gpio_port;
    uint32_t        b_gpio_pin;
    uint32_t        b_gpio_af;
    uint32_t        timer_periph;
    uint16_t        a_timer_channel;
    uint16_t        b_timer_channel;
}Encoder_InitTypeDef;


void TIMER2_CH0_CH1_Encoder_Init(void);
int Get_TIMER2_Encoder(void);

#endif

