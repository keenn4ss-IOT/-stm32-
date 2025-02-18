#include "esp8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "lcd_init.h"
#include "core_json.h"


#define WIFI_SSID        "gg8266"
#define WIFI_PASSWD      "88888888"

#define MQTT_CLIENT_ID   "a13OIm1ilhC.device1|securemode=2\\,signmethod=hmacsha256\\,timestamp=1730594444719|"  
#define MQTT_USER_NAME   "device1&a13OIm1ilhC"
#define MQTT_PASSWD      "aa02b05e8718b189307f1e67610b9d5a0b2e18746d21034f93c09b54c4911871"
#define BROKER_ASDDRESS  "a13OIm1ilhC.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC        "/sys/a13OIm1ilhC/device1/thing/service/property/set"	
#define PUB_TOPIC        "/sys/a13OIm1ilhC/device1/thing/event/property/post"
//#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"	
#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\,\\\"lux\\\":%d\\,\\\"ppm\\\":%d\\,\\\"sy01\\\":%d\\,}\\,\\\"version\\\":\\\"1.0.0\\\"}"



/*
*************************************
宏定义
*************************************
*/

unsigned char receive_buf[512];	  //串口2接收缓存数组
unsigned char receive_start = 0;	//串口2接收开始标志位
uint16_t receive_count = 0;	      //串口2接收数据计数器
uint16_t receive_finish = 0;	    //串口2接收结束标志位 

extern uint8_t led_status;
extern uint8_t sg90_status;
extern uint8_t temp_value;
extern uint8_t humi_value;
extern uint16_t lux_value;
extern uint16_t ppm_value;
extern uint16_t sy01_value;

/**
  * @brief          解析json数据
  * @param[in]      json_msg:json数据,json_len:json数据长度
  * @retval         返回0找到指定json数据，否则返回1
  */
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
    uint8_t retval = 1; // 默认失败
    JSONStatus_t result;
    
    // 验证JSON数据
    if (JSON_Validate((const char *)json_msg, json_len) != JSONSuccess)
    {
        return retval; // 如果验证失败，直接返回
    }

    // 定义查询路径
    const char *queries[] = {"params.led", "params.sg90"};
    char *values[2];
    size_t valueLengths[2];

    // 搜索LED和SG90的状态
    for (int i = 0; i < 2; i++)
    {
        result = JSON_Search((char *)json_msg, json_len, queries[i], strlen(queries[i]), &values[i], &valueLengths[i]);
        if (result != JSONSuccess)
        {
            // 如果搜索失败，继续下一个查询，但标记为失败
            continue;
        }

        // 临时终止字符串并打印
        char save = values[i][valueLengths[i]];
        values[i][valueLengths[i]] = '\0';
        printf("Found: %s %zu -> %s\n", queries[i], valueLengths[i], values[i]);
        values[i][valueLengths[i]] = save;

        // 更新状态
        if (strcmp(queries[i], "params.led") == 0)
        {
            led_status = (uint8_t)atoi(values[i]);
        }
        else if (strcmp(queries[i], "params.sg90") == 0)
        {
            sg90_status = (uint8_t)atoi(values[i]);
        }
    }

    // 如果至少有一个查询成功，返回成功
    if (values[0] != NULL || values[1] != NULL)
    {
        retval = 0;
    }

    return retval;

}


/**
  * @brief          串口2数据接收处理函数
  * @param[in]      none
  * @retval         none
  */
void uart2_receiver_handle(void)
{
  unsigned char receive_data = 0;   
  if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);//串口2接收1位数据
    receive_buf[receive_count++] = receive_data;
    receive_start = 1;	                              //串口2接收数据开始标志位置1
    receive_finish = 0;	                              //串口2接收数据完成标志位清0
 }
	
	

	
	

}
/**
  * @brief          串口2数据接收清0函数
  * @param[in]      len:清空的数据长度
  * @retval         none
  */
void uart2_receiver_clear(uint16_t len)	
{
	memset(receive_buf,0x00,len);
	receive_count = 0;
	receive_start = 0;
	receive_finish = 0;
}
/**
  * @brief          向esp8266发送命令函数，成功返回0，失败返回1
  * @param[in]      cmd:发送的命令,len:命令的长度,rec_data:期望接收数据
  * @retval         none
  */
uint8_t esp8266_send_cmd(unsigned char *cmd,unsigned char len,char *rec_data)	
{
  unsigned char retval =0;
  unsigned int count = 0;

  HAL_UART_Transmit(&huart2, cmd, len, 1000);	                                   
  while((receive_start == 0)&&(count<1000))
  {
    count++;
    HAL_Delay(1);
  }

  if(count >= 1000)	
  {
    retval = 1;	
  }
  else	
  {
    do
    {
      receive_finish++;
      HAL_Delay(1);
    }
    while(receive_finish < 500);
    retval = 2;
    if(strstr((const char*)receive_buf, rec_data))	
    {
      retval = 0;	
    }
  }
  uart2_receiver_clear(receive_count);
  return retval;
}
/**
  * @brief          esp8266配置wifi网络
  * @param[in]      none
  * @retval         网络配置成功返回0,否则返回1
  */
uint8_t esp8266_config_network(void)
{
	uint8_t retval =0;
	uint16_t count = 0;
	
	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n",strlen("AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n"), 1000);
	
	while((receive_start == 0)&&(count<1000))
	{
		count++;
		HAL_Delay(1);
	}
	
	if(count >= 1000)	
	{
		retval = 1;	
	}
	else
	{
		HAL_Delay(8000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;
    }
	}
  uart2_receiver_clear(receive_count);
	return retval;
}
/**
  * @brief          esp8266连接服务
  * @param[in]      none
  * @retval         连接成功返回0,否则返回1
  */
uint8_t esp8266_connect_server(void)
{
	uint8_t retval=0;
	uint16_t count = 0;

	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n",strlen("AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n"), 1000);	
	while((receive_start == 0)&&(count<1000))	
	{
		count++;
		HAL_Delay(1);
	}
	
	if(count >= 1000)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(5000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;	
    }
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          esp8266复位
  * @param[in]      none
  * @retval         返回0复位成功,返回1复位失败
  */
uint8_t esp8266_reset(void)
{
	uint8_t retval =0;
	uint16_t count = 0;
	
	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+RST\r\n",8, 1000);
	while((receive_start == 0)&&(count<2000))	
	{
		count++;
		HAL_Delay(1);
	}
	if(count >= 2000)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(5000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;	
    }
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          通过esp8266发送数据   通过MQTT协议发送设备状态数据到云端。
  * @param[in]      none
  * @retval         返回0发送数据成功,返回1发送数据失败
  */
uint8_t esp8266_send_msg(void)	
{
	uint8_t retval =0;	
  uint16_t count = 0;			
	static uint8_t error_count=0;
	unsigned char msg_buf[256];
  
	sprintf((char *)msg_buf,"AT+MQTTPUB=0,\""PUB_TOPIC"\",\""JSON_FORMAT"\",0,0\r\n",temp_value,humi_value,lux_value,ppm_value,sy01_value);
  //sprintf((char *)msg_buf,"AT+MQTTPUB=0,\""PUB_TOPIC"\",\""JSON_FORMAT"\",0,0\r\n",temp_value,humi_value,lux_value);
	HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
  HAL_UART_Transmit(&huart1, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
	while((receive_start == 0)&&(count<500))	
	{
		count++;
		HAL_Delay(1);
	}
	if(count >= 500)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(50);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
			error_count=0;
		}
		else 
		{
			error_count++;
			if(error_count==5)
			{
				error_count=0;
        printf("RECONNECT MQTT BROKER!!!\r\n");
				esp8266_init();
			}
		}
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          通过esp8266接收数据   接收来自MQTT代理服务器的控制消息，并解析这些消息。
  * @param[in]      none
  * @retval         返回0接收数据正常,返回1接收数据异常或无数据
  */
uint8_t esp8266_receive_msg(void)	
{
  uint8_t retval =0;
	int msg_len=0;
	uint8_t msg_body[128] = {0};
  
	if(receive_start == 1)	
	{
		do
    {
			receive_finish++;
			HAL_Delay(1);
		}
    while(receive_finish < 5);	
		
		if(strstr((const char*)receive_buf,"+MQTTSUBRECV:"))
		{
			sscanf((const char *)receive_buf,"+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s",&msg_len,msg_body);
      printf("len:%d,msg:%s\r\n",msg_len,msg_body);
			if(strlen((const char*)msg_body)== msg_len)
			{
        retval = parse_json_msg(msg_body,msg_len);
			}
      else
      {
        retval = 2;
      }
		}
    else 
    {
      retval = 2;
    }
	}
  else
  {
    retval = 2;
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          esp8266初始化
  * @param[in]      none
  * @retval         none
  */
void esp8266_init(void)
{
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);           											//打开串口2接收中断
	
	LCD_ShowString(0,5,"1.SETTING STATION MODE                  ",BLUE,WHITE,12,0);
	while(esp8266_send_cmd((uint8_t *)"AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	LCD_ShowString (0,5,"2.CLOSE ESP8266 ECHO                   ",BLUE,WHITE,12,0);
	while(esp8266_send_cmd((uint8_t *)"ATE0\r\n",strlen("ATE0\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	LCD_ShowString (0,5,"3.NO AUTO CONNECT WIFI                   ",BLUE,WHITE,12,0);
	while(esp8266_send_cmd((uint8_t *)"AT+CWAUTOCONN=0\r\n",strlen("AT+CWAUTOCONN=0\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	LCD_ShowString (0,5,"4.RESET ESP8266            ",BLUE,WHITE,12,0);
	while(esp8266_reset() != 0)
	{
		HAL_Delay(5000);
	}
	LCD_ShowString (0,5,"5.CONFIG WIFI NETWORK     ",BLUE,WHITE,12,0);
	while(esp8266_config_network() != 0)
	{
		HAL_Delay(7000);
	}
	LCD_ShowString (0,5,"6.MQTT USER CONFIG            ",BLUE,WHITE,12,0);
	while(esp8266_send_cmd((uint8_t *)"AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n",
                          strlen("AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	LCD_ShowString (0,5,"7.CONNECT MQTT BROKER             ",BLUE,WHITE,12,0);
	while(esp8266_connect_server() != 0)
	{
		HAL_Delay(8000);
	}
	LCD_ShowString (0,5,"8.SUBSCRIBE TOPIC                  ",BLUE,WHITE,12,0);
	while(esp8266_send_cmd((uint8_t *)"AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n",strlen("AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	LCD_ShowString (0,5,"9.ESP8266 INIT OK!!!                          ",BLUE,WHITE,12,0);
}





/*
串口通信设置
UART配置：
使用的是HAL_UART_Transmit和HAL_UART_Receive函数进行串口通信，这意味着STM32的UART（这里是UART2）被配置为与ESP8266模块通信。
接收处理：
uart2_receiver_handle函数处理来自ESP8266的串口数据。它检查UART2的接收中断标志位(UART_FLAG_RXNE)，如果有数据，则接收并存储到receive_buf数组中。


ESP8266模块控制
命令发送：
esp8266_send_cmd函数用于向ESP8266发送AT命令，并等待响应。它使用HAL_UART_Transmit发送命令，并通过检查receive_start标志位来判断是否收到响应。
网络配置：
esp8266_config_network函数通过发送AT+CWJAP命令配置ESP8266连接到指定的Wi-Fi网络。
MQTT连接：
esp8266_connect_server函数通过AT+MQTTCONN命令连接到MQTT代理服务器。
数据发送与接收：
esp8266_send_msg函数通过AT+MQTTPUB命令将数据发布到指定的MQTT主题。
esp8266_receive_msg函数处理从MQTT订阅的主题接收到的消息，解析JSON数据并更新设备状态。
*/

