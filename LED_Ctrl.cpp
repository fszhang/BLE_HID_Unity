#include "LED_Ctrl.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <Ticker.h>

#define LEDS_COUNT      24
#define WS2812_PIN      26
#define CHANNEL         1


void taskLightCircle();

//Ticker tickerCircle;

uint8_t circleLedIndex;
bool circleDirection;

volatile uint8_t lightData[LEDS_COUNT][3];
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, WS2812_PIN, CHANNEL, TYPE_GRB);
int delayval = 10;
void SetLight(uint8_t ledIndex);

LED_Ctrl::LED_Ctrl() 
{

}

void LED_Ctrl::Init() 
{      

    strip.begin();
    strip.setBrightness(100);
    //SetLight(LTURNLIGHT,1);
    //tickerCircle.attach(0.1,taskLightCircle);  

    for(byte i=0;i<LEDS_COUNT;i++)
    {
        lightData[i][0] = 100;
        lightData[i][1] = 100;
        lightData[i][2] = 100;

    }  
    SetLight(0);
    //tickerCircle.detach
}

void LED_Ctrl::SetDirection(bool direction)
{

    circleDirection = direction;

} 

void LED_Ctrl::SetLightData(uint8_t colorR, uint8_t colorG, uint8_t colorB)
{

    for(byte i=0;i<LEDS_COUNT;i++)
    {
        lightData[i][0] = colorR;
        lightData[i][1] = colorG;
        lightData[i][2] = colorB;
    }  
    SetLight(0);

} 


void SetLight(uint8_t ledIndex) 
{
 
    for (int i = ledIndex,j=0; j < LEDS_COUNT; j++) 
    {
        if(circleDirection)
        {
            i++;
            if(i==LEDS_COUNT)
                i=0;
        }
        else
        {
            if(i>0)
                i--;
            else
                i = LEDS_COUNT - 1;
        }

        strip.setLedColorData(j, lightData[i][0], lightData[i][1], lightData[i][2]); 
    } 
    
    strip.show();
    delay(5);
}

void taskLightCircle()
{
    if(circleLedIndex<(LEDS_COUNT-1))
        circleLedIndex++;
    else
        circleLedIndex = 0;

    SetLight(circleLedIndex);
}