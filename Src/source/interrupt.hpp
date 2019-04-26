/*
 * interrupt.hpp
 *
 *  Created on: 2019/04/02
 *      Author: shun2
 */

#ifndef SOURCE_INTERRUPT_HPP_
#define SOURCE_INTERRUPT_HPP_

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif
void GPIO_interrupt_callback(uint16_t GPIO_Pin);
void interrupt_callback(TIM_HandleTypeDef *htim);
#ifdef __cplusplus
};
#endif


#endif /* SOURCE_INTERRUPT_HPP_ */
