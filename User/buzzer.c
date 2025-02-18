#include "buzzer.h"
#include "gpio.h"
#include "main.h"

void buzzer_on(void)
	{
// 开启蜂鸣器
HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);
}


void buzzer_off(void)
	{
// 关闭蜂鸣器
HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
}


//		直接复制到主函数
//		HAL_GPIO_TogglePin(buzzer_GPIO_Port, buzzer_Pin);          //翻转buzzer引脚电平		
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, HAL_GPIO_ReadPin (GPIOC, GPIO_PIN_14) == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET); 	//翻转buzzer引脚电平	
//		buzzer_off();
//		HAL_Delay(200);
//		buzzer_on ();
//		HAL_Delay(200);

void buzzer_flash(void)
{
	for(int i = 0; i < 6; i++)
	{
		HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET); 
		HAL_Delay (100);

		HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
		HAL_Delay (100);
	}
}

void buzzer_togglePin(void)
{
	HAL_GPIO_TogglePin(buzzer_GPIO_Port, buzzer_Pin);
}	

