#include "stdint.h"  //uint8_t����������

/* SRAM����ַ, ���� SRAM_FSMC_NEX ��������������ַ��ַ
 * ����һ��ʹ��FSMC�Ŀ�1(BANK1)������SRAM, ��1��ַ��Χ�ܴ�СΪ256MB,���ֳ�4��:
 * �洢��1(FSMC_NE1)��ַ��Χ: 0X6000 0000 ~ 0X63FF FFFF
 * �洢��2(FSMC_NE2)��ַ��Χ: 0X6400 0000 ~ 0X67FF FFFF
 * �洢��3(FSMC_NE3)��ַ��Χ: 0X6800 0000 ~ 0X6BFF FFFF
 * �洢��4(FSMC_NE4)��ַ��Χ: 0X6C00 0000 ~ 0X6FFF FFFF
 */
 
#define SRAM_FSMC_NEX           3         /* ʹ��FSMC_NE3��SRAM_CS,ȡֵ��Χֻ����: 1~4 */
#define SRAM_BASE_ADDR         (0X60000000 + (0X4000000 * (SRAM_FSMC_NEX - 1)))  //0X6800 0000

void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen);
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen);

void fsmc_sram_test(void);
