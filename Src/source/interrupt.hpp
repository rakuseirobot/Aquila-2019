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
void interrupt_callback(TIM_HandleTypeDef *htim);
#ifdef __cplusplus
};
#endif


#endif /* SOURCE_INTERRUPT_HPP_ */
