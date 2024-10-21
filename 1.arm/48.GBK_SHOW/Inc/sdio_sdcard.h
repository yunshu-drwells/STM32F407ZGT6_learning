#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "sdio.h"  //hsd

#define SD_TIMEOUT             ((uint32_t)100000000)    /* 超时时间 */
#define SD_TRANSFER_OK         ((uint8_t)0x00)
#define SD_TRANSFER_BUSY       ((uint8_t)0x01)

/* 根据 SD_HandleTypeDef 定义的宏，用于快速计算容量 */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 30)

/*  
 *  SD传输时钟分频，由于HAL库运行效率低，很容易产生上溢（读SD卡时）/下溢错误（写SD卡时）
 *  使用4bit模式时，需降低SDIO时钟频率，将该宏改为 1，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 16M * 4bit = 64Mbps 
 *  使用1bit模式时，该宏SDIO_TRANSF_CLK_DIV改为 0，SDIO时钟频率：48/( SDIO_TRANSF_CLK_DIV + 2 ) = 24M * 1bit = 24Mbps 
 */
#define  SDIO_TRANSF_CLK_DIV        1   
/******************************************************************************************/

extern SD_HandleTypeDef hsd; //SD卡句柄 sdio.c
extern HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;   /* SD卡信息结构体 */

/* 函数声明 */
uint8_t sd_init(void); 
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo);         /* 获取卡信息函数 */
uint8_t get_sd_card_state(void);                                    /* 获取卡的状态 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);  /* 读SD卡 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt); /* 写SD卡 */

#endif
