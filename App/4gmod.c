#include "4gmod.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define dNetTIMCmp 43200000
extern u8 temprx[200];
typedef enum
{
    eQsp_HIGH=0, 
    eQsp_LOW,
	  eQsp_Eor=2,
}EQspVaule;

typedef struct {  
    u8  NetTIMflag;
    u32 NetTIMcnt;//心跳   
}stNetTIM_TaskCycle;

stNetTIM_TaskCycle stNetTIMTaskCycle;

const char* AT_CMD[] =  {
                           "AT+CSQ",//signal quality 指令
                          "AT+QIDEACT=1",
                          "AT+QICSGP=1,1,\"UNINET\",\"\",\"\",1",
                          "AT+QIACT=1",
	
	                        "AT+QSSLCFG=\"sslversion\",1,4",
	                        "AT+QSSLCFG=\"ciphersuite\",1,0XFFFF",
	                        "AT+QSSLCFG=\"seclevel\",1,0",
	                        "AT+QGPSLOC?",
	
                          "AT+QNTP=1,\"182.92.12.11\",123",
                          "AT+CCLK?",
                      
                        };

const char* AT_GPSCMD[] =  {
                        //	"AT+QGPS=1",
                        "AT+QGPSCFG=\"autogps\",1",
                        "AT+QGPSCFG=\"nmeasrc\",1",
                        "AT+QGPSGNMEA=\"GGA\""
                        //"AT+QLOCCFG=\"1\",1",	
                        //"AT+QCELLLOC",
                        };  

u8 QSN_Flag=0; 
stuUSARTx4G *MOD_4G=&stuUSART6;
                        
u16 getRSSI(void)
{
    u8 rst = sendATCommand(AT_CMD[0], "+CSQ: ", 1,3);
    u16 qsp_vaule;
    if(TRUE == rst)
    {
        qsp_vaule = atoi(strchr((char*)MOD_4G->RX_BUF, ' ') + 1);  
        if(qsp_vaule==99) 
        { 
            rst = FALSE;
            QSN_Flag=eQsp_Eor;
        } 
			  else if(qsp_vaule<6)
				{
					  QSN_Flag=eQsp_Eor;   
				}
        else if(qsp_vaule<10)
        {
            QSN_Flag=eQsp_LOW;
        }
				else
        {
            QSN_Flag=eQsp_HIGH;
            LED_COM1_OFF;
            LED_COM2_OFF;
            LED_STRLENTH_ON;        
				}					
    }
    return qsp_vaule;
}                        
                        
void sendOverTCP(u16 len)
{
	  uint32_t delay=0X09000000;
    LED_COM1_ON;
    
    MYDMA_Enable(DMA2_Stream6,len);
    MOD_4G->RX_STATION=1;
    while(MOD_4G->TX_STATION);

    
    while(MOD_4G->RX_STATION)/* Wait until RTU_COM_6 receive the RxBuffer */
    {
       if(!(delay--))
       {
          MOD_4G->RX_STATION=0;
          strcpy((char*)MOD_4G->RX_BUF, "Error\r\n");
       } 
    }		
    LED_COM1_OFF;
}

u8 sendPacket(const char* buf, const char* rpl, u16 len)
{
    memcpy(MOD_4G->TX_BUF, buf, len);
    MOD_4G->mynbase[0] = 1;
    MOD_4G->mynbase[1] =0; 
    sendOverTCP(len);  
   // memset(temprx,0,sizeof(temprx));
    if(0 == strncmp((char*)MOD_4G->RX_BUF, rpl, strlen(rpl)))
        return TRUE;
    else
        return FALSE;
    
}

u8 sendATCommand(const char* cmd, const char* rpl, u8 nRN,u8 nRN1)
{
   u16 len= 0;
   strcpy((char*)MOD_4G->TX_BUF, cmd);
   strcat((char*)MOD_4G->TX_BUF, "\r\n");
   len=strlen((char*)MOD_4G->TX_BUF);
   MOD_4G->mynbase[0] = nRN;
   MOD_4G->mynbase[1] =nRN1; 
   sendOverTCP(len);
   //memset(temprx,0,sizeof(temprx));
  if(0 == strncmp((char*)MOD_4G->RX_BUF, rpl, strlen(rpl)))
    return TRUE;
  else
    return FALSE;
}

u8 getCurrentTime(char* strTime, s16 len)
{
    if(FALSE == sendATCommand(AT_CMD[8], "+QNTP: 0,\"20", 3,0))
      return FALSE;
    if(FALSE == sendATCommand(AT_CMD[9], "+CCLK: \"", 1,0))
      return FALSE;

    strncpy(strTime, strchr((char*)MOD_4G->RX_BUF, '\"') + 1, len - 4);
    return TRUE;
}

void GetNetTim(u8 flag)
{
   char timestr[21];
  
   if(flag)
   {
      if(stNetTIMTaskCycle.NetTIMflag)
      {
          getCurrentTime(timestr, 21);
          getRealTimeStamp(timestr);
      }  
   }  
   else
   {
      getCurrentTime(timestr, 21);
      getRealTimeStamp(timestr);
   }  
}  

u8 Init4GMod(void)
{
    u8 i=3;

    stNetTIMTaskCycle.NetTIMcnt=0;
    stNetTIMTaskCycle.NetTIMflag=0;
    MOD_4G->bRecord =0;//串口接收字符串作用
  
    while(i--)
    {	
        if(getRSSI(  )) i=0;
    }
    
    for(i = 1; i < 7; i++)
    {
        while(FALSE == sendATCommand(AT_CMD[i], "OK", 1,0))
        {
 
        }
    }
    
    GetNetTim(0);
    sendATCommand("ATE0", "OK", 1,0);
    return TRUE;
}



void Net_TIM(void)
{  
   Tim_CHA(&stNetTIMTaskCycle.NetTIMflag,&stNetTIMTaskCycle.NetTIMcnt,dNetTIMCmp);    
}

u8 WaitRdy(const char* rpl)
{
   //uint32_t delay=0X0c000000;
   
   if(!MOD_4G->RX_STATION)
   {  
      MOD_4G->bRecord=0;
      MOD_4G->mynbase[0] =1;
      MOD_4G->mynbase[1] =0;
      MOD_4G->RX_STATION=1;
   }  
//   /*while(MOD_4G->RX_STATION)/* Wait until RTU_COM_6 receive the RxBuffer */
//   {
//    
//   }	*/
//	
   if(0 == strncmp((char*)MOD_4G->RX_BUF, rpl, strlen(rpl)))
    return TRUE;
   else
    return FALSE;
}  


