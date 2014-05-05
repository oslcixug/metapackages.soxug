#define LUNIX_PAS_G


#include <time.h>
#ifdef aux
# include <sys/types.h>   /* A/UX needs this. */
# include <sys/time.h>    /* A/UX needs this, too! */
#endif

#include <p2c/p2c.h>
#include <p2c/lunix_pas.h>
#include <p2c/sysglobals.h>



Void lunix_intToDate (long idate, daterec *date, timerec *time)
{
  struct tm *temp_tm;

  temp_tm = localtime(&idate);
  date->year = temp_tm->tm_year;
  date->day = temp_tm->tm_mday;
  date->month = temp_tm->tm_mon + 1;
  time->hour = temp_tm->tm_hour;
  time->minute = temp_tm->tm_min;
  time->centisecond = temp_tm->tm_sec * 100;
}

