#include "Data.h"
#include "driver_ir_receiver.h"

/*Set Time*/
struct Time_Data TimeClock={22,0,0};

/*Data*/
const char* GameData[]={"GAME","PlatForm","PlatForm Two","Snake"};
const char* MusicData[]={"MUSIC","Lone Brave","Two Tigers"};
const char* SettingData[]={"Setting","Nothing"};
const char* LightData[]={"LIGHT","Open"};
const char* WriterData[]={"WRITER","IMAGE"};
const char* VideoData[]={"VIDEO","Nothing"};
/*Game*/
extern void game1_task(void* params);
extern void game2_task(void* params);
extern void Game_Snake_Init(void* params);
TaskHandle_t xgame1_TaskHandle=NULL;
TaskHandle_t xgame2_TaskHandle=NULL;
TaskHandle_t xgame3_TaskHandle=NULL;
static struct Task_Data Task_GameData[]={{NULL,NULL,NULL,NULL,NULL},{"game1Task",game1_task,&xgame1_TaskHandle,NULL,128},{"game2Task",game2_task,&xgame2_TaskHandle,NULL,128},
								{"game3Task",Game_Snake_Init,&xgame3_TaskHandle,NULL,250}};
/*Light*/
extern void Light_task(void* params);
struct Task_Data Task_LightData[]={{NULL,NULL,NULL,NULL,NULL},{"LightTask",Light_task,NULL,NULL,128}};

/*Music*/
extern void MUSIC1_Analysis(void);
extern void MUSIC2_Analysis(void);
extern void Music_Task(void* params);
static TaskHandle_t xMusic1_TaskHandle=NULL;
static TaskHandle_t xMusic2_TaskHandle=NULL;
struct MusicFun MusicAllFun[] = {{MUSIC1_Analysis,&xMusic1_TaskHandle},{MUSIC2_Analysis,&xMusic2_TaskHandle}};
static struct Task_Data Task_MusicData[]={{NULL,NULL,NULL,NULL,NULL},{"Music1Task",Music_Task,&xMusic1_TaskHandle,&MusicAllFun[0],128}
										 ,{"Music2Task",Music_Task,&xMusic2_TaskHandle,&MusicAllFun[1],128}};

										 
/*Writer*/
extern void Writer_Iamge_task(void* params);								
static struct Task_Data Task_WriterData[]={{NULL,NULL,NULL,NULL,NULL},{"WriterImage",Writer_Iamge_task,NULL,NULL,128}};

/*All*/
struct Item_Data AllItem_Data[] = {{NULL,NULL,NULL},{LightData,2,Task_LightData},{MusicData,3,Task_MusicData},{GameData,4,Task_GameData},{SettingData,2,NULL}
									,{WriterData,2,Task_WriterData},{VideoData,2,NULL}};

/*Page Handle*/
static TaskHandle_t g_xTaskHanlde_Second_Menu=NULL;
static TaskHandle_t g_xTaskHanlde_Start_Menu=NULL;
static TaskHandle_t g_xTaskHanlde_Third_Menu=NULL;

struct Task_Data Task_AllData[]={{"Start_Menu",Start_Menu,&g_xTaskHanlde_Start_Menu,NULL,90},
							{"Second_Menu",Second_Menu,&g_xTaskHanlde_Second_Menu,NULL,90},
							{"Third_Menu",Third_Menu,&g_xTaskHanlde_Third_Menu,&AllItem_Data[1],90}};
uint8_t Layer=0;
static QueueHandle_t g_xQueueTaskControl_IR;
//static signed char pcWriteBuffer[200];
SemaphoreHandle_t g_xIRMutex;


			
void Task_Control(void)
{
	UBaseType_t freeNum;
	TaskHandle_t xTaskHandleCur;	
	
	g_xQueueTaskControl_IR = xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueueTaskControl_IR);
	TypedefDataIR DataIR;
	g_xIRMutex=xSemaphoreCreateMutex();
	u8g2Init(&u8g2);
	
	//创建所有任务
	
	CreateTask(&Task_AllData[Layer]);

	while(1)
	{
		xSemaphoreTake(g_xIRMutex,portMAX_DELAY);
		
//		vTaskList(pcWriteBuffer);
//		printf("%s\n\r",pcWriteBuffer);	
//		printf("\n");
		
		if(pdPASS==xQueueReceive(g_xQueueTaskControl_IR,&DataIR,portMAX_DELAY))
		{
			
			
			Anal_IRData(&DataIR);
			if(ulTaskNotifyTake(pdTRUE,0))    //接收任务通知
			{
				vTaskDelay(150);
				Ment=None;
				xSemaphoreGive(g_xIRMutex);
				CreateTask(&Task_AllData[Layer]);
			}
			Task_AllData[2].p_AllItem_Data=&AllItem_Data[i_pageAll];      //实时更新页面三里面的参数
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
		xSemaphoreGive(g_xIRMutex);
		
	}
	
}






/*Operate Task*/
void CreateTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	xTaskCreate(Temp_TaskData->func,Temp_TaskData->Name,Temp_TaskData->Word,
					Temp_TaskData->p_AllItem_Data,osPriorityNormal+1,Temp_TaskData->p_TaskHandle);
}

void DelectTask(void* params)
{
	struct Task_Data* Temp_TaskData = params;
	vTaskDelete(*Temp_TaskData->p_TaskHandle);
	
}

/*analysis IR_Data*/
static void Anal_IRData(void* params)
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


