#ifndef _DS18B20_H     //防止重复定义（_DS18B20_H  开头)
#define _DS18B20_H

//头文件包含
#include "common.h"    //包含公共要素头文件
#include "gpio.h"      //用到gpio构件
#include "delay.h"		//延时函数

vuint_16 Temperature;

/*端口定义
 * */
#define DB20_IO    GPIOC
#define DB20_PIN   (0x01<<17)

#define DS18B20	   (PTC_NUM | 17)
/*常量定义
 * */
#define  SKIP_ROM  		0xCC			//跳过ROM命令
#define  FUNCTION_TURN	0x44			//功能转换命令
#define  READ_DATA		0xBE			//读暂存器命令

/*操作定义
 * */
#define   ds18b20_Out()   		(DB20_IO->PDDR |= DB20_PIN)			//输出操作
#define   ds18b20_In()   		(DB20_IO->PDDR &= ~DB20_PIN)		//输入操作
#define   ds18b20_High()		(DB20_IO->PDOR |= DB20_PIN)			//拉高操作
#define   ds18b20_Low()			(DB20_IO->PDOR &= ~DB20_PIN)		//拉低操作
#define   ds18b20_Val()         (DB20_IO->PDIR & DB20_PIN)			//输入的值

/*DS18B20的定义函数
 * */
void ds18b20_Init(void);			//初始化DS18B20为GPIO口和输出
void ds18b20_Reset(void);			//DS18B20的复位操作
void ds18b20_Write(uint_8 data);	//DS18B20的写操作
uint_8 ds18b20_ReadByte(void);				//DS18B20的读一个字节操作
uint_8 ds18b20_Read(void);				//DS18B20的读数据操作
uint_8 ds18b20_Judge(uint_8 T_H);	//DS18B20的判断温度

#endif
