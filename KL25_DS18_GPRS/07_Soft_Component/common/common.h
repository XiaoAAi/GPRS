//=====================================================================
//�ļ����ƣ�common.h
//���ܸ�Ҫ������Ҫ��ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2012-10-12   V1.0
//         2013-02-02 V2.0(WYH)
//         2016-03-27 V3.0(XD)
//=====================================================================
#ifndef _COMMON_H    //��ֹ�ظ����壨_COMMON_H  ��ͷ)
#define _COMMON_H

//1��оƬ�Ĵ���ӳ���ļ����������ں������ļ�
#include "MKL25Z4.h"      // ����оƬͷ�ļ�
#include "core_cm0plus.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "system_MKL25Z4.h"

//2�����忪�����ж�
#define ENABLE_INTERRUPTS   __enable_irq()  //�����ж�
#define DISABLE_INTERRUPTS  __disable_irq()  //�����ж�


//3��λ�����꺯������λ����λ����üĴ���һλ��״̬��
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //�üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //��üĴ���һλ��״̬

//4���ض�������������ͣ����ͱ����궨�壩
typedef unsigned char        uint_8;   // �޷���8λ�����ֽ�
typedef unsigned short int   uint_16;  // �޷���16λ������
typedef unsigned long int    uint_32;  // �޷���32λ��������
typedef unsigned long long   uint_64;
typedef char                 int_8;    // �з���8λ��
typedef short int            int_16;   // �з���16λ��
typedef int                  int_32;   // �з���32λ��
typedef long long            int_64;
//���Ż�����
typedef volatile uint_8      vuint_8;  // ���Ż��޷���8λ�����ֽ�
typedef volatile uint_16     vuint_16; // ���Ż��޷���16λ������
typedef volatile uint_32     vuint_32; // ���Ż��޷���32λ��������
typedef volatile int_8       vint_8;   // ���Ż��з���8λ��
typedef volatile int_16      vint_16;  // ���Ż��з���16λ��
typedef volatile int_32      vint_32;  // ���Ż��з���32λ��

//5. ����ϵͳʹ�õ�ʱ��Ƶ��
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // оƬϵͳʱ��Ƶ��(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // оƬ����ʱ��Ƶ��(KHz)


#endif //��ֹ�ظ����壨_COMMON_H  ��β)
