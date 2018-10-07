#include "rtc.h"
#include "time.h"
#include "delay.h"
#include "usart.h" 

NVIC_InitTypeDef   NVIC_InitStructure;
time_t tt55;
//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}
//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

time_t getTimeStamp(void)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
	struct tm tmp_time;
	time_t t;
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

 
  tmp_time.tm_hour = RTC_TimeStructure.RTC_Hours;
  tmp_time.tm_min = RTC_TimeStructure.RTC_Minutes;
  tmp_time.tm_sec = RTC_TimeStructure.RTC_Seconds;
  tmp_time.tm_mday = RTC_DateStructure.RTC_Date;
  tmp_time.tm_mon = RTC_DateStructure.RTC_Month;
  tmp_time.tm_year = RTC_DateStructure.RTC_Year;

  t = mktime(&tmp_time);

  return t;
}

time_t getRealTimeStamp(char *timestr)
{
	 RTC_TimeTypeDef RTC_TimeStruct;
   RTC_DateTypeDef RTC_DateStruct;
	 struct tm tmp_time;
	 
	 uint8_t i;
  /* Get the current Time */
	
  
	 for(i=0;i<17;i++)
	 {
			timestr[i]-=0x30;
	 }	 
	 RTC_DateStruct.RTC_Year=100+timestr[0]*10+timestr[1]; 
	 RTC_DateStruct.RTC_Month=timestr[3]*10+timestr[4]-1;
	 RTC_DateStruct.RTC_Date=timestr[6]*10+timestr[7];
	 
	 RTC_TimeStruct.RTC_Hours=timestr[9]*10+timestr[10];
	 RTC_TimeStruct.RTC_Minutes=timestr[12]*10+timestr[13];
	 RTC_TimeStruct.RTC_Seconds=timestr[15]*10+timestr[16];

   RTC_Set_Time( RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,RTC_H12_AM);	//����ʱ��
	 RTC_Set_Date(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,1);		//��������
   
   tmp_time.tm_hour = RTC_TimeStruct.RTC_Hours;
	 tmp_time.tm_min = RTC_TimeStruct.RTC_Minutes;
	 tmp_time.tm_sec = RTC_TimeStruct.RTC_Seconds;
   tmp_time.tm_mday = RTC_DateStruct.RTC_Date;
   tmp_time.tm_mon = RTC_DateStruct.RTC_Month;
   tmp_time.tm_year = RTC_DateStruct.RTC_Year;

    tt55= mktime(&tmp_time);
   return 0;
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return 1;		//LSE ����ʧ��. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
    RTC_Init(&RTC_InitStructure);
 
		RTC_Set_Time(23,59,56,RTC_H12_AM);	//����ʱ��
		RTC_Set_Date(14,5,5,1);		//��������
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	} 
 
	return 0;
}

//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:Сʱ,����,����
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
	//EXTI_InitTypeDef   EXTI_InitStructure;

}

//�����Ի��Ѷ�ʱ������  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 

}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{    
									 
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{    
								
}
 












