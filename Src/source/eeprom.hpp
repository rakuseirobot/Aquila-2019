/*
 * eeorom.h
 *
 * Created: 2019/02/02 18:17:37
 *  Author: shun2
 */ 


#ifndef EEOROM_H_
#define EEOROM_H_

#include <stdint.h>

namespace erom{
    void WritePage(uint16_t eeaddress,uint8_t *data);
    void WriteSingle(uint16_t eeaddress,uint8_t data );
    uint8_t ReadSingle(uint16_t eeaddress);
    void ReadMulti(uint16_t eeaddress,uint8_t *d,uint16_t val);
}


#endif /* EEOROM_H_ */