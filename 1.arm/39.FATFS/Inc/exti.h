#include "main.h"  //LEDx��KEYx���Ŷ���
#include "usart.h"  //fputc
#include "norflash.h"  //norflash_write
#include "lcd.h"  //lcd_show_string
#include "mymalloc.h"  //mymalloc��myfree

//norflash���
const uint8_t g_text_buf[] = {"STM32 SPI TEST!"}; /* Ҫд��FLASH���ַ������� */
#define TEXT_SIZE sizeof(g_text_buf) /* TEXT�ַ������� */
uint8_t datatemp[TEXT_SIZE]; 
uint32_t flashsize = 16 * 1024 * 1024; /* FLASH ��СΪ16M�ֽ� */;

//SD���
extern SD_HandleTypeDef hsd;
//const uint8_t str[] = {"STM32 SDIO TEST!"};
#define BUF_SIZE 512
uint8_t w_buffer[BUF_SIZE] = {"STM32 SDIO TEST!"};
uint8_t r_buffer[BUF_SIZE];
int8_t i;
#define BLOCK_NUM 1

uint8_t DMA_SEND_OK, DMA_RCV_OK;  //���ͺͽ���״̬��־λ

//SRAM���
uint8_t *p;
uint8_t sramx = 0;                  /* Ĭ��Ϊ�ڲ�sram */
uint8_t paddr[20];                  /* ���P Addr:+p��ַ��ASCIIֵ */
uint16_t memused = 0;
int8_t block_size = 0;
/**
 * @brief       �жϷ����������Ҫ��������
                ��HAL�������е��ⲿ�жϷ�����������ô˺���
 * @param       GPIO_Pin:�ж����ź�
 * @retval      ��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(10);      /* ���� */
		switch(GPIO_Pin){
			case KEY0_Pin:
				/*key0*/
				printf("Key0 press, Test SD!\n");
				printf("start write SD! \n");
				DMA_SEND_OK = 0;  //�����־λ
							
				/* �ӵ�����1�����ַ����ʼ,д��һ���顣��ʱʱ��1000ms */
				if(HAL_OK == HAL_SD_WriteBlocks_DMA(&hsd, w_buffer, BLOCK_NUM, 1)){
					//д�����л����HAL_SD_CARD_PROGRAMMING״̬;д������ɺ�����HAL_SD_CARD_TRANSFER״̬
					while(!DMA_SEND_OK || HAL_SD_CARD_TRANSFER != HAL_SD_GetCardState(&hsd)){ //��ѭ���ȴ�DMA�������
						HAL_Delay(1);// ���һ��С�ӳ٣�����æ�ȴ�
          }
					printf("Write SD successed! \n");
				}else{
					printf("Write failed, something error occured!\n");
				}
				printf("start read SD! \n");
				DMA_RCV_OK = 0;
				/* �ӵ�����1�����ַ����ʼ,��һ���顣��ʱʱ��1000ms*/
				if(HAL_OK == HAL_SD_ReadBlocks_DMA(&hsd, r_buffer, BLOCK_NUM, 1)){
					//ͨ������£���ȡ����ǰ���Լ���ȡ�����п���״̬һֱ����HAL_SD_CARD_TRANSFER
					while(!DMA_RCV_OK){
						HAL_Delay(1);// ���һ��С�ӳ٣�����æ�ȴ�
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
				lcd_show_string(30, 230, 200, 16, 16, "SD read something like: ", BLUE);
				lcd_show_string(30+13*15, 230, 209, 16, 16, (char *)r_buffer, BLUE);     /* ��ʾ���� */
				//printf("State:%d\n", HAL_SD_GetCardState(&hsd));
				break;
			case  KEY1_Pin:
				/*key1*/
				printf("Key1 press, Test norflash!\n");
				printf("Start write W25Q128! \n");
				norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);  /* �ӵ�����100����ַ����ʼ,д��SIZE���ȵ����� */
				printf("write W25Q128 finished! \n");
				printf("Start read W25Q128!\n");  /* �ӵ�����100����ַ����ʼ,����SIZE���ֽ� */
				norflash_read(datatemp, flashsize - 100, TEXT_SIZE);
				printf("read something like: %s\n", (char *)datatemp);
				lcd_show_string(30, 250, 230, 16, 16, "Norflash read something like: ", BLUE);
				lcd_show_string(30+16*15, 250, 209, 16, 16, (char *)datatemp, BLUE);     /* ��ʾ���� */
				break;
			case  KEY2_Pin:
				printf("SRAM start malloc! \n");
				p = mymalloc(sramx, 2048);  /* ����2K�ֽ�,��д������,��ʾ��lcd��Ļ���� */
				if (p != NULL){
						sprintf((char *)p, "Memory Malloc Test%03d", block_size++);            /* ��pд��һЩ���� */
						lcd_show_string(30, 270, 209, 16, 16, (char *)p, BLUE);     /* ��ʾP������ */
				}
				sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)p);
				lcd_show_string(30, 290, 209, 16, 16, (char *)paddr, BLUE); /* ��ʾp�ĵ�ַ */
				
				lcd_show_string(30, 310, 200, 16, 16, "SRAMIN   USED:", BLUE);
				memused = my_mem_perused(SRAMIN);
				sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
				lcd_show_string(30 + 112, 310, 200, 16, 16, (char *)paddr, BLUE);   /* ��ʾ�ڲ��ڴ�ʹ���� */
				//printf("SRAM start free! \n");
				//myfree(sramx, p);   /* �ͷ��ڴ� */
        //p = 0;              /* ָ��յ�ַ */
				break;
			default:
				break;
		}
}
