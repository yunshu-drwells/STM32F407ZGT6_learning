#include "main.h"  //LEDx��KEYx���Ŷ��� cur_brightness��str
#include "usart.h"  //fputc
#include "lcd.h"  //lcd_display_off��lcd_set_backlight
#include "stdint.h" //
#include "delay.h"  //delay_ms

extern uint8_t cur_brightness;
extern uint8_t* str[23];
/**
 * @brief       �жϷ����������Ҫ��������
                ��HAL�������е��ⲿ�жϷ�����������ô˺���
 * @param       GPIO_Pin:�ж����ź�
 * @retval      ��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(20);      /* ���� */

		switch(GPIO_Pin){
			case KEY0_Pin:
				/*key0*/
				if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin)){
					//HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET); //�رձ���
					lcd_set_backlight_by_pwm(0x00); // ����ռ�ձ�Ϊ0���رձ���
				  lcd_display_off();  //�ر���ʾ		
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);  //LED1��
				}else{
					//HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);//�򿪱���
					lcd_set_backlight_by_pwm(0xFF); // ����ռ�ձ�Ϊ255��������������
					lcd_display_on();  //����ʾ
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1��
				}
				break;
			case  KEY1_Pin:
				cur_brightness = lcd_get_backlight_by_pwm(); //��ȡ��ǰ����ֵ
				if(cur_brightness > 5){
					cur_brightness -= 5;
					lcd_set_backlight_by_pwm(cur_brightness); // ����ռ�ձȽ��ͣ���������
					sprintf((char*)str, "current brightness:%3u", cur_brightness);
					lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* ��ʾLCD ID */
				}
				break;
			case  KEY2_Pin:
				cur_brightness = lcd_get_backlight_by_pwm(); //��ȡ��ǰ����ֵ
				if(cur_brightness < 255){
					cur_brightness += 5;
					lcd_set_backlight_by_pwm(cur_brightness); // ����ռ�ձ����󣬵�������
					sprintf((char*)str, "current brightness:%3u", cur_brightness);
					lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* ��ʾLCD ID */
				}
				break;
			default:
				break;
		}
}
