#ifndef _DS18B20_H     //��ֹ�ظ����壨_DS18B20_H  ��ͷ)
#define _DS18B20_H

//ͷ�ļ�����
#include "common.h"    //��������Ҫ��ͷ�ļ�
#include "gpio.h"      //�õ�gpio����
#include "delay.h"		//��ʱ����

vuint_16 Temperature;

/*�˿ڶ���
 * */
#define DB20_IO    GPIOC
#define DB20_PIN   (0x01<<17)

#define DS18B20	   (PTC_NUM | 17)
/*��������
 * */
#define  SKIP_ROM  		0xCC			//����ROM����
#define  FUNCTION_TURN	0x44			//����ת������
#define  READ_DATA		0xBE			//���ݴ�������

/*��������
 * */
#define   ds18b20_Out()   		(DB20_IO->PDDR |= DB20_PIN)			//�������
#define   ds18b20_In()   		(DB20_IO->PDDR &= ~DB20_PIN)		//�������
#define   ds18b20_High()		(DB20_IO->PDOR |= DB20_PIN)			//���߲���
#define   ds18b20_Low()			(DB20_IO->PDOR &= ~DB20_PIN)		//���Ͳ���
#define   ds18b20_Val()         (DB20_IO->PDIR & DB20_PIN)			//�����ֵ

/*DS18B20�Ķ��庯��
 * */
void ds18b20_Init(void);			//��ʼ��DS18B20ΪGPIO�ں����
void ds18b20_Reset(void);			//DS18B20�ĸ�λ����
void ds18b20_Write(uint_8 data);	//DS18B20��д����
uint_8 ds18b20_ReadByte(void);				//DS18B20�Ķ�һ���ֽڲ���
uint_8 ds18b20_Read(void);				//DS18B20�Ķ����ݲ���
uint_8 ds18b20_Judge(uint_8 T_H);	//DS18B20���ж��¶�

#endif
