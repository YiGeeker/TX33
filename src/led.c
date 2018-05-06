/* 该程序主要控制TX＋、TX-、GPS和STATUS四个LED的状态 */
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "led.h"

/* LED的GPIO初始化 */
void Led_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  LED_TXP_R | LED_TXN_R | LED_STA_G | LED_STA_R;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  LED_TXP_B | LED_TXN_B | LED_GPS_B | LED_GPS_G;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* TX+显示设置，只有蓝色和红色 */
void Led_TXP(LED_COLOR Color)
{ 
	if (Color == Blue)
	{
		GPIO_SetBits(LED_TXP_B_PORT, LED_TXP_B);
		GPIO_ResetBits(LED_TXP_R_PORT, LED_TXP_R);
	}
	else
	{
		/* 否则为红色 */
		GPIO_SetBits(LED_TXP_R_PORT, LED_TXP_R);
		GPIO_ResetBits(LED_TXP_B_PORT ,LED_TXP_B);
	}
}

/* TX-显示设置，只有蓝色和红色 */
void Led_TXN(LED_COLOR Color)
{ 
	if (Color == Blue)
	{
		GPIO_SetBits(LED_TXN_B_PORT, LED_TXN_B);
		GPIO_ResetBits(LED_TXN_R_PORT, LED_TXN_R);
	}
	else
	{
		/* 否则为红色 */
		GPIO_SetBits(LED_TXN_R_PORT, LED_TXN_R);
		GPIO_ResetBits(LED_TXN_B_PORT, LED_TXN_B);
	}
}

/* GPS显示设置，只有红色和绿色 */
void Led_GPS(LED_COLOR Color)
{
	if (Color == Red)
	{
		GPIO_SetBits(LED_GPS_B_PORT, LED_GPS_B);
		GPIO_ResetBits(LED_GPS_G_PORT, LED_GPS_G);
	}	
	else
	{
		/* 否则为绿色 */
	   	GPIO_SetBits(LED_GPS_G_PORT, LED_GPS_G);
		GPIO_ResetBits(LED_GPS_B_PORT, LED_GPS_B);
	}
}

/* STATUS显示设置，只有绿色和红色 */
void Led_STATUS(LED_COLOR Color)
{ 
	if (Color == Green)
	{
		GPIO_SetBits(LED_STA_G_PORT, LED_STA_G);
		GPIO_ResetBits(LED_STA_R_PORT, LED_STA_R);
	}
	else
	{
		/* 否则为红色 */
		GPIO_SetBits(LED_STA_R_PORT, LED_STA_R);
		GPIO_ResetBits(LED_STA_G_PORT, LED_STA_G);
	}
}
