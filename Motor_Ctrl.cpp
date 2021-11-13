/*
 * Motor_Ctrl.cpp
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#include "Motor_Ctrl.h"
#include <Ticker.h>
#include "ESP32PWM.h"

#define PWM_M_PIN      25       //Motor control PWM output PIN
#define M_BREAK_PIN    27       //Motor control break output PIN
#define M_FR_PIN       33       //Motor control forward/reverse output PIN
#define M_CNT_A_PIN    32       //Motor encoder input A PIN
#define M_CNT_B_PIN    35       //Motor encoder input B PIN

#define M_SPEED_MAX    255      //Max speed control value
#define COUNT_TIME     200      //Motor speed count time(mS)

ESP32PWM pwm_M;
Ticker timerMCount;                
volatile uint16_t M_Counter=0;
volatile uint16_t M_FCounter=0;
volatile uint16_t M_RCounter=0;

uint16_t M_speed = 0;
int M_FR=0;
Motor_Ctrl::Motor_Ctrl() 
{

}

//Counter for motor encoder
//Reverse:
//                  _______                       _______
//pulse B _________|       |_____________________|       |_______________        
//                      _______                       _______
//pulse A _____________|       |_____________________|       |___________ 
//
//Forward:
//                          _______                       _______
//pulse B _________________|       |_____________________|       |_______        
//                      _______                       _______
//pulse A _____________|       |_____________________|       |___________  
//                      ^
//                      |
//                     Interrupt: M_Counter_Encoder
//
//            | <-----------------timerMCount---------------------->| 
//            |  ____       ____       ____       ____       ____   |                 
//pulse ________|    |_____|    |_____|    |_____|    |_____|    |____
//
//
void M_Counter_Encoder()
{  
    M_Counter++;
    if(digitalRead(M_CNT_B_PIN)==0)
        M_FCounter++;
    else
        M_RCounter++;
}

//Timer interrupt, every COUNT_TIME 
void readEncoder()
{
    M_speed = M_Counter;
    if(M_FCounter > M_RCounter)         
        M_FR = 1;
    else if(M_FCounter < M_RCounter)
        M_FR = 0;
    else
        M_speed = 0;
    M_FCounter = 0;
    M_RCounter = 0;
    M_Counter = 0; 
}

void Motor_Ctrl::Init() 
{
    pinMode(M_BREAK_PIN, OUTPUT);  
    pinMode(M_FR_PIN, OUTPUT); 
    pinMode(M_CNT_A_PIN, INPUT);    
    pinMode(M_CNT_B_PIN, INPUT);  
    M_Break();
    //M_Release();
    //M_Reverse();
    M_Forward();
    ESP32PWM::allocateTimer(0);
    pwm_M.attachPin(PWM_M_PIN, 20000, 10);  //PWM output 20KHz 10 bit
    M_SetSpeed(0);
    
    attachInterrupt(M_CNT_A_PIN, M_Counter_Encoder, RISING);//Interrupt for encoder pulse A PIN     
    timerMCount.attach_ms(COUNT_TIME, readEncoder);  //Timer counter
    interrupts();                      //Enable interrupts 
}

//Get the speed data 
void Motor_Ctrl::ReadSpeed(int16_t* speed) 
{
    if(M_FR==1)
        *speed = M_speed;
    else if(M_FR==0)
        *speed = -M_speed;
    else
        *speed = 0;
}

void Motor_Ctrl::M_Break() 
{
    digitalWrite(M_BREAK_PIN, LOW);
}

void Motor_Ctrl::M_Release() 
{
    digitalWrite(M_BREAK_PIN, HIGH);
}

void Motor_Ctrl::M_Forward() 
{
    digitalWrite(M_FR_PIN, LOW);
}

void Motor_Ctrl::M_Reverse() 
{
    digitalWrite(M_FR_PIN, HIGH);
}

void Motor_Ctrl::M_SetSpeed( byte Speed ) 
{
    pwm_M.writeScaled( 1 - (float)Speed/(float)M_SPEED_MAX);
}

