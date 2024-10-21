#include "main.h"  //LEDx、KEYx引脚定义 IS_KEY0_PRES、load_draw_dialog、lcd_draw_bline、rtp_test、ctp_test、emwin_test_touch、OS_TimeMS
#include "usart.h"  //fputc
#include "lcd.h"  //lcd_show_string
#include "touch.h"  //tp_dev
#include "norflash.h"  //norflash_write、norflash_read
#include "mymalloc.h"  //mymalloc
#include "GUI.h"  //GUI_TOUCH_Exec

extern uint8_t IS_KEY0_PRES;

//norflash相关
const uint8_t g_text_buf[] = {"STM32 SPI TEST!"}; /* 要写到FLASH的字符串数组 */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长度 */
uint8_t datatemp[TEXT_SIZE]; 
uint32_t flashsize = 16 * 1024 * 1024; /* FLASH 大小为16M字节 */;

//SRAM相关
uint8_t *p;
uint8_t sramx = 2;                  /* 默认为外部sram (SRAMIN:0; SRAMCCM:1; SRAMEX:2)*/
uint8_t paddr[20];                  /* 存放P Addr:+p地址的ASCII值 */
uint16_t memused = 0;
int8_t block_size = 0;

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
				printf("Key0 press, Test Touch!\n");
				IS_KEY0_PRES = 1;
				if (tp_dev.touchtype == 0xFF)
				{
					printf("This is ctp screen!\n");
					lcd_show_string(10, 170, 200, 16, 16, "Press KEY0 to Adjust", RED); // 电阻屏才显示 
				}

				delay_ms(1000);
				load_draw_dialog();
				
				if (tp_dev.touchtype & 0x80)
				{
					printf("This is ctp test!\n");
					ctp_test(); // 电容屏测试 
				}
				else
				{
					printf("This is rtp test!\n");
					rtp_test(); // 电阻屏测试 
				}
				break;
			case  KEY1_Pin:
				/*key1*/
				printf("Key1 press, Test norflash!\n");
				printf("Start write W25Q128! \n");
				norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);  /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
				printf("write W25Q128 finished! \n");
				printf("Start read W25Q128!\n");  /* 从倒数第100个地址处开始,读出SIZE个字节 */
				norflash_read(datatemp, flashsize - 100, TEXT_SIZE);
				printf("read something like: %s\n", (char *)datatemp);
				lcd_show_string(30, 330, 230, 16, 16, "Norflash read something like: ", BLUE);
				lcd_show_string(30+16*15, 330, 209, 16, 16, (char *)datatemp, BLUE);     /* 显示内容 */
				break;
			case  KEY2_Pin:
				printf("key2 press, SRAM start malloc! \n");
				p = mymalloc(sramx, 2048);  /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", block_size++);            /* 向p写入一些内容 */
						lcd_show_string(30, 350, 209, 16, 16, (char *)p, BLUE);     /* 显示P的内容 */
				}
				sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)p);
				lcd_show_string(30, 370, 209, 16, 16, (char *)paddr, BLUE); /* 显示p的地址 */
				
				lcd_show_string(30, 390, 200, 16, 16, "SRAMEX   USED:", BLUE);
				memused = my_mem_perused(SRAMEX);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 390, 200, 16, 16, (char *)paddr, BLUE);   /* 显示内部内存使用率 */
				break;
			case KEY_UP_Pin:
					printf("Key_up press, emWin get location! \n");
					emwin_test_touch();  //emWin坐标获取
				break;
			default:
				break;
		}
}

uint16_t count_time = 0;
extern __IO int32_t OS_TimeMS;  //GUI_X.c

/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        count_time ++;
        OS_TimeMS ++;
        
        if (count_time == 10)
        {
            GUI_TOUCH_Exec();  /* 每10ms调用一次，触发调用触摸驱动 */
            count_time = 0;
        }
    }
}
