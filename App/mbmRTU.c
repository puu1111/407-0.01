#include "mbmRTU.h"
#include "mqtt.h"
#if MBM_FLIE

#define MBM_DEFAULT_RESPONSE_TIMEOUT  1500
#define MBM_RX_NUM  20
#define dMBMPlayLoad 20
#define dOneNum 90
typedef struct
{
    u8     ubFlag;
    u16    usNTimeOutMS;
    u16    usNTimeLeft;
} xTimerInternalHandle;
xTimerInternalHandle MbmTimerHdls[2];

typedef enum
{
    MBM_STATE_NONE,             /*!< Not yet started. */
    MBM_STATE_SEND,             /*!< Frame will be sent. */
    MBM_STATE_WAITING,          /*!< Waiting for an event. */
    MBM_STATE_DISASSEMBLE,      /*!< Disassembling the frame. */
    MBM_STATE_ERROR,            /*!< An error occurred. */
    MBM_STATE_DONE              /*!< We are done processing the request. */
} eMBMQueryState;


u8 Mbm_RxBuf[MBM_RX_NUM];
u8 MB_rxcnt=0;
u8 MBErrorcnt=0;
u8 eState=0;
stMBM_TaskCycle stMBMTaskCycle;
stPoint_Base  *pstNor=stNorPoint;
stPoint_Base  *pstEvent=stEventPoint;
stConfig_Base stCbase;
stuUSARTx *MBM_COM=&stuUSART3;
                

/* CRC 高位字节值表 */ 
const u8  auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
} ; 

/* CRC低位字节值表*/ 
const u8 auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
} ;


u16 Crc_16(u8 *puchMsg, u16 usDataLen) 
{ 
	u16 uchCRCHi = 0xFF ;              /* 高CRC字节初始化  */ 
	u16 uchCRCLo = 0xFF ;              /* 低CRC 字节初始化 */ 
	u16 temp16;
	u32 uIndex ;                      /* CRC循环中的索引  */ 
	while (usDataLen--)                  /* 传输消息缓冲区   */ 
	{ 
		temp16=*puchMsg++;
		uIndex = uchCRCHi ^ temp16 ; /* 计算CRC          */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return (uchCRCHi << 8 | uchCRCLo) ; 
}

/***************  配置LED用到的I/O口 *******************/
void MBMSerialInit(u8 ucPort, u32 ulBaudRate)
{
	  u16 usTimeoutMS;
	  
    if( ulBaudRate > 19200 )
    {
        usTimeoutMS = 2;
    }
    else
    {
				/* The number of ticks required for a character is given by 
				* xTicksCh = TIMER_TICKS_PER_SECOND * 11 / BAUDRATE
			  * The total timeout is given by xTicksCh * 3.5 = xTicksCh * 7/2.
			  */
        usTimeoutMS = ( u16 ) ( ( 1000UL * 11UL * 7UL ) / ( 2 * ulBaudRate ) );
    }
		MbmTimerHdls[0].ubFlag=0;
		MbmTimerHdls[0].usNTimeOutMS = usTimeoutMS;
		MbmTimerHdls[0].usNTimeLeft = usTimeoutMS;
		MbmTimerHdls[1].ubFlag=0;
		MbmTimerHdls[1].usNTimeOutMS = MBM_DEFAULT_RESPONSE_TIMEOUT;
		MbmTimerHdls[1].usNTimeLeft = MBM_DEFAULT_RESPONSE_TIMEOUT;
}

u8 MBMPro(u16 ucFuncode,u8 ucSlaveAddress, u16 usInputStartAddress, u16 usNInputs,
                        u16 *arubBufferOut)
{
	  u16 CrcData;
		u16 buf_count,temp;
	  u8 i,j=0;
  
	  switch(eState)
		{	
				case MBM_STATE_NONE:
				     	 MBM_COM->TX_BUF[0]=ucSlaveAddress;
			         MBM_COM->TX_BUF[1]=ucFuncode;
				       MBM_COM->TX_BUF[2]=usInputStartAddress>>8;
			      	 MBM_COM->TX_BUF[3]=usInputStartAddress;
				       MBM_COM->TX_BUF[4]=usNInputs>>8;
			      	 MBM_COM->TX_BUF[5]=usNInputs;
				       temp=Crc_16((u8 *)MBM_COM->TX_BUF,6);  
							 MBM_COM->TX_BUF[6]=temp>>8;
							 MBM_COM->TX_BUF[7]=temp;
				       eState=MBM_STATE_SEND;
				       break;
				case MBM_STATE_SEND:    
               MYDMA_Enable(DMA1_Stream3,8);  
                while(stuUSART3.TX_STATION);        
				       //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					     //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
				       MbmTimerHdls[1].ubFlag=1;
				       eState=MBM_STATE_WAITING;
				       break;
        case MBM_STATE_WAITING:
					     break;//有问题
        
        case MBM_STATE_DISASSEMBLE:	
               //memcpy(rxbuf, MBM_COM->RX_BUF, MB_rxcnt-1);
					     switch(MBM_COM->RX_BUF[1])
							 {	 
									 case 1:
									 case 2: 
									 case 5: 	 
											buf_count= MBM_COM->RX_BUF[2];
											CrcData = Crc_16((u8 *)MBM_COM->RX_BUF, buf_count+3);					
											if(CrcData == MBM_COM->RX_BUF[buf_count+4] + (MBM_COM->RX_BUF[buf_count+3] << 8))
											{								
												  arubBufferOut[0]=MBM_COM->RX_BUF[3];
												  MB_rxcnt=0;	
													eState=MBM_STATE_NONE;
													//bsp_DelayMS(110);
													return 1;
											}	
											else// if(MBErrorcnt>5) 
											{
													//MBErrorcnt=0;
													for(i=0;i<buf_count;i+=2)
													{
														  arubBufferOut[j]=0;
															j++;
													}  
													return 1;
											}	 	
									   
									 case 3:
									 case 4: 
									 case 6:
										  buf_count= MBM_COM->RX_BUF[2];
											CrcData = Crc_16((u8 *)MBM_COM->RX_BUF, buf_count+3);					
											if(CrcData == MBM_COM->RX_BUF[buf_count+4] + (MBM_COM->RX_BUF[buf_count+3] << 8))
											{
													for(i=0;i<buf_count;i+=2)
													{
															arubBufferOut[j]=MBM_COM->RX_BUF[i+3]*0x100+MBM_COM->RX_BUF[i+4];
															j++;
													}
												  MB_rxcnt=0;	
													eState=MBM_STATE_NONE;
													return 1;
											}	
                      else
											{
													for(i=0;i<buf_count;i+=2)
													{
														  arubBufferOut[j]=0;
															j++;
													}  
                          eState=MBM_STATE_NONE;
													return 1;
											}	 	
											
                     default: eState=MBM_STATE_SEND;	break;
							 }			 
         case MBM_STATE_ERROR:	
              {
                  if(MBErrorcnt>5) 
                  {
                      MBErrorcnt=0;
                      for(i=0;i<buf_count;i+=2)
                      {
                          arubBufferOut[j]=0;
                          j++;
                      }  
                      eState=MBM_STATE_SEND;
                      return 1;
                  }	 	
                  eState=MBM_STATE_SEND;
              } 
              break;              
		}	
    return 0;		
}	

u8 MBMFun(u8 ucFuncode,u8 ucSlaveAddress, u16 usInputStartAddress, u16 usNInputs,
                        u16 *arubBufferOut)
{
	 u8  eFlag = 0;
	 do
	 {	 
			eFlag=MBMPro(ucFuncode,ucSlaveAddress,usInputStartAddress,usNInputs,arubBufferOut);
	 }
	 while(!eFlag);
	  return eFlag;
}	

void vMBPUSART1ISR(u8 data)
{
   MBM_COM->RX_BUF[MB_rxcnt++]=data;
	 MbmTimerHdls[0].ubFlag=1;
	 MbmTimerHdls[0].usNTimeLeft=MbmTimerHdls[0].usNTimeOutMS+1;
}

void vMBPTimerISR(void)
{
   if(MbmTimerHdls[0].ubFlag) 
	 {
		  MbmTimerHdls[0].usNTimeLeft--;
	    if(!MbmTimerHdls[0].usNTimeLeft)
			{
				   MBErrorcnt=0;
				   MbmTimerHdls[0].usNTimeLeft=MbmTimerHdls[0].usNTimeOutMS;
				   MbmTimerHdls[1].usNTimeLeft=MBM_DEFAULT_RESPONSE_TIMEOUT;
				   MbmTimerHdls[1].ubFlag=0;
				   MbmTimerHdls[0].ubFlag=0;
					 eState=MBM_STATE_DISASSEMBLE;
			}	
	 }
   else if(MbmTimerHdls[1].ubFlag) 
   {
		  MbmTimerHdls[1].usNTimeLeft--;
	    if(!MbmTimerHdls[1].usNTimeLeft)
			{
				  if(MBErrorcnt>5)
					{
					    MbmTimerHdls[1].usNTimeLeft=MBM_DEFAULT_RESPONSE_TIMEOUT;
							MbmTimerHdls[1].ubFlag=0;
							eState=MBM_STATE_ERROR;
						//  MBErrorcnt=0;
					}	
					else
					{  
						  MBErrorcnt++;
							MbmTimerHdls[1].usNTimeLeft=MBM_DEFAULT_RESPONSE_TIMEOUT;
							MbmTimerHdls[1].ubFlag=0;
							eState=MBM_STATE_NONE;
					}	
			}	
	 }		 
}

void MBM_TASKTIM(void)
{
    Tim_CHA(&stMBMTaskCycle.heartflag,&stMBMTaskCycle.heartcnt,stTimTaskCycle.heart);
    Tim_CHA(&stMBMTaskCycle.valueflag,&stMBMTaskCycle.indvaluecnt,stTimTaskCycle.indvalue);
}  

void Vaule_Cha(uint8_t* databuf,uint8_t i,uint16_t *cnt,uint32_t *NegData)
{
    float ftemp;
    switch(pstNor[i].dataType)
    {
        case 1:  databuf[(*cnt)++]=0x02;
                 databuf[(*cnt)++]=NegData[i];
                 break;
        
        case 2:  databuf[(*cnt)++]=0x04;
                 NegData[i]=NegData[i]*(uint32_t)pstNor[i].Afloat+(uint32_t)pstNor[i].Bfloat;		
                 databuf[(*cnt)++]=NegData[i]>>8;
                 databuf[(*cnt)++]=NegData[i];
                
                 break;
        case 3:  databuf[(*cnt)++]=0x03;                 
                 NegData[i]=NegData[i]*(uint32_t)pstNor[i].Afloat+(uint32_t)pstNor[i].Bfloat;		
                 databuf[(*cnt)++]=NegData[i]>>8;
                 databuf[(*cnt)++]=NegData[i];
                 break;
      
        case 4:
        case 6:	 databuf[(*cnt)++]=0x06;
                 //ftemp=NegData[i];     
                 NegData[i]=NegData[i]*(uint32_t)pstNor[i].Afloat+(uint32_t)pstNor[i].Bfloat;		
                 databuf[(*cnt)++]=NegData[i]>>24;
                 databuf[(*cnt)++]=NegData[i]>>16;
                 databuf[(*cnt)++]=NegData[i]>>8;
                 databuf[(*cnt)++]=NegData[i];
                 break;	
        
        case 5:
        case 7:  databuf[(*cnt)++]=0x05;
                 //ftemp=NegData[i];  
                 NegData[i]=NegData[i]*(uint32_t)pstNor[i].Afloat+(uint32_t)pstNor[i].Bfloat;		
                 databuf[(*cnt)++]=NegData[i]>>24;
                 databuf[(*cnt)++]=NegData[i]>>16;
                 databuf[(*cnt)++]=NegData[i]>>8;
                 databuf[(*cnt)++]=NegData[i];
                 break;	
        
        case 8:
        case 9:  databuf[(*cnt)++]=0x33;
                  ftemp=*(float *)(NegData+i);    
                 ftemp=ftemp*pstNor[i].Afloat+pstNor[i].Bfloat;
                 NegData[i]=*(uint32_t *)&ftemp;									
                 databuf[(*cnt)++]=NegData[i]>>24;
                 databuf[(*cnt)++]=NegData[i]>>16;
                 databuf[(*cnt)++]=NegData[i]>>8;
                 databuf[(*cnt)++]=NegData[i];
                 break;	
    }	
}  

void Vaule_Pro(uint8_t* databuf,uint8_t i,uint16_t *cnt,uint32_t *NegData)
{
    
    databuf[(*cnt)++]=0x22;//地址描述 两字接无符号整数  两层地址
    databuf[(*cnt)++]=0x04;
    databuf[(*cnt)++]=pstNor[i].dataDevice>>8;//设备
    databuf[(*cnt)++]=pstNor[i].dataDevice;
    databuf[(*cnt)++]=pstNor[i].dataChannl>>8;//通道
    databuf[(*cnt)++]=pstNor[i].dataChannl;
    databuf[(*cnt)++]=0x04;//时间描述
    databuf[(*cnt)++]=databuf[12];
    databuf[(*cnt)++]=databuf[13];
    databuf[(*cnt)++]=databuf[14];
    databuf[(*cnt)++]=databuf[15]; 
    Vaule_Cha(databuf,i,cnt,NegData);
}

void EventVaule_Cha(uint8_t* databuf,uint8_t i,uint16_t *cnt,uint32_t *NegData)
{
  float ftemp;
   switch(pstEvent[i].dataType)
  {
      case 1:  databuf[(*cnt)++]=0x02;
               databuf[(*cnt)++]=NegData[i];
               break;
      
      case 2:  databuf[(*cnt)++]=0x04;
               ftemp=NegData[i]; 
               NegData[i]=ftemp*pstEvent[i].Afloat+pstEvent[i].Bfloat;						           
               databuf[(*cnt)++]=NegData[i]>>8;
               databuf[(*cnt)++]=NegData[i];
              
               break;
      case 3:  databuf[(*cnt)++]=0x03;
               ftemp=NegData[i];  
               NegData[i]=ftemp*pstEvent[i].Afloat+pstEvent[i].Bfloat;		
               databuf[(*cnt)++]=NegData[i]>>8;
               databuf[(*cnt)++]=NegData[i];
               break;
    
      case 4:
      case 6:	 databuf[(*cnt)++]=0x06;
               ftemp=NegData[i];     
               NegData[i]=ftemp*pstEvent[i].Afloat+pstEvent[i].Bfloat;		
               databuf[(*cnt)++]=NegData[i]>>24;
               databuf[(*cnt)++]=NegData[i]>>16;
               databuf[(*cnt)++]=NegData[i]>>8;
               databuf[(*cnt)++]=NegData[i];
               break;	
      
      case 5:
      case 7:  databuf[(*cnt)++]=0x05;
               ftemp=NegData[i];  
               NegData[i]=ftemp*pstEvent[i].Afloat+pstEvent[i].Bfloat;		
               databuf[(*cnt)++]=NegData[i]>>24;
               databuf[(*cnt)++]=NegData[i]>>16;
               databuf[(*cnt)++]=NegData[i]>>8;
               databuf[(*cnt)++]=NegData[i];
               break;	
      
      case 8:
      case 9:  databuf[(*cnt)++]=0x33;
                ftemp=*(float *)(NegData+i);    
               ftemp=ftemp*pstEvent[i].Afloat+pstEvent[i].Bfloat;
               NegData[i]=*(uint32_t *)&ftemp;									
               databuf[(*cnt)++]=NegData[i]>>24;
               databuf[(*cnt)++]=NegData[i]>>16;
               databuf[(*cnt)++]=NegData[i]>>8;
               databuf[(*cnt)++]=NegData[i];
               break;	
  }
}

void EventVaule_Pro(uint8_t* databuf,uint8_t i,uint16_t *cnt,uint32_t *NegData)
{
   
    databuf[(*cnt)++]=0x22;//地址描述 两字接无符号整数  两层地址
    databuf[(*cnt)++]=0x04;
    databuf[(*cnt)++]=pstEvent[i].dataDevice>>8;//设备
    databuf[(*cnt)++]=pstEvent[i].dataDevice;
    databuf[(*cnt)++]=pstEvent[i].dataChannl>>8;//通道
    databuf[(*cnt)++]=pstEvent[i].dataChannl;
    EventVaule_Cha(databuf,i,cnt,NegData);
    databuf[(*cnt)++]=0x04;//时间描述
    databuf[(*cnt)++]=databuf[12];
    databuf[(*cnt)++]=databuf[13];
    databuf[(*cnt)++]=databuf[14];
    databuf[(*cnt)++]=databuf[15];
    databuf[(*cnt)++]=0x7F;
} 

u16 makeCH395_CRT_DataPack(u8* databuf,u32 *NegData,u8 MSG_TYPE,\
	u8 MSG_NUM)
{
	  u16 i,tempnum,j=0,k=0;  	  
	  //csr.CCID	 
	  u8 tempdata[10]={00,0x01,0x01};  
//		u8 tempCCID[6]={0x75,0x32,0xc9,0x00,0x00,0x01};
	  u32 tempCKS;
	  u16 datalen=0;	 
	  u8 tempCKSlarry[4];

		//DUT_CCID[11]=0x02;
		memcpy(databuf,tempdata,3);
	  memcpy(databuf+5,hexCCID,6);//CCID   
	  databuf[11]=MSG_TYPE;    //消息类型
	  tempCKS=getTimeStamp();//时间戳
	  tempCKSlarry[0]=tempCKS>>24;
	  tempCKSlarry[1]=tempCKS>>16;
	  tempCKSlarry[2]=tempCKS>>8;
	  tempCKSlarry[3]=tempCKS;
	  memcpy(databuf+12,(u8 *)tempCKSlarry,4);
	  databuf[16]=0X01;
	  databuf[17]=0x00;
	  databuf[18]=0X00;
	  databuf[19]=0X00;
		LED_COM2_OFF;
		switch(MSG_TYPE)
	  {	
			  case 0x01://心跳预处理
			  {
              datalen=dMBMPlayLoad;
							databuf[3]=datalen>>8;
							databuf[4]=datalen; 
							return datalen;
        }
				
				case 0x20://示值
			  {				
           j=dMBMPlayLoad+2;
            
				   for(i=0;i<MSG_NUM;i++)
				   {				
              if(!pstNor[i+stCbase.OverNum].filter ||stMBMTaskCycle.valueflag||!csr.filter_flag)
              {                               
                  Vaule_Pro(databuf,i+stCbase.OverNum,&j,NegData);
                  stMBMTaskCycle.valueflag=2;
                  k++;
              }
              else
              {     
                  tempnum=j;
                  Vaule_Pro(databuf,i+stCbase.OverNum,&j,NegData);               
                  if(NegData[i+stCbase.OverNum]!=pstNor[i+stCbase.OverNum].EventVaule)
                  {
                      k++;
                  }  
                  else
                  {
                      j=tempnum;
                  }  
              }   
              pstNor[i+stCbase.OverNum].EventVaule=NegData[i+stCbase.OverNum];
                              
					 }
           
           stCbase.OverNum+=MSG_NUM;
          
           if(k)
           {  
               databuf[dMBMPlayLoad]=0;
               databuf[dMBMPlayLoad+1]=k;//示值条数
               datalen=j;  //字节长度
               databuf[3]=datalen>>8;
               databuf[4]=datalen;             
               return datalen;
           }
           else
           {
               return 0;
           }             
			 } 
			 
			 case 0x1f://事件
			 {
				   j=dMBMPlayLoad+2;
				   for(i=0;i<MSG_NUM;i++)
				   {
              if(pstNor[i+stCbase.OverNum].filter<2)
              {      
                  tempnum=j;                
                  EventVaule_Pro(databuf,i+stCbase.OverNum,&j,NegData);
                  if(NegData[i+stCbase.OverNum]!=pstEvent[i+stCbase.OverNum].EventVaule)
                  {
                     pstEvent[i+stCbase.OverNum].filter=2;
                     k++;
                  } 
                  else if(!pstEvent[i+stCbase.OverNum].filter||!csr.filter_flag||stMBMTaskCycle.valueflag)
                  {
                      k++;
                  } 
                  else if(NegData[i+(stCbase.OverNum)]!=pstEvent[i+stCbase.OverNum].EventVaule||stMBMTaskCycle.valueflag)
                  {
                      k++;
                  }     
                  else
                  {     
                       j=tempnum;
                  }  
              }
              else
              {     
                  tempnum=j;
                  EventVaule_Pro(databuf,i+stCbase.OverNum,&j,NegData);               
                  if(NegData[i+stCbase.OverNum]==pstEvent[i+stCbase.OverNum].EventVaule)
                  {
                      pstNor[i+stCbase.OverNum].filter=1;
                  }  
                  k++;
              }  
              pstEvent[i+stCbase.OverNum].EventVaule=NegData[i+stCbase.OverNum];              
					 }
           stCbase.OverNum+=MSG_NUM;
          // stMBM_TaskCycle.valueflag=0;
           if(k)
           {  
               databuf[dMBMPlayLoad]=0;
               databuf[dMBMPlayLoad+1]=k;//示值条数
               datalen=j;  //字节长度
               databuf[3]=datalen>>8;
               databuf[4]=datalen;             
               return datalen;
           }
           else
           {
               return 0;
           }         
			 }	 
		}		

    return 0;	 
}

void RXValue_Cha(stPoint_Base *pPoints,uint16_t *ReBuffer,uint32_t* usNRegs)
{
    switch(pPoints->regType)
    {
       case 1:
       case 2:	 
       case 5:  *usNRegs=(uint16_t)ReBuffer[0];
                break;		

       case 3:
       case 4:  if(pPoints->datadigit==0xff)
                {		
                    if(pPoints->dataType>3)
                    {																							
                        
                        switch(pPoints->dataType)
                        {
                            case 4:
                            case 5:
                            case 8:	
                                    *usNRegs=ReBuffer[1]; 
                                    *usNRegs+=(ReBuffer[0]<<16);
                                    break;
                            default:
                                    *usNRegs=ReBuffer[1]; 
                                    *usNRegs+=(ReBuffer[0]<<16);
                                    break; 																
                        }	
                    }	
                    else
                    {
                        *usNRegs=ReBuffer[0]; 
                    }	
                }
                else
                {
                    *usNRegs=ReBuffer[0]; 
                  (*usNRegs)=(*usNRegs)&(1<<(pPoints->datadigit))?1:0;
                }	
                break;	
     }  
}   

u8 MBM_Pro(u8 j,u8 k,stPoint_Base *pbase)
{
   u8 i,ucinput,eStatus;
   u16 tempBuf[2];
   u16 num;
  
   LED_COM2_ON;
   for( i = 0; i < k; i++)
   {             
       num=i+j*k;  
       ucinput=pstEvent[num].dataType>3?2:1;
       if(pstEvent[num].regType<50)
       { 
           eStatus=MBMFun(pbase[num].regType,pbase[num].slaveID, pbase[num].startAddr, ucinput,tempBuf);	        
           RXValue_Cha(&pbase[num],tempBuf,(stMBMTaskCycle.regs)+i);
       }
       else
       {
          // usNRegs[i]=Vspeed;
          // RXValue_Cha(&pstEvent[i],tempBuf,usNRegs+i);
       }    
   } 
   return eStatus;   
}  

void MBM_Upload_CHA(u16 typecnt,stPoint_Base *pbase,u8 mestype)
{
   u16 temptxLen;
   u8  j,k; 
  
   stCbase.OverNum=0;
   if(stMBMTaskCycle.valueflag==2) stMBMTaskCycle.valueflag=0;
     
  
   k=typecnt/dOneNum;       
   for(j=0;j<k;j++)
   {       
       if(MBM_Pro(j,dOneNum,pbase))
       {  
          
          temptxLen=makeCH395_CRT_DataPack(stMBMTaskCycle.data,stMBMTaskCycle.regs,mestype,dOneNum);
          if(temptxLen) mqtt_PRO((char *)stMBMTaskCycle.data,temptxLen,1);
       }   
   }  
   k=typecnt%dOneNum;
   if(k)
   {
       j=0;
       if(MBM_Pro(j,k,pbase))
       { 
         	
          temptxLen=makeCH395_CRT_DataPack(stMBMTaskCycle.data,stMBMTaskCycle.regs,mestype,k);
          if(temptxLen) mqtt_PRO((char *)stMBMTaskCycle.data,temptxLen,1);  
       } 
   }  
}  

void MBM_Upload_Pro(void)
{
  //  MBMSerialInit(MBM_SERIAL_PORT, MBM_SERIAL_BAUDRATE, MBM_PARITY);
    u16 temptxLen;
   
    if(stMBMTaskCycle.heartflag)
    {   
         temptxLen=makeCH395_CRT_DataPack(stMBMTaskCycle.data,stMBMTaskCycle.regs,ZX_HeartFlag,ucEventcnt);
         mqtt_PRO((char *)stMBMTaskCycle.data,temptxLen,1);
         stMBMTaskCycle.heartflag=0;
    }
    else
    {  
       
        MBM_Upload_CHA(ucEventcnt,pstEvent,ZX_EventFlag);
        MBM_Upload_CHA(ucNorcnt,pstNor,ZX_VauleFlag);
    }       
}
#endif    
