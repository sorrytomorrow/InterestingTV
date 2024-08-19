#include "u8g2.h"


#define SEND_BUFFER_DISPLAY_MS(u8g2, ms)\
  do {\
    u8g2_SendBuffer(u8g2); \
    HAL_Delay(ms);\
  }while(0);

  
//��������ʾ
void testDrawProcess(u8g2_t *u8g2)
{
	for(int i=10;i<=80;i=i+2)
	{
		u8g2_ClearBuffer(u8g2); 
			
		char buff[20];
		sprintf(buff,"%d%%",(int)(i/80.0*100));
		
		u8g2_SetFont(u8g2,u8g2_font_ncenB12_tf);
		u8g2_DrawStr(u8g2,16,32,"STM32 U8g2");//�ַ���ʾ
		
		u8g2_SetFont(u8g2,u8g2_font_ncenB08_tf);
		u8g2_DrawStr(u8g2,100,49,buff);//��ǰ������ʾ
		
		u8g2_DrawRBox(u8g2,16,40,i,10,4);//Բ��������ο�
		u8g2_DrawRFrame(u8g2,16,40,80,10,4);//Բ�Ǿ���
		
		u8g2_SendBuffer(u8g2);
	}
	HAL_Delay(500);
}
