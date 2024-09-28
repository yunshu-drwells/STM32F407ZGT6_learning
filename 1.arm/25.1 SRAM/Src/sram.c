#include "sram.h"
#include "usart.h"  //fputc

/**
 * @brief       往SRAM指定地址写入指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大32bit)
 * @retval      无
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
 * @brief       从SRAM指定地址读取指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大32bit)
 * @retval      无
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}

/*******************测试函数**********************************/

/**
 * @brief       测试函数 在SRAM指定地址写入1个字节
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       data    : 要写入的字节
 * @retval      无
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1); /* 写入1个字节 */
}

/**
 * @brief       测试函数 在SRAM指定地址读取1个字节
 * @param       addr    : 开始读取的地址(最大32bit)
 * @retval      读取到的数据(1个字节)
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1); /* 读取1个字节 */
    return data;
}

/**
 * @brief       外部内存测试(最大支持1M字节内存测试)
 * @param       无
 * @retval      无
 */
void fsmc_sram_test()
{
    uint32_t i = 0;
    uint8_t temp = 0;
    uint8_t pre_val = 0; /* 在地址0读到的数据 */

	/* 每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节 */
    for (i = 0; i < 1024 * 1024; i += 4096){
			  sram_write(&temp, i, 1); //写入的数据是[0:255]
        temp++;
    }

    /* 依次读出之前写入的数据,进行校验 */
    for (i = 0; i < 1024 * 1024; i += 4096)  {
        sram_read(&temp, i, 1);

        if (0 != temp && temp <= pre_val){
						printf("Some error occer to SRAM\n");
            break; /* 后面读出的数据一定要比第一次读到的数据大 */
        }
				pre_val = temp;
        //printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* 显示已经测试过的内存容量 */
    }
		printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* 显示全部测试过的内存容量 */
}
