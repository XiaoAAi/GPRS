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
//在某个库函数中已经定义过了，，
//#define TRUE					(1)
//#define FALSE					(0)


//数组长度
#define BUF_MAX  (100)
//串口2接收的数据缓冲区
char Data_Buf[BUF_MAX];


#define RECI_MAX   (3)
char Date_Re[RECI_MAX];

//接收标志1为接收完毕，0表示未接收完毕
uint_8 Received_flag;

//#define LENGTH_DATA	  (2048)
////串口1 发送的数据缓冲区
//char Data_Send[LENGTH_DATA];



////接收的计数变量(votatile 防止编译器优化)
//volatile uint_8 send_count;


//===============================================================================================
//发送相关的一些函数函数->gprs.c：
//功能：测试相关一些基础命令，进行连接（AT测试，查询卡是否插好）
//参数：void
//返回值：uint_8
int_8 GPRS_init(void);
//功能：主要是判断是否连接成功？？
//参数：char *Server_IP_Port
//返回值：uint_8 1，连接成功 ，0 表示连接失败
uint_8 GPRS_Judg_Conn(char *Server_IP_Port);
//功能：发送数据
//参数：uint_8 Data:发送的数据 char * AT_QISEND：表示main函数的局部变量AT+QISEND=length的指令数组
//返回值：uint_8
int_8 GPRS_Send_Data(uint_8 * Sender_AndroidEnd);
//功能：清理发送的数据缓冲区
//参数：无
//返回值： 无
void Clear_Send_Data(char * Send_AndroidEnd);
//功能：计算总的传输字节的长度，判断字符串长度是否越界
//参数：uint_8 *Data_Send  字符串数组
//返回值：return length :返回字符串的长度  return LENGH_OUT 表示超出传送范围
uint_16 Caculate_Length(char *Data_Send);
//功能：将字符串长度和AT+QISEND= 指令一起打包成字符串(字符串的连接也可以用strcat函数)
//参数：uint_16 AT_numlength :表示数组长度
//char *AT_QISEND:局部变量->传入地址，用于存储转化后的字符串(main函数的局部变量)
//uint_16 length_value : 数值(现在用于字节长度)uint_16 的值表示大于0的值
//返回值：无
void Pack_String(char *AT_QISEND, uint_16 AT_numlength, uint_16 length_value);

#endif

