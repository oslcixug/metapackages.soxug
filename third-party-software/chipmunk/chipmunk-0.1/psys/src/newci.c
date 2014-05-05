#define NEWCI_G

#include <stdio.h>
#include <ctype.h>

#if defined(aux)
# include <sys/types.h>   
# include <sys/time.h>
# include <time.h>
#elif defined(__sgi)
# include <sys/types.h>   
# include <sys/time.h>
# include <time.h>
#elif defined(rs6000)
# include <sys/time.h>
# include <time.h>
#elif defined(BSD)
# include <sys/time.h>
#else
# include <time.h>
#endif
#ifdef OS2
# include <sys/timeb.h>
#endif

#include <p2c/p2c.h>
#include <p2c/newci.h>
#include <p2c/newcrt.h>


boolean newci_findprocedure(name, start)
Char *name;
_PROCEDURE *start;
{
  return(0);
}


boolean newci_findprogram(name)
Char *name;
{
  return(0);
}


void newci_loadprogram(name)
Char *name;
{
  return;
}


void newci_markprogram(id)
long *id;
{
  *id = 0;
  return;
}


void newci_releaseprogram(id)
long *id;
{
  *id = 0;
  return;
}


/*
   procedure inputstring(var s : string; mode : newci_imode;
                         possterm : newci_argstr; var term : char;
                         echoterm : boolean; var pos : integer);
*/
extern Void newci_inputstring PP((Char *s, int mode, Char *possterm,
				  Char *term, int echoterm, long *pos));

     
/*  This procedure should be made to do something real.  */
void newci_inputstring(s, mode, possterm, term, echoterm, pos)
Char *s;
int mode;
Char *possterm;
Char *term;
int echoterm;
long *pos;
{
  char buf[1024];
  char *cp;

  fgets(buf, 1024, stdin);
  if (cp = (char *) strchr(buf, '\n'))
    *cp = '\0';
  if (*buf)
    strcpy(s, buf);
}


Static Void goodfixname(name, ext, home, force)
Char *name;
Char *ext, *home;
boolean force;
{
  long i;
  boolean hassuffix;
  Char STR1[256];
  long FORLIM;

 /* fs_zapspaces(name); */
  if (strlen(name) > 2 && name[0] == '*' && name[1] == '*') {
    strcpy(name, name + 2);
    sprintf(name, "%s%s", home, strcpy(STR1, name));
  }
  if (force) {
    i = strlen(name);
    while (i > 0 && name[i - 1] != '.' && name[i - 1] != '/' && name[i - 1] != ':')
      i--;
    if (i > 0 && name[i - 1] == '.')
      name[i - 1] = '\0';
  }
  if (*name == '\0')
    return;
  if (name[strlen(name) - 1] == '.') {
    name[strlen(name) - 1] = '\0';
    return;
  }
  if (name[strlen(name) - 1] == ':')
    return;
  hassuffix = false;
  FORLIM = strlen(name);
  for (i = 0; i < FORLIM; i++) {
    switch (name[i]) {

    case '/':
    case ':':
      hassuffix = false;
      break;

    case '.':
      hassuffix = true;
      break;
    }
  }
  if (!hassuffix && *ext != '\0')
    sprintf(name + strlen(name), ".%s", ext);
}


Void newci_fixfname(s, ext, home)
Char *s, *ext, *home;
{
  goodfixname(s, ext, home, 0);
}

Void newci_forcefname(s, ext, home)
Char *s, *ext, *home;
{
  goodfixname(s, ext, home, 1);
}


void newci_fullshellescape(args, opts)
char *args, *opts;
{
  while (isspace(*args) || *args == '!') args++;
  if (fork())
    wait(NULL);
  else
    if (*args)
      execlp("/bin/csh", "xterm", "-e", args, 0);
    else
      execlp("/bin/csh", "xterm", 0);
}

void newci_shellescape(args)
char *args;
{
  newci_fullshellescape(args, "");
}

void newci_fulleditescape(fn, opts, ln, pos)
char *fn, *opts;
long ln, pos;
{
  if (fork())
    wait(NULL);
  else
    execlp("emacs", "emacs", fn, 0);
}


void newci_eatargument()
{
  int i;

  for (i = 2; i < P_argc; P_argv[i-1] = P_argv[i++])
    ;
  P_argc--;
}

void newci_parseswitch(tab, size, bad)
newci_parsetab tab;
long size;
char *bad;
{
  int i, j, found, pos = 1;

  for (i = 0; i < size; tab[i++].used = 0)
    ;
  *bad = '\000';
  
  while ((P_argc > 1) && (P_argv[1][0] == '-')) {
    found = 0;
    for (i = 0; (i < size) && (! found); i++) {
      for (j = 0; (tab[i].switch_[j] != '\0') && (! found); j++)
	if (tab[i].switch_[j] == P_argv[1][1]) 
	  found = 1;
      if (found) break;
    }
    if (! found) {
      bad[strlen(bad)+1] = '\0';
      bad[strlen(bad)] = P_argv[1][1];
    } else {
      tab[i].used = pos++;
      switch (toupper(tab[i].kind)) {
      case 'B':
	break;
      case 'I':
	tab[i].UU.i = atol(P_argv[1]+2);
	break;
      case 'R':
	sscanf(P_argv[1]+2, "%lf", &tab[i].UU.r);
	break;
      case 'S':
	tab[i].UU.s = P_argv[1]+2;
	break;
      case 'M':
	if (P_argc > 2 && !P_argv[1][2]) {
	  tab[i].UU.s = P_argv[2];
	  newci_eatargument();
	} else {
	  tab[i].UU.s = P_argv[1]+2;
	}
	break;
      }
    }
    newci_eatargument();
  }
}


long timers_sysclock()
{

#ifdef OS2
  struct timeb tm;

  _ftime(&tm);

  return((long)(tm.time % 86400) * 100 + tm.millitm / 10);

#else

  struct timeval time;
  struct timezone tz;

  gettimeofday(&time, &tz);

  return((time.tv_sec % 86400) * 100 + time.tv_usec / 10000);

#endif

}


long newci_fullseconds()
{
  return(time(NULL));
}


void newci_nullrecover()
{
  _Escape(P_escapecode);
}







