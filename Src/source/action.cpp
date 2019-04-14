/*
 * action.cpp
 *
 * Created: 2018/08/23 13:56:34
 *  Author: shun2
 */ 
#include "action.hpp"
#include "ui_control.hpp"
#include "motor_control.hpp"

uint32_t Servo_count = 750; //1.5ms  2ms:1000 1ms:500
bool Victim_front = false;
uint8_t Victim_front_kit = 0;

#warning Program isnt finished
void Drop_kit(uint8_t lr,uint16_t num){ //lr:1:?E 0:??
	if(lr==1){
		for(int i=0;i<=num;i++){
			//PORTK.OUTSET = PIN0_bm|PIN1_bm;
			//PORTK.OUTCLR = PIN2_bm|PIN3_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN1_bm|PIN2_bm;
			//PORTK.OUTCLR = PIN0_bm|PIN3_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN2_bm|PIN3_bm;
			//PORTK.OUTCLR = PIN0_bm|PIN1_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN3_bm|PIN0_bm;
			//PORTK.OUTCLR = PIN1_bm|PIN2_bm;
			HAL_Delay(2);
		}
	}
	else if(lr==0){
		for(int i=0;i<=num;i++){
			//PORTK.OUTSET = PIN3_bm|PIN0_bm;
			//PORTK.OUTCLR = PIN1_bm|PIN2_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN2_bm|PIN3_bm;
			//PORTK.OUTCLR = PIN0_bm|PIN1_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN1_bm|PIN2_bm;
			//PORTK.OUTCLR = PIN0_bm|PIN3_bm;
			HAL_Delay(2);
			//PORTK.OUTSET = PIN0_bm|PIN1_bm;
			//PORTK.OUTCLR = PIN2_bm|PIN3_bm;
			HAL_Delay(2);
		}
	}
	//PORTK.OUTCLR = PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm;
	return;
}

void throw_kit(void){
	for(int i=0;i<=40;i++){
		//PORTK.OUTSET = PIN0_bm;
		//PORTK.OUTCLR = PIN1_bm|PIN2_bm|PIN3_bm;
		HAL_Delay(2);
		//PORTK.OUTSET = PIN1_bm;
		//PORTK.OUTCLR = PIN0_bm|PIN2_bm|PIN3_bm;
		HAL_Delay(2);
		//PORTK.OUTSET = PIN2_bm;
		//PORTK.OUTCLR = PIN0_bm|PIN1_bm|PIN3_bm;
		HAL_Delay(2);
		//PORTK.OUTSET = PIN3_bm;
		//PORTK.OUTCLR = PIN0_bm|PIN1_bm|PIN2_bm;
		HAL_Delay(2);
	}
}

void finded_victim(uint8_t co,uint8_t lr){//How many kits does victim need? and Left or Right?
	motor::set_Status(motor::PAUSE);
	led(Redled,1);
	led(Blueled,1);
	led(Greenled,1);
	if(lr==1){
		//Trhow Left
		if(co==1){
			Drop_kit(0);
			Drop_kit(1);
		}
		else if(co==2){
			Drop_kit(0);
			Drop_kit(1);
			Drop_kit(0);
			Drop_kit(1);
			Drop_kit(1);
		}
	}
	else if(lr==3){
		//Trhow Right
		if(co==1){
			Drop_kit(1);
			Drop_kit(0);
		}
		else if(co==2){
			Drop_kit(1);
			Drop_kit(0);
			Drop_kit(1);
			Drop_kit(0);
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
	#warning Not yet//ここはステッピングモーターのピンすべてLOW
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
