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

//内核时钟频率为48000000
uint32_t SystemCoreClock = 48000000;

//======================================================================
//函数名称：SystemInit
//功能概要：初始化系统的时钟，并把PORT时钟门启用
//参数说明：无
//函数返回：无
//======================================================================
void SystemInit (void) {

	unsigned int i = 0;
	char temp_reg = 0;

#if (DISABLE_WDOG)  //关闭看门狗
    SIM->COPC = (uint32_t)0x00u;    //SIM_COPC: COPT=0,COPCLKS=0,COPW=0
#endif

//1.首先从FEI模式过渡到FBE模式

    //C2= 0x1C，因为我们的核心板上采用了外部晶振为8Mhz，属于高频率范围，
    //C2[RANGE]设置为0b01；C2[HGO]设为1以配置晶振来进行高增益操作；因为
    //正在使用的外部参考时钟源是晶振，所以C2[EREFS]设置为1。
    MCG_C2 = (MCG_C2_RANGE0(1) | MCG_C2_EREFS0_MASK);
    //C1 = 0x90 ，C1[CLKS]设置为2’b10，以便选择作为系统时钟源的外部参考时钟。
    //C1[FRDIV]设置为3'b011，对应256分频,因为8MHz/256=31.25kHz在由FLL要求的
    //31.25kHz至39.0625 kHz频率范围内。C1[IREFS]清除为0，选择外部参考时钟和
    //外部晶振。
    MCG_C1 = (MCG_C1_CLKS(2) | MCG_C1_FRDIV(3));
    //需要等到S[OSCINIT]被置位外部晶振才能使用
    for (i = 0 ; i < 20000 ; i++)
    {
        //如果S[OSCINIT]在循环结束之前被置位就跳出循环
        if (MCG_S & MCG_S_OSCINIT0_MASK) break;
    }
    //等待参考时钟状态位清0
    for (i = 0 ; i < 2000 ; i++)
    {
        //如果IREFST在循环结束之前被清0就跳出循环
        if (!(MCG_S & MCG_S_IREFST_MASK)) break;
    }
    //等待时钟状态位以显示时钟源为外部参考时钟
    for (i = 0 ; i < 2000 ; i++)
    {
        //如果CLKST显示外部时钟被选择，在循环结束之前就跳出循环
        if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break;
    }

//2.现在处于FBE状态，使能时钟监视器，由FBE直接转换为PBE模式
    MCG_C6 |= MCG_C6_CME0_MASK;
    //配置PLL为2分频
    MCG_C5 |= MCG_C5_PRDIV0(1);
    //配置MCG_C6 以设置PLL倍频因子并且时能PLL，PLLS位被置位来时能PLL，MCGOUT时钟源仍然是外部参考时钟
    temp_reg = MCG_C6; //存储当前C6的值（因为CME0位之前被置位了）
    temp_reg &= ~MCG_C6_VDIV0_MASK; //将VDIV清0
    temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0); // 重新写值到VDIV 并且时能PLL
    MCG_C6 = temp_reg; // 更新MCG_C6的值
    // 等待PLLST状态位被置
    for (i = 0 ; i < 2000 ; i++)
    {
        //如果PLLST在循环结束之前被置位就跳出循环
        if (MCG_S & MCG_S_PLLST_MASK)
        	break;
    }
    //等待LOCK位被置
    for (i = 0 ; i < 4000 ; i++)
    {
        //如果LOCK在循环结束之前被置位就跳出循环
        if (MCG_S & MCG_S_LOCK0_MASK)
        	break;
    }

//3.现在处于PBE模式。最后，PBE模式转换成PEE模式
    // 设置核心时钟分频器2分频
    //设置总线时钟分频器2分频 (总线时钟的时钟源是核心时钟)
    SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(1) );
    //清CLKS来打开CLKS多路复用器来选择PLL作为MCGCLKOUT
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    // 等待时钟状态位更新
    for (i = 0 ; i < 2000 ; i++)
    {
        //如果CLKST在循环结束之前等于3就跳出循环
        if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3) break;
    }
//4.现在处于PEE模式

    //初始化后，开始PORT模块时钟
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK
  			  | SIM_SCGC5_PORTB_MASK
  			  | SIM_SCGC5_PORTC_MASK
  			  | SIM_SCGC5_PORTD_MASK
  			  | SIM_SCGC5_PORTE_MASK;
}
