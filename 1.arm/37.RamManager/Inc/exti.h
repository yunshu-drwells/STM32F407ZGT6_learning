#include "main.h"  //LEDx、KEYx引脚定义
#include "usart.h"  //fputc
#include "mymalloc.h"  //
#include "lcd.h"  //lcd_show_string

extern uint8_t i;
extern uint8_t *p;
uint8_t sramx = 0;                  /* 默认为内部sram */
const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN  ", " SRAMCCM ", " SRAMEX  "};
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
				printf("KEY0 press,start malloc! \n");
				p = mymalloc(sramx, 2048);  /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", i);            /* 向p写入一些内容 */
						lcd_show_string(30, 290, 209, 16, 16, (char *)p, BLUE);     /* 显示P的内容 */
				}

				break;	
			case  KEY1_Pin:
				/*key1*/
				printf("KEY1 press,start free! \n");
				myfree(sramx, p);   /* 释放内存 */
        p = 0;              /* 指向空地址 */
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
