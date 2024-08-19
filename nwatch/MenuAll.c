#include "MenuAll.h"
#include "typedefs.h"
#include "Image.h"
#include "fun_u8g2.h"
#include "stm32_u8g2.h"


#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

struct Time_Data{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
};

/*TimeData Set*/
struct Time_Data TimeClock={22,00,00};

/*Print Format*/
void PrintVarFormat(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const uint8_t *font, char var)
{
    char var_buff[1];             //用来存ASCII码
    u8g2_SetFont(u8g2, font);       //设定字体
    sprintf(var_buff, "%16d", var);  //传递变量（十进制（占四格））
    u8g2_DrawStr(u8g2, x, y, var_buff); //显示
}


/*Start Menu*/
void Start_Menu(void)
{
	u8g2_t u8g2;
	char Day_Str[]="Friday";
	char Month_Str[]="2024/10/29";
	
		
	u8g2Init(&u8g2);
	u8g2_ClearBuffer(&u8g2);
	/*绘制上方*/
	u8g2_DrawXBMP(&u8g2,7,0,14,14,Mini_Watch);
	u8g2_DrawXBMP(&u8g2,27,0,14,14,Mini_Timer);
	u8g2_DrawXBMP(&u8g2,47,0,14,14,Mini_Music);
	u8g2_DrawXBMP(&u8g2,67,0,14,14,Mini_Game);
	u8g2_DrawXBMP(&u8g2,87,0,14,14,Mini_Set);
	u8g2_DrawXBMP(&u8g2,107,0,14,14,Mini_Exit);
	
	/*绘制下方日期*/
	u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
	u8g2_DrawStr(&u8g2,50,52,Day_Str);
	u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
	u8g2_DrawStr(&u8g2,40,63,Month_Str);
	
	while(1)
	{
		if(TimeClock.Second==60)
			TimeClock.Minute++;
		if(TimeClock.Minute==60)
			TimeClock.Hour++;
		PrintVarFormat(&u8g2,2,17,u8g2_font_36x32_tf);     //绘制Hour 17到49
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(&u8g2,,,":");
		PrintVarFormat(&u8g2,);     //绘制Minute
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(&u8g2,,,":");
		PrintVarFormat(&u8g2,);     //绘制Second  36x32
		
		u8g2_SendBuffer(&u8g2);
		TimeClock.Second++;
		VTaskDelay(1000);
	}
	
	
}