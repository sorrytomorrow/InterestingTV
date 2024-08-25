#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core


#include "draw.h"
#include "resources.h"
#include "typedefs.h"

#include "driver_lcd.h"
#include "driver_rotary_encoder.h"
#include "driver_ir_receiver.h"

/*球的图形*/
static const byte ballImg[] ={
	0x03,0x03,
};
/*挡球板的图形*/
static const byte platform_1[] ={
	0x60,0x70,0x50,0x10,0x30,0xF0,0xF0,0x30,0x10,0x50,0x70,0x60,
};
static const byte platform_2[] ={
	0x06,0x0E,0x0A,0x08,0x0C,0x0F,0x0F,0x0C,0x08,0x0A,0x0E,0x06,
	
	
};
/*清除图像*/
static const byte clearImg[] ={
	0,0,0,0,0,0,0,0,0,0,0,0,
};

/*变量定义*/
static uint8_t *g_framebuffer;
static uint32_t g_xres, g_yres, g_bpp;
static QueueHandle_t g_xQueuePlatform1,g_xQueuePlatform2;
static QueueHandle_t g_xQueuePlatform2_IR;
static QueueHandle_t g_xQueuePlatform_RT;
static QueueSetHandle_t  g_xQueueLists;
static byte uptMove1,uptMove2;
static byte lives_0,lives_1, lives_origin; /*生命变量*/
void game2_draw();
typedef struct platform{
	byte x;
	byte y;
	
}Typedefplatform;

typedef struct{
	float x;
	float y;
	float velX;
	float velY;
}s_ball;

Typedefplatform platform1;
Typedefplatform platform2;
static s_ball ball;



/*宏定义*/
#define NOINVERT	false
#define INVERT		true
#define begin_lives 5
static bool btnExit();


/*挡球板1任务*/
void platform1_task(void* params)
{
	Typedefplatform* Templatform = params;
	TypedefDataOT idata;
	byte platformXtmp = Templatform->x;
	draw_bitmap(platformXtmp,Templatform->y, platform_1,12,8, NOINVERT, 0); /*宽和高*/
	draw_flushArea(platformXtmp,Templatform->y,12,8);
	
	while(1)
	{
		xQueueReceive(g_xQueuePlatform1,&idata,portMAX_DELAY);
		uptMove1=idata.data;   
		// Hide platform
		draw_bitmap(platformXtmp,Templatform->y, clearImg, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp,Templatform->y, 12, 8);
									
		// Move platform
		if(uptMove1 == UPT_MOVE_RIGHT)
			platformXtmp += 3;
		else if(uptMove1 == UPT_MOVE_LEFT)
			platformXtmp -= 3;
		uptMove1 = UPT_MOVE_NONE;					
		// Make sure platform stays on screen
		if(platformXtmp>250)
			platformXtmp = 0;
		else if(platformXtmp > g_xres - 12)
			platformXtmp = g_xres - 12;
		
		// Draw platform
		draw_bitmap(platformXtmp,Templatform->y, platform_1, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp,Templatform->y, 12, 8);
		Templatform->x = platformXtmp;
		
	}


}
/*挡球板2任务*/
void platform2_task(void* params)
{
	Typedefplatform* Templatform = params;
	TypedefDataOT idata;
	byte platformXtmp = Templatform->x;
	draw_bitmap(platformXtmp,Templatform->y, platform_2,12,8, NOINVERT, 0); /*宽和高*/
	draw_flushArea(platformXtmp,Templatform->y,12,8);
	
	while(1)
	{
		xQueueReceive(g_xQueuePlatform2,&idata,portMAX_DELAY);
		uptMove1=idata.data;   
		// Hide platform
		draw_bitmap(platformXtmp,Templatform->y, clearImg, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp,Templatform->y, 12, 8);
									
		// Move platform
		if(uptMove1 == UPT_MOVE_RIGHT)
			platformXtmp += 3;
		else if(uptMove1 == UPT_MOVE_LEFT)
			platformXtmp -= 3;
		uptMove1 = UPT_MOVE_NONE;					
		// Make sure platform stays on screen
		if(platformXtmp>250)
			platformXtmp = 0;
		else if(platformXtmp > g_xres - 12)
			platformXtmp = g_xres - 12;
		
		// Draw platform
		draw_bitmap(platformXtmp,Templatform->y, platform_2, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp,Templatform->y, 12, 8);
		Templatform->x = platformXtmp;
		
	}


}


/*红外遥控写入挡球板队列*/
static void InputDataIR(void)
{
	static int Last_data;
	TypedefDataIR idata;
	TypedefDataOT odata;
	xQueueReceive(g_xQueuePlatform2_IR,&idata,0);
	/*判断向左*/
	if(idata.data==0xe0)
	{
		odata.data = UPT_MOVE_LEFT;
		
	}
	else if(idata.data==0x90)
	{
		odata.data = UPT_MOVE_RIGHT;
	}
	else if(idata.data==0x00)
	{
		odata.data = Last_data;
	}
	else
	{
		odata.data=0;
	}
	Last_data = odata.data;
	xQueueSend(g_xQueuePlatform1,&odata,0);
	
}
/*旋转编码器写入数据*//*如果速度越快的话写入到Platform队列的数据就越多*/
static void InputDataRot(void)
{
	uint8_t cnt,i;
	uint8_t Right;
	TypedefDataRT idata;
	TypedefDataOT odata;
	xQueueReceive(g_xQueuePlatform_RT,&idata,0);
	if(idata.Speed>0)
	{
		Right=1;
	}
	else
	{
		Right=0;
		idata.Speed = 0-idata.Speed;
	}
	if(idata.Speed>120)
	{
		cnt=4;
	}
	else if(idata.Speed>60)
	{
		cnt=2;
	}
	else{cnt=1;}
	
	odata.data = Right?UPT_MOVE_LEFT:UPT_MOVE_RIGHT;
	for(i=0;i<cnt;i++)
	{
		xQueueSend(g_xQueuePlatform2,&odata,0);  //
	}
	
}

/*MPU6050陀螺仪写入数据*//*倾斜角越大写入到挡球板队列的数据就越多*/
#if 0
static void InputDataMPU6(void)
{
	uint8_t cnt=0,i;
	TypedefMpu6D idata;
	TypedefDataOT odata;
	xQueueReceive(g_xQueuePlatform_MPU6,&idata,0);
	if(idata.Ax_val>90)
	{
		odata.data=UPT_MOVE_LEFT;
		if(idata.Ax_val>=92)
			cnt=2;
		
	}
	else
	{
		odata.data=UPT_MOVE_RIGHT;
		if(idata.Ax_val<=88)
			cnt=2;
		
	}
	for(i=0;i<cnt;i++)
	{
		xQueueSend(g_xQueuePlatform,&odata,0);     //写入挡球板队列
	}
		
	
}
#endif


/*解析多个设备给队列集写入的数据*/
static void InputDataInPlatform(void *params)
{
	QueueHandle_t g_TempQueueLine;
	while(1)
	{
		g_TempQueueLine=xQueueSelectFromSet(g_xQueueLists,portMAX_DELAY);
		if(g_TempQueueLine)
		{
			if(g_TempQueueLine==g_xQueuePlatform2_IR)
			{
				InputDataIR();
			}
			else if(g_TempQueueLine==g_xQueuePlatform_RT)
			{
				InputDataRot();
			}
//			else if(g_TempQueueLine==g_xQueuePlatform_MPU6)
//			{
//				InputDataMPU6();
//			}
//		
		}
	
	}

}


/*两个挡球板游戏*/
void game2_task(void* params)
{
	/*初始化*/
    draw_init();
    draw_end();
	/*挡球板初始化赋值坐标*/
	g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
	platform1.x = g_xres/2-6;
	platform1.y = g_yres-16;

	platform2.x = g_xres/2-6;
	platform2.y = 8;

	/*传递队列*/
	g_xQueuePlatform2_IR = xQueueCreate(15,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueuePlatform2_IR);
	
	g_xQueuePlatform_RT = Get_QueueDriRotenHandle();
	/*创建挡球板队列*/
	g_xQueuePlatform1 = xQueueCreate(15,sizeof(TypedefDataOT));
	g_xQueuePlatform2 = xQueueCreate(15,sizeof(TypedefDataOT));
	
	/*创建队列集*/
	g_xQueueLists = xQueueCreateSet(45);
	xQueueAddToSet(g_xQueuePlatform2_IR,g_xQueueLists);
	xQueueAddToSet(g_xQueuePlatform_RT,g_xQueueLists);
	
	
	/*创建任务*/
	xTaskCreate(InputDataInPlatform,"InputDatatask",128,NULL,osPriorityNormal,NULL);
	xTaskCreate(platform1_task,"platform_task1",128,&platform1,osPriorityNormal,NULL);
	xTaskCreate(platform2_task,"platform_task2",128,&platform2,osPriorityNormal,NULL);
	
	/*初始化球*/
	ball.x = g_xres / 2;
	ball.y = g_yres - 18;   
	ball.velX = -0.5;
	ball.velY = -0.6;
	
	lives_0 = lives_origin = 5;    //begin live
	lives_1 = 5;				   //begin live	
	while(1)
	{
		game2_draw();
		btnExit();
		vTaskDelay(50);
	}
		
}





void game2_draw()
{
	bool gameEnded = (lives_0 == 255 || lives_1 == 255);

	byte platformXtmp_1 = platform1.x;
	byte platformXtmp_2 = platform2.x;

    static bool first = 1;

	// Move ball
	// hide ball
	draw_bitmap(ball.x, ball.y, clearImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    //draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    //draw_flushArea(platformX, g_yres - 8, 12, 8);
	
	if(!gameEnded)
	{
		ball.x += ball.velX;
		ball.y += ball.velY;
	}

	bool blockCollide1 = false;
	bool blockCollide2 = false;
	const float ballX = ball.x;
	const byte ballY = ball.y;

	
	// Side wall collision
	if(ballX > g_xres - 2)
	{
		if(ballX > 240)
			ball.x = 0;		
		else
			ball.x = g_xres - 2;
		ball.velX = -ball.velX;		
	}
	if(ballX < 0)
    {
		ball.x = 0;		
		ball.velX = -ball.velX;	
    }

	// Platform collision
	bool platformCollision_0 = false;
	bool platformCollision_1 = false;
	
	//触碰到挡板1
	if(!gameEnded && ballY >= g_yres-13 && ballY < 240 && ballX >= platform1.x && ballX <= platform1.x + 12)
	{
		platformCollision_0 = true;
		// buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
		ball.y = g_yres - 14;
		if(ball.velY > 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
	}

	//触碰到挡板2
	if(!gameEnded && ballY >0 && ballY <= 11 && ballX >= platform2.x && ballX <= platform2.x + 12)
	{
		platformCollision_1 = true;
		// buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
		ball.y = 12;
		if(ball.velY < 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
	}
	
	//触碰到底部回弹
	if(!gameEnded && !platformCollision_0 && (ballY > g_yres-10 || blockCollide1))
	{
		
		if(!blockCollide1)
		{
			// buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = g_yres - 9;
			lives_1--;
		}
		ball.velY *= -1;
	}
	//触碰到顶部回弹
	if(!gameEnded && !platformCollision_1 && (ballY < 8 || blockCollide2))
	{
		
		if(!blockCollide2)
		{
			// buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = 9;
			lives_0--;
		}
		ball.velY *= -1;
	}

	// Draw ball
	draw_bitmap(ball.x, ball.y, ballImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    


    // Draw lives
    if(lives_1 != 255)
    {
        LOOP(lives_origin, i)
        {
            if (i < lives_1)
                draw_bitmap((g_xres - (begin_lives*8)) + (8*i), g_yres-8, livesImg, 7, 8, NOINVERT, 0);
            else
                draw_bitmap((g_xres - (begin_lives*8)) + (8*i), g_yres-8, clearImg, 7, 8, NOINVERT, 0);
            draw_flushArea((g_xres - (begin_lives*8)) + (8*i), g_yres-8, 7, 8);    
        }
    } 
	
	if(lives_0 != 255)
    {
        LOOP(lives_origin, i)
        {
            if (i < lives_0)
                draw_bitmap((g_xres - (begin_lives*8)) + (8*i),0 , livesImg, 7, 8, NOINVERT, 0);
            else
                draw_bitmap((g_xres - (begin_lives*8)) + (8*i), 0, clearImg, 7, 8, NOINVERT, 0);
            draw_flushArea((g_xres - (begin_lives*8)) + (8*i), 0, 7, 8);    
        }
    }  

	
	// No lives left (255 because overflow)
	if(lives_0 == 255)
	{
		draw_string_P(STR_GAMEOVER, false, 34, 20);
		draw_string_P("PLAY2LOSE!", false, 34, 40);
	}
	else if(lives_1 == 255)
	{
		draw_string_P(STR_GAMEOVER, false, 34, 20);
		draw_string_P("PLAY1LOSE!", false, 34, 40);
	}
		

}
/*keil Task*/
static bool btnExit()
{
	
	
//	if(lives == 255)
//	{
//		//game1_start();
//	}
	if(lives_1 == 255 || lives_0 == 255)
	{
		//pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_NONE);	
		//animation_start(display_load, ANIM_MOVE_OFF);
		
		vTaskDelete(NULL);
	}
	return true;
}


