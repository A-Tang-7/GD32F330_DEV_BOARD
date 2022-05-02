#include "adc_key.h"

//PA1:ADC_IN1

//上：0
//下：888左右
//左：472左右
//右：2055左右
//按下：3013左右
//空闲：4095

static bool IsKeyValInRange(const uint16_t key_val, const uint16_t target, const uint16_t offset);
static bool IsAnyKeyPress(const uint16_t key_val);

void ADC_Key_Init(void)
{
    ADC_Init();  
}

/*!
    \brief      ADC按键扫描
    \param[in]  mode: 连续触发模式
        \arg        0：不支持连续触发
                    1：支持连续触发
    \param[out] none
    \retval     ADC_KEY_UP_PRESS 
                or ADC_KEY_DOWN_PRESS 
                or ADC_KEY_LEFT_PRESS 
                or ADC_KEY_RIGHT_PRESS 
                or ADC_KEY_CENTER_PRESS
*/
uint8_t ADC_Key_Scan(uint8_t mode)
{
    static uint8_t key_release_flag=1;
    uint16_t key_val=0;

    if(mode)    key_release_flag=1;
	
    key_val = Get_ADC_Average(10);
	if(key_release_flag && (IsAnyKeyPress(key_val)))
	{
        delay_1ms(10);//去抖动 
		key_release_flag=0;
        if(IsKeyValInRange(key_val,ADC_KEY_UP_VAL,KEY_VAL_OFFSET))			return ADC_KEY_UP_PRESS;
        else if(IsKeyValInRange(key_val,ADC_KEY_DOWN_VAL,KEY_VAL_OFFSET))   return ADC_KEY_DOWN_PRESS;
        else if(IsKeyValInRange(key_val,ADC_KEY_LEFT_VAL,KEY_VAL_OFFSET))	return ADC_KEY_LEFT_PRESS;
        else if(IsKeyValInRange(key_val,ADC_KEY_RIGHT_VAL,KEY_VAL_OFFSET))	return ADC_KEY_RIGHT_PRESS;
        else if(IsKeyValInRange(key_val,ADC_KEY_CENTER_VAL,KEY_VAL_OFFSET))	return ADC_KEY_CENTER_PRESS;
        else    return 0;
    }
    else if(key_val>=KEY_RELEASE)    key_release_flag=1; 

	return 0;// 无按键按下
}

/*!
    \brief      判断数值是否在范围内
    \param[in]  key_val: 待检测数值
	\param[in]  target: 目标值
	\param[in]  offset: 目标正负偏差值
    \param[out] none
    \retval     true
				false
*/
static bool IsKeyValInRange(const uint16_t key_val, const uint16_t target, const uint16_t offset)
{
	if((key_val>=(target-offset)) && (key_val<=(target+offset)))	return true;
	else return false;
}

static bool IsAnyKeyPress(const uint16_t key_val)
{
	if(IsKeyValInRange(key_val,ADC_KEY_UP_VAL,KEY_VAL_OFFSET)
			|| IsKeyValInRange(key_val,ADC_KEY_DOWN_VAL,KEY_VAL_OFFSET)
			|| IsKeyValInRange(key_val,ADC_KEY_LEFT_VAL,KEY_VAL_OFFSET) 
			|| IsKeyValInRange(key_val,ADC_KEY_RIGHT_VAL,KEY_VAL_OFFSET)
			|| IsKeyValInRange(key_val,ADC_KEY_CENTER_VAL,KEY_VAL_OFFSET))return true;
	else return false;
}


