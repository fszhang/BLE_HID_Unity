#ifndef LED_CTRL_H__
#define LED_CTRL_H__

#include "Arduino.h"
#define LTURNLIGHT      0,24
#define RTURNLIGHT      25,1
#define REARLIGHT       0,16
#define FRONTLIGHT      17,8
class LED_Ctrl 
{
    
private:
    char readChar;
public:
    LED_Ctrl();
    void Init();
    //void SetLight();
    void SetLightData(uint8_t colorR, uint8_t colorG, uint8_t colorB);
    void SetLED(bool LEDOn);
    void SetDirection(bool direction);
};

#endif // LED_CTRL_H
