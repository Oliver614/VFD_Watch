#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include "vfd_display.h"


#define NTP_SERVER "uk.pool.ntp.org"
#define TZ_INFO "" 

WiFiUDP udp;

TaskHandle_t vfdTask;
// vfdDisplay vfd;

const char ssid[]     = "BTHub6-R3CX";
const char password[] = "gCG4k4FKwrm7";


int seconds = 0;
int minutes = 0;
struct tm local;

Ticker updateTime;

void initialBoot(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  for(int i = 0, time = 0; WiFi.status() != WL_CONNECTED; ++i, ++time) {
    i = i % 4;
    //wifiAnimation(i);
    Serial.print(".");
    if(time > 10){
      //startManualTimeSet(vfd); //
      WiFi.mode( WIFI_OFF );
      return;
    }
  }

  configTzTime(TZ_INFO, NTP_SERVER); // config system time via NTP server
  getLocalTime(&local, 10000);       // get time for 10s
  setenv("TZ", TZ_INFO, 1); // reset timezone
  tzset();

  WiFi.mode( WIFI_OFF );
}

Ticker updateButtons; 
vfdDisplay vfd;


void setup() {
  // put your setup code here, to run once:
  initialBoot();
  time_t now;
  time(&now);
  tm *time = localtime(&now);
  vfd.setDP(1,1);

  vfd.start();
  vfd.buttonSetup();

  updateButtons.attach(0.1, [](){
  vfd.buttonFunctions();
  });
vfd.setMinutes(time->tm_min);
vfd.setHours(time->tm_hour);

}

void loop(){

}