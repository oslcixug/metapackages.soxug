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
/* From input file "vtools.text" */



#include <p2c/p2c.h>




#define VSWITCH_G
#include "vswitch.h"


/*For Attributes*/

#define N_Res           1
#define N_Type          2
#define N_bVinit        3
#define N_bVpulse       4
#define N_bRise         5
#define N_bFall         6
#define N_bState        7
#define N_mVinit        8
#define N_mVpulse       9
#define N_mRise         10
#define N_mFall         11
#define N_mWidth        12
#define Cstart          13

#define Bistable        0
#define Monostable      1
#define Off             0
#define On              1

#define Gatename        "Voltage Source Switch"


typedef struct VswitchConst {
  double Conductance;
  boolean State, OldState, NewState;
  double Time;
  boolean Limit, OldLimit, NewLimit;
  double OldVideal, NewVideal, Videal, Reset_Time, Reset_Videal;
  boolean Reset_State;
} VswitchConst;


typedef struct ICVswitch {
  double ICRes;   /*Default 50ohm*/
  long ICType;   /*Default 0*/
  double ICbVinit;   /*Default 0*/
  double ICbVpulse;   /*Default Vdd*/
  double ICbRise;   /*Default Dtmax*/
  double ICbFall;   /*Default Dtmax*/
  long ICbState;   /*Default 0*/
  double ICmVinit;   /*Default 0*/
  double ICmVpulse;   /*Default Vdd*/
  double ICmRise;   /*Default Dtmax*/
  double ICmFall;   /*Default Dtmax*/
  double ICmWidth;   /*Default 5*Dtmax*/
} ICVswitch;



Local Void Vswitchfill(Inst, State, Erase)
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


Local double Bistablemaker(Inst, time, State, Ex, OldVideal)
log_grec *Inst;
double time;
boolean State, Ex;
double OldVideal;
{
  double Newtime, Ratio;

  if (Ex)
    Newtime = AnaAccum + AnaDt - time;
  else
    Newtime = AnaAccum + AnaDtlast - time;
  if (State) {
    if (Newtime >= Inst->attr[N_bRise - 1].UU.r)
      return (Inst->attr[N_bVpulse - 1].UU.r);
    else {
      Ratio = Newtime / Inst->attr[N_bRise - 1].UU.r;
      return (Ratio * Inst->attr[N_bVpulse - 1].UU.r + (1 - Ratio) * OldVideal);
    }
  } else {
    if (Newtime >= Inst->attr[N_bFall - 1].UU.r)
      return (Inst->attr[N_bVinit - 1].UU.r);
    else {
      Ratio = Newtime / Inst->attr[N_bFall - 1].UU.r;
      return (Ratio * Inst->attr[N_bVinit - 1].UU.r + (1 - Ratio) * OldVideal);
    }
  }
}


Local double Monostablemaker(Inst, time, State, Ex, OldVideal)
log_grec *Inst;
double time;
boolean State, Ex;
double OldVideal;
{
  double Newtime, Ratio;

  if (State) {
    if (Ex)
      Newtime = AnaAccum + AnaDt - time;
    else
      Newtime = AnaAccum + AnaDtlast - time;
    if (Newtime < Inst->attr[N_mRise - 1].UU.r) {
      Ratio = Newtime / Inst->attr[N_mRise - 1].UU.r;
      return (Ratio * Inst->attr[N_mVpulse - 1].UU.r + (1 - Ratio) * OldVideal);
    } else {
      Newtime -= Inst->attr[N_mRise - 1].UU.r;
      if (Newtime < Inst->attr[N_mWidth - 1].UU.r)
	return (Inst->attr[N_mVpulse - 1].UU.r);
      else {
	Newtime -= Inst->attr[N_mWidth - 1].UU.r;
	if (Newtime < Inst->attr[N_mFall - 1].UU.r) {
	  Ratio = Newtime / Inst->attr[N_mFall - 1].UU.r;
	  return (Ratio * Inst->attr[N_mVinit - 1].UU.r +
		  (1 - Ratio) * Inst->attr[N_mVpulse - 1].UU.r);
	} else
	  return (Inst->attr[N_mVinit - 1].UU.r);
      }
    }
  } else
    return (Inst->attr[N_mVinit - 1].UU.r);
}



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local Void Ex_Vswitch(act)
Analog_32_action *act;
{
  double MinI1, dI1dV1;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1;
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  double Fout;
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;   /*Use with statement to get to internal variables*/
  switch (WITH->attr[N_Type - 1].UU.nv) {

  case Bistable:
    Fout = Bistablemaker(act->inst, VswitchVar->Time, VswitchVar->State, true,
			 VswitchVar->OldVideal);
    break;

  case Monostable:
    Fout = Monostablemaker(act->inst, VswitchVar->Time, VswitchVar->State,
			   true, VswitchVar->OldVideal);
    break;
  }
  VswitchVar->NewVideal = Fout;
  MinI1 = VswitchVar->Conductance * (Fout - VPin1);
  dI1dV1 = VswitchVar->Conductance;
  WITH1 = AnaSystem;
  WITH1->Mat[Pin1][AnaSysCol] += MinI1;
  WITH1->Mat[Pin1][Pin1] += dI1dV1;
  WITH1->MatNZ[Pin1][AnaSysCol] = true;
  WITH1->MatNZ[Pin1][Pin1] = true;
  AnaCapex(act->inst);
}  /*Ex_Vswitch*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Vswitch(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1;
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  double Fout;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case Bistable:
      Fout = Bistablemaker(act->inst, VswitchVar->Time, VswitchVar->State,
			   false, VswitchVar->OldVideal);
      break;

    case Monostable:
      Fout = Monostablemaker(act->inst, VswitchVar->Time, VswitchVar->State,
			     false, VswitchVar->OldVideal);
      break;
    }
    act->Iin = VswitchVar->Conductance * (VPin1 - Fout) + act->Iin;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_Vswitch*/


Local Void GetCnf_Vswitch(NewIC)
ICVswitch *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICVswitch *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("VSWITCH", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "RES")) {
	TRY(try1);
	  WITH->ICRes = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICRes = 50.0;
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
	if (!strcmp(Keyword, "VINIT")) {
	  TRY(try2);
	    WITH->ICbVinit = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try2);
	    WITH->ICbVinit = 0.0;
	  ENDTRY(try2);
	}
	if (!strcmp(Keyword, "VPULSE")) {
	  TRY(try3);
	    WITH->ICbVpulse = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try3);
	    WITH->ICbVpulse = AnaVdd;
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
	if (!strcmp(Keyword, "VINIT")) {
	  TRY(try6);
	    WITH->ICmVinit = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try6);
	    WITH->ICmVinit = 0.0;
	  ENDTRY(try6);
	}
	if (!strcmp(Keyword, "VPULSE")) {
	  TRY(try7);
	    WITH->ICmVpulse = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try7);
	    WITH->ICmVpulse = AnaVdd;
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

Local Void initlib_Vswitch(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICVswitch *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewIC = (ICVswitch *)Malloc(sizeof(ICVswitch));
  NewIC->ICRes = 50.0;
  NewIC->ICType = 0;
  NewIC->ICbVinit = 0.0;
  NewIC->ICbVpulse = AnaVdd;
  NewIC->ICbRise = 3 * AnaDtmax;
  NewIC->ICbFall = 3 * AnaDtmax;
  NewIC->ICbState = Off;
  NewIC->ICmVinit = 0.0;
  NewIC->ICmVpulse = AnaVdd;
  NewIC->ICmRise = 3 * AnaDtmax;
  NewIC->ICmFall = 3 * AnaDtmax;
  NewIC->ICmWidth = 6 * AnaDtmax;
  GetCnf_Vswitch(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Vswitch_Initlib*/


Local Void ResetInit_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  VswitchVar->Reset_Time = -1e20;
  if (WITH1->attr[N_Type - 1].UU.nv != Bistable) {
    VswitchVar->Reset_Videal = WITH1->attr[N_mVinit - 1].UU.r;
    VswitchVar->Reset_State = false;
    return;
  }
  if (WITH1->attr[N_bState - 1].UU.nv == Off) {
    VswitchVar->Reset_Videal = WITH1->attr[N_bVinit - 1].UU.r;
    VswitchVar->Reset_State = false;
  } else {
    VswitchVar->Reset_Videal = WITH1->attr[N_bVpulse - 1].UU.r;
    VswitchVar->Reset_State = true;
  }
}


Local boolean Vswitchcheck(Inst, Attrnum, act)
log_grec *Inst;
long Attrnum;
Analog_32_action *act;
{
  boolean Result;
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Type:
    if (Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else {
      if (Inst->attr[N_Type - 1].UU.nv == Bistable) {
	if (Inst->attr[N_bState - 1].UU.nv == Off)
	  VswitchVar->State = false;
	else
	  VswitchVar->State = true;
	VswitchVar->OldVideal = VswitchVar->Videal;
      } else {
	VswitchVar->OldVideal = Inst->attr[N_mVinit - 1].UU.r;
	VswitchVar->Videal = VswitchVar->OldVideal;
	VswitchVar->State = false;
      }
      VswitchVar->Time = AnaAccum;
      Vswitchfill(act->inst, VswitchVar->State, false);
      ResetInit_Vswitch(act);
    }
    break;

  case N_Res:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].UU.r >
	  AnaResTooBig || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      VswitchVar->Conductance = 1 / Inst->attr[Attrnum - 1].UU.r;
    break;

  case N_bVinit:
  case N_bVpulse:
  case N_mVinit:
  case N_mVpulse:
    if (Inst->attr[Attrnum - 1].UU.r < AnaLowest ||
	Inst->attr[Attrnum - 1].blnk || Inst->attr[Attrnum - 1].UU.r > AnaHighest)
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
    if (VswitchVar->Reset_Time == -1e20)
      ResetInit_Vswitch(act);
    break;
  }
  return Result;
}


Local Void Attr_Vswitch(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 2)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Vswitchcheck(act->inst, Attrnum, act);
}



Local Void Newgate_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICVswitch *ICptr;
  long Index;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICVswitch *)Cellptr->Ainfo;
  VswitchVar = (VswitchConst *)Malloc(sizeof(VswitchConst));
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)VswitchVar;
  ResetInit_Vswitch(act);
  VswitchVar->Conductance = 1 / ICptr->ICRes;
  VswitchVar->Limit = false;
  VswitchVar->OldLimit = false;
  VswitchVar->NewLimit = false;
  VswitchVar->State = VswitchVar->Reset_State;
  VswitchVar->Time = VswitchVar->Reset_Time;
  VswitchVar->OldVideal = VswitchVar->Reset_Videal;
  VswitchVar->Videal = VswitchVar->OldVideal;
  AnaCapInit(act->inst);
  WITH = act->inst;
  WITH->attr[N_Res - 1].UU.r = ICptr->ICRes;
  WITH->attr[N_Type - 1].UU.nv = ICptr->ICType;
  WITH->attr[N_bVinit - 1].UU.r = ICptr->ICbVinit;
  WITH->attr[N_bVpulse - 1].UU.r = ICptr->ICbVpulse;
  WITH->attr[N_mVinit - 1].UU.r = ICptr->ICmVinit;
  WITH->attr[N_mVpulse - 1].UU.r = ICptr->ICmVpulse;
  WITH->attr[N_bRise - 1].UU.r = ICptr->ICbRise;
  WITH->attr[N_bFall - 1].UU.r = ICptr->ICbFall;
  WITH->attr[N_mRise - 1].UU.r = ICptr->ICmRise;
  WITH->attr[N_bState - 1].UU.nv = ICptr->ICbState;
  WITH->attr[N_mFall - 1].UU.r = ICptr->ICmFall;
  WITH->attr[N_mWidth - 1].UU.r = ICptr->ICmWidth;
  for (Index = 0; Index <= Cstart - 2; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar, *Old_Vswitchvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Vswitchvar = (VswitchConst *)A_Oldgate->InstVar;
  VswitchVar = (VswitchConst *)Malloc(sizeof(VswitchConst));
  *VswitchVar = *Old_Vswitchvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)VswitchVar;
  AnaCapCopy(act->inst);
}


Local Void Dispose_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  Free(VswitchVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;
  Char STR1[256];

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  VswitchVar->Conductance = 1 / WITH1->attr[N_Res - 1].UU.r;
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &VswitchVar->Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  VswitchVar->State = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  VswitchVar->Limit = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &VswitchVar->OldVideal);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &VswitchVar->Reset_Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  VswitchVar->Reset_State = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &VswitchVar->Reset_Videal);
  getc(*AnaLogglobals->actfile);
  VswitchVar->Time = AnaAccum - fabs(VswitchVar->Time);
      /*ABS due to -1e20 in old file formats*/
  VswitchVar->Videal = VswitchVar->OldVideal;
}

Local Void Writegate_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  AnaCapattrwrite((long)Cstart, act->inst);   /*Write capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  fprintf(*AnaLogglobals->actfile, "% .5E\n", AnaAccum - VswitchVar->Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  VswitchVar->State ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  VswitchVar->Limit ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", VswitchVar->OldVideal);
  fprintf(*AnaLogglobals->actfile, "% .5E\n", VswitchVar->Reset_Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  VswitchVar->Reset_State ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", VswitchVar->Reset_Videal);
}



Local Void Reset_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  VswitchVar->Limit = false;
  VswitchVar->OldLimit = false;
  VswitchVar->NewLimit = false;
  VswitchVar->Time = VswitchVar->Reset_Time;
  VswitchVar->State = VswitchVar->Reset_State;
  VswitchVar->OldVideal = VswitchVar->Reset_Videal;
  VswitchVar->Videal = VswitchVar->Reset_Videal;
  if (AnaLogglobals->showpage == act->page) {
    Vswitchfill(act->inst, VswitchVar->State, false);
    Limitfill(act->inst, VswitchVar->Limit, false);
  }
}


Local Void Set_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  VswitchVar->Reset_State = VswitchVar->State;
  VswitchVar->Reset_Videal = VswitchVar->OldVideal;
  VswitchVar->Reset_Time = VswitchVar->Time - AnaAccum;
}


Local Void PreUpdate_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr;
  double VPin1;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
  VPin1 = Pin1Ptr->last;
  VswitchVar->NewLimit = (fabs(VswitchVar->NewVideal - VPin1) > 0.1);
  if (WITH1->attr[N_Type - 1].UU.nv != Monostable)
    return;
  if (VswitchVar->State && AnaAccum + AnaDtlast >
	WITH1->attr[N_mRise - 1].UU.r + WITH1->attr[N_mFall - 1].UU.r +
	WITH1->attr[N_mWidth - 1].UU.r + VswitchVar->Time)
    VswitchVar->NewState = false;
  else
    VswitchVar->NewState = VswitchVar->State;
}

Local Void Update_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  if (act->inst->attr[N_Type - 1].UU.nv == Monostable) {
    VswitchVar->OldState = VswitchVar->State;
    VswitchVar->State = VswitchVar->NewState;
  }
  VswitchVar->Videal = VswitchVar->NewVideal;
  VswitchVar->OldLimit = VswitchVar->Limit;
  VswitchVar->Limit = VswitchVar->NewLimit;
}

Local Void Display_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage != act->page)
    return;
  if (WITH1->attr[N_Type - 1].UU.nv == Monostable) {
    if (VswitchVar->State != VswitchVar->OldState)
      Vswitchfill(act->inst, VswitchVar->State, false);
  }
  if (VswitchVar->OldLimit != VswitchVar->Limit)
    Limitfill(act->inst, VswitchVar->Limit, false);
}


Local Void Touch_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;
  log_action *WITH;
  log_grec *WITH1;

  WITH = AnaLogglobals;
  WITH1 = act->inst;
  if (WITH->actx != -2 && WITH->actx != -3 && WITH->actx != -4 || WITH->acty != 0)
    return;
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  if (WITH1->attr[N_Type - 1].UU.nv == Bistable) {
    VswitchVar->State = !VswitchVar->State;
    VswitchVar->Time = AnaAccum;
  } else {
    if (!VswitchVar->State) {
      VswitchVar->State = true;
      VswitchVar->Time = AnaAccum;
    }
  }
  VswitchVar->OldVideal = VswitchVar->Videal;
  Vswitchfill(act->inst, VswitchVar->State, false);
  WITH->actflag = true;
  AnaChanged = true;
}

Local Void Refresh_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  Vswitchfill(act->inst, VswitchVar->State, false);
  Limitfill(act->inst, VswitchVar->Limit, false);
}

Local Void Erase_Vswitch(act)
Analog_32_action *act;
{
  VswitchConst *VswitchVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  Vswitchfill(act->inst, VswitchVar->State, true);
  Limitfill(act->inst, VswitchVar->Limit, true);
}


Local Void Probe_Vswitch(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  VswitchConst *VswitchVar;
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
  AnaScoreboard(Gatename, (long)AnaMessGate1);
  if (Node1 != Pin1 || Node1 == AnaNotyet) {
    AnaScoreboard(" <undefined state>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    return;
  }
  A_Gate = (Anainstlist *)act->inst->info;
  VswitchVar = (VswitchConst *)A_Gate->InstVar;
  if (WITH->attr[N_Type - 1].UU.nv == Bistable) {
    if (VswitchVar->State)
      AnaScoreboard("Bistable - On", (long)AnaMessGate2);
    else
      AnaScoreboard("Bistable - Off", (long)AnaMessGate2);
  } else {
    if (VswitchVar->State)
      AnaScoreboard("Monostable - On", (long)AnaMessGate2);
    else
      AnaScoreboard("Monostable - Off", (long)AnaMessGate2);
  }
  sprintf(STR3, "V = %s (Ideal)", AnaProbeVoltStr(STR1, VswitchVar->Videal));
  AnaScoreboard(STR3, (long)AnaMessGate3);
  if (VswitchVar->Limit)
    AnaScoreboard("Current limited", (long)AnaMessGate4);
  else
    AnaScoreboard("Full voltage delivered", (long)AnaMessGate4);
}


/*=============================================================================*/
/*                 Library Listing For Cell Vswitch                            */
/*=============================================================================*/


Void Log_vswitch_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/

  switch (act->action) {

  case Analog_act_newkind:
    initlib_Vswitch(act);
    break;

  case Analog_act_ex:
    Ex_Vswitch(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Vswitch(act);
    break;

  case Analog_act_update:
    Update_Vswitch(act);
    break;

  case Analog_act_display:
    Display_Vswitch(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Vswitch(act);
    break;

  case Analog_act_reset:
    Reset_Vswitch(act);
    break;

  case Analog_act_newgate:
    Newgate_Vswitch(act);
    break;

  case Analog_act_copygate:
    Copygate_Vswitch(act);
    break;

  case Analog_act_disposegate:
    Dispose_Vswitch(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Vswitch(act);
    break;

  case Analog_act_writegate:
    Writegate_Vswitch(act);
    break;

  case Analog_act_Iin:
    Iin_Vswitch(act);
    break;

  case Analog_act_erase:
    Erase_Vswitch(act);
    break;

  case Analog_act_refresh:
    Refresh_Vswitch(act);
    break;

  case Analog_act_touch:
    Touch_Vswitch(act);
    break;

  case Analog_act_set:
    Set_Vswitch(act);
    break;

  case Analog_act_clear:
    ResetInit_Vswitch(act);
    break;

  case Analog_act_probe:
    Probe_Vswitch(act);
    break;
  }
}

#undef N_Res
#undef N_Type
#undef N_bVinit
#undef N_bVpulse
#undef N_bRise
#undef N_bFall
#undef N_bState
#undef N_mVinit
#undef N_mVpulse
#undef N_mRise
#undef N_mFall
#undef N_mWidth
#undef Cstart
#undef Bistable
#undef Monostable
#undef Off
#undef On
#undef Gatename





/* End. */
