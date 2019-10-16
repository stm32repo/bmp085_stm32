/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

struct bmp085_result
{
	int32_t t_cel;
	int32_t p_pas;
	float p_mm_rt_st;
	float p0_mm_rt_st;
};

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

#define MK_STOP HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI)
//#define MK_STOP __NOP
#define LCD_ON_MUSK 1
#define LCD_OFF_MUSK 0
#define BUTTON_UP 1
#define BUTTON_DOWN 2
#define BUTTON_MID 0
#define BUTTON_MENU_TIME 3
#define BUTTON_MENU_ALARM 4

#define MENU_DEFULT 0
#define MENU_SET_TIME 1
#define MENU_SET_ALARM_MAX 2
#define MENU_SET_ALARM_MIN 3

#define SOUND_ENABLE 1
#define SOUND_DISABLE 0
#define BEEP_SHORT 50
#define BEEP_LONG 400

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

int32_t get_temperature_convert(uint8_t* i2c_buffer, uint8_t* temperature);
int32_t get_pressure_convert(uint8_t* i2c_buffer, uint8_t* pressure);
uint8_t bmp085_get_data(I2C_HandleTypeDef *hi2c, struct bmp085_result *result);
void menu_set_time(void);
void lcd_digit(LCD_HandleTypeDef* lcd, uint8_t digit, uint8_t number, uint8_t mask);
void delay_timer(uint16_t ms);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
