#include "luxreader.h"

LuxReader::LuxReader()
{

}

void LuxReader::begin(uint16_t startByte, bool firstStart)
{
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE,0x23))
    {
        error = true;
    }

    calib_e = startByte;

    endByte = calib_e + sizeof(calib) * 2;

    DEBUG("Start byte: ", startByte);
    DEBUG("End byte: ", endByte);
    DEBUG("FirstStart: ", firstStart);

    if(firstStart)
        writeCalib();
    else
        readCalib();
}

uint16_t LuxReader::endBute()
{
    return endByte;
}

void LuxReader::setCalib(int8_t val)
{
    calib = val;

    writeCalib();
}

int8_t LuxReader::getCalib()
{
    return calib;
}

bool LuxReader::isLux()
{
    if(error) return false;

    if(!lightMeter.measurementReady()) return false;

    float tempLux = lightMeter.readLightLevel();

    if(tempLux < 0)
    {
        error = true;
        return false;
    }

    int temp = (int)(filter(tempLux) + calib);

    bool is = true;

    if(temp == lux)
        is = false;

    lux = temp;

    return is;
}

int16_t LuxReader::getLux()
{
    return lux;
}

bool LuxReader::isError()
{
    return error;
}

float LuxReader::filter(float value)
{
    _lastValue += (float)(value - _lastValue) * 0.1;
    return _lastValue;
}

void LuxReader::writeCalib()
{
    int size = sizeof(calib);

    DEBUG("writeCalib: ", calib);

    EEPROM.put(calib_e, calib);
    EEPROM.put(calib_e + size, calib);
}

void LuxReader::readCalib()
{
    int size = sizeof(calib);

    auto calib_t1 = calib;
    auto calib_t2 = calib;

    EEPROM.get(calib_e, calib_t1);
    EEPROM.get(calib_e + size, calib_t2);

    if(calib_t1 != calib_t2 || calib_t1 > 25 || calib_t1 < -25)
    {
        writeCalib();
    }
    else
    {
        calib = calib_t1;
        DEBUG("readCalib: ", calib);
    }
}
