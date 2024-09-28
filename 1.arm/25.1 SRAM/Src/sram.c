#include "sram.h"
#include "usart.h"  //fputc

/**
 * @brief       ��SRAMָ����ַд��ָ����������
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       datalen : Ҫд����ֽ���(���32bit)
 * @retval      ��
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       ��SRAMָ����ַ��ȡָ����������
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param       datalen : Ҫ��ȡ���ֽ���(���32bit)
 * @retval      ��
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}

/*******************���Ժ���**********************************/

/**
 * @brief       ���Ժ��� ��SRAMָ����ַд��1���ֽ�
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       data    : Ҫд����ֽ�
 * @retval      ��
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1); /* д��1���ֽ� */
}

/**
 * @brief       ���Ժ��� ��SRAMָ����ַ��ȡ1���ֽ�
 * @param       addr    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @retval      ��ȡ��������(1���ֽ�)
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1); /* ��ȡ1���ֽ� */
    return data;
}

/**
 * @brief       �ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)
 * @param       ��
 * @retval      ��
 */
void fsmc_sram_test()
{
    uint32_t i = 0;
    uint8_t temp = 0;
    uint8_t pre_val = 0; /* �ڵ�ַ0���������� */

	/* ÿ��4K�ֽ�,д��һ������,�ܹ�д��256������,�պ���1M�ֽ� */
    for (i = 0; i < 1024 * 1024; i += 4096){
			  sram_write(&temp, i, 1); //д���������[0:255]
        temp++;
    }

    /* ���ζ���֮ǰд�������,����У�� */
    for (i = 0; i < 1024 * 1024; i += 4096)  {
        sram_read(&temp, i, 1);

        if (0 != temp && temp <= pre_val){
						printf("Some error occer to SRAM\n");
            break; /* �������������һ��Ҫ�ȵ�һ�ζ��������ݴ� */
        }
				pre_val = temp;
        //printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* ��ʾ�Ѿ����Թ����ڴ����� */
    }
		printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* ��ʾȫ�����Թ����ڴ����� */
}
