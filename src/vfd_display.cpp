#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Ticker.h>
#include <Wire.h>
#include "Vfd_Display.h"

Adafruit_MCP23017 mcp;

int x = 10;
int _temp = 0;
uint manual_hours = 0;
uint manual_minutes = 0;
uint32_t dutycycle = 100;
bool screenActive = 1;
bool clockSetMode = 0;

OneButton button_1(btn0, true);
OneButton button_2(btn1, true);
OneButton button_3(btn2, true);

uint8_t dutyCycle = 100;
uint32_t freqMultiplex = 1000;
uint32_t freqHeat = 10000;

Ticker tickerMultiplex;

int vfdDisplay::_posMultiplex = 0;
uint16_t vfdDisplay::_dataMultiplex[5] = {
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000
};

// ======================================================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SETUP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ======================================================================================================

void vfdDisplay::start(){

    Serial.begin(115200);
    pinMode(en_24v, OUTPUT);
    pinMode(heat_nsleep, OUTPUT);
    pinMode(heat_int1, OUTPUT);
    pinMode(heat_int2, OUTPUT);

    digitalWrite(en_24v, HIGH);
    digitalWrite(heat_nsleep, HIGH);
    digitalWrite(heat_int1, LOW);
    digitalWrite(heat_int2, LOW);

    Wire.setClock(400000L);
    mcp.begin();
    for(int i = 0; i < 16; i++){
    mcp.pinMode(i,OUTPUT);
    }
    mcp.writeGPIOAB(0x0000); // all pins low

    ledcSetup(0, freqHeat, 8);
    ledcAttachPin(heat_int1, 0);
    ledcWrite(0, dutyCycle);

    tickerMultiplex.attach_ms(
    1000.0/freqMultiplex,
    _nextMultiplex);

}

void vfdDisplay::activateVFD(){
        digitalWrite(en_24v, HIGH);
        digitalWrite(heat_nsleep, HIGH);
        digitalWrite(heat_int2, LOW);
        ledcSetup(0, freqHeat, 8);
        ledcAttachPin(heat_int1, 0);
        ledcWrite(0, dutycycle);
}

void vfdDisplay::deactivateVFD(){
        digitalWrite(en_24v, LOW);
        digitalWrite(heat_nsleep, LOW);
        ledcDetachPin(heat_int1);
}

// ======================================================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BUTTONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ======================================================================================================

void vfdDisplay::hoursUp(){
    manual_hours = (manual_hours +1) % 24;
    if (manual_hours < 23){
        manual_hours = 0; 
    }
}

void vfdDisplay::minutesUp(){
    manual_minutes = (manual_minutes + 1) % 60;
    if (manual_minutes < 59){
        manual_minutes = 0;
    }
}

void vfdDisplay::activateDeactivate(){
    if (screenActive == 0){
        screenActive = 1;
        vfdDisplay::activateVFD();}
    else if (screenActive == 1){
        screenActive = 0;
        vfdDisplay::deactivateVFD();}
    Serial.begin(115200);
    Serial.println(screenActive);
} 

void vfdDisplay::changeBrightness(){
    dutycycle = (dutycycle + 20);
    if (dutycycle == 200)
        dutycycle = 80;
    vfdDisplay::activateVFD();
    Serial.begin(115200);
    Serial.println(dutycycle);
}

void vfdDisplay::manualSetMode(){
    if (clockSetMode == 0)
        clockSetMode = 1;
    else if (clockSetMode == 1)
        clockSetMode= 0;
    Serial.begin(115200);
    Serial.println(clockSetMode);

}

void vfdDisplay::buttonSetup(){
    button_1.attachLongPressStart(vfdDisplay::manualSetMode);
    button_2.attachClick(vfdDisplay::changeBrightness);
    button_3.attachLongPressStart(vfdDisplay::activateDeactivate);
}

void vfdDisplay::buttonFunctions(){

    if (clockSetMode == 1){
        button_1.attachClick(vfdDisplay::hoursUp);
        button_2.attachClick(vfdDisplay::minutesUp);
    }

    button_1.tick();
    button_2.tick();
    button_3.tick();
}

// ======================================================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DISPLAY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ======================================================================================================

void vfdDisplay::_nextMultiplex() {
  mcp.writeGPIOAB(0x0000);                        // turn off to reduce crosstalk
  _posMultiplex = (_posMultiplex + 1) % 5;        // interate the position
  mcp.writeGPIOAB(_dataMultiplex[_posMultiplex]); // write out
}

void vfdDisplay::_updateMultiplex(){
  for (int i = 0; i < 5; i++){
    uint16_t tempStore = 0;
    // .digit array only valid for characters 0-3
    if (i <  4 && _screen.digit[i].a) {tempStore |= (1 << SEG_A);}
    if (i <  4 && _screen.digit[i].b) {tempStore |= (1 << SEG_B);}
    if (i <  4 && _screen.digit[i].c) {tempStore |= (1 << SEG_C);}
    if (i <  4 && _screen.digit[i].d) {tempStore |= (1 << SEG_D);}
    if (i <  4 && _screen.digit[i].e) {tempStore |= (1 << SEG_E);}
    if (i <  4 && _screen.digit[i].f) {tempStore |= (1 << SEG_F);}
    if (i <  4 && _screen.digit[i].g) {tempStore |= (1 << SEG_G);}
    if (i == 4 && _screen.dp[0])      {tempStore |= (1 << SEG_DP1);}
    if (i == 4 && _screen.dp[1])      {tempStore |= (1 << SEG_DP2);}

    switch (i) {
      case 0: {tempStore |= (1 << CHAR_1); break;}
      case 1: {tempStore |= (1 << CHAR_2); break;}
      case 2: {tempStore |= (1 << CHAR_3); break;}
      case 3: {tempStore |= (1 << CHAR_4); break;}
      case 4: {tempStore |= (1 << CHAR_5); break;}
    }
    _dataMultiplex[i] = tempStore;
  }
}

void vfdDisplay::setHours(uint8_t hours){
  switch ((hours % 100)/10) { //        a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[0] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[0] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[0] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[0] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[0] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[0] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[0] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[0] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[0] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[0] = c; break;}
  }
  switch (hours % 10) { //              a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[1] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[1] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[1] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[1] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[1] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[1] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[1] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[1] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[1] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[1] = c; break;}
  }
  _updateMultiplex();
}

void vfdDisplay::setMinutes(uint8_t minutes){
  switch ((minutes % 100)/10) { //      a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[2] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[2] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[2] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[2] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[2] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[2] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[2] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[2] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[2] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[2] = c; break;}
  }
  switch (minutes % 10) { //            a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[3] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[3] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[3] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[3] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[3] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[3] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[3] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[3] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[3] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[3] = c; break;}
  }
  _updateMultiplex();
}

void vfdDisplay::setDP(bool dp1, bool dp2){
  if (dp2 && dp1)   {_screen.dp[0] = 1; _screen.dp[1] = 1;}
  if (dp2 && !dp1)  {_screen.dp[0] = 1; _screen.dp[1] = 0;}
  if (!dp2 && dp1)  {_screen.dp[0] = 0; _screen.dp[1] = 1;}
  if (!dp2 && !dp1) {_screen.dp[0] = 0; _screen.dp[1] = 0;}

  _updateMultiplex();
}