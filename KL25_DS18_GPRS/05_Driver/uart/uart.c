//======================================================================
//文件名称：uart.c
//功能概要：uart底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2015-7-14 V1.0
//======================================================================
#include "uart.h"

//=====串口1、2号地址映射====
static const UART_MemMapPtr UART_ARR[] = {UART1_BASE_PTR, UART2_BASE_PTR};
//====定义串口IRQ号对应表====
static IRQn_Type table_irq_uart[3] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};

//内部函数声明
uint_8 uart_is_uartNo(uint_8 uartNo);

//======================================================================
//函数名称：uart_init
//功能概要：初始化uart模块                                                
//参数说明：uartNo:串口号：UART_0、UART_1、UART_2 
//          baud:波特率：300、600、1200、2400、4800、9600、19200、115200...
//函数返回：无
//======================================================================
void uart_init(uint_8 uartNo, uint_32 baud_rate)
{
    //局部变量声明
    uint_16 sbr;
    uint_8 temp;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR;//uartch_0为UART0_MemMapPtr类型指针
    UART_MemMapPtr uartch_1_2;    //uartch_1_2为UART_MemMapPtr类型指针

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    //根据带入参数uartNo，给局部变量uartch_0或uartch_1_2赋值
    if(uartNo==0)
    {
        //UART0选择MCGFLLCLK_khz=48000Khz时钟源
        SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);
        SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
#ifdef UART_0_GROUP
        //依据选择配置对应引脚为UART0
        switch(UART_0_GROUP)
        {
        case 0:
            PORTE_PCR20 |= PORT_PCR_MUX(0x4);    //使能UART0_TXD
            PORTE_PCR21 |= PORT_PCR_MUX(0x4);    //使能UART0_RXD
            break;
        case 1:
            PORTA_PCR2 |= PORT_PCR_MUX(0x2);     //使能UART0_TXD
            PORTA_PCR1 |= PORT_PCR_MUX(0x2);     //使能UART0_RXD
            break;
        case 2:
            PORTA_PCR14 |= PORT_PCR_MUX(0x3);    //使能UART0_TXD
            PORTA_PCR15 |= PORT_PCR_MUX(0x3);    //使能UART0_RXD
            break;
        case 3:
            PORTB_PCR17 |= PORT_PCR_MUX(0x3);    //使能UART0_TXD
            PORTB_PCR16 |= PORT_PCR_MUX(0x3);    //使能UART0_RXD
            break;
        case 4:
        default:
            PORTD_PCR7 |= PORT_PCR_MUX(0x3);     //使能UART0_TXD
            PORTD_PCR6 |= PORT_PCR_MUX(0x3);     //使能UART0_RXD
            break;
        }
#endif
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;    //启动串口0时钟

        //暂时关闭串口0发送与接收功能
        UART0_C2_REG(uartch_0) &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

        //配置串口工作模式:8位无校验模式
        sbr = (uint_16)((SYSTEM_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART0_BDH_REG(uartch_0) & ~(UART0_BDH_SBR(0x1F));
        UART0_BDH_REG(uartch_0) = temp | UART0_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART0_BDL_REG(uartch_0) = (uint_8)(sbr & UART0_BDL_SBR_MASK);

        //初始化控制寄存器、清标志位
        UART0_C4_REG(uartch_0) = 0x0F;
        UART0_C1_REG(uartch_0) = 0x00;
        UART0_C3_REG(uartch_0) = 0x00;
        UART0_MA1_REG(uartch_0) = 0x00;
        UART0_MA2_REG(uartch_0) = 0x00;
        UART0_S1_REG(uartch_0) |= 0x1F;
        UART0_S2_REG(uartch_0) |= 0xC0;
        //禁用串口发送中断
        UART0_C2_REG(uartch_0) &= ~UART0_C2_TIE_MASK;
        //启动发送接收
        UART0_C2_REG(uartch_0) |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
    }
    else
    {
        switch (uartNo)
        {
        case UART_1:
#ifdef UART_1_GROUP
            switch(UART_1_GROUP)
            {
            case 0:
                PORTE_PCR0 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                PORTE_PCR1 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                break;
            case 1:
                PORTA_PCR14 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                PORTA_PCR15 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                break;
            case 2:
            default:
                PORTC_PCR4 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
                PORTC_PCR5 = PORT_PCR_MUX(0x3);    //使能UART1_RXD
                break;
            }
#endif
            SIM_SCGC4 |= SIM_SCGC4_UART1_MASK; //启动串口1时钟
            break;
        case UART_2:
#ifdef UART_2_GROUP
            switch(UART_2_GROUP)
            {
            case 0:
                PORTE_PCR22 = PORT_PCR_MUX(0x4); //使能UART2_TXD
                PORTE_PCR23 = PORT_PCR_MUX(0x4); //使能UART2_RXD
                break;
            case 1:
                PORTD_PCR3 = PORT_PCR_MUX(0x3); //使能UART2_TXD
                PORTD_PCR2 = PORT_PCR_MUX(0x3); //使能UART2_RXD
                break;
            case 2:
            default:
                PORTD_PCR5 = PORT_PCR_MUX(0x3); //使能UART2_TXD
                PORTD_PCR6 = PORT_PCR_MUX(0x3); //使能UART2_RXD
                break;
            }
#endif
            SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;//启动串口2时钟
            break;
        default:
            break;  //传参错误，返回
        }
        uartch_1_2 = UART_ARR[uartNo-1]; //获得UART1、2模块相应口基地址

        //暂时关闭串口1、2发送与接收功能
        UART_C2_REG(uartch_1_2) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

        //配置波特率,KL25串口1、2时钟频率使用 Bus clock = 24M 总线时钟
        //配置串口工作模式,8位无校验模式
        sbr = (uint_16)((BUS_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART_BDH_REG(uartch_1_2) & ~(UART_BDH_SBR(0x1F));
        UART_BDH_REG(uartch_1_2) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART_BDL_REG(uartch_1_2) = (uint_8)(sbr & UART_BDL_SBR_MASK);

        //初始化控制寄存器、清标志位
        UART_C1_REG(uartch_1_2)=0x00;
        UART_C3_REG(uartch_1_2)=0x00;
        UART_S2_REG(uartch_1_2)=0x00;
        //禁用串口发送中断
        UART_C2_REG(uartch_1_2) &= ~UART_C2_TIE_MASK;
        //启动发送接收
        UART_C2_REG(uartch_1_2)|= (UART_C2_TE_MASK | UART_C2_RE_MASK);
    }
}
//======================================================================
//函数名称：uart_send1
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//          ch:要发送的字节
//函数返回：函数执行状态：1=发送成功；0=发送失败。
//功能概要：串行发送1个字节
//======================================================================
uint_8 uart_send1(uint_8 uartNo, uint_8 ch)
{
    uint_32 t;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//查询指定次数
    {
        if(0==uartNo)     //判断使用的哪个串口
        {
        	//发送缓冲区为空则发送数据
            if ( UART0_S1_REG(uartch_0) & UART0_S1_TDRE_MASK )
            {
                UART0_D_REG(uartch_0) = ch;
                break;
            }
        }
        else
        {
        	//发送缓冲区为空则发送数据
            if (UART_S1_REG(uartch_1_2) & UART_S1_TDRE_MASK )
            {
                UART_D_REG(uartch_1_2) = ch;
                break;
            }
        }
    }//end for
    if (t >= 0xFBBB)
        return 0; //发送超时，发送失败
    else
        return 1; //发送成功
}

//======================================================================
//函数名称：uart_sendN
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//         buff: 发送缓冲区
//         len:发送长度
//函数返回： 函数执行状态：1=发送成功；0=发送失败
//功能概要：串行 接收n个字节   
//======================================================================
uint_8 uart_sendN(uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (i = 0; i < len; i++)
    {
        if (! uart_send1(uartNo, buff[i])) //发送一个字节数据，失败则跳出循环
        {
            break;
        }
    }
    if(i<len)
        return 0;    //发送出错
    else
        return 1;    //发送成功
}

//======================================================================
//函数名称：uart_send_string
//参数说明：uartNo:UART模块号:UART_0、UART_1、UART_2 
//          buff:要发送的字符串的首地址
//函数返回： 函数执行状态：1=发送成功；0=发送失败
//功能概要：从指定UART端口发送一个以'\0'结束的字符串
//======================================================================
uint_8 uart_send_string(uint_8 uartNo, void *buff)
{
    uint_16 i = 0;
    uint_8 *buff_ptr = (uint_8 *)buff;     //定义指针指向要发送字符串首地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for(i = 0; buff_ptr[i] != '\0'; i++)   //遍历字符串里的字符
    {
        if (!uart_send1(uartNo,buff_ptr[i]))//发送指针对应的字符
            return 0;  //发送失败,返回
    }
    return 1;          //发送成功
}

//======================================================================
//函数名称：uart_re1
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//        *fp:接收成功标志的指针:*fp=1:接收成功；*fp=0:接收失败
//函数返回：接收返回字节
//功能概要：串行接收1个字节
//======================================================================
uint_8 uart_re1(uint_8 uartNo,uint_8 *fp)
{
    uint_32 t;
    uint_8  dat;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        *fp=0;
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//查询指定次数
    {
        if(0==uartNo)  //判断使用的哪个串口
        {
        //判断接收缓冲区是否满
        if (UART0_S1_REG(uartch_0) & UART0_S1_RDRF_MASK)
            {
                dat=UART0_D_REG(uartch_0);    //获取数据,清接收中断位
                *fp = 1;  //接收成功
                break;
            }
        }
        else
        {
            //判断接收缓冲区是否满
            if(UART_S1_REG(uartch_1_2) & UART_S1_RDRF_MASK)
            {
                dat=UART_D_REG(uartch_1_2);    //获取数据,清接收中断位
                *fp= 1;  //接收成功
                break;
            }
        }
    }//end for
    if(t >= 0xFBBB) 
    {
        dat = 0xFF; 
        *fp = 0;    //未收到数据
    }
    return dat;    //返回接收到的数据
}

//======================================================================
//函数名称：uart_reN
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2                                                   
//          buff: 接收缓冲区
//          len:接收长度
//函数返回：函数执行状态 1=接收成功;0=接收失败
//功能概要：串行 接收n个字节,放入buff中
//======================================================================
uint_8 uart_reN(uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;
    uint_8 flag = 0;

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    //判断是否能接受数据
    for (i = 0; i < len && flag==1; i++)
    {
        buff[i] = uart_re1(uartNo, &flag); //接受数据
    }
    if (i < len)
        return 0; //接收失败
    else
        return 1; //接收成功
}

//======================================================================
//函数名称：uart_enable_re_int
//参数说明：uartNo: 串口号:UART_0、UART_1、UART_2 
//函数返回：无
//功能概要：开串口接收中断
//======================================================================
void uart_enable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0 == uartNo)
        UART0_C2_REG(uartch_0) |= UART0_C2_RIE_MASK;    //开放UART接收中断
    else 
        UART_C2_REG(uartch_1_2) |= UART_C2_RIE_MASK;    //开放UART接收中断
    NVIC_EnableIRQ(table_irq_uart[uartNo]);    //开中断控制器IRQ中断
}

//======================================================================
//函数名称：uart_disable_re_int
//参数说明：uartNo: 串口号 :UART_0、UART_1、UART_2 
//函数返回：无
//功能概要：关串口接收中断
//======================================================================
void uart_disable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch_0 = UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0==uartNo)
        UART0_C2_REG(uartch_0) &= ~UART0_C2_RIE_MASK;    //禁止UART接收中断
    else
        UART_C2_REG(uartch_1_2) &= ~UART_C2_RIE_MASK;    //禁止UART接收中断
    NVIC_DisableIRQ(table_irq_uart[uartNo]);
}

//======================================================================
//函数名称：uart_get_re_int
//参数说明：uartNo: 串口号 :UART_0、UART_1、UART_2
//函数返回：接收中断标志 1=有接收中断;0=无接收中断
//功能概要：获取串口接收中断标志,同时禁用发送中断
//======================================================================
uint_8 uart_get_re_int(uint_8 uartNo)
{
    uint_8 flag;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //获取UART0基地址
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //获取UART1或者2基地址

    //判断传入串口号参数是否有误，有误直接退出
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    if(0==uartNo)
    {
        //禁用串口发送中断，防止误中断
        UART0_C2_REG(uartch_0) &= (~UART0_C2_TIE_MASK);
        //获取接收中断标志，需同时判断RDRF和RIE
        flag= UART0_S1_REG(uartch_0) & (UART0_S1_RDRF_MASK);
        return(BGET(UART0_S1_RDRF_SHIFT,flag)
                & BGET(UART0_C2_RIE_SHIFT,UART0_C2_REG(uartch_0)));
    }
    else
    {
        //禁用串口发送中断，防止误中断
        UART_C2_REG(uartch_1_2) &= (~UART_C2_TIE_MASK);
        //获取接收中断标志，需同时判断RDRF和RIE
        flag= UART_S1_REG(uartch_1_2) & (UART_S1_RDRF_MASK);
        return(BGET(UART_S1_RDRF_SHIFT,flag)
                & BGET(UART_C2_RIE_SHIFT,UART_C2_REG(uartch_1_2)));
    }
}

//----------------------以下为内部函数存放处--------------------------------------
//=====================================================================
//函数名称：uart_is_uartNo
//函数返回：1:串口号在合理范围内，0：串口号不合理
//参数说明：串口号uartNo  :UART_0、UART_1、UART_2
//功能概要：为程序健壮性而判断uartNo是否在串口数字范围内
//=====================================================================
uint_8 uart_is_uartNo(uint_8 uartNo)
{
    if(uartNo < UART_0 || uartNo > UART_2)
        return 0;
    else
        return 1;
}
//----------------------------内部函数结束--------------------------------------
