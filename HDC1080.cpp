/*
 * HDC1080.cpp
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#include <Arduino.h>
#include "HDC1080.h"
#include <Wire.h>

HDC1080_I2C::HDC1080_I2C(byte chipAddress,TwoWire *TwoWireInstance ) 
{
    _chipAddress = (uint8_t) chipAddress;
    _TwoWireInstance = TwoWireInstance;
} 

void HDC1080_I2C::WriteRegister( byte reg, byte value ) 
{
    _TwoWireInstance->beginTransmission(_chipAddress);
    _TwoWireInstance->write(reg);
    _TwoWireInstance->write(value);
    _TwoWireInstance->endTransmission();
}

int8_t HDC1080_I2C::Init( void ) 
{
    byte Data[2];
    ReadRegister(0xFE, 2, Data);
    if( (Data[0]!= 0x54) || (Data[1]!= 0x49) )
        return -1;
    ReadRegister(0xFF, 2, Data);
    if( (Data[0]!= 0x10) || (Data[1]!= 0x50) )
        return -1;  
    Data[0] = 0x16;
    Data[1] = 0;
    WriteRegister(0x02,2,Data);
    return 1;          
}

void HDC1080_I2C::ConvStart(void)
{
    _TwoWireInstance->beginTransmission(_chipAddress);
    _TwoWireInstance->write(0);
    _TwoWireInstance->endTransmission();
}

void HDC1080_I2C::HeatEnable(void)
{
    byte Data[2]; 
    Data[0] = 0x36;
    Data[1] = 0;
    WriteRegister(0x02,2,Data);
}

void HDC1080_I2C::HeatDisable(void)
{
    byte Data[2]; 
    Data[0] = 0x16;
    Data[1] = 0;
    WriteRegister(0x02,2,Data);
}

void HDC1080_I2C::ReadTH(int16_t* Temp, uint8_t* Humidity)
{ 
    uint8_t THData[4];
    ConvStart();
    delay(10);
    ReadRegister(4, THData);
    *Temp = (((int32_t)THData[0]*256 + THData[1])*10)/397 - 400;
    *Humidity = ((uint16_t)THData[2]*25)/64;
}

void HDC1080_I2C::WriteRegister( byte reg, byte count, byte *values ) 
{
    if (count == 0) 
        return;
    uint8_t regist = (uint8_t) reg;
    _TwoWireInstance->beginTransmission(_chipAddress);
    _TwoWireInstance->write(regist);
    for (byte index = 0; index < count; index++) 
    {
        _TwoWireInstance->write(values[index]);
    }
    _TwoWireInstance->endTransmission();
}


byte HDC1080_I2C::ReadRegister( byte reg ) 
{
    byte value;
    uint8_t _size = 1;
    uint8_t regist;
    regist = (uint8_t) reg;
    _TwoWireInstance->beginTransmission(_chipAddress);
    _TwoWireInstance->write(regist);
    _TwoWireInstance->endTransmission();
    _TwoWireInstance->requestFrom(_chipAddress, _size);
    value = _TwoWireInstance->read();
    return value;
}

void HDC1080_I2C::ReadRegister( byte count, byte *values ) 
{
    if (count == 0) 
        return;
    uint8_t _count = (uint8_t) count;
    byte index = 0;                    
    _TwoWireInstance->beginTransmission(_chipAddress);  
    _TwoWireInstance->requestFrom(_chipAddress, _count);
    while (_TwoWireInstance->available()) 
    {
        values[index++] = _TwoWireInstance->read();
    }
}

void HDC1080_I2C::ReadRegister( byte reg,  byte count, byte *values ) 
{
    if (count == 0) 
        return;
    uint8_t _count = (uint8_t) count;
    uint8_t regist = (uint8_t) reg;
    byte index = 0;                    
    _TwoWireInstance->beginTransmission(_chipAddress);
    _TwoWireInstance->write(regist);
    _TwoWireInstance->endTransmission();
    _TwoWireInstance->requestFrom(_chipAddress, _count);
    while (_TwoWireInstance->available()) 
    {
        values[index++] = _TwoWireInstance->read();
    }
}
