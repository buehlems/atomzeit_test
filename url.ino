#include <string.h>
#include "url.h"
#include "util.h"


/****f* 
  *  NAME
  *    Url -- 
  *  SYNOPSIS
  *   Url url(wifi);
  *  FUNCTION
  *    define url
  *  INPUTS
  *    wifi      - a wifi
  *  RESULT
  *    ---
   ******
*/
Url::Url(WiFi *wifi) {
  this->wifi = wifi;
  resetRespBuf();
  invalidateSocket();
}

Url::~Url(void) {
}




/****f* 
  *  NAME
  *    resetRespBuf -- empties the response buffer
  *  SYNOPSIS
  *    resetRespBuf();
  *  FUNCTION
  *    
  *  INPUTS
  *    ---
  *  RESULT
  *    ---
   ******
*/
void Url::resetRespBuf(){
  respBuf[0] = '\0';
}

/****f* 
  *  NAME
  *    invalidateSocket -- invalidates the socket -> no active connection
  *  SYNOPSIS
  *    invalidateSocket();
  *  FUNCTION
  *    sets the host to 0.0.0.0
  *  INPUTS
  *    ---
  *  RESULT
  *    ---
   ******
*/
void Url::invalidateSocket(){
  for(int i=0; i<sizeof(socket.host); i++)
    socket.host[0]=0;
}

/****f* 
  *  NAME
  *    closeConnection -- closes the connection
  *  SYNOPSIS
  *    n=closeConnecion()
  *  FUNCTION
  *    closes the socket to this server
  *  INPUTS
  *     ---
  *  RESULT
  *    number of sockets closed
   ******
*/
int Url::closeConnection(){
    int retcode=wifi->closeSocket(socket.socket);
    invalidateSocket();
    util::msgln("number of sockets closed: %d",retcode);
    return retcode;
}


/****f* 
  *  NAME
  *    waitForServerResponse -- wait until the server responds
  *  SYNOPSIS
  *    int retCode=waitForServerResponse(retries,waitdelay)
  *  FUNCTION
  *    waits for the server response
  *  INPUTS
  *    retries  - number of retries
  *    waitdelay - time to wait between retries (in ms)
  *  RESULT
  *    error code
   ******
*/
int Url::waitForServerResponse(const int retries, const int waitDelay){
  boolean gotResponse=false;
  int retcode;

  // get server response
  for(int i=0; i<retries; i++){
    util::msgln("check for server response #%d",i+1);

    retcode=wifi->getClientSocket(socket.socket,socket); // check socket and fill socket structure

    util::msgln("wifi->getClientSocket retcode=%s",wifi->rc2Str(retcode));
    wifi->printSocket(socket);
 
   if(retcode==RESPOK && socket.size>0){
      util::printfln("requestWebPage ok");
      gotResponse=true;
      break;
    }
    delay(waitDelay);
  }

  if(!gotResponse){
    wifi->printResponse(socket);
    closeConnection();
    return noServerResponse;
  }
  return(ok);
}


/****f* 
  *  NAME
  *    requestWebPage -- requests a web page
  *  SYNOPSIS
  *    int retCode=requestWebPage("http://www.web-calendar.org","de/world/europe/germany/weil-im-schonbuch--01?menu=sun")
  *  FUNCTION
  *    opens a socket to a server and requests a web page.
  *  INPUTS
  *    server   -  name or IP of the server
  *    page     -  path to the page
  *  RESULT
  *    error code
   ******
*/
int Url::requestWebPage(const char *server, const char* page){
  boolean gotResponse=false;

  util::msgln("open client socket %s",server);
  socket.socket=wifi->openSocket(0,0,server,80); // last number is port
  if(socket.socket<0){
    util::msgln("client socket error=%s",wifi->ec2Str(socket.socket));
    return serverError;
  }else{
    util::msgln("done open client socket #=%d",socket.socket);
  }

  // check socket and fill socket structure
  int retcode=wifi->getClientSocket(socket.socket,socket);
  if(retcode != RESPOK){
    // util::msgln("wifi->getClientSocket retcode=%s",wifi->ec2Str(retcode)); // debug
    return serverError;
  }

  // no web page required
  if(page==NULL)
    return ok;

  // request web page
  wifi->sendStringMulti(socket,"GET ");
  wifi->sendStringMulti(socket,page);
    // rest
  wifi->sendStringMulti(socket," HTTP/1.1\r\nHost: ");
  wifi->sendStringMulti(socket,server);
  wifi->sendStringMulti(socket,"\r\n\r\n");
  wifi->sendStringTerminate(socket);

  // get server response
  return(waitForServerResponse());
}

/****f* 
 *  NAME
 *    findString_int -- finds a string in a web page and fills the web buffer
 *  SYNOPSIS
 *    int retCode=findString("datum");
 *  FUNCTION
 *    finds a string in a web page and fills the web buffer. The web buffer will start with the search string, if found.
 *    the web page must have been opened before with requestWebPage.
 *    will not close connections if string not found
 *  INPUTS
 *    sstr   -  the string to be found
 *    mode   -  mode
 *  RESULT
 *    error code
 ******
 */
int Url::findString_int(const char *sstr, const int mode){
  char *begin; // first matching character in respBuf
  boolean found=false;
  int bytesRead=0;
  int matching=0; // number of matching characters
  const char htmlEnd[]="</HTML>"; // end of a html file

  // util::printfln("findString search %s",sstr); // debug
  // util::printfln("buffer at input *** %s ***",getBuf()); // debug

  if(mode&resetBuf){
    resetRespBuf();
  }

  // find beginning of buffer to fill
  bytesRead=strlen(respBuf);
  char *readBuf=&respBuf[bytesRead]; // initially read to the beginning of respBuf, later there maybe a partial match at the beginning to skip
  int readBufSize=respBufSize-bytesRead;


  while(!found){
    // new data needed?
    if(socket.size==0){
      if(waitForServerResponse()!=ok){
	return stringNotFound;
      }
    }
    bytesRead=wifi->getData(socket,readBuf,readBufSize,WiFiReadRaw);
    // util::printfln("findString getData try %d, got %d bytes", readBufSize, bytesRead); // debug
    if(bytesRead<0){
      return stringNotFound;
    }
    readBuf[bytesRead]='\0'; // mark the end. There is always space for one more character
    // util::msgln("buffer after getData=%s***",getBuf()); // debug
    matching=matchString(respBuf,sstr,&begin);
    // util::printfln("found %d matching bytes with search string",matching); // debug
    // at least partial string found -> copy found part to beginning of buffer
    if(matching>0){
      //copy to beginning
      strcpy(respBuf,begin);
      // util::msgln("buffer after copy search string to beginning=%s***",getBuf()); // debug
      if(matching == strlen(sstr)){
	if(mode & removeSearchString){
	  // util::printfln("removeSearchString mode=%x removeSearchString=%x",mode,removeSearchString); // debug
	  strcpy(respBuf,respBuf+strlen(sstr));
	  // util::msgln("buffer after removeSearchString=%s***",getBuf()); // debug
	}
	found=true;
	break;
      }
      // now we are sure that we have partial match -> start fill after partial match
      readBuf=respBuf+matching;
      readBufSize=respBufSize-matching;
    }else{
      // check for html end
      matching=matchString(respBuf,htmlEnd,&begin);
      // full end found
      if(matching == strlen(htmlEnd)){
	return stringNotFound;
      }
      if(matching>0){
	//copy to beginning
	strcpy(respBuf,begin);
	// now we are sure that we have partial match -> start fill after partial match
	readBuf=respBuf+matching;
	readBufSize=respBufSize-matching;
      }else{
	
	// no match -> fill buffer from beginning
	readBuf=respBuf;
	readBufSize=respBufSize;
      } // if(matching>0) - partial match htmlEnd
    } // if(matching>0) - partial match search string
  } // while(!found)

  if(!found){
    // should never be reached
    return stringNotFound;
  }
  // fully fill the buffer
  readBuf=respBuf+strlen(respBuf); 
  readBufSize=respBufSize-strlen(respBuf); 
  bytesRead=wifi->getData(socket,readBuf,readBufSize,WiFiReadRaw);
  if(bytesRead<0){
    return stringNotFound;
  }
  readBuf[bytesRead]='\0'; // mark the end. There is always space for one more character
  if(mode & closeAfterFind){
    closeConnection();
  }
  return ok;
}

/****f* 
 *  NAME
 *    findString -- finds a string in a web page and fills the web buffer
 *  SYNOPSIS
 *    int retCode=findString("datum");
 *  FUNCTION
 *    finds a string in a web page and fills the web buffer. The web buffer will start with the search string, if found.
 *    the web page must have been opened before with requestWebPage.
 *    will close connection if string not found
 *  INPUTS
 *    sstr   -  the string to be found
 *    mode   -  mode
 *  RESULT
 *    error code
 ******
 */
int Url::findString(const char *sstr, const int mode){
  int retcode=findString_int(sstr, mode);
  if(retcode==stringNotFound){
    closeConnection();
  }
  return retcode;
}

/****f* 
 *  NAME
 *    matchString -- finds a string in a buffer
 *  SYNOPSIS
 *    int n=matchString("Hello, how are yo","you",&begin);
 *  FUNCTION
 *    finds a (partial) string in a buffer. Returns the number of matching characters. 
 *    begin will point to the first matching character in the buffer
 *  INPUTS
 *    buf      - the buffer
 *    matchstr - the string to match
 *    begin    - will point to first matching character in buf (unless NULL)
 *  RESULT
 *    number of matching characters, begin will be set
 ******
 */
int Url::matchString(const char *buf, const char *matchstr, char **begin){
  char *current_1=(char *)buf; // current_1[n] points to next to match character in buf
  int n=0; // position in matchstr to match
  boolean found=false;

  // util::printfln("matchString: find %s in %s",matchstr,buf); // debug

  while(!found && current_1[n] != '\0'){
    // current position matches?
    if(current_1[n] == matchstr[n]){
      n++;
      // full match?
      if(matchstr[n]=='\0'){
	found=true;
      }
    }else{
      // no match 
      n=0; // -> reset to beginning of matchstr
      current_1++; // try next position
    }
  } // while(!found ... )

  // at least partial match
  if(n>0 && begin!=NULL){
    *begin=current_1; // store pointer to first matching character
  }
  return n;
}