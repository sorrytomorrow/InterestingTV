/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

#include "data.h"
#include "Game_Snake.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* just variables*/
static BaseType_t ret;

/*TaskHandle*/
static TaskHandle_t xSoundTaskHandle=NULL;
TaskHandle_t xTask_ControlHandle=NULL;

/*QueueHandle*/
QueueHandle_t g_xQueuePlayMusic;
static SemaphoreHandle_t g_xI2cMutex; //信号量句柄
/*extern task*/


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void GetI2cMutex(void)
{
	xSemaphoreTake(g_xI2cMutex,portMAX_DELAY);
}
void ReleaseI2cMutex(void)
{
	xSemaphoreGive(g_xI2cMutex);
}


/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */
	LCD_Init();
	LCD_Clear();
  
	MPU6050_Init();
	IRReceiver_Init();
	RotaryEncoder_Init();
	g_xI2cMutex=xSemaphoreCreateMutex();
  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
//  extern void Start_Menu(void);
//  xTaskCreate(Start_Menu,"Start_Menu_Task",128, NULL, osPriorityNormal,NULL);
	
  extern void Task_Control(void);
  xTaskCreate(Task_Control,"Task_Control",128, NULL, osPriorityNormal,&xTask_ControlHandle);
	
//  extern void Music1_Task(void* params);
//  xTaskCreate(Music1_Task,"Task_Control",128, NULL, osPriorityNormal+1,NULL);  
  
    //Game_Snake_Init();
//	extern void Game_Snake_Init(void* params);
//	xTaskCreate(Game_Snake_Init,"Task_Control",128, NULL, osPriorityNormal,&xTask_ControlHandle);
  
	
//	extern void Third_Menu(void* params);
//	xTaskCreate(Third_Menu,"Start_Menu_Task",128, &AllItem_Data[1], osPriorityNormal,NULL);


//extern void game1_task(void *params);
	
  //xTaskCreate(MenuTask,"menutask",128, NULL, osPriorityNormal,NULL);
   //extern void game2_task(void);
  //xTaskCreate(game1_task,"PlayGame",128, NULL, osPriorityNormal,NULL);
   //xTaskCreate(game2_task,"PlayGame2",128, NULL, osPriorityNormal,NULL);
  //xTaskCreate(PlayMusic,"PlayMusic",128, NULL, osPriorityNormal,NULL);
  //xTaskCreate(IR_Contorl_Menu_task,"IRContorMenuTask",128, NULL, osPriorityNormal+1,NULL);
  //text
  

  

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
#if 0
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	
	//恢复与暂停
	uint8_t Runing_State=0;
	LCD_Init();
	LCD_Clear();
	
	uint8_t dev, data;
	TypedefDataIR Data_Input_IR1;
    IRReceiver_Init();
	
	
	/*创建音乐播放器的队列*/
	g_xQueuePlayMusic = xQueueCreate(8,sizeof(TypedefDataIR));

    while (1)
    {
		//读取红外遥控器，dev读取的是哪一个设备id,把哪一个按键的值保存在data里
        if (pdPASS==xQueueReceive(g_xQueuePlayMusic,&Data_Input_IR1,portMAX_DELAY))
        {
			data=Data_Input_IR1.data;
            /*播放音乐*/
			if(data==0xa8)
			{
				extern void PlayMusic(void *params);
				if(xSoundTaskHandle==NULL)
				{
					//LCD_Clear();
					//LCD_PrintString(0,0,"Play Musicing");
					ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &xSoundTaskHandle);
					Runing_State=1;
				}
				else
				{
					if(Runing_State)
					{
						vTaskSuspend(xSoundTaskHandle);   //传递任务句柄  暂停这个任务使这个任务到暂停状态里面去
						Runing_State = 0;
						PassiveBuzzer_Control(0);
						//LCD_ClearLine(0,2);
						//LCD_PrintString(0,2,"Music Pause");
						
					}
					else
					{
						vTaskResume(xSoundTaskHandle);   //恢复这个任务，使这个任务到ready状态里面去
						Runing_State = 1;
						LCD_ClearLine(0,2);
					}
				}
				
			}
			/*停止音乐*/
			if(data==0xa2)
			{
				if(xSoundTaskHandle!=NULL)
				{
					//LCD_Clear();
					//LCD_PrintString(0,0,"Delect Music");
					vTaskDelete(xSoundTaskHandle);    //杀死这个任务，只要把这个任务的句柄传递到这个函数
					PassiveBuzzer_Control(0);//停止蜂鸣器
					xSoundTaskHandle=NULL;
				}
				else
				{
					//LCD_ClearLine(0,2);
					//LCD_PrintString(0,2,"No music");
					vTaskDelay(100);
					//LCD_ClearLine(0,2);
				}
			}
			
        }
    }
  /* USER CODE END StartDefaultTask */
}
#endif
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

