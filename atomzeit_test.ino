
// unclear if needed
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h> 
#include <Time.h>

// needed
#include <Ticker.h>


#include "util.h"
#include "atomzeit.h"

#include "url.h"
WiFi wifi(5000); // 5 sec is needed to find out that URL is not valid
atomzeit atom(&wifi); // try later in loop

 // Cycle Timer
Ticker tn(10L*60L*1000L); // SMA loop time
Ticker t1(1*1000L); // 1s

void setup(void) {

    /*
     * The Seeeduino Stalker allows you turn power
     * off to the Bee port altogether which makes
     * our lives easier.
     */
    // pinMode(POWER_BTBEE, OUTPUT);
    // pinMode(DISCONNECT_BTBEE, OUTPUT);

    Serial.begin(115200); // debug 

    // start Wifi
    util::println("starting WiFi");
    wifi.init(); // start command mode and close active sockets
    tn.over();
}


void loop(void){
  int retCode=0;
  long t_rest=tn.rest();
  // util::msgln("remaining time=%ld", t_rest);
  // atomzeit atom(&wifi);

  if(tn.check()){ // time is over
    util::println("********** new loop **********");
    retCode=getAtomZeitFromWeb();
    util::printfln("getAtomZeitFromWeb retCode=%d",retCode);

    date date=atom.getDate();
    hm time=atom.getTime();
    unsignel long midnight=atom.getMillis0();
    util::printfln("date: %d.%d.%d time: %d:%d",date.d,date.m,date.y,time.hh,time.mm);
    util::printfln("midnight millis: %ld, current millis: %ld",midnight,millis());
    util::printfln("millis since midnight: %10ld",millis()-midnight);
    util::printfln("calculated from time:  %10ld",(long) (time.hh*60+time)*60000);

    // get sunrise
    minute sunrise(atom.getSunrise());
    util::printfln("sunrise: %d:%d",sunrise.gethh(),sunrise.getmm());

    // get sunset
    minute sunset(atom.getSunset());
    util::printfln("sunset: %d:%d",sunset.gethh(),sunset.getmm());
 
    util::println("waiting ...");
  }
  t1.sleep();
}

