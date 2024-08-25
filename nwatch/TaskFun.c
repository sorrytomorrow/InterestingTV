#include "MenuAll.h"
#include "driver_ir_receiver.h"


/*动态创建任务*/
static TaskHandle_t g_xTaskHanlde_Second_Menu=NULL;
static TaskHandle_t g_xTaskHanlde_Start_Menu=NULL;

static uint8_t Layer=0;
static QueueHandle_t g_xQueueTaskControl_IR;
SemaphoreHandle_t g_xIRBinary;
struct Task_Data Task_AllData[]={{"Start_Menu",Start_Menu,&g_xTaskHanlde_Start_Menu},
							{"Second_Menu",Second_Menu,&g_xTaskHanlde_Second_Menu}};

								



void Task_Control(void)
{
	g_xQueueTaskControl_IR = xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueueTaskControl_IR);
	TypedefDataIR DataIR;
	g_xIRBinary=xSemaphoreCreateBinary();
	//创建所有任务
	CreateTask(&Task_AllData[Layer]);
//	CreateTask(&Task_AllData[Layer+1]);
//	SuspendTask(&Task_AllData[Layer+1]);
	while(1)
	{
		
		if(pdPASS==xQueueReceive(g_xQueueTaskControl_IR,&DataIR,0))
		{
			Anal_IRData(&DataIR);
			//确认
			if(Ment==Enter)
			{
				if(Layer<1)
				{
					
					DelectTask(&Task_AllData[Layer]);
					Layer++;
					testDrawProcess(&u8g2);
					CreateTask(&Task_AllData[Layer]);
				}
			}
			//返回
			else if(Ment==Return)
			{
				if(Layer>0)
				{
					
					DelectTask(&Task_AllData[Layer]);
					Layer--;
					testDrawProcess(&u8g2);
					CreateTask(&Task_AllData[Layer]);
				}
			}
			
				
			
		}
		
		vTaskDelay(150);
	}
	
}






void CreateTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	xTaskCreate(Temp_TaskData->func,Temp_TaskData->Name,128,
					NULL,osPriorityNormal,Temp_TaskData->p_TaskHandle);
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

#if 0
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


