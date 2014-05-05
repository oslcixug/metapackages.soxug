/* Output from p2c, the Pascal-to-C translator */
/* From input file "logdig.text" */


/* "LOG", the circuit editing and simulation system,
   "DigLOG", the digital simulator for LOG.
   Copyright (C) 1985, 1990 David Gillespie.
   Author's address: daveg@csvax.caltech.edu; 256-80 Caltech/Pasadena CA 91125.

   "AnaLOG", the analog simulator for LOG.
   Copyright (C) 1985, 1990 John Lazzaro.
   Author's address: lazzaro@csvax.caltech.edu; 256-80 Caltech.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (any version).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */


/*

      LOG 4.1   Digital logic simulation routines (external interface)

      David Gillespie  7/13/85

      The following module is subject to change at any time.
*/



#include <p2c/p2c.h>


#define LOGDIG_G
#include "logdig.h"


Const log_16_viarr log_16_vi = {
  0, 1, 3
};

Const log_16_ivarr log_16_iv = {
  log_none, log_zero, log_one, log_one
};

Const log_16_vbarr log_16_vb = {
  false, false, true
};

Const log_16_bvarr log_16_bv = {
  log_zero, log_one
};




/* All the real stuff is in LOGSIM and LOGSIMA */






/* End. */
