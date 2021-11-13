#include "LED_Ctrl.h"
#include "Motor_Ctrl.h"
#include <Ticker.h>
#include "BleGamepad.h"
#include "GamepadCtrl.h"
#include "GamepadHW.h"
#include <Wire.h>
#include "HDC1080.h"

GamepadCtrl gamepadCtrl;    //Gamepad control 
GamepadHW gamepadHW;        //Gamepad GPIO
LED_Ctrl led_Ctrl;
Motor_Ctrl motor_Ctrl;
int16_t speed;
uint8_t speedCtrlStick=1;
uint8_t motorbreak=0;

HDC1080_I2C hdc1080(0x40);  //TH Sensor 
int16_t temperature;
uint8_t humidity;

uint8_t powerUp = 0;
uint16_t powerUpIndex;
uint8_t customData[10];

void setup() 
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    motor_Ctrl.Init();
    led_Ctrl.Init();       //LED ring
    Wire.begin();          //Initialize I2C
    hdc1080.Init();
    bleGamepad.begin();
    gamepadHW.GPIO_Init();
}
void loop() 
{
    gamepadHW.readPinData();           //Get the gamepad PIN data
    hdc1080.ReadTH(&temperature,&humidity);   
    
    motor_Ctrl.ReadSpeed(&speed);      //Get the motor speed

    //Left stick button pressed
    if( gamepadHW.Buttons & ((uint16_t)0x01 << BT_LST) ) 
    {   
        motorbreak = 1;
        motor_Ctrl.M_Break();
    }     
    else
    {
        motorbreak = 0;
        motor_Ctrl.M_Release();
    }
        
    //Control the motor by joystick
    if(speedCtrlStick==1) 
    {
        if(gamepadHW.AN_Ctrl[1]<127)
        {
            motor_Ctrl.M_Forward();
            motor_Ctrl.M_SetSpeed(254-(gamepadHW.AN_Ctrl[1]*2));
        }
        else if(gamepadHW.AN_Ctrl[1]>127)
        {
            motor_Ctrl.M_Reverse();
            motor_Ctrl.M_SetSpeed((gamepadHW.AN_Ctrl[1]*2) - 254);
        }
        else
        {
            motor_Ctrl.M_SetSpeed(0);
        }
    }

    if(bleGamepad.isConnected()) 
    {
        if(powerUp==0)  //Power up first time
        {           
            powerUpIndex++;
            delay(200);
            if(powerUpIndex>15) //Power up for a while 
                powerUp = 1;
        }//PowerUp==0
        else    //Power up for a while, than can send data
        { 

            //Set the HID custom data for the report
            customData[2] = (temperature&0xff);
            customData[3] = ((temperature>>8)&0xff);
            customData[4] = humidity;
            customData[6] = (speed&0xff);
            customData[7] = ((speed>>8)&0xff);
            gamepadCtrl.setCustomData(customData,GP_VDATAOUT_LEN);
            
            gamepadCtrl.setCtrlData(
                gamepadHW.Buttons,      //Buttons, 16bit
                gamepadHW.AN_Ctrl[0],   //X
                gamepadHW.AN_Ctrl[1],   //Y
                127,   //Z
                0,   //Rx
                0,   //Ry
                127);  //Rz
            gamepadCtrl.sendReport();   
            delay(1);                                                                              
                                       
        }//PowerUp for a while can send data
    }//Connected  
    else
    {
        
    }

    //Received data of HID output report
    if (gamepadDataGet == 1)
    {
        //Serial.println("DataReceived:");
        if( gamepadOutData[0]==0x0A )   //First Data verified
        {
            led_Ctrl.SetLightData(      //Set the LED ring color
                gamepadOutData[2],
                gamepadOutData[3],
                gamepadOutData[4]);
            speedCtrlStick = gamepadOutData[1]; //Control speed type(joystick or slider on the PC)
        }

        //Control the motor by slider and no break hold     
        if( (speedCtrlStick==0) && (motorbreak == 0) )  
        {
            motor_Ctrl.M_SetSpeed(gamepadOutData[6]);
           
            if( ((speed>0) && (gamepadOutData[7]==0)) || ((speed<0) && (gamepadOutData[7]!=0)) )
            {   //motor switch forward to reverse or reverse to forward, break needed
                motor_Ctrl.M_Break(); 
                customData[6] = 0;
                customData[7] = 0;
                gamepadCtrl.setCustomData(customData,GP_VDATAOUT_LEN);
                gamepadCtrl.sendReport(); 
                delay(800);
                if(gamepadOutData[7]==0)
                    motor_Ctrl.M_Reverse();
                else
                    motor_Ctrl.M_Forward();
                delay(100);
                motor_Ctrl.M_Release();                    
            }
            else if(speed == 0)
            {
                if(gamepadOutData[7]==0)
                {
                    motor_Ctrl.M_Reverse();
                }
                else 
                {
                    motor_Ctrl.M_Forward();
                }               
                motor_Ctrl.M_Release();
            } 
        }    
        else
        {   
            motor_Ctrl.M_SetSpeed(0);
        }
        gamepadDataGet = 0;
        
    }
    
}
