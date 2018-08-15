/*
** ###################################################################
**     Processors:          MKL25Z128FM4
**                          MKL25Z128FT4
**                          MKL25Z128LH4
**                          MKL25Z128VLK4
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          GNU C Compiler - CodeSourcery Sourcery G++
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    KL25P80M48SF0RM, Rev.3, Sep 2012
**     Version:             rev. 2.5, 2015-02-19
**     Build:               b150220
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright (c) 2015 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2012-06-13)
**         Initial version.
**     - rev. 1.1 (2012-06-21)
**         Update according to reference manual rev. 1.
**     - rev. 1.2 (2012-08-01)
**         Device type UARTLP changed to UART0.
**     - rev. 1.3 (2012-10-04)
**         Update according to reference manual rev. 3.
**     - rev. 1.4 (2012-11-22)
**         MCG module - bit LOLS in MCG_S register renamed to LOLS0.
**         NV registers - bit EZPORT_DIS in NV_FOPT register removed.
**     - rev. 1.5 (2013-04-05)
**         Changed start of doxygen comment.
**     - rev. 2.0 (2013-10-29)
**         Register accessor macros added to the memory map.
**         Symbols for Processor Expert memory map compatibility added to the memory map.
**         Startup file for gcc has been updated according to CMSIS 3.2.
**         System initialization updated.
**     - rev. 2.1 (2014-07-16)
**         Module access macro module_BASES replaced by module_BASE_PTRS.
**         System initialization and startup updated.
**     - rev. 2.2 (2014-08-22)
**         System initialization updated - default clock config changed.
**     - rev. 2.3 (2014-08-28)
**         Update of startup files - possibility to override DefaultISR added.
**     - rev. 2.4 (2014-10-14)
**         Interrupt INT_LPTimer renamed to INT_LPTMR0.
**     - rev. 2.5 (2015-02-19)
**         Renamed interrupt vector LLW to LLWU.
**
** ###################################################################
*/

/*!
 * @file MKL25Z4
 * @version 2.5
 * @date 2015-02-19
 * @brief Device specific configuration file for MKL25Z4 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "MKL25Z4.h"

//�ں�ʱ��Ƶ��Ϊ48000000
uint32_t SystemCoreClock = 48000000;

//======================================================================
//�������ƣ�SystemInit
//���ܸ�Ҫ����ʼ��ϵͳ��ʱ�ӣ�����PORTʱ��������
//����˵������
//�������أ���
//======================================================================
void SystemInit (void) {

	unsigned int i = 0;
	char temp_reg = 0;

#if (DISABLE_WDOG)  //�رտ��Ź�
    SIM->COPC = (uint32_t)0x00u;    //SIM_COPC: COPT=0,COPCLKS=0,COPW=0
#endif

//1.���ȴ�FEIģʽ���ɵ�FBEģʽ

    //C2= 0x1C����Ϊ���ǵĺ��İ��ϲ������ⲿ����Ϊ8Mhz�����ڸ�Ƶ�ʷ�Χ��
    //C2[RANGE]����Ϊ0b01��C2[HGO]��Ϊ1�����þ��������и������������Ϊ
    //����ʹ�õ��ⲿ�ο�ʱ��Դ�Ǿ�������C2[EREFS]����Ϊ1��
    MCG_C2 = (MCG_C2_RANGE0(1) | MCG_C2_EREFS0_MASK);
    //C1 = 0x90 ��C1[CLKS]����Ϊ2��b10���Ա�ѡ����Ϊϵͳʱ��Դ���ⲿ�ο�ʱ�ӡ�
    //C1[FRDIV]����Ϊ3'b011����Ӧ256��Ƶ,��Ϊ8MHz/256=31.25kHz����FLLҪ���
    //31.25kHz��39.0625 kHzƵ�ʷ�Χ�ڡ�C1[IREFS]���Ϊ0��ѡ���ⲿ�ο�ʱ�Ӻ�
    //�ⲿ����
    MCG_C1 = (MCG_C1_CLKS(2) | MCG_C1_FRDIV(3));
    //��Ҫ�ȵ�S[OSCINIT]����λ�ⲿ�������ʹ��
    for (i = 0 ; i < 20000 ; i++)
    {
        //���S[OSCINIT]��ѭ������֮ǰ����λ������ѭ��
        if (MCG_S & MCG_S_OSCINIT0_MASK) break;
    }
    //�ȴ��ο�ʱ��״̬λ��0
    for (i = 0 ; i < 2000 ; i++)
    {
        //���IREFST��ѭ������֮ǰ����0������ѭ��
        if (!(MCG_S & MCG_S_IREFST_MASK)) break;
    }
    //�ȴ�ʱ��״̬λ����ʾʱ��ԴΪ�ⲿ�ο�ʱ��
    for (i = 0 ; i < 2000 ; i++)
    {
        //���CLKST��ʾ�ⲿʱ�ӱ�ѡ����ѭ������֮ǰ������ѭ��
        if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break;
    }

//2.���ڴ���FBE״̬��ʹ��ʱ�Ӽ���������FBEֱ��ת��ΪPBEģʽ
    MCG_C6 |= MCG_C6_CME0_MASK;
    //����PLLΪ2��Ƶ
    MCG_C5 |= MCG_C5_PRDIV0(1);
    //����MCG_C6 ������PLL��Ƶ���Ӳ���ʱ��PLL��PLLSλ����λ��ʱ��PLL��MCGOUTʱ��Դ��Ȼ���ⲿ�ο�ʱ��
    temp_reg = MCG_C6; //�洢��ǰC6��ֵ����ΪCME0λ֮ǰ����λ�ˣ�
    temp_reg &= ~MCG_C6_VDIV0_MASK; //��VDIV��0
    temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0); // ����дֵ��VDIV ����ʱ��PLL
    MCG_C6 = temp_reg; // ����MCG_C6��ֵ
    // �ȴ�PLLST״̬λ����
    for (i = 0 ; i < 2000 ; i++)
    {
        //���PLLST��ѭ������֮ǰ����λ������ѭ��
        if (MCG_S & MCG_S_PLLST_MASK)
        	break;
    }
    //�ȴ�LOCKλ����
    for (i = 0 ; i < 4000 ; i++)
    {
        //���LOCK��ѭ������֮ǰ����λ������ѭ��
        if (MCG_S & MCG_S_LOCK0_MASK)
        	break;
    }

//3.���ڴ���PBEģʽ�����PBEģʽת����PEEģʽ
    // ���ú���ʱ�ӷ�Ƶ��2��Ƶ
    //��������ʱ�ӷ�Ƶ��2��Ƶ (����ʱ�ӵ�ʱ��Դ�Ǻ���ʱ��)
    SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(1) );
    //��CLKS����CLKS��·��������ѡ��PLL��ΪMCGCLKOUT
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    // �ȴ�ʱ��״̬λ����
    for (i = 0 ; i < 2000 ; i++)
    {
        //���CLKST��ѭ������֮ǰ����3������ѭ��
        if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3) break;
    }
//4.���ڴ���PEEģʽ

    //��ʼ���󣬿�ʼPORTģ��ʱ��
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK
  			  | SIM_SCGC5_PORTB_MASK
  			  | SIM_SCGC5_PORTC_MASK
  			  | SIM_SCGC5_PORTD_MASK
  			  | SIM_SCGC5_PORTE_MASK;
}
