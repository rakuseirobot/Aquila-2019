/*
 * interrupt.cpp
 *
 *  Created on: 2019/04/02
 *      Author: shun2
 */

#include "interrupt.hpp"
#include "../peripheral.hpp"
#include "motor_control.hpp"
#include "ui_control.hpp"

extern uint32_t int_count;

void interrupt_callback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM13)
	{
		motor::check_job();
		if(motor::status()==motor::BUSY){
			motor::check_Enocoder();
			motor::pid();
		}
		motor::stm_studio();
		M1_Encoder_COUNT=32767;
		M2_Encoder_COUNT=32767;
		/*motor::pid();*/
	}
}
