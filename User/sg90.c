#include "sg90.h" 
#include "tim.h"
//���
void sg90_init()
{
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	
	
}

//void sg90_run()
//{
//			/*
//		t = 0.5ms������������-�����ת�� 0 ��
//		t = 1.0ms������������-�����ת�� 45��
//		t = 1.5ms������������-�����ת�� 90��
//		t = 2.0ms������������-�����ת�� 135��
//		t = 2.5ms������������-�����ת�� 180��
//		*/
//	
//	while(1){           //ת90�ȡ�ת180�ȡ���λ��ѭ��
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









