/*
 * spi_control.cpp
 *
 *  Created on: 2019/04/01
 *      Author: shun2
 */
#include "spi_control.hpp"

spi::spi(SPI_HandleTypeDef *spit){
	spi_u=spit;
}
void spi::send(uint8_t* txBuffer,uint8_t* rxBuffer){
	uint8_t len = sizeof(txBuffer) / sizeof(txBuffer);
	HAL_SPI_TransmitReceive(spi_u,(uint8_t*)txBuffer,(uint8_t*)rxBuffer,len,2000);
	return;
}
