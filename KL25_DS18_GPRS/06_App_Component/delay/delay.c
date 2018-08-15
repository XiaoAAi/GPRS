/*
 * delay.c
 *
 *  Created on: 2017年11月18日
 *      Author: Administrator
 */
#include "delay.h"

void delay_us(uint_16 m) {
	
	while (m--) {
		asm("NOP");
	}
}

void delay_ms(uint_16 m) {
	uint_8 i;
	while (m--) {
		for(i=0; i<200;i++){
			delay_us(12);
		}
	}
}

void delay_s(uint_16 m){
	while(m--){
		delay_ms(1000);
	}
}


void delayms(uint_16 m)
{
	gprs_count = m;
	while(gprs_count != 0);
}


//功能：OLED计时，每一次循环20ms
//参数：uint_8 timer: 表示所需要延时的时间以s为单位
//返回值：uint_8 flag  1 表示延时成功  0 表示继续延时
uint_8 DelayS(uint_8 timer)
{
	uint_8 flag = 0;
	if(oled_count >= 100*timer)
	{
		oled_count = 0;
		flag = 1;
	}
	return flag;
}
