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


extern float P_GAIN,I_GAIN,D_GAIN;

#define SPEED_GAIN 0.07

#define Acc_thre_u 2.9
#define Acc_thre_d -4
#define Acc_slope_thre 0.1
#define Ang_slope_Norm 178
#define Ang_slope_thre 15
#define Ang_x_Norm 180
#define Ang_x_thre 3

#define Motor_thre 20

#define PID_LEFT_MOTOR 0
#define PID_RIGHT_MOTOR 1

#define SPEED_LEFT_MOTOR 0
#define SPEED_RIGHT_MOTOR 1
#define SPEED_TARGET 2
#define SPEED_FIX_TARGET 3

#define PID_COUNT_PAST 1
#define PID_COUNT_NOW 0

#define PID_DEV_NOW 0
#define PID_KASAN 1
#define PID_DEV_PAST 2

#define M1_TIM_CHANNEL &htim5
#define M2_TIM_CHANNEL &htim3
#define M1_Encoder_COUNT TIM5->CNT
#define M2_Encoder_COUNT TIM3->CNT

namespace motor{
	extern int32_t Motor_target;
	extern int32_t Right_count,Left_count;
	extern int32_t MOTOR_SPEED[4],MOTOR_COUNT[2][2];
	extern int32_t MOTOR_PID_var[2][3];
	extern int32_t lkasan,rkasan,ldevn,rdevn,ldevp,rdevp,lpwm,rpwm;
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
		ONE_ADVANCE=1,
		TWO_ADVANCE=2,
		RIGHT_TURN,
		LEFT_TURN,
		ONE_BACK,
		TWO_BACK,
		HALF_ADVANCE,
		HALF_BACK,
		RIGHT_TURN_NO_GYRO,
		LEFT_TURN_NO_GYRO,
		BRAKE=0
	}move_t;
	enum move_dis_t{
		ONE_BLOCK=2600,
		TWO_BLOCK=5200,
		TURN=6000,
		HALF_BLOCK=1500,
	};

	typedef enum{
		FREE=0,
		BUSY=1,
		PAUSE=2,
		RESTART=3,
		NOPID=4,
		BACK=5,
		RETURN=6,
	}task_status_t;

	extern move_t Task_Before,Task_Save;
	void stm_studio();
	void pid();
	void start_timer();
	void stop_timer();
	void start_encoder();
	void stop_encoder();
	void check_job();
	void task_add(move_t right,move_t left);
	void set_Status(task_status_t s);
	task_status_t check_task();
	int32_t Get_Encoder(ch_t x);
	void brake(ch_t x);
	void forward(ch_t x);
	void back(ch_t x);
	void set_pwm(ch_t x,uint16_t sp);
	void set_pwm(uint16_t sp);
	task_status_t status(void);
	task_status_t status(motor::ch_t m);
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
}

void init_motor(void);
int16_t smaller_s(int16_t x,int16_t y);
void enkaigei(void);
#endif /* MOTOR_CONTROL_H_ */
