#ifndef _MBMRTU_H
#define _MBMRTU_H

#include "sys.h"


typedef struct {
    u8  data[1200];
    u8  heartflag;
    u8  valueflag;
    u32 heartcnt;//心跳
    u32 indvaluecnt;//上传周期
    u32 regs[500];
}stMBM_TaskCycle;

extern stMBM_TaskCycle stMBMTaskCycle;
void MBM_Upload_Pro(void);
void MBM_TASKTIM(void);
void vMBPTimerISR(void);
void vMBPUSART1ISR(u8 data);
void MBMSerialInit(u8 ucPort, u32 ulBaudRate);

                       
#endif
