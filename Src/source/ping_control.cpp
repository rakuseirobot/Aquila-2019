/*
 * ping_control.cpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */
#include "ping_control.hpp"
#include "delay.hpp"
#include "uart_control.hpp"

extern uart xbee;
uart serial_ping = xbee;

uint16_t ping(int no){
	uint16_t ping_data=0;
	switch(no){
		case 1:ping_data=ping_check(PING1_GPIO_Port,PING1_Pin);break;
		case 2:ping_data=ping_check(PING2_GPIO_Port,PING2_Pin);break;
		case 3:ping_data=ping_check(PING3_GPIO_Port,PING3_Pin);break;
		case 4:ping_data=ping_check(PING4_GPIO_Port,PING4_Pin);break;
		case 5:ping_data=ping_check(PING5_GPIO_Port,PING5_Pin);break;
		case 6:ping_data=ping_check(PING6_GPIO_Port,PING6_Pin);break;
		default:break;
	}
	return ping_data;
}
uint16_t ping(ping_ch_t no){
	uint16_t ping_data=0;
	switch(no){
		case LEFT_BACK:ping_data=ping_check(PING1_GPIO_Port,PING1_Pin);break;
		case LEFT_FRONT:ping_data=ping_check(PING2_GPIO_Port,PING2_Pin);break;
		case FRONT:ping_data=ping_check(PING3_GPIO_Port,PING3_Pin);break;
		case RIGHT_FRONT:ping_data=ping_check(PING4_GPIO_Port,PING4_Pin);break;
		case RIGHT_BACK:ping_data=ping_check(PING5_GPIO_Port,PING5_Pin);break;
		case BACK:ping_data=ping_check(PING6_GPIO_Port,PING6_Pin);break;
		default:break;
	}
	return ping_data;
}
uint16_t ping_check (GPIO_TypeDef* port,uint16_t pin){
	//ping(&PORTD,PIN2_bm);
	GPIO_InitTypeDef PING_GPIOHandler;
	htim6.Init.Prescaler=168;
	HAL_TIM_Base_Init(&htim6);
	PING_GPIOHandler.Pull = GPIO_NOPULL;
	PING_GPIOHandler.Speed = GPIO_SPEED_FAST;
	PING_GPIOHandler.Mode = GPIO_MODE_OUTPUT_PP;
	PING_GPIOHandler.Pin = pin;
	HAL_GPIO_Init(port,&PING_GPIOHandler);
	HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET);
	_delay_us(4);
	HAL_GPIO_TogglePin(port,pin);
	PING_GPIOHandler.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(port,&PING_GPIOHandler);
	__HAL_TIM_SET_COUNTER(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(port,pin)==GPIO_PIN_RESET && __HAL_TIM_GET_COUNTER(&htim6)<9500);
	__HAL_TIM_SET_COUNTER(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(port,pin)==GPIO_PIN_SET  && __HAL_TIM_GET_COUNTER(&htim6)<9500);
	HAL_TIM_Base_Stop(&htim6);
	return __HAL_TIM_GET_COUNTER(&htim6);
}
uint8_t c_p(int x){
 if(ping(x)<Sikiti){
	return 1;
 }else if(ping(x)<1400){
    return 2;
 }else if(ping(x)<2360){
	return 3;
 }else if(ping(x)<3200){
	return 4;
 }else{
	return 5;
 }
 return 255;
}

uint16_t smaller(uint16_t x,uint16_t y){
 if(x<y){
  return x;
 }else{
  return y;
 }
}

uint8_t check_ping(int x){//x::direction,return 1,2,3,4,0(0=error)
	switch(x){
		case 0:
			return smaller(c_p(1),c_p(2));
			break;
		case 1:
			return c_p(3);
			break;
		case 2:
			return smaller(c_p(4),c_p(5));
			break;
		case 3:
			return c_p(6);
			break;
		default:
			return 0;
			break;
	}
}

void ping_debug(void){
	for(uint8_t i=1;i<=6;i++){
		serial_ping.putint(ping(i));
		serial_ping.string(",");
	}
	serial_ping.string("\n\r");
	return;
}


