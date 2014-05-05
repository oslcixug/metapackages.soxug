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





#define IDIFF_G
#include "idiff.h"


/*For Attributes*/

#define N_Freeze        1
#define N_Crowbar       2
#define N_Type          3
#define N_Dc            4
#define N_pIinit        5
#define N_pIpulse       6
#define N_pDelay        7
#define N_pRise         8
#define N_pFall         9
#define N_pWidth        10
#define N_pPeriod       11
#define N_pRDelay       12
#define N_sOffset       13
#define N_sAmp          14
#define N_sFreq         15
#define N_sPhase        16
#define N_sDelay        17
#define Cstart          18

#define Gatename        "Current Source"


typedef struct IDiffConst {
  double Time;
  boolean First, Limit, OldLimit, NewLimit;
  double Reset_Time;
  boolean Reset_First, Reset_Freeze;
} IDiffConst;


typedef struct ICIDiff {
  boolean ICFreeze;   /*Default Off*/
  double ICCrowbar;   /*Default 0.1*/
  long ICType;   /*Default 0*/
  double ICdc;   /*Default 0*/
  double ICpIinit;   /*Default 0*/
  double ICpIpulse;   /*Default 100n*/
  double ICpDelay;   /*Default 5*Dtmax*/
  double ICpRise;   /*Default Dtmax*/
  double ICpFall;   /*Default Dtmax*/
  double ICpWidth;   /*Default 5*Dtmax*/
  double ICpPeriod;   /*Default 12*Dtmax*/
  double ICpRDelay;   /*Default 0*/
  double ICsOffset;   /*Default 5n*/
  double ICsAmp;   /*Default 1n*/
  double ICsFreq;   /*Default 1/(Dtmin*10)*/
  double ICsPhase;   /*Default 90Deg*/
  double ICsDelay;   /*Default 0*/
} ICIDiff;



Local Void Switchfill(Inst, Erase)
log_grec *Inst;
boolean Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = 6;
  LLy = 1;
  URx = 9;
  URy = 4;
  (*AnaLogglobals->hook.xform)(Inst, &LLx, &LLy);
  (*AnaLogglobals->hook.xform)(Inst, &URx, &URy);
  if (Erase)
    m_color((long)AnaLogglobals->color.backgr);
  else {
    if (Inst->attr[N_Freeze - 1].UU.b)
      m_color((long)AnaColormap.C_Switchoff);
    else
      m_color((long)AnaColormap.C_Switchon);
  }
  m_fillrect((long)LLx, (long)LLy, (long)URx, (long)URy);
  (*AnaLogglobals->hook.unhidecursor)();
}

Local Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
boolean Limitflag, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = -6;
  LLy = 1;
  URx = -9;
  URy = 4;
  (*AnaLogglobals->hook.xform)(Inst, &LLx, &LLy);
  (*AnaLogglobals->hook.xform)(Inst, &URx, &URy);
  if (Erase)
    m_color((long)AnaLogglobals->color.backgr);
  else {
    if (Limitflag)
      m_color((long)AnaColormap.C_Limiton);
    else
      m_color((long)AnaLogglobals->color.backgr);
  }
  m_fillrect((long)LLx, (long)LLy, (long)URx, (long)URy);
  (*AnaLogglobals->hook.unhidecursor)();
}

Local double Pulsemaker(Inst, Ftime, First)
log_grec *Inst;
double Ftime;
boolean First;
{
  double Result;
  boolean Found;

  Found = false;
  if (First) {
    if (Ftime < Inst->attr[N_pRDelay - 1].UU.r) {
      Result = Inst->attr[N_pIinit - 1].UU.r;
      Found = true;
    } else
      Ftime -= Inst->attr[N_pRDelay - 1].UU.r;
  }
  if (Found)
    return Result;
  if (Ftime > Inst->attr[N_pPeriod - 1].UU.r)
    Ftime -= Inst->attr[N_pPeriod - 1].UU.r *
	     (long)(Ftime / Inst->attr[N_pPeriod - 1].UU.r);
  if (Ftime < Inst->attr[N_pDelay - 1].UU.r) {
    Result = Inst->attr[N_pIinit - 1].UU.r;
    Found = true;
  } else
    Ftime -= Inst->attr[N_pDelay - 1].UU.r;
  if (!Found) {
    if (Ftime < Inst->attr[N_pRise - 1].UU.r) {
      Result =
	Ftime / Inst->attr[N_pRise - 1].UU.r * Inst->attr[N_pIpulse - 1].UU.r +
	(1 - Ftime / Inst->attr[N_pRise - 1].UU.r) * Inst->attr[N_pIinit - 1].UU.r;
      Found = true;
    } else
      Ftime -= Inst->attr[N_pRise - 1].UU.r;
  }
  if (!Found) {
    if (Ftime < Inst->attr[N_pWidth - 1].UU.r) {
      Result = Inst->attr[N_pIpulse - 1].UU.r;
      Found = true;
    } else
      Ftime -= Inst->attr[N_pWidth - 1].UU.r;
  }
  if (Found)
    return Result;
  if (Ftime < Inst->attr[N_pFall - 1].UU.r)
    return (Ftime / Inst->attr[N_pFall - 1].UU.r * Inst->attr[N_pIinit - 1].UU.r +
	    (1 - Ftime / Inst->attr[N_pFall - 1].UU.r) *
	    Inst->attr[N_pIpulse - 1].UU.r);
  else
    return (Inst->attr[N_pIinit - 1].UU.r);
  return Result;
}



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local Void Ex_IDiff(act)
Analog_32_action *act;
{
  /*IDifferent*/
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  long Pin1, Pin2;
  double VPin1, VPin2;
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  double Fout, Ftime, Etemp, Blend, Th;
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;   /*Use with statement to get to internal variables*/
  if (WITH->attr[N_Freeze - 1].UU.b)
    Ftime = IDiffVar->Time;
  else
    Ftime = IDiffVar->Time + AnaDt;
  switch (WITH->attr[N_Type - 1].UU.nv) {

  case 0:
    Fout = WITH->attr[N_Dc - 1].UU.r;
    break;

  case 1:
    Fout = Pulsemaker(act->inst, Ftime, IDiffVar->First);
    break;

  case 2:
    if (Ftime < WITH->attr[N_sDelay - 1].UU.r)
      Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
	       sin(6.2831853 * (WITH->attr[N_sPhase - 1].UU.r / 360));
    else
      Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
	       sin(6.2831853 * (WITH->attr[N_sFreq - 1].UU.r *
				(Ftime - WITH->attr[N_sDelay - 1].UU.r) +
				WITH->attr[N_sPhase - 1].UU.r / 360));
/* p2c: tools.text, line 4796: Note:
 * Line breaker spent 2.0+1.00 seconds, 5000 tries on line 224 [251] */
    break;
  }
  Th = VPin2 - VPin1 + WITH->attr[N_Crowbar - 1].UU.r;
  if (fabs(Th) < 0.1) {
    Etemp = AnaExpo(7080 * Th);
    Blend = 1 / (1 + Etemp);
    MinI2 = Fout * Blend;
    dI2dV2 = Etemp * Blend * Blend;
    dI2dV2 *= 7080 * Fout;
  } else {
    if (Th > 0)
      MinI2 = 0.0;
    else
      MinI2 = Fout;
    dI2dV2 = 0.0;
  }
  MinI1 = -MinI2;
  dI2dV1 = -dI2dV2;
  dI1dV1 = dI2dV2;
  dI1dV2 = dI2dV1;
  WITH1 = AnaSystem;
  WITH1->Mat[Pin1][AnaSysCol] += MinI1;
  WITH1->Mat[Pin1][Pin1] += dI1dV1;
  WITH1->Mat[Pin1][Pin2] += dI1dV2;
  WITH1->Mat[Pin2][AnaSysCol] += MinI2;
  WITH1->Mat[Pin2][Pin1] += dI2dV1;
  WITH1->Mat[Pin2][Pin2] += dI2dV2;

  WITH1->MatNZ[Pin1][AnaSysCol] = true;
  WITH1->MatNZ[Pin1][Pin1] = true;
  WITH1->MatNZ[Pin1][Pin2] = true;
  WITH1->MatNZ[Pin2][AnaSysCol] = true;
  WITH1->MatNZ[Pin2][Pin1] = true;
  WITH1->MatNZ[Pin2][Pin2] = true;
  AnaCapex(act->inst);
}  /*Ex_IDiff*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_IDiff(act)
Analog_32_action *act;
{
  /*IDifferent*/
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, Vpin2;
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  double Fout, Ftime, Th;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Vpin2 = Pin2Ptr->now;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    if (WITH->attr[N_Freeze - 1].UU.b)
      Ftime = IDiffVar->Time;
    else
      Ftime = IDiffVar->Time + AnaDtlast;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case 0:
      Fout = WITH->attr[N_Dc - 1].UU.r;
      break;

    case 1:
      Fout = Pulsemaker(act->inst, Ftime, IDiffVar->First);
      break;

    case 2:
      if (Ftime < WITH->attr[N_sDelay - 1].UU.r)
	Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
		 sin(6.2831853 * (WITH->attr[N_sPhase - 1].UU.r / 360));
      else
	Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
		 sin(6.2831853 * (WITH->attr[N_sFreq - 1].UU.r *
				  (Ftime - WITH->attr[N_sDelay - 1].UU.r) +
				  WITH->attr[N_sPhase - 1].UU.r / 360));
/* p2c: tools.text, line 4796: Note:
 * Line breaker spent 3.0+1.00 seconds, 5000 tries on line 314 [251] */
      break;
    }
    Th = Vpin2 - VPin1 + WITH->attr[N_Crowbar - 1].UU.r;
    if (fabs(Th) < 0.1)
      act->Iin = Fout / (1 + AnaExpo(7080 * Th)) + act->Iin;
    else {
      if (Th < 0)
	act->Iin = Fout + act->Iin;
    }
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    if (WITH->attr[N_Freeze - 1].UU.b)
      Ftime = IDiffVar->Time;
    else
      Ftime = IDiffVar->Time + AnaDtlast;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case 0:
      Fout = WITH->attr[N_Dc - 1].UU.r;
      break;

    case 1:
      Fout = Pulsemaker(act->inst, Ftime, IDiffVar->First);
      break;

    case 2:
      if (Ftime < WITH->attr[N_sDelay - 1].UU.r)
	Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
		 sin(6.2831853 * (WITH->attr[N_sPhase - 1].UU.r / 360));
      else
	Fout = WITH->attr[N_sOffset - 1].UU.r + WITH->attr[N_sAmp - 1].UU.r *
		 sin(6.2831853 * (WITH->attr[N_sFreq - 1].UU.r *
				  (Ftime - WITH->attr[N_sDelay - 1].UU.r) +
				  WITH->attr[N_sPhase - 1].UU.r / 360));
/* p2c: tools.text, line 4796: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 353 [251] */
      break;
    }
    Th = Vpin2 - VPin1 + WITH->attr[N_Crowbar - 1].UU.r;
    if (fabs(Th) < 0.1)
      act->Iin = act->Iin - Fout / (1 + AnaExpo(7080 * Th));
    else {
      if (Th < 0)
	act->Iin = act->Iin - Fout;
    }
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_IDiff*/


Local Void GetCnf_IDiff(NewIC)
ICIDiff *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICIDiff *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("IDIFF", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "FREEZE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	WITH->ICFreeze = false;
	if (!strcmp(Keyword, "ON"))
	  WITH->ICFreeze = true;
      }
      if (!strcmp(Keyword, "CROWBAR")) {
	TRY(try1);
	  WITH->ICCrowbar = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICCrowbar = 0.1;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "TYPE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "SINE"))
	  WITH->ICType = 2;
	else {
	  if (!strcmp(Keyword, "PULSE"))
	    WITH->ICType = 1;
	  else
	    WITH->ICType = 0;
	}
	strcpy(Keyword, "  ");
      }
      if (!strcmp(Keyword, "DC")) {
	TRY(try2);
	  WITH->ICdc = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->ICdc = 1e-9;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "SINE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "OFFSET")) {
	  TRY(try3);
	    WITH->ICsOffset = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try3);
	    WITH->ICsOffset = 5e-9;
	  ENDTRY(try3);
	}
	if (!strcmp(Keyword, "AMP")) {
	  TRY(try4);
	    WITH->ICsAmp = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try4);
	    WITH->ICsAmp = 1e-9;
	  ENDTRY(try4);
	}
	if (!strcmp(Keyword, "FREQ")) {
	  TRY(try5);
	    WITH->ICsFreq = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try5);
	    WITH->ICsFreq = 1 / (AnaDtmax * 10);
	  ENDTRY(try5);
	}
	if (!strcmp(Keyword, "PHASE")) {
	  TRY(try6);
	    WITH->ICsPhase = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try6);
	    WITH->ICsPhase = 0.0;
	  ENDTRY(try6);
	}
	if (!strcmp(Keyword, "DELAY")) {
	  TRY(try7);
	    WITH->ICsDelay = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try7);
	    WITH->ICsDelay = 0.0;
	  ENDTRY(try7);
	}
      }
      if (!strcmp(Keyword, "PULSE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "IINIT")) {
	  TRY(try8);
	    WITH->ICpIinit = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try8);
	    WITH->ICpIinit = 0.0;
	  ENDTRY(try8);
	}
	if (!strcmp(Keyword, "IPULSE")) {
	  TRY(try9);
	    WITH->ICpIpulse = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try9);
	    WITH->ICpIpulse = 100e-9;
	  ENDTRY(try9);
	}
	if (!strcmp(Keyword, "DELAY")) {
	  TRY(try10);
	    WITH->ICpDelay = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try10);
	    WITH->ICpDelay = 6 * AnaDtmax;
	  ENDTRY(try10);
	}
	if (!strcmp(Keyword, "RISE")) {
	  TRY(try11);
	    WITH->ICpRise = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try11);
	    WITH->ICpRise = AnaDtmax * 3;
	  ENDTRY(try11);
	}
	if (!strcmp(Keyword, "FALL")) {
	  TRY(try12);
	    WITH->ICpFall = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try12);
	    WITH->ICpFall = AnaDtmax * 3;
	  ENDTRY(try12);
	}
	if (!strcmp(Keyword, "WIDTH")) {
	  TRY(try13);
	    WITH->ICpWidth = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try13);
	    WITH->ICpWidth = 6 * AnaDtmax;
	  ENDTRY(try13);
	}
	if (!strcmp(Keyword, "PERIOD")) {
	  TRY(try14);
	    WITH->ICpPeriod = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try14);
	    WITH->ICpPeriod = 20 * AnaDtmax;
	  ENDTRY(try14);
	}
	if (!strcmp(Keyword, "RDELAY")) {
	  TRY(try15);
	    WITH->ICpRDelay = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try15);
	    WITH->ICpRDelay = 0.0;
	  ENDTRY(try15);
	}
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_IDiff(act)
Analog_32_action *act;
{
  /*IDifferent*/
  AnaCell_rec *NewCell;
  ICIDiff *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewCell->plist_cell[1].standalone = false;
  NewCell->plist_cell[1].active = true;
  NewIC = (ICIDiff *)Malloc(sizeof(ICIDiff));
  NewIC->ICFreeze = false;
  NewIC->ICCrowbar = 0.1;
  NewIC->ICType = 0;
  NewIC->ICdc = 1e-9;
  NewIC->ICsOffset = 5e-9;
  NewIC->ICsAmp = 1e-9;
  NewIC->ICsFreq = 1 / (AnaDtmax * 10);
  NewIC->ICsPhase = 0.0;
  NewIC->ICsDelay = 0.0;
  NewIC->ICpIinit = 0.0;
  NewIC->ICpIpulse = 100e-9;
  NewIC->ICpDelay = 6 * AnaDtmax;
  NewIC->ICpRise = 3 * AnaDtmax;
  NewIC->ICpFall = 3 * AnaDtmax;
  NewIC->ICpWidth = 6 * AnaDtmax;
  NewIC->ICpPeriod = 20 * AnaDtmax;
  NewIC->ICpRDelay = 0.0;
  GetCnf_IDiff(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*IDiff_Initlib*/



Local boolean IDiffcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Freeze:
    Result = false;
    break;

  case N_Type:
    if (Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Dc:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaITooBig)
      Result = false;
    break;

  case N_Crowbar:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaVdd)
      Result = false;
    break;

  case N_sOffset:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r - Inst->attr[N_sAmp - 1].UU.r < 0 ||
	Inst->attr[Attrnum - 1].UU.r + Inst->attr[N_sAmp - 1].UU.r < 0 ||
	Inst->attr[Attrnum - 1].UU.r > AnaITooBig)
      Result = false;
    break;

  case N_sAmp:
    if (Inst->attr[N_sOffset - 1].UU.r - Inst->attr[Attrnum - 1].UU.r < 0 ||
	Inst->attr[N_sOffset - 1].UU.r + Inst->attr[Attrnum - 1].UU.r < 0 ||
	Inst->attr[Attrnum - 1].blnk ||
	(unsigned)Inst->attr[Attrnum - 1].UU.r > AnaITooBig)
      Result = false;
    break;

  case N_sFreq:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaFreqTooBig)
      Result = false;
    break;

  case N_sPhase:
    if (Inst->attr[Attrnum - 1].blnk ||
	fabs(Inst->attr[Attrnum - 1].UU.r) > AnaPhaseTooBig)
      Result = false;
    break;

  case N_sDelay:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;

  case N_pIinit:
  case N_pIpulse:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaITooBig)
      Result = false;
    break;

  case N_pRise:
  case N_pFall:
  case N_pPeriod:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[N_pDelay - 1].UU.r + Inst->attr[N_pRise - 1].UU.r +
	Inst->attr[N_pFall - 1].UU.r + Inst->attr[N_pWidth - 1].UU.r -
	1e-13 * Inst->attr[N_pPeriod - 1].UU.r > Inst->attr[N_pPeriod - 1].UU.r ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;

  case N_pDelay:
  case N_pWidth:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[N_pDelay - 1].UU.r + Inst->attr[N_pRise - 1].UU.r +
	Inst->attr[N_pFall - 1].UU.r + Inst->attr[N_pWidth - 1].UU.r -
	1e-13 * Inst->attr[N_pPeriod - 1].UU.r > Inst->attr[N_pPeriod - 1].UU.r ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;

  case N_pRDelay:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;
  }
  return Result;
}


Local Void Attr_IDiff(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = IDiffcheck(act->inst, Attrnum);
}


Local Void Newgate_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICIDiff *ICptr;
  long Index;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIDiff *)Cellptr->Ainfo;
  IDiffVar = (IDiffConst *)Malloc(sizeof(IDiffConst));
  IDiffVar->Reset_Time = 0.0;
  IDiffVar->Reset_First = true;
  IDiffVar->Reset_Freeze = ICptr->ICFreeze;
  IDiffVar->Time = IDiffVar->Reset_Time;
  IDiffVar->First = IDiffVar->Reset_First;
  IDiffVar->Limit = false;
  IDiffVar->OldLimit = false;
  IDiffVar->NewLimit = false;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)IDiffVar;
  AnaCapInit(act->inst);
  WITH = act->inst;
  WITH->attr[N_Freeze - 1].UU.b = ICptr->ICFreeze;
  WITH->attr[N_Crowbar - 1].UU.r = ICptr->ICCrowbar;
  WITH->attr[N_Type - 1].UU.nv = ICptr->ICType;
  WITH->attr[N_Dc - 1].UU.r = ICptr->ICdc;
  WITH->attr[N_pIinit - 1].UU.r = ICptr->ICpIinit;
  WITH->attr[N_pIpulse - 1].UU.r = ICptr->ICpIpulse;
  WITH->attr[N_pDelay - 1].UU.r = ICptr->ICpDelay;
  WITH->attr[N_pRise - 1].UU.r = ICptr->ICpRise;
  WITH->attr[N_pFall - 1].UU.r = ICptr->ICpFall;
  WITH->attr[N_pWidth - 1].UU.r = ICptr->ICpWidth;
  WITH->attr[N_pPeriod - 1].UU.r = ICptr->ICpPeriod;
  WITH->attr[N_pRDelay - 1].UU.r = ICptr->ICpRDelay;
  WITH->attr[N_sOffset - 1].UU.r = ICptr->ICsOffset;
  WITH->attr[N_sAmp - 1].UU.r = ICptr->ICsAmp;
  WITH->attr[N_sFreq - 1].UU.r = ICptr->ICsFreq;
  WITH->attr[N_sPhase - 1].UU.r = ICptr->ICsPhase;
  WITH->attr[N_sDelay - 1].UU.r = ICptr->ICsDelay;
  for (Index = 0; Index <= Cstart - 2; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar, *Old_IDiffvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_IDiffvar = (IDiffConst *)A_Oldgate->InstVar;
  IDiffVar = (IDiffConst *)Malloc(sizeof(IDiffConst));
  *IDiffVar = *Old_IDiffvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)IDiffVar;
  AnaCapCopy(act->inst);
}


Local Void Dispose_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  Free(IDiffVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;
  Char STR1[256];

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &IDiffVar->Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  IDiffVar->First = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  IDiffVar->Limit = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &IDiffVar->Reset_Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  IDiffVar->Reset_First = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  IDiffVar->Reset_Freeze = (*STR1 == 'T' || *STR1 == 't');
}

Local Void Writegate_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  AnaCapattrwrite((long)Cstart, act->inst);   /*Write capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  fprintf(*AnaLogglobals->actfile, "% .5E\n", IDiffVar->Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  IDiffVar->First ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  IDiffVar->Limit ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", IDiffVar->Reset_Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  IDiffVar->Reset_First ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  IDiffVar->Reset_Freeze ? " TRUE" : "FALSE");
}



Local Void Reset_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  IDiffVar->Time = IDiffVar->Reset_Time;
  IDiffVar->First = IDiffVar->Reset_First;
  act->inst->attr[N_Freeze - 1].UU.b = IDiffVar->Reset_Freeze;
  IDiffVar->Limit = false;
  IDiffVar->OldLimit = false;
  IDiffVar->NewLimit = false;
  if (AnaLogglobals->showpage == act->page) {
    Limitfill(act->inst, IDiffVar->Limit, false);
    Switchfill(act->inst, false);
  }
}

Local Void Set_Idiff(act)
Analog_32_action *act;
{
  IDiffConst *IdiffVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  IdiffVar = (IDiffConst *)A_Gate->InstVar;
  IdiffVar->Reset_Time = IdiffVar->Time;
  IdiffVar->Reset_First = IdiffVar->First;
  IdiffVar->Reset_Freeze = act->inst->attr[N_Freeze - 1].UU.b;
}

Local Void Clear_Idiff(act)
Analog_32_action *act;
{
  IDiffConst *IdiffVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICIDiff *ICptr;

  A_Gate = (Anainstlist *)act->inst->info;
  IdiffVar = (IDiffConst *)A_Gate->InstVar;
  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIDiff *)Cellptr->Ainfo;
  IdiffVar->Reset_Time = 0.0;
  IdiffVar->Reset_First = true;
  IdiffVar->Reset_Freeze = ICptr->ICFreeze;
}

Local Void PreUpdate_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, VPin2;
  log_grec *WITH;

  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  WITH = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  VPin2 = Pin2Ptr->now;
  IDiffVar->NewLimit = (VPin2 - VPin1 + WITH->attr[N_Crowbar - 1].UU.r > -0.0004);
}

Local Void Update_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  IDiffVar->OldLimit = IDiffVar->Limit;
  IDiffVar->Limit = IDiffVar->NewLimit;
  if (!WITH1->attr[N_Freeze - 1].UU.b)
    IDiffVar->Time += AnaDtlast;
}

Local Void Display_IDiff(act)
Analog_32_action *act;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (!WITH1->attr[N_Freeze - 1].UU.b) {
    if (WITH1->attr[N_Type - 1].UU.nv == 1) {
      if (IDiffVar->First) {
	if (IDiffVar->Time > WITH1->attr[N_pRDelay - 1].UU.r) {
	  IDiffVar->First = false;
	  IDiffVar->Time -= WITH1->attr[N_pRDelay - 1].UU.r;
	}
      }
      if (!IDiffVar->First) {
	if (IDiffVar->Time > WITH1->attr[N_pPeriod - 1].UU.r)
	  IDiffVar->Time -= WITH1->attr[N_pPeriod - 1].UU.r *
	      (long)(IDiffVar->Time / WITH1->attr[N_pPeriod - 1].UU.r);
      }
    }
  }
  if (IDiffVar->OldLimit != IDiffVar->Limit && AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, IDiffVar->Limit, false);
}


Local Void Touch_IDiff(act)
Analog_32_action *act;
{
  log_action *WITH;
  log_grec *WITH1;

  WITH = AnaLogglobals;
  WITH1 = act->inst;
  if (WITH->actx != 1 && WITH->actx != 2 || WITH->acty != 0 && WITH->acty != 1)
    return;
  WITH1->attr[N_Freeze - 1].UU.b = !WITH1->attr[N_Freeze - 1].UU.b;
  WITH->actflag = true;
  AnaChanged = true;
  Switchfill(act->inst, false);
}


Local Void Erase_Idiff(Inst)
log_grec *Inst;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)Inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  Switchfill(Inst, true);
  Limitfill(Inst, IDiffVar->Limit, true);
}

Local Void Refresh_Idiff(Inst)
log_grec *Inst;
{
  IDiffConst *IDiffVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)Inst->info;
  IDiffVar = (IDiffConst *)A_Gate->InstVar;
  Switchfill(Inst, false);
  Limitfill(Inst, IDiffVar->Limit, false);
}



Local Void Probe_Idiff(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  IDiffConst *IdiffVar;
  AnaExt_rec *Node1Ptr, *Node2Ptr;
  double Node1, Node2, Pin1, Pin2;
  log_grec *WITH;
  Char STR1[20];
  Char STR2[24];
  Char STR3[22];
  Char STR4[256];
  Char STR6[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Pin2 = A_Gate->Pininfo[1].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  A_Gate = (Anainstlist *)act->inst->info;
  IdiffVar = (IDiffConst *)A_Gate->InstVar;
  switch (WITH->attr[N_Type - 1].UU.nv) {

  case 0:
    sprintf(STR1, "%s (DC)", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s (Pulse)", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR3, "%s (Sine)", Gatename);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;
  }
  if (Node1 != Pin1 || Node1 == AnaNotyet || Node2 != Pin2 || Node2 == AnaNotyet) {
    AnaScoreboard(" <undefined state>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  if (IdiffVar->Limit)
    AnaScoreboard("Voltage limited", (long)AnaMessGate2);
  else
    AnaScoreboard("Full current delivered", (long)AnaMessGate2);
  if (WITH->attr[N_Type - 1].UU.nv == 0) {
    sprintf(STR6, "I = %s (Ideal)",
	    AnaProbeAmpStr(STR4, WITH->attr[N_Dc - 1].UU.r));
    AnaScoreboard(STR6, (long)AnaMessGate3);
  } else
    AnaScoreboard("$", (long)AnaMessGate3);
}


/*=============================================================================*/
/*                 Library Listing For Cell IDiff                            */
/*=============================================================================*/


Void Log_idiff_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/

  switch (act->action) {

  case Analog_act_newkind:
    initlib_IDiff(act);
    break;

  case Analog_act_ex:
    Ex_IDiff(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_IDiff(act);
    break;

  case Analog_act_update:
    Update_IDiff(act);
    break;

  case Analog_act_display:
    Display_IDiff(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_IDiff(act);
    break;

  case Analog_act_reset:
    Reset_IDiff(act);
    break;

  case Analog_act_newgate:
    Newgate_IDiff(act);
    break;

  case Analog_act_copygate:
    Copygate_IDiff(act);
    break;

  case Analog_act_disposegate:
    Dispose_IDiff(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_IDiff(act);
    break;

  case Analog_act_writegate:
    Writegate_IDiff(act);
    break;

  case Analog_act_Iin:
    Iin_IDiff(act);
    break;

  case Analog_act_erase:
    Erase_Idiff(act->inst);
    break;

  case Analog_act_refresh:
    Refresh_Idiff(act->inst);
    break;

  case Analog_act_touch:
    Touch_IDiff(act);
    break;

  case Analog_act_set:
    Set_Idiff(act);
    break;

  case Analog_act_clear:
    Clear_Idiff(act);
    break;

  case Analog_act_probe:
    Probe_Idiff(act);
    break;
  }
}

#undef N_Freeze
#undef N_Crowbar
#undef N_Type
#undef N_Dc
#undef N_pIinit
#undef N_pIpulse
#undef N_pDelay
#undef N_pRise
#undef N_pFall
#undef N_pWidth
#undef N_pPeriod
#undef N_pRDelay
#undef N_sOffset
#undef N_sAmp
#undef N_sFreq
#undef N_sPhase
#undef N_sDelay
#undef Cstart
#undef Gatename









/* End. */
