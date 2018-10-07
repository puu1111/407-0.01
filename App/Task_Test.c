#include "main.h"  

//OS_STK Stk_Task_LED[TASK_TEST_LED_STK_SIZE];
//OS_STK Stk_Task_TCP_Client[TASK_TCP_CLIENT_STK_SIZE];

//LED闪烁任务
void Task_LED(void *pdata)
{		 
    while(1)
	{
	//	OSTimeDlyHMSM(0, 0, 0, 1000);//
		GPIO_ToggleBits(LED1);		
	}
}
//tcp客户端收发任务
void Task_TCP_Client(void *pdata)
{
	__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	unsigned char tcp_data[] = "tcp 客户端实验!\n";
	struct tcp_pcb *pcb;
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	ETH_BSP_Config();
	LwIP_Init();

	/* TCP_Client Init */
	TCP_Client_Init(TCP_LOCAL_PORT,TCP_SERVER_PORT,TCP_SERVER_IP);
	while(1)
	{  
		pcb = Check_TCP_Connect();
		if(pcb != 0)
		{	
			TCP_Client_Send_Data(pcb,tcp_data,sizeof(tcp_data));	//该函数为主动向服务器发送函数，
		}		
		LocalTime += 10;
		LwIP_Periodic_Handle(LocalTime);		
	
	}
}

