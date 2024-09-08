#include "Data.h"
#include "driver_ir_receiver.h"
#include "driver_passive_buzzer.h"

enum{
	Music_Play,
	Music_Pause,
	Music_Return,
	Music_None,
};
static uint8_t Music_Ment=Music_None;
static QueueHandle_t g_xQueueMusic1_IR;


/*extern*/
extern TaskHandle_t xMusic1_TaskHandle;

static void Anl_InputData(void* params)
{
	TypedefDataIR* Temp = params;
	static bool flag_P=true;
	
	if(Temp->data==0xa8)
	{
		/*暂停*/
		if(flag_P)
		{
			Music_Ment=Music_Pause;
			flag_P=false;
		}
		/*开始*/		
		else
		{
			Music_Ment=Music_Play;
			flag_P=true;
		}
	}
	/*返回*/
	else if(Temp->data==0xc2)
	{
		Music_Ment=Music_Return;
		
	}
}


void Task_Music_Ir(TaskHandle_t* Handle)
{
	TypedefDataIR Data_Input;
	xSemaphoreTake(g_xIRMutex,0);
	while(1)
	{
		
		
		xQueueReceive(g_xQueueMusic1_IR,&Data_Input,portMAX_DELAY);			//读取数据
		Anl_InputData(&Data_Input);
		/*唤醒*/
		if(Music_Ment==Music_Play)
		{
			vTaskResume(*Handle);
			Music_Ment=Music_None;
		}
			
		else if(Music_Ment==Music_Pause)
		{
			vTaskSuspend(*Handle);
			PassiveBuzzer_Control(0);//停止蜂鸣器
			Music_Ment=Music_None;

		}
		else if(Music_Ment==Music_Return)
		{
			
			vTaskDelete(*Handle);
			PassiveBuzzer_Control(0);//停止蜂鸣器
			Music_Ment=Music_None;
			/*释放资源*/
			vQueueDelete(g_xQueueMusic1_IR);
			Clear_RegisterQueueHandle();
			xTaskNotifyGive(xTask_ControlHandle);  //任务通知
			xSemaphoreGive(g_xIRMutex);//释放锁
			testDrawProcess(&u8g2);
			vTaskDelete(NULL);
		}
		
	}
}


void Music_Task(void* params)
{
	struct MusicFun* Temp = params;
	TypedefDataIR Data_Input;
	PassiveBuzzer_Init();
	g_xQueueMusic1_IR = xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueueMusic1_IR);
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawXBMP(&u8g2,39,10,50,50,music_tx);
	u8g2_SendBuffer(&u8g2);
	xTaskCreate(Task_Music_Ir,"Task_Music_Ir",128, Temp->Music_Handle, osPriorityNormal+1,NULL);
	
	
	while(1)
	{
		Temp->func();
		
	}
}