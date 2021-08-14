#ifndef RTCTIMERS_H
#define RTCTIMERS_H

//#define DEBUG_ENABLE

#ifndef DEBUG_ENABLE
#define DEBUG(x,y)
#endif

#ifdef DEBUG_ENABLE
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
extern SoftwareSerial mySerial;
#define DEBUG(x,y) mySerial.print(F("[RTCTIMERS] "));mySerial.print(x);mySerial.println(y)
#endif

#include <inttypes.h>
#include <EEPROM.h>
#include "GyverTimer.h"

class RTCTimers
{
public:
    RTCTimers(uint8_t sizeTimers);
    ~RTCTimers();

    void begin(uint16_t startByte,bool firstStart);
    uint16_t endBute();

    bool setTimer(uint8_t ind,uint8_t hour1,uint8_t min1,uint8_t hour2,uint8_t min2);
    void setTimerStart(uint8_t ind,uint8_t hour,uint8_t min);
    void setTimerEnd(uint8_t ind,uint8_t hour,uint8_t min);

    uint8_t getHourStart(uint8_t ind);
    uint8_t getHourEnd(uint8_t ind);
    uint8_t getMinuteStart(uint8_t ind);
    uint8_t getMinuteEnd(uint8_t ind);
    void setEnable(bool val);
    bool getEnable();

    ///вызвать до work(!!!)
    void setTime(uint8_t locTime,uint8_t min);

    bool isOn();
    bool isOff();

    void work();

    bool getTimeError() const;
    void setTimeError(bool value);

private:
    const uint8_t sizeTimers;
    uint16_t locTime = 0;//минуты в сутках
    uint16_t lastLocTime = 0;

    uint16_t startByte_e = 0;
    uint8_t endTime_s = 0;
    uint8_t enable_s = 0;
    uint8_t endByte_s = 0;

    bool enable = true;
    bool timeError = false;

    bool isTimeOn = false;

    bool isReturnOn = false;
    bool isReturnOff = false;

    uint16_t *startTime;
    uint16_t *endTime;
    bool *isTime;

    void checkTimers();
    void setIsTime(bool val);

    void writeTimer(uint8_t ind);
    void writeAll();
    void readAll();

    void writeEnable();


    GTimer errorTime;
};

#endif // RTCTIMERS_H
