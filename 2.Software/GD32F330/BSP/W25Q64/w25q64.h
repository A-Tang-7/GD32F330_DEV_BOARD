#ifndef __W25Q64_H_
#define __W25Q64_H_

#include "gd32f3x0.h"
#include "systick.h"

#define SPI0_GPIO_PORT  GPIOA
#define SPI0_GPIO_CLK   RCU_GPIOA
#define FLASH_CS_PIN    GPIO_PIN_4
#define SPI0_SCK_PIN    GPIO_PIN_5
#define SPI0_MISO_PIN   GPIO_PIN_6
#define SPI0_MOSI_PIN   GPIO_PIN_7

#define FLASH_CS_ENABLE     GPIO_BC(SPI0_GPIO_PORT) = FLASH_CS_PIN;
#define FLASH_CS_DISABLE    GPIO_BOP(SPI0_GPIO_PORT) = FLASH_CS_PIN;

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

#define W25Q80_ID 	0XEF13 	
#define W25Q16_ID 	0XEF14
#define W25Q32_ID 	0XEF15
#define W25Q64_ID 	0XEF16
#define W25Q128_ID 	0XEF17

void W25Q64_Init(void);
/* 读取芯片ID */
uint16_t W25QXX_ReadID(void);
/* 写SPI FLASH */
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
/* 读取SPI FLASH，在指定地址开始读取指定长度的数据 */
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);

#endif

