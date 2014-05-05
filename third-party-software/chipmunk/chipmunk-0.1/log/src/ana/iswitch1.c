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





#define ISWITCH1_G
#include "iswitch1.h"


/*For Attributes*/

#define N_Crowbar       1
#define N_Type          2
#define N_bIinit        3
#define N_bIpulse       4
#define N_bRise         5
#define N_bFall         6
#define N_bState        7
#define N_mIinit        8
#define N_mIpulse       9
#define N_mRise         10
#define N_mFall         11
#define N_mWidth        12
#define Cstart          13

#define Bistable        0
#define Monostable      1
#define Off             0
#define On              1

#define GateName        "Current Source Switch"



typedef struct Iswitch1Const {
  boolean State, OldState, NewState;
  double Time;
  boolean Limit, OldLimit, NewLimit;
  double OldIideal, NewIideal, Iideal, Reset_Time, Reset_Iideal;
  boolean Reset_State;
} Iswitch1Const;


typedef struct ICIswitch1 {
  double ICCrowbar;   /*Default 0*/
  long ICType;   /*Default 0*/
  double ICbIinit;   /*Default 0*/
  double ICbIpulse;   /*Default 10n*/
  double ICbRise;   /*Default Dtmax*/
  double ICbFall;   /*Default Dtmax*/
  long ICbState;   /*Default 0*/
  double ICmIinit;   /*Default 0*/
  double ICmIpulse;   /*Default 10n*/
  double ICmRise;   /*Default Dtmax*/
  double ICmFall;   /*Default Dtmax*/
  double ICmWidth;   /*Default 5*Dtmax*/
} ICIswitch1;



Local Void Iswitch1fill(Inst, State, Erase)
log_grec *Inst;
boolean State, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = -17;
  LLy = 2;
  URx = -13;
  URy = -2;
  (*AnaLogglobals->hook.xform)(Inst, &LLx, &LLy);
  (*AnaLogglobals->hook.xform)(Inst, &URx, &URy);
  if (Erase)
    m_color((long)AnaLogglobals->color.backgr);
  else {
    if (State)
      m_color((long)AnaColormap.C_Switchon);
    else
      m_color((long)AnaColormap.C_Switchoff);
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
  LLx = -2;
  LLy = 2;
  URx = 2;
  URy = -2;
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


Local double Bistablemaker(Inst, time, State, Ex, OldIideal)
log_grec *Inst;
double time;
boolean State, Ex;
double OldIideal;
{
  double Newtime, Ratio;

  if (Ex)
    Newtime = AnaAccum + AnaDt - time;
  else
    Newtime = AnaAccum + AnaDtlast - time;
  if (State) {
    if (Newtime >= Inst->attr[N_bRise - 1].UU.r)
      return (Inst->attr[N_bIpulse - 1].UU.r);
    else {
      Ratio = Newtime / Inst->attr[N_bRise - 1].UU.r;
      return (Ratio * Inst->attr[N_bIpulse - 1].UU.r + (1 - Ratio) * OldIideal);
    }
  } else {
    if (Newtime >= Inst->attr[N_bFall - 1].UU.r)
      return (Inst->attr[N_bIinit - 1].UU.r);
    else {
      Ratio = Newtime / Inst->attr[N_bFall - 1].UU.r;
      return (Ratio * Inst->attr[N_bIinit - 1].UU.r + (1 - Ratio) * OldIideal);
    }
  }
}


Local double Monostablemaker(Inst, time, State, Ex, OldIideal)
log_grec *Inst;
double time;
boolean State, Ex;
double OldIideal;
{
  double Newtime, Ratio;

  if (State) {
    if (Ex)
      Newtime = AnaAccum + AnaDt - time;
    else
      Newtime = AnaAccum + AnaDtlast - time;
    if (Newtime < Inst->attr[N_mRise - 1].UU.r) {
      Ratio = Newtime / Inst->attr[N_mRise - 1].UU.r;
      return (Ratio * Inst->attr[N_mIpulse - 1].UU.r + (1 - Ratio) * OldIideal);
    } else {
      Newtime -= Inst->attr[N_mRise - 1].UU.r;
      if (Newtime < Inst->attr[N_mWidth - 1].UU.r)
	return (Inst->attr[N_mIpulse - 1].UU.r);
      else {
	Newtime -= Inst->attr[N_mWidth - 1].UU.r;
	if (Newtime < Inst->attr[N_mFall - 1].UU.r) {
	  Ratio = Newtime / Inst->attr[N_mFall - 1].UU.r;
	  return (Ratio * Inst->attr[N_mIinit - 1].UU.r +
		  (1 - Ratio) * Inst->attr[N_mIpulse - 1].UU.r);
	} else
	  return (Inst->attr[N_mIinit - 1].UU.r);
      }
    }
  } else
    return (Inst->attr[N_mIinit - 1].UU.r);
}



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local Void Ex_Iswitch1(act)
Analog_32_action *act;
{
  double MinI1, dI1dV1;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1;
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  double Fout, Th, Etemp, Blend;
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;   /*Use with statement to get to internal variables*/
  switch (WITH->attr[N_Type - 1].UU.nv) {

  case Bistable:
    Fout = Bistablemaker(act->inst, Iswitch1Var->Time, Iswitch1Var->State,
			 true, Iswitch1Var->OldIideal);
    break;

  case Monostable:
    Fout = Monostablemaker(act->inst, Iswitch1Var->Time, Iswitch1Var->State,
			   true, Iswitch1Var->OldIideal);
    break;
  }
  Iswitch1Var->NewIideal = Fout;
  Th = VPin1 - AnaVdd + WITH->attr[N_Crowbar - 1].UU.r;
  if (fabs(Th) < 0.1) {
    Etemp = AnaExpo(7080 * Th);
    Blend = 1 / (1 + Etemp);
    MinI1 = Fout * Blend;
    dI1dV1 = Etemp * Blend * Blend;
    dI1dV1 *= 7080 * Fout;
  } else {
    if (Th > 0)
      MinI1 = 0.0;
    else
      MinI1 = Fout;
    dI1dV1 = 0.0;
  }
  WITH1 = AnaSystem;
  WITH1->Mat[Pin1][AnaSysCol] += MinI1;
  WITH1->Mat[Pin1][Pin1] += dI1dV1;

  WITH1->MatNZ[Pin1][AnaSysCol] = true;
  WITH1->MatNZ[Pin1][Pin1] = true;
  AnaCapex(act->inst);
}  /*Ex_Iswitch1*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Iswitch1(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1;
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  double Fout, Th;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case Bistable:
      Fout = Bistablemaker(act->inst, Iswitch1Var->Time, Iswitch1Var->State,
			   false, Iswitch1Var->OldIideal);
      break;

    case Monostable:
      Fout = Monostablemaker(act->inst, Iswitch1Var->Time, Iswitch1Var->State,
			     false, Iswitch1Var->OldIideal);
      break;
    }
    Th = VPin1 - AnaVdd + WITH->attr[N_Crowbar - 1].UU.r;
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
}  /*Iin_Iswitch1*/


Local Void GetCnf_Iswitch1(NewIC)
ICIswitch1 *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICIswitch1 *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("ISWITCH1", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
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
	if (!strcmp(Keyword, "MONOSTABLE"))
	  WITH->ICType = 1;
	else
	  WITH->ICType = 0;
      }
      if (!strcmp(Keyword, "BISTABLE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "IINIT")) {
	  TRY(try2);
	    WITH->ICbIinit = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try2);
	    WITH->ICbIinit = 0.0;
	  ENDTRY(try2);
	}
	if (!strcmp(Keyword, "IPULSE")) {
	  TRY(try3);
	    WITH->ICbIpulse = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try3);
	    WITH->ICbIpulse = 10e-9;
	  ENDTRY(try3);
	}
	if (!strcmp(Keyword, "RISE")) {
	  TRY(try4);
	    WITH->ICbRise = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try4);
	    WITH->ICbRise = 3 * AnaDtmax;
	  ENDTRY(try4);
	}
	if (!strcmp(Keyword, "FALL")) {
	  TRY(try5);
	    WITH->ICbFall = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try5);
	    WITH->ICbFall = 3 * AnaDtmax;
	  ENDTRY(try5);
	}
	if (!strcmp(Keyword, "STATE")) {
	  (*AnaLogglobals->hook.getword)(Arg, Keyword);
	  if (!strcmp(Keyword, "ON"))
	    WITH->ICbState = On;
	  else
	    WITH->ICbState = Off;
	}
      }
      if (!strcmp(Keyword, "MONOSTABLE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "IINIT")) {
	  TRY(try6);
	    WITH->ICmIinit = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try6);
	    WITH->ICmIinit = 0.0;
	  ENDTRY(try6);
	}
	if (!strcmp(Keyword, "IPULSE")) {
	  TRY(try7);
	    WITH->ICmIpulse = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try7);
	    WITH->ICmIpulse = 10e-9;
	  ENDTRY(try7);
	}
	if (!strcmp(Keyword, "RISE")) {
	  TRY(try8);
	    WITH->ICmRise = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try8);
	    WITH->ICmRise = 3 * AnaDtmax;
	  ENDTRY(try8);
	}
	if (!strcmp(Keyword, "FALL")) {
	  TRY(try9);
	    WITH->ICmFall = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try9);
	    WITH->ICmFall = 3 * AnaDtmax;
	  ENDTRY(try9);
	}
	if (!strcmp(Keyword, "WIDTH")) {
	  TRY(try10);
	    WITH->ICmWidth = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try10);
	    WITH->ICmWidth = 6 * AnaDtmax;
	  ENDTRY(try10);
	}
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_Iswitch1(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICIswitch1 *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewIC = (ICIswitch1 *)Malloc(sizeof(ICIswitch1));
  NewIC->ICCrowbar = 0.1;
  NewIC->ICType = 0;
  NewIC->ICbIinit = 0.0;
  NewIC->ICbIpulse = 10e-9;
  NewIC->ICbRise = 3 * AnaDtmax;
  NewIC->ICbFall = 3 * AnaDtmax;
  NewIC->ICbState = Off;
  NewIC->ICmIinit = 0.0;
  NewIC->ICmIpulse = 10e-9;
  NewIC->ICmRise = 3 * AnaDtmax;
  NewIC->ICmFall = 3 * AnaDtmax;
  NewIC->ICmWidth = 6 * AnaDtmax;
  GetCnf_Iswitch1(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Iswitch1_Initlib*/


Local Void ResetInit_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  Iswitch1Var->Reset_Time = -1e20;
  if (WITH1->attr[N_Type - 1].UU.nv != Bistable) {
    Iswitch1Var->Reset_Iideal = WITH1->attr[N_mIinit - 1].UU.r;
    Iswitch1Var->Reset_State = false;
    return;
  }
  if (WITH1->attr[N_bState - 1].UU.nv == Off) {
    Iswitch1Var->Reset_Iideal = WITH1->attr[N_bIinit - 1].UU.r;
    Iswitch1Var->Reset_State = false;
  } else {
    Iswitch1Var->Reset_Iideal = WITH1->attr[N_bIpulse - 1].UU.r;
    Iswitch1Var->Reset_State = true;
  }
}


Local boolean Iswitch1check(Inst, Attrnum, act)
log_grec *Inst;
long Attrnum;
Analog_32_action *act;
{
  boolean Result;
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Crowbar:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaVdd)
      Result = false;
    break;

  case N_Type:
    if (Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else {
      if (Inst->attr[N_Type - 1].UU.nv == Bistable) {
	if (Inst->attr[N_bState - 1].UU.nv == Off)
	  Iswitch1Var->State = false;
	else
	  Iswitch1Var->State = true;
	Iswitch1Var->OldIideal = Iswitch1Var->Iideal;
      } else {
	Iswitch1Var->OldIideal = Inst->attr[N_mIinit - 1].UU.r;
	Iswitch1Var->Iideal = Iswitch1Var->OldIideal;
	Iswitch1Var->State = false;
      }
      Iswitch1Var->Time = AnaAccum;
      Iswitch1fill(act->inst, Iswitch1Var->State, false);
      ResetInit_Iswitch1(act);
    }
    break;

  case N_bIinit:
  case N_bIpulse:
  case N_mIinit:
  case N_mIpulse:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaITooBig)
      Result = false;
    break;

  case N_bRise:
  case N_bFall:
  case N_mRise:
  case N_mFall:
  case N_mWidth:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;

  case N_bState:
    if (Iswitch1Var->Reset_Time == -1e20)
      ResetInit_Iswitch1(act);
    break;
  }
  return Result;
}


Local Void Attr_Iswitch1(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 2)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Iswitch1check(act->inst, Attrnum, act);
}


Local Void Newgate_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICIswitch1 *ICptr;
  long Index;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIswitch1 *)Cellptr->Ainfo;
  Iswitch1Var = (Iswitch1Const *)Malloc(sizeof(Iswitch1Const));
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)Iswitch1Var;
  ResetInit_Iswitch1(act);
  Iswitch1Var->Limit = false;
  Iswitch1Var->OldLimit = false;
  Iswitch1Var->NewLimit = false;
  Iswitch1Var->State = Iswitch1Var->Reset_State;
  Iswitch1Var->Time = Iswitch1Var->Reset_Time;
  Iswitch1Var->OldIideal = Iswitch1Var->Reset_Iideal;
  Iswitch1Var->Iideal = Iswitch1Var->OldIideal;
  AnaCapInit(act->inst);
  WITH = act->inst;
  WITH->attr[N_Crowbar - 1].UU.r = ICptr->ICCrowbar;
  WITH->attr[N_Type - 1].UU.nv = ICptr->ICType;
  WITH->attr[N_bIinit - 1].UU.r = ICptr->ICbIinit;
  WITH->attr[N_bIpulse - 1].UU.r = ICptr->ICbIpulse;
  WITH->attr[N_mIinit - 1].UU.r = ICptr->ICmIinit;
  WITH->attr[N_mIpulse - 1].UU.r = ICptr->ICmIpulse;
  WITH->attr[N_bRise - 1].UU.r = ICptr->ICbRise;
  WITH->attr[N_bFall - 1].UU.r = ICptr->ICbFall;
  WITH->attr[N_bState - 1].UU.nv = ICptr->ICbState;
  WITH->attr[N_mRise - 1].UU.r = ICptr->ICmRise;
  WITH->attr[N_mFall - 1].UU.r = ICptr->ICmFall;
  WITH->attr[N_mWidth - 1].UU.r = ICptr->ICmWidth;
  for (Index = 0; Index <= Cstart - 2; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var, *Old_Iswitch1var;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Iswitch1var = (Iswitch1Const *)A_Oldgate->InstVar;
  Iswitch1Var = (Iswitch1Const *)Malloc(sizeof(Iswitch1Const));
  *Iswitch1Var = *Old_Iswitch1var;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)Iswitch1Var;
  AnaCapCopy(act->inst);
}


Local Void Dispose_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Free(Iswitch1Var);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;
  Char STR1[256];

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &Iswitch1Var->Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  Iswitch1Var->State = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  Iswitch1Var->Limit = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &Iswitch1Var->OldIideal);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &Iswitch1Var->Reset_Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  Iswitch1Var->Reset_State = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &Iswitch1Var->Reset_Iideal);
  getc(*AnaLogglobals->actfile);
  Iswitch1Var->Time = AnaAccum - fabs(Iswitch1Var->Time);
      /*ABS for file format compatible*/
  Iswitch1Var->Iideal = Iswitch1Var->OldIideal;
}

Local Void Writegate_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  AnaCapattrwrite((long)Cstart, act->inst);   /*Write capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  fprintf(*AnaLogglobals->actfile, "% .5E\n", AnaAccum - Iswitch1Var->Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  Iswitch1Var->State ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  Iswitch1Var->Limit ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", Iswitch1Var->OldIideal);
  fprintf(*AnaLogglobals->actfile, "% .5E\n", Iswitch1Var->Reset_Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  Iswitch1Var->Reset_State ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", Iswitch1Var->Reset_Iideal);
}



Local Void Reset_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Iswitch1Var->Limit = false;
  Iswitch1Var->OldLimit = false;
  Iswitch1Var->NewLimit = false;
  Iswitch1Var->Time = Iswitch1Var->Reset_Time;
  Iswitch1Var->State = Iswitch1Var->Reset_State;
  Iswitch1Var->OldIideal = Iswitch1Var->Reset_Iideal;
  Iswitch1Var->Iideal = Iswitch1Var->Reset_Iideal;
  if (AnaLogglobals->showpage == act->page) {
    Limitfill(act->inst, Iswitch1Var->Limit, false);
    Iswitch1fill(act->inst, Iswitch1Var->State, false);
  }
}

Local Void Set_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  Iswitch1Var->Reset_State = Iswitch1Var->State;
  Iswitch1Var->Reset_Iideal = Iswitch1Var->OldIideal;
  Iswitch1Var->Reset_Time = Iswitch1Var->Time - AnaAccum;
}


Local Void PreUpdate_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr;
  double VPin1;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
  VPin1 = Pin1Ptr->last;
  Iswitch1Var->NewLimit = (VPin1 - AnaVdd + WITH1->attr[N_Crowbar - 1].UU.r >
			   -0.0004);
  if (WITH1->attr[N_Type - 1].UU.nv != Monostable)
    return;
  if (Iswitch1Var->State && AnaAccum + AnaDtlast >
	WITH1->attr[N_mRise - 1].UU.r + WITH1->attr[N_mFall - 1].UU.r +
	WITH1->attr[N_mWidth - 1].UU.r + Iswitch1Var->Time)
    Iswitch1Var->NewState = false;
  else
    Iswitch1Var->NewState = Iswitch1Var->State;
}


Local Void Update_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  if (act->inst->attr[N_Type - 1].UU.nv == Monostable) {
    Iswitch1Var->OldState = Iswitch1Var->State;
    Iswitch1Var->State = Iswitch1Var->NewState;
  }
  Iswitch1Var->Iideal = Iswitch1Var->NewIideal;
  Iswitch1Var->OldLimit = Iswitch1Var->Limit;
  Iswitch1Var->Limit = Iswitch1Var->NewLimit;
}


Local Void Display_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage != act->page)
    return;
  if (WITH1->attr[N_Type - 1].UU.nv == Monostable) {
    if (Iswitch1Var->State != Iswitch1Var->OldState)
      Iswitch1fill(act->inst, Iswitch1Var->State, false);
  }
  if (Iswitch1Var->OldLimit != Iswitch1Var->Limit)
    Limitfill(act->inst, Iswitch1Var->Limit, false);
}


Local Void Touch_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;
  log_action *WITH;
  log_grec *WITH1;

  WITH = AnaLogglobals;
  WITH1 = act->inst;
  if (WITH->actx != -2 && WITH->actx != -3 && WITH->actx != -4 || WITH->acty != 0)
    return;
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  if (WITH1->attr[N_Type - 1].UU.nv == Bistable) {
    Iswitch1Var->State = !Iswitch1Var->State;
    Iswitch1Var->Time = AnaAccum;
  } else {
    if (!Iswitch1Var->State) {
      Iswitch1Var->State = true;
      Iswitch1Var->Time = AnaAccum;
    }
  }
  Iswitch1Var->OldIideal = Iswitch1Var->Iideal;
  Iswitch1fill(act->inst, Iswitch1Var->State, false);
  WITH->actflag = true;
  AnaChanged = true;
}


Local Void Refresh_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Iswitch1fill(act->inst, Iswitch1Var->State, false);
  Limitfill(act->inst, Iswitch1Var->Limit, false);
}

Local Void Erase_Iswitch1(act)
Analog_32_action *act;
{
  Iswitch1Const *Iswitch1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  Iswitch1fill(act->inst, Iswitch1Var->State, true);
  Limitfill(act->inst, Iswitch1Var->Limit, true);
}

Local Void Probe_Iswitch1(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  Iswitch1Const *Iswitch1Var;
  AnaExt_rec *Node1Ptr;
  double Node1, Pin1;
  log_grec *WITH;
  Char STR1[256];
  Char STR3[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node1 = Node1Ptr->ltimestep;
  AnaScoreboard(GateName, (long)AnaMessGate1);
  if (Node1 != Pin1 || Node1 == AnaNotyet) {
    AnaScoreboard(" <undefined state>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    return;
  }
  A_Gate = (Anainstlist *)act->inst->info;
  Iswitch1Var = (Iswitch1Const *)A_Gate->InstVar;
  if (WITH->attr[N_Type - 1].UU.nv == Bistable) {
    if (Iswitch1Var->State)
      AnaScoreboard("Bistable - On", (long)AnaMessGate2);
    else
      AnaScoreboard("Bistable - Off", (long)AnaMessGate2);
  } else {
    if (Iswitch1Var->State)
      AnaScoreboard("Monostable - On", (long)AnaMessGate2);
    else
      AnaScoreboard("Monostable - Off", (long)AnaMessGate2);
  }
  sprintf(STR3, "I = %s (Ideal)", AnaProbeAmpStr(STR1, Iswitch1Var->Iideal));
  AnaScoreboard(STR3, (long)AnaMessGate3);
  if (Iswitch1Var->Limit)
    AnaScoreboard("Voltage limited", (long)AnaMessGate4);
  else
    AnaScoreboard("Full current delivered", (long)AnaMessGate4);
}


/*=============================================================================*/
/*                 Library Listing For Cell Iswitch1                            */
/*=============================================================================*/


Void Log_iswitch1_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/

  switch (act->action) {

  case Analog_act_newkind:
    initlib_Iswitch1(act);
    break;

  case Analog_act_ex:
    Ex_Iswitch1(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Iswitch1(act);
    break;

  case Analog_act_update:
    Update_Iswitch1(act);
    break;

  case Analog_act_display:
    Display_Iswitch1(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Iswitch1(act);
    break;

  case Analog_act_reset:
    Reset_Iswitch1(act);
    break;

  case Analog_act_newgate:
    Newgate_Iswitch1(act);
    break;

  case Analog_act_copygate:
    Copygate_Iswitch1(act);
    break;

  case Analog_act_disposegate:
    Dispose_Iswitch1(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Iswitch1(act);
    break;

  case Analog_act_writegate:
    Writegate_Iswitch1(act);
    break;

  case Analog_act_Iin:
    Iin_Iswitch1(act);
    break;

  case Analog_act_erase:
    Erase_Iswitch1(act);
    break;

  case Analog_act_refresh:
    Refresh_Iswitch1(act);
    break;

  case Analog_act_touch:
    Touch_Iswitch1(act);
    break;

  case Analog_act_set:
    Set_Iswitch1(act);
    break;

  case Analog_act_clear:
    ResetInit_Iswitch1(act);
    break;

  case Analog_act_probe:
    Probe_Iswitch1(act);
    break;
  }
}

#undef N_Crowbar
#undef N_Type
#undef N_bIinit
#undef N_bIpulse
#undef N_bRise
#undef N_bFall
#undef N_bState
#undef N_mIinit
#undef N_mIpulse
#undef N_mRise
#undef N_mFall
#undef N_mWidth
#undef Cstart
#undef Bistable
#undef Monostable
#undef Off
#undef On
#undef GateName









/* End. */
