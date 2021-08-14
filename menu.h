/*
 *  Уневерсальное меню
 *  v 0.01
 * (Незаконченно, прототип)
*/

#ifndef MENU_H
#define MENU_H



//#define DEBUG_ENABLE

#ifndef DEBUG_ENABLE
#define DEBUG(x,y)
#endif

#ifdef DEBUG_ENABLE
//#include <HardwareSerial.h>
#include <SoftwareSerial.h>
extern SoftwareSerial mySerial;
#define DEBUG(x,y) mySerial.print(F("[MENU] "));mySerial.print(x);mySerial.println(y)
#endif

#include <inttypes.h>
#include <stdarg.h>

enum Acts: uint8_t
{
    NO,
    JUMP,
    EDIT,
    LVLUP,
    LVLDOWN,
    LVLDOWN2
};

struct item
{
    item()
    {
        fInput = nullptr;
        fOutput = nullptr;
        fUpdate = nullptr;
        upDownAct = NO;
        edit1Act = NO;
        edit2Act = NO;
        jampIndUp = -1;
        jampIndDown = -1;
    }

    void (*fInput)();
    void (*fOutput)();
    void (*fUpdate)();

    Acts upDownAct;
    Acts edit1Act;
    Acts edit2Act;

    int8_t jampIndUp;
    int8_t jampIndDown;

};

class Menu
{
public:
    Menu();

    void begin(uint8_t n, ...);

    void start(uint8_t newLvl = -1, int8_t _ind = -1);

    void update();
    void update(uint8_t _lvl, uint8_t _ind);

    void btEdit1();
    void btEdit2();
    void btUp();
    void btDown();

    void setIndValue(uint8_t i);
    void setRange(uint8_t ind,int16_t _min,int16_t _max);
    void setCircle(uint8_t ind,bool val);
    void setValue(uint8_t ind,int16_t val);
    int16_t getValue(uint8_t ind);

    void lvlUpSetup();
    void indUpSetup();
    void indUpSetup(uint8_t ind);
    uint8_t getLvlSetup();
    uint8_t getIndSetup();

    void setActInput(void(*fn)());
    void setActOutput(void(*fn)());
    void setUpdate(void(*fn)());

    void setActInputAllLvl(void(*fn)());
    void setActOutputAllLvl(void(*fn)());
    void setUpdateAllLvl(void(*fn)());
    
    void setUpDownAct(Acts a);
    void setUpDownActAll(Acts a);
    
    void setEdit1Act(Acts a);
    void setEdit2Act(Acts a);
    
    void setEdit1ActAll(Acts a);
    void setEdit2ActAll(Acts a);

    void setJampIndUp(uint8_t upInd);
    void setJampIndUpAll(uint8_t upInd);
    void setJampIndSoUp(int8_t shift = 0);
    void setJampIndSoUpAll(int8_t shift = 0);

    void setJampIndDown(uint8_t downInd);
    void setJampIndDownAll(uint8_t downInd);
    void setJampIndSoDown(int8_t shift = 0);
    void setJampIndSoDownAll(int8_t shift = 0);

    void setLvl(uint8_t newLvl, int8_t _ind = -1);

private:
    //временно (кастыль)
    uint8_t indValue = 0;
    int16_t tempVal[2];
    int16_t min[2];
    int16_t max[2];
    bool circle[2];
    //

    uint8_t size;
    uint8_t *lineSize;
    uint8_t *lineSm;

    item *items;

    uint8_t lvl = 0;
    uint8_t *ind;

    uint8_t setuplvl = 0;
    uint8_t setupInd = 0;

    uint8_t getIndex(uint8_t _lvl, uint8_t _ind);

    void actInput();
    void actOutput();
    void indUp();
    void indDown();
    void lvlUp();
    void lvlDown();
};

#endif // MENU_H
