// Header:
// File Name: 串口配置
// Author:
// Date:

#include "usart.h"	

u8 UartRunFlag=1;
stuUSARTx stuUSART1;
stuUSARTx stuUSART3;
stuUSARTx4G stuUSART6;
extern u8 temprx[300];
void MYDMA_Config(USART_TypeDef* USARTx,DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr,IRQn_Type IRQnx);
void Uart1_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity); 
void Uart6_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity); 
void Uart3_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity);


void vUARTConfiguration(u8 COMx,u32 bound,u16 WordLength,u16 StopBits,u16 Parity, FunctionalState Flag)
{
	if(COMx==1 && Flag==ENABLE)
	{
		Uart1_init(bound,WordLength,StopBits,Parity); 
		MYDMA_Config(USART1,DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)stuUSART1.TX_BUF,USART1_SEN_SIZE,DMA2_Stream7_IRQn);
	}
	else if(COMx==6 && Flag==ENABLE)
	{
		Uart6_init(bound,WordLength,StopBits,Parity); 
		MYDMA_Config(USART6,DMA2_Stream6,DMA_Channel_5,(u32)&USART6->DR,(u32)stuUSART6.TX_BUF,USART6_SEN_SIZE,DMA2_Stream6_IRQn); 
	}
	else if(COMx==3 && Flag==ENABLE)
	{
		Uart3_init(bound,WordLength,StopBits,Parity); 
		MYDMA_Config(USART3,DMA1_Stream3,DMA_Channel_4,(u32)&USART3->DR,(u32)stuUSART3.TX_BUF,USART3_SEN_SIZE,DMA1_Stream3_IRQn);
	}
}

//初始化IO 串口1 
//波特率 字长 停止位 校验
void Uart1_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
  
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9  |	GPIO_Pin_10; //GPIOA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10	

	USART_DeInit(USART1);
   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = WordLength ;//字长为8位数据格式 USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//一个停止位  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//无奇偶校验位    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
}

//初始化IO 串口6
//波特率 字长 停止位 校验
void Uart6_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能
  
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //RX
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //TX
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化
	
	USART_DeInit(USART6);
   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = WordLength ;//字长为8位数据格式 USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//一个停止位  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//无奇偶校验位    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART6, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART6, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
}


//初始化IO 串口3 
//波特率 字长 停止位 校验
void Uart3_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
  
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //
	
	//USART3端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //
	
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;                
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;       
//	GPIO_Init(GPIOC, &GPIO_InitStructure);  /*  */	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;              //PC0 控制485收发控制引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    //速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);                   //按配置设置

	USART_DeInit(USART3);
   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = WordLength ;//字长为8位数据格式 USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//一个停止位  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//无奇偶校验位    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART3, ENABLE);  //使能串口1 
	

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

	RS485_2_RX_TX=0;				                        //接收模式	

}

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//USARTx: USART1-
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  
void MYDMA_Config(USART_TypeDef* USARTx,DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr,IRQn_Type IRQnx)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//等待DMA可配置 
	
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = ndtr;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA_Streamx, &DMA_InitStructure);//初始化DMA Stream
	
	DMA_ITConfig(DMA_Streamx, DMA_IT_TC, ENABLE);	
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel                   = IRQnx;   //DMA2_Stream7_IRQn         
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
		
	DMA_Cmd(DMA_Streamx, DISABLE);
} 

//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	if(DMA_Streamx == DMA1_Stream3)
	{
		stuUSART3.TX_STATION = 1;
		RS485_2_RX_TX=1;  //发送模式
		//delay_ms(1);      //延时	
	}
	else if(DMA_Streamx == DMA2_Stream6)
	{
		stuUSART6.TX_STATION = 1;
	}
	else if(DMA_Streamx == DMA2_Stream7)
	{
		stuUSART1.TX_STATION = 1;
	}
	
	
	DMA_Cmd(DMA_Streamx, DISABLE);                      //关闭DMA传输 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//确保DMA可以被设置  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //数据传输量  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //开启DMA传输 
}	  


//串口1


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		stuUSART1.RX_STATION = 2;
	
	  SettinRxPro(USART_ReceiveData(USART1),&stuUSART1);
	//	stuUSART1.TX_BUF[stuUSART1.TX_len[1]++] = USART_ReceiveData(USART1);
    		
		USART_ClearFlag(USART1, USART_IT_RXNE);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		stuUSART1.RX_STATION = 1;//接受完成
  }
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{//贞接收   必须清楚否则接受不到第一个数据
		USART1->SR;
		USART1->DR;
		//stuUSART1.TX_STATION = 1;
		
	}
} 

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
	{ 
		stuUSART1.TX_len[1]=0;
		stuUSART1.TX_STATION = 0;
		DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//清除DMA2_Steam7传输完成标志
		//LED_COM1_OFF;
		DMA_Cmd(DMA2_Stream7, DISABLE);   //关闭DMA传输
	}
}



//串口3


void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		stuUSART3.RX_STATION = 2;	

	  vMBPUSART1ISR(USART_ReceiveData(USART3));
		USART_ClearFlag(USART3, USART_IT_RXNE);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		stuUSART3.RX_STATION = 1;//接受完成
  }
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) 
	{//帧接收
		stuUSART3.TX_STATION = 1;
		USART3->SR;
		USART3->DR;
	}
} 

void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET)//等待DMA1_Steam3传输完成
	{ 
		stuUSART3.TX_len[1]=0;
		stuUSART3.TX_STATION = 0;
		DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);//清除DMA1_Steam3传输完成标志
		DMA_Cmd(DMA1_Stream3, DISABLE);   //关闭DMA传输
    delay_ms(2);      //延时
		RS485_2_RX_TX=0;  //接收模式
		

	}
}


void USART6_IRQHandler(void)                	//串口6中断服务程序
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  
	{
	//	stuUSART6.RX_STATION = 2;	
    // if(stuUSART6.RX_STATION<2)
    // {       
       // stuUSART3.RX_BUF[stuUSART6.RX_len[0]]=USART_ReceiveData(USART6);// 
         stuUSART6.RX_STATION=SettinRxPro4G(USART_ReceiveData(USART6),\
         &stuUSART6);
    // }  
    
		USART_ClearFlag(USART6, USART_IT_RXNE);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	//	stuUSART6.RX_STATION = 1;//接受完成
  }
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET) 
	{//帧接收
//		stuUSART6.TX_STATION = 1;
		/*if(EC20.Station[0] == 0)
		{
			if(strcmp(EC20.ACK_BUFF,stuUSART6.RX_BUF) == 0)
			{
				LED_STRLENTH_ON;
			  EC20.Station[0] = 1;	
			}			
		}
		else
		{
			EC20.Station[0]++;
			if(EC20.Station[0]>5)
				EC20.Station[0] = 1;
		}*/
   //	memset(temprx,0,sizeof(temprx));
    //if(stuUSART6.RX_STATION==2) stuUSART6.RX_STATION=0;//作为第一次收到nRDY过滤的作用
		USART6->SR;
		USART6->DR;
	}
} 


void DMA2_Stream6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream6,DMA_FLAG_TCIF6)!=RESET)//等待DMA1_Steam3传输完成
	{ 
		stuUSART6.TX_len[1]=0;
		stuUSART6.TX_STATION = 0;

	
		DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);//清除DMA1_Steam3传输完成标志
		DMA_Cmd(DMA2_Stream6, DISABLE);   //关闭DMA传输
	}
}






