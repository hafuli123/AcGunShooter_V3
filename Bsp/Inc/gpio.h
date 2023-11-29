/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
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

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

