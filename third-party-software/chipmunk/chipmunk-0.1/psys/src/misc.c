#define MISC_G

#include <stdio.h>
#include <p2c/p2c.h>
#include <p2c/misc.h>

#ifndef LACK_GETPWNAM 
#include <pwd.h>
#endif

Void misc_upc(Char *s)
{
  for (; *s; s++)
    if (islower(*s))
      *s = toupper(*s);
}


void misc_getioerrmsg(s, i)
char *s;
long i;
{
  sprintf(s, "misc_getioerrmsg Error number %ld", i);
}

void misc_printerror(esc, ior)
long esc, ior;
{
}


#ifdef OS2
/* Here are things OS/2 does not know about that anaLOG needs */

int getuid()
{
   return 1000;
}

int getgid()
{
   return 1000;
}

char *cuserid(s)
char *s;
{
   char *sp;
   static char sbuf[L_cuserid + 1];
 
   if ((sp = getenv("USER")) == NULL) {
     if (s != NULL)
       s[0] = '\0';
     return NULL;
   }
   return strncpy(s != NULL ? s : sbuf, sp, L_cuserid);
}

long lrand48()
{
   return rand();
}
 
void srand48(seedv)
long seedv;
{
   srand( (int)seedv);
}

int _cfree(ptr)
char *ptr;
{
   printf("Call to _cfree.\n");
   free(ptr);
   return 0;
}

int link(path1, path2)
char *path1, *path2;
{
   printf("Call to link. %s to %s\n", path1, path2);
   rename(path1, path2);
   return 0;
}

int fork()
{
   printf("Call to fork.\n");
   return 0;
}
void _flsbuf(ptr, ch)
char *ptr;
char ch;
{
   printf("Call to _flsbuf.\n");
   return;
}

#endif /* OS/2 */

#ifdef __bsdi__
char *cuserid(s)
char *s;
{
   char *sp;
   static char sbuf[L_cuserid + 1];
 
   if ((sp = getenv("USER")) == NULL) {
     if (s != NULL)
       s[0] = '\0';
     return NULL;
   }
   return strncpy(s != NULL ? s : sbuf, sp, L_cuserid);
}

long lrand48()
{
   return random();
}
 
void srand48(seedv)
long seedv;
{
   srandom( (int)seedv);
}

int cfree(ptr)
char *ptr;
{
   free(ptr);
   return 0;
}

int _cfree(ptr)
char *ptr;
{
   printf("Call to _cfree.\n");
   free(ptr);
   return 0;
}
#endif

#ifdef freebsd

int cfree(ptr)
char *ptr;
{
   free(ptr);
   return 0;
}

int _cfree(ptr)
char *ptr;
{
   printf("Call to _cfree.\n");
   free(ptr);
   return 0;
}

char *cuserid(s)
char *s;
{
   char *sp;
   static char sbuf[L_cuserid + 1];
 
   if ((sp = getenv("USER")) == NULL) {
     if (s != NULL)
       s[0] = '\0';
     return NULL;
   }
   return strncpy(s != NULL ? s : sbuf, sp, L_cuserid);
}

char *_cuserid(s)
char *s;
{
   char *sp;
   static char sbuf[L_cuserid + 1];

   if ((sp = getenv("USER")) == NULL) {
     if (s != NULL)
       s[0] = '\0';
     return NULL;
   }
   return strncpy(s != NULL ? s : sbuf, sp, L_cuserid);
}

#endif

#ifdef __MACHTEN__

char *cuserid(s)
char *s;
{
   char *sp;
   static char sbuf[L_cuserid + 1];
 
   if ((sp = getenv("USER")) == NULL) {
     if (s != NULL)
       s[0] = '\0';
     return NULL;
   }
   return strncpy(s != NULL ? s : sbuf, sp, L_cuserid);
}

long lrand48()
{
   return random();
}
 
void srand48(seedv)
long seedv;
{
   srandom( (int)seedv);
}

int cfree(ptr)
char *ptr;
{
   free(ptr);
   return 0;
}

int _cfree(ptr)
char *ptr;
{
   printf("Call to _cfree.\n");
   free(ptr);
   return 0;
}

#endif


#ifndef LACK_GETPWNAM 

/* tilde-expansion for platforms with passwd mechanisms.
 * this replaces a leading ~/ with the user's home directory,
 * or, ~foo with foo's home directory, if foo is a recognised user.
 */

Char *tilde_expand(s)
Char *s;
{
Char *name;
Char *ret = NULL;
int len = 0;
struct passwd *pw;

  if (*s == '~') {
    if (strlen(s) > 1)
      len = strcspn(s+1, "/");
    if (len > 0) {
      name = (Char *) Malloc(len + 1);
      strncpy(name, s+1, len);
      name[len] = '\0';
      pw = getpwnam(name);
      Free(name);
    } else {
      pw = getpwuid( geteuid() );
    } 
    if (pw != NULL) {
      ret = (Char *) Malloc(strlen(pw->pw_dir) + strlen(s) - len);
      strcpy(ret,pw->pw_dir);
      if (strlen(s) > len + 1)
        strcat(ret, s + len + 1);
    }
  }
  if (ret == NULL)
    ret = strdup(s);
  return ret; 
}

#else

/* simple tilde expansion for platforms without usernames/password mechanisms.
 * replace a leading '~' with the value of the $HOME environment variable.
 */
Char *tilde_expand(s)
Char *s;
{
Char *hp;
Char *ret = NULL;

  if (*s == '~' && (hp = getenv("HOME")) != NULL) {
    if ((ret = (Char *) Malloc(strlen(hp) + strlen(s))) != NULL) {
      strcpy(ret, hp);
      if (strlen(s) > 1)
        strcat(ret, s + 1);
    }
  }
  if (ret == NULL)
    ret = strdup(s);
  return ret;
}

#endif

