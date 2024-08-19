#include "u8g2.h"


#define SEND_BUFFER_DISPLAY_MS(u8g2, ms)\
  do {\
    u8g2_SendBuffer(u8g2); \
    HAL_Delay(ms);\
  }while(0);

  
//进度条显示
void testDrawProcess(u8g2_t *u8g2)
{
	for(int i=10;i<=80;i=i+2)
	{
		u8g2_ClearBuffer(u8g2);    //清空缓冲区
			
		char buff[20];
		sprintf(buff,"%d%%",(int)(i/80.0*100));
		
		u8g2_SetFont(u8g2,u8g2_font_ncenB12_tf);
		u8g2_DrawStr(u8g2,16,32,"STM32 U8g2");//字符显示
		
		u8g2_SetFont(u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(u8g2,100,49,buff);//当前进度显示
		
		u8g2_DrawRBox(u8g2,16,40,i,10,4);//圆角填充框矩形框
		u8g2_DrawRFrame(u8g2,16,40,80,10,4);//圆角矩形
		
		u8g2_SendBuffer(u8g2);    //从缓冲区中发送数据
	}
	HAL_Delay(500);
}


void Desktop2_VCU(u8g2_t *u8g2)
{
    u8g2_SetDrawColor(u8g2,1);		
	u8g2_ClearBuffer(u8g2);  
    u8g2_SetFont(u8g2, u8g2_font_8x13B_tr);
	
    u8g2_DrawStr(u8g2,0,16,"MaximumSpeed:");		
    u8g2_DrawStr(u8g2,0,32,"MinimumSpeed:");	
    u8g2_SendBuffer(u8g2);

}


void draw_gear(u8g2_t *u8g2, int x, int y, int size, int tooth_width) 
{
		int i;
		u8g2_ClearBuffer(u8g2);
		for (i = 0; i < 8; i++) 
		{
			u8g2_DrawBox(u8g2, x - size, y - tooth_width, size * 2, tooth_width * 2);
			u8g2_DrawCircle(u8g2, x, y, size, U8G2_DRAW_ALL);
			x += size / 2;
		}
		u8g2_SendBuffer(u8g2);
}
