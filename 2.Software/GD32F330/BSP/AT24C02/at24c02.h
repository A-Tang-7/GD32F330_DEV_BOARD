#ifndef __24C02_H_
#define __24C02_H_

#include "gd32f3x0.h"
#include "usart.h"

#define I2C_COM             I2C0
#define I2C_CLK             RCU_I2C0
#define I2C_GPIO_CLK        RCU_GPIOB
#define I2C_GPIO_PORT       GPIOB
#define I2C_SCL_GPIO_PIN    GPIO_PIN_6
#define I2C_SDA_GPIO_PIN    GPIO_PIN_7  

#define I2C_SPEED           400000
#define I2C_SLAVE_ADDRESS   0xA0
#define I2C_PAGE_SIZE       8

#define I2C_TIMEOUT         (uint16_t)(5000)

#define I2C_OK                 1
#define I2C_FAIL               0
#define I2C_END                1

#define BUFFER_SIZE              256

void AT24C02_Init(void);
/* 写数据到eeprom */
void AT24C02_Write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte);
/* 从eeprom读数据 */
uint8_t AT24C02_Read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte);

#endif


