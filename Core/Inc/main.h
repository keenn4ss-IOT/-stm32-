/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define buzzer_Pin GPIO_PIN_14
#define buzzer_GPIO_Port GPIOC
#define mode_light_Pin GPIO_PIN_15
#define mode_light_GPIO_Port GPIOC
#define button_mode_Pin GPIO_PIN_1
#define button_mode_GPIO_Port GPIOA
#define button_mode_EXTI_IRQn EXTI1_IRQn
#define esp8266_Pin GPIO_PIN_2
#define esp8266_GPIO_Port GPIOA
#define esp8266A3_Pin GPIO_PIN_3
#define esp8266A3_GPIO_Port GPIOA
#define button_up_Pin GPIO_PIN_4
#define button_up_GPIO_Port GPIOA
#define button_down_Pin GPIO_PIN_5
#define button_down_GPIO_Port GPIOA
#define button_enter_Pin GPIO_PIN_6
#define button_enter_GPIO_Port GPIOA
#define button_enter_EXTI_IRQn EXTI9_5_IRQn
#define sy01___Pin GPIO_PIN_7
#define sy01___GPIO_Port GPIOA
#define a5516___Pin GPIO_PIN_0
#define a5516___GPIO_Port GPIOB
#define mq2___Pin GPIO_PIN_1
#define mq2___GPIO_Port GPIOB
#define DHT11_Pin GPIO_PIN_15
#define DHT11_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_4
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_5
#define SDA_GPIO_Port GPIOB
#define RES_Pin GPIO_PIN_6
#define RES_GPIO_Port GPIOB
#define DC_Pin GPIO_PIN_7
#define DC_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_8
#define CS_GPIO_Port GPIOB
#define BLK_Pin GPIO_PIN_9
#define BLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
