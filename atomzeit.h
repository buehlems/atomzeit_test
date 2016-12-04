#ifndef ATOMZEIT_H
#define ATOMZEIT_H

#include "sun.h" // date
#include "hms.h" // hm
#include "WiFi.h" // WiFi
#include "url.h" // url

class Atomzeit {
 public:
  const char SUNRISE=0x01; // next event is sunrise
  const char SUNSET=0x02; // next event is sunset
  const char NEXTDAY=0x03; // next event is sunrise next day
  const char BEYOND=0x04; // beyond all the events
  const char ERROR=-1; // error, e.g. not initialized
  Atomzeit(WiFi *_w);
  ~Atomzeit();
  int getAtomzeitFromWeb();
  int getSunrise() { return sunrise; }
  int getSunset()  { return sunset; }
  Date &getDate() { return date; }
  Hm &getTime() { return time; }
  unsigned long getMillis0();
  long millis();
  bool isInitialized();
  long minutes2millis(int m);
  int millis2minutes(unsigned long mil);
  long millisSunrise();
  long millisNextSunrise();
  long millisSunset();
  int getNextEvent(char *type);
  int getNextEvent(int time,char *type=NULL);
  
 private:
  Url *url;
  int sunrise=-1; // sunrise today in minutes from midnight
  int sunset=-1;  // sunset today in minutes from midnight
  Date date;
  Hm time;
  unsigned long millis0=0; // milli counter at 0:00h

};

#endif
