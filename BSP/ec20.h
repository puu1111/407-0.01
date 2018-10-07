#ifndef __EC20_H
#define __EC20_H



#include "stm32f4xx_conf.h"
#include "sys.h" 
//#include "delay.h"

typedef struct
{
	u8 Station[2];
	char ACK_BUFF[255];
	char TXT_BUFF[6][100];
}EC20x;


typedef enum
{
	EC20_OFF,
	EC20_QIDEACT1,
	EC20_QIACT1,
	EC20_QICSGP1,
	EC20_CGATT,
	EC20_QICSGP
}EC20_STATION;



//芯片工作正常
#define RCC_EC20_POWER        RCC_AHB1Periph_GPIOF
#define EC2_POWER_PORT 				GPIOF
#define EC2_POWER_PIN       	GPIO_Pin_6
#define EC2_POWER_SOURCE    	6

#define EC2_POWER_OFF 			  GPIO_ResetBits(EC2_POWER_PORT , EC2_POWER_PIN)
#define EC2_POWER_ON 					GPIO_SetBits(EC2_POWER_PORT , EC2_POWER_PIN)

void EC20_Configuration(void);

extern EC20x EC20;
#endif





















