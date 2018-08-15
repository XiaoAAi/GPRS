/*
 * delay.h
 *	因为必须在Source文件中才能让includes.h文件调用，所以没有注意文件结构。
 *	但是很是不解，为什么light.h文件就能被includes.h文件调用，而自己所编写的文件却不能被调用。
 *
 *  Created on: 2017年11月18日
 *      Author: Administrator
 */

#ifndef _DELAY_H     //防止重复定义（_DELAY_H  开头)
#define _DELAY_H
//头文件包含
#include "common.h"    //包含公共要素头文件
#include "gpio.h"      //用到gpio构件

uint_16 gprs_count ;

//时钟计数
uint_16 tpm_count;
//OLED显示计时
uint_16 oled_count;

void delay_us(uint_16 m);
void delay_ms(uint_16 m);
void delay_s(uint_16 m);
void delayms(uint_16 m);
//功能：OLED计时，每一次循环20ms
//参数：uint_8 timer: 表示所需要延时的时间以s为单位
//返回值：uint_8 flag  1 表示延时成功  0 表示继续延时
uint_8 DelayS(uint_8 timer);




#endif

