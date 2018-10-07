#include "ec20.h"

EC20x EC20;

void EC20_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(  RCC_EC20_POWER, ENABLE); 
	
	
	GPIO_InitStructure.GPIO_Pin = EC2_POWER_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(EC2_POWER_PORT, &GPIO_InitStructure);
	EC2_POWER_ON;

	delay_ms(200);
	
	EC2_POWER_OFF;
//  delay_ms(30000);
//	stuUSART6.TX_STATION = 1;
//	strcpy(stuUSART6.TX_BUF, "AT\r\n");	
//	strcpy(EC20_ACK, "READY\r\n");	
//	MYDMA_Enable(DMA2_Stream6,strlen(stuUSART6.TX_BUF)+20);
//	
//  delay_ms(500);
//  if(strcmp(EC20_ACK,stuUSART6.TX_BUF) == 0)
//		LED_STRLENTH_ON;
//	else
//		LED_ERR_ON;
}

























