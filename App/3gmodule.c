#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Com_Setting.h"



/* Private function prototypes -----------------------------------------------*/
void sendOverTCP(void);
u8 sendATCommtest(const char* cmd, const char* rpl, uint8_t nRN,uint8_t nRN1);
void sendOverTest(void);
const char* AT_CMD[] =  {
                           "AT+CSQ",//signal quality ÷∏¡Ó
                          "AT+QIDEACT=1",
                          "AT+QICSGP=1,1,\"UNINET\",\"\",\"\",1",
                          "AT+QIACT=1",
	
	                        "AT+QSSLCFG=\"sslversion\",1,4",
	                        "AT+QSSLCFG=\"ciphersuite\",1,0XFFFF",
	                        "AT+QSSLCFG=\"seclevel\",1,0",
	
						
							"AT+QGPSLOC?",
	
                          "AT+QNTP=1,\"182.92.12.11\",123",
                          "AT+CCLK?",
                          "AT+QCFG=\"nwscanmode\,0"
//                          "AT+QIACT?",
//                          "AT+QIOPEN=1,0,\"TCP\",\"54.222.186.69\",8705,0,2",
//                          "AT+QISTATE=1,0",
//                          "AT+QISEND=0,7",
//                          "AT+QISEND=0,0",
//                          "AT+QICLOSE=0",
                        };

const char* AT_GPSCMD[] =  {
                        //	"AT+QGPS=1",
                        "AT+QGPSCFG=\"autogps\",1",
                        "AT+QGPSCFG=\"nmeasrc\",1",
                        "AT+QGPSGNMEA=\"GGA\""
                        //"AT+QLOCCFG=\"1\",1",	
                        //"AT+QCELLLOC",
                        };                        
                        
u8 init3GModule(void)
{
	int i=3;
	memset(RxBuffer, 0, 332);
	 RxCount1=0;
	bRecord=0;
	rnCount=0;
	while(i--)
	{	
			if(TRUE == getRSSI(  )) i=0;
  }//STM_RTU_LEDOn(LED7);

  for(i = 1; i < 7; i++)
  {
    while(FALSE == sendATCommand(AT_CMD[i], "OK", 1,0))
    {
//      Delay(0xffffff);
    }
  }

  /* for(i = 0; i < 4; i++)
  {
    while(FALSE == sendATCommand(AT_GPSCMD[i], "OK", 1,0))
    {
//      Delay(0xffffff);
			   return FALSE;
    }
  }*/
  
  initRTC();
  return TRUE;
}

u8 init3GMTest(void)
{
	int i;
 // if(FALSE == getRSSI(  )) return FALSE;
  STM_RTU_LEDOn(LED7);

	 sendATCommtest(AT_CMD[0], "+CSQ: ", 1,3);
  for(i = 1; i < 4; i++)
  {
    while(FALSE == sendATCommtest(AT_CMD[i], "OK", 1,0))
    {
//      Delay(0xffffff);
			   return FALSE;
    }
  }
  
 
  
  initRTC();
  return TRUE;
}

u8 sendATCommand(const char* cmd, const char* rpl, uint8_t nRN,uint8_t nRN1)
{
  strcpy((char*)TxBuffer, cmd);
  strcat((char*)TxBuffer, "\r\n");

  NbrOfDataToTransfer = strlen((char*)TxBuffer);
  myRN = nRN;
  myRN1 = nRN1;
	 USART_Cmd(USART1, ENABLE);
  (void)sendOverTCP(  );
 
  if(0 == strncmp((char*)RxBuffer, rpl, strlen(rpl)))
    return TRUE;
  else
    return FALSE;
}


u8 getRSSI(void)
{
    u8 rst = sendATCommand(AT_CMD[0], "+CSQ: ", 1,3);

    if(TRUE == rst)
    {
        rssi[0] = atoi(strchr((char*)RxBuffer, ' ') + 1);
      
        if(rssi[0]==99) 
        { 
          rst = FALSE;
          QSN_Flag=2;
        } 
			  else if(rssi[0]<6)
				{
					  QSN_Flag=2;
            
				    ///STM_RTU_LEDOff(LED5);
           // STM_RTU_LEDOff(LED7);
           // STM_RTU_LEDOn(LED6);
           
				}
        else if(rssi[0]<10)
        {
            QSN_Flag=1;
        }
				else
        {
            QSN_Flag=0;
				    STM_RTU_LEDOff(LED5);
            STM_RTU_LEDOff(LED7);
            STM_RTU_LEDOn(LED6);
           
				}					
    }

    return rst;
}

u8 getCoordinateFromGPS(float* dPointData)
{
	char* pString;
	
    if(FALSE == sendATCommand(AT_CMD[8], "+QGPSLOC: ", 1,0))
      return FALSE;
	
	pString = strchr((char*)RxBuffer, ',') + 1;
	dPointData[0] = (strtod(pString, &pString) / 100) + 0.188;
	pString = strchr(pString, ',') + 1;
	dPointData[1] = (strtod(pString, NULL) / 100) + 0.007;
	return TRUE;
}

u8 getCurrentTime(char* strTime, int len)
{
    if(FALSE == sendATCommand(AT_CMD[9], "+QNTP: 0,\"20", 3,0))
      return FALSE;
    if(FALSE == sendATCommand(AT_CMD[10], "+CCLK: \"", 1,0))
      return FALSE;

    strncpy(strTime, strchr((char*)RxBuffer, '\"') + 1, len - 4);
    return TRUE;
}

u8 sendPacket(const uint8_t* buf, const char* rpl, uint16_t len)
{
    memcpy(TxBuffer, buf, len);
    NbrOfDataToTransfer = len;
    myRN = 1;
     USART_Cmd(USART1, ENABLE);
    (void)sendOverTCP(  );
     
    if(0 == strncmp((char*)RxBuffer, rpl, strlen(rpl)))
        return TRUE;
    else
        return FALSE;
}


void sendOverTCP(void)
{
	uint32_t delay=0X03000000;
  STM_RTU_LEDOn(LED1);
 // STM_RTU_LEDOff(LED2);
	
  runningFlag = TRUE;
  /* Enable the RTU_COM_3 Transmoit interrupt: this interrupt is generated when the
  RTU_COM_3 transmit data register is empty */
//  USART_ITConfig(RTU_COM_3, USART_IT_TXE, ENABLE);
	
  DMA1_Channel7->CCR &= (uint16_t)(~0x01);
	DMA1_Channel7->CNDTR = NbrOfDataToTransfer;
	DMA1_Channel7->CCR |= 0x01;
	USART1->CR3=1<<7;
	
	
  /* Wait until RTU_COM_3 send the TxBuffer */
  while(runningFlag);

  /* The software must wait until TC=1. The TC flag remains cleared during all data
  transfers and it is set by hardware at the last frame°Øs end of transmission*/
 // while (USART_GetFlagStatus(RTU_COM_3, USART_FLAG_TC) == RESET)
 // {}

  runningFlag = TRUE;
  /* Enable the RTU_COM_3 Receive interrupt: this interrupt is generated when the
  RTU_COM_3 receive data register is not empty */
  USART_ITConfig(RTU_COM_3, USART_IT_RXNE, ENABLE);//Temporary shielding 7.12

  /* Wait until RTU_COM_3 receive the RxBuffer */
	while(runningFlag)
  {
     if(!(delay--))
		 {
			  runningFlag=0;
			  strcpy((char*)RxBuffer, "Error\r\n");
		 } 
	}		
 // xTimerStop(xTimers[1], 100);

  STM_RTU_LEDOff(LED1);
}


/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
u8 Delay(__IO uint32_t nCount)
{
	 uint32_t delay;
  bRecord = TRUE;
  runningFlag = TRUE;
       
  for(; nCount != 0; nCount--)
  {
    if(FALSE == runningFlag)
    {
        if(0 == strncmp((char*)RxBuffer, "DTU:CONFIG", 10))
        {
					  USART_Cmd(USART2, ENABLE);			
            strcpy((char*)TxBuffer, "OK\r\n");
            NbrOfDataToTransfer = 4;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
            while(runningFlag);
					  bRecord = FALSE;
						runningFlag = TRUE;
						myRN = 0;
						bRecord = TRUE;
						memset(RxBuffer, 0, 255);
            return FALSE;
        }
				if(0 == strncmp((char*)RxBuffer, "DTU:TEST", 8))
				{
					   USART_Cmd(USART2, ENABLE);			
						strcpy((char*)TxBuffer, "DTU:TEST OK\r\n");
            NbrOfDataToTransfer = 13;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
					  memset(RxBuffer, 0, 255);
					  bRecord = TRUE;
            while(runningFlag);
					  runningFlag = TRUE;
            return TEST;
				}	
    }
  }
  bRecord = FALSE;
  return TRUE;
}

#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Example of timeout situation management.
  * @param  None.
  * @retval None.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Use application may try to recover the communication by resetting I2C
    peripheral (calling the function I2C_SoftwareResetCmd()) then re-start
    the transmission/reception from a previously stored recover point.
    For simplicity reasons, this example only shows a basic way for errors
    managements which consists of stopping all the process and requiring system
    reset. */

  STM_RTU_LEDOn(LED4);

  /* Block communication and all processes */
  while (1)
  {
  }
}
#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

