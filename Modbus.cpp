#include "Modbus.h"

ModbusLite::ModbusLite()
{

}

void ModbusLite::begin(uint8_t slave, Stream &serial, int8_t pin)
{
    _u8MBSlave = slave;
    _serial = &serial;

    pinEn = pin;
    if(pinEn >= 0)
        pinMode(pin, OUTPUT);

}

void ModbusLite::writeSingleRegister(uint16_t u16WriteAddress, uint16_t u16WriteValue)
{
    uint8_t u8ModbusADU[16];///было 256
    uint8_t u8ModbusADUSize = 0;

    u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
    u8ModbusADU[u8ModbusADUSize++] = 0x06;

    u8ModbusADU[u8ModbusADUSize++] = highByte(u16WriteAddress);
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16WriteAddress);

    u8ModbusADU[u8ModbusADUSize++] = highByte(u16WriteValue);
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16WriteValue);

    uint16_t u16CRC = 0xFFFF;
    for (int i = 0; i < u8ModbusADUSize; i++)
    {
        u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
    }
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
    u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
    u8ModbusADU[u8ModbusADUSize] = 0;

    while (_serial->read() != -1); ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    if(pinEn >= 0)
        digitalWrite(pinEn, HIGH);

    for (int i = 0; i < u8ModbusADUSize; i++)
    {
        _serial->write(u8ModbusADU[i]);
    }

    _serial->flush();    // flush transmit buffer

    if(pinEn >= 0)
        digitalWrite(pinEn, LOW);
}

void ModbusLite::writeMultiRegister(uint16_t u16WriteAddress, uint16_t u16WriteValue, uint16_t coint)
{
    uint8_t u8ModbusADU[64];///было 256
    uint8_t u8ModbusADUSize = 0;

    u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
    u8ModbusADU[u8ModbusADUSize++] = 0x10;

    u8ModbusADU[u8ModbusADUSize++] = highByte(u16WriteAddress);
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16WriteAddress);

    u8ModbusADU[u8ModbusADUSize++] = highByte(coint);
    u8ModbusADU[u8ModbusADUSize++] = lowByte(coint);
    u8ModbusADU[u8ModbusADUSize++] = lowByte(coint << 1);//умножить на 2 ?!

    for (int i = 0; i < lowByte(coint); i++)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(u16WriteValue);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(u16WriteValue);
    }

    uint16_t u16CRC = 0xFFFF;
    for (int i = 0; i < u8ModbusADUSize; i++)
    {
        u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
    }
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
    u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
    u8ModbusADU[u8ModbusADUSize] = 0;

    while (_serial->read() != -1); ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if(pinEn >= 0)
        digitalWrite(pinEn, HIGH);

    for (int i = 0; i < u8ModbusADUSize; i++)
    {
        _serial->write(u8ModbusADU[i]);
    }

    _serial->flush();    // flush transmit buffer

    if(pinEn >= 0)
        digitalWrite(pinEn, LOW);
}
