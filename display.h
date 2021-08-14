#ifndef DISPLAY_H
#define DISPLAY_H

//#define DEBUG_ENABLE

#ifndef DEBUG_ENABLE
#define DEBUG(x,y)
#endif

#ifdef DEBUG_ENABLE
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
extern SoftwareSerial mySerial;
#define DEBUG(x,y) mySerial.print(F("[DISPLAY] "));mySerial.print(x);mySerial.println(y)
#endif

#include "GyverTimer.h"
#include "GyverTM1637.h"

class Display
{
public:
    Display(uint8_t clk, uint8_t dio);

    void displayVal(uint8_t bitVal, int16_t val);
    void displayVal(int16_t value);
    void displayVal(uint8_t bit0,uint8_t bit1,uint8_t bit2,uint8_t bit3);
    void displayTwoVal(uint8_t v1,uint8_t v2);

    void displayBlinkOn(bool s0 = true,bool s1 = true,bool s2 = true,bool s3 = true);
    void displayBlinkOff();


    void pointOff();
    void pointOn();
    void pointBlink_1();
    void pointBlink_025();

    void pointLock();
    void pointUnLock();

    void work();

private:
    int8_t pointType = 0; // 0 отключён, 1 мигание частое, 2 мигание редкое, 3 включен    
    int8_t newPointType = 0;


    bool isBlink = false;
    bool blink[4]{};
    uint8_t lastData[4]{};//костыль

    bool lock = false;


    GyverTM1637 disp;
    GTimer dispBlinkTimer;
    GTimer pointBlinkTimer;
    GTimer pointBlinkTimerEn;

    bool dispBlinkFlag = true;

    void pointUpdate();
};

#endif // DISPLAY_H
