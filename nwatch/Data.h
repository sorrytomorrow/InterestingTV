#ifndef __MenuAll_H__
#define __MenuAll_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "cmsis_os.h"
#include "Image.h"

#include "fun_u8g2.h"
#include "stm32_u8g2.h"
#include "driver_timer.h"

#define Right 1
#define Left 2
#define Enter 3
#define Return 4
#define None 0
#define DeviceName "Otto"


/*struct*/
struct Image_Data{
	const unsigned char* Image;
	char* Name;
};

struct Time_Data{
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
};

struct Item_Data{
	const char** Name;     //对指针的地址进解指针操作,得到这个指针里面存储的字符串的地址
	uint8_t Num;
	struct Task_Data* p_FunData;  
};

struct Task_Data{
	char* Name;
	void (*func)(void* params);
	TaskHandle_t*  p_TaskHandle;
	//struct Item_Data* p_AllItem_Data;
	void* p_AllItem_Data;
	uint32_t Word;
};

struct MusicFun{
	void (* func)(void);
	TaskHandle_t* Music_Handle;
};


/*extern variable*/
extern u8g2_t u8g2;
extern uint8_t Ment;
extern uint8_t Layer;
extern SemaphoreHandle_t g_xIRMutex;
extern uint8_t i_pageAll;
extern uint8_t i_indexAll;
extern struct Time_Data TimeClock;
extern const char* GameData[];
extern struct Item_Data AllItem_Data[];
extern struct Task_Data Task_AllData[];
extern TaskHandle_t xTask_ControlHandle;
//函数声明
void Start_Menu(void* params);
void Second_Menu(void* params);
void Third_Menu(void* params);
void CreateTask(void* params);
void DelectTask(void* params);
void SuspendTask(void* params);
void ResumeTask(void* params);
void Anal_IRData(void* params);







#endif