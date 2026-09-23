#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* --- ÚJ VÁLTOZÓK A SZÁMLÁLÓHOZ ÉS PERGÉSMENTESÍTÉSHEZ --- */
  uint8_t counter = 0;                  // A 0-15 közötti bináris számláló értéke
  uint8_t debounce_count = 0;           // Számláló a gomb pergetésmentesítéséhez
  uint8_t current_state_debounce = 0;   // A gomb stabilizált (szűrt) állapota
  uint8_t previous_state_edge = 0;       // A gomb előző állapota a fel-él detektálásához

  while (1)
  {
    /* --- 1. GOMB BEOLVASÁSA (D0 / PG9) --- */
    uint8_t raw_btn = HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9);

    /* --- 2. SZOFTVERES PERGÉSMENTESÍTÉS (DEBOUNCING) --- */
    if (raw_btn == GPIO_PIN_SET)
    {
      if (debounce_count < 20)
      {
        debounce_count++; // Növeljük a számlálót, ha a gomb lenyomva van
      }
      else
      {
        current_state_debounce = 1; // Ha elérte a 20-at, stabilan lenyomottnak tekintjük
      }
    }
    else
    {
      if (debounce_count > 0)
      {
        debounce_count--; // Csökkentjük a számlálót, ha a gomb fel van engedve
      }
      else
      {
        current_state_debounce = 0; // Ha elérte a 0-t, stabilan felengedettnek tekintjük
      }
    }

    /* --- 3. ÉLDETEKTÁLÁS ÉS SZÁMLÁLÓ LÉPTETÉS --- */
    // Csak akkor léptetünk, ha MENT a gomb 0-ról 1-re (felfutó él), így 1 nyomás = 1 léptetés
    if (current_state_debounce && !previous_state_edge)
    {
      counter = (counter + 1) & 0x0F; // Számláló növelése, max 15 (0x0F mask 0-15 között tartja)
    }
    previous_state_edge = current_state_debounce; // Eltároljuk az állapotot a következő ciklushoz

    /* --- 4. LED-EK VEZÉRLÉSE A BINÁRIS ÉRTÉK ALAPJÁN --- */
    // D1 (PG14) -> 1. bit (0x01)
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, (counter & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D2 (PF15) -> 2. bit (0x02)
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, (counter & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D3 (PE13) -> 3. bit (0x04)
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, (counter & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D4 (PF14) -> 4. bit (0x08)
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, (counter & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_Delay(5); // Kis mintavételezési várakozási idő (5 ms)
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* --- ÚJ: ÓRAJELEK BEKAPCSOLÁSA A HASZNÁLT PORTOKHOZ (E, F, G) --- */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* --- 1. LED-EK INICIALIZÁLÁSA KIMENETKÉNT (D1, D2, D3, D4) --- */
  // Kezdőérték kikapcsolva (RESET)
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET); // D1
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15 | GPIO_PIN_14, GPIO_PIN_RESET); // D2 és D4
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET); // D3

  // PG14 (D1) beállítása kimenetnek
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  // PF15 (D2) és PF14 (D4) beállítása kimenetnek
  GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  // PE13 (D3) beállítása kimenetnek
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* --- 2. GOMB INICIALIZÁLÁSA BEMENETKÉNT (D0 / PG9) --- */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN; // Belső lehúzó ellenállás
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
