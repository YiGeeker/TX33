/* 显示屏使用的是HTM320240F1墨水屏 */
#ifndef __LCD_H
#define __LCD_H

#define	LCD_DATA_PORT GPIOE

#define	LCD_WR_GPIO GPIO_Pin_11	/* GPIOB */
#define	LCD_RD_GPIO GPIO_Pin_12	/* GPIOB */
#define	LCD_CD_GPIO GPIO_Pin_10	/* GPIOB */

#define	LCD_RST_GPIO GPIO_Pin_7	/* GPIOE */
#define	LCD_BLA_GPIO GPIO_Pin_6	/* GPIOA */


#define A0_0  GPIO_ResetBits(GPIOB, LCD_CD_GPIO)
#define A0_1  GPIO_SetBits(GPIOB, LCD_CD_GPIO)
#define WR_0  GPIO_ResetBits(GPIOB, LCD_WR_GPIO)
#define WR_1  GPIO_SetBits(GPIOB, LCD_WR_GPIO)
#define RD_0  GPIO_ResetBits(GPIOB, LCD_RD_GPIO)
#define RD_1  GPIO_SetBits(GPIOB, LCD_RD_GPIO)

#define RST_0  GPIO_ResetBits(GPIOE, LCD_RST_GPIO)
#define RST_1  GPIO_SetBits(GPIOE, LCD_RST_GPIO)	

#define BGL_LOW GPIO_ResetBits(GPIOA, LCD_BLA_GPIO)
#define BGL_HIGH  GPIO_SetBits(GPIOA, LCD_BLA_GPIO)	

#define BUSY GPIO_ReadInputDataBit(LCD_DATA_PORT, GPIO_Pin_14) /* 读取RA8835的忙状态 */
#define DATAOUT LCD_DATA_PORT->ODR							   /* 数据输出 */
#define DATAIN  LCD_DATA_PORT->IDR							   /* 数据输入 */

#define BASEPART1 0x00
#define BASEPART2 0x28
#define BASEPART3 0x50
#define BASEPART4 0x00

#define WIDTH 320				/* 图像宽度大小 */
#define HEIGHT 240				/* 图像高度大小 */

typedef enum Lcd_Sys_Cmd
{
	SystemSet  = 0x40,			/* 初始化设置，后续8字节参数串systemset_P8[] */
	Scroll     = 0x44,			/* 设定显示起始地址和显示范围，后续10字节参数串scroll_P10[] */
	BasePart1  = 0x00,
	BasePart2  = 0x28,
	BasePart3  = 0x50,
	BasePart4  = 0x00,

	Sleep      = 0x53,			/* 休闲模式设置，后续参数：无 */
	CursorSet  = 0x46,			/* 设置光标地址，后续参数2字节 */
	CursorGet  = 0x47,			/* 读出光标地址，后续参数2字节 */
	CursorR    = 0x4c,			/* 光标向右方向移动，后续参数：无 */
	CursorL    = 0x4d,			/* 光标向左方向移动，后续参数：无 */
	CursorU    = 0x4e,			/* 光标向上方向移动，后续参数：无 */
	CursorD    = 0x4f,			/* 光标向下方向移动，后续参数：无 */
	CursorForm = 0x5d,			/* 光标形状设置，后续参数2字节 */
	ShowOff    = 0x58,			/* 显示关闭，后续参数1字节 */
	ShowOn     = 0x59,			/* 显示状态打开，后续参数1字节 */
	PixelMove  = 0x5a,			/* 点位移量设置，后续参数1字节 */
	Overlay    = 0x5b,			/* 显示合成设置，后续参数1字节 */
	CgramAddr  = 0x5c,			/* CGRAM首地址设置，后续参数2字节 */
	DataWrite  = 0x42,			/* 显示数据写入，后续参数N字节 */
	DataRead   = 0x43,			/* 显示数据读取，后续参数N字节 */
	Busy       = 0x40			/* 忙信号D6，0x40 */
}LCDSYSCMD;

typedef struct struct_Wnd_Rect
{
	uint8_t top;				/* 窗口显示文字的上边界坐标 */
	uint16_t left;				/* 窗口的左边界坐标 */
	uint16_t right;				/* 窗口的右边界坐标 */
}WNDRECT;						/* 窗口坐标结构体 */

typedef enum enum_WndItem
{
	SetVoltage,					/* 设置电压窗口 */
	GetVoltage,					/* 获取电压窗口 */
	/* SetWaveform,				/\* 设置波形窗口 *\/ */
	SetCurrent,					/* 设置电流窗口 */
	GetCurrent,					/* 获取电流窗口 */
	SetFreq,					/* 设置频率窗口 */
	GetFreq,					/* 获取频率窗口 */
	SetDuty,					/* 设置占空比窗口 */
	GetDuty						/* 获取占空比窗口 */
}WNDITEM;

void Lcd_GPIO_Config(void);		/* LCD的GPIO初始化 */
void Lcd_Init(void);			/* LCD显示初始化 */
void Lcd_Show_On(void);			/* 开启显示 */
void Lcd_Show_Off(void);		/* 关闭显示 */
void Lcd_Cls(uint8_t Part);		/* 液晶清屏， */
void Lcd_Draw_Pixel(uint16_t x, uint8_t y, uint8_t Attr); /* 显示点 */
void Lcd_Draw_Pic(uint8_t x, uint8_t y, const uint8_t *lpString, uint8_t Inverse); /* 显示图片 */
void Lcd_Draw_Text(uint8_t x, uint8_t y, uint8_t Height, const uint8_t *lpString, uint16_t Size, uint8_t Inverse); /* 写入一行文字 */
void Lcd_Draw_Number_Text(uint8_t x, uint8_t y, uint8_t Height, const char *lpString, uint8_t Inverse); /* 写入一行数字 */
void Lcd_Locate_Pixel_XY(uint16_t x, uint8_t y); /* 光标定位 */
void Lcd_Locate_Char_XY(uint8_t x, uint8_t y);	 /* 光标定位 */
void Lcd_Check_Busy(void);						 /* 忙检测 */
void Lcd_Send_Cmd(LCDSYSCMD Cmd);				 /* 写命令子函数 */
void Lcd_Send_Data(uint8_t Data);				 /* 写数据 */
uint8_t Lcd_Read_Data(void);					 /* 读数据 */

void Lcd_Show_Logo(void);		/* 显示开机电子所标志 */
void Lcd_Show_Wait_Text(void);	/* 显示等待画面 */
void Lcd_Show_Main_Sheet(void);							   /* 显示主操作表格 */
void Lcd_Set_Waveform(WAVEFORM Waveform, uint8_t Inverse); /* 显示波形文字 */
void Lcd_Set_Item_Int(WNDITEM Item, uint16_t Value, uint8_t Inverse); /* 设置窗口显示整数 */
void Lcd_Set_Item_Float(WNDITEM Item, float Value, uint8_t Num, uint8_t Inverse); /* 设置窗口显示小数 */

#endif

