#include "button.h"
#include "gpio.h"
#include "main.h"
#include "delay.h"
#include "led.h"



volatile uint8_t button_mode = 0 ;
volatile uint8_t currentmode = 0 ;

volatile uint8_t button_enter = 0 ;
volatile uint8_t mode2_state = 0 ;

//--------------------------------------------------------

#define DEBOUNCE_DELAY_MS 1000

// 记录上一次按键事件的时间
static volatile uint32_t last_debounce_time[2] = {0}; // 假设有两个按键

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t current_time = HAL_GetTick();
    uint8_t pin_index = (GPIO_Pin == GPIO_PIN_1) ? 0 : 1; // 假设只有两个按键

    // 检查是否已经过了去抖动时间
    if ((current_time - last_debounce_time[pin_index]) > DEBOUNCE_DELAY_MS)
    {
        last_debounce_time[pin_index] = current_time; // 更新最后一次去抖时间

        if (GPIO_Pin == GPIO_PIN_1) // PA1
        {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
            {
                button_mode = 1; // 设置按键按下的标志位
            }
        }
        else if (GPIO_Pin == GPIO_PIN_6) // PA6
        {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET)
            {
                button_enter++;
            }
        }
    }
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}


//--------------------------------------------------------



uint16_t cheat_button(uint16_t *x)//阈值调节+1
{
	if(HAL_GPIO_ReadPin (GPIOA,GPIO_PIN_4) == RESET)
	{
		
		(*x)++;
	}
	else if(HAL_GPIO_ReadPin (GPIOA,GPIO_PIN_5) == RESET)
	{
		
		(*x)--;
	}
	return *x;
}


uint16_t cheat_button_10(uint16_t *x)//阈值调节+10
{
	if(HAL_GPIO_ReadPin (GPIOA,GPIO_PIN_4) == RESET)
	{
		
		(*x)+=10;
	}
	else if(HAL_GPIO_ReadPin (GPIOA,GPIO_PIN_5) == RESET)
	{
		
		(*x)-=10;
	}
	return *x;
}




