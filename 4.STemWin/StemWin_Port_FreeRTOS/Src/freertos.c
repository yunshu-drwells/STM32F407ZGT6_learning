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

#include "touch.h"  //tp_dev
#include "norflash.h"  //norflash_init

#include "dht11.h"  //dht11_init、dht11_read_data

#include "lsens.h"  //lsens_init

#include "GUI.h"  //GUI_TOUCH_Exec
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

uint16_t id;  //norflash id

uint8_t t = 0; 
uint16_t temperature; 
uint16_t humidity;

uint16_t adcx;
/* USER CODE END Variables */
osThreadId WebServerHandle;
osThreadId TouchHandle;
osThreadId IOTHandle;
osThreadId GUIHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void MainTask(void);   
/* USER CODE END FunctionPrototypes */

void WebServer_Task(void const * argument);
void Touch_Task(void const * argument);
void IOT_Task(void const * argument);
void GUI_Task(void const * argument);

extern void MX_LWIP_Init(void);
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
  /* definition and creation of WebServer */
  osThreadDef(WebServer, WebServer_Task, osPriorityHigh, 0, 256);
  WebServerHandle = osThreadCreate(osThread(WebServer), NULL);

  /* definition and creation of Touch */
  osThreadDef(Touch, Touch_Task, osPriorityAboveNormal, 0, 256);
  TouchHandle = osThreadCreate(osThread(Touch), NULL);

  /* definition and creation of IOT */
  osThreadDef(IOT, IOT_Task, osPriorityNormal, 0, 256);
  IOTHandle = osThreadCreate(osThread(IOT), NULL);

  /* definition and creation of GUI */
  osThreadDef(GUI, GUI_Task, osPriorityBelowNormal, 0, 2048);
  GUIHandle = osThreadCreate(osThread(GUI), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_WebServer_Task */
/**
  * @brief  Function implementing the WebServer thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_WebServer_Task */
void WebServer_Task(void const * argument)
{
    
                 
  /* init code for LWIP */
  MX_LWIP_Init();

  /* USER CODE BEGIN WebServer_Task */
	taskENTER_CRITICAL();           /* 杩涘叆涓寸晫鍖? */
	//SPI Flash
	norflash_init();  //鍒濆鍖杗orflash
	id = norflash_read_id();
	printf("norflash id is %d\n", id);
	
	taskEXIT_CRITICAL();            /* 閫?鍑轰复鐣屽尯 */
  /* Infinite loop */
  for(;;)
  {

		osDelay(10);
  }
  /* USER CODE END WebServer_Task */
}

/* USER CODE BEGIN Header_Touch_Task */
/**
* @brief Function implementing the Touch thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Touch_Task */
void Touch_Task(void const * argument)
{
  /* USER CODE BEGIN Touch_Task */
	taskENTER_CRITICAL();           /* 杩涘叆涓寸晫鍖? */
	delay_init(168);                        /* 寤舵椂鍒濆鍖? */
	lcd_init();                             /* 鍒濆鍖朙CD */
  g_point_color = RED;
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* 灏哃CD ID鎵撳嵃鍒發cd_id鏁扮粍 */
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1浜?
	lcd_set_backlight_by_pwm(0xFF); // 璁剧疆鍗犵┖姣斾负255锛屽紑鍚儗鍏夋渶浜?
	lcd_clear(colors[color_index]);  //娓呭睆
	
	//screen touch init
	uint8_t res = tp_dev.init();                      // 瑙︽懜灞忓垵濮嬪寲
	if(!res){
		printf("LCD Touch init Successful!\n");
	}
	while (dht11_init())    /* DHT11初始化 */
	{
			printf("DHT11 Error !\n");
			delay_ms(200);
	}
	
	lsens_init();                           /* 初始化光敏传感器 */
	
	//emwin_test_touch();  //emWin鍧愭爣鑾峰彇
	taskEXIT_CRITICAL();            /* 閫?鍑轰复鐣屽尯 */
  /* Infinite loop */
  for(;;)
  {
		if (t % 5 == 0) /* 每100ms读取一次 */ { 
				dht11_read_data(&temperature, &humidity); /* 读取温湿度值 */
				
				printf("temperature: %d.%d\n", temperature>>8, (temperature & 0xFF));/* 显示温度 */ 
				printf("humidity: %d.%d", humidity>>8, (humidity & 0xFF)); /* 显示湿度 */ 
		}
		if(t % 10 == 0) /* 每200ms读取一次 */{ 
				adcx = lsens_get_val();                                 /* 获取ADC值 */
				printf("bright:%d\n", adcx);
		}
		t++; 
		//瑙︽懜灞忛渶瑕佽疆璇㈡娴嬶紝鍚﹀垯emWin娌℃湁鍔炴硶瑙﹀彂浜嬩欢
		GUI_TOUCH_Exec();
    osDelay(20);
  }
  /* USER CODE END Touch_Task */
}

/* USER CODE BEGIN Header_IOT_Task */
/**
* @brief Function implementing the IOT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IOT_Task */
void IOT_Task(void const * argument)
{
  /* USER CODE BEGIN IOT_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END IOT_Task */
}

/* USER CODE BEGIN Header_GUI_Task */
/**
* @brief Function implementing the GUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GUI_Task */
void GUI_Task(void const * argument)
{
  /* USER CODE BEGIN GUI_Task */
	MainTask();
  /* Infinite loop */
	/*
  for(;;)
  {
    osDelay(1);
  }
	*/
  /* USER CODE END GUI_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
