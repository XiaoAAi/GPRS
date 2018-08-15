//=====================================================================
//文件名称：isr.c
//功能概要： 中断底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-11-12   V1.0
//         2016-03-27 V2.0(XD)
//=====================================================================
#include "includes.h"


//extern uint_32 tpm_count;
//extern uint_16 oled_count;

//========================中断函数服务例程===============================
//串口0接收中断服务例程
void UART0_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    DISABLE_INTERRUPTS;    //关总中断
    if(uart_get_re_int(UART_0))
    {
        ch = uart_re1(UART_0, &flag);    //调用接收一个字节的函数，清接收中断位
        if(flag)
        {
            uart_send1(UART_0, ch);     //向原串口发回一个字节
        }
    }
    ENABLE_INTERRUPTS;
}

//串口1接收中断服务例程
void UART1_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    //用来计数
//    static uint_16 Received_count = 0;
    DISABLE_INTERRUPTS;    //关总中断
    if(uart_get_re_int(UART_1))
    {
        ch = uart_re1(UART_1, &flag);    //调用接收一个字节的函数，清接收中断位
        if(flag)
        {
            uart_send1(UART_1, ch);     //向原串口发回一个字节
        }
    }
    ENABLE_INTERRUPTS;
}

//串口2接收中断服务例程
void UART2_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    static uint_8 Received_count = 0;
    static uint_8 data_count = 0;
    DISABLE_INTERRUPTS;    //关总中断
    if(uart_get_re_int(UART_2))
    {
        ch = uart_re1(UART_2, &flag);    //调用接收一个字节的函数，清接收中断位
        if(flag)
        {
			if(ch != '\0')
			{
				Data_Buf[data_count] = ch;
				data_count++;
				if(data_count > BUF_MAX)
				{
					data_count = 0;
				}
			}

        	//用于接受服务器转发的数据
        	if(ch == 0xEE)
        	{
        		Received_count = 0;
        		//Received_flag = 0;
        	}
        	else
        	{
        		if(ch == 0xEF)
        		{
        			Received_flag = 1;
        			//直接通过GPRS进行发送（考虑到GPRS是直接和服务器相连接的，可以省掉不必要的操作）
        			//这也是KL25向服务器发送确认指令，标志服务器发送成功
//        			GPRS_Send_Data((uint_8 *)"EE");
        		}
        		else
        		{
        			Date_Re[Received_count] = ch;
        			Received_count++;
        		}

        	}

//            uart_send1(UART_2, ch);     //向原串口发回一个字节
        }
    }
    ENABLE_INTERRUPTS;
}

////TPM0中断服务例程
//void TPM2_IRQHandler(void)
//{
//	if(tpm_get_int(TPM_2) == 1)   //若有TPM0的溢出中断
//	{
//		tpm_clear_int(TPM_2);     //清TPM0的溢出中断标志
//
//	}
//}

//TPM1中断服务例程
void TPM1_IRQHandler(void)
{
	if(tpm_get_int(TPM_1) == 1)   //若有TPM1的溢出中断
	{
		tpm_clear_int(TPM_1);     //清TPM1的溢出中断标志
		//控制GPRS显示的时间
		gprs_count--;
		//每次自加1，经过了10ms
		tpm_count++ ;
		//控制OLED显示的时间
		oled_count++;
	}
}


