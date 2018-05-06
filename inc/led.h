#ifndef __LED_H
#define __LED_H

/* TX+的GPIO定义 */
#define LED_TXP_R GPIO_Pin_6	/* 亮红色的GPIO */
#define LED_TXP_B GPIO_Pin_4	/* 亮蓝色的GPIO */

#define LED_TXP_R_PORT GPIOA
#define LED_TXP_B_PORT GPIOC

/* TX-的GPIO */
#define LED_TXN_R GPIO_Pin_4	/* 亮红色的GPIO */
#define LED_TXN_B GPIO_Pin_3	/* 亮蓝色的GPIO */

#define LED_TXN_R_PORT GPIOA
#define LED_TXN_B_PORT GPIOC

/* GPS的GPIO */
#define LED_GPS_B GPIO_Pin_1	/* 亮蓝色的GPIO */
#define LED_GPS_G GPIO_Pin_2	/* 亮绿色的GPIO */

#define LED_GPS_B_PORT GPIOC
#define LED_GPS_G_PORT GPIOC

/* STATUS的GPIO */
#define LED_STA_R GPIO_Pin_7	/* 亮红色的GPIO */
#define LED_STA_G GPIO_Pin_5	/* 亮绿色的GPIO */

#define LED_STA_R_PORT GPIOA
#define LED_STA_G_PORT GPIOA

typedef enum LED_Color {Blue, Green, Red}LED_COLOR; /* LED颜色的可能值 */

void Led_GPIO_Config(void);

void Led_TXP(LED_COLOR Color);	  /* TX+显示设置，只有蓝色和红色 */
void Led_TXN(LED_COLOR Color);	  /* TX-显示设置，只有蓝色和红色 */
void Led_GPS(LED_COLOR Color);	  /* GPS显示设置，只有红色和绿色 */
void Led_STATUS(LED_COLOR Color); /* STATUS显示设置，只有绿色和红色 */

#endif
