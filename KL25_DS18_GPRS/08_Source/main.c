//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//======================================================================

#include "includes.h"   //包含总头文件

//=========构造的函数==========
//读取温度值
void Read_Temperature();
//GPRS发送信息包到服务器
void Send_Message(char * Ip_Port, uint_8 * Sender_AndroidEnd);
//控制延时函数
uint_8  Collection_time();
//控制OLED显示的延时时间，可以进行设置相应延时
uint_8 DelayS(uint_8 timer);
//CRC8位校验码
uint_8 Crc_8(uint_8 data);
//Android端命令的执行
void Android_Control();
//=========全局变量==============

//控制采集时间变量(初始化为1分钟)
uint_8 Control_Coltime = 0x04;
//红灯的状态
uint_8 red_state = 0x00;
//温度值
uint_16 temp_T = 0;

int main(void)
{
	//DS18B20定义的保存字符串数组
	char tempt1[20] ={"Temperature is "};

	//通过TCP协议连接服务器
	char Ip_Port[41] = {"AT+CIPSTART=\"TCP\",\"47.94.153.56\",8888"};

	//发送Android的状态信息数据包(包头、编号1、灯灭、采集时间1min、温度值、温度值校验位、包尾)
	uint_8 Sender_Android[7] = {0xEE,0x01,0x00,0x04,0x00,0x00,0xFF};
	uint_8 Sender_End[7] = {0x00};

//	//测试发送数据
//	uint_8 Received_Test[5] = {0xEE,0x01,0x01,0x08,0xEF};
    //2. 关总中断
    DISABLE_INTERRUPTS;

    //3. 初始化外设模块
    light_init(LIGHT_BLUE, LIGHT_OFF);  //蓝灯初始化
    light_init(LIGHT_GREEN, LIGHT_OFF);  //绿灯初始化
    light_init(LIGHT_RED, LIGHT_OFF);  //红灯初始化

    uart_init(UART_0, 115200);    //初始化串口0，波特率为9600
    uart_init(UART_1, 115200);    //初始化串口1，波特率为115200
    uart_init(UART_2, 115200);    //初始化串口2，波特率为115200
    //uart_init(UART_Debug, 9600);    //初始化UART_Debug，波特率为9600
    /*DS18B20初始化
     * */
    ds18b20_Init();			//初始化DS18B20为GPIO口和输出
    /*OLED初始化
     * */
    OLED_Init();
    //tpm配置初始化
    //最多0xFFFF->16b，也就是65536，也就是说，3000*20=60000<65536是符合的！
    //延时10ms,也就是说每20ms中断一次
//    tpm_timer_init(TPM_2,3000,20);
    //延时10ms,也就是说每10ms中断一次
    tpm_timer_init(TPM_1,3000,10);

    //5. 使能模块中断
    uart_enable_re_int(UART_0);   //使能串口0接收中断
    uart_enable_re_int(UART_1);   //使能串口1接收中断
    uart_enable_re_int(UART_2);   //使能串口2接收中断

//    tpm_enable_int(TPM_2);        //使能TPM0中断
    tpm_enable_int(TPM_1);        //使能TPM1中断
    //6. 开总中断
    ENABLE_INTERRUPTS;   //开总中断
    
	//进入主循环
    //延时至少10s(测试基本不用)
    //delay_s(10);
	//主循环开始==================================================================
	for(;;)
	{
		//延时1s
		if(DelayS(1) == 1)
		{
			//读取温度值，并显示在OLED屏上
			Read_Temperature(tempt1);
		}
//		Send_Message(Ip_Port,Sender_Android);
		//如果控制延时到点了，就执行下列函数
		if(Collection_time() == 1)
		{
			//处理Sender_Android数组到Sender_End数组进行数据打包
			uint_8 i;
			Sender_Android[2] = red_state;
			Sender_Android[3] = Control_Coltime;
			Sender_Android[4] = (uint_8)(temp_T&0xFF);
			Sender_Android[5] = Crc_8((uint_8)(temp_T&0xFF));
			for(i=0; i<7; i++)
			{
				Sender_End[i] = (char)(Sender_Android[i]&0xFF);
			}
			//发送数据到服务器
			Send_Message(Ip_Port,Sender_End);
//			Send_Message(Ip_Port,Sender_Android);
		}
		//android端控制程序
		Android_Control();
//		//延时200ms
//		delay_ms(200);
//		uart_reN(UART_2,5,Received_Test);
		//以下加入用户程序--------------------------------------------------------
	}//主循环end_for
	//主循环结束==================================================================
}

//功能：将读取温度进行一个封装
//参数：无
//返回值：无
void Read_Temperature(char * tempt1)
{
	if(ds18b20_Read()){
		temp_T = Temperature;
//		printf((char *)"温度是：%d `C\n",temp_T);
		OLED_P6x8Str(0,0,(uint_8 *)tempt1);
		OLED_Print_Num(30,1,temp_T);
		OLED_P8x16Str(70,1,(uint_8 *)"`C");
//		//延时1s（通过TPM进行硬件延时）
//		delay_ms(1000);
		//翻转蓝灯
		light_change(LIGHT_BLUE);
	}
	else{
		temp_T = Temperature;
//		printf((char *)"温度是：-%d `C\n",temp_T);
		OLED_P6x8Str(0,0,(uint_8 *)tempt1);
		OLED_Print_Num(30,1,temp_T);
		OLED_P8x16Str(70,1,(uint_8 *)"`C");
//		//延时1s（通过TPM进行硬件延时）
//		delay_ms(1000);
		//翻转蓝灯
		light_change(LIGHT_BLUE);
	}
}
//功能：通过GPRS发送数据到服务器
//参数：无
//返回值：无
void Send_Message(char * Ip_Port, uint_8 * Sender_AndroidEnd)
{
	//测试指令AT AT+CPIN?
	if(GPRS_init() == 1)
	{
		//判断指令：AT+CIPSTATUS=0  AT+CGATT=1  AT+CGACT=1,1 AT+CIPSTART="TCP","47.94.153.56",8888
		if(GPRS_Judg_Conn(Ip_Port) == 1)
		{
			//发送指令： AT+CIPSEND=1
			if(GPRS_Send_Data(Sender_AndroidEnd) == 1)
			{
				light_change(LIGHT_GREEN);
			}
		}
	}

}

//功能：控制延时，进行数据采集和数据包的发送
//参数：无
//返回值：uint_8 flag  1 执行  0 不执行
uint_8  Collection_time()
{
	uint_8 flag = 0;
	if(Control_Coltime == 0x04)
	{
		//延时6秒(测试)
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
//		//延时20秒（测试）
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


//功能:计算数据的CRC校验码，输出8位校验码
//参数：uint_8 data
//返回值：uint_8 crcV
//已验证，没有问题 //检验是否正确就可以直接把数据通过CRC校验的方式再次进行验证看输出的校验码是否相等即可。
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



//功能：控制KL25的亮灭、采集时间等
//参数：无
//返回值：无
void Android_Control()
{
	if(Received_flag == 1)
	{
		//发送服务器确认信息
		GPRS_Send_Data((uint_8 *)"FF");
		if(Date_Re[0] == 0x01)
		{
			if(Date_Re[1] == 0x01)
			{
				light_control(LIGHT_GREEN,1);
				light_control(LIGHT_BLUE,1);
				//红灯亮
				light_control(LIGHT_RED,0);
				red_state = 0x01;
			}
			if(Date_Re[1] == 0x00)
			{
				light_control(LIGHT_GREEN,1);
				light_control(LIGHT_BLUE,1);
				//红灯灭
				light_control(LIGHT_RED,1);
				red_state = 0x00;
			}
			//直接将控制采集时间赋值给Control_Coltime
			Control_Coltime = Date_Re[2];
		}
		//每次进入都要进行清零，赋初值
		Received_flag = 0;
	}
}
