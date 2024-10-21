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
#include "crc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "lcd.h"
#include "touch.h"
#include "exti.h"
#include "norflash.h"
#include "sram.h"
#include "mymalloc.h"

#include "GUI.h"  //GUI_PID_STATE
#include "GUIDEMO.h" //GUIDEMO_Main
#include "WM.h"
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

/* 10个触控点的颜色(电容触摸屏用) */
const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW, MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

uint8_t IS_KEY0_PRES = 0;  /* KEY0是否按下 */
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
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_CRC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	delay_init(168);                        /* 延时初始化 */
	lcd_init();                             /* 初始化LCD */
  g_point_color = RED;
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* 将LCD ID打印到lcd_id数组 */
	//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1亮
	lcd_set_backlight_by_pwm(0xFF); // 设置占空比为255,开启背光最亮
	
	cur_brightness = lcd_get_backlight_by_pwm(); //获取当前背光值
	sprintf((char*)str, "current brightness:%3u", cur_brightness);

	lcd_clear(colors[color_index]);  //清屏
	lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
	lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
	lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
	lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* 显示LCD ID */
	lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示亮度 */
	lcd_show_string(10, 170, 200, 16, 16, "KEY0:Test Touch!", RED);
	lcd_show_string(10, 190, 200, 16, 16, "KEY1:Test norflash!", RED);
	lcd_show_string(10, 210, 200, 16, 16, "KEY2:Test SRAM!", RED);
	lcd_show_string(10, 230, 280, 16, 16, "KEY_UP:Test emWin get touch location!", RED);
	
	//screen touch init
	uint8_t res = tp_dev.init();                      // 触摸屏初始化

	lcd_fill(10, 80, 240, 80, WHITE);    /* 清除显示 */
	lcd_show_string(10, 80, 200, 24, 24, "TOUCH TEST", RED);
	
	if(!res){
		lcd_show_string(10, 250, 200, 16, 16, "LCD Touch init Successful!", RED);
	}

	norflash_init();  //初始化norflash
	uint16_t id = norflash_read_id();
	printf("norflash id is %d\n", id);
	lcd_show_string(10, 270, 200, 16, 16, "norflash id is", RED);
	lcd_show_xnum(10+120, 270, id, 5, 16, 0, BLUE);
	sprintf((char *)datatemp, "%s", (char *)g_text_buf);
	while ((id == 0) || (id == 0xFFFF)) /* 检测不到FLASH芯片 */ { 
		printf("FLASH Check Failed!\n");
		lcd_fill(10, 270, 240, 80, WHITE);    /* 清除显示 */
		lcd_show_string(10, 270, 200, 16, 16, "FLASH Check Failed!", RED);
		HAL_Delay(500); 
		printf("Please Check! \n");
		lcd_fill(10, 270, 240, 80, WHITE);    /* 清除显示 */
		lcd_show_string(10, 270, 200, 16, 16, "Please Check!", RED);
		HAL_Delay(500); 
	}
	
	sram_init();                        /* SRAM初始化 */
    
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
	my_mem_init(SRAMEX);                /* 初始化外部SRAM内存池 */
	my_mem_init(SRAMCCM);               /* 初始化内部CCM内存池 */
	lcd_show_string(10, 290, 200, 16, 16, "SRAM init Successful!", RED);

	//启动TIM6定时器中断
	if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
	{
			Error_Handler();
			printf("TIM6 Start IT ERROR!\n");
	}
	lcd_display_dir(1);  //设置横屏显示
	GUI_Init();  //emWin初始化
	GUIDEMO_Main();  //GUIDEMO入口

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* LED0闪烁 */
		HAL_Delay(300);
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
/**
 * @brief       清空屏幕并在右上角显示 "RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
		lcd_show_string(0, 0, 200, 16, 16, "EXIT", RED); /* 显示清屏区域 */
}

/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
        return;

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1; /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0; /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0; /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x; /* 选取基本增量坐标轴 */
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       电阻触摸屏测试函数
 * @param       无
 * @retval      无
 */
void rtp_test(void)
{
    uint8_t key;
    uint8_t i = 0;

    while (1)
    {
        key = IS_KEY0_PRES;
        tp_dev.scan(0);

        if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
        {
            if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
            {
                if (tp_dev.x[0] > (lcddev.width - 24) && tp_dev.y[0] < 16)
                {
                    load_draw_dialog(); /* 清除 */
                }
                else
                {
                    tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);   /* 画点 */
                }
            }
        }
        else 
        {
            delay_ms(10);       /* 没有按键按下的时候 */
        }

        if (key == 1)   /* KEY0按下,则执行校准程序 */
        {
            lcd_clear(WHITE);   /* 清屏 */
            tp_adjust();        /* 屏幕校准 */
            tp_save_adjust_data();
            load_draw_dialog();
        }

        i++;

        if (i % 20 == 0)HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* LED0闪烁 */
    }
}

/**
 * @brief       电容触摸屏测试函数
 * @param       无
 * @retval      无
 */
void ctp_test(void)
{
		printf("ctp_test!\n");
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];        /* 最后一次的数据 */
    uint8_t maxp = 5;

    if (lcddev.id == 0x1018)maxp = 10;

    while (IS_KEY0_PRES)
    {
        tp_dev.scan(0);

        for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)  /* 坐标在屏幕范围内 */
                {
                    if (lastpos[t][0] == 0xFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]);  /* 画线 */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];

                    if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)  //按RST
                    {
                        load_draw_dialog(); /* 清除 */
                    }
										
										if (tp_dev.x[t] < 32 && tp_dev.y[t] < 20) //按EXIT 
                    {
												lcd_clear(colors[color_index]);  //清屏
												lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
												lcd_show_string(10, 80, 240, 24, 24, "TOUCH TEST", RED);
												lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
												lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* 显示LCD ID */
												lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示亮度 */
												lcd_show_string(10, 170, 200, 16, 16, "KEY0:Test Touch!", RED);
												lcd_show_string(10, 190, 200, 16, 16, "KEY1:Test norflash!", RED);
												lcd_show_string(10, 210, 200, 16, 16, "KEY2:Test SRAM!", RED);
                        return;  //退出测试死循环
                    }
                }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }

        delay_ms(5);
        i++;

        if (i % 20 == 0)
        {
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* LED0闪烁 */
        }
    }
}

void emwin_test_touch(void){
	GUI_PID_STATE State;
	GUI_Init();
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font20_ASCII);
	GUI_SetColor(GUI_YELLOW);
	GUI_Clear();
	while(1){
		//执行触摸笔检测
		GUI_TOUCH_Exec();
		//获取触摸笔状态值
		GUI_TOUCH_GetState(&State);
		//是否按下
		if(State.Pressed){
			//打印触摸笔坐标信息
			GUI_DispStringAt("X:", 0, 0);
			GUI_DispDecAt(State.x, 32, 0, 4);
			GUI_DispStringAt("Y:", 0, 24);
			GUI_DispDecAt(State.y, 32, 24, 4);
			
			GUI_DispStringAt("EXIT", lcddev.width - 40, 0);
		}

		delay_ms(10);
		if (State.x > lcddev.width - 40 && State.y < 20) //按EXIT 
		{
				GUI_SetBkColor(GUI_WHITE);
				GUI_Clear();
				lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
				lcd_show_string(10, 80, 240, 24, 24, "TOUCH TEST", RED);
				lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
				lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* 显示LCD ID */
				lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示亮度 */
				lcd_show_string(10, 170, 200, 16, 16, "KEY0:Test Touch!", RED);
				lcd_show_string(10, 190, 200, 16, 16, "KEY1:Test norflash!", RED);
				lcd_show_string(10, 210, 200, 16, 16, "KEY2:Test SRAM!", RED);
				return;  //退出测试死循环
		}
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
