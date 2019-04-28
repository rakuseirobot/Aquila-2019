/*
 * wrapper.cpp
 *
 *  Created on: 2019/01/13
 *      Author: shun2
 */

#include "wrapper.hpp"
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
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

void kayaki(void){
	while(1){
		mv_cap(MV_LEFT,true);
		mv_cap(MV_FRONT,true);
		mv_cap(MV_RIGHT,true);
	}
}

void cpploop(void) {
	HAL_GPIO_WritePin(FET2_GPIO_Port,FET2_Pin,GPIO_PIN_SET);
	init_lcd();
	led_count_set(20);
	buzzer();
	init_motor();
	lcd_clear();
	lcd_putstr("Hello");
	xbee.string("Hello!");
	led_count_set(77);
	_h_stack_dfs();
	while(1){
		color_check();
		xbee.putint(red+green+blue);
		xbee.string("\n\r");
	}
	while(1){
		buzzer();
	}
}
