#ifndef _COM_SETTING_H
#define _COM_SETTING_H

#include "sys.h"

 struct serial {
    int32_t baudRate;
    int8_t parity;
    int8_t wordLength;
    int8_t stopBits;
};


typedef struct{
	 
    int8_t regType;
    int8_t slaveID;
    int16_t startAddr;
	  uint8_t datadigit;
	  uint8_t dataType;
    uint16_t dataDevice;
	  uint16_t dataChannl;
	  float Afloat;
	  float Bfloat;
	  uint8_t filter;//过滤值为2时表示进入非正常情况
	  uint32_t EventVaule;
	  uint8_t None;
}stPoint_Base;

struct cloudServer {
	  char CCID[12];
    uint8_t filter_flag;
	  uint16_t heartbeat_cycle ;
	  uint16_t indvalue_cycle ;
    char name[52];
    int16_t port;
    char username[33];
    char password[96];
  
};

typedef struct {
    u32 heart;//心跳
    u32 indvalue;//上传周期
  
}stTim_TaskCycle;

typedef struct
{
    uint8_t   TxNum;
    uint8_t   OverNum;
} stConfig_Base;             
    
extern u8 hexCCID[6];
extern u16 ucNorcnt;
extern u16 ucEventcnt;
extern struct serial srl;
extern struct cloudServer csr;
extern stTim_TaskCycle  stTimTaskCycle;
extern stConfig_Base stConfigBase;
extern stPoint_Base  stNorPoint[];
extern stPoint_Base  stEventPoint[];
uint8_t ReceiveSetting(void);
void LoadSetting(void);
u8 ConFigWait(void);

#endif
