/**
****************************(C) COPYRIGHT 2023 图解编程****************************
* @file       led.c
* @brief      控制led灯相关函数
* @note       
* @history
*  Version    Date            Author
*  V1.0.0     05-15-2023      图解编程    
*
****************************(C) COPYRIGHT 2023 图解编程****************************
*/
 
#include "led.h"
#include "main.h"

/**
  * @brief          控制led1亮灭函数
  * @param[in]      status: 为1时led灯点亮,为0时led灯熄灭
  * @retval         none
  */
void set_led(uint8_t status)
{
  if(status==1)
  {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  }
}

void mode_light_on(void)
{
	HAL_GPIO_WritePin(mode_light_GPIO_Port, mode_light_Pin, GPIO_PIN_RESET);
}
void mode_light_off(void)
{
	HAL_GPIO_WritePin(mode_light_GPIO_Port, mode_light_Pin, GPIO_PIN_SET);
}

void led_flash(void)
{
	for(int i = 0; i < 6; i++)
	{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); 
        HAL_Delay (100);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_Delay (100);
	}
}

