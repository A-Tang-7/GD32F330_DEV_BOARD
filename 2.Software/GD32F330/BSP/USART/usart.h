#ifndef __USART_H_
#define __USART_H_

#include "gd32f3x0.h"
#include <stdio.h>
#include <stdarg.h>	 //可变长参数
#include <string.h>

#define USART0_TX_PIN       GPIO_PIN_9
#define USART0_RX_PIN       GPIO_PIN_10
#define USART0_GPIO_PORT    GPIOA
#define USART0_GPIO_CLK     RCU_GPIOA
#define USART0_AF           GPIO_AF_1

//由于GD32F330的PA2、PA3口不支持5v容忍
//所以无法使用手头5v供电的串口转ttl模块
//将USART1的引脚改为PA14、PA15才能使用
//但是PA14为SWCLK，需要SWD下载时不能使用，只能将TX改为PA8
//可是PA8与PA15不能组成一对串口来使用
//所以USART1需要和5V设备通讯的话无法直接使用
//如果一定要使用不支持5v容忍的IO和5v设备串口通信的话，需要硬件电平转换
#define USART1_TX_PIN       GPIO_PIN_8
#define USART1_RX_PIN       GPIO_PIN_15
#define USART1_GPIO_PORT    GPIOA
#define USART1_GPIO_CLK     RCU_GPIOA
#define USART1_AF           GPIO_AF_1

#define USART1_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		400					//最大发送缓存字节数

void Usart0_Init(const uint32_t baudrate);
void Usart1_Init(const uint32_t baudrate);
void u1_printf(char* fmt,...);

#endif


