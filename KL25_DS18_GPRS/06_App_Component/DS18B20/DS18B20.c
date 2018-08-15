#include "DS18B20.h"

vuint_16 Temperature = 0;

/*��ʼ��DS18B20ΪGPIO�ں����
 * */
void ds18b20_Init(){
//	gpio_init(DS18B20,GPIO_OUTPUT,1);
	PORTC->PCR[17] |= 0x01<<8;
	ds18b20_Out();				//���
}

/*DS18B20�ĸ�λ����
 * */
void ds18b20_Reset(){
	ds18b20_Out();				//���
	ds18b20_Low();				//����
	delay_us(2178);				//��ʱ500us
//	ds18b20_In();				//����(���벻���ͷ����ߣ�������һЩKL25�İ��ӿ��ԣ�һ�󲿷ֶ�������)
	ds18b20_High();				//�������ͷ����ߣ�
	delay_us(345);				//��ʱ80us
	while(ds18b20_Val());		//�ӳٵ�ֵΪ����ֵ
	while(!ds18b20_Val());		//һֱ�ӳٵ���λ����
}
/*DS18B20��д����
 * */
void ds18b20_Write(uint_8 data){
	uint_8 i;
	ds18b20_Out();					//���
	for(i=0;i<8;i++){
		ds18b20_Low();				//����
		delay_us(40);				//��ʱ10us
		if(data & 0x01){
			ds18b20_High();			//����
		}
		delay_us(192);				//��ʱ45us
		ds18b20_High();				//�������ͷ����ߣ�
		delay_us(12);				//��ʱ1us
		data >>= 1;
	}
}

/*DS18B20�Ķ�һ���ֽڲ���
 * */
uint_8 ds18b20_ReadByte(){
	uint_8 temp;
	uint_8 i;

	for(i=0;i<8;i++){
		temp>>=1;
		ds18b20_Out();			//���
		ds18b20_High();			//����
		ds18b20_Low();			//����
//		ds18b20_In();			//���루�ͷ����ߣ�(���벻���ͷ����ߣ�������һЩKL25�İ��ӿ��ԣ�һ�󲿷ֶ�������)
		ds18b20_High();				//�������ͷ����ߣ�
		delay_us(40);			//��ʱ10us
		if(ds18b20_Val()){
			temp|=0x80;
		}
			delay_us(193);			//��ʱ45us

	}
	return temp;
}

/*�ж��¶ȵ�����
 * */
uint_8 ds18b20_Judge(uint_8 T_H){
	if(T_H & 0xFC){			//�����¶�Ϊ��ֵ
		return 1;
	}
	else{					//�����¶�Ϊ��ֵ
		return 0;
	}
}


/*�����ݲ���
 * */
uint_8 ds18b20_Read(){
	uint_8 T_H,T_L;
	/*1����ʼת��
	 * */
	ds18b20_Reset();
	ds18b20_Write(SKIP_ROM);			//д������ROM���ֽ�����
	ds18b20_Write(FUNCTION_TURN);		//д�뿪ʼת����������
	delay_ms(800);						//��ʱ��Լ800ms
	/*2����ȡ�ݴ�����
	 * */
	ds18b20_Reset();
	ds18b20_Write(SKIP_ROM);			//д������ROM���ֽ�����
	ds18b20_Write(READ_DATA);			//д����ݴ�Ĺ�������
	T_L = ds18b20_ReadByte();			//�������λ�ֽڡ����Ͱ�λ
	T_H = ds18b20_ReadByte();			//�����һλ�ֽڡ����߰�λ
	ds18b20_Reset();					//��λ����
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





