#ifndef __KEY_H
#define __KEY_H

#define KEY_UP_GPIO	    GPIO_Pin_2
#define KEY_DOWN_GPIO	GPIO_Pin_3
#define KEY_MENU_GPIO	GPIO_Pin_4
#define KEY_ON_GPIO	    GPIO_Pin_5
#define KEY_OFF_GPIO	GPIO_Pin_6

#define KEY_UP_PORT	    GPIOE
#define KEY_DOWN_PORT	GPIOE
#define KEY_MENU_PORT	GPIOE
#define KEY_ON_PORT	    GPIOE
#define KEY_OFF_PORT	GPIOE

#define key_UP 0
#define key_DOWN 1
#define menu 2
#define key_ON 3
#define key_OFF 4

void Key_GPIO_Config(void);
void Key_NVIC_Config(void);
void Key_All_Disable(void);
void Key_All_Enable(void);

#endif
