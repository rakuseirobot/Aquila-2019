/*
 * lcd_control.cpp
 *
 * Created: 2018/08/23 3:52:05
 *  Author: shun2
 */ 

#include "lcd_control.hpp"



void init_lcd(void){
	twi lcd_twi(&TWID,lcd_fre);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x38);	//FunctionSet無効化
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x39);	//FunctionSet有効化
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x14);	//Internal OSC frequency
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x7e);	//コントラスト設定	//70?
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x55);	//Power,ICON,Contrast Control
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x6c);	//Follower Control
	lcd_twi.Stop();
	_delay_ms(500);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x38);	//FunctionSet無効化
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x0c);	//Display ON\OFF Control
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x01);	//ClearDisplay
	lcd_twi.Stop();
	_delay_ms(50);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x06);	//EntryMode??
	lcd_twi.Stop();
	_delay_ms(50);
	return;
}
void lcd_putstr(TWI_t *port, const char *data)
{
	//twi lcd_twi(&TWID,400000);
	twi lcd_twi(&TWID,lcd_fre);
	lcd_twi.Address(LCD1_SLvADD,0);
	uint8_t i=0;
	uint16_t datalong=strlen(data);
	for(i=0; i<datalong; i++)
	{
		if(i==datalong-1){
			lcd_twi.WriteSingle(0x40);
		}
		else{
			lcd_twi.WriteSingle(0xc0);
		}
		lcd_twi.WriteSingle(*data);
		data++;
		_delay_us(10);
	}
	lcd_twi.Stop();
	//_delay_us(500);
	return;
}
void lcd_putmath(TWI_t *port,uint16_t num){
	char data[7]={0};
	data[0]=(num%=10) + '0';
	lcd_putstr(port,data);
	return;
}
void lcd_putdec(TWI_t *port, uint16_t num){
	char data[7]={0};
	data[0]=(num%=1000000)/100000 + '0';
	data[1]=(num%=100000)/10000 + '0';
	data[2]=(num%=10000)/1000 + '0';
	data[3]=(num%=1000)/100 + '0';
	data[4]=(num%=100)/10 + '0';
	data[5]=(num%=10) + '0';
	lcd_putstr(port,data);
	return;
}


void lcd_clear(void){
	//twi lcd_twi(&TWID,400000);
	twi lcd_twi(&TWID,lcd_fre);
	lcd_twi.Address(LCD1_SLvADD,0);
	lcd_twi.WriteSingle(0x00);
	lcd_twi.WriteSingle(0x01);	//ClearDisplay
	lcd_twi.Stop();
	_delay_us(600);
	return;
}
