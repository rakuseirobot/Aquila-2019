/*
 * spi_control.hpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */

#ifndef SOURCE_SPI_CONTROL_HPP_
#define SOURCE_SPI_CONTROL_HPP_

#include "../peripheral.hpp"

struct spi{
	SPI_HandleTypeDef *spi_u;
	spi(SPI_HandleTypeDef *spit);
	void send(uint8_t* txBuffer,uint8_t* rxBuffer);
};



#endif /* SOURCE_SPI_CONTROL_HPP_ */
