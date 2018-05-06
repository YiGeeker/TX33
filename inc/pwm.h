#ifndef __PWM_H
#define	__PWM_H

/* 各MOSFET管驱动GPIO定义 */
#define PWM_PUL_GPIO GPIO_Pin_6	/* GPIOC */
#define PWM_PUR_GPIO GPIO_Pin_7	/* GPIOC */
#define PWM_PDL_GPIO GPIO_Pin_14 /* GPIOD */
#define PWM_PDR_GPIO GPIO_Pin_15 /* GPIOD */
#define PWM_PSW_GPIO GPIO_Pin_12 /* GPIOD */

void Pwm_GPIO_Config(void);		/* 初始化PWM的GPIO */
void Pwm_NVIC_Config(void);		/* 初始化PWM的NVIC */
void Pwm_Init(uint16_t nFreq, float fDuty); /* 初始化PWM的计数器和各通道，nFreq为初始频率，fDuty为初始占空比，频率可调范围为1-500Hz */
void Pwm_Set_Value(uint16_t nFreq, float fDuty); /* 设置PWM的频率和占空比 */
void Pwm_Switch_Enable(void);					 /* 打开外接开关 */
void Pwm_Enable(void);							 /* 打开除外接开关以外的PWM */
void Pwm_Disable(void);							 /* 关闭所有PWM，包括外接开关 */

#endif
