/*
 * interrupt.cpp
 *
 *  Created on: 2019/04/02
 *      Author: shun2
 */

#include "interrupt.hpp"
#include "../peripheral.hpp"
#include "motor_control.hpp"


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim13)
	{
		motor::check_job();
	}
}
