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
#include "stm32f4xx_hal.h"

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
#define MP3_IO1_O_Pin GPIO_PIN_2
#define MP3_IO1_O_GPIO_Port GPIOC
#define MP3_IO2_O_Pin GPIO_PIN_3
#define MP3_IO2_O_GPIO_Port GPIOC
#define MP3_IO5_O_Pin GPIO_PIN_1
#define MP3_IO5_O_GPIO_Port GPIOA
#define MP3_IO4_Pin GPIO_PIN_4
#define MP3_IO4_GPIO_Port GPIOC
#define MP3_IO3_O_Pin GPIO_PIN_5
#define MP3_IO3_O_GPIO_Port GPIOC
#define AUDIO_CTRL_Pin GPIO_PIN_10
#define AUDIO_CTRL_GPIO_Port GPIOB
#define MTR_CTRL_Pin GPIO_PIN_13
#define MTR_CTRL_GPIO_Port GPIOB
#define REDLSZ_CTRL_O_Pin GPIO_PIN_14
#define REDLSZ_CTRL_O_GPIO_Port GPIOB
#define INVLSZ_CTRL_O_Pin GPIO_PIN_15
#define INVLSZ_CTRL_O_GPIO_Port GPIOB
#define BAT_CHGCMPLT_I_Pin GPIO_PIN_15
#define BAT_CHGCMPLT_I_GPIO_Port GPIOA
#define PWR_CTRL_O_Pin GPIO_PIN_3
#define PWR_CTRL_O_GPIO_Port GPIOB
#define MAG_STAT_I_Pin GPIO_PIN_4
#define MAG_STAT_I_GPIO_Port GPIOB
#define BAT_CHGSTAT_I_Pin GPIO_PIN_5
#define BAT_CHGSTAT_I_GPIO_Port GPIOB
#define BTN_PWR_I_Pin GPIO_PIN_8
#define BTN_PWR_I_GPIO_Port GPIOB
#define BTN_USER_I_Pin GPIO_PIN_9
#define BTN_USER_I_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
