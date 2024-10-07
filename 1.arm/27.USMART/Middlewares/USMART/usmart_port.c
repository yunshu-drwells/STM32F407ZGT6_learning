/**
 ****************************************************************************************************
 * @file        usmart_port.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART ��ֲ�ļ�
 *
 *              ͨ���޸ĸ��ļ�,���Է���Ľ�USMART��ֲ����������
 *              ��:USMART_ENTIMX_SCAN == 0ʱ,����Ҫʵ��: usmart_get_input_string����.
 *              ��:USMART_ENTIMX_SCAN == 1ʱ,��Ҫ��ʵ��4������:
 *              usmart_timx_reset_time
 *              usmart_timx_get_time
 *              usmart_timx_init
 *              USMART_TIMX_IRQHandler
 *
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 *
 * V3.4֮ǰ�汾��ϸ�޸�˵����USMART�ļ����µ�:readme.txt
 *
 * V3.4 20200324
 * 1, ����usmart_port.c��usmart_port.h,���ڹ���USMART����ֲ,�����޸�
 * 2, �޸ı���������ʽΪ: uint8_t, uint16_t, uint32_t
 * 3, �޸�usmart_reset_runtimeΪusmart_timx_reset_time
 * 4, �޸�usmart_get_runtimeΪusmart_timx_get_time
 * 5, �޸�usmart_scan����ʵ�ַ�ʽ,�ĳ���usmart_get_input_string��ȡ������
 * 6, �޸�printf����ΪUSMART_PRINTF�궨��
 * 7, �޸Ķ�ʱɨ����غ���,���ú궨�巽ʽ,������ֲ
 *
 * V3.5 20201220
 * 1���޸Ĳ��ִ�����֧��AC6������
 *
 ****************************************************************************************************
 */
#include "usart.h"  //g_usart_rx_sta��g_usart_rx_buf��g_rx_buffer
#include "usmart.h"
#include "usmart_port.h"
#include "tim.h"  //TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim4;      /* ��ʱ����� */

/**
 * @brief       ��ȡ����������(�ַ���)
 *   @note      USMARTͨ�������ú������ص��ַ����Ի�ȡ����������������Ϣ
 * @param       ��
 * @retval
 *   @arg       0,  û�н��յ�����
 *   @arg       ����,�������׵�ַ(������0)
 */
char *usmart_get_input_string(void)
{
    uint8_t len;
    char *pbuf = 0;
		
    if (g_usart_rx_sta & 0x8000)        /* ���ڽ�����ɣ� */
    {
				//printf("receive down!\n");
        len = g_usart_rx_sta & 0x3fff;  /* �õ��˴ν��յ������ݳ��� */
        g_usart_rx_buf[len] = '\0';     /* ��ĩβ���������. */
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;             /* ������һ�ν��� */
    }

    return pbuf;
}

/* ���ʹ���˶�ʱ��ɨ��, ����Ҫ�������º��� */
#if USMART_ENTIMX_SCAN == 1

/**
 * ��ֲע��:��������stm32Ϊ��,���Ҫ��ֲ������mcu,������Ӧ�޸�.
 * usmart_reset_runtime,�����������ʱ��,��ͬ��ʱ���ļ����Ĵ����Լ���־λһ������.��������װ��ֵΪ���,������޶ȵ��ӳ���ʱʱ��.
 * usmart_get_runtime,��ȡ��������ʱ��,ͨ����ȡCNTֵ��ȡ,����usmart��ͨ���жϵ��õĺ���,���Զ�ʱ���жϲ�����Ч,��ʱ����޶�
 * ֻ��ͳ��2��CNT��ֵ,Ҳ���������+���һ��,���������2��,û������,���������ʱ,������:2*������CNT*0.1ms.��STM32��˵,��:13.1s����
 * ������:USMART_TIMX_IRQHandler��Timer4_Init,��Ҫ����MCU�ص������޸�.ȷ������������Ƶ��Ϊ:10Khz����.����,��ʱ����Ҫ�����Զ���װ�ع���!!
 */

/**
 * @brief       ��λruntime
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ��
 */
void usmart_timx_reset_time(void)
{
    __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE); /* ����жϱ�־λ */
    __HAL_TIM_SET_AUTORELOAD(&htim4, 0XFFFF);      /* ����װ��ֵ���õ���� */
    __HAL_TIM_SET_COUNTER(&htim4, 0);              /* ��ն�ʱ����CNT */
    usmart_dev.runtime = 0;
}

/**
 * @brief       ���runtimeʱ��
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ִ��ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms
 */
uint32_t usmart_timx_get_time(void)
{
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) == SET)  /* �������ڼ�,�����˶�ʱ����� */
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&htim4);
    return usmart_dev.runtime;                                 /* ���ؼ���ֵ */
}

/**
 * @brief       ��ʱ����ʼ������
 * @param       arr:�Զ���װ��ֵ
 *              psc:��ʱ����Ƶϵ��
 * @retval      ��
 */ 
void usmart_timx_init(uint16_t arr, uint16_t psc)
{
    USMART_TIMX_CLK_ENABLE();
    
    htim4.Instance = TIM4;                 /* ͨ�ö�ʱ��4 */
    htim4.Init.Prescaler = psc;                   /* ��Ƶϵ�� */
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;  /* ���ϼ����� */
    htim4.Init.Period = arr;                      /* �Զ�װ��ֵ */
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim4);
    HAL_TIM_Base_Start_IT(&htim4);                /* ʹ�ܶ�ʱ���Ͷ�ʱ���ж� */
    HAL_NVIC_SetPriority(TIM4_IRQn,1,1);                  /* �����ж����ȼ�����ռ���ȼ�3�������ȼ�3 */
    HAL_NVIC_EnableIRQ(TIM4_IRQn);                        /* ����ITM�ж� */ 
		//printf("usmart_timx_init called!\n");
}

//stm32f4xx_it.c->TIM4_IRQHandler
/**
 * @brief       USMART��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
/*
void USMART_TIMX_IRQHandler(void)
{
    if(__HAL_TIM_GET_IT_SOURCE(&htim4,TIM_IT_UPDATE)==SET)// ����ж� 
    {
        usmart_dev.scan();                                   // ִ��usmartɨ�� 
        __HAL_TIM_SET_COUNTER(&htim4, 0);;    // ��ն�ʱ����CNT 
        __HAL_TIM_SET_AUTORELOAD(&htim4, 100);// �ָ�ԭ�������� 
    }
    
    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);// ����жϱ�־λ 
}
*/
#endif
















