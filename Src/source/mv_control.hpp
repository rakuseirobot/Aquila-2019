/*
 * mv_control.h
 *
 * Created: 2018/09/18 16:51:37
 *  Author: shun2
 */ 


#ifndef MV_CONTROL_HPP_
#define MV_CONTROL_HPP_
#include <stdint.h>
#include "../peripheral.hpp"

#define MV_DATA_DIR 0
#define MV_DATA_TYPE 1
#define MV_DATA_PING 2

#define NOT_FIND_FRONT_WALL 0
#define FIND_FRONT_WALL 1

#define FIND_NOTHING 0
#define FIND_H 3
#define FIND_U 5
#define FIND_S 4
#define FIND_SERMO 6
#define FIND_ERROR 8



void mv_sig(uint8_t i,bool ud);
void init_mv(void);
bool kit_chk(uint8_t num);

extern uint8_t MV_RECIEVED_DATA[3];

typedef enum{
    MV_LEFT = 1,
    MV_FRONT = 2,
    MV_RIGHT = 3,
}mv_ch_t;
void int_task_check_mv(uint16_t GPIO_Pin);
void mv_cap(mv_ch_t di,bool st);
uint8_t mv_spi_send(mv_ch_t i,uint8_t val);
void mv_after_stop_task_check(void);
void mv_task_check(void);
bool check_sig(bool check = true);
void int_task_check_mv(uint16_t GPIO_Pin);
void throw_kit_at_front(uint8_t lr,uint8_t type);
uint8_t check_mv(mv_ch_t dir);


#endif /* MV_CONTROL_H_ */
