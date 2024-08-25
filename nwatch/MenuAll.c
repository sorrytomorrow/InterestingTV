#include "MenuAll.h"
#include "typedefs.h"
#include "Image.h"



#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include "driver_ir_receiver.h"



u8g2_t u8g2;
/*TimeData Set*/
struct Time_Data TimeClock={22,0,0};
/*index*/
static int16_t index[][6] = {{8,17},{52,17},{96,17},{-36,17},{140,17}}; //选框的坐标 
struct Image_Data AllImage_Data[]={{Clear_Big,""},{Light,"Light"},{Music,"Music"},
									{Game,"Game"},{Set,"SetUp"},{Exit,"Exit"},{Clear_Big,""}};

static QueueHandle_t g_xQueueMenu_IR;
uint8_t Ment = None;
uint8_t i_pageAll=1;

/*Start Menu*/
void Start_Menu(void)
{
	
	char Day_Str[]="Friday";
	char Month_Str[]="2024/10/29";
	char buffer[11];
	BaseType_t preTime;
	u8g2Init(&u8g2);
	
	while(1)
	{
		
		
		u8g2_ClearBuffer(&u8g2);
		/*Draw iamge*/
		u8g2_DrawXBMP(&u8g2,7,0,14,14,Mini_Watch);
		u8g2_DrawXBMP(&u8g2,27,0,14,14,Mini_Timer);
		u8g2_DrawXBMP(&u8g2,47,0,14,14,Mini_Music);
		u8g2_DrawXBMP(&u8g2,67,0,14,14,Mini_Game);
		u8g2_DrawXBMP(&u8g2,87,0,14,14,Mini_Set);
		u8g2_DrawXBMP(&u8g2,107,0,14,14,Mini_Exit);
		
		/*Draw Data*/
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(&u8g2,50,52,Day_Str);
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(&u8g2,40,63,Month_Str);
		
		if(TimeClock.Second==60)
		{
			TimeClock.Second=0;
			TimeClock.Minute++;
		}
			
		if(TimeClock.Minute==60)
		{
			TimeClock.Minute=0;
			TimeClock.Hour++;
		}
		if(TimeClock.Hour==24)
			TimeClock.Hour=0;
		/*show time*/
		u8g2_SetFont(&u8g2, u8g2_font_mystery_quest_24_tf );       //设定字体
		sprintf(buffer,"%02d:%02d:%02d",TimeClock.Hour,TimeClock.Minute,TimeClock.Second);  //传递变量（十进制（占四格））
		u8g2_DrawStr(&u8g2,22,40,buffer); //显示
		
		u8g2_SendBuffer(&u8g2);
		TimeClock.Second++;
		vTaskDelay(1000);
	}
	
	
}




/*Second Menu*/
/*Menu*/
void UI_Arrow(void* params)
{
	static uint8_t i=1;
	static bool Flag= true;
	/*下方*/
	if(Flag)
	{
		u8g2_DrawXBMP(&u8g2,0+i*10,0,10,9,Right_Arrow);
		u8g2_DrawXBMP(&u8g2,118-i*10,0,10,9,Left_Arrow);
		
		i++;
		if(i==3)
		{
			i=2;
			Flag=false;
		}
	}
	else
	{
		u8g2_DrawXBMP(&u8g2,0+i*10,0,10,9,Clear_Arrow);
		u8g2_DrawXBMP(&u8g2,118-i*10,0,10,9,Clear_Arrow);
		i--;
		if(i==0)
		{
			i=1;
			Flag=true;
		}
	}
	/*图标名字*/
	u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
	u8g2_DrawStr(&u8g2,52,60,params);
	
	
	
}



void Second_Menu(void)
{
	/*Init*/
	u8g2Init(&u8g2);
	TypedefDataIR DataIR;
	uint8_t i_page=1,j=0;
	uint8_t width;       //得到下方字体的宽度动态变化方向键
	bool Flag_R=true,Flag_L=true;
	u8g2_ClearBuffer(&u8g2);
	
	//下方字体箭头包围(先写上第一张图的名字,不然得不到width)
	u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
	u8g2_DrawStr(&u8g2,52,60,AllImage_Data[i_page].Name);
	
	width = u8g2_GetStrWidth(&u8g2,AllImage_Data[i_page].Name);
	u8g2_DrawXBMP(&u8g2,35,52,16,8,Select_Left);   //下方
	u8g2_DrawXBMP(&u8g2,52+width+1,52,16,8,Select_Right);
	
	while(1)
	{

		/*Draw Picture*/
		
		u8g2_DrawXBMP(&u8g2,index[0][0],index[0][1],25,30,AllImage_Data[i_page-1].Image);
		u8g2_DrawXBMP(&u8g2,index[1][0],index[1][1],25,30,AllImage_Data[i_page].Image);
		u8g2_DrawXBMP(&u8g2,index[2][0],index[2][1],25,30,AllImage_Data[i_page+1].Image);
		
		
		/*Draw Arrow*/
		u8g2_DrawXBMP(&u8g2,0,0,10,9,Right_Arrow);
		u8g2_DrawXBMP(&u8g2,118,0,10,9,Left_Arrow);
		
		
		
		//Draw Device_Name
		u8g2_SetFont(&u8g2,u8g2_font_tenthinguys_t_all);
		u8g2_DrawStr(&u8g2,50,9,DeviceName);
		u8g2_SetFont(&u8g2,u8g2_font_ncenB08_tf);
		
		/*Right*/
		if(Ment==Right)
		{
			
			u8g2_DrawXBMP(&u8g2,35,51,80,16,Clear_Word);
			while(j<44&&i_page<5)
			{
				u8g2_DrawXBMP(&u8g2,0,16,128,32,Clear_Big);
				
				
				j+=4;
				u8g2_DrawXBMP(&u8g2,index[0][0]-j,index[0][1],25,30,AllImage_Data[i_page-1].Image);
				u8g2_DrawXBMP(&u8g2,index[1][0]-j,index[1][1],25,30,AllImage_Data[i_page].Image);
				u8g2_DrawXBMP(&u8g2,index[2][0]-j,index[2][1],25,30,AllImage_Data[i_page+1].Image);
				u8g2_DrawXBMP(&u8g2,index[4][0]-j,index[4][1],25,30,AllImage_Data[i_page+2].Image);
				u8g2_SendBuffer(&u8g2);
				
				
			}
			i_page++;j=0;
			if(i_page>=5)
				i_page=5;
			width = u8g2_GetStrWidth(&u8g2,AllImage_Data[i_page].Name);
			
			Ment=None;
			
		}
		/*Left*/
		else if(Ment==Left)
		{
			
			u8g2_DrawXBMP(&u8g2,35,51,80,16,Clear_Word);
			while(j<44&&i_page>1)
			{

				u8g2_DrawXBMP(&u8g2,0,16,128,32,Clear_Big);
			
				j+=4;
				u8g2_DrawXBMP(&u8g2,index[3][0]+j,index[3][1],25,30,AllImage_Data[i_page-2].Image);
				u8g2_DrawXBMP(&u8g2,index[0][0]+j,index[0][1],25,30,AllImage_Data[i_page-1].Image);
				u8g2_DrawXBMP(&u8g2,index[1][0]+j,index[1][1],25,30,AllImage_Data[i_page].Image);
				u8g2_DrawXBMP(&u8g2,index[2][0]+j,index[2][1],25,30,AllImage_Data[i_page+1].Image);
				u8g2_SendBuffer(&u8g2);
				
				
			}
			i_page--;j=0;
			
			if(i_page<=1)
				i_page=1;
			width = u8g2_GetStrWidth(&u8g2,AllImage_Data[i_page].Name);
			Ment=None;
		}
	
		
		i_pageAll=i_page;
		UI_Arrow(AllImage_Data[i_page].Name);
		u8g2_DrawXBMP(&u8g2,35,52,16,8,Select_Left);   //下方
		u8g2_DrawXBMP(&u8g2,52+width+1,52,16,8,Select_Right);
		
		u8g2_SendBuffer(&u8g2);
		vTaskDelay(150);
	}
}





/*Third Menu*/
void Third_Menu(void* params)
{
	

}







