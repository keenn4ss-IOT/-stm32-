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
�궨��
*************************************
*/

unsigned char receive_buf[512];	  //����2���ջ�������
unsigned char receive_start = 0;	//����2���տ�ʼ��־λ
uint16_t receive_count = 0;	      //����2�������ݼ�����
uint16_t receive_finish = 0;	    //����2���ս�����־λ 

extern uint8_t led_status;
extern uint8_t sg90_status;
extern uint8_t temp_value;
extern uint8_t humi_value;
extern uint16_t lux_value;
extern uint16_t ppm_value;
extern uint16_t sy01_value;

/**
  * @brief          ����json����
  * @param[in]      json_msg:json����,json_len:json���ݳ���
  * @retval         ����0�ҵ�ָ��json���ݣ����򷵻�1
  */
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
    uint8_t retval = 1; // Ĭ��ʧ��
    JSONStatus_t result;
    
    // ��֤JSON����
    if (JSON_Validate((const char *)json_msg, json_len) != JSONSuccess)
    {
        return retval; // �����֤ʧ�ܣ�ֱ�ӷ���
    }

    // �����ѯ·��
    const char *queries[] = {"params.led", "params.sg90"};
    char *values[2];
    size_t valueLengths[2];

    // ����LED��SG90��״̬
    for (int i = 0; i < 2; i++)
    {
        result = JSON_Search((char *)json_msg, json_len, queries[i], strlen(queries[i]), &values[i], &valueLengths[i]);
        if (result != JSONSuccess)
        {
            // �������ʧ�ܣ�������һ����ѯ�������Ϊʧ��
            continue;
        }

        // ��ʱ��ֹ�ַ�������ӡ
        char save = values[i][valueLengths[i]];
        values[i][valueLengths[i]] = '\0';
        printf("Found: %s %zu -> %s\n", queries[i], valueLengths[i], values[i]);
        values[i][valueLengths[i]] = save;

        // ����״̬
        if (strcmp(queries[i], "params.led") == 0)
        {
            led_status = (uint8_t)atoi(values[i]);
        }
        else if (strcmp(queries[i], "params.sg90") == 0)
        {
            sg90_status = (uint8_t)atoi(values[i]);
        }
    }

    // ���������һ����ѯ�ɹ������سɹ�
    if (values[0] != NULL || values[1] != NULL)
    {
        retval = 0;
    }

    return retval;

}


/**
  * @brief          ����2���ݽ��մ�����
  * @param[in]      none
  * @retval         none
  */
void uart2_receiver_handle(void)
{
  unsigned char receive_data = 0;   
  if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);//����2����1λ����
    receive_buf[receive_count++] = receive_data;
    receive_start = 1;	                              //����2�������ݿ�ʼ��־λ��1
    receive_finish = 0;	                              //����2����������ɱ�־λ��0
 }
	
	

	
	

}
/**
  * @brief          ����2���ݽ�����0����
  * @param[in]      len:��յ����ݳ���
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
  * @brief          ��esp8266������������ɹ�����0��ʧ�ܷ���1
  * @param[in]      cmd:���͵�����,len:����ĳ���,rec_data:������������
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
  * @brief          esp8266����wifi����
  * @param[in]      none
  * @retval         �������óɹ�����0,���򷵻�1
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
  * @brief          esp8266���ӷ���
  * @param[in]      none
  * @retval         ���ӳɹ�����0,���򷵻�1
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
  * @brief          esp8266��λ
  * @param[in]      none
  * @retval         ����0��λ�ɹ�,����1��λʧ��
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
  * @brief          ͨ��esp8266��������   ͨ��MQTTЭ�鷢���豸״̬���ݵ��ƶˡ�
  * @param[in]      none
  * @retval         ����0�������ݳɹ�,����1��������ʧ��
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
  * @brief          ͨ��esp8266��������   ��������MQTT����������Ŀ�����Ϣ����������Щ��Ϣ��
  * @param[in]      none
  * @retval         ����0������������,����1���������쳣��������
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
  * @brief          esp8266��ʼ��
  * @param[in]      none
  * @retval         none
  */
void esp8266_init(void)
{
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);           											//�򿪴���2�����ж�
	
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
����ͨ������
UART���ã�
ʹ�õ���HAL_UART_Transmit��HAL_UART_Receive�������д���ͨ�ţ�����ζ��STM32��UART��������UART2��������Ϊ��ESP8266ģ��ͨ�š�
���մ���
uart2_receiver_handle������������ESP8266�Ĵ������ݡ������UART2�Ľ����жϱ�־λ(UART_FLAG_RXNE)����������ݣ�����ղ��洢��receive_buf�����С�


ESP8266ģ�����
����ͣ�
esp8266_send_cmd����������ESP8266����AT������ȴ���Ӧ����ʹ��HAL_UART_Transmit���������ͨ�����receive_start��־λ���ж��Ƿ��յ���Ӧ��
�������ã�
esp8266_config_network����ͨ������AT+CWJAP��������ESP8266���ӵ�ָ����Wi-Fi���硣
MQTT���ӣ�
esp8266_connect_server����ͨ��AT+MQTTCONN�������ӵ�MQTT�����������
���ݷ�������գ�
esp8266_send_msg����ͨ��AT+MQTTPUB������ݷ�����ָ����MQTT���⡣
esp8266_receive_msg���������MQTT���ĵ�������յ�����Ϣ������JSON���ݲ������豸״̬��
*/

