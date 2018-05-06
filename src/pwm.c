/* 该程序主要用于桥路控制和外接高压的PWM控制信号 */
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "pwm.h"

uint16_t Bridge_High = 0;		/* 桥路PWM输出高电平的计数值 */
uint16_t Bridge_Low = 0;		/* 桥路PWM输出低电平的计数值 */
uint16_t SW_High = 0;			/* 开关PWM输出高电平的计数值 */
uint16_t SW_Low = 0;			/* 开关PWM输出低电平的计数值 */

uint8_t Transimit_On = 0;		/* 指示是否在发射电流，为0表示关闭，为1表示正在发射 */

uint16_t Get_Best_Prescaler(uint8_t nFreq); /* 得到特定频率下的最佳分频系数 */

/* 初始化PWM的GPIO */
void Pwm_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

	/* GPIOC、GPIOD和AFIO使能，必须使能AFIO才能重映射 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	/* GPIOC的用于PWM的GPIO定义 */
    GPIO_InitStructure.GPIO_Pin =  PWM_PUL_GPIO | PWM_PUR_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 复用推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOD的用于PWM的GPIO定义 */
    GPIO_InitStructure.GPIO_Pin =  PWM_PDL_GPIO | PWM_PDR_GPIO | PWM_PSW_GPIO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
    GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOC和GPIOD重映射 */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3|GPIO_Remap_TIM4, ENABLE);
}

/* 初始化PWM的NVIC */
void Pwm_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    
    NVIC_Init(&NVIC_InitStructure);
}

/* 初始化PWM的计数器和各通道，nFreq为初始频率，fDuty为初始占空比，频率可调范围为1-500Hz */
void Pwm_Init(uint16_t nFreq, float fDuty)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	TIM_OCInitTypeDef  TIM_OCInitStructure = {0};

	Transimit_On = 0;
	
	/* 计算计数值，频率为nFreq */
    uint16_t TIM_Period = (uint16_t)(16e5/nFreq);
	
	/* PWM信号电平跳变值，占空比为fDuty */
	Bridge_High = (uint16_t)(TIM_Period*fDuty/2);
	Bridge_Low = (uint16_t)(TIM_Period-Bridge_High);
	
	/* TIM3和TIM4时钟使能，频率为72MHz */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE); 
	
	/* TIM3和TIM4基本设置 */
  	TIM_TimeBaseStructure.TIM_Period = 0xffff-1;
  	TIM_TimeBaseStructure.TIM_Prescaler = 45-1; /* 设置分频，分频后为1.6MHz，这是频率为25时的最佳分频系数 */
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ; /* 设置时钟分频系数：不分频 */
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* TIM3通道1和TIM4通道4设置，即PUL和PDR */
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive; /* 配置为输出比较模式，当定时器发生跳变时输出无效电平 */
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; /* 初始情况电平不输出 */
  	TIM_OCInitStructure.TIM_Pulse = Bridge_High; /* 设置初始跳变值，当计数器计数到这个值时，电平发生跳变 */
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 设置有效电平为高电平 */
	
  	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
  	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Disable);

  	/* TIM3通道2和TIM4通道3设置，即PUR和PDL */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active; /* 配置为输出比较模式，当定时器发生跳变时输出有效电平 */
  	TIM_OCInitStructure.TIM_Pulse = (uint16_t)(TIM_Period/2); /* 设置初始跳变值，即与上一个桥路相差180度相位 */
	
  	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
  	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable);

	/* 设置外部开关控制的PWM，即PSW */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive; /* 配置为输出比较模式，当定时器发生跳变时输出无效电平 */
	TIM_OCInitStructure.TIM_Pulse = SW_High; /* 设置初始跳变值，当计数器计数到这个值时，电平发生跳变 */

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);

	/* 初始中断设置，不定义PSW，即TIM4的TIM_IT_CC1 */
    TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
    
	TIM_ARRPreloadConfig(TIM3, ENABLE);
  	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* 初始情况不开始计数 */
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM4, DISABLE);
}

/* 设置PWM的频率和占空比 */
void Pwm_Set_Value(uint16_t nFreq, float fDuty)
{
	uint16_t Prescaler = Get_Best_Prescaler(nFreq); /* 获得最佳分频系数 */
    uint16_t TIM_Period = (uint16_t)(72e6/Prescaler/nFreq); /* 计算计数值，频率为nFreq */

	/* 修改分频系数 */
	TIM_PrescalerConfig(TIM3, Prescaler-1, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM4, Prescaler-1, TIM_PSCReloadMode_Immediate);
	
	TIM_Period &= 0xfffe;	/* 将TIM_Period舍为偶数，使得控制外接开关的PWM不会错位 */
	
	/* PWM信号信号电平跳变值，占空比为fDuty */
	Bridge_High = (uint16_t)(TIM_Period*(fDuty/2));
	Bridge_Low = (uint16_t)(TIM_Period-Bridge_High);
	SW_High = Bridge_High;
	SW_Low = TIM_Period/2-Bridge_High;

	/* 强制各路PWM初始输出为对应电平，PUL、PDR和PSW初始输出高电平，PUR和PDL初始输出低电平 */
	TIM3->CCMR1 = (TIM3->CCMR1 & 0x8c8c) | 0x4050;
	TIM4->CCMR1 = (TIM4->CCMR2 & 0xff8c) | 0x0050;
	TIM4->CCMR2 = (TIM4->CCMR2 & 0x8c8c) | 0x5040;
	
	/* 改变计数器的初始值 */
	TIM_SetCompare1(TIM3, Bridge_High);
	TIM_SetCompare4(TIM4, Bridge_High);
	TIM_SetCompare2(TIM3, (uint16_t)(TIM_Period/2));
	TIM_SetCompare3(TIM4, (uint16_t)(TIM_Period/2));

	/* 计数器清零 */
	TIM_SetCounter(TIM3, 0);
	TIM_SetCounter(TIM4, 0);
}

/* 打开外接开关 */
void Pwm_Switch_Enable(void)
{
	TIM4->CCER |= 0x0001;		/* 使能外接开关输出 */
	TIM_SetCompare1(TIM4, SW_High);
	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
}

/* 打开除外接开关以外的PWM */
void Pwm_Enable(void)
{
	/* 使能各路输出 */
	TIM3->CCER |= 0x0011;
	TIM4->CCER |= 0x1100;
	
	Transimit_On = 1;
	
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

/* 关闭所有PWM，包括外接开关 */
void Pwm_Disable(void)
{
	/* 禁止各路输出，包括外接开关 */
	TIM3->CCER &= 0xffee;
	TIM4->CCER &= 0xeefe;
	
	TIM_ITConfig(TIM4, TIM_IT_CC1, DISABLE);
	
	Transimit_On = 0;
}

/* 得到特定频率下的最佳分频系数 */
uint16_t Get_Best_Prescaler(uint8_t nFreq)
{
	uint16_t i;
	uint16_t temp = 1;

	temp = (uint16_t)((float)(72e6/65536.0/nFreq)+1);

	for(i=0; i<256; i++)
	{
		if(72000000%(temp+i) == 0) 
        {
            temp += i;
            break;
        }
	}

	return temp;
}

void TIM3_IRQHandler(void)
{
	uint16_t capture = 0;
	static uint8_t offset = 0x00; /* 高低电平翻转控制 */

	/* TIM3通道1设置，即PUL */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		capture = TIM_GetCapture1(TIM3);
		
		if (offset&0x01) {
			if(!Transimit_On)
			{
				TIM_Cmd(TIM3, DISABLE);
				TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 | TIM_IT_CC2); /* 必须清除中断标志，否则依然会中断修改offset的值 */
				offset = 0x00;
				return;	/* 必须在此处return，否则会往下运行修改offset的值 */
				/* 还有一种解决办法，就是将offset声明为全局变量，然后在开启PWM时清零 */
			}else{
				TIM3->CCMR1 = (TIM3->CCMR1 & 0xff8f) | 0x0020;
				TIM_SetCompare1(TIM3, capture+Bridge_High);
			}
		}else {
			TIM3->CCMR1 = (TIM3->CCMR1 & 0xff8f) | 0x0010;
			TIM_SetCompare1(TIM3, capture+Bridge_Low);
		}
		
		offset^=0x01;
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	}

	/* TIM3通道2设置，即PUR */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
	{
		capture = TIM_GetCapture2(TIM3);
		
		if (offset&0x02) {
			TIM3->CCMR1 = (TIM3->CCMR1 & 0x8fff) | 0x1000;
			TIM_SetCompare2(TIM3, capture+Bridge_Low);
		}else {
			TIM3->CCMR1 = (TIM3->CCMR1 & 0x8fff) | 0x2000;
			TIM_SetCompare2(TIM3, capture+Bridge_High);
		}
		
		offset^=0x02;		
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	}
}

void TIM4_IRQHandler(void)
{
	uint16_t capture = 0;
	static uint8_t offset = 0x00; /* 高低电平翻转控制 */

	/* TIM4通道1设置，即PSW */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		capture = TIM_GetCapture1(TIM4);
		
		if (offset&0x04) {
			TIM4->CCMR1 = (TIM4->CCMR1 & 0xff8f) | 0x0020;
			TIM_SetCompare1(TIM4, capture+SW_High);
		}else {
			TIM4->CCMR1 = (TIM4->CCMR1 & 0xff8f) | 0x0010;
			TIM_SetCompare1(TIM4, capture+SW_Low);
		}
		
		offset^=0x04;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	}

	/* TIM4通道3设置，即PDL */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
	{
		capture = TIM_GetCapture3(TIM4);
		
		if (offset&0x01) {
			TIM4->CCMR2 = (TIM4->CCMR2 & 0xff8f) | 0x0010;
			TIM_SetCompare3(TIM4, capture+Bridge_Low);
		}else {
			TIM4->CCMR2 = (TIM4->CCMR2 & 0xff8f) | 0x0020;
			TIM_SetCompare3(TIM4, capture+Bridge_High);
		}
		
		offset^=0x01;		
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	}

	/* TIM4通道4设置，即PDR */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)
	{
		capture = TIM_GetCapture4(TIM4);

		if (offset&0x02) {
			if(!Transimit_On)
			{
				TIM_Cmd(TIM4, DISABLE);
				TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 | TIM_IT_CC3 | TIM_IT_CC4); /* 必须清除中断标志，否则依然会中断修改offset的值 */
				offset = 0x00;
				return;	/* 必须在此处return，否则会往下运行修改offset的值 */
				/* 还有一种解决办法，就是将offset声明为全局变量，然后在开启PWM时清零 */
			}else
			{
				TIM4->CCMR2 = (TIM4->CCMR2 & 0x8fff) | 0x2000;
				TIM_SetCompare4(TIM4, capture+Bridge_High);
			}
		}else {
			TIM4->CCMR2 = (TIM4->CCMR2 & 0x8fff) | 0x1000;
			TIM_SetCompare4(TIM4, capture+Bridge_Low);
		}
		
		offset^=0x02;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
	}
}
