/*
 * GamepadHW.h
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#ifndef GAMEPADHW_H
#define GAMEPADHW_H

#include "Arduino.h"

#define AN_PIN_LX       34
#define AN_PIN_LY       39
#define BTN_PIN_LST     36

#define BT_DPADU   0
#define BT_DPADR   1
#define BT_DPADD   2
#define BT_DPADL   3

#define BT_BTW    4
#define BT_BTN    5
#define BT_BTS    6
#define BT_BTE    7

#define BT_LSD    8
#define BT_RSD    9
#define BT_LTR    10
#define BT_RTR    11

#define BT_LST    12
#define BT_RST    13

#define BT_SEL    14
#define BT_START  15

class GamepadHW 
{
private:
    uint16_t buttonData;   
    int AnlogStickData[6];
      
public:
    void GPIO_Init( void );
    void readPinData( void );
    uint8_t AN2Byte( int AN_Data , uint16_t ANMin, uint16_t ANMidL, uint16_t ANMidR, uint16_t ANMax);
    uint8_t ANTR2Byte(int AN_Data);
    uint16_t Buttons;
    uint8_t AN_Ctrl[6];
    uint8_t Batttery;
};

#endif // GAMEPADHW_H
