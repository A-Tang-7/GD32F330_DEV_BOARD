#ifndef __ADC_KEY_H_
#define __ADC_KEY_H_

#include "adc.h"
#include "gd32f3x0.h"
#include "stdbool.h"
#include "systick.h"
#include "usart.h"

#define ADC_KEY_UP_PRESS        1
#define ADC_KEY_DOWN_PRESS      2
#define ADC_KEY_LEFT_PRESS      3
#define ADC_KEY_RIGHT_PRESS     4
#define ADC_KEY_CENTER_PRESS    5

#define ADC_KEY_UP_VAL		0
#define ADC_KEY_DOWN_VAL	888
#define ADC_KEY_LEFT_VAL	472
#define ADC_KEY_RIGHT_VAL	2055
#define ADC_KEY_CENTER_VAL	3013

#define KEY_VAL_OFFSET		15

#define KEY_RELEASE			4095-KEY_VAL_OFFSET

void ADC_Key_Init(void);
uint8_t ADC_Key_Scan(uint8_t mode);

#endif


