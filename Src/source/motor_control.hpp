/*
 * motor_control.h
 *
 * Created: 2018/08/23 3:04:31
 *  Author: shun2
 */ 


#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include "data_structure.hpp"
#include <stdint.h>

#define Acc_thre_u 2.9
#define Acc_thre_d -4
#define Acc_slope_thre 0.1
#define Ang_slope_Norm 178
#define Ang_slope_thre 15
#define Ang_x_Norm 180
#define Ang_x_thre 3
namespace motor{
	typedef enum{
		MOTOR_RIGHT,
		MOTOR_LEFT
	}ch_t;
	typedef enum{
		MOTOR_BACK,
		MOTOR_ADVANCE,
		MOTOR_BRAKE,
	}move_sig_t;
	typedef enum{
		ONE_ADVANCE,
		TWO_ADVANCE,
		RIGHT_TURN,
		LEFT_TURN,
		ONE_BACK,
		TWO_BACK,
		HALF_ADVANCE,
		HALF_BACK,
		RIGHT_TURN_NO_GYRO,
		LEFT_TURN_NO_GYRO,
		BRAKE
	}move_t;
	typedef enum{
		ONE_BLOCK,
		TWO_BLOCK,
		TURN,
		HALF_BLOCK,
	}move_dis_t;
	void check_job();
	void brake(ch_t x);
	void forward(ch_t x);
	void back(ch_t x);
	void set_speed(ch_t x,uint16_t sp);
	void set_speed(uint16_t sp);
	uint8_t status(motor::ch_t m);
	void wait(bool check=true);
	void move(move_t x=BRAKE);/*
	void forever(void);
	void back(int x);
	void advance(int x);
	void turn(int x);*/
	void fix(uint8_t x,uint8_t ping1,uint8_t ping2,int no);
	//uint8_t fix_position(void);
	uint8_t notify_half(uint8_t x);
	void fix_position(uint8_t x =v::front);
	void gb_fix(void);
	void turn_fix(uint8_t x=0);
	uint8_t notify_long_acc(uint8_t x,bool buz=true);
	uint8_t notify_long_ang(uint8_t x,bool buz=true);
	uint8_t notify_long_ex(void);
	void fix_angle(void);
	void fix_angle_v(float angl);
	void set_angle(float ang);
	#define m_speed 7
	#define m_turnspeed 6
}

void Save_angle(void);
void init_motor(void);
uint8_t mspi(uint8_t val,motor::ch_t i);
void m_send(motor::ch_t rl,motor::move_sig_t sig,uint8_t y,motor::move_dis_t dis);
void m_send(motor::ch_t rl,motor::move_sig_t sig,uint8_t y,uint8_t z);
int16_t smaller_s(int16_t x,int16_t y);
void enkaigei(void);
#endif /* MOTOR_CONTROL_H_ */
