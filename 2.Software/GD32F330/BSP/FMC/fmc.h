#ifndef __FMC_H_
#define __FMC_H_

#include "gd32f3x0.h"

#define FMC_WRITE_START_ADDR    ((uint32_t)0x0800D000U) //第52k位置

#define GD32_FLASH_SIZE 64
#define FMC_SECTOR_SIZE 1024     //中小内存版本芯片，每个page的大小是1k字节
#define FMC_BASE_ADDR   0x08000000

void FMC_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);
void FMC_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);	

#endif

