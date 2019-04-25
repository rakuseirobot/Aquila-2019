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
#include "ui_control.hpp"
#include "delay.hpp"
#include "lcd_control.hpp"
#include "spi_lcd.hpp"
#include "motor_control.hpp"
#include "color_control.hpp"
#include "ping_control.hpp"
#include "mv_control.hpp"
#include "action.hpp"
#include "iris.hpp"
#include <string.h>
uart serial(&huart2);
uart xbee(&huart1);
spi spi_t(&hspi1);
jy901 gyro(&hi2c2);

void cpploop(void) {
	HAL_GPIO_WritePin(GPIOD,ST_MOTOR_CH1_Pin|ST_MOTOR_CH2_Pin|ST_MOTOR_CH3_Pin|ST_MOTOR_CH4_Pin,GPIO_PIN_RESET);
	init_lcd();
	led_count_set(20);
	buzzer();
	init_motor();
	lcd_clear();
	lcd_putstr("Hello");
	xbee.string("Hello!");
	led_count_set(77);
	while(1){
		motor::move(motor::ONE_ADVANCE);
	}
	while(1){
		ping_debug();
	}
	while(1){
		buzzer();
	}
}
