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

/* Output from p2c, the Pascal-to-C translator */
/* From input file "tools.text" */



#include <p2c/p2c.h>


#define NUMBERS_G
#include "numbers.h"


#define TitleX          (-80)
#define InfoX           (-49)
#define Line1Y          41
#define Line2Y          52
#define Line3Y          63
#define Line4Y          74
#define Line5Y          85
#define Line6Y          96

#define GateName        "anaLOG status"




typedef struct NumbersConst {
  Char OldMessScoreboard[256], OldMessNode[256], OldMessGate1[256],
       OldMessGate2[256], OldMessGate3[256], OldMessGate4[256],
       OldMessGate5[256];
  boolean OldErase, OldExact, OldProbeOn;
} NumbersConst;


typedef struct ICNumbers {
  Char InitMessScoreboard[256], InitMessNode[256], InitMessGate1[256],
       InitMessGate2[256], InitMessGate3[256], InitMessGate4[256],
       InitMessGate5[256];
} ICNumbers;


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/


Local Void GetCnf_Numbers(NewIC)
ICNumbers *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  ICNumbers *WITH;

  WITH = NewIC;
  do {
    AnaGetcommand("NUMBERS", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "MESSAGE")) {
	TRY(try1);
	  strcpy(WITH->InitMessScoreboard, Arg);
	RECOVER(try1);
	  /* Strread(Arg,1,Dummy,InitMessScoreboard); */
	  strcpy(WITH->InitMessScoreboard, "Welcome To anaLOG");
	ENDTRY(try1);
      }
    }
  } while (Found);
}

Local Void initlib_NUMBERS(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICNumbers *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->plist_cell = NULL;
  NewCell->phase = Anasimple;
  NewIC = (ICNumbers *)Malloc(sizeof(ICNumbers));
  strcpy(NewIC->InitMessScoreboard, "Welcome to anaLOG");
  strcpy(NewIC->InitMessNode, " <Not on a Node> ");
  strcpy(NewIC->InitMessGate1, " <Not on a Gate> ");
  strcpy(NewIC->InitMessGate2, "  ");
  strcpy(NewIC->InitMessGate3, "  ");
  strcpy(NewIC->InitMessGate4, "  ");
  strcpy(NewIC->InitMessGate5, "  ");
  GetCnf_Numbers(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
  act->kind->x1 = -60;
  act->kind->x2 = 60;
  act->kind->y1 = -20;
  act->kind->y2 = 35;
}  /*NFET4_Initlib*/

Local Void Attr_Numbers(act)
Analog_32_action *act;
{
  long Attrnum;
  log_grec *WITH;
  log_gattrrec *WITH1;

  WITH = act->inst;
  Attrnum = act->pin;
  act->ok = true;
  switch (Attrnum) {

  case AnaN_Vdd:
  case AnaN_Lowest:
  case AnaN_Small:
  case AnaN_Tol:
  case AnaN_Eqn_Epsilon:
  case AnaN_Vardone_Epsilon:
  case AnaN_Timefactor:
  case AnaN_Nodecap:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	  WITH->attr[Attrnum - 1].UU.r < AnaLimits[Attrnum - 1]
	    [(long)AnaMin].r || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      switch (Attrnum) {

      case AnaN_Vdd:
	WITH1 = &WITH->attr[AnaN_Highest - 1];
	WITH1->changed = true;
	WITH1->UU.r += WITH->attr[Attrnum - 1].UU.r - AnaVdd;
	AnaHighest = WITH1->UU.r;
	AnaVdd = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Lowest:
	AnaLowest = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Small:
	AnaSmall = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Tol:
	AnaTol = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Eqn_Epsilon:
	AnaEqn_Epsilon = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Vardone_Epsilon:
	AnaVardone_Epsilon = WITH->attr[Attrnum - 1].UU.r;
	break;

      case AnaN_Timefactor:
	AnaTimefactor = WITH->attr[Attrnum - 1].UU.r;
	AnaTimeReset();
	break;

      case AnaN_Nodecap:
	AnaNodecap = WITH->attr[Attrnum - 1].UU.r;
	break;
      }
    }
    break;

  case AnaN_Highest:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	WITH->attr[Attrnum - 1].UU.r < AnaVdd || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      AnaHighest = WITH->attr[Attrnum - 1].UU.r;
    break;

  case AnaN_Vstep:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	  WITH->attr[Attrnum - 1].UU.r <= AnaLimits[Attrnum - 1]
	    [(long)AnaMin].r || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      AnaVstep = WITH->attr[Attrnum - 1].UU.r;
      AnaVshift = AnaVstep / AnaSrange;
    }
    break;

  case AnaN_Srange:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	  WITH->attr[Attrnum - 1].UU.r < AnaLimits[Attrnum - 1]
	    [(long)AnaMin].r || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      AnaSrange = WITH->attr[Attrnum - 1].UU.r;
      AnaVshift = AnaVstep / AnaSrange;
    }
    break;

  case AnaN_Timerelax:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	  WITH->attr[Attrnum - 1].UU.r < AnaLimits[Attrnum - 1]
	    [(long)AnaMin].r || WITH->attr[Attrnum - 1].UU.r >= AnaDtmax ||
	  WITH->attr[Attrnum - 1].UU.r <= AnaTimemin ||
	  WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      AnaTimerelax = WITH->attr[Attrnum - 1].UU.r;
      AnaTimeReset();
    }
    break;

  case AnaN_Dtmax:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	WITH->attr[Attrnum - 1].UU.r < AnaLimits[Attrnum - 1][(long)AnaMin].r ||
	WITH->attr[Attrnum - 1].UU.r <= AnaTimerelax ||
	WITH->attr[Attrnum - 1].UU.r <= AnaTimemin || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      AnaDtmax = WITH->attr[Attrnum - 1].UU.r;
      AnaTimeReset();
    }
    break;

  case AnaN_Timemin:
    if (WITH->attr[Attrnum - 1].UU.r > AnaLimits[Attrnum - 1][(long)AnaMax].r ||
	WITH->attr[Attrnum - 1].UU.r < AnaLimits[Attrnum - 1][(long)AnaMin].r ||
	WITH->attr[Attrnum - 1].UU.r >= AnaTimerelax ||
	WITH->attr[Attrnum - 1].UU.r >= AnaDtmax || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      AnaTimemin = WITH->attr[Attrnum - 1].UU.r;
    break;

  case AnaN_Iterfirstmax:
  case AnaN_Timeincrease:
    if (WITH->attr[Attrnum - 1].UU.U73.i1 > AnaLimits[Attrnum - 1]
					    [(long)AnaMax].i ||
	WITH->attr[Attrnum - 1].UU.U73.i1 < AnaLimits[Attrnum - 1]
	  [(long)AnaMin].i || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else {
      switch (Attrnum) {

      case AnaN_Iterfirstmax:
	AnaIterfirstmax = WITH->attr[Attrnum - 1].UU.U73.i1;
	break;

      case AnaN_Timeincrease:
	AnaTimeincrease = WITH->attr[Attrnum - 1].UU.U73.i1;
	break;
      }
    }
    break;
  }
}

Local Void Openconfig_Numbers(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  WITH->attr[AnaN_Vdd - 1].UU.r = AnaVdd;
  WITH->attr[AnaN_Dtmax - 1].UU.r = AnaDtmax;
  WITH->attr[AnaN_Vstep - 1].UU.r = AnaVstep;
  WITH->attr[AnaN_Srange - 1].UU.r = AnaSrange;
  WITH->attr[AnaN_Highest - 1].UU.r = AnaHighest;
  WITH->attr[AnaN_Lowest - 1].UU.r = AnaLowest;
  WITH->attr[AnaN_Small - 1].UU.r = AnaSmall;
  WITH->attr[AnaN_Tol - 1].UU.r = AnaTol;
  WITH->attr[AnaN_Eqn_Epsilon - 1].UU.r = AnaEqn_Epsilon;
  WITH->attr[AnaN_Vardone_Epsilon - 1].UU.r = AnaVardone_Epsilon;
  WITH->attr[AnaN_Timefactor - 1].UU.r = AnaTimefactor;
  WITH->attr[AnaN_Timemin - 1].UU.r = AnaTimemin;
  WITH->attr[AnaN_Iterfirstmax - 1].UU.U73.i1 = AnaIterfirstmax;
  WITH->attr[AnaN_Timeincrease - 1].UU.U73.i1 = AnaTimeincrease;
  WITH->attr[AnaN_Timerelax - 1].UU.r = AnaTimerelax;
  WITH->attr[AnaN_Nodecap - 1].UU.r = AnaNodecap;
  for (Index = 0; Index < AnaParamnum; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Readgate_Numbers(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaVdd = WITH->attr[AnaN_Vdd - 1].UU.r;
    AnaDtmax = WITH->attr[AnaN_Dtmax - 1].UU.r;
    AnaVstep = WITH->attr[AnaN_Vstep - 1].UU.r;
    AnaSrange = WITH->attr[AnaN_Srange - 1].UU.r;
    AnaHighest = WITH->attr[AnaN_Highest - 1].UU.r;
    AnaLowest = WITH->attr[AnaN_Lowest - 1].UU.r;
    AnaSmall = WITH->attr[AnaN_Small - 1].UU.r;
    AnaTol = WITH->attr[AnaN_Tol - 1].UU.r;
    AnaEqn_Epsilon = WITH->attr[AnaN_Eqn_Epsilon - 1].UU.r;
    AnaVardone_Epsilon = WITH->attr[AnaN_Vardone_Epsilon - 1].UU.r;
    AnaTimefactor = WITH->attr[AnaN_Timefactor - 1].UU.r;
    AnaTimemin = WITH->attr[AnaN_Timemin - 1].UU.r;
    AnaIterfirstmax = WITH->attr[AnaN_Iterfirstmax - 1].UU.U73.i1;
    AnaTimeincrease = WITH->attr[AnaN_Timeincrease - 1].UU.U73.i1;
    AnaTimerelax = WITH->attr[AnaN_Timerelax - 1].UU.r;
    AnaNodecap = WITH->attr[AnaN_Nodecap - 1].UU.r;
    AnaTimeReset();
    AnaVshift = AnaVstep / AnaSrange;
  }
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  AnaErase = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  AnaExact = (*STR1 == 'T' || *STR1 == 't');
}


Local Void Writegate_Numbers(act)
Analog_32_action *act;
{
  fprintf(*AnaLogglobals->actfile, "%s\n", AnaErase ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n", AnaExact ? " TRUE" : "FALSE");
}



Local Void Message_Numbers(act)
Analog_32_action *act;
{
  short Tx, Ty;
  NumbersConst *NumbersVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNumbers *ICptr;
  boolean Draw;
  NumbersConst *WITH;

  Draw = true;
  switch (act->messagenum) {

  case AnaMessNode:
    Draw = (strcmp(act->message, AnaLastnodeStr) != 0);
    if (Draw)
      strcpy(AnaLastnodeStr, act->message);
    break;

  case AnaMessGate1:
    Draw = (strcmp(act->message, AnaLastGate1Str) != 0);
    if (Draw)
      strcpy(AnaLastGate1Str, act->message);
    break;

  case AnaMessGate2:
    Draw = (strcmp(act->message, AnaLastGate2Str) != 0);
    if (Draw)
      strcpy(AnaLastGate2Str, act->message);
    break;

  case AnaMessGate3:
    Draw = (strcmp(act->message, AnaLastGate3Str) != 0);
    if (Draw)
      strcpy(AnaLastGate3Str, act->message);
    break;

  case AnaMessGate4:
    Draw = (strcmp(act->message, AnaLastGate4Str) != 0);
    if (Draw)
      strcpy(AnaLastGate4Str, act->message);
    break;

  case AnaMessGate5:
    Draw = (strcmp(act->message, AnaLastGate5Str) != 0);
    if (Draw)
      strcpy(AnaLastGate5Str, act->message);
    break;
  }
  if (!Draw)
    return;
  A_Gate = (Anainstlist *)act->inst->info;
  NumbersVar = (NumbersConst *)A_Gate->InstVar;
  WITH = NumbersVar;
  if (act->page == AnaLogglobals->showpage) {
    (*AnaLogglobals->hook.hidecursor)();
    if (act->messagenum == AnaMessFlags) {
      if (WITH->OldErase != AnaErase) {
	Tx = -40;
	Ty = 0;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	if (AnaErase)
	  m_color((long)AnaLogglobals->color.selword);
	else
	  m_color((long)AnaLogglobals->color.menuword);
	m_centerstr((long)Tx, (long)Ty, NULL, "Erase");
	Tx = -40;
	Ty = 15;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	if (AnaErase)
	  m_color((long)AnaLogglobals->color.menuword);
	else
	  m_color((long)AnaLogglobals->color.selword);
	m_centerstr((long)Tx, (long)Ty, NULL, "Set");
      }
      if (WITH->OldExact != AnaExact) {
	Tx = 40;
	Ty = 0;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	if (AnaExact)
	  m_color((long)AnaLogglobals->color.selword);
	else
	  m_color((long)AnaLogglobals->color.menuword);
	m_centerstr((long)Tx, (long)Ty, NULL, "Exact");
	Tx = 40;
	Ty = 15;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	if (AnaExact)
	  m_color((long)AnaLogglobals->color.menuword);
	else
	  m_color((long)AnaLogglobals->color.selword);
	m_centerstr((long)Tx, (long)Ty, NULL, "Relaxed");
      }

      /*New probe section*/

      if (WITH->OldProbeOn != AnaProbeOn) {
	if (AnaProbeOn)
	  m_color((long)AnaLogglobals->color.gate);
	else
	  m_color((long)AnaLogglobals->color.backgr);
	Tx = TitleX;
	Ty = Line1Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, "Node:");
	Tx = TitleX;
	Ty = Line2Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, "Gate:");

	if (AnaProbeOn)
	  m_color((long)AnaColormap.C_Probe);
	else
	  m_color((long)AnaLogglobals->color.backgr);
	Tx = InfoX;
	Ty = Line1Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessNode);
	Tx = InfoX;
	Ty = Line2Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate1);
	Tx = InfoX;
	Ty = Line3Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate2);
	Tx = InfoX;
	Ty = Line4Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate3);
	Tx = InfoX;
	Ty = Line5Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate4);
	Tx = InfoX;
	Ty = Line6Y;
	(*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate5);

      }
    } else {
      m_color((long)AnaLogglobals->color.backgr);
      switch (act->messagenum) {

      case AnaMessScoreboard:
	Tx = 0;
	Ty = 30;
	break;

      case AnaMessNode:
	Tx = InfoX;
	Ty = Line1Y;
	break;

      case AnaMessGate1:
	Tx = InfoX;
	Ty = Line2Y;
	break;

      case AnaMessGate2:
	Tx = InfoX;
	Ty = Line3Y;
	break;

      case AnaMessGate3:
	Tx = InfoX;
	Ty = Line4Y;
	break;

      case AnaMessGate4:
	Tx = InfoX;
	Ty = Line5Y;
	break;

      case AnaMessGate5:
	Tx = InfoX;
	Ty = Line6Y;
	break;
      }
      (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
      switch (act->messagenum) {

      case AnaMessScoreboard:
	m_centerstr((long)Tx, (long)Ty, NULL, WITH->OldMessScoreboard);
	break;

      case AnaMessNode:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessNode);
	break;

      case AnaMessGate1:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate1);
	break;

      case AnaMessGate2:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate2);
	break;

      case AnaMessGate3:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate3);
	break;

      case AnaMessGate4:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate4);
	break;

      case AnaMessGate5:
	m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate5);
	break;
      }
      if (act->messagenum == AnaMessScoreboard)
	m_color((long)AnaLogglobals->color.message);
      else
	m_color((long)AnaColormap.C_Probe);
      if (act->message[0] == '$') {
	Cellptr = (AnaCell_rec *)act->inst->kind->info;
	ICptr = (ICNumbers *)Cellptr->Ainfo;
	switch (act->messagenum) {

	case AnaMessScoreboard:
	  strcpy(act->message, ICptr->InitMessScoreboard);
	  break;

	case AnaMessNode:
	  strcpy(act->message, ICptr->InitMessNode);
	  break;

	case AnaMessGate1:
	  strcpy(act->message, ICptr->InitMessGate1);
	  break;

	case AnaMessGate2:
	  strcpy(act->message, ICptr->InitMessGate2);
	  break;

	case AnaMessGate3:
	  strcpy(act->message, ICptr->InitMessGate3);
	  break;

	case AnaMessGate4:
	  strcpy(act->message, ICptr->InitMessGate4);
	  break;

	case AnaMessGate5:
	  strcpy(act->message, ICptr->InitMessGate5);
	  break;
	}
      }
      switch (act->messagenum) {

      case AnaMessScoreboard:
	m_centerstr((long)Tx, (long)Ty, NULL, act->message);
	break;

      case AnaMessNode:
      case AnaMessGate1:
      case AnaMessGate2:
      case AnaMessGate3:
      case AnaMessGate4:
      case AnaMessGate5:
	m_drawstr((long)Tx, (long)Ty, NULL, act->message);
	break;
      }
    }
    (*AnaLogglobals->hook.unhidecursor)();
  }
  if (act->messagenum == AnaMessFlags) {
    WITH->OldExact = AnaExact;
    WITH->OldErase = AnaErase;
    if (WITH->OldProbeOn && !AnaProbeOn) {
      Cellptr = (AnaCell_rec *)act->inst->kind->info;
      ICptr = (ICNumbers *)Cellptr->Ainfo;
      strcpy(WITH->OldMessNode, ICptr->InitMessNode);
      strcpy(WITH->OldMessGate1, ICptr->InitMessGate1);
      strcpy(WITH->OldMessGate2, ICptr->InitMessGate2);
      strcpy(WITH->OldMessGate3, ICptr->InitMessGate3);
      strcpy(WITH->OldMessGate4, ICptr->InitMessGate4);
      strcpy(WITH->OldMessGate5, ICptr->InitMessGate5);
      strcpy(AnaLastGate1Str, ICptr->InitMessGate1);
      strcpy(AnaLastGate2Str, ICptr->InitMessGate2);
      strcpy(AnaLastGate3Str, ICptr->InitMessGate3);
      strcpy(AnaLastGate4Str, ICptr->InitMessGate4);
      strcpy(AnaLastGate5Str, ICptr->InitMessGate5);
    }
    WITH->OldProbeOn = AnaProbeOn;
    return;
  }
  if (act->message[0] == '$') {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICNumbers *)Cellptr->Ainfo;
    switch (act->messagenum) {

    case AnaMessScoreboard:
      strcpy(act->message, ICptr->InitMessScoreboard);
      break;

    case AnaMessNode:
      strcpy(act->message, ICptr->InitMessNode);
      break;

    case AnaMessGate1:
      strcpy(act->message, ICptr->InitMessGate1);
      break;

    case AnaMessGate2:
      strcpy(act->message, ICptr->InitMessGate2);
      break;

    case AnaMessGate3:
      strcpy(act->message, ICptr->InitMessGate3);
      break;

    case AnaMessGate4:
      strcpy(act->message, ICptr->InitMessGate4);
      break;

    case AnaMessGate5:
      strcpy(act->message, ICptr->InitMessGate5);
      break;
    }
  }
  switch (act->messagenum) {

  case AnaMessScoreboard:
    strcpy(WITH->OldMessScoreboard, act->message);
    break;

  case AnaMessNode:
    strcpy(WITH->OldMessNode, act->message);
    break;

  case AnaMessGate1:
    strcpy(WITH->OldMessGate1, act->message);
    break;

  case AnaMessGate2:
    strcpy(WITH->OldMessGate2, act->message);
    break;

  case AnaMessGate3:
    strcpy(WITH->OldMessGate3, act->message);
    break;

  case AnaMessGate4:
    strcpy(WITH->OldMessGate4, act->message);
    break;

  case AnaMessGate5:
    strcpy(WITH->OldMessGate5, act->message);
    break;
  }
}


Local Void Refresh_Numbers(act)
Analog_32_action *act;
{
  short Tx, Ty;
  Anainstlist *A_Gate;
  NumbersConst *NumbersVar, *WITH;

  A_Gate = (Anainstlist *)act->inst->info;
  NumbersVar = (NumbersConst *)A_Gate->InstVar;
  WITH = NumbersVar;
  (*AnaLogglobals->hook.hidecursor)();
  Tx = 0;
  Ty = 30;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_color((long)AnaLogglobals->color.message);
  m_centerstr((long)Tx, (long)Ty, NULL, WITH->OldMessScoreboard);
  Tx = 40;
  Ty = -15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_color((long)AnaLogglobals->color.gate);
  m_centerstr((long)Tx, (long)Ty, NULL, "Results");
  Tx = 40;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  WITH->OldExact = AnaExact;
  if (WITH->OldExact)
    m_color((long)AnaLogglobals->color.selword);
  else
    m_color((long)AnaLogglobals->color.menuword);
  m_centerstr((long)Tx, (long)Ty, NULL, "Exact");
  Tx = 40;
  Ty = 15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  if (WITH->OldExact)
    m_color((long)AnaLogglobals->color.menuword);
  else
    m_color((long)AnaLogglobals->color.selword);
  m_centerstr((long)Tx, (long)Ty, NULL, "Relaxed");
  Tx = -40;
  Ty = -15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_color((long)AnaLogglobals->color.gate);
  m_centerstr((long)Tx, (long)Ty, NULL, "Memory");
  Tx = -40;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  WITH->OldErase = AnaErase;
  if (WITH->OldErase)
    m_color((long)AnaLogglobals->color.selword);
  else
    m_color((long)AnaLogglobals->color.menuword);
  m_centerstr((long)Tx, (long)Ty, NULL, "Erase");
  Tx = -40;
  Ty = 15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  if (WITH->OldErase)
    m_color((long)AnaLogglobals->color.menuword);
  else
    m_color((long)AnaLogglobals->color.selword);
  m_centerstr((long)Tx, (long)Ty, NULL, "Set");

  /*New probe section*/

  if (AnaProbeOn) {
    m_color((long)AnaLogglobals->color.gate);
    Tx = TitleX;
    Ty = Line1Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, "Node:");
    Tx = TitleX;
    Ty = Line2Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, "Gate:");
    m_color((long)AnaColormap.C_Probe);
    Tx = InfoX;
    Ty = Line1Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessNode);
    Tx = InfoX;
    Ty = Line2Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate1);
    Tx = InfoX;
    Ty = Line3Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate2);
    Tx = InfoX;
    Ty = Line4Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate3);
    Tx = InfoX;
    Ty = Line5Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate4);
    Tx = InfoX;
    Ty = Line6Y;
    (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
    m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate5);
  }

  (*AnaLogglobals->hook.unhidecursor)();
}


Local Void Erase_Numbers(act)
Analog_32_action *act;
{
  short Tx, Ty;
  Anainstlist *A_Gate;
  NumbersConst *NumbersVar, *WITH;

  A_Gate = (Anainstlist *)act->inst->info;
  NumbersVar = (NumbersConst *)A_Gate->InstVar;
  WITH = NumbersVar;
  (*AnaLogglobals->hook.hidecursor)();
  m_color((long)AnaLogglobals->color.backgr);
  Tx = -40;
  Ty = -15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Memory");
  Tx = -40;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Erase");
  Tx = -40;
  Ty = 15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Set");
  Tx = 40;
  Ty = -15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Results");
  Tx = 40;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Exact");
  Tx = 40;
  Ty = 15;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, "Relaxed");


  /*New probe section*/

  Tx = TitleX;
  Ty = Line1Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, "Node:");
  Tx = TitleX;
  Ty = Line2Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, "Gate:");
  Tx = InfoX;
  Ty = Line1Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessNode);
  Tx = InfoX;
  Ty = Line2Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate1);
  Tx = InfoX;
  Ty = Line3Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate2);
  Tx = InfoX;
  Ty = Line4Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate3);
  Tx = InfoX;
  Ty = Line5Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate4);
  Tx = InfoX;
  Ty = Line6Y;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_drawstr((long)Tx, (long)Ty, NULL, WITH->OldMessGate5);

  Tx = 0;
  Ty = 30;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, WITH->OldMessScoreboard);
  (*AnaLogglobals->hook.unhidecursor)();
}



Local Void Newgate_Numbers(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNumbers *ICptr;
  NumbersConst *NumbersVar;
  Analog_32_action Newact;

  NumbersVar = (NumbersConst *)Malloc(sizeof(NumbersConst));
  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICNumbers *)Cellptr->Ainfo;
  strcpy(NumbersVar->OldMessScoreboard, ICptr->InitMessScoreboard);
  strcpy(NumbersVar->OldMessNode, ICptr->InitMessNode);
  strcpy(NumbersVar->OldMessGate1, ICptr->InitMessGate1);
  strcpy(NumbersVar->OldMessGate2, ICptr->InitMessGate2);
  strcpy(NumbersVar->OldMessGate3, ICptr->InitMessGate3);
  strcpy(NumbersVar->OldMessGate4, ICptr->InitMessGate4);
  strcpy(NumbersVar->OldMessGate5, ICptr->InitMessGate5);
  NumbersVar->OldErase = AnaErase;
  NumbersVar->OldExact = AnaExact;
  NumbersVar->OldProbeOn = AnaProbeOn;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NumbersVar;
  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Numbers(&Newact);
}

Local Void Copygate_Numbers(act)
Analog_32_action *act;
{
  NumbersConst *NumbersVar, *Old_NumbersVar;
  Anainstlist *A_Gate, *A_Oldgate;


  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_NumbersVar = (NumbersConst *)A_Oldgate->InstVar;
  NumbersVar = (NumbersConst *)Malloc(sizeof(NumbersConst));
  *NumbersVar = *Old_NumbersVar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NumbersVar;
}

Local Void Dispose_Numbers(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  NumbersConst *NumbersVar;

  A_Gate = (Anainstlist *)act->inst->info;
  NumbersVar = (NumbersConst *)A_Gate->InstVar;
  Free(NumbersVar);
}


Local Void Touch_Numbers(act)
Analog_32_action *act;
{
  log_action *WITH;

  WITH = AnaLogglobals;
  if (WITH->actx >= -2 && WITH->actx <= 2 && WITH->actx <= 5 && WITH->actx >= -5)
    return;
  WITH->actflag = true;
  if (WITH->actx < -2 && (WITH->acty == 0 || WITH->acty == 1)) {
    AnaErase = true;
    AnaAnalog_ChangeReset(AnaErase);
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Erasing reset memory");
    return;
  }
  if (WITH->actx < -2 && (WITH->acty == 3 || WITH->acty == 4)) {
    AnaErase = false;
    AnaAnalog_ChangeReset(AnaErase);
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Setting reset memory");
    return;
  }
  if (WITH->actx > 2 && (WITH->acty == 0 || WITH->acty == 1)) {
    AnaExact = true;
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Restoring accuracy");
    return;
  }
  if (WITH->actx <= 2 || WITH->acty != 3 && WITH->acty != 4)
    return;
  AnaExact = false;
  AnaScoreboard("#", (long)AnaMessFlags);
  (*AnaLogglobals->hook.vmessage)("Relaxing accuracy");
}



Local Void Probe_Numbers(act)
Analog_32_action *act;
{
  Char dtstr[256];
  Char STR2[256];

  AnaScoreboard(GateName, (long)AnaMessGate1);
  if (AnaChanged) {
    if (AnaReady)
      AnaScoreboard("Aborting", (long)AnaMessGate2);
    else
      AnaScoreboard("Extracting", (long)AnaMessGate2);
  } else {
    if (AnaReady) {
      if (AnaStart)
	AnaScoreboard("Starting", (long)AnaMessGate2);
      else
	AnaScoreboard("Simulating", (long)AnaMessGate2);
    } else
      AnaScoreboard("Unsimulatable", (long)AnaMessGate2);
  }
  if (!AnaReady || AnaChanged) {
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    AnaScoreboard("$", (long)AnaMessGate5);
    return;
  }
  switch (AnaEpoch) {

  case AnaE_Evaluate:
    AnaScoreboard("Computing Gates", (long)AnaMessGate3);
    break;

  case AnaE_Solve:
    AnaScoreboard("Solving Matrix", (long)AnaMessGate3);
    break;

  case AnaE_Calculate:
    AnaScoreboard("Computing Nodes", (long)AnaMessGate3);
    break;

  case AnaE_Refresh:
    AnaScoreboard("Updating Nodes", (long)AnaMessGate3);
    break;

  case AnaE_Adapt:
    AnaScoreboard("Convergence Test", (long)AnaMessGate3);
    break;

  case AnaE_Memory1:
    AnaScoreboard("Finished timestep", (long)AnaMessGate3);
    break;

  case AnaE_Memory2:
    AnaScoreboard("Updating State", (long)AnaMessGate3);
    break;

  case AnaE_Update:
    AnaScoreboard("Not finished", (long)AnaMessGate3);
    break;
  }
  if (AnaError)
    AnaScoreboard("gearshift-Matrix", (long)AnaMessGate4);
  else {
    if (AnaOverflow)
      AnaScoreboard("gearshift-overflow", (long)AnaMessGate4);
    else {
      if (AnaToobig && AnaSuccess)
	AnaScoreboard("Relaxed solution", (long)AnaMessGate4);
      else {
	if (AnaToobig && !AnaSuccess)
	  AnaScoreboard("gearshift-accuracy", (long)AnaMessGate4);
	else {
	  if (AnaSuccess)
	    AnaScoreboard("Finished timestep", (long)AnaMessGate4);
	  else {
	    if (AnaItercount > AnaItermax)
	      AnaScoreboard("gearshift-slow", (long)AnaMessGate4);
	    else {
	      sprintf(STR2, "Step %ld", AnaItercount);
	      AnaScoreboard(STR2, (long)AnaMessGate4);
	    }
	  }
	}
      }
    }
  }
  (*AnaLogglobals->hook.realunit)(AnaDt, 3, "s", dtstr);
  sprintf(STR2, "dt(?) = %s", dtstr);
  AnaScoreboard(STR2, (long)AnaMessGate5);
}


/*==========================================================================*/
/*                 Library Listing For Cell Numbers                    */
/*==========================================================================*/

Void Log_numbers_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_NUMBERS(act);
    break;

  case Analog_act_attrchange:
    Attr_Numbers(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Numbers(act);
    break;

  case Analog_act_readgate:
    Readgate_Numbers(act);
    break;

  case Analog_act_writegate:
    Writegate_Numbers(act);
    break;

  case Analog_act_newgate:
    Newgate_Numbers(act);
    break;

  case Analog_act_copygate:
    Copygate_Numbers(act);
    break;

  case Analog_act_disposegate:
    Dispose_Numbers(act);
    break;

  case Analog_act_message:
    Message_Numbers(act);
    break;

  case Analog_act_erase:
    Erase_Numbers(act);
    break;

  case Analog_act_refresh:
    Refresh_Numbers(act);
    break;

  case Analog_act_touch:
    Touch_Numbers(act);
    break;

  case Analog_act_probe:
    Probe_Numbers(act);
    break;
  }
}

#undef TitleX
#undef InfoX
#undef Line1Y
#undef Line2Y
#undef Line3Y
#undef Line4Y
#undef Line5Y
#undef Line6Y
#undef GateName








/* End. */
