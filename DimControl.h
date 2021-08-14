#ifndef DIMCONTROL_H
#define DIMCONTROL_H

//#define DEBUG_ENABLE
#define MODBUS_ENABLE


#ifndef DEBUG_ENABLE
#define DEBUG(x,y)
#endif

#ifdef DEBUG_ENABLE
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
extern SoftwareSerial mySerial;
#define DEBUG(x,y) mySerial.print(F("[DIMCONTROL] "));mySerial.print(x);mySerial.println(y)
#endif

#include <EEPROM.h>
#include "Modbus.h"
#include "GyverTimer.h"

class DimControl
{

public:
    DimControl(uint8_t pinSwitch, uint8_t slave,
               uint8_t nLux1, uint8_t nLux2,
               uint8_t stabLux, uint16_t timeNormLux, uint16_t timeStabLux);

    void begin(uint16_t startByte,bool firstStart);    
    uint16_t endBute();

    bool isError();
    void resetError();

    void setAuto();
    void setHand();
    void setOn();
    void setOff();    
    void setNoSend(bool val);

    bool getAuto();
    bool getHand();
    bool getOff();
    bool getOn();

    void setDim(int16_t value);
    void setDimPercent(uint8_t value);
    void setToLux(int16_t lux);

    int16_t getDim();
    uint8_t getDimPercent();
    int16_t getToLux();

    bool isRange();

    void work(int16_t lux);

private:
    uint8_t pinSwitch;

    bool error = false;
    bool autoMode = false;
    bool enable = true;

    bool hardEnable = false;

    int16_t dimValue = 5;
    int16_t newDimVal = 0;
    int16_t handDimVal = 0;
    int8_t dimPercent = 0;

    int16_t lux = 0;
    int16_t toLux = 5;
    int16_t luxValStab = 0;

    uint8_t nLux1 = 0;
    uint8_t nLux2 = 0;
    uint8_t stabLuxK = 1;
    uint16_t timeNormLux = 30000;
    uint16_t timeStabLux = 30000;

    //адреса байтов хранения значений
    uint16_t dimValue_e = 0;
    uint16_t autoMode_e = 0;
    uint16_t toLux_e = 0;
    uint16_t error_e = 0;
    uint16_t endByte = 0;

    bool rangeLux = false;
    bool luxStab = false;

    ModbusLite modBus;
    GTimer rangeLuxTimer;
    GTimer stabLuxTimer;
    GTimer stabLuxTimerLoop;
    GTimer writeValDimmTimer;
    GTimer busUpdatedTimer;

    void sendValueAuto();
    void sendValue();
    void sendValueOff();


    void writeDimValue();
    void readDimValue(bool &err);

    void writeMode();
    void readMode(bool &err);

    void writeToLux();
    void readToLux(bool &err);

    void writeError();
    void readError(bool &err);


    void writeAll();
    void readAll();

    int8_t dimmToPercent(int16_t dim);
    int16_t percentToDim(int8_t percent);

};

#endif
