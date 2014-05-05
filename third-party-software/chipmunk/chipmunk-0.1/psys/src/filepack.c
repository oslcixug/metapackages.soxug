#define FILEPACK_G

#include <stdio.h>
#include <math.h>
#include <p2c/p2c.h>
#include <p2c/filepack.h>

#ifndef SYSDEVS_H
#include <p2c/sysdevs.h>
#endif

#ifndef SYSGLOBALS_H
#include <p2c/sysglobals.h>
#endif

void fp_open_dir(dirfib, dirname, dirinfo)
fib *dirfib;
char *dirname;
misc_catentry *dirinfo;
{
  fprintf(stderr, "FILEPACK kluge: fp_open_dir not yet supported\n");
}

void fp_close_dir(dfib)
fib *dfib;
{
  fprintf(stderr, "FILEPACK kluge: fp_close_dir not yet supported\n");
}

void fp_cat_dir(dfib, numfiles, where)
fib *dfib;
short *numfiles;
Anyptr *where;
{
  fprintf(stderr, "FILEPACK kluge: fp_cat_dir not yet supported\n");
  *numfiles = 0;
}

void fp_change(filename1, filename2)
char *filename1, *filename2;
{
  char filename3[256];
  char *cp;

  if (strchr(filename2, '/')) {
    strcpy(filename3, filename2);
  } else {
    strcpy(filename3, filename1);
    cp = (char *) strrchr(filename3, '/');
    if (cp)
      cp++;
    else
      cp = filename3;
    strcpy(cp, filename2);
  }
  if (access(filename3,0)==0)
    unlink(filename3);
  if (!link(filename1, filename3))
    unlink(filename1);
}

boolean fp_getwd(path, rootname)
char *path, *rootname;
{
  rootname[0] = '\0';
  getcwd(path, fidleng+1);
}

void fp_remove(filename)
char *filename;
{
  unlink(filename);
}


