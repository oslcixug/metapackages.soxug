#define GENERAL_3_G

#include <stdio.h>
#include <p2c/p2c.h>
#include <p2c/general_3.h>

char *ioerror_message(s, err)
char *s;
long err;
{
  sprintf(s, "ioerror_message ERROR number %ld", err);
}
  
