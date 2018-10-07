/*******RE_LED_STRLENTH****************************************************************
文件名称：main.C
功    能：TCP客户端模式收发测试，基于UC0S II操作系统
实验平台：基于STM32F407VET6 开发板
库版本  ：V1.0.0 
***********************************************************************/
#include "sys.h"
#include "mqtt.h"

stSys_Base Sys_Base;

void SysInitTim(void)
{
    static u16 timcnt=0;
    if(!Sys_Base.InitTim)
    {
        timcnt++;
        if(timcnt>20000)
        {
            timcnt=0;
            Sys_Base.InitTim=TRUE;
        }
    }   
}  

void Init_Base(void)
{
   stMBMTaskCycle.valueflag=1;
   stuUSART1.bRecord=1;
   stuUSART6.bRecord=1;
   stuUSART6.RX_STATION = 0;		
   Sys_Base.InitTim=0;
   Sys_Base.ProSta=0;
   stMBMTaskCycle.heartflag=0;
   stMBMTaskCycle.valueflag=0;
   	LED_POWER_ON;
    LED_COM1_ON;
    LED_COM2_ON;
    LED_STRLENTH_ON;
    LED_ERR_ON; 
  // USART_Cmd(USART6, DISABLE);
}  

void mqtt_PRO(char *dataBuf,uint16_t tempTxLen,uint8_t simflag)
{
	  static  uint8_t Status3G=0; 
	  
		if (simflag)
		{		
				Status3G=mqtt_pub(dataBuf,tempTxLen);				 
				//if(ucHeart_FLAG==Heart_STATE_ERROR)
				//ucHeart_FLAG=Heart_STATE_UP;			
		}	
		if(Status3G==1)
		{										
				LED_ERR_OFF;
        GetNetTim(1);//NTF 网络对时
		}
		else
		{       								
				LED_ERR_ON;
        while(!Status3G)
        {	          
            /*USART_ITConfig(RTU_COM_2, USART_IT_RXNE, DISABLE);
            STM_RTU_LEDOff(RST);
            bsp_DelayMS(300);
            STM_RTU_LEDOn(RST);
            bsp_DelayMS(8000);
            init3GModule(  );*/
            Status3G=mqtt_pub(dataBuf,tempTxLen);                 
        }    
        // USART_ITConfig(RTU_COM_2, USART_IT_RXNE, ENABLE);
		}		 
}

int main(void)
{
	//初始化板子
	LED_Configuration();
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  delay_init(168);		//延时初始化 
	 TIM7_Int_Init(10-1,8400-1);
  TIM3_Int_Init(10-1,8400-1);
  vUARTConfiguration(COM1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,ENABLE);
  vUARTConfiguration(COM6,115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,ENABLE);
  vUARTConfiguration(COM3,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,ENABLE);
	IWDG_Init(8,50000);//200s 系统复位
  LoadSetting();
  My_RTC_Init();
	EC20_Configuration();
  Init_Base(); 

	while(1)
	{
      switch(Sys_Base.ProSta)
      {
          case  eInit_None:  
          {
              if(!Sys_Base.InitTim)
              {  
                  if(ConFigWait())
                  {
                      LED_STRLENTH_OFF;
                      LED_ERR_OFF;
                      //USART_Cmd(USART6, DISABLE);
                      Sys_Base.ProSta=eInit_Config;
                  }  
                  if(WaitRdy("RDY\r\n"))
                  {  
                      Sys_Base.ProSta=eInit_4GConfig;
                  }  
              }
              else
              {
                  LED_COM1_OFF;
                  LED_COM2_OFF;
                  if(WaitRdy("RDY\r\n"))
                  {
                      Sys_Base.ProSta=eInit_4GConfig;
                  }
              }                
          }  break;
          case  eInit_Config: 
          {
              while(ReceiveSetting());
              LoadSetting();
              if(WaitRdy("RDY\r\n"))
              {  
                  Sys_Base.ProSta=eInit_4GConfig;
              }  
          }  
          break;
          case  eInit_4GConfig:  
          {
             
              Init4GMod();
              Sys_Base.ProSta=eInit_Finish;
          }break;  
          case  eInit_Finish: 
          {
              
               MBMSerialInit(COM3, 9600);
              while(1)
              {
                 MBM_Upload_Pro();
              }  
              
          }      
      }  
	}

}

