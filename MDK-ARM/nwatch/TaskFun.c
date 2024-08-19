#include "TaskFun.h"
/*task handel*/
TaskHandle_t xMenuOrigin_TaskHandle;
TaskHandle_t xgame1_TaskHandle;
TaskHandle_t xgame2_TaskHandle;
TaskHandle_t xPlayMusic_TaskHandle;
TaskHandle_t xMenuSecond1_TaskHandle;
TaskHandle_t xMenuSecond2_TaskHandle;

/*任务句柄数组*/
//TaskHandle_t HandleAll[][5]={{}};

/*extern task*/
extern void menu_task_origin(void* params);
extern void game1_task(void *params);
extern void game2_task(void* params);
extern void PlayMusic(void *params);
extern void menu_task_second(void* params);
/*creat task all*/
void CreatAllTask(void)
{
//	xTaskCreate(game1_task,"PlayGame1",128, NULL, osPriorityNormal,&xgame1_TaskHandle);
//	xTaskCreate(game2_task,"PlayGame2",128, NULL, osPriorityNormal,&xgame2_TaskHandle);
	xTaskCreate(menu_task_origin,"Showorigin",128,&Menu_Origin,osPriorityNormal,&xMenuOrigin_TaskHandle);
	xTaskCreate(menu_task_second,"Showsecond",128,&Menu_Second_Plat1,osPriorityNormal,&xMenuSecond1_TaskHandle);
	xTaskCreate(menu_task_second,"Showsecond",128,&Menu_Second_Plat2,osPriorityNormal,&xMenuSecond2_TaskHandle);
//	xTaskCreate(PlayMusic,"PlayMusic",128,NULL,osPriorityNormal,&xPlayMusic_TaskHandle);

}

/*suspend task all*/
void SuspendAllTask(void)
{
//	vTaskSuspend(xgame1_TaskHandle);
//	vTaskSuspend(xgame2_TaskHandle);
	vTaskSuspend(xMenuOrigin_TaskHandle);
	vTaskSuspend(xMenuSecond1_TaskHandle);
//	vTaskSuspend(xPlayMusic_TaskHandle);

}
/*wake task*/
void ResumeTask(uint8_t page,int8_t index)
{
	if(page==1)
	{
		if(index==0)
			vTaskResume(xMenuOrigin_TaskHandle);
	}
	else if(page==2)
	{
		if(index==0)
			vTaskResume(xMenuSecond1_TaskHandle);
	}

}

/*suspend task*/
void SuspendOneTask(uint8_t page,int8_t index)
{
	if(page==1)
	{
		if(index==0)
			vTaskSuspend(xMenuOrigin_TaskHandle);
	}
	else if(page==2)
	{
		if(index==0)
			vTaskSuspend(xMenuSecond1_TaskHandle);
	}

}

/*creat task*/
void CreatOneTask(uint8_t page,int8_t index)
{
	if(page==1)
	{
		xTaskCreate(menu_task_origin,"Showorigin",128,&Menu_Origin,osPriorityNormal,&xMenuOrigin_TaskHandle);
	}
	else if(page==2)
	{
		if(index==0)
			xTaskCreate(menu_task_second,"Showsecond",128,&Menu_Second_Plat1,osPriorityNormal,&xMenuSecond1_TaskHandle);
		else if(index==1)
			xTaskCreate(menu_task_second,"Showsecond",128,&Menu_Second_Plat2,osPriorityNormal,&xMenuSecond1_TaskHandle);
	}

}

/*delect task*/
void DeleteOneTask(uint8_t page,int8_t index)
{
	if(page==1)
	{
		if(index==0)
			vTaskDelete(xMenuOrigin_TaskHandle);
	}
	else if(page==2)
	{
		if(index==0)
			vTaskDelete(xMenuSecond1_TaskHandle);
		else if(index==1)
			vTaskDelete(xMenuSecond2_TaskHandle);
	}

}



