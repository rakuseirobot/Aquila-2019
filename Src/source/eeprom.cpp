/*
 * eeprom.cpp
 *
 * Created: 2019/02/02 18:17:20
 *  Author: shun2
 */ 

#include "eeprom.hpp"
#include "stm32f4xx.h"
#include "../peripheral.hpp"
#include "main.h"
#define EEPROM_ADDR 0xAE

I2C_HandleTypeDef eeprom_i2c = hi2c2;

void erom::WriteSingle(uint16_t eeaddress,uint8_t data ) //0~31999
{
	uint8_t tx[3]={static_cast<uint8_t>(eeaddress >> 8),static_cast<uint8_t>(eeaddress & (uint8_t)0xFF),data};
	HAL_I2C_Master_Transmit(&eeprom_i2c, EEPROM_ADDR<<1,tx,3, 100);
    HAL_Delay(20);
    return;
}
void erom::WritePage(uint16_t pages,uint8_t *data )  //ページ単位での代入!アドレスじゃないよ!  0-500!!
{   
    uint16_t eeaddress=pages*64;
    uint8_t tx[66]={};
    tx[0]=eeaddress >> 8;
    tx[1]=eeaddress & 0xFF;
    for (uint8_t i=0;i<63;i++){
        tx[i+2]=*data;
        data++;
    }
    HAL_I2C_Master_Transmit(&eeprom_i2c, EEPROM_ADDR<<1,tx,66,100);
    HAL_Delay(20);
    return;
}

uint8_t erom::ReadSingle(uint16_t eeaddress) 
{
    uint8_t rdata[1]= {0xFF};
    HAL_I2C_Mem_Read(&eeprom_i2c,EEPROM_ADDR,eeaddress,I2C_MEMADD_SIZE_16BIT,rdata,1,100);
    return rdata[0];
}

void erom::ReadMulti(uint16_t eeaddress,uint8_t *d,uint16_t val) //どこ指定してもいい!
{

    HAL_I2C_Mem_Read(&eeprom_i2c,EEPROM_ADDR,eeaddress,I2C_MEMADD_SIZE_16BIT,d,val,100);
    return;
}
