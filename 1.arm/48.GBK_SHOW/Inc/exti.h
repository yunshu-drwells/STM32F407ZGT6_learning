#include "main.h"  //LEDx、KEYx引脚定义
#include "usart.h"  //fputc
#include "norflash.h"  //norflash_write
#include "lcd.h"  //lcd_show_string
#include "mymalloc.h"  //mymalloc、myfree
#include "exfuns.h"  //fs[]
#include "main.h"  //erase_gbk_sector
#include "ff.h"  //f_mount
#include "fonts.h"  //fonts_init
#include "text.h"  //text_show_string

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

//更新字库相关
extern uint8_t sd_mount_status;               /* SD 挂载状态 */
extern uint8_t flash_mount_status;               /* FLASH 挂载状态 */
extern uint8_t fonts_update_res;
uint8_t t = 0;
uint8_t fontx[2];   /* GBK码 */
uint32_t fontcnt = 0;
uint8_t j = 0;

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
					lcd_show_string(30, 250, 200, 16, 16, "SD write something like: ", BLUE);
					lcd_show_string(30+13*15, 250, 209, 16, 16, (char *)w_buffer, BLUE);     /* 显示内容 */
				}else{
					printf("Write failed, something error occured!\n");
				}
				delay_ms(1000);
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
				lcd_show_string(30, 250, 200, 16, 16, "SD read something like: ", BLUE);
				lcd_show_string(30+13*15, 250, 209, 16, 16, (char *)r_buffer, BLUE);     /* 显示内容 */
				//printf("State:%d\n", HAL_SD_GetCardState(&hsd));
				break;
			case  KEY1_Pin:
				/*key1*/
				printf("Key1 press, Test norflash!\n");
				printf("Start write W25Q128! \n");
				norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);  /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
				printf("write W25Q128 finished! \n");
				lcd_show_string(30, 270, 230, 16, 16, "Norflash write something like: ", BLUE);
				lcd_show_string(30+16*15, 270, 209, 16, 16, (char *)datatemp, BLUE);     /* 显示内容 */
				printf("Start read W25Q128!\n");  /* 从倒数第100个地址处开始,读出SIZE个字节 */
				delay_ms(1000);
				norflash_read(datatemp, flashsize - 100, TEXT_SIZE);
				printf("read something like: %s\n", (char *)datatemp);
				lcd_show_string(30, 270, 230, 16, 16, "Norflash read something like: ", BLUE);
				lcd_show_string(30+16*15, 270, 209, 16, 16, (char *)datatemp, BLUE);     /* 显示内容 */
				break;
			case  KEY2_Pin:
				printf("Key2 press, Test SRAM!\n");
				printf("SRAM start malloc! \n");
				p = mymalloc(sramx, 2048);  /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", block_size++);            /* 向p写入一些内容 */
						lcd_show_string(30, 290, 209, 16, 16, (char *)p, BLUE);     /* 显示P的内容 */
				}
				sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)p);
				lcd_show_string(30, 310, 209, 16, 16, (char *)paddr, BLUE); /* 显示p的地址 */
				
				lcd_show_string(30, 330, 200, 16, 16, "SRAMIN   USED:", BLUE);
				memused = my_mem_perused(SRAMIN);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 330, 200, 16, 16, (char *)paddr, BLUE);   /* 显示内部内存使用率 */
				//printf("SRAM start free! \n");
				//myfree(sramx, p);   /* 释放内存 */
        //p = 0;              /* 指向空地址 */
				break;
			case  KEY_UP_Pin:
				printf("Key_up press, update font!\n");
				//更新字库
				printf("Start erase norflash charactor lib!\n");
				lcd_show_string(30, 510, 280, 16, 16, "Start erase norflash charactor lib!", RED);
				erase_gbk_sector();  //清空norflash已有的字库
				printf("Erased norflash charactor lib!\n");
				lcd_fill(20, 510, 200 + 20, 90 + 16, WHITE);
				lcd_show_string(30, 510, 200, 16, 16, "Erased norflash charactor lib!", RED);
				lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);	
				while (fonts_init()) {               // 检查字库 
						printf("update font\r\n");
						//lcd_clear(WHITE);               // 清屏 
					
						//循环操作直到挂载u盘成功
						while(0 != sd_mount_status) {         // 等待 SD 设备挂载完成
								lcd_show_string(30, 530, 200, 16, 16, "SD Connecting...", RED);
								sd_mount_status = f_mount(fs[0], "0:", 1);        // 挂载SD卡
								delay_ms(20);
						}
						lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);
						lcd_show_string(30, 530, 200, 16, 16, "Font Updating...", RED);

						fonts_update_res = fonts_update_font(20, 90, 16, (uint8_t *)"0:", RED);  // 更新字库
						while (fonts_update_res)         // 更新失败 
						{
								lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);
								delay_ms(200);
								lcd_show_string(30, 530, 530, 16, 16, "Font Update Failed!", RED);
						}
						lcd_fill(20, 530, 200 + 20, 90 + 16, WHITE);
						lcd_show_string(30, 530, 200, 16, 16, "Font Update Success!   ", RED);
						//lcd_clear(WHITE);   // 清屏 
				}

				text_show_string(30, 550, 200, 16, "正点原子STM32开发板", 16, 0, RED);
				text_show_string(30, 570, 200, 16, "GBK字库测试程序", 16, 0, RED);
				text_show_string(30, 590, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
				text_show_string(30, 610, 200, 16, "按KEY_UP, 强制更新字库", 16, 0, RED);

				text_show_string(30, 630, 200, 16, "内码高字节:", 16, 0, BLUE);
				text_show_string(30, 650, 200, 16, "内码低字节:", 16, 0, BLUE);
				text_show_string(30, 670, 200, 16, "汉字计数器:", 16, 0, BLUE);

				text_show_string(30, 700, 200, 24, "对应汉字(24*24)为:", 24, 0, BLUE);
				text_show_string(30, 724, 200, 16, "对应汉字(16*16)为:", 16, 0, BLUE);
				text_show_string(30, 740, 200, 16, "对应汉字(12*12)为:", 12, 0, BLUE);
				
				for (i = 0x81; i < 0xff; i++) {              // GBK内码高字节范围为0x81~0xFE
						fontx[0] = i;
						lcd_show_num(118, 630, i, 3, 16, BLUE); // 显示内码高字节

						for (j = 0x40; j < 0xfe; j++) {          // GBK内码低字节范围为 0x40~0x7E, 0x80~0xFE)
								if (j == 0x7f)
										continue;

								fontcnt++;
								lcd_show_num(118, 650, j, 3, 16, BLUE);       // 显示内码低字节 
								lcd_show_num(118, 670, fontcnt, 5, 16, BLUE); // 汉字计数显示 
								fontx[1] = j;
								text_show_font(30 + 198, 700, fontx, 24, 0, BLUE);
								text_show_font(30 + 144, 724, fontx, 16, 0, BLUE);
								text_show_font(30 + 108, 740, fontx, 12, 0, BLUE);
								t = 200;
								
								HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
								delay_ms(20);
						}
				}
				break;
			default:
				break;
		}
}
