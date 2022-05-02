#include "w25q64.h"

/* SPI0дһ���ֽڣ������ӻ��������� */
static uint8_t SPI0_ReadWriteByte(uint8_t TxData);
/* ��ȡW25QXX��״̬�Ĵ��� */
static uint8_t W25QXX_ReadSR(void);
/* W25QXXдʹ�� */
static void W25QXX_Write_Enable(void);
/* SPI��һҳ(0~65535)��д������256���ֽڵ����� */
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
/* �޼���дSPI FLASH */
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
/* ����һ������ */
static void W25QXX_Erase_Sector(uint32_t Dst_Addr);
/* �ȴ����� */
static void W25QXX_Wait_Busy(void);

void W25Q64_Init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(SPI0_GPIO_CLK);

    /* configure SPI0 GPIO:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_af_set(SPI0_GPIO_PORT, GPIO_AF_0, SPI0_SCK_PIN | SPI0_MISO_PIN | SPI0_MOSI_PIN);
    gpio_mode_set(SPI0_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_SCK_PIN | SPI0_MISO_PIN | SPI0_MOSI_PIN);
    gpio_output_options_set(SPI0_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI0_SCK_PIN | SPI0_MISO_PIN | SPI0_MOSI_PIN);

    /* configure Flash_CS pin */
    gpio_mode_set(SPI0_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, FLASH_CS_PIN);
    gpio_output_options_set(SPI0_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, FLASH_CS_PIN);
    GPIO_BOP(SPI0_GPIO_PORT) = FLASH_CS_PIN;

    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* configure SPI0 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    spi_i2s_deinit(SPI1);
    spi_init_struct.device_mode = SPI_SLAVE;
    spi_init_struct.nss         = SPI_NSS_SOFT;
    spi_init(SPI1, &spi_init_struct);
    spi_enable(SPI1);

    FLASH_CS_DISABLE;
    spi_enable(SPI0);
}

/*!
    \brief      SPI0дһ���ֽڣ������ӻ���������
    \param[in]  TxData��Ҫд����ֽ�
    \param[out] none
    \retval     �ӻ������ֽ�
*/
static uint8_t SPI0_ReadWriteByte(uint8_t TxData)
{		
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET){}//�ȴ���������  

    spi_i2s_data_transmit(SPI0, TxData); //ͨ������SPIx����һ��byte����

    while (spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET){} //�ȴ�������һ��byte  

    return spi_i2s_data_receive(SPI0); //����ͨ��SPIx������յ�����	  
}

/*!
    \brief      ��ȡW25QXX��״̬�Ĵ���
                BIT7  6   5   4   3   2   1   0
                SPR   RV  TB BP2 BP1 BP0 WEL BUSY
                SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
                TB,BP2,BP1,BP0:FLASH����д��������
                WEL:дʹ������
                BUSY:æ���λ(1,æ;0,����)
                Ĭ��:0x00
    \param[in]  none
    \param[out] none
    \retval     �ӻ������ֽ�
*/
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	FLASH_CS_ENABLE;                            //ʹ������   
	SPI0_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI0_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     
	return byte;   
} 

/*!
    \brief      дW25QXX״̬�Ĵ���
                ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д
    \param[in]  none
    \param[out] none
    \retval     �ӻ������ֽ�
*/
void W25QXX_Write_SR(uint8_t sr)   
{   
	FLASH_CS_ENABLE;                            //ʹ������   
	SPI0_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI0_ReadWriteByte(sr);               //д��һ���ֽ�  
	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
}   


/*!
    \brief      W25QXXдʹ��
                ��WEL��λ
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void W25QXX_Write_Enable(void)   
{
	FLASH_CS_ENABLE;                            //ʹ������   
    SPI0_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
} 
  
// /*!
//     \brief      W25QXXд��ֹ
//                 ��WEL����
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// static void W25QXX_Write_Disable(void)   
// {  
// 	FLASH_CS_ENABLE;                            //ʹ������   
//     SPI0_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
// 	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
// } 	

/*!
    \brief      ��ȡоƬID
    \param[in]  none
    \param[out] none
    \retval     0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
                0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
                0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
                0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
                0XEF17,��ʾоƬ�ͺ�ΪW25Q128
*/ 
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	FLASH_CS_ENABLE;				    
	SPI0_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI0_ReadWriteByte(0x00); 	    
	SPI0_ReadWriteByte(0x00); 	    
	SPI0_ReadWriteByte(0x00); 	 			   
	Temp|=SPI0_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI0_ReadWriteByte(0xFF);	 
	FLASH_CS_DISABLE;				    
	return Temp;
}   		    

/*!
    \brief      ��ȡSPI FLASH����ָ����ַ��ʼ��ȡָ�����ȵ�����  
    \param[in]  pBuffer:���ݴ洢��
    \param[in]  ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
    \param[in]  NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
    \param[out] none
    \retval     none
*/ 
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	FLASH_CS_ENABLE;                            //ʹ������   
    SPI0_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPI0_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
    SPI0_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI0_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI0_ReadWriteByte(0XFF);   //ѭ������  
    }
	FLASH_CS_DISABLE;  				    	      
}  

/*!
    \brief      SPI��һҳ(0~65535)��д������256���ֽڵ�����
                ��ָ����ַ��ʼд�����256�ֽڵ�����
    \param[in]  pBuffer:���ݴ洢��
                WriteAddr:��ʼд��ĵ�ַ(24bit)
                NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
	FLASH_CS_ENABLE;                            //ʹ������   
    SPI0_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPI0_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    SPI0_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI0_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI0_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 

/*!
    \brief      �޼���дSPI FLASH
                ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
                �����Զ���ҳ���� 
                ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
    \param[in]  pBuffer:���ݴ洢��
                WriteAddr:��ʼд��ĵ�ַ(24bit)
                NumByteToWrite:Ҫд����ֽ���(���65535)
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

/*!
    \brief      дSPI FLASH 
                ��ָ����ַ��ʼд��ָ�����ȵ�����
                �ú�������������!
    \param[in]  pBuffer:���ݴ洢��
                WriteAddr:��ʼд��ĵ�ַ(24bit)						
                NumByteToWrite:Ҫд����ֽ���(���65535)  
    \param[out] none
    \retval     none
*/ 
uint8_t W25QXX_BUFFER[4096];//��䶨�岻�ܷ��ں�����	 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	
    

   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}

// /*!
//     \brief      ��������оƬ
//                 �ȴ�ʱ�䳬��
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_Erase_Chip(void)   
// {                                   
//     W25QXX_Write_Enable();                  //SET WEL 
//     W25QXX_Wait_Busy();   
//   	FLASH_CS_ENABLE;                            //ʹ������   
//     SPI0_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
// 	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
// 	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
// }   

/*!
    \brief      ����һ������
                ����һ��ɽ��������ʱ��:150ms
    \param[in]  Dst_Addr:������ַ ����ʵ����������
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//����falsh�������,������   
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	FLASH_CS_ENABLE;                            //ʹ������   
    SPI0_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPI0_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI0_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI0_ReadWriteByte((uint8_t)Dst_Addr);  
	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				   //�ȴ��������
}  

/*!
    \brief      �ȴ�����
    \param[in]  none
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  

// /*!
//     \brief      �������ģʽ
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_PowerDown(void)   
// { 
//   	FLASH_CS_ENABLE;                            //ʹ������   
//     SPI0_ReadWriteByte(W25X_PowerDown);        //���͵�������  
// 	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
//     delay_1ms(1);                               //�ȴ�TPD  
// }   

// /*!
//     \brief      ����
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_WAKEUP(void)   
// {  
//   	FLASH_CS_ENABLE;                            //ʹ������   
//     SPI0_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
// 	FLASH_CS_DISABLE;                            //ȡ��Ƭѡ     	      
//     delay_1ms(1);                               //�ȴ�TRES1
// }   








