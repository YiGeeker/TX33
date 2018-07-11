#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "DS1809.h"

uint8_t move_num = 0;

void RES_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	/* GPIOA、GPIOD和AFIO使能，必须使能AFIO才能重映射 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	/* RES_UC的GPIO定义 */
    GPIO_InitStructure.GPIO_Pin =  RES_UC_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 复用推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* RES_DC的GPIO定义 */
    GPIO_InitStructure.GPIO_Pin =  RES_DC_GPIO;
	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOC和GPIOD重映射 */
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
}

/* 初始化RES控制的NVIC */
void RES_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    
    NVIC_Init(&NVIC_InitStructure);
}


/* 初始化RES的控制输出，PWM的有效输出至少是1ms */
void RES_PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	TIM_OCInitTypeDef  TIM_OCInitStructure = {0};

	/* TIM2和TIM4时钟使能，频率为72MHz */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM4, ENABLE); 
	
	/* TIM2和TIM4基本设置 */
  	TIM_TimeBaseStructure.TIM_Period = 0xffff-1;
  	TIM_TimeBaseStructure.TIM_Prescaler = 3; /* 设置分频，分频后为1.6MHz，这是频率为25时的最佳分频系数 */
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ; /* 设置时钟分频系数：不分频 */
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* TIM4通道1和TIM2通道3设置，即UC和DC */
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive; /* 配置为输出比较模式，当定时器发生跳变时输出无效电平 */
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; /* 初始情况电平不输出 */
  	TIM_OCInitStructure.TIM_Pulse = 22336; /* 设置初始跳变值，当计数器计数到这个值时，电平发生跳变 */
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; /* 设置有效电平为低电平 */
	
  	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
  	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	/* 初始中断设置，即TIM4的TIM_IT_CC1和TIM2的TIM_IT_CC3 */
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
    
	TIM_ARRPreloadConfig(TIM4, ENABLE);
  	TIM_ARRPreloadConfig(TIM2, ENABLE);

	/* 初始情况不开始计数 */
	TIM_Cmd(TIM4, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
}

void RES_Set_Value(int8_t dNum)
{
	if(dNum<0)
	{
		move_num = -dNum;
		TIM_SetCounter(TIM2, 0);
		TIM2->CCER |= 0x0100;
		TIM_Cmd(TIM2, ENABLE);
	}else
	{
		move_num = dNum;
		TIM_SetCounter(TIM4, 0);
		TIM4->CCER |= 0x0001;
		TIM_Cmd(TIM4, ENABLE);
	}
}
