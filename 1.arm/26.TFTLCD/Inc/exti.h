#include "main.h"  //LEDx、KEYx引脚定义 cur_brightness、str
#include "usart.h"  //fputc
#include "lcd.h"  //lcd_display_off、lcd_set_backlight
#include "stdint.h" //
#include "delay.h"  //delay_ms

extern uint8_t cur_brightness;
extern uint8_t* str[23];
/**
 * @brief       中断服务程序中需要做的事情
                在HAL库中所有的外部中断服务函数都会调用此函数
 * @param       GPIO_Pin:中断引脚号
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(20);      /* 消抖 */

		switch(GPIO_Pin){
			case KEY0_Pin:
				/*key0*/
				if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin)){
					//HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET); //关闭背光
					lcd_set_backlight_by_pwm(0x00); // 设置占空比为0，关闭背光
				  lcd_display_off();  //关闭显示		
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);  //LED1灭
				}else{
					//HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);//打开背光
					lcd_set_backlight_by_pwm(0xFF); // 设置占空比为255，开启背光最亮
					lcd_display_on();  //打开显示
					HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  //LED1亮
				}
				break;
			case  KEY1_Pin:
				cur_brightness = lcd_get_backlight_by_pwm(); //获取当前背光值
				if(cur_brightness > 5){
					cur_brightness -= 5;
					lcd_set_backlight_by_pwm(cur_brightness); // 设置占空比降低，调暗背光
					sprintf((char*)str, "current brightness:%3u", cur_brightness);
					lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示LCD ID */
				}
				break;
			case  KEY2_Pin:
				cur_brightness = lcd_get_backlight_by_pwm(); //获取当前背光值
				if(cur_brightness < 255){
					cur_brightness += 5;
					lcd_set_backlight_by_pwm(cur_brightness); // 设置占空比增大，调亮背光
					sprintf((char*)str, "current brightness:%3u", cur_brightness);
					lcd_show_string(10, 150, 240, 16, 16, (char *)str, RED); /* 显示LCD ID */
				}
				break;
			default:
				break;
		}
}
