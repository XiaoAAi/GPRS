//=====================================================================
//�ļ����ƣ�isr.c
//���ܸ�Ҫ�� �жϵײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2012-11-12   V1.0
//         2016-03-27 V2.0(XD)
//=====================================================================
#include "includes.h"


//extern uint_32 tpm_count;
//extern uint_16 oled_count;

//========================�жϺ�����������===============================
//����0�����жϷ�������
void UART0_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    DISABLE_INTERRUPTS;    //�����ж�
    if(uart_get_re_int(UART_0))
    {
        ch = uart_re1(UART_0, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
        if(flag)
        {
            uart_send1(UART_0, ch);     //��ԭ���ڷ���һ���ֽ�
        }
    }
    ENABLE_INTERRUPTS;
}

//����1�����жϷ�������
void UART1_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    //��������
//    static uint_16 Received_count = 0;
    DISABLE_INTERRUPTS;    //�����ж�
    if(uart_get_re_int(UART_1))
    {
        ch = uart_re1(UART_1, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
        if(flag)
        {
            uart_send1(UART_1, ch);     //��ԭ���ڷ���һ���ֽ�
        }
    }
    ENABLE_INTERRUPTS;
}

//����2�����жϷ�������
void UART2_IRQHandler(void)
{
    uint_8 ch;
    uint_8 flag;
    static uint_8 Received_count = 0;
    static uint_8 data_count = 0;
    DISABLE_INTERRUPTS;    //�����ж�
    if(uart_get_re_int(UART_2))
    {
        ch = uart_re1(UART_2, &flag);    //���ý���һ���ֽڵĺ�����������ж�λ
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

        	//���ڽ��ܷ�����ת��������
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
        			//ֱ��ͨ��GPRS���з��ͣ����ǵ�GPRS��ֱ�Ӻͷ����������ӵģ�����ʡ������Ҫ�Ĳ�����
        			//��Ҳ��KL25�����������ȷ��ָ���־���������ͳɹ�
//        			GPRS_Send_Data((uint_8 *)"EE");
        		}
        		else
        		{
        			Date_Re[Received_count] = ch;
        			Received_count++;
        		}

        	}

//            uart_send1(UART_2, ch);     //��ԭ���ڷ���һ���ֽ�
        }
    }
    ENABLE_INTERRUPTS;
}

////TPM0�жϷ�������
//void TPM2_IRQHandler(void)
//{
//	if(tpm_get_int(TPM_2) == 1)   //����TPM0������ж�
//	{
//		tpm_clear_int(TPM_2);     //��TPM0������жϱ�־
//
//	}
//}

//TPM1�жϷ�������
void TPM1_IRQHandler(void)
{
	if(tpm_get_int(TPM_1) == 1)   //����TPM1������ж�
	{
		tpm_clear_int(TPM_1);     //��TPM1������жϱ�־
		//����GPRS��ʾ��ʱ��
		gprs_count--;
		//ÿ���Լ�1��������10ms
		tpm_count++ ;
		//����OLED��ʾ��ʱ��
		oled_count++;
	}
}


