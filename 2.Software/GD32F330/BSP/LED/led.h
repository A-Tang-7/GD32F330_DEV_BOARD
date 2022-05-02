#ifndef __LED_H_
#define __LED_H_

//LED0:PB10
//LED1:PB11

#include "gd32f3x0.h"

#define LED0_PIN        GPIO_PIN_10
#define LED0_GPIO_PORT  GPIOB
#define LED0_GPIO_CLK   RCU_GPIOB

#define LED1_PIN        GPIO_PIN_11
#define LED1_GPIO_PORT  GPIOB
#define LED1_GPIO_CLK   RCU_GPIOB

#define LED0_ON    	gpio_bit_reset(LED0_GPIO_PORT,LED0_PIN)
#define LED0_OFF    gpio_bit_set(LED0_GPIO_PORT,LED0_PIN)
#define LED1_ON     gpio_bit_reset(LED1_GPIO_PORT,LED1_PIN)
#define LED1_OFF    gpio_bit_set(LED1_GPIO_PORT,LED1_PIN)
#define LED0_TOGGLE gpio_bit_toggle(LED0_GPIO_PORT,LED0_PIN)
#define LED1_TOGGLE gpio_bit_toggle(LED1_GPIO_PORT,LED1_PIN)


void Led_Init(void);

#endif

