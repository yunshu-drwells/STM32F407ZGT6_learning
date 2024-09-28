#include "main.h"  //LEDx��KEYx���Ŷ��塢fsmc_sram_test
#include "usart.h"  //fputc
#include "stm32f4xx_hal_gpio.h"  //HAL_GPIO_TogglePin��HAL_GPIO_ReadPin
#include "sram.h"
#include "lcd.h"  //lcd_show_xnum

#define BUFFER_SIZE 262144  //SRAM�ܴ�С1024*1024Bytes=262144*4Bytes����Ϊ������SRAM��֧���С����оƬ��������

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
				lcd_show_string(30, 230, 239, 210 + 16, 16, "g_test_buffer show:   ", BLUE);
				for (ts = 0; ts < BUFFER_SIZE; ts++) {
					lcd_show_xnum(30+15*10, 230, g_test_buffer[ts], 6, 16, 0, BLUE); // ��ʾ�������� 
				}
				if(!diff)
				  printf("Nothing change to g_test_buffer[]!\n");
				/*key1*/
				break;
			case KEY2_Pin:
				break;
			default:
				break;
		}
}
