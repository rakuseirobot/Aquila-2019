/*
 * ui_control.cpp
 *
 * Created: 2018/08/23 6:05:21
 *  Author: shun2
 */ 
#include "ui_control.hpp"
#include "delay.hpp"

void buzzer(uint16_t t){
	for(int i=0;i<=1000-t;i++){
		//ブザーピンHIGH
		_delay_us(t);
		//ブザーピンLOW
		_delay_us(t);
	}
}


void led(uint8_t ledkind,uint8_t ledmode){//0:消灯1:点灯3:TGL4:全部消灯
	if(ledmode==1){
		HAL_GPIO_WritePin(FLED_BLUE_GPIO_Port,ledkind,GPIO_PIN_RESET);
	}
	else if(ledmode==0){
		HAL_GPIO_WritePin(FLED_BLUE_GPIO_Port,ledkind,GPIO_PIN_SET);
	}
	else if(ledmode==3){
		HAL_GPIO_TogglePin(FLED_BLUE_GPIO_Port,ledkind);
	}
	else {
		HAL_GPIO_WritePin(FLED_BLUE_GPIO_Port,FLED_BLUE_Pin|FLED_GREEN_Pin|FLED_RED_Pin,GPIO_PIN_SET);
	}
}

void error_led(uint8_t no,uint8_t i){
	if(no==2){
		if(i==1){
			HAL_GPIO_WritePin(E1_GPIO_Port,E1_Pin,GPIO_PIN_SET);
		}
		else if(i==0){
			HAL_GPIO_WritePin(E1_GPIO_Port,E1_Pin,GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_TogglePin(E1_GPIO_Port,E1_Pin);
		}
	}
	else if(no==1){
		if(i==1){
			HAL_GPIO_WritePin(E2_GPIO_Port,E2_Pin,GPIO_PIN_SET);
		}
		else if(i==0){
			HAL_GPIO_WritePin(E2_GPIO_Port,E2_Pin,GPIO_PIN_RESET);
		}
		else{
			HAL_GPIO_TogglePin(E2_GPIO_Port,E2_Pin);
		}
	}
	return;
}


void start(void){
	error_led(1,1);
	error_led(2,0);
	uint8_t timet = 150;
	while(!SW1&&!SW2&&!SW3){
		led(Redled,1);
		led(Blueled,0);
		led(Greenled,0);
		error_led(1,3);
		error_led(2,3);
		_delay_ms(timet);
		led(Redled,0);
		led(Blueled,1);
		led(Greenled,0);
		error_led(1,3);
		error_led(2,3);
		_delay_ms(timet);
		led(Redled,0);
		led(Blueled,0);
		led(Greenled,1);
		error_led(1,3);
		error_led(2,3);
		_delay_ms(timet);
	}
}
