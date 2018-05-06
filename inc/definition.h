#ifndef __DEFINITION_H
#define __DEFINITION_H

typedef enum
{
	WndInit,					/* 窗口初始化 */
	SelectNext,					/* 菜单选择 */
	AddSelect,					/* 增加当前选择值 */
	SubSelect,					/* 减少当前选择值 */
	TransmitOn,					/* 开始发射 */
	TransmitOff,				/* 发射关闭 */
	None						/* 没有消息处理 */
}MESSAGE;						/* 当前消息可能值 */

typedef enum enum_Waveform {SQUARE, TRIANGULAR}WAVEFORM; /* 当前波形可能值 */

void SendMessage(MESSAGE Message); /* 发送消息给消息处理函数 */

#endif
