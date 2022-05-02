#include "systick.h"

#include "adc_key.h"
#include "at24c02.h"
#include "led.h"
#include "usart.h"
#include "w25q64.h"
#include "exti.h"
#include "timer.h"
#include "pwm.h"
#include "dma.h"
#include "fmc.h"
#include "encoder.h"

void ADC_Key_Process(void);

const uint8_t TEXT_Buffer[]={"GD32 modify FLASH TEST"};
#define SIZE sizeof(TEXT_Buffer)	 	//数组长度

int encoder=0;
int main(void)
{
    uint8_t datatemp[SIZE];
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

	Led_Init();
	Usart0_Init(115200);
	ADC_Key_Init();
    FMC_Write(FMC_WRITE_START_ADDR,(uint16_t*)TEXT_Buffer,SIZE);
    TIMER2_CH0_CH1_Encoder_Init();
    // Exti_Init();
    // Timer1_IRQ_Init(2000);
    // Timer1_CH2_PWM_Init();
    // USART0_TX_DMA_Init();

	// AT24C02_Init();
	// W25Q64_Init(); 
	
    printf("helloooo\r\n");
    LED1_ON;
    LED0_OFF;
    FMC_Read(FMC_WRITE_START_ADDR,(uint16_t*)datatemp,SIZE);
    printf("%s\r\n",datatemp);
    while(1) 
	{
         LED0_TOGGLE;
         LED1_TOGGLE;
	     delay_1ms(500);
//        encoder=Get_TIMER2_Encoder();
//        if(encoder>0)   printf("clockwise\r\n");
//        else if(encoder<0)  printf("anti-clockwise\r\n");
//        else printf("noting happen\r\n");
        // delay_1ms(500);
    }
}

/*!
    \brief      ADC按键处理程序，主要用于测试
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ADC_Key_Process(void)
{
    uint8_t key_val=0;
    key_val = ADC_Key_Scan(1);
    if(key_val == ADC_KEY_UP_PRESS){
        printf("ADC_KEY_UP_PRESS\r\n");
    }else if(key_val == ADC_KEY_DOWN_PRESS){
        printf("ADC_KEY_DOWN_PRESS\r\n");
    }else if(key_val == ADC_KEY_LEFT_PRESS){
        printf("ADC_KEY_LEFT_PRESS\r\n");
    }else if(key_val == ADC_KEY_RIGHT_PRESS){
        printf("ADC_KEY_RIGHT_PRESS\r\n");
    }else if(key_val == ADC_KEY_CENTER_PRESS){
        printf("ADC_KEY_CENTER_PRESS\r\n");
    }
}

