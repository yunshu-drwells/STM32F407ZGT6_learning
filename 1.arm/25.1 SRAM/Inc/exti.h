#include "main.h"  //LEDx��KEYx���Ŷ��塢fsmc_sram_test
#include "usart.h"  //fputc
#include "stm32f4xx_hal_gpio.h"  //HAL_GPIO_TogglePin��HAL_GPIO_ReadPin

#define BUFFER_SIZE 262144  //SRAM�ܴ�С1024*1024=262144*4Bytes����Ϊ������SRAM��֧���С����оƬ��������

extern uint32_t g_test_buffer[BUFFER_SIZE];  //�ܼ�1000 000Byte����
extern uint32_t ts;
int8_t diff = 0; 
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
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0��
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0��
				fsmc_sram_test();    /* ����SRAM���� */
				break;
			case KEY1_Pin:
				printf("KEY1 pressed!\n");
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0��
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0��
				for (ts = 0; ts < BUFFER_SIZE; ts++){
						if(g_test_buffer[ts] != ts){
							//printf("%d\n", g_test_buffer[ts]); /* ��ʾ�������� */
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
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  //LED0��
				HAL_Delay(50);
				HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  //LED0��
				//��SRAM�п��ٵ��������¸�ֵ
				for (ts = 0; ts < BUFFER_SIZE; ts++){
						g_test_buffer[ts] = ts;         /* Ԥ��������� */
				}
				diff = 0;
				printf("Recovered g_test_buffer[]!");
				break;
			default:
				break;
		}
}
