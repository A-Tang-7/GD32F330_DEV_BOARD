#ifndef __USART_H_
#define __USART_H_

#include "gd32f3x0.h"
#include <stdio.h>
#include <stdarg.h>	 //�ɱ䳤����
#include <string.h>

#define USART0_TX_PIN       GPIO_PIN_9
#define USART0_RX_PIN       GPIO_PIN_10
#define USART0_GPIO_PORT    GPIOA
#define USART0_GPIO_CLK     RCU_GPIOA
#define USART0_AF           GPIO_AF_1

//����GD32F330��PA2��PA3�ڲ�֧��5v����
//�����޷�ʹ����ͷ5v����Ĵ���תttlģ��
//��USART1�����Ÿ�ΪPA14��PA15����ʹ��
//����PA14ΪSWCLK����ҪSWD����ʱ����ʹ�ã�ֻ�ܽ�TX��ΪPA8
//����PA8��PA15�������һ�Դ�����ʹ��
//����USART1��Ҫ��5V�豸ͨѶ�Ļ��޷�ֱ��ʹ��
//���һ��Ҫʹ�ò�֧��5v���̵�IO��5v�豸����ͨ�ŵĻ�����ҪӲ����ƽת��
#define USART1_TX_PIN       GPIO_PIN_8
#define USART1_RX_PIN       GPIO_PIN_15
#define USART1_GPIO_PORT    GPIOA
#define USART1_GPIO_CLK     RCU_GPIOA
#define USART1_AF           GPIO_AF_1

#define USART1_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		400					//����ͻ����ֽ���

void Usart0_Init(const uint32_t baudrate);
void Usart1_Init(const uint32_t baudrate);
void u1_printf(char* fmt,...);

#endif


