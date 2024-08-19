#ifndef __TaskFun_h__
#define __TaskFun_h__
#include "menudata.h"
#include "typedefs.h"
#include "cmsis_os.h"

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include <stdlib.h>
#include <stdio.h>
void CreatAllTask(void);
void SuspendAllTask(void);
void ResumeTask(uint8_t page,int8_t index);
void SuspendOneTask(uint8_t page,int8_t index);
void CreatOneTask(uint8_t page,int8_t index);
void DeleteOneTask(uint8_t page,int8_t index);
#endif