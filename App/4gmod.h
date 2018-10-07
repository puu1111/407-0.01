#ifndef _4gmod_H
#define _4gmod_H

#include "sys.h"

void Net_TIM(void);
void GetNetTim(u8 flag);

u16 getRSSI(void);
u8 Init4GMod(void);
u8 WaitRdy(const char* rpl);
u8 sendPacket(const char* buf, const char* rpl, u16 len);
u8 sendATCommand(const char* cmd, const char* rpl, u8 nRN,u8 nRN1);

#endif
