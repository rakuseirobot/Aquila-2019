/*
 * interrupt.cpp
 *
 *  Created on: 2019/04/02
 *      Author: shun2
 */

#include "interrupt.hpp"
#include "stm32f4xx.h"
#include "../peripheral.hpp"
#include "motor_control.hpp"
#include "ui_control.hpp"
#include "uart_control.hpp"
#include "action.hpp"
#include "mv_control.hpp"
#include "stm32f4xx.h"

//extern uart xbee;
extern uint32_t KIT_DROP_COUNT;
extern kit_drop_status_t KIT_DROP_Status;
void GPIO_interrupt_callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
		case MVS1_Pin:
		case MVS2_Pin:
		case MVS3_Pin:
			int_task_check_mv(GPIO_Pin);
			break;
		default:
			break;
	}
	return;
}
void interrupt_callback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2){
		if(KIT_DROP_COUNT>0){
			ST_Motor_Move(KIT_DROP_Status,KIT_DROP_COUNT);
			KIT_DROP_COUNT-=1;
		}
		else{
			HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
			KIT_DROP_COUNT=0;
			KIT_DROP_Status=FREE;
			HAL_TIM_Base_Stop_IT(&htim2);
		}
	}
	if (htim->Instance == TIM13)
	{
		motor::check_job();
		if(motor::status()==motor::BUSY){
			motor::pid();
		}
		motor::stm_studio();
		M1_Encoder_COUNT=32767;
		M2_Encoder_COUNT=32767;
		/*motor::pid();*/
	}
}
