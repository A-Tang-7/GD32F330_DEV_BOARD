#ifndef __PWM_H_
#define __PWM_H_

#include "gd32f3x0.h"
#include "timer.h"

typedef struct
{
    rcu_periph_enum timer_clk;
    uint32_t        gpio_port;
    uint32_t        gpio_pin;
    uint32_t        periph;
    uint16_t        channel;
    uint16_t        psc;
    uint16_t        arr;
}PWM_InitTypeDef;

#define PWM_GPIO_PORT       GPIOB
#define PWM_GPIO_PIN        GPIO_PIN_10
#define PWM_TIMER_CLK       RCU_TIMER1
#define PWM_TIMER_PERIPH    TIMER1
#define PWM_TIMER_CHANNEL   TIMER_CH_2
#define PWM_TIMER_PSC       84-1    //10KHz
#define PWM_TIMER_ARR       100-1

void Timer1_CH2_PWM_Init(void);


#endif

