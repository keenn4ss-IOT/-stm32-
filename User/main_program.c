#include "main_program.h"
#include <stdio.h>
#include "time_handle.h"
#include "lcd.h"
#include "lcd_init.h"
#include "dht11.h"
#include "esp8266.h"
#include "led.h"
#include "a5516.h"
#include "adc.h"
#include "mq2.h"
#include "math.h"
#include "sg90.h"
#include "sy01.h"
#include "buzzer.h"
#include "button.h"

#include "gpio.h"
#include "button.h"



/*
*************************************
�궨��
*************************************
*/

/*
*************************************
��������
*************************************
*/
extern uint16_t index_oled;
extern uint16_t index_dht11;
extern uint8_t index_send_msg;
extern uint8_t index_led;
extern uint8_t index_sg90;

extern uint8_t button_enter;
extern uint8_t mode2_state;

/*
*************************************
��������
*************************************
*/

/*
*************************************
��������
*************************************
*/
uint8_t led_status =0;
uint8_t sg90_status =0;
uint8_t temp_value =0;
uint8_t humi_value =0;
uint16_t lux_value =0;
uint16_t ppm_value =0;
uint16_t sy01_value =0;


uint8_t lcdclear_sign_auto = 1;	//������־λ auto
uint8_t lcdclear_sign_manual = 1;	//������־λ manual
uint8_t lcdclear_sign_ud = 1; 		//������־λ threshole_value

//uint8_t oledclear_sign2 = 1;    //������־λ auto
//uint8_t oledclear_sign1 = 1; 		//������־λ manual
//uint8_t oledclear_sign3 = 1; 		//������־λ threshole_value

/*
*************************************
��������
*************************************
*/

/**
  * @brief          ��ʼ������,�൱��Arduino��setup()����,ֻ�ڳ�ʼ��ʱ��ִ��һ��
  * @param[in]      none
  * @retval         none
  */
void user_init_program(void)
{
	LCD_Init();//LCD��ʼ��
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	a5516_init();
	mq2_init ();
	sg90_init();
	sy01_init();
  HAL_ADCEx_Calibration_Start(&hadc1);
	dht11_init ();
	esp8266_init();
//  while(dht11_init())
//  {
//    HAL_Delay(500);
//    printf("dht11 init faild\r\n");
//		LCD_ShowString(8,40,"dht11 init faild",BLUE,WHITE,12,0);
//  }

	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
  LCD_ShowString(35,90,"START",BLACK,WHITE,24,0);
	HAL_Delay(500);
  time_slot_start();
}
/**
  * @brief          ��ʼ������,�൱��Arduino��loop()����,һֱִ�иú���
  * @param[in]      none
  * @retval         none
  */
//-----------------------------------------------------------------------------------------------------------------------------Mode_manual
void main_manual(void)
{
	LCD_clearrefresh(&lcdclear_sign_manual);           //��Ļˢ�±�־λ
  if(esp8266_receive_msg()!=1)
  {
			if(index_led==1)
		{
			set_led(led_status);
			index_led=0;
		}
			if(index_sg90==1)
		{
			set_sg90(sg90_status);
			index_sg90=0;
		}
  }
  if(index_dht11 ==1)
  {
    dht11_read_data(&temp_value,&humi_value);
    index_dht11=0;
  }
	
	//��ȡADC
	{
			lux_value = adc_read();			//5516���ߴ�������ֵ
			ppm_value = adc_read();	  	//mq2���崫������ֵ
			sy01_value = adc_read();			//sy01������������ֵ
	}
	

	lcdclear_sign_auto = 1;  //�ָ�������־λ     
	
	LCD_ShowString(8,5,"temp : ",BLUE,WHITE,12,0);
	LCD_ShowString(8,20,"humi : ",BLUE,WHITE,12,0);
	LCD_ShowString(8,35,"lux : ",BLUE,WHITE,12,0);
	LCD_ShowString(8,50,"ppm : ",BLUE,WHITE,12,0);
	LCD_ShowString(8,65,"db : ",BLUE,WHITE,12,0);
	LCD_ShowString(8,95,"mode : manual",BLUE,WHITE,16,0);
	
	LCD_ShowIntNum(60,5,temp_value,3,RED,WHITE,12);//��ʾ����
	LCD_ShowIntNum(60,20,humi_value,3,RED,WHITE,12);//��ʾ����
	LCD_ShowIntNum(60,35,lux_value,4,RED,WHITE,12);//��ʾ����
	LCD_ShowIntNum(60,50,ppm_value,4,RED,WHITE,12);//��ʾ����
	LCD_ShowIntNum(60,65,sy01_value,4,RED,WHITE,12);//��ʾ����
	
	
  if(index_send_msg==1)
	{
		index_send_msg =0;
		if(esp8266_send_msg()==0)
		{
		}
		else
		{
			printf("msg send error\r\n");
		}
	}
	lcdclear_sign_auto = 1;		//�ָ�������־λ
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------mode_auto
void main_auto(void)
{
	if(button_enter %7 != 0)
	{
		threshold_value_set();
	}

	else
	{
		LCD_clearrefresh(&lcdclear_sign_auto);           //��Ļˢ�±�־λ
		dht11_read_data(&temp_value,&humi_value);
		{
			lux_value = adc_read();			//5516���ߴ�������ֵ
			ppm_value = adc_read();	  	//mq2���崫������ֵ
			sy01_value = adc_read();			//sy01������������ֵ
		}
	
if(index_oled==1)
	{
		LCD_ShowString(8,5,"temp : ",BLUE,WHITE,12,0);
		LCD_ShowString(8,20,"humi : ",BLUE,WHITE,12,0);
		LCD_ShowString(8,35,"lux : ",BLUE,WHITE,12,0);
		LCD_ShowString(8,50,"ppm : ",BLUE,WHITE,12,0);
		LCD_ShowString(8,65,"db : ",BLUE,WHITE,12,0);
		LCD_ShowString(8,95,"mode : auto",BLUE,WHITE,16,0);

		LCD_ShowIntNum(60,5,temp_value,3,RED,WHITE,12);
		LCD_ShowIntNum(60,20,humi_value,3,RED,WHITE,12);

		LCD_ShowIntNum(60,35,lux_value,4,RED,WHITE,12);
		lux_check();

		LCD_ShowIntNum(60,50,ppm_value,4,RED,WHITE,12);

		LCD_ShowIntNum(60,65,sy01_value,4,RED,WHITE,12);

		num_check(temp_value,humi_value);

	}
		
			lcdclear_sign_manual = 1;		//�ָ�������־λ
			lcdclear_sign_ud = 1;				//�ָ�������־λ
	}
	
	  if(index_send_msg==1)
	{
		index_send_msg =0;
		if(esp8266_send_msg()==0)
		{
		}
		else
		{
			printf("msg send error\r\n");
		}
	}
  
}











