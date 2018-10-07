#ifndef __USART_H
#define __USART_H
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART1_SEN_SIZE 				259			//发送数据长度,最好等于
#define USART1_REC_SIZE  				259  		//定义最大接收字节数 200

#define USART3_SEN_SIZE 				259			//发送数据长度,最好等于
#define USART3_REC_SIZE  				259  		//定义最大接收字节数 200

#define USART6_SEN_SIZE 				1400			//发送数据长度,最好等于
#define USART6_REC_SIZE  				1400  		//定义最大接收字节数 200

#define COM1 1
#define COM3 3
#define COM6 6

//收发模式引脚控制
#define RS485_2_RX_TX		PCout(0)	           //485控制.0,接收;1,发送.

typedef struct
{
	u8  	RX_STATION;
	u8  	TX_STATION;
  u8    bRecord;
  u8    mynbase[2];
	char  TX_BUF[USART1_SEN_SIZE];     //接收缓冲,最大USART_REC_LEN个字节. 
	char  RX_BUF[USART1_REC_SIZE];			//发送数据缓冲区
  u16 	RX_len[2];
  u16 	TX_len[2];	
}stuUSARTx;

typedef struct
{
	u8  	RX_STATION;
	u8  	TX_STATION;
  u8    bRecord;
  u8    mynbase[2];
	char  TX_BUF[USART6_SEN_SIZE];     //接收缓冲,最大USART_REC_LEN个字节. 
	char  RX_BUF[USART6_REC_SIZE];			//发送数据缓冲区
  u16 	RX_len[2];
  u16 	TX_len[2];	
}stuUSARTx4G;

extern stuUSARTx stuUSART1;
extern stuUSARTx stuUSART3;
extern stuUSARTx4G stuUSART6;
extern u8 UartRunFlag;

void vUARTConfiguration(u8 COMx,u32 bound,u16 WordLength,u16 StopBits,u16 Parity, FunctionalState Flag);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);
u8 SettinRxPro(u8 res,stuUSARTx *uartx);
u8 SettinRxPro4G(u8 res,stuUSARTx4G *uartx);
//u8 WaitRdy(u8 res,stuUSARTx *uartx);


#endif


