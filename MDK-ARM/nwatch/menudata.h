#ifndef __menudata_h__
#define __menudata_h__

#include "typedefs.h"
struct MenuImageData{
	//uint8_t index;
	const byte* p_ImageData;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t hight;
};

struct MenuPageData{
	uint8_t page;
	int8_t  index;

};
/*Í¼Ïñ*/
extern struct MenuImageData Menu_Origin; 
extern struct MenuImageData Menu_PlatForm; 
extern struct MenuImageData Menu_PlatFormDouble;
extern struct MenuImageData Menu_start;
extern struct MenuImageData Menu_select_Box_left;
extern struct MenuImageData Menu_select_Box_right;
extern struct MenuImageData Menu_left;
extern struct MenuImageData Menu_right;
extern struct MenuImageData Menu_Second_Plat1;
extern struct MenuImageData Menu_Second_Plat2;
/*Ò³Ãæ*/
extern struct MenuPageData Menu_Page0;
extern struct MenuPageData Menu_Page1;
#endif