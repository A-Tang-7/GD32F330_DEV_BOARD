#ifndef __TIMER_H_
#define __TIMER_H_

#include "gd32f3x0.h"
#include "led.h"

#define TIMER1_IRQ      TIMER1_IRQn
#define TIMER1_CLK      RCU_TIMER1
#define TIMER1_PERIPH   TIMER1
#define TIMER1_PSC_1US  8400-1
#define TIMER1_ARR_1US  1
#define TIMER1_PSC_5US  (8400*5)-1      

typedef struct
{
    uint8_t         timer_nvic_irq;
    rcu_periph_enum timer_clk;
    uint32_t        timer_periph;
    uint16_t        timer_psc;
    uint16_t        timer_arr;
}Timer_InitTypeDef;

void Timer_Init(Timer_InitTypeDef timer);
void Timer1_IRQ_Init(uint32_t time_us);

#endif

