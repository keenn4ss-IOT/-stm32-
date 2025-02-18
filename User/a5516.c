#include "adc.h"
//���ߴ�����
uint16_t adc_value[3];
extern uint16_t lux_value;
uint16_t lux_min = 700;

void a5516_init()
{

HAL_ADCEx_Calibration_Start(&hadc1);   //ADCУ׼
HAL_ADC_Start(&hadc1);                 //����ADCת��
HAL_ADC_PollForConversion(&hadc1, 10);
}


uint16_t adc_read()
{
	int i;
			for(i=0;i<3;i++)
		{
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 10);
			adc_value [i] = HAL_ADC_GetValue(&hadc1);
			return adc_value[i];
		}
}

void lux_check(void)
{
	if(lux_value > lux_min)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
}
