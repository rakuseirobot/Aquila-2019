/*
 * wrapper.cpp
 *
 *  Created on: 2019/01/13
 *      Author: shun2
 */

#include "wrapper.hpp"

#include "stm32f4xx_hal.h"
#include "main.h"
#include <math.h>
#include "peripheral.hpp"
#include "uart_control.hpp"
#include "spi_control.hpp"
#include "JY901.hpp"
uart serial(&huart2);
spi spi_t(&hspi1);
jy901 gyro(&hi2c2);
void cpploop(void) {
	while(1){
		HAL_GPIO_TogglePin(E1_GPIO_Port,E1_Pin);
		HAL_GPIO_TogglePin(CL_GPIO_Port,CL_Pin);
		HAL_Delay(1);
	}
}


