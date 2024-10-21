#ifndef _DLG_H
#define _DLG_H

#include "DIALOG.h"

//将每个DLG.c中的绘制函数声明在这里

extern WM_HWIN CreateWindowMain(void);
extern WM_HWIN CreateLightingMaster(void);
extern WM_HWIN CreateSpotlight(void);
extern WM_HWIN CreateTemperature(void);
extern WM_HWIN CreateHumidity(void);
extern WM_HWIN CreateBrightnesss(void);
extern WM_HWIN CreateFan(void);
extern WM_HWIN CreateAlarm(void);

#endif
