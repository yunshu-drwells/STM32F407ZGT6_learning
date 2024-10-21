#include "GUI.h"
//#include "lcd.h"
#include "touch.h"  //tp_dev、tp_read_xy2


void GUI_TOUCH_X_ActivateX(void) 
{

}

void GUI_TOUCH_X_ActivateY(void)
{

}

//获取X坐标AD值
int  GUI_TOUCH_X_MeasureX(void) 
{
	int32_t xvalue;
	uint16_t x,y; 
	
	/* 判断电阻屏还是电容屏 */
	if ((tp_dev.touchtype & 0x80) == 0)
	{
			tp_dev.scan(0);
			
			if (tp_dev.sta & TP_PRES_DOWN)                              /* 触摸屏被按下 */
			{
					/* 电阻屏出现触摸没有那么顺畅的问题，请把touch.c文件下的TP_ERR_RANGE宏定义设置大一点 */
					tp_read_xy2(&x,&y); 
					return (4095 - x); 
					//return tp_read_xoy(0X90);                             /* CMD_RDX=0XD0  */
			}
	}
	else        /* 电容屏 */
	{
			tp_dev.scan(0);
			xvalue = tp_dev.x[0];
			return xvalue;
	}
	return 0;
}

//获取Y坐标AD值
int  GUI_TOUCH_X_MeasureY(void) 
{	
	int32_t yvalue;
	uint16_t x,y;
	
	/* 判断电阻屏还是电容屏 */
	if ((tp_dev.touchtype & 0x80) == 0)
	{
			/* 解决快速按下的鼠标乱屏问题 */
			tp_dev.scan(0);
			
			if (tp_dev.sta & TP_PRES_DOWN)                              /* 触摸屏被按下 */
			{
					/* 电阻屏出现触摸没有那么顺畅的问题，请把touch.c文件下的TP_ERR_RANGE宏定义设置大一点 */
					tp_read_xy2(&x,&y); 
					return y; 
					//return tp_read_xoy(0XD0);                             /*CMD_RDX=0XD0  */
			}
	}
	else        /* 电容屏 */
	{
			yvalue = tp_dev.y[0];
			return yvalue;
	}
	return 0;
}
