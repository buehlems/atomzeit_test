#ifndef HMS_H
#define HMS_H

typedef struct {
  int h;
  int m;
} Hm;

typedef struct {
  int h;
  int m;
  int s;
} Hms;

class Minute {
 private:
  int h;
  int m;
 public:
  const static int day=24*60;
  Minute(void) { h=0; m=0; }
  Minute(Hm hm) { h=hm.h; m=hm.m; }
  Minute(int m) { minutes2time(m); }
  ~Minute(void) { };
  int time2minutes(int h, int m);
  int time2minutes(Hm &t) { return time2minutes(t.h,t.m); } ;
  void minutes2time(int m);
  int geth() { return h; };
  int getm() { return m; };
  int getMinutesDay() { return(60*h+m); };
  void getHm(Hm &t) {t.h=geth(); t.m=getm(); };
};

class Second : public Minute {
 private:
  int s;
 public:
  const static long day=24l*60l*60l;
  Second(void) { s=0;};
  ~Second(void) { };
  long time2seconds(int h, int m, int s);
  int time2seconds(Hms &t) { return time2seconds(t.h,t.m,t.s); } ;
  void seconds2time(long s);
  int gets() { return s; };
  long getSecondsDay() { return ((long)60*getMinutesDay()+s); };
  void getHms(Hms &t) {t.h=geth(); t.m=getm(); t.s=gets(); };

};
  

#endif
