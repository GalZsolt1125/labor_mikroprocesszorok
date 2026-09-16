#include "main.h"
#include "gpio.h"

#include "../../External/Inc/example.h"

void SystemClock_Config(void);

int main(void)
{
	exampleFunction();

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

  /* MÓDOSÍTÁS: A memóriacímekkel való közvetlen, hibás regiszter-felülírások helyett
     a PB0 lábat (NUCLEO-F429ZI kártyán az LD1 zöld LED) konfiguráljuk fel.
     Órajel engedélyezése bitmaszkolással (nem törli a többi periféria órajelét): */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* PB0 láb beállítása kimenetnek a HAL struktúrával: */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;            // PB0 = LD1 LED
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Push-Pull kimenet
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // Nincs húzóellenállás
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Alacsony sebesség
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  while (1)
  {
      /* MÓDOSÍTÁS: A PD7 helyett a PB0 láb (LD1) állapotát váltjuk (Toggle)
         a biztonságos HAL függvénnyel: */
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

      /* MÓDOSÍTÁS: Pontatlan for ciklus helyett a beépített, pontos HAL időzítőt
         használjuk az 500 ms-os várakozáshoz: */
      HAL_Delay(500);
  }
}

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

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
