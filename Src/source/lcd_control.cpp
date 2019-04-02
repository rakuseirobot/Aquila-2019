/*
 * lcd_control.cpp
 *
 * Created: 2018/08/23 3:52:05
 *  Author: shun2
 */ 

#include "lcd_control.hpp"
#include <stdint.h>
#include <string.h>
#include "../peripheral.hpp"
#include "stm32f4xx.h"
#include "delay.hpp"

I2C_HandleTypeDef lcd_i2c = hi2c2;


void init_lcd(void){
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x38},2, 100);//FunctionSet無効化
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x39},2, 100);//FunctionSet有効化
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x14},2, 100);//Internal OSC frequency
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x7e},2, 100);//コントラスト設定	//70?
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x55},2, 100);//Power,ICON,Contrast Control
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x6c},2, 100);//Follower Control
	HAL_Delay(500);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x38},2, 100);//FunctionSet無効化
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x0c},2, 100);//Display ON\OFF Control
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x01},2, 100);//ClearDisplay
	HAL_Delay(50);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x06},2, 100);//EntryMode??
	HAL_Delay(50);
	return;
}
void lcd_putstr(const char *data)
{
	//twi lcd_twi(&TWID,400000);
	uint8_t i=0;
	uint16_t datalong=strlen(data);
	uint8_t tx[datalong*2] = {};
	for(i=0; i<datalong; i++)
	{
		if(i==datalong-1){
			tx[i*2-1]=0x40;
		}
		else{
			tx[i*2-1]=0xc0;
		}
		tx[i*2]=*data;
		data++;
		_delay_us(10);
	}
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1,tx,datalong*2,100);
	//_delay_us(500);
	return;
}
void lcd_putmath(uint16_t num){
	char data[7]={0};
	data[0]=(num%=10) + '0';
	lcd_putstr(data);
	return;
}
void lcd_putdec(uint16_t num){
	char data[7]={0};
	data[0]=(num%=1000000)/100000 + '0';
	data[1]=(num%=100000)/10000 + '0';
	data[2]=(num%=10000)/1000 + '0';
	data[3]=(num%=1000)/100 + '0';
	data[4]=(num%=100)/10 + '0';
	data[5]=(num%=10) + '0';
	lcd_putstr(data);
	return;
}


void lcd_clear(void){
	//twi lcd_twi(&TWID,400000);
	HAL_I2C_Master_Transmit(&lcd_i2c, LCD1_SLvADD<<1, (uint8_t*){0x00,0x01},2, 100);
	_delay_us(600);
	return;
}
