#include "timer.h"

static void Timer_IRQ_Init(Timer_InitTypeDef timer);

/***********************BSP Start***********************/
void Timer_Init(Timer_InitTypeDef timer)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(timer.timer_clk);
    timer_deinit(timer.timer_periph);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER configuration */
    timer_initpara.prescaler         = timer.timer_psc;    //84MHzʱ�ӣ�����8399Ԥ��Ƶ��ÿ���1��Ϊ5us
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;  //alignedmode��counterdirectionһ��ʹ��
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;    //�����ñ�ʾ�������ϼ���ģʽ
    timer_initpara.period            = timer.timer_arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer.timer_periph, &timer_initpara);

    timer_enable(timer.timer_periph);
}

static void Timer_IRQ_Init(Timer_InitTypeDef timer)
{
    Timer_Init(timer);

    nvic_irq_enable(timer.timer_nvic_irq, 1, 1);
    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(timer.timer_periph, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(timer.timer_periph, TIMER_INT_UP);

    // timer_enable(timer.timer_periph);
}
/***********************BSP End***********************/


/***********************Middleware Start***********************/
/*!
    \brief      ��ʱ��1�жϳ�ʼ��
    \param[in]  time_us��������Ҫ��ʱ��ʱ�䣬��λΪus
                ��timer_psc����ΪTIMER1_PSC_1USʱ��timer_us������Ϊ65535-1
                ��timer_psc����ΪTIMER1_PSC_5USʱ��timer_us������Ϊ65535*5-1
    \param[out] none
    \retval     none
*/
void Timer1_IRQ_Init(uint32_t time_us)
{
    uint8_t timer_psc;  //Ԥ��Ƶ���ú�ÿ�������ʱ��
    Timer_InitTypeDef Timer_InitStructure={ .timer_nvic_irq = TIMER1_IRQ,
                                            .timer_clk      = TIMER1_CLK,
                                            .timer_periph   = TIMER1_PERIPH,
                                            .timer_psc      = TIMER1_PSC_5US};

    //����ʱʱ��xUSת����arrֵ
    timer_psc = (Timer_InitStructure.timer_psc+1) / 8400;
    time_us /= timer_psc;
    Timer_InitStructure.timer_arr = time_us;

    Timer_IRQ_Init(Timer_InitStructure);
}

/*!
    \brief      this function handles TIMER1 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
  */
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP)) {
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
        
        LED0_TOGGLE;
        LED1_TOGGLE;
    }
}


/***********************Middleware End***********************/

