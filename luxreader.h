#ifndef LUXREADER_H
#define LUXREADER_H

//#define DEBUG_ENABLE

#ifndef DEBUG_ENABLE
#define DEBUG(x,y)
#endif

#ifdef DEBUG_ENABLE
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
extern SoftwareSerial mySerial;
#define DEBUG(x,y) mySerial.print(F("[LUXREADER] "));mySerial.print(x);mySerial.println(y)
#endif

#include <BH1750.h>
#include <EEPROM.h>

class LuxReader
{
public:
    LuxReader();

    void begin(uint16_t startByte, bool firstStart);
    uint16_t endBute();

    void setCalib(int8_t val);
    int8_t getCalib();

    bool isLux();
    int16_t getLux();

    bool isError();
private:
    BH1750 lightMeter;
    bool error = false;
    int8_t calib = 0;
    int16_t lux = 0;

    uint16_t calib_e = 0;
    uint16_t endByte = 0;

    float _lastValue = 0.0;
    float filter(float value);

    void writeCalib();
    void readCalib();
};

#endif // LUXREADER_H
