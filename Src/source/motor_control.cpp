/*
 * motor_control.cpp
 *
 * Created: 2018/08/23 3:04:47
 *  Author: shun2
 */ 

/*
PB2,PB3 --SS
*/
#include "motor_control.hpp"
#include "lcd_control.hpp"
#include "ping_control.hpp"
#include "ui_control.hpp"
//#include "data_structure.hpp"
#include "mv_control.hpp"
#include "JY901.hpp"
#include "delay.hpp"
#include "uart_control.hpp"
//#include "initializing.hpp"
#include <stdlib.h>
#include "main.h"
#include "../peripheral.hpp"
#include "stm32f4xx.h"
#include <queue>
//#include "core.hpp"
#include <stdint.h>
#include <math.h>
#include "action.hpp"

extern jy901 gyro;
jy901 motor_gyro = gyro;

extern uart xbee;
uart motor_serial = xbee;

extern uart serial;


const int rose=700;
const int16_t longway = 4500;

#if rose>Sikiti*2
#error "Rose's value is wrong!"
//rose=(Sikiti/2)*3
#else
#endif

const int gbno=120;

float P_GAIN=1;
float I_GAIN=0;
float D_GAIN=0.4;


uint8_t data=000;
void init_motor(void){
	M1_Encoder_COUNT=32767;
	M2_Encoder_COUNT=32767;
	//motor::speed=100;
	HAL_GPIO_WritePin(M1_PWM_GPIO_Port,M1_PWM_Pin|M2_PWM_Pin,GPIO_PIN_SET);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	motor::set_pwm(motor::MOTOR_SPEED[SPEED_TARGET]);
	motor::start_timer();
	motor::start_encoder();
}

int16_t smaller_s(int16_t x,int16_t y){
	if(x<y){
		return x;
	}
	else {
		return y;
	}
}

namespace motor{
	float b_angle=0.0;
	std::queue<move_t> Motor_task;
	move_t Task_Before,Task_Save;
	int32_t Motor_target,mtasksize;
	int32_t Right_count,Left_count;
	int32_t MOTOR_SPEED[5]={0,0,750,400,300},MOTOR_COUNT[2][2]={{0,0},{0,0}};
	int32_t MOTOR_PID_var[2][3];
	int32_t lkasan,rkasan,ldevn,rdevn,ldevp,rdevp,lpwm,rpwm;
	task_status_t Right_Motor_Status=FREE,Left_Motor_Status=FREE,mstatus;
	uint16_t SAVE_PWM[2]={0,0};
	void start_timer(void){
		HAL_TIM_Base_Start_IT(&htim13);
		return;
	}
	void stop_timer(void){
		HAL_TIM_Base_Stop_IT(&htim13);
		return;
	}
	void set_Status(task_status_t s){
		if(s==FREE){
			set_pwm(MOTOR_SPEED[SPEED_TARGET]);
		}
		else if(s==PAUSE){
			brake(MOTOR_LEFT);
			brake(MOTOR_RIGHT);
			SAVE_PWM[SPEED_RIGHT_MOTOR]=__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2);
			SAVE_PWM[SPEED_LEFT_MOTOR]=__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1);
			set_pwm(0);
		}
		else if(s==RESTART){
			set_pwm(MOTOR_RIGHT,SAVE_PWM[SPEED_RIGHT_MOTOR]);
			set_pwm(MOTOR_LEFT,SAVE_PWM[SPEED_LEFT_MOTOR]);
			set_Status(BUSY);
		}
		else if(s==RETURN){
			switch(Task_Save){
				case ONE_ADVANCE:
				case ONE_BACK:
					Motor_target=ONE_BLOCK;
					break;
				case TWO_ADVANCE:
				case TWO_BACK:
					Motor_target=TWO_BLOCK;
					break;
				case HALF_ADVANCE:
				case HALF_BACK:
					Motor_target=HALF_BLOCK;
					break;
				case RIGHT_TURN_NO_GYRO:
				case LEFT_TURN_NO_GYRO:
					Motor_target=TURN;
					break;
				case BRAKE:
				case RIGHT_TURN:
				case LEFT_TURN:
					break;
				default:
					break;
			switch(Task_Save){
				case ONE_ADVANCE:
				case TWO_ADVANCE:
				case HALF_ADVANCE:
					Right_count=Motor_target;
					Left_count=Motor_target;
					break;
				case ONE_BACK:
				case TWO_BACK:
				case HALF_BACK:
					Right_count=Motor_target*-1;
					Left_count=Motor_target*-1;
					break;
				case RIGHT_TURN:
					//move(RIGHT_TURN);
					break;
				case LEFT_TURN:
					//move(LEFT_TURN);
					break;
				case BRAKE:
					break;
				case RIGHT_TURN_NO_GYRO:
					Right_count=Motor_target*-1;
					Left_count=Motor_target;
					break;
				case LEFT_TURN_NO_GYRO:
					Right_count=Motor_target;
					Left_count=Motor_target*-1;
					break;
				default:
					break;
				}
			}
			set_Status(BUSY);
			Task_Save=BRAKE;
		}
		else if(s==BACK){
			brake(MOTOR_LEFT);
			brake(MOTOR_RIGHT);
			Task_Save=Task_Before;
			switch(Task_Before){
				case ONE_ADVANCE:
				case ONE_BACK:
					Motor_target=ONE_BLOCK;
					break;
				case TWO_ADVANCE:
				case TWO_BACK:
					Motor_target=TWO_BLOCK;
					break;
				case HALF_ADVANCE:
				case HALF_BACK:
					Motor_target=HALF_BLOCK;
					break;
				case RIGHT_TURN_NO_GYRO:
				case LEFT_TURN_NO_GYRO:
					Motor_target=TURN;
					break;
				case BRAKE:
				case RIGHT_TURN:
				case LEFT_TURN:
					break;
				default:
					break;
			switch(Task_Before){
				case ONE_ADVANCE:
				case TWO_ADVANCE:
				case HALF_ADVANCE:
					Right_count=(Motor_target-Right_count)*-1;
					Left_count=(Motor_target-Left_count)*-1;
					break;
				case ONE_BACK:
				case TWO_BACK:
				case HALF_BACK:
					Right_count=(Motor_target-Right_count);
					Left_count=(Motor_target-Left_count);
					break;
				case RIGHT_TURN:
					//move(RIGHT_TURN);
					break;
				case LEFT_TURN:
					//move(LEFT_TURN);
					break;
				case BRAKE:
					break;
				case RIGHT_TURN_NO_GYRO:
					Right_count=(Motor_target-Right_count);
					Left_count=(Motor_target-Left_count)*-1;
					break;
				case LEFT_TURN_NO_GYRO:
					Right_count=(Motor_target-Right_count)*-1;
					Left_count=(Motor_target-Left_count);
					break;
				default:
					break;
				}
			}
			set_Status(BUSY);
		}
		Right_Motor_Status=s;
		Left_Motor_Status=s;
	}
	void stm_studio(void){
		mtasksize=Motor_task.size();
		mstatus=status();
		ldevn=MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW];
		rdevn=MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW];
		ldevp=MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_PAST];
		rdevp=MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_PAST];
		lkasan=MOTOR_PID_var[PID_LEFT_MOTOR][PID_KASAN];
		rkasan=MOTOR_PID_var[PID_RIGHT_MOTOR][PID_KASAN];
		rpwm=__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2);
		lpwm=__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1);
	}
	void pid(){
		int32_t LC=Get_Encoder(MOTOR_LEFT),RC=Get_Encoder(MOTOR_RIGHT);
		Left_count-=LC;
		Right_count+=RC;
		if(Left_count<0){
			back(MOTOR_LEFT);
		}
		else if(Left_count>0){
			forward(MOTOR_LEFT);
		}
		else{
			brake(MOTOR_LEFT);
		}
		if(Right_count<0){
			back(MOTOR_RIGHT);
		}
		else if(Right_count>0){
			forward(MOTOR_RIGHT);
		}
		else{
			brake(MOTOR_RIGHT);
		}
		MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_NOW]=LC;
		MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_NOW]=RC;
		//if(abs(Left_count)>=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN){//目標地点までの偏差が一周期に進むべきカウント数以上の場合
		if(MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_NOW]<0){
			MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW]=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN-MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_NOW]*-1;
		}
		else{
			MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW]=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN-MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_NOW];
		}
		MOTOR_PID_var[PID_LEFT_MOTOR][PID_KASAN]+=MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW];
		
		//}
		//else{}
		//if(abs(Right_count)>=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN){//目標地点までの偏差が一周期に進むべきカウント数以上の場合
		if(MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_NOW]<0){
			MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW]=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN-MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_NOW]*-1;
		}
		else{
			MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW]=MOTOR_SPEED[SPEED_TARGET]*SPEED_GAIN-MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_NOW];
		}
		MOTOR_PID_var[PID_RIGHT_MOTOR][PID_KASAN]+=MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW];
		
		//}
		//else{}
		
		MOTOR_SPEED[SPEED_LEFT_MOTOR]=P_GAIN*MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW]+I_GAIN*MOTOR_PID_var[PID_LEFT_MOTOR][PID_KASAN]+D_GAIN*(MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW]-MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_PAST]);
		MOTOR_SPEED[SPEED_RIGHT_MOTOR]=P_GAIN*MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW]+I_GAIN*MOTOR_PID_var[PID_RIGHT_MOTOR][PID_KASAN]+D_GAIN*(MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW]-MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_PAST]);
		if(motor::status()==motor::BUSY){
			set_pwm(MOTOR_LEFT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)+MOTOR_SPEED[SPEED_LEFT_MOTOR]);
			set_pwm(MOTOR_RIGHT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)+MOTOR_SPEED[SPEED_RIGHT_MOTOR]);
		}
		MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_PAST]=MOTOR_COUNT[PID_LEFT_MOTOR][PID_COUNT_NOW];
		MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_PAST]=MOTOR_COUNT[PID_RIGHT_MOTOR][PID_COUNT_NOW];
		MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_PAST]=MOTOR_PID_var[PID_LEFT_MOTOR][PID_DEV_NOW];
		MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_PAST]=MOTOR_PID_var[PID_RIGHT_MOTOR][PID_DEV_NOW];
		if(__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)<80){
			set_pwm(MOTOR_LEFT,MOTOR_SPEED[SPEED_FIX_TARGET]);
		}if(__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)<80){
			set_pwm(MOTOR_RIGHT,MOTOR_SPEED[SPEED_FIX_TARGET]);
		}
	}
	void start_encoder(void){
		HAL_TIM_Encoder_Start_IT(M1_TIM_CHANNEL,TIM_CHANNEL_ALL);
		HAL_TIM_Encoder_Start_IT(M2_TIM_CHANNEL,TIM_CHANNEL_ALL);
		M1_Encoder_COUNT=32767;
		M2_Encoder_COUNT=32767;
		return;
	}
	void stop_encoder(void){
		HAL_TIM_Encoder_Stop_IT(M1_TIM_CHANNEL,TIM_CHANNEL_ALL);
		HAL_TIM_Encoder_Stop_IT(M2_TIM_CHANNEL,TIM_CHANNEL_ALL);
		return;
	}
	void check_job(){
		if(check_task()!=FREE){
			switch(Motor_task.front()){
			case ONE_ADVANCE:
			case ONE_BACK:
				Motor_target=ONE_BLOCK;
				break;
			case TWO_ADVANCE:
			case TWO_BACK:
				Motor_target=TWO_BLOCK;
				break;
			case HALF_ADVANCE:
			case HALF_BACK:
				Motor_target=HALF_BLOCK;
				break;
			case RIGHT_TURN_NO_GYRO:
			case LEFT_TURN_NO_GYRO:
				Motor_target=TURN;
				break;
			case BRAKE:
			case RIGHT_TURN:
			case LEFT_TURN:
				break;
			default:
				break;
			}
			switch(Motor_task.front()){
			case ONE_ADVANCE:
			case TWO_ADVANCE:
			case HALF_ADVANCE:
				Right_count+=Motor_target;
				Left_count+=Motor_target;
				break;
			case ONE_BACK:
			case TWO_BACK:
			case HALF_BACK:
				Right_count-=Motor_target;
				Left_count-=Motor_target;
				break;
			case RIGHT_TURN:
				move(RIGHT_TURN);
				break;
			case LEFT_TURN:
				move(LEFT_TURN);
				break;
			case BRAKE:
				break;
			case RIGHT_TURN_NO_GYRO:
				Right_count-=Motor_target;
				Left_count+=Motor_target;
				break;
			case LEFT_TURN_NO_GYRO:
				Right_count+=Motor_target;
				Left_count-=Motor_target;
				break;
			default:
				break;
			}
			Motor_task.pop();
			set_Status(BUSY);
		}
	}
	void task_add(move_t ta){
		Motor_task.push(ta);
	}
	task_status_t check_task(){
		if(Motor_task.size()!=0){
			return BUSY;
		}
		else{
			return FREE;
		}
	}
	int32_t Get_Encoder(ch_t x){
		if(x==MOTOR_LEFT){
			if(M1_Encoder_COUNT<32767){
				return (32767-M1_Encoder_COUNT)*-1;
			}
			else{
				return (M1_Encoder_COUNT-32767);
			}
		}
		else if (x==MOTOR_RIGHT){
			if(M2_Encoder_COUNT<32767){
				return (32767-M2_Encoder_COUNT)*-1;
			}
			else{
				return (M2_Encoder_COUNT-32767);
			}

		}
		return 400;
	}
	void brake(ch_t x){
		if(x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M1_INA_GPIO_Port,M1_INA_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_INB_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INA_Pin|M2_INB_Pin|M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void forward(ch_t x){
		if(x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M1_INA_GPIO_Port,M1_INA_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_INB_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M1_EN_GPIO_Port,M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INA_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M2_INB_GPIO_Port,M2_INB_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M2_EN_GPIO_Port,M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void back(ch_t x){
		if(x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M1_INA_GPIO_Port,M1_INA_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_INB_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_EN_GPIO_Port,M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INA_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M2_INB_GPIO_Port,M2_INB_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M2_EN_GPIO_Port,M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void set_pwm(ch_t x,uint16_t sp){
		if(sp>1000){
			sp=1000;
		}
		if(sp<0){
			sp=0;
		}
		if(x==MOTOR_LEFT){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sp);//1
		}
		else if (x==MOTOR_RIGHT){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sp);//2
		}
		else{

		}
		return;
	}
	void set_pwm(uint16_t sp){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sp);//1
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sp);//2
	}
	task_status_t status(void){//1:free 2:busy
		if(Left_Motor_Status==PAUSE&&Left_Motor_Status==PAUSE){
			return PAUSE;
		}
		if(Left_Motor_Status==NOPID||Right_Motor_Status==NOPID){
			return NOPID;
		}
		if(Left_Motor_Status!=FREE||Right_Motor_Status!=FREE){
			return BUSY;
		}
		else{
			return FREE;
		}
	}
	task_status_t status(motor::ch_t m){//1:free 2:busy
		if(m==MOTOR_RIGHT&&Right_Motor_Status!=FREE){
			return BUSY;
		}
		else if(m==MOTOR_LEFT&&Left_Motor_Status!=FREE){
			return BUSY;
		}
		else{
			return FREE;
		}
	}
	void wait(bool check){
		do{
			if(MV_RECIEVED_DATA[MV_DATA_TYPE]!=FIND_NOTHING){		
				/*HAL_NVIC_DisableIRQ(MVS1_EXTI_IRQn);
				HAL_NVIC_DisableIRQ(MVS2_EXTI_IRQn);
				HAL_NVIC_DisableIRQ(MVS3_EXTI_IRQn);*/
				if((MV_RECIEVED_DATA[MV_DATA_DIR]==MV_LEFT||MV_RECIEVED_DATA[MV_DATA_DIR]==MV_RIGHT)||(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT&&(Task_Before==ONE_BACK||Task_Before==TWO_BACK))){
					mv_task_check();
				}
			}/*
			if(abs(Right_count)<=Motor_thre){
				motor::brake(motor::MOTOR_RIGHT);
				HAL_TIM_Encoder_Stop(M2_TIM_CHANNEL,TIM_CHANNEL_ALL);
				Right_count=0;
			}
			if(abs(Left_count)<=Motor_thre){
				motor::brake(motor::MOTOR_LEFT);
				HAL_TIM_Encoder_Stop(M1_TIM_CHANNEL,TIM_CHANNEL_ALL);
				Left_count=0;
			}
			if(abs(Right_count)<=Motor_thre&&abs(Left_count)<=Motor_thre){
				break;
			}*/
			if(motor::status()!=BUSY){
				break;
			}
		}while(check_task()!=FREE||(abs(Right_count)>=Motor_thre||abs(Left_count)>=Motor_thre));
		motor::start_encoder();

		if(MV_RECIEVED_DATA[MV_DATA_TYPE]!=FIND_NOTHING){
			mv_after_stop_task_check();
		}
		HAL_NVIC_EnableIRQ(MVS1_EXTI_IRQn);
		HAL_NVIC_EnableIRQ(MVS2_EXTI_IRQn);
		HAL_NVIC_EnableIRQ(MVS3_EXTI_IRQn);
		return;
	}
	void move(move_t x){// x = 0:1 block Advance 1:2 blocks Advance 2:Right Turn with Gyro 3:Left Turn with Gyro 4:1 block Back 5:2 block Back 6:Half block Advance 7:Half block Back 8:right Turn without Compass 9:left Turn without Compass 
		b_angle=motor_gyro.read_angle();
		Right_count=0;
		Left_count=0;
		if((x!=LEFT_TURN&&x!=RIGHT_TURN)&&x!=BRAKE){
			set_Status(BUSY);
		}
		MOTOR_SPEED[SPEED_LEFT_MOTOR]=0;
		MOTOR_SPEED[SPEED_RIGHT_MOTOR]=0;
		set_pwm(MOTOR_SPEED[SPEED_TARGET]);
		float first = 0;
		float now = 0;
		bool st_f = false;
		mv_cap(MV_LEFT,true);
		mv_cap(MV_FRONT,true);
		mv_cap(MV_RIGHT,true);
		motor::wait();
		if(x!=BRAKE){
			Task_Before=x;
		}
		serial.putint(x);
		serial.string("<<\n\r");
		switch(x){
			case ONE_ADVANCE: //1block advance
				task_add(x);
				st_f=true;
				//HAL_Delay(300);
			break;
			case TWO_ADVANCE: //2block advance
				task_add(x);
				st_f=true;
				//HAL_Delay(300);
			break;
			case RIGHT_TURN: //Right turn with gyro
				set_Status(NOPID);
				first = motor_gyro.read_angle();
				//motor_serial.putint(first);
				//motor_serial.string("\n\r");
				turn_retry2:
				forward(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				HAL_Delay(2);
				now = motor_gyro.read_angle();
				//motor_serial.putint(now);
				//motor_serial.string("\n\r");
				do{
					now=motor_gyro.read_angle();
					//motor_serial.putint(now);
					//motor_serial.string("\n\r");
				}while((first<now?now-first:now-first+360)>270||(first<now?now-first:now-first+360)<90);
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
				HAL_Delay(100);
				now=motor_gyro.read_angle();
				if((first<now?now-first:now-first+360)>270||(first<now?now-first:now-first+360)<90){
					goto turn_retry2;
				}
				if(first<=90){
					fix_angle_v(first-90+360);
				}
				else{
					fix_angle_v(first-90);
				}
			break;
			case LEFT_TURN: //Left Turn with gyro
				set_Status(NOPID);
				first = motor_gyro.read_angle();
				//motor_serial.putint(first);
				//motor_serial.string("\n\r");
				turn_retry3:
				back(MOTOR_LEFT);
				forward(MOTOR_RIGHT);
				HAL_Delay(2);
				now=motor_gyro.read_angle();
				//motor_serial.putint(now);
				//motor_serial.string("\n\r");
				do{
					now=motor_gyro.read_angle();
					//motor_serial.putint(now);
					//motor_serial.string("\n\r");
				}while((first<now?now-first:now-first+360)<90||(first<now?now-first:now-first+360)>270);
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
				now=motor_gyro.read_angle();
				HAL_Delay(100);
				if((first<now?now-first:now-first+360)<90||(first<now?now-first:now-first+360)>270){
					goto turn_retry3;
				}
				if(first>=270){
					fix_angle_v(first+90-360);
				}
				else{
					fix_angle_v(first+90);
				}
				break;
			case ONE_BACK: //1block back
				task_add(x);
				st_f=true;
			break;
			case TWO_BACK: //2block back
				task_add(x);
				st_f=true;
			break;

			case HALF_ADVANCE: //half advance
				task_add(x);
				st_f=true;
			break;
			case HALF_BACK: //half back
				task_add(x);
				st_f=true;
			break;
			case LEFT_TURN_NO_GYRO: //Left Turn without gyro (????)
				task_add(x);
				//HAL_Delay(300);
			break;
			case RIGHT_TURN_NO_GYRO: //Right Turn without gyro (???)
				task_add(x);
			break;
			default:
				brake(MOTOR_RIGHT);
				brake(MOTOR_LEFT);
			break;
		}
		motor::Right_count=0;
		motor::Left_count=0;
		motor::wait();
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		set_Status(FREE);
		if(st_f==true){
			HAL_Delay(500);
			fix_angle_v(b_angle);
		}
		set_Status(FREE);
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		mv_cap(MV_LEFT,true);
		mv_cap(MV_FRONT,true);
		mv_cap(MV_RIGHT,true);
		set_Status(FREE);
		HAL_Delay(200);
	}
	void forever(void){
		forward(MOTOR_LEFT);
		forward(MOTOR_RIGHT);
		while(ping(FRONT)>=3);
		motor::brake(MOTOR_RIGHT);
		motor::brake(MOTOR_LEFT);
		return;
	}	void fix(uint8_t x,uint8_t ping1,uint8_t ping2,int no){//x=right,left ping1,ping2:Compare ping number no:Sikiiti
		int val=0;
		if(x==1){//left
			back(MOTOR_RIGHT);
			forward(MOTOR_LEFT);
		}
		else if(x==2){//right
			back(MOTOR_LEFT);
			forward(MOTOR_RIGHT);
		}
		while(1){
			val=ping(ping1)-ping(ping2);
			if(abs(val)>=no){
				break;
			}
		}
		motor::brake(MOTOR_RIGHT);
		motor::brake(MOTOR_LEFT);
		return;
	}
	#define fixno 5 //gb_fix Sikiiti
	const int gbbest=175;//185
	uint32_t tbest = 600;
	void gb_fix(void){
		set_Status(NOPID);
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		int16_t dis[3];
		dis[0]=ping(FRONT);//Forward
		dis[1]=ping(PING_BACK);//Back
		float ang=motor_gyro.read_angle_y();
		if(ang<=Ang_slope_Norm-Ang_slope_thre*1.5){
			if(ping(PING_BACK)<=Sikiti/2){
				motor::move(HALF_BACK);
			}
		}
		else if(ang>=Ang_slope_Norm+Ang_slope_thre*1.5){
			if(ping(FRONT)<=Sikiti/2){
				motor::move(HALF_ADVANCE);
			}
		}
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		set_Status(NOPID);
		dis[0]=ping(FRONT);//Forward
		dis[1]=ping(PING_BACK);//Back
		if(Sikiti>=dis[0]){//前の壁基準
			bool flag=false;
			if(FIND_BRICK==MV_FRONT){
				if(!(Sikiti>=dis[1])){
					return;
				}
				else{
					flag=true;
				}
			}
			if(flag==false){
				HAL_Delay(3);
				dis[0]=ping(FRONT);
				if(!(Sikiti>=dis[0])){
					return;
				}
				lcd_clear();
				if((gbbest-dis[0])<fixno*-1){
					lcd_putstr("gb_fixF");
					forward(MOTOR_RIGHT);
					forward(MOTOR_LEFT);
					while(dis[0]>gbbest){
						if(dis[0]>=longway){
							break;
						}
						dis[0]=ping(FRONT);
					}
					motor::brake(MOTOR_LEFT);
					motor::brake(MOTOR_RIGHT);
				}
				else if((gbbest-dis[0])>fixno){
					lcd_putstr("gb_fixF");
					back(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					while(dis[0]<gbbest){
						if(dis[0]>=longway){
							break;
						}
						dis[0]=ping(FRONT);
					}
					motor::brake(MOTOR_LEFT);
					motor::brake(MOTOR_RIGHT);
				}
				lcd_clear();
				return;
			}
		}
		if(Sikiti>=dis[1]){
			HAL_Delay(3);
			dis[1]=ping(PING_BACK);
			if(!(Sikiti>=dis[1])){
				return;
			}
			lcd_clear();
			if((gbbest-dis[1])>fixno){
				lcd_putstr("gb_fixF");
				forward(MOTOR_RIGHT);
				forward(MOTOR_LEFT);
				while(dis[1]<gbbest){
					if(dis[1]>=longway){
						break;
					}
					dis[1]=ping(PING_BACK);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			else if((gbbest-dis[1])<fixno*-1){
				lcd_putstr("gb_fixB");
				back(MOTOR_RIGHT);
				back(MOTOR_LEFT);
				while(dis[1]>gbbest){
					if(dis[1]>=longway){
						break;
					}
					dis[1]=ping(PING_BACK);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			return;
		}
		else{return;}
		return;
	}
	const int32_t turnvalue = 5;
	void turn_fix(uint8_t force){
		set_Status(NOPID);
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		int val=0;
		ping_ch_t chk[2]={};
		if (!force&&smaller_s(ping(LEFT_BACK),ping(LEFT_FRONT))>=Sikiti&&smaller_s(ping(RIGHT_FRONT),ping(RIGHT_BACK))>=Sikiti){
			return;
		}
		else{
			if (smaller_s(ping(LEFT_BACK),ping(LEFT_FRONT))<=Sikiti){
				if(!(ping(LEFT_FRONT)<=Sikiti&&ping(LEFT_BACK)<=Sikiti)){
					return;
				}
				else if(FIND_BRICK==MV_LEFT){
					if(!(smaller_s(ping(RIGHT_BACK),ping(RIGHT_FRONT))<=Sikiti)){
						return;
					}
				}
				else{
					chk[0]=LEFT_BACK;
					chk[1]=LEFT_FRONT;
				}
			}
			if (smaller_s(ping(RIGHT_BACK),ping(RIGHT_FRONT))<=Sikiti){
				if(!(ping(RIGHT_BACK)<=Sikiti&&ping(RIGHT_FRONT)<=Sikiti)){
					return;
				}
				else if(FIND_BRICK==MV_RIGHT){
					return;
				}
				else{
					chk[0]=RIGHT_FRONT;
					chk[1]=RIGHT_BACK;
				}
			}
			//usart_putdec(ping(chk[0]));
			//usart_string(",");
			//usart_putdec(ping(chk[1]));
			val=ping(chk[0])-ping(chk[1]);
			//usart_string(",");
			//usart_puthex2(abs(val));
			//usart_string("\n\r");
			if(val < turnvalue*-1){ //Right Turn
				lcd_clear();
				lcd_putstr("FixingTurn");
				back(MOTOR_LEFT);
				forward(MOTOR_RIGHT);
				do{
					val=ping(chk[0])-ping(chk[1]);
				}while(val<turnvalue*-1);
			}
			else if(val > turnvalue){
				lcd_clear();
				lcd_putstr("FixingTurn");
				forward(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				do{
					val=ping(chk[0])-ping(chk[1]);
				}while(val>turnvalue);
			}
			else{
				lcd_clear();
				//lcd_putstr("Finished!");
			//	usart_string("Needless\n\r");
				return;
			}
			motor::brake(MOTOR_LEFT);
			motor::brake(MOTOR_RIGHT);
			//motor::wait();
			lcd_clear();
			//lcd_putstr("Succees");
			//usart_string("Succees!\n\r");
			//HAL_Delay(1000);
			//lcd_clear();
			return;
			//lcd_putdec(val);
			//HAL_Delay(2);
		}
	}
	
	uint8_t notify_position(void){
		int16_t dis[10];
		for (uint8_t i=0;i<=7;i++){
			dis[i]=ping(i);
		}
		if(smaller_s(dis[2],dis[1])>=Sikiti&&rose>=smaller_s(dis[1],dis[2])){
			lcd_clear();
			lcd_putstr("NotifyP");
			motor::move(RIGHT_TURN);
			motor::move(ONE_ADVANCE);
			motor::move(LEFT_TURN);
			motor::move(ONE_BACK);
			lcd_clear();
			return 1;
		}
		else if(smaller_s(dis[5],dis[4])>=Sikiti&&rose>=smaller_s(dis[5],dis[4])){
			lcd_clear();
			lcd_putstr("NotifyP");
			motor::move(LEFT_TURN);
			motor::move(ONE_ADVANCE);
			motor::move(RIGHT_TURN);
			motor::move(ONE_BACK);
			lcd_clear();
			return 1;
		}
		else{return 0;}
			
		return 0;
	}
	
	
	uint8_t notify_half(uint8_t x){
		uint16_t dis[5];
		uint8_t i = 0;
		dis[0] = ping(LEFT_BACK);
		dis[1] = ping(LEFT_FRONT);
		dis[2] = ping(RIGHT_FRONT);
		dis[3] = ping(RIGHT_BACK);
		if((dis[0]<Sikiti&&!(dis[1]<Sikiti))||(!(dis[0]<Sikiti)&&dis[1]<Sikiti)){
			i=1;
		}else if((dis[2]<Sikiti&&!(dis[3]<Sikiti))||(!(dis[2]<Sikiti)&&dis[3]<Sikiti)){
			i=2;
		}else{
			i=0;
		}
		if(i>0){
			HAL_Delay(100);
			xbee.string("\n\r\x1b[7mLEFT_BACK:");
			xbee.putint(dis[0]);
			xbee.string("LEFT_FRONT:");
			xbee.putint(dis[1]);
			xbee.string("RIGHT_FRONT:");
			xbee.putint(dis[2]);
			xbee.string("RIGHT_BACK:");
			xbee.putint(dis[3]);
			xbee.string("\n\r");
			dis[0] = ping(LEFT_BACK);
			dis[1] = ping(LEFT_FRONT);
			dis[2] = ping(RIGHT_FRONT);
			dis[3] = ping(RIGHT_BACK);
			xbee.string("[２回目]LEFT_BACK:");
			xbee.putint(dis[0]);
			xbee.string("LEFT_FRONT:");
			xbee.putint(dis[1]);
			xbee.string("RIGHT_FRONT:");
			xbee.putint(dis[2]);
			xbee.string("RIGHT_BACK:");
			xbee.putint(dis[3]);
			xbee.string("\x1b[0m\n\r");
			if((dis[0]<Sikiti&&!(dis[1]<Sikiti))||(!(dis[0]<Sikiti)&&dis[1]<Sikiti)){
				i=1;
			}else if((dis[2]<Sikiti&&!(dis[3]<Sikiti))||(!(dis[2]<Sikiti)&&dis[3]<Sikiti)){
				i=2;
			}else{
				i=0;
			}
		}
		if(i==1){
			if(FIND_BRICK==MV_LEFT){
				buzzer(200);
				return 0;
			}
		}
		else if(i==2){
			if(FIND_BRICK==MV_RIGHT){
				buzzer(200);
				return 0;
			}
		}
		else{
		}
		if(i>0){
			lcd_clear();
			lcd_putstr("NotifyHA");
			motor_serial.string("\x1b[42mNotifyHalf\x1b[49m");
			motor_serial.string("\n\r");
			if (x == v::front){
				motor::move(HALF_ADVANCE);
			}
			else if(x == v::back){
				motor::move(HALF_BACK);
			}
			lcd_clear();
			i=1;
		}
		return i;
	}
	uint8_t notify_long_ex(void){
		int16_t dis[3];
		dis[0] = ping(FRONT);
		if(dis[0]>=longway){
			led(Blueled,1);
			lcd_clear();
			lcd_putstr("NotifyL");
			led(Blueled,1);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			dis[0] = ping(FRONT);
			if(gbbest<=dis[0]){
				led(Redled,1);
				do{forward(MOTOR_RIGHT);
					forward(MOTOR_LEFT);
					dis[0] = ping(FRONT);
					dis[1] = smaller_s(ping(LEFT_FRONT),ping(LEFT_BACK));
					dis[2] = smaller_s(ping(RIGHT_BACK),ping(RIGHT_FRONT));/*
					usart_putdec(dis[1]);
					usart_putdec(dis[2]);
					usart_string("\n\r");*/

				}while(/*PORTJ.IN&dev == 0 && */(dis[2]<=Sikiti)&&(dis[1]<=Sikiti)&&(dis[0] >= gbbest));
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			motor::move(HALF_ADVANCE);
			led(Blueled,0);
			led(Redled,0);
			motor_serial.string("saka!");
			motor_serial.string("\n\r");
			motor::fix_position();
			//w_w();
			return 1;
		}
		else{
			return 0;
		}
	}
	uint8_t notify_long(void){
		int16_t dis;
		dis = ping(FRONT);
		if(dis >= longway){
			lcd_clear();
			lcd_putstr("NotifyL");
			led(Blueled,1);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");
			motor::move(ONE_ADVANCE);
			motor::turn_fix();
			lcd_clear();
			lcd_putstr("NotifyL");

			dis = ping(FRONT);
			if(dis >= gbbest){
				led(Redled,1);
				do{
					forward(MOTOR_LEFT);
					forward(MOTOR_RIGHT);
					dis = ping(FRONT);
				}while(dis >= gbbest);
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
				//motor::wait();
				led(Redled,0);
			}
			motor_serial.string("saka!");
			motor_serial.string("\n\r");
			led(Blueled,0);
			motor::gb_fix();
			lcd_clear();
			return 1;
		}
		else{
			return 0;
		}
	}
	
	uint8_t notify_long_acc(uint8_t x,bool buz){//0:なし,1:下り,2:上り
		float ac=motor_gyro.read_acc_x();
		float now=0;
		if(x==v::front){//前進中
			if(ac>=Acc_thre_u){//上り
				if(longway<=ping(FRONT)){
					ac=motor_gyro.read_acc_x();
					if(ac>=Acc_thre_u){
						if(buz==true){
						buzzer(400);
						buzzer(800);
						}
					}
					else{
						return 0;
					}
				}
				else{
					return 0;
				}
				lcd_clear();
				lcd_putstr("NotiL!U");
				forward(MOTOR_RIGHT);
				forward(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				while(ac>=Acc_thre_u){
					ac=motor_gyro.read_acc_x();
					if(!(ac>=Acc_thre_u)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					/*serial.putfloat(ac);
					serial.string(",");
					serial.putfloat(now);
					serial.string("\n\r");*/
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//そこまで傾いていない
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
							#warning Not yet
							//右の速度落とす
							do 
							{
								now=motor_gyro.read_acc_y();

							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							#warning Not yet
							//左の速度落とす
							do
							{
								now=motor_gyro.read_acc_y();
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//大きく傾いている
						led(Redled,1);
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(!(ac>=Acc_thre_u)){
						ac=motor_gyro.read_acc_x();
					}
				}
				led(Redled,0);
				error_led(2,0);
				error_led(1,0);
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				motor::brake(MOTOR_RIGHT);
				motor::brake(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 2;
			}
			else if(ac<=Acc_thre_d){//下り
				if(longway/2<=ping(PING_BACK)){
					ac=motor_gyro.read_acc_x();
					if(ac<=Acc_thre_d){
						if(buz==true){
						buzzer(800);
						buzzer(400);
						}
					}
					else{
						return 0;
					}
				}
				else{
					return 0;
				}
				lcd_clear();
				lcd_putstr("NotiL!D");
				forward(MOTOR_RIGHT);
				forward(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				while(ac<=Acc_thre_d){
					ac=motor_gyro.read_acc_x();
					if(!(ac<=Acc_thre_d)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					/*serial.putfloat(ac);
					serial.string(",");
					serial.putfloat(now);
					serial.string("\n\r");*/
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//そこまで傾いていない
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
							#warning Not yet
							//左の速度落とす
							do 
							{
								now=motor_gyro.read_acc_y();
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							#warning Not yet
							//右の速度落とす
							do
							{
								now=motor_gyro.read_acc_y();
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//大きく傾いている
						led(Redled,1);
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(!(ac<=Acc_thre_d)){
						ac=motor_gyro.read_acc_x();
					}
				}
				led(Redled,0);
				error_led(2,0);
				error_led(1,0);
				ac=motor_gyro.read_acc_x();
				if(ac<=Acc_thre_d){
					notify_long_acc(x,false);
				}
				motor::brake(MOTOR_RIGHT);
				motor::brake(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				if(ac<=Acc_thre_d){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 1;
			}
		}else if(x==v::back){//後進中
			if(ac>=Acc_thre_u){//下り
				if(longway/2<=ping(FRONT)){
					ac=motor_gyro.read_acc_x();
					if(ac>=Acc_thre_u){
						if(buz==true){
						buzzer(400);
						buzzer(800);
						}
					}
					else{
						return 0;
					}
				}
				else{
					return 0;
				}
				lcd_clear();
				lcd_putstr("NotiL!D");
				back(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				ac=motor_gyro.read_acc_x();
				while(ac>=Acc_thre_u){
					ac=motor_gyro.read_acc_x();
					if(!(ac>=Acc_thre_u)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//そこまで傾いていない
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
							#warning Not yet
							//左の速度落とす
							do 
							{
								now=motor_gyro.read_acc_y();
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							#warning Not yet
							//右の速度落とす
							do
							{
								now=motor_gyro.read_acc_y();
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//大きく傾いている
						led(Redled,1);
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(!(ac>=Acc_thre_u)){
						ac=motor_gyro.read_acc_x();
					}
				}
				led(Redled,0);
				error_led(2,0);
				error_led(1,0);
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				motor::brake(MOTOR_RIGHT);
				motor::brake(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 1;
			}
			else if(ac<=Acc_thre_d){//昇り
				if(longway<=ping(PING_BACK)){
					ac=motor_gyro.read_acc_x();
					if(ac<=Acc_thre_d){
						if(buz==true){
						buzzer(800);
						buzzer(400);
						}
					}
					else{
						return 0;
					}
				}
				else{
					return 0;
				}
				lcd_clear();
				lcd_putstr("NotiL!U");
				back(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				ac=motor_gyro.read_acc_x();
				while(ac<=Acc_thre_d){
					ac=motor_gyro.read_acc_x();
					if(!(ac<=Acc_thre_d)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//そこまで傾いていない
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
							#warning Not yet
							//右速度落とす
							do 
							{
								now=motor_gyro.read_acc_y();
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							//左速度落とす
							#warning Not yet
							do
							{
								now=motor_gyro.read_acc_y();
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//大きく傾いている
						led(Redled,1);
						if(now>Acc_slope_thre){//右向いてる
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//左を向いてる
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(!(ac<=Acc_thre_d)){
						ac=motor_gyro.read_acc_x();
					}
				}
				led(Redled,0);
				error_led(2,0);
				error_led(1,0);
				ac=motor_gyro.read_acc_x();
				if(ac<=Acc_thre_d){
					notify_long_acc(x,false);
				}
				motor::brake(MOTOR_RIGHT);
				motor::brake(MOTOR_LEFT);
				ac=motor_gyro.read_acc_x();
				if(ac<=Acc_thre_d){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 2;
			}
		}
		else{
			lcd_clear();
			return 0;
		}
		return 0;
	}
	
	uint8_t notify_long_ang(uint8_t x,bool buz){//0:なし,1:下り,2:上り
		float ang=motor_gyro.read_angle_y();
		float anx=motor_gyro.read_angle_x();
		if(x==v::front){//前進中
			if(ang<=Ang_slope_Norm-Ang_slope_thre){//上り
				set_Status(NOPID);
				lcd_clear();
				lcd_putstr("NotiL!U");
				xbee.string("\x1b[43mNotify Long Up! Go ahead!\n\r\x1b[49m");
				buzzer(400);
				buzzer(800);
				forward(MOTOR_RIGHT);
				forward(MOTOR_LEFT);
				set_pwm(MOTOR_SPEED[SPEED_TARGET]);
				while(ang<=Ang_slope_Norm-Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//そこまで傾いていない
						if(anx>Ang_x_Norm){//右向いてる
							error_led(2,1);
							error_led(1,0);
							set_pwm(MOTOR_LEFT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//右速度落とす
							do 
							{
								anx=motor_gyro.read_angle_x();
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							set_pwm(MOTOR_RIGHT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//左速度落とす
							do
							{
								anx=motor_gyro.read_angle_x();
							} while (anx<Ang_x_Norm);
						}
						set_pwm(MOTOR_SPEED[SPEED_TARGET]);
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
				}
				lcd_clear();
				return 2;
			}
			else if(ang>=Ang_slope_Norm+Ang_slope_thre){//下り
				set_Status(NOPID);
				lcd_clear();
				lcd_putstr("NotiL!D");
				xbee.string("\x1b[43mNotify Long Down! Go ahead!\n\r\x1b[49m");
				buzzer(800);
				buzzer(400);
				forward(MOTOR_LEFT);
				forward(MOTOR_RIGHT);
				while(ang>=Ang_slope_Norm+Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//そこまで傾いていない
						if(anx>Ang_x_Norm){//右向いてる
							error_led(2,1);
							error_led(1,0);
							set_pwm(MOTOR_RIGHT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//左速度落とす
							do 
							{
								anx=motor_gyro.read_angle_x();
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							set_pwm(MOTOR_LEFT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//右速度落とす
							do
							{
								anx=motor_gyro.read_angle_x();
							} while (anx<Ang_x_Norm);
						}
						set_pwm(MOTOR_SPEED[SPEED_TARGET]);
						//速度戻す
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
				}
				lcd_clear();
				return 1;
			}
		}else if(x==v::back){//後進中
			if(ang>=Ang_slope_Norm+Ang_slope_thre){//下り
				set_Status(NOPID);
				lcd_clear();
				lcd_putstr("NotiL!U");
				xbee.string("\x1b[43mNotify Long Up! Go back!\n\r\x1b[49m");
				back(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				buzzer(400);
				buzzer(800);
				while(ang>=Ang_slope_Norm+Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//そこまで傾いていない
						if(anx>Ang_x_Norm){//右向いてる
							error_led(2,1);
							error_led(1,0);
							set_pwm(MOTOR_LEFT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//右速度落とす
							do 
							{
								anx=motor_gyro.read_angle_x();
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							set_pwm(MOTOR_RIGHT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//左速度落とす
							do
							{
								anx=motor_gyro.read_angle_x();
							} while (anx<Ang_x_Norm);
						}
						set_pwm(MOTOR_SPEED[SPEED_TARGET]);
						//速度戻す
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
				}
				lcd_clear();
				return 1;
			}
			else if(ang<=Ang_slope_Norm-Ang_slope_thre){//昇り
				set_Status(NOPID);
				lcd_clear();
				lcd_putstr("NotiL!U");
				xbee.string("\x1b[43mNotify Long Up! Go back!\n\r\x1b[49m");
				back(MOTOR_LEFT);
				back(MOTOR_RIGHT);
				buzzer(800);
				buzzer(400);
				while(ang<=Ang_slope_Norm-Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//そこまで傾いていない
						if(anx>Ang_x_Norm){//右向いてる
							error_led(2,1);
							error_led(1,0);
							set_pwm(MOTOR_RIGHT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_2)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//左速度落とす
							do 
							{
								anx=motor_gyro.read_angle_x();
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//左を向いてる
							error_led(2,0);
							error_led(1,1);
							set_pwm(MOTOR_LEFT,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)-MOTOR_SPEED[SPEED_SLOPE_FIX_DEV]);
							//右速度落とす
							do
							{
								anx=motor_gyro.read_angle_x();
							} while (anx<Ang_x_Norm);
						}
						set_pwm(MOTOR_SPEED[SPEED_TARGET]);
						//速度戻す
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
				}
				lcd_clear();
				return 2;
			}
		}
		else{
			lcd_clear();
			return 0;
		}
		return 0;
	}
	// uint8_t fix_position(void){
	// 	uint8_t i=0;
	// 	uint8_t e=0;
	// 	e = motor::notify_long();
	// 	if(!e){motor::notify_half();}
	// 	motor::turn_fix();
	// 	//i=motor::notify_position();
	// 	motor::gb_fix();
	// 	motor::turn_fix();
	// 	return e;
	// }
	void fix_position(uint8_t x){
		set_Status(NOPID);
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		turn_fix();
		set_Status(NOPID);
		notify_half(x);
		set_Status(NOPID);
		gb_fix();
		set_Status(NOPID);
		turn_fix();
		set_Status(NOPID);
		FIND_BRICK=0;
		//return;
	}
	
	void fix_angle(void){
		set_Status(NOPID);
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		float now=0;
		float siki=1;//修正する閾値
		now=motor_gyro.read_angle();
		if(abs(now-b_angle)>siki){
			lcd_clear();
			lcd_putstr("fix_angl");
			if(now>b_angle){
				if(abs(now-b_angle)<=180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do 
					{
						now=motor_gyro.read_angle();
					} while (abs(now-b_angle)>siki);
				}
				else if(abs(now-b_angle)>180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-b_angle)>siki);
				}
			}
			else if(now<b_angle){
				if(abs(now-b_angle)<=180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-b_angle)>siki);
				}
				else if(abs(now-b_angle)>180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-b_angle)>siki);
				}
			}
		}
		lcd_clear();
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		return;
	}void fix_angle_v(float angl){
		set_Status(NOPID);
		set_pwm(MOTOR_SPEED[SPEED_FIX_TARGET]);
		float now=0;
		float siki=1;//修正する閾値
		now=motor_gyro.read_angle();
		if(abs(now-angl)>siki){
			lcd_clear();
			lcd_putstr("fix_ag_v");
			if(now>angl){
				if(abs(now-angl)<=180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do 
					{
						now=motor_gyro.read_angle();
					} while (abs(now-angl)>siki);
				}
				else if(abs(now-angl)>180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-angl)>siki);
				}
			}
			else if(now<angl){
				if(abs(now-angl)<=180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-angl)>siki);
				}
				else if(abs(now-angl)>180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-angl)>siki);
				}
			}
		}
		lcd_clear();
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		return;
	}
	void set_angle(float ang){
		float now=0;
		float siki=1;//修正する閾値
		now=motor_gyro.read_angle();
		if(abs(now-ang)>siki){
			lcd_clear();
			lcd_putstr("set_ag_v");
			if(now>ang){
				if(abs(now-ang)<=180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do 
					{
						now=motor_gyro.read_angle();
					} while (abs(now-ang)>siki);
				}
				else if(abs(now-ang)>180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-ang)>siki);
				}
			}
			else if(now<ang){
				if(abs(now-ang)<=180){
					forward(MOTOR_RIGHT);
					back(MOTOR_LEFT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-ang)>siki);
				}
				else if(abs(now-ang)>180){
					forward(MOTOR_LEFT);
					back(MOTOR_RIGHT);
					do
					{
						now=motor_gyro.read_angle();
					} while (abs(now-ang)>siki);
				}
			}
		}
		lcd_clear();
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		return;
	}
}

void enkaigei(void){
	float first=motor_gyro.read_angle(), now=0;
	motor::set_Status(motor::NOPID);
	motor::set_pwm(300);
	while(1){
		now=motor_gyro.read_angle();
		if(abs(now-first)>1){
			if(now-first>0){
				if(abs(now-first)<=180){
					motor::forward(motor::MOTOR_LEFT);
					motor::back(motor::MOTOR_RIGHT);
				}
				else if(abs(now-first)>180){
					motor::forward(motor::MOTOR_RIGHT);
					motor::back(motor::MOTOR_LEFT);
				}
			}
			else if(now-first<0){
				if(abs(now-first)<=180){
					motor::back(motor::MOTOR_LEFT);
					motor::forward(motor::MOTOR_RIGHT);
				}
				else if(abs(now-first)>180){
					motor::forward(motor::MOTOR_LEFT);
					motor::back(motor::MOTOR_RIGHT);
				}
			}
		}
		else{
			motor::brake(motor::MOTOR_LEFT);
			motor::brake(motor::MOTOR_RIGHT);
		}
		xbee.string("First:");
		xbee.putfloat(first);
		xbee.string(",Now:");
		xbee.putfloat(now);
		xbee.string("\n\r");
	}
}
