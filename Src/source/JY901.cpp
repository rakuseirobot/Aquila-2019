/*
 * JY901.c
 *
 *  Created on: 2019/01/13
 *      Author: shun2
 */

#include "JY901.hpp"

#include "stm32f4xx_hal.h"
#include <cmath>

jy901::jy901(I2C_HandleTypeDef *jy_i2ct,uint16_t ad){
	jy_i2c = jy_i2ct;
	addr = ad;
}

float jy901::read_angle(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x3f,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((int16_t)(ret[1] << 8) | ret[0])  / 32768 * 180) + 180;
}

float jy901::read_angle_x(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x3d,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((int16_t)(ret[1] << 8) | ret[0])  / 32768 * 180) + 180;
}

float jy901::read_angle_y(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x3e,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((int16_t)(ret[1] << 8) | ret[0])  / 32768 * 180) + 180;
}


float jy901::read_acc_x(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x34,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((ret[1] << 8) | ret[0])  / 32768 * 16 * 9.8);
}

float jy901::read_acc_y(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x35,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((ret[1] << 8) | ret[0])  / 32768 * 16 * 9.8);
}

float jy901::read_acc_z(void){
	int16_t ret[2];
	HAL_I2C_Mem_Read(jy_i2c,addr,0x36,I2C_MEMADD_SIZE_8BIT,(uint8_t*) ret,2,0xFFFF);
	return ((float)((ret[1] << 8) | ret[0])  / 32768 * 16 * 9.8);
}
/*
float jy901::read_moving_angle(void){
	float x=jy901::read_acc_x(),y=jy901::read_acc_y();
	float s = acosf(fabsf(y)/hypotf(x,y)*M_PI);
	if(y<0){
		return (s+1)*180;
	}
	else if(y==0){
		return (x>0?s:s+1)*180;
	}
	else{
		return (s)*180;
	}
}*/
