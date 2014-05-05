#define FS_G


#include <p2c/p2c.h>
#include <p2c/fs.h>



Void fs_zapspaces(Char *s)
{
  char *cp1 = s;
  char *cp2 = s;

  while (*cp1) {
    if (*cp1 > ' ')
      *cp2++ = *cp1;
    cp1++;
  }
  *cp2 = '\0';
}


Void fs_fstripname(Char *s, Char *pvname, Char *ppath, Char *pfname)
{
  char *cp;

  *pvname = '\0';
  strcpy(ppath, s);
  cp = (char *) strrchr(ppath, '/');
  *cp = '\0';
  strcpy(pfname, cp+1);
}


