#ifndef SYSTIMERECORD_H_
#define SYSTIMERECORD_H_
#include "common.h"
//时分秒毫秒：HHMMSSCCCC
int getTimeStandard();

//iTimeout 毫秒为单位
int getTimeMillisecond(int iTimeout, timespec& ts);

int gettimeElapse(uint32_t uiHHMMSSCCCC);



#endif /* TIME_TOOLS_H_ */
