#include "sg90.h" 
#include "tim.h"
//舵机
void sg90_init()
{
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	
	
}

//void sg90_run()
//{
//			/*
//		t = 0.5ms——————-舵机会转到 0 °
//		t = 1.0ms——————-舵机会转到 45°
//		t = 1.5ms——————-舵机会转到 90°
//		t = 2.0ms——————-舵机会转到 135°
//		t = 2.5ms——————-舵机会转到 180°
//		*/
//	
//	while(1){           //转90度、转180度、复位，循环
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);//0
//		HAL_Delay(1000);
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,150);
//		HAL_Delay(1000);
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);//180
//		HAL_Delay(1000);
//	
//	
//					}	
//}

void set_sg90(uint8_t status_sg90)
{
	  if( status_sg90 == 1 )
  {
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,150);
  }
  else
  {
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);
  }

}









