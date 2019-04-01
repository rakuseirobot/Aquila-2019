/*
 * uart_control.cpp
 *
 *  Created on: 2019/01/15
 *      Author: shun2
 */
#include "uart_control.hpp"

#include "stm32f4xx_hal.h"
#include <math.h>
#include <string.h>
uart::uart(UART_HandleTypeDef *huart){
	use_uart=huart;
}

uint8_t uart::get(void)
{
	uint8_t c = 0;
	char buf[1];
	HAL_UART_Receive(use_uart,(uint8_t *)buf,sizeof(buf),0xFFFF);
	c=buf[0];
	return c;
}

void uart::string(const char *s){
	HAL_UART_Transmit(use_uart, (uint8_t*)s, strlen(s), 0xFFFF);
}

void uart::putfloat(float data){//float型(-あり)
	uart::putint(data);
	uart::string(".");
	data=fmodf(data,1);
	data*=10;
	uart::putnum(data);
	data=fmodf(data,1);
	data*=10;
	uart::putnum(data);
	data=fmodf(data,1);
	data*=10;
	uart::putnum(data);
	data=fmodf(data,1);
	data*=10;
	uart::putnum(data);
	return;
}
void uart::putint(int64_t data){
	if(0>data){
		uart::string("-");
		data=data*-1;
	}
	uint8_t f = log10f(data)+1;
	if(data==0){
		uart::putnum(0);
	}
	switch(f){
		case 10:
			uart::putnum(data/1000000000);
			data %= 1000000000; // @suppress("break がない case の終わり")
		case 9:
			uart::putnum(data/100000000);
			data %= 100000000; // @suppress("break がない case の終わり")
		case 8:
			uart::putnum(data/10000000);
			data %= 10000000; // @suppress("break がない case の終わり")
		case 7:
			uart::putnum(data/1000000);
			data %= 1000000; // @suppress("break がない case の終わり")
		case 6:
			uart::putnum(data/100000);
			data %= 100000;// @suppress("break がない case の終わり")
		case 5:
			uart::putnum(data/10000);
			data %= 10000;// @suppress("break がない case の終わり")
		case 4:
			uart::putnum(data/1000);
			data %= 1000;// @suppress("break がない case の終わり")
		case 3:
			uart::putnum(data/100);
			data %= 100;// @suppress("break がない case の終わり")
		case 2:
			uart::putnum(data/10);
			data %= 10;// @suppress("break がない case の終わり")
		case 1:
			uart::putnum(data);
			break;
		default:
			return;
	}
}

void uart::send(uint8_t c){
	uint8_t buf[1];
	buf[0]=c;
	HAL_UART_Transmit(use_uart,(uint8_t *)buf,1,0xFFFF);
}
void uart::putnum(uint8_t n){
	uart::send(n + '0');
}
