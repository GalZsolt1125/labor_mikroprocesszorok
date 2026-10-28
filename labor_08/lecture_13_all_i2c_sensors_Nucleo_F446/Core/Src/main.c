/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "humidity_sensor.h"
#include "light_sensor.h"
#include "temperature_sensor.h"
#include <stdio.h>

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
}

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
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  // Debouncing code from the 7th chapter
#define DEBOUNCING_TRESHOLD 20

  uint8_t count_debounce = 0;
  uint8_t previous_state_debounce;
  uint8_t current_state_debounce = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);

  uint8_t previous_state_edge = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);
  uint8_t counter = 0x01;

  // Initialize OLED display for showing lm_temperature reading
  OLED_Init();

  // Light sensor
  // Storing measured light level
  float lightLevel;
  uint16_t channel_0;
  uint16_t channel_1;

  // Character array for displaying the measured light level
  char lightLevel_char[18];
  uint16_t lightlevel_integer;
  uint16_t lightlevel_fractional;
  char channels_char[18];

  // Set the I2C slave address of the light sensor
  //LightSensor_SetAddress(LightSensor_ADDR_VDD);
  LightSensor_SetAddress(LightSensor_ADDR_FLOAT);
  //LightSensor_SetAddress(LightSensor_ADDR_GND);

  // Initialize the light sensor
  LightSensor_SetPowerState(LightSensor_POWER_ON);
  LightSensor_SetGain(LightSensor_GAIN_16X);
  LightSensor_SetIntegrationTime(LightSensor_INTEG_101m);

  // Temperature sensor
  // For storing the measured hih_temperature
  float lm_temperature;
  uint16_t lm_temperature_int;
  uint16_t lm_temperature_frac;

  // Character array for displaying the measured hih_temperature
  char lm_temperature_char[11];

  /*
   * Use the Overtemperature Shoutdown function of the IC
   * (LED next to the LM75A IC)
   */
  // Set the overtemperature threshold (turn on) value to 28 degrees Celsius
  LM75A_SetOSTemperature(28);
  // Set the overtemperature hysteresis (turn off) value to 27 degrees Celsius
  LM75A_SetOSHysteresis(27);

  // Humidity sensor
  // Storing measured data
  float humidityLevel;
  float hih_temperature;

  // Character array for displaying the measured humidity
  int string_length = 20;
  char humidityLevel_char[string_length];
  char hih_temperature_char[string_length];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  count_debounce = 0;
	  while (count_debounce < DEBOUNCING_TRESHOLD)
	  {
		  previous_state_debounce = current_state_debounce;
		  current_state_debounce = HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);
		  if (previous_state_debounce == current_state_debounce)
		  {
			  count_debounce++;
		  }
		  else
		  {
			  count_debounce = 0;
		  }
	  }
	  if(current_state_debounce && !previous_state_edge)
	  {
		  counter <<= 1;
		  if (counter == 0x10)
		  {
			  counter = 0x01;
		  }
	  }
	  previous_state_edge = current_state_debounce;

	  // Light sensor
	  if (counter == 0x01)
	  {
		  // Read sensor
		  LightSensor_GetLightLevel(&lightLevel);
		  LightSensor_GetCH0Value(&channel_0);
		  LightSensor_GetCH1Value(&channel_1);

		  // String conversion
		  lightlevel_integer = (uint16_t)lightLevel;
		  lightlevel_fractional = (uint16_t)(lightLevel * 100) - 100 * (uint16_t)lightLevel;
		  sprintf(lightLevel_char, "%d.%d lux", lightlevel_integer, lightlevel_fractional);
		  sprintf(channels_char, "%d, %d", channel_0, channel_1);

		  OLED_Fill(Black);

		  OLED_SetCursor(2, 0);
		  OLED_WriteString("Illuminance:", Font_7x10, White);
		  OLED_SetCursor(2, 10);
		  OLED_WriteString(lightLevel_char, Font_7x10, White);

		  OLED_SetCursor(2, 10+10);
		  OLED_WriteString("Channel 0, 1:", Font_7x10, White);
		  OLED_SetCursor(2, 10+10+10);
		  OLED_WriteString(channels_char, Font_7x10, White);
	  }
	  else if(counter == 0x02)
	  {
		  // Temperature sensor
		  // Read the sensor.
		  LM75A_GetTemperature(&lm_temperature);

		  // Convert the float type value to string. The integer and fractional parts handled separately.
		  lm_temperature_int = (uint16_t)lm_temperature;
		  lm_temperature_frac = (uint16_t)(lm_temperature * 100) - 100 * (uint16_t)lm_temperature;
		  sprintf(lm_temperature_char, "%d.%d C", lm_temperature_int, lm_temperature_frac);

		  // Update screen
		  OLED_Fill(Black);
		  OLED_SetCursor(2, 0);
		  OLED_WriteString("Temperature:", Font_7x10, White);
		  OLED_SetCursor(2, 10);
		  OLED_WriteString(lm_temperature_char, Font_7x10, White);
		  // The position of the circle depends on the length of the fractional part of the displayed number.
		  if (lm_temperature_char[4] == ' ')
		  {
			  OLED_DrawCircle(33, 12, 2, White);
		  }
		  else
		  {
			  OLED_DrawCircle(40, 12, 2, White);
		  }
	  }
	  else if (counter == 0x04)
	  {
		  /* Read sensor and convert data
	  	 	 	 Integer and fractional parts converted separately,
	  	 	 	 since casting to (uint16_t) will ignore fractional part of the float type number.*/
		  HumTempSensor_GetValues(&humidityLevel, &hih_temperature);
		  sprintf(humidityLevel_char, "%d.%d%%", (uint16_t)humidityLevel, (uint16_t)(humidityLevel * 100) - ((uint16_t)humidityLevel) * 100);
		  sprintf(hih_temperature_char, "%d.%d C", (uint16_t)hih_temperature, (uint16_t)(hih_temperature * 100) - ((uint16_t)hih_temperature) * 100 );

		  // Update screen
		  OLED_Fill(Black);

		  OLED_SetCursor(2, 0);
		  OLED_WriteString("Relative humidity:", Font_7x10, White);
		  OLED_SetCursor(2, 10);
		  OLED_WriteString(humidityLevel_char, Font_7x10, White);
		  OLED_SetCursor(2, 10+10);
		  OLED_WriteString("Temperature:", Font_7x10, White);
		  OLED_SetCursor(2, 10+10+10);
		  OLED_WriteString(hih_temperature_char, Font_7x10, White);
		  // The position of the circle depends on the length of the fractional part of the displayed number
		  if (hih_temperature_char[4] == ' ')
		  {
			  OLED_DrawCircle(33, 32, 2, White);
		  }
		  else
		  {
			  OLED_DrawCircle(40, 32, 2, White);
		  }
	  }
	  else if (counter == 0x08)
	  {
		  // Temperature sensor
		  // Read the sensor.
		  LM75A_GetTemperature(&lm_temperature);

		  // Convert the float type value to string. The integer and fractional parts handled separately.
		  lm_temperature_int = (uint16_t)lm_temperature;
		  lm_temperature_frac = (uint16_t)(lm_temperature * 100) - 100 * (uint16_t)lm_temperature;
		  sprintf(lm_temperature_char, "%d.%d C", lm_temperature_int, lm_temperature_frac);

		  // Humidity sensor

		  HumTempSensor_GetValues(&humidityLevel, &hih_temperature);
		  sprintf(hih_temperature_char, "%d.%d C", (uint16_t)hih_temperature, (uint16_t)(hih_temperature * 100) - ((uint16_t)hih_temperature) * 100 );


		  // Update screen
		  OLED_Fill(Black);
		  OLED_SetCursor(2, 0);
		  OLED_WriteString("LM75:", Font_7x10, White);
		  OLED_SetCursor(2, 10);
		  OLED_WriteString(lm_temperature_char, Font_7x10, White);
		  // The position of the circle depends on the length of the fractional part of the displayed number.
		  if (lm_temperature_char[4] == ' ')
		  {
			  OLED_DrawCircle(33, 12, 2, White);
		  }
		  else
		  {
			  OLED_DrawCircle(40, 12, 2, White);
		  }
		  OLED_SetCursor(2, 20);
		  OLED_WriteString("HIH8120:", Font_7x10, White);
		  OLED_SetCursor(2, 30);
		  OLED_WriteString(hih_temperature_char, Font_7x10, White);
		  // The position of the circle depends on the length of the fractional part of the displayed number.
		  if (hih_temperature_char[4] == ' ')
		  {
			  OLED_DrawCircle(33, 32, 2, White);
		  }
		  else
		  {
			  OLED_DrawCircle(40, 32, 2, White);
		  }
	  }
	  else
	  {
		  OLED_Fill(Black);

	  }
	  OLED_UpdateScreen();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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
