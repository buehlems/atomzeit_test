
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
    util::println(F("starting WiFi"));
    wifi.init(); // start command mode and close active sockets
    tn.over();
}


void loop(void){
  int retCode=0;
  long t_rest=tn.rest();
  // util::msgln(F("remaining time=%ld"), t_rest);
  // atomzeit atom(&wifi);

  if(tn.check()){ // time is over
    util::println(F("********** new loop **********"));

    #define WORLD 1
    #if WORLD == 0
    retCode=atom.getAtomzeitFromWeb();
    util::printfln("getAtomZeitFromWeb retCode=%d",retCode);
    #else
    retCode=atom.getWorldTimeFromWeb("/api/timezone/Europe/Berlin.txt");
    // retCode=atom.getWorldTimeFromWeb("/api/timezone/America/Caracas.txt"); // to check negative UTC
    util::printfln("getWorldTimeFromWeb retCode=%d",retCode);
    retCode=atom.getSunriseSunsetFromWeb("/json?lat=48.633690&lng=9.047205&formatted=0"); // https://api.sunrise-sunset.org/json?lat=48.633690&lng=9.047205&formatted=0
    util::printfln("getSunriseSunsetFromWeb retCode=%d",retCode);
    #endif

    Date date=atom.getDate();
    Hms time=atom.getTime();
    Hm  utc=atom.getUTC();
    Minute sunrise(atom.getSunrise());
    Minute sunset(atom.getSunset());
    unsigned long midnight=atom.getMillis0();
    util::printfln(F("date: %02d.%02d.%04d time: %02d:%02d:%02d utc: %02d:%02d"),date.d,date.m,date.y,time.h,time.m,time.s,utc.h,utc.m);
    util::printfln(F("sunrise BB: %02d:%02d"),sunrise.geth(),sunrise.getm());
    util::printfln(F("sunset BB:  %02d:%02d"),sunset.geth(),sunset.getm());
    util::printfln(F("midnight millis: signed %ld unsigned %lu, current millis: %lu"),midnight,midnight,millis());
    util::printfln(F("millis since midnight: %10lu"),millis()-midnight);
    util::printfln(F("calculated from time:  %10lu"),(long) (time.h*60+time.m)*60000);

    // next event
    char eventType;
    int minutes2Event=atom.getNextEvent(&eventType);
    util::printfln(F("next event %d in %d minutes"),eventType,minutes2Event);

    // check millis since midnight
    util::printfln(F("Check millis since midnight (1s delay between lines)"));
    long t;
    for(int i=0; i<10; i++){
       t=atom.millis();
       util::printfln(F("millis since midnight: %ld"),t);
      delay(1000);
    }
 
    util::println(F("waiting ..."));
  }
  t1.sleep();
}

