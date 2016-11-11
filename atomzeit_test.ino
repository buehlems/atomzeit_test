#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h> 
#include <Time.h>
#include <Ticker.h>


// #include "sma_web.h"
#include "util.h"
#include "smaLcd.h"
#include "sma.h"

#include "WiFi.h"
#include "url.h"
WiFi wifi(5000); // 5 sec is needed to find out that URL is not valid
url url(&wifi); 

 
/*
 * Faster baud rates seem to be dropping characters, need
 * to do more definitive testing at faster rates. We aren't
 * terribly concerned about speed, so it's fine to use
 * slightly slower speeds.
 */
#define BTBEE_BAUD_RATE    38400

#define MAX_RETRIES 3

unsigned long total_kwh = 0;
unsigned long day_kwh = 0;
unsigned long spot_ac = 0;
unsigned long time = 0;

unsigned long last_total_kwh = 0;
unsigned long last_day_kwh = 0;
unsigned long last_spot_ac = 0;
unsigned long last_time = 0;

// initialize the library with the numbers of the interface pins
smaLcd lcd(8, 13, 9, 4, 5, 6, 7);

char fbuf[16];

// Cycle Timer
// Ticker tn(10L*60L*1000L); // SMA loop time
Ticker tn(10L*60L*1000L); // SMA loop time
Ticker t1(1*1000L); // 1s

void printTime(unsigned long secsSince2000){
  DateTime now(secsSince2000);
  util::msg("time: %lu ",secsSince2000);
  util::msgln("%d-%.2d-%.2d %2d:%.2d:%.2d",
		now.year(),
		now.month(),
		now.day(),
		now.hour(),
		now.minute(),
		now.second()); 
}

char *getDate(unsigned long secsSince2000, char *strbuf){
  DateTime now(secsSince2000);
  sprintf(strbuf,"%d-%.2d-%.2d",
		now.year(),
		now.month(),
		now.day()); 
  return (strbuf);
}

char *getYear(unsigned long secsSince2000, char *strbuf){
  DateTime now(secsSince2000);
  itoa(now.year(),strbuf,10);
  return (strbuf);
}

char *getTime(unsigned long secsSince2000, char *strbuf){
  DateTime now(secsSince2000);
  sprintf(strbuf,"%d:%.2d:%.2d",
		now.hour(),
		now.minute(),
		now.second()); 
  return(strbuf);
}



void setup(void) {

    /*
     * The Seeeduino Stalker allows you turn power
     * off to the Bee port altogether which makes
     * our lives easier.
     */
    // pinMode(POWER_BTBEE, OUTPUT);
    // pinMode(DISCONNECT_BTBEE, OUTPUT);

    Serial.begin(115200); // debug 

    // start Bluetooth
    // Serial3.begin(BTBEE_BAUD_RATE);
    // setupMaster();

    // start Wifi
    util::println("starting WiFi");
    wifi.init(); // start command mode and close active sockets

    // start LCD
    lcd.begin(16, 2);
    lcd.setCursor(0, 1);
    lcd.printConnected();
    tn.over();

    // test values for web page
    total_kwh=9876543;
    day_kwh=123456;
    spot_ac=0;
    last_time=1000000;
}

/****f* 
  *  NAME
  *    checkSMAValue -- checks an SMA value for error and resets it to old value if error
  *  SYNOPSIS
  *    newval=checkSMAValue(newval,oldval,"total_kwh",true);
  *  FUNCTION
  *    there are two error types:
  *      value==SMAError
  *      if it is a growing value: newval < oldval
  *  INPUTS
  *    newval      - the new value
  *    oldval      - the previous value
  *    name        - name of the value for the debug message
  *    growing     - if true check if value has decreased
  *  RESULT
  *    ---
   ******
*/

unsigned long checkSMAValue(unsigned long newval, const unsigned long oldval, const char *name, const bool growing){
  if(growing&(newval<oldval)|| newval==SMA_ERROR){
    util::msgln("%s error. Reuse last value");
    newval=oldval;
  }
  return newval;
}

void smaLoop(void) {
    total_kwh = 0;
    day_kwh = 0;
    spot_ac = 0;
    time = 0;
    bool status = false;
    unsigned char retries = MAX_RETRIES;
    static bool connected=false;
    int bt_init_mode; // start with init all

    /*
     * Try and talk to the inverter.
     */
    while (!status && retries-- ) {
      util::msgln("Enter get_status_loop");
      util::msgln("status= %#x",status);
      util::msgln("connected=%d retries= %d",(int)connected,retries+1);
        delay(5000);

        // btbee_power(HIGH);
        if (connected || bt_init()) {
	  lcd.printConnected();
	  status = bt_get_status(&total_kwh, &day_kwh, &spot_ac, &time);
	    if(status){
	      util::println("*****");
	      util::printfln("total KWH: %s",dtostrf(total_kwh/1000.0,7,3,fbuf));
	      util::printfln("today KWH: %s",dtostrf(day_kwh/1000.0,7,3,fbuf));
	      util::printfln("current KW: %s",dtostrf(spot_ac/1000.0,7,3,fbuf));
	      printTime(time); // print formatted time;
	      util::println("*****");
	      total_kwh=checkSMAValue(total_kwh,last_total_kwh,"total_kwh",true);
	      day_kwh=checkSMAValue(day_kwh,last_day_kwh,"day_kwh",false);
	      spot_ac=checkSMAValue(spot_ac,last_spot_ac,"spot_ac",false);
	      
	      lcd.printTotal(total_kwh);
	      lcd.printDay(day_kwh);
	      lcd.printCurrent(spot_ac);
	      connected=true;
	    }else{
	      lcd.printConnected();
	      util::println("(E) smaLoop get_status has failed");
	      connected=false; // try to reconnect
	    }
        }
	util::msgln("connected=%d",(int)connected);
        // btbee_power(LOW);
    }

    /*
     * It looks like the SMA inverter returns the date and
     * time of the last non-zero data read, so if we haven't
     * gotten any fresh data it's quite probable that the
     * sun has gone down (need to see the SMA specs to be sure).
     * Change to checking with the inverter once per hour to
     * save power, and we'll eventually wake up with some
     * new data.
     */
    if (status) {
        if (total_kwh == last_total_kwh &&
	    day_kwh == last_day_kwh &&
            spot_ac == last_spot_ac &&
            time == last_time) {
	    util::msgln("Inverter values unchanged since last call. Sun has probably set.");
        } else {
	  last_total_kwh = total_kwh;
	  last_day_kwh = day_kwh;
	  last_spot_ac = spot_ac;
	  last_time = time;
	  util::msgln("Inverter values have changed since last call. Sun is probably still shining.");
	}
    } else {
      util::msgln("smaLoop error: unexpected status = %#x",status);
    }
}

void web(){
  const char *url="www.b-67.de"; // the target URL
  // const char *url="b-67.spdns.de"; // the target URL
  WiFiSocket socket;  
  int retcode;

  // string buffer
  char strbuf[11];  // 2013-11-28\0

  // start socket
  lcd.printStatus('O'); // open socket
  util::print("open client socket ");
  util::println(url);
  socket.socket=wifi.openSocket(0,0,url,80); // last number is port
  if(socket.socket<0){
    util::msgln("client socket error=%s",wifi.ec2Str(socket.socket));
    return;
  }else{
    util::println("done open client socket ");
    util::msgln("client socket=%d",socket.socket);
  }

  // check socket and fill socket structure
  lcd.printStatus('G'); // getClientSocket
  retcode=wifi.getClientSocket(socket.socket,socket);
  if(retcode != RESPOK){
    util::msgln("wifi.getClientSocket retcode=%s",wifi.ec2Str(retcode)); // debug
    return;
  }

  // send values
  // http://www.b-67.de/cgi-bin/arduino.pl?clock=20:12:54&p=20&day=20.12.2013&Eday=4325&Etot=54356.324

  // initial command
  lcd.printStatus('H'); // send HTTP command (GET)
  wifi.sendStringMulti(socket,"GET /cgi-bin/arduino.pl?clock=");
  
  // add time
  getTime(last_time,strbuf); // 12:04:58
  wifi.sendStringMulti(socket,strbuf);

  // add date
  wifi.sendStringMulti(socket,"&day=");
  getDate(last_time,strbuf); // 2014-12-28
  wifi.sendStringMulti(socket,strbuf);

  // add power
  wifi.sendStringMulti(socket,"&p=");
  ultoa(spot_ac,strbuf,10); // 32145 [W]
  wifi.sendStringMulti(socket,strbuf);

  // kWh day
  wifi.sendStringMulti(socket,"&Eday=");
  dtostrf(day_kwh/1000.0,0,3,strbuf); // 210.534 [kWh]
  wifi.sendStringMulti(socket,strbuf);

  // kWh total
  wifi.sendStringMulti(socket,"&Etot=");
  ultoa((total_kwh+500)/1000,strbuf,10); // 4321231 [kWh]
  wifi.sendStringMulti(socket,strbuf);

  // rest
  wifi.sendStringMulti(socket," HTTP/1.1\r\nHost: ");
  wifi.sendStringMulti(socket,url);
  wifi.sendStringMulti(socket,"\r\n\r\n");
  wifi.sendStringTerminate(socket);

  // get response
  lcd.printStatus('R'); // get HTTP Response
  for(int i=0; i<5; i++){
    util::println("check for server response");
    util::msgln("check for server response #%d",i+1);
    retcode=wifi.getClientSocket(socket.socket,socket);
    util::msgln("wifi.getClientSocket retcode=%s",wifi.rc2Str(retcode));
    wifi.printSocket(socket);
    util::msgln("retcode=%d socket.size=%d",retcode,socket.size); // debug
    if(retcode==RESPOK && socket.size>0){
      util::msgln("break");
      break;
    }
    delay(3000);
  }
  lcd.printStatus('S'); // receive and print data from server
  util::println("receiving data from server");
  int num=wifi.printResponse(socket);
  if(num>=0){
    util::msgln("socket had %d bytes",num);
  }else{
    util::msgln("wifi.printReponse retcode=%s",wifi.ec2Str(num)); // debug
  }

  // close socket
    lcd.printStatus('E'); // close (end) socket
  retcode=wifi.closeSocket(socket.socket);
  util::msgln("number of sockets closed: %d",retcode);
  util::println("done publish on web");
  return;
}


void loop(void){
  int retCode=0;
  // smaLoop
  long t_rest=tn.rest();
  // util::msgln("remaining time=%ld", t_rest);
  lcd.printCountdown(t_rest);
  if(tn.check()){ // time is over
    util::println("********** new loop **********");
    // smaLoop(); // get new data
    // web(); // publish on the web
    // retCode=url.requestWebPage("b-67.spdns.de","/arduino.html");
    // retCode=url.requestWebPage("www.uhrzeit.org","/atomuhr.php");
    retCode=url.requestWebPage("www.atomzeit.eu","/");
    util::printfln("requestWebPage retCode=%d",retCode);

    // get date and time
    url.resetRespBuf();
    retCode=url.findString("Aktuelle Zeit:",url::removeSearchString);

    WiFiSocket *socket=url.getSocket();
    util::printfln("findString retCode=%d\nBuffer=%s",retCode,url.getBuf());
    
    int day,month,year,h,m;
    sscanf(url.getBuf(),"%d.%d.%d%*s%d:%d",&day,&month,&year,&h,&m);
    util::printfln("date: %d.%d.%d time: %d:%d",day,month,year,h,m);

    // get sunrise
    retCode=url.findString("<b>Sonnenaufgang:</b> ",url::removeSearchString);
    sscanf(url.getBuf(),"%d:%d",&h,&m);

    util::printfln("findString retCode=%d\nBuffer=%s",retCode,url.getBuf());   
    util::printfln("sunrise: %d:%d",h,m);

    retCode=url.findString("Sonnenuntergang:",url::closeAfterFind | url::removeSearchString);
    sscanf(url.getBuf(),"%d:%d",&h,&m);

    util::printfln("findString retCode=%d\nBuffer=%s",retCode,url.getBuf());   
    util::printfln("sunset: %d:%d",h,m);

    util::println("waiting ...");
  }
  lcd.printStatus('W'); // wait for countdown
  t1.sleep();
}

void setupMaster()
{
  util::println("Switching to master");
  bt_send("STWMOD=1");//set the bluetooth work in master mode
  bt_send("STNA=SeeedBTMaster");//set the bluetooth name as "SeeedBTMaster"
  bt_send("STAUTO=0");// Auto-connection is forbidden here
  delay(2000); // This delay is required.
  Serial3.flush();
  util::println("Bluetooth master is inquiring!");
  delay(2000); // This delay is required.
}

