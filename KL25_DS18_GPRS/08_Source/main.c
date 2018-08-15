//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//======================================================================

#include "includes.h"   //������ͷ�ļ�

//=========����ĺ���==========
//��ȡ�¶�ֵ
void Read_Temperature();
//GPRS������Ϣ����������
void Send_Message(char * Ip_Port, uint_8 * Sender_AndroidEnd);
//������ʱ����
uint_8  Collection_time();
//����OLED��ʾ����ʱʱ�䣬���Խ���������Ӧ��ʱ
uint_8 DelayS(uint_8 timer);
//CRC8λУ����
uint_8 Crc_8(uint_8 data);
//Android�������ִ��
void Android_Control();
//=========ȫ�ֱ���==============

//���Ʋɼ�ʱ�����(��ʼ��Ϊ1����)
uint_8 Control_Coltime = 0x04;
//��Ƶ�״̬
uint_8 red_state = 0x00;
//�¶�ֵ
uint_16 temp_T = 0;

int main(void)
{
	//DS18B20����ı����ַ�������
	char tempt1[20] ={"Temperature is "};

	//ͨ��TCPЭ�����ӷ�����
	char Ip_Port[41] = {"AT+CIPSTART=\"TCP\",\"47.94.153.56\",8888"};

	//����Android��״̬��Ϣ���ݰ�(��ͷ�����1�����𡢲ɼ�ʱ��1min���¶�ֵ���¶�ֵУ��λ����β)
	uint_8 Sender_Android[7] = {0xEE,0x01,0x00,0x04,0x00,0x00,0xFF};
	uint_8 Sender_End[7] = {0x00};

//	//���Է�������
//	uint_8 Received_Test[5] = {0xEE,0x01,0x01,0x08,0xEF};
    //2. �����ж�
    DISABLE_INTERRUPTS;

    //3. ��ʼ������ģ��
    light_init(LIGHT_BLUE, LIGHT_OFF);  //���Ƴ�ʼ��
    light_init(LIGHT_GREEN, LIGHT_OFF);  //�̵Ƴ�ʼ��
    light_init(LIGHT_RED, LIGHT_OFF);  //��Ƴ�ʼ��

    uart_init(UART_0, 115200);    //��ʼ������0��������Ϊ9600
    uart_init(UART_1, 115200);    //��ʼ������1��������Ϊ115200
    uart_init(UART_2, 115200);    //��ʼ������2��������Ϊ115200
    //uart_init(UART_Debug, 9600);    //��ʼ��UART_Debug��������Ϊ9600
    /*DS18B20��ʼ��
     * */
    ds18b20_Init();			//��ʼ��DS18B20ΪGPIO�ں����
    /*OLED��ʼ��
     * */
    OLED_Init();
    //tpm���ó�ʼ��
    //���0xFFFF->16b��Ҳ����65536��Ҳ����˵��3000*20=60000<65536�Ƿ��ϵģ�
    //��ʱ10ms,Ҳ����˵ÿ20ms�ж�һ��
//    tpm_timer_init(TPM_2,3000,20);
    //��ʱ10ms,Ҳ����˵ÿ10ms�ж�һ��
    tpm_timer_init(TPM_1,3000,10);

    //5. ʹ��ģ���ж�
    uart_enable_re_int(UART_0);   //ʹ�ܴ���0�����ж�
    uart_enable_re_int(UART_1);   //ʹ�ܴ���1�����ж�
    uart_enable_re_int(UART_2);   //ʹ�ܴ���2�����ж�

//    tpm_enable_int(TPM_2);        //ʹ��TPM0�ж�
    tpm_enable_int(TPM_1);        //ʹ��TPM1�ж�
    //6. �����ж�
    ENABLE_INTERRUPTS;   //�����ж�
    
	//������ѭ��
    //��ʱ����10s(���Ի�������)
    //delay_s(10);
	//��ѭ����ʼ==================================================================
	for(;;)
	{
		//��ʱ1s
		if(DelayS(1) == 1)
		{
			//��ȡ�¶�ֵ������ʾ��OLED����
			Read_Temperature(tempt1);
		}
//		Send_Message(Ip_Port,Sender_Android);
		//���������ʱ�����ˣ���ִ�����к���
		if(Collection_time() == 1)
		{
			//����Sender_Android���鵽Sender_End����������ݴ��
			uint_8 i;
			Sender_Android[2] = red_state;
			Sender_Android[3] = Control_Coltime;
			Sender_Android[4] = (uint_8)(temp_T&0xFF);
			Sender_Android[5] = Crc_8((uint_8)(temp_T&0xFF));
			for(i=0; i<7; i++)
			{
				Sender_End[i] = (char)(Sender_Android[i]&0xFF);
			}
			//�������ݵ�������
			Send_Message(Ip_Port,Sender_End);
//			Send_Message(Ip_Port,Sender_Android);
		}
		//android�˿��Ƴ���
		Android_Control();
//		//��ʱ200ms
//		delay_ms(200);
//		uart_reN(UART_2,5,Received_Test);
		//���¼����û�����--------------------------------------------------------
	}//��ѭ��end_for
	//��ѭ������==================================================================
}

//���ܣ�����ȡ�¶Ƚ���һ����װ
//��������
//����ֵ����
void Read_Temperature(char * tempt1)
{
	if(ds18b20_Read()){
		temp_T = Temperature;
//		printf((char *)"�¶��ǣ�%d `C\n",temp_T);
		OLED_P6x8Str(0,0,(uint_8 *)tempt1);
		OLED_Print_Num(30,1,temp_T);
		OLED_P8x16Str(70,1,(uint_8 *)"`C");
//		//��ʱ1s��ͨ��TPM����Ӳ����ʱ��
//		delay_ms(1000);
		//��ת����
		light_change(LIGHT_BLUE);
	}
	else{
		temp_T = Temperature;
//		printf((char *)"�¶��ǣ�-%d `C\n",temp_T);
		OLED_P6x8Str(0,0,(uint_8 *)tempt1);
		OLED_Print_Num(30,1,temp_T);
		OLED_P8x16Str(70,1,(uint_8 *)"`C");
//		//��ʱ1s��ͨ��TPM����Ӳ����ʱ��
//		delay_ms(1000);
		//��ת����
		light_change(LIGHT_BLUE);
	}
}
//���ܣ�ͨ��GPRS�������ݵ�������
//��������
//����ֵ����
void Send_Message(char * Ip_Port, uint_8 * Sender_AndroidEnd)
{
	//����ָ��AT AT+CPIN?
	if(GPRS_init() == 1)
	{
		//�ж�ָ�AT+CIPSTATUS=0  AT+CGATT=1  AT+CGACT=1,1 AT+CIPSTART="TCP","47.94.153.56",8888
		if(GPRS_Judg_Conn(Ip_Port) == 1)
		{
			//����ָ� AT+CIPSEND=1
			if(GPRS_Send_Data(Sender_AndroidEnd) == 1)
			{
				light_change(LIGHT_GREEN);
			}
		}
	}

}

//���ܣ�������ʱ���������ݲɼ������ݰ��ķ���
//��������
//����ֵ��uint_8 flag  1 ִ��  0 ��ִ��
uint_8  Collection_time()
{
	uint_8 flag = 0;
	if(Control_Coltime == 0x04)
	{
		//��ʱ6��(����)
//		if(tpm_count >= 600)
//		{
//			tpm_count = 0;
//
//		}
		delayms(600);
		flag = 1;

	}
	if(Control_Coltime == 0x08)
	{
//		//��ʱ20�루���ԣ�
//		if(tpm_count >= 2000)
//		{
//
//			tpm_count = 0;
//		}
		delayms(2000);
		flag = 1;
	}
	return flag;
}


//����:�������ݵ�CRCУ���룬���8λУ����
//������uint_8 data
//����ֵ��uint_8 crcV
//����֤��û������ //�����Ƿ���ȷ�Ϳ���ֱ�Ӱ�����ͨ��CRCУ��ķ�ʽ�ٴν�����֤�������У�����Ƿ���ȼ��ɡ�
uint_8 Crc_8(uint_8 data)
{
   uint_8 crcM = 0x23;
   uint_8 crcV = 0x0;
   int i;
   crcV = data;
   for (i =0; i<8; i++)
   {
   		if ((crcV & 0x80) == 0x80)
   		{
            crcV = (uint_8)((crcV << 0x01) ^ crcM);
         }
         else
           {
              crcV = (uint_8)(crcV << 0x01);
           }
    }
    return crcV;
}



//���ܣ�����KL25�����𡢲ɼ�ʱ���
//��������
//����ֵ����
void Android_Control()
{
	if(Received_flag == 1)
	{
		//���ͷ�����ȷ����Ϣ
		GPRS_Send_Data((uint_8 *)"FF");
		if(Date_Re[0] == 0x01)
		{
			if(Date_Re[1] == 0x01)
			{
				light_control(LIGHT_GREEN,1);
				light_control(LIGHT_BLUE,1);
				//�����
				light_control(LIGHT_RED,0);
				red_state = 0x01;
			}
			if(Date_Re[1] == 0x00)
			{
				light_control(LIGHT_GREEN,1);
				light_control(LIGHT_BLUE,1);
				//�����
				light_control(LIGHT_RED,1);
				red_state = 0x00;
			}
			//ֱ�ӽ����Ʋɼ�ʱ�丳ֵ��Control_Coltime
			Control_Coltime = Date_Re[2];
		}
		//ÿ�ν��붼Ҫ�������㣬����ֵ
		Received_flag = 0;
	}
}
