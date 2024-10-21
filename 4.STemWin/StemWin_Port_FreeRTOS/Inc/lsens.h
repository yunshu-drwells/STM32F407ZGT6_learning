#ifndef __LSENS_H
#define __LSENS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/******************************************************************************************/
/* 光敏传感器对应ADC3的输入引脚和通道 定义 */

#define LSENS_ADC3_CHX_GPIO_PORT            GPIOF
#define LSENS_ADC3_CHX_GPIO_PIN             GPIO_PIN_7
#define LSENS_ADC3_CHX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)     /* PF口时钟使能 */

#define LSENS_ADC3_CHX                      ADC_CHANNEL_5                                   /* 通道Y,  0 <= Y <= 18 */ 

/******************************************************************************************/

void lsens_init(void);          /* 初始化光敏传感器 */
uint8_t lsens_get_val(void);    /* 读取光敏传感器的值 */
#endif
