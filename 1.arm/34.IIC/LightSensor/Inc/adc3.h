#ifndef __ADC3_H
#define __ADC3_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"


/******************************************************************************************/
/* ADC 定义 */

#define ADC_ADCX                            ADC3
#define ADC3_CHY_CLK_ENABLE()               do{ __HAL_RCC_ADC3_CLK_ENABLE(); }while(0)      /* ADC1 时钟使能 */

/******************************************************************************************/

void adc3_init(void);                           /* ADC3初始化 */
void adc3_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime);    /* ADC3通道设置 */
uint32_t adc3_get_result(uint32_t ch);          /* 获得某个通道值 */
uint32_t adc3_get_result_average(uint32_t ch, uint8_t times);                                       /* 得到某个通道给定次数采样的平均值 */

#endif
