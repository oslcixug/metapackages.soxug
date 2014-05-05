/* Header for module logsima, generated by p2c */
#ifndef LOGSIMA_H
#define LOGSIMA_H


#ifndef LOGDEF_H
#include "logdef.h"
#endif


#ifndef LOGDIG_H
#include "logdig.h"
#endif

#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif


#ifdef LOGSIMA_G
# define vextern
#else
# define vextern extern
#endif


typedef struct nodeinfo {
  log_16_value v, v0, v00, defv, truev;
} nodeinfo;

typedef struct kindinfo {
  na_long info;
  boolean hasproc, instance;
  short numppins, numpvars;
  Void (*kindproc) PP((log_16_action *act));
  log_krec *knext;
} kindinfo;

typedef struct gateinfo {
  log_nrec **ppins;
  Char *pvars;
} gateinfo;


vextern log_tool *logsima_tool_16;
vextern log_16_action logsima_action;
vextern Void (*logsima_dig_inst) PP((log_16_action *act));


extern Void logsima_init PV();
extern Void executegates PP((boolean *active, log_grec *g));
extern Void executeprog PP((uchar *pr, log_nrec **pins, gateinfo *ginfo,
			    na_long *vars));
extern Void clearnodes PP((log_nrec *n));
extern Void copynodes PP((log_nrec *n, boolean *stable));
extern Void callgate PP((log_16_actionkinds action));


#undef vextern

#endif /*LOGSIMA_H*/

/* End. */