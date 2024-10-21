/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "delay.h"
#include "lcd.h"
#include "stdio.h"  //sprintf

#include "norflash.h"

#include "diskio.h"
#include "fatfs.h"  //SDFatFS、USERFatFS、format_flash
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
/* USER CODE BEGIN Variables */
//lcd
extern uint8_t lcd_id[12]; //main.c
extern uint16_t colors[];  //main.c
extern uint8_t color_index;  //main.c

const uint8_t g_text_buf[] = {"STM32 SPI TEST!"}; /* 要写到FLASH的字符串数组 */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长�? */
uint8_t datatemp[TEXT_SIZE]; 


unsigned long recv = 0;

HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;   /* SD卡信息结构体 */
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void init_disks(){
	while(disk_initialize(0)){  //返回0，表示SD初始化成�?
		lcd_show_string(30, 140, 200, 16, 16, "SD Card Error!", RED);
		delay_ms(500);
		lcd_show_string(30, 140, 200, 16, 16, "Please Check! ", RED);
		delay_ms(500);
	}
	lcd_show_string(30, 140, 200, 16, 16, "SD Card init OK! ", BLUE);
	disk_ioctl(0, 1, (void*)&recv);
	printf("SD sector count: %d\n", (int)recv);
	lcd_show_string(30, 172, 200, 16, 16, "SD sector count:", BLUE);
	lcd_show_num(30 + 10 * 14, 172, (int)recv , 8, 16, BLUE); 
	
	while(disk_initialize(1)){  //返回0，表示Norflash初始化成�?
		lcd_show_string(30, 204, 200, 16, 16, "Noflash Error!", RED);
		delay_ms(500);
		lcd_show_string(30, 204, 200, 16, 16, "Please Check! ", RED);
		delay_ms(500);
	}
	lcd_show_string(30, 236, 200, 16, 16, "Noflash init OK! ", BLUE);
	disk_ioctl(1, 1, (void*)&recv);
	printf("noflash sector count: %d\n", (int)recv);
	lcd_show_string(30, 268, 200, 16, 16, "Noflash sector count:", BLUE);
	lcd_show_num(30 + 10 * 14, 268, (int)recv , 8, 16, BLUE); 
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
    
                 
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */
	taskENTER_CRITICAL();           /* 进入临界�? */
	delay_init(168);                    // 延时初始�?
	lcd_init();                             // 初始化LCD
	
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // 将LCD ID打印到lcd_id数组
	lcd_clear(colors[color_index]);  //清屏

	lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
	lcd_show_string(10, 47, 220, 24, 24, "Mem Manage", RED);
	lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
	
	lcd_show_string(30, 108, 200, 16, 16, "FreeRTOS with FATFS Test!", RED);

	norflash_init();  //初始化norflash
	uint16_t id = norflash_read_id();
	printf("norflash id is %d\n", id);
	sprintf((char *)datatemp, "%s", (char *)g_text_buf);
	while ((id == 0) || (id == 0xFFFF)) /* �?测不到FLASH芯片 */ { 
		printf("FLASH Check Failed!\n"); 
		HAL_Delay(500); 
		printf("Please Check! \n"); 
		HAL_Delay(500); 
	}

	//初始化SD卡和norflash
	//init_disks();

	
	/*
	// 挂载文件系统
	if (f_mount(&USERFatFS, "1:", 1) == FR_OK) {
		printf("Norflash mount succeed!\n");
	} else {
		printf("Norflash mount failed!\n");
	}
	uint8_t res = 0;
	res = f_mount(&SDFatFS, "0:", 1);        // 挂载SD
	printf("f_mount sd res:%u\n", res);
	if (res == FR_OK) {
		printf("SD mount succeed!\n");
	} else {
		printf("SD mount failed!\n");
	}
	*/
	

	printf("fuck\n");
	//format_flash();
	//挂载SD卡和norflash
	//fmout_disks();
	
	taskEXIT_CRITICAL();            /* �?出临界区 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
