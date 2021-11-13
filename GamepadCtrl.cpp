/*
 * GamepadCtrl.cpp
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */

#include "BleGamepad.h"
#include "GamepadCtrl.h"
#include "Arduino.h"

void GamepadCtrl::sendReport(void)
{
    uint8_t reportData[GP_VDATAIN_LEN+8],i;

    reportData[0] = _buttons;
    reportData[1] = (_buttons >> 8);
    reportData[2] = _x;
    reportData[3] = _y;
    reportData[4] = _z;
    reportData[5] = _rX;
    reportData[6] = _rY;
    reportData[7] = _rZ;
    for(i=0;i<GP_VDATAIN_LEN;i++)
    {
        reportData[8+i] =  _customData[i];   
    } 
    bleGamepad.sendReport(GAMEPAD_ID,reportData,GP_VDATAIN_LEN+8);
}
void GamepadCtrl::setCtrlData(uint16_t b,uint8_t x, uint8_t y, uint8_t z, uint8_t rX, uint8_t rY, uint8_t rZ)
{
    uint8_t Dpad[4],DpadS;
    uint8_t buttontemp; 
    Dpad[0] = b&0x01;
    Dpad[1] = (b>>1)&0x01;
    Dpad[2] = (b>>2)&0x01;
    Dpad[3] = (b>>3)&0x01;

    //           1
    //        8     2
    //     7     0     3
    //        6     4
    //           5
    
    DpadS = Dpad[0] + Dpad[1]*3 + Dpad[2]*5 + Dpad[3]*7;
    if( (DpadS%2) == 0  )
    {
        if( (Dpad[0] == 1) && (DpadS == 8) ) 
            buttontemp = 8;
        else
            buttontemp = DpadS/2;
    }
    else
    {
        buttontemp = DpadS;      
    }

    _buttons = (b&0xfff0) + (buttontemp&0x0f);
    _x = x;
    _y = y;
    _z = z;
    _rX = rX; 
    _rY = rY; 
    _rZ = rZ;       
}

void GamepadCtrl::setCustomData(uint8_t* cData, uint8_t dataLen)
{
    uint8_t i;
    if(dataLen>GP_VDATAIN_LEN)
        dataLen = GP_VDATAIN_LEN;
    for(i=0;i<dataLen;i++)
    {
        _customData[i] =  cData[i];   
    } 
}
