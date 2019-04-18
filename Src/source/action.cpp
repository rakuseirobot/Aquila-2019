/*
 * action.cpp
 *
 * Created: 2018/08/23 13:56:34
 *  Author: shun2
 */ 
#include "action.hpp"
#include "ui_control.hpp"
#include "motor_control.hpp"
#include "stm32f429xx.h"
#include "../peripheral.hpp"
#include "uart_control.hpp"

extern uart xbee;

uint32_t Servo_count = 750; //1.5ms  2ms:1000 1ms:500
bool Victim_front = false;
uint8_t Victim_front_kit = 0;
uint32_t KIT_DROP_COUNT=0;
kit_drop_status_t KIT_DROP_Status=FREE;
#warning Program isnt finished
uint8_t KIT_FRONT_flag[2]={0,0};
void ST_Motor_Move(kit_drop_status_t lr,uint16_t num){
	if(lr==DROP_RIGHT){
		if((10000>=num&&num>8000)||(6000>=num&&num>4000)||(1000>=num)){
			switch(num%4){
				case 0:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_RESET);
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_RESET);
					break;
				case 3:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_RESET);
					break;
				default:
					break;
			}
		}
		else if((8000>=num&&num>6000)||(4000>=num&&num>1000)){
			switch(num%4){
				case 3:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_RESET);
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_RESET);
					break;
				case 0:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_RESET);
					break;
				default:
					break;
			}
		}
	}
	else if(lr==DROP_LEFT){
		if((10000>=num&&num>8000)||(6000>=num&&num>4000)||(1000>=num)){
			switch(num%4){
				case 3:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_RESET);
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_RESET);
					break;
				case 0:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_RESET);
					break;
				default:
					break;
			}
		}
		else if((8000>=num&&num>6000)||(4000>=num&&num>1000)){
			switch(num%4){
				case 0:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
					break;
				case 1:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_RESET);
					break;
				case 2:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin,GPIO_PIN_RESET);
					break;
				case 3:
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH4_Pin|ST_MOTOR_CH1_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin,GPIO_PIN_RESET);
					break;
				default:
					break;
			}
		}
	}
	return;
}
void Drop_kit(kit_drop_status_t lr,uint16_t num){ //lr:1:?E 0:??
	if(lr==DROP_LEFT){
		KIT_DROP_Status=DROP_LEFT;
	}
	else if(lr==DROP_RIGHT){
		KIT_DROP_Status=DROP_RIGHT;
	}
	if(num==2){
		KIT_DROP_COUNT=10000;
	}
	else if(num==1){
		KIT_DROP_COUNT=6000;
	}
	else{
		KIT_DROP_COUNT=0;
	}
	if(num!=0){
		HAL_TIM_Base_Start_IT(&htim2);
	}
	xbee.string("Drop_kit!!");
	return;
}

void finded_victim(uint8_t co,uint8_t lr){//How many kits does victim need? and Left or Right?
	motor::set_Status(motor::PAUSE);
	led(Redled,1);
	led(Blueled,1);
	led(Greenled,1);
	if(lr==1){
		//Trhow Left
		if(co==1){
			Drop_kit(DROP_LEFT,1);
		}
		else if(co==2){
			Drop_kit(DROP_LEFT,2);
		}
	}
	else if(lr==3){
		//Trhow Right
		if(co==1){
			Drop_kit(DROP_RIGHT,1);
		}
		else if(co==2){
			Drop_kit(DROP_RIGHT,2);
		}
	}
	if(lr==2){
		//Do nothing
		for (int i = 0; i<=3; i++){
			error_led(1,3);
			error_led(2,4);
			buzzer();
			HAL_Delay(300);
		}
		Victim_front=true;
		Victim_front_kit=co;
	}
	else{
		Victim_front=false;
	}
	led(Redled,0);
	led(Blueled,0);
	led(Greenled,0);
	error_led(1,1);
	for (int i = 0; i<=5; i++){
		error_led(1,3);
		error_led(2,4);
		buzzer();
		HAL_Delay(300);
	}
	while(KIT_DROP_Status!=FREE){
		xbee.putint(KIT_DROP_COUNT);
		xbee.string("\n\r");
	}
	error_led(1,0);
	error_led(2,0);
}





//static uint16_t EEMEM rectime = 0x1000;
bool record_time(unsigned int time){
	//eeprom_busy_wait();
	//eeprom_write_word(&rectime,time);
	return true;
}
uint16_t read_time(void){
	//eeprom_busy_wait();
	return 1; //eeprom_read_word(&rectime);
}
