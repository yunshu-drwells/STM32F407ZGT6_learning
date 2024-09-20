#include "main.h"  //LEDx、KEYx、BEEP引脚定义
#include "usart.h"  //fputc
#include "stm32f4xx_hal_gpio.h"  //HAL_GPIO_TogglePin、HAL_GPIO_ReadPin

/**
 * @brief       中断服务程序中需要做的事情
                在HAL库中所有的外部中断服务函数都会调用此函数
 * @param       GPIO_Pin:中断引脚号
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(10);      /* 消抖 */

		switch(GPIO_Pin){
			case KEY0_Pin:
				/*key0*/
				printf("KEY0 pressed!\n");
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);  //LED0翻转

				break;
			case KEY1_Pin:
				printf("KEY1 pressed!\n");
				/*key1*/
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);  //LED1翻转					

				break;
			case KEY2_Pin:
				printf("KEY2 pressed!\n");
				/*key2*/
				if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET){
					HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);  //LED0翻转
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);  //LED1翻转
					//蜂鸣器响50ms
					HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  /*蜂鸣器响*/
					HAL_Delay(50);
					HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  /*蜂鸣器不响*/
				}
				break;
			default:
				break;
		}
}
