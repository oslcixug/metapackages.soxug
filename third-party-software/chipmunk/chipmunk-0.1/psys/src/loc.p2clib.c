
/* "p2c"  Copyright 1989  Dave Gillespie */


/* Library for use with p2c, the Pascal to C translator */
/* CIT-local portion */



#include <p2c/p2c.h>

#include <p2c/citinfomod.h>



P_citinforec *P_citinfo;
long P_citSystem;
static P_citinforec citinfo;


#ifdef citPWS
pascal char citinfomod;
#endif





void _local_p2c_init()
{
#ifdef citPWS
    struct pasc_citinfo {
        char username[32];
        char homepath[122];
        char loginat[6];
        short node;
        short userid;
        short groupid;
        char machine[20];
        char novice;
    } *pcit = P__var(citinfomod, -4, struct pasc_citinfo *);
    P_citinfo = &citinfo;
    str_ptoc(P_citinfo->username, pcit->username);
    P_citinfo->node = pcit->node;
    P_citinfo->userid = pcit->userid;
    *P_citinfo->machine = 0;
    P_citSystem = P__var(citinfomod, -16, int);
#else
    char *getlogin();
    P_citinfo = &citinfo;
    cuserid(P_citinfo->username);

#ifdef OS2
    if( getenv("HOME") != NULL)
      strcpy(P_citinfo->homepath, getenv("HOME"));
#else
    strcpy(P_citinfo->homepath, getenv("HOME"));
#endif

    /* should initialize loginat somehow */
    P_citinfo->node = 0;
    P_citinfo->userid = getuid();
    P_citinfo->groupid = getgid();
    *P_citinfo->machine = 0;
    P_citinfo->novice = 0;
#endif
}





/* End. */

