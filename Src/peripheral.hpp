/*
 * peripheral.hpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */

#ifndef PERIPHERAL_HPP_
#define PERIPHERAL_HPP_

#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"

extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim13;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;




#endif /* PERIPHERAL_HPP_ */
