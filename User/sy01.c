#include "adc.h"
#include "led.h"
#include "buzzer.h"
#include "sg90.h"
#include "tim.h"
//����������

//uint16_t sy01_max = 400;


void sy01_init()
{

HAL_ADCEx_Calibration_Start(&hadc1);   //ADCУ׼
HAL_ADC_Start(&hadc1);                 //����ADCת��

}

//void sy01_check(uint8_t sy01_value)
//{
//	if(sy01_value > sy01_max)
//	{
//		buzzer_flash ();
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,150);
//	}
//	else if (sy01_value < sy01_max)
//	{
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);
//	}
//}


