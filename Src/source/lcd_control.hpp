/*
 * lcd_control.h
 *
 * Created: 2018/08/23 3:51:49
 *  Author: shun2
 */ 


#ifndef LCD_CONTROL_H_
#define LCD_CONTROL_H_
#include <avr/io.h>
#include <util/delay.h>
#include "xmega_twi.hpp"
#include "initializing.hpp"


#define LCD1_TWI &TWIF
#define LCD1_SLvADD 0x7c
#define lcd_fre 300000

void lcd_putstr(TWI_t *port, const char *data);
void lcd_putmath(TWI_t *port,uint16_t num);
void lcd_putdec(TWI_t *port, uint16_t num);
void init_lcd(void);

void lcd_clear(void);




#endif /* LCD_CONTROL_H_ */