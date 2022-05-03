#ifndef VFD_DISPLAY_H
#define VFD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "OneButton.h"

// defining the esp32 gpio
#define btn0 25
#define btn1 26
#define btn2  0
#define heat_int1 32
#define heat_int2 33
#define heat_nsleep 27
#define bat_sense 34
#define en_24v 2

// defining the mcp gpio
#define SEG_A      9
#define SEG_B     11
#define SEG_C      2
#define SEG_D     13
#define SEG_E      6
#define SEG_F     10
#define SEG_G      5
#define SEG_DP1    9
#define SEG_DP2   15

#define CHAR_1     7
#define CHAR_2     3
#define CHAR_3    12
#define CHAR_4     8
#define CHAR_5     1 //DP

class vfdDisplay{
    // static Adafruit_MCP23017 mcp;
    Ticker tickermultiplex;
public:

  struct character{ // data type of a single 7-segment
    bool a;
    bool b;
    bool c;
    bool d;
    bool e;
    bool f;
    bool g;
    // initalize bool variables as false to avoid unwanted side effects
    character() : a(0), b(0), c(0), d(0), e(0), f(0), g(0) {}
    character(bool a_, bool b_, bool c_, bool d_, bool e_, bool f_, bool g_) :
             a(a_), b(b_), c(c_), d(d_), e(e_), f(f_), g(g_) {}
  };
  struct screen{   // data type of a whole screen + decimal points
    character digit[4];
    bool dp[2] = {1};
  };

  static void start();
  static void activateVFD();
  static void deactivateVFD();
  static void minutesUp();
  static void hoursUp();
  static void activateDeactivate();
  static void changeBrightness();
  static void manualSetMode();
  static void buttonFunctions();
  static void buttonSetup();
  static void drawDisplay();
  void setHours(uint8_t hours);
  void setMinutes(uint8_t minutes);
  void setDP(bool dp1, bool dp2);
  private:
      screen _screen; // exchange varable
  static int _posMultiplex;
  static uint16_t _dataMultiplex[5];
  static void _nextMultiplex();
  void _updateMultiplex();
};

#endif