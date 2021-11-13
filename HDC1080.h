/*
 * HDC1080.h
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#ifndef HDC1080_I2C_h
#define HDC1080_I2C_h

#include <Arduino.h>
#include <Wire.h>

class HDC1080_I2C {
public:

    HDC1080_I2C(byte chipAddress, TwoWire *TwoWireInstance = &Wire);
    void WriteRegister(byte reg, byte value);
    void WriteRegister(byte reg, byte count, byte *values);
    byte ReadRegister(byte reg);
    void ReadRegister(byte reg, byte count, byte *values);
    void ReadRegister( byte count, byte *values );
    int8_t Init( void ); 
    void ReadTH(int16_t* Temp, uint8_t* Humidity);
    void ConvStart(void);
    void HeatEnable(void);
    void HeatDisable(void);

    
private:
    uint16_t _chipAddress;
    TwoWire *_TwoWireInstance = NULL;   // TwoWire Instance
    byte MIFARE_TwoStepHelper(byte command, byte blockAddr, long data);
};

#endif
