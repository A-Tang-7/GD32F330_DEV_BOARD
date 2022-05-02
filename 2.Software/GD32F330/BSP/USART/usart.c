#include "usart.h"

typedef struct
{
    uint32_t 		com;
    rcu_periph_enum	usart_com_clk;
    uint32_t        usart_tx_pin;
    uint32_t        usart_rx_pin;
    uint32_t        usart_gpio_port;
    rcu_periph_enum usart_gpio_clk;
    uint32_t        usart_af;
    uint32_t        baudrate;
}Usart_InitTypeDef;

static void Usart_Init(const Usart_InitTypeDef usart);

//重定义系统输出函数为usart0
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

/**********加入如下代码可以不用勾选Use MicroLIB**********/
struct __FILE 
{ 
	int handle; 

}; 
FILE __stdout;  

// int fputc(int ch, FILE *f)
// {
//     while((USART_STAT(USART0) & USART_STAT_TC) == 0);
//     USART_TDATA(USART0) = (uint8_t)ch;
//     return ch;
// }
/******************************************************/

static void Usart_Init(const Usart_InitTypeDef usart)
{
    rcu_periph_clock_enable(usart.usart_gpio_clk);
    rcu_periph_clock_enable(usart.usart_com_clk);

    gpio_af_set(usart.usart_gpio_port, usart.usart_af, usart.usart_tx_pin);
    gpio_af_set(usart.usart_gpio_port, usart.usart_af, usart.usart_rx_pin);

    gpio_mode_set(usart.usart_gpio_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, usart.usart_tx_pin);
    gpio_output_options_set(usart.usart_gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, usart.usart_tx_pin);
    gpio_mode_set(usart.usart_gpio_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, usart.usart_rx_pin);
    gpio_output_options_set(usart.usart_gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, usart.usart_rx_pin);

    usart_deinit(usart.com);
    usart_baudrate_set(usart.com, usart.baudrate);
    usart_receive_config(usart.com, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart.com, USART_TRANSMIT_ENABLE);

    usart_enable(usart.com);
}

void Usart0_Init(const uint32_t baudrate)
{
    Usart_InitTypeDef Usart_InitStructure = { .com             = USART0,
                                              .usart_com_clk   = RCU_USART0,
                                              .usart_tx_pin    = USART0_TX_PIN,
                                              .usart_rx_pin    = USART0_RX_PIN,
                                              .usart_gpio_port = USART0_GPIO_PORT,
                                              .usart_gpio_clk  = USART0_GPIO_CLK,
                                              .usart_af        = USART0_AF,
                                              .baudrate        = baudrate };
                                              
    Usart_Init(Usart_InitStructure);
}

void Usart1_Init(const uint32_t baudrate)
{
    Usart_InitTypeDef Usart_InitStructure = { .com             = USART1,
                                              .usart_com_clk   = RCU_USART1,
                                              .usart_tx_pin    = USART1_TX_PIN,
                                              .usart_rx_pin    = USART1_RX_PIN,
                                              .usart_gpio_port = USART1_GPIO_PORT,
                                              .usart_gpio_clk  = USART1_GPIO_CLK,
                                              .usart_af        = USART1_AF,
                                              .baudrate        = baudrate };

    Usart_Init(Usart_InitStructure);
}


__align(8) uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
uint16_t USART1_RX_STA=0; 
void u1_printf(char* fmt,...)  
{  
    printf("enter\r\n");
	uint16_t i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	    while(usart_flag_get(USART1, USART_FLAG_TC) == RESET);  //等待上次传输完成 
        usart_data_transmit(USART1, (uint8_t) USART1_TX_BUF[j]);//发送数据到串口
	}
}





