#ifndef URL_H
#define URL_H

#include "WiFi.h"


class Url {
 public:
  const static int ok=            0;
  const static int serverError=  -1; // error during open socket
  const static int noServerResponse= -2; // could connect to server, but no valid answer to GET command
  const static int stringNotFound= -3;   // could not find string in server response

  const static int closeAfterFind       = 0x01;
  const static int removeSearchString   = 0x02;
  const static int resetBuf             = 0x04;

  Url(WiFi *wifi);
  ~Url(void);
  int requestWebPage(const char *server, const char* page=NULL);
  int findString(const char *sstr, int mode=0);
  WiFiSocket *getSocket() { return &socket; }
  int closeConnection();
  void resetRespBuf();
  char *getBuf() { return respBuf; }




 private:
  const static int responseRetries=10; // how many retries for server response?
  const static int responseWait=1000; // how many retries for server response?

  WiFi *wifi;
  WiFiSocket socket;
  const static int respBufSize=32; // size of the buffer to store (part of) the response
  char respBuf[respBufSize+1]; // one byte for \0

  void invalidateSocket(); // invalidates the socket structure
  int matchString(const char *buf, const char *matchstr, char **begin=NULL);
  int waitForServerResponse(const int retries=responseRetries, const int waitDelay=responseWait);
  int findString_int(const char *sstr, int mode=0); 
  
 };

#endif /* URL_H */
