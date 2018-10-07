// Header:
// File Name: ��������
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

//��ʼ��IO ����1 
//������ �ֳ� ֹͣλ У��
void Uart1_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
  
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9  |	GPIO_Pin_10; //GPIOA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10	

	USART_DeInit(USART1);
   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = WordLength ;//�ֳ�Ϊ8λ���ݸ�ʽ USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//һ��ֹͣλ  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//����żУ��λ    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
}

//��ʼ��IO ����6
//������ �ֳ� ֹͣλ У��
void Uart6_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��
  
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //RX
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //TX
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��
	
	USART_DeInit(USART6);
   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = WordLength ;//�ֳ�Ϊ8λ���ݸ�ʽ USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//һ��ֹͣλ  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//����żУ��λ    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
}


//��ʼ��IO ����3 
//������ �ֳ� ֹͣλ У��
void Uart3_init(u32 bound,u16 WordLength,u16 StopBits,u16 Parity){ 
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
  
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //
	
	//USART3�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //
	
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;                
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;       
//	GPIO_Init(GPIOC, &GPIO_InitStructure);  /*  */	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;              //PC0 ����485�շ���������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    //�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //����
	GPIO_Init(GPIOC,&GPIO_InitStructure);                   //����������

	USART_DeInit(USART3);
   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = WordLength ;//�ֳ�Ϊ8λ���ݸ�ʽ USART_WordLength_8b
	USART_InitStructure.USART_StopBits = StopBits ;//һ��ֹͣλ  						USART_StopBits_1
	USART_InitStructure.USART_Parity = Parity ;//����żУ��λ    					USART_Parity_No
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1 
	

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	RS485_2_RX_TX=0;				                        //����ģʽ	

}

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//USARTx: USART1-
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void MYDMA_Config(USART_TypeDef* USARTx,DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr,IRQn_Type IRQnx)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA Stream
	
	DMA_ITConfig(DMA_Streamx, DMA_IT_TC, ENABLE);	
	USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel                   = IRQnx;   //DMA2_Stream7_IRQn         
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
		
	DMA_Cmd(DMA_Streamx, DISABLE);
} 

//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	if(DMA_Streamx == DMA1_Stream3)
	{
		stuUSART3.TX_STATION = 1;
		RS485_2_RX_TX=1;  //����ģʽ
		//delay_ms(1);      //��ʱ	
	}
	else if(DMA_Streamx == DMA2_Stream6)
	{
		stuUSART6.TX_STATION = 1;
	}
	else if(DMA_Streamx == DMA2_Stream7)
	{
		stuUSART1.TX_STATION = 1;
	}
	
	
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
}	  


//����1


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		stuUSART1.RX_STATION = 2;
	
	  SettinRxPro(USART_ReceiveData(USART1),&stuUSART1);
	//	stuUSART1.TX_BUF[stuUSART1.TX_len[1]++] = USART_ReceiveData(USART1);
    		
		USART_ClearFlag(USART1, USART_IT_RXNE);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		stuUSART1.RX_STATION = 1;//�������
  }
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{//�����   �������������ܲ�����һ������
		USART1->SR;
		USART1->DR;
		//stuUSART1.TX_STATION = 1;
		
	}
} 

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
	{ 
		stuUSART1.TX_len[1]=0;
		stuUSART1.TX_STATION = 0;
		DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//���DMA2_Steam7������ɱ�־
		//LED_COM1_OFF;
		DMA_Cmd(DMA2_Stream7, DISABLE);   //�ر�DMA����
	}
}



//����3


void USART3_IRQHandler(void)                	//����3�жϷ������
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		stuUSART3.RX_STATION = 2;	

	  vMBPUSART1ISR(USART_ReceiveData(USART3));
		USART_ClearFlag(USART3, USART_IT_RXNE);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		stuUSART3.RX_STATION = 1;//�������
  }
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) 
	{//֡����
		stuUSART3.TX_STATION = 1;
		USART3->SR;
		USART3->DR;
	}
} 

void DMA1_Stream3_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET)//�ȴ�DMA1_Steam3�������
	{ 
		stuUSART3.TX_len[1]=0;
		stuUSART3.TX_STATION = 0;
		DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);//���DMA1_Steam3������ɱ�־
		DMA_Cmd(DMA1_Stream3, DISABLE);   //�ر�DMA����
    delay_ms(2);      //��ʱ
		RS485_2_RX_TX=0;  //����ģʽ
		

	}
}


void USART6_IRQHandler(void)                	//����6�жϷ������
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
	//	stuUSART6.RX_STATION = 1;//�������
  }
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET) 
	{//֡����
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
    //if(stuUSART6.RX_STATION==2) stuUSART6.RX_STATION=0;//��Ϊ��һ���յ�nRDY���˵�����
		USART6->SR;
		USART6->DR;
	}
} 


void DMA2_Stream6_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream6,DMA_FLAG_TCIF6)!=RESET)//�ȴ�DMA1_Steam3�������
	{ 
		stuUSART6.TX_len[1]=0;
		stuUSART6.TX_STATION = 0;

	
		DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);//���DMA1_Steam3������ɱ�־
		DMA_Cmd(DMA2_Stream6, DISABLE);   //�ر�DMA����
	}
}






