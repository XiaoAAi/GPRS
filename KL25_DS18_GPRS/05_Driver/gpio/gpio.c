//=====================================================================
//文件名称：gpio.c
//功能概要：GPIO底层驱动构件源文件
//=====================================================================
#include "gpio.h"   //包含本构件头文件

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                      PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//GPIO口基地址放入常数数据组GPIO_ARR[0]~GPIO_ARR[4]中
const GPIO_MemMapPtr GPIO_ARR[]={PTA_BASE_PTR,PTB_BASE_PTR,
                       PTC_BASE_PTR,PTD_BASE_PTR,PTE_BASE_PTR};

//内部函数声明
void gpio_get_port_pin(uint_16 port_pin,uint_8* port,uint_8* pin);

//=====================================================================
//函数名称：gpio_init
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//          dir：引脚方向（0=输入，1=输出,可用引脚方向宏定义）
//          state：端口引脚初始状态（0=低电平，1=高电平）
//功能概要：初始化指定端口引脚作为GPIO引脚功能，并定义为输入或输出，若是输出，
//          还指定初始状态是低电平或高电平
//=====================================================================
void gpio_init(uint_16 port_pin, uint_8 dir, uint_8 state)
{
    //局部变量声明
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针 
    GPIO_MemMapPtr gpio_ptr;    //声明port_ptr为GPIO结构体类型指针
    uint_8 port,pin;            //声明端口port、引脚pin变量
    //根据带入参数port_pin，解析出端口与引脚分别赋给port,pin
    gpio_get_port_pin(port_pin , &port , &pin);
    //根据port，给局部变量port_ptr、gpio_ptr赋值（获得两个基地址）
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    gpio_ptr = GPIO_ARR[port];  //获得GPIO模块相应口基地址

    //设定相应端口的相应引脚功能为GPIO（即令引脚控制寄存器的MUX=0b001）
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK; //置D10-D8=000
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(1);    //置D10-D8=001

    //根据带入参数dir，决定引脚为输出还是输入
    if (1 == dir)   //希望为输出
    {
         BSET(pin,GPIO_PDDR_REG(gpio_ptr));//数据方向寄存器的pin位=1，定义为输出
         gpio_set(port_pin, state);  //调用gpio_set函数，设定引脚初始状态
    }
    else           //希望为输入
    {
        BCLR(pin,GPIO_PDDR_REG(gpio_ptr));//数据方向寄存器的pin位=0，定义为输入
    }
}

//=====================================================================
//函数名称：gpio_set
//函数返回：无
//参数说明：port_pin：端口号|引脚号（如：(PTB_NUM)|(9) 表示为B口9号脚）
//       state：引脚初始状态（0=低电平，1=高电平）
//功能概要：设定引脚状态为低电平或高电平
//=====================================================================
void gpio_set(uint_16 port_pin, uint_8 state)
{
    //局部变量声明
    GPIO_MemMapPtr  gpio_ptr;  //声明port_ptr为GPIO结构体类型指针（首地址）
    uint_8 port,pin;           //声明端口port、引脚pin变量
    //根据带入参数port_pin，解析出端口与引脚分别赋给port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //根据port，给局部变量gpio_ptr赋值（GPIO基地址）
    gpio_ptr = GPIO_ARR[port]; 

    //根据带入参数state，决定引脚为输出1还是0
    if (1==state)
    {
        BSET(pin,GPIO_PDOR_REG(gpio_ptr));
    }
    else
    {
        BCLR(pin,GPIO_PDOR_REG(gpio_ptr));
    }
}

//=====================================================================
//函数名称：gpio_get
//函数返回：指定引脚的状态（1或0）
//参数说明：port_pin：端口号|引脚号（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：获取指定引脚的状态（1或0）
//=====================================================================
uint_8 gpio_get(uint_16 port_pin)
{
    //局部变量声明
    GPIO_MemMapPtr  gpio_ptr;  //声明port_ptr为GPIO结构体类型指针（首地址）
    uint_8 port,pin;           //声明端口port、引脚pin变量
    //根据带入参数port_pin，解析出端口与引脚分别赋给port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //根据port，给局部变量gpio_ptr赋值（GPIO基地址）
    gpio_ptr = GPIO_ARR[port];

    //返回引脚的状态
    return ((BGET(pin,GPIO_PDIR_REG(gpio_ptr)))>=1 ? 1:0);
}

//=====================================================================
//函数名称：gpio_reverse
//函数返回：无
//参数说明：port_pin：端口号|引脚号（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：反转指定引脚输出状态。
//=====================================================================
void gpio_reverse(uint_16 port_pin)
{
    //局部变量声明
    GPIO_MemMapPtr  gpio_ptr;  //声明port_ptr为GPIO结构体类型指针（首地址）
    uint_8 port,pin;           //声明端口port、引脚pin变量
    //根据带入参数port_pin，解析出端口与引脚分别赋给port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //根据port，给局部变量gpio_ptr赋值（GPIO基地址）
    gpio_ptr = GPIO_ARR[port]; 

    //反转指定引脚输出状态
    BSET(pin,GPIO_PTOR_REG(gpio_ptr));
}

//=====================================================================
//函数名称：gpio_pull
//函数返回：无
//参数说明：port_pin：端口号|引脚号（如：(PTB_NUM)|(9) 表示为B口9号脚）
//        pullselect：引脚拉高低电平（ 0=拉低电平，1=拉高电平）
//功能概要：使指定引脚上拉高电平或下拉低电平
//=====================================================================
void gpio_pull(uint_16 port_pin, uint_8 pullselect)
{
    //局部变量声明
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint_8 port,pin;           //声明端口port、引脚pin变量
    //根据带入参数port_pin，解析出端口与引脚分别赋给port,pin
    gpio_get_port_pin(port_pin , &port , &pin);

    //根据port，给局部变量port_ptr赋值（获得基地址）
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址

    //设定相应端口的相应引脚功能为GPIO（即令引脚控制寄存器的MUX=0b001）
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK; //置D10-D8=000
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(1);    //置D10-D8=001

    //设定正常驱动能力（低驱动能力）（5mA）
    BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    //将引脚上下拉使能
    BSET(PORT_PCR_PE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    //根据带入参数pullselect，决定引脚拉高还是拉低
    if (1==pullselect) 
    {
        BSET(PORT_PCR_PS_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
    else
    {
        BCLR(PORT_PCR_PS_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
}

//=====================================================================
//函数名称：gpio_enable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//       irqtype：引脚中断类型，由宏定义给出，再次列举如下：
//                LOW_LEVEL    8      //低电平触发
//                HIGH_LEVEL   12     //高电平触发
//                RISING_EDGE  9      //上升沿触发
//                FALLING_EDGE 10     //下降沿触发
//                DOUBLE_EDGE  11     //双边沿触发
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数开启引脚中断，并
//        设置中断触发条件。
//注          意： KL25芯片，只有PORTA、PORTD口具有GPIO类中断功能
//          KW01芯片，只有PORTA、PORTC、PORTD口具有GPIO类中断功能
//=====================================================================
void gpio_enable_int(uint_16 port_pin,uint_8 irqtype)
{
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址

    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_ISF_MASK;  //清除引脚中断标志
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_IRQC(irqtype);//使能引脚中断

    switch(port)
    {
    case 0:   //PTA
        NVIC_EnableIRQ(PORTA_IRQn);   //开中断控制器IRQ中断
        break;
    case 3:   //PTD
        NVIC_EnableIRQ(PORTD_IRQn);  //开中断控制器IRQ中断
        break;
    default:;
    }
}

//=====================================================================
//函数名称：gpio_disable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数关闭引脚中断
//注          意： KL25芯片，只有PORTA、PORTD口具有GPIO类中断功能
//           KW01芯片，只有PORTA、PORTC、PORTD口具有GPIO类中断功能
//=====================================================================
void gpio_disable_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin, &port, &pin);

    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_IRQC_MASK;//禁用引脚中断
    
    switch(port)
    {
    case 0://PTA
        NVIC_DisableIRQ(PORTA_IRQn);   //关中断控制器IRQ中断
        break;
    case 3://PTD
        NVIC_DisableIRQ(PORTD_IRQn);   //关中断控制器IRQ中断
        break;
    default:;
    }
}

//=====================================================================
//函数名称：gpio_get_int
//函数返回：引脚GPIO中断标志（1或0）1表示引脚有GPIO中断，0表示引脚没有GPIO中断。
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时,获取中断标志。
//注          意： KL25芯片，只有PORTA、PORTD口具有GPIO类中断功能
//           KW01芯片，只有PORTA、PORTC、PORTD口具有GPIO类中断功能
//=====================================================================
uint_8 gpio_get_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址

    return ((BGET(PORT_PCR_ISF_SHIFT,PORT_PCR_REG(port_ptr, pin)))>=1 ? 1:0);
}

//=====================================================================
//函数名称：gpio_clear_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时,清除中断标志。
//注          意： KL25芯片，只有PORTA、PORTD口具有GPIO类中断功能
//           KW01芯片，只有PORTA、PORTC、PORTD口具有GPIO类中断功能
//=====================================================================
void gpio_clear_int(uint_16 port_pin)
{
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_ISF_MASK;  //清除引脚中断标志
}

//=====================================================================
//函数名称：gpio_clear_allint
//函数返回：无
//参数说明：无
//功能概要：清除所有端口的GPIO中断
//注          意： KL25芯片，只有PORTA、PORTD口具有GPIO类中断功能
//           KW01芯片，只有PORTA、PORTC、PORTD口具有GPIO类中断功能
//=====================================================================
void gpio_clear_allint(void)
{
    uint_8 i;
    for(i=0;i<5;i++)
    {
        PORT_ISFR_REG(PORT_ARR[i])=PORT_ISFR_ISF_MASK;
    }
}

//=====================================================================
//函数名称：gpio_drive_strength
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//       control：控制引脚的驱动能力，1=高驱动，0=正常驱动
//         （引脚被配置为数字输出时，引脚控制寄存器的DSE=1：高驱动， DSE=0：正常驱动）
//功能概要：（引脚驱动能力：指引脚输入或输出电流的承受力，一般用mA单位度量
//            ，正常驱动能力5mA,高驱动能力18mA。）当引脚被配置为数字输出时,
//            对引脚的驱动能力进行设置，只有PTB0,PTB1,PTD6,PTD7同时具有高驱
//            动能力和正常驱动能力,这些引脚可用于直接驱动LED或给MOSFET（金氧
//            半场效晶体管）供电，该函数只适用于上述4个引脚。
//=====================================================================
void gpio_drive_strength(uint_16 port_pin, uint_8 control)
{
    //局部变量声明
    PORT_MemMapPtr port_ptr;    //声明port_ptr为PORT结构体类型指针
    uint_8 port;
    uint_8 pin;
    gpio_get_port_pin(port_pin , &port , &pin);

    //根据带入参数port，给局部变量port_ptr赋值
    port_ptr = PORT_ARR[port];

    //根据带入参数control，决定引脚为输出高电流还是正常电流
    BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    if (1 == control)
    {
        BSET(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
    else
    {
        BCLR(PORT_PCR_DSE_SHIFT,PORT_PCR_REG(port_ptr, pin));
    }
}

//----------------------以下为内部函数存放处-----------------
//=====================================================================
//函数名称：gpio_get_port_pin
//函数返回：无
//参数说明：port_pin：端口号|引脚号（如：(PTB_NUM)|(9) 表示为B口9号脚）
//       port：端口号（传指带出参数）
//       pin:引脚号（0~31，实际取值由芯片的物理引脚决定）（传指带出参数）
//功能概要：将传进参数port_pin进行解析，得出具体端口号与引脚号，分别赋值给port与pin,返回。
//       （例：(PTB_NUM)|(9)解析为PORTB与9，并将其分别赋值给port与pin）。
//=====================================================================
void gpio_get_port_pin(uint_16 port_pin,uint_8* port,uint_8* pin)
{
    *port = (port_pin>>8);
    *pin = port_pin;
}
//----------------------------内部函数结束--------------------

