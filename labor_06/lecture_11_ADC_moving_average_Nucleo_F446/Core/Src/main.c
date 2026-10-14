/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Etch A Sketch (A1=PC0, A2=PC3)
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
#include "adc.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "stdio.h"
#include "stdlib.h"
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

/**
  * @brief Garantált csatornaváltó és beolvasó függvény
  */
uint32_t ADC_Read_Channel(ADC_HandleTypeDef* hadc, uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /* 1. ADC leállítása */
    HAL_ADC_Stop(hadc);

    /* 2. Csatorna konfigurálása */
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES; // Stabilabb mintavételezés

    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
    {
        return 0;
    }

    /* 3. Mérés indítása */
    HAL_ADC_Start(hadc);

    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK)
    {
        uint32_t val = HAL_ADC_GetValue(hadc);
        HAL_ADC_Stop(hadc);
        return val;
    }

    HAL_ADC_Stop(hadc);
    return 0;
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_ADC1_Init();
  MX_I2C2_Init();

  /* USER CODE BEGIN 2 */

  /* Manuálisan engedélyezzük a GPIOC órajelet és az analóg módokat a biztonság kedvéért */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_3; // PC0 (A1) és PC3 (A2)
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  OLED_Init();
  OLED_Fill(Black);
  OLED_UpdateScreen();

  const uint8_t SCREEN_WIDTH = 128;
  const uint8_t SCREEN_HEIGHT = 64;

  /* Kezdeti mérések beolvasása */
  uint32_t init_raw_x = ADC_Read_Channel(&hadc1, ADC_CHANNEL_10); // A1 (PC0)
  uint32_t init_raw_y = ADC_Read_Channel(&hadc1, ADC_CHANNEL_13); // A2 (PC3)

  float filtered_X = (float)init_raw_x;
  float filtered_Y = (float)init_raw_y;

  uint8_t last_x = (uint8_t)((filtered_X / 4095.0f) * (SCREEN_WIDTH - 1));
  uint8_t last_y = (SCREEN_HEIGHT - 1) - (uint8_t)((filtered_Y / 4095.0f) * (SCREEN_HEIGHT - 1));

  /* Szűrő együttható (EMA): 0.08 ideális a sima mozgáshoz */
  const float ALPHA = 0.08f;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* 1. Mérés beolvasása mindkét csatornáról külön-külön */
    uint32_t raw_x = ADC_Read_Channel(&hadc1, ADC_CHANNEL_10); // A1 = PC0 = X tengely[cite: 1]
    uint32_t raw_y = ADC_Read_Channel(&hadc1, ADC_CHANNEL_13); // A2 = PC3 = Y tengely[cite: 1]

    /* 2. Exponenciális simítás (EMA) külön mindkét tengelyre */
    filtered_X = (ALPHA * (float)raw_x) + ((1.0f - ALPHA) * filtered_X);
    filtered_Y = (ALPHA * (float)raw_y) + ((1.0f - ALPHA) * filtered_Y);

    /* 3. Koordináták kiszámítása */
    uint8_t current_x = (uint8_t)((filtered_X / 4095.0f) * (SCREEN_WIDTH - 1));
    uint8_t current_y = (SCREEN_HEIGHT - 1) - (uint8_t)((filtered_Y / 4095.0f) * (SCREEN_HEIGHT - 1));

    /* 4. Vonallal összekötés a szaggatás elkerülésére (Bresenham) */
    int dx = abs((int)current_x - (int)last_x);
    int dy = -abs((int)current_y - (int)last_y);
    int sx = (last_x < current_x) ? 1 : -1;
    int sy = (last_y < current_y) ? 1 : -1;
    int err = dx + dy;

    int x = last_x;
    int y = last_y;

    while (1) {
        OLED_DrawPixel(x, y, White);
        if (x == current_x && y == current_y) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }

    last_x = current_x;
    last_y = current_y;

    OLED_UpdateScreen();
    HAL_Delay(10);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
