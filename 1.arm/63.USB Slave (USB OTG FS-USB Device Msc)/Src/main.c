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
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "lcd.h"
#include "norflash.h"  //norflash_init norflash_read_id
#include "exti.h"

#include "sdio_sdcard.h"
#include "sram.h"
#include "mymalloc.h"  //my_mem_init 

#include "usbd_storage.h"  //USBD_DISK_fops  (USBD_Storage_Interface_fops_FS)
#include "usbd_core.h"  //USBD_Init USBD_RegisterClass USBD_MSC_RegisterStorage USBD_Start
#include "usbd_desc.h"  //MSC_Desc
#include "usbd_msc.h"  //USBD_MSC 
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

HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;   /* SD����Ϣ�ṹ�� */
uint64_t card_capacity;             /* SD������ */

USBD_HandleTypeDef USBD_Device;             /* USB Device����ṹ�� */
extern volatile uint8_t g_usb_state_reg;    /* USB״̬ */
extern volatile uint8_t g_device_state;     /* USB���� ��� */
uint8_t usb_sta;  //usb״̬
uint8_t offline_cnt = 0;
uint8_t tct = 0;
uint8_t device_sta;  //�豸״̬
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
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
	delay_init(168);  //��ʼ���Զ�����ʱ(���ж�)
	lcd_init();                             /* ��ʼ��LCD */
  g_point_color = RED;
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* ��LCD ID��ӡ��lcd_id���� */
	//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1��
	lcd_set_backlight_by_pwm(0xFF); // ����ռ�ձ�Ϊ255,������������
	
	cur_brightness = lcd_get_backlight_by_pwm(); //��ȡ��ǰ����ֵ
	sprintf((char*)str, "current brightness:%3u", cur_brightness);

	lcd_clear(colors[color_index]);  //����
	lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
	lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
	lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
	lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* ��ʾLCD ID */
	lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* ��ʾ���� */
	lcd_show_string(10, 170, 200, 16, 16, "KEY0:Test SD!", RED);
	lcd_show_string(10, 190, 200, 16, 16, "KEY1:Test norflash!", RED);
	lcd_show_string(10, 210, 200, 16, 16, "KEY2:Test SRAM!", RED);
	
	norflash_init();  //��ʼ��norflash
	uint16_t id = norflash_read_id();
	printf("norflash id is %d\n", id);
	sprintf((char *)datatemp, "%s", (char *)g_text_buf);
	while ((id == 0) || (id == 0xFFFF)) /* ��ⲻ��FLASHоƬ */ { 
		printf("FLASH Check Failed!\n"); 
		HAL_Delay(500); 
		printf("Please Check! \n"); 
		HAL_Delay(500); 
		lcd_show_string(30, 330, 200, 16, 16, "SPI FLASH Error!", RED); /* ���SPI FLASH���� */
	}
	lcd_show_string(30, 330, 200, 16, 16, "SPI FLASH Size:12MB", RED);
	printf("SPI FLASH Size:12MB\n");
	//sd��ʼ��
	if (sd_init()){  /* ��ʼ��SD�� */
			lcd_show_string(30, 350, 200, 16, 16, "SD Card Error!", RED);           /* ���SD������ */
	}	else {            /* SD ������ */
			lcd_show_string(30, 350, 200, 16, 16, "SD Card Size:     MB", RED);
			card_capacity = (uint64_t)(g_sd_card_info_handle.LogBlockNbr) * (uint64_t)(g_sd_card_info_handle.LogBlockSize);     /* ����SD������ */
			lcd_show_num(134, 350, card_capacity >> 20, 5, 16, RED);                /* ��ʾSD������ */
	}
	/*
	printf("SDIO test!\n");
	HAL_SD_GetCardInfo(&hsd, &g_sd_card_info_handle);  //��ʼ����ɺ��ȡsd����Ϣ
	printf("CardType:%u\n", g_sd_card_info_handle.CardType);  //������
	printf("CardVersion:%u\n", g_sd_card_info_handle.CardVersion);  //���汾
	printf("Class:%u\n", g_sd_card_info_handle.Class);  //������
	printf("BlockSize:%u\n", g_sd_card_info_handle.BlockSize);  //���С
	printf("LogBlockNbr:%u\n", g_sd_card_info_handle.LogBlockNbr);  //�Կ�Ϊ��λ�Ŀ����߼�����
	printf("LogBlockSize:%u\n", g_sd_card_info_handle.LogBlockSize);  //ָ���߼����С�����ֽ�Ϊ��λ��
	*/
	/*
	4G��SD card��g_sd_card_info_handle.BlockSize��g_sd_card_info_handle.LogBlockSize����512����g_sd_card_info_handle.BlockNbrֵ��7626752

	��������4G/512Bytes=4*1024*1024*1024/512Bytes = 8388608��������˵g_sd_card_info_handle.BlockNbr��Ӧ����8192��

	���洢�����ԣ�4G/512Bytes=4*1000*1000*1000/512Bytes = 7812500����ȥ�ļ�ϵͳ�ͱ����飬��ʣ7626752
	*/
	
	sram_init();                        /* SRAM��ʼ�� */
    
	my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */
	my_mem_init(SRAMEX);                /* ��ʼ���ⲿSRAM�ڴ�� */
	my_mem_init(SRAMCCM);               /* ��ʼ���ڲ�CCM�ڴ�� */
	
	USBD_Init(&USBD_Device, &MSC_Desc, DEVICE_FS);       /* ��ʼ��USB */
	USBD_RegisterClass(&USBD_Device, &USBD_MSC);        /* ����� */
	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);    /* ΪMSC����ӻص����� */
	//USBD_StatusTypeDef status = USBD_Start(&USBD_Device);
	USBD_Start(&USBD_Device);                           /* ����USB */
	//printf("USBD_Start returned: %d\n", status);

	delay_ms(1800);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* LED0��˸ */
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
