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

/* USER CODE END Variables */
osThreadId startTaskHandle;
osThreadId task1TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void start_task(void const * argument);
void task1(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of startTask */
  osThreadDef(startTask, start_task, osPriorityAboveNormal, 0, 128);
  startTaskHandle = osThreadCreate(osThread(startTask), NULL);

  /* definition and creation of task1Task */
  osThreadDef(task1Task, task1, osPriorityHigh, 0, 128);
  task1TaskHandle = osThreadCreate(osThread(task1Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_start_task */
/**
  * @brief  Function implementing the startTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_start_task */
void start_task(void const * argument)
{
    
    
    

  /* USER CODE BEGIN start_task */
	taskENTER_CRITICAL();           /* 进入临界区 */
	delay_init(168);                    // 延时初始化
	lcd_init();                             // 初始化LCD
	
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // 将LCD ID打印到lcd_id数组
	lcd_clear(colors[color_index]);  //清屏

	lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
	lcd_show_string(10, 47, 220, 24, 24, "Mem Manage", RED);
	lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
	
	lcd_show_string(30, 118, 200, 16, 16, "Total Heap Mem:      Bytes", RED);
	lcd_show_string(30, 139, 200, 16, 16, "Free Heap Mem:      Bytes", RED);
	lcd_show_string(30, 160, 200, 16, 16, "Malloc Addr:", RED);
	taskEXIT_CRITICAL();            /* 退出临界区 */
  /* Infinite loop */
  for(;;)
  {

    osDelay(100);
  }
  /* USER CODE END start_task */
}

/* USER CODE BEGIN Header_task1 */
/**
* @brief Function implementing the task1Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task1 */
void task1(void const * argument)
{
  /* USER CODE BEGIN task1 */
	uint8_t key         = 0;
	uint8_t *buf        = NULL;
	size_t  free_size   = 0;
  /* Infinite loop */
  for(;;)
  {
		key = key_scan(0);    
		switch (key) {
				case KEY0_PRES: {                                                /* 申请内存和使用内存 */
						buf = pvPortMalloc(30);
						sprintf((char *)buf, "0x%p", buf);
						lcd_show_string(130, 160, 200, 16, 16, (char *)buf, BLUE);
						break;
				}
				case KEY1_PRES: {                                                /* 释放内存 */
						if (NULL != buf) {
								vPortFree(buf);
								buf = NULL;
						}
						break;
				}
				default:
						break;
		}
		
		lcd_show_xnum(114+35, 118, configTOTAL_HEAP_SIZE, 5, 16, 0, BLUE);     /* 显示总内存大小 */
		free_size = xPortGetFreeHeapSize();                                 /* 获取内存剩余大小 */
		lcd_show_xnum(114+30, 139, free_size, 5, 16, 0, BLUE);                 /* 显示剩余内存大小 */
    osDelay(1);
  }
  /* USER CODE END task1 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
