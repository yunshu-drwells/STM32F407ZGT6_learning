#include "exti.h"
#include "usart.h"  //fputc

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
		printf("HAL_UART_TxCpltCallback called\n");
		if(huart->Instance == USART1)
		{
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);/*点亮LED0*/
				HAL_Delay(50);  /*SysTick中断优先级高于UART*/
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);/*熄灭LED0*/
				printf("send finish\n");
				
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1)
		{
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);/*点亮LED1*/
				HAL_Delay(50);  /*SysTick中断优先级高于UART*/
				printf("data: st[0]:%x st[1]:%x\n", st[0], st[1]);
				HAL_UART_Receive_IT(huart, st, 2);
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);/*熄灭LED1*/
				printf("receive finish\n");
		}
}
