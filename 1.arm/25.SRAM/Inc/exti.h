#include "main.h"  //LEDx、KEYx引脚定义、fsmc_sram_test
#include "usart.h"  //fputc
#include "stm32f4xx_hal_gpio.h"  //HAL_GPIO_TogglePin、HAL_GPIO_ReadPin

#define BUFFER_SIZE 262144  //SRAM总大小1024*1024=262144*4Bytes，因为是外扩SRAM可支配大小就是芯片本身容量

extern uint32_t g_test_buffer[BUFFER_SIZE];  //总计1000 000Byte数据
extern uint32_t ts;
int8_t diff = 0; 
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
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0亮
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0灭
				fsmc_sram_test();    /* 测试SRAM容量 */
				break;
			case KEY1_Pin:
				printf("KEY1 pressed!\n");
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0亮
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0灭
				for (ts = 0; ts < BUFFER_SIZE; ts++){
						if(g_test_buffer[ts] != ts){
							//printf("%d\n", g_test_buffer[ts]); /* 显示测试数据 */
							printf("g_test_buffer[] was changed!\n");
							diff = 1;
							break;
						}
				}
				if(!diff)
				  printf("Nothing change to g_test_buffer[]!\n");
				/*key1*/
				break;
			case KEY2_Pin:
				printf("KEY2 pressed!\n");
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0亮
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0灭
				//给SRAM中开辟的数组重新赋值
				for (ts = 0; ts < BUFFER_SIZE; ts++){
						g_test_buffer[ts] = ts;         /* 预存测试数据 */
				}
				diff = 0;
				printf("Recovered g_test_buffer[]!");
				break;
			default:
				break;
		}
}
