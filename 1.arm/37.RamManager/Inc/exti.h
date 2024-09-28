#include "main.h"  //LEDx��KEYx���Ŷ���
#include "usart.h"  //fputc
#include "mymalloc.h"  //
#include "lcd.h"  //lcd_show_string

extern uint8_t i;
extern uint8_t *p;
uint8_t sramx = 0;                  /* Ĭ��Ϊ�ڲ�sram */
const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN  ", " SRAMCCM ", " SRAMEX  "};
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
				printf("KEY0 press,start malloc! \n");
				p = mymalloc(sramx, 2048);  /* ����2K�ֽ�,��д������,��ʾ��lcd��Ļ���� */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", i);            /* ��pд��һЩ���� */
						lcd_show_string(30, 290, 209, 16, 16, (char *)p, BLUE);     /* ��ʾP������ */
				}

				break;	
			case  KEY1_Pin:
				/*key1*/
				printf("KEY1 press,start free! \n");
				myfree(sramx, p);   /* �ͷ��ڴ� */
        p = 0;              /* ָ��յ�ַ */
				break;
			case  KEY2_Pin:
				/*key2*/
				sramx++;
				printf("KEY2 press,change RAM:%s!\n", (char *)SRAM_NAME_BUF[sramx]);

				if (sramx >= SRAMBANK) sramx = 0;
				lcd_show_string(60, 226, 200, 16, 16, (char *)SRAM_NAME_BUF[sramx], BLUE);
				break;
			default:
				break;
		}
}
