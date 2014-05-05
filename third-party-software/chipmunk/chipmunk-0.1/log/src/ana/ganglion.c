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
/* From input file "stdlib.text" */


/*When its time to add subthreshold light, look at IDIFF for new update,
  write procedures*/





#include <p2c/p2c.h>




#define GANGLION_G
#include "ganglion.h"


#define N_CapVinGnd     1
#define N_CapVinVout    2
#define N_Threshold     3
#define N_OutRes        4
#define N_CapVleak      5
#define N_Steepness     6
#define N_Leakage       7
#define N_Vt            8
#define N_Ktqn          9
#define N_Early         10
#define Cstart          11

/*Actual pin coordinates*/

#define G_Vin           1
#define G_Vout          2
#define G_Vleak         3

#define MaxExpo         709

#define Gatename        "Ganglion"


typedef struct GanglionConst {

  /*Put any internal variables for system here*/
  /*Internal variables for transistor paramters used in model*/

  double V0;   /*kt/qn*/
  double Kn1;   /*nfet diode leakage current*(W/L)*/
  double Kn2;   /*qn/kt*/
  double Kn4;   /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  double TwoKn4;   /*2*Kn4*/
  double Kn5;   /*Expo(-Kn2*Vtn)*/
  double InvVcross;   /*Inverse Early Voltage*/

  double Kp1;   /*Kp1/3*/
  double Kp4;   /*Kp4/3*/
  double TwoKp4;   /*2*Kp4*/

  double TauVinVout, TauVin, TauVleak, Conductance;


  boolean Limit, OldLimit, NewLimit;

  /*For every 'state variable'*/
  /*OldState:Statetype;*/
  /*Newstate:Statetype;*/
  /*State:statetype;*/
} GanglionConst;


typedef struct ICGanglion {
  double InitCapVinGnd, InitCapVinVout, InitThreshold, InitOutRes,
	 InitCapVleak, InitSteepness, InitLeakage, InitVt,
	 InitKtqn, InitEarly;

  double InitCbVin, InitCbVout, InitCbVleak;
} ICGanglion;


typedef enum {
  On, Off, Switch
} Exposign;


/****************************************************************************/
/*            Procedure To Light 'Limiting' Light                           */
/****************************************************************************/


Local Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
boolean Limitflag, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = 8;
  URx = 12;
  LLy = 8;
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


/****************************************************************************/
/*                   Cell Installation Procedures                           */
/****************************************************************************/

Local Void GetCnf_Ganglion(NewIC)
ICGanglion *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICGanglion *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("GANGLION", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "CAPVINGND")) {
	TRY(try1);
	  WITH->InitCapVinGnd = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->InitCapVinGnd = 4e-12;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "CAPVINVOUT")) {
	TRY(try2);
	  WITH->InitCapVinVout = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->InitCapVinVout = 1e-12;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "THRESHOLD")) {
	TRY(try3);
	  WITH->InitThreshold = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitThreshold = 1.7;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "OUTRES")) {
	TRY(try4);
	  WITH->InitOutRes = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitOutRes = 50.0;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "CAPVLEAK")) {
	TRY(try5);
	  WITH->InitCapVleak = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitCapVleak = 10e-15;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "STEEPNESS")) {
	TRY(try6);
	  WITH->InitSteepness = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try6);
	  WITH->InitSteepness = 25.0;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "I0")) {
	TRY(try7);
	  WITH->InitLeakage = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try7);
	  WITH->InitLeakage = 5e-17;
	ENDTRY(try7);
      }
      if (!strcmp(Keyword, "VT")) {
	TRY(try8);
	  WITH->InitVt = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try8);
	  WITH->InitVt = 0.9;
	ENDTRY(try8);
      }
      if (!strcmp(Keyword, "EARLY")) {
	TRY(try9);
	  WITH->InitEarly = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try9);
	  WITH->InitEarly = 50.0;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "KT/QN")) {
	TRY(try10);
	  WITH->InitKtqn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try10);
	  WITH->InitKtqn = 0.04;
	ENDTRY(try10);
      }
      if (!strcmp(Keyword, "CBVIN")) {
	TRY(try11);
	  WITH->InitCbVin = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try11);
	  WITH->InitCbVin = 10e-15;
	ENDTRY(try11);
      }
      if (!strcmp(Keyword, "CBVOUT")) {
	TRY(try12);
	  WITH->InitCbVout = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try12);
	  WITH->InitCbVout = 10e-15;
	ENDTRY(try12);
      }
      if (!strcmp(Keyword, "CBVLEAK")) {
	TRY(try13);
	  WITH->InitCbVleak = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try13);
	  WITH->InitCbVleak = 10e-15;
	ENDTRY(try13);
      }
    }
  } while (Found);
}


Local Void INITLIB_GANGLION(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICGanglion *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[G_Vin - 1].standalone = false;
  NewCell->plist_cell[G_Vin - 1].active = true;
  NewCell->plist_cell[G_Vout - 1].standalone = false;
  NewCell->plist_cell[G_Vout - 1].active = true;
  NewCell->plist_cell[G_Vleak - 1].standalone = false;
  NewCell->plist_cell[G_Vleak - 1].active = true;
  NewIC = (ICGanglion *)Malloc(sizeof(ICGanglion));
  NewIC->InitCapVinGnd = 4e-12;
  NewIC->InitCapVinVout = 1e-12;
  NewIC->InitThreshold = 1.7;
  NewIC->InitOutRes = 50.0;
  NewIC->InitCapVleak = 10e-15;
  NewIC->InitSteepness = 25.0;
  NewIC->InitLeakage = 5e-17;
  NewIC->InitVt = 0.9;
  NewIC->InitKtqn = 0.04;
  NewIC->InitEarly = 50.0;
  NewIC->InitCbVin = 10e-15;
  NewIC->InitCbVout = 10e-15;
  NewIC->InitCbVleak = 10e-15;
  GetCnf_Ganglion(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Ganglion_Initlib*/



/*Some useful constants from globals (in addition to Vdd,Highest,Lowest,0)
      CapTooSmall=1e-60;
      CapTooBig=1e3;
      ResTooSmall=1e-15;
      ResTooBig=1e15;
      ITooBig=1e15;
      ITooSmall=1e-60;
      VTooBig=1e3;
      VTooSmall=1e-60;
      PhaseTooBig=1e3;
      FreqTooBig=1e5;
      TimeTooBig=1e60;
      TimeTooSmall=1e-60;*/



Local boolean Ganglioncheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Threshold:
    if (Inst->attr[Attrnum - 1].UU.r >= AnaVdd ||
	Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_OutRes:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 ||
	Inst->attr[Attrnum - 1].UU.r >= AnaResTooBig ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      GanglionVar->Conductance = 1 / Inst->attr[Attrnum - 1].UU.r;
    break;

  case N_Steepness:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else {
      GanglionVar->Kn4 = GanglionVar->Kn1 *
	  AnaExpo(GanglionVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
	  (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      GanglionVar->Kp4 = GanglionVar->Kn4 / 3;
      GanglionVar->TwoKn4 = 2 * GanglionVar->Kn4;
      GanglionVar->TwoKp4 = 2 * GanglionVar->Kp4;
      GanglionVar->Kn5 = AnaExpo(-GanglionVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Leakage:
    if ((unsigned)Inst->attr[N_Leakage - 1].UU.r > 1 ||
	Inst->attr[N_Leakage - 1].blnk)
      Result = false;
    else {
      GanglionVar->Kn1 = Inst->attr[N_Leakage - 1].UU.r;
      GanglionVar->Kp1 = GanglionVar->Kn1 / 3;
      GanglionVar->Kn4 = GanglionVar->Kn1 *
	  AnaExpo(GanglionVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
	  (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      GanglionVar->Kp4 = GanglionVar->Kn4 / 3;
      GanglionVar->TwoKn4 = 2 * GanglionVar->Kn4;
      GanglionVar->TwoKp4 = 2 * GanglionVar->Kp4;
    }
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[N_Ktqn - 1].UU.r > 1 || Inst->attr[N_Ktqn - 1].blnk)
      Result = false;
    else {
      GanglionVar->V0 = Inst->attr[N_Ktqn - 1].UU.r;
      GanglionVar->Kn2 = 1 / GanglionVar->V0;
      GanglionVar->Kn4 = GanglionVar->Kn1 *
	  AnaExpo(GanglionVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
	  (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      GanglionVar->Kp4 = GanglionVar->Kn4 / 3;
      GanglionVar->TwoKn4 = 2 * GanglionVar->Kn4;
      GanglionVar->TwoKp4 = 2 * GanglionVar->Kp4;
      GanglionVar->Kn5 = AnaExpo(-GanglionVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Early:
    if (Inst->attr[N_Early - 1].UU.r <= 0 ||
	Inst->attr[N_Early - 1].UU.r > AnaVTooBig)
      Result = false;
    else {
      if (Inst->attr[N_Early - 1].blnk)
	GanglionVar->InvVcross = 0.0;
      else
	GanglionVar->InvVcross = 1 / Inst->attr[N_Early - 1].UU.r;
    }
    break;
  }
  return Result;

  /*Recompute all internal variables that depend on it*/
}


Local Void Attr_Ganglion(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 8) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 &&
      ((1L << Attrnum) &
       ((1L << N_CapVinGnd) | (1L << N_CapVinVout) | (1L << N_CapVleak))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = Ganglioncheck(act->inst, Attrnum);
}


Local Void Newgate_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICGanglion *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICGanglion *)Cellptr->Ainfo;
  GanglionVar = (GanglionConst *)Malloc(sizeof(GanglionConst));
  GanglionVar->V0 = ICptr->InitKtqn;   /*kt/qn*/
  GanglionVar->Kn1 = ICptr->InitLeakage;   /*nfet diode leakage current*1*/
  GanglionVar->Kp1 = GanglionVar->Kn1 / 3;   /*pfet diode leakage current*1*/
  GanglionVar->Kn2 = 1 / GanglionVar->V0;   /*qn/kt*/
  GanglionVar->Kn4 = GanglionVar->Kn1 * AnaExpo(
	GanglionVar->Kn2 * ICptr->InitVt) / (ICptr->InitVt * ICptr->InitVt);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  GanglionVar->Kp4 = GanglionVar->Kn4 / 3;   /*p equivalent*/
  GanglionVar->TwoKn4 = 2 * GanglionVar->Kn4;   /*2*Kn4*/
  GanglionVar->TwoKp4 = 2 * GanglionVar->Kp4;   /*2*Kp4*/
  GanglionVar->Kn5 = AnaExpo(-GanglionVar->Kn2 * ICptr->InitVt);
      /*Expo(-Kn2*Vtn)*/
  if (ICptr->InitEarly == 0)
    GanglionVar->InvVcross = 0.0;
  else
    GanglionVar->InvVcross = 1 / ICptr->InitEarly;
  GanglionVar->Conductance = 1 / ICptr->InitOutRes;
  GanglionVar->Limit = false;
  GanglionVar->OldLimit = false;
  GanglionVar->NewLimit = false;
  /*Give internal variables values derived from initial attribute values*/
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)GanglionVar;
  AnaCapInit(act->inst);   /*Initialize attributes of Cstart*/
  A_Gate->Pininfo[G_Vin - 1].Cap = ICptr->InitCbVin;
  A_Gate->Pininfo[G_Vout - 1].Cap = ICptr->InitCbVout;
  A_Gate->Pininfo[G_Vleak - 1].Cap = ICptr->InitCbVleak;
  WITH = act->inst;
  WITH->attr[N_CapVinGnd - 1].UU.r = ICptr->InitCapVinGnd;
  WITH->attr[N_CapVinGnd - 1].blnk = false;
  WITH->attr[N_CapVinGnd - 1].changed = true;
  WITH->attr[N_CapVinVout - 1].UU.r = ICptr->InitCapVinVout;
  WITH->attr[N_CapVinVout - 1].blnk = false;
  WITH->attr[N_CapVinVout - 1].changed = true;
  WITH->attr[N_Threshold - 1].UU.r = ICptr->InitThreshold;
  WITH->attr[N_Threshold - 1].blnk = false;
  WITH->attr[N_Threshold - 1].changed = true;
  WITH->attr[N_OutRes - 1].UU.r = ICptr->InitOutRes;
  WITH->attr[N_OutRes - 1].blnk = false;
  WITH->attr[N_OutRes - 1].changed = true;
  WITH->attr[N_CapVleak - 1].UU.r = ICptr->InitCapVleak;
  WITH->attr[N_CapVleak - 1].blnk = false;
  WITH->attr[N_CapVleak - 1].changed = true;
  WITH->attr[N_Steepness - 1].UU.r = ICptr->InitSteepness;
  WITH->attr[N_Steepness - 1].blnk = false;
  WITH->attr[N_Steepness - 1].changed = true;
  WITH->attr[N_Leakage - 1].UU.r = ICptr->InitLeakage;
  WITH->attr[N_Leakage - 1].blnk = false;
  WITH->attr[N_Leakage - 1].changed = true;
  WITH->attr[N_Vt - 1].UU.r = ICptr->InitVt;
  WITH->attr[N_Vt - 1].blnk = false;
  WITH->attr[N_Vt - 1].changed = true;
  WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;
  WITH->attr[N_Early - 1].blnk = false;
  WITH->attr[N_Early - 1].changed = true;
  WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;
  WITH->attr[N_Ktqn - 1].blnk = false;
  WITH->attr[N_Ktqn - 1].changed = true;
}

Local Void Copygate_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar, *Old_Ganglionvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Ganglionvar = (GanglionConst *)A_Oldgate->InstVar;
  GanglionVar = (GanglionConst *)Malloc(sizeof(GanglionConst));
  *GanglionVar = *Old_Ganglionvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)GanglionVar;
  AnaCapCopy(act->inst);
}


Local Void Dispose_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  Free(GanglionVar);
  AnaCapDispose(act->inst);
}


Local Void Readgate_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICGanglion *ICptr;
  log_grec *WITH;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICGanglion *)Cellptr->Ainfo;
    A_Gate->Pininfo[G_Vin - 1].Cap = ICptr->InitCbVin;
    A_Gate->Pininfo[G_Vout - 1].Cap = ICptr->InitCbVout;
    A_Gate->Pininfo[G_Vleak - 1].Cap = ICptr->InitCbVleak;
    WITH = act->inst;
    WITH->attr[N_CapVinGnd - 1].UU.r = ICptr->InitCapVinGnd;
    WITH->attr[N_CapVinGnd - 1].blnk = false;
    WITH->attr[N_CapVinGnd - 1].changed = true;
    WITH->attr[N_CapVinVout - 1].UU.r = ICptr->InitCapVinVout;
    WITH->attr[N_CapVinVout - 1].blnk = false;
    WITH->attr[N_CapVinVout - 1].changed = true;
    WITH->attr[N_Threshold - 1].UU.r = ICptr->InitThreshold;
    WITH->attr[N_Threshold - 1].blnk = false;
    WITH->attr[N_Threshold - 1].changed = true;
    WITH->attr[N_OutRes - 1].UU.r = ICptr->InitOutRes;
    WITH->attr[N_OutRes - 1].blnk = false;
    WITH->attr[N_OutRes - 1].changed = true;
    WITH->attr[N_CapVleak - 1].UU.r = ICptr->InitCapVleak;
    WITH->attr[N_CapVleak - 1].blnk = false;
    WITH->attr[N_CapVleak - 1].changed = true;
    WITH->attr[N_Steepness - 1].UU.r = ICptr->InitSteepness;
    WITH->attr[N_Steepness - 1].blnk = false;
    WITH->attr[N_Steepness - 1].changed = true;
    WITH->attr[N_Leakage - 1].UU.r = ICptr->InitLeakage;
    WITH->attr[N_Leakage - 1].blnk = false;
    WITH->attr[N_Leakage - 1].changed = true;
    WITH->attr[N_Vt - 1].UU.r = ICptr->InitVt;
    WITH->attr[N_Vt - 1].blnk = false;
    WITH->attr[N_Vt - 1].changed = true;
    WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;
    WITH->attr[N_Early - 1].blnk = false;
    WITH->attr[N_Early - 1].changed = true;
    WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;
    WITH->attr[N_Ktqn - 1].blnk = false;
    WITH->attr[N_Ktqn - 1].changed = true;
  }
  WITH = act->inst;
  GanglionVar->V0 = WITH->attr[N_Ktqn - 1].UU.r;   /*kt/qn*/
  GanglionVar->Kn1 = WITH->attr[N_Leakage - 1].UU.r;
      /*n diode leakage current*(W/L)*/
  GanglionVar->Kp1 = GanglionVar->Kn1 / 3;   /*p diode leakage current*(W/L)*/
  GanglionVar->Kn2 = 1 / GanglionVar->V0;   /*qn/kt*/
  GanglionVar->Kn4 = GanglionVar->Kn1 *
		     AnaExpo(GanglionVar->Kn2 * WITH->attr[N_Vt - 1].UU.r) /
		     (WITH->attr[N_Vt - 1].UU.r * WITH->attr[N_Vt - 1].UU.r);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  GanglionVar->Kp4 = GanglionVar->Kn4 / 3;   /*p equivalent*/
  GanglionVar->TwoKn4 = 2 * GanglionVar->Kn4;   /*2*Kn4*/
  GanglionVar->TwoKp4 = 2 * GanglionVar->Kp4;   /*2*Kp4*/
  GanglionVar->Kn5 = AnaExpo(-GanglionVar->Kn2 * WITH->attr[N_Vt - 1].UU.r);
      /*Expo(-Kn2*Vtn)*/
  if (WITH->attr[N_Early - 1].blnk)
    GanglionVar->InvVcross = 0.0;
  else
    GanglionVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
  GanglionVar->Conductance = 1 / WITH->attr[N_OutRes - 1].UU.r;
}


Local Void Reset_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  GanglionVar->Limit = false;
  GanglionVar->OldLimit = false;
  GanglionVar->NewLimit = false;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, GanglionVar->Limit, false);
}

Local Void PreUpdate_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  AnaExt_rec *PinInPtr;
  double VPinIn;
  AnaExt_rec *PinOutPtr;
  double VPinOut;
  AnaExt_rec *PinLeakPtr;
  double VPinLeak;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  PinInPtr = (AnaExt_rec *)WITH1->pin[G_Vin - 1]->info;
  VPinIn = PinInPtr->last;
  PinOutPtr = (AnaExt_rec *)WITH1->pin[G_Vout - 1]->info;
  VPinOut = PinOutPtr->last;
  PinLeakPtr = (AnaExt_rec *)WITH1->pin[G_Vleak - 1]->info;
  VPinLeak = PinLeakPtr->last;
  GanglionVar->NewLimit = (VPinLeak >= WITH1->attr[N_Vt - 1].UU.r);
}


Local Void Update_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  GanglionVar->OldLimit = GanglionVar->Limit;
  GanglionVar->Limit = GanglionVar->NewLimit;
}


Local Void Display_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (GanglionVar->OldLimit != GanglionVar->Limit)
      Limitfill(act->inst, GanglionVar->Limit, false);
  }
}


Local Void Refresh_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  Limitfill(act->inst, GanglionVar->Limit, false);
}



Local Void Erase_Ganglion(act)
Analog_32_action *act;
{
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  Limitfill(act->inst, GanglionVar->Limit, true);
}


/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_Ganglion(act)
Analog_32_action *act;
{
  double MinIin, dIindVin, dIindVout, dIindVleak, MinIout, dIoutdVin,
	 dIoutdVout, dIoutdVleak, MinIleak, dIleakdVin, dIleakdVout,
	 dIleakdVleak;
  AnaExt_rec *PinInPtr;
  long PinIn;
  double VPinIn, VMemIn;
  AnaExt_rec *PinOutPtr;
  long PinOut;
  double VPinOut, VMemOut;
  AnaExt_rec *PinLeakPtr;
  long PinLeak;
  double VPinLeak, VMemLeak;
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  double Icap, DeltaVin, DeltaVout, DeltaVleak, ExpoArg, ExpoVleak,
	 Kn5_ExpoVleak, BlendVleak, InvBlendVleak, dBlendVleakdVleak,
	 Kn1_ExpoVleak, Kn1_ExpoVleak_BlendVleak, Kn4_Vpinleak_Vpinleak,
	 PureLeak, dPureLeakdVleak, ExpoThreshold, BlendVin, dBlendVindVin;
  Exposign Sign;
  double Res, FullLeak, dFullLeakdVleak, dFullLeakdVin;
  log_grec *WITH;
  AnaMatRec *WITH2;



  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  PinInPtr = (AnaExt_rec *)WITH->pin[G_Vin - 1]->info;
  PinIn = PinInPtr->nodenum;
  VPinIn = PinInPtr->last;
  VMemIn = A_Gate->Pininfo[G_Vin - 1].Vmem;
  PinOutPtr = (AnaExt_rec *)WITH->pin[G_Vout - 1]->info;
  PinOut = PinOutPtr->nodenum;
  VPinOut = PinOutPtr->last;
  VMemOut = A_Gate->Pininfo[G_Vout - 1].Vmem;
  PinLeakPtr = (AnaExt_rec *)WITH->pin[G_Vleak - 1]->info;
  PinLeak = PinLeakPtr->nodenum;
  VPinLeak = PinLeakPtr->last;
  VMemLeak = A_Gate->Pininfo[G_Vleak - 1].Vmem;
      /*Use with statement to get to internal variables*/

  ExpoArg = WITH->attr[N_Steepness - 1].UU.r *
	    (WITH->attr[N_Threshold - 1].UU.r - VPinIn);
  if (fabs(ExpoArg) < MaxExpo) {
    ExpoThreshold = AnaExpo(ExpoArg);
    BlendVin = 1 / (1 + ExpoThreshold);
    dBlendVindVin = WITH->attr[N_Steepness - 1].UU.r * BlendVin * BlendVin *
		    ExpoThreshold;
    Sign = Switch;
  } else {
    if (ExpoArg > 0) {
      BlendVin = 0.0;
      dBlendVindVin = 0.0;
      Sign = Off;
    } else {
      BlendVin = 1.0;
      dBlendVindVin = 0.0;
      Sign = On;
    }
  }

  if (Sign == On || Sign == Switch) {
    ExpoVleak = AnaExpo(GanglionVar->Kn2 * VPinLeak);
    Kn5_ExpoVleak = GanglionVar->Kn5 * ExpoVleak;
    BlendVleak = 1 / (1 + Kn5_ExpoVleak);
    InvBlendVleak = 1 - BlendVleak;
    dBlendVleakdVleak = -GanglionVar->Kn2 * Kn5_ExpoVleak * BlendVleak * BlendVleak;

    Kn1_ExpoVleak = GanglionVar->Kn1 * ExpoVleak;
    Kn1_ExpoVleak_BlendVleak = Kn1_ExpoVleak * BlendVleak;
    Kn4_Vpinleak_Vpinleak = GanglionVar->Kn4 * VPinLeak * VPinLeak;
    PureLeak = Kn1_ExpoVleak_BlendVleak + InvBlendVleak * Kn4_Vpinleak_Vpinleak;
    dPureLeakdVleak = GanglionVar->Kn2 * Kn1_ExpoVleak_BlendVleak +
	dBlendVleakdVleak * (Kn1_ExpoVleak - Kn4_Vpinleak_Vpinleak) +
	InvBlendVleak * GanglionVar->TwoKn4 * VPinLeak;

    Res = 1 + VPinIn * GanglionVar->InvVcross;
    FullLeak = PureLeak * Res;
    dFullLeakdVleak = dPureLeakdVleak * Res;
    dFullLeakdVin = PureLeak * GanglionVar->InvVcross;


    MinIin = -FullLeak * BlendVin;
    dIindVin = FullLeak * dBlendVindVin + dFullLeakdVin * BlendVin;
    dIindVleak = dFullLeakdVleak * BlendVin;

    MinIout = GanglionVar->Conductance * (AnaVdd * BlendVin - VPinOut);
    dIoutdVout = GanglionVar->Conductance;
    dIoutdVin = -GanglionVar->Conductance * AnaVdd * dBlendVindVin;
  } else {
    MinIin = 0.0;
    dIindVin = 0.0;
    dIindVleak = 0.0;
    MinIout = -GanglionVar->Conductance * VPinOut;
    dIoutdVout = GanglionVar->Conductance;
    dIoutdVin = 0.0;
  }


  DeltaVin = VPinIn - VMemIn;
  DeltaVout = VPinOut - VMemOut;
  DeltaVleak = VPinLeak - VMemLeak;


  if (AnaChangedDt)
    GanglionVar->TauVin = WITH->attr[N_CapVinGnd - 1].UU.r / AnaDt;
  Icap = GanglionVar->TauVin * DeltaVin;
  MinIin -= Icap;
  dIindVin += GanglionVar->TauVin;

  if (AnaChangedDt)
    GanglionVar->TauVinVout = WITH->attr[N_CapVinVout - 1].UU.r / AnaDt;
  Icap = GanglionVar->TauVinVout * (DeltaVin - DeltaVout);
  MinIin -= Icap;
  dIindVout = -GanglionVar->TauVinVout;
  dIindVin += GanglionVar->TauVinVout;
  MinIout += Icap;
  dIoutdVout += GanglionVar->TauVinVout;
  dIoutdVin -= GanglionVar->TauVinVout;

  if (AnaChangedDt)
    GanglionVar->TauVleak = WITH->attr[N_CapVleak - 1].UU.r / AnaDt;
  Icap = GanglionVar->TauVleak * (DeltaVleak - DeltaVin);
  MinIleak = -Icap;
  dIleakdVin = -GanglionVar->TauVleak;
  dIleakdVleak = GanglionVar->TauVleak;
  MinIin += Icap;
  dIindVin += GanglionVar->TauVleak;
  dIindVleak -= GanglionVar->TauVleak;
  Icap = GanglionVar->TauVleak * (DeltaVleak - DeltaVout);
  MinIleak -= Icap;
  dIleakdVout = -GanglionVar->TauVleak;
  dIleakdVleak += GanglionVar->TauVleak;
  MinIout += Icap;
  dIoutdVout += GanglionVar->TauVleak;
  dIoutdVleak = -GanglionVar->TauVleak;


  WITH2 = AnaSystem;
  WITH2->Mat[PinIn][AnaSysCol] += MinIin;
  WITH2->Mat[PinIn][PinIn] += dIindVin;
  WITH2->Mat[PinIn][PinOut] += dIindVout;
  WITH2->Mat[PinIn][PinLeak] += dIindVleak;
  WITH2->Mat[PinOut][AnaSysCol] += MinIout;
  WITH2->Mat[PinOut][PinIn] += dIoutdVin;
  WITH2->Mat[PinOut][PinOut] += dIoutdVout;
  WITH2->Mat[PinOut][PinLeak] += dIoutdVleak;
  WITH2->Mat[PinLeak][AnaSysCol] += MinIleak;
  WITH2->Mat[PinLeak][PinIn] += dIleakdVin;
  WITH2->Mat[PinLeak][PinOut] += dIleakdVout;
  WITH2->Mat[PinLeak][PinLeak] += dIleakdVleak;
  WITH2->MatNZ[PinIn][AnaSysCol] = true;
  WITH2->MatNZ[PinIn][PinIn] = true;
  WITH2->MatNZ[PinIn][PinOut] = true;
  WITH2->MatNZ[PinIn][PinLeak] = true;
  WITH2->MatNZ[PinOut][AnaSysCol] = true;
  WITH2->MatNZ[PinOut][PinIn] = true;
  WITH2->MatNZ[PinOut][PinOut] = true;
  WITH2->MatNZ[PinOut][PinLeak] = true;
  WITH2->MatNZ[PinLeak][AnaSysCol] = true;
  WITH2->MatNZ[PinLeak][PinIn] = true;
  WITH2->MatNZ[PinLeak][PinOut] = true;
  WITH2->MatNZ[PinLeak][PinLeak] = true;
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_Ganglion*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Ganglion(act)
Analog_32_action *act;
{
  AnaExt_rec *PinInPtr;
  double VPinIn, VMemIn;
  AnaExt_rec *PinOutPtr;
  double VPinOut, VMemOut;
  AnaExt_rec *PinLeakPtr;
  double VPinLeak, VMemLeak;
  GanglionConst *GanglionVar;
  Anainstlist *A_Gate;
  double DeltaVin, DeltaVout, DeltaVleak, ExpoArg, ExpoVleak, BlendVleak;
  log_grec *WITH;


  WITH = act->inst;   /*Use Dtlast, Dtlast+Accum for time values*/
  A_Gate = (Anainstlist *)WITH->info;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  PinInPtr = (AnaExt_rec *)WITH->pin[G_Vin - 1]->info;
  VPinIn = PinInPtr->now;
  VMemIn = A_Gate->Pininfo[G_Vin - 1].Vmem;
  PinOutPtr = (AnaExt_rec *)WITH->pin[G_Vout - 1]->info;
  VPinOut = PinOutPtr->now;
  VMemOut = A_Gate->Pininfo[G_Vout - 1].Vmem;
  PinLeakPtr = (AnaExt_rec *)WITH->pin[G_Vleak - 1]->info;
  VPinLeak = PinLeakPtr->now;
  VMemLeak = A_Gate->Pininfo[G_Vleak - 1].Vmem;
  switch (act->pin) {

  case G_Vin:   /*Allows attr^[]. access*/
    WITH = act->inst;
    DeltaVin = VPinIn - VMemIn;
    act->Iin = GanglionVar->TauVinVout * (DeltaVin - VPinOut + VMemOut) + act->Iin;
    act->Iin = GanglionVar->TauVleak * (DeltaVin - VPinLeak + VMemLeak) + act->Iin;
    act->Iin = GanglionVar->TauVin * DeltaVin + act->Iin;

    ExpoArg = WITH->attr[N_Steepness - 1].UU.r *
	      (WITH->attr[N_Threshold - 1].UU.r - VPinIn);
    if (fabs(ExpoArg) < MaxExpo) {
      ExpoVleak = AnaExpo(GanglionVar->Kn2 * VPinLeak);
      BlendVleak = 1 / (1 + GanglionVar->Kn5 * ExpoVleak);
      act->Iin =
	1 / (1 + AnaExpo(ExpoArg)) * (1 + VPinIn * GanglionVar->InvVcross) *
	(GanglionVar->Kn1 * ExpoVleak * BlendVleak +
	 (1 - BlendVleak) * GanglionVar->Kn4 * VPinLeak * VPinLeak) + act->Iin;
    } else {
      if (ExpoArg < 0) {
	ExpoVleak = AnaExpo(GanglionVar->Kn2 * VPinLeak);
	BlendVleak = 1 / (1 + GanglionVar->Kn5 * ExpoVleak);
	act->Iin = (1 + VPinIn * GanglionVar->InvVcross) *
		   (GanglionVar->Kn1 * ExpoVleak * BlendVleak + (1 - BlendVleak) *
		      GanglionVar->Kn4 * VPinLeak * VPinLeak) + act->Iin;
      }
    }
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case G_Vout:   /*Allows attr^[]. access*/
    WITH = act->inst;
    DeltaVout = VPinOut - VMemOut;
    act->Iin = GanglionVar->TauVinVout * (DeltaVout - VPinIn + VMemIn) + act->Iin;
    act->Iin = GanglionVar->TauVleak * (DeltaVout - VPinLeak + VMemLeak) + act->Iin;

    ExpoArg = WITH->attr[N_Steepness - 1].UU.r *
	      (WITH->attr[N_Threshold - 1].UU.r - VPinIn);
    if (fabs(ExpoArg) < MaxExpo)
      act->Iin = GanglionVar->Conductance *
		 (VPinOut - AnaVdd * (1 / (1 + AnaExpo(ExpoArg)))) + act->Iin;
    else {
      if (ExpoArg > 0)
	act->Iin = GanglionVar->Conductance * VPinOut + act->Iin;
      else
	act->Iin = GanglionVar->Conductance * (VPinOut - AnaVdd) + act->Iin;
    }

    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case G_Vleak:   /*Allows attr^[]. access*/
    WITH = act->inst;
    DeltaVleak = VPinLeak - VMemLeak;
    act->Iin = GanglionVar->TauVleak * (DeltaVleak - VPinIn + VMemIn) + act->Iin;
    act->Iin = GanglionVar->TauVleak * (DeltaVleak - VPinOut + VMemOut) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  default:
    printf("Data Structure Corruption\n");
    _Escape(0);
    break;
  }
}  /*Iin_Ganglion*/


Local Void Probe_Ganglion(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *NodeInPtr, *NodeOutPtr, *NodeLeakPtr;
  double VPinIn, VPinOut, VPinLeak, NodeIn, NodeOut, NodeLeak;
  GanglionConst *GanglionVar;
  double ExpoVleak, BlendVleak, Ileak;
  log_grec *WITH;
  Char STR1[22];
  Char STR2[256];
  Char STR3[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  VPinIn = A_Gate->Pininfo[G_Vin - 1].Vmem;
  VPinOut = A_Gate->Pininfo[G_Vout - 1].Vmem;
  VPinLeak = A_Gate->Pininfo[G_Vleak - 1].Vmem;
  NodeInPtr = (AnaExt_rec *)WITH->pin[G_Vin - 1]->info;
  NodeOutPtr = (AnaExt_rec *)WITH->pin[G_Vout - 1]->info;
  NodeLeakPtr = (AnaExt_rec *)WITH->pin[G_Vleak - 1]->info;
  NodeIn = NodeInPtr->ltimestep;
  NodeOut = NodeOutPtr->ltimestep;
  NodeLeak = NodeLeakPtr->ltimestep;
  GanglionVar = (GanglionConst *)A_Gate->InstVar;
  if (VPinIn != NodeIn || VPinOut != NodeOut || VPinLeak != NodeLeak ||
      VPinIn == AnaNotyet || VPinOut == AnaNotyet || VPinLeak == AnaNotyet) {
    sprintf(STR1, "%s <undefined>", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    return;
  }
  AnaScoreboard(Gatename, (long)AnaMessGate1);
  if (GanglionVar->Limit) {
    AnaScoreboard("Ileak out of subthreshold", (long)AnaMessGate2);
    return;
  }
  ExpoVleak = AnaExpo(GanglionVar->Kn2 * VPinLeak);
  BlendVleak = 1 / (1 + GanglionVar->Kn5 * ExpoVleak);
  Ileak = GanglionVar->Kn1 * ExpoVleak * BlendVleak +
	  GanglionVar->Kn4 * VPinLeak * VPinLeak * (1 - BlendVleak);
  sprintf(STR3, "Ileak = %s", AnaProbeAmpStr(STR2, Ileak));
  AnaScoreboard(STR3, (long)AnaMessGate2);
}



Void Log_ganglion_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    INITLIB_GANGLION(act);
    break;

  case Analog_act_ex:
    Ex_Ganglion(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Ganglion(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    Update_Ganglion(act);
    break;

  case Analog_act_display:
    Display_Ganglion(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Ganglion(act);
    break;

  case Analog_act_reset:
    Reset_Ganglion(act);
    break;

  case Analog_act_newgate:
    Newgate_Ganglion(act);
    break;

  case Analog_act_copygate:
    Copygate_Ganglion(act);
    break;

  case Analog_act_disposegate:
    Dispose_Ganglion(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Ganglion(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Ganglion(act);
    break;

  case Analog_act_erase:
    Erase_Ganglion(act);
    break;

  case Analog_act_refresh:
    Refresh_Ganglion(act);
    break;

  case Analog_act_probe:
    Probe_Ganglion(act);
    break;
  }
}

#undef N_CapVinGnd
#undef N_CapVinVout
#undef N_Threshold
#undef N_OutRes
#undef N_CapVleak
#undef N_Steepness
#undef N_Leakage
#undef N_Vt
#undef N_Ktqn
#undef N_Early
#undef Cstart
#undef G_Vin
#undef G_Vout
#undef G_Vleak
#undef MaxExpo
#undef Gatename




/* End. */
