#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "sdio.h"  //hsd

#define SD_TIMEOUT             ((uint32_t)100000000)    /* ��ʱʱ�� */
#define SD_TRANSFER_OK         ((uint8_t)0x00)
#define SD_TRANSFER_BUSY       ((uint8_t)0x01)

/* ���� SD_HandleTypeDef ����ĺ꣬���ڿ��ټ������� */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 30)

/*  
 *  SD����ʱ�ӷ�Ƶ������HAL������Ч�ʵͣ������ײ������磨��SD��ʱ��/�������дSD��ʱ��
 *  ʹ��4bitģʽʱ���轵��SDIOʱ��Ƶ�ʣ����ú��Ϊ 1��SDIOʱ��Ƶ�ʣ�48/( SDIO_TRANSF_CLK_DIV + 2 ) = 16M * 4bit = 64Mbps 
 *  ʹ��1bitģʽʱ���ú�SDIO_TRANSF_CLK_DIV��Ϊ 0��SDIOʱ��Ƶ�ʣ�48/( SDIO_TRANSF_CLK_DIV + 2 ) = 24M * 1bit = 24Mbps 
 */
#define  SDIO_TRANSF_CLK_DIV        1   
/******************************************************************************************/

extern SD_HandleTypeDef hsd; //SD����� sdio.c
extern HAL_SD_CardInfoTypeDef  g_sd_card_info_handle;   /* SD����Ϣ�ṹ�� */

/* �������� */
uint8_t sd_init(void); 
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo);         /* ��ȡ����Ϣ���� */
uint8_t get_sd_card_state(void);                                    /* ��ȡ����״̬ */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);  /* ��SD�� */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt); /* дSD�� */

#endif
