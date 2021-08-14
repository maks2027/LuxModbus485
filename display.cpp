#include "display.h"

Display::Display(uint8_t clk, uint8_t dio): disp(clk,dio)
{
    disp.brightness(7);
    disp.clear();
}

void Display::displayVal(uint8_t bitVal, int16_t val)
{    
    if(isBlink)
        dispBlinkTimer.setTimeout(1000);

    bool negative = false;
    if (val < 0) negative = true;

    if (val > 999)
    {
        lastData[1] = 9;
        lastData[2] = 9;
        lastData[3] = 9;
    }
    else if (val < -99)
    {
        lastData[2] = 9;
        lastData[3] = 9;
    }
    else
    {
        val = abs(val);

        lastData[1] = (int)val / 100;
        uint16_t temp = (int)lastData[1] * 100;
        lastData[2] = ((int)val - temp) / 10;
        temp += lastData[2] * 10;
        lastData[3] = val - temp;
    }

    disp.displayByte(0, bitVal);
    lastData[0] = bitVal;

    if (negative)
    {
        disp.displayByte(1,  0x40);
        lastData[1] = 0x40;
    }
    else
    {
        disp.display(1, lastData[1]);
    }

    disp.display(2, lastData[2]);
    disp.display(3, lastData[3]);
}

void Display::displayVal(int16_t value)
{
    if (value > 9999 || value < -999) return;
    boolean negative = false;

    if(isBlink)
        dispBlinkTimer.setTimeout(1000);

    if (value < 0) negative = true;
    value = abs(value);
    lastData[0] = (int)value / 1000;      	// количесто тысяч в числе
    uint16_t b = (int)lastData[0] * 1000; 	// вспомогательная переменная
    lastData[1] = ((int)value - b) / 100; 	// получем количество сотен
    b += lastData[1] * 100;               	// суммируем сотни и тысячи
    lastData[2] = (int)(value - b) / 10;  	// получем десятки
    b += lastData[2] * 10;                	// сумма тысяч, сотен и десятков
    lastData[3] = value - b;              	// получаем количество единиц

    if (!negative) {
        for (byte i = 0; i < 3; i++) {
            if (lastData[i] == 0) lastData[i] = 10;
            else break;
        }
    } else {
        for (byte i = 0; i < 3; i++) {
            if (lastData[i] == 0) {
                if (lastData[i + 1] == 0){
                    lastData[i] = 10;
                } else {
                    lastData[i] = 11;
                    break;
                }
            }
        }
    }

    disp.display(lastData);
}

void Display::displayVal(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3)
{
    if(isBlink)
        dispBlinkTimer.setTimeout(1000);

    disp.displayByte(bit0,bit1,bit2,bit3);
}

void Display::displayTwoVal(uint8_t v1, uint8_t v2)
{
    if (v1 > 99 || v2 > 99) return;

    if(isBlink)
        dispBlinkTimer.setTimeout(1000);

    lastData[0] = (v1 / 10);
    lastData[1] = v1 % 10;
    lastData[2] = v2 / 10;
    lastData[3] = v2 % 10;

    disp.display(lastData);
}

void Display::displayBlinkOn(bool s0, bool s1, bool s2, bool s3)
{
    blink[0] = s0;
    blink[1] = s1;
    blink[2] = s2;
    blink[3] = s3;

    isBlink = true;

    DEBUG("displayBlinkOn","");

    dispBlinkTimer.setTimeout(100);
}

void Display::displayBlinkOff()
{    
    dispBlinkTimer.stop();

    isBlink = false;

    disp.display(lastData);

    DEBUG("displayBlinkOff","");
}

void Display::pointOff()
{
    if(newPointType == 0) return;

    newPointType = 0;

    DEBUG("pointOff","");

    pointUpdate();
}

void Display::pointOn()
{
    if(newPointType == 3) return;

    newPointType = 3;

    DEBUG("pointOn","");

    pointUpdate();
}

void Display::pointBlink_1()
{
    if(newPointType == 2) return;
    newPointType = 2;

    DEBUG("pointBlink: ","1 hz");

    pointUpdate();
}

void Display::pointBlink_025()
{
    if(newPointType == 1) return;
    newPointType = 1;

    DEBUG("pointBlink: ","0.25 hz");

    pointUpdate();
}

void Display::pointLock()
{
    lock = true;
}

void Display::pointUnLock()
{
    if(!lock) return;

    lock = false;

    pointUpdate();
}

void Display::work()
{
    if(pointBlinkTimer.isReady())
    {
        disp.point(0,true);

        pointBlinkTimerEn.setTimeout(300);
    }

    if(pointBlinkTimerEn.isReady())
    {
        disp.point(0,false);
    }

    if(dispBlinkTimer.isReady())
    {
        if(dispBlinkFlag)
        {
            disp.display(lastData);
        }
        else
        {
            for(int i = 0; i < 4; i++)
            {
                if(blink[i])
                    disp.displayByte(i, 0x00);
            }
        }

        dispBlinkFlag = !dispBlinkFlag;

        dispBlinkTimer.setTimeout(500);
    }
}

void Display::pointUpdate()
{
    if(lock) return;

    if(pointType != newPointType)
    {
        if(newPointType == 0)
        {
            pointBlinkTimer.stop();
            pointBlinkTimerEn.stop();
            disp.point(0, false);
        }
        else if(newPointType == 1)
            pointBlinkTimer.setInterval(4000);
        else if(newPointType == 2)
            pointBlinkTimer.setInterval(1000);
        else if(newPointType == 3)
        {
            pointBlinkTimer.stop();
            pointBlinkTimerEn.stop();
            disp.point(0, true);
        }

        pointType = newPointType;
    }
}
