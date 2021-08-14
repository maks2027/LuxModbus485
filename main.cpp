#define CONTROL_TYPE 1 // 0 кнопки, 1 энкодер
#define RTC_TYPE 2 // 1 DS1307, 2 DS3231
#define ONE_START 39  // первый запуск

#define PIN_DISP_CLK 2//дисплей
#define PIN_DISP_DIO 3//дисплей
#define PIN_BUT_UP 4 //кнопка или энкодер
#define PIN_BUT_DOWN 5 //кнопка или энкодер
#define PIN_BUT_SW 6 //средняя кнопка энкодера
#define PIN_485_EN 7 //пин для переключения передачи
#define PIN_SEND_EN 8 //отключение передачи

//SDA A4
//SCL A5

#define STAB_LUX 2 // стабильность освещения, на сколько пунктов может изменится свет в течении 300 мс
#define TIME_STAB_LUX 30000 //через сколько считать свет стабильным
#define NORM_LUX 0 // сколько + - считать за норму
#define NORM_LUX_2 1 // сколько + - считать за норму после выхода на режим
#define TIME_NORM_LUX 30000 // через сколько считать что вышел на уставку
#define ID_BUS 34 //адрес утройства modbus

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>


#include "RTClib.h"
#include "GyverTM1637.h"
#include "GyverWDT.h"
#include "GyverTimer.h"


#include "GyverButton.h"

#include "rtctimers.h"
#include "luxreader.h"
#include "DimControl.h"
#include "display.h"
#include "menu.h"

//#define DEBUG_ENABLE


#if (CONTROL_TYPE == 1)
#include "GyverEncoder.h"
Encoder enc1(PIN_BUT_UP, PIN_BUT_DOWN);
#else
GButton butUp(PIN_BUT_UP);
GButton butDown(PIN_BUT_DOWN);
#endif
GButton butEdit(PIN_BUT_SW);

int8_t hour = 0;
int8_t minute = 0;

#ifdef DEBUG_ENABLE
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
#endif

Menu menu;

bool rtcWork = true;
#if (RTC_TYPE == 1)
RTC_DS1307 rtc;
#endif

#if (RTC_TYPE == 2)
RTC_DS3231 rtc;
#endif

Display disp(PIN_DISP_CLK, PIN_DISP_DIO);
DimControl dim(PIN_485_EN, ID_BUS, NORM_LUX, NORM_LUX_2, STAB_LUX, TIME_NORM_LUX, TIME_STAB_LUX);
LuxReader lux;
RTCTimers timers(2);
GTimer timeTimer;
GTimer lvlTimer;
//*********************************************************************************************

#ifdef DEBUG_ENABLE
extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
    int freeValue;
    if ((int)__brkval == 0)
        freeValue = ((int)&freeValue) - ((int)&__bss_end);
    else
        freeValue = ((int)&freeValue) - ((int)__brkval);
    return freeValue;
}
#endif

//*********************************************************************************************
void setup()
{
    Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024);
    Serial.begin(9600);
#ifdef DEBUG_ENABLE
    mySerial.begin(9600);
#endif
    Wire.begin();

    pinMode(13, OUTPUT);
    pinMode(PIN_DISP_CLK, OUTPUT);
    pinMode(PIN_DISP_DIO, OUTPUT);
    pinMode(PIN_BUT_UP, INPUT_PULLUP);
    pinMode(PIN_BUT_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUT_SW, INPUT_PULLUP);
    pinMode(PIN_485_EN, OUTPUT);
    pinMode(PIN_SEND_EN, INPUT_PULLUP);


#if (CONTROL_TYPE == 1)
    enc1.setType(TYPE2);
    butEdit.setType(HIGH_PULL);
#else
    butUp.setType(LOW_PULL);
    butDown.setType(LOW_PULL);
    butEdit.setType(LOW_PULL);
    butUp.setDebounce(0);
    butDown.setDebounce(0);
    butEdit.setDebounce(0);
#endif

    bool fristStart = false;

    if(EEPROM.read(1) != ONE_START)///проверить!!!!!!!!!!!
    {
        EEPROM.write(1,ONE_START);
        fristStart = true;
    }

    if (!rtc.begin())
    {
        rtcWork = false;
    }

    DateTime time = rtc.now();
    hour = time.hour();
    minute = time.minute();


    menu.begin(4,2,8,8,5);

    //**************************************************** lvl 0

    menu.setEdit1ActAll(JUMP);
    menu.setEdit2ActAll(LVLUP);
    menu.setJampIndUpAll(0);

    menu.setActInput([](){
        dim.setHand();
        menu.setRange(0,0,100);
        menu.setValue(0, dim.getDimPercent());
        disp.displayVal(_P, menu.getValue(0));
        menu.setCircle(0,false);
    });

    menu.setUpdate([](){ disp.displayVal(_P,menu.getValue(0)); dim.setDimPercent(menu.getValue(0)); });
    menu.setUpDownAct(EDIT);
    menu.indUpSetup();


    menu.setActInput([](){ dim.setAuto(); disp.displayVal(_A,lux.getLux()); });
    menu.setUpdate([](){ disp.displayVal(_A,lux.getLux()); });

    //**************************************************** lvl 1
    menu.lvlUpSetup();
    menu.setUpDownActAll(JUMP);
    menu.setEdit1ActAll(LVLUP);
    menu.setEdit2ActAll(LVLDOWN);
    menu.setJampIndSoUpAll();

    menu.setActInput([](){ disp.displayVal(_o,_n,_empty,_empty); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_L,_empty,_empty,_empty); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_t,_empty,_empty,_empty); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_t,_1,_o,_n); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_t,_1,_o,_f); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_t,_2,_o,_n); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_t,_2,_o,_f); lvlTimer.start(); });
    menu.indUpSetup();

    menu.setActInput([](){ disp.displayVal(_C,_O,_L,_empty); lvlTimer.start(); });


    //**************************************************** lvl 2
    menu.lvlUpSetup();
    menu.setUpDownActAll(EDIT);
    menu.setJampIndSoUpAll(-2);

    //*************************************** on
    menu.setActInput([](){ menu.setRange(0,0,1); menu.setValue(0,timers.getEnable()); menu.setCircle(0,true); disp.displayVal(menu.getValue(0)); lvlTimer.start(); });
    menu.setUpdate([](){  disp.displayVal(menu.getValue(0)); lvlTimer.start(); });
    menu.setActOutput([](){ timers.setEnable(menu.getValue(0)); });
    menu.setEdit1Act(LVLDOWN);
    menu.indUpSetup();

    //*************************************** L

    menu.setActInput([](){ disp.displayVal(dim.getToLux()); menu.setRange(0,0,80); menu.setValue(0,dim.getToLux()); menu.setCircle(0,false); lvlTimer.start(); });
    menu.setUpdate([](){ disp.displayVal(menu.getValue(0)); lvlTimer.start(); });
    menu.setActOutput([](){ dim.setToLux(menu.getValue(0)); dim.resetError(); disp.pointUnLock(); });
    menu.setEdit1Act(LVLDOWN);
    menu.indUpSetup();

    //*************************************** t
    menu.setActInput([](){
        menu.setRange(0,0,23);
        menu.setRange(1,0,59);
        menu.setValue(0,hour);
        menu.setValue(1,minute);
        menu.setCircle(0,true);
        menu.setCircle(1,true);
        disp.displayTwoVal(menu.getValue(0),menu.getValue(1));
        disp.displayBlinkOn(true, true, false, false);
        lvlTimer.start();
    });
    menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0),menu.getValue(1)); lvlTimer.start(); });
    menu.setActOutput([](){ disp.displayBlinkOff(); });
    menu.setEdit1Act(LVLUP);
    menu.indUpSetup();

    //*************************************** t11
    menu.setActInput([](){
        disp.displayTwoVal(timers.getHourStart(0),timers.getMinuteStart(0));
        menu.setRange(0,0,23);
        menu.setRange(1,0,59);
        menu.setValue(0,timers.getHourStart(0));
        menu.setValue(1,timers.getMinuteStart(0));
        menu.setCircle(0,true);
        disp.displayBlinkOn(true, true, false, false);
        lvlTimer.start();
    });

    menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0),menu.getValue(1)); lvlTimer.start(); });
    menu.setActOutput([](){ disp.displayBlinkOff(); });
    menu.setEdit1Act(LVLUP);
    menu.indUpSetup();

    //*************************************** t12
    menu.setActInput([](){
        disp.displayTwoVal(timers.getHourEnd(0),timers.getMinuteEnd(0));
        menu.setRange(0,0,23);
        menu.setRange(1,0,59);
        menu.setValue(0,timers.getHourEnd(0));
        menu.setValue(1,timers.getMinuteEnd(0));
        menu.setCircle(0,true);
        disp.displayBlinkOn(true, true, false, false);
        lvlTimer.start();
    });

    menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0),menu.getValue(1));  lvlTimer.start(); });
    menu.setActOutput([](){ disp.displayBlinkOff(); });
    menu.setEdit1Act(LVLUP);
    menu.indUpSetup();


    //*************************************** t21
    menu.setActInput([](){
        disp.displayTwoVal(timers.getHourStart(1),timers.getMinuteStart(1));
        menu.setRange(0,0,23);
        menu.setRange(1,0,59);
        menu.setValue(0,timers.getHourStart(1));
        menu.setValue(1,timers.getMinuteStart(1));
        menu.setCircle(0,true);
        disp.displayBlinkOn(true, true, false, false);
        lvlTimer.start();
    });

    menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0),menu.getValue(1));  lvlTimer.start(); });
    menu.setActOutput([](){ disp.displayBlinkOff(); });
    menu.setEdit1Act(LVLUP);
    menu.indUpSetup();

    //*************************************** t22
    menu.setActInput([](){
        disp.displayTwoVal(timers.getHourEnd(1),timers.getMinuteEnd(1));
        menu.setRange(0,0,23);
        menu.setRange(1,0,59);
        menu.setValue(0,timers.getHourEnd(1));
        menu.setValue(1,timers.getMinuteEnd(1));
        menu.setCircle(0,true);
        disp.displayBlinkOn(true, true, false, false);
        lvlTimer.start();
    });

    menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0),menu.getValue(1));  lvlTimer.start(); });
    menu.setActOutput([](){ disp.displayBlinkOff(); });
    menu.setEdit1Act(LVLUP);
    menu.indUpSetup();

    //*************************************** C
    menu.setActInput([](){
        menu.setRange(0,-25,25);
        menu.setValue(0,lux.getCalib());
        disp.displayVal(lux.getCalib());
        menu.setCircle(0,false);
        lvlTimer.start();
    });

    menu.setUpdate([](){ disp.displayVal(menu.getValue(0)); lvlTimer.start(); });
    menu.setActOutput([](){ lux.setCalib(menu.getValue(0)); });
    menu.setEdit1Act(LVLDOWN);


    //**************************************************** lvl 3
    menu.lvlUpSetup();
    menu.setEdit1ActAll(LVLDOWN2);
    menu.setUpDownActAll(EDIT);

    //**************************************
    menu.setActInput([](){
        disp.displayTwoVal(menu.getValue(0),menu.getValue(1));
        menu.setIndValue(1);
        menu.setCircle(0,true);
        disp.displayBlinkOn(false, false, true, true);
        lvlTimer.start();
    });
    menu.setUpdate([](){ disp.displayTwoVal(menu.getValue(0), menu.getValue(1)); lvlTimer.start(); });
    menu.setActOutput([](){ rtc.adjust( DateTime(2021, 3, 28, menu.getValue(0), menu.getValue(1), 0)); disp.displayBlinkOff(); });
    menu.indUpSetup();

    //*************************************** t11 - t22
    for(int i = 0; i < 4; i++)
    {
        menu.setActInput([](){
            disp.displayTwoVal(menu.getValue(0),menu.getValue(1));
            menu.setIndValue(1);
            menu.setCircle(1,true);
            disp.displayBlinkOn(false, false, true, true);
            lvlTimer.start();
        });

        menu.setUpdate([](){  disp.displayTwoVal(menu.getValue(0), menu.getValue(1)); lvlTimer.start(); });
        menu.indUpSetup();
    }

    menu.indUpSetup(1);

    menu.setActOutput([](){ timers.setTimerStart(0,menu.getValue(0),menu.getValue(1)); disp.displayBlinkOff(); });
    menu.indUpSetup();

    menu.setActOutput([](){ timers.setTimerEnd(0,menu.getValue(0),menu.getValue(1)); disp.displayBlinkOff(); });
    menu.indUpSetup();

    menu.setActOutput([](){ timers.setTimerStart(1,menu.getValue(0),menu.getValue(1)); disp.displayBlinkOff(); });
    menu.indUpSetup();

    menu.setActOutput([](){ timers.setTimerEnd(1,menu.getValue(0),menu.getValue(1)); disp.displayBlinkOff(); });
    menu.indUpSetup();


    timeTimer.setInterval(1000);
    lvlTimer.setTimeout(30000);
    lvlTimer.stop();


    dim.begin(10,fristStart);
    lux.begin(dim.endBute(),fristStart);
    timers.begin(lux.endBute(),fristStart);

#ifdef DEBUG_ENABLE
    disp.displayVal(_U, _1, _0, _d);///версия
#else
    disp.displayVal(_U, _1, _0, _r);///версия
#endif
    delay(1000);


    if(lux.isError())//ошибка чтения света
    {
        disp.displayVal(_E, _r, _r, _1);
        dim.setHand();
        delay(3000);
    }

    if(dim.isError())//ошибка чтения памяти
    {
        disp.displayVal(_E, _r, _r, _2);
        disp.pointBlink_025();
        disp.pointLock();
        delay(3000);
    }

    if(!rtcWork)//ошибка времени
    {
        timers.setTimeError(true);
        disp.displayVal(_E, _r, _r, _3);
        delay(3000);
    }

    if(dim.getHand())
        menu.start(0,0);
    else
        menu.start(0,1);
}

void loop()
{
    dim.setNoSend(!digitalRead(PIN_SEND_EN));

    if(timeTimer.isReady() && rtcWork)
    {
        DateTime time = rtc.now();

        if(time.hour() > 23 || time.hour() < 0)
            rtcWork = false;

        if(time.minute() > 59 || time.minute() < 0)
            rtcWork = false;

        if(rtcWork)
        {
            hour = time.hour();
            minute = time.minute();
        }
        else
        {
            hour = 0;
            minute = 0;
            timers.setTimeError(true);
        }
    }

    if(lux.isLux())
        menu.update(0,1);

    timers.setTime(hour,minute);
    timers.work();

    if(timers.isOn())
        dim.setOn();

    if(timers.isOff())
        dim.setOff();


    dim.work(lux.getLux());
    disp.work();

    if(lvlTimer.isReady())
    {
        menu.setLvl(0);
    }

    if(dim.getAuto() && dim.getOn())
    {
        if(dim.isRange())
            disp.pointOn();
        else
            disp.pointBlink_1();
    }
    else
    {
        disp.pointOff();
    }


#if (CONTROL_TYPE == 1)
    enc1.tick();

    if (enc1.isRight())
    {
        menu.btUp();
    }

    if (enc1.isLeft())
    {
        menu.btDown();
    }

#else
    butUp.tick();
    butDown.tick();

    if (butUp.isClick())
    {
        menu.btUp();
    }

    if (butDown.isClick())
    {
        menu.btDown();
    }
#endif

    butEdit.tick();

    if (butEdit.isSingle())
    {
        menu.btEdit1();
    }

#ifdef DEBUG_ENABLE
    if (butEdit.isDouble())
    {
        mySerial.print("RAM: ");
        mySerial.println(memoryFree());
    }
#endif

    if (butEdit.isHolded())
    {
        menu.btEdit2();
    }

    Watchdog.reset();
}
