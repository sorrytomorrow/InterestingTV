#ifndef __MenuAll_H__
#define __MenuAll_H__

#define Right 1
#define Left 2
#define None 0

struct Image_Data{
	const unsigned char* Image;
	char* Name;

};


void Start_Menu(void);
void Menu(void);

#endif