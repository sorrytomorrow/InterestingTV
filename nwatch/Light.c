#include "Data.h"
#include "driver_ir_receiver.h"

static uint8_t Light_Status=0;
static QueueHandle_t g_xLightQueue;

static void Anl_InputData(void* params)
{
	TypedefDataIR* Temp = params;

	if(Temp->data == 0xc2)
	{
		
		vQueueDelete(g_xLightQueue);
		Clear_RegisterQueueHandle();
		testDrawProcess(&u8g2);
		xTaskNotifyGive(xTask_ControlHandle);
		xSemaphoreGive(g_xIRMutex);// Õ∑≈À¯
		vTaskDelete(NULL);
	}
	
	
}


void Light_task(void* params)
{
	g_xLightQueue=xQueueCreate(1,sizeof(TypedefDataIR));
	RegisterQueueHandle(g_xLightQueue);
	TypedefDataIR Data_Input;
	uint32_t Light_Delay=0;
	xSemaphoreTake(g_xIRMutex,0);   //…œÀ¯
	u8g2Init(&u8g2);
	
	
	while(1)
	{
		xQueueReceive(g_xLightQueue,&Data_Input,0);
		Anl_InputData(&Data_Input);
		u8g2_DrawBox(&u8g2,0,0,128,64);
		u8g2_SendBuffer(&u8g2);

	}
}