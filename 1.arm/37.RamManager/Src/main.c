/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "exti.h"
#include "sram.h"
#include "mymalloc.h"  //my_mem_init
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
uint8_t lcd_id[12];
uint16_t colors[12] = {WHITE, BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BRRED, GRAY, LGRAY, BROWN};uint8_t color_index = 0;
uint8_t cur_brightness;
uint8_t* str[23];

uint8_t paddr[20];                  /* 存放P Addr:+p地址的ASCII值 */
uint16_t memused = 0;
uint8_t i = 0;
uint8_t *p = 0;
uint8_t *tp = 0;
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
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();                             /* 初始化LCD */
  g_point_color = RED;
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* 将LCD ID打印到lcd_id数组 */
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1亮
	lcd_set_backlight_by_pwm(0xFF); // 设置占空比为255,开启背光最亮
	
	cur_brightness = lcd_get_backlight_by_pwm(); //获取当前背光值
	sprintf((char*)str, "current brightness:%3u", cur_brightness);
	
	sram_init();                        /* SRAM初始化 */
    
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
	my_mem_init(SRAMEX);                /* 初始化外部SRAM内存池 */
	my_mem_init(SRAMCCM);               /* 初始化内部CCM内存池 */

	lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
	lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
	lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
	lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* 显示LCD ID */
	lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示亮度 */
	
	lcd_show_string(10, 170, 200, 16, 16, "KEY0:SRAM malloc!", RED);
	lcd_show_string(10, 190, 200, 16, 16, "KEY1:SRAM free!", RED);
	lcd_show_string(10, 210, 200, 16, 16, "KEY2:Change SRAM!", RED);
	
	lcd_show_string(60, 226, 200, 16, 16, " SRAMIN ", BLUE);
	
	lcd_show_string(30, 242, 200, 16, 16, "SRAMIN   USED:", BLUE);
	lcd_show_string(30, 258, 200, 16, 16, "SRAMCCM  USED:", BLUE);
	lcd_show_string(30, 274, 200, 16, 16, "SRAMEX   USED:", BLUE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (tp != p){
				tp = p;
				sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)tp);
				lcd_show_string(30, 306, 209, 16, 16, (char *)paddr, BLUE); /* 显示p的地址 */

				if (p){
						lcd_show_string(30, 290, 280, 16, 16, (char *)p, BLUE); /* 显示P的内容 */
				}	else {
						lcd_fill(30, 290, 209, 296, WHITE);                     /* p=0,清除显示 */
				}
		}

		//delay_ms(10);
		HAL_Delay(10);
		i++;

		if ((i % 20) == 0) {
				memused = my_mem_perused(SRAMIN);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 242, 200, 16, 16, (char *)paddr, BLUE);   /* 显示内部内存使用率 */
				
				memused = my_mem_perused(SRAMCCM);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 258, 200, 16, 16, (char *)paddr, BLUE);   /* 显示CCM内存使用率 */
				
				memused = my_mem_perused(SRAMEX);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 274, 200, 16, 16, (char *)paddr, BLUE);   /* 显示外部内存使用率 */
				
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* LED0闪烁 */
				HAL_Delay(300);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
