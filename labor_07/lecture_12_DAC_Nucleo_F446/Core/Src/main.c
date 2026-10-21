/* USER CODE BEGIN Header */
/**
  ****************************************************************************
  * @file           : main.c
  * @brief          : ECG signal generation using DAC
  ****************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdint.h>
#include "OLED.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ECG_SAMPLES 400U
#define ECG_SAMPLE_PERIOD_MS 1U

#define DAC_MAX_VALUE 4095U
#define DAC_REFERENCE_VOLTAGE 3.3f
#define ECG_OFFSET_VOLTAGE 1.65f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint32_t ecg_data[ECG_SAMPLES];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

void GenerateECG(uint32_t *ecg, uint32_t length);
float Gaussian(float x, float mean, float sigma);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float Gaussian(float x, float mean, float sigma)
{
    float z = (x - mean) / sigma;
    return expf(-0.5f * z * z);
}

void GenerateECG(uint32_t *ecg, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        float t = (float)i / (float)length;

        float P =  0.20f * Gaussian(t, 0.20f, 0.025f);
        float Q = -0.15f * Gaussian(t, 0.37f, 0.010f);
        float R =  1.15f * Gaussian(t, 0.40f, 0.006f);
        float S = -0.30f * Gaussian(t, 0.425f, 0.012f);
        float T =  0.32f * Gaussian(t, 0.66f, 0.060f);

        float ecg_signal = P + Q + R + S + T;

        float voltage = ECG_OFFSET_VOLTAGE + ecg_signal;

        if (voltage < 0.0f)
        {
            voltage = 0.0f;
        }

        if (voltage > DAC_REFERENCE_VOLTAGE)
        {
            voltage = DAC_REFERENCE_VOLTAGE;
        }

        ecg[i] = (uint32_t)(
            voltage * DAC_MAX_VALUE / DAC_REFERENCE_VOLTAGE
            + 0.5f
        );
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    uint32_t ecg_index = 0;
    uint32_t previous_tick = 0;
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    MX_GPIO_Init();
    MX_DAC_Init();
    MX_I2C2_Init();

    /* USER CODE BEGIN 2 */

    // Generate ECG waveform
    GenerateECG(ecg_data, ECG_SAMPLES);

    // Set initial DAC output
    if (HAL_DAC_SetValue(&hdac,
                         DAC_CHANNEL_1,
                         DAC_ALIGN_12B_R,
                         ecg_data[0]) != HAL_OK)
    {
        Error_Handler();
    }

    // Start DAC
    if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    previous_tick = HAL_GetTick();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        uint32_t current_tick = HAL_GetTick();

        if ((uint32_t)(current_tick - previous_tick)
                >= ECG_SAMPLE_PERIOD_MS)
        {
            previous_tick += ECG_SAMPLE_PERIOD_MS;

            ecg_index++;

            if (ecg_index >= ECG_SAMPLES)
            {
                ecg_index = 0;
            }

            HAL_DAC_SetValue(&hdac,
                             DAC_CHANNEL_1,
                             DAC_ALIGN_12B_R,
                             ecg_data[ecg_index]);
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

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                            FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief Error Handler
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

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* USER CODE END 6 */
}

#endif
