#include "main.h"  //LEDx、KEYx引脚定义
#include "usart.h"  //fputc

extern SD_HandleTypeDef hsd;
//const uint8_t str[] = {"STM32 SDIO TEST!"};
#define BUF_SIZE 512
uint8_t w_buffer[BUF_SIZE] = {"STM32 SDIO TEST!"};
uint8_t r_buffer[BUF_SIZE];
extern int8_t i;

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
				printf("KEY0 press,start write SD! \n");
				/* 从倒数第1个块地址处开始,写入一个块。超时时间1000ms */
				if(HAL_OK == HAL_SD_WriteBlocks(&hsd, w_buffer, 0, 1, 1000)){
					//写操作中会进入HAL_SD_CARD_PROGRAMMING状态;写数据完成后会进入HAL_SD_CARD_TRANSFER状态
					while(HAL_SD_CARD_TRANSFER != HAL_SD_GetCardState(&hsd)){ //死循环等待传输完成，传输完成会进入断开状态
						HAL_Delay(1);// 添加一个小延迟，避免忙等待
          }
					printf("Write SD successed! \n");
				}else{
					printf("Write failed, something error occured!\n");
				}
				break;	
			case  KEY1_Pin:
				/*key1*/
				printf("KEY1 press,start read SD! \n");
					/* 从倒数第1个块地址处开始,读一个块。超时时间1000ms*/
				if(HAL_OK == HAL_SD_ReadBlocks(&hsd, r_buffer, 0, 1, 1000)){
					//通常情况下，读取操作前后以及读取过程中卡的状态一直保持HAL_SD_CARD_TRANSFER
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
				//printf("State:%d\n", HAL_SD_GetCardState(&hsd));
				break;
			case  KEY2_Pin:
				/*key2*/
				printf("KEY2 press,start erase SD! \n");
				//擦除块1
				if(HAL_OK == HAL_SD_Erase(&hsd, 0, 0)){  //[BlockStartAdd, BlockEndAdd]
					//擦除操作中会进入HAL_SD_CARD_PROGRAMMING状态;擦除数据完成后会进入HAL_SD_CARD_TRANSFER状态
					while(HAL_SD_CARD_TRANSFER != HAL_SD_GetCardState(&hsd)){  //死循环等待传输完成
					  HAL_Delay(1);// 添加一个小延迟，避免忙等待
          }
					printf("Erase SD successed! \n");
				}else{
					printf("Erase failed, something error occured!\n");
				}
				break;
			default:
				break;
		}
}
