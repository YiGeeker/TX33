#include "misc.h"
#include "definition.h"
#include "pwm.h"
#include "led.h"
#include "lcd.h"
#include "key.h"

MESSAGE message = None;				/* 当前操作消息 */
WAVEFORM current_waveform = SQUARE; /* 当前波形 */
WNDITEM current_item = SetWaveform; /* 当前操作窗口 */
uint16_t freq = 25;					/* 发射频率 */
uint8_t duty_square = 50;		 /* 方波的占空比，已乘以100转换为整数后的值 */
float duty_triangular = 3.75e-3; /* 三角波的占空比 */

void TranslateMessage(void);	/* 处理当前消息 */

/* 延时函数 */
static void Delay(uint32_t Time)
{
	unsigned char j;
    
	while(Time--)
	{	
		for(j=0; j<=20; j++);                 
	}
}

int main (void)
{
    Pwm_GPIO_Config();
	Pwm_NVIC_Config();
    Led_GPIO_Config();
	Lcd_GPIO_Config();
    Key_GPIO_Config();
    Key_NVIC_Config();
	
	Key_All_Disable();
	Lcd_Init();
	Pwm_Init(freq, duty_square/100.0);
	
    Lcd_Show_Logo();
    Delay(1000000);
    Lcd_Cls(0);
    Lcd_Show_Wait_Text();
    Delay(1000000);
    Lcd_Cls(0);
    Lcd_Show_Main_Sheet();
	
	SendMessage(WndInit);
	
    while (1)
	{
		TranslateMessage();
	}
}

/* 处理当前消息 */
void TranslateMessage(void)
{
	switch(message)
	{
	case None: return;
	case SelectNext:
	{
		if(current_waveform == SQUARE)
		{
			switch(current_item)
			{
			case SetWaveform:
			{
				current_item = SetFreq;
				Lcd_Set_Waveform(current_waveform, 0);

				Lcd_Set_Item_Int(SetDuty, duty_square, 0);
				
				Lcd_Set_Item_Int(SetFreq, freq, 1);
				break;
			}
			case SetFreq:
			{
				current_item = SetDuty;
				Lcd_Set_Item_Int(SetFreq, freq, 0);
				Lcd_Set_Item_Int(SetDuty, duty_square, 1);
				break;
			}
			case SetDuty:
			{
				current_item = SetWaveform;
				Lcd_Set_Item_Int(SetDuty, duty_square, 0);
				Lcd_Set_Waveform(current_waveform, 1);
				break;
			}
            default: break;
			}
		}else
		{
			switch(current_item)
			{
			case SetWaveform:
			{				
				current_item = SetFreq;
				Lcd_Set_Waveform(current_waveform, 0);

				duty_triangular = (float)(75e-6*2*freq);
				Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0);
				
				Lcd_Set_Item_Int(SetFreq, freq, 1);
				break;
			}
			case SetFreq:
			{
				current_item = SetWaveform;
				Lcd_Set_Item_Int(SetFreq, freq, 0);
				Lcd_Set_Waveform(current_waveform, 1);
				break;
			}
            default: break;
			}
		}
		
		message = None;			/* 这条语句不能放在整个函数的最后，否则会使消息还未处理就被清除 */
		break;
	}
	case AddSelect:
	{
		switch(current_item)
		{
		case SetWaveform:
		{
			if(current_waveform == SQUARE)
			{
				current_waveform = TRIANGULAR;
			}else
			{
				current_waveform = SQUARE;
			}
			Lcd_Set_Waveform(current_waveform, 1);
			break;
		}
		case SetFreq:
		{
			if(freq >= 500)
			{
				freq = 1;
			}else
			{
				freq++;
			}
			Lcd_Set_Item_Int(SetFreq, freq, 1);

			if(current_waveform == TRIANGULAR)
			{				
				duty_triangular = (float)(75e-6*2*freq);
				Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0);
			}
			
			break;
		}
		case SetDuty:
		{
			if(duty_square >= 100)
			{
				duty_square = 5;
			}else
			{
				duty_square += 5;
			}
			Lcd_Set_Item_Int(SetDuty, duty_square, 1);
			break;
		}
        default: break;
		}

		message = None;
		break;
	}
	case SubSelect:
	{
		switch(current_item)
		{
		case SetWaveform:
		{
			if(current_waveform == SQUARE)
			{
				current_waveform = TRIANGULAR;
			}else
			{
				current_waveform = SQUARE;
			}
			Lcd_Set_Waveform(current_waveform, 1);
			break;
		}
		case SetFreq:
		{
			if(freq <= 1)
			{
				freq = 500;
			}else
			{
				freq--;
			}
			Lcd_Set_Item_Int(SetFreq, freq, 1);

			if(current_waveform == TRIANGULAR)
			{				
				duty_triangular = (float)(75e-6*2*freq);
				Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0);
			}
			
			break;
		}
		case SetDuty:
		{
			if(duty_square <= 5)
			{
				duty_square = 100;
			}else
			{
				duty_square -= 5;
			}
			Lcd_Set_Item_Int(SetDuty, duty_square, 1);
			break;
		}
        default: break;
		}
		message = None;
		break;
	}
	case TransmitOn:
	{
		switch(current_item)
		{
		case SetWaveform:
		{
			Lcd_Set_Waveform(current_waveform, 0);

			if(current_waveform == SQUARE)
			{
				Lcd_Set_Item_Int(SetDuty, duty_square, 0);
			}else
			{
				duty_triangular = (float)(75e-6*2*freq);
				Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0);
			}
			
			break;
		}
		case SetFreq:
		{
			Lcd_Set_Item_Int(SetFreq, freq, 0);
			break;
		}
		case SetDuty:
		{
			Lcd_Set_Item_Int(SetDuty, duty_square, 0);
			break;
		}
		default: break;
		}

		if(current_waveform == SQUARE)
		{
			Pwm_Set_Value(freq, duty_square/100.0);
		}else
		{
			Pwm_Set_Value(freq, duty_triangular);
			Pwm_Switch_Enable();
		}
		Pwm_Enable();
		Led_STATUS(Red);
		
		message = None;
		break;
	}
	case TransmitOff:
	{
		switch(current_item)
		{
		case SetWaveform:
		{
			Lcd_Set_Waveform(current_waveform, 1);
			break;
		}
		case SetFreq:
		{
			Lcd_Set_Item_Int(SetFreq, freq, 1);
			break;
		}
		case SetDuty:
		{			
			Lcd_Set_Item_Int(SetDuty, duty_square, 1);
			break;	
		}
		default: break;
		}
		
		Pwm_Disable();
		Led_STATUS(Green);
		
		message = None;
		break;
	}
	case WndInit:
	{
		Lcd_Set_Waveform(current_waveform, 1);
		Lcd_Set_Item_Int(SetFreq, freq, 0);
		Lcd_Set_Item_Int(SetDuty, duty_square, 0);
		Led_STATUS(Green);
		Key_All_Enable();

		message = None;
		break;
	}
	default:
		break;
	}
}

/* 发送消息给消息处理函数 */
inline void SendMessage(MESSAGE Message)
{
	message = Message;
}
