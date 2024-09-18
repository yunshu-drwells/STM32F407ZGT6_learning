#include "stm32f4xx_hal.h"
#ifndef __EXTI_H
#define __EXTI_H


extern uint8_t st[2];
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#endif
