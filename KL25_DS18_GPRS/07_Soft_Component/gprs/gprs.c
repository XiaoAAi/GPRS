#include "gprs.h"


//���ܣ�ִ�жԸ���->����->���ӵĽ�һ���ڲ�����Ҳ����һЩ��Ҫ���ã�
//����:char *Server_IP_Port
//����ֵ��Ϊuint_8  = 1��ʾ���ӳɹ�   < 0 ��ʾ����ʧ��
int_8 GPRS_Config(char *Server_IP_Port);
//���ܣ������ִ�����ַ�����ʽ
//������uint_16 AT_numlength :��ʾ���鳤��
//char *AT_NUM:�ֲ�����->�����ַ�����ڴ洢ת������ַ�����Pack_String�����ľֲ�������
//uint_16 length_value : ��ֵ(���������ֽڳ���)uint_16 ��ֵ��ʾ����0��ֵ
//����ֵ�� (����Ҫ����ֵ����Ϊ�����ǵ�ַ�����Կ��Խ��иı�ʵ��)
void Num_To_String(char *AT_NUM, uint_16 num_length, uint_16 length_value);

//================================���ʹ�õĺ���============================================
//���ܣ�����ATָ����ñȽϵĻط����ַ��������õȴ�ʱ��ȣ�ͬʱ�����ж�ATָ���Ƿ��ͳɹ�
//������char *AT_command �����͵�ATָ��, char *com_val �����ñȽϵĻط����ַ���, uint_16 wait_time_MS:���õȴ�ʱ��
//����ֵ��1�����ͳɹ���0������ʧ��
uint_8 Send_AT_Command(char *AT_command, char *com_val,uint_16 wait_time_Ms);
//��Ҫ��Ϊ���ܹ�ֱ�ӽ���ASCII��ֵ���в���
//���ܣ�����ATָ����ñȽϵĻط����ַ��������õȴ�ʱ��ȣ�ͬʱ�����ж�ATָ���Ƿ��ͳɹ�
//������char *AT_command �����͵�ATָ��, uint_8 com_val �����ñȽϵĻط����ַ���, uint_16 wait_time_MS:���õȴ�ʱ��
//����ֵ��1�����ͳɹ���0������ʧ��
//uint_8 Send_AT_Command_Data(char *AT_command, uint_8 com_val, uint_16 wait_time_Ms);

//====================================�ڲ�ʹ�õĺ���==============================

//���ܣ�ͨ��strcmp����ж�Data_Buf��com_val�������Ƿ���ȣ�������Ӧֵ
//������char *com_val
//����ֵ��1����ʾ��� ��0����ʾ�����
uint_8 judge_right(char *com_val);

//���ܣ�ͨ��forѭ������ж�����Ҫ�ĵ�ASCII��ֵ����
uint_8 judge_ture(uint_8 com_val);

//���ܣ�����ָ�����\r\n�������������صĻ�����
void send_Command(char *AT_command);


//===============================================================================

//���ܣ�����ATָ����ñȽϵĻط����ַ��������õȴ�ʱ��ȣ�ͬʱ�����ж�ATָ���Ƿ��ͳɹ�
//������char *AT_command �����͵�ATָ��, char *com_val �����ñȽϵĻط����ַ���, uint_16 wait_time_MS:���õȴ�ʱ��
//����ֵ��TRUE�����ͳɹ���FALSE������ʧ��
uint_8 Send_AT_Command(char *AT_command, char *com_val, uint_16 wait_time_Ms)
{
	int i = 3;
	//�����Ӧ�Ļ�����
	for(i=0; i<BUF_MAX; i++)
	{
		Data_Buf[i] = '\0';
	}
	//ͨ��strcat�����ϲ�AT_command�������һ���̶����ȵ��ַ�����"AT"
//	strcat(AT_command,"\r\n");
	//ͨ��UART2���ڷ������
	uart_send_string(UART_2,AT_command);
	//��ʱwait_time_MS ms
	delayms(wait_time_Ms);
	//�жϷ���ֵ��uart2�Ľ��ջ��������ַ����Ա�
	if(judge_right(com_val) == 1)
	{
		//����Ϊ���ͳɹ�
		return TRUE;
	}
	//�ط��Ĵ�
	else
	{
		while(1)
		{
			//����ATָ��
			send_Command(AT_command);
			//��ʱ����
			delayms(wait_time_Ms);
			//�жϷ���ֵ��uart2���ջ��������ַ������жԱ�
			if(judge_right(com_val) == 1)
			{
				//���ͳɹ�
				return TRUE;
			}
			else if(i < 0)
			{
				return FALSE;
			}
			i--;
		}
	}
}

//���ܣ�ͨ��strstr����ж���Data_Buf�� ���ҵ�һ�γ����ַ���com_val��ֵ
//������char *com_val
//����ֵ��1����ʾ��� ��0����ʾ�����
//uint_8 judge_right(char *com_val)
//{
//	//ͨ��strstr������������Data_Buf�в��ҵ�һ�γ����ַ���com_val���ж��Ƿ����ҵ���
//	if(strstr(Data_Buf,com_val) != NULL)
//	{
//		return TRUE;
//	}
//	else
//	{
//		return FALSE;
//	}
//}
uint_8 judge_right(char * com_val)
{
	uint_8 i ;
	uint_8 rval;
	char * temp = Data_Buf;
	for (i = 0; i < BUF_MAX - strlen(com_val); i++) {
		rval = strncmp(temp + i, com_val, strlen(com_val));
		if (!rval)
		{
			return TRUE;
		}
	}
	return FALSE;
}



////��Ҫ��Ϊ���ܹ�ֱ�ӽ���ASCII��ֵ���в���,�������ڷ���AT+QISEND=ָ����Խ��з������ݵ��ֽ����ݵ����롣���Խ��������ֳ�������
////���ܣ�����ATָ����ñȽϵĻط����ַ��������õȴ�ʱ��ȣ�ͬʱ�����ж�ATָ���Ƿ��ͳɹ�
////������char *AT_command �����͵�ATָ��,uint_8 com_val ����ʾ'>'��ASCII��ֵ�����ж���, uint_16 wait_time_MS:���õȴ�ʱ��
////����ֵ��TRUE�����ͳɹ���FALSE������ʧ��
//uint_8 Send_AT_Command_Data(char *AT_command, uint_8 com_val, uint_16 wait_time_Ms)
//{
//	int i = 3;
//	//����AT�������2
//	send_Command(AT_command);
//	//��ʱwait_time_MS ms
//	delayms(wait_time_Ms);
//	//�жϷ���ֵ��uart2�Ľ��ջ��������ַ����Ա�
//	if(judge_ture(com_val) == 1)
//	{
//		//����Ϊ���ͳɹ�
//		return TRUE;
//	}
//	//�ط��Ĵ�
//	else
//	{
//		while(1)
//		{
//			//����AT�������2
//			send_Command(AT_command);
//			//��ʱ����
//			delayms(wait_time_Ms);
//			//�жϷ���ֵ��uart2���ջ��������ַ������жԱ�
//			if(judge_ture(com_val) == 1)
//			{
//				//���ͳɹ�
//				return TRUE;
//			}
//			else if(i < 0)
//			{
//				return FALSE;
//			}
//			i--;
//		}
//	}
//}

//���ܣ�ͨ��forѭ������ж�����Ҫ�ĵ�ASCII��ֵ����
//������char *com_val
//����ֵ��TRUE����ʾ��� ��FALSE����ʾ�����
uint_8 judge_ture(uint_8 com_val)
{
	int i;
	for(i=0; i<BUF_MAX; i++)
	{
		if(Data_Buf[i] == com_val)
		{
			return TRUE;
		}
	}
	return FALSE;
}


//���ܣ�����ָ�����\r\n�������������صĻ�����
//������char *AT_command
//����ֵ����
void send_Command(char *AT_command)
{
	int i;
	//�����Ӧ�Ļ�����
	for(i=0; i<BUF_MAX; i++)
	{
		Data_Buf[i] = '\0';
	}
	//ͨ��strcat�����ϲ�AT_command�������һ���̶����ȵ��ַ�����"AT"
//	strcat(AT_command,"\r\n");
	//ͨ��UART2���ڷ������
	uart_send_string(UART_2,AT_command);
	uart_send_string(UART_2,"\r\n");
}


//==================================================GPRS�ⲿʹ�ú���======================================
//���ܣ��������һЩ��������������ӣ�AT���ԣ���ѯ���Ƿ��ã�
//������void
//����ֵ��uint_8 ���Գ��� ���Գɹ� ����1 ʧ�ܷ��� 0
int_8 GPRS_init(void)
{
	int_8 flag = 0;
	//����ATָ������Ƿ��лط�
	if(Send_AT_Command("AT","OK",5) == 1)
	{
		//����AT+CPIN?�Ƿ񿨲��
		if(Send_AT_Command("AT+CPIN?","READY",15) == 1)
		{
			//���Գɹ���־λ
			flag = TEST_OK;
			goto end;
		}
		else
		{
			//����-2 û��׼���ñ�־λ
			flag = NO_SIM_CARD_ERROR;
			goto end;
		}
	}
	else
	{
//		����-22 AT����û��׼���ñ�־λ
		flag = AT_ERROR;
		goto end;
	}
	//����flag��־
	end: return flag;
}

//���ܣ���Ҫ���ж��Ƿ����ӳɹ�����
//������char *Server_IP_Port
//����ֵ��int_8 1�����ӳɹ� ��С��0 ��ʾ����ʧ��
uint_8 GPRS_Judg_Conn(char *Server_IP_Port)
{
	uint_8 flag  = 0;
	//��ѯGPRS��״̬�Ƿ����ӳɹ� 200ms
	if(Send_AT_Command("AT+CIPSTATUS=0","CONNECT OK",15) == 1)
	{
		//���ӳɹ���־λ
		flag = CONNEXCT_OK;
		goto end;
	}
	//��ѯGPRS����״̬Ϊ IP CLOSE ״̬��������ֱ�ӽ������ӽ��У��Ϳ���ֱ�Ӵ�������
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP INITIAL",15) == 1)
	{
		//�������GPRS����ֵΪ1�����ʾ�ɹ�����
		if(GPRS_Config(Server_IP_Port) == 1)
		{
			//���ӳɹ���־λ
			flag = CONNEXCT_OK;
			goto end;
		}
		//����С��0 ����������󣬿��Խ��в鿴�Ǹ�����//��߻��Ǽ�����������
//		else if(GPRS_Config(Server_IP_Port) < 0)
		else
		{
			//����ʧ�ܱ�־λ
			flag = DIS_CONNECT;
			goto end;
		}
	}
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP GPRSACT",15) == 1)
	{
		if(Send_AT_Command(Server_IP_Port,"CONNECT OK",30) == 1)
		{
				//���ӳɹ���־λ
				flag = CONNEXCT_OK;
				goto end;
		}
	}
	//ִ�жԸ���->����->���ӵĽ�һ���ڲ���
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP CLOSE",15) == 1)
	{
		if(Send_AT_Command(Server_IP_Port,"CONNECT OK",30) == 1)
		{
				//���ӳɹ���־λ
				flag = CONNEXCT_OK;
				goto end;
		}
	}
	end:return flag;
}

//���ܣ���������
//������uint_8 Data:���͵����� char * AT_QISEND����ʾmain�����ľֲ�����AT+QISEND=length��ָ������
//����ֵ��uint_8 1����ʾ�������ݳɹ���0 ��ʾ��������ʧ��
int_8 GPRS_Send_Data(uint_8 * Sender_AndroidEnd)
{
	int_8 flag = 0;
	//��������(��������ͨ�� AT+QISEND 62 ��ʾASCIIֵ ��>��,50��ʾ50ms)
	if(Send_AT_Command("AT+CIPSEND=1",">",7)==1)
	{
//		//���ڷ�������
//		uart_send_string(UART_2,Data);
//		if(Send_AT_Command(Sender_AndroidEnd,"OK",100)==1)
//		{
		//ͨ��strcat�����ϲ�AT_command�������һ���̶����ȵ��ַ�����"AT"
	//	strcat(AT_command,"\r\n");
		//ͨ��UART2���ڷ������
		uart_sendN(UART_2,7,Sender_AndroidEnd);
		//��ʱwait_time_MS ms
		delayms(100);
			//���÷��ͳɹ���־
		flag = SEND_OK;
		goto end;
	}
	else
	{
		//���ط��ʹ��� -14
		flag = AT_CIPSEND_ERROR;
		goto end;
	}
	end: return flag;
}

//���ܣ�ִ�жԸ���->����->���ӵĽ�һ���ڲ�����Ҳ����һЩ��Ҫ���ã�
//����:char *Server_IP_Port
//����ֵ��Ϊuint_8  = 1��ʾ���ӳɹ�   < 0 ��ʾ����ʧ��
int_8 GPRS_Config(char *Server_IP_Port)
{
	int_8 flag = 0;
	//�������磬��3s
	if(Send_AT_Command("AT+CGATT=1","OK",20))
	{
		//�������� 2S
		if(Send_AT_Command("AT+CGACT=1,1","OK",50))
		{
			//�������� 2S
			if(Send_AT_Command(Server_IP_Port,"CONNECT OK",40))
			{
				//���ӳɹ���־
				flag = CONNEXCT_OK;
				goto end;
			}
			else
			{
				//��������ʧ��ֵ -13
				flag = AT_CIPSTART_ERROR;
				goto end;
			}
		}
		else
		{
			//��������ʧ��
			flag = AT_CGACT_ERROR;
			goto end;
		}
	}
	else
	{
		//���ظ���������� -11
		flag = AT_CGATT_ERROR;
		goto end;
	}
	end:return flag;
}

//���ܣ������͵����ݻ�����
//��������
//����ֵ�� ��
void Clear_Send_Data(char * Send_AndroidEnd)
{
	uint_16 i;
	uint_16 length = 0;
	length = strlen(Send_AndroidEnd);
	for(i=0; i<length; i++)
	{
		Send_AndroidEnd[i] = '\0';
	}

}
//���ܣ������ܵĴ����ֽڵĳ��ȣ��ж��ַ��������Ƿ�Խ��
//������uint_8 *Data_Send  �ַ�������
//����ֵ��return length :�����ַ����ĳ���  return LENGH_OUT ��ʾ�������ͷ�Χ
uint_16 Caculate_Length(char *Data_Send)
{
	uint_16 length = 0;
	length = strlen(Data_Send);
	if(length == 0 || length > 1024)
	{
		//�������ͷ�Χ
		return LENGTH_OUT;
	}
	else
	{
		//�����ַ����ĳ���
		return length;
	}

}

//���ܣ����ַ������Ⱥ�AT+QISEND= ָ��һ�������ַ���(�ַ���������Ҳ������strcat����)
//������uint_16 AT_numlength :��ʾ���鳤��
//char *AT_QISEND:�ֲ�����->�����ַ�����ڴ洢ת������ַ���(main�����ľֲ�����)
//uint_16 length_value : ��ֵ(���������ֽڳ���)uint_16 ��ֵ��ʾ����0��ֵ
//����ֵ����
void Pack_String(char *AT_QISEND, uint_16 AT_numlength, uint_16 length_value)
{
	//����һ���ֲ����������մ洢�����ݳ���ֵ���ַ���ת��
	char AT_num[5];
	//ֱ�ӽ��н���ֵ���ϳ��ַ���
	Num_To_String(AT_num,5,length_value);
	//ʹ��sprintfֱ�ӽ�AT+QISEND=ָ���װ���ַ���
	sprintf((char *)AT_QISEND,"AT+QISEND=");
	//ͨ��strcat������AT_QISEND�����AT_NUM����������һ�𣬴洢��AT_QISEND������
	strcat(AT_QISEND,AT_num);
}

//������Բ��ֵ��þ��вο���ֵ
//	uint_8 i;
//	for(i=0; i<AT_QISEND_LENGTH; i++)
//	{
//		AT_QISEND[i] = '\0';
//	}
//	char AT_QISEND1[20];
//Ϊʲô������sprintfû�취ֱ���ã���
//	sprintf((char *)AT_QISEND1,"AT+QISEND=%d",length);

//���ܣ������ִ�����ַ�����ʽ
//������uint_16 AT_numlength :��ʾ���鳤��
//char *AT_NUM:�ֲ�����->�����ַ�����ڴ洢ת������ַ�����Pack_String�����ľֲ�������
//uint_16 length_value : ��ֵ(���������ֽڳ���)uint_16 ��ֵ��ʾ����0��ֵ
//����ֵ�� (����Ҫ����ֵ����Ϊ�����ǵ�ַ�����Կ��Խ��иı�ʵ��)
void Num_To_String(char *AT_NUM, uint_16 num_length, uint_16 length_value)
{
	//����Ϊint�ͣ�������Ҫ�ж�iС��0
	int i=0;
	uint_8 j=0;
	//��һ����ת�����ݸ�AT_NUM
	char temp_num[num_length];
	if(length_value > 0)
	{
		//ֱ�ӿ���ͨ��while���ж��˳�ѭ��
		while(length_value > 0)
		{
			//ʵ����ʱ����ĸ�ֵ��i���Լӣ�ֱ��+'0'����������ASCII��ֵ��Ҳ���¡�
			temp_num[i++] = length_value % 10 + '0';
			length_value /= 10;
		}
		temp_num[i] = '\0';
		//�Ӹߵ��ͽ��з����滻
		//forѭ����whileѭ��һ��������ж���ִ�У����ǲ�ִ�С�
		//��ο�C#���촰��ѧϰ�ʼ�(��ִ�г�ʼ�����ڽ����жϣ���->ִ��ѭ����䣬��->����ѭ��������ν������һ���ֳ����ִ��)
		for(i-=1; i>=0; i--,j++)
		{
			AT_NUM[j] = temp_num[i];
		}
		AT_NUM[j] = '\0';
	}
	else
	{
		//����ʵ���˸�ֵ����ʵ����i�Լ�1
		//temp_num[i++] = 48;			//��0��ASCII��ֵ������Ӧ��ֵ
		AT_NUM[i++] = '0';
		AT_NUM[i] = '\0';
	}
}
