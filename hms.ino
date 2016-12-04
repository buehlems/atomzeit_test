#include "hms.h"

/****f* 
  *  NAME
  *    time2minutes -- converts h:m into minutes only
  *  SYNOPSIS
  *   time2minutes(10,20);
  *  FUNCTION
  *    calculates the number of minutes and sets the internal values;
  *  INPUTS
  *    h      - number of hours
  *    m      - number of minutes
  *  RESULT
  *    total number of minutes
   ******
*/
int Minute::time2minutes(int h, int m) {
  this->h=h;
  this->m=m;
  return(h*60+m);
}

/****f* 
  *  NAME
  *    minutes2time -- converts minutes to h:m
  *  SYNOPSIS
  *   minutes2time(1034);
  *  FUNCTION
  *    converts minutes into h:m, sets the internal values;
  *  INPUTS
  *    m       - number of minutes
  *  RESULT
  *    ---
   ******
*/
void Minute::minutes2time(int m) {
  this->h=m/60;
  this->m=m%60;
}


/****f* 
  *  NAME
  *    time2seconds -- converts h:m:s into seconds only
  *  SYNOPSIS
  *   time2seconds(10,20,5);
  *  FUNCTION
  *    calculates the number of seconds and sets the internal values;
  *  INPUTS
  *    h      - number of hours
  *    m      - number of minutes
  *    s      - number of seconds
  *  RESULT
  *    total number of seconds
   ******
*/
long Second::time2seconds(int h, int m, int s) {
  this->s=s;
  return((long) 60*time2minutes(h,m)+s);
}

/****f* 
  *  NAME
  *    seconds2time -- converts minutes to h:m:s
  *  SYNOPSIS
  *   seconds2time(78304);
  *  FUNCTION
  *    converts seconds into h:m:s, sets the internal values;
  *  INPUTS
  *    s       - number of seconds
  *  RESULT
  *    ---
   ******
*/
void Second::seconds2time(long s) {
  this->s=s%60;
  minutes2time(s/60);
}

