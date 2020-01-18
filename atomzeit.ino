// #include <Arduino.h>

#include <limits.h>

#include "util.h"
#include "url.h"

/****f* 
  *  NAME
  *    Atomzeit -- 
  *  SYNOPSIS
  *   Atomzeit az=Atomzeit(wifi);
  *  FUNCTION
  *    define Atomzeit
  *  INPUTS
  *    wifi      - a wifi
  *  RESULT
  *    ---
   ******
*/
Atomzeit::Atomzeit(WiFi_TL *_w){
  url=new Url(_w);
  utc.h=0;
  utc.m=0;
}

Atomzeit::~Atomzeit(){
  delete url;
}

/****f* 
  *  NAME
  *    replaceChar -- 
  *  SYNOPSIS
  *   replaceDash("2020-01-18T08:51:56.144171+01:00",'-',':',2);
  *  FUNCTION
  *    replace character n times
  *  INPUTS
  *    str: the string
  *    c: the character to be replaced
  *    r: the replacement
  *    n: max n times (0: all aka UINT_MAX)
  *  RESULT
  *    -
   ******
*/
void Atomzeit::replaceChar(char *str, char c, char r, unsigned int n){
  if(n=0)
    n=UINT_MAX;
  while(*str !='\0'){
    if(*str == c){
      *str=r;
      if(--n == 0)
	break;
    }
    str++;
  }
}

/****f* 
  *  NAME
  *    WorldTimeFromWeb -- 
  *  SYNOPSIS
  *   int retcode=getWorldTimeFromWeb("/api/timezone/Europe/Berlin.txt");
  *  FUNCTION
  *    get current time values from http://worldtimeapi.org:
  *        date & time 
  *        millis0: milli counter at 0:00h
  *        utc: offest to UTC in minutes
  *  INPUTS
  *    timezone: the timezone as indicated here: http://worldtimeapi.org/timezones
  *  RESULT
  *    return code (see url.h)
   ******
*/
int Atomzeit::getWorldTimeFromWeb(const char *timezone){
  unsigned long us;
  int retCode=url->ok;
  util::printfln(F("********** getWorldTimeFromWeb %s **********"),timezone);
  retCode=url->requestWebPage("worldtimeapi.org",timezone);
  util::printfln(F("requestWebPage retCode=%d"),retCode);

  // get date and time
  url->resetRespBuf();
  retCode=url->findString("datetime: ",url->removeSearchString); // remove the search string and refill buffer after found
  // the buffer should now point to something like: 
  // 2020-01-17T20:56:21.553777+01:00
  // 01234567890123456789012345678901 (fits exactly into the response buffer of 32 bytes)
  util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());

  // although the datetime line should fully fit into the resp buffer, we get it in two steps to be on the safe side

  Date ad; // atom date
  // %02hhd: reads the next two characters into a 1 byte
  sscanf(url->getBuf(),"%d-%02hhd-%02hhd",&date.y,&date.m,&date.d);
  
  retCode=url->findString("T",url->removeSearchString); // remove the search string and refill buffer after found
  util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());
  sscanf(url->getBuf(),"%d:%d:%d.%lu%d:%d",&time.h,&time.m,&time.s,&us,&utc.h,&utc.m);
  
  util::printfln(F("date: %d.%d.%d time: %d:%d:%d utc: %d:%d"),date.d,date.m,date.y,time.h,time.m,time.s,utc.h,utc.m);

  // now calculate the millis at last midnight
  unsigned long t=::millis();
  unsigned long millis_since_midnight=(time.h*60+time.m)*60000;
  this->millis0=t-millis_since_midnight;

  url->closeConnection();

  return retCode;
}

/****f* 
  *  NAME
  *    sunRiseSetGetTime -- 
  *  SYNOPSIS
  *   int retcode=sunRiseSetGetTime(const char *type,hm t);
  *  FUNCTION
  *    get today's sunrise or sunset adjusted to local time (+ UTC)
  *  INPUTS
  *    type: "sunrise" or "sunset"
  *    t: struct to time of the event
  *  RESULT
  *    return code (see url.h)
   ******
*/
int Atomzeit::sunRiseSetGetTime(const char* type, Hm &t){
  
  int retCode;
  
  // search sunrise or sunset
  retCode=url->findString(type,url->removeSearchString); // remove the search string and refill buffer after found
  // the buffer should now point to something like: 
  // sunrise":"2020-01-18T07:28:05+00:00"
  // 012345678901234567890123456789012345
  // util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());

  // move on to T
  retCode=url->findString("T",url->removeSearchString); // remove the search string and refill buffer after found
  // the buffer should now point to something like: 
  // "07:28:05+00:00"
  // 012345678901234567890123456789012345
  // util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());

  // get the time
  sscanf(url->getBuf(),"%d:%d:",&t.h,&t.m); // we don't care about the seconds

  // local time adjust
  t.h += utc.h;
  t.m += utc.m;

  util::printfln(F("%s %d:%d"),type,t.h, t.m);
  return retCode;
}


/****f* 
  *  NAME
  *    getSunriseSunsetFromWeb -- 
  *  SYNOPSIS
  *   int retcode=getSunriseSunsetFromWeb("json?lat=48.633690&lng=9.047205&formatted=0");
  *  FUNCTION
  *    get today's sunrise and sunset from api.sunrise-sunset.org
  *        sunrise, sunset in minutes from midnight
  *  INPUTS
  *    timezone: the timezone as indicated here: http://worldtimeapi.org/timezones
  *  RESULT
  *    return code (see url.h)
   ******
*/
int Atomzeit::getSunriseSunsetFromWeb(const char* coordinates){

  int retCode=url->ok;
  util::printfln(F("********** getSunriseSunsetFromWeb %s **********"),coordinates);
  retCode=url->requestWebPage("api.sunrise-sunset.org",coordinates);
  util::printfln(F("requestWebPage retCode=%d"),retCode);

  url->resetRespBuf();

  Hm sunevent;

  // get sunrise
  retCode=sunRiseSetGetTime("sunrise", sunevent);
  Minute sunrise(sunevent);

  // get sunset
  retCode=sunRiseSetGetTime("sunset", sunevent);
  Minute sunset(sunevent);

  url->closeConnection(); // connection is not needed anymore

  // now calc the minutes of sunrise and sunset from midnight
  this->sunrise=sunrise.getMinutesDay(); 
  this->sunset =sunset.getMinutesDay(); 
        
  return retCode;
}


/****f* 
  *  NAME
  *    getAtomZeitFromWeb -- 
  *  SYNOPSIS
  *   int retcode=getAtomZeitFromWeb();
  *  FUNCTION
  *    get current time values from atomzeit.eu (Berlin):
  *        date & time 
  *        sunrise, sunset in minutes from midnight
  *        millis0: milli counter at 0:00h
  *        no UTC adjustment
  *  INPUTS
  *    
  *  RESULT
  *    return code (see url.h)
   ******
*/
int Atomzeit::getAtomzeitFromWeb(){
  int retCode=url->ok;
  util::println(F("********** getAtomzeitFromWeb **********"));
  // retCode=url->requestWebPage("www.uhrzeit.org","/atomuhr.php");
  retCode=url->requestWebPage("www.atomzeit.eu","/");
  util::printfln(F("requestWebPage retCode=%d"),retCode);

  // get date and time
  url->resetRespBuf();
  retCode=url->findString("Aktuelle Zeit:",url->removeSearchString);

  WiFiSocket *socket=url->getSocket();
  util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());
    
  Date ad; // atom date
  sscanf(url->getBuf(),"%d.%d.%d%*s%d:%d",&date.d,&date.m,&date.y,&time.h,&time.m);
  time.s=0;
  util::printfln(F("date: %02hhd.%%02hhd.%d time: %d:%d"),date.d,date.m,date.y,time.h,time.m);

  // get sunrise
  Hm tmp;
  retCode=url->findString("<b>Sonnenaufgang:</b> ",url->removeSearchString);
  sscanf(url->getBuf(),"%d:%d",&tmp.h,&tmp.m);
  Minute sunrise(tmp); // sunrise and sunset

  util::msgln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());   
  util::msgln(F("sunrise Berlin: %d:%d"),sunrise.geth(),sunrise.getm());

  retCode=url->findString("Sonnenuntergang:",url->closeAfterFind | url->removeSearchString);
  sscanf(url->getBuf(),"%d:%d",&tmp.h,&tmp.m);
  Minute sunset(tmp);

  util::printfln(F("findString retCode=%d\nBuffer=%s"),retCode,url->getBuf());   
  util::printfln(F("sunset Berlin: %d:%d"),sunset.geth(),sunset.getm());

  // now calc the minutes of sunrise and sunset from midnight
  Sun sunRiseSet(date.y); // needed for adjusting Berlin sunrise/set to local sunrise/set
  this->sunrise=sunrise.getMinutesDay()+sunRiseSet.adjustSunRise(date); 
  this->sunset =sunset.getMinutesDay()+sunRiseSet.adjustSunSet(date); 
    
  // now calculate the millis at last midnight
  unsigned long t=::millis();
  unsigned long millis_since_midnight=(time.h*60+time.m)*60000;
  this->millis0=t-millis_since_midnight;
    
  return retCode;
}




/****f* 
  *  NAME
  *    getMillis0 -- get value of the millis counter at midnight today
  *  SYNOPSIS
  *   unsigned long m=getMillis0();
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    
  *  RESULT
  *    millis couner at midnight (see url.h)
   ******
*/
unsigned long Atomzeit::getMillis0() { 
  return this->millis0; 
}

/****f* 
  *  NAME
  *    millis() -- calculate the milliseconds passed since midnight
  *  SYNOPSIS
  *   long m=millis();
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    
  *  RESULT
  *    millis seconds since midnight. -1 if invalid
   ******
*/
long Atomzeit::millis() { 
  if(getMillis0()==0 && ! isInitialized()){
    return -1;
  }
  unsigned long t=::millis();
  return (long)(t-getMillis0());
}

/****f* 
  *  NAME
  *    isInitialized() -- check if Atomzeit has been initialized
  *  SYNOPSIS
  *   bool isinit=atomzeit.isInitialized();
  *  FUNCTION
  *    In order to properly set all internal structures, you must get the current time from the web by calling 
  *    getAtomzeitFromWeb. This will set sunrise and sunset. This function will check of sunrise and sunset are > =0 and then
  *    return true.
  *  INPUTS
  *    
  *  RESULT
  *    t if initialized
   ******
*/
bool Atomzeit::isInitialized() { 
  return (this->sunrise>=0 && this->sunset>=0) ? true:false;
}

/****f* 
  *  NAME
  *    minutes2millis -- calculate the millis of a point in time
  *  SYNOPSIS
  *   long md=millis(minutes);
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    minutes - the minutes since midnight
  *  RESULT
  *    milli seconds since midnight. -1 if invalid
   ******
*/
long Atomzeit::minutes2millis(int m) { 
  return((long)m * 60 * 1000);
}

/****f* 
  *  NAME
  *    millis2minutes - convert milli seconds into minutes
  *  SYNOPSIS
  *   int minutes=millis2minutes(::millis());
  *  FUNCTION
  *    
  *  INPUTS
  *    mil - the milli secondds
  *  RESULT
  *    minutes
   ******
*/
int Atomzeit::millis2minutes(unsigned long mil) { 
  return(mil/((unsigned long)60*1000));
}


/****f* 
  *  NAME
  *    millisSunrise -- calculate the millis of the sunrise today
  *  SYNOPSIS
  *   long m=millisSunrise();
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    
  *  RESULT
  *    millis seconds since midnight. -1 if invalid
   ******
*/
long Atomzeit::millisSunrise() { 
  return(minutes2millis(this->sunrise));
}

/****f* 
  *  NAME
  *    millisNextSunrise -- calculate the millis of the sunrise of the next day
  *  SYNOPSIS
  *   long m=millisNextSunrise();
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    
  *  RESULT
  *    millis seconds since midnight today. -1 if invalid
   ******
*/
long Atomzeit::millisNextSunrise() { 
  return(minutes2millis(this->sunrise+Minute::day));
}


/****f* 
  *  NAME
  *    millisSunset -- calculate the millis of the sunset today
  *  SYNOPSIS
  *   long m=millisSunset();
  *  FUNCTION
  *    getAtomzeit must have run before to get the actual time and calculate the millis counter at the beginning of this day
  *  INPUTS
  *    
  *  RESULT
  *    millis seconds since midnight. -1 if invalid
   ******
*/
long Atomzeit::millisSunset() { 
  return(minutes2millis(this->sunset));
}

/****f* 
  *  NAME
  *    getNextEvent -- get the minutes from time until the next event (sunrise or sunset)
  *  SYNOPSIS
  *   int m=getNextEvent(minutes,&type);
  *  FUNCTION
  *    calculate the time until the next event occurs
  *  INPUTS
  *    time - current time in minutes
  *    type - return value for the type (optional)
  *  RESULT
  *    minuntes until the next event and set type. 
   ******
*/
int Atomzeit::getNextEvent(int time, char *type) { 
  int m=0; // remaining time
  char t=0; // type

  if(!this->isInitialized()){
    m=this->ERROR;
    t=this->ERROR;
  }else if(time > sunrise+Minute::day){
    t=this->BEYOND;
    m=0;
  }else if(time > sunset){
    t=this->NEXTDAY;
    m= sunrise+Minute::day-time;
  }else if(time > sunrise){
    t=this->SUNSET;
    m= sunset-time;
  }else{ 
    t=this->SUNRISE;
    m= sunrise-time;
  }
  if(type)
    *type=t;
  return m;
}

/****f* 
  *  NAME
  *    getNextEvent - get the minutes until the next event (sunrsise or sunset)
  *  SYNOPSIS
  *   int m=getNextEvent(&type);
  *  FUNCTION
  *    calculate the time from now until the next event occurs
  *  INPUTS
  *    type - return value for the type (optional)
  *  RESULT
  *    minuntes until the next event and set type. 
   ******
*/
int Atomzeit::getNextEvent(char *type) { 
  long ml=this->millis();
  return(getNextEvent(millis2minutes(ml),type));
}