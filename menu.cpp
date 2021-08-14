#include "menu.h"

Menu::Menu()
{    

}

void Menu::begin(uint8_t n, ...)
{
    uint8_t sum = 0;

    size = n;
    lineSize = new uint8_t[n];
    lineSm = new uint8_t[n];
    ind = new uint8_t[n];

    va_list factor;         //указатель va_list
    va_start(factor, n);    // устанавливаем указатель

    for (int i = 0; i < n; i++)
    {
        uint8_t temp = va_arg(factor, int);  // получаем значение текущего параметра типа int

        ind[i] = 0;

        lineSize[i] = temp;

        DEBUG("lineSize: ", temp);

        lineSm[i] = sum;

        DEBUG("lineSm: ", sum);

        sum += temp;
    }

    va_end(factor); // завершаем обработку параметров

    DEBUG("sum: ", sum);

    items = new item[sum];
}

void Menu::start(uint8_t newLvl, int8_t _ind)
{
    if(newLvl < 0 || newLvl >= size)
    {
       actInput();
       return;
    }

    lvl = newLvl;

    if( _ind >=  0 && _ind <  lineSize[newLvl])
        ind[newLvl] = _ind;

    actInput();
}

void Menu::update()
{
    if(items[getIndex(lvl, ind[lvl])].fUpdate != nullptr)
        items[getIndex(lvl, ind[lvl])].fUpdate();

    DEBUG("update","");
}

void Menu::update(uint8_t _lvl, uint8_t _ind)
{
    if(_lvl == lvl && _ind == ind[lvl])
        if(items[getIndex(lvl, ind[lvl])].fUpdate != nullptr)
        {
            items[getIndex(lvl, ind[lvl])].fUpdate();
            DEBUG("update","");
        }
}

void Menu::btEdit1()
{
    DEBUG("btEdit1 ", (uint8_t)items[getIndex(lvl, ind[lvl])].edit1Act);

    if(items[getIndex(lvl, ind[lvl])].edit1Act == LVLUP)
    {
        actOutput();
        lvlUp();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit1Act == LVLDOWN)
    {
        actOutput();
        lvlDown();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit1Act == LVLDOWN2)
    {
        actOutput();
        lvlDown();
        lvlDown();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit1Act == JUMP)
    {
        actOutput();
        indUp();
        actInput();
    }
}

void Menu::btEdit2()
{
    DEBUG("btEdit2 ",(uint8_t)items[getIndex(lvl, ind[lvl])].edit2Act);

    if(items[getIndex(lvl, ind[lvl])].edit2Act == LVLUP)
    {
        actOutput();
        lvlUp();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit2Act == LVLDOWN)
    {
        actOutput();
        lvlDown();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit1Act == LVLDOWN2)
    {
        actOutput();
        lvlDown();
        lvlDown();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].edit2Act == JUMP)
    {
        actOutput();
        indUp();
        actInput();
    }
}

void Menu::btUp()
{
    DEBUG("btUp ",(uint8_t)items[getIndex(lvl, ind[lvl])].upDownAct);

    if(items[getIndex(lvl, ind[lvl])].upDownAct == JUMP)
    {
        actOutput();
        indUp();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].upDownAct == EDIT)
    {
        tempVal[indValue]++;
        if(tempVal[indValue] > max[indValue])
        {
            if(circle[indValue])
                tempVal[indValue] = min[indValue];
            else
                tempVal[indValue] = max[indValue];
        }

        update();
    }
}

void Menu::btDown()
{
    DEBUG("btDown ",(uint8_t)items[getIndex(lvl, ind[lvl])].upDownAct);

    if(items[getIndex(lvl, ind[lvl])].upDownAct == JUMP)
    {
        actOutput();
        indDown();
        actInput();
    }
    else if(items[getIndex(lvl, ind[lvl])].upDownAct == EDIT)
    {
        tempVal[indValue]--;
        if(tempVal[indValue] < min[indValue])
        {
            if(circle[indValue])
                tempVal[indValue] = max[indValue];
            else
                tempVal[indValue] = min[indValue];
        }

        update();
    }
}

void Menu::setIndValue(uint8_t i)
{
    indValue = i;
}

void Menu::setRange(uint8_t ind, int16_t _min, int16_t _max)
{
    min[ind] = _min;
    max[ind] = _max;
}

void Menu::setCircle(uint8_t ind, bool val)
{
    circle[ind] = val;
}

void Menu::setValue(uint8_t ind, int16_t val)
{
    if(val > max[ind])
        tempVal[ind] = max[ind];
    else if(val < min[ind])
        tempVal[ind] = min[ind];
    else
        tempVal[ind] = val;


}

int16_t Menu::getValue(uint8_t ind)
{
    return tempVal[ind];
}

void Menu::lvlUpSetup()
{
    setuplvl++;
    setupInd = 0;
    DEBUG("NewLvlSetup ",setuplvl);
}

void Menu::indUpSetup()
{
    setupInd++;
    DEBUG("NewIndSetup ",setupInd);
}

void Menu::indUpSetup(uint8_t ind)
{
    setupInd = ind;
    DEBUG("NewIndSetup ",setupInd);
}

uint8_t Menu::getLvlSetup()
{
    return setuplvl;
}

uint8_t Menu::getIndSetup()
{
    return setupInd;
}

void Menu::setActInput(void (*fn)())
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setActInput: ","");

    items[getIndex(setuplvl, setupInd)].fInput = fn;
}

void Menu::setActOutput(void (*fn)())
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setActOutput: ","");

    items[getIndex(setuplvl, setupInd)].fOutput = fn;
}

void Menu::setUpdate(void (*fn)())
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setUpdate: ","");

    items[getIndex(setuplvl, setupInd)].fUpdate = fn;
}

void Menu::setActInputAllLvl(void (*fn)())
{
    for(int i = 0;i < lineSize[setuplvl]; i++)
        items[getIndex(setuplvl, i)].fInput = fn;
}

void Menu::setActOutputAllLvl(void (*fn)())
{
    for(int i = 0;i < lineSize[setuplvl]; i++)
        items[getIndex(setuplvl, i)].fOutput = fn;
}

void Menu::setUpdateAllLvl(void (*fn)())
{
    for(int i = 0;i < lineSize[setuplvl]; i++)
        items[getIndex(setuplvl, i)].fUpdate = fn;
}

void Menu::setUpDownAct(Acts a)
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setUpDownAct: ",(uint8_t)a);

    items[getIndex(setuplvl, setupInd)].upDownAct = a;
}
void Menu::setUpDownActAll(Acts a)
{
    if(setuplvl >= size) return;

    DEBUG("setUpDownActLvl: ",(uint8_t)a);

    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].upDownAct = a;
    }
}

void Menu::setEdit1Act(Acts a)
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setEdit1Act: ",(uint8_t)a);

    items[getIndex(setuplvl, setupInd)].edit1Act = a;
}

void Menu::setEdit2Act(Acts a)
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setEdit2Act: ",(uint8_t)a);

    items[getIndex(setuplvl, setupInd)].edit2Act = a;
}

void Menu::setEdit1ActAll(Acts a)
{
    if(setuplvl >= size) return;

    DEBUG("setEdit1ActLvl ",(uint8_t)a);

    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].edit1Act = a;
    }
}

void Menu::setEdit2ActAll(Acts a)
{
    if(setuplvl >= size) return;

    DEBUG("setEdit2ActLvl ",(uint8_t)a);

    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].edit2Act = a;
    }
}

void Menu::setJampIndUp(uint8_t upInd)
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setJampIndUp: ", upInd);

    items[getIndex(setuplvl, setupInd)].jampIndUp = upInd;
}

void Menu::setJampIndUpAll(uint8_t upInd)
{
    if(setuplvl >= size) return;

    DEBUG("setJampIndUpLvl: ", upInd);

    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].jampIndUp = upInd;
    }
}

void Menu::setJampIndSoUp(int8_t shift)
{
    setJampIndUp(setupInd + shift);
}

void Menu::setJampIndSoUpAll(int8_t shift)
{
    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].jampIndUp = i + shift;
    }
}

void Menu::setJampIndDown(uint8_t downInd)
{
    if(setuplvl >= size) return;
    if(setupInd >=  lineSize[setuplvl]) return;

    DEBUG("setJampIndDown: ", downInd);

    items[getIndex(setuplvl, setupInd)].jampIndUp = downInd;
}

void Menu::setJampIndDownAll(uint8_t downInd)
{
    if(setuplvl >= size) return;

    DEBUG("setJampIndDownLvl: ", downInd);

    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].jampIndUp = downInd;
    }
}

void Menu::setJampIndSoDown(int8_t shift)
{
    setJampIndDown(setupInd + shift);
}

void Menu::setJampIndSoDownAll(int8_t shift)
{
    for(int i = 0;i < lineSize[setuplvl]; i++)
    {
        items[getIndex(setuplvl, i)].jampIndDown = i + shift;
    }
}

void Menu::setLvl(uint8_t newLvl, int8_t _ind)
{
    if(newLvl < 0 || newLvl >= size) return;


    actOutput();

    lvl = newLvl;

    if( _ind >=  0 && _ind <  lineSize[newLvl])
        ind[newLvl] = _ind;

    actInput();
}

uint8_t Menu::getIndex(uint8_t _lvl, uint8_t _ind)
{
    return lineSm[_lvl] + _ind;
}

void Menu::actInput()
{
    if(items[getIndex(lvl, ind[lvl])].fInput != nullptr)
        items[getIndex(lvl, ind[lvl])].fInput();

    DEBUG("actInput","");
}

void Menu::actOutput()
{
    if(items[getIndex(lvl, ind[lvl])].fOutput != nullptr)
        items[getIndex(lvl, ind[lvl])].fOutput();

    DEBUG("actOutput","");
}

void Menu::indUp()
{
    if(ind[lvl] == lineSize[lvl] - 1)
    {
        ind[lvl] = 0;
    }
    else
        ind[lvl]++;

    DEBUG("indUp: ",ind[lvl]);

    indValue = 0;
}

void Menu::indDown()
{
    if(ind[lvl] == 0)
    {
        ind[lvl] = lineSize[lvl] - 1;
    }
    else
        ind[lvl]--;

    DEBUG("indDown: ",ind[lvl]);

    indValue = 0;
}

void Menu::lvlUp()
{
    if(lvl == size - 1) return;

    int8_t temp = items[getIndex(lvl, ind[lvl])].jampIndUp;

    indValue = 0;

    lvl++;

    if(temp >= 0 && temp < lineSize[lvl])
        ind[lvl] = temp;

    DEBUG("lvlUp: ", lvl);
}

void Menu::lvlDown()
{
    if(lvl == 0) return;

    int8_t temp = items[getIndex(lvl, ind[lvl])].jampIndDown;

    indValue = 0;

    lvl--;

    if(temp >= 0 && temp < lineSize[lvl])
        ind[lvl] = temp;

    DEBUG("lvlDown: ", lvl);
}
