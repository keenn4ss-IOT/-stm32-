#include "dht11.h"
#include "lcd.h"
#include "led.h"
#include "buzzer.h"
#include "button.h"
#include "main_program.h"
#include "lcd_init.h"

uint16_t temp_max = 38;
uint16_t temp_min = 20;   
uint16_t humi_max = 45;
uint16_t humi_min = 10;
uint16_t sy01_max = 2000;
uint16_t ppm_max = 2000;




extern uint8_t lcdclear_sign_auto ; 		//清屏标志位 
extern uint8_t lcdclear_sign_manual ;	//清屏标志位 
extern uint8_t lcdclear_sign_ud ; 		//清屏标志位 

extern uint8_t button_enter;
extern uint8_t mode2_state;

extern uint16_t lux_value ;
extern uint16_t ppm_value ;
extern uint16_t sy01_value ;

/**
  * @brief          微妙延时函数
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         none
  */
static void delay_us(uint32_t delay)
{
	uint32_t i=0;  
	while(delay--)
	{
		i=10;
		while(i--) ;    
	}
}

/**
  * @brief          端口变为输出
  * @param[in]      none
  * @retval         none
  */
void dht11_io_out (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          端口变为输入
  * @param[in]      none
  * @retval         none
  */
void dht11_io_in (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          DHT11端口复位，发出起始信号（IO发送）
  * @param[in]      none
  * @retval         none
  */
void dht11_rst (void)
{
	dht11_io_out();
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_RESET);
	HAL_Delay(20); //拉低至少18ms
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_SET);
	delay_us(30); //主机拉高20~40us
}

/**
  * @brief          等待DHT11回应
  * @param[in]      none
  * @retval         返回1:未检测到DHT11，返回0:成功（IO接收）
  */
uint8_t dht11_check(void)
{ 
  uint8_t retry=0;
  dht11_io_in();//IO到输入状态
  while (HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11会拉低40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1; else retry=0;
  while (!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11拉低后会再次拉高40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1;
  return 0;
}

/**
  * @brief          从DHT11读取一个位
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         返回值：1/0
  */
uint8_t dht11_read_bit(void)
{ 
	uint8_t retry=0;
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//等待变为低电平
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//等待变高电平
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us	//用于判断高低电平，即数据1或0
    if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin))return 1; else return 0;
}

/**
  * @brief          从DHT11读取一个字节  
  * @param[in]      none
  * @retval         返回值：读到的数据
  */
uint8_t dht11_read_byte(void)
{
	uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++){
        dat<<=1;
        dat|=dht11_read_bit();
    }
    return dat;
}

/**
  * @brief          DHT11初始化
  * @param[in]      none
  * @retval         返回1:初始化失败 返回0:初始化成功
  */
uint8_t dht11_init (void)
{	
	dht11_rst();
  HAL_Delay(1000);
  dht11_rst();
	return dht11_check();
}

/**
  * @brief          读取一次数据//湿度值(十进制，范围:20%~90%) ，温度值(十进制，范围:0~50°)，返回值：0,正常;1,失败
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         none
  */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi)
{
  uint8_t buf[5];
  uint8_t i;
  dht11_rst();//DHT11端口复位，发出起始信号
  if(dht11_check()==0)
  { //等待DHT11回应
    for(i=0;i<5;i++)
    {//读取5位数据
      buf[i]=dht11_read_byte(); //读出数据
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
    {	//数据校验
      *humi = buf[0]; //将湿度值放入指针1
      *temp = buf[2]; //将温度值放入指针2
    }
  }
  else 
  {
    return 1;
  }
  return 0;
}





///**
//  * @brief          检查所有传感器是否超出阈值，并做出相应的响应
//  * @param[in]      temp_value: 当前温度值     humi_value: 当前湿度值			
//  * @retval         HAL_StatusTypeDef HAL_OK if check and response successful, otherwise error status
//  */

void num_check(uint8_t temp_value, uint8_t humi_value)
{
//    // 检查温度是否超出阈值
	if( temp_value > temp_max )
	{
		LCD_ShowString(8,40,"temp too high",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,temp_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
	else if( temp_value < temp_min )
	{
		LCD_ShowString(8,40,"temp too low",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,temp_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
//    // 检查湿度是否超出阈值
	if (humi_value > humi_max )
	{
		LCD_ShowString(8,40,"humi too high",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,humi_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
	else if( humi_value < humi_min )
	{
		LCD_ShowString(8,40,"humi too low",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,humi_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
//		// 检查噪声是否超出阈值
	if ( sy01_value > sy01_max )
	{
		LCD_ShowString(8,40,"noise too high",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,humi_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
//		// 检查可燃气体成分是否超出阈值
	if ( ppm_value > ppm_max )
	{
		LCD_ShowString(8,40,"ppm too high",RED,WHITE,16,0);
		LCD_ShowIntNum(8,80,ppm_value,2,RED,WHITE,16);
		buzzer_flash();
		
		main_auto();
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	}
}






///**
//  * @brief          显示要更改的数值阈值，并由按键设置      
//  */
void threshold_value_set(void)
{
	LCD_clearrefresh(&lcdclear_sign_ud);           //屏幕刷新标志位
		
//	if(button_enter %7 == 1)
//		{
//			OLED_printf (0,0,"temp max = %d        ",temp_max);
//			temp_max = cheat_button(&temp_max);
//		}
	if( button_enter %7 == 1 )
	{
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowString(8,40,"temp max =     ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,temp_max,2,BLUE,WHITE,16);
		temp_max = cheat_button(&temp_max);
	}

	if( button_enter %7 == 2 )
	{
		LCD_ShowString(8,40,"temp min =    ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,temp_min,2,BLUE,WHITE,16);
		temp_min = cheat_button(&temp_min);
	}

	if( button_enter %7 == 3 )
	{
		LCD_ShowString(8,40,"humi max =    ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,humi_max,2,BLUE,WHITE,16);
		humi_max = cheat_button(&humi_max);
	}	

	if( button_enter %7 == 4 )
	{
		LCD_ShowString(8,40,"humi min =    ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,humi_min,2,BLUE,WHITE,16);
		humi_min = cheat_button(&humi_min);
	}	
		
	if( button_enter %7 == 5 )
	{
		LCD_ShowString(8,40,"sy01 max =    ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,sy01_max,4,BLUE,WHITE,16);
		sy01_max = cheat_button_10(&sy01_max);
	}		
	
	if( button_enter %7 == 6 )
	{
		LCD_ShowString(8,40,"ppm max =    ",BLUE,WHITE,16,0);
		LCD_ShowIntNum(50,80,ppm_max,4,BLUE,WHITE,16);
		ppm_max = cheat_button_10(&ppm_max);
	}	
	lcdclear_sign_auto = 1;
}	






