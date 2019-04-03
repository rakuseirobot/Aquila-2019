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
#include "mv_control.hpp"
#include "data_structure.hpp"
#include "JY901.hpp"
#include "delay.hpp"
#include "uart_control.hpp"
//#include "initializing.hpp"
#include <stdlib.h>
#include "main.h"
#include "stm32f4xx.h"
#include <queue>
#include "Array_wrapper.hpp"
//#include "core.hpp"

extern jy901 gyro;
jy901 motor_gyro = gyro;

extern uart serial;
uart motor_serial = serial;


const int rose=700;
const int16_t longway = 4500;

#if rose>Sikiti*2
#error "Rose's value is wrong!"
//rose=(Sikiti/2)*3
#else
#endif

const int gbno=120;
float Saved_angle=0;



uint8_t data=000;
void init_motor(void){
	HAL_GPIO_WritePin(M1_PWM_GPIO_Port,M1_PWM_Pin|M2_PWM_Pin,GPIO_PIN_SET);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}
void Save_angle(void){
	Saved_angle=motor_gyro.read_angle();
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
	array_wrapper<uint16_t, move_dis_t> Move_Distance = {
	  1,1,1,1,
	};
	float b_angle=0.0;
	std::queue<move_t> Motor_task;
	int32_t Motor_target;
	int32_t Right_count,Left_count;
	task_status_t Right_Motor_Status=FREE,Left_Motor_Status=FREE;
	void check_job(){
		if(check_task()!=FREE){
			Motor_target=Move_Distance[Motor_task.front()];
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
				Right_count-=Motor_target;
				Left_count+=Motor_target;
				break;
			case LEFT_TURN:
				Right_count+=Motor_target;
				Left_count-=Motor_target;
				break;
			}
			Motor_task.pop();
		}
	}
	void check_Enocoder(void){
		 Left_count+=__HAL_TIM_GetCounter(&htim3);
		 Right_count+=__HAL_TIM_GetCounter(&htim5);
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
		 return;
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
	void brake(ch_t x){
		if(x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M1_INA_GPIO_Port,M1_INA_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_INB_Pin|M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INA_Pin|M2_INB_Pin|M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void forward(ch_t x){
		if(x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M1_INA_GPIO_Port,M1_INA_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INA_Pin|M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void back(ch_t x){
		if(x==MOTOR_RIGHT){
			HAL_GPIO_WritePin(M1_INB_GPIO_Port,M1_INB_Pin|M1_EN_Pin,GPIO_PIN_SET);
		}
		else if (x==MOTOR_LEFT){
			HAL_GPIO_WritePin(M2_INA_GPIO_Port,M2_INB_Pin|M2_EN_Pin,GPIO_PIN_SET);
		}
		return;
	}
	void set_speed(ch_t x,uint16_t sp){
		sp*10;
		if(x==MOTOR_RIGHT){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sp);//1
		}
		else if (x==MOTOR_LEFT){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sp);//2
		}
		else{

		}
		return;
	}
	void set_speed(uint16_t sp){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,sp);//1
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,sp);//2
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
	void motor_job(move_t job){
		switch(job){
		case ONE_ADVANCE:
			break;
		case TWO_ADVANCE:
			break;
		case RIGHT_TURN:
			break;
		case LEFT_TURN:
			break;
		case ONE_BACK:
			break;
		case TWO_BACK:
			break;
		case HALF_ADVANCE:
			break;
		case HALF_BACK:
			break;
		case RIGHT_TURN_NO_GYRO:
			break;
		case LEFT_TURN_NO_GYRO:
			break;
		case BRAKE:
			break;
		default:
			break;
		}
		return;
	}
	void wait(bool check){
		while(check_task()!=FREE&&abs(Right_count)>=Motor_thre&&abs(Left_count)>=Motor_thre){
			check_sig(check);
		}
		return;
	}
	void move(move_t x){// x = 0:1 block Advance 1:2 blocks Advance 2:Right Turn with Gyro 3:Left Turn with Gyro 4:1 block Back 5:2 block Back 6:Half block Advance 7:Half block Back 8:right Turn without Compass 9:left Turn without Compass 
		HAL_Delay(5);
		float first = 0;
		float now = 0;
		bool st_f = false;
		b_angle=motor_gyro.read_angle();
		mv_cap(MV_LEFT,true);
		mv_cap(MV_FRONT,true);
		mv_cap(MV_RIGHT,true);
		motor::wait();
		switch(x){
			case ONE_ADVANCE: //1block advance
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_speed,1);
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_speed,1);
				st_f=true;
				//HAL_Delay(300);
			break;
			case TWO_ADVANCE: //2block advance
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_speed,2);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_speed,2);
				st_f=true;
				//HAL_Delay(300);
			break;
			case RIGHT_TURN: //Right turn with gyro
				first = motor_gyro.read_angle();
				//serial.putint(first);
				//serial.string("\n\r");
				turn_retry2:
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_turnspeed,5);
				m_send(MOTOR_LEFT,MOTOR_BACK,m_turnspeed,5);
				HAL_Delay(2);
				now = motor_gyro.read_angle();
				//serial.putint(now);
				//serial.string("\n\r");
				do{
					now=motor_gyro.read_angle();
					//serial.putint(now);
					//serial.string("\n\r");
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){	
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_turnspeed,5);
						m_send(MOTOR_LEFT,MOTOR_BACK,m_turnspeed,5);
					}
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
				first = motor_gyro.read_angle();
				//serial.putint(first);
				//serial.string("\n\r");
				turn_retry3:
				m_send(MOTOR_RIGHT,MOTOR_BACK,m_turnspeed,5);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_turnspeed,5);
				HAL_Delay(2);
				now=motor_gyro.read_angle();
				//serial.putint(now);
				//serial.string("\n\r");
				do{
					now=motor_gyro.read_angle();
					//serial.putint(now);
					//serial.string("\n\r");
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){	
						m_send(MOTOR_RIGHT,MOTOR_BACK,m_turnspeed,5);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_turnspeed,5);
					}
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
				m_send(MOTOR_RIGHT,MOTOR_BACK,m_speed,1);
				m_send(MOTOR_LEFT,MOTOR_BACK,m_speed,1);
				st_f=true;
			break;
			case TWO_BACK: //2block back
				m_send(MOTOR_RIGHT,MOTOR_BACK,m_speed,2);
				m_send(MOTOR_LEFT,MOTOR_BACK,m_speed,2);
				st_f=true;
			break;

			case HALF_ADVANCE: //half advance
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_speed,4);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_speed,4);
				st_f=true;
			break;
			case HALF_BACK: //half back
				m_send(MOTOR_RIGHT,MOTOR_BACK,m_speed,4);
				m_send(MOTOR_LEFT,MOTOR_BACK,m_speed,4);
				st_f=true;
			break;
			case LEFT_TURN_NO_GYRO: //Left Turn without gyro (????)
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_turnspeed,3);
				m_send(MOTOR_LEFT,MOTOR_BACK,m_turnspeed,3);
				//HAL_Delay(300);
			break;
			case RIGHT_TURN_NO_GYRO: //Right Turn without gyro (???)
				m_send(MOTOR_RIGHT,MOTOR_BACK,m_turnspeed,3);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_turnspeed,3);
			break;
			default:
				motor::brake(MOTOR_RIGHT);
				motor::brake(MOTOR_LEFT);
			break;
		}
		motor::wait();
		if(st_f==true){
			fix_angle_v(b_angle);
		}
		motor::wait();
		mv_cap(MV_LEFT,true);
		mv_cap(MV_FRONT,true);
		mv_cap(MV_RIGHT,true);
		HAL_Delay(200);
	}
	void forever(void){
		m_send(MOTOR_RIGHT,MOTOR_ADVANCE,5,0);
		m_send(MOTOR_LEFT,MOTOR_ADVANCE,5,0);
		while(ping(FRONT)>=3);
		motor::move();
		return;
	}/*
	void back(int x){
		if(x==1){
			lcd_putstr("back1");
		}
		else if(x==2){
			lcd_putstr("back2");
		}
		motor::move(x-1);
		lcd_clear();
		return;
	}

	void advance(int x){
		if(x==1){
			lcd_putstr("advance1");
		}
		else if(x==2){
			lcd_putstr("advance2");
		}
		motor::move(x+3);
		lcd_clear();
		return;
	}

	void turn(int x){//right left ???
		if(x==1){
			lcd_putstr("turn_l");
		}
		else if(x==2){
			lcd_putstr("turn_r");
		}
		motor::move(x+1);
		lcd_clear();
		return;
	}*/

	void fix(uint8_t x,uint8_t ping1,uint8_t ping2,int no){//x=right,left ping1,ping2:Compare ping number no:Sikiiti 
		int val=0;
		if(x==1){//left
			m_send(MOTOR_RIGHT,MOTOR_BACK,2,1);
			m_send(MOTOR_LEFT,MOTOR_ADVANCE,2,1);
		}
		else if(x==2){//right
			m_send(MOTOR_RIGHT,MOTOR_ADVANCE,2,1);
			m_send(MOTOR_LEFT,MOTOR_BACK,2,1);
		}
		while(1){
			val=ping(ping1)-ping(ping2);
			if(abs(val)>=no){
				break;
			}
		}
		motor::move();
		return;
	}
	#define fixno 5 //gb_fix Sikiiti
	const int gbbest=175;//185
	uint32_t tbest = 600;
	void gb_fix(void){
		int16_t dis[3];
		dis[0]=ping(FRONT);//Forward
		dis[1]=ping(BACK);//Back
		float ang=motor_gyro.read_angle_y();
		if(ang<=Ang_slope_Norm-Ang_slope_thre*1.5){
			motor::move(HALF_BACK);
		}
		else if(ang>=Ang_slope_Norm+Ang_slope_thre*1.5){
			motor::move(HALF_ADVANCE);
		}
		
		dis[0]=ping(FRONT);//Forward
		dis[1]=ping(BACK);//Back
		if(Sikiti>=dis[0]){
			HAL_Delay(1);
			dis[0]=ping(FRONT);
			if(!(Sikiti>=dis[0])){
				return;
			}
			lcd_clear();
			if((gbbest-dis[0])<fixno*-1){
				lcd_putstr("gb_fixF");
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
				while(dis[0]>gbbest){
					if(dis[0]>=longway){
						break;
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
					}
					dis[0]=ping(FRONT);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			else if((gbbest-dis[0])>fixno){
				lcd_putstr("gb_fixB");
				m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
				m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
				while(dis[0]<gbbest){
					if(dis[0]>=longway){
						break;
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
						m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
					}
					dis[0]=ping(FRONT);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			lcd_clear();
		}
		else if(Sikiti>=dis[1]){
			HAL_Delay(1);
			dis[1]=ping(BACK);
			if(!(Sikiti>=dis[1])){
				return;
			}
			lcd_clear();
			if((gbbest-dis[1])>fixno){
				lcd_putstr("gb_fixF");
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
				while(dis[1]<gbbest){
					if(dis[1]>=longway){
						break;
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
					}
					dis[1]=ping(BACK);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
			else if((gbbest-dis[1])<fixno*-1){
				lcd_putstr("gb_fixB");
				m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
				m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
				while(dis[1]>gbbest){
					if(dis[1]>=longway){
						break;
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
						m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
					}
					dis[1]=ping(BACK);
				}
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
			}
		}
		else{}
		lcd_clear();
		return;
	}
	const int32_t turnvalue = 5;
	void turn_fix(uint8_t force){
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
			}
			if (smaller_s(ping(RIGHT_BACK),ping(RIGHT_FRONT))<=Sikiti){
				if(!(ping(RIGHT_BACK)<=Sikiti&&ping(RIGHT_FRONT)<=Sikiti)){
					return;
				}
			}
			int ldiff = (ping(LEFT_FRONT)+ping(LEFT_BACK))/2;
			int rdiff = (ping(RIGHT_BACK)+ping(RIGHT_FRONT))/2;
			
			if(rdiff>ldiff){
				chk[0]=LEFT_BACK;
				chk[1]=LEFT_FRONT;
				//usart_string("use Left!\n\r");
			}
			else if(rdiff<ldiff){
				chk[0]=RIGHT_FRONT;
				chk[1]=RIGHT_BACK;
				//usart_string("use Right!\n\r");
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
				m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
				do{
					val=ping(chk[0])-ping(chk[1]);
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,1,2);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,1,2);
					}
				}while(val<turnvalue*-1);
			}
			else if(val > turnvalue){
				lcd_clear();
				lcd_putstr("FixingTurn");
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
				m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
				do{
					val=ping(chk[0])-ping(chk[1]);
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,1,2);
						m_send(MOTOR_LEFT,MOTOR_BACK,1,2);
					}
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
		uint8_t dis[10];
		uint8_t i = 0;
		dis[0] = c_p(1);
		dis[1] = c_p(2);
		dis[2] = c_p(5);
		dis[3] = c_p(4);
		if(dis[0]==1&&dis[1]!=1){
			i=2;
		}else if(dis[0]!=1&&dis[1]==1){
			i=2;
		}else if(dis[2]==1&&dis[3]!=1){
			i=2;
		}else if(dis[2]!=1&&dis[3]==1){
			i=2;
		}else{
			i=0;
		}
		if(i==2){
		dis[0] = c_p(1);
		dis[1] = c_p(2);
		dis[2] = c_p(5);
		dis[3] = c_p(4);
			if(dis[0]==1&&dis[1]!=1){
				i=1;
			}else if(dis[0]!=1&&dis[1]==1){
				i=1;
			}else if(dis[2]==1&&dis[3]!=1){
				i=1;
			}else if(dis[2]!=1&&dis[3]==1){
				i=1;
			}else{
				i=0;
			}
		}if(i==1){
			lcd_clear();
			lcd_putstr("NotifyHA");
			serial.string("NotifyHalf");
			serial.string("\n\r");
			if (x == v::front){
				motor::move(HALF_ADVANCE);
			}
			else if(x == v::back){
				motor::move(HALF_BACK);
			}
			lcd_clear();
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
				do{
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,7,1);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,7,1);
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
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,m_speed,1);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,m_speed,1);
					dis = ping(FRONT);
				}while(dis >= gbbest);
				motor::brake(MOTOR_LEFT);
				motor::brake(MOTOR_RIGHT);
				//motor::wait();
				led(Redled,0);
			}
			serial.string("saka!");
			serial.string("\n\r");
			led(Blueled,0);
			motor::gb_fix();
			lcd_clear();
			return 1;
		}
		else{
			return 0;
		}
	}
	
	uint8_t notify_long_acc(uint8_t x,bool buz){//0:‚Θ‚µ,1:‰Ί‚θ,2:γ‚θ
		float ac=motor_gyro.read_acc_x();
		uint8_t spos = 6;
		float now=0;
		if(x==v::front){//‘Oi’†
			if(ac>=Acc_thre_u){//γ‚θ
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
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
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
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos-2,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
								}
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_LEFT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos-2,3);
								}
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//‘ε‚«‚­X‚Ά‚Δ‚Ά‚ι
						led(Redled,1);
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_RIGHT)==1||motor::status(MOTOR_LEFT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
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
				motor::move();
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 2;
			}
			else if(ac<=Acc_thre_d){//‰Ί‚θ
				if(longway/2<=ping(BACK)){
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
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
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
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos-3,3);
								}
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos-3,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
								}
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//‘ε‚«‚­X‚Ά‚Δ‚Ά‚ι
						led(Redled,1);
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
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
				motor::move();
				ac=motor_gyro.read_acc_x();
				if(ac<=Acc_thre_d){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 1;
			}
		}else if(x==v::back){//γi’†
			if(ac>=Acc_thre_u){//‰Ί‚θ
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
				m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
				m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
				ac=motor_gyro.read_acc_x();
				while(ac>=Acc_thre_u){
					ac=motor_gyro.read_acc_x();
					if(!(ac>=Acc_thre_u)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos-2,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
								}
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos-2,3);
								}
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//‘ε‚«‚­X‚Ά‚Δ‚Ά‚ι
						led(Redled,1);
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
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
				motor::move();
				ac=motor_gyro.read_acc_x();
				if(ac>=Acc_thre_u){
					notify_long_acc(x,false);
				}
				HAL_Delay(1);
				lcd_clear();
				motor::move(HALF_BACK);
				return 1;
			}
			else if(ac<=Acc_thre_d){//Έ‚θ
				if(longway<=ping(BACK)){
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
				m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
				m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
				ac=motor_gyro.read_acc_x();
				while(ac<=Acc_thre_d){
					ac=motor_gyro.read_acc_x();
					if(!(ac<=Acc_thre_d)){
						ac=motor_gyro.read_acc_x();
					}
					led(Redled,0);
					now=motor_gyro.read_acc_y();
					if(/*Acc_slope_thre*5>abs(now)&&*/abs(now)>Acc_slope_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos-3,3);
								}
							} while (now>Acc_slope_thre);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								now=motor_gyro.read_acc_y();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos-3,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
								}
							} while (now<Acc_slope_thre*-1);
						}
					}
					/*else if(abs(now)>Acc_slope_thre*5){//‘ε‚«‚­X‚Ά‚Δ‚Ά‚ι
						led(Redled,1);
						if(now>Acc_slope_thre){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
						}
						else if(now<Acc_slope_thre*-1){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
						}
					}*/
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
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
				motor::move();
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
	
	uint8_t notify_long_ang(uint8_t x,bool buz){//0:‚Θ‚µ,1:‰Ί‚θ,2:γ‚θ
		float ang=motor_gyro.read_angle_y();
		float anx=motor_gyro.read_angle_x();
		uint8_t spos = 6;
		if(x==v::front){//‘Oi’†
			if(ang<=Ang_slope_Norm-Ang_slope_thre){//γ‚θ
				lcd_clear();
				lcd_putstr("NotiL!U");
				buzzer(400);
				buzzer(800);
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
				while(ang<=Ang_slope_Norm-Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(anx>Ang_x_Norm){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos-2,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
								}
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos-2,3);
								}
							} while (anx<Ang_x_Norm);
						}
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						error_led(2,0);
						error_led(1,0);
					}
				}
				return 2;
			}
			else if(ang>=Ang_slope_Norm+Ang_slope_thre){//‰Ί‚θ
				lcd_clear();
				lcd_putstr("NotiL!D");
				buzzer(800);
				buzzer(400);
				m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
				m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
				while(ang>=Ang_slope_Norm+Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(anx>Ang_x_Norm){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos-2,3);
								}
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos-2,3);
									m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
								}
							} while (anx<Ang_x_Norm);
						}
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
						m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						error_led(2,0);
						error_led(1,0);
					}
				}
				return 1;
			}
		}else if(x==v::back){//γi’†
			if(ang>=Ang_slope_Norm+Ang_slope_thre){//‰Ί‚θ
				lcd_clear();
				lcd_putstr("NotiL!U");
				m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
				m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
				buzzer(400);
				buzzer(800);
				while(ang>=Ang_slope_Norm+Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(anx>Ang_x_Norm){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos-2,3);
								}
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos-2,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
								}
							} while (anx<Ang_x_Norm);
						}
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						error_led(2,0);
						error_led(1,0);
					}
				}
				return 1;
			}
			else if(ang<=Ang_slope_Norm-Ang_slope_thre){//Έ‚θ
				lcd_clear();
				lcd_putstr("NotiL!U");
				m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
				m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
				buzzer(800);
				buzzer(400);
				while(ang<=Ang_slope_Norm-Ang_slope_thre){
					ang=motor_gyro.read_angle_y();
					anx=motor_gyro.read_angle_x();
					if(abs(anx-Ang_x_Norm)>Ang_x_thre){//‚»‚±‚ά‚ΕX‚Ά‚Δ‚Ά‚Θ‚Ά
						if(anx>Ang_x_Norm){//‰Eό‚Ά‚Δ‚ι
							error_led(2,1);
							error_led(1,0);
							do 
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos-2,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
								}
							} while (anx>Ang_x_Norm);
						}
						else if(anx<Ang_x_Norm){//¶‚πό‚Ά‚Δ‚ι
							error_led(2,0);
							error_led(1,1);
							do
							{
								anx=motor_gyro.read_angle_x();
								if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
									m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
									m_send(MOTOR_RIGHT,MOTOR_BACK,spos-2,3);
								}
							} while (anx<Ang_x_Norm);
						}
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					}
					else{
						error_led(2,0);
						error_led(1,0);
					}
					if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
						m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
						m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						error_led(2,0);
						error_led(1,0);
					}
				}
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
		turn_fix();
		notify_half(x);
		gb_fix();
		turn_fix();
		//return;
	}
	
	void fix_angle(void){
		float now=0;
		uint8_t spos=1;
		float siki=1;//C³‚·‚ιθ‡’l
		now=motor_gyro.read_angle();
		if(abs(now-b_angle)>siki){
			lcd_clear();
			lcd_putstr("fix_angl");
			if(now-b_angle>0){
				if(abs(now-b_angle)<=180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do 
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
					} while (abs(now-b_angle)>siki);
				}
				else if(abs(now-b_angle)>180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-b_angle)>siki);
				}
			}
			else if(now-b_angle<0){
				if(abs(now-b_angle)<=180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-b_angle)>siki);
				}
				else if(abs(now-b_angle)>180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
					} while (abs(now-b_angle)>siki);
				}
			}
		}
		lcd_clear();
		motor::brake(MOTOR_LEFT);
		motor::brake(MOTOR_RIGHT);
		return;
	}void fix_angle_v(float angl){
		float now=0;
		uint8_t spos=1;
		float siki=1;//C³‚·‚ιθ‡’l
		now=motor_gyro.read_angle();
		if(abs(now-angl)>siki){
			lcd_clear();
			lcd_putstr("fix_ag_v");
			if(now-angl>0){
				if(abs(now-angl)<=180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do 
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
					} while (abs(now-angl)>siki);
				}
				else if(abs(now-angl)>180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-angl)>siki);
				}
			}
			else if(now-angl<0){
				if(abs(now-angl)<=180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-angl)>siki);
				}
				else if(abs(now-angl)>180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
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
		uint8_t spos=4;
		float siki=1;//C³‚·‚ιθ‡’l
		now=motor_gyro.read_angle();
		if(abs(now-ang)>siki){
			lcd_clear();
			lcd_putstr("set_ag_v");
			if(now-ang>0){
				if(abs(now-ang)<=180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do 
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
					} while (abs(now-ang)>siki);
				}
				else if(abs(now-ang)>180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-ang)>siki);
				}
			}
			else if(now-ang<0){
				if(abs(now-ang)<=180){
					m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
					m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_BACK,spos,3);
							m_send(MOTOR_LEFT,MOTOR_ADVANCE,spos,3);
						}
					} while (abs(now-ang)>siki);
				}
				else if(abs(now-ang)>180){
					m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
					m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
					do
					{
						now=motor_gyro.read_angle();
						if(motor::status(MOTOR_LEFT)==1||motor::status(MOTOR_RIGHT)==1){
							m_send(MOTOR_RIGHT,MOTOR_ADVANCE,spos,3);
							m_send(MOTOR_LEFT,MOTOR_BACK,spos,3);
						}
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
	uint8_t spos=5;
	while(1){
		now=motor_gyro.read_angle();
		if(abs(now-first)>1){
			if(now-first>0){
				if(abs(now-first)<=180){
					m_send(motor::MOTOR_RIGHT,motor::MOTOR_ADVANCE,spos,3);
					m_send(motor::MOTOR_LEFT,motor::MOTOR_BACK,spos,3);
				}
				else if(abs(now-first)>180){
					m_send(motor::MOTOR_RIGHT,motor::MOTOR_BACK,spos,3);
					m_send(motor::MOTOR_LEFT,motor::MOTOR_ADVANCE,spos,3);
				}
			}
			else if(now-first<0){
				if(abs(now-first)<=180){
					m_send(motor::MOTOR_RIGHT,motor::MOTOR_BACK,spos,3);
					m_send(motor::MOTOR_LEFT,motor::MOTOR_ADVANCE,spos,3);
				}
				else if(abs(now-first)>180){
					m_send(motor::MOTOR_RIGHT,motor::MOTOR_ADVANCE,spos,3);
					m_send(motor::MOTOR_LEFT,motor::MOTOR_BACK,spos,3);
				}
			}
		}
		else{
			motor::brake(motor::MOTOR_LEFT);
			motor::brake(motor::MOTOR_RIGHT);
		}
	}
}
