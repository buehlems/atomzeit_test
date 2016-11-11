#include <Ticker.h>

#include "WiFi.h"
#include "util.h"

// states for get response
#define WAITPLUS 0
#define WAITRSP 1
#define WAITEND 2
  // wifi.startCommandMode();

/****f* 
  *  NAME
  *    WiFi -- 
  *  SYNOPSIS
  *    WiFi wifi(200);
  *  FUNCTION
  *    define WiFi
  *  INPUTS
  *    to      - timeout in ms (default 1000)
  *  RESULT
  *    ---
   ******
*/
WiFi::WiFi(unsigned int to) {
  timeout=to;
  sendBufReset();
#ifdef DEBUG_SEND
  debugbufReset();
#endif // DEBUG_SEND
}

WiFi::~WiFi(void) {
}

size_t WiFi::write(const char c){
#ifdef DEBUG_SEND
  add2debugbuf(c);
#endif
  return Serial2.write(c);
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skstt=1");
  *  FUNCTION
  *    
  *  INPUTS
  *    str      - complete command
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str){
  print("at+");
  println(str);
  #ifdef DEBUG_SEND
  util::printfln("at+%s",str); // debug
  #endif
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skstt",1);
  *  FUNCTION
  *    sends at+skstt=1
  *  INPUTS
  *    str      - WiFi command
  *    n        - number
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str,const int n){
  print("at+");
  print(str);
  print("=");
  println(n);
#ifdef DEBUG_SEND
  util::printfln("at+%s=%d",str,n); // debug
#endif
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skrcv",1,100);
  *  FUNCTION
  *    sends at+skrcv=1,100
  *  INPUTS
  *    str      - WiFi command
  *    n1       - number 1
  *    n2       - number 2
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str,const int n1, const int n2){
  print("at+");
  print(str);
  print("=");
  print(n1);
  print(",");
  println(n2);
  #ifdef DEBUG_SEND
  util::printfln("at+%s=%d,%d",str,n1,n2); // debug
  #endif
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skrcv",1,100);
  *  FUNCTION
  *    sends at+skrcv=1,100
  *  INPUTS
  *    str      - WiFi command
  *    n1       - number 1
  *    n2       - number 2
  *    n3       - number 3
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str,const int n1, const int n2 , const int n3){
  print("at+");
  print(str);
  print("=");
  print(n1);
  print(",");
  print(n2);
  print(",");
  println(n3);

#ifdef DEBUG_SEND
  util::printfln("at+%s=%d,%d,%d",str,n1,n2,n3);
#endif
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skct",protocol,mode,timeout,port);
  *  FUNCTION
  *    sends skct command
  *  INPUTS
  *    str      - WiFi command
  *    n1       - number 1
  *    n2       - number 2
  *    n3       - number 3
  *    n4       - number 4
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str,const int n1, const int n2, const int n3, const int n4){
  print("at+");
  print(str);
  print("=");
  print(n1);
  print(",");
  print(n2);
  print(",");
  print(n3);
  print(",");
  println(n4);
#ifdef DEBUG_SEND
  util::printfln("at+%s=%d,%d,%d,%d",str,n1,n2,n3,n4);
#endif
}

/****f* 
  *  NAME
  *    cmd -- send cmd to wifi prepending at+
  *  SYNOPSIS
  *    cmd("skct",protocol,mode,timeout,port);
  *  FUNCTION
  *    sends skct command
  *  INPUTS
  *    str      - WiFi command
  *    n1       - number 1
  *    n2       - number 2
  *    n3       - number 3
  *    n4       - number 4
  *  RESULT
  *    ---
   ******
*/
void WiFi::cmd(const char *str1,const int n1, const int n2, const char *str2, const int n4){
  print("at+");
  print(str1);
  print("=");
  print(n1);
  print(",");
  print(n2);
  print(",");
  print(str2);
  print(",");
  println(n4);
#ifdef DEBUG_SEND
  util::printfln("at+%s=%d,%d,%s,%d",str1,n1,n2,str2,n4);
#endif
}


/****f* 
  *  NAME
  *    startCommandMode -- starts the command mode
  *  SYNOPSIS
  *    result=wifi.startCommandMode()
  *  FUNCTION
  *    checks if command mode is active. If not starts the command mode
  *  INPUTS
  *  RESULT
  *    true if success
   ******
*/
bool WiFi::startCommandMode(void){
  // test mode
  char buffer[16]; //+ERR=-100\n\r\n\r
  int retcode=-1;

  util::msgln("test mode");
  // test mode
  cmd(""); // blank command
  retcode=getResponse(buffer,16);
  if(retcode==RESPOK){
    util::msgln("in command mode");
    return true;
  }

  util::msgln("response=%s message=%s",rc2Str(retcode),buffer);
  util::println("enter command mode");
  delay(2000);
  for(int i=0; i<3; i++){
    write('+');
  }
  delay(600);
  retcode=getResponse(buffer,16);
  // util::msgln("response=%s message=%s",rc2Str(retcode),buffer); 
  if(retcode==RESPOK){
    return true;
  }else{
    return false;
  }
}

/****f* 
  *  NAME
  *    flushIn -- flush the input buffer of the WiFi
  *  SYNOPSIS
  *    n = wifi.flushIn();
  *  FUNCTION
  *    get the raw response from the WiFi interface 
  *    (including +OK/ERR and end marker)
  *  INPUTS
    *  RESULT
  *    number of bytes flushed 
   ******
*/

int WiFi::flushIn(){
  int n=0;
  char c;
  if(available() > 0){
    util::msg("WiFi::flush ");
  }
  while(available() > 0){
    c=read();
    util::debug(c); // print 
    n++;
  }
  if(n>0){
    util::msgln("");
  }
  return(n);
}


/****f* 
  *  NAME
  *    getResponseRaw -- get the raw response from the WiFi interface
  *  SYNOPSIS
  *    status = getResponseRaw(buffer, 12);
  *  FUNCTION
  *    get the raw response from the WiFi interface 
  *    (including +OK/ERR and end marker)
  *  INPUTS
  *    buffer      - buffer for the response. 
  *                  Make sure that it is large enough to include header and trailer and a 0x00 at the end
  *    length      - Buffer size
  *    mode        - default: WiFiFlush
  *  RESULT
  *    >= 0: number of bytes in buffer (only possible in raw mode)
  *    RESPCOMPLETE: complete message in buffer (ended with 0x00), not in raw mode.
  *    RESPOVERFLOW: buffer overflow with or without flush
  *    RESTIMEOUT: timeout. Message read until timeout is in buffer, ended with 0x00
 
   ******
*/

int WiFi::getResponseRaw(char *buffer,size_t buflength,int mode){
  Ticker t((long)timeout);
  const char normalResponseEnd[]="\r\n\r\n";
  const char *socketResponseEnd=normalResponseEnd+2;
  char c;
  size_t ovn=0; // debug

  int state=WAITPLUS;

  const char *responseEnd = mode&WiFiGetServerSocketResponse ? socketResponseEnd:normalResponseEnd;

  size_t n=0; // current length of response
  const char *endPointer=responseEnd; // pointer on responseEnd
  while(!t.timeOut()){
    if(available() > 0){
      c=read();
      n++;
      t.begin(); // restart time
      // Serial.write(c); // debug
      *buffer++ = c;
      buflength--;
      // check for end
      if(mode&WiFiReadRaw){
	if(buflength<=0){
	  if(mode&WiFiFlush){
	    ovn=flushIn();
	  }
	  return(n); // in raw mode return number of bytes in buffer
	}
      }else{
	// check for end of message
	if(c == *endPointer){
	  endPointer++; // point to next character of responseEnd
	  // response complete?
	  if(*endPointer == 0x00){
	    *buffer = 0x00; // make valid string
	    return RESPCOMPLETE;
	  }
	}else{
	  endPointer=(char *)responseEnd; // reset response pointer to start
	}
	// no message end, check if there is still space in buffer for next char
	if(buflength <= 1){ // need one more byte for the 0x00 at the end
	  if(buflength==1)
	    *buffer = 0x00; // mark end if there is still some space
	  // read and discard rest
	  if(mode&WiFiFlush){
	    ovn=flushIn();
	    n+=ovn;
	  }else{
	    util::msgln("overflow without flush"); // debug
	  }
	  return RESPOVERFLOW;
	}
      } //  if(mode&WiFiReadRaw)
    } // if(available > 0)
  } // while(!t.timeOut())
  // time out
  *buffer = 0x00;
  return RESPTIMEOUT;
}

/****f* 
  *  NAME
  *    evalResponse -- evaluate response
  *  SYNOPSIS
  *    status = evalResponse(buffer);
  *  FUNCTION
  *    check if response follows basic format: +OK, +ERR=
  *  INPUTS
  *    buffer      - buffer for the response. 
   *  RESULT
  *    RESPOK: message is OK-message (+OK)
  *    RESPERR: message is error message (+ERR)
  *    RESPSOCKETEND: end of socket message (\r\n)
  *    RESPINVALID: no valid message. None of the above
  *    also see Wifi.h for error codes (RESP...)
   ******
*/
int WiFi::evalResponse(const char *buffer){
  if(strncmp(buffer,"+OK",3) == 0){
    return RESPOK;
  }
  if(strncmp(buffer,"+ERR=",5) == 0){
    return RESPERR;
  }
  // end of socket message?
  if(strcmp(buffer,"\r\n")==0){
    return(RESPSOCKETEND);
  }
  return RESPINVALID;
}

/****f* 
  *  NAME
  *    getResponse -- get response and make basic evaluation
  *  SYNOPSIS
  *    status = getResponse(buffer, length);
  *  FUNCTION
  *    get WiFi response and check if it follows basic format: +OK, +ERR=
  *  INPUTS
  *    buffer      - buffer for the response. 
  *    length      - Buffer size
  *    mode       -  bitwise: WiFiFlush, WiFiGetServerSocketResponse, WiFiReadRaw. Default: WiFiFlush
  *  RESULT
  *    >= 0: number of bytes in buffer (only possible in raw mode)
  *    RESPOVERFLOW: buffer overflow with or without flush. No valid message in buffer
  *    RESPTIMEOUT: timeout. No valid message in buffer
  *    RESPOK: message is OK-message (+OK)
  *    RESPERR: message is error message (+ERR)
  *    RESPSOCKETEND: end of socket message (\r\n)
  *    RESPINVALID: no valid message. None of the above
   ******
*/
int WiFi::getResponse(char *buffer, size_t length, int mode){
  int status=getResponseRaw(buffer,length,mode);
  if(status != RESPCOMPLETE || status >= 0){
    return(status);
  }
  status=evalResponse(buffer);
  return status;
}

/****f* 
  *  NAME
  *    printResponse -- print response of WiFito Serial. Mainly for debug
  *  SYNOPSIS
  *    n = printResponse;
  *  FUNCTION
  *    
  *  INPUTS
   *  RESULT
  *    number of characters printed
  ******
*/
int WiFi::printResponse(){

  delay(10); // make sure response has arrived (could use timeout instead)

  int n=available();
  util::msgln("received %d characters",n); 
  while(available() > 0){
    char c=read();
    Serial.write(c);
  }
  return n;
}

/****f* 
  *  NAME
  *    searchNumber -- searches the first number character in a string (-,0-9)
  *  SYNOPSIS
  *    numberstart=searchNumber(str);
  *  FUNCTION
  *    
  *  INPUTS
  *    buffer      - buffer of the string. 
   *  RESULT
  *    pointer to the first number character
  ******
*/
const char *WiFi::searchNumber(const char *buffer){
  while(*buffer != 0 && *buffer!='-' && (*buffer<'0' || *buffer>'9')){
    buffer++;
  }
  return buffer;
}

/****f* 
  *  NAME
  *    getNumber -- returns the first number found in string
  *  SYNOPSIS
  *    int n=getNumber(str,*str);
  *  FUNCTION
  *    
  *  INPUTS
  *    buffer      - buffer of the string.
  *    nextbuffer  - points to next character after the number 
  *  RESULT
  *    (int) number
  *     NONUMERROR if no number found in buffer
  ******
*/
int WiFi::getNumber(const char *buffer,const char **nextbuffer){
  buffer=searchNumber(buffer); // skip leading characters

  // no number given?
  if(*buffer==0){
    return NONUM_ERROR;
  }

  // check sign
  int sign=1;
  if(*buffer=='-'){
    sign=-1;
    buffer++;
  }

  // now calculate the number
  int num=0;
  while(*buffer>='0' && *buffer<='9'){
    num=num*10;
    num+=*buffer-'0';
    buffer++;
  }
  num*=sign;
  if(nextbuffer){
    *nextbuffer=buffer;
  }
  return(num);
}



/****f* 
  *  NAME
  *    rc2Str -- converts a return code from getResponse to a string. Gives some idea about quality of message in buffer.
  *  SYNOPSIS
  *    char buffer[32]=rc2Str(RESCOMPLETE);
  *  FUNCTION
  *    returns "complete message"
  *  INPUTS
  *    rc          - return code
   *  RESULT
  *    message
  ******
*/

char *WiFi::rc2Str(const int rc){
  char *msg=NULL;

  switch(rc) {
  case RESPOK: msg="OK"; break;
  case RESPERROR: msg="general response error"; break;
  case RESPCOMPLETE: msg="complete message"; break;
  case RESPTIMEOUT: msg="timeout"; break;
  case RESPOVERFLOW: msg="buffer overflow"; break;
  case RESPERR: msg="ERR"; break;
  case RESPINVALID: msg="message invalid"; break;
  case RESPSOCKETEND: msg="end of socket"; break;
  default: msg="rc unkown"; 
  }
  return msg;
}

/****f* 
  *  NAME
  *    ec2Str -- converts an error code as defined by the WiFi HW to a string.
  *              Usually you get them from a message like "+ERR=-5"
  *  SYNOPSIS
  *    char buffer[32]=ec2Str(COMMAND_ERROR);
  *  FUNCTION
  *    returns "incorrect command format"
  *  INPUTS
  *    ec          - error code
   *  RESULT
  *    error message
  ******
*/

char *WiFi::ec2Str(const int ec){
  char *msg=NULL;

  if(ec<=RESPERROR){ // error when receiving response
    msg=rc2Str(ec);
  }else{
    switch(ec) { // error in response
    case COMMAND_ERROR           : msg="incorrect command format"; break;
    case NOT_SUPPORTED           : msg="not supported"; break;
    case OPERATION_ERROR         : msg="operation error"; break;
    case PARAMETER_ERROR         : msg="parameter error"; break;
    case NOT_PERMITTED           : msg="not permitted"; break;
    case MEMORY_ERROR            : msg="not enough memory"; break;
    case FLASH_ERROR             : msg="flash error"; break;
    case JOIN_FAILED             : msg="join has failed"; break;
    case NO_SOCKET_AVAILABLE     : msg="no more socket available"; break;
    case SOCKET_INACTIVE         : msg="socket not active"; break;
    case SOCKET_CONNECTION_FAILED: msg="socket connection has failed"; break;
    case UNDEFINED_ERROR         : msg="undefined error"; break;
    case NONUM_ERROR             : msg="no number returned"; break;
    case STARTCOMMAND_FAILED     : msg="starting command mode has failed"; break;
    default                      : msg="error code unkown"; 
    }
  }
  return msg;
}

/****f* 
  *  NAME
  *    scanSockets -- scan sockets and print socket states
  *  SYNOPSIS
  *    activeSockets=scanSockets(1,5);
  *  FUNCTION
  *    prints state of sockets 1<=n<=5
  *    if only one number is given scan from 0...n1
  *  INPUTS
  *    n1 - start socket (default: 0)
  *    n2 - end socket (default: 15)
   *  RESULT
  *    number of active sockets
  ******
*/
int WiFi::scanSockets(int n1, int n2){
  char buffer[bufsize];
  int activeSockets=0;
  // handle defaults
  if(n2==-1){
    if(n1==-1){
      n1=0;
      n2=15;
    }else{
      n2=n1;
      n1=0;
    }
  }
  for(int i=n1; i<=n2; i++){
    cmd("skstt",i); // get socket state
    int retcode=getResponse(buffer,sizeof(buffer));
    // util::msgln("scanSockets: response=%s\nmessage=%s",rc2Str(retcode),buffer);

    if(retcode==RESPOK){
      activeSockets++;
    }
  }
  return activeSockets;
}

/****f* 
  *  NAME
  *    closeSockets -- close sockets
  *  SYNOPSIS
  *    closedSockets=closeSockets(1,5);
  *  FUNCTION
  *    checks and closes sockets 1-5
  *    if only one number is given close from 0-n1
  *  INPUTS
  *    n1 - start socket (default: 0)
  *    n2 - end socket (default: 15)
   *  RESULT
  *    number of closed sockets
  ******
*/
int WiFi::closeSockets(int n1, int n2){
  char buffer[bufsize];
  int activeSockets=0;
  // handle defaults
  if(n2==-1){
    if(n1==-1){
      n1=0;
      n2=15;
    }else{
      n2=n1;
      n1=0;
    }
  }

  // now looping through all selected sockets
  for(int i=n2; i>=n1; i--){
    util::msgln("checking socket %d",i);
    // checking socket
    cmd("skstt",i); // get status
    int retcode=getResponse(buffer,sizeof(buffer));
    // util::msgln("closeSockets check: response=%s\nmessage=%s",rc2Str(retcode),buffer);
    // printEc(buffer);

    // close socket if used
    if(retcode==RESPOK){
      util::msgln("close socket %d",i);
      activeSockets++;
      cmd("skcls",i);
      retcode=getResponse(buffer,sizeof(buffer));
      // util::msgln("closeSockets close: %d, response=%s\nmessage=%s",i,rc2Str(retcode),buffer);
    }else if(retcode==RESPERR){
      int ec=getNumber(buffer);
      if(ec != SOCKET_INACTIVE){
	util::msgln("closeSockets message error: socket %d error=%s",i,ec2Str(ec));
      }
    }else{
      util::msgln("closeSockets response error: socket %d error=%s",i,rc2Str(retcode));
    }
  }
  return activeSockets;
}

/****f* 
  *  NAME
  *    closeSocket -- close socket
  *  SYNOPSIS
  *    n=closeSockets(n);
  *  FUNCTION
  *    checks and closes socket n
  *  INPUTS
  *    n - number of socket to be closed
   *  RESULT
  *    number of closed sockets
  ******
*/
int WiFi::closeSocket(byte n){
  return(closeSockets(n,n));
}

/****f* 
  *  NAME
  *    closeSocket -- close socket
  *  SYNOPSIS
  *    closedSocket=closeSockets(s);
  *  FUNCTION
  *    checks and closes socket s
  *  INPUTS
  *    s - socket to be closed
   *  RESULT
  *    number of closed sockets
  ******
*/
int WiFi::closeSocket(WiFiSocket &s){
  return(closeSocket(s.socket));
}

/****f* 
  *  NAME
  *    printEc -- check wifi response and print error
  *  SYNOPSIS
  *    printEc("+ERR=-1");
  *  FUNCTION
   *    prints "incorrect command format"
  *  INPUTS
  *    buffer - the wifi response
   *  RESULT
  *    ---
  ******
*/
void WiFi::printEc(char *buffer){
  int retcode=evalResponse(buffer);
  if(retcode==RESPERR)
    util::msgln("%s",ec2Str(getNumber(buffer)));
}


/****f* 
  *  NAME
  *    init -- starts command mode and closes active sockets
  *  SYNOPSIS
  *    closedSocket=init(10);
  *  FUNCTION
   *    starts command mode and closes sockets 0-10
  *  INPUTS
  *    n1 - start socket (default: 0)
  *    n2 - end socket (default: 15)
   *  RESULT
  *    number of closed sockets
  ******
*/
int WiFi::init(int n1, int n2){
  const int retries=5; // maximum number of retries to start command mode
  bool commandMode=false;

  Serial2.begin(WIFI_BAUD_RATE); // WiFi

  int i;
  for(i=0; i<10 && !commandMode; i++){
    commandMode=startCommandMode();
    if(commandMode){
      util::println("WiFi::init: start command mode success");
    }else{
      util::println("WiFi::init: start command mode has failed");
      delay(1000);
    }
  }

  if(!commandMode){
      return STARTCOMMAND_FAILED;
  }

  int nsockets=closeSockets(n1,n2);
  util::msgln("init: found and closed %d active sockets",nsockets);
  return nsockets;
}

/****f* 
  *  NAME
  *    openSocket -- opens a server socket
  *  SYNOPSIS
  *    serverSocket=wifi.openSocket(0,1,120,80)
  *  FUNCTION
   *    opens socket socket: TCP, server, timout 120s, port 80
  *  INPUTS
  *    protocol - 0: TCP, 1: UDP
  *    mode - 0: client, 1: server
  *    timeout - timeout in sec.
  *    port - port number
   *  RESULT
  *    number of the socket
  ******
*/
int WiFi::openSocket(int protocol, int mode, int timeout, int port){
  char buffer[bufsize];
  cmd("skct",protocol,mode,timeout,port);
  int retcode=getResponse(buffer,bufsize);
  util::msgln("response=%s message=%s",rc2Str(retcode),buffer); // debug
  int n=getNumber(buffer);
  return n;
}

/****f* 
  *  NAME
  *    openSocket -- opens a client socket
  *  SYNOPSIS
  *    serverSocket=wifi.openSocket(0,0,"www.b-67.de",443)
  *  FUNCTION
   *    opens socket socket: TCP, client,server: www.b-67.de", port 443
  *  INPUTS
  *    protocol - 0: TCP, 1: UDP
  *    mode - 0: client, 1: server
  *    timout/server - 
  *    port - port number
   *  RESULT
  *    number of the socket
  ******
*/
int WiFi::openSocket(int protocol, int mode, const char *addr, int port){
  char buffer[bufsize];

  cmd("skct",protocol,mode,addr,port);
  int retcode=getResponse(buffer,bufsize);
  util::msgln("response=%s message=%s",rc2Str(retcode),buffer); // debug
  util::hexdump((unsigned char *)buffer,strlen(buffer)); // debug
  int n=getNumber(buffer);
  return n;
}

/****f* 
  *  NAME
  *    evalSocket -- evaluates a socket
  *  SYNOPSIS
  *    ec=wifi.evalSocket(buffer,response)
  *  FUNCTION
   *    evaluates the socket response in buffer
  *  INPUTS
  *    buffer - string with socket response
  *    response - result
   *  RESULT
  *    RESPOK: socket is good
  *    <0: Error. Number indicates position in response that has failed
  ******
*/
int WiFi::evalSocket(byte motherSocket, const char *buffer, WiFiSocket &response){

  const char *ptr=buffer; 

  response.mother=motherSocket;
 
  int n=getNumber(ptr,&ptr);
  if(n==NONUM_ERROR) return(-1);
  response.socket=n;

  n=getNumber(ptr,&ptr);
  if(n==NONUM_ERROR) return(-2);
  response.status=n;

  for(int i=0; i<4; i++){
    n=getNumber(ptr,&ptr);
    if(n==NONUM_ERROR) return(-3-i);
    response.host[i]=n;
  }
  
  n=getNumber(ptr,&ptr);
  if(n==NONUM_ERROR) return(-7);
  response.port=n;

  n=getNumber(ptr,&ptr);
  if(n==NONUM_ERROR) return(-8);
  response.size=n;
  return(RESPOK);
}

/****f* 
  *  NAME
  *    getSocket -- gets a (sub) socket
  *  SYNOPSIS
  *    ec=getSocketClient(1,response)
  *  FUNCTION
   *    gets the (next) sub socket in socket 1. If response.n==socket this is the "mother socket". The real clients will follow
  *  INPUTS
  *    socket - "mother socket"
  *    mode - WiFiGetServerSocketResponse or 0 (for client socket)
  *    response - result
  *  RESULT
  *    RESPOK: valid socket found
  *    RESPSOCKETEND: end of socket message (\r\n)
  *    RESPOVERFLOW: buffer overflow with or without flush. No valid message in buffer
  *    RESTIMEOUT: timeout. No valid message in buffer
  *    RESPERR: message is error message (+ERR)
  *    RESPINVALID: no valid message. None of the above
  ******
*/
int WiFi::getSocket(byte socket, const int mode, WiFiSocket &response){
  char buffer[bufsize];

  if(!available()){ // response available?
    // util::msgln("Sending skstt"); // debug
    cmd("skstt",socket); // no, get socket status
  }

  // get the mother socket. Usually something like +OK=1,1,"255.255.255.255",80,0
  int retcode=getResponse(buffer,bufsize,mode);
  util::msgln("getSocket: response=%s message=%s",rc2Str(retcode),buffer);
  util::hexdump((unsigned char *)buffer,strlen(buffer));

  // return if socket end or WiFi responds with error
  if(retcode == RESPOVERFLOW || retcode == RESPTIMEOUT || retcode == RESPERR || retcode == RESPSOCKETEND){
    return retcode;
  }

  // a mother socket msg will look like +OK=1,1,"255.255.255.255",80,0 -> RESPOK
  // a child socket msg will look like 2,2,"37.209.31.218",53773,309 -> RESPINVALID

  retcode=evalSocket(socket,buffer,response);
  return retcode;
}

/****f* 
  *  NAME
  *    getServerSocket -- gets a (sub) socket of a server socket
  *  SYNOPSIS
  *    ec=getSocketClient(1,response)
  *  FUNCTION
   *    gets the (next) sub socket in socket 1. If response.n==socket this is the "mother socket". The real clients will follow
  *  INPUTS
  *    socket - "mother socket"
  *    response - result
  *  RESULT
  *    RESPOK: valid socket found
  *    RESPSOCKETEND: end of socket message (\r\n)
  *    RESPOVERFLOW: buffer overflow with or without flush. No valid message in buffer
  *    RESTIMEOUT: timeout. No valid message in buffer
  *    RESPERR: message is error message (+ERR)
  *    RESPINVALID: no valid message. None of the above
  ******
*/
int WiFi::getServerSocket(byte socket, WiFiSocket &response){
  return getSocket(socket,WiFiGetServerSocketResponse,response);
}

/****f* 
  *  NAME
  *    getClientSocket -- gets a (sub) socket of a client socket
  *  SYNOPSIS
  *    ec=getSocketClient(1,response)
  *  FUNCTION
   *    gets the (next) sub socket in socket 1. If response.n==socket this is the "mother socket". The real clients will follow
  *  INPUTS
  *    socket - "mother socket"
  *    response - result
  *  RESULT
  *    RESPOK: valid socket found
  *    RESPSOCKETEND: end of socket message (\r\n)
  *    RESPOVERFLOW: buffer overflow with or without flush. No valid message in buffer
  *    RESTIMEOUT: timeout. No valid message in buffer
  *    RESPERR: message is error message (+ERR)
  *    RESPINVALID: no valid message. None of the above
  ******
*/
int WiFi::getClientSocket(byte socket, WiFiSocket &response){
  return getSocket(socket,0,response);
}

/****f* 
  *  NAME
  *    printSocket -- prints the socket response in a formated way
  *  SYNOPSIS
  *    printSocketResponse(socket)
  *  FUNCTION
   *    
  *  INPUTS
  *    socket - pointer to the socket
   *  RESULT
  *    n/a
  ******
*/
void WiFi::printSocket(WiFiSocket *socket){
  util::msgln("printSocket");
  if(socket->mother == socket->socket){
      util::msgln("mother socket");
    }else{
      util::msgln("child socket");
  }
  util::msgln("mother: %3.3d",socket->mother);
  util::msgln("socket: %3.3d",socket->socket);
  util::msg("status: ");
  switch(socket->status) {
  case 0           : util::msgln("disconnected"); break;
  case 1           : util::msgln("detecting"); break;
  case 2           : util::msgln("connected"); break;
  default          : util::msgln("unknown %d",socket->status); break;
  }
  util::msg("host:");
  for(int i=0; i<4; i++){
    util::msg(" %3.3d",socket->host[i]);
  }
  util::msgln();
  util::msgln("port: %u",socket->port);
  util::msgln("size: %u",socket->size);
}

/****f* 
  *  NAME
  *    printSocket -- prints the socket response in a formated way
  *  SYNOPSIS
  *    printSocketResponse(socket)
  *  FUNCTION
   *    
  *  INPUTS
  *    socket - the socket
   *  RESULT
  *    n/a
  ******
*/
void WiFi::printSocket(WiFiSocket &socket){
  printSocket(&socket);
}

/****f* 
  *  NAME
  *    receiveDataPrep -- prepare data receive
  *  SYNOPSIS
  *    n=receiveDataPrep(socket, 32)
  *  FUNCTION
   *    sends the receive command and evaluates response. If successful data can be received after command finishes
  *    siez - size of the buffer to store the data (<= 512)

  *  INPUTS
  *    socket - the socket
  *    bufsize - size of the buffer to store the data (<= 512)
   *  RESULT
  *    >=0: number of bytes to be received
  *    <0: error code. See getResponse for details
  ******
*/
int WiFi::receiveDataPrep(WiFiSocket &socket, const int size){
  char buffer[bufsize];

  cmd("skrcv",socket.socket,size); // receive data
 
  int retcode=getResponse(buffer,bufsize);

  // util::msgln("receiveDataPrep: response=%s\nmessage=%s",wifi.rc2Str(retcode),buffer); // debug
  // util::hexdump((unsigned char *)buffer,strlen(buffer)); // debug

  if(retcode == RESPERR || retcode == RESPOK){
    retcode=getNumber(buffer); // return the error code
    if(retcode<0){
      	util::msgln("receiveDataPrep error: socket %d error=%s",socket.socket,ec2Str(retcode));
    }
  }
  return (retcode);
}

/****f* 
  *  NAME
  *    receiveDataPrep -- prepare data receive. Use size as defined in socket
  *  SYNOPSIS
  *    n=receiveDataPrep(socket)
  *  FUNCTION
   *    sends the receive command and evaluates response. If successful data can be received after command finishes
  *  INPUTS
  *    socket - the socket
   *  RESULT
  *    >=0: number of bytes to be received
  *    <0: error code. See getResponse for details
  ******
*/
int WiFi::receiveDataPrep(WiFiSocket &socket){

  int size=socket.size;
  if(size>512)
    size=512;

  return receiveDataPrep(socket,size);
}

/****f* 
  *  NAME
  *    receiveData -- receive data
  *  SYNOPSIS
  *    n=receiveData(const WiFiSocket &socket,&buffer,bsize,0)
  *  FUNCTION
   *    reads data from socket. Stops after reading a max of bufsize bytes. Updates socket.size
   *    receiceDataPrep must be run before
  *  INPUTS
  *    socket - the socket
  *    buffer - a buffer to store the data
  *    bufsize - size of the buffer
  *    mode - WiFiFlush or WiFiNoFlush
   *  RESULT
  *    >=0: number of bytes in buffer
  *    <0: error code. See getResponseRaw for details
  ******
*/

int WiFi::receiveData(WiFiSocket &socket, char *buffer, const size_t bufsize, const int mode){
  int bytes2read = socket.size<bufsize ? socket.size : bufsize;
  int mode_int = mode | WiFiReadRaw;
  int ret=getResponseRaw(buffer,bytes2read,mode_int);
  if(ret==RESPTIMEOUT){
    util::msgln("WiFi::receiveData2Buf: time out");
  }
  if(ret >0) 
    socket.size -= bytes2read;
  return ret;
}

/****f* 
  *  NAME
  *    getData -- get data from a socket
  *  SYNOPSIS
  *    n=getData(socket,buf,128,WiFiRaw)
  *  FUNCTION
   *    receives data from a socket and stores it in the buffer. Updates the socket size accordingly
  *  INPUTS
  *    socket -  the socket
  *    buf    -  the buffer to store the data. Will be 0-terminated
  *    bufsize - the max. size of the buffer including the '\0'
  *    mode    - the mode (default: WiFiFlush)
  *  RESULT
  *    >0: number of bytes received
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::getData(WiFiSocket &socket,  char *buffer, const size_t bufsize, const int mode ){
  int bytesReceived=0;
  int n=socket.size<bufsize ? socket.size:bufsize;
  if(n==0){
    // nothing to read
    return(0);
  }

  n=receiveDataPrep(socket,n);
  // util::msgln("size: %d receive: %d\n",socket.size<bufsize ? socket.size:bufsize,n); // debug
  if(n < 0) 
    return n;
  bytesReceived=receiveData(socket,buffer,n,mode);
  if(bytesReceived<0){
    return bytesReceived;
    util::ascdump(buffer,n);
  }
  return bytesReceived;
}


/****f* 
  *  NAME
  *    sendData -- send data r
  *  SYNOPSIS
  *    n=sendData(socket, buffer, n)
  *  FUNCTION
   *    sends sends n bytes to socket
  *  INPUTS
  *    socket - the socket
  *    d1 - buffer1 with the data
  *    n1 - number of bytes
  *    d2 - buffer2 with the data
  *    n2 - number of bytes
  *  RESULT
  *    RESPOK: data successfully sent
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::sendData(const WiFiSocket &socket, const char *d1, const size_t n1, const char *d2, const size_t n2){

  char buffer[bufsize];
  int n=n1+n2; // total number of real data to send

  // for some reason the wifi wants multiples of 6 bytes.
  int nf=(sendMod-n%sendMod)%sendMod; // will make 6 to 0
  int nall=n+nf; // all bytes to send
  util::msgln("sendData: n1=%d n2=%d n=%d fill=%d total=%d", n1, n2, n, nf,nall); // debug

  int retcode;
  int ec; // error code or number of bytes

    cmd("sksnd",socket.socket,nall); // send data
    retcode=getResponse(buffer,bufsize);

    // util::msgln("sendData: response=%s message=%s",rc2Str(retcode),buffer); // debug
    // util::hexdump((unsigned char *)buffer,strlen(buffer)); // debug

    if(retcode==RESPTIMEOUT || retcode==RESPOVERFLOW)
      return(retcode);
  
    ec=getNumber(buffer); // return the error code or the number of bytes to be sent
  
    if(retcode == RESPERR){
      util::msgln("sendData error: socket %d error=%s",socket.socket,ec2Str(ec));
      return retcode;
    }

  // retcode should now be RESPOK
  if(ec!=nall){
    util::msgln("sendData error: cannot send all the data. Requested: %d allowed: %d",n,ec);
    return RESPCANTSEND;
  }

  if(n1>0) util::msg("sending data1:"); // debug
  // ready to send data of buffer 1
  for(int i=0; i<n1; i++){
    write(d1[i]);
    util::msg(d1[i]); // debug
  }
  if(n1>0) util::msgln(""); // debug

  if(n2>0) util::msg("sending data2:"); // debug
  // ready to send data of buffer 2
  for(int i=0; i<n2; i++){
    write(d2[i]);
    util::msg(d2[i]); // debug
  }

  // send fill bytes
  if(nf>0) util::msgln("sending %d fill bytes",nf); // debug
  for(int i=0; i<nf; i++){
    write(' ');
  }
  
  util::msgln(); // debug
  return (retcode);
}

/****f* 
  *  NAME
  *    sendString -- send string
  *  SYNOPSIS
  *    n=sendData(socket, str)
  *  FUNCTION
   *    sends sends string to socket
  *  INPUTS
  *    socket - the socket
  *    string - the string
  *  RESULT
  *    RESPOK: data successfully sent
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::sendString(const WiFiSocket &socket, const char *string){
  return(sendData(socket,string,strlen(string)));
}

/****f* 
  *  NAME
  *    sendStringMulti -- send string with multiple send commands
  *  SYNOPSIS
  *    n=sendStringMulti(socket, str)
  *  FUNCTION
   *    sends sends string to socket in packets of n*6 bytes. 
   *    Requires sendStringTerminate to flush the remaining bytes.
  *  INPUTS
  *    socket - the socket
  *    string - the string
  *  RESULT
  *    RESPOK: data successfully sent
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::sendStringMulti(const WiFiSocket &socket, const char *string){
  int n1=strlen(sendBuf);
  int n2=strlen(string);
  int n=n1+n2; // total length
  int n3; // the part of string to be sent
  int rest; // the part of string not sent
  int retcode=RESPOK;

  util::msgln("*****");
  util::msgln("sendStringMulti: n1=%d n2=%d total=%d",n1,n2,n);
  if(n1>0) util::hexdump(sendBuf,n1); 
  if(n2>0) util::hexdump(string,n2); 

  // enough bytes to send?
  if(n1+n2>=sendMod){
    // calculate the real number to send
    rest=n%sendMod; // bytes to send next time
    n3=n2-rest; // bytes to send this time
    util::msgln("sending %d out of %d characters",n1+n3,n);
    retcode=sendData(socket,sendBuf,n1,string,n3);
    // copy rest of string to send buffer
    strcpy(sendBuf,string+n3);
  }else{
    // append string to send buffer
    util::msgln("do NOT send %d characters",n);
    strcpy(sendBuf+n1,string);
  }
  util::msgln("remaining bytes for next sendMulti: %d",strlen(sendBuf)); // debug
  return retcode;
}

/****f* 
  *  NAME
  *    sendStringTerminate -- send the last characters of the multi string
  *  SYNOPSIS
  *    n=sendStringTerminate(socket)
  *  FUNCTION
   *    sends the remaining characters of a multi string.
  *  INPUTS
  *    socket - the socket
  *  RESULT
  *    RESPOK: data successfully sent
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::sendStringTerminate(const WiFiSocket &socket){
  int n1=strlen(sendBuf);
  int retcode=RESPOK;

  util::msgln("*****");
  util::msgln("sendStringTerminate: send final string l=%d",n1);
  util::hexdump(sendBuf,n1); // debug

  if(n1>0){
    retcode=sendData(socket,sendBuf,n1);
  }
  sendBufReset();

  #ifdef DEBUG_SEND
  debugbufSetEnd();
  util::msg("sendStringTerminate: string sent:");
  util::msgln(debugbuf);
  util::hexdump(debugbuf,strlen(debugbuf));
  debugbufReset();
  #endif
  util::msgln(""); // debug

  return retcode;
}


/****f* 
  *  NAME
  *    printResponse -- print the response in the WifiBuffer
  *  SYNOPSIS
  *    n=printResponse(socket)
  *  FUNCTION
   *    receives data from a socket and prints it immediatly
  *  INPUTS
  *    socket - the socket
  *  RESULT
  *    <0: number of bytes printed
  *    RESPERR: error when sending command (+ERR)
  *    RESPINVALID: no valid message. None of the above
  *    RESPCANTSEND: WiFi does not allow to send all data at once. Nothing sent.
  *    also see Wifi.h for error codes (RESP...)
   *    <0: error code. See getResponse for details
  ******
*/
int WiFi::printResponse(WiFiSocket *socket){
  char buffer[bufsize];
  int bytesReceived=0;
  int retCode=0;
  while(socket->size>0){
    int n=socket->size<bufsize ? socket->size:bufsize;
    n=receiveDataPrep(*socket,n);
    // util::msgln("size: %d receive: %d\n",socket->size<bufsize ? socket->size:bufsize,n); // debug
    if(n < 0) 
      return n;
    retCode=receiveData(*socket,buffer,n,WiFiReadRaw);
    if(retCode<0)
      return retCode;
    bytesReceived += n;
    util::ascdump(buffer,n);
  }
  return bytesReceived;

}

/****f* 
  *  NAME
  *    printResponse -- prints the socket response in a formated way
  *  SYNOPSIS
  *    printResponse(socket)
  *  FUNCTION
   *    
  *  INPUTS
  *    socket - the socket
   *  RESULT
  *    n/a
  ******
*/
int WiFi::printResponse(WiFiSocket &socket){
  printResponse(&socket);
}


#ifdef DEBUG_SEND
bool WiFi::add2debugbuf(const char c){
  if(debugpointer<debugbuf+debugbuf_size-1){ // need one char for \0
    *debugpointer++=c;
    return true;
  }else{
    *(debugpointer-1)='|'; // mark overflow
    return false;
  }
}
#endif // DEBUG_SEND
