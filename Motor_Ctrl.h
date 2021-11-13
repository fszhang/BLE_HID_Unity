/*
 * Motor_Ctrl.h
 *
 *  Created on: Sep 01, 2021
 *      Author: Francis Zhang
 */
#ifndef MOTOR_CTRL_H__
#define MOTOR_CTRL_H__

#include "Arduino.h"


class Motor_Ctrl 
{
    
private:
    char readChar;
public:
    Motor_Ctrl();
    void Init();
    void M_Break();
    void M_Release();
    void M_Forward();
    void M_Reverse();
    void M_SetSpeed( byte Speed ); 
    void Direct( byte Direction );
    void ReadSpeed(int16_t* speed);

};

#endif // MOTOR_CTRL_H
