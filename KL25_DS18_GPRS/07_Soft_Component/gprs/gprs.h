#ifndef _GPRS_H
#define _GPRS_H

#include "includes.h"




#define AT_ERROR			  (-22)
#define COMMUNITE_ERROR        (-1)
#define NO_SIM_CARD_ERROR     (-2)
#define SIM_CARD_NO_REG_ERROR (-3)
#define CALL_ERROR			  (-4)
#define AT_CPMS_ERROR		  (-5)
#define AT_CMGF_ERROR		  (-6)
#define AT_CSCS_ERROR		  (-7)
#define AT_CMGS_ERROR         (-8)
#define END_CHAR_ERROR		  (-9)
#define AT_CGATT_ERROR   	  (-11)
#define AT_CGACT_ERROR	      (-12)
#define AT_CIPSTART_ERROR	  (-13)
#define AT_CIPSEND_ERROR	  (-14)
#define AT_CIPMUX_ERROR       (-15)
#define AT_CIPQSEND_ERROR     (-16)
#define AT_CSTT_ERROR		  (-17)
#define AT_CIICR_ERROR		  (-18)
#define AT_CGATT1_ERROR       (-50)
#define EAT0_ERROR            (-51)

#define LENGTH_OUT				(0)

#define TEST_OK					(1)
#define CONNEXCT_OK				(1)
#define DIS_CONNECT				(0)
#define SEND_OK					(1)
//��ĳ���⺯�����Ѿ�������ˣ���
//#define TRUE					(1)
//#define FALSE					(0)


//���鳤��
#define BUF_MAX  (100)
//����2���յ����ݻ�����
char Data_Buf[BUF_MAX];


#define RECI_MAX   (3)
char Date_Re[RECI_MAX];

//���ձ�־1Ϊ������ϣ�0��ʾδ�������
uint_8 Received_flag;

//#define LENGTH_DATA	  (2048)
////����1 ���͵����ݻ�����
//char Data_Send[LENGTH_DATA];



////���յļ�������(votatile ��ֹ�������Ż�)
//volatile uint_8 send_count;


//===============================================================================================
//������ص�һЩ��������->gprs.c��
//���ܣ��������һЩ��������������ӣ�AT���ԣ���ѯ���Ƿ��ã�
//������void
//����ֵ��uint_8
int_8 GPRS_init(void);
//���ܣ���Ҫ���ж��Ƿ����ӳɹ�����
//������char *Server_IP_Port
//����ֵ��uint_8 1�����ӳɹ� ��0 ��ʾ����ʧ��
uint_8 GPRS_Judg_Conn(char *Server_IP_Port);
//���ܣ���������
//������uint_8 Data:���͵����� char * AT_QISEND����ʾmain�����ľֲ�����AT+QISEND=length��ָ������
//����ֵ��uint_8
int_8 GPRS_Send_Data(uint_8 * Sender_AndroidEnd);
//���ܣ������͵����ݻ�����
//��������
//����ֵ�� ��
void Clear_Send_Data(char * Send_AndroidEnd);
//���ܣ������ܵĴ����ֽڵĳ��ȣ��ж��ַ��������Ƿ�Խ��
//������uint_8 *Data_Send  �ַ�������
//����ֵ��return length :�����ַ����ĳ���  return LENGH_OUT ��ʾ�������ͷ�Χ
uint_16 Caculate_Length(char *Data_Send);
//���ܣ����ַ������Ⱥ�AT+QISEND= ָ��һ�������ַ���(�ַ���������Ҳ������strcat����)
//������uint_16 AT_numlength :��ʾ���鳤��
//char *AT_QISEND:�ֲ�����->�����ַ�����ڴ洢ת������ַ���(main�����ľֲ�����)
//uint_16 length_value : ��ֵ(���������ֽڳ���)uint_16 ��ֵ��ʾ����0��ֵ
//����ֵ����
void Pack_String(char *AT_QISEND, uint_16 AT_numlength, uint_16 length_value);

#endif

