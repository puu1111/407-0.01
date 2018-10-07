/*********************************************************************
 * 文件名  ：led.c
 * 描述    ：led 应用函数库
 *          
 * 实验平台：基于STM32F407VET6 开发板
 * 硬件连接：-----------------
 *          |   PE13 - LED1   |
 *          |   PE14 - LED2   |
 *          |   PE15 - LED3   |
 *           ----------------- 
 * 库版本  ：V1.0.0		
***********************************************************************/
#include "sys.h"
/***************  配置LED用到的I/O口 *******************/
void LED_Configuration(void)
{

	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(  RCC_LED_POWER|RCC_LED_COM1|RCC_LED_COM2|RCC_LED_STRLENTH|RCC_LED_ERR, ENABLE); 
	
	
	GPIO_InitStructure.GPIO_Pin = LED_POWER_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(LED_POWER_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED_COM1_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(LED_COM1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_COM2_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(LED_COM2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_STRLENTH_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(LED_STRLENTH_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED_ERR_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(LED_ERR_PORT, &GPIO_InitStructure);
	
	/*初始化完后，关闭3个LED*/ 
	LED_POWER_ON;
	LED_COM1_OFF;
	LED_COM2_OFF;
	LED_STRLENTH_OFF;
	LED_ERR_OFF; 
}


/**
  * @brief  Toggles the specified GPIO pins.
  * @param  GPIOx Where x can be (A..K) to select the GPIO peripheral for STM32F429X device or
  *                      x can be (A..I) to select the GPIO peripheral for STM32F40XX and STM32F427X devices.
  * @param  GPIO_Pin Specifies the pins to be toggled.
  * @retval None
  */
void LED_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIOx->ODR ^= GPIO_Pin;
}



