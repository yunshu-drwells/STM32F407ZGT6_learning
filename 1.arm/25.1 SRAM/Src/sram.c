#include "sram.h"
#include "usart.h"  //fputc
#include "main.h"  //BUFFER_SIZE

extern uint32_t g_test_buffer[BUFFER_SIZE];  //总计262144*4Byte数据

/**
 * @brief       往SRAM指定地址写入指定长度数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大32bit)
 * @retval      无
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
		//printf("i: %d, write p:%x\n", addr,  SRAM_BASE_ADDR + addr);
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
				//printf("i: %d, write temp %d\n", addr,  *(volatile uint32_t *)(SRAM_BASE_ADDR + addr));
				//printf("g_test_buffer[%d]: %u\n", addr, g_test_buffer[addr]);
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
		/*
		if(addr == 0){
			*pbuf = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
			printf("*pbuf: %d\n", *pbuf);
		}
		*/
	
		//printf("i: %d, read from p:%x\n", addr,  SRAM_BASE_ADDR + addr);
    for (; datalen != 0; datalen--)
    {
        *pbuf = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
				//printf("i %d, read temp %d\n", addr, *pbuf);
				pbuf++;
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
uint32_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1); /* 读取1个字节 */
    return data;
}
#define start 0
#define end 1024
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
    for (i = start*1024; i < 1024 * end; i += 4096){
			  sram_write(&temp, i, 1); //写入的数据是[0:255]
				//HAL_Delay(10);
				temp++;
				//刚写完读，则完全正确
				//sram_read(&pre_val, i, 1);
				//printf("i %d, read temp %d\n", i, pre_val);
    }
		//确保在屏障之前的所有读写操作在屏障之后的读写操作之前完成。
		//__sync_synchronize(); // 全内存屏障
		/*
		pre_val = 0;
		for (j = 0; j < 1024 * 1024; j += 4096){
				//i=65536（16*4096）之前的数据异常
				sram_read(&pre_val, j, 1);
				printf("i %d, read temp %d\n", j, pre_val);
    }
		pre_val = 0;
		printf("after temp++, temp is %d\n", temp);
		temp = 0;
		*/
		//sram_read(&temp, 0, 1);
		//printf("i 0, read temp %d, pre_val %d\n", temp, pre_val);
		//HAL_Delay(1000);
		
    /* 依次读出之前写入的数据,进行校验 */		
    for (i = start*1024; i < 1024 * end; i += 4096)  {
        sram_read(&temp, i, 1);
				//printf("i %d, read temp %d, pre_val %d\n", i, temp, pre_val);
				//printf("%d\n",*(volatile uint8_t *)(g_test_buffer + i/4));
        if (0 != temp && temp <= pre_val){
						printf("Some error occer to SRAM\n");
            break; // 后面读出的数据一定要比第一次读到的数据大 
        }
				pre_val = temp;
        //printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* 显示已经测试过的内存容量 */
    }
		printf("Ex Memory Test:   %dKB\n", (uint16_t)(temp + 1) * 4); /* 显示全部测试过的内存容量 */
		/*
		for (i = 0; i < 1024 * 1024; i += 4096)  {
			printf("%d\n",*(volatile uint8_t *)(g_test_buffer + i/4));
		}
		*/
}
