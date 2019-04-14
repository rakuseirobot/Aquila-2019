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
uart serial(&huart2);
uart xbee(&huart1);
spi spi_t(&hspi1);
jy901 gyro(&hi2c2);
uint32_t int_count=0;
void cpploop(void) {
	init_lcd();
	led_count_set(20);
	buzzer();
	init_motor();
	motor::set_pwm(500);
	led_count_set(30);
	HAL_Delay(1000);
	led_count_set(29);
	lcd_clear();
	lcd_putstr("Hello");
	led_count_set(77);
	while(1){
		motor::move(motor::ONE_ADVANCE);
		led_one_count();
		HAL_Delay(1000);
	}
}


