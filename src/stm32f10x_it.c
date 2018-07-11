/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "definition.h"

extern uint16_t Bridge_High;	/* 桥路PWM输出高电平的计数值 */
extern uint16_t Bridge_Low;		/* 桥路PWM输出低电平的计数值 */
extern uint16_t SW_High;		/* 开关PWM输出高电平的计数值 */
extern uint16_t SW_Low;			/* 开关PWM输出低电平的计数值 */
extern uint8_t Transimit_On; /* 指示是否在发射电流，为0表示关闭，为1表示正在发射 */
extern uint8_t move_num;

extern void Delay_Nus(uint16_t nCount);
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

void EXTI2_IRQHandler(void)		/* 向上选择按键被触发 */
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET) /* 确保是否产生了EXTI Line中断 */
	{
		SendMessage(AddSelect);
		EXTI_ClearITPendingBit(EXTI_Line2); /* 清除中断标志位 */
	}
	 
}

void EXTI3_IRQHandler(void)		/* 向下选择按键被触发 */
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) /* 确保是否产生了EXTI Line中断 */
	{
		SendMessage(SubSelect);
		EXTI_ClearITPendingBit(EXTI_Line3); /* 清除中断标志位 */
	}  
}

void EXTI4_IRQHandler(void)		/* 菜单选择按键被触发 */
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) /* 确保是否产生了EXTI Line中断 */
	{		
        SendMessage(SelectNext);
		EXTI_ClearITPendingBit(EXTI_Line4); /* 清除中断标志位 */
	}  
}

void EXTI9_5_IRQHandler(void)	/* 发射按键被触发 */
{
	Delay_Nus(2);
//	if(GPIO_ReadInputDataBit(KEY_ON_PORT, KEY_ON_GPIO) == 0)
//	{
		if(EXTI_GetITStatus(EXTI_Line5) != RESET) /* 确保是否产生了EXTI Line中断 */
		{
			SendMessage(TransmitOn);

			/* 禁止menu按键操作 */
			EXTI->IMR &= ~(EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
			EXTI_ClearITPendingBit(EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
			//EXTI_ClearITPendingBit(EXTI_Line3);
			//EXTI_ClearITPendingBit(EXTI_Line4);
		
			EXTI_ClearITPendingBit(EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line8 | EXTI_Line9); /* 清除中断标志位 */
			//EXTI_ClearITPendingBit(EXTI_Line6);
			//EXTI_ClearITPendingBit(EXTI_Line7);
			//EXTI_ClearITPendingBit(EXTI_Line8);
			//EXTI_ClearITPendingBit(EXTI_Line9);
		}
//	}else if(GPIO_ReadInputDataBit(KEY_OFF_PORT, KEY_OFF_GPIO) == 0)
//	{
		else if(EXTI_GetITStatus(EXTI_Line6) != RESET) /* 确保是否产生了EXTI Line中断 */
		{
			SendMessage(TransmitOff);

			/* 使能menu按键操作 */
			EXTI->IMR |= (EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
			EXTI_ClearITPendingBit(EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
			//EXTI_ClearITPendingBit(EXTI_Line3);
			//EXTI_ClearITPendingBit(EXTI_Line4);
		
			EXTI_ClearITPendingBit(EXTI_Line5 | EXTI_Line6 | EXTI_Line7 | EXTI_Line8 | EXTI_Line9); /* 清除中断标志位 */
			//EXTI_ClearITPendingBit(EXTI_Line6);
			//EXTI_ClearITPendingBit(EXTI_Line7);
			//EXTI_ClearITPendingBit(EXTI_Line8);
			//EXTI_ClearITPendingBit(EXTI_Line9);
		}
//	}	
}

void TIM2_IRQHandler(void)
{
	uint16_t capture = 0;
	static uint8_t offset = 0x00; /* 电阻调节高低电平翻转控制 */
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		capture = TIM_GetCapture3(TIM2);

		if(offset&0x04)
		{
			if(move_num <= 0)
			{
				TIM_Cmd(TIM2, DISABLE);
			}else
			{
				move_num--;
				TIM2->CCMR2 = (TIM2->CCMR2&0xff8f)|0x0010;
				TIM_SetCompare3(TIM2, capture+22336);
			}
		}else
		{
			TIM2->CCMR2 = (TIM2->CCMR2&0xff8f)|0x0020;
			TIM_SetCompare3(TIM2, capture+43200);
		}

		offset^=0x04;
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	}
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

	/* TIM4通道1设置，即RES_UC */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		capture = TIM_GetCapture1(TIM4);
		
		if(offset&0x01)
		{
			if(move_num <= 0)
			{
				TIM_Cmd(TIM2, DISABLE);
			}else
			{
				move_num--;
				TIM4->CCMR1 = (TIM4->CCMR1&0xff8f)|0x0010;
				TIM_SetCompare1(TIM4, capture+22336);
			}
		}else
		{
			TIM4->CCMR1 = (TIM4->CCMR1&0xff8f)|0x0020;
			TIM_SetCompare1(TIM4, capture+43200);
		}
		
		offset^=0x01;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	}

	/* TIM4通道3设置，即PDL */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
	{
		capture = TIM_GetCapture3(TIM4);
		
		if (offset&0x04) {
			TIM4->CCMR2 = (TIM4->CCMR2 & 0xff8f) | 0x0010;
			TIM_SetCompare3(TIM4, capture+Bridge_Low);
		}else {
			TIM4->CCMR2 = (TIM4->CCMR2 & 0xff8f) | 0x0020;
			TIM_SetCompare3(TIM4, capture+Bridge_High);
		}
		
		offset^=0x04;		
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	}

	/* TIM4通道4设置，即PDR */
	if (TIM_GetITStatus(TIM4, TIM_IT_CC4) != RESET)
	{
		capture = TIM_GetCapture4(TIM4);

		if (offset&0x08) {
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
		
		offset^=0x08;
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
