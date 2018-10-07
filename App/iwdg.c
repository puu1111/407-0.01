#include "iwdg.h"

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е�3λ��Ч!)
//rlr:�Զ���װ��ֵ,0~0XFFF.
//��Ƶ����=4*2^prer.�����ֵֻ����256!
//rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).

#define dFeedCmp 150000
typedef struct {  
    u8  feedflag;
    u32 feedcnt;//����   
}stdog_TaskCycle;
 
stdog_TaskCycle stdogTaskCycle;

void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��

	IWDG_SetReload(rlr);   //����IWDGװ��ֵ

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //ʹ�ܿ��Ź�
  stdogTaskCycle.feedflag=0;
  stdogTaskCycle.feedcnt=0;
}

//ι�������Ź�


void IWDG_Feed_TIM(void)
{
   Tim_CHA(&stdogTaskCycle.feedflag,&stdogTaskCycle.feedcnt,dFeedCmp);  
    if(stdogTaskCycle.feedflag)  
    {  
        IWDG_ReloadCounter();//reload
    }  
}
