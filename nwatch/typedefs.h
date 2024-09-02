/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
typedef uint8_t byte;
typedef uint16_t uint;
typedef uint32_t ulong;

typedef unsigned char millis8_t;
typedef unsigned int  millis_t;


#define UPT_MOVE_NONE	0
#define UPT_MOVE_RIGHT	1
#define UPT_MOVE_LEFT	2

#define ploatfrom_1     0
#define ploatfrom_2     1

#define NOINVERT	false
#define INVERT		true

typedef struct Data_OT{
	int32_t data;
}TypedefDataOT;


typedef enum
{
	DISPLAY_DONE,
	DISPLAY_BUSY,
//	DISPLAY_TOOFAST
} display_t;

#define PROGMEM


// Quick and easy macro for a for loop
#define LOOP(count, var) for(var=0;var<count;var++)



#endif /* TYPEDEFS_H_ */
