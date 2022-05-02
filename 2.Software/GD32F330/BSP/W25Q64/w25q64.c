#include "w25q64.h"

/* SPI0写一个字节，并读从机返回内容 */
static uint8_t SPI0_ReadWriteByte(uint8_t TxData);
/* 读取W25QXX的状态寄存器 */
static uint8_t W25QXX_ReadSR(void);
/* W25QXX写使能 */
static void W25QXX_Write_Enable(void);
/* SPI在一页(0~65535)内写入少于256个字节的数据 */
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
/* 无检验写SPI FLASH */
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
/* 擦除一个扇区 */
static void W25QXX_Erase_Sector(uint32_t Dst_Addr);
/* 等待空闲 */
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
    \brief      SPI0写一个字节，并读从机返回内容
    \param[in]  TxData：要写入的字节
    \param[out] none
    \retval     从机返回字节
*/
static uint8_t SPI0_ReadWriteByte(uint8_t TxData)
{		
    while (spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET){}//等待发送区空  

    spi_i2s_data_transmit(SPI0, TxData); //通过外设SPIx发送一个byte数据

    while (spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET){} //等待接收完一个byte  

    return spi_i2s_data_receive(SPI0); //返回通过SPIx最近接收的数据	  
}

/*!
    \brief      读取W25QXX的状态寄存器
                BIT7  6   5   4   3   2   1   0
                SPR   RV  TB BP2 BP1 BP0 WEL BUSY
                SPR:默认0,状态寄存器保护位,配合WP使用
                TB,BP2,BP1,BP0:FLASH区域写保护设置
                WEL:写使能锁定
                BUSY:忙标记位(1,忙;0,空闲)
                默认:0x00
    \param[in]  none
    \param[out] none
    \retval     从机返回字节
*/
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	FLASH_CS_ENABLE;                            //使能器件   
	SPI0_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPI0_ReadWriteByte(0Xff);             //读取一个字节  
	FLASH_CS_DISABLE;                            //取消片选     
	return byte;   
} 

/*!
    \brief      写W25QXX状态寄存器
                只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
    \param[in]  none
    \param[out] none
    \retval     从机返回字节
*/
void W25QXX_Write_SR(uint8_t sr)   
{   
	FLASH_CS_ENABLE;                            //使能器件   
	SPI0_ReadWriteByte(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	SPI0_ReadWriteByte(sr);               //写入一个字节  
	FLASH_CS_DISABLE;                            //取消片选     	      
}   


/*!
    \brief      W25QXX写使能
                将WEL置位
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void W25QXX_Write_Enable(void)   
{
	FLASH_CS_ENABLE;                            //使能器件   
    SPI0_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
	FLASH_CS_DISABLE;                            //取消片选     	      
} 
  
// /*!
//     \brief      W25QXX写禁止
//                 将WEL清零
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */
// static void W25QXX_Write_Disable(void)   
// {  
// 	FLASH_CS_ENABLE;                            //使能器件   
//     SPI0_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
// 	FLASH_CS_DISABLE;                            //取消片选     	      
// } 	

/*!
    \brief      读取芯片ID
    \param[in]  none
    \param[out] none
    \retval     0XEF13,表示芯片型号为W25Q80  
                0XEF14,表示芯片型号为W25Q16    
                0XEF15,表示芯片型号为W25Q32  
                0XEF16,表示芯片型号为W25Q64 
                0XEF17,表示芯片型号为W25Q128
*/ 
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	FLASH_CS_ENABLE;				    
	SPI0_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI0_ReadWriteByte(0x00); 	    
	SPI0_ReadWriteByte(0x00); 	    
	SPI0_ReadWriteByte(0x00); 	 			   
	Temp|=SPI0_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI0_ReadWriteByte(0xFF);	 
	FLASH_CS_DISABLE;				    
	return Temp;
}   		    

/*!
    \brief      读取SPI FLASH，在指定地址开始读取指定长度的数据  
    \param[in]  pBuffer:数据存储区
    \param[in]  ReadAddr:开始读取的地址(24bit)
    \param[in]  NumByteToRead:要读取的字节数(最大65535)
    \param[out] none
    \retval     none
*/ 
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	FLASH_CS_ENABLE;                            //使能器件   
    SPI0_ReadWriteByte(W25X_ReadData);         //发送读取命令   
    SPI0_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
    SPI0_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI0_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI0_ReadWriteByte(0XFF);   //循环读数  
    }
	FLASH_CS_DISABLE;  				    	      
}  

/*!
    \brief      SPI在一页(0~65535)内写入少于256个字节的数据
                在指定地址开始写入最大256字节的数据
    \param[in]  pBuffer:数据存储区
                WriteAddr:开始写入的地址(24bit)
                NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
	FLASH_CS_ENABLE;                            //使能器件   
    SPI0_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
    SPI0_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    SPI0_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI0_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI0_ReadWriteByte(pBuffer[i]);//循环写数  
	FLASH_CS_DISABLE;                            //取消片选 
	W25QXX_Wait_Busy();					   //等待写入结束
} 

/*!
    \brief      无检验写SPI FLASH
                必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
                具有自动换页功能 
                在指定地址开始写入指定长度的数据,但是要确保地址不越界!
    \param[in]  pBuffer:数据存储区
                WriteAddr:开始写入的地址(24bit)
                NumByteToWrite:要写入的字节数(最大65535)
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

/*!
    \brief      写SPI FLASH 
                在指定地址开始写入指定长度的数据
                该函数带擦除操作!
    \param[in]  pBuffer:数据存储区
                WriteAddr:开始写入的地址(24bit)						
                NumByteToWrite:要写入的字节数(最大65535)  
    \param[out] none
    \retval     none
*/ 
uint8_t W25QXX_BUFFER[4096];//这句定义不能放在函数内	 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	
    

   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}

// /*!
//     \brief      擦除整个芯片
//                 等待时间超长
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_Erase_Chip(void)   
// {                                   
//     W25QXX_Write_Enable();                  //SET WEL 
//     W25QXX_Wait_Busy();   
//   	FLASH_CS_ENABLE;                            //使能器件   
//     SPI0_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
// 	FLASH_CS_DISABLE;                            //取消片选     	      
// 	W25QXX_Wait_Busy();   				   //等待芯片擦除结束
// }   

/*!
    \brief      擦除一个扇区
                擦除一个山区的最少时间:150ms
    \param[in]  Dst_Addr:扇区地址 根据实际容量设置
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();   
  	FLASH_CS_ENABLE;                            //使能器件   
    SPI0_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
    SPI0_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    SPI0_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI0_ReadWriteByte((uint8_t)Dst_Addr);  
	FLASH_CS_DISABLE;                            //取消片选     	      
    W25QXX_Wait_Busy();   				   //等待擦除完成
}  

/*!
    \brief      等待空闲
    \param[in]  none
    \param[out] none
    \retval     none
*/ 
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  

// /*!
//     \brief      进入掉电模式
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_PowerDown(void)   
// { 
//   	FLASH_CS_ENABLE;                            //使能器件   
//     SPI0_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
// 	FLASH_CS_DISABLE;                            //取消片选     	      
//     delay_1ms(1);                               //等待TPD  
// }   

// /*!
//     \brief      唤醒
//     \param[in]  none
//     \param[out] none
//     \retval     none
// */ 
// static void W25QXX_WAKEUP(void)   
// {  
//   	FLASH_CS_ENABLE;                            //使能器件   
//     SPI0_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
// 	FLASH_CS_DISABLE;                            //取消片选     	      
//     delay_1ms(1);                               //等待TRES1
// }   








