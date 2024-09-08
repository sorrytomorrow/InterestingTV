#include "Data.h"
#include "driver_ir_receiver.h"


void Writer_Iamge_task(void* params)
{
	xSemaphoreTake(g_xIRMutex,0);
	QueueHandle_t g_xQueueWriter_IR;
	TypedefDataIR Data_Input;
	g_xQueueWriter_IR = xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xQueueWriter_IR);
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawXBMP(&u8g2,34,2,60,60,Writer_Iamge);
	u8g2_SendBuffer(&u8g2);
	
	
	while(1)
	{
		xQueueReceive(g_xQueueWriter_IR,&Data_Input,portMAX_DELAY);
		if(Data_Input.data == 0xc2)
		{
			vQueueDelete(g_xQueueWriter_IR);
			Clear_RegisterQueueHandle();
			xTaskNotifyGive(xTask_ControlHandle);  //任务通知
			xSemaphoreGive(g_xIRMutex);//释放锁
			testDrawProcess(&u8g2);
			vTaskDelete(NULL);
		}
	}
}