#include "DimControl.h"

DimControl::DimControl(uint8_t pinSwitch,uint8_t slave,
                       uint8_t nLux1, uint8_t nLux2,
                       uint8_t stabLux,uint16_t timeNormLux, uint16_t timeStabLux):
    pinSwitch(pinSwitch),
    nLux1(nLux1),
    nLux2(nLux2),
    stabLuxK(stabLux),
    timeNormLux(timeNormLux),
    timeStabLux(timeStabLux)
{
    modBus.begin(slave,Serial,pinSwitch);
}

void DimControl::begin(uint16_t startByte, bool firstStart)
{
    uint16_t i = startByte;

    const uint8_t cointSaves = 2;

    dimValue_e = i;//10
    i += sizeof(dimValue) * cointSaves;//14

    autoMode_e = i;
    i += sizeof(autoMode) * cointSaves;//16

    toLux_e = i;
    i += sizeof(toLux) * cointSaves;//20

    error_e = i;
    i += sizeof(error) * cointSaves;//22

    endByte = i;

    DEBUG("Start byte: ",startByte);
    DEBUG("End byte: ",endByte);
    DEBUG("FirstStart: ",firstStart);


    if(firstStart)
        writeAll();
    else
        readAll();

    dimPercent = dimmToPercent(newDimVal);

    rangeLuxTimer.setTimeout(timeNormLux);
    rangeLuxTimer.stop();

    stabLuxTimer.setTimeout(timeStabLux);
    stabLuxTimer.stop();

    if (autoMode)
        busUpdatedTimer.setInterval(5000);
    else
        busUpdatedTimer.setInterval(1000);

    stabLuxTimerLoop.setInterval(300);
    writeValDimmTimer.setTimeout(60000);
    writeValDimmTimer.stop();

    DEBUG("error: ",error);
    DEBUG("newDimVal: ",newDimVal);
    DEBUG("dimPercent: ", dimPercent);
    DEBUG("autoMode: ",autoMode);
    DEBUG("enable: ",enable);

}

uint16_t DimControl::endBute()
{
    return endByte;
}

bool DimControl::isError()
{
    return error;
}

void DimControl::resetError()
{
    error = false;
    writeError();
}

void DimControl::setAuto()
{
    if(autoMode) return;

    DEBUG("setAuto","");

    autoMode = true;
    busUpdatedTimer.setInterval(5000);
    writeMode();
}

void DimControl::setHand()
{
    if(!autoMode) return;

    DEBUG("setHand","");

    autoMode = false;
    busUpdatedTimer.setInterval(1000);
    writeMode();
}

void DimControl::setOn()
{
    if(enable) return;

    DEBUG("setOn","");

    enable = true;    
}

void DimControl::setOff()
{
    if(!enable) return;

    DEBUG("setOff","");

    enable = false;    
}

void DimControl::setNoSend(bool val)
{
    hardEnable = !val;
}

bool DimControl::getAuto()
{
    return autoMode;
}

bool DimControl::getHand()
{
    return !autoMode;
}

bool DimControl::getOff()
{
    return !enable;
}

bool DimControl::getOn()
{
    return enable;
}

void DimControl::setDim(int16_t value)
{
    if(autoMode) return;

    if(value > 1023 || value < 0) return;

    newDimVal = value;

    writeValDimmTimer.start();
}

void DimControl::setDimPercent(uint8_t value)
{
    if(autoMode) return;

    if(value > 100 || value < 0) return;

    newDimVal = percentToDim(value);
    dimPercent = value;

    writeValDimmTimer.start();
}

void DimControl::setToLux(int16_t lux)
{
    if(lux < 0 || lux > 9999) return;

    toLux = lux;

    writeToLux();
}

int16_t DimControl::getDim()
{
    return dimValue;
}

uint8_t DimControl::getDimPercent()
{
    return dimPercent;
}

int16_t DimControl::getToLux()
{
    return toLux;
}

bool DimControl::isRange()
{
    return rangeLux;
}

void DimControl::work(int16_t _lux)
{
    lux = _lux;

    if (_lux >= luxValStab + stabLuxK || _lux + stabLuxK <= luxValStab)
    {
        luxStab = false;
#ifdef DEBUG_ENABLE
        digitalWrite(13, HIGH);
#endif
        stabLuxTimer.start();
    }

    if (stabLuxTimerLoop.isReady())
    {
        luxValStab = _lux;       
    }

    if(stabLuxTimer.isReady())
    {
        luxStab = true;

         DEBUG("Stab lux",);
#ifdef DEBUG_ENABLE
        digitalWrite(13, LOW);
#endif
    }

    if(rangeLuxTimer.isReady())
    {
        rangeLux = true;
        DEBUG("Value in range",);
    }

    if(writeValDimmTimer.isReady())
    {
        writeDimValue();
    }

    if(busUpdatedTimer.isReady())
    {
        if(enable)
        {
            if(autoMode)
                sendValueAuto();
            else
                sendValue();
        }
        else
        {
            sendValueOff();
        }
    }
}

void DimControl::sendValueAuto()
{
    if (!hardEnable) return;

    if (rangeLux && !luxStab) return;

    int x = toLux - lux;

    if (rangeLux)
        if (!(x > nLux2 || x < -nLux2)) return;

    if (x > nLux1 || x < -nLux1)
    {
        if (x > 10)  x = 10;
        if (x < -10) x = -10;

        if(x < 4 && x > 0) x = 1;
        if(x > -4 && x < 0) x = -1;


        newDimVal = newDimVal + x;

        if (newDimVal > 1023) newDimVal = 1023;
        if (newDimVal < 0) newDimVal = 0;

        dimPercent = dimmToPercent(newDimVal);

        sendValue();

        rangeLux = false;
        rangeLuxTimer.start();
        writeValDimmTimer.start();
    }
}

void DimControl::sendValue()
{
    if (!hardEnable) return;    

    if (newDimVal < 0) newDimVal = 0;
    if (newDimVal > 1023) newDimVal = 1023;

    if (newDimVal != dimValue)
    {
        DEBUG("Bus value: ", newDimVal);


#ifdef MODBUS_ENABLE
        modBus.writeMultiRegister(40, newDimVal, 4);
#endif

        dimValue = newDimVal;
    }
}

void DimControl::sendValueOff()
{
    if (!hardEnable) return;

    if (0 != dimValue)
    {
        DEBUG("Bus value: ", 0);

#ifdef MODBUS_ENABLE
        modBus.writeMultiRegister(40, 0, 4);
#endif

        dimValue = 0;
    }
}

void DimControl::writeDimValue()
{
    DEBUG("writeDimValue: ",newDimVal);

    EEPROM.put(dimValue_e, newDimVal);
    EEPROM.put(dimValue_e + 2, newDimVal);
}

void DimControl::readDimValue(bool &err)
{
    int16_t dimValue_t1;
    int16_t dimValue_t2;

    EEPROM.get(dimValue_e, dimValue_t1);
    EEPROM.get(dimValue_e + 2, dimValue_t2);

    if(dimValue_t1 != dimValue_t2 || dimValue_t1 < 0 || dimValue_t1 > 1023)
    {
        err = true;
        writeDimValue();
    }
    else
        newDimVal = dimValue_t1;
}

void DimControl::writeMode()
{
    DEBUG("writeMode: ",autoMode);

    EEPROM.put(autoMode_e, autoMode);
    EEPROM.put(autoMode_e + 1, autoMode);
}

void DimControl::readMode(bool &err)
{
    bool autoMode_t1;
    bool autoMode_t2;

    EEPROM.get(autoMode_e, autoMode_t1);
    EEPROM.get(autoMode_e + 1, autoMode_t2);

    if(autoMode_t1 != autoMode_t2)
    {
        err = true;
        writeMode();
    }
    else
        autoMode = autoMode_t1;
}

void DimControl::writeToLux()
{
    DEBUG("writeToLux: ", toLux);

    EEPROM.put(toLux_e, toLux);
    EEPROM.put(toLux_e + 2, toLux);
}

void DimControl::readToLux(bool &err)
{
    int16_t toLux_t1;
    int16_t toLux_t2;

    EEPROM.get(toLux_e, toLux_t1);
    EEPROM.get(toLux_e + 2, toLux_t2);

    if(toLux_t1 != toLux_t2 || toLux_t1 > 80 || toLux_t1 < 0)
    {
        err = true;
        writeToLux();
    }
    else
        toLux = toLux_t1;
}

void DimControl::writeError()
{
    DEBUG("writeError: ", error);

    EEPROM.put(error_e, error);
    EEPROM.put(error_e + 1, error);
}

void DimControl::readError(bool &err)
{
    bool error_t1;
    bool error_t2;

    EEPROM.get(error_e, error_t1);
    EEPROM.get(error_e + 1, error_t2);

    if(error_t1 != error_t2)
    {
        err = true;
        writeError();
    }
    else
        error = error_t1;
}

void DimControl::writeAll()
{
    writeDimValue();
    writeMode();
    writeToLux();
    writeError();
}

void DimControl::readAll()
{
    bool temp = false;

    readDimValue(temp);
    readMode(temp);
    readToLux(temp);
    readError(temp);

    if(temp)
        error = temp;

    writeError();
}

int8_t DimControl::dimmToPercent(int16_t dim)
{
    int8_t temp;

    temp = round(100 / (1023 / (float)dim));
    if (temp > 100) temp = 100;
    if (temp < 0) temp = 0;

    return temp;
}

int16_t DimControl::percentToDim(int8_t percent)
{
    if(percent <= 0) return 0;
    if(percent >= 100) return 1023;

    return round(percent * (1023 / (float)100));
}
