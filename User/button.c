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

// ��¼��һ�ΰ����¼���ʱ��
static volatile uint32_t last_debounce_time[2] = {0}; // ��������������

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t current_time = HAL_GetTick();
    uint8_t pin_index = (GPIO_Pin == GPIO_PIN_1) ? 0 : 1; // ����ֻ����������

    // ����Ƿ��Ѿ�����ȥ����ʱ��
    if ((current_time - last_debounce_time[pin_index]) > DEBOUNCE_DELAY_MS)
    {
        last_debounce_time[pin_index] = current_time; // �������һ��ȥ��ʱ��

        if (GPIO_Pin == GPIO_PIN_1) // PA1
        {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
            {
                button_mode = 1; // ���ð������µı�־λ
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



uint16_t cheat_button(uint16_t *x)//��ֵ����+1
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


uint16_t cheat_button_10(uint16_t *x)//��ֵ����+10
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




