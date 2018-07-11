#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "definition.h"
#include "key.h"

void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	EXTI_InitTypeDef EXTI_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	 
	GPIO_InitStructure.GPIO_Pin =  KEY_UP_GPIO | KEY_DOWN_GPIO | KEY_MENU_GPIO | KEY_ON_GPIO | KEY_OFF_GPIO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* 下降沿中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
     
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* 下降沿中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* 下降沿中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* 下降沿中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource6); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* 下降沿中断 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 

}

void Key_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); /* 第一组是2个抢占优先级，8个响应优先级，共16种 */
  
	/* 配置P[A|B|C|D|E]2.3.4为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

void Key_All_Disable(void)
{
	EXTI->IMR &= ~(EXTI_Line2 | EXTI_Line3 | EXTI_Line4 | EXTI_Line5 | EXTI_Line6);
}

void Key_All_Enable(void)
{
	EXTI->IMR |= (EXTI_Line2 | EXTI_Line3 | EXTI_Line4 | EXTI_Line5 | EXTI_Line6);
}  
