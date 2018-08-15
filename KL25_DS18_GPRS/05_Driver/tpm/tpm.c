//=====================================================================
//�ļ����ƣ�tpm.c
//���ܸ�Ҫ��tpm�ײ���������Դ�ļ�
//=====================================================================
#include "tpm.h"

//���˿ڻ���ַ���볣��������PORT_ARR[0]~PORT_ARR[4]��
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//��ʱ��ģ��0,1,2��ַӳ��
TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

static uint_8 tpm_mux_val(uint_16 tpmx_Chy,uint_8* TPM_i,uint_8* chl);

//===================================================================
//�������ƣ�tpm_timer_init
//���ܸ�Ҫ��tpmģ���ʼ�������ü�����Ƶ��f�����������ʱ��MOD_Value��
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//       f����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//       MOD_Value����λ��ms����Χȡ���ڼ�����Ƶ���������λ����16λ��
//�������أ���
//===================================================================
void tpm_timer_init(uint_16 TPM_i,uint32_t f,float MOD_Value)
{
    //�ֲ���������
    uint_32 clk_f,clk_div;
    //1. ����SIMʱ����
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //2.����ʱ�ӣ�Ĭ��ѡ����PLL/2ʱ��Դ����48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
    //3. ��������ʱ��Ƶ��f�������Ƶ����clk_div�����Ƶϵ��clk_f=48000/f,
    //���Ƶ����clk_div=sqrt(clk_f)�����磺f=3000Khz,��clk_f=16��clk_div=4
    clk_f=48000/f;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //4. дTPM_i��״̬�Ϳ��ƼĴ���
    TPM_ARR[TPM_i]->SC |= TPM_SC_TOIE_MASK|TPM_SC_CMOD(1)| \
                          TPM_SC_PS(clk_div);
    //5. ����������
    TPM_ARR[TPM_i]->CNT = 0;
    //6. ����ģ���Ĵ���
    TPM_ARR[TPM_i]->MOD = f*MOD_Value;  //��ģ���Ĵ�����ֵ
}
//===================================================================
//�������ƣ�pwm_init
//���ܸ�Ҫ��pwmģ���ʼ����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//       duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//         Align��PWM�������뷽ʽ���к궨�峣�����ã�
//       pol��PWM����ѡ���к궨�峣�����ã�
//�������أ���
//===================================================================
void pwm_init(uint_16 tpmx_Chy,float duty,uint_8 Align,uint_8 pol)
{
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port,pin;           //�������Ķ˿ڡ����ź���ʱ����
    uint_8 mux_val;
    uint_8 TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint_32 period;
    // ��ֹԽ��
    if(duty>100.0) duty=100.0;
    //1. gpio���Ž���
    port = (tpmx_Chy>>8);    //�������Ķ˿�
    pin = tpmx_Chy;          //�����������ź�
    //2. ����port�����ֲ�����port_ptr��ֵ,��û���ַ
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    //3. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. ����pin,ָ�������Ź���ΪTPM��ͨ�����ܣ��������ſ��ƼĴ�����MUX=mux_val��
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. PWM���뷽ʽ���趨
    //�д��󣬣�������������Ǽ�����ʽ������PWM�Ķ��䷽ʽ������
    //��⣡������P555 KL25�ο��ֲ�
    if(Align == PWM_CENTER)     //���Ķ���
        TPM_ARR[TPM_i]->SC |= TPM_SC_CPWMS_MASK;
    else                            //���ض���
        TPM_ARR[TPM_i]->SC &= ~TPM_SC_CPWMS_MASK;
    //6. PWM���Ե��趨
    if(pol == PWM_PLUS)         //������
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                         TPM_CnSC_ELSB_MASK;
    else                            //������
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                         TPM_CnSC_ELSA_MASK;
    //7. PWMռ�ձȵ��趨
    period=TPM_ARR[TPM_i]->MOD;    //�������ڣ�period��
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint_32)(period*duty/100);
}

//=====================================================================
//�������ƣ�pwm_update
//���ܸ�Ҫ��tpmxģ��Chyͨ����PWM����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ��0ͨ����
//       duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//�������أ���
//=====================================================================
void pwm_update(uint_16 tpmx_Chy,float duty)
{
    uint_8 TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint_32 period;
    // ��ֹԽ��
    if(duty>100.0)  duty=100.0;
    //1. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //2. ����PWMͨ���Ĵ���ֵ
    period=TPM_ARR[TPM_i]->MOD;
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint32_t)period*duty/100;
}

//===================================================================
//�������ƣ�incap_init
//���ܸ�Ҫ��incapģ���ʼ����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//       capmode�����벶׽ģʽ�������ء��½��ء�˫���أ����к궨�峣��ʹ��
//�������أ���
//===================================================================
void incap_init(uint_16 tpmx_Chy,uint_8 capmode)
{
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port,pin;           //�������Ķ˿ڡ����ź���ʱ����
    uint_8 TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint_8 mux_val;

    //1. gpio���Ž���
    port = (tpmx_Chy>>8);    //�������Ķ˿�
    pin = tpmx_Chy;          //�����������ź�
    //2. ����port�����ֲ�����port_ptr��ֵ,��û���ַ
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    //3. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. ����pin,ָ�������Ź���ΪTPM��ͨ�����ܣ��������ſ��ƼĴ�����MUX=mux_val��
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. ���벶׽�����趨
    if(capmode == CAP_UP)           //�����ز�׽
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
    else if(capmode == CAP_DOWN)    //�½��ز�׽
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    else if(capmode == CAP_DOUBLE)  //˫���ز�׽
    {
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    }
    //6. chlͨ���ж�ʹ��
    TPM_CnSC_REG(TPM_ARR[TPM_i],chl) |= TPM_CnSC_CHIE_MASK;
}

//=====================================================================
//�������ƣ�tpm_get_capvalue
//���ܸ�Ҫ����ȡtpmxģ��Chyͨ���ļ�������ǰֵ
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//�������أ�tpmxģ��Chyͨ���ļ�������ǰֵ
//=====================================================================
uint_16 tpm_get_capvalue(uint_16 tpmx_Chy)
{
    uint_8 TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint_16 cnt;
    tpm_mux_val(tpmx_Chy,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    cnt=TPM_CnV_REG(TPM_ARR[TPM_i],chl);
    return cnt;

}

//===================================================================
//�������ƣ�outcompare_init
//���ܸ�Ҫ��outcompareģ���ʼ����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//        comduty������Ƚϵ�ƽ��תλ��ռ�����ڵı�����0.0~100.0��Ӧ0%~100%
//        cmpmode������Ƚ�ģʽ����ת��ƽ��ǿ�Ƶ͵�ƽ��ǿ�Ƹߵ�ƽ��,�к궨�峣��ʹ��
//�������أ���
//===================================================================
void outcompare_init(uint_16 tpmx_Chy,float comduty,uint_8 cmpmode)
{
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint_8 port,pin;           //�������Ķ˿ڡ����ź���ʱ����
    uint_8 mux_val;
    uint_8 TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint_32 mod;

    if(comduty>100.0)  comduty=100.0;

    //1. gpio���Ž���
    port = (tpmx_Chy>>8);    //�������Ķ˿�
    pin = tpmx_Chy;          //�����������ź�
    //2. ����port�����ֲ�����port_ptr��ֵ,��û���ַ
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    //3. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    mux_val=tpm_mux_val(tpmx_Chy,&TPM_i,&chl);
    //4. ����pin,ָ�������Ź���ΪTPM��ͨ�����ܣ��������ſ��ƼĴ�����MUX=mux_val��
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. ����Ƚ�ģʽ���趨
    if(cmpmode == CMP_REV)        //��תģʽ
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
                                       TPM_CnSC_ELSA_MASK;
    else if(cmpmode == CMP_LOW)   //ǿ�Ƶ͵�ƽģʽ
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
             TPM_CnSC_ELSB_MASK;
    else if(cmpmode == CMP_HIGH)  //ǿ�Ƹߵ�ƽģʽ
      TPM_CnSC_REG(TPM_ARR[TPM_i],chl)=TPM_CnSC_MSA_MASK| \
                      TPM_CnSC_ELSA_MASK|TPM_CnSC_ELSB_MASK;
    //6. ����Ƚ�ռ�ձȵ��趨
    mod=TPM_ARR[TPM_i]->MOD;   //��ȡģ���Ĵ�����ֵ��MOD��
    TPM_CnV_REG(TPM_ARR[TPM_i],chl)=(uint32_t)mod*(comduty/100);
}

//===================================================================
//�������ƣ�tpm_enable_int
//���ܸ�Ҫ��ʹ��tpmģ���жϡ�
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//�������أ���
//===================================================================
void tpm_enable_int(uint_8 TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //��TPM�ж�
    NVIC_EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//�������ƣ�tpm_disable_int
//���ܸ�Ҫ������tpmģ���жϡ�
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//�������أ���
//===================================================================
void tpm_disable_int(uint_8 TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //��TPM�ж�
    NVIC_DisableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//�������ƣ�tpm_get_int
//���ܸ�Ҫ����ȡTPMģ���жϱ�־
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//�������أ��жϱ�־ 1=���жϲ���;0=���жϲ���
//===================================================================
uint_8 tpm_get_int(uint_8 TPM_i)
{
  //��ȡTPM_iģ���жϱ�־λ
  if(((TPM_SC_REG(TPM_ARR[TPM_i]) & TPM_SC_TOF_MASK)==TPM_SC_TOF_MASK))
    return 1;
  else
    return 0;
}

//===================================================================
//�������ƣ�tpm_chl_get_int
//���ܸ�Ҫ����ȡTPMͨ���жϱ�־
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2��TPMC_i��0-5ͨ��
//�������أ��жϱ�־ 1=���жϲ���;0=���жϲ���
//===================================================================
uint_8 tpm_chl_get_int(uint_8 TPM_i,uint_8 TPMC_i)
{
   //��ȡTPM_iģ��TPMC_iͨ���жϱ�־λ
   if(((TPM_CnSC_REG(TPM_ARR[TPM_i],TPMC_i)&TPM_CnSC_CHF_MASK)\
                      ==TPM_CnSC_CHF_MASK))
       return 1;
   else
       return 0;
}

//===================================================================
//�������ƣ�tpm_clear_int
//���ܸ�Ҫ�����TPM�жϱ�־
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//�������أ�����жϱ�־λ
//===================================================================
void tpm_clear_int(uint_8 TPM_i)
{
    //���TPM_iģ���жϱ�־λ
    BSET(TPM_SC_TOF_SHIFT,TPM_SC_REG(TPM_ARR[TPM_i]));
}

//===================================================================
//�������ƣ�tpm_clear_chl_int
//���ܸ�Ҫ�����TPMͨ���жϱ�־
//����˵����TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2��TPMC_i��0-5ͨ��
//�������أ����TPMͨ���жϱ�־λ
//===================================================================
void tpm_clear_chl_int(uint_8 TPM_i,uint_8 TPMC_i)
{
    //���TPM_iģ��TPMC_iͨ���жϱ�־λ
    BSET(TPM_CnSC_CHF_SHIFT,TPM_CnSC_REG(TPM_ARR[TPM_i],TPMC_i));
}

//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�tpm_mux_val
//���ܸ�Ҫ������������tpmx_Chy���н������ó�����ģ�����ͨ���ţ�����TPM_CH0
//        ������PORT���ţ����������ŷ���mux_val����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//
//�������أ�gpio���üĴ�������ֵ
//=====================================================================
static uint_8 tpm_mux_val(uint_16 tpmx_Chy,uint_8* TPM_i,uint_8* chl)
{
    uint_8 port,pin;
    //1.����ģ��ź�ͨ����
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
    //2.�������Ÿ��üĴ�������ֵ
    port = (tpmx_Chy>>8);
    pin = tpmx_Chy;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}
