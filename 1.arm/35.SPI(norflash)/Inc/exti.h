#include "main.h"  //LEDx��KEYx���Ŷ���
#include "usart.h"  //fputc
#include "norflash.h"  //norflash_write

const uint8_t g_text_buf[] = {"STM32 SPI TEST"}; /* Ҫд��FLASH���ַ������� */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT�ַ������� */
uint8_t datatemp[TEXT_SIZE]; 
uint32_t flashsize = 16 * 1024 * 1024; /* FLASH ��СΪ16M�ֽ� */;

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
				printf("KEY0 press,start write W25Q128! \n");
				norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);  /* �ӵ�����100����ַ����ʼ,д��SIZE���ȵ����� */
				printf("write W25Q128 finished! \n");
				break;
			case  KEY1_Pin:
				printf("KEY1 press,start read W25Q128!\n");  /* �ӵ�����100����ַ����ʼ,����SIZE���ֽ� */
				norflash_read(datatemp, flashsize - 100, TEXT_SIZE);
				printf("read something like: %s\n", (char *)datatemp);
				break;
			default:
				break;
		}
}
