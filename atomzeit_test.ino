
// unclear if needed
#include <Arduino.h>

// #include <SPI.h>
// #include <Wire.h>
// #include <LiquidCrystal.h>
// #include <RTClib.h> 
// #include <Time.h>

// needed
#include <Ticker.h>
#include "util.h"
#include "atomzeit.h"

#include "url.h"
WiFi wifi(5000); // 5 sec is needed to find out that URL is not valid
Atomzeit atom(&wifi); // try later in loop

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
    retCode=atom.getAtomzeitFromWeb();
    util::printfln("getAtomZeitFromWeb retCode=%d",retCode);

    Date date=atom.getDate();
    Hm time=atom.getTime();
    unsigned long midnight=atom.getMillis0();
    util::printfln("date: %d.%d.%d time: %d:%d",date.d,date.m,date.y,time.h,time.m);
    util::printfln("midnight millis: %ld %lu, current millis: %lu",midnight,midnight,millis());
    util::printfln("millis since midnight: %lu",millis()-midnight);
    util::printfln("calculated from time:  %10lu",(long) (time.h*60+time.m)*60000);

    // get sunrise
    Minute sunrise(atom.getSunrise());
    util::printfln("sunrise BB: %d:%d",sunrise.geth(),sunrise.getm());

    // get sunset
    Minute sunset(atom.getSunset());
    util::printfln("sunset BB: %d:%d",sunset.geth(),sunset.getm());

    // next event
    char eventType;
    int minutes2Event=atom.getNextEvent(&eventType);
    util::printfln("next event %d in %d minutes",eventType,minutes2Event);

    // check millis since midnight
    util::printfln("Check millis since midnight (1s delay between lines)");
    long t;
    for(int i=0; i<10; i++){
       t=atom.millis();
      util::printfln("millis since midnight: %ld",t);
      delay(1000);
    }
 
    util::println("waiting ...");
  }
  t1.sleep();
}

