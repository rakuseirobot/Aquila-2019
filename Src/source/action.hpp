/*
 * action.h
 *
 * Created: 2018/08/23 13:57:09
 *  Author: shun2
 */ 


#ifndef ACTION_H_
#define ACTION_H_
#include <stdint.h>

void init_Servo();
void Drop_kit(uint8_t lr,uint16_t num=120);
void throw_kit(void);
//void finded_victim(void);
void finded_victim(uint8_t co,uint8_t lr);
bool record_time(uint16_t time);
uint16_t read_time(void);
extern bool Victim_front;
extern uint8_t Victim_front_kit;
#endif /* ACTION_H_ */