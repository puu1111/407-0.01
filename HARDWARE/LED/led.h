#ifndef _LED_H_
#define _LED_H_
#include "stm32f4xx.h" 

//#define LED1 		GPIOE , GPIO_Pin_13   ec20_tx rs232-tx  ec20str err
//#define LED2 		GPIOE , GPIO_Pin_14
//#define LED3 		GPIOE , GPIO_Pin_15
void LED_Configuration(void);
void LED_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);


//芯片工作正常
#define RCC_LED_POWER         RCC_AHB1Periph_GPIOG
#define LED_POWER_PORT 				GPIOG
#define LED_POWER_PIN       	GPIO_Pin_15
#define LED_POWER_SOURCE    	15
//COM1数据
#define RCC_LED_COM1          RCC_AHB1Periph_GPIOC
#define LED_COM1_PORT 				GPIOC
#define LED_COM1_PIN        	GPIO_Pin_13
#define LED_COM1_SOURCE     	13
//COM2数据
#define RCC_LED_COM2          RCC_AHB1Periph_GPIOB
#define LED_COM2_PORT 				GPIOB
#define LED_COM2_PIN        	GPIO_Pin_9
#define LED_COM2_SOURCE     	9
//EC20信号强度
#define RCC_LED_STRLENTH      RCC_AHB1Periph_GPIOB
#define LED_STRLENTH_PORT 		GPIOB
#define LED_STRLENTH_PIN      GPIO_Pin_8
#define LED_STRLENTH_SOURCE   8
//错误灯
#define RCC_LED_ERR           RCC_AHB1Periph_GPIOG
#define LED_ERR_PORT 					GPIOG
#define LED_ERR_PIN        		GPIO_Pin_6
#define LED_ERR_SOURCE     		6

															
															
#define LED_POWER_ON 					GPIO_ResetBits(LED_POWER_PORT , LED_POWER_PIN)
#define LED_COM1_ON 					GPIO_ResetBits(LED_COM1_PORT , LED_COM1_PIN)
#define LED_COM2_ON 					GPIO_ResetBits(LED_COM2_PORT , LED_COM2_PIN)
#define LED_STRLENTH_ON 			GPIO_ResetBits(LED_STRLENTH_PORT , LED_STRLENTH_PIN)
#define LED_ERR_ON 						GPIO_SetBits(LED_ERR_PORT , LED_ERR_PIN)

#define LED_POWER_OFF 				GPIO_SetBits(LED_POWER_PORT , LED_POWER_PIN)
#define LED_COM1_OFF 					GPIO_SetBits(LED_COM1_PORT , LED_COM1_PIN)
#define LED_COM2_OFF 					GPIO_SetBits(LED_COM2_PORT , LED_COM2_PIN)
#define LED_STRLENTH_OFF 			GPIO_SetBits(LED_STRLENTH_PORT , LED_STRLENTH_PIN)
#define LED_ERR_OFF						GPIO_ResetBits(LED_ERR_PORT , LED_ERR_PIN)


#define RE_LED_POWER 					LED_TogglePin(LED_POWER_PORT , LED_POWER_PIN)
#define RE_LED_COM1						LED_TogglePin(LED_COM1_PORT , LED_COM1_PIN)
#define RE_LED_COM2						LED_TogglePin(LED_COM2_PORT , LED_COM2_PIN)
#define RE_LED_STRLENTH 			LED_TogglePin(LED_STRLENTH_PORT , LED_STRLENTH_PIN)
#define RE_LED_ERR						LED_TogglePin(LED_ERR_PORT , LED_ERR_PIN)


//#define LED1_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_13)
//#define LED2_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_14)
//#define LED3_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_15)

#endif
