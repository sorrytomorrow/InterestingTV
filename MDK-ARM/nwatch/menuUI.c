#include <stdlib.h>
#include <stdio.h>



#include "TaskFun.h"
#include "draw.h"
#include "resources.h"
#include "driver_lcd.h"
#include "driver_rotary_encoder.h"
#include "driver_ir_receiver.h"

/*宏*/
#define NOINVERT	false
#define INVERT		true
#define LEFT        0
#define RIGHT       1
#define RETURN      3
#define Enter       4
#define NONE        5
#define Max_page    2
#define Min_page    1
#define Max_index   1
#define Min_index   0


/*Variable Private*/
static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;
QueueHandle_t m_xQueueMenu_IR;
static TypedefDataIR DataInput;
static int8_t m_event=NONE;




void AnalDataFromIR(uint8_t Temp_DataInput)
{
	

	//static int8_t Last_data=NONE;
	/*判断向左*/
	if(Temp_DataInput==0xe0)
	{
		m_event = LEFT;
		
	}
	else if(Temp_DataInput==0x90)
	{
		m_event = RIGHT;
	}
	else if(Temp_DataInput==0xa8)
	{
		m_event = Enter;
	}
	
	else if(Temp_DataInput==0xc2)
	{
		m_event = RETURN;
	}
	else
	{
		m_event = NONE;
	}
	

}


void IR_Contorl_Menu_task(void* params)
{
	
	/*Init page&index*/
	
	draw_init();
    draw_end();
	g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
	
	
	TypedefDataIR iData;
	m_xQueueMenu_IR=Get_QueueIRHandle();
	struct MenuPageData Current_MenuPage;
	Current_MenuPage=Menu_Page0;
//	/*创建所有任务*/
//	CreatAllTask();
//	/*挂起所有任务*/
//	SuspendAllTask();
	
	
	while(1)
	{
		
		if(pdPASS==xQueueReceive(m_xQueueMenu_IR,&iData,portMAX_DELAY))
		{
			
			AnalDataFromIR(iData.data);   //得到事件
			if(m_event==Enter)
			{
				if(Current_MenuPage.page!=Max_page)  //页数为最大页时不删除
					DeleteOneTask(Current_MenuPage.page,Current_MenuPage.index);
				if(Current_MenuPage.page<Max_page)    //未达到最大页
				{
					Current_MenuPage.page+=1;
					CreatOneTask(Current_MenuPage.page,Current_MenuPage.index);
				}				
			}
			else if(m_event==RETURN)    //返回
			{
				if(Current_MenuPage.page!=Min_page)
					DeleteOneTask(Current_MenuPage.page,Current_MenuPage.index);
				
				if(Current_MenuPage.page>Min_page)
				{
					Current_MenuPage.page-=1;
					CreatOneTask(Current_MenuPage.page,Current_MenuPage.index);
				}
					
			}
			else if(m_event==RIGHT)   //向右
			{
				if(Current_MenuPage.index!=Max_index)  //1是index最大值
					DeleteOneTask(Current_MenuPage.page,Current_MenuPage.index);
				if(Current_MenuPage.index<1) //1是index最大值
				{
					Current_MenuPage.index+=1;
					CreatOneTask(Current_MenuPage.page,Current_MenuPage.index);
				}
					
			}
			
			else if(m_event==RIGHT)   //向左
			{
				if(Current_MenuPage.index!=Min_index)  //0是index最小值
					DeleteOneTask(Current_MenuPage.page,Current_MenuPage.index);
				if(Current_MenuPage.index>Min_index) //0是index最小值
				{
					Current_MenuPage.index-=1;
					CreatOneTask(Current_MenuPage.page,Current_MenuPage.index);
				}
					
			}
			
		}
		vTaskDelay(100);
	}
}


/*主界面任务*/
void menu_task_origin(void* params)
{
	
	/*Init*/
    
	struct MenuImageData* TempMenuPage = params;
	
	/*Draw menuorigin Screen*/
	draw_bitmap(TempMenuPage->x,TempMenuPage->y,TempMenuPage->p_ImageData , TempMenuPage->width,TempMenuPage->hight, NOINVERT, 0);
	draw_flushArea(TempMenuPage->x,TempMenuPage->y, TempMenuPage->width,TempMenuPage->hight);
	draw_bitmap(Menu_start.x,Menu_start.y, Menu_start.p_ImageData, Menu_start.width,Menu_start.hight, NOINVERT, 0);
	draw_flushArea(Menu_start.x,Menu_start.y,Menu_start.width,Menu_start.hight);
	while(1)
	{
		
		//绘制左边
		draw_bitmap(Menu_select_Box_left.x,Menu_select_Box_left.y,   //坐标
					Menu_select_Box_left.p_ImageData, Menu_select_Box_left.width,//图片宽高
					Menu_select_Box_left.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_left.x,Menu_select_Box_left.y,
						Menu_select_Box_left.width,Menu_select_Box_left.hight);
		//绘制右边
		draw_bitmap(Menu_select_Box_right.x,Menu_select_Box_right.y, 
					Menu_select_Box_right.p_ImageData, Menu_select_Box_right.width,
					Menu_select_Box_right.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_right.x,Menu_select_Box_right.y,
					Menu_select_Box_right.width,Menu_select_Box_right.hight);
		
		vTaskDelay(300);
		//清空左边
		draw_bitmap(Menu_select_Box_left.x,Menu_select_Box_left.y,select_clear, 
					Menu_select_Box_left.width,Menu_select_Box_left.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_left.x,Menu_select_Box_left.y,
						Menu_select_Box_left.width,Menu_select_Box_left.hight);
		
		//清空右边
		draw_bitmap(Menu_select_Box_right.x,Menu_select_Box_right.y,select_clear, 
					Menu_select_Box_right.width,Menu_select_Box_right.hight, NOINVERT, 0);
					
		draw_flushArea(Menu_select_Box_right.x,Menu_select_Box_right.y,
					Menu_select_Box_right.width,Menu_select_Box_right.hight);
					
		vTaskDelay(300);
	
	}
	

}




/*二级页面任务*/
void menu_task_second(void* params)
{
	
	/*Init*/
    
	struct MenuImageData* TempMenuPage = params;
	
	/*Draw menuorigin Screen*/
	draw_bitmap(TempMenuPage->x,TempMenuPage->y,TempMenuPage->p_ImageData , TempMenuPage->width,TempMenuPage->hight, NOINVERT, 0);
	draw_flushArea(TempMenuPage->x,TempMenuPage->y, TempMenuPage->width,TempMenuPage->hight);
	/*创建开始*/
	draw_bitmap(Menu_start.x,Menu_start.y, Menu_start.p_ImageData, Menu_start.width,Menu_start.hight, NOINVERT, 0);
	draw_flushArea(Menu_start.x,Menu_start.y,Menu_start.width,Menu_start.hight);
	
	draw_bitmap(Menu_right.x,Menu_right.y, Menu_right.p_ImageData, Menu_right.width,Menu_right.hight, NOINVERT, 0);
	draw_flushArea(Menu_right.x,Menu_right.y,Menu_right.width,Menu_right.hight);
	
	draw_bitmap(Menu_left.x,Menu_left.y, Menu_left.p_ImageData, Menu_left.width,Menu_left.hight, NOINVERT, 0);
	draw_flushArea(Menu_left.x,Menu_left.y,Menu_left.width,Menu_left.hight);
	
	while(1)
	{
		
		//绘制左边
		draw_bitmap(Menu_select_Box_left.x,Menu_select_Box_left.y,   //坐标
					Menu_select_Box_left.p_ImageData, Menu_select_Box_left.width,//图片宽高
					Menu_select_Box_left.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_left.x,Menu_select_Box_left.y,
						Menu_select_Box_left.width,Menu_select_Box_left.hight);
		//绘制右边
		draw_bitmap(Menu_select_Box_right.x,Menu_select_Box_right.y, 
					Menu_select_Box_right.p_ImageData, Menu_select_Box_right.width,
					Menu_select_Box_right.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_right.x,Menu_select_Box_right.y,
					Menu_select_Box_right.width,Menu_select_Box_right.hight);
		
		vTaskDelay(300);
		//清空左边
		draw_bitmap(Menu_select_Box_left.x,Menu_select_Box_left.y,select_clear, 
					Menu_select_Box_left.width,Menu_select_Box_left.hight, NOINVERT, 0);
		
		draw_flushArea(Menu_select_Box_left.x,Menu_select_Box_left.y,
						Menu_select_Box_left.width,Menu_select_Box_left.hight);
		
		//清空右边
		draw_bitmap(Menu_select_Box_right.x,Menu_select_Box_right.y,select_clear, 
					Menu_select_Box_right.width,Menu_select_Box_right.hight, NOINVERT, 0);
					
		draw_flushArea(Menu_select_Box_right.x,Menu_select_Box_right.y,
					Menu_select_Box_right.width,Menu_select_Box_right.hight);
					
		vTaskDelay(300);
	
	}
	

}