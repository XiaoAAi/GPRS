/*
 * OLED.h
 *
 *  Created on: 2017Äê11ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef _OLED_H_
#define _OLED_H_
#include "gpio.h"
#include "common.h"
#include "delay.h"

#define  OLED_SCL_PIN	(PTB_NUM | 16)
#define  OLED_SDA_PIN	(PTB_NUM | 10)
#define  OLED_RST_PIN	(PTB_NUM | 8)
#define  OLED_DC_PIN	(PTB_NUM | 2)
#define  OLED_CS_PIN	(PTB_NUM | 0)



void OLED_Init(void);
void OLED_Fill(uint_8 bmp_data);
void OLED_WrDat(uint_8 data);
void OLED_Wr6Dat(uint_8 data);
void OLED_PutPixel(uint_8 x,uint_8 y);
void OLED_ClrPixel(uint_8 x,uint_8 y);
void OLED_Set_Pos(uint_8 x, uint_8 y);
void OLED_HEXACSII(uint_16 hex,uint_8* Print);
void OLED_Print_Num1(uint_8 x, uint_8 y, int_16 num);
void dis_bmp(uint_16 high, uint_16 width, uint_8 *p,uint_8 value);
void OLED_Print_Num(uint_8 x, uint_8 y, uint_16 num);
void OLED_P8x16Str(uint_8 x,uint_8 y,uint_8 ch[]);
void OLED_P6x8Str(uint_8 x,uint_8 y,uint_8 ch[]);



#endif /* 08_SOURCE_OLED_H_ */
