#include "gprs.h"


//功能：执行对附着->激活->连接的进一步在操作（也就是一些主要配置）
//参数:char *Server_IP_Port
//返回值：为uint_8  = 1表示连接成功   < 0 表示连接失败
int_8 GPRS_Config(char *Server_IP_Port);
//功能：将数字处理成字符串形式
//参数：uint_16 AT_numlength :表示数组长度
//char *AT_NUM:局部变量->传入地址，用于存储转化后的字符串（Pack_String函数的局部变量）
//uint_16 length_value : 数值(现在用于字节长度)uint_16 的值表示大于0的值
//返回值： (不需要返回值，因为传入是地址，所以可以进行改变实参)
void Num_To_String(char *AT_NUM, uint_16 num_length, uint_16 length_value);

//================================组合使用的函数============================================
//功能：发送AT指令，设置比较的回发的字符串，设置等待时间等，同时用于判断AT指令是否发送成功
//参数：char *AT_command ：发送的AT指令, char *com_val ：设置比较的回发的字符串, uint_16 wait_time_MS:设置等待时间
//返回值：1：发送成功，0：发送失败
uint_8 Send_AT_Command(char *AT_command, char *com_val,uint_16 wait_time_Ms);
//主要是为了能够直接进行ASCII码值进行查找
//功能：发送AT指令，设置比较的回发的字符串，设置等待时间等，同时用于判断AT指令是否发送成功
//参数：char *AT_command ：发送的AT指令, uint_8 com_val ：设置比较的回发的字符串, uint_16 wait_time_MS:设置等待时间
//返回值：1：发送成功，0：发送失败
//uint_8 Send_AT_Command_Data(char *AT_command, uint_8 com_val, uint_16 wait_time_Ms);

//====================================内部使用的函数==============================

//功能：通过strcmp语句判断Data_Buf和com_val看他们是否相等，返回相应值
//参数：char *com_val
//返回值：1：表示相等 ，0：表示不相等
uint_8 judge_right(char *com_val);

//功能：通过for循环语句判断所需要的的ASCII码值，，
uint_8 judge_ture(uint_8 com_val);

//功能：发送指令，加入\r\n，并且清除了相关的缓冲区
void send_Command(char *AT_command);


//===============================================================================

//功能：发送AT指令，设置比较的回发的字符串，设置等待时间等，同时用于判断AT指令是否发送成功
//参数：char *AT_command ：发送的AT指令, char *com_val ：设置比较的回发的字符串, uint_16 wait_time_MS:设置等待时间
//返回值：TRUE：发送成功，FALSE：发送失败
uint_8 Send_AT_Command(char *AT_command, char *com_val, uint_16 wait_time_Ms)
{
	int i = 3;
	//清空相应的缓冲区
	for(i=0; i<BUF_MAX; i++)
	{
		Data_Buf[i] = '\0';
	}
	//通过strcat函数合并AT_command传入的是一个固定长度的字符串如"AT"
//	strcat(AT_command,"\r\n");
	//通过UART2串口发送命令。
	uart_send_string(UART_2,AT_command);
	//延时wait_time_MS ms
	delayms(wait_time_Ms);
	//判断返回值和uart2的接收缓冲区的字符串对比
	if(judge_right(com_val) == 1)
	{
		//则以为发送成功
		return TRUE;
	}
	//重发四次
	else
	{
		while(1)
		{
			//发送AT指令
			send_Command(AT_command);
			//延时函数
			delayms(wait_time_Ms);
			//判断返回值和uart2接收缓冲区的字符串进行对比
			if(judge_right(com_val) == 1)
			{
				//发送成功
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

//功能：通过strstr语句判断在Data_Buf中 查找第一次出现字符串com_val的值
//参数：char *com_val
//返回值：1：表示相等 ，0：表示不相等
//uint_8 judge_right(char *com_val)
//{
//	//通过strstr函数，可以在Data_Buf中查找第一次出现字符串com_val，判断是否能找到，
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



////主要是为了能够直接进行ASCII码值进行查找,仅仅用于发送AT+QISEND=指令，可以进行发送数据的字节数据的输入。可以进行数据字长的输入
////功能：发送AT指令，设置比较的回发的字符串，设置等待时间等，同时用于判断AT指令是否发送成功
////参数：char *AT_command ：发送的AT指令,uint_8 com_val ：表示'>'的ASCII码值，进行对照, uint_16 wait_time_MS:设置等待时间
////返回值：TRUE：发送成功，FALSE：发送失败
//uint_8 Send_AT_Command_Data(char *AT_command, uint_8 com_val, uint_16 wait_time_Ms)
//{
//	int i = 3;
//	//发送AT命令到串口2
//	send_Command(AT_command);
//	//延时wait_time_MS ms
//	delayms(wait_time_Ms);
//	//判断返回值和uart2的接收缓冲区的字符串对比
//	if(judge_ture(com_val) == 1)
//	{
//		//则以为发送成功
//		return TRUE;
//	}
//	//重发四次
//	else
//	{
//		while(1)
//		{
//			//发送AT命令到串口2
//			send_Command(AT_command);
//			//延时函数
//			delayms(wait_time_Ms);
//			//判断返回值和uart2接收缓冲区的字符串进行对比
//			if(judge_ture(com_val) == 1)
//			{
//				//发送成功
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

//功能：通过for循环语句判断所需要的的ASCII码值，，
//参数：char *com_val
//返回值：TRUE：表示相等 ，FALSE：表示不相等
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


//功能：发送指令，加入\r\n，并且清除了相关的缓冲区
//参数：char *AT_command
//返回值：无
void send_Command(char *AT_command)
{
	int i;
	//清空相应的缓冲区
	for(i=0; i<BUF_MAX; i++)
	{
		Data_Buf[i] = '\0';
	}
	//通过strcat函数合并AT_command传入的是一个固定长度的字符串如"AT"
//	strcat(AT_command,"\r\n");
	//通过UART2串口发送命令。
	uart_send_string(UART_2,AT_command);
	uart_send_string(UART_2,"\r\n");
}


//==================================================GPRS外部使用函数======================================
//功能：测试相关一些基础命令，进行连接（AT测试，查询卡是否插好）
//参数：void
//返回值：uint_8 测试程序 测试成功 返回1 失败返回 0
int_8 GPRS_init(void)
{
	int_8 flag = 0;
	//发送AT指令，测试是否有回发
	if(Send_AT_Command("AT","OK",5) == 1)
	{
		//发送AT+CPIN?是否卡插好
		if(Send_AT_Command("AT+CPIN?","READY",15) == 1)
		{
			//测试成功标志位
			flag = TEST_OK;
			goto end;
		}
		else
		{
			//返回-2 没有准备好标志位
			flag = NO_SIM_CARD_ERROR;
			goto end;
		}
	}
	else
	{
//		返回-22 AT测试没有准备好标志位
		flag = AT_ERROR;
		goto end;
	}
	//返回flag标志
	end: return flag;
}

//功能：主要是判断是否连接成功？？
//参数：char *Server_IP_Port
//返回值：int_8 1，连接成功 ，小于0 表示连接失败
uint_8 GPRS_Judg_Conn(char *Server_IP_Port)
{
	uint_8 flag  = 0;
	//查询GPRS的状态是否连接成功 200ms
	if(Send_AT_Command("AT+CIPSTATUS=0","CONNECT OK",15) == 1)
	{
		//连接成功标志位
		flag = CONNEXCT_OK;
		goto end;
	}
	//查询GPRS连接状态为 IP CLOSE 状态，，可以直接进行连接进行，就可以直接传输数据
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP INITIAL",15) == 1)
	{
		//如果配置GPRS返回值为1，则表示成功连接
		if(GPRS_Config(Server_IP_Port) == 1)
		{
			//连接成功标志位
			flag = CONNEXCT_OK;
			goto end;
		}
		//配置小于0 ，则产生错误，可以进行查看那个错误//这边还是继续运行两次
//		else if(GPRS_Config(Server_IP_Port) < 0)
		else
		{
			//连接失败标志位
			flag = DIS_CONNECT;
			goto end;
		}
	}
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP GPRSACT",15) == 1)
	{
		if(Send_AT_Command(Server_IP_Port,"CONNECT OK",30) == 1)
		{
				//连接成功标志位
				flag = CONNEXCT_OK;
				goto end;
		}
	}
	//执行对附着->激活->连接的进一步在操作
	else if(Send_AT_Command("AT+CIPSTATUS=0","IP CLOSE",15) == 1)
	{
		if(Send_AT_Command(Server_IP_Port,"CONNECT OK",30) == 1)
		{
				//连接成功标志位
				flag = CONNEXCT_OK;
				goto end;
		}
	}
	end:return flag;
}

//功能：发送数据
//参数：uint_8 Data:发送的数据 char * AT_QISEND：表示main函数的局部变量AT+QISEND=length的指令数组
//返回值：uint_8 1，表示发送数据成功，0 表示发送数据失败
int_8 GPRS_Send_Data(uint_8 * Sender_AndroidEnd)
{
	int_8 flag = 0;
	//发送数据(发送数据通过 AT+QISEND 62 表示ASCII值 “>”,50表示50ms)
	if(Send_AT_Command("AT+CIPSEND=1",">",7)==1)
	{
//		//串口发送数据
//		uart_send_string(UART_2,Data);
//		if(Send_AT_Command(Sender_AndroidEnd,"OK",100)==1)
//		{
		//通过strcat函数合并AT_command传入的是一个固定长度的字符串如"AT"
	//	strcat(AT_command,"\r\n");
		//通过UART2串口发送命令。
		uart_sendN(UART_2,7,Sender_AndroidEnd);
		//延时wait_time_MS ms
		delayms(100);
			//设置发送成功标志
		flag = SEND_OK;
		goto end;
	}
	else
	{
		//返回发送错误 -14
		flag = AT_CIPSEND_ERROR;
		goto end;
	}
	end: return flag;
}

//功能：执行对附着->激活->连接的进一步在操作（也就是一些主要配置）
//参数:char *Server_IP_Port
//返回值：为uint_8  = 1表示连接成功   < 0 表示连接失败
int_8 GPRS_Config(char *Server_IP_Port)
{
	int_8 flag = 0;
	//附着网络，，3s
	if(Send_AT_Command("AT+CGATT=1","OK",20))
	{
		//激活网络 2S
		if(Send_AT_Command("AT+CGACT=1,1","OK",50))
		{
			//连接网络 2S
			if(Send_AT_Command(Server_IP_Port,"CONNECT OK",40))
			{
				//连接成功标志
				flag = CONNEXCT_OK;
				goto end;
			}
			else
			{
				//返回连接失败值 -13
				flag = AT_CIPSTART_ERROR;
				goto end;
			}
		}
		else
		{
			//激活网络失败
			flag = AT_CGACT_ERROR;
			goto end;
		}
	}
	else
	{
		//返回附着网络错误 -11
		flag = AT_CGATT_ERROR;
		goto end;
	}
	end:return flag;
}

//功能：清理发送的数据缓冲区
//参数：无
//返回值： 无
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
//功能：计算总的传输字节的长度，判断字符串长度是否越界
//参数：uint_8 *Data_Send  字符串数组
//返回值：return length :返回字符串的长度  return LENGH_OUT 表示超出传送范围
uint_16 Caculate_Length(char *Data_Send)
{
	uint_16 length = 0;
	length = strlen(Data_Send);
	if(length == 0 || length > 1024)
	{
		//超出传送范围
		return LENGTH_OUT;
	}
	else
	{
		//返回字符串的长度
		return length;
	}

}

//功能：将字符串长度和AT+QISEND= 指令一起打包成字符串(字符串的连接也可以用strcat函数)
//参数：uint_16 AT_numlength :表示数组长度
//char *AT_QISEND:局部变量->传入地址，用于存储转化后的字符串(main函数的局部变量)
//uint_16 length_value : 数值(现在用于字节长度)uint_16 的值表示大于0的值
//返回值：无
void Pack_String(char *AT_QISEND, uint_16 AT_numlength, uint_16 length_value)
{
	//定义一个局部变量来接收存储的数据长度值的字符串转换
	char AT_num[5];
	//直接进行将数值整合成字符串
	Num_To_String(AT_num,5,length_value);
	//使用sprintf直接将AT+QISEND=指令封装成字符串
	sprintf((char *)AT_QISEND,"AT+QISEND=");
	//通过strcat函数将AT_QISEND数组和AT_NUM数组连接在一起，存储在AT_QISEND数组中
	strcat(AT_QISEND,AT_num);
}

//多余测试部分但让具有参考价值
//	uint_8 i;
//	for(i=0; i<AT_QISEND_LENGTH; i++)
//	{
//		AT_QISEND[i] = '\0';
//	}
//	char AT_QISEND1[20];
//为什么这样的sprintf没办法直接用，，
//	sprintf((char *)AT_QISEND1,"AT+QISEND=%d",length);

//功能：将数字处理成字符串形式
//参数：uint_16 AT_numlength :表示数组长度
//char *AT_NUM:局部变量->传入地址，用于存储转化后的字符串（Pack_String函数的局部变量）
//uint_16 length_value : 数值(现在用于字节长度)uint_16 的值表示大于0的值
//返回值： (不需要返回值，因为传入是地址，所以可以进行改变实参)
void Num_To_String(char *AT_NUM, uint_16 num_length, uint_16 length_value)
{
	//必须为int型，在下面要判断i小于0
	int i=0;
	uint_8 j=0;
	//做一个中转，传递给AT_NUM
	char temp_num[num_length];
	if(length_value > 0)
	{
		//直接可以通过while的判断退出循环
		while(length_value > 0)
		{
			//实现临时数组的赋值和i的自加，直接+'0'，就算忘记ASCII码值，也不怕。
			temp_num[i++] = length_value % 10 + '0';
			length_value /= 10;
		}
		temp_num[i] = '\0';
		//从高到低进行反向替换
		//for循环和while循环一样，如果判断是执行，不是不执行。
		//请参考C#聊天窗口学习笔记(先执行初始化，在进行判断（对->执行循环语句，错->跳出循环），其次进行最后一部分程序的执行)
		for(i-=1; i>=0; i--,j++)
		{
			AT_NUM[j] = temp_num[i];
		}
		AT_NUM[j] = '\0';
	}
	else
	{
		//不但实现了赋值，还实现了i自加1
		//temp_num[i++] = 48;			//用0的ASCII码值进行相应赋值
		AT_NUM[i++] = '0';
		AT_NUM[i] = '\0';
	}
}
