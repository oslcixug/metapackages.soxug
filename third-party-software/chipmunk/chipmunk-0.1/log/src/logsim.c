/* Output from p2c, the Pascal-to-C translator */
/* From input file "logsim.text" */


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

      LOG 4.1   Digital logic simulation routines

      David Gillespie  7/13/85

      The following module is subject to change at any time.
*/


/* $debug$ {*/


#include <p2c/p2c.h>


#define LOGSIM_G
#include "logsim.h"



#define newci_findprocedure  findprocedure
extern boolean findprocedure PP((Char *name, Void (**start)()));


/*$ if debugging $
   $ debug on $
$ end $*/



#define traceactions    false

#define digtimestep     1
#define stabdelay       2
#define digonoff        3

#define histcolor       1
#define histvis         2
#define histsize        3


typedef log_16_value tlist[(long)log_one - (long)log_none + 1];
typedef tlist ttable[(long)log_one - (long)log_none + 1];
typedef long histcolortabletype[12];


static Const ttable bcomb = {
  { log_none, log_zero, log_one },
  { log_zero, log_zero, log_one },
  { log_one, log_one, log_one }
};

static Const histcolortabletype histcolortable = {
  log_cyan, log_orange, log_yellow, log_pink, log_green, log_red, log_white,
  log_mred, log_lgray, log_dcyan, log_dyellow, log_dred
};




Static long rndseed, newsystime, oldsystime, stabtime, resetcounter;
Static log_sigrec *vddsig, *gndsig, *resetsig;
Static boolean isstable, stabilizing, passready, maketimebid;
Static double nexttimebid;
Static boolean noprogloaded;
Static log_krec *kbase_16;

Static na_strlistrec *diglbl;
Static short dignumattrs;
Static log_kattrrec *digkattr;
Static log_gattrrec *diggattr;

Static short backgrcolor, ledoncolor, ledoffcolor, scopecolor, scopescancolor;






/* The following routines are exported so that users' gates can use them. */


Static Void log_16_input(lact, n, v)
log_action *lact;
log_nrec *n;
log_16_value *v;
{
  nodeinfo *ni;

  ni = (nodeinfo *)n->info;
  *v = ni->v;
}



Static Void log_16_output(lact, n, v)
log_action *lact;
log_nrec *n;
log_16_value v;
{
  nodeinfo *ni;

  ni = (nodeinfo *)n->info;
  if ((int)v > 2)
    v = log_one;
  if (ni->v0 == log_none)
    ni->v0 = v;
  else if (ni->v0 != v && v != log_none)
    (*lact->hook.nodeconflict)(n);
}



Static Void log_16_ocoutput(lact, n, v)
log_action *lact;
log_nrec *n;
log_16_value v;
{
  nodeinfo *ni;

  if (v != log_zero)
    return;
  ni = (nodeinfo *)n->info;
  if (ni->v0 != log_none)
    (*lact->hook.nodeconflict)(n);
  else
    ni->v0 = v;
}



Static Void getpointmark(num, defx, defy, x, y)
short num, defx, defy, *x, *y;
{
  log_action *WITH;
  log_hook2 *WITH1;

  *x = defx;
  *y = defy;
  WITH = logsima_action.lact;
  WITH1 = WITH->hook2;
  (*WITH1->findpointmarker)(WITH->actgate->kind, num, x, y);
}





Static Void log_16_led(lact, x, y, v)
log_action *lact;
short x, y;
log_16_value v;
{
  (*lact->hook.xform)(lact->actgate, &x, &y);
  (*lact->hook.hidecursorrect)(x - 2L, y - 2L, x + 2L, y + 2L);
  switch (v) {

  case log_none:
    m_color((long)lact->color.backgr);
    break;

  case log_zero:
    m_color((long)ledoffcolor);
    break;

  case log_one:
    m_color((long)ledoncolor);
    break;
  }
  m_fillrect(x - 2L, y - 2L, x + 2L, y + 2L);
  (*lact->hook.unhidecursor)();
}



Static Void log_16_eraled(lact, x, y)
log_action *lact;
short x, y;
{
  (*lact->hook.xform)(lact->actgate, &x, &y);
  (*lact->hook.hidecursorrect)(x - 2L, y - 2L, x + 2L, y + 2L);
  m_color((long)lact->color.backgr);
  m_fillrect(x - 2L, y - 2L, x + 2L, y + 2L);
  (*lact->hook.unhidecursor)();
}



Static Void log_16_plotled(lact, x, y, v)
log_action *lact;
short x, y;
log_16_value v;
{
  na_strlistrec *l1;
  Char STR2[256];

  if (v == log_none)
    return;
  switch (v) {

  case log_zero:
    l1 = strlist_append(&lact->actstrlist, "color offled");
    break;

  case log_one:
    l1 = strlist_append(&lact->actstrlist, "color onled");
    break;
  }
  (*lact->hook2->plainxform)(lact->actgate, &x, &y);
  sprintf(STR2, "fillbox %ld %ld %ld %ld", x - 3L, y - 3L, x + 3L, y + 3L);
  l1 = strlist_append(&lact->actstrlist, STR2);
}






/* The following routines control LOG's standard non-GDL gates. */


Void Log_16_ledgate(act)
log_16_action *act;
{
  nodeinfo *pin1;
  log_action *WITH;
  log_grec *WITH1;
  char TEMP;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_sim:
    pin1 = (nodeinfo *)WITH1->pin[0]->info;
    P_clrbits_S(*(unsigned long *)&(WITH1->vars), 0, 1);
    P_putbits_US(*(unsigned long *)&(WITH1->vars), 0,
		 log_16_vi[(long)pin1->v - (long)log_none], 1);
    break;

  case act_16_draw:
    if (WITH->refrflag || P_getbits_US((unsigned long)WITH1->vars, 0, 1) !=
			  P_getbits_US((unsigned long)WITH1->vars, 1, 1)) {
      log_16_led(act->lact, 0, 0,
		 log_16_iv[P_getbits_US((unsigned long)WITH1->vars, 0, 1)]);
      TEMP = P_getbits_US((unsigned long)WITH1->vars, 0, 1);
      P_clrbits_S(*(unsigned long *)&(WITH1->vars), 1, 1);
      P_putbits_US(*(unsigned long *)&(WITH1->vars), 1, TEMP, 1);
    }
    break;

  case act_16_erase:
    log_16_eraled(act->lact, 0, 0);
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    else if (!strcmp(WITH->genfunc, "PLOT"))
      log_16_plotled(act->lact, 0, 0,
	log_16_iv[P_getbits_US((unsigned long)WITH1->vars, 0, 1)]);
    break;
  }
}



Void Log_16_ledgate2(act)
log_16_action *act;
{
  long i;
  nodeinfo *pin1;
  log_action *WITH;
  log_grec *WITH1;
  long FORLIM;
  char TEMP;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_sim:
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 1)));
    FORLIM = WITH1->kind->numpins;
    for (i = 0; i < FORLIM; i++) {
      pin1 = (nodeinfo *)WITH1->pin[i]->info;
      switch (pin1->v) {

      case log_zero:
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (1L << 0));
	break;

      case log_one:
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (1L << 1));
	break;
      }
    }
    if ((((unsigned long)WITH1->vars) & (1L << 0)) != 0 &&
	(((unsigned long)WITH1->vars) & (1L << 1)) != 0) {
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 1)));
    }
    break;

  case act_16_draw:
    if (WITH->refrflag || P_getbits_US((unsigned long)WITH1->vars, 0, 1) !=
			  P_getbits_US((unsigned long)WITH1->vars, 1, 1)) {
      log_16_led(act->lact, 0, 0,
		 log_16_iv[P_getbits_US((unsigned long)WITH1->vars, 0, 1)]);
      TEMP = P_getbits_US((unsigned long)WITH1->vars, 0, 1);
      P_clrbits_S(*(unsigned long *)&(WITH1->vars), 1, 1);
      P_putbits_US(*(unsigned long *)&(WITH1->vars), 1, TEMP, 1);
    }
    break;

  case act_16_erase:
    log_16_eraled(act->lact, 0, 0);
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    else if (!strcmp(WITH->genfunc, "PLOT"))
      log_16_plotled(act->lact, 0, 0,
	log_16_iv[P_getbits_US((unsigned long)WITH1->vars, 0, 1)]);
    break;
  }
}



Void Log_16_clock(act)
log_16_action *act;
{
  log_action *WITH;
  log_grec *WITH1;
  int TEMP;
  long TEMP1;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_sim:
    if (resetcounter > 0)
      WITH1->info = (na_long)0;
    if ((isstable || WITH1->attr[0].UU.nv != 2) && (long)WITH1->info < 1) {
      switch (WITH1->attr[0].UU.nv) {

      case 0:
	if (WITH1->attr[1].UU.U73.i1 == 0)
	  WITH1->info = (na_long)0;
	else
	  WITH1->info = (na_long)(100 / WITH1->attr[1].UU.U73.i1);
	break;

      case 1:
	WITH1->info = (na_long)WITH1->attr[2].UU.U73.i1;
	break;

      case 2:
	WITH1->info = (na_long)WITH1->attr[3].UU.U73.i1;
	break;
      }
      if (resetcounter > 0) {
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (1L << 2));
	P_clrbits_S(*(unsigned long *)&(WITH1->vars), 2, 1);
      } else {
	switch (WITH1->attr[4].UU.nv) {

	case 0:
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) == 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 0));
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) == 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 2)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 2));
	  if ((((unsigned long)WITH1->vars) & (1L << 0)) != 0)
	    WITH1->info = (na_long)((long)WITH1->info / 2);
	  else
	    WITH1->info = (na_long)((long)WITH1->info - (long)WITH1->info / 2);
	  break;

	case 1:
	  TEMP1 = (P_getbits_US((unsigned long)WITH1->vars, 2, 1) + 1) & 3;
	  P_clrbits_S(*(unsigned long *)&(WITH1->vars), 2, 1);
	  P_putbits_US(*(unsigned long *)&(WITH1->vars), 2, TEMP1, 1);
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 4)) != 0 &&
		  (((unsigned long)WITH1->vars) & (1L << 5)) != 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 0));
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 4)) == 0 &&
		  (((unsigned long)WITH1->vars) & (1L << 5)) != 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 2)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 2));
	  if (P_getbits_US((unsigned long)WITH1->vars, 2, 1) > 0 ||
	      (long)WITH1->info < 4)
	    WITH1->info = (na_long)((long)WITH1->info / 4);
	  else
	    WITH1->info = (na_long)((long)WITH1->info - (long)WITH1->info / 4 * 3);
	  break;
	}
      }
    }
    switch (WITH1->attr[0].UU.nv) {

    case 0:
      if (newsystime > oldsystime && newsystime < oldsystime + 10000)
	WITH1->info = (na_long)((long)WITH1->info + oldsystime - newsystime);
      break;

    case 1:
    case 2:
      WITH1->info = (na_long)((long)WITH1->info - 1);
      break;
    }
    break;

  case act_16_draw:
    if (WITH->refrflag || ((((unsigned long)WITH1->vars) & (1L << 1)) != 0) !=
			  ((((unsigned long)WITH1->vars) & (1L << 0)) != 0)) {
      log_16_led(act->lact, 5, -5,
	log_16_bv[((((unsigned long)WITH1->vars) & (1L << 0)) != 0) - false]);
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 1)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 1));
    }
    if (WITH->refrflag || ((((unsigned long)WITH1->vars) & (1L << 3)) != 0) !=
			  ((((unsigned long)WITH1->vars) & (1L << 2)) != 0)) {
      log_16_led(act->lact, 5, 5,
	log_16_bv[((((unsigned long)WITH1->vars) & (1L << 2)) != 0) - false]);
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 2)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 3)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 3));
    }
    break;

  case act_16_erase:
    log_16_eraled(act->lact, 5, -5);
    log_16_eraled(act->lact, 5, 5);
    break;

  case act_16_configchgate:
    switch (WITH->actx) {

    case 2:
    case 3:
    case 4:
      if (WITH1->attr[WITH->actx - 1].UU.U73.i1 <= 0 ||
	  WITH1->attr[WITH->actx - 1].UU.U73.i1 > 32767)
	WITH->actflag = false;
      break;
    }
    WITH1->info = (na_long)10;
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    break;
  }
}



Static Void makeconstpin(act, num, val)
log_16_action *act;
long num;
boolean val;
{
  na_strlistrec *l1;
  Char STR1[256];

  if (act->lact->actgate->pin[num - 1]->ref <= 1)
    return;
  sprintf(STR1, "%ld", num);
  l1 = strlist_append(&act->lact->actstrlist, STR1);
  if (val)
    l1->value = (Anyptr)vddsig->np;
  else
    l1->value = (Anyptr)gndsig->np;
}



Void Log_16_switch(act)
log_16_action *act;
{
  short xx, yy;
  log_action *WITH;
  log_grec *WITH1;
  int TEMP;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_draw:
    if (WITH->refrflag || ((((unsigned long)WITH1->vars) & (1L << 1)) != 0) !=
			  ((((unsigned long)WITH1->vars) & (1L << 0)) != 0)) {
      getpointmark(1, 0, 0, &xx, &yy);
      log_16_led(act->lact, xx, yy,
	log_16_bv[((((unsigned long)WITH1->vars) & (1L << 0)) != 0) - false]);
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 1)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 1));
    }
    break;

  case act_16_erase:
    getpointmark(1, 0, 0, &xx, &yy);
    log_16_eraled(act->lact, xx, yy);
    break;

  case act_16_touch:
    TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) == 0);
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 0));
    WITH->actflag = true;
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT")) {
      WITH->actflag = true;
      strcpy(WITH->actstr, "P");
    } else if (!strcmp(WITH->genfunc, "PLOT")) {
      getpointmark(1, 0, 0, &xx, &yy);
      log_16_plotled(act->lact, xx, yy,
	log_16_bv[((((unsigned long)WITH1->vars) & (1L << 0)) != 0) - false]);
    } else if (!strcmp(WITH->genfunc, "CONSTPINS"))
      makeconstpin(act, 1L, (((unsigned long)WITH1->vars) & (1L << 0)) != 0);
    break;
  }
}



Void Log_16_pulse(act)
log_16_action *act;
{
  log_action *WITH;
  log_grec *WITH1;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  if (act->action != act_16_sim)
    return;
  if ((long)WITH1->info <= 0) {
    if ((((unsigned long)WITH1->vars) & (1L << 0)) != 0)
      WITH1->info = (na_long)6;
    return;
  }
  WITH1->info = (na_long)((long)WITH1->info - 1);
  if ((long)WITH1->info <= 0)
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
}



Void Log_16_break(act)
log_16_action *act;
{
  log_action *WITH;
  log_grec *WITH1;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_sim:
    if ((((unsigned long)WITH1->vars) & (1L << 0)) != 0) {
      WITH->pwrflag = false;
      (*WITH->hook.vmessage)("Simulation is OFF (triggered BREAK gate)");
    }
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    break;
  }
}



Void Log_16_scope(act)
log_16_action *act;
{
  boolean flag;
  nodeinfo *pin1, *pin2;
  short tx, ty;
  log_action *WITH;
  log_grec *WITH1;
  int TEMP;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_draw:
    if (WITH->pwrflag && diggattr[digonoff - 1].UU.nv != 0) {
      pin1 = (nodeinfo *)WITH1->pin[0]->info;
      pin2 = (nodeinfo *)WITH1->pin[1]->info;
      flag = (pin1->v == log_one);
      if ((((unsigned long)WITH1->vars) & (1L << 14)) == 0 || isstable) {
	TEMP = ((((unsigned long)WITH1->vars) & (1L << 8)) == 0);
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 8)));
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 8));
	/*need to look at gate attributes*/
	if ((((unsigned long)WITH1->vars) & (1L << 8)) == 0) {
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 9)) == 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 9)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 9));
	  if ((((unsigned long)WITH1->vars) & (1L << 9)) == 0) {
	    TEMP = ((((unsigned long)WITH1->vars) & (1L << 10)) == 0);
	    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 10)));
	    WITH1->vars =
	      (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 10));
	  }
	}
	if (pin2->v == log_one && (((unsigned long)WITH1->vars) & (1L << 2)) == 0) {
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 10)));
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 9)));
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 8)));
	}
	TEMP = ((((unsigned long)WITH1->vars) & (1L << 0)) != 0 ||
		((((unsigned long)WITH1->vars) & (1L << 1)) != 0) != flag);
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 0));
	if (((((unsigned long)WITH1->vars) & (1L << 11)) == 0 ||
	     (((unsigned long)WITH1->vars) & (1L << 8)) == 0) &&
	    ((((unsigned long)WITH1->vars) & (1L << 12)) == 0 ||
	     (((unsigned long)WITH1->vars) & (1L << 9)) == 0) &&
	    ((((unsigned long)WITH1->vars) & (1L << 13)) == 0 ||
	     (((unsigned long)WITH1->vars) & (1L << 10)) == 0) &&
	    ((long)WITH1->info > 0 ||
	     (pin2->v != log_zero &&
	      (((unsigned long)WITH1->vars) & (1L << 2)) == 0))) {
/* p2c: logsim.text, line 599: 
 * Note: Line breaker spent 2.0 seconds, 5000 tries on line 650 [251] */
	  tx = 0;
	  ty = 0;
	  (*WITH->hook.xform)(WITH->actgate, &tx, &ty);
	  tx += (long)WITH1->info * 2 - 7;
	  (*WITH->hook.hidecursor)();
	  m_color((long)WITH->color.backgr);
	  m_drawline((long)tx, ty - 7L, (long)tx, ty + 6L);
	  m_drawline(tx + 1L, ty - 7L, tx + 1L, ty + 4L);
	  if ((((unsigned long)WITH1->vars) & (1L << 0)) != 0)
	    m_color((long)scopecolor);
	  else
	    m_color((long)WITH->color.backgr);
	  m_drawline((long)tx, ty - 6L, (long)tx, ty + 3L);
	  if (flag)
	    m_color((long)scopecolor);
	  else
	    m_color((long)WITH->color.backgr);
	  m_drawline((long)tx, ty - 7L, tx + 2L, ty - 7L);
	  if (flag)
	    m_color((long)WITH->color.backgr);
	  else
	    m_color((long)scopecolor);
	  m_drawline((long)tx, ty + 4L, tx + 2L, ty + 4L);
	  WITH1->info = (na_long)(((long)WITH1->info + 1) % 13);
/* p2c: logsim.text, line 566:
 * Note: Using % for possibly-negative arguments [317] */
	  tx = 0;
	  ty = 0;
	  (*WITH->hook.xform)(WITH->actgate, &tx, &ty);
	  tx += (long)WITH1->info * 2 - 7;
	  m_color((long)scopescancolor);
	  m_drawline((long)tx, ty + 5L, (long)tx, ty + 6L);
	  (*WITH->hook.unhidecursor)();
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
	}
      }
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 1)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) |
			      (((long)(pin1->v == log_one)) << 1));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 2)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) |
			      (((long)(pin2->v == log_one)) << 2));
    }
    break;

  case act_16_erase:
    m_color((long)WITH->color.backgr);
    tx = 0;
    ty = 0;
    (*WITH->hook.xform)(WITH->actgate, &tx, &ty);
    m_fillrect(tx - 7L, ty - 7L, tx + 19L, ty + 6L);
    break;

  case act_16_reset:
    WITH1->info = (na_long)0;
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    break;
  }
}


/* Local variables for Log_16_7seg: */
struct LOC_Log_16_7seg {
  log_16_action *act;
  na_strlistrec *l1;
} ;

Local Void plotline(x1, y1, x2, y2, LINK)
short x1, y1, x2, y2;
struct LOC_Log_16_7seg *LINK;
{
  Char STR2[256];

  sprintf(STR2, "line %d %d %d %d", x1, y1, x2, y2);
  LINK->l1 = strlist_append(&LINK->act->lact->actstrlist, STR2);
}



Void Log_16_7seg(act_)
log_16_action *act_;
{
  struct LOC_Log_16_7seg V;
  short t1, tx, ty;
  log_action *WITH;
  log_grec *WITH1;
  int TEMP;

  V.act = act_;
  WITH = V.act->lact;
  WITH1 = WITH->actgate;
  switch (V.act->action) {

  case act_16_sim:
    if (((((unsigned long)WITH1->vars) & (1L << 2)) != 0) !=
	((((unsigned long)WITH1->vars) & (1L << 6)) != 0) ||
	((((unsigned long)WITH1->vars) & (1L << 3)) != 0) !=
	((((unsigned long)WITH1->vars) & (1L << 7)) != 0) ||
	((((unsigned long)WITH1->vars) & (1L << 4)) != 0) !=
	((((unsigned long)WITH1->vars) & (1L << 8)) != 0) ||
	((((unsigned long)WITH1->vars) & (1L << 5)) != 0) !=
	((((unsigned long)WITH1->vars) & (1L << 9)) != 0)) {
/* p2c: logsim.text, line 694: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 758 [251] */
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (1L << 0));
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 2)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 6)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 6));
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 3)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 7)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 7));
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 4)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 8)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 8));
      TEMP = ((((unsigned long)WITH1->vars) & (1L << 5)) != 0);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 9)));
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((long)TEMP) << 9));
    }
    break;

  case act_16_draw:
    if (WITH->refrflag || (((unsigned long)WITH1->vars) & (1L << 0)) != 0) {
      (*WITH->hook.hidecursor)();
      tx = 0;
      ty = 0;
      (*WITH->hook.xform)(WITH->actgate, &tx, &ty);
      m_color((long)WITH->color.backgr);
      m_drawline(tx - 5L, ty - 10L, tx + 5L, ty - 10L);
      m_drawline(tx - 5L, (long)ty, tx + 5L, (long)ty);
      m_drawline(tx - 5L, ty + 10L, tx + 5L, ty + 10L);
      m_drawline(tx - 5L, ty - 10L, tx - 5L, ty + 10L);
      m_drawline(tx + 5L, ty - 10L, tx + 5L, ty + 10L);
      m_color((long)ledoncolor);
      t1 = ((((unsigned long)WITH1->vars) & (1L << 6)) != 0) * 8 +
	   ((((unsigned long)WITH1->vars) & (1L << 7)) != 0) * 4 +
	   ((((unsigned long)WITH1->vars) & (1L << 8)) != 0) * 2 +
	   ((((unsigned long)WITH1->vars) & (1L << 9)) != 0);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x2812) == 0)
	m_drawline(tx - 5L, ty - 10L, tx + 5L, ty - 10L);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x1083) == 0)
	m_drawline(tx - 5L, (long)ty, tx + 5L, (long)ty);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x8492L) == 0)
	m_drawline(tx - 5L, ty + 10L, tx + 5L, ty + 10L);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x208e) == 0)
	m_drawline(tx - 5L, ty - 10L, tx - 5L, (long)ty);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x2ba) == 0)
	m_drawline(tx - 5L, (long)ty, tx - 5L, ty + 10L);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0xd860L) == 0)
	m_drawline(tx + 5L, ty - 10L, tx + 5L, (long)ty);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0xd004L) == 0)
	m_drawline(tx + 5L, (long)ty, tx + 5L, ty + 10L);
      WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 0)));
      (*WITH->hook.unhidecursor)();
    }
    break;

  case act_16_erase:
    tx = 0;
    ty = 0;
    (*WITH->hook.xform)(WITH->actgate, &tx, &ty);
    m_color((long)WITH->color.backgr);
    m_drawline(tx - 5L, ty - 10L, tx + 5L, ty - 10L);
    m_drawline(tx - 5L, (long)ty, tx + 5L, (long)ty);
    m_drawline(tx - 5L, ty + 10L, tx + 5L, ty + 10L);
    m_drawline(tx - 5L, ty - 10L, tx - 5L, ty + 10L);
    m_drawline(tx + 5L, ty - 10L, tx + 5L, ty + 10L);
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT"))
      WITH->actflag = true;
    else if (!strcmp(WITH->genfunc, "PLOT")) {
      V.l1 = strlist_append(&WITH->actstrlist, "color onled");
      tx = 0;
      ty = 0;
      (*WITH->hook2->plainxform)(WITH->actgate, &tx, &ty);
      t1 = ((((unsigned long)WITH1->vars) & (1L << 6)) != 0) * 8 +
	   ((((unsigned long)WITH1->vars) & (1L << 7)) != 0) * 4 +
	   ((((unsigned long)WITH1->vars) & (1L << 8)) != 0) * 2 +
	   ((((unsigned long)WITH1->vars) & (1L << 9)) != 0);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x2812) == 0)
	plotline(tx - 5, ty - 10, tx + 5, ty - 10, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x1083) == 0)
	plotline(tx - 5, ty, tx + 5, ty, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x8492L) == 0)
	plotline(tx - 5, ty + 10, tx + 5, ty + 10, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x208e) == 0)
	plotline(tx - 5, ty - 10, tx - 5, ty, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0x2ba) == 0)
	plotline(tx - 5, ty, tx - 5, ty + 10, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0xd860L) == 0)
	plotline(tx + 5, ty - 10, tx + 5, ty, &V);
      if ((unsigned)t1 >= 32 || ((1L << t1) & 0xd004L) == 0)
	plotline(tx + 5, ty, tx + 5, ty + 10, &V);
    }
    break;
  }
}



Void Log_16_keypad(act)
log_16_action *act;
{
  short x1, y1, n;
  log_action *WITH;
  log_grec *WITH1;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_touch:
    x1 = (WITH->actx + 4) / 2;
    y1 = (WITH->acty + 4) / 2;
    switch (y1 * 10 + x1) {

    case 0:
      n = 7;
      break;

    case 1:
      n = 8;
      break;

    case 2:
      n = 9;
      break;

    case 3:
      n = 15;
      break;

    case 10:
      n = 4;
      break;

    case 11:
      n = 5;
      break;

    case 12:
      n = 6;
      break;

    case 13:
      n = 14;
      break;

    case 20:
      n = 1;
      break;

    case 21:
      n = 2;
      break;

    case 22:
      n = 3;
      break;

    case 23:
      n = 13;
      break;

    case 30:
      n = 0;
      break;

    case 31:
      n = 10;
      break;

    case 32:
      n = 11;
      break;

    case 33:
      n = 12;
      break;

    default:
      n = 0;
      break;
    }
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 2)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((n / 8L) & 1) << 2));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 3)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((n / 4L) & 1) << 3));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 4)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (((n / 2L) & 1) << 4));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 5)));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | ((n & 1L) << 5));
    WITH1->vars = (na_long)(((unsigned long)WITH1->vars) | (1L << 0));
    WITH->actflag = true;
    break;

  case act_16_gengate:
    if (!strcmp(WITH->genfunc, "INERT")) {
      WITH->actflag = true;
      strcpy(WITH->actstr, "KLMNP");
    } else if (!strcmp(WITH->genfunc, "CONSTPINS")) {
      makeconstpin(act, 1L, (((unsigned long)WITH1->vars) & (1L << 5)) != 0);
      makeconstpin(act, 2L, (((unsigned long)WITH1->vars) & (1L << 4)) != 0);
      makeconstpin(act, 3L, (((unsigned long)WITH1->vars) & (1L << 3)) != 0);
      makeconstpin(act, 4L, (((unsigned long)WITH1->vars) & (1L << 2)) != 0);
      makeconstpin(act, 5L, (((unsigned long)WITH1->vars) & (1L << 0)) != 0);
    }
    break;
  }
}









/* The following routines run the digital simulation. */


Static short digglowcol(n, nonecolor)
log_nrec *n;
short nonecolor;
{
  short Result;
  nodeinfo *ni;

  ni = (nodeinfo *)n->info;
  switch (ni->v) {

  case log_none:
    Result = nonecolor;
    break;

  case log_zero:
    Result = ledoffcolor;
    break;

  case log_one:
    Result = ledoncolor;
    break;
  }
  return Result;
}


Static boolean digtrigger(n)
log_nrec *n;
{
  boolean Result;
  nodeinfo *ni;

  ni = (nodeinfo *)n->info;
  Result = false;
  if ((ni->v00 == log_zero) && (ni->v == log_one))
      Result = true;
  return Result;
}



Static Void callallgates(kind)
log_16_actionkinds kind;
{
  short pg;
  log_grec *g, *saveactgate;
  log_action *WITH;
  short FORLIM;
  log_krec *WITH1;

  WITH = logsima_action.lact;
  saveactgate = WITH->actgate;
  FORLIM = WITH->numpages;
  for (pg = 0; pg < FORLIM; pg++) {
    g = WITH->gbase[pg];
    while (g != NULL) {
      WITH1 = g->kind;
      if (WITH1->simtype == logsima_tool_16 && ((kindinfo *)WITH1->info)->hasproc) {
	WITH->actgate = g;
	callgate(kind);
      }
      g = g->next;
    }
  }
  WITH->actgate = saveactgate;
}



Static Void callkind(kind)
log_16_actionkinds kind;
{
  kindinfo *ki;
  log_grec *saveactgate;
  log_action *WITH;
  log_krec *WITH1;

  WITH = logsima_action.lact;
  WITH1 = WITH->actkind;
  saveactgate = WITH->actgate;
  WITH->actgate = (log_grec *)WITH->actkind;
      /*just to satisfy "with actgate^" statements*/
  logsima_action.action = kind;
  ki = (kindinfo *)WITH1->info;
  (*ki->kindproc)(&logsima_action);
  WITH->actgate = saveactgate;
}



Static Void callallkinds(kind)
log_16_actionkinds kind;
{
  kindinfo *ki;
  log_krec *k;
  log_action *WITH;

  WITH = logsima_action.lact;
  k = kbase_16;
  while (k != NULL) {
    ki = (kindinfo *)k->info;
    if (ki->hasproc) {
      WITH->actkind = k;
      callkind(kind);
    }
    k = ki->knext;
  }
}



Static Void pass_16(act)
log_16_action *act;
{
  short pg;
  log_grec *g;
  log_action *WITH;
  short FORLIM;
  log_krec *WITH1;

  WITH = act->lact;
  oldsystime = newsystime;
  newsystime = timers_sysclock();
  if (!passready || WITH->resetflag) {
    if (vddsig == NULL)
      (*WITH->hook.getsig)("Vdd", &vddsig);
    if (gndsig == NULL)
      (*WITH->hook.getsig)("Gnd", &gndsig);
    if (resetsig == NULL)
      (*WITH->hook.getsig)("Reset", &resetsig);
    if (WITH->resetflag) {
      callallgates(act_16_reset);
      resetcounter = 10;
    }
    if (WITH->pwrflag) {
      maketimebid = false;
      if (diggattr[digonoff - 1].UU.nv != 0) {
	(*WITH->hook.clearconflicts)(logsima_tool_16);
	clearnodes(WITH->nbase);
	if (vddsig->np->simtype == logsima_tool_16)
	  log_16_output(act->lact, vddsig->np, log_one);
	if (gndsig->np->simtype == logsima_tool_16)
	  log_16_output(act->lact, gndsig->np, log_zero);
	if (resetsig->np->simtype == logsima_tool_16)
	  log_16_output(act->lact, resetsig->np,
			log_16_bv[(resetcounter > 0) - false]);
	FORLIM = WITH->numpages;
	for (pg = 0; pg < FORLIM; pg++)
	  executegates(&maketimebid, WITH->gbase[pg]);
	nexttimebid = diggattr[digtimestep - 1].UU.r;
      }
      passready = maketimebid;
    }
  }
  if ( WITH->pwrflag && maketimebid )
    WITH->busyflag = true;
  if (maketimebid)
    logsima_tool_16->nexttstep = nexttimebid;
  if (stabilizing && (isstable || timers_sysclock() > stabtime))
    stabilizing = false;
  if (WITH->showpage > 0) {
    g = WITH->gbase[WITH->showpage - 1];
    while (g != NULL) {
      WITH1 = g->kind;
      if (WITH1->simtype == logsima_tool_16 && ((kindinfo *)WITH1->info)->hasproc) {
	WITH->actgate = g;
	callgate(act_16_draw);
      }
      g = g->next;
    }
  }
  if (WITH->probenode != NULL && WITH->probenode->simtype == logsima_tool_16)
    WITH->baselinecolor = digglowcol(WITH->probenode, WITH->color.wire[0]);
}




Static Void tstep_16(act)
log_16_action *act;
{
  log_nrec *n;
  nodeinfo *ni;
  boolean stable;
  log_action *WITH;

  WITH = act->lact;
  if (!WITH->actflag)
    return;
  stable = true;
  copynodes(WITH->nbase, &stable);
  if (stabilizing) {
    n = WITH->nbase;
    while (n != NULL) {
      if (n->simtype == logsima_tool_16 && P_rand(&WITH->rndseed, 5L) == 0) {
	ni = (nodeinfo *)n->info;
	ni->v = ni->v00;
      }
      n = n->next;
    }
  }
  isstable = stable;
  passready = false;
  if (resetcounter > 0)
    resetcounter--;
}





/* The following procedures are exported for the LOGSIMA module to use. */


Void log_16_conf(act, n)
log_16_action *act;
log_nrec *n;
{
  (*act->lact->hook.nodeconflict)(n);
}


Void log_16_noprog(act)
log_16_action *act;
{
  Char STR1[44];

  if (!noprogloaded) {
    sprintf(STR1, "No program loaded for digital gate %s",
	    act->lact->actkind->name);
    (*act->lact->hook.message)(STR1);
    noprogloaded = true;
  }
}


Static Void nullkindproc(act)
log_16_action *act;
{
  /* ignore all calls */
}






/* This procedure runs the "status" screen. */


Static Void status_16(flag)
boolean *flag;
{
  Char s[256];
  log_action *WITH;

  WITH = logsima_action.lact;
  nk_gotoxy(0, 4);
  (*WITH->hook.realunit)(nexttimebid, 4, "s", s);
  printf("Next time bid: %s\t\n", s);
  printf("Pass ready:    %5s\n", passready ? "TRUE" : "FALSE");
  printf("Stabilizing:   %5s\n", stabilizing ? "TRUE" : "FALSE");
  WITH->actflag = false;
}





/* These routines handle the simtype 16 interface to LOG. */


Static Void parseicommand(lact)
log_action *lact;
{
  boolean flag;
  log_action *WITH;

  WITH = lact;
  if (!strcmp(WITH->func, "DIGON")) {
    (*WITH->hook.clearfunc)();
    diggattr[digonoff - 1].UU.nv = 1;
    (*WITH->hook.vmessage)("Digital simulation is ON");
    return;
  }
  if (!strcmp(WITH->func, "DIGOFF")) {
    (*WITH->hook.clearfunc)();
    diggattr[digonoff - 1].UU.nv = 0;
    (*WITH->hook.vmessage)("Digital simulation is OFF");
    return;
  }
  if (!strcmp(WITH->func, "DIGONOFF")) {
    flag = (diggattr[digonoff - 1].UU.nv != 0);
    (*WITH->hook.getbool)(WITH->funcarg, &flag);
    diggattr[digonoff - 1].UU.nv = flag;
    (*WITH->hook.clearfunc)();
    (*WITH->hook.vmessageflag)("Digital simulation is ",
			       diggattr[digonoff - 1].UU.nv != 0);
    return;
  }
  if (!strcmp(WITH->func, "STABILIZE")) {
    (*WITH->hook.clearfunc)();
    stabtime = timers_sysclock() +
	       (long)floor(diggattr[stabdelay - 1].UU.r * 100 + 0.5);
    stabilizing = true;
    return;
  }
  if (!strcmp(WITH->func, "DIGSTEP")) {
    (*WITH->hook.getreal)(WITH->funcarg, &diggattr[digtimestep - 1].UU.r,
			  1e-8);
    (*WITH->hook.clearfunc)();
  }
}


Static Void parsecommand(lact)
log_action *lact;
{
  log_action *WITH;

  WITH = lact;
  if (!strcmp(WITH->func, "STABDELAY")) {
    (*WITH->hook.getreal)(WITH->funcarg, &diggattr[stabdelay - 1].UU.r, 5.0);
    (*WITH->hook.clearfunc)();
  }
}


Static Char *log_actionkinds_NAMES[] = {
  "ACT_INIT", "ACT_ENDINIT", "ACT_EXIT", "ACT_CLEARMSG", "ACT_STATUS",
  "ACT_CNF", "ACT_IMMED", "ACT_FUNC", "ACT_COLOR", "ACT_SELECT", "ACT_CLEAR",
  "ACT_EDIT", "ACT_PASS", "ACT_TSTEP", "ACT_ERASEGATE", "ACT_TOUCHGATE",
  "ACT_HISTORY", "ACT_HISTVAL", "ACT_HISTSTR", "ACT_TRIGGER", "ACT_GLOWCOL",
  "ACT_NEWGATE", "ACT_DISPOSEGATE", "ACT_COPYGATE", "ACT_WRITEGATE",
  "ACT_READGATE", "ACT_CONNECTGATE", "ACT_DISCONNECTGATE", "ACT_CONFIGGATE",
  "ACT_CONFIGCHGATE", "ACT_CONFIGRELGATE", "ACT_CONFIGNODE",
  "ACT_CONFIGCHNODE", "ACT_CONFIGRELNODE", "ACT_CONFIGHIST",
  "ACT_CONFIGCHHIST", "ACT_CONFIGRELHIST", "ACT_NEWKIND", "ACT_DISPOSEKIND",
  "ACT_NEWNODE", "ACT_DISPOSENODE", "ACT_COPYNODE", "ACT_COMBINENODES",
  "ACT_COMBINEINTONODE", "ACT_WRITENODE", "ACT_READNODE", "ACT_REFNODES",
  "ACT_NODEVAL", "ACT_GENERAL", "ACT_GENNODE", "ACT_GENKIND", "ACT_GENGATE"
} ;



Static Char *actionname(Result, action)
Char *Result;
log_actionkinds action;
{
  long i;
  Char s[33];

/* p2c: logsim.text, line 1069: Note:
 * Line breaker spent 0.0+2.00 seconds, 5000 tries on line 1304 [251] */
  strcpy(s, log_actionkinds_NAMES[(long)action]);
  i = strlen(s) + 1;
  s[i - 1] = '\0';
/* p2c: logsim.text, line 1070:
 * Note: Modification of string length may translate incorrectly [146] */
  return strcpy(Result, s);
}



Static Void showgatepins(g)
log_grec *g;
{
  short i, FORLIM;
  Char STR1[256];

  FORLIM = g->kind->numpins;
  for (i = 1; i <= FORLIM; i++) {
    sprintf(STR1, "%d", i);
    (*logsima_action.lact->hook2->showpinname)(g, i,
      logsima_action.lact->color.backgr, STR1);
  }
  FORLIM = g->kind->numpins;
  for (i = 1; i <= FORLIM; i++) {
    sprintf(STR1, "%d", i);
    (*logsima_action.lact->hook2->showpinname)(g, i,
      logsima_action.lact->color.pinnum, STR1);
  }
}



Static Void newnode_16(lact, n)
log_action *lact;
log_nrec **n;
{
  log_action *WITH;

  WITH = lact;
  (*WITH->hook.newnode)(n, log_16_simtype);
  (*n)->keep = true;
  (*n)->ref = 1;
}


Static Void disposenode_16(lact, n)
log_action *lact;
log_nrec **n;
{
  log_action *WITH;

  WITH = lact;
  (*n)->keep = false;
  (*WITH->hook.switchnode)(n, NULL);
}



Static Void digproc1()
{
}


Static Void digproc2()
{
  log_gattrrec *WITH;

  WITH = &diggattr[logsima_action.lact->actx - 1];
  switch (logsima_action.lact->actx) {

  case 1:
  case 2:
    if (WITH->UU.r <= 0)
      logsima_action.lact->actflag = false;
    break;
  }
}


Static Void digproc3()
{
}


/* Local variables for Log_16_proc: */
struct LOC_Log_16_proc {
  kindinfo *ki;
} ;

Local Void setupkindproc(LINK)
struct LOC_Log_16_proc *LINK;
{
  short pc, i, len;
  Char procname[256];
  log_krec *WITH;

  WITH = logsima_action.lact->actkind;
  pc = 1;
  while (WITH->proc[pc - 1] != '\0' && WITH->proc[pc - 1] != '\022')
    pc++;
  if (WITH->proc[pc - 1] != '\022') {
    LINK->ki->hasproc = false;
    LINK->ki->kindproc = nullkindproc;
    return;
  }
  pc += 2;
  len = WITH->proc[pc - 1] - 128;
  procname[len] = '\0';
/* p2c: logsim.text, line 1155:
 * Note: Modification of string length may translate incorrectly [146] */
  for (i = 0; i < len; i++)
    procname[i] = WITH->proc[pc + i];
  LINK->ki->hasproc = newci_findprocedure(procname,
      (Void(**) PV())(&LINK->ki->kindproc));
  if (!LINK->ki->hasproc)
    LINK->ki->kindproc = log_16_noprog;
}



Void Log_16_proc(lact)
log_action *lact;
{
  struct LOC_Log_16_proc V;
  nodeinfo *ni, *ni2;
  log_krec *k2;
  gateinfo *gi, *gi2;
  na_strlistrec *l1;
  short i;
  double val1, val2;
  Char STR1[33];
  Char STR2[46];
  Char STR3[40];
  log_action *WITH;
  Void (*TEMP) PV();
  Void (*TEMP1) PV();
  Void (*TEMP2) PV();
  short FORLIM;

  logsima_action.lact = lact;
  if (lact->traceflag && traceactions && ((unsigned long)lact->action >= 32 ||
	((1L << ((long)lact->action)) &
	 ((1L << ((long)act_pass)) | (1L << ((long)act_tstep)))) == 0)) {
    sprintf(STR2, "log_16 gets %s", actionname(STR1, lact->action));
    (*lact->hook.trace)(STR2);
  }
  WITH = lact;
  switch (WITH->action) {

  case act_init:
    logsima_tool_16 = WITH->acttool;
    WITH->acttool->ready = true;
    WITH->acttool->simulator = true;
    strcpy(WITH->acttool->shortname, "Digital");
    if (*WITH->acttool->comment == '\0')
      strcpy(WITH->acttool->comment, "Digital simulator");
    logsima_init();
    WITH->rndseed = timers_sysclock();
    newsystime = 0;
    vddsig = NULL;
    gndsig = NULL;
    resetsig = NULL;
    resetcounter = 0;
    isstable = false;
    stabilizing = false;
    passready = false;
    maketimebid = false;
    noprogloaded = false;
    kbase_16 = NULL;
    logsima_action.hook_input = log_16_input;
    logsima_action.hook_output = log_16_output;
    logsima_action.hook_ocoutput = log_16_ocoutput;
    logsima_action.hook_led = log_16_led;
    logsima_action.hook_eraled = log_16_eraled;
    logsima_action.hook_plotled = log_16_plotled;
    l1 = strlist_append(&WITH->acttool->hlbl, "Digital signal trace");
    l1 = strlist_append(&WITH->acttool->hlbl, "");
    l1 = strlist_append(&WITH->acttool->hlbl,
	"2VCyan,Orange,Yellow,Pink,Green,Red,White,Medium Red,Light Gray,Dark Cyan,Dark Yellow,Dark Red:Color:");
    l1 = strlist_append(&WITH->acttool->hlbl, "BY:Phosphor visible:");
    l1 = strlist_append(&WITH->acttool->hlbl, "");
    l1 = strlist_append(&WITH->acttool->hlbl, "R.8:Signal height:");
    strlist_init(&diglbl);
    l1 = strlist_append(&diglbl, "Digital simulator");
    l1 = strlist_append(&diglbl, "");
    l1 = strlist_append(&diglbl, "Us,10ns:Timestep:");
    l1 = strlist_append(&diglbl, "");
    l1 = strlist_append(&diglbl, "2R5:Stabilize time:");
    l1 = strlist_append(&diglbl, "");
    l1 = strlist_append(&diglbl, "1VOff,On:Digital simulation:");
    (*WITH->hook.parselabel)(&diglbl, &dignumattrs, &digkattr);
    (*WITH->hook.newattrs)(&diggattr, dignumattrs, digkattr);
    break;

  case act_clearmsg:
    noprogloaded = false;
    break;

  case act_immed:
    parseicommand(lact);
    if (*lact->func != '\0')
      callallkinds(act_16_immed);
    break;

  case act_func:
    parsecommand(lact);
    if (*lact->func != '\0')
      callallkinds(act_16_func);
    break;

  case act_cnf:
    parsecommand(lact);
    if (*lact->func != '\0')
      callallkinds(act_16_cnf);
    break;

  case act_select:
    TEMP = digproc1;
    TEMP1 = digproc2;
    TEMP2 = digproc3;
    (*WITH->hook.editattrs)(diggattr, dignumattrs, digkattr, diglbl,
			    "Digital", TEMP, TEMP1, TEMP2);
    break;

  case act_status:
    status_16(&WITH->actflag);
    break;

  case act_color:
    backgrcolor = WITH->color.backgr;
    (*WITH->hook.getcolor)("ONLED", &ledoncolor, log_red);
    (*WITH->hook.getcolor)("OFFLED", &ledoffcolor, log_black);
    (*WITH->hook.getcolor)("SCOPE", &scopecolor, log_yellow);
    (*WITH->hook.getcolor)("SCOPESCAN", &scopescancolor, log_white);
    break;

  case act_newkind:
    V.ki = (kindinfo *)Malloc(sizeof(kindinfo));
    V.ki->info = NULL;
    V.ki->numppins = 0;
    V.ki->numpvars = 0;
    if (WITH->actkind->proc[0] == '\027') {
      V.ki->instance = true;
      if (WITH->actkind->proc[1] > '"')
	V.ki->numppins = (WITH->actkind->proc[3] - 32) * 128 +
			 WITH->actkind->proc[2] - 64;
      if (WITH->actkind->proc[1] > '$')
	V.ki->numpvars = (WITH->actkind->proc[5] - 32) * 128 +
			 WITH->actkind->proc[4] - 64;
    } else
      V.ki->instance = false;
    setupkindproc(&V);
    V.ki->knext = kbase_16;
    kbase_16 = WITH->actkind;
    WITH->actkind->info = (Anyptr)V.ki;
    callkind(act_16_newkind);
    break;

  case act_disposekind:
    V.ki = (kindinfo *)WITH->actkind->info;
    if (WITH->actkind == kbase_16)
      kbase_16 = ((kindinfo *)kbase_16->info)->knext;
    else {
      k2 = kbase_16;
      while (k2 != NULL && ((kindinfo *)k2->info)->knext != WITH->actkind)
	k2 = ((kindinfo *)k2->info)->knext;
      if (k2 != NULL)
	((kindinfo *)k2->info)->knext = V.ki->knext;
    }
    callkind(act_16_disposekind);
    Free(V.ki);
    break;

  case act_edit:
    switch (WITH->acty) {

    case 0:
      edit_16(&WITH->actproc, &WITH->actx, WITH->actstr);
      break;

    case 1:
    case 2:
      dump_16(&WITH->actproc, &WITH->actstrlist, WITH->acty == 1);
      break;

    case 3:
      read_16(&WITH->actproc, &WITH->actx, WITH->actstrlist);
      break;
    }
    break;

  case act_pass:
    pass_16(&logsima_action);
    break;

  case act_tstep:
    tstep_16(&logsima_action);
    break;

  case act_erasegate:
    callgate(act_16_erase);
    break;

  case act_glowcol:
    WITH->actx = digglowcol(WITH->actnode, (int)WITH->actx);
    break;

  case act_trigger:
    WITH->actflag = digtrigger(WITH->actnode);
    break;

  case act_history:
  case act_nodeval:
    ni = (nodeinfo *)WITH->actnode->info;
    if (ni->v == ni->v00 || logsima_tool_16->deltatime == 0.0 || !WITH->pwrflag) {
      switch (ni->v00) {

      case log_none:
	WITH->actval = 0.0;
	break;

      case log_zero:
	WITH->actval = -0.5;
	break;

      case log_one:
	WITH->actval = 0.5;
	break;
      }
    } else {
      switch (ni->v) {

      case log_none:
	val1 = 0.0;
	break;

      case log_zero:
	val1 = -0.5;
	break;

      case log_one:
	val1 = 0.5;
	break;
      }
      switch (ni->v00) {

      case log_none:
	val2 = 0.0;
	break;

      case log_zero:
	val2 = -0.5;
	break;

      case log_one:
	val2 = 0.5;
	break;
      }
      WITH->actval = val2 +
		     (val1 - val2) * logsima_tool_16->deltatime / nexttimebid;
    }
    if (WITH->action == act_nodeval)
      WITH->actval += 0.5;
    break;

  case act_histval:
    WITH->actval *= WITH->actgattr[histsize - 1].UU.r;
    if (!WITH->actgattr[histvis - 1].UU.b)
      WITH->acty = -1;
    else
      WITH->acty = histcolortable[WITH->actgattr[histcolor - 1].UU.nv];
    break;

  case act_histstr:
    if ((unsigned long)WITH->acty < 32 && ((1L << WITH->acty) & 0x6) != 0 &&
	WITH->actval > -1 && WITH->actval < 1) {
      if (WITH->actval > 0.2)
	strcpy(WITH->actstr, "One");
      else if (WITH->actval < -0.2)
	strcpy(WITH->actstr, "Zero");
      else
	strcpy(WITH->actstr, "Not driven");
    }
    break;

  case act_touchgate:
    callgate(act_16_touch);
    break;

  case act_newgate:
    V.ki = (kindinfo *)WITH->actgate->kind->info;
    if (V.ki->instance) {
      gi = (gateinfo *)Malloc(sizeof(gateinfo));
      gi->ppins = (log_nrec **)Malloc(V.ki->numppins * sizeof(log_nrec *));

      /* code initializing ppins should go here */

      gi->pvars = (Char *)Malloc((long)V.ki->numpvars);
      if (V.ki->numpvars != 0)
	memset((Anyptr)gi->pvars, 0, (long)V.ki->numpvars);
      WITH->actgate->info = (Anyptr)gi;

    }
    callgate(act_16_new);
    break;

  case act_disposegate:
    V.ki = (kindinfo *)WITH->actgate->kind->info;
    if (V.ki->instance) {
      gi = (gateinfo *)WITH->actgate->info;

      /* code disposing ppins should go here */

      Free(gi->ppins);
      Free(gi->pvars);
      Free(gi);
    }
    callgate(act_16_dispose);
    break;

  case act_copygate:
    V.ki = (kindinfo *)WITH->actgate->kind->info;
    if (V.ki->instance) {
      gi = (gateinfo *)Malloc(sizeof(gateinfo));
      gi2 = (gateinfo *)WITH->actgate2->info;
      gi->ppins = (log_nrec **)Malloc(V.ki->numppins * sizeof(log_nrec *));

      /* code initializing ppins should go here */

      gi->pvars = (Char *)Malloc((long)V.ki->numpvars);
      if (V.ki->numpvars != 0)
	memmove((Anyptr)gi2->pvars, (Anyptr)gi->pvars, (long)V.ki->numpvars);
      WITH->actgate->info = (Anyptr)gi;
    }
    callgate(act_16_copy);
    break;

  case act_writegate:
    callgate(act_16_write);
    break;

  case act_readgate:
    callgate(act_16_read);
    break;

  case act_connectgate:
    callgate(act_16_connect);
    V.ki = (kindinfo *)WITH->actgate->kind->info;
    if (V.ki->instance) {
      gi = (gateinfo *)WITH->actgate->info;
      FORLIM = V.ki->numppins;
      for (i = 0; i < FORLIM; i++)   /*this calls Log_16_proc recursively*/
	newnode_16(lact, &gi->ppins[i]);
    }
    break;

  case act_disconnectgate:
    callgate(act_16_disconnect);
    V.ki = (kindinfo *)WITH->actgate->kind->info;
    if (V.ki->instance) {
      gi = (gateinfo *)WITH->actgate->info;
      FORLIM = V.ki->numppins;
      for (i = 0; i < FORLIM; i++)   /*this calls Log_16_proc recursively*/
	disposenode_16(lact, &gi->ppins[i]);
    }
    break;

  case act_configgate:
    callgate(act_16_configgate);
    break;

  case act_configchgate:
    callgate(act_16_configchgate);
    break;

  case act_configrelgate:
    callgate(act_16_configrelgate);
    break;

  case act_configchhist:
    /* blank case */
    break;

  case act_newnode:
    ni = (nodeinfo *)Malloc(sizeof(nodeinfo));
    WITH->actnode->info = (Anyptr)ni;
    ni->v = log_none;
    ni->v0 = log_none;
    ni->v00 = log_none;
    ni->defv = log_none;
    ni->truev = log_none;
    break;

  case act_disposenode:
    ni = (nodeinfo *)WITH->actnode->info;
    Free(ni);
    break;

  case act_copynode:
    ni = (nodeinfo *)Malloc(sizeof(nodeinfo));
    ni2 = (nodeinfo *)WITH->actnode2->info;
    *ni = *ni2;
    WITH->actnode->info = (Anyptr)ni;
    break;

  case act_combineintonode:
    /* blank case */
    break;

  case act_combinenodes:
    ni = (nodeinfo *)WITH->actnode->info;
    ni2 = (nodeinfo *)WITH->actnode2->info;
    ni->v = bcomb[(long)ni->v - (long)log_none][(long)ni2->v - (long)log_none];
    ni->v00 = ni->v;
    break;

  case act_writenode:
    ni = (nodeinfo *)WITH->actnode->info;
    fprintf(*WITH->actfile, "%d\n", (int)ni->v);
    break;

  case act_readnode:
    ni = (nodeinfo *)WITH->actnode->info;
    fscanf(*WITH->actfile, "%hd%*[^\n]", &i);
    getc(*WITH->actfile);
    ni->v = log_16_iv[i];
    ni->v00 = ni->v;
    break;

  case act_refnodes:
    callallgates(act_16_refnodes);
    break;

  case act_general:
    /* blank case */
    break;

  /* nothing to do */
  case act_gennode:
    break;

  /* nothing to do */
  case act_genkind:
    if (WITH->actkind == NULL)
      callallkinds(act_16_genkind);
    else if (!strcmp(WITH->genfunc, "DUMPKIND")) {
      callkind(act_16_genkind);
      if (WITH->actstrlist == NULL) {
	sprintf(STR3, "Definition for digital gate %s:", WITH->actkind->name);
	l1 = strlist_insert(&WITH->actstrlist, STR3);
	dump_16(&WITH->actkind->proc, &l1->next, true);
      }
    } else
      callkind(act_16_genkind);
    break;

  case act_gengate:   /*ignore*/
    if (WITH->actgate == NULL)
      callallgates(act_16_gengate);
    else if (!strcmp(WITH->genfunc, "SHOWPINS")) {
      callgate(act_16_gengate);
      if (!WITH->actflag) {
	showgatepins(WITH->actgate);
	WITH->actflag = true;
      }
    } else
      callgate(act_16_gengate);
    break;
  }
}











/* End. */
