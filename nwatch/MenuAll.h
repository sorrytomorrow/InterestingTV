#ifndef __MenuAll_H__
#define __MenuAll_H__

#include "typedefs.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "cmsis_os.h"

#include "fun_u8g2.h"
#include "stm32_u8g2.h"
#include "driver_timer.h"

#define Right 1
#define Left 2
#define Enter 3
#define Return 4
#define None 0
#define DeviceName "Otto"


extern u8g2_t u8g2;
extern uint8_t Ment;
extern SemaphoreHandle_t g_xIRBinary;
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
	char* Name;
	void (*func)(void);           //函数指针
};

struct Task_Data{
	char* Name;
	void (*func)(void);
	TaskHandle_t*  p_TaskHandle;
	
	
};

//函数声明
void Start_Menu(void);
void Second_Menu(void);
void CreateTask(void* params);
void DelectTask(void* params);
void SuspendTask(void* params);
void ResumeTask(void* params);
void Anal_IRData(void* params);
#endif