#include "main.h"  //LEDx��KEYx���Ŷ���
#include "usart.h"  //fputc

extern SD_HandleTypeDef hsd;
//const uint8_t str[] = {"STM32 SDIO TEST!"};
#define BUF_SIZE 512
uint8_t w_buffer[BUF_SIZE] = {"STM32 SDIO TEST!"};
uint8_t r_buffer[BUF_SIZE];
int8_t i;
#define BLOCK_NUM 1

uint8_t DMA_SEND_OK, DMA_RCV_OK;  //���ͺͽ���״̬��־λ

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
				printf("KEY0 press,start write SD! \n");
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
				break;	
			case  KEY1_Pin:
				/*key1*/
				printf("KEY1 press,start read SD! \n");
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
				//printf("State:%d\n", HAL_SD_GetCardState(&hsd));
				break;
			case  KEY2_Pin:
				/*key2*/
				printf("KEY2 press,start erase SD! \n");
				//������1
				if(HAL_OK == HAL_SD_Erase(&hsd, BLOCK_NUM, BLOCK_NUM+1)){  //[BlockStartAdd, BlockEndAdd]
					//���������л����HAL_SD_CARD_PROGRAMMING״̬;����������ɺ�����HAL_SD_CARD_TRANSFER״̬
					while(HAL_SD_CARD_TRANSFER != HAL_SD_GetCardState(&hsd)){  //��ѭ���ȴ��������
					  HAL_Delay(1);// ���һ��С�ӳ٣�����æ�ȴ�
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
