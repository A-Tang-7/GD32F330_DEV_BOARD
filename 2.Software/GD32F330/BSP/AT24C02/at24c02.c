#include "at24c02.h"

//PB7:IIC0_SDA
//PB6:IIC0_SCL

/*  */
static uint8_t AT24C02_Page_Write(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte);
/*  */
static uint8_t eeprom_wait_standby_state_timeout(void);

typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_CLEAR_ADDRESS_FLAG,
    I2C_TRANSMIT_DATA,
    I2C_STOP,
} i2c_process_enum;

void AT24C02_Init(void)
{
    rcu_periph_clock_enable(I2C_GPIO_CLK);
    rcu_periph_clock_enable(I2C_CLK);

    //GPIO Config
    gpio_af_set(I2C_GPIO_PORT, GPIO_AF_1, I2C_SCL_GPIO_PIN);
    gpio_af_set(I2C_GPIO_PORT, GPIO_AF_1, I2C_SDA_GPIO_PIN);
    gpio_mode_set(I2C_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_mode_set(I2C_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_GPIO_PIN);
    gpio_output_options_set(I2C_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);    

    //I2C Config
    i2c_clock_config(I2C_COM, I2C_SPEED, I2C_DTCY_2);//I2C_DTCY_2:快模式，最高400kHz
    i2c_mode_addr_config(I2C_COM, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_SLAVE_ADDRESS);
    i2c_enable(I2C_COM);
    i2c_ack_config(I2C_COM, I2C_ACK_ENABLE);    
}


/*!
    \brief      写超过一个字节
    \param[in]  p_buffer: 待写的数据
    \param[in]  write_address: 写入的地址
    \param[in]  number_of_byte: 写入的字节数
    \param[out] none
    \retval     none
*/
static uint8_t AT24C02_Page_Write(uint8_t *p_buffer, uint8_t write_address, uint8_t number_of_byte)
{
    uint8_t state = I2C_START;
    uint16_t timeout = 0;
    uint8_t i2c_timeout_flag = 0;

    while(!(i2c_timeout_flag))
    {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(I2C_COM, I2C_FLAG_I2CBSY) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                i2c_start_on_bus(I2C_COM);//发送起始信号
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy in WRITE!\n");
            }
            break;

        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((! i2c_flag_get(I2C_COM, I2C_FLAG_SBSEND)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                i2c_master_addressing(I2C_COM, I2C_SLAVE_ADDRESS, I2C_TRANSMITTER);//主机发送从机地址
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends start signal timeout in WRITE!\n");
            }
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((! i2c_flag_get(I2C_COM, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                i2c_flag_clear(I2C_COM, I2C_FLAG_ADDSEND);
                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master clears address flag timeout in WRITE!\n");
            }
            break;

        case I2C_TRANSMIT_DATA:
            /* wait until the transmit data buffer is empty */
            while((!i2c_flag_get(I2C_COM, I2C_FLAG_TBE)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                /* send the EEPROM's internal address to write to : only one byte address */
                i2c_data_transmit(I2C_COM, write_address);//写一个字节
                timeout = 0;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends EEPROM's internal address timeout in WRITE!\n");
            }

            /* wait until BTC bit is set */
            //BTC:字节发送结束
            while((!i2c_flag_get(I2C_COM, I2C_FLAG_BTC)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                timeout = 0;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends data timeout in WRITE!\n");
            }

            while(number_of_byte--)
            {
                i2c_data_transmit(I2C_COM, *p_buffer);
                p_buffer++;
                /* wait until BTC bit is set */
                while((!i2c_flag_get(I2C_COM, I2C_FLAG_BTC)) && (timeout < I2C_TIMEOUT)) {
                    timeout++;
                }
                if(timeout < I2C_TIMEOUT){
                    timeout = 0;
                }else{
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c master sends data timeout in WRITE!\n");
                }
            }

            timeout = 0;
            state = I2C_STOP;
            break;

        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(I2C_COM);
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(I2C_COM) & 0x0200) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT){
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in WRITE!\n");
            }
            break;

        default:
            state = I2C_START;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            printf("i2c master sends start signal in WRITE.\n");
            break;
        }
    }

    return I2C_END;
}

/*!
    \brief      等待eeprom
    \param[in]  none
    \param[out] none
    \retval     none
*/
static uint8_t eeprom_wait_standby_state_timeout(void)
{
    __IO uint32_t val = 0;
    uint8_t state = I2C_START;
    uint16_t timeout = 0;
    uint8_t i2c_timeout_flag = 0;

    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            /* i2c master sends start signal only when the bus is idle */
            while(i2c_flag_get(I2C_COM, I2C_FLAG_I2CBSY) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT) {
                i2c_start_on_bus(I2C_COM);
                timeout = 0;
                state = I2C_SEND_ADDRESS;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c bus is busy!\n");
            }
            break;

        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((!i2c_flag_get(I2C_COM, I2C_FLAG_SBSEND)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT) {
                i2c_master_addressing(I2C_COM, I2C_SLAVE_ADDRESS, I2C_TRANSMITTER);
                timeout = 0;
                state = I2C_CLEAR_ADDRESS_FLAG;
            }else{
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends start signal timeout in EEPROM standby!\n");
            }
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            while((!((i2c_flag_get(I2C_COM, I2C_FLAG_ADDSEND)) || (i2c_flag_get(I2C_COM, I2C_FLAG_AERR)))) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }
            if(timeout < I2C_TIMEOUT) {
                if(i2c_flag_get(I2C_COM, I2C_FLAG_ADDSEND)) {
                    i2c_flag_clear(I2C_COM, I2C_FLAG_ADDSEND);
                    timeout = 0;

                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(I2C_COM);
                    i2c_timeout_flag = I2C_OK;
                    /* exit the function */
                    return I2C_END;
                }else{
                    /* clear the bit of AE */
                    i2c_flag_clear(I2C_COM, I2C_FLAG_AERR);
                    timeout = 0;
                    state = I2C_STOP;
                }
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master clears address flag timeout in EEPROM standby!\n");
            }
            break;

        case I2C_STOP:
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(I2C_COM);

            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(I2C_COM) & 0x0200) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }

            if(timeout < I2C_TIMEOUT) {
                timeout = 0;
                state = I2C_START;
            } else {
                timeout = 0;
                state = I2C_START;
                printf("i2c master sends stop signal timeout in EEPROM standby!\n");
            }
            break;

        default:
            state = I2C_START;
            timeout = 0;
            printf("i2c master sends start signal end in EEPROM standby!.\n");
            break;
        }
    }

    return I2C_END;
}

/*!
    \brief      写数据到eeprom
    \param[in]  p_buffer: 待写的数据
    \param[in]  write_address: 写入的地址
    \param[in]  number_of_byte: 写入的字节数
    \param[out] none
    \retval     none
*/
void AT24C02_Write(uint8_t *p_buffer, uint8_t write_address, uint16_t number_of_byte)
{
    uint8_t number_of_page = 0;
    uint8_t number_of_single = 0;
    uint8_t address = 0;
    uint8_t count = 0;

    address = write_address % I2C_PAGE_SIZE;    //与页首地址的距离
    count = I2C_PAGE_SIZE - address;            //与页尾地址的距离
    number_of_page = number_of_byte / I2C_PAGE_SIZE;    //需要写的页数
    number_of_single = number_of_byte % I2C_PAGE_SIZE;  //不满一页的字节数

    //地址正好是页地址的开头
    if(0 == address) 
    {
        while(number_of_page--) {
            AT24C02_Page_Write(p_buffer, write_address, I2C_PAGE_SIZE);
            eeprom_wait_standby_state_timeout();
            write_address += I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }

        if(0 != number_of_single) {
            AT24C02_Page_Write(p_buffer, write_address, number_of_single);
            eeprom_wait_standby_state_timeout();
        }
    } else {
        /* if write_address is not I2C_PAGE_SIZE aligned */
        if(number_of_byte < count) {
            AT24C02_Page_Write(p_buffer, write_address, number_of_byte);
            eeprom_wait_standby_state_timeout();
        } else {
            number_of_byte -= count;
            number_of_page = number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;

            if(0 != count) {
                AT24C02_Page_Write(p_buffer, write_address, count);
                eeprom_wait_standby_state_timeout();
                write_address += count;
                p_buffer += count;
            }

            /* write page */
            while(number_of_page--) {
                AT24C02_Page_Write(p_buffer, write_address, I2C_PAGE_SIZE);
                eeprom_wait_standby_state_timeout();
                write_address += I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }

            /* write single */
            if(0 != number_of_single) {
                AT24C02_Page_Write(p_buffer, write_address, number_of_single);
                eeprom_wait_standby_state_timeout();
            }
        }
    }
}

/*!
    \brief      从eeprom读数据
    \param[in]  p_buffer: 读取缓存区
    \param[in]  read_address: 写入的地址
    \param[in]  number_of_byte: 写入的字节数
    \param[out] none
    \retval     none
*/
uint8_t AT24C02_Read(uint8_t *p_buffer, uint8_t read_address, uint16_t number_of_byte)
{
    uint8_t state = I2C_START;
    uint8_t read_cycle = 0;
    uint16_t timeout = 0;
    uint8_t i2c_timeout_flag = 0;

    while(!(i2c_timeout_flag)) {
        switch(state) {
        case I2C_START:
            if(RESET == read_cycle) {
                /* i2c master sends start signal only when the bus is idle */
                while(i2c_flag_get(I2C_COM, I2C_FLAG_I2CBSY) && (timeout < I2C_TIMEOUT)) {
                    timeout++;
                }

                if(timeout < I2C_TIMEOUT) {
                    /* whether to send ACK or not for the next byte */
                    if(2 == number_of_byte) {
                        i2c_ackpos_config(I2C_COM, I2C_ACKPOS_NEXT);
                    }
                } else {
                    // i2c_bus_reset();
                    timeout = 0;
                    state = I2C_START;
                    printf("i2c bus is busy in READ!\n");
                }
            }

            /* send the start signal */
            i2c_start_on_bus(I2C_COM);
            timeout = 0;
            state = I2C_SEND_ADDRESS;
            break;

        case I2C_SEND_ADDRESS:
            /* i2c master sends START signal successfully */
            while((!i2c_flag_get(I2C_COM, I2C_FLAG_SBSEND)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }

            if(timeout < I2C_TIMEOUT) {
                if(RESET == read_cycle) {
                    i2c_master_addressing(I2C_COM, I2C_SLAVE_ADDRESS, I2C_TRANSMITTER);
                    state = I2C_CLEAR_ADDRESS_FLAG;
                } else {
                    i2c_master_addressing(I2C_COM, I2C_SLAVE_ADDRESS, I2C_RECEIVER);
                    if(number_of_byte < 3) {
                        /* disable acknowledge */
                        i2c_ack_config(I2C_COM, I2C_ACK_DISABLE);
                    }
                    state = I2C_CLEAR_ADDRESS_FLAG;
                }
                timeout = 0;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master sends start signal timeout in READ!\n");
            }
            break;

        case I2C_CLEAR_ADDRESS_FLAG:
            /* address flag set means i2c slave sends ACK */
            while((!i2c_flag_get(I2C_COM, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }

            if(timeout < I2C_TIMEOUT) {
                i2c_flag_clear(I2C_COM, I2C_FLAG_ADDSEND);
                if((SET == read_cycle) && (1 == number_of_byte)) {
                    /* send a stop condition to I2C bus */
                    i2c_stop_on_bus(I2C_COM);
                }

                timeout = 0;
                state = I2C_TRANSMIT_DATA;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master clears address flag timeout in READ!\n");
            }
            break;

        case I2C_TRANSMIT_DATA:
            if(RESET == read_cycle) {
                /* wait until the transmit data buffer is empty */
                while((! i2c_flag_get(I2C_COM, I2C_FLAG_TBE)) && (timeout < I2C_TIMEOUT)) {
                    timeout++;
                }

                if(timeout < I2C_TIMEOUT) {
                    /* send the EEPROM's internal address to write to : only one byte address */
                    i2c_data_transmit(I2C_COM, read_address);
                    timeout = 0;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master wait data buffer is empty timeout in READ!\n");
                }

                /* wait until BTC bit is set */
                while((!i2c_flag_get(I2C_COM, I2C_FLAG_BTC)) && (timeout < I2C_TIMEOUT)) {
                    timeout++;
                }

                if(timeout < I2C_TIMEOUT) {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle++;
                } else {
                    timeout = 0;
                    state = I2C_START;
                    read_cycle = 0;
                    printf("i2c master sends EEPROM's internal address timeout in READ!\n");
                }
            } else {
                while(number_of_byte) {
                    timeout++;

                    if(3 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(I2C_COM, I2C_FLAG_BTC));
                        /* disable acknowledge */
                        i2c_ack_config(I2C_COM, I2C_ACK_DISABLE);
                    }

                    if(2 == number_of_byte) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(I2C_COM, I2C_FLAG_BTC));
                        /* send a stop condition to I2C bus */
                        i2c_stop_on_bus(I2C_COM);
                    }

                    /* wait until RBNE bit is set */
                    if(i2c_flag_get(I2C_COM, I2C_FLAG_RBNE)) {
                        /* read a byte from the EEPROM */
                        *p_buffer = i2c_data_receive(I2C_COM);

                        /* point to the next location where the byte read will be saved */
                        p_buffer++;

                        /* decrement the read bytes counter */
                        number_of_byte--;
                        timeout = 0;
                    }

                    if(timeout > I2C_TIMEOUT) {
                        timeout = 0;
                        state = I2C_START;
                        read_cycle = 0;
                        printf("i2c master sends data timeout in READ!\n");
                    }
                }

                timeout = 0;
                state = I2C_STOP;
            }
            break;

        case I2C_STOP:
            /* i2c master sends STOP signal successfully */
            while((I2C_CTL0(I2C_COM) & 0x0200) && (timeout < I2C_TIMEOUT)) {
                timeout++;
            }

            if(timeout < I2C_TIMEOUT) {
                timeout = 0;
                state = I2C_END;
                i2c_timeout_flag = I2C_OK;
            } else {
                timeout = 0;
                state = I2C_START;
                read_cycle = 0;
                printf("i2c master sends stop signal timeout in READ!\n");
            }
            break;

        default:
            state = I2C_START;
            read_cycle = 0;
            i2c_timeout_flag = I2C_OK;
            timeout = 0;
            printf("i2c master sends start signal in READ.\n");
            break;
        }
    }

    return I2C_END;
}





