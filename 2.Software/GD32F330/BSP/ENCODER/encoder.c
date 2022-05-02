#include "encoder.h"

//PB4：encoder.timer_periph_CH0
//PB5：encoder.timer_periph_CH1

static void Encoder_Init(Encoder_InitTypeDef encoder);
static int Get_Encoder(uint32_t timer_periph);

static void Encoder_Init(Encoder_InitTypeDef encoder)
{
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    //初始化时钟
    rcu_periph_clock_enable(encoder.a_gipo_clk);
    rcu_periph_clock_enable(encoder.b_gpio_clk);
    rcu_periph_clock_enable(encoder.timer_clk);

    //初始化对应IO口
    gpio_mode_set(encoder.a_gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, encoder.a_gpio_pin);
    gpio_mode_set(encoder.b_gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, encoder.b_gpio_pin);
    gpio_output_options_set(encoder.a_gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, encoder.a_gpio_pin);
    gpio_output_options_set(encoder.b_gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, encoder.b_gpio_pin);
    gpio_af_set(encoder.a_gpio_port, encoder.a_gpio_af, encoder.a_gpio_pin);
    gpio_af_set(encoder.b_gpio_port, encoder.b_gpio_af, encoder.b_gpio_pin);

    //初始化定时器
    timer_deinit(encoder.timer_periph);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER configuration */
    timer_initpara.prescaler         = 0;   
    timer_initpara.period            = 65535; 
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;  //alignedmode和counterdirection一起使用
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;    //该设置表示边沿向上计数模式
    timer_init(encoder.timer_periph, &timer_initpara);

    timer_channel_input_struct_para_init(&timer_icinitpara);
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0;
    timer_input_capture_config(encoder.timer_periph, encoder.a_timer_channel, &timer_icinitpara);
    timer_input_capture_config(encoder.timer_periph, encoder.b_timer_channel, &timer_icinitpara);
    timer_counter_value_config(encoder.timer_periph,COUNTER_INIT);
    timer_quadrature_decoder_mode_config(encoder.timer_periph, TIMER_ENCODER_MODE2, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);

    timer_auto_reload_shadow_enable(encoder.timer_periph);
    timer_enable(encoder.timer_periph);
}

static int Get_Encoder(uint32_t timer_periph)
{
	int encoder_val;
	encoder_val = timer_counter_read(timer_periph)-COUNTER_INIT;			//获取当前编码器值
	delay_1ms(100);	    //采样周期，默认100ms
	encoder_val = timer_counter_read(timer_periph)-COUNTER_INIT-encoder_val;	//获取差值
    timer_counter_value_config(timer_periph,COUNTER_INIT);
	return encoder_val;
}


void TIMER2_CH0_CH1_Encoder_Init(void)
{
    Encoder_InitTypeDef Encoder_InitStructure={ .a_gipo_clk      = ENCODER_A_GPIO_CLK,
                                                .a_gpio_port     = ENCODER_A_GPIO_PORT,
                                                .a_gpio_pin      = ENCODER_A_GPIO_PIN,
                                                .a_gpio_af       = ENCODER_A_GPIO_AF,
                                                .b_gpio_clk      = ENCODER_B_GPIO_CLK,
                                                .b_gpio_port     = ENCODER_B_GPIO_PORT,
                                                .b_gpio_pin      = ENCODER_B_GPIO_PIN,
                                                .b_gpio_af       = ENCODER_B_GPIO_AF,
                                                .timer_periph    = ENCODER_TIMER_PERIPH,
                                                .timer_clk       = ENCODER_TIMER_CLK,
                                                .a_timer_channel = ENCODER_A_TIMER_CHANNEL,
                                                .b_timer_channel = ENCODER_B_TIMER_CHANNEL };
    Encoder_Init(Encoder_InitStructure);
}

int Get_TIMER2_Encoder(void)
{
    return Get_Encoder(ENCODER_TIMER_PERIPH);
}
















