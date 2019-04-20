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
#include "uart_control.hpp"

extern uart xbee;

void _delay_s(uint32_t time){
	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT< (time*700000));
	HAL_TIM_Base_Stop(&htim7);
	return;
}
void _delay_ms(uint32_t time){
	HAL_Delay(time);
	return;
}

void _delay_us(uint32_t time){

	htim7.Instance->CNT = 0;
	HAL_TIM_Base_Start(&htim7);
	while(htim7.Instance->CNT < time && HAL_TIM_Base_GetState(&htim7)==HAL_OK){
	}
	HAL_TIM_Base_Stop(&htim7);
	return;
}
