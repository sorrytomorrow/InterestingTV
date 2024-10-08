/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */
#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core


#include "draw.h"
#include "resources.h"

#include "driver_lcd.h"
#include "driver_rotary_encoder.h"
#include "driver_ir_receiver.h"
#include "driver_mpu6050.h"
#include "Data.h"

#define sprintf_P  sprintf
#define PSTR(a)  a

#define PLATFORM_WIDTH	12
#define PLATFORM_HEIGHT	4
#define UPT_MOVE_NONE	0
#define UPT_MOVE_RIGHT	1
#define UPT_MOVE_LEFT	2
#define BLOCK_COLS		32
#define BLOCK_ROWS		5
#define BLOCK_COUNT		(BLOCK_COLS * BLOCK_ROWS)

typedef struct{
	float x;
	float y;
	float velX;
	float velY;
}s_ball;

static const byte block[] ={
	0x07,0x07,0x07,
};

static const byte platform[] ={
	0x60,0x70,0x50,0x10,0x30,0xF0,0xF0,0x30,0x10,0x50,0x70,0x60,
};

static const byte ballImg[] ={
	0x03,0x03,
};

static const byte clearImg[] ={
	0,0,0,0,0,0,0,0,0,0,0,0,
};

static bool btnExit(void);
static bool btnRight(void);
static bool btnLeft(void);
void game1_draw(void);

static byte uptMove;
static s_ball ball;
static bool* blocks;
static byte lives, lives_origin;
static uint score;
static byte platformX;
static bool first = 1;
static bool ifDelete=false;
static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;
static QueueHandle_t g_xQueuePlatform,g_xQueuePlatform_RT,g_xQueuePlatform_IR; /*挡球板队列(队列的句柄)*/
static QueueHandle_t g_xQueuePlatform_MPU6;
static QueueSetHandle_t g_QueueLine;

/*任务句柄*/
static TaskHandle_t xgame1PlatHandel;
static TaskHandle_t xgame1WDataHandel;
static TaskHandle_t xgame1AllInputHandel;
//外部的任务句柄
extern TaskHandle_t xgame1_TaskHandle;

void Rel_resources1(void);
/* 挡球板任务 */
static void platform_task(void *params)
{
    byte platformXtmp = platformX;    
    uint8_t dev, data, last_data;
	TypedefDataOT Data_Input;
	uint8_t Statue=0;
    // Draw platform
    draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
    
    while (1)
    {
		
        /* 读取platform中的数据 */
		xQueueReceive(g_xQueuePlatform,&Data_Input,portMAX_DELAY);
		
		uptMove=Data_Input.data;   
		// Hide platform
		draw_bitmap(platformXtmp, g_yres - 8, clearImg, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
		
		// Move platform
		if(uptMove == UPT_MOVE_RIGHT)
			platformXtmp += 3;
		else if(uptMove == UPT_MOVE_LEFT)
			platformXtmp -= 3;
		uptMove = UPT_MOVE_NONE;
		
		// Make sure platform stays on screen
		if(platformXtmp > 250)
			platformXtmp = 0;
		else if(platformXtmp > g_xres - PLATFORM_WIDTH)
			platformXtmp = g_xres - PLATFORM_WIDTH;
		
		// Draw platform
		draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
		draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
		
		platformX = platformXtmp;
            
		
    }
}

/*红外遥控写入挡球板队列*/
static void InputDataIR(void)
{
	static int Last_data;
	TypedefDataIR idata;
	TypedefDataOT odata;
	//xSemaphoreTake(g_xIRMutex,portMAX_DELAY);
	xQueueReceive(g_xQueuePlatform_IR,&idata,0);
	//xSemaphoreGive(g_xIRMutex);
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
	else if(idata.data==0xc2)
	{
		xTaskNotifyGive(xgame1_TaskHandle);
		vTaskDelay(50);
		vTaskDelete(xgame1PlatHandel);
		vQueueDelete(g_xQueuePlatform);
		vTaskDelete(NULL);
	}
	else
	{
		odata.data=0;
	}
	Last_data = odata.data;
	xQueueSend(g_xQueuePlatform,&odata,0);
	
}
/*旋转编码器写入数据*//*如果速度越快的话写入到Platform队列的数据就越多*/
static void InputDataRot(void)
{
	uint8_t cnt,i;
	uint8_t Right1;
	TypedefDataRT idata;
	TypedefDataOT odata;
	xQueueReceive(g_xQueuePlatform_RT,&idata,0);
	if(idata.Speed>0)
	{
		Right1=1;
	}
	else
	{
		Right1=0;
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
	
	odata.data = Right1?UPT_MOVE_LEFT:UPT_MOVE_RIGHT;
	for(i=0;i<cnt;i++)
	{
		xQueueSend(g_xQueuePlatform,&odata,0);  //
	}
	
}

/*MPU6050陀螺仪写入数据*//*倾斜角越大写入到挡球板队列的数据就越多*/
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





/*解析多个设备给队列集写入的数据*/
static void InputDataInPlatform(void *params)
{
	QueueHandle_t g_TempQueueLine;
	while(1)
	{
		
		g_TempQueueLine=xQueueSelectFromSet(g_QueueLine,portMAX_DELAY);
		if(g_TempQueueLine)
		{
			if(g_TempQueueLine==g_xQueuePlatform_IR)
			{
				InputDataIR();
			}
			else if(g_TempQueueLine==g_xQueuePlatform_RT)
			{
				InputDataRot();
			}
			else if(g_TempQueueLine==g_xQueuePlatform_MPU6)
			{
				InputDataMPU6();
			}
		
		}
	
	}

}






/*整体游戏*/
void game1_task(void *params)
{		
    uint8_t dev, data, last_data;
	
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
    RotaryEncoder_Init();
	first=1; 						//归一
	
	/*创建队列,从红外传感器中读取数据*//*消费者*/
	g_xQueuePlatform = xQueueCreate(15,sizeof(TypedefDataOT));
	
	g_xQueuePlatform_IR = xQueueCreate(15,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueuePlatform_IR);
	g_xQueuePlatform_RT = Get_QueueDriRotenHandle();
	//g_xQueuePlatform_MPU6 = Get_QueueMpu6Handle();
	
	/*创建队列集*//*把任务加入到队列集里面去*/
	g_QueueLine=xQueueCreateSet(30);
	xQueueAddToSet(g_xQueuePlatform_IR,g_QueueLine);
	xQueueAddToSet(g_xQueuePlatform_RT,g_QueueLine);
	//xQueueAddToSet(g_xQueuePlatform_MPU6,g_QueueLine);
	
	uptMove = UPT_MOVE_NONE;

	ball.x = g_xres / 2;
	ball.y = g_yres - 10;
        
	ball.velX = -0.5;
	ball.velY = -0.6;
//	ball.velX = -1;
//	ball.velY = -1.1;

	blocks = pvPortMalloc(BLOCK_COUNT);
    memset(blocks, 0, BLOCK_COUNT);
	
	lives = lives_origin = 3;
	score = 0;
	platformX = (g_xres / 2) - (PLATFORM_WIDTH / 2);
	xSemaphoreTake(g_xIRMutex,0);     //上锁
	//xTaskCreate(WDataTQueFromMpu,"WDataTQueFromMpuTask",128,NULL,osPriorityNormal+1,&xgame1WDataHandel);/*MPU6050写入数据*/
	xTaskCreate(InputDataInPlatform, "InputTask", 60, NULL, osPriorityNormal+1,&xgame1AllInputHandel);
    xTaskCreate(platform_task, "platform_task", 80, NULL, osPriorityNormal+1, &xgame1PlatHandel);

    while (1)
    {
		if(ulTaskNotifyTake(pdTRUE,0))
		{
			//释放资源
			draw_bitmap(0,0, clearALL, 128,64, NOINVERT, 0);
			draw_flushArea(0,0,128,64);
			Rel_resources1();
			testDrawProcess(&u8g2);
			xTaskNotifyGive(xTask_ControlHandle);    //通知任务控制任务
			xSemaphoreGive(g_xIRMutex);//释放锁
			vTaskDelete(NULL);
		}
		
        game1_draw();
		btnExit();
        //draw_end();
        vTaskDelay(50);
    }
}

static bool btnExit()
{
	
	
//	if(lives == 255)
//	{
//		//game1_start();
//	}
	if(lives == 255)
	{
		vTaskDelay(2000);
		//释放资源
		draw_bitmap(0,0, clearALL, 128,64, NOINVERT, 0);
		draw_flushArea(0,0,128,64);
		Rel_resources1();
		testDrawProcess(&u8g2);
		xTaskNotifyGive(xTask_ControlHandle);    //通知任务控制任务
		xSemaphoreGive(g_xIRMutex);//释放锁
		vTaskDelete(xgame1AllInputHandel);
		vTaskDelete(xgame1PlatHandel);
		vTaskDelete(NULL);
	}
	return true;
}

static bool btnRight()
{
	uptMove = UPT_MOVE_RIGHT;
	return false;
}

static bool btnLeft()
{
	uptMove = UPT_MOVE_LEFT;
	return false;
}

void game1_draw(void)
{
	bool gameEnded = ((score >= BLOCK_COUNT) || (lives == 255));

	byte platformXtmp = platformX;
	
    
	byte x,y,i;
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

	bool blockCollide = false;
	const float ballX = ball.x;
	const byte ballY = ball.y;

	// Block collision
	byte idx = 0;
	LOOP(BLOCK_COLS, x)
	{
		LOOP(BLOCK_ROWS, y)
		{
			if(!blocks[idx] && ballX >= x * 4 && ballX < (x * 4) + 4 && ballY >= (y * 4) + 8 && ballY < (y * 4) + 8 + 4)
			{
//				buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
				// led_flash(LED_GREEN, 50, 255); // 100ask todo
				blocks[idx] = true;

                // hide block
                draw_bitmap(x * 4, (y * 4) + 8, clearImg, 3, 8, NOINVERT, 0);                
                draw_flushArea(x * 4, (y * 4) + 8, 3, 8);                
				blockCollide = true;
				score++;
			}
			idx++;
		}
	}


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
	bool platformCollision = false;
	if(!gameEnded && ballY >= g_yres - PLATFORM_HEIGHT - 2 && ballY < 240 && ballX >= platformX && ballX <= platformX + PLATFORM_WIDTH)
	{
		platformCollision = true;
		// buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
		ball.y = g_yres - PLATFORM_HEIGHT - 2;
		if(ball.velY > 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
	}

	// Top/bottom wall collision
	if(!gameEnded && !platformCollision && (ballY > g_yres - 2 || blockCollide))
	{
		if(ballY > 240)
		{
			// buzzer_buzz(200, TONE_2_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = 0;
		}
		else if(!blockCollide)
		{
			// buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = g_yres - 1;
			lives--;
		}
		ball.velY *= -1;
	}

	// Draw ball
	draw_bitmap(ball.x, ball.y, ballImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    //draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    //draw_flushArea(platformX, g_yres - 8, 12, 8);

    if (first)
    {
        first = 0;
        
    	// Draw blocks
    	idx = 0;
    	LOOP(BLOCK_COLS, x)
    	{
    		LOOP(BLOCK_ROWS, y)
    		{
    			if(!blocks[idx])
    			{
    				draw_bitmap(x * 4, (y * 4) + 8, block, 3, 8, NOINVERT, 0);
                    draw_flushArea(x * 4, (y * 4) + 8, 3, 8);                
    			}
    			idx++;
    		}
    	}
        
    }

	// Draw score
	char buff[6];
	sprintf_P(buff, PSTR("%u"), score);
	draw_string(buff, false, 0, 0);

    // Draw lives
    if(lives != 255)
    {
        LOOP(lives_origin, i)
        {
            if (i < lives)
                draw_bitmap((g_xres - (3*8)) + (8*i), 1, livesImg, 7, 8, NOINVERT, 0);
            else
                draw_bitmap((g_xres - (3*8)) + (8*i), 1, clearImg, 7, 8, NOINVERT, 0);
            draw_flushArea((g_xres - (3*8)) + (8*i), 1, 7, 8);    
        }
    }   

	// Got all blocks
	if(score >= BLOCK_COUNT)
		draw_string_P(PSTR(STR_WIN), false, 50, 32);

	// No lives left (255 because overflow)
	if(lives == 255)
		draw_string_P(PSTR(STR_GAMEOVER), false, 34, 32);

}

/*Release resources*/
void Rel_resources1(void)
{
	Clear_RegisterQueueHandle();
	vPortFree(blocks);
	blocks=NULL;                             
	vQueueDelete(g_xQueuePlatform_RT);		  
	vQueueDelete(g_xQueuePlatform_IR);
	vQueueDelete(g_QueueLine);
	
}

