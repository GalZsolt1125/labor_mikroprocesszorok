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
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
    OLED_STATE_CCE = 0x00,
    OLED_STATE_FONTS = 0x01,
    OLED_STATE_LINE = 0x02,
    OLED_STATE_RECTANGLE = 0x03,
    OLED_STATE_CIRCLE = 0x04,
    OLED_STATE_ARC = 0x05,
    OLED_STATE_POLYLINE = 0x06,
    OLED_STATE_MY_SCREEN = 0x07
} OLED_Test_state;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

OLED_Test_state test_state = OLED_STATE_CCE;

GPIO_PinState previous_button_state[3] = {0u, 0u, 0u};
GPIO_PinState current_button_state[3] = {0u, 0u, 0u};

uint8_t start_x = 0u;
uint8_t start_r = 1u;
uint16_t start_angle = 0u;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* Random Line generator */
void OLED_TestLine()
{
    int random_x1 = rand() % 128;
    int random_x2 = rand() % 128;

    OLED_DrawLine((uint8_t)random_x1,
                  0,
                  (uint8_t)random_x2,
                  63,
                  WHITE);

    return;
}


/* Rectangle screen saver */
void OLED_TestRectangle()
{
    uint32_t delta;

    for (delta = 0; delta < 5; delta++)
    {
        OLED_DrawRectangle(
            1 + (5 * delta),
            1 + (5 * delta),
            OLED_WIDTH - 1 - (5 * delta),
            OLED_HEIGHT - 1 - (5 * delta),
            WHITE
        );
    }

    return;
}


/* Pipe - Circle screen saver */
void OLED_TestCircle(uint8_t* x, uint8_t* r)
{
    OLED_DrawCircle(*x, 31, *r, WHITE);

    *r += 1;
    *x += 2;

    if (100 < *x)
    {
        *x = 0;
        *r = 1;

        OLED_Fill(BLACK);
    }

    return;
}


/* Arc */
void OLED_TestArc(uint16_t* angle)
{
    OLED_Fill(BLACK);

    OLED_DrawArc(63, 31, 5,  *angle, 270 + (*angle), WHITE);
    OLED_DrawArc(63, 31, 10, *angle, 270 + (*angle), WHITE);
    OLED_DrawArc(63, 31, 15, *angle, 270 + (*angle), WHITE);
    OLED_DrawArc(63, 31, 20, *angle, 270 + (*angle), WHITE);

    *angle += 10;

    return;
}


/* Polyline */
void OLED_TestPolyline()
{
    OLED_Vertex loc_vertex[] =
    {
        {63, 11},
        {43, 20},
        {63, 51},
        {83, 20},
        {63, 11}
    };

    OLED_DrawPolyline(
        loc_vertex,
        sizeof(loc_vertex) / sizeof(loc_vertex[0]),
        WHITE
    );

    return;
}

void OLED_TestFonts()
{
    OLED_SetCursor(2, 0);
    OLED_WriteString("16x26", Font_16x26, WHITE);

    OLED_SetCursor(2, 26);
    OLED_WriteString("11x18", Font_11x18, WHITE);

    OLED_SetCursor(2, 26 + 18);
    OLED_WriteString("7x10", Font_7x10, WHITE);

    OLED_SetCursor(2, 26 + 18 + 10);
    OLED_WriteString("6x8", Font_6x8, WHITE);

    return;
}

void OLED_TestCCE()
{
    OLED_SetCursor(25, 5);
    OLED_WriteString("Crystal", Font_11x18, WHITE);

    OLED_SetCursor(36, 5 + 18);
    OLED_WriteString("Clear", Font_11x18, WHITE);

    OLED_SetCursor(3, 5 + 18 + 18);
    OLED_WriteString("Electronics", Font_11x18, WHITE);

    return;
}

void OLED_TestMyScreen()
{
    OLED_Fill(BLACK);

    /* Kulso teglalap */
    OLED_DrawRectangle(
        1,
        1,
        OLED_WIDTH - 2,
        OLED_HEIGHT - 2,
        WHITE
    );

    /* Belso teglalap */
    OLED_DrawRectangle(
        4,
        4,
        OLED_WIDTH - 5,
        OLED_HEIGHT - 5,
        WHITE
    );

    /* Korok a negy sarokban */
    OLED_DrawCircle(10, 10, 4, WHITE);
    OLED_DrawCircle(117, 10, 4, WHITE);
    OLED_DrawCircle(10, 53, 4, WHITE);
    OLED_DrawCircle(117, 53, 4, WHITE);

    /* Nev */
    OLED_SetCursor(24, 18);
    OLED_WriteString("Gal Zsolt", Font_7x10, WHITE);

    /* Kurzus */
    OLED_SetCursor(17, 36);
    OLED_WriteString("Zsolti a Beka", Font_6x8, WHITE);

    return;
}

int main(void)
{

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C2_Init();

    OLED_Init();

    HAL_Delay(1000);

    OLED_Fill(BLACK);
    OLED_UpdateScreen();

    HAL_Delay(1000);

    while (1)
    {

        current_button_state[0] =
            HAL_GPIO_ReadPin(HMI_BTN_2_GPIO_Port, HMI_BTN_2_Pin);

        current_button_state[1] =
            HAL_GPIO_ReadPin(HMI_BTN_3_GPIO_Port, HMI_BTN_3_Pin);

        current_button_state[2] =
            HAL_GPIO_ReadPin(HMI_BTN_4_GPIO_Port, HMI_BTN_4_Pin);

        if ((current_button_state[0] != previous_button_state[0]) &&
            (GPIO_PIN_RESET == current_button_state[0]))
        {
            if (OLED_STATE_CCE == test_state)
            {
                test_state = OLED_STATE_MY_SCREEN;
            }
            else
            {
                test_state -= 1;
            }

            OLED_Fill(BLACK);
            OLED_UpdateScreen();
        }


        if ((current_button_state[2] != previous_button_state[2]) &&
            (GPIO_PIN_RESET == current_button_state[2]))
        {
            if (OLED_STATE_MY_SCREEN == test_state)
            {
                test_state = OLED_STATE_CCE;
            }
            else
            {
                test_state += 1;
            }

            OLED_Fill(BLACK);
            OLED_UpdateScreen();
        }

        if ((current_button_state[1] != previous_button_state[1]) &&
            (GPIO_PIN_RESET == current_button_state[1]))
        {
            if (OFF == OLED_GetDisplayInverse())
            {
                OLED_SetDisplayInverse(ON);
            }
            else
            {
                OLED_SetDisplayInverse(OFF);
            }
        }

        HAL_GPIO_WritePin(
            HMI_LED_2_GPIO_Port,
            HMI_LED_2_Pin,
            current_button_state[0]
        );

        HAL_GPIO_WritePin(
            HMI_LED_3_GPIO_Port,
            HMI_LED_3_Pin,
            current_button_state[1]
        );

        HAL_GPIO_WritePin(
            HMI_LED_4_GPIO_Port,
            HMI_LED_4_Pin,
            current_button_state[2]
        );


        previous_button_state[0] = current_button_state[0];
        previous_button_state[1] = current_button_state[1];
        previous_button_state[2] = current_button_state[2];

        switch (test_state)
        {
            case OLED_STATE_FONTS:

                OLED_TestFonts();

                break;


            case OLED_STATE_LINE:

                OLED_TestLine();

                break;


            case OLED_STATE_RECTANGLE:

                OLED_TestRectangle();

                break;


            case OLED_STATE_CIRCLE:

                OLED_TestCircle(&start_x, &start_r);

                break;


            case OLED_STATE_ARC:

                OLED_TestArc(&start_angle);

                break;


            case OLED_STATE_POLYLINE:

                OLED_TestPolyline();

                break;


            case OLED_STATE_MY_SCREEN:

                OLED_TestMyScreen();

                break;


            case OLED_STATE_CCE:

            default:

                OLED_TestCCE();

                break;
        }


        OLED_UpdateScreen();

        HAL_Delay(50);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE1
    );


    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSE;

    RCC_OscInitStruct.HSEState =
        RCC_HSE_BYPASS;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_ON;

    RCC_OscInitStruct.PLL.PLLSource =
        RCC_PLLSOURCE_HSE;

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

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV4;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV2;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_5) != HAL_OK)
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


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}

#endif
