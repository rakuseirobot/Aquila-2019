/*
 * color_control.h
 *
 * Created: 2018/08/23 18:28:14
 *  Author: shun2
 */ 


#ifndef COLOR_CONTROL_H_
#define COLOR_CONTROL_H_

#include <stdint.h>

#define RANGE PIN0_bm
#define CK PIN1_bm
#define GATE PIN2_bm
#define DOUT PIN3_bm

extern uint16_t red, green, blue;

void init_color(void);
void color_read(void);
uint16_t color_shift(void);
uint8_t color_check(void);

#endif /* COLOR_CONTROL_H_ */
