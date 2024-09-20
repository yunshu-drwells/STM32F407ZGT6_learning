#include "main.h"  //LEDx��KEYx��BEEP���Ŷ���
#include "usart.h"  //fputc
#include "stm32f4xx_hal_gpio.h"  //HAL_GPIO_TogglePin��HAL_GPIO_ReadPin

/**
 * @brief       �жϷ����������Ҫ��������
                ��HAL�������е��ⲿ�жϷ�����������ô˺���
 * @param       GPIO_Pin:�ж����ź�
 * @retval      ��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(10);      /* ���� */

		switch(GPIO_Pin){
			case KEY0_Pin:
				/*key0*/
				printf("KEY0 pressed!\n");
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);  //LED0��ת

				break;
			case KEY1_Pin:
				printf("KEY1 pressed!\n");
				/*key1*/
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);  //LED1��ת					

				break;
			case KEY2_Pin:
				printf("KEY2 pressed!\n");
				/*key2*/
				if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET){
					HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);  //LED0��ת
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);  //LED1��ת
					//��������50ms
					HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  /*��������*/
					HAL_Delay(50);
					HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  /*����������*/
				}
				break;
			default:
				break;
		}
}
