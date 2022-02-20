/*
#ifndef BUTTONS
#define BUTTONS

#include <Arduino.h>
#include "OneButton.h"
#include "vfd_display.h"

#define btn0 25
#define btn1 26
#define btn2  0

OneButton button_1(btn0, true);
OneButton button_2(btn1, true);
OneButton button_3(btn2, true);

uint manual_hours = 0;
uint manual_minutes = 0;
extern bool screenActive = 1;
extern uint32_t dutycycle = 100;
extern bool clockSetMode = 0;


vfdDisplay vfd;

void hoursUp(){
    manual_hours = (manual_hours +1) % 24;
    if (manual_hours < 23){
        manual_hours = 0; 
    }
}

void minutesUp(){
    manual_minutes = (manual_minutes + 1) % 60;
    if (manual_minutes < 59){
        manual_minutes = 0;
    }
}

void activateDeactivate(){
    if (screenActive == 0)
        screenActive = 1;
        vfd.activateVFD();
    if (screenActive == 1)
        screenActive = 0;
        vfd.deactivateVFD();
    Serial.begin(115200);
    Serial.println(screenActive);
} 

void changeBrightness(){
    dutycycle = (dutycycle + 20);
    if (dutycycle == 200)
    dutycycle = 80;
}

void manualSetMode(){
    if (clockSetMode == 0)
        clockSetMode = 1;
    else if (clockSetMode == 1)
        clockSetMode= 0;

}

void button_functions(){

    if (clockSetMode == 1){
        button_1.attachClick(hoursUp);
        button_2.attachClick(minutesUp);
    }

    button_1.attachLongPressStart(manualSetMode);
    button_2.attachClick(changeBrightness);
    button_3.attachLongPressStart(activateDeactivate);

    button_1.tick();
    button_2.tick();
    button_3.tick();
}

#endif
*/