#include "main.h"  //LEDx、KEYx引脚定义
#include "usart.h"  //fputc
#include "norflash.h"  //norflash_write

const uint8_t g_text_buf[] = {"STM32 SPI TEST"}; /* 要写到FLASH的字符串数组 */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长度 */
uint8_t datatemp[TEXT_SIZE]; 
uint32_t flashsize = 16 * 1024 * 1024; /* FLASH 大小为16M字节 */;

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
				printf("KEY0 press,start write W25Q128! \n");
				norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);  /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
				printf("write W25Q128 finished! \n");
				break;
			case  KEY1_Pin:
				printf("KEY1 press,start read W25Q128!\n");  /* 从倒数第100个地址处开始,读出SIZE个字节 */
				norflash_read(datatemp, flashsize - 100, TEXT_SIZE);
				printf("read something like: %s\n", (char *)datatemp);
				break;
			default:
				break;
		}
}
