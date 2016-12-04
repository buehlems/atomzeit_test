#include "util.h"

util::util(void) {
}

util::~util(void) {
}

void util::hexdump(const unsigned char *buf, const unsigned int len, const unsigned int dump_width) {
  #ifdef DEBUG
  const unsigned char *p; // current position
  const unsigned char *q; // last position dump_width
    const unsigned char *e = buf + len; // end
    unsigned char i;

    // util::msg("hexdump l=",len);
    util::printf(" ");
    for (p = q = buf; p <= e; ) {
      // dump_width or end reached?
      if (p == e || (p - q) == (int)dump_width) {
	// fill dump_width with blanks
	for (i = 0; i < dump_width - (p - q); i++) {
	  util::printf("   ");
	}

	util::printf(": ");
	// print again but blank out non-ASCII
	for (; q < p; q++) {
	  util::printf("%c",(*q >= 0x20 && *q <= 0x7e) ? *q : '.');
	}
	util::println("");
	q = p;
	
	if (p == e) {
	  break;
	}
      } else {
	if(*p < 0x10) util::printf("0");
	util::printf("%x",*p); 
	p++;
      }
      util::printf(" ");
    }
    // util::println("");

    // util::msg("hexdum end");

    #endif /* DEBUG */
}

void util::ascdump(unsigned char *buf, unsigned int len) {
#ifdef DEBUG
  for(unsigned int n=0; n<len;n++){
    util::printf("%c",buf[n]);
  }
  util::printfln("");
#endif /* DEBUG */
}
void util::var_printf(const char *fmt, va_list ap) {
  static char strbuf[maxstr];

  vsnprintf(strbuf,sizeof(strbuf),fmt,ap);
  Serial.print(strbuf);
}

void util::var_printf(const char *str) {
   Serial.print(str);
}

void util::printf(const char *fmt, ...) {
   va_list ap;

   va_start(ap,fmt);
   var_printf(fmt,ap);
   va_end(ap);
}

void util::printfln(const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    util::var_printf(fmt,ap);
    va_end(ap);
    util::var_printf("\n");
}

void util::msg(const char c) {
  #ifdef DEBUG
  util::print(c);
  #endif /* DEBUG */
}

void util::msg(const char *fmt, ...) {
  #ifdef DEBUG
    va_list ap;
    va_start(ap,fmt);
    util::var_printf(fmt, ap);
    va_end(ap);
  #endif /* DEBUG */
}

void util::msgln(const char *fmt, ...) {
  #ifdef DEBUG
  va_list ap;
  va_start(ap,fmt);
   util::var_printf(fmt,ap);
   va_end(ap);
   util::var_printf("\n");
  #endif /* DEBUG */
}

void util::print(const char c) {
  Serial.print(c);
}

void util::print(const char *str) {
  Serial.print(str);
}

void util::println(const char *str) {
  util::print(str);
  Serial.println();
}

void util::debug(const char *str, unsigned int uint) {
  Serial.print(str);
  Serial.print("0x");
  Serial.print(uint, HEX);
}

void util::debugln(const char *str, unsigned int uint) {
  util::debug(str,uint);
  Serial.println();
}

void util::debug(const char c){
  char d;
  d=(c >= 0x20 && c <= 0x7e) ? c : '.';
  util::print(d);
  // util::debug(" ",(unsigned int)c);util::print("  ");
}

void util::debugln(const char c){
  util::debug(c);
  Serial.println();
}

void util::endlessLoop(){
    printfln("going into endless loop");
    while(1){}
}

/****f* 
  *  NAME
  *    strappend 
  *  SYNOPSIS
  *    if(strappend(str1,str2,maxlen)>=maxlen) printf "string too long";
  *  FUNCTION
   *    appends add string tor str, but only if length of new string is < maxlen (excluding \0)
  *  INPUTS
  *    str1 - destination string
  *    str2 - sting to add
  *    maxlen - max length of new string (including \0)
   *  RESULT
  *    size of new string (not appended if len >= maxlen)
  ******
*/

int util::strappend(char *str1, const char *str2, const int max) {
  int len=strlen(str1)+strlen(str2);

  if(len<max){
    strcat(str1,str2);
  }else{
    util::msgln("Error strappend: string too short %d>=%d",strlen(str1)+strlen(str2),max);
    util::msgln("  destination: %s %d",str1,strlen(str1));
    util::msgln("  append:      %s %d",str2,strlen(str2));
  }
  return(len);
}
