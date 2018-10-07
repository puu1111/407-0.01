#ifndef SETTING_H_INCLUDED
#define SETTING_H_INCLUDED

#include "sys.h"

struct serial {
	 // int32_t workmode;
    int32_t baudRate;
    int8_t parity;
    int8_t wordLength;
    int8_t stopBits;
};


typedef struct{
	  uint8_t regAllNum;
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

typedef struct
{
    uint8_t   TxBuf[255];
	  uint8_t   RxBuf[255];
    uint8_t   TxNum;
    uint8_t   OverNum;
} stConfig_Base;

typedef enum
{
	  Heart_STATE_NONE,
	  Heart_STATE_UP,
    Heart_STATE_ON,             /*!< Not yet started. */
    Heart_STATE_WAITING,          /*!< Waiting for an event. */
    Heart_STATE_ERROR,            /*!< An error occurred. */
} eHeartQueryState;
/*struct point {
    int8_t regType;
    int8_t slaveID;
    int16_t startAddr;
	  int8_t datadigit;
    int8_t dataType;
	  uint16_t dataDevice;
	  uint16_t dataChannl;
	  char  pointCode;
    //char pointCode[8];
};*/
extern uint8_t ucHeartSendFlag;
extern uint8_t QSN_Flag;
extern uint8_t ucSendVaule_FLAG;
extern uint8_t QSN_Flag;
extern uint8_t ucHeart_FLAG;
extern uint8_t hCCID[6];
extern stConfig_Base stConfigBase;
extern uint8_t ucNorcnt;
extern uint8_t	ucEventcnt;
extern stPoint_Base  stNorPoint[25];
extern stPoint_Base  stEventPoint[25];
extern uint8_t RxBuffer[];
extern BOOL runningFlag;
extern uint8_t myRN;
extern uint8_t myRN1;
extern BOOL bRecord;

extern struct serial srl;
extern struct cloudServer csr;
extern struct point* pPoints[];
extern uint8_t counter;
void TCP_Config_TimPro(void);
uint8_t ReceiveSetting(void);
void LoadSetting(void);

#endif /* SETTING_H_INCLUDED */
