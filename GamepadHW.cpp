/*
 * GamepadHW.cpp
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#include "GamepadHW.h"

uint16_t ANStick_Adj[4][4] =
{
    {0,1740,1940,4095},
    {0,1820,2020,4095}, 
    {700,1844,1998,3442},
    {500,1871,1936,3314}
};


void GamepadHW::GPIO_Init( void )
{
    pinMode(AN_PIN_LX,INPUT_PULLDOWN);
    pinMode(AN_PIN_LY,INPUT_PULLDOWN);
    pinMode(BTN_PIN_LST,INPUT_PULLDOWN);
}

void GamepadHW::readPinData( void )
{
    uint8_t i;
    AnlogStickData[0] = analogRead(AN_PIN_LX);
    AnlogStickData[1] = analogRead(AN_PIN_LY);

    for(i=0;i<4;i++)
    {
        AN_Ctrl[i] = AN2Byte(AnlogStickData[i],ANStick_Adj[i][0],ANStick_Adj[i][1],ANStick_Adj[i][2],ANStick_Adj[i][3]);  
    }
    
    AN_Ctrl[4] = ANTR2Byte(AnlogStickData[4]); 
    AN_Ctrl[5] = ANTR2Byte(AnlogStickData[5]); 
 
    //Serial.printf("LX:%d  LY:%d  RX:%d  RY:%d  LTR:%d  RTR:%d\r\n",
    //AnlogStickData[0],
    //AnlogStickData[1],
    //AnlogStickData[2],
    //AnlogStickData[3],    
    //AnlogStickData[4],
    //AnlogStickData[5]);
    //Serial.printf("BT:%d  \r\n",buttonData);

    buttonData = 0;
 
    buttonData |= digitalRead(BTN_PIN_LST)<<BT_LST;  
    Buttons = buttonData;                
    //Serial.printf("LX:%d  LY:%d  RX:%d  RY:%d\r\n",AN_Ctrl[0],AN_Ctrl[1],AN_Ctrl[2],AN_Ctrl[3]);   
    //delay(100);
}


uint8_t GamepadHW::AN2Byte( int AN_Data , uint16_t ANMin, uint16_t ANMidL, uint16_t ANMidR, uint16_t ANMax )
{   
    if( AN_Data > ANMax )
        AN_Data = ANMax;
    else if(AN_Data < ANMin)
        AN_Data = ANMin;

  
    if(AN_Data>ANMidR)
    {           
        return ((AN_Data - ANMidR)*127)/(ANMax-ANMidR) + 127; 
    }
    else if( AN_Data<ANMidL )
    {
        return ((AN_Data - ANMin)*127)/(ANMidL-ANMin);
    }
    else
        return 127;
}


uint8_t GamepadHW::ANTR2Byte( int AN_Data )
{   
    if(AN_Data>1870)
    {
        AN_Data = 1870;
    }
    else if(AN_Data<500)
    {
        AN_Data = 500;
    }

    return ((1870 - AN_Data)*2)/11;
}
