#include "dialog.h"//包含window对话框 头文件
#include "include_dlg.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_Fontfont;

void MainTask(void)

{

	GUI_Init();                     //初始化emWin/ucGUI
  GUI_UC_SetEncodeUTF8();         //使能utf-8方案

    //GUI_SetFont(&GUI_Fontfont);  // 设置字体
    //GUI_DispString("主灯");
    //GUI_SetFont(GUI_FONT_32_ASCII);  // 设置字体
    //GUI_DispString("233");

	//CreateWindow();                 //创建窗体,父窗体是桌面背景
	CreateWindowMain();
	//CreateTemperature();  //测试Temperature子窗口
	//CreateHumidity();  //测试Humidity子窗口
	//CreateBrightnesss();  //测试Brightnesss子窗口
	//CreateAlarm();  //测试Alarm子窗口
	//CreateFan();  //测试Fan子窗口
	//CreateLightingMaster();  //测试LightingMaster子窗口
	//CreateSpotlight();  //测试Spotlight子窗口
	while(1) {GUI_Delay(20);}       //调用GUI_Delay函数延时20MS(最终目的是调用GUI_Exec()函数)

}
