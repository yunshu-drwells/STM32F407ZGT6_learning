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
#include "dma.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "norflash.h"
#include "exti.h"
#include "sdio_sdcard.h"

#include "delay.h"
#include "sram.h"
#include "mymalloc.h"

#include "usmart.h"
#include "exfuns.h"  //fs、exfuns_init
#include "diskio.h"  //disk_initialize
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

HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;   /* SD卡信息结构体 */
uint8_t res = 0;
uint32_t total_space, free_space;
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
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_SDIO_SD_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	delay_init(168);
	
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
	lcd_show_string(10, 170, 200, 16, 16, "KEY0:Test SD!", RED);
	lcd_show_string(10, 190, 200, 16, 16, "KEY1:Test norflash!", RED);
	lcd_show_string(10, 210, 200, 16, 16, "KEY2:Test SRAM!", RED);
	
	norflash_init();  //初始化norflash
	uint16_t id = norflash_read_id();
	printf("norflash id is %d\n", id);
	sprintf((char *)datatemp, "%s", (char *)g_text_buf);
	while ((id == 0) || (id == 0xFFFF)) /* 检测不到FLASH芯片 */ { 
		printf("FLASH Check Failed!\n"); 
		HAL_Delay(500); 
		printf("Please Check! \n"); 
		HAL_Delay(500); 
	}
	
	sram_init();                        /* SRAM初始化 */
    
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
	my_mem_init(SRAMEX);                /* 初始化外部SRAM内存池 */
	my_mem_init(SRAMCCM);               /* 初始化内部CCM内存池 */
	
	usmart_dev.init(84);                    /* USMART初始化 */
	//开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
	

	while (sd_init())   // 检测不到SD卡 
	{
			lcd_show_string(30, 330, 200, 16, 16, "SD Card Error!", RED);
			delay_ms(500);
			lcd_show_string(30, 330, 200, 16, 16, "Please Check! ", RED);
			delay_ms(500);
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); // LED1闪烁 
	}

	
  /*
	printf("SDIO test!\n");
	HAL_SD_GetCardInfo(&hsd, &g_sd_card_info_handle);  //初始化完成后获取sd卡信息
	printf("CardType:%u\n", g_sd_card_info_handle.CardType);  //卡类型
	printf("CardVersion:%u\n", g_sd_card_info_handle.CardVersion);  //卡版本
	printf("Class:%u\n", g_sd_card_info_handle.Class);  //卡类型
	printf("BlockSize:%u\n", g_sd_card_info_handle.BlockSize);  //块大小
	printf("LogBlockNbr:%u\n", g_sd_card_info_handle.LogBlockNbr);  //以块为单位的卡的逻辑容量
	printf("LogBlockSize:%u\n", g_sd_card_info_handle.LogBlockSize);  //指定逻辑块大小（以字节为单位）
	*/

	exfuns_init();                  /* 为fatfs相关变量申请内存 */
	
	res = f_mount(fs[0], "0:", 1);        // 挂载SD卡 
	//当调用 `f_mount` 函数来挂载文件系统时，`disk_initialize` 会被间接调用，以确保底层存储设备（如SD卡）已准备好进行读写操作
	/*
	`f_mount` 函数用于在 FatFs 模块中注册或注销文件系统对象。它的返回值如下：
	- FR_OK = 0,				// (0) Succeeded 操作成功
	- FR_DISK_ERR,			// (1) A hard error occurred in the low level disk I/O layer  低级磁盘 I/O 错误
	- FR_INT_ERR,				// (2) Assertion failed 
	- FR_NOT_READY,			// (3) The physical drive cannot work 
	- FR_NO_FILE,				// (4) Could not find the file 
	- FR_NO_PATH,				// (5) Could not find the path 
	- FR_INVALID_NAME,		// (6) The path name format is invalid 
	- FR_DENIED,				// (7) Access denied due to prohibited access or directory full 
	- FR_EXIST,				// (8) Access denied due to prohibited access 
	- FR_INVALID_OBJECT,		// (9) The file/directory object is invalid 
	- FR_WRITE_PROTECTED,		// (10) The physical drive is write protected 
	- FR_INVALID_DRIVE,		// (11) The logical drive number is invalid  无效的驱动器号
	- FR_NOT_ENABLED,			// (12) The volume has no work area 卷未启用
	- FR_NO_FILESYSTEM,		// (13) There is no valid FAT volume 没有有效的文件系统
	- FR_MKFS_ABORTED,		// (14) The f_mkfs() aborted due to any problem 
	- FR_TIMEOUT,				// (15) Could not get a grant to access the volume within defined period 
	- FR_LOCKED,				// (16) The operation is rejected according to the file sharing policy 
	- FR_NOT_ENOUGH_CORE,		// (17) LFN working buffer could not be allocated 
	- FR_TOO_MANY_OPEN_FILES,	// (18) Number of open files > FF_FS_LOCK 
	- FR_INVALID_PARAMETER	// (19) Given parameter is invalid 
	*/
	printf("f_mount sd res:%u\n", res);
	if(FR_OK == res){
		lcd_show_string(30, 330, 200, 16, 16, "SD Disk Mount Successed!", RED);     /* FLASH格式化成功 */
	}
	if (res == 0X0D) {               /* SD磁盘,FAT文件系统错误,重新格式化SD */
			printf("sd fs error!\n");
			lcd_show_string(30, 330, 200, 16, 16, "SD Disk Formatting...", RED);         /* 格式化SD */
			res = f_mkfs("0:", 0, 0, FF_MAX_SS);                                            /* 格式化SD,0:,盘符;0,使用默认格式化参数 */

			if (res == 0){
					f_setlabel((const TCHAR *)"0:ALIENTEK_SD");                                    /* 设置SD磁盘的名字为：ALIENTEK_SD */
					lcd_show_string(30, 330, 200, 16, 16, "SD Disk Format Finish", RED);     /* 格式化完成 */
			}	else	{
					lcd_show_string(30, 330, 200, 16, 16, "SD Disk Format Error ", RED);     /* 格式化失败 */
			}
			delay_ms(1000);
	}

	//lcd_fill(30, 150, 240, 150 + 16, WHITE);    /* 清除显示 */
	
	res = f_mount(fs[1], "1:", 1);  /* 挂载FLASH */
	printf("f_mount flash res:%u\n", res);
	if(FR_OK == res){
		lcd_show_string(30, 350, 200, 16, 16, "Flash Disk Mount Successed!", RED);     /* FLASH格式化成功 */
	}
	if (res == 0X0D) {                /* FLASH磁盘,FAT文件系统错误,重新格式化FLASH */
			printf("flash fs error!\n");
			lcd_show_string(30, 350, 200, 16, 16, "Flash Disk Formatting...", RED);         /* 格式化FLASH */
			res = f_mkfs("1:", 0, 0, FF_MAX_SS);                                            /* 格式化FLASH,1:,盘符;1,使用默认格式化参数 */

			if (res == 0)	{
					f_setlabel((const TCHAR *)"1:ALIENTEK_FLASH");                                    /* 设置Flash磁盘的名字为：ALIENTEK_FLASH */
					lcd_show_string(30, 350, 200, 16, 16, "Flash Disk Format Finish", RED);     /* 格式化完成 */
			}	else {
					lcd_show_string(30, 350, 200, 16, 16, "Flash Disk Format Error ", RED);     /* 格式化失败 */
			}
			delay_ms(1000);
	}
	while (exfuns_get_free("0", &total_space, &free_space)) /* 得到SD卡的总容量和剩余容量 */
	{
			lcd_show_string(30, 370, 200, 16, 16, "SD Card Fatfs Error!", RED);
			delay_ms(200);
			lcd_fill(30, 370, 240, 150 + 16, WHITE);/* 清除显示 */
			delay_ms(200);
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); /* LED1闪烁 */
	}

	lcd_show_string(30, 370, 200, 16, 16, "FATFS OK!", BLUE);
	lcd_show_string(30, 390, 200, 16, 16, "SD Total Size:     MB", BLUE);
	lcd_show_string(30, 410, 200, 16, 16, "SD Free Size:      MB", BLUE);
	lcd_show_num(30 + 8 * 14, 390, total_space >> 10, 5, 16, BLUE);               /* 显示SD卡总容量 MB */
	lcd_show_num(30 + 8 * 14, 410, free_space >> 10, 5, 16, BLUE);                /* 显示SD卡剩余容量 MB */
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
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
/* LED状态设置函数 */
void led_set(uint8_t sta)
{
	sta ? \
                      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
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
