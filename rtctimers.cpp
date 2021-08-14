#include "rtctimers.h"

RTCTimers::RTCTimers(uint8_t sizeTimers): sizeTimers(sizeTimers)
{
    errorTime.setTimeout(90000);
    errorTime.stop();
}

RTCTimers::~RTCTimers()
{
    delete [] startTime;
    delete [] endTime;
    delete [] isTime;
}

void RTCTimers::begin(uint16_t startByte, bool firstStart)
{
    startTime = new uint16_t[sizeTimers]{};
    endTime = new uint16_t[sizeTimers]{};
    isTime = new bool[sizeTimers]{};

    uint8_t i = 0;

    startByte_e = startByte;
    i += sizeTimers * 2 * 2;

    endTime_s = i;//8
    i += sizeTimers * 2 * 2;

    enable_s = i;//16
    i += 2;

    endByte_s  = i;//18

    DEBUG("Start byte: ", startByte);
    DEBUG("End byte: ", startByte_e + endByte_s);
    DEBUG("FirstStart: ", firstStart);

    if(firstStart)
        writeAll();
    else
        readAll();

    errorTime.start();
}

uint16_t RTCTimers::endBute()
{
    return startByte_e + endByte_s;
}

bool RTCTimers::setTimer(uint8_t ind,uint8_t hour1,uint8_t min1,uint8_t hour2,uint8_t min2)
{
    if(ind >= sizeTimers) return false;
    if(hour1 > 23 || min1 > 59) return false;
    if(hour2 > 23 || min2 > 59) return false;

    startTime[ind] = hour1 * 60 + min1;
    endTime[ind] = hour2 * 60 + min2;

    checkTimers();

    writeTimer(ind);

    return true;
}

void RTCTimers::setTimerStart(uint8_t ind, uint8_t hour, uint8_t min)
{
    if(ind >= sizeTimers) return;
    if(hour > 23 || min > 59) return;

    startTime[ind] = hour * 60 + min;

    checkTimers();

    writeTimer(ind);
}

void RTCTimers::setTimerEnd(uint8_t ind, uint8_t hour, uint8_t min)
{
    if(ind >= sizeTimers) return;
    if(hour > 23 || min > 59) return;

    endTime[ind] = hour * 60 + min;

    checkTimers();

    writeTimer(ind);
}

uint8_t RTCTimers::getHourStart(uint8_t ind)
{
    if(ind < 0 || ind >= sizeTimers) return 0;

    return startTime[ind] / 60;
}

uint8_t RTCTimers::getHourEnd(uint8_t ind)
{
    if(ind < 0 || ind >= sizeTimers) return 0;

    return endTime[ind] / 60;
}

uint8_t RTCTimers::getMinuteStart(uint8_t ind)
{
    if(ind < 0 || ind >= sizeTimers) return 0;

    return startTime[ind] % 60;
}

uint8_t RTCTimers::getMinuteEnd(uint8_t ind)
{
    if(ind < 0 || ind >= sizeTimers) return 0;

    return endTime[ind] % 60;
}

void RTCTimers::setEnable(bool val)
{
    if(val == enable) return;

    enable = val;

    checkTimers();

    writeEnable();
}

bool RTCTimers::getEnable()
{
    return enable;
}

void RTCTimers::setTime(uint8_t hour, uint8_t min)
{
    if(hour > 23 || min > 59) return;

    locTime = hour * 60 + min;
}

bool RTCTimers::isOn()
{
    if(isReturnOn) return false;

    isReturnOn = true;

    return isTimeOn;
}

bool RTCTimers::isOff()
{
    if(isReturnOff)return false;

    isReturnOff = true;

    return !isTimeOn;
}

void RTCTimers::work()
{
    if(locTime != lastLocTime && !timeError)
    {
        errorTime.start();

        checkTimers();

        lastLocTime = locTime;
    }

    if(errorTime.isReady())
    {
        setTimeError(true);
    }
}

bool RTCTimers::getTimeError() const
{
    return timeError;
}

void RTCTimers::setTimeError(bool value)
{
    if(timeError == value) return;

    timeError = value;

    if(timeError)
    {
        errorTime.stop();
        checkTimers();
        DEBUG("Time error",);
    }
    else
    {
        errorTime.start();
        checkTimers();
        DEBUG("Time no error",);
    }
}

void RTCTimers::checkTimers()
{
    if(!enable)//выключен
    {
        DEBUG("checkTimers: disabled",);
        setIsTime(enable);
        return;
    }

    if(timeError)//ошибка
    {
      DEBUG("checkTimers: timeError",);
      setIsTime(true);
      return;
    }

    bool isWith = false;
    bool isTimers = false;

    for(uint8_t i = 0; i < sizeTimers;i++)
    {
        isTime[i] = false;
        if(startTime[i] != endTime[i])
        {
            isTimers = true;
            if(startTime[i] < endTime[i])
            {
                if(startTime[i] <= locTime && locTime < endTime[i])
                {
                    isTime[i] = true;
                    isWith = true;
                }
            }
            else
            {
                if(!(endTime[i] <= locTime && locTime < startTime[i]))
                {
                    isTime[i] = true;
                    isWith = true;
                }
            }
        }
    }

    if(!isTimers)//если не одного таймера не заданно
        isWith = true;

    DEBUG("checkTimers: ", isWith);

    setIsTime(isWith);
}

void RTCTimers::setIsTime(bool val)
{
    if(isTimeOn == val) return;

    isTimeOn = val;

    isReturnOn = false;
    isReturnOff = false;
}

void RTCTimers::writeTimer(uint8_t ind)
{
    if(ind >= sizeTimers) return;

    EEPROM.put(startByte_e + (ind * 2), startTime[ind]);//23-24;25-26;
    EEPROM.put(startByte_e + (sizeTimers * 2) + (ind * 2), startTime[ind]);//27-28;29-30;

    EEPROM.put(startByte_e + endTime_s + (ind * 2), endTime[ind]);//31-32;33-34
    EEPROM.put(startByte_e + endTime_s + (sizeTimers * 2) + (ind * 2), endTime[ind]);//35-36;37-38

    DEBUG("write: ", ind);
}

void RTCTimers::writeAll()
{
    for(int i = 0; i < sizeTimers; i++)
    {
        writeTimer(i);
    }

    writeEnable();
}

void RTCTimers::readAll()
{
    for(int i = 0; i < sizeTimers; i++)
    {
        volatile uint16_t timeStart1;
        volatile uint16_t timeStart2;
        volatile uint16_t timeEnd1;
        volatile uint16_t timeEnd2;

        EEPROM.get(startByte_e + (i * 2), timeStart1);//23-24;25-26;
        EEPROM.get(startByte_e + (sizeTimers * 2) + (i * 2), timeStart2);//27-28;29-30;

        if(timeStart1 != timeStart2 || timeStart1 > 1439) continue;

        EEPROM.get(startByte_e + endTime_s + (i * 2), timeEnd1);//31-32;33-34
        EEPROM.get(startByte_e + endTime_s + (sizeTimers * 2) + (i * 2), timeEnd2);//35-36;37-38

        if(timeEnd1 != timeEnd2 || timeEnd1 > 1439) continue;

        DEBUG("Read startTime: ", timeStart1);
        DEBUG("Read endTime: ", timeEnd1);

        startTime[i] = timeStart1;
        endTime[i] = timeEnd1;
    }

    bool enable_t1 = true;
    bool enable_t2 = false;

    EEPROM.get(startByte_e + enable_s, enable_t1);
    EEPROM.get(startByte_e + enable_s + 1, enable_t2);

    if(enable_t1 != enable_t2)//при преоброзовании из int в bool и обратно в int сохраяется значение а не 0 или 1
        writeEnable();
    else
        enable = enable_t1;
}

void RTCTimers::writeEnable()
{
    DEBUG("writeEnable: ", enable);

    EEPROM.put(startByte_e + enable_s, enable);
    EEPROM.put(startByte_e + enable_s + 1, enable);
}
