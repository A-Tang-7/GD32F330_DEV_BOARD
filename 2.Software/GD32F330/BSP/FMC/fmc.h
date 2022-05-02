#ifndef __FMC_H_
#define __FMC_H_

#include "gd32f3x0.h"

#define FMC_WRITE_START_ADDR    ((uint32_t)0x0800D000U) //��52kλ��

#define GD32_FLASH_SIZE 64
#define FMC_SECTOR_SIZE 1024     //��С�ڴ�汾оƬ��ÿ��page�Ĵ�С��1k�ֽ�
#define FMC_BASE_ADDR   0x08000000

void FMC_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);
void FMC_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);	

#endif

