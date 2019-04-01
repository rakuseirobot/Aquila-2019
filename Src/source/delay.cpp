/*
 * delay.cpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */
#include "../peripheral.hpp"
#include "delay.hpp"
#include <stdint.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f429xx.h>

void _delay_s(uint32_t time){
	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT< (time*1000000));
	HAL_TIM_Base_Stop(&htim7);
	return;
}
void _delay_ms(uint32_t time){
	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT < (time*1000));
	HAL_TIM_Base_Stop(&htim7);
	return;
}

void _delay_us(uint32_t time){
	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT < time);
	HAL_TIM_Base_Stop(&htim7);
	return;
}
