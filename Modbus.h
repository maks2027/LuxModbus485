#ifndef MODBUSLITE_H
#define MODBUSLITE_H

#include "Arduino.h"

class ModbusLite
{
    Stream* _serial;
    uint8_t  _u8MBSlave;
    int8_t pinEn = -1;

    uint16_t crc16_update(uint16_t crc, uint8_t a)
    {
        int i;

        crc ^= a;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        }

        return crc;
    }


public:
    ModbusLite();

    void begin(uint8_t slave, Stream &serial,int8_t pin = -1);


    void writeSingleRegister(uint16_t u16WriteAddress, uint16_t u16WriteValue);
    void writeMultiRegister(uint16_t u16WriteAddress, uint16_t u16WriteValue, uint16_t coint);

};

#endif
