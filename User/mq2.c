#include "adc.h"
#include "buzzer.h"
#include "led.h"
//���崫����
extern uint16_t ppm_value;
//uint16_t ppm_max = 1000;

void mq2_init()
{

HAL_ADCEx_Calibration_Start(&hadc1);   //ADCУ׼
HAL_ADC_Start(&hadc1);                 //����ADCת��

}

//void ppm_check(uint8_t ppm_value)
//{
//	if(ppm_value > ppm_max)
//	{
//		buzzer_flash();
//	}
//	
//}

