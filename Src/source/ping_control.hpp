/*
 * ping_control.hpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */

#ifndef SOURCE_PING_CONTROL_HPP_
#define SOURCE_PING_CONTROL_HPP_

#include "../peripheral.hpp"

#define Sikiti 600


typedef enum{
    LEFT_BACK=1,
    LEFT_FRONT=2,
    FRONT=3,
    RIGHT_FRONT=4,
    RIGHT_BACK=5,
    PING_BACK=6
}ping_ch_t;


uint16_t ping(int no);
uint16_t ping(ping_ch_t no);
uint16_t ping_check (GPIO_TypeDef* port,uint16_t pin);
uint8_t c_p(int x);
uint16_t smaller(uint16_t x,uint16_t y);
uint8_t check_ping(int x);
void ping_debug(void);




#endif /* SOURCE_PING_CONTROL_HPP_ */
