#include "kernel.h"
#include <Herkulex.h>

void setup()
{
delay(2000);  //a delay to have time for serial monitor opening
    Serial.begin(115200);    // Open serial communications
    Serial.println("Begin");
    Herkulex.beginSerial1(115200); //open serial port 1 
  
    Herkulex.reboot(idfr); 
    Herkulex.reboot(idfl);
    Herkulex.reboot(idbr);
    Herkulex.reboot(idbl);
    Herkulex.reboot(idd);

    delay(500); 
    Herkulex.initialize(); //initialize motors
}

void loop()
{
    if(isblack())
    {
        apply(Move(2, 1));
        if(wallr())
        {
            apply(Move(1, 2));
        }
        else
        {
            apply(Move(1, 1));
            apply(Move(0, 1));
        }
    }
    else
    {
        if(!walll())
        {
            apply(Move(3, 1));
            apply(Move(0, 1));
        }
        else if(!wallf())
        {
            apply(Move(0, 1));
        }
        else if(!wallr())
        {
            apply(Move(1, 1));
            apply(Move(0, 1));
        }
        else
        {
            apply(Move(1, 2));
            apply(Move(0, 1));
        }
    }
}
