/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define AIN2_Pin GPIO_PIN_0
#define AIN2_GPIO_Port GPIOC
#define AIN1_Pin GPIO_PIN_1
#define AIN1_GPIO_Port GPIOC
#define BIN2_Pin GPIO_PIN_2
#define BIN2_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_3
#define BIN1_GPIO_Port GPIOC
#define E4A_Pin GPIO_PIN_0
#define E4A_GPIO_Port GPIOA
#define E4B_Pin GPIO_PIN_1
#define E4B_GPIO_Port GPIOA
#define E2A_Pin GPIO_PIN_6
#define E2A_GPIO_Port GPIOA
#define E2B_Pin GPIO_PIN_7
#define E2B_GPIO_Port GPIOA
#define CIN2_Pin GPIO_PIN_12
#define CIN2_GPIO_Port GPIOB
#define CIN1_Pin GPIO_PIN_13
#define CIN1_GPIO_Port GPIOB
#define DIN2_Pin GPIO_PIN_14
#define DIN2_GPIO_Port GPIOB
#define DIN1_Pin GPIO_PIN_15
#define DIN1_GPIO_Port GPIOB
#define USER_KEY_Pin GPIO_PIN_6
#define USER_KEY_GPIO_Port GPIOC
#define USER_LED_Pin GPIO_PIN_7
#define USER_LED_GPIO_Port GPIOC
#define PWMA_Pin GPIO_PIN_8
#define PWMA_GPIO_Port GPIOA
#define PWMB_Pin GPIO_PIN_9
#define PWMB_GPIO_Port GPIOA
#define PWMC_Pin GPIO_PIN_10
#define PWMC_GPIO_Port GPIOA
#define PWMD_Pin GPIO_PIN_11
#define PWMD_GPIO_Port GPIOA
#define E1A_Pin GPIO_PIN_15
#define E1A_GPIO_Port GPIOA
#define E1B_Pin GPIO_PIN_3
#define E1B_GPIO_Port GPIOB
#define E3A_Pin GPIO_PIN_6
#define E3A_GPIO_Port GPIOB
#define E3B_Pin GPIO_PIN_7
#define E3B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
