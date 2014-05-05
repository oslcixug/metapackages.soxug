/* Header for module logsimh, generated by p2c */
#ifndef LOGSIMH_H
#define LOGSIMH_H


#ifndef LOGDEF_H
#include "logdef.h"
#endif

#ifndef LOGHIER_H
#include "loghier.h"
#endif


#ifndef LOGDIG_H
#include "logdig.h"
#endif

#ifndef LOGSIMA_H
#include "logsima.h"
#endif


#ifndef LOGSIMED_H
#include "logsimed.h"
#endif

#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif

#ifndef NEWCI_H
#include <p2c/newci.h>
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#ifndef MATH_H
#include <p2c/math.h>
#endif

#ifndef CITINFOMOD_H
#include <p2c/citinfomod.h>
#endif


#ifdef LOGSIMH_G
# define vextern
#else
# define vextern extern
#endif


extern Void Log_16_inst PP((log_16_action *act));
extern Void Log_dig_inst PP((log_16_action *act));


extern Void Log_16_digh PP((log_16_action *act));


#undef vextern

#endif /*LOGSIMH_H*/

/* End. */