/*!
    此部分FMC代码参考正点原子STM32 FLASH模拟EEPROM实验修改
    Shout Out to Alientek
*/
#include "fmc.h"

static uint16_t FMC_ReadHalfWord(uint32_t faddr);
static void FMC_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);

/*!
    \brief      读取指定地址的半字(16位数据)
    \param[in]  faddr: 读地址(此地址必须为2的倍数!!)
    \param[out] none
    \retval     对应数据
*/
static uint16_t FMC_ReadHalfWord(uint32_t faddr)
{
	return *(__IO uint16_t*)faddr; 
}

/*!
    \brief      不检查的写入
    \param[in]  WriteAddr:起始地址
    \param[in]  pBuffer:数据指针
    \param[in]  NumToWrite:半字(16位)数  
    \param[out] none
    \retval     none
*/
static void FMC_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
        fmc_halfword_program(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 

/*!
    \brief      从指定地址开始读出指定长度的数据
    \param[in]  ReadAddr:起始地址
    \param[in]  pBuffer:数据指针
    \param[in]  NumToRead:半字(16位)数  
    \param[out] none
    \retval     none
*/
void FMC_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=FMC_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


/*!
    \brief      从指定地址开始写入指定长度的数据
    \param[in]  WriteAddr:起始地址(此地址必须为2的倍数!!)
    \param[in]  pBuffer:数据指针
    \param[in]  NumToWrite:半字(16位)数(就是要写入的16位数据的个数)
    \param[out] none
    \retval     none
*/
uint16_t FMC_BUF[FMC_SECTOR_SIZE/2];//最多是2K字节
void FMC_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain;     //扇区内剩余地址(16位字计算)	   
 	uint16_t i;    
	uint32_t offaddr;       //去掉0X08000000后的地址
	if(WriteAddr<FMC_BASE_ADDR||(WriteAddr>=(FMC_BASE_ADDR+1024*GD32_FLASH_SIZE)))return;//非法地址
	fmc_unlock();						//解锁
	offaddr=WriteAddr-FMC_BASE_ADDR;		//实际偏移地址.
	secpos=offaddr/FMC_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%FMC_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=FMC_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		FMC_Read(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR,FMC_BUF,FMC_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(FMC_BUF[secoff+i]!=0XFFFF)   break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
            fmc_page_erase(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				FMC_BUF[i+secoff]=pBuffer[i];	  
			}
			FMC_Write_NoCheck(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR,FMC_BUF,FMC_SECTOR_SIZE/2);//写入整个扇区  
		}else FMC_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)   break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(FMC_SECTOR_SIZE/2))secremain=FMC_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	fmc_lock();   //上锁
}






