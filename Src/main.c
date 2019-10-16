/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint32_t testd = 0;
volatile uint8_t status_interface = MENU_DEFULT;
volatile uint8_t button = 0;
uint32_t alarm_max_pressure = 7800;
uint32_t alarm_min_pressure = 7300;
uint8_t sound_alarm = SOUND_ENABLE;
volatile uint32_t tim_count;
RTC_TimeTypeDef get_time, set_time;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

LCD_HandleTypeDef hlcd;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim11;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_LCD_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM11_Init(void);
/* USER CODE BEGIN PFP */



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	tim_count++;
}

/* sound signal */
void beeper(uint32_t signal_long) {
	for(uint32_t per = 0;per < signal_long;per++) {
			for(uint32_t up = 0;up < 50;up++);
			GPIOA->BSRR = GPIO_BSRR_BR_12;
			for(uint32_t down = 0;down < 200;down++);
			GPIOA->BSRR = GPIO_BSRR_BS_12;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if ((GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_13) != 0) {
		if (GPIO_Pin == GPIO_PIN_0) button = BUTTON_UP;
		if (GPIO_Pin == GPIO_PIN_1) button = BUTTON_MID;
		if (GPIO_Pin == GPIO_PIN_13) button = BUTTON_DOWN;
		if(((GPIOH->IDR & GPIO_IDR_IDR_0) | (GPIOC->IDR & GPIO_IDR_IDR_13)) == (GPIO_IDR_IDR_0 | GPIO_IDR_IDR_13)) {
			status_interface = MENU_SET_TIME;
			button = BUTTON_MENU_TIME;
		}
		if(((GPIOH->IDR & GPIO_IDR_IDR_0) | (GPIOH->IDR & GPIO_IDR_IDR_1)) == (GPIO_IDR_IDR_0 | GPIO_IDR_IDR_1)) {
			status_interface = MENU_SET_ALARM_MAX;
			button = BUTTON_MENU_ALARM;
		}
		if(((GPIOC->IDR & GPIO_IDR_IDR_13) | (GPIOH->IDR & GPIO_IDR_IDR_1)) == (GPIO_IDR_IDR_13 | GPIO_IDR_IDR_1)) {
			status_interface = MENU_SET_ALARM_MIN;
			button = BUTTON_MENU_ALARM;
		}			
		beeper(BEEP_SHORT);
	}
}

void show_time(void) {
	
}

/* Set system time */
void menu_set_time(void) {
		status_interface = MENU_DEFULT;
		uint32_t hold_time;
		HAL_LCD_Clear(&hlcd);
		lcd_digit(&hlcd, get_time.Seconds%10 , 5, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Seconds/10)%10 , 4, LCD_OFF_MUSK);		
		lcd_digit(&hlcd, get_time.Minutes%10 , 3, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Minutes/10)%10 , 2, LCD_OFF_MUSK);		
		lcd_digit(&hlcd, 10, 3, LCD_ON_MUSK); //point
		lcd_digit(&hlcd, get_time.Hours%10 , 1, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Hours/10)%10 , 0, LCD_OFF_MUSK);
		lcd_digit(&hlcd, 10, 1, LCD_ON_MUSK); //point
		HAL_LCD_UpdateDisplayRequest(&hlcd);
	
		// Set secconds
		hold_time = 0;
		while(button != BUTTON_MID) {
			if(button == BUTTON_UP) {
				get_time.Seconds = (get_time.Seconds+1)%60;
				button = BUTTON_MENU_TIME;
			}
			if(button == BUTTON_DOWN) {
				get_time.Seconds = (get_time.Seconds+59)%60;
				button = BUTTON_MENU_TIME;
			}
			lcd_digit(&hlcd, get_time.Seconds%10 , 5, LCD_OFF_MUSK);
			lcd_digit(&hlcd, (get_time.Seconds/10)%10 , 4, LCD_OFF_MUSK);
			lcd_digit(&hlcd, 11, 5, LCD_ON_MUSK);
			lcd_digit(&hlcd, 11, 4, LCD_ON_MUSK);
			HAL_LCD_UpdateDisplayRequest(&hlcd);
			if (hold_time > 5000) { // exit timeout ~30sec, not set
				status_interface = MENU_DEFULT;
				return;
			}
			hold_time++;
		}
		lcd_digit(&hlcd, get_time.Seconds%10 , 5, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Seconds/10)%10 , 4, LCD_OFF_MUSK);
		button = BUTTON_MENU_TIME;
		
		//Set minuts
		hold_time = 0;
		while(button != BUTTON_MID) {
			if(button == BUTTON_UP) {
				get_time.Minutes = (get_time.Minutes+1)%60;
				button = BUTTON_MENU_TIME;
			}
			if(button == BUTTON_DOWN) {
				get_time.Minutes = (get_time.Minutes+59)%60;
				button = BUTTON_MENU_TIME;
			}
			lcd_digit(&hlcd, get_time.Minutes%10 , 3, LCD_OFF_MUSK);
			lcd_digit(&hlcd, (get_time.Minutes/10)%10 , 2, LCD_OFF_MUSK);
			lcd_digit(&hlcd, 11, 3, LCD_ON_MUSK);
			lcd_digit(&hlcd, 10, 3, LCD_ON_MUSK); //point
			lcd_digit(&hlcd, 11, 2, LCD_ON_MUSK);
			HAL_LCD_UpdateDisplayRequest(&hlcd);
			if (hold_time > 5000) { // exit timeout ~30sec, not set
				status_interface = MENU_DEFULT;
				return;
			}
			hold_time++;
		}
		lcd_digit(&hlcd, get_time.Minutes%10 , 3, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Minutes/10)%10 , 2, LCD_OFF_MUSK);
		lcd_digit(&hlcd, 10, 3, LCD_ON_MUSK); //point
		button = BUTTON_MENU_TIME;
		
		//Set hous
		hold_time = 0;
		while(button != BUTTON_MID) {
			if(button == BUTTON_UP) {
				get_time.Hours = (get_time.Hours+1)%24;
				button = BUTTON_MENU_TIME;
			}
			if(button == BUTTON_DOWN) {
				get_time.Hours = (get_time.Hours+23)%24;
				button = BUTTON_MENU_TIME;
			}
			lcd_digit(&hlcd, get_time.Hours%10 , 1, LCD_OFF_MUSK);
			lcd_digit(&hlcd, (get_time.Hours/10)%10 , 0, LCD_OFF_MUSK);
			lcd_digit(&hlcd, 11, 1, LCD_ON_MUSK);
			lcd_digit(&hlcd, 10, 1, LCD_ON_MUSK);
			lcd_digit(&hlcd, 11, 0, LCD_ON_MUSK);
			HAL_LCD_UpdateDisplayRequest(&hlcd);
			if (hold_time > 5000) { // exit timeout ~30sec, not set
				status_interface = MENU_DEFULT;
				return;
			}
			hold_time++;
		}
		lcd_digit(&hlcd, get_time.Hours%10 , 1, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (get_time.Hours/10)%10 , 0, LCD_OFF_MUSK);
		HAL_LCD_UpdateDisplayRequest(&hlcd);
		//apply time 
		HAL_RTC_SetTime(&hrtc, &get_time, RTC_FORMAT_BIN);
		status_interface = MENU_DEFULT;
}

/* alarm signal when exceeded pressure limit */
void menu_set_alarm_pressure(uint8_t mode) {
		uint32_t tmp_pres;
		if(mode == MENU_SET_ALARM_MAX) tmp_pres = alarm_max_pressure;
		if(mode == MENU_SET_ALARM_MIN) tmp_pres = alarm_min_pressure;
		uint32_t hold_time = 0;
		HAL_LCD_Clear(&hlcd);
	while(1) {
		lcd_digit(&hlcd, tmp_pres%10, 5, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (tmp_pres/10)%10, 4, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (tmp_pres/100)%10, 3, LCD_OFF_MUSK);
		lcd_digit(&hlcd, (tmp_pres/1000)%10, 2, LCD_OFF_MUSK);
		lcd_digit(&hlcd, 10, 4, LCD_ON_MUSK); //point
		lcd_digit(&hlcd, 11, 2, LCD_ON_MUSK);
		lcd_digit(&hlcd, 11, 3, LCD_ON_MUSK);
		lcd_digit(&hlcd, 11, 4, LCD_ON_MUSK);
		lcd_digit(&hlcd, 11, 5, LCD_ON_MUSK);
		HAL_LCD_UpdateDisplayRequest(&hlcd);
		if(button == BUTTON_MID)	{
			if(mode == MENU_SET_ALARM_MAX) alarm_max_pressure = tmp_pres;
			if(mode == MENU_SET_ALARM_MIN) alarm_min_pressure = tmp_pres;
			status_interface = MENU_DEFULT;
			return;
		}
		if(button == BUTTON_UP)	{
			button = BUTTON_MENU_ALARM;
			if(tmp_pres < 10000) tmp_pres += 10;
		}
		if(button == BUTTON_DOWN)	{
			button = BUTTON_MENU_ALARM;
			if(tmp_pres > 10) tmp_pres -= 10;
		}
		if (hold_time > 5000) {
			status_interface = MENU_DEFULT;
			return;
		}
		hold_time++;
	}
}

void delay_timer(uint16_t ms) {
	htim11.Init.Period = ms;
	if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
 	tim_count = 0;
	__HAL_TIM_CLEAR_IT (&htim11, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	while(tim_count == 0);
	HAL_TIM_Base_Stop_IT(&htim11);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	//__HAL_RCC_PWR_CLK_ENABLE();
	//HAL_PWR_EnableBkUpAccess();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_LCD_Init();
  MX_RTC_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//HAL_TIM_Base_Start_IT(&htim11);
	HAL_LCD_Clear(&hlcd);
	
	for(uint32_t startp = 0;startp < 500000;startp++);
	SysTick-> CTRL = 0;
	//DBGMCU->CR |= (DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY);
	__HAL_FLASH_SLEEP_POWERDOWN_ENABLE(); //Disable flash in Sleep mode
	HAL_PWREx_EnableLowPowerRunMode(); //Enable low power mode run CPU
	//HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	
  while (1)  {	
		switch (status_interface) {
			case MENU_DEFULT: {					
		/* get pressure */		
				struct bmp085_result test_str;
				bmp085_get_data(&hi2c1, &test_str);
				testd = test_str.p_mm_rt_st*10;
		/* print pressure */		
				HAL_LCD_Clear(&hlcd);
				lcd_digit(&hlcd, testd%10, 5, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (testd/10)%10, 4, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (testd/100)%10, 3, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (testd/1000)%10, 2, LCD_OFF_MUSK);
				lcd_digit(&hlcd, 12, 0, LCD_OFF_MUSK);
				lcd_digit(&hlcd, 10, 4, LCD_ON_MUSK); //point
				HAL_LCD_UpdateDisplayRequest(&hlcd);
				/* check max and min alarm */
				if(testd > alarm_max_pressure) beeper(BEEP_LONG);
				if(testd < alarm_min_pressure) beeper(BEEP_LONG);
				if (status_interface != MENU_DEFULT) break;
				MK_STOP;
				if (status_interface != MENU_DEFULT) break;
		/* print  temperature */
				HAL_LCD_Clear(&hlcd);
				lcd_digit(&hlcd, test_str.t_cel%10, 5, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (test_str.t_cel/10)%10, 4, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (test_str.t_cel/100)%10, 3, LCD_OFF_MUSK);
				if((test_str.t_cel/1000)%10 != 0) lcd_digit(&hlcd, (test_str.t_cel/1000)%10, 2, LCD_OFF_MUSK);
				lcd_digit(&hlcd, 14, 0, LCD_OFF_MUSK);
				lcd_digit(&hlcd, 10, 4, LCD_ON_MUSK); //point
				HAL_LCD_UpdateDisplayRequest(&hlcd);
				if (status_interface != MENU_DEFULT) break;
				MK_STOP;
				if (status_interface != MENU_DEFULT) break;
		/* print clock */
				HAL_RTC_GetTime(&hrtc, &get_time, RTC_FORMAT_BIN);
				HAL_LCD_Clear(&hlcd);							
				lcd_digit(&hlcd, get_time.Minutes%10 , 5, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (get_time.Minutes/10)%10 , 4, LCD_OFF_MUSK);	
				lcd_digit(&hlcd, 13, 3, LCD_OFF_MUSK);	
				lcd_digit(&hlcd, get_time.Hours%10 , 2, LCD_OFF_MUSK);
				lcd_digit(&hlcd, (get_time.Hours/10)%10 , 1, LCD_OFF_MUSK);
				HAL_LCD_UpdateDisplayRequest(&hlcd);
				if (status_interface != MENU_DEFULT) break;
				MK_STOP;
				break;
				}
			case MENU_SET_TIME: {
				menu_set_time();
				break;
			}
			case MENU_SET_ALARM_MAX: {
				menu_set_alarm_pressure(MENU_SET_ALARM_MAX);
				break;
			}
			case MENU_SET_ALARM_MIN: {
				menu_set_alarm_pressure(MENU_SET_ALARM_MIN);
				break;
			}
		}
			
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LCD;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LCD Initialization Function
  * @param None
  * @retval None
  */
static void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_2;
  hlcd.Init.Divider = LCD_DIVIDER_31;
  hlcd.Init.Duty = LCD_DUTY_1_3;
  hlcd.Init.Bias = LCD_BIAS_1_3;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_3;
  hlcd.Init.DeadTime = LCD_DEADTIME_0;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_4;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 15;
  sTime.Minutes = 7;
  sTime.Seconds = 15;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 10;
  sDate.Year = 19;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp 
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 5, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 33;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim11, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA4 PA5 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
