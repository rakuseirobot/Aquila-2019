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

void mv_sig(uint8_t i,bool ud);
void init_mv(void);
bool kit_chk(uint8_t num);

typedef enum{
    MV_LEFT,
    MV_FRONT,
    MV_RIGHT
}mv_ch_t;
void mv_cap(mv_ch_t di,bool st);
uint8_t mv_spi_send(mv_ch_t i,uint8_t val);
bool check_sig(bool check = true);
void int_task_check_mv(uint16_t GPIO_Pin);
uint8_t check_mv(mv_ch_t dir);


#endif /* MV_CONTROL_H_ */
