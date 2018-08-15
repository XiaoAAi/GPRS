//=====================================================================
//文件名称：tpm.c
//功能概要：tpm底层驱动构件源文件
//=====================================================================
#include "tpm.h"

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//定时器模块0,1,2地址映射
TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

static uint_8 tpm_mux_val(uint_16 tpmx_Chy,uint_8* TPM_i,uint_8* chl);

//===================================================================
//函数名称：tpm_timer_init
//功能概要：tpm模块初始化，设置计数器频率f及计数器溢出时间MOD_Value。
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//       f：单位：Khz，取值：375、750、1500、3000、6000、12000、24000、48000
//       MOD_Value：单位：ms，范围取决于计数器频率与计数器位数（16位）
//函数返回：无
//===================================================================
void tpm_timer_init(uint_16 TPM_i,uint32_t f,float MOD_Value)
{
    //局部变量声明
    uint_32 clk_f,clk_div;
    //1. 开启SIM时钟门
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //2.开启时钟，默认选择用PLL/2时钟源，即48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
    //3. 由期望的时钟频率f，计算分频因子clk_div。因分频系数clk_f=48000/f,
    //则分频因子clk_div=sqrt(clk_f)。例如：f=3000Khz,则clk_f=16，clk_div=4
    clk_f=48000/f;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //4. 写TPM_i的状态和控制寄存器
    TPM_ARR[TPM_i]->SC |= TPM_SC_TOIE_MASK|TPM_SC_CMOD(1)| \
                          TPM_SC_PS(clk_div);
    //5. 计数器清零
    TPM_ARR[TPM_i]->CNT = 0;
    //6. 设置模数寄存器
    TPM_ARR[TPM_i]->MOD = f*MOD_Value;  //给模数寄存器赋值
}
//===================================================================
//函数名称：pwm_init
//功能概要：pwm模块初始化。
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//       duty：占空比：0.0~100.0对应0%~100%
//         Align：PWM计数对齐方式（有宏定义常数可用）
//       pol：PWM极性选择（有宏定义常数可用）
//函数返回：无
//===================================================================
void pwm_init(uint_16 tpmx_Chy,float duty,uint_8 Align,uint_8 pol)
{
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint_8 port,pin;           //解析出的端口、引脚号临时变量
    uint_8 mux_val;
    uint_8 TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint_32 period;
    // 防止越界
    if(duty>100.0) duty=100.0;
    //1. gpio引脚解析
    port = (tpmx_Chy>>8);    //解析出的端口
    pin = tpmx_Chy;          //解析出的引脚号
    //2. 根据port，给局部变量port_ptr赋值,获得基地址
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    //3. 取得引脚复用值，并获得解析的tpm模块号和通道号
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. 根据pin,指定该引脚功能为TPM的通道功能（即令引脚控制寄存器的MUX=mux_val）
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. PWM对齐方式的设定
    //有错误，，，，，，这个是计数方式，不是PWM的对其方式的设置
    //误解！！！见P555 KL25参考手册
    if(Align == PWM_CENTER)     //中心对齐
        TPM_ARR[TPM_i]->SC |= TPM_SC_CPWMS_MASK;
    else                            //边沿对齐
        TPM_ARR[TPM_i]->SC &= ~TPM_SC_CPWMS_MASK;
    //6. PWM极性的设定
    if(pol == PWM_PLUS)         //正极性
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                         TPM_CnSC_ELSB_MASK;
    else                            //负极性
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                         TPM_CnSC_ELSA_MASK;
    //7. PWM占空比的设定
    period=TPM_ARR[TPM_i]->MOD;    //计算周期（period）
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint_32)(period*duty/100);
}

//=====================================================================
//函数名称：pwm_update
//功能概要：tpmx模块Chy通道的PWM更新
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块0通道）
//       duty：占空比：0.0~100.0对应0%~100%
//函数返回：无
//=====================================================================
void pwm_update(uint_16 tpmx_Chy,float duty)
{
    uint_8 TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint_32 period;
    // 防止越界
    if(duty>100.0)  duty=100.0;
    //1. 取得引脚复用值，并获得解析的tpm模块号和通道号
    tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //2. 更新PWM通道寄存器值
    period=TPM_ARR[TPM_i]->MOD;
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint32_t)period*duty/100;
}

//===================================================================
//函数名称：incap_init
//功能概要：incap模块初始化。
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//       capmode：输入捕捉模式（上升沿、下降沿、双边沿），有宏定义常数使用
//函数返回：无
//===================================================================
void incap_init(uint_16 tpmx_Chy,uint_8 capmode)
{
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint_8 port,pin;           //解析出的端口、引脚号临时变量
    uint_8 TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint_8 mux_val;

    //1. gpio引脚解析
    port = (tpmx_Chy>>8);    //解析出的端口
    pin = tpmx_Chy;          //解析出的引脚号
    //2. 根据port，给局部变量port_ptr赋值,获得基地址
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    //3. 取得引脚复用值，并获得解析的tpm模块号和通道号
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. 根据pin,指定该引脚功能为TPM的通道功能（即令引脚控制寄存器的MUX=mux_val）
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. 输入捕捉参数设定
    if(capmode == CAP_UP)           //上升沿捕捉
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
    else if(capmode == CAP_DOWN)    //下降沿捕捉
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    else if(capmode == CAP_DOUBLE)  //双边沿捕捉
    {
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    }
    //6. chl通道中断使能
    TPM_CnSC_REG(TPM_ARR[TPM_i],chl) |= TPM_CnSC_CHIE_MASK;
}

//=====================================================================
//函数名称：tpm_get_capvalue
//功能概要：获取tpmx模块Chy通道的计数器当前值
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//函数返回：tpmx模块Chy通道的计数器当前值
//=====================================================================
uint_16 tpm_get_capvalue(uint_16 tpmx_Chy)
{
    uint_8 TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint_16 cnt;
    tpm_mux_val(tpmx_Chy,&TPM_i,&chl);   //解析tpm模块号和通道号
    cnt=TPM_CnV_REG(TPM_ARR[TPM_i],chl);
    return cnt;

}

//===================================================================
//函数名称：outcompare_init
//功能概要：outcompare模块初始化。
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//        comduty：输出比较电平翻转位置占总周期的比例：0.0~100.0对应0%~100%
//        cmpmode：输出比较模式（翻转电平、强制低电平、强制高电平）,有宏定义常数使用
//函数返回：无
//===================================================================
void outcompare_init(uint_16 tpmx_Chy,float comduty,uint_8 cmpmode)
{
    PORT_MemMapPtr port_ptr;   //声明port_ptr为PORT结构体类型指针
    uint_8 port,pin;           //解析出的端口、引脚号临时变量
    uint_8 mux_val;
    uint_8 TPM_i,chl;  //由tpmx_Chy解析出的tpm模块号、通道号临时变量
    uint_32 mod;

    if(comduty>100.0)  comduty=100.0;

    //1. gpio引脚解析
    port = (tpmx_Chy>>8);    //解析出的端口
    pin = tpmx_Chy;          //解析出的引脚号
    //2. 根据port，给局部变量port_ptr赋值,获得基地址
    port_ptr = PORT_ARR[port];  //获得PORT模块相应口基地址
    //3. 取得引脚复用值，并获得解析的tpm模块号和通道号
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. 根据pin,指定该引脚功能为TPM的通道功能（即令引脚控制寄存器的MUX=mux_val）
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. 输出比较模式的设定
    if(cmpmode == CMP_REV)        //翻转模式
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
                                       TPM_CnSC_ELSA_MASK;
    else if(cmpmode == CMP_LOW)   //强制低电平模式
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
             TPM_CnSC_ELSB_MASK;
    else if(cmpmode == CMP_HIGH)  //强制高电平模式
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
                      TPM_CnSC_ELSA_MASK|TPM_CnSC_ELSB_MASK;
    //6. 输出比较占空比的设定
    mod=TPM_ARR[TPM_i]->MOD;   //读取模数寄存器的值（MOD）
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint32_t)mod*(comduty/100);
}

//===================================================================
//函数名称：tpm_enable_int
//功能概要：使能tpm模块中断。
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//函数返回：无
//===================================================================
void tpm_enable_int(uint_8 TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //开TPM中断
    NVIC_EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//函数名称：tpm_disable_int
//功能概要：禁用tpm模块中断。
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//函数返回：无
//===================================================================
void tpm_disable_int(uint_8 TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //开TPM中断
    NVIC_DisableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//函数名称：tpm_get_int
//功能概要：获取TPM模块中断标志
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint_8 tpm_get_int(uint_8 TPM_i)
{
  //获取TPM_i模块中断标志位
  if(((TPM_SC_REG(TPM_ARR[TPM_i]) & TPM_SC_TOF_MASK)==TPM_SC_TOF_MASK))
    return 1;
  else
    return 0;
}

//===================================================================
//函数名称：tpm_chl_get_int
//功能概要：获取TPM通道中断标志
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2，TPMC_i：0-5通道
//函数返回：中断标志 1=有中断产生;0=无中断产生
//===================================================================
uint_8 tpm_chl_get_int(uint_8 TPM_i,uint_8 TPMC_i)
{
   //获取TPM_i模块TPMC_i通道中断标志位
   if(((TPM_CnSC_REG(TPM_ARR[TPM_i],TPMC_i)&TPM_CnSC_CHF_MASK)\
                      ==TPM_CnSC_CHF_MASK))
       return 1;
   else
       return 0;
}

//===================================================================
//函数名称：tpm_clear_int
//功能概要：清除TPM中断标志
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2
//函数返回：清除中断标志位
//===================================================================
void tpm_clear_int(uint_8 TPM_i)
{
    //清除TPM_i模块中断标志位
    BSET(TPM_SC_TOF_SHIFT,TPM_SC_REG(TPM_ARR[TPM_i]));
}

//===================================================================
//函数名称：tpm_clear_chl_int
//功能概要：清除TPM通道中断标志
//参数说明：TPM_i：模块号，使用宏定义：TPM_0、TPM_1、TPM_2，TPMC_i：0-5通道
//函数返回：清除TPM通道中断标志位
//===================================================================
void tpm_clear_chl_int(uint_8 TPM_i,uint_8 TPMC_i)
{
    //清除TPM_i模块TPMC_i通道中断标志位
    BSET(TPM_CnSC_CHF_SHIFT,TPM_CnSC_REG(TPM_ARR[TPM_i],TPMC_i));
}

//------以下为内部函数------
//=====================================================================
//函数名称：tpm_mux_val
//功能概要：将传进参数tpmx_Chy进行解析，得出具体模块号与通道号（例：TPM_CH0
//        解析出PORT引脚，并根据引脚返回mux_val）。
//参数说明：tpmx_Chy：模块通道号（例：TPM_CH0表示为TPM0模块第0通道）
//
//函数返回：gpio复用寄存器传入值
//=====================================================================
static uint_8 tpm_mux_val(uint_16 tpmx_Chy,uint_8* TPM_i,uint_8* chl)
{
    uint_8 port,pin;
    //1.解析模块号和通道号
    switch(tpmx_Chy)
    {
        case TPM0_CH0:*TPM_i =0;*chl=0;break;
        case TPM0_CH1:*TPM_i =0;*chl=1;break;
        case TPM0_CH2:*TPM_i =0;*chl=2;break;
        case TPM0_CH3:*TPM_i =0;*chl=3;break;
        case TPM0_CH4:*TPM_i =0;*chl=4;break;
        case TPM0_CH5:*TPM_i =0;*chl=5;break;
        case TPM1_CH0:*TPM_i =1;*chl=0;break;
        case TPM1_CH1:*TPM_i =1;*chl=1;break;
        case TPM2_CH0:*TPM_i =2;*chl=0;break;
        case TPM2_CH1:*TPM_i =2;*chl=1;break;
    }
    //2.解析引脚复用寄存器传入值
    port = (tpmx_Chy>>8);
    pin = tpmx_Chy;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}
