/* Header for module GENERAL_2, generated by p2c */
#ifndef GENERAL_2_H
#define GENERAL_2_H



#ifndef IODECLARATIONS_H
#include <p2c/iodecl.h>
#endif



#ifdef GENERAL_2_G
# define vextern
#else
# define vextern extern
#endif
extern Void P_readnumber PP((long device, double *num));
extern Void P_writenumber PP((long device, double value));
extern Void P_readstring PP((long device, Char *str));
extern Void readstring_until PP((int term, long device, Char *str));
extern Void P_writestring PP((long device, Char *str));
extern Void P_readnumberln PP((long device, double *num));
extern Void P_writenumberln PP((long device, double value));
extern Void P_writestringln PP((long device, Char *str));
extern Void P_readuntil PP((int term, long device));
extern Void P_skipfor PP((long count, long device));



#undef vextern

#endif /*GENERAL_2_H*/

/* End. */

