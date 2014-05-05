
#include <stdio.h>
#include <ctype.h>
#include <p2c/p2c.h>

typedef union swap2 {
        short ival;
        unsigned char c[2];
} swap2;

short getshortsw(c)
  char *c;
  {
    swap2 s;

    if (bigendian)
      {
        s.c[1] = c[1];
        s.c[0] = c[0];
      }
    else
      {
        s.c[0] = c[1];
        s.c[1] = c[0];
      }
    
    return s.ival;
  }
     
short shortsw(sh)
  short sh;
  {
    swap2 s1,s2;
    s1.ival = sh;

    if (bigendian)
      {
        s2.c[1] = s1.c[1];
        s2.c[0] = s1.c[0];
      }
    else
      {
        s2.c[0] = s1.c[1];
        s2.c[1] = s1.c[0];
      }
     return s2.ival;
 }

typedef union swap4 {
        long ival;
        unsigned char c[4];
} swap4;

long getintsw(c)
char *c;
{
    swap4 s;

    if (bigendian)
      {
        s.c[0] = c[0];
        s.c[1] = c[1];
        s.c[2] = c[2];
        s.c[3] = c[3];
      }
    else
      {
        s.c[0] = c[3];
        s.c[1] = c[2];
        s.c[2] = c[1];
        s.c[3] = c[0];
      }
    return s.ival;
  }

long intsw(ii)
long ii;
{
    swap4 s1,s2;
    s1.ival = ii;

    if (bigendian)
      {
        s2.c[0] = s1.c[0];
        s2.c[1] = s1.c[1];
        s2.c[2] = s1.c[2];
        s2.c[3] = s1.c[3];
      }
    else
      {
        s2.c[0] = s1.c[3];
        s2.c[1] = s1.c[2];
        s2.c[2] = s1.c[1];
        s2.c[3] = s1.c[0];
      }

     return s2.ival;
}

short reverse(s)
short s;
{
    short b = 0,i;
    b = s & 0x1;
    for (i=0;i<15;i++)
        b = (b<<1) | ((s = s>>1)&1);
    return b;
}









