/*
 * delay.c
 *
 *  Created on: 2017��11��18��
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


//���ܣ�OLED��ʱ��ÿһ��ѭ��20ms
//������uint_8 timer: ��ʾ����Ҫ��ʱ��ʱ����sΪ��λ
//����ֵ��uint_8 flag  1 ��ʾ��ʱ�ɹ�  0 ��ʾ������ʱ
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
