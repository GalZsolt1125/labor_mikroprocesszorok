/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   IWDG Watchdog reset demonstration
 */
/* USER CODE END Header */

#include "main.h"
#include "iwdg.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static volatile uint32_t counter;
static uint32_t wdt_reloads;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_IWDG_Init();

    /* USER CODE BEGIN 2 */

    counter = 0;
    wdt_reloads = 0;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        counter++;

        if (counter == 500000)
        {
            HAL_GPIO_WritePin(
                HMI_LED_1_GPIO_Port,
                HMI_LED_1_Pin,
                GPIO_PIN_SET
            );

            while (counter > 0)
            {
                counter--;
            }

            HAL_GPIO_WritePin(
                HMI_LED_1_GPIO_Port,
                HMI_LED_1_Pin,
                GPIO_PIN_RESET
            );

            HAL_IWDG_Refresh(&hiwdg);

            wdt_reloads++;

            if (wdt_reloads == 5)
            {
                while (1)
                {
                    // No watchdog refresh
                    // Watchdog will reset MCU
                }
            }
        }

        /* USER CODE END WHILE */
        /* USER CODE BEGIN 3 */
    }

    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE3
    );

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_LSI |
        RCC_OSCILLATORTYPE_HSE;

    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSE;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

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
