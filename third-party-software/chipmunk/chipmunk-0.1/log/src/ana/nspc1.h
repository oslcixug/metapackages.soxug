/* edited by Bhusan to add fourth terminal and spice output */
#ifndef NSPC1_H
#define NSPC1_H


#ifndef LOGDEF_H
#include "logdef.h"
#endif


#ifndef NSOLVER_H
#include "nsolver.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif

#ifndef SYSGLOBALS_H
#include <p2c/sysglobals.h>
#endif

#ifndef NEWCI_H
#include <p2c/newci.h>
#endif


#ifdef NSPC1_G
# define vextern
#else
# define vextern extern
#endif

/*$If Float$
$float_hdw on$
$End$*/
/*$If Debugflag$
$Debug$
$End$*/



extern Void Log_NSPC1_INITLIB_32 PP((Analog_32_action *act));


#undef vextern

#endif /*NFET7_H*/

/* End. */
