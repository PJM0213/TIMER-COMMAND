/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t rx;
uint8_t buf[20], buf_index;

uint16_t frequency; //0~65,536 GPTimer 16bit
uint16_t T[3];

uint8_t LED_COMMAND[4][30] = {">LEDnm", "n: Insert pin Numb", "m: Insert pin state", "LD1:0, LD2:1, LD3:2"};
uint8_t KEY_COMMAND[4][30] = {">KEYn", "?: Current User Key", "N: Press times", "C: Clear Press times"};
uint8_t TIM_COMMAND[5][30] = {">TIMnm", "n: Insert pin Numb", "m: frequency of LED", "frequency scope: 1 ~ 100", "LD1:0, LD2:1, LD3:2"};

uint16_t IC_trigger_CH1;
uint16_t IC_trigger_CH2;
uint16_t IC_timer_CH1;
uint16_t IC_timer_CH2;

uint16_t IC_previous_timer_CH2;
uint16_t IC_lapse = 20000;
uint16_t IC_pulse = 2000;

uint16_t servo_angle;
uint16_t timer_cnt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM9_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM9_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &rx, 1);
  HAL_TIM_IC_Start_IT(&htim9, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(IC_trigger_CH1 == 1 && IC_trigger_CH2 == 1) // CH1, CH2 open
	  {
	  	  IC_trigger_CH1 = 0;
	  	  IC_trigger_CH2 = 0;

	  	  IC_lapse = IC_timer_CH2 - IC_previous_timer_CH2; //Input Capture 주기
	  	  if(IC_lapse < 0) IC_lapse += 65535; //경과 시간

	  	  IC_pulse = IC_timer_CH2 - IC_timer_CH1; //Input Capture 폭
	  	  if(IC_timer_CH2 < IC_timer_CH1)
	  	  IC_previous_timer_CH2 = IC_timer_CH2;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1599;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 15;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 65535;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim9, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  if (HAL_TIM_IC_ConfigChannel(&htim9, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pins : PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
	 HAL_UART_Transmit(&huart1, (uint8_t*)&rx, 1, 1000); //transmit input data
	 HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx, 1);
	 buf[buf_index++] = rx;  //store the character and pass to next index

	 if(buf_index != 1) //Backspace input
	 {
	 	if(buf[buf_index-1] == 8) //buf_index++:  post increment operator  //backspace ASCII == 8
	 	{
	 	    buf[buf_index-2] = ' ';
	 	 	buf[buf_index-1] = '\b'; //'\b' = backspace
	 	 	sprintf((char*)&rx, "%c", buf[buf_index-2]);
	 	 	HAL_UART_Transmit(&huart1, (uint8_t*)&rx, strlen((const char*)&rx), 1000);
	 	 	sprintf((char*)&rx, "%c", buf[buf_index-1]);
	 	 	HAL_UART_Transmit(&huart1, (uint8_t*)&rx, strlen((const char*)&rx), 1000);

	 	 	buf_index = buf_index-2; //buf_index = ' ';
	    }
	 } //Backspace Key

	 if(rx == 13) //Enter ASCII == 13, frequency setting
	 {
		buf[buf_index] = 0; //input null at last index
		char*p;

		if((p = strstr((char*)buf, "TIM")) != 0) // search the string "TIM" in buffer
		{
		 	if(p[3] == '?')
		 	{
		 		char buffer[20];
		 		sprintf(buffer, "Frequency = %d\n", frequency);
		 		HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
		 	}
		 	else
		 	{
		 	 	frequency = atoi(&p[3]); //conversion from "string" to integer

		 	   	if(frequency == 0)
		 	 	{
		 	 		HAL_TIM_Base_Stop_IT(&htim3);
		 	 	}
		 	 	else
		 	 	{
		 	 		uint16_t T = (10000 / frequency) /2 - 1; //(from '0') -> LED Toggle period
		 	 		htim3.Init.Period = T;
		 	 		HAL_TIM_Base_Init(&htim3);
		 	 		HAL_TIM_Base_Start_IT(&htim3);
		 	 	}
		 	 }
		} //Frequency Setting

		if((p = strstr((char*)buf, "HELP")) != 0) //search the string "HELP" in buffer
		{
			 char*p2;
			 char HELP_buffer[50]={""};
			 if((p2 = strstr((char*)buf, "LED")) != 0)
			 {
				 if(*p2 == p[5])
				 {
					 for(int i = 0; i <= 3; i++)
					 {
						 sprintf(HELP_buffer, "%s\r\n", LED_COMMAND[i]);
						 HAL_UART_Transmit(&huart1, (uint8_t*)HELP_buffer, strlen(HELP_buffer), 1000);
					 }
				 }
			 }
			 else if((p2 = strstr((char*)buf, "KEY")) != 0)
			 {
				 if(*p2 == p[5])
				 {
					 for(int i = 0; i <= 3; i++)
					 {
						 sprintf(HELP_buffer, "%s\r\n", KEY_COMMAND[i]);
						 HAL_UART_Transmit(&huart1, (uint8_t*)HELP_buffer, strlen(HELP_buffer), 1000);
					 }
				 }
			 }
			 else if((p2 = strstr((char*)buf, "TIM")) != 0)
			 {
				 if(*p2 == p[5])
				 {
					 for(int i = 0; i <= 4; i++)
					 {
						 sprintf(HELP_buffer, "%s\r\n", TIM_COMMAND[i]);
						 HAL_UART_Transmit(&huart1, (uint8_t*)HELP_buffer, strlen(HELP_buffer), 1000);
				 	 }
				 }
			 }
		}
		buf_index = 0;

		if((p = strstr((char*)buf, "SVM")) != 0) //search the string "SVM" in buffer
		{
			if(p[3] == '?')
			{
				char buffer[35]={""};
				sprintf(buffer, ">SVM Pulse Width = %d us\r\n>SVM T = %d us\r\n", IC_pulse, IC_lapse);
				HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
			}
			else //Angle Setting
			{
				servo_angle = atoi(&p[3]);
				if(servo_angle >= 0 && servo_angle <= 180)
				{
					HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_1);
				}
				else
				{
					char buffer[50]={""};
					sprintf(buffer, "%d = Value Error\r\n", servo_angle);
					HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
				}
		    }
		}

		if((p = strstr((char*)buf, "S")) != 0) //search the string "S" in buffer
		{
		  	char*p2;
		  	if((p2 = strstr((char*)buf, "ON")) !=0)
		  	{
		  		if(*p2 == p[1])
		  		{
		  			double distance = (IC_pulse / 29.0) / 2.0;
		  			char buffer[50]={""};
		  			sprintf(buffer, ">Distance from Object = %.1lfcm\r\n", distance);
		  			HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
		  		}
		  	}
		  	else if((p2 = strstr((char*)buf, "OFF")) != 0)
		  	{
		  		if(*p2 == p[1])
		  		{
		  			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
		  		}
		  	}
		 }

	  }
   }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3)
  {
	  HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
	  HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance == TIM9)
 {
	 IC_trigger_CH1 = 1;
	 IC_timer_CH1 = TIM9->CCR1;  //TIM9 Channel 1

	 IC_trigger_CH2 = 1;
	 IC_timer_CH2 = TIM9->CCR2;  //TIM9 Channel 2
 }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
