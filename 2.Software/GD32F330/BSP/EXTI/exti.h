#ifndef __EXTI_H_
#define __EXTI_H_

#include "gd32f3x0.h"
#include "led.h"

#define EXTI_GPIO_CLK       RCU_GPIOA
#define EXTI_GPIO_PORT      GPIOA  
#define EXTI_GPIO_PIN       GPIO_PIN_0
#define EXTI_IRQ            EXTI0_1_IRQn
#define EXTI_SOURCE_PORT    EXTI_SOURCE_GPIOA
#define EXTI_SOURCE_PIN     EXTI_SOURCE_PIN0
#define EXTI_LINE           EXTI_0
#define EXTI_TRIG_TYPE      EXTI_TRIG_FALLING

void Exti_Init(void);

#endif

