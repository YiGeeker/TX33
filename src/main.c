#include "misc.h"
#include "definition.h"
#include "pwm.h"
#include "led.h"
#include "lcd.h"
#include "key.h"

MESSAGE message = None;				/* 当前操作消息 */
//WAVEFORM current_waveform = SQUARE; /* 当前波形 */
WNDITEM current_item = SetVoltage;	/* 当前操作窗口 */
uint16_t freq = 25;					/* 发射频率 */
uint8_t duty_square = 50;		 /* 方波的占空比，已乘以100转换为整数后的值 */
float duty_triangular = 3.75e-3; /* 三角波的占空比 */
const uint32_t R_trim_max = 50000;	 /* 电压调节电阻最大值 */
uint8_t R_trim_num = 14;			 /* 电压调节电阻当前位置 */
int8_t dR_trim_num = 0;			 /* 电压调节电阻调节值 */

void TranslateMessage(void);	/* 处理当前消息 */
float Rtrim2V(float R_trim);	/* 通过调压电阻得到电压 */

/* 延时函数 */
static void Delay(uint32_t Time)
{
	unsigned char j;
    
	while(Time--)
	{	
		for(j=0; j<=20; j++);                 
	}
}

/* 微秒级延时 */
void Delay_Nus(uint16_t nCount)
{
    while(nCount--);
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
	
    /* Lcd_Show_Logo(); */
    /* Delay(1000000); */
    /* Lcd_Cls(0); */
    /* Lcd_Show_Wait_Text(); */
    /* Delay(1000000); */
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
		switch(current_item)
		{
		case SetVoltage:
		{
			current_item = SetFreq;
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*(R_trim_num+dR_trim_num)/64), 1, 0);
			
			/* Lcd_Set_Waveform(current_waveform, 0); */

			/* Lcd_Set_Item_Int(SetDuty, duty_square, 0); */
			
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
			current_item = SetVoltage;
			Lcd_Set_Item_Int(SetDuty, duty_square, 0);
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*(R_trim_num+dR_trim_num)/64), 1, 1);
			//Lcd_Set_Waveform(current_waveform, 1);
			break;
		}
        default: break;
		}

		message = None;			/* 这条语句不能放在整个函数的最后，否则会使消息还未处理就被清除 */
		break;
	}
	case AddSelect:
	{
		switch(current_item)
		{
		case SetVoltage:
		{
			/* if(current_waveform == SQUARE) */
			/* { */
			/* 	current_waveform = TRIANGULAR; */
			/* }else */
			/* { */
			/* 	current_waveform = SQUARE; */
			/* } */
			/* Lcd_Set_Waveform(current_waveform, 1); */
			if (R_trim_num+dR_trim_num<=1)
			{
				dR_trim_num = 64-R_trim_num;
			}else
			{
				dR_trim_num--;
			}
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*(R_trim_num+dR_trim_num)/64), 1, 1);
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

			/* if(current_waveform == TRIANGULAR) */
			/* {				 */
			/* 	duty_triangular = (float)(75e-6*2*freq); */
			/* 	Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0); */
			/* } */
			
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
		case SetVoltage:
		{
			/* if(current_waveform == SQUARE) */
			/* { */
			/* 	current_waveform = TRIANGULAR; */
			/* }else */
			/* { */
			/* 	current_waveform = SQUARE; */
			/* } */
			/* Lcd_Set_Waveform(current_waveform, 1); */
			if (R_trim_num+dR_trim_num>=64)
			{
				dR_trim_num = -R_trim_num+1;
			}else
			{
				dR_trim_num++;
			}
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*(R_trim_num+dR_trim_num)/64), 1, 1);
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

			/* if(current_waveform == TRIANGULAR) */
			/* {				 */
			/* 	duty_triangular = (float)(75e-6*2*freq); */
			/* 	Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0); */
			/* } */
			
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
		case SetVoltage:
		{
			/* Lcd_Set_Waveform(current_waveform, 0); */

			/* if(current_waveform == SQUARE) */
			/* { */
			/* 	Lcd_Set_Item_Int(SetDuty, duty_square, 0); */
			/* }else */
			/* { */
			/* 	duty_triangular = (float)(75e-6*2*freq); */
			/* 	Lcd_Set_Item_Float(SetDuty, duty_triangular*100, 3, 0); */
			/* } */
			R_trim_num += dR_trim_num;
			dR_trim_num = 0;
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*R_trim_num/64), 1, 0);
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

		
		Pwm_Set_Value(freq, duty_square/100.0);
		Pwm_Enable();
		Led_STATUS(Red);
		
		message = None;
		break;
	}
	case TransmitOff:
	{
		switch(current_item)
		{
		case SetVoltage:
		{
			//Lcd_Set_Waveform(current_waveform, 1);
			Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*R_trim_num/64), 1, 1);
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
		/* Lcd_Set_Waveform(current_waveform, 1); */
		Lcd_Set_Item_Float(SetVoltage, Rtrim2V(R_trim_max*R_trim_num/64), 1, 1);
		/* Lcd_Set_Item_Int(SetVoltage, 14, 1); */
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

/* 通过调压电阻得到电压 */
inline float Rtrim2V(float R_trim)
{
	return 11900*40/(R_trim+10912)-0.0543*40;
}
