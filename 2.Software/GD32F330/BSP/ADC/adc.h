#ifndef __ADC_H_
#define __ADC_H_

#include "gd32f3x0.h"
#include  "systick.h"

#define ADC_GPIO_CLK    RCU_GPIOA
#define ADC_GPIO_PORT   GPIOA
#define ADC_GPIO_PIN    GPIO_PIN_1
#define ADC_CHANNEL     ADC_CHANNEL_1

void ADC_Init(void);
uint16_t Get_ADC_Average(const uint8_t times);
#endif

