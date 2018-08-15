#include "DS18B20.h"

vuint_16 Temperature = 0;

/*初始化DS18B20为GPIO口和输出
 * */
void ds18b20_Init(){
//	gpio_init(DS18B20,GPIO_OUTPUT,1);
	PORTC->PCR[17] |= 0x01<<8;
	ds18b20_Out();				//输出
}

/*DS18B20的复位操作
 * */
void ds18b20_Reset(){
	ds18b20_Out();				//输出
	ds18b20_Low();				//下拉
	delay_us(2178);				//延时500us
//	ds18b20_In();				//输入(输入不能释放总线，可能有一些KL25的板子可以，一大部分都不可以)
	ds18b20_High();				//上拉（释放总线）
	delay_us(345);				//延时80us
	while(ds18b20_Val());		//延迟到值为拉低值
	while(!ds18b20_Val());		//一直延迟到复位结束
}
/*DS18B20的写操作
 * */
void ds18b20_Write(uint_8 data){
	uint_8 i;
	ds18b20_Out();					//输出
	for(i=0;i<8;i++){
		ds18b20_Low();				//下拉
		delay_us(40);				//延时10us
		if(data & 0x01){
			ds18b20_High();			//上拉
		}
		delay_us(192);				//延时45us
		ds18b20_High();				//上拉（释放总线）
		delay_us(12);				//延时1us
		data >>= 1;
	}
}

/*DS18B20的读一个字节操作
 * */
uint_8 ds18b20_ReadByte(){
	uint_8 temp;
	uint_8 i;

	for(i=0;i<8;i++){
		temp>>=1;
		ds18b20_Out();			//输出
		ds18b20_High();			//拉高
		ds18b20_Low();			//拉低
//		ds18b20_In();			//输入（释放总线）(输入不能释放总线，可能有一些KL25的板子可以，一大部分都不可以)
		ds18b20_High();				//上拉（释放总线）
		delay_us(40);			//延时10us
		if(ds18b20_Val()){
			temp|=0x80;
		}
			delay_us(193);			//延时45us

	}
	return temp;
}

/*判断温度的正负
 * */
uint_8 ds18b20_Judge(uint_8 T_H){
	if(T_H & 0xFC){			//测量温度为负值
		return 1;
	}
	else{					//测量温度为正值
		return 0;
	}
}


/*读数据操作
 * */
uint_8 ds18b20_Read(){
	uint_8 T_H,T_L;
	/*1、开始转换
	 * */
	ds18b20_Reset();
	ds18b20_Write(SKIP_ROM);			//写入跳过ROM的字节命令
	ds18b20_Write(FUNCTION_TURN);		//写入开始转换功能命令
	delay_ms(800);						//延时大约800ms
	/*2、读取暂存数据
	 * */
	ds18b20_Reset();
	ds18b20_Write(SKIP_ROM);			//写入跳过ROM的字节命令
	ds18b20_Write(READ_DATA);			//写入读暂存的功能命令
	T_L = ds18b20_ReadByte();			//读入第零位字节——低八位
	T_H = ds18b20_ReadByte();			//读入第一位字节——高八位
	ds18b20_Reset();					//复位操作
	if(ds18b20_Judge(T_H)){
		Temperature =(uint_16)(T_H<<8)+(uint_16)T_L;
		Temperature = ((~Temperature)+0x0001);
		Temperature >>= 4;
//		Temperature *= 0.0625;
		return 0;
	}
	else{
		Temperature =(uint_16)(T_H<<8)+(uint_16)T_L;
		Temperature >>= 4;
//		Temperature *= 0.0625;
		return 1;
	}

}





