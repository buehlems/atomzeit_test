/*

sma_web

Latest version found at https://github.com/buehlems/sma_web
Licence
Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
http://creativecommons.org/licenses/by-nc-sa/3.0/

You are free:
    to Share - to copy, distribute and transmit the work
    to Remix - to adapt the work
Under the following conditions:
    Attribution - You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
    Noncommercial - You may not use this work for commercial purposes.
    Share Alike - If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.

Work is copyright Markus Buehler, (c)2014.
*/


#ifndef WIFI_H
#define WIFI_H

// #define DEBUG_SEND

// WIFI error rates: 115200 - 1/3k bytes,   57600 - 1/300kbytes, 38400 - 0/1Mbytes
#define WIFI_BAUD_RATE     38400

const int RESPOK=0; 
const int RESPMSG=200; // only marker for begin of response messages (not errors)
const int RESPSOCKETEND=-201; // end of socket answer
const int RESPERROR=-220; // marker for begin of response errors
const int RESPCOMPLETE=-221;
const int RESPTIMEOUT=-222;
const int RESPOVERFLOW=-223;
const int RESPERR=-224;
const int RESPCANTSEND=-225; // can't send all the data
const int RESPINVALID=-235;

// WiFi error codes
// codes defined by WiFi
const int COMMAND_ERROR=-1;
const int NOT_SUPPORTED=-2;
const int OPERATION_ERROR=-3;
const int PARAMETER_ERROR=-4;
const int NOT_PERMITTED=-5;
const int MEMORY_ERROR=-6;
const int FLASH_ERROR=-7;
const int JOIN_FAILED=-10;
const int NO_SOCKET_AVAILABLE=-11;
const int SOCKET_INACTIVE=-12;
const int SOCKET_CONNECTION_FAILED=-13;
const int UNDEFINED_ERROR=-100;
// own error codes
const int NONUM_ERROR=-32768;
const int STARTCOMMAND_FAILED=-102;

// operation modes (bitwise defined)
const int WiFiNoFlush=0; // don't flush WiFi module if overflow
const int WiFiFlush=0x01; // flush WiFi module if overflow
const int WiFiGetServerSocketResponse=0x02; // special mode for getting server socket response (only 1 \r\n)
const int WiFiReadRaw=0x04; // read without any data evaluation

struct _WiFiSocket {
  byte mother; // the number of the mother socket
  byte socket; // the number of this socket
  byte status;
  byte host[4];
  unsigned int port;
  unsigned int size;
};

typedef struct _WiFiSocket WiFiSocket;

class WiFi {
 public:
  WiFi(unsigned int to=1000);
  ~WiFi(void);

  // basic functions
  int available() {return Serial2.available();}
  int read()  { return Serial2.read();}
  size_t print(const char *str) {return Serial2.print(str);}
  size_t println(const char *str) {return Serial2.println(str);}
  size_t print(const int i) {return Serial2.print(i);}
  size_t println(const int i) {return Serial2.println(i);}
  size_t write(const char c);
  void flush() {Serial2.flush();}

  // at functions
  void cmd(const char *str);
  void cmd(const char *str,const int n);
  void cmd(const char *str,const int n1, const int n2);
  void cmd(const char *str,const int n1, const int n2, const int n3);
  void cmd(const char *str,const int n1, const int n2, const int n3, const int n4);
  void cmd(const char *str1,const int n1, const int n2, const char *str2, const int n4);

  bool startCommandMode(void);
  int getResponse(char *buffer, size_t length, int mode=WiFiFlush);
  int printResponse();
  char *rc2Str(const int rc); // return codes to string
  char *ec2Str(const int rc); // error codes to string
  int getNumber(const char *buffer,const char **nextbuffer=NULL);
  int scanSockets(int n1=-1, int n2=-1);
  int closeSockets(int n1=-1, int n2=-1);
  int closeSocket(byte s);
  int closeSocket(WiFiSocket &s);
  int openSocket(int protocol, int mode, int timeout, int port);
  int openSocket(int protocol, int mode, const char *server, int port);
  void printEc(char *buffer);
  int init(int n1=2, int n2=-1); // switch to command mode and close sockets
  int getServerSocket(byte socket, WiFiSocket &response); // get the next Client of this socket
  int getClientSocket(byte socket, WiFiSocket &response); // get the next Client of this socket
  bool isMotherSocket(WiFiSocket &socket) { return socket.mother==socket.socket ; } // check if this is mother socket
  void printSocket(WiFiSocket *socket);
  void printSocket(WiFiSocket &socket);
  int flushIn(); // flush the WiFi
  int receiveDataPrep(WiFiSocket &socket,int size);
  int receiveDataPrep(WiFiSocket &socket);
  int receiveData(WiFiSocket &socket, char *buffer, size_t bufsize, int mode=WiFiFlush);
  int getData(WiFiSocket &socket,  char *buffer, const size_t bufsize, const int mode=WiFiFlush );
  int sendData(const WiFiSocket &socket, const char *d1, const size_t n1, const char *d2=NULL, const size_t n2=0);
  int sendString(const WiFiSocket &socket, const char *string);
  int sendStringMulti(const WiFiSocket &socket, const char *string);
  int sendStringTerminate(const WiFiSocket &socket);
  int printResponse(WiFiSocket *socket);
  int printResponse(WiFiSocket &socket);
 

 private:
  const static size_t bufsize=64;
  const static int sendMod=6; // for some reason the wifi sends only multiples of 6 bytes.
  char sendBuf[sendMod]; // buffer for not sent characters because the number was not a multiple of 6
  void sendBufReset() { sendBuf[0] = '\0' ; }
  unsigned int timeout;

  const char *searchNumber(const char *buffer);
  int evalResponse(const char *buffer);
  int getResponseRaw(char *buffer, size_t length, int mode=WiFiFlush);
  int evalSocket(byte motherSocket, const char *buffer, WiFiSocket &response);
  int getSocket(byte socket, const int mode, WiFiSocket &response); // get the next sub socket of this socket

  #ifdef DEBUG_SEND
  const static size_t debugbuf_size=128;
  char debugbuf[debugbuf_size];
  char *debugpointer;
  void debugbufReset() {debugpointer=debugbuf; }
  void debugbufSetEnd() {*debugpointer = '\0' ; }
  bool add2debugbuf(const char c);
  
  #endif // DEBUG_SEND
 };

#endif /* WIFI_H */
