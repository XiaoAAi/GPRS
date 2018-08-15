//======================================================================
//�ļ����ƣ�uart.c
//���ܸ�Ҫ��uart�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2015-7-14 V1.0
//======================================================================
#include "uart.h"

//=====����1��2�ŵ�ַӳ��====
static const UART_MemMapPtr UART_ARR[] = {UART1_BASE_PTR, UART2_BASE_PTR};
//====���崮��IRQ�Ŷ�Ӧ��====
static IRQn_Type table_irq_uart[3] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};

//�ڲ���������
uint_8 uart_is_uartNo(uint_8 uartNo);

//======================================================================
//�������ƣ�uart_init
//���ܸ�Ҫ����ʼ��uartģ��                                                
//����˵����uartNo:���ںţ�UART_0��UART_1��UART_2 
//          baud:�����ʣ�300��600��1200��2400��4800��9600��19200��115200...
//�������أ���
//======================================================================
void uart_init(uint_8 uartNo, uint_32 baud_rate)
{
    //�ֲ���������
    uint_16 sbr;
    uint_8 temp;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR;//uartch_0ΪUART0_MemMapPtr����ָ��
    UART_MemMapPtr uartch_1_2;    //uartch_1_2ΪUART_MemMapPtr����ָ��

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    //���ݴ������uartNo�����ֲ�����uartch_0��uartch_1_2��ֵ
    if(uartNo==0)
    {
        //UART0ѡ��MCGFLLCLK_khz=48000Khzʱ��Դ
        SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);
        SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
#ifdef UART_0_GROUP
        //����ѡ�����ö�Ӧ����ΪUART0
        switch(UART_0_GROUP)
        {
        case 0:
            PORTE_PCR20 |= PORT_PCR_MUX(0x4);    //ʹ��UART0_TXD
            PORTE_PCR21 |= PORT_PCR_MUX(0x4);    //ʹ��UART0_RXD
            break;
        case 1:
            PORTA_PCR2 |= PORT_PCR_MUX(0x2);     //ʹ��UART0_TXD
            PORTA_PCR1 |= PORT_PCR_MUX(0x2);     //ʹ��UART0_RXD
            break;
        case 2:
            PORTA_PCR14 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_TXD
            PORTA_PCR15 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_RXD
            break;
        case 3:
            PORTB_PCR17 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_TXD
            PORTB_PCR16 |= PORT_PCR_MUX(0x3);    //ʹ��UART0_RXD
            break;
        case 4:
        default:
            PORTD_PCR7 |= PORT_PCR_MUX(0x3);     //ʹ��UART0_TXD
            PORTD_PCR6 |= PORT_PCR_MUX(0x3);     //ʹ��UART0_RXD
            break;
        }
#endif
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;    //��������0ʱ��

        //��ʱ�رմ���0��������չ���
        UART0_C2_REG(uartch_0) &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);

        //���ô��ڹ���ģʽ:8λ��У��ģʽ
        sbr = (uint_16)((SYSTEM_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART0_BDH_REG(uartch_0) & ~(UART0_BDH_SBR(0x1F));
        UART0_BDH_REG(uartch_0) = temp | UART0_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART0_BDL_REG(uartch_0) = (uint_8)(sbr & UART0_BDL_SBR_MASK);

        //��ʼ�����ƼĴ��������־λ
        UART0_C4_REG(uartch_0) = 0x0F;
        UART0_C1_REG(uartch_0) = 0x00;
        UART0_C3_REG(uartch_0) = 0x00;
        UART0_MA1_REG(uartch_0) = 0x00;
        UART0_MA2_REG(uartch_0) = 0x00;
        UART0_S1_REG(uartch_0) |= 0x1F;
        UART0_S2_REG(uartch_0) |= 0xC0;
        //���ô��ڷ����ж�
        UART0_C2_REG(uartch_0) &= ~UART0_C2_TIE_MASK;
        //�������ͽ���
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
                PORTE_PCR0 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
                PORTE_PCR1 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
                break;
            case 1:
                PORTA_PCR14 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
                PORTA_PCR15 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
                break;
            case 2:
            default:
                PORTC_PCR4 = PORT_PCR_MUX(0x3);    //ʹ��UART1_TXD
                PORTC_PCR5 = PORT_PCR_MUX(0x3);    //ʹ��UART1_RXD
                break;
            }
#endif
            SIM_SCGC4 |= SIM_SCGC4_UART1_MASK; //��������1ʱ��
            break;
        case UART_2:
#ifdef UART_2_GROUP
            switch(UART_2_GROUP)
            {
            case 0:
                PORTE_PCR22 = PORT_PCR_MUX(0x4); //ʹ��UART2_TXD
                PORTE_PCR23 = PORT_PCR_MUX(0x4); //ʹ��UART2_RXD
                break;
            case 1:
                PORTD_PCR3 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
                PORTD_PCR2 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
                break;
            case 2:
            default:
                PORTD_PCR5 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
                PORTD_PCR6 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
                break;
            }
#endif
            SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;//��������2ʱ��
            break;
        default:
            break;  //���δ��󣬷���
        }
        uartch_1_2 = UART_ARR[uartNo-1]; //���UART1��2ģ����Ӧ�ڻ���ַ

        //��ʱ�رմ���1��2��������չ���
        UART_C2_REG(uartch_1_2) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

        //���ò�����,KL25����1��2ʱ��Ƶ��ʹ�� Bus clock = 24M ����ʱ��
        //���ô��ڹ���ģʽ,8λ��У��ģʽ
        sbr = (uint_16)((BUS_CLK_KHZ*1000)/(baud_rate * 16));
        temp = UART_BDH_REG(uartch_1_2) & ~(UART_BDH_SBR(0x1F));
        UART_BDH_REG(uartch_1_2) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
        UART_BDL_REG(uartch_1_2) = (uint_8)(sbr & UART_BDL_SBR_MASK);

        //��ʼ�����ƼĴ��������־λ
        UART_C1_REG(uartch_1_2)=0x00;
        UART_C3_REG(uartch_1_2)=0x00;
        UART_S2_REG(uartch_1_2)=0x00;
        //���ô��ڷ����ж�
        UART_C2_REG(uartch_1_2) &= ~UART_C2_TIE_MASK;
        //�������ͽ���
        UART_C2_REG(uartch_1_2)|= (UART_C2_TE_MASK | UART_C2_RE_MASK);
    }
}
//======================================================================
//�������ƣ�uart_send1
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//          ch:Ҫ���͵��ֽ�
//�������أ�����ִ��״̬��1=���ͳɹ���0=����ʧ�ܡ�
//���ܸ�Ҫ�����з���1���ֽ�
//======================================================================
uint_8 uart_send1(uint_8 uartNo, uint_8 ch)
{
    uint_32 t;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//��ѯָ������
    {
        if(0==uartNo)     //�ж�ʹ�õ��ĸ�����
        {
        	//���ͻ�����Ϊ����������
            if ( UART0_S1_REG(uartch_0) & UART0_S1_TDRE_MASK )
            {
                UART0_D_REG(uartch_0) = ch;
                break;
            }
        }
        else
        {
        	//���ͻ�����Ϊ����������
            if (UART_S1_REG(uartch_1_2) & UART_S1_TDRE_MASK )
            {
                UART_D_REG(uartch_1_2) = ch;
                break;
            }
        }
    }//end for
    if (t >= 0xFBBB)
        return 0; //���ͳ�ʱ������ʧ��
    else
        return 1; //���ͳɹ�
}

//======================================================================
//�������ƣ�uart_sendN
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//         buff: ���ͻ�����
//         len:���ͳ���
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�   
//======================================================================
uint_8 uart_sendN(uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for (i = 0; i < len; i++)
    {
        if (! uart_send1(uartNo, buff[i])) //����һ���ֽ����ݣ�ʧ��������ѭ��
        {
            break;
        }
    }
    if(i<len)
        return 0;    //���ͳ���
    else
        return 1;    //���ͳɹ�
}

//======================================================================
//�������ƣ�uart_send_string
//����˵����uartNo:UARTģ���:UART_0��UART_1��UART_2 
//          buff:Ҫ���͵��ַ������׵�ַ
//�������أ� ����ִ��״̬��1=���ͳɹ���0=����ʧ��
//���ܸ�Ҫ����ָ��UART�˿ڷ���һ����'\0'�������ַ���
//======================================================================
uint_8 uart_send_string(uint_8 uartNo, void *buff)
{
    uint_16 i = 0;
    uint_8 *buff_ptr = (uint_8 *)buff;     //����ָ��ָ��Ҫ�����ַ����׵�ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    for(i = 0; buff_ptr[i] != '\0'; i++)   //�����ַ�������ַ�
    {
        if (!uart_send1(uartNo,buff_ptr[i]))//����ָ���Ӧ���ַ�
            return 0;  //����ʧ��,����
    }
    return 1;          //���ͳɹ�
}

//======================================================================
//�������ƣ�uart_re1
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//        *fp:���ճɹ���־��ָ��:*fp=1:���ճɹ���*fp=0:����ʧ��
//�������أ����շ����ֽ�
//���ܸ�Ҫ�����н���1���ֽ�
//======================================================================
uint_8 uart_re1(uint_8 uartNo,uint_8 *fp)
{
    uint_32 t;
    uint_8  dat;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        *fp=0;
        return 0;
    }

    for (t = 0; t < 0xFBBB; t++)//��ѯָ������
    {
        if(0==uartNo)  //�ж�ʹ�õ��ĸ�����
        {
        //�жϽ��ջ������Ƿ���
        if (UART0_S1_REG(uartch_0) & UART0_S1_RDRF_MASK)
            {
                dat=UART0_D_REG(uartch_0);    //��ȡ����,������ж�λ
                *fp = 1;  //���ճɹ�
                break;
            }
        }
        else
        {
            //�жϽ��ջ������Ƿ���
            if(UART_S1_REG(uartch_1_2) & UART_S1_RDRF_MASK)
            {
                dat=UART_D_REG(uartch_1_2);    //��ȡ����,������ж�λ
                *fp= 1;  //���ճɹ�
                break;
            }
        }
    }//end for
    if(t >= 0xFBBB) 
    {
        dat = 0xFF; 
        *fp = 0;    //δ�յ�����
    }
    return dat;    //���ؽ��յ�������
}

//======================================================================
//�������ƣ�uart_reN
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2                                                   
//          buff: ���ջ�����
//          len:���ճ���
//�������أ�����ִ��״̬ 1=���ճɹ�;0=����ʧ��
//���ܸ�Ҫ������ ����n���ֽ�,����buff��
//======================================================================
uint_8 uart_reN(uint_8 uartNo ,uint_16 len ,uint_8* buff)
{
    uint_16 i;
    uint_8 flag = 0;

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    //�ж��Ƿ��ܽ�������
    for (i = 0; i < len && flag==1; i++)
    {
        buff[i] = uart_re1(uartNo, &flag); //��������
    }
    if (i < len)
        return 0; //����ʧ��
    else
        return 1; //���ճɹ�
}

//======================================================================
//�������ƣ�uart_enable_re_int
//����˵����uartNo: ���ں�:UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ�������ڽ����ж�
//======================================================================
void uart_enable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0 == uartNo)
        UART0_C2_REG(uartch_0) |= UART0_C2_RIE_MASK;    //����UART�����ж�
    else 
        UART_C2_REG(uartch_1_2) |= UART_C2_RIE_MASK;    //����UART�����ж�
    NVIC_EnableIRQ(table_irq_uart[uartNo]);    //���жϿ�����IRQ�ж�
}

//======================================================================
//�������ƣ�uart_disable_re_int
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2 
//�������أ���
//���ܸ�Ҫ���ش��ڽ����ж�
//======================================================================
void uart_disable_re_int(uint_8 uartNo)
{
    UART0_MemMapPtr uartch_0 = UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return;
    }

    if(0==uartNo)
        UART0_C2_REG(uartch_0) &= ~UART0_C2_RIE_MASK;    //��ֹUART�����ж�
    else
        UART_C2_REG(uartch_1_2) &= ~UART_C2_RIE_MASK;    //��ֹUART�����ж�
    NVIC_DisableIRQ(table_irq_uart[uartNo]);
}

//======================================================================
//�������ƣ�uart_get_re_int
//����˵����uartNo: ���ں� :UART_0��UART_1��UART_2
//�������أ������жϱ�־ 1=�н����ж�;0=�޽����ж�
//���ܸ�Ҫ����ȡ���ڽ����жϱ�־,ͬʱ���÷����ж�
//======================================================================
uint_8 uart_get_re_int(uint_8 uartNo)
{
    uint_8 flag;
    UART0_MemMapPtr uartch_0=UART0_BASE_PTR; //��ȡUART0����ַ
    UART_MemMapPtr uartch_1_2 = UART_ARR[uartNo-1]; //��ȡUART1����2����ַ

    //�жϴ��봮�ںŲ����Ƿ���������ֱ���˳�
    if(!uart_is_uartNo(uartNo))
    {
        return 0;
    }

    if(0==uartNo)
    {
        //���ô��ڷ����жϣ���ֹ���ж�
        UART0_C2_REG(uartch_0) &= (~UART0_C2_TIE_MASK);
        //��ȡ�����жϱ�־����ͬʱ�ж�RDRF��RIE
        flag= UART0_S1_REG(uartch_0) & (UART0_S1_RDRF_MASK);
        return(BGET(UART0_S1_RDRF_SHIFT,flag)
                & BGET(UART0_C2_RIE_SHIFT,UART0_C2_REG(uartch_0)));
    }
    else
    {
        //���ô��ڷ����жϣ���ֹ���ж�
        UART_C2_REG(uartch_1_2) &= (~UART_C2_TIE_MASK);
        //��ȡ�����жϱ�־����ͬʱ�ж�RDRF��RIE
        flag= UART_S1_REG(uartch_1_2) & (UART_S1_RDRF_MASK);
        return(BGET(UART_S1_RDRF_SHIFT,flag)
                & BGET(UART_C2_RIE_SHIFT,UART_C2_REG(uartch_1_2)));
    }
}

//----------------------����Ϊ�ڲ�������Ŵ�--------------------------------------
//=====================================================================
//�������ƣ�uart_is_uartNo
//�������أ�1:���ں��ں���Χ�ڣ�0�����ںŲ�����
//����˵�������ں�uartNo  :UART_0��UART_1��UART_2
//���ܸ�Ҫ��Ϊ����׳�Զ��ж�uartNo�Ƿ��ڴ������ַ�Χ��
//=====================================================================
uint_8 uart_is_uartNo(uint_8 uartNo)
{
    if(uartNo < UART_0 || uartNo > UART_2)
        return 0;
    else
        return 1;
}
//----------------------------�ڲ���������--------------------------------------
