#include "main.h"  //LEDx、KEYx引脚定义
#include "usart.h"  //fputc
#include "norflash.h"  //norflash_write
#include "lcd.h"  //lcd_show_string
#include "mymalloc.h"  //mymalloc、myfree

//norflash相关
const uint8_t g_text_buf[] = {"STM32 SPI TEST!"}; /* 要写到FLASH的字符串数组 */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长度 */
uint8_t datatemp[TEXT_SIZE]; 
uint32_t flashsize = 16 * 1024 * 1024; /* FLASH 大小为16M字节 */;

//SD相关
extern SD_HandleTypeDef hsd;
//const uint8_t str[] = {"STM32 SDIO TEST!"};
#define BUF_SIZE 512
uint8_t w_buffer[BUF_SIZE] = {"STM32 SDIO TEST!"};
uint8_t r_buffer[BUF_SIZE];
int8_t i;
#define BLOCK_NUM 1

uint8_t DMA_SEND_OK, DMA_RCV_OK;  //发送和接收状态标志位

//SRAM相关
uint8_t *p;
uint8_t sramx = 0;                  /* 默认为内部sram */
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
				printf("Key0 press, Test SD!\n");
				printf("start write SD! \n");
				DMA_SEND_OK = 0;  //清除标志位
							
				/* 从倒数第1个块地址处开始,写入一个块。超时时间1000ms */
				if(HAL_OK == HAL_SD_WriteBlocks_DMA(&hsd, w_buffer, BLOCK_NUM, 1)){
					//写操作中会进入HAL_SD_CARD_PROGRAMMING状态;写数据完成后会进入HAL_SD_CARD_TRANSFER状态
					while(!DMA_SEND_OK || HAL_SD_CARD_TRANSFER != HAL_SD_GetCardState(&hsd)){ //死循环等待DMA传输完成
						HAL_Delay(1);// 添加一个小延迟，避免忙等待
          }
					printf("Write SD successed! \n");
				}else{
					printf("Write failed, something error occured!\n");
				}
				printf("start read SD! \n");
				DMA_RCV_OK = 0;
				/* 从倒数第1个块地址处开始,读一个块。超时时间1000ms*/
				if(HAL_OK == HAL_SD_ReadBlocks_DMA(&hsd, r_buffer, BLOCK_NUM, 1)){
					//通常情况下，读取操作前后以及读取过程中卡的状态一直保持HAL_SD_CARD_TRANSFER
					while(!DMA_RCV_OK){
						HAL_Delay(1);// 添加一个小延迟，避免忙等待
					}
					printf("Read SD successed! \n");
					i = 0;
					if('\0' == r_buffer[i]){
						printf("read some zeros\n");
					}else
						printf("read something like: %c", (char *)r_buffer[i++]);
					while('\0' != r_buffer[i]){
						printf("%c", (char *)r_buffer[i++]);
					}
					printf("\n");
				}else{
					printf("Read failed, something error occured!\n");
				}
				for(i=0; i<16; i++){
					printf("%d ", r_buffer[i]);
				}
				printf("\n");
				for(i=0; i<16; i++){
					printf("%c ", r_buffer[i]);
				}
				printf("\n");
				lcd_show_string(30, 230, 200, 16, 16, "SD read something like: ", BLUE);
				lcd_show_string(30+13*15, 230, 209, 16, 16, (char *)r_buffer, BLUE);     /* 显示内容 */
				//printf("State:%d\n", HAL_SD_GetCardState(&hsd));
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
				lcd_show_string(30, 250, 230, 16, 16, "Norflash read something like: ", BLUE);
				lcd_show_string(30+16*15, 250, 209, 16, 16, (char *)datatemp, BLUE);     /* 显示内容 */
				break;
			case  KEY2_Pin:
				printf("SRAM start malloc! \n");
				p = mymalloc(sramx, 2048);  /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", block_size++);            /* 向p写入一些内容 */
						lcd_show_string(30, 270, 209, 16, 16, (char *)p, BLUE);     /* 显示P的内容 */
				}
				sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)p);
				lcd_show_string(30, 290, 209, 16, 16, (char *)paddr, BLUE); /* 显示p的地址 */
				
				lcd_show_string(30, 310, 200, 16, 16, "SRAMIN   USED:", BLUE);
				memused = my_mem_perused(SRAMIN);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 310, 200, 16, 16, (char *)paddr, BLUE);   /* 显示内部内存使用率 */
				//printf("SRAM start free! \n");
				//myfree(sramx, p);   /* 释放内存 */
        //p = 0;              /* 指向空地址 */
				break;
			default:
				break;
		}
}
