/*
 * ui_control.h
 *
 * Created: 2018/08/23 6:05:02
 *  Author: shun2
 */ 


#ifndef UI_CONTROL_H_
#define UI_CONTROL_H_

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f429xx.h"
#include "stm32f4xx_hal_gpio.h"

#define Greenled FLED_GREEN_Pin
#define Blueled FLED_BLUE_Pin
#define Redled FLED_RED_Pin

#define SW1 HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)!=0
#define SW2 HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin)!=0
#define SW3 HAL_GPIO_ReadPin(SW3_GPIO_Port,SW3_Pin)!=0

void led_one_count(void);
void led_count_set(uint8_t count);
void init_buzzer(void);
void buzzer(uint16_t t=800);
void delay_us(uint16_t t);
void led(uint16_t ledkind,uint8_t ledmode);
void error_led(uint8_t no,uint8_t i);
void start(void);


#endif /* UI_CONTROL_H_ */
