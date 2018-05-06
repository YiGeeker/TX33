/* 显示屏使用的是HTM320240F1墨水屏 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "definition.h"
#include "lcd.h"
#include "lcd_show_data.h"

uint8_t SystemSet_P8[] =  {0x30, 0x87, 0x07, 0x27, 0x3f, 0xef, 0x28, 0x00};

uint8_t Scroll_P10[] = {0x00, BASEPART1, 0xf0, /* 第一显示区RAM，10KB，240行 */
						0x00, BASEPART2, 0xf0, /* 第二显示区RAM，10KB，240行 */
						0x00, BASEPART3,		/* 第三显示区RAM，10KB */
						0x00, BASEPART4};		/* 第四显示区RAM，32KB */

WNDRECT Wnd_Set_Waveform;		/* 设置波形窗口的位置信息 */
WNDRECT Wnd_Set_Current;		/* 设置电流窗口的位置信息 */
WNDRECT Wnd_Get_Current;		/* 获取电流窗口的位置信息 */
/* WNDRECT Wnd_Set_Voltage; */
/* WNDRECT Wnd_Get_Voltage; */
WNDRECT Wnd_Set_Freq;			/* 设置频率窗口的位置信息 */
WNDRECT Wnd_Get_Freq;			/* 获取频率窗口的位置信息 */
WNDRECT Wnd_Set_Duty;			/* 设置占空比窗口的位置信息 */
WNDRECT Wnd_Get_Duty;			/* 获取占空比窗口的位置信息 */

void Lcd_Delay_Nus(uint16_t nCount); /* 微秒级延时 */
void Lcd_Delay_IO(uint32_t nCount);
WNDRECT *Get_Item_Struct(WNDITEM); /* 获取窗口控件的位置信息 */
void Lcd_Clear_Wnd_Rect(WNDRECT *lpWnd, uint8_t Height); /* 清除窗口内的数据 */

/* 微秒级延时 */
void Lcd_Delay_Nus(uint16_t nCount)
{
    while(nCount--);
}

void Lcd_Delay_IO(uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

/* LCD的GPIO初始化 */
void Lcd_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LCD_BLA_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_WR_GPIO | LCD_RD_GPIO | LCD_CD_GPIO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15 | LCD_RST_GPIO; /* 液晶数据口为PE8~PE15 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOE, &GPIO_InitStructure);
		
	RST_0;
	Lcd_Delay_IO(1000);
	RST_1;    	
}

/* LCD显示初始化 */
void Lcd_Init(void)
{
	uint8_t i;

	BGL_LOW;					/* 开启背景光 */
	
	Lcd_Send_Cmd(SystemSet);	/* 初始化设置 */	
	for(i=0;i<8 ;i++)
		Lcd_Send_Data(SystemSet_P8[i]);
	
	Lcd_Send_Cmd(Scroll);		/* 设定显示起始地址和显示范围 */
	for(i=0;i<10;i++) 
		Lcd_Send_Data(Scroll_P10[i]);
	
	Lcd_Send_Cmd(PixelMove);		/* 点位移量设置 */
	Lcd_Send_Data(0x00);		/* 不移动  */
	
	Lcd_Send_Cmd(Overlay);		/* 显示合成设置    */
	Lcd_Send_Data(0x00);		/* 二重合成，文本显示，逻辑或 */
	
	Lcd_Show_Off();					/* 关闭显示 */
	Lcd_Cls(0);					/* 液晶清屏，32KB RAM 清零  */
	Lcd_Show_On();					/* 开启显示 */
}

/******************************************************* 
                 	开启显示
*******************************************************/
void Lcd_Show_On(void)
{
	Lcd_Send_Cmd(ShowOn);		/* 显示状态开启 */
	Lcd_Send_Data(0x57);		/* 显示图像，光标1Hz闪烁 */
}

/******************************************************* 
                  	关闭显示
*******************************************************/
void Lcd_Show_Off(void)
{
	Lcd_Send_Cmd(ShowOff);		/* 显示状态关闭 */
	Lcd_Send_Data(0x57);		/* 显示图像，光标1Hz闪烁 */
}

/******************************************************* 
           液晶清屏， 10KB RAM 清零
           Part = 0 清所有显示区 32KB RAM
           Part = 1 清显示区1
           Part = 2 清显示区2
           Part = 3 清显示区3
           Part = 4 清显示区4                 
*******************************************************/
void Lcd_Cls(uint8_t Part)
{
	uint32_t i = 10240;			/* 10240 = 0x2800  10KB RAM */
 
	Lcd_Send_Cmd(CursorSet);					 /* 写入光标地址 */
	Lcd_Send_Data(0x00);						 /* 先写低八位 */
	
	if(Part == 1) Lcd_Send_Data(BASEPART1);		 /* 显示区1 */
	else if(Part == 2) Lcd_Send_Data(BASEPART2); /* 显示区2 */
	else if(Part == 3) Lcd_Send_Data(BASEPART3); /* 显示区3 */
	else if(Part == 4) Lcd_Send_Data(BASEPART4); /* 显示区4 */
	else {i = 32768;   Lcd_Send_Data(0x00);}	 /* 所有显示区 32KB RAM */
	
	Lcd_Send_Cmd(CursorR);						 /* 光标向右方向移动 */
	Lcd_Send_Cmd(DataWrite);					 /* 显示数据写入 */
	while(i--) Lcd_Send_Data(0x00);				 /* 写入0 */
}

/******************************************************* 
                       显示点
                 x横坐标: 0~319 320
                 y纵坐标: 0~239 240
                 Attr显示属性：0-画点，1-消点
*******************************************************/
void Lcd_Draw_Pixel(uint16_t x, uint8_t y, uint8_t Attr)
{ 
	uint8_t temp, tempd;
	
	tempd = 0x80 >> (x & 0x07);
	Lcd_Locate_Pixel_XY(x, y);	/* 光标定位 */
	Lcd_Send_Cmd(DataRead);
	
	temp =Lcd_Read_Data();
	if(Attr) temp &= ~tempd;
	else  temp |= tempd;
	
	Lcd_Locate_Pixel_XY(x, y);	/* 光标定位 */	
	Lcd_Send_Cmd(DataWrite);	/* 显示数据写入 */
	Lcd_Send_Data(temp);
}

/******************************************************* 
                    显示图片
              x横坐标: 0~39 320/8=40
              y纵坐标: 0~239 240
*******************************************************/
void Lcd_Draw_Pic(uint8_t x, uint8_t y, const uint8_t *lpString, uint8_t Inverse)
{
	uint8_t i, j;
	
	Lcd_Send_Cmd(CursorD);		/* 光标向下方向移动 */
	for(j=0; j<WIDTH/8; j++)
	{ 
	    Lcd_Locate_Char_XY(x+j, y); /* 光标定位 */
		Lcd_Send_Cmd(DataWrite);	 /* 显示数据写入 */
		for(i=0; i<HEIGHT; i++) 
		{
			if(Inverse) Lcd_Send_Data(~*lpString);
			else  Lcd_Send_Data(*lpString); 
			lpString++;
		}
	}
}

/******************************************************* 
                   写入一行文字
              x横坐标: 0~39 320/8=40
              y纵坐标: 0~239 240
*******************************************************/
void Lcd_Draw_Text(uint8_t x, uint8_t y, uint8_t Height, const uint8_t *lpString, uint16_t Size, uint8_t Inverse)
{
	uint8_t i, j;
	
	Lcd_Send_Cmd(CursorD);		/* 光标向下方向移动 */
	for(i=0; i<Size/Height; i++)
	{
		Lcd_Locate_Char_XY(x+i, y);
		Lcd_Send_Cmd(DataWrite);
		for(j=0; j<Height; j++) 
		{
			if(Inverse) Lcd_Send_Data(~*lpString);
			else  Lcd_Send_Data(*lpString); 
			lpString++;
		}
	}
}

/******************************************************* 
                   写入一行数字
              x横坐标: 0~39 320/8=40
              y纵坐标: 0~239 240
*******************************************************/
void Lcd_Draw_Number_Text(uint8_t x, uint8_t y, uint8_t Height, const char *lpString, uint8_t Inverse)
{
	uint8_t i = 0, n = 0;
	uint8_t *lpData, *lpDataTemp;

	n = strlen(lpString);
	lpData = (uint8_t *)malloc(sizeof(uint8_t)*16*n);
	lpDataTemp = lpData;

	for(i=0; i<n; i++)
	{
		if(*lpString >= '0' && *lpString <= '9')
		{
			lpDataTemp = memcpy(lpDataTemp, number_text[*lpString-'0'], 16);
		}else
		{
			lpDataTemp = memcpy(lpDataTemp, number_text[10], 16);
		}
	    
		lpDataTemp += sizeof(uint8_t)*16;
		lpString += sizeof(uint8_t);
	}

	Lcd_Draw_Text(x, y, Height, lpData, sizeof(uint8_t)*16*n, Inverse);
	
	free(lpData);
}

/******************************************************* 
                  	   光标定位
                  x横坐标: 0~319 320
                  y纵坐标: 0~239 240
                  图层2
*******************************************************/
void Lcd_Locate_Pixel_XY(uint16_t x, uint8_t y)
{
	uint16_t addr;
 
	addr = (uint16_t)(y) * 0x28 + x/8;
	Lcd_Send_Cmd(CursorSet);						 /* 写入光标地址 */
	Lcd_Send_Data((uint8_t)(addr & 0xff));			 /* 先写低八位 */
	Lcd_Send_Data((uint8_t)(addr >> 8) + BASEPART2); /* 后写高八位 */
}

/******************************************************* 
                       光标定位
                  x横坐标: 0~39 320/8=40
                  y纵坐标: 0~239 240
                  图层2                 
*******************************************************/
void Lcd_Locate_Char_XY(uint8_t x, uint8_t y)
{
    uint16_t addr;
	
	addr = (uint16_t)(y) * 0x28 + x;
	Lcd_Send_Cmd(CursorSet);						 /* 写入光标地址 */
	Lcd_Send_Data((uint8_t)(addr & 0xff));			 /* 先写低八位 */
	Lcd_Send_Data((uint8_t)(addr >> 8) + BASEPART2); /* 后写高八位 */
}

/* 忙检测 */
void Lcd_Check_Busy(void)
{
	/* 将PE14设为输入下拉 */
	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_6);
    LCD_DATA_PORT->CRH = (LCD_DATA_PORT->CRH & 0xf0ffffff) | 0x08000000; 
    
    WR_1;						/* 写禁用 */
    A0_0;
    RD_0;						/* 读使能 */
    Lcd_Delay_Nus(0);
    while(BUSY==1);
    RD_1;   
		
    /* 将PE14设为输出推挽 */
	LCD_DATA_PORT->CRH = (LCD_DATA_PORT->CRH & 0xf0ffffff) | 0x03000000;
}

/* cmd, High:DATA, Low:Instruction Code-> A0 */
/* 写命令子函数 */
void Lcd_Send_Cmd(LCDSYSCMD Cmd)
{
    Lcd_Check_Busy();

    RD_1;
    A0_1; 
	DATAOUT = (DATAOUT & 0x00ff) + ((uint8_t)Cmd<<8);
    WR_0;
    Lcd_Delay_Nus(10);
    WR_1;						/* 上升沿写入 */
}

/* 写数据 */
void Lcd_Send_Data(uint8_t Data)
{
    Lcd_Check_Busy ();
    
    RD_1;
    A0_0;   
	DATAOUT = (DATAOUT & 0x00ff) + (Data<<8);
    WR_0;
    Lcd_Delay_Nus(10);
    WR_1;						/* 上升沿写入 */
}

/* 读数据 */
uint8_t Lcd_Read_Data(void)
{
    uint8_t temp = 0;
    
	/* 将PE8~PE15设为输入 */
	LCD_DATA_PORT->CRH = 0x88888888;
    
    A0_1;
    WR_1;
    RD_0;
	Lcd_Delay_Nus(5);
	temp = (DATAIN >> 8) & 0x00FF; /* 获取PE8~PE15的数据 */
    RD_1;
	A0_1;
    
	/* 将PE8~PE15设为推挽输出 */
	LCD_DATA_PORT->CRH = 0x33333333;
	
    return temp;
}

/***************************************************************************************
具体操作函数的应用
***************************************************************************************/
/* 显示开机电子所标志 */
inline void Lcd_Show_Logo(void)
{
	Lcd_Draw_Pic(0, 0, logo_IECAS, 0);
}

/* 显示等待画面 */
void Lcd_Show_Wait_Text(void)
{
	Lcd_Draw_Text(2, 36, 32, wait_text_line1, sizeof(wait_text_line1), 0);
	Lcd_Draw_Text(8, 104, 32, wait_text_line2, sizeof(wait_text_line2), 0);
	Lcd_Draw_Text(11, 172, 32, wait_text_line3, sizeof(wait_text_line3), 0);
}

/* 显示主操作表格 */
void Lcd_Show_Main_Sheet(void)
{
	uint16_t i = 0;
	uint8_t width = 2, j = 0;

	/* 画所有的横线 */
	for(i=0; i<WIDTH; i++)
	{
		for(j=0; j<width; j++)
		{
			Lcd_Draw_Pixel(i, j, 0);
			Lcd_Draw_Pixel(i, 38+j, 0);
			Lcd_Draw_Pixel(i, 78+j, 0);
			Lcd_Draw_Pixel(i, 118+j, 0);
			Lcd_Draw_Pixel(i, 158+j, 0);
			Lcd_Draw_Pixel(i, 198+j, 0);
			Lcd_Draw_Pixel(i, 238+j, 0);
		}
	}

	/* 画斜线 */
	for(i=0; i<104; i++)
	{
		Lcd_Draw_Pixel(210+i, 38+5*i/13, 0);
	}

	for(i=0; i<104; i++)
	{
		Lcd_Draw_Pixel(104+i, 78+5*i/13, 0);
	}
	
	
	/* 画所有的竖线 */
	for(i=0; i<HEIGHT; i++)
	{
		for(j=0; j<width; j++)
		{
			Lcd_Draw_Pixel(j, i, 0);
			Lcd_Draw_Pixel(104+j, i, 0);
			Lcd_Draw_Pixel(318+j, i, 0);
		}
	}

	for(i=0; i<HEIGHT-40; i++)
	{
		for(j=0; j<width; j++)
		{
			Lcd_Draw_Pixel(210+j, i, 0);
		}
	}

	/* 添加各标题，顺序为从左往右，从上到下 */
	Lcd_Draw_Text(5, 12, 16, TX33_caption_text, sizeof(TX33_caption_text), 0); /* 显示“TX33” */
	Lcd_Draw_Text(17, 12, 16, set_value_caption_text, sizeof(set_value_caption_text), 0); /* 显示“设定值” */
	Lcd_Draw_Text(30, 12, 16, get_value_caption_text, sizeof(get_value_caption_text), 0); /* 显示“实测值” */
	Lcd_Draw_Text(3, 51, 16, waveform_caption_text, sizeof(waveform_caption_text), 0); /* 显示“发射波形” */
	Lcd_Draw_Text(2, 91, 16, current_cation_text, sizeof(current_cation_text), 0); /* 显示“发射电流(A)” */
	/* Lcd_Draw_Text(2, 131, 16, voltage_caption_text, sizeof(voltage_caption_text), 0); /\* 显示“发射电压(V)” *\/ */
	Lcd_Draw_Text(1, 131, 16, freq_caption_text, sizeof(freq_caption_text), 0); /* 显示“发射频率(Hz)” */
	Lcd_Draw_Text(2, 171, 16, duty_caption_text, sizeof(duty_caption_text), 0); /* 显示“占空比(%)” */
	Lcd_Draw_Text(3, 211, 16, GPS_info_caption_text, sizeof(GPS_info_caption_text), 0); /* 显示“GPS信息” */

	/* 设置各窗口坐标 */
	Wnd_Set_Waveform.top = 51;
	Wnd_Set_Waveform.left = 106;
	Wnd_Set_Waveform.right = 210;
	
	Wnd_Set_Current.top = 91;
	Wnd_Set_Current.left = 106;
	Wnd_Set_Current.right = 210;

	Wnd_Get_Current.top = 91;
	Wnd_Get_Current.left = 212;
	Wnd_Get_Current.right = 318;
	
	/* Wnd_Set_Voltage.top = 131; */
	/* Wnd_Set_Voltage.left = 106; */
	/* Wnd_Set_Voltage.right = 210; */

	/* Wnd_Get_Voltage.top = 131; */
	/* Wnd_Get_Voltage.left = 212; */
	/* Wnd_Get_Voltage.right = 318; */
	
	Wnd_Set_Freq.top = 131;
	Wnd_Set_Freq.left = 106;
	Wnd_Set_Freq.right = 210;

	Wnd_Get_Freq.top = 131;
	Wnd_Get_Freq.left = 212;
	Wnd_Get_Freq.right = 318;

	Wnd_Set_Duty.top = 171;
	Wnd_Set_Duty.left = 106;
	Wnd_Set_Duty.right = 210;

	Wnd_Get_Duty.top = 171;
	Wnd_Get_Duty.left = 212;
	Wnd_Get_Duty.right = 318;
}

/* 显示波形文字 */
void Lcd_Set_Waveform(WAVEFORM Waveform, uint8_t Inverse)
{
	Lcd_Clear_Wnd_Rect(Get_Item_Struct(SetWaveform), 16);
	if(Waveform == SQUARE)
	{
		Lcd_Draw_Text(18, 51, 16, square_text, sizeof(square_text), Inverse);
	}else if(Waveform == TRIANGULAR)
	{
		Lcd_Draw_Text(17, 51, 16, triangular_text, sizeof(triangular_text), Inverse);
	}
}

/* 设置窗口显示整数 */
void Lcd_Set_Item_Int(WNDITEM Item, uint16_t Value, uint8_t Inverse)
{
	uint8_t n = 0, x = 0;
	WNDRECT *lpWnd;
	char Buffer[8];

	lpWnd = Get_Item_Struct(Item);
	Lcd_Clear_Wnd_Rect(lpWnd, 16);
	
    sprintf(Buffer, "%d", Value);
	n = strlen(Buffer);

	x = (uint8_t)((lpWnd->left+(lpWnd->right-lpWnd->left-8*n)/2+4) >> 3);

	Lcd_Draw_Number_Text(x, lpWnd->top, 16, Buffer, Inverse);
}

/* 设置窗口显示小数 */
void Lcd_Set_Item_Float(WNDITEM Item, float Value, uint8_t Num, uint8_t Inverse)
{
	uint8_t n = 0, x = 0;
	WNDRECT *lpWnd;
	char Buffer[8];

	lpWnd = Get_Item_Struct(Item);
	Lcd_Clear_Wnd_Rect(lpWnd, 16);
	
    sprintf(Buffer, "%.*f", Num, Value);
	n = strlen(Buffer);

	x = (uint8_t)((lpWnd->left+(lpWnd->right-lpWnd->left-8*n)/2+4) >> 3);

	Lcd_Draw_Number_Text(x, lpWnd->top, 16, Buffer, Inverse);
}

/* 清除窗口内的数据 */
void Lcd_Clear_Wnd_Rect(WNDRECT *lpWnd, uint8_t Height)
{
	uint8_t x, n;
	uint8_t i, j;

	x = (uint8_t)((lpWnd->left >> 3)+1);
	n = (uint8_t)((lpWnd->right >> 3)-x);
	
	Lcd_Send_Cmd(CursorD);		/* 光标向下方向移动 */
	for(i=0; i<n; i++)
	{
		Lcd_Locate_Char_XY(x+i, lpWnd->top);
		Lcd_Send_Cmd(DataWrite);
		for(j=0; j<Height; j++) 
		{
			Lcd_Send_Data(0x00);
		}
	}
}

/* 获取窗口控件的位置信息 */
WNDRECT *Get_Item_Struct(WNDITEM Item)
{
	switch (Item)
	{
	case SetWaveform:
	{
		return &Wnd_Set_Waveform;
	}
	case SetCurrent:
	{
		return &Wnd_Set_Current;
	}
	case GetCurrent:
	{
		return &Wnd_Get_Current;
	}
	/* case SetVoltage: */
	/* { */
	/*	return &Wnd_Set_Voltage; */
	/* } */
	/* case GetVoltage: */
	/* { */
	/* 	return &Wnd_Get_Voltage; */
	/* } */
	case SetFreq:
	{
		return &Wnd_Set_Freq;
	}
	case GetFreq:
	{
		return &Wnd_Get_Freq;
	}
	case SetDuty:
	{
		return &Wnd_Set_Duty;
	}
	case GetDuty:
	{
		return &Wnd_Get_Duty;
	}
	default:
		return NULL;
	}
}
