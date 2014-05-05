#define SYSDEVS_G


#if defined(aux)
# include <sys/types.h>  /* A/UX needs this. */
# include <time.h>
# include <sys/time.h>   /* A/UX needs this, too! */
#elif defined(__sgi)
# include <sys/types.h>  
# include <time.h>
# include <sys/time.h>   
#elif defined(rs6000)
# include <sys/time.h>
# include <time.h>
#elif defined(BSD)
# include <time.h>
# include <sys/time.h>
# include <sys/types.h>
#else
# include <time.h>
#endif
#ifdef OS2
# include <sys/timeb.h>
#endif
  
#include <p2c/p2c.h>

#ifndef SYSGLOBALS_H
#include <p2c/sysglobals.h>
#endif

#ifndef SYSDEVS_H
#include <p2c/sysdevs.h>
#endif

#ifdef OS2

long sysclock()
{

  struct timeb tm;

  _ftime(&tm);

  return((long)(tm.time % 86400) * 100 + tm.millitm / 10);

}

#else

long sysclock()
{

  struct timeval time;
  struct timezone tz;

  gettimeofday(&time, &tz);

  return(time.tv_sec % 86400 * 100 + time.tv_usec / 10000);   

}

#endif

void sysdate(thedate)
daterec *thedate;
{
  struct tm *t;
  time_t temp;

  temp = time(NULL);
  t = localtime(&temp);
  thedate->year = (char) t->tm_year % 100;
  thedate->day = t->tm_mday;
  thedate->month = t->tm_mon+1;
}

#ifdef OS2
void systime(thetime)
     timerec *thetime;
{
  
  struct timeb time;
  struct tm *t;
  long temp; /* G. Liu + */
  
  _ftime(&time);
  temp = time.time;
  t = localtime(&temp);
  thetime->hour = t->tm_hour;
  thetime->minute = t->tm_min;
  thetime->centisecond = t->tm_sec*100 + time.millitm/10;

}

#else

void systime(thetime)
     timerec *thetime;
{
  struct timeval time;
  struct timezone tz;
  struct tm *t;
  time_t temp; /* G. Liu + */
  
  gettimeofday(&time, &tz);
  temp = time.tv_sec;
  t = localtime(&temp);
  thetime->hour = t->tm_hour;
  thetime->minute = t->tm_min;
  thetime->centisecond = t->tm_sec*100 + time.tv_usec/10000;
}

#endif

void SETRUNLIGHT(c)
char c;
{
}

void BEEP()
{
}
