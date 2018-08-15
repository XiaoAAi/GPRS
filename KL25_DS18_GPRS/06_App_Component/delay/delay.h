/*
 * delay.h
 *	��Ϊ������Source�ļ��в�����includes.h�ļ����ã�����û��ע���ļ��ṹ��
 *	���Ǻ��ǲ��⣬Ϊʲôlight.h�ļ����ܱ�includes.h�ļ����ã����Լ�����д���ļ�ȴ���ܱ����á�
 *
 *  Created on: 2017��11��18��
 *      Author: Administrator
 */

#ifndef _DELAY_H     //��ֹ�ظ����壨_DELAY_H  ��ͷ)
#define _DELAY_H
//ͷ�ļ�����
#include "common.h"    //��������Ҫ��ͷ�ļ�
#include "gpio.h"      //�õ�gpio����

uint_16 gprs_count ;

//ʱ�Ӽ���
uint_16 tpm_count;
//OLED��ʾ��ʱ
uint_16 oled_count;

void delay_us(uint_16 m);
void delay_ms(uint_16 m);
void delay_s(uint_16 m);
void delayms(uint_16 m);
//���ܣ�OLED��ʱ��ÿһ��ѭ��20ms
//������uint_8 timer: ��ʾ����Ҫ��ʱ��ʱ����sΪ��λ
//����ֵ��uint_8 flag  1 ��ʾ��ʱ�ɹ�  0 ��ʾ������ʱ
uint_8 DelayS(uint_8 timer);




#endif

