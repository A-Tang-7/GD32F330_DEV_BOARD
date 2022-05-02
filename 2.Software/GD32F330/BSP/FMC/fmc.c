/*!
    �˲���FMC����ο�����ԭ��STM32 FLASHģ��EEPROMʵ���޸�
    Shout Out to Alientek
*/
#include "fmc.h"

static uint16_t FMC_ReadHalfWord(uint32_t faddr);
static void FMC_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);

/*!
    \brief      ��ȡָ����ַ�İ���(16λ����)
    \param[in]  faddr: ����ַ(�˵�ַ����Ϊ2�ı���!!)
    \param[out] none
    \retval     ��Ӧ����
*/
static uint16_t FMC_ReadHalfWord(uint32_t faddr)
{
	return *(__IO uint16_t*)faddr; 
}

/*!
    \brief      ������д��
    \param[in]  WriteAddr:��ʼ��ַ
    \param[in]  pBuffer:����ָ��
    \param[in]  NumToWrite:����(16λ)��  
    \param[out] none
    \retval     none
*/
static void FMC_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{ 			 		 
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
        fmc_halfword_program(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 

/*!
    \brief      ��ָ����ַ��ʼ����ָ�����ȵ�����
    \param[in]  ReadAddr:��ʼ��ַ
    \param[in]  pBuffer:����ָ��
    \param[in]  NumToRead:����(16λ)��  
    \param[out] none
    \retval     none
*/
void FMC_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=FMC_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


/*!
    \brief      ��ָ����ַ��ʼд��ָ�����ȵ�����
    \param[in]  WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
    \param[in]  pBuffer:����ָ��
    \param[in]  NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���)
    \param[out] none
    \retval     none
*/
uint16_t FMC_BUF[FMC_SECTOR_SIZE/2];//�����2K�ֽ�
void FMC_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)	
{
	uint32_t secpos;	   //������ַ
	uint16_t secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	uint16_t secremain;     //������ʣ���ַ(16λ�ּ���)	   
 	uint16_t i;    
	uint32_t offaddr;       //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<FMC_BASE_ADDR||(WriteAddr>=(FMC_BASE_ADDR+1024*GD32_FLASH_SIZE)))return;//�Ƿ���ַ
	fmc_unlock();						//����
	offaddr=WriteAddr-FMC_BASE_ADDR;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/FMC_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%FMC_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=FMC_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		FMC_Read(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR,FMC_BUF,FMC_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(FMC_BUF[secoff+i]!=0XFFFF)   break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
            fmc_page_erase(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR);//�����������
			for(i=0;i<secremain;i++)//����
			{
				FMC_BUF[i+secoff]=pBuffer[i];	  
			}
			FMC_Write_NoCheck(secpos*FMC_SECTOR_SIZE+FMC_BASE_ADDR,FMC_BUF,FMC_SECTOR_SIZE/2);//д����������  
		}else FMC_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)   break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(FMC_SECTOR_SIZE/2))secremain=FMC_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	fmc_lock();   //����
}






