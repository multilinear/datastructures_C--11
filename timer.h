#include <sys/timeb.h>

double tdiff(timeb& t1, timeb& t2) {
  return ((double)(t1.time - t2.time)) + (((double)(t1.millitm - t2.millitm))/1000);
  //return (t1.millitm - t2.millitm);
}
