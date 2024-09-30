/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Src/usbd_storage.c
  * @author  MCD Application Team
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usbd_storage.h"
//#include "stm324xg_eval_sd.h"
#include "sdio_sdcard.h"  //SD Card (g_sd_card_info_handle)
#include "norflash.h"  //W25Q128

/* �ļ�ϵͳ���ⲿFLASH����ʼ��ַ
 * ���Ƕ���SPI FLASHǰ12M���ļ�ϵͳ��, ���Ե�ַ��0��ʼ
 */
#define USB_STORAGE_FLASH_BASE  0
#define USB_STORAGE_SD_BASE  0

/* �Լ������һ�����USB״̬�ļĴ��� */
/* bit0:��ʾ����������SD��д������ */
/* bit1:��ʾ��������SD���������� */
/* bit2:SD��д���ݴ����־λ */
/* bit3:SD�������ݴ����־λ */
/* bit4:1,��ʾ��������ѯ����(�������ӻ�������) */
volatile uint8_t g_usb_state_reg = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 

#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t writestatus, readstatus = 0;
/* USB Mass storage Standard Inquiry Data */
int8_t STORAGE_Inquirydata[] = { /* 36 */
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'D', 'C', 'A', 'R', 'D', ' ', ' ', /* Manufacturer: 8 bytes  */
  'S', 'D', ' ', 'S', 't', 'o', 'r', 'a', /* Product     : 16 Bytes */
  'g', 'e', ' ', ' ', ' ', ' ', ' ', ' ',
  '1', '.', '0','0',                      /* Version     : 4 Bytes  */
  /* LUN 1 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', /* Manufacturer: 8 bytes  */
  'S', 'P', 'I', 'F', 'l', 'a', 's', 'h', /* Product     : 16 Bytes */
  ' ', 'D', 'i', 's', 'k', ' ', ' ', ' ',
  '0', '.', '0','1',                      /* Version     : 4 Bytes  */
}; 

/* Private function prototypes -----------------------------------------------*/
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata, 
};
/* Private functions ---------------------------------------------------------*/

/**
 * @brief       ��ʼ���洢�豸
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SPI FLASH
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Init(uint8_t lun)
{
		printf("STORAGE_Init called\n");
    uint8_t res;
    switch(lun)
    {
				case 0:     /* SD Card */
						sd_init();
						res = USBD_OK;
						break;
				case 1:     /* SPI FLASH */
            norflash_init();
            res = USBD_OK;
            break;	 
        default :
            res = USBD_FAIL;
    }
    return res;
}

/**
 * @brief       ��ȡ�洢�豸�������Ϳ��С
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SPI FLASH
 * @param       block_num  : ������(������)
 * @param       block_size : ���С(������С)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    uint8_t res;
    switch(lun)
    {
				case 0:     /* SD Card */
            *block_size = g_sd_card_info_handle.BlockSize;
            *block_num = g_sd_card_info_handle.LogBlockNbr - 1;
            res = USBD_OK;
            break;
        case 1:     /* SPI FLASH */
            *block_size = 512;
            *block_num = (1024 * 1024 * 12) / 512;    /* SPI FLASH��ǰ��12M�ֽ�,�ļ�ϵͳ�� */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}

/**
 * @brief       �鿴�洢�豸�Ƿ����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  1, SPI FLASH
 * @retval      ����״̬
 *   @arg       0    , ����
 *   @arg       ���� , δ����
 */
int8_t STORAGE_IsReady(uint8_t lun)
{
    uint8_t res;

    g_usb_state_reg |= 0x10;    /* �����ѯ */

    switch(lun)
    {
				case 0:     /* SD Card */
            res = USBD_OK;
            break;
        case 1:                 /* SPI FLASH */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}


/**
 * @brief       �鿴�洢�豸�Ƿ�д����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  1, SPI FLASH
 * @retval      ������״̬
 *   @arg       0    , û�ж�����
 *   @arg       ���� , �ж�����
 */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
    uint8_t res;
    switch(lun)
    {
				case 0:     /* SD Card */
            res = USBD_OK;
            break;
        case 1:     /* SPI FLASH */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }

    return res;
}

/**
 * @brief       �Ӵ洢�豸��ȡ����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SPI FLASH
 * @param       buf        : ���ݴ洢���׵�ַָ��
 * @param       blk_addr   : Ҫ��ȡ�ĵ�ַ(������ַ)
 * @param       blk_len    : Ҫ��ȡ�Ŀ���(������)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    uint8_t res;

    g_usb_state_reg |= 0x02;    /* ������ڶ����� */
    
    switch(lun)
    {
				case 0:     /* SD Card */
						sd_read_disk(buf, USB_STORAGE_SD_BASE + blk_addr, blk_len);
            res = USBD_OK;
            break;
        case 1:                 /* SPI FLASH */
            norflash_read(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}

/**
 * @brief       ��洢�豸д����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SPI FLASH
 * @param       buf        : ���ݴ洢���׵�ַָ��
 * @param       blk_addr   : Ҫд��ĵ�ַ(������ַ)
 * @param       blk_len    : Ҫд��Ŀ���(������)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    uint8_t res;
    
    g_usb_state_reg |= 0x01;    /* �������д���� */

    switch(lun)
    {
				case 0:     /* SD Card */
						sd_write_disk(buf, USB_STORAGE_SD_BASE + blk_addr, blk_len);
            res = USBD_OK;
            break;
        case 1:     /* SPI FLASH */
            norflash_write(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    return res;
}

/**
 * @brief       ��ȡ֧�ֵ�����߼���Ԫ����
 *   @note      ע��, ���ﷵ�ص��߼���Ԫ�����Ǽ�ȥ��1��.
 *              0, �ͱ�ʾ1��; 1, ��ʾ2��; �Դ�����
 * @param       ��
 * @retval      ֧�ֵ��߼���Ԫ���� - 1
 */
int8_t STORAGE_GetMaxLun (void)
{
    HAL_SD_CardInfoTypeDef sd_card_info;
    HAL_SD_GetCardInfo(&hsd, &sd_card_info);

    /* STORAGE_LUN_NBR ��usbd_conf.h���涨��, Ĭ����2 */
    if (sd_card_info.LogBlockNbr)    /* ���SD������, ��֧��2������ */
    {
        return STORAGE_LUN_NBR - 1;
    }
    else    /* SD��������, ��ֻ֧��1������ */
    {
        return STORAGE_LUN_NBR - 2;
    }
}
 
/**
  * @brief BSP Tx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_WriteCpltCallback(void)
{
  writestatus = 1;
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_ReadCpltCallback(void)
{
  readstatus = 1;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

