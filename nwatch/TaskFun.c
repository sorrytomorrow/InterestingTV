#include "Data.h"
#include "driver_ir_receiver.h"

/*Set Time*/
struct Time_Data TimeClock={22,0,0};

const char* GameData[]={"Game","PlatForm","PlatForm Two","Car"};
struct Item_Data AllItem_Data[] = {{NULL,NULL},{GameData,4},{GameData,4},{GameData,4}};
/*动态创建任务*/
static TaskHandle_t g_xTaskHanlde_Second_Menu=NULL;
static TaskHandle_t g_xTaskHanlde_Start_Menu=NULL;
static TaskHandle_t g_xTaskHanlde_Third_Menu=NULL;
static uint8_t Layer=0;
static QueueHandle_t g_xQueueTaskControl_IR;
SemaphoreHandle_t g_xIRBinary;
struct Task_Data Task_AllData[]={{"Start_Menu",Start_Menu,&g_xTaskHanlde_Start_Menu,NULL},
							{"Second_Menu",Second_Menu,&g_xTaskHanlde_Second_Menu,NULL},
							{"Third_Menu",Third_Menu,&g_xTaskHanlde_Third_Menu,&AllItem_Data[1]}};

								
void Task_Control(void)
{
	g_xQueueTaskControl_IR = xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueueTaskControl_IR);
	TypedefDataIR DataIR;
	g_xIRBinary=xSemaphoreCreateBinary();
	//创建所有任务
	//CreateTask(&Task_AllData[Layer+2]);
//	CreateTask(&Task_AllData[Layer+1]);
	CreateTask(&Task_AllData[Layer]);
//	SuspendTask(&Task_AllData[Layer+1]);
	//SuspendTask(&Task_AllData[Layer+2]);
	while(1)
	{
		
		if(pdPASS==xQueueReceive(g_xQueueTaskControl_IR,&DataIR,portMAX_DELAY))
		{
			Anal_IRData(&DataIR);
			//确认
			if(Ment==Enter)
			{
				if(Layer<2)
				{
					vTaskDelay(10);
					DelectTask(&Task_AllData[Layer]);
					Layer++;
					testDrawProcess(&u8g2);
					vTaskDelay(10);
					CreateTask(&Task_AllData[Layer]);
					Ment=None;
				}
			}
			//返回
			else if(Ment==Return)
			{
				if(Layer>0)
				{
					vTaskDelay(10);
					DelectTask(&Task_AllData[Layer]);
					Layer--;
					testDrawProcess(&u8g2);
					vTaskDelay(10);
					CreateTask(&Task_AllData[Layer]);
					Ment=None;
				}
			}
			
		}
		Task_AllData[2].p_AllItem_Data=&AllItem_Data[i_pageAll];      //实时更新页面三里面的参数
	}
	
}






/*Operate Task*/
void CreateTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	xTaskCreate(Temp_TaskData->func,Temp_TaskData->Name,128,
					Temp_TaskData->p_AllItem_Data,osPriorityNormal+1,Temp_TaskData->p_TaskHandle);
}

void DelectTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	vTaskDelete(*Temp_TaskData->p_TaskHandle);
	
}

/*analysis IR_Data*/
void Anal_IRData(void* params)
{
	TypedefDataIR* TempData=params;
	if(TempData->data==0xe0)
		Ment=Left;
	else if(TempData->data==0x90)
		Ment=Right;
	else if(TempData->data==0xa8)
		Ment=Enter;
	else if(TempData->data==0xc2)
		Ment=Return;
		
}

#if 1
void SuspendTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	vTaskSuspend(*Temp_TaskData->p_TaskHandle);
}

void ResumeTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	vTaskResume(*Temp_TaskData->p_TaskHandle);
	
}
#endif


