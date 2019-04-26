/*
 * action.h
 *
 * Created: 2018/08/23 13:57:09
 *  Author: shun2
 */ 


#ifndef ACTION_H_
#define ACTION_H_
#include <stdint.h>



typedef enum{
    FREE,
    DROP_RIGHT,
    DROP_LEFT,
}kit_drop_status_t;


extern kit_drop_status_t KIT_DROP_Status;
extern uint32_t KIT_DROP_COUNT;

void init_Servo();
void ST_Motor_Move(kit_drop_status_t lr,uint16_t num);
void Drop_kit(kit_drop_status_t lr,uint16_t num);
//void finded_victim(void);
void finded_victim(uint8_t co,uint8_t lr);
bool record_time(uint16_t time);
uint16_t read_time(void);
extern bool Victim_front;
extern uint8_t Victim_front_kit;
#endif /* ACTION_H_ */
