/*
 * JY901.h
 *
 *  Created on: 2019/01/13
 *      Author: shun2
 */

#ifndef SOURCE_JY901_H_
#define SOURCE_JY901_H_
#include "stm32f4xx_hal.h"

class jy901{
	public:
		uint16_t addr;
		jy901(I2C_HandleTypeDef *jy_i2ct,uint16_t ad=0x50<<1);
		float read_angle(void);
		float read_acc_x(void);
		float read_acc_y(void);
		float read_acc_z(void);
		float read_moving_angle(void);
	private:
		I2C_HandleTypeDef *jy_i2c;
};


#endif /* SOURCE_JY901_H_ */
