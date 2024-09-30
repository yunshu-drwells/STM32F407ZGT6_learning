#include "string.h"
#include "usart.h"
#include "sdio_sdcard.h"

/**
 * @brief       ��ʼ��SD��
 * @param       ��
 * @retval      ����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;
	
		//���� HAL_SD_Init ǰ��� hsd.ErrorCode
		hsd.ErrorCode = HAL_SD_ERROR_NONE; // ����������
    SD_Error = HAL_SD_Init(&hsd);
    if (SD_Error != HAL_OK)
    {
				printf("HAL_SD_Init error!\n");
        return 1;
    }
    
    HAL_SD_GetCardInfo(&hsd, &g_sd_card_info_handle);                  /* ��ȡSD����Ϣ */

    SD_Error = HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);  /* ʹ��4bit������ģʽ */
    if (SD_Error != HAL_OK)
    {
				printf("HAL_SD_ConfigWideBusOperation error!\n");
        return 2;
    }
    
    return 0;
}

/**
 * @brief       ��ȡ����Ϣ����
 * @param       cardinfo:SD����Ϣ���
 * @retval      ����ֵ:��ȡ����Ϣ״ֵ̬
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    
    sta = HAL_SD_GetCardInfo(&hsd, cardinfo);
    
    return sta;
}

/**
 * @brief       �ж�SD���Ƿ���Դ���(��д)����
 * @param       ��
 * @retval      ����ֵ:SD_TRANSFER_OK      ������ɣ����Լ�����һ�δ���
                       SD_TRANSFER_BUSY SD ����æ�������Խ�����һ�δ���
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       ��SD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    
    __disable_irq();                                                                       /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_ReadBlocks(&hsd, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* ���sector�Ķ����� */

    /* �ȴ�SD������ */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq(); /* �������ж� */
    
    return sta;
}

/**
 * @brief       дSD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    
    __disable_irq();                                                                        /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_WriteBlocks(&hsd, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* ���sector��д���� */

    /* �ȴ�SD��д�� */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq();     /* �������ж� */
    
    return sta;
}
