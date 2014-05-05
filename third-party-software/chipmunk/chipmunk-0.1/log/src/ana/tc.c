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




#define TC_G
#include "tc.h"




#define Normal          0
#define Wide            1
#define Hwr             2
#define Fwr             3

#define N_Voffset       1
#define N_Ipos          2
#define N_Ineg          3
#define N_IKtqn         4

#define N_Leakage       5
#define N_Vt            6
#define N_Early         7
#define N_Ktqn          8
#define N_CapVset       9
#define N_CapVout       10
#define N_CapVdiff      11

#define Cstart          12

/*Actual pin coordinates*/

#define R_Vplus         2   /*Virtual pin 1 for TC family*/
#define R_Vminus        1   /*Virtual pin 2 for TC family*/
#define R_Vout          4   /*Virtual pin 3 for TC family*/
#define R_Vset          3   /*Virtual pin 4 for TC family*/

#define A_Vplus         1   /*Virtual pin 1 for ABS family*/
#define A_Vminus        2   /*Virtual pin 2 for ABS family*/
#define A_Vout          4   /*Virtual pin 3 for ABS family*/
#define A_Vset          3   /*Virtual pin 4 for ABS family*/

#define NormalName      "Transconductance Amp"
#define WideName        "Wide Range Amp"
#define HwrName         "Half Wave Rectifier"
#define FwrName         "Full Wave Rectifier"




typedef struct TCAmpConst {

  /*Put any internal variables for system here*/
  /*Internal variables for transistor paramters used in model*/

  double IV0;   /*kt/qn for tanh*/
  double IKn2;   /*qn/kt for tanh*/
  double HalfIKn2;   /*0.5*IKn2*/
  double HalfOffset;   /*0.5*attr^[Voffset].r*/

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

  double TauVset, TauVout, TauVdiff;

  boolean Limit, OldLimit, NewLimit;

  /*For every 'state variable'*/
  /*OldState:Statetype;*/
  /*Newstate:Statetype;*/
  /*State:statetype;*/
} TCAmpConst;


typedef struct ICTCAmp {
  double InitVoffset, InitIpos, InitIneg, InitIKtqn;

  double InitLeakage, InitVt, InitEarly, InitKtqn, InitCapVset, InitCapVout,
	 InitCapVdiff;

  double InitCgbVplus, InitCgbVminus, InitCdbVout, InitCgbVset;
} ICTCAmp;


typedef enum {
  Positive, Negative, Zero
} Deltasign;


Static long AmpRange;


/*The library of shared procedures among all four gates*/


/****************************************************************************/
/*            Procedure To Light 'Limiting' Light                           */
/****************************************************************************/


Static Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
boolean Limitflag, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = -7;
  URx = -3;
  LLy = 2;
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


/****************************************************************************/
/*                   Cell Installation Procedures                           */
/****************************************************************************/

Static Void GetCnf_TCAmp(NewIC)
ICTCAmp *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICTCAmp *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    switch (AmpRange) {

    case Normal:
      AnaGetcommand("OPAMP", Arg, &Found);
      break;

    case Wide:
      AnaGetcommand("WRAMP", Arg, &Found);
      break;

    case Hwr:
      AnaGetcommand("HWR", Arg, &Found);
      break;

    case Fwr:
      AnaGetcommand("FWR", Arg, &Found);
      break;
    }
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "VOFFSET")) {
	TRY(try1);
	  WITH->InitVoffset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->InitVoffset = 0.0;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "IPOS")) {
	TRY(try2);
	  WITH->InitIpos = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->InitIpos = 1.0;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "INEG")) {
	TRY(try3);
	  WITH->InitIneg = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitIneg = 1.0;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "IKT/QN")) {
	TRY(try4);
	  WITH->InitIKtqn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitIKtqn = 0.04;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "I0")) {
	TRY(try5);
	  WITH->InitLeakage = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitLeakage = 5e-17;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "VT")) {
	TRY(try6);
	  WITH->InitVt = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try6);
	  WITH->InitVt = 0.9;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "EARLY")) {
	TRY(try7);
	  WITH->InitEarly = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try7);
	  WITH->InitEarly = 50.0;
	ENDTRY(try7);
      }
      if (!strcmp(Keyword, "KT/QN")) {
	TRY(try8);
	  WITH->InitKtqn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try8);
	  WITH->InitKtqn = 0.04;
	ENDTRY(try8);
      }
      if (!strcmp(Keyword, "CAPVSET")) {
	TRY(try9);
	  WITH->InitCapVset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try9);
	  WITH->InitCapVset = 10e-15;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "CAPVOUT")) {
	TRY(try10);
	  WITH->InitCapVout = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try10);
	  WITH->InitCapVout = 10e-15;
	ENDTRY(try10);
      }
      if (!strcmp(Keyword, "CAPVDIFF")) {
	TRY(try11);
	  WITH->InitCapVdiff = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try11);
	  WITH->InitCapVdiff = 10e-15;
	ENDTRY(try11);
      }
      if (!strcmp(Keyword, "CGBVPLUS")) {
	TRY(try12);
	  WITH->InitCgbVplus = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try12);
	  WITH->InitCgbVplus = 10e-15;
	ENDTRY(try12);
      }
      if (!strcmp(Keyword, "CGBVMINUS")) {
	TRY(try13);
	  WITH->InitCgbVminus = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try13);
	  WITH->InitCgbVminus = 10e-15;
	ENDTRY(try13);
      }
      if (!strcmp(Keyword, "CDBVOUT")) {
	TRY(try14);
	  WITH->InitCdbVout = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try14);
	  WITH->InitCdbVout = 10e-15;
	ENDTRY(try14);
      }
      if (!strcmp(Keyword, "CGBVSET")) {
	TRY(try15);
	  WITH->InitCgbVset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try15);
	  WITH->InitCgbVset = 10e-15;
	ENDTRY(try15);
      }
    }
  } while (Found);
}


Static Void INITLIB_TCAMP(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICTCAmp *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  switch (AmpRange) {

  case Normal:
  case Wide:
    NewCell->plist_cell[R_Vplus - 1].standalone = false;
    NewCell->plist_cell[R_Vplus - 1].active = true;
    NewCell->plist_cell[R_Vminus - 1].standalone = false;
    NewCell->plist_cell[R_Vminus - 1].active = true;
    NewCell->plist_cell[R_Vout - 1].standalone = false;
    NewCell->plist_cell[R_Vout - 1].active = true;
    NewCell->plist_cell[R_Vset - 1].standalone = false;
    NewCell->plist_cell[R_Vset - 1].active = true;
    break;

  case Hwr:
  case Fwr:
    NewCell->plist_cell[A_Vplus - 1].standalone = false;
    NewCell->plist_cell[A_Vplus - 1].active = true;
    NewCell->plist_cell[A_Vminus - 1].standalone = false;
    NewCell->plist_cell[A_Vminus - 1].active = true;
    NewCell->plist_cell[A_Vout - 1].standalone = false;
    NewCell->plist_cell[A_Vout - 1].active = true;
    NewCell->plist_cell[A_Vset - 1].standalone = false;
    NewCell->plist_cell[A_Vset - 1].active = true;
    break;
  }
  NewIC = (ICTCAmp *)Malloc(sizeof(ICTCAmp));
  NewIC->InitVoffset = 0.0;
  NewIC->InitIpos = 1.0;
  NewIC->InitIneg = 1.0;
  NewIC->InitIKtqn = 0.04;
  NewIC->InitLeakage = 5e-17;
  NewIC->InitVt = 0.9;
  NewIC->InitEarly = 50.0;
  NewIC->InitKtqn = 0.04;
  NewIC->InitCapVset = 10e-15;
  NewIC->InitCapVout = 10e-15;
  NewIC->InitCapVdiff = 10e-15;
  NewIC->InitCgbVplus = 10e-15;
  NewIC->InitCgbVminus = 10e-15;
  NewIC->InitCdbVout = 10e-15;
  NewIC->InitCgbVset = 10e-15;
  GetCnf_TCAmp(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*TCAmp_Initlib*/


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


Static boolean TCAmpcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Voffset:
    if (fabs(Inst->attr[Attrnum - 1].UU.r) > AnaVdd || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      TCAmpVar->HalfOffset = Inst->attr[Attrnum - 1].UU.r / 2;
    break;

  case N_Ipos:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Ineg:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || AmpRange == Hwr ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_IKtqn:
    if ((unsigned)Inst->attr[N_IKtqn - 1].UU.r > 1 || Inst->attr[N_IKtqn - 1].blnk)
      Result = false;
    else {
      TCAmpVar->IV0 = Inst->attr[N_IKtqn - 1].UU.r;
      TCAmpVar->IKn2 = 1 / TCAmpVar->IV0;
      TCAmpVar->HalfIKn2 = TCAmpVar->IKn2 / 2;
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else {
      TCAmpVar->Kn4 = TCAmpVar->Kn1 *
		      AnaExpo(TCAmpVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		      (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      TCAmpVar->Kp4 = TCAmpVar->Kn4 / 3;
      TCAmpVar->TwoKn4 = 2 * TCAmpVar->Kn4;
      TCAmpVar->TwoKp4 = 2 * TCAmpVar->Kp4;
      TCAmpVar->Kn5 = AnaExpo(-TCAmpVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Leakage:
    if ((unsigned)Inst->attr[N_Leakage - 1].UU.r > 1 ||
	Inst->attr[N_Leakage - 1].blnk)
      Result = false;
    else {
      TCAmpVar->Kn1 = Inst->attr[N_Leakage - 1].UU.r;
      TCAmpVar->Kp1 = TCAmpVar->Kn1 / 3;
      TCAmpVar->Kn4 = TCAmpVar->Kn1 *
		      AnaExpo(TCAmpVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		      (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      TCAmpVar->Kp4 = TCAmpVar->Kn4 / 3;
      TCAmpVar->TwoKn4 = 2 * TCAmpVar->Kn4;
      TCAmpVar->TwoKp4 = 2 * TCAmpVar->Kp4;
    }
    break;

  case N_Early:
    if (Inst->attr[N_Early - 1].UU.r <= 0 ||
	Inst->attr[N_Early - 1].UU.r > AnaVTooBig)
      Result = false;
    else {
      if (Inst->attr[N_Early - 1].blnk)
	TCAmpVar->InvVcross = 0.0;
      else
	TCAmpVar->InvVcross = 1 / Inst->attr[N_Early - 1].UU.r;
    }
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[N_Ktqn - 1].UU.r > 1 || Inst->attr[N_Ktqn - 1].blnk)
      Result = false;
    else {
      TCAmpVar->V0 = Inst->attr[N_Ktqn - 1].UU.r;
      TCAmpVar->Kn2 = 1 / TCAmpVar->V0;
      TCAmpVar->Kn4 = TCAmpVar->Kn1 *
		      AnaExpo(TCAmpVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		      (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      TCAmpVar->Kp4 = TCAmpVar->Kn4 / 3;
      TCAmpVar->TwoKn4 = 2 * TCAmpVar->Kn4;
      TCAmpVar->TwoKp4 = 2 * TCAmpVar->Kp4;
      TCAmpVar->Kn5 = AnaExpo(-TCAmpVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;
  }
  return Result;

  /*Recompute all internal variables that depend on it*/
  /*Recompute all internal variables that depend on it*/
}


Static Void Attr_TCAmp(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 11) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 && ((1L << Attrnum) &
	  ((1L << N_CapVset) | (1L << N_CapVout) | (1L << N_CapVdiff))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = TCAmpcheck(act->inst, Attrnum);
}


Static Void Newgate_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICTCAmp *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICTCAmp *)Cellptr->Ainfo;
  TCAmpVar = (TCAmpConst *)Malloc(sizeof(TCAmpConst));
  TCAmpVar->HalfOffset = ICptr->InitVoffset / 2;
  TCAmpVar->IV0 = ICptr->InitIKtqn;
  TCAmpVar->IKn2 = 1 / TCAmpVar->IV0;
  TCAmpVar->HalfIKn2 = TCAmpVar->IKn2 / 2;
  TCAmpVar->V0 = ICptr->InitKtqn;   /*kt/qn*/
  TCAmpVar->Kn1 = ICptr->InitLeakage;   /*nfet diode leakage current*1*/
  TCAmpVar->Kp1 = TCAmpVar->Kn1 / 3;   /*pfet diode leakage current*1*/
  TCAmpVar->Kn2 = 1 / TCAmpVar->V0;   /*qn/kt*/
  TCAmpVar->Kn4 = TCAmpVar->Kn1 * AnaExpo(TCAmpVar->Kn2 * ICptr->InitVt) /
		  (ICptr->InitVt * ICptr->InitVt);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  TCAmpVar->Kp4 = TCAmpVar->Kn4 / 3;   /*p equivalent*/
  TCAmpVar->TwoKn4 = 2 * TCAmpVar->Kn4;   /*2*Kn4*/
  TCAmpVar->TwoKp4 = 2 * TCAmpVar->Kp4;   /*2*Kp4*/
  TCAmpVar->Kn5 = AnaExpo(-TCAmpVar->Kn2 * ICptr->InitVt);
      /*Expo(-Kn2*Vtn)*/
  if (ICptr->InitEarly == 0)
    TCAmpVar->InvVcross = 0.0;
  else
    TCAmpVar->InvVcross = 1 / ICptr->InitEarly;
  TCAmpVar->Limit = false;
  TCAmpVar->OldLimit = false;
  TCAmpVar->NewLimit = false;
  /*Give internal variables values derived from initial attribute values*/
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)TCAmpVar;
  AnaCapInit(act->inst);   /*Initialize attributes of Cstart*/
  switch (AmpRange) {

  case Normal:
  case Wide:
    A_Gate->Pininfo[R_Vplus - 1].Cap = ICptr->InitCgbVplus;
    A_Gate->Pininfo[R_Vminus - 1].Cap = ICptr->InitCgbVminus;
    A_Gate->Pininfo[R_Vout - 1].Cap = ICptr->InitCdbVout;
    A_Gate->Pininfo[R_Vset - 1].Cap = ICptr->InitCgbVset;
    break;

  case Hwr:
  case Fwr:
    A_Gate->Pininfo[A_Vplus - 1].Cap = ICptr->InitCgbVplus;
    A_Gate->Pininfo[A_Vminus - 1].Cap = ICptr->InitCgbVminus;
    A_Gate->Pininfo[A_Vout - 1].Cap = ICptr->InitCdbVout;
    A_Gate->Pininfo[A_Vset - 1].Cap = ICptr->InitCgbVset;
    break;
  }
  WITH = act->inst;
  WITH->attr[N_Voffset - 1].UU.r = ICptr->InitVoffset;
  WITH->attr[N_Voffset - 1].blnk = false;
  WITH->attr[N_Voffset - 1].changed = true;
  WITH->attr[N_Ipos - 1].UU.r = ICptr->InitIpos;
  WITH->attr[N_Ipos - 1].blnk = false;
  WITH->attr[N_Ipos - 1].changed = true;
  WITH->attr[N_Ineg - 1].UU.r = ICptr->InitIneg;
  if (AmpRange == Hwr)
    WITH->attr[N_Ineg - 1].blnk = true;
  else
    WITH->attr[N_Ineg - 1].blnk = false;
  WITH->attr[N_Ineg - 1].changed = true;
  WITH->attr[N_IKtqn - 1].UU.r = ICptr->InitIKtqn;
  WITH->attr[N_IKtqn - 1].blnk = false;
  WITH->attr[N_IKtqn - 1].changed = true;
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
  WITH->attr[N_CapVset - 1].UU.r = ICptr->InitCapVset;
  WITH->attr[N_CapVset - 1].blnk = false;
  WITH->attr[N_CapVset - 1].changed = true;
  WITH->attr[N_CapVout - 1].UU.r = ICptr->InitCapVout;
  WITH->attr[N_CapVout - 1].blnk = false;
  WITH->attr[N_CapVout - 1].changed = true;
  WITH->attr[N_CapVdiff - 1].UU.r = ICptr->InitCapVdiff;
  WITH->attr[N_CapVdiff - 1].blnk = false;
  WITH->attr[N_CapVdiff - 1].changed = true;
}


Static Void Copygate_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar, *Old_TCAmpvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_TCAmpvar = (TCAmpConst *)A_Oldgate->InstVar;
  TCAmpVar = (TCAmpConst *)Malloc(sizeof(TCAmpConst));
  *TCAmpVar = *Old_TCAmpvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)TCAmpVar;
  AnaCapCopy(act->inst);
}


Static Void Dispose_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Free(TCAmpVar);
  AnaCapDispose(act->inst);
}


Static Void Readgate_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICTCAmp *ICptr;
  log_grec *WITH;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICTCAmp *)Cellptr->Ainfo;
    switch (AmpRange) {

    case Normal:
    case Wide:
      A_Gate->Pininfo[R_Vplus - 1].Cap = ICptr->InitCgbVplus;
      A_Gate->Pininfo[R_Vminus - 1].Cap = ICptr->InitCgbVminus;
      A_Gate->Pininfo[R_Vout - 1].Cap = ICptr->InitCdbVout;
      A_Gate->Pininfo[R_Vset - 1].Cap = ICptr->InitCgbVset;
      break;

    case Hwr:
    case Fwr:
      A_Gate->Pininfo[A_Vplus - 1].Cap = ICptr->InitCgbVplus;
      A_Gate->Pininfo[A_Vminus - 1].Cap = ICptr->InitCgbVminus;
      A_Gate->Pininfo[A_Vout - 1].Cap = ICptr->InitCdbVout;
      A_Gate->Pininfo[A_Vset - 1].Cap = ICptr->InitCgbVset;
      break;
    }
    WITH = act->inst;
    WITH->attr[N_Voffset - 1].UU.r = ICptr->InitVoffset;
    WITH->attr[N_Voffset - 1].blnk = false;
    WITH->attr[N_Voffset - 1].changed = true;
    WITH->attr[N_Ipos - 1].UU.r = ICptr->InitIpos;
    WITH->attr[N_Ipos - 1].blnk = false;
    WITH->attr[N_Ipos - 1].changed = true;
    WITH->attr[N_Ineg - 1].UU.r = ICptr->InitIneg;
    if (AmpRange == Hwr)
      WITH->attr[N_Ineg - 1].blnk = true;
    else
      WITH->attr[N_Ineg - 1].blnk = false;
    WITH->attr[N_Ineg - 1].changed = true;
    WITH->attr[N_IKtqn - 1].UU.r = ICptr->InitIKtqn;
    WITH->attr[N_IKtqn - 1].blnk = false;
    WITH->attr[N_IKtqn - 1].changed = true;
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
    WITH->attr[N_CapVset - 1].UU.r = ICptr->InitCapVset;
    WITH->attr[N_CapVset - 1].blnk = false;
    WITH->attr[N_CapVset - 1].changed = true;
    WITH->attr[N_CapVout - 1].UU.r = ICptr->InitCapVout;
    WITH->attr[N_CapVout - 1].blnk = false;
    WITH->attr[N_CapVout - 1].changed = true;
    WITH->attr[N_CapVdiff - 1].UU.r = ICptr->InitCapVdiff;
    WITH->attr[N_CapVdiff - 1].blnk = false;
    WITH->attr[N_CapVdiff - 1].changed = true;
  }
  WITH = act->inst;
  TCAmpVar->HalfOffset = WITH->attr[N_Voffset - 1].UU.r / 2;
  TCAmpVar->IV0 = WITH->attr[N_IKtqn - 1].UU.r;   /*kt/qn for tanh*/
  TCAmpVar->IKn2 = 1 / TCAmpVar->IV0;   /*qn/kt for tanh*/
  TCAmpVar->HalfIKn2 = TCAmpVar->IKn2 / 2;
  TCAmpVar->V0 = WITH->attr[N_Ktqn - 1].UU.r;   /*kt/qn*/
  TCAmpVar->Kn1 = WITH->attr[N_Leakage - 1].UU.r;
      /*n diode leakage current*(W/L)*/
  TCAmpVar->Kp1 = TCAmpVar->Kn1 / 3;   /*p diode leakage current*(W/L)*/
  TCAmpVar->Kn2 = 1 / TCAmpVar->V0;   /*qn/kt*/
  TCAmpVar->Kn4 = TCAmpVar->Kn1 *
		  AnaExpo(TCAmpVar->Kn2 * WITH->attr[N_Vt - 1].UU.r) /
		  (WITH->attr[N_Vt - 1].UU.r * WITH->attr[N_Vt - 1].UU.r);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  TCAmpVar->Kp4 = TCAmpVar->Kn4 / 3;   /*p equivalent*/
  TCAmpVar->TwoKn4 = 2 * TCAmpVar->Kn4;   /*2*Kn4*/
  TCAmpVar->TwoKp4 = 2 * TCAmpVar->Kp4;   /*2*Kp4*/
  TCAmpVar->Kn5 = AnaExpo(-TCAmpVar->Kn2 * WITH->attr[N_Vt - 1].UU.r);
      /*Expo(-Kn2*Vtn)*/
  if (WITH->attr[N_Early - 1].blnk)
    TCAmpVar->InvVcross = 0.0;
  else
    TCAmpVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
}


Static Void Reset_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  TCAmpVar->Limit = false;
  TCAmpVar->OldLimit = false;
  TCAmpVar->NewLimit = false;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, TCAmpVar->Limit, false);
}


Static Void PreUpdate_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  AnaExt_rec *PinPlusPtr;
  double VPinplus;
  AnaExt_rec *PinMinusPtr;
  double VPinMinus;
  AnaExt_rec *PinOutPtr;
  double VPinOut;
  AnaExt_rec *PinSetPtr;
  double VPinSet;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  switch (AmpRange) {

  case Normal:
  case Wide:
    PinPlusPtr = (AnaExt_rec *)WITH1->pin[R_Vplus - 1]->info;
    VPinplus = PinPlusPtr->last;
    PinMinusPtr = (AnaExt_rec *)WITH1->pin[R_Vminus - 1]->info;
    VPinMinus = PinMinusPtr->last;
    PinOutPtr = (AnaExt_rec *)WITH1->pin[R_Vout - 1]->info;
    VPinOut = PinOutPtr->last;
    PinSetPtr = (AnaExt_rec *)WITH1->pin[R_Vset - 1]->info;
    VPinSet = PinSetPtr->last;
    break;

  case Hwr:
  case Fwr:
    PinPlusPtr = (AnaExt_rec *)WITH1->pin[A_Vplus - 1]->info;
    VPinplus = PinPlusPtr->last;
    PinMinusPtr = (AnaExt_rec *)WITH1->pin[A_Vminus - 1]->info;
    VPinMinus = PinMinusPtr->last;
    PinOutPtr = (AnaExt_rec *)WITH1->pin[A_Vout - 1]->info;
    VPinOut = PinOutPtr->last;
    PinSetPtr = (AnaExt_rec *)WITH1->pin[A_Vset - 1]->info;
    VPinSet = PinSetPtr->last;
    break;
  }
  TCAmpVar->NewLimit = (VPinSet >= WITH1->attr[N_Vt - 1].UU.r ||
			VPinplus >= AnaVdd && VPinMinus >= AnaVdd);
}


Static Void Update_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  TCAmpVar->OldLimit = TCAmpVar->Limit;
  TCAmpVar->Limit = TCAmpVar->NewLimit;
}


Static Void Display_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (TCAmpVar->OldLimit != TCAmpVar->Limit)
      Limitfill(act->inst, TCAmpVar->Limit, false);
  }
}


Static Void Refresh_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Limitfill(act->inst, TCAmpVar->Limit, false);
}



Static Void Erase_TCAmp(act)
Analog_32_action *act;
{
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Limitfill(act->inst, TCAmpVar->Limit, true);
}



Static Void Probe_TCAmp(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr, *Node3Ptr, *Node4Ptr;
  double Node1, Node2, Node3, Node4, Pin1, Pin2, Pin3, Pin4;
  TCAmpConst *TCAmpVar;
  double ExpoVset, BlendVset, Iset;
  boolean Outhigh, Outlow;
  long IntAmp;
  log_grec *WITH;
  Char STR1[256];
  Char STR2[256];

  WITH = act->inst;
  IntAmp = AmpRange;
  A_Gate = (Anainstlist *)WITH->info;
  switch (IntAmp) {

  case Normal:
  case Wide:
    Pin1 = A_Gate->Pininfo[R_Vplus - 1].Vmem;
    Pin2 = A_Gate->Pininfo[R_Vminus - 1].Vmem;
    Pin3 = A_Gate->Pininfo[R_Vout - 1].Vmem;
    Pin4 = A_Gate->Pininfo[R_Vset - 1].Vmem;
    Node1Ptr = (AnaExt_rec *)WITH->pin[R_Vplus - 1]->info;
    Node2Ptr = (AnaExt_rec *)WITH->pin[R_Vminus - 1]->info;
    Node3Ptr = (AnaExt_rec *)WITH->pin[R_Vout - 1]->info;
    Node4Ptr = (AnaExt_rec *)WITH->pin[R_Vset - 1]->info;
    break;

  case Hwr:
  case Fwr:
    Pin1 = A_Gate->Pininfo[A_Vplus - 1].Vmem;
    Pin2 = A_Gate->Pininfo[A_Vminus - 1].Vmem;
    Pin3 = A_Gate->Pininfo[A_Vout - 1].Vmem;
    Pin4 = A_Gate->Pininfo[A_Vset - 1].Vmem;
    Node1Ptr = (AnaExt_rec *)WITH->pin[A_Vplus - 1]->info;
    Node2Ptr = (AnaExt_rec *)WITH->pin[A_Vminus - 1]->info;
    Node3Ptr = (AnaExt_rec *)WITH->pin[A_Vout - 1]->info;
    Node4Ptr = (AnaExt_rec *)WITH->pin[A_Vset - 1]->info;
    break;
  }
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  Node3 = Node3Ptr->ltimestep;
  Node4 = Node4Ptr->ltimestep;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  switch (IntAmp) {

  case Normal:
    AnaScoreboard(NormalName, (long)AnaMessGate1);
    break;

  case Wide:
    AnaScoreboard(WideName, (long)AnaMessGate1);
    break;

  case Hwr:
    AnaScoreboard(HwrName, (long)AnaMessGate1);
    break;

  case Fwr:
    AnaScoreboard(FwrName, (long)AnaMessGate1);
    break;
  }
  if (Node1 != Pin1 || Node2 != Pin2 || Node1 == AnaNotyet ||
      Node2 == AnaNotyet || Node3 != Pin3 || Node4 != Pin4 ||
      Node3 == AnaNotyet || Node4 == AnaNotyet) {
    AnaScoreboard(" <undefined>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    return;
  }
  if (Pin4 > WITH->attr[N_Vt - 1].UU.r)
    AnaScoreboard("Iset out of subthreshold", (long)AnaMessGate2);
  else {
    if (Pin1 < Pin4 && Pin2 < Pin4)
      AnaScoreboard("Iset limited by V+,V-", (long)AnaMessGate2);
    else {
      ExpoVset = AnaExpo(TCAmpVar->Kn2 * Pin4);
      BlendVset = 1 / (1 + TCAmpVar->Kn5 * ExpoVset);
      Iset = TCAmpVar->Kn1 * ExpoVset * BlendVset +
	     TCAmpVar->Kn4 * Pin4 * Pin4 * (1 - BlendVset);
      sprintf(STR2, "Iset = %s", AnaProbeAmpStr(STR1, Iset));
      AnaScoreboard(STR2, (long)AnaMessGate2);
    }
  }
  if (fabs(Pin1 - Pin2) < 2 * WITH->attr[N_Ktqn - 1].UU.r)
    AnaScoreboard("Tanh linear", (long)AnaMessGate3);
  else
    AnaScoreboard("Tanh saturated", (long)AnaMessGate3);
  switch (IntAmp) {

  case Normal:
    if (Pin1 < Pin2) {
      Outhigh = (Pin3 >= AnaVdd + Pin4);
      Outlow = (Pin3 <= Pin1 - Pin4);
    } else {
      Outhigh = (Pin3 >= AnaVdd);
      Outlow = (Pin3 <= Pin2 - Pin4);
    }
    break;

  case Wide:
    if (Pin1 < Pin2) {
      Outhigh = (Pin3 >= AnaVdd + Pin4);
      Outlow = (Pin3 < 0);
    } else {
      Outhigh = (Pin3 >= AnaVdd);
      Outlow = (Pin3 + Pin4 < 0);
    }
    break;

  case Hwr:
    if (Pin1 > Pin2)
      Outhigh = (Pin3 >= AnaVdd + Pin4);
    else
      Outhigh = (Pin3 >= AnaVdd);
    Outlow = false;
    break;

  case Fwr:
    Outhigh = (Pin3 >= AnaVdd);
    Outlow = false;
    break;
  }
  if (Outhigh) {
    AnaScoreboard("Vout too high", (long)AnaMessGate4);
    return;
  }
  if (Outlow)
    AnaScoreboard("Vout too low", (long)AnaMessGate4);
  else
    AnaScoreboard("Vout ok", (long)AnaMessGate4);
}


Local Void Ex_TCAmp(act)
Analog_32_action *act;
{
  double MinI1, dI1dV1, dI1dV2, dI1dV3, dI1dV4, MinI2, dI2dV1, dI2dV2, dI2dV3,
	 dI2dV4, MinI3, dI3dV1, dI3dV2, dI3dV3, dI3dV4, MinI4, dI4dV1, dI4dV2,
	 dI4dV3, dI4dV4;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2, VMem2;
  AnaExt_rec *Pin3Ptr;
  long Pin3;
  double VPin3, VMem3;
  AnaExt_rec *Pin4Ptr;
  long Pin4;
  double VPin4, VMem4;
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  double Icap, Del, ExpoDel, BlendDel, InvBlendDel, dBlendDeldV2, V1Offset,
	 V2Offset, MaxV1V2, dMaxV1V2dV2, dMaxV1V2dV1, MinV1V2, dMinV1V2dV1,
	 dMinV1V2dV2, Vcmp, ExpoVcmp, BlendVcmp, InvBlendVcmp,
	 dBlendVcmpdVset, dBlendVcmpdV2, dBlendVcmpdV1, Vstr, dVstrdV1,
	 dVstrdV2, dVstrdVset, ExpoVstr, Kn5_ExpoVstr, BlendVstr,
	 InvBlendVstr, dBlendVstr_Temp, dBlendVstrdV1, dBlendVstrdV2,
	 dBlendVstrdVset, Iset_Temp1, Iset_Temp2, Iset, dIset_Temp1,
	 dIset_Temp2, dIsetdV1, dIsetdV2, dIsetdVset, TCtanch, dTCTanchdV1,
	 dTCTanchdV2, ExpoVcmpVdd, BlendVcmpVdd, InvBlendVcmpVdd,
	 dBlendVcmpVddTemp, dBlendVcmpVdddV1, dBlendVcmpVdddV2, MinV1V2Vdd,
	 dMinV1V2VdddV1, dMinV1V2VdddV2, VcmpVdd, Vbot, dVbotdV1, dVbotdV2,
	 dVbotdVout, ExpoVbot, BlendVbotTemp, BlendVbot, InvBlendVbot,
	 dBlendVbotTemp, dBlendVbotdV1, dBlendVbotdV2, dBlendVbotdVout,
	 IbotTemp1, IbotTemp2, IbotTemp3, Ibot, dIbotTemp1, dIbotTemp2,
	 dIbotdV1, dIbotdV2, dIbotdVout, dIbotdVset, Vhighcmp, ExpoVhighcmp,
	 BlendVhighcmp, InvBlendVhighcmp, dBlendVhighcmpdV2, MaxVddV2,
	 dMaxVddV2dV2, Vtop1, ExpoVtop1, BlendVtop1Temp, BlendVtop1,
	 InvBlendVtop1, Itop1, dBlendVtop1dVout, dItop1Temp1, dItop1Temp2,
	 dItop1Temp3, dItop1dVout, Vtop2, ExpVtop2, BlendVtop2Temp,
	 BlendVtop2, InvBlendVtop2, dBlendVtop2Temp1, dBlendVtop2dVout,
	 dBlendVtop2dVset, dBlendVtop2dV2, Itop2, dItop2Temp1, dItop2Temp2,
	 dItop2dVout, dItop2dVset, dItop2dV2, Ihigh, dIhighTemp, dIhighdV1,
	 dIhighdV2, dIhighdVout, dIhighdVset;
/* p2c: stdlib.text, line 4982: 
 * Note: Line breaker spent 2.0 seconds, 5000 tries on line 1035 [251] */
  Deltasign Sign;
  double ResPositive, ResNegative, dResPositivedVout, dResNegativedVout,
	 IresTempPos, IresTempNeg, Ires, dIresTemp, dIresdV1, dIresdV2,
	 dIresdVout, TCRes, dTCTemp, dTCResdV1, dTCResdV2, dTCResdVout, Vlow1,
	 ExpoVlow1, ExpoVlow1Temp, BlendVlow1, InvBlendVlow1,
	 dBlendVlow1dVout, dBlendVlow1dVset, Ilow1Temp1, Ilow1Temp2,
	 Ilow1Temp3, Ilow1, dIlow1dVset, dIlow1dVout, Vlow2, ExpoVlow2,
	 ExpoVlow2Temp, BlendVlow2, InvBlendVlow2, dBlendVlow2dVout,
	 Ilow2Temp1, Ilow2Temp2, Ilow2Temp3, Ilow2, dIlow2dVout, Delta1,
	 Delta2, Delta3, Delta4, Itop2Temp1, Itop2Temp2, Itop2Temp3;
/* p2c: stdlib.text, line 4982: 
 * Note: Line breaker spent 2.0 seconds, 5000 tries on line 1047 [251] */
  log_grec *WITH;
  AnaMatRec *WITH1;



  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[R_Vplus - 1]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  VMem1 = A_Gate->Pininfo[R_Vplus - 1].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[R_Vminus - 1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;
  VMem2 = A_Gate->Pininfo[R_Vminus - 1].Vmem;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[R_Vout - 1]->info;
  Pin3 = Pin3Ptr->nodenum;
  VPin3 = Pin3Ptr->last;
  VMem3 = A_Gate->Pininfo[R_Vout - 1].Vmem;
  Pin4Ptr = (AnaExt_rec *)WITH->pin[R_Vset - 1]->info;
  Pin4 = Pin4Ptr->nodenum;
  VPin4 = Pin4Ptr->last;
  VMem4 = A_Gate->Pininfo[R_Vset - 1].Vmem;
      /*Use with statement to get to internal variables*/

  /*Implementation notes*/
  /*1.attributes may be referred to directly by attr^[]. syntax*/
  /*2.use Dt, Accum+Dt for timevalue*/
  /*Numbers used below are in 'virtual pin numbers' */
  /*Pin1=Vplus*/
  /*Pin3=Vout*/
  /*Pin2=Vminus*/
  /*Pin4=Vset*/


  V1Offset = VPin1 + TCAmpVar->HalfOffset;
  V2Offset = VPin2 - TCAmpVar->HalfOffset;
  Del = V1Offset - V2Offset;
  if (fabs(Del) < 0.000708) {
    ExpoDel = AnaExpo(100000L * Del);
    BlendDel = 1 / (1 + ExpoDel);
    InvBlendDel = 1 - BlendDel;
    dBlendDeldV2 = 100000L * ExpoDel * BlendDel * BlendDel;
    MaxV1V2 = V2Offset * BlendDel + V1Offset * InvBlendDel;
    dMaxV1V2dV2 = BlendDel - Del * dBlendDeldV2;
    dMaxV1V2dV1 = 1 - dMaxV1V2dV2;
    MinV1V2 = V1Offset * BlendDel + V2Offset * InvBlendDel;
    dMinV1V2dV1 = dMaxV1V2dV2;
    dMinV1V2dV2 = dMaxV1V2dV1;
    Sign = Zero;
  } else {
    if (Del > 0) {
      BlendDel = 0.0;
      InvBlendDel = 1.0;
      dBlendDeldV2 = 0.0;
      MaxV1V2 = V1Offset;
      dMaxV1V2dV2 = 0.0;
      dMaxV1V2dV1 = 1.0;
      MinV1V2 = V2Offset;
      dMinV1V2dV1 = 0.0;
      dMinV1V2dV2 = 1.0;
      Sign = Positive;
    } else {
      BlendDel = 1.0;
      InvBlendDel = 0.0;
      dBlendDeldV2 = 0.0;
      MaxV1V2 = V2Offset;
      dMaxV1V2dV2 = 1.0;
      dMaxV1V2dV1 = 0.0;
      MinV1V2 = V1Offset;
      dMinV1V2dV1 = 1.0;
      dMinV1V2dV2 = 0.0;
      Sign = Negative;
    }
  }

  Vcmp = MaxV1V2 - VPin4;
  if (fabs(Vcmp) < 0.000708) {
    ExpoVcmp = AnaExpo(100000L * Vcmp);   /*Could be optimized via flags*/
    BlendVcmp = 1 / (1 + ExpoVcmp);
    InvBlendVcmp = 1 - BlendVcmp;
    dBlendVcmpdVset = 100000L * ExpoVcmp * BlendVcmp * BlendVcmp;
    dBlendVcmpdV2 = -dBlendVcmpdVset * dMaxV1V2dV2;
    dBlendVcmpdV1 = -dBlendVcmpdVset - dBlendVcmpdV2;
    Vstr = MaxV1V2 * BlendVcmp + VPin4 * InvBlendVcmp;
    dVstrdV1 = dMaxV1V2dV1 * BlendVcmp + Vcmp * dBlendVcmpdV1;
    dVstrdV2 = dMaxV1V2dV2 * BlendVcmp + Vcmp * dBlendVcmpdV2;
    dVstrdVset = InvBlendVcmp + Vcmp * dBlendVcmpdVset;
  } else {
    if (Vcmp > 0) {
      /*BlendVcmp:=0;*/
      /*InvBlendVcmp:=1;*/
      /*dBlendVcmpdVset:=0;*/
      /*dBlendVcmpdV2:=0;*/
      /*dBlendVcmpdV1:=0;*/
      Vstr = VPin4;
      dVstrdV1 = 0.0;
      dVstrdV2 = 0.0;
      dVstrdVset = 1.0;
    } else {
      /*BlendVcmp:=1;*/
      /*InvBlendVcmp:=0;*/
      /*dBlendVcmpdVset:=0;*/
      /*dBlendVcmpdV2:=0;*/
      /*dBlendVcmpdV1:=0;*/
      Vstr = MaxV1V2;
      dVstrdV1 = dMaxV1V2dV1;
      dVstrdV2 = dMaxV1V2dV2;
      dVstrdVset = 0.0;
    }
  }


  ExpoVstr = AnaExpo(TCAmpVar->Kn2 * Vstr);
  Kn5_ExpoVstr = TCAmpVar->Kn5 * ExpoVstr;
  BlendVstr = 1 / (1 + Kn5_ExpoVstr);
  InvBlendVstr = 1 - BlendVstr;
  dBlendVstr_Temp = -TCAmpVar->Kn2 * Kn5_ExpoVstr * BlendVstr * BlendVstr;
  dBlendVstrdV1 = dBlendVstr_Temp * dVstrdV1;
  dBlendVstrdV2 = dBlendVstr_Temp * dVstrdV2;
  dBlendVstrdVset = dBlendVstr_Temp * dVstrdVset;
  /*^Could be optimized via flags*/

  Iset_Temp1 = TCAmpVar->Kn1 * ExpoVstr;
  Iset_Temp2 = TCAmpVar->Kn4 * Vstr * Vstr;
  Iset = Iset_Temp1 * BlendVstr + Iset_Temp2 * InvBlendVstr;
  dIset_Temp1 = Iset_Temp1 - Iset_Temp2;
  dIset_Temp2 = Iset_Temp1 * TCAmpVar->Kn2 * BlendVstr +
		TCAmpVar->TwoKn4 * InvBlendVstr * Vstr;
  dIsetdV1 = dIset_Temp1 * dBlendVstrdV1 + dIset_Temp2 * dVstrdV1;
  dIsetdV2 = dIset_Temp1 * dBlendVstrdV2 + dIset_Temp2 * dVstrdV2;
  dIsetdVset = dIset_Temp1 * dBlendVstrdVset + dIset_Temp2 * dVstrdVset;
  /*^Could be optimized via flags*/


  TCtanch = AnaTanch(Del * TCAmpVar->HalfIKn2);
  dTCTanchdV1 = (1 - TCtanch * TCtanch) * TCAmpVar->HalfIKn2;
  dTCTanchdV2 = -dTCTanchdV1;


  if (AmpRange == Normal) {
    VcmpVdd = AnaVdd - MinV1V2;
    if (fabs(VcmpVdd) < 0.000708) {
      ExpoVcmpVdd = AnaExpo(100000L * VcmpVdd);
      BlendVcmpVdd = 1 / (1 + ExpoVcmpVdd);
      InvBlendVcmpVdd = 1 - BlendVcmpVdd;
      dBlendVcmpVddTemp = 100000L * BlendVcmpVdd * BlendVcmpVdd * ExpoVcmpVdd;
      dBlendVcmpVdddV1 = dBlendVcmpVddTemp * dMinV1V2dV1;
      dBlendVcmpVdddV2 = dBlendVcmpVddTemp * dMinV1V2dV2;
      MinV1V2Vdd = AnaVdd * BlendVcmpVdd + MinV1V2 * InvBlendVcmpVdd;
      dMinV1V2VdddV1 = VcmpVdd * dBlendVcmpVdddV1 + dMinV1V2dV1 * InvBlendVcmpVdd;
      dMinV1V2VdddV2 = VcmpVdd * dBlendVcmpVdddV2 + dMinV1V2dV2 * InvBlendVcmpVdd;
    } else {
      if (VcmpVdd > 0) {
	/*BlendVcmpVdd:=0;*/
	/*InvBlendVcmpVdd:=1;*/
	/*dBlendVcmpVddTemp:=0;*/
	/*dBlendVcmpVdddV1:=0;*/
	/*dBlendVcmpVdddV2:=0;*/
	MinV1V2Vdd = MinV1V2;
	dMinV1V2VdddV1 = dMinV1V2dV1;
	dMinV1V2VdddV2 = dMinV1V2dV2;
      } else {
	/*BlendVcmpVdd:=1;*/
	/*InvBlendVcmpVdd:=0;*/
	/*dBlendVcmpVddTemp:=0;*/
	/*dBlendVcmpVdddV1:=0;*/
	/*dBlendVcmpVdddV2:=0;*/
	MinV1V2Vdd = AnaVdd;
	dMinV1V2VdddV1 = 0.0;
	dMinV1V2VdddV2 = 0.0;
      }
    }

    Vbot = MinV1V2Vdd - VPin3;
    dVbotdV1 = dMinV1V2VdddV1;
    dVbotdV2 = dMinV1V2VdddV2;
    dVbotdVout = -1.0;


    ExpoVbot = AnaExpo(TCAmpVar->Kn2 * Vbot);
    BlendVbotTemp = TCAmpVar->Kn5 * ExpoVbot;
    BlendVbot = 1 / (1 + BlendVbotTemp);
    InvBlendVbot = 1 - BlendVbot;

    dBlendVbotTemp = TCAmpVar->Kn2 * BlendVbot * BlendVbot * BlendVbotTemp;
    dBlendVbotdV1 = -dBlendVbotTemp * dVbotdV1;
    dBlendVbotdV2 = -dBlendVbotTemp * dVbotdV2;
    dBlendVbotdVout = dBlendVbotTemp;

    IbotTemp1 = TCAmpVar->Kn1 * ExpoVbot;
    IbotTemp2 = IbotTemp1 * BlendVbot;
    IbotTemp3 = TCAmpVar->Kn4 * Vbot * Vbot;
    Ibot = IbotTemp2 + InvBlendVbot * IbotTemp3;

    dIbotTemp1 = TCAmpVar->Kn2 * IbotTemp2 + TCAmpVar->TwoKn4 * Vbot * InvBlendVbot;
    dIbotTemp2 = IbotTemp1 - IbotTemp3;
    dIbotdV1 = dIbotTemp1 * dVbotdV1 + dIbotTemp2 * dBlendVbotdV1;
    dIbotdV2 = dIbotTemp1 * dVbotdV2 + dIbotTemp2 * dBlendVbotdV2;
    dIbotdVout = dIbotTemp1 * dVbotdVout + dIbotTemp2 * dBlendVbotdVout;
    dIbotdVset = 0.0;
  } else {
    Vlow1 = VPin4 - VPin3;
    ExpoVlow1 = AnaExpo(TCAmpVar->Kn2 * Vlow1);
    ExpoVlow1Temp = TCAmpVar->Kn5 * ExpoVlow1;
    BlendVlow1 = 1 / (1 + ExpoVlow1Temp);
    InvBlendVlow1 = 1 - BlendVlow1;
    dBlendVlow1dVout = TCAmpVar->Kn2 * ExpoVlow1Temp * BlendVlow1 * BlendVlow1;
    dBlendVlow1dVset = -dBlendVlow1dVout;


    Ilow1Temp1 = TCAmpVar->Kn1 * ExpoVlow1;
    Ilow1Temp2 = TCAmpVar->Kn4 * Vlow1 * Vlow1;
    Ilow1Temp3 = Ilow1Temp1 * BlendVlow1;
    Ilow1 = Ilow1Temp3 + InvBlendVlow1 * Ilow1Temp2;
    dIlow1dVset = TCAmpVar->Kn2 * Ilow1Temp3 +
		  (Ilow1Temp2 - Ilow1Temp1) * dBlendVlow1dVout +
		  TCAmpVar->TwoKn4 * Vlow1 * InvBlendVlow1;
    dIlow1dVout = -dIlow1dVset;

    Vlow2 = -VPin3;
    ExpoVlow2 = AnaExpo(TCAmpVar->Kn2 * Vlow2);
    ExpoVlow2Temp = TCAmpVar->Kn5 * ExpoVlow2;
    BlendVlow2 = 1 / (1 + ExpoVlow2Temp);
    InvBlendVlow2 = 1 - BlendVlow2;
    dBlendVlow2dVout = TCAmpVar->Kn2 * ExpoVlow2Temp * BlendVlow2 * BlendVlow2;

    Ilow2Temp1 = TCAmpVar->Kn1 * ExpoVlow2;
    Ilow2Temp2 = Ilow2Temp1 * BlendVlow2;
    Ilow2Temp3 = TCAmpVar->Kn4 * Vlow2 * Vlow2;
    Ilow2 = Ilow2Temp2 + InvBlendVlow2 * Ilow2Temp3;
    dIlow2dVout = (Ilow2Temp1 - Ilow2Temp3) * dBlendVlow2dVout -
	Ilow2Temp2 * TCAmpVar->Kn2 - InvBlendVlow2 * TCAmpVar->TwoKn4 * Vlow2;

    Ibot = BlendDel * Ilow1 + InvBlendDel * Ilow2;
    dIbotTemp1 = BlendDel * dIlow1dVset;
    dIbotTemp2 = dBlendDeldV2 * (Ilow1 - Ilow2);
    dIbotdV1 = -dIbotTemp2;
    dIbotdV2 = dIbotTemp2;
    dIbotdVout = InvBlendDel * dIlow2dVout - dIbotTemp1;
    dIbotdVset = dIbotTemp1;
  }



  if (AmpRange == Normal) {
    Vhighcmp = AnaVdd - VPin2;
    if (fabs(Vhighcmp) < 0.000708) {
      ExpoVhighcmp = AnaExpo(100000L * Vhighcmp);
      BlendVhighcmp = 1 / (1 + ExpoVhighcmp);
      InvBlendVhighcmp = 1 - BlendVhighcmp;
      dBlendVhighcmpdV2 = BlendVhighcmp * BlendVhighcmp * ExpoVhighcmp * 100000L;
      MaxVddV2 = VPin2 * BlendVhighcmp + AnaVdd * InvBlendVhighcmp;
      dMaxVddV2dV2 = BlendVhighcmp - Vhighcmp * dBlendVhighcmpdV2;
    } else {
      if (Vhighcmp > 0) {
	/*BlendVhighcmp:=0;*/
	/*InvBlendVhighcmp:=1;*/
	/*dBlendVhighcmpdV2:=0;*/
	MaxVddV2 = AnaVdd;
	dMaxVddV2dV2 = 0.0;
      } else {
	/*BlendVhighcmp:=1;*/
	/*InvBlendVhighcmp:=0;*/
	/*dBlendVhighcmpdV2:=0;*/
	MaxVddV2 = VPin2;
	dMaxVddV2dV2 = 1.0;
      }
    }
  } else {
    /*BlendVhighcmp:=0;*/
    /*InvBlendVhighcmp:=1;*/
    /*dBlendVhighcmpdV2:=0;*/
    MaxVddV2 = AnaVdd;
    dMaxVddV2dV2 = 0.0;
  }



  if (Sign == Zero) {
    Vtop1 = VPin3 - AnaVdd;
    ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
    BlendVtop1Temp = TCAmpVar->Kn5 * ExpoVtop1;
    BlendVtop1 = 1 / (1 + BlendVtop1Temp);
    InvBlendVtop1 = 1 - BlendVtop1;
    dBlendVtop1dVout = -TCAmpVar->Kn2 * BlendVtop1 * BlendVtop1 * BlendVtop1Temp;

    dItop1Temp1 = TCAmpVar->Kp1 * ExpoVtop1;
    dItop1Temp2 = TCAmpVar->Kp4 * Vtop1 * Vtop1;
    dItop1Temp3 = dItop1Temp1 * BlendVtop1;
    Itop1 = dItop1Temp3 + InvBlendVtop1 * dItop1Temp2;
    dItop1dVout = TCAmpVar->Kn2 * dItop1Temp3 +
		  (dItop1Temp1 - dItop1Temp2) * dBlendVtop1dVout +
		  TCAmpVar->TwoKp4 * InvBlendVtop1 * Vtop1;

    Vtop2 = VPin3 + VPin4 - MaxVddV2;
    ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
    BlendVtop2Temp = TCAmpVar->Kn5 * ExpVtop2;
    BlendVtop2 = 1 / (1 + BlendVtop2Temp);
    InvBlendVtop2 = 1 - BlendVtop2;
    dBlendVtop2Temp1 = -TCAmpVar->Kn2 * BlendVtop2 * BlendVtop2Temp * BlendVtop2;
    dBlendVtop2dVout = dBlendVtop2Temp1;
    dBlendVtop2dVset = dBlendVtop2Temp1;
    dBlendVtop2dV2 = -dBlendVtop2Temp1 * dMaxVddV2dV2;

    Itop2Temp1 = TCAmpVar->Kp1 * ExpVtop2;
    Itop2Temp2 = TCAmpVar->Kp4 * Vtop2 * Vtop2;
    Itop2Temp3 = Itop2Temp1 * BlendVtop2;
    Itop2 = Itop2Temp3 + InvBlendVtop2 * Itop2Temp2;
    dItop2Temp1 = TCAmpVar->Kn2 * Itop2Temp3 +
		  TCAmpVar->TwoKp4 * InvBlendVtop2 * Vtop2;
    dItop2Temp2 = Itop2Temp1 - Itop2Temp2;
    dItop2dVout = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVout;
    dItop2dVset = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVset;
    dItop2dV2 = dItop2Temp2 * dBlendVtop2dV2 - dItop2Temp1 * dMaxVddV2dV2;

    Ihigh = -(BlendDel * Itop1 + InvBlendDel * Itop2);
    dIhighTemp = Itop1 - Itop2;
    dIhighdV1 = dBlendDeldV2 * dIhighTemp;
    dIhighdV2 = -(dBlendDeldV2 * dIhighTemp + InvBlendDel * dItop2dV2);
    dIhighdVout = -(BlendDel * dItop1dVout + InvBlendDel * dItop2dVout);
    dIhighdVset = -InvBlendDel * dItop2dVset;
  } else {
    if (Sign == Positive) {
      Vtop2 = VPin3 + VPin4 - MaxVddV2;
      ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
      BlendVtop2Temp = TCAmpVar->Kn5 * ExpVtop2;
      BlendVtop2 = 1 / (1 + BlendVtop2Temp);
      InvBlendVtop2 = 1 - BlendVtop2;
      dBlendVtop2Temp1 = -TCAmpVar->Kn2 * BlendVtop2 * BlendVtop2Temp * BlendVtop2;
      dBlendVtop2dVout = dBlendVtop2Temp1;
      dBlendVtop2dVset = dBlendVtop2Temp1;
      dBlendVtop2dV2 = -dBlendVtop2Temp1 * dMaxVddV2dV2;

      Itop2Temp1 = TCAmpVar->Kp1 * ExpVtop2;
      Itop2Temp2 = TCAmpVar->Kp4 * Vtop2 * Vtop2;
      Itop2Temp3 = Itop2Temp1 * BlendVtop2;
      Itop2 = Itop2Temp3 + InvBlendVtop2 * Itop2Temp2;
      dItop2Temp1 = TCAmpVar->Kn2 * Itop2Temp3 +
		    TCAmpVar->TwoKp4 * InvBlendVtop2 * Vtop2;
      dItop2Temp2 = Itop2Temp1 - Itop2Temp2;
      dItop2dVout = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVout;
      dItop2dVset = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVset;
      dItop2dV2 = dItop2Temp2 * dBlendVtop2dV2 - dItop2Temp1 * dMaxVddV2dV2;

      Ihigh = -Itop2;
      dIhighdV1 = 0.0;
      dIhighdV2 = -dItop2dV2;
      dIhighdVout = -dItop2dVout;
      dIhighdVset = -dItop2dVset;
    } else {
      Vtop1 = VPin3 - AnaVdd;
      ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
      BlendVtop1Temp = TCAmpVar->Kn5 * ExpoVtop1;
      BlendVtop1 = 1 / (1 + BlendVtop1Temp);
      InvBlendVtop1 = 1 - BlendVtop1;
      dBlendVtop1dVout = -TCAmpVar->Kn2 * BlendVtop1 * BlendVtop1 * BlendVtop1Temp;

      dItop1Temp1 = TCAmpVar->Kp1 * ExpoVtop1;
      dItop1Temp2 = TCAmpVar->Kp4 * Vtop1 * Vtop1;
      dItop1Temp3 = dItop1Temp1 * BlendVtop1;
      Itop1 = dItop1Temp3 + InvBlendVtop1 * dItop1Temp2;
      dItop1dVout = TCAmpVar->Kn2 * dItop1Temp3 +
		    (dItop1Temp1 - dItop1Temp2) * dBlendVtop1dVout +
		    TCAmpVar->TwoKp4 * InvBlendVtop1 * Vtop1;
      Ihigh = -Itop1;
      dIhighdV1 = 0.0;
      dIhighdV2 = 0.0;
      dIhighdVout = -dItop1dVout;
      dIhighdVset = 0.0;
    }
  }


  ResPositive = 1 + (AnaVdd - VPin3) * TCAmpVar->InvVcross;
  ResNegative = 1 + VPin3 * TCAmpVar->InvVcross;
  dResPositivedVout = -TCAmpVar->InvVcross;
  dResNegativedVout = TCAmpVar->InvVcross;

  IresTempPos = ResPositive * WITH->attr[N_Ipos - 1].UU.r;
  IresTempNeg = ResNegative * WITH->attr[N_Ineg - 1].UU.r;
  Ires = BlendDel * IresTempNeg + InvBlendDel * IresTempPos;

  dIresTemp = IresTempPos - IresTempNeg;
  dIresdV1 = dBlendDeldV2 * dIresTemp;
  dIresdV2 = -dIresdV1;
  dIresdVout = BlendDel * TCAmpVar->InvVcross * WITH->attr[N_Ineg - 1].UU.r -
      InvBlendDel * TCAmpVar->InvVcross * WITH->attr[N_Ipos - 1].UU.r;



  TCRes = Ires * TCtanch;
  dTCTemp = Ires * dTCTanchdV1;
  dTCResdV1 = dIresdV1 * TCtanch + dTCTemp;
  dTCResdV2 = dIresdV2 * TCtanch - dTCTemp;
  dTCResdVout = dIresdVout * TCtanch;


  MinI3 = Iset * TCRes + Ibot + Ihigh;
  dI3dV1 = -(Iset * dTCResdV1 + TCRes * dIsetdV1 + dIbotdV1 + dIhighdV1);
  dI3dV2 = -(Iset * dTCResdV2 + TCRes * dIsetdV2 + dIbotdV2 + dIhighdV2);
  dI3dV3 = -(Iset * dTCResdVout + dIbotdVout + dIhighdVout);
  dI3dV4 = -(TCRes * dIsetdVset + dIbotdVset + dIhighdVset);


  Delta1 = VPin1 - VMem1;
  Delta2 = VPin2 - VMem2;
  Delta3 = VPin3 - VMem3;
  Delta4 = VPin4 - VMem4;

  if (AnaChangedDt)
    TCAmpVar->TauVset = WITH->attr[N_CapVset - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta1);   /*CapVset - Vplus*/
  MinI4 = -Icap;
  dI4dV1 = -TCAmpVar->TauVset;
  dI4dV4 = TCAmpVar->TauVset;
  MinI1 = Icap;
  dI1dV1 = TCAmpVar->TauVset;
  dI1dV4 = -TCAmpVar->TauVset;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta2);   /*CapVset - Vminus*/
  MinI4 -= Icap;
  dI4dV2 = -TCAmpVar->TauVset;
  dI4dV4 += TCAmpVar->TauVset;
  MinI2 = Icap;
  dI2dV2 = TCAmpVar->TauVset;
  dI2dV4 = -TCAmpVar->TauVset;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta3);   /*CapVset - Vout*/
  MinI4 -= Icap;
  dI4dV3 = -TCAmpVar->TauVset;
  dI4dV4 += TCAmpVar->TauVset;
  MinI3 += Icap;
  dI3dV3 += TCAmpVar->TauVset;
  dI3dV4 -= TCAmpVar->TauVset;

  if (AnaChangedDt)
    TCAmpVar->TauVout = WITH->attr[N_CapVout - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVout * (Delta3 - Delta2);   /*CapVout Minus*/
  MinI3 -= Icap;
  dI3dV2 -= TCAmpVar->TauVout;
  dI3dV3 += TCAmpVar->TauVout;
  MinI2 += Icap;
  dI2dV2 += TCAmpVar->TauVout;
  dI2dV3 = -TCAmpVar->TauVout;
  Icap = TCAmpVar->TauVout * (Delta3 - Delta1);   /*CapVout Vplus*/
  MinI3 -= Icap;
  dI3dV1 -= TCAmpVar->TauVout;
  dI3dV3 += TCAmpVar->TauVout;
  MinI1 += Icap;
  dI1dV1 += TCAmpVar->TauVout;
  dI1dV3 = -TCAmpVar->TauVout;

  if (AnaChangedDt)
    TCAmpVar->TauVdiff = WITH->attr[N_CapVdiff - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVdiff * (Delta2 - Delta1);   /*CapVdiff*/
  MinI2 -= Icap;
  dI2dV1 = -TCAmpVar->TauVdiff;
  dI2dV2 += TCAmpVar->TauVdiff;
  MinI1 += Icap;
  dI1dV1 += TCAmpVar->TauVdiff;
  dI1dV2 = -TCAmpVar->TauVdiff;
  WITH1 = AnaSystem;
  WITH1->Mat[Pin1][AnaSysCol] += MinI1;
  WITH1->Mat[Pin1][Pin1] += dI1dV1;
  WITH1->Mat[Pin1][Pin2] += dI1dV2;
  WITH1->Mat[Pin1][Pin3] += dI1dV3;
  WITH1->Mat[Pin1][Pin4] += dI1dV4;
  WITH1->Mat[Pin2][AnaSysCol] += MinI2;
  WITH1->Mat[Pin2][Pin1] += dI2dV1;
  WITH1->Mat[Pin2][Pin2] += dI2dV2;
  WITH1->Mat[Pin2][Pin3] += dI2dV3;
  WITH1->Mat[Pin2][Pin4] += dI2dV4;
  WITH1->Mat[Pin3][AnaSysCol] += MinI3;
  WITH1->Mat[Pin3][Pin1] += dI3dV1;
  WITH1->Mat[Pin3][Pin2] += dI3dV2;
  WITH1->Mat[Pin3][Pin3] += dI3dV3;
  WITH1->Mat[Pin3][Pin4] += dI3dV4;
  WITH1->Mat[Pin4][AnaSysCol] += MinI4;
  WITH1->Mat[Pin4][Pin1] += dI4dV1;
  WITH1->Mat[Pin4][Pin2] += dI4dV2;
  WITH1->Mat[Pin4][Pin3] += dI4dV3;
  WITH1->Mat[Pin4][Pin4] += dI4dV4;

  WITH1->MatNZ[Pin1][AnaSysCol] = true;
  WITH1->MatNZ[Pin1][Pin1] = true;
  WITH1->MatNZ[Pin1][Pin2] = true;
  WITH1->MatNZ[Pin1][Pin3] = true;
  WITH1->MatNZ[Pin1][Pin4] = true;
  WITH1->MatNZ[Pin2][AnaSysCol] = true;
  WITH1->MatNZ[Pin2][Pin1] = true;
  WITH1->MatNZ[Pin2][Pin2] = true;
  WITH1->MatNZ[Pin2][Pin3] = true;
  WITH1->MatNZ[Pin2][Pin4] = true;
  WITH1->MatNZ[Pin3][AnaSysCol] = true;
  WITH1->MatNZ[Pin3][Pin1] = true;
  WITH1->MatNZ[Pin3][Pin2] = true;
  WITH1->MatNZ[Pin3][Pin3] = true;
  WITH1->MatNZ[Pin3][Pin4] = true;
  WITH1->MatNZ[Pin4][AnaSysCol] = true;
  WITH1->MatNZ[Pin4][Pin1] = true;
  WITH1->MatNZ[Pin4][Pin2] = true;
  WITH1->MatNZ[Pin4][Pin3] = true;
  WITH1->MatNZ[Pin4][Pin4] = true;
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_TCAmp*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_TCAmp(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  AnaExt_rec *Pin3Ptr;
  double VPin3, VMem3;
  AnaExt_rec *Pin4Ptr;
  double VPin4, VMem4;
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  double Del, BlendDel, InvBlendDel, V1Offset, V2Offset, MaxV1V2, MinV1V2,
	 Vcmp, BlendVcmp, ExpoVstr, BlendVstr, Vstr, TCtanch, BlendVcmpVdd,
	 MinV1V2Vdd, VcmpVdd, Vbot, ExpoVbot, BlendVbot, Ibot, Vhighcmp,
	 BlendVhighcmp, MaxVddV2, Vtop1, ExpoVtop1, BlendVtop1, Vtop2,
	 ExpVtop2, BlendVtop2, Ihigh;
  Deltasign Sign;
  double TCRes, Iset, Vlow1, ExpoVlow1, BlendVlow1, Vlow2, ExpoVlow2,
	 BlendVlow2, Delta1, Delta2, Delta3, Delta4;
  log_grec *WITH;



  WITH = act->inst;

  /*Implementation notes*/
  /*1. Use Dtlast, Dtlast+Accum for time values*/
  /*Numbers used below are in 'virtual pin numbers' */
  /*Pin1=Vplus*/
  /*Pin3=Vout*/
  /*Pin2=Vminus*/
  /*Pin4=Vset*/


  A_Gate = (Anainstlist *)WITH->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[R_Vplus - 1]->info;
  VPin1 = Pin1Ptr->now;
  VMem1 = A_Gate->Pininfo[R_Vplus - 1].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[R_Vminus - 1]->info;
  VPin2 = Pin2Ptr->now;
  VMem2 = A_Gate->Pininfo[R_Vminus - 1].Vmem;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[R_Vout - 1]->info;
  VPin3 = Pin3Ptr->now;
  VMem3 = A_Gate->Pininfo[R_Vout - 1].Vmem;
  Pin4Ptr = (AnaExt_rec *)WITH->pin[R_Vset - 1]->info;
  VPin4 = Pin4Ptr->now;
  VMem4 = A_Gate->Pininfo[R_Vset - 1].Vmem;
  switch (act->pin) {

  case R_Vplus:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta1 = VPin1 - VMem1;
    act->Iin = TCAmpVar->TauVdiff * (Delta1 - VPin2 + VMem2) +
	       TCAmpVar->TauVout * (Delta1 - VPin3 + VMem3) +
	       TCAmpVar->TauVset * (Delta1 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case R_Vminus:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta2 = VPin2 - VMem2;
    act->Iin = TCAmpVar->TauVout * (Delta2 - VPin3 + VMem3) +
	       TCAmpVar->TauVdiff * (Delta2 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta2 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case R_Vout:   /*Allows attr^[]. access*/
    WITH = act->inst;
    V1Offset = VPin1 + TCAmpVar->HalfOffset;
    V2Offset = VPin2 - TCAmpVar->HalfOffset;
    Del = V1Offset - V2Offset;
    if (fabs(Del) < 0.000708) {
      BlendDel = 1 / (1 + AnaExpo(100000L * Del));
      InvBlendDel = 1 - BlendDel;
      MaxV1V2 = V2Offset * BlendDel + V1Offset * InvBlendDel;
      MinV1V2 = V1Offset * BlendDel + V2Offset * InvBlendDel;
      Sign = Zero;
    } else {
      if (Del > 0) {
	BlendDel = 0.0;
	InvBlendDel = 1.0;
	MaxV1V2 = V1Offset;
	MinV1V2 = V2Offset;
	Sign = Positive;
      } else {
	BlendDel = 1.0;
	InvBlendDel = 0.0;
	MaxV1V2 = V2Offset;
	MinV1V2 = V1Offset;
	Sign = Negative;
      }
    }

    Vcmp = MaxV1V2 - VPin4;
    if (fabs(Vcmp) < 0.000708) {
      BlendVcmp = 1 / (1 + AnaExpo(100000L * Vcmp));
      Vstr = MaxV1V2 * BlendVcmp + VPin4 * (1 - BlendVcmp);
    } else {
      if (Vcmp > 0)
	Vstr = VPin4;
      else
	Vstr = MaxV1V2;
    }


    ExpoVstr = AnaExpo(TCAmpVar->Kn2 * Vstr);
    BlendVstr = 1 / (1 + TCAmpVar->Kn5 * ExpoVstr);
    Iset = TCAmpVar->Kn1 * ExpoVstr * BlendVstr +
	   TCAmpVar->Kn4 * Vstr * Vstr * (1 - BlendVstr);

    TCtanch = AnaTanch(Del * TCAmpVar->HalfIKn2);

    if (AmpRange == Normal) {
      VcmpVdd = AnaVdd - MinV1V2;
      if (fabs(VcmpVdd) < 0.000708) {
	BlendVcmpVdd = 1 / (1 + AnaExpo(100000L * VcmpVdd));
	MinV1V2Vdd = AnaVdd * BlendVcmpVdd + MinV1V2 * (1 - BlendVcmpVdd);
      } else {
	if (VcmpVdd > 0)
	  MinV1V2Vdd = MinV1V2;
	else
	  MinV1V2Vdd = AnaVdd;
      }

      Vbot = MinV1V2Vdd - VPin3;
      ExpoVbot = AnaExpo(TCAmpVar->Kn2 * Vbot);
      BlendVbot = 1 / (1 + TCAmpVar->Kn5 * ExpoVbot);
      Ibot = TCAmpVar->Kn1 * ExpoVbot * BlendVbot +
	     (1 - BlendVbot) * TCAmpVar->Kn4 * Vbot * Vbot;
    } else {
      Vlow1 = VPin4 - VPin3;
      ExpoVlow1 = AnaExpo(TCAmpVar->Kn2 * Vlow1);
      BlendVlow1 = 1 / (1 + TCAmpVar->Kn5 * ExpoVlow1);
      Vlow2 = -VPin3;
      ExpoVlow2 = AnaExpo(TCAmpVar->Kn2 * Vlow2);
      BlendVlow2 = 1 / (1 + TCAmpVar->Kn5 * ExpoVlow2);


      Ibot = BlendDel * (TCAmpVar->Kn1 * ExpoVlow1 * BlendVlow1 +
			 (1 - BlendVlow1) * TCAmpVar->Kn4 * Vlow1 * Vlow1) +
	     InvBlendDel * (TCAmpVar->Kn1 * ExpoVlow2 * BlendVlow2 +
			    (1 - BlendVlow2) * TCAmpVar->Kn4 * Vlow2 * Vlow2);
    }

    if (AmpRange == Normal) {
      Vhighcmp = AnaVdd - VPin2;
      if (fabs(Vhighcmp) < 0.000708) {
	BlendVhighcmp = 1 / (1 + AnaExpo(100000L * Vhighcmp));
	MaxVddV2 = VPin2 * BlendVhighcmp + AnaVdd * (1 - BlendVhighcmp);
      } else {
	if (Vhighcmp > 0)
	  MaxVddV2 = AnaVdd;
	else
	  MaxVddV2 = VPin2;
      }
    } else
      MaxVddV2 = AnaVdd;

    if (Sign == Zero) {
      Vtop1 = VPin3 - AnaVdd;
      ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
      BlendVtop1 = 1 / (1 + TCAmpVar->Kn5 * ExpoVtop1);
      Vtop2 = VPin3 + VPin4 - MaxVddV2;
      ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
      BlendVtop2 = 1 / (1 + TCAmpVar->Kn5 * ExpVtop2);
      Ihigh = -(BlendDel * (TCAmpVar->Kp1 * ExpoVtop1 * BlendVtop1 +
		  (1 - BlendVtop1) * TCAmpVar->Kp4 * Vtop1 * Vtop1) +
		InvBlendDel * (TCAmpVar->Kp1 * ExpVtop2 * BlendVtop2 +
		  (1 - BlendVtop2) * TCAmpVar->Kp4 * Vtop2 * Vtop2));
    } else {
      if (Sign == Positive) {
	Vtop2 = VPin3 + VPin4 - MaxVddV2;
	ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
	BlendVtop2 = 1 / (1 + TCAmpVar->Kn5 * ExpVtop2);
	Ihigh = -(TCAmpVar->Kp1 * ExpVtop2 * BlendVtop2 +
		  (1 - BlendVtop2) * TCAmpVar->Kp4 * Vtop2 * Vtop2);
      } else {
	Vtop1 = VPin3 - AnaVdd;
	ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
	BlendVtop1 = 1 / (1 + TCAmpVar->Kn5 * ExpoVtop1);
	Ihigh = -(TCAmpVar->Kp1 * ExpoVtop1 * BlendVtop1 +
		  (1 - BlendVtop1) * TCAmpVar->Kp4 * Vtop1 * Vtop1);
      }
    }

    TCRes = (BlendDel * (1 + VPin3 * TCAmpVar->InvVcross) *
	     WITH->attr[N_Ineg - 1].UU.r +
	     InvBlendDel * (1 + (AnaVdd - VPin3) * TCAmpVar->InvVcross) *
	     WITH->attr[N_Ipos - 1].UU.r) * TCtanch;

    Delta3 = VPin3 - VMem3;
    act->Iin = TCAmpVar->TauVout * (Delta3 - VPin2 + VMem2) - Ibot - Ihigh -
	       Iset * TCRes + TCAmpVar->TauVout * (Delta3 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta3 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case R_Vset:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta4 = VPin4 - VMem4;
    act->Iin = TCAmpVar->TauVset * (Delta4 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta4 - VPin2 + VMem2) +
	       TCAmpVar->TauVset * (Delta4 - VPin3 + VMem3) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_TCAmp*/




/*==========================================================================*/
/*                 Library Listing For  Cell TCAmp                    */
/*==========================================================================*/


Static Void TC_System(act)
Analog_32_action *act;
{




  /****************************************************************************/
  /*            Procedure To Simulate Cell In Algorithm                        */
  /****************************************************************************/
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    INITLIB_TCAMP(act);
    break;

  case Analog_act_ex:
    Ex_TCAmp(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_TCAmp(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    Update_TCAmp(act);
    break;

  case Analog_act_display:
    Display_TCAmp(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_TCAmp(act);
    break;

  case Analog_act_reset:
    Reset_TCAmp(act);
    break;

  case Analog_act_newgate:
    Newgate_TCAmp(act);
    break;

  case Analog_act_copygate:
    Copygate_TCAmp(act);
    break;

  case Analog_act_disposegate:
    Dispose_TCAmp(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_TCAmp(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_TCAmp(act);
    break;

  case Analog_act_erase:
    Erase_TCAmp(act);
    break;

  case Analog_act_refresh:
    Refresh_TCAmp(act);
    break;

  case Analog_act_probe:
    Probe_TCAmp(act);
    break;
  }
}


Local Void Ex_Abs(act)
Analog_32_action *act;
{
  double MinI1, dI1dV1, dI1dV2, dI1dV3, dI1dV4, MinI2, dI2dV1, dI2dV2, dI2dV3,
	 dI2dV4, MinI3, dI3dV1, dI3dV2, dI3dV3, dI3dV4, MinI4, dI4dV1, dI4dV2,
	 dI4dV3, dI4dV4;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2, VMem2;
  AnaExt_rec *Pin3Ptr;
  long Pin3;
  double VPin3, VMem3;
  AnaExt_rec *Pin4Ptr;
  long Pin4;
  double VPin4, VMem4;
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;
  double Icap, Del, ExpoDel, BlendDel, InvBlendDel, dBlendDeldV2, V1Offset,
	 V2Offset, MaxV1V2, dMaxV1V2dV2, dMaxV1V2dV1, Vcmp, ExpoVcmp,
	 BlendVcmp, InvBlendVcmp, dBlendVcmpdVset, dBlendVcmpdV2,
	 dBlendVcmpdV1, Vstr, dVstrdV1, dVstrdV2, dVstrdVset, ExpoVstr,
	 Kn5_ExpoVstr, BlendVstr, InvBlendVstr, dBlendVstr_Temp,
	 dBlendVstrdV1, dBlendVstrdV2, dBlendVstrdVset, Iset_Temp1,
	 Iset_Temp2, Iset, dIset_Temp1, dIset_Temp2, dIsetdV1, dIsetdV2,
	 dIsetdVset, TCtanch, dTCTanchdV1, Vtop1, ExpoVtop1, BlendVtop1Temp,
	 BlendVtop1, InvBlendVtop1, Itop1, dBlendVtop1dVout, dItop1Temp1,
	 dItop1Temp2, dItop1Temp3, dItop1dVout, Vtop2, ExpVtop2,
	 BlendVtop2Temp, BlendVtop2, InvBlendVtop2, dBlendVtop2Temp1,
	 dBlendVtop2dVout, dBlendVtop2dVset, Itop2, dItop2Temp1, dItop2Temp2,
	 dItop2dVout, dItop2dVset, Ihigh, dIhighTemp, dIhighdV1, dIhighdV2,
	 dIhighdVout, dIhighdVset;
/* p2c: stdlib.text, line 5669: 
 * Note: Line breaker spent 2.0 seconds, 5000 tries on line 1919 [251] */
  Deltasign Sign;
  double Ires, dIresdVout, TCRes, dTCTemp, dTCResdV1, dTCResdV2, dTCResdVout,
	 Delta1, Delta2, Delta3, Delta4, Itop2Temp1, Itop2Temp2, Itop2Temp3,
	 Absv, dAbsvdV1;
  log_grec *WITH;
  AnaMatRec *WITH1;



  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[A_Vplus - 1]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  VMem1 = A_Gate->Pininfo[A_Vplus - 1].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[A_Vminus - 1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;
  VMem2 = A_Gate->Pininfo[A_Vminus - 1].Vmem;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[A_Vout - 1]->info;
  Pin3 = Pin3Ptr->nodenum;
  VPin3 = Pin3Ptr->last;
  VMem3 = A_Gate->Pininfo[A_Vout - 1].Vmem;
  Pin4Ptr = (AnaExt_rec *)WITH->pin[A_Vset - 1]->info;
  Pin4 = Pin4Ptr->nodenum;
  VPin4 = Pin4Ptr->last;
  VMem4 = A_Gate->Pininfo[A_Vset - 1].Vmem;
      /*Use with statement to get to internal variables*/

  /*Implementation notes*/
  /*1.attributes may be referred to directly by attr^[]. syntax*/
  /*2.use Dt, Accum+Dt for timevalue*/
  /*Numbers used below are in 'virtual pin numbers' */
  /*Pin1=Vplus*/
  /*Pin3=Vout*/
  /*Pin2=Vminus*/
  /*Pin4=Vset*/


  V1Offset = VPin1 + TCAmpVar->HalfOffset;
  V2Offset = VPin2 - TCAmpVar->HalfOffset;
  Del = V1Offset - V2Offset;
  if (fabs(Del) < 0.000708) {
    ExpoDel = AnaExpo(100000L * Del);
    BlendDel = 1 / (1 + ExpoDel);
    InvBlendDel = 1 - BlendDel;
    dBlendDeldV2 = 100000L * ExpoDel * BlendDel * BlendDel;
    MaxV1V2 = V2Offset * BlendDel + V1Offset * InvBlendDel;
    dMaxV1V2dV2 = BlendDel - Del * dBlendDeldV2;
    dMaxV1V2dV1 = 1 - dMaxV1V2dV2;
    Sign = Zero;
  } else {
    if (Del > 0) {
      BlendDel = 0.0;
      InvBlendDel = 1.0;
      dBlendDeldV2 = 0.0;
      MaxV1V2 = V1Offset;
      dMaxV1V2dV2 = 0.0;
      dMaxV1V2dV1 = 1.0;
      Sign = Positive;
    } else {
      BlendDel = 1.0;
      InvBlendDel = 0.0;
      dBlendDeldV2 = 0.0;
      MaxV1V2 = V2Offset;
      dMaxV1V2dV2 = 1.0;
      dMaxV1V2dV1 = 0.0;
      Sign = Negative;
    }
  }

  Vcmp = MaxV1V2 - VPin4;
  if (fabs(Vcmp) < 0.000708) {
    ExpoVcmp = AnaExpo(100000L * Vcmp);   /*Could be optimized via flags*/
    BlendVcmp = 1 / (1 + ExpoVcmp);
    InvBlendVcmp = 1 - BlendVcmp;
    dBlendVcmpdVset = 100000L * ExpoVcmp * BlendVcmp * BlendVcmp;
    dBlendVcmpdV2 = -dBlendVcmpdVset * dMaxV1V2dV2;
    dBlendVcmpdV1 = -dBlendVcmpdVset - dBlendVcmpdV2;
    Vstr = MaxV1V2 * BlendVcmp + VPin4 * InvBlendVcmp;
    dVstrdV1 = dMaxV1V2dV1 * BlendVcmp + Vcmp * dBlendVcmpdV1;
    dVstrdV2 = dMaxV1V2dV2 * BlendVcmp + Vcmp * dBlendVcmpdV2;
    dVstrdVset = InvBlendVcmp + Vcmp * dBlendVcmpdVset;
  } else {
    if (Vcmp > 0) {
      /*BlendVcmp:=0;*/
      /*InvBlendVcmp:=1;*/
      /*dBlendVcmpdVset:=0;*/
      /*dBlendVcmpdV2:=0;*/
      /*dBlendVcmpdV1:=0;*/
      Vstr = VPin4;
      dVstrdV1 = 0.0;
      dVstrdV2 = 0.0;
      dVstrdVset = 1.0;
    } else {
      /*BlendVcmp:=1;*/
      /*InvBlendVcmp:=0;*/
      /*dBlendVcmpdVset:=0;*/
      /*dBlendVcmpdV2:=0;*/
      /*dBlendVcmpdV1:=0;*/
      Vstr = MaxV1V2;
      dVstrdV1 = dMaxV1V2dV1;
      dVstrdV2 = dMaxV1V2dV2;
      dVstrdVset = 0.0;
    }
  }


  ExpoVstr = AnaExpo(TCAmpVar->Kn2 * Vstr);
  Kn5_ExpoVstr = TCAmpVar->Kn5 * ExpoVstr;
  BlendVstr = 1 / (1 + Kn5_ExpoVstr);
  InvBlendVstr = 1 - BlendVstr;
  dBlendVstr_Temp = -TCAmpVar->Kn2 * Kn5_ExpoVstr * BlendVstr * BlendVstr;
  dBlendVstrdV1 = dBlendVstr_Temp * dVstrdV1;
  dBlendVstrdV2 = dBlendVstr_Temp * dVstrdV2;
  dBlendVstrdVset = dBlendVstr_Temp * dVstrdVset;
  /*^Could be optimized via flags*/

  Iset_Temp1 = TCAmpVar->Kn1 * ExpoVstr;
  Iset_Temp2 = TCAmpVar->Kn4 * Vstr * Vstr;
  Iset = Iset_Temp1 * BlendVstr + Iset_Temp2 * InvBlendVstr;
  dIset_Temp1 = Iset_Temp1 - Iset_Temp2;
  dIset_Temp2 = Iset_Temp1 * TCAmpVar->Kn2 * BlendVstr +
		TCAmpVar->TwoKn4 * InvBlendVstr * Vstr;
  dIsetdV1 = dIset_Temp1 * dBlendVstrdV1 + dIset_Temp2 * dVstrdV1;
  dIsetdV2 = dIset_Temp1 * dBlendVstrdV2 + dIset_Temp2 * dVstrdV2;
  dIsetdVset = dIset_Temp1 * dBlendVstrdVset + dIset_Temp2 * dVstrdVset;
  /*^Could be optimized via flags*/


  TCtanch = AnaTanch(Del * TCAmpVar->HalfIKn2);
  dTCTanchdV1 = (1 - TCtanch * TCtanch) * TCAmpVar->HalfIKn2;
  Absv = -TCtanch * BlendDel * WITH->attr[N_Ipos - 1].UU.r;
  dAbsvdV1 = (TCtanch * dBlendDeldV2 - dTCTanchdV1 * BlendDel) *
	     WITH->attr[N_Ipos - 1].UU.r;

  if (AmpRange == Fwr) {
    Absv = TCtanch * InvBlendDel * WITH->attr[N_Ineg - 1].UU.r + Absv;
    dAbsvdV1 = (dTCTanchdV1 * InvBlendDel + TCtanch * dBlendDeldV2) *
	       WITH->attr[N_Ineg - 1].UU.r + dAbsvdV1;
  }



  if (Sign == Zero && AmpRange == Hwr) {
    Vtop1 = VPin3 - AnaVdd;
    ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
    BlendVtop1Temp = TCAmpVar->Kn5 * ExpoVtop1;
    BlendVtop1 = 1 / (1 + BlendVtop1Temp);
    InvBlendVtop1 = 1 - BlendVtop1;
    dBlendVtop1dVout = -TCAmpVar->Kn2 * BlendVtop1 * BlendVtop1 * BlendVtop1Temp;

    dItop1Temp1 = TCAmpVar->Kp1 * ExpoVtop1;
    dItop1Temp2 = TCAmpVar->Kp4 * Vtop1 * Vtop1;
    dItop1Temp3 = dItop1Temp1 * BlendVtop1;
    Itop1 = dItop1Temp3 + InvBlendVtop1 * dItop1Temp2;
    dItop1dVout = TCAmpVar->Kn2 * dItop1Temp3 +
		  (dItop1Temp1 - dItop1Temp2) * dBlendVtop1dVout +
		  TCAmpVar->TwoKp4 * InvBlendVtop1 * Vtop1;

    Vtop2 = VPin3 + VPin4 - AnaVdd;
    ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
    BlendVtop2Temp = TCAmpVar->Kn5 * ExpVtop2;
    BlendVtop2 = 1 / (1 + BlendVtop2Temp);
    InvBlendVtop2 = 1 - BlendVtop2;
    dBlendVtop2Temp1 = -TCAmpVar->Kn2 * BlendVtop2 * BlendVtop2Temp * BlendVtop2;
    dBlendVtop2dVout = dBlendVtop2Temp1;
    dBlendVtop2dVset = dBlendVtop2Temp1;

    Itop2Temp1 = TCAmpVar->Kp1 * ExpVtop2;
    Itop2Temp2 = TCAmpVar->Kp4 * Vtop2 * Vtop2;
    Itop2Temp3 = Itop2Temp1 * BlendVtop2;
    Itop2 = Itop2Temp3 + InvBlendVtop2 * Itop2Temp2;
    dItop2Temp1 = TCAmpVar->Kn2 * Itop2Temp3 +
		  TCAmpVar->TwoKp4 * InvBlendVtop2 * Vtop2;
    dItop2Temp2 = Itop2Temp1 - Itop2Temp2;
    dItop2dVout = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVout;
    dItop2dVset = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVset;

    Ihigh = -(InvBlendDel * Itop1 + BlendDel * Itop2);
    dIhighTemp = Itop2 - Itop1;
    dIhighdV1 = dBlendDeldV2 * dIhighTemp;
    dIhighdV2 = -dIhighdV1;
    dIhighdVout = -(InvBlendDel * dItop1dVout + BlendDel * dItop2dVout);
    dIhighdVset = -BlendDel * dItop2dVset;
  } else {
    if (Sign == Negative && AmpRange == Hwr || AmpRange == Fwr) {
      Vtop2 = VPin3 + VPin4 - AnaVdd;
      ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
      BlendVtop2Temp = TCAmpVar->Kn5 * ExpVtop2;
      BlendVtop2 = 1 / (1 + BlendVtop2Temp);
      InvBlendVtop2 = 1 - BlendVtop2;
      dBlendVtop2Temp1 = -TCAmpVar->Kn2 * BlendVtop2 * BlendVtop2Temp * BlendVtop2;
      dBlendVtop2dVout = dBlendVtop2Temp1;
      dBlendVtop2dVset = dBlendVtop2Temp1;

      Itop2Temp1 = TCAmpVar->Kp1 * ExpVtop2;
      Itop2Temp2 = TCAmpVar->Kp4 * Vtop2 * Vtop2;
      Itop2Temp3 = Itop2Temp1 * BlendVtop2;
      Itop2 = Itop2Temp3 + InvBlendVtop2 * Itop2Temp2;
      dItop2Temp1 = TCAmpVar->Kn2 * Itop2Temp3 +
		    TCAmpVar->TwoKp4 * InvBlendVtop2 * Vtop2;
      dItop2Temp2 = Itop2Temp1 - Itop2Temp2;
      dItop2dVout = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVout;
      dItop2dVset = dItop2Temp1 + dItop2Temp2 * dBlendVtop2dVset;

      Ihigh = -Itop2;
      dIhighdV1 = 0.0;
      dIhighdV2 = 0.0;
      dIhighdVout = -dItop2dVout;
      dIhighdVset = -dItop2dVset;
    } else {
      Vtop1 = VPin3 - AnaVdd;
      ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
      BlendVtop1Temp = TCAmpVar->Kn5 * ExpoVtop1;
      BlendVtop1 = 1 / (1 + BlendVtop1Temp);
      InvBlendVtop1 = 1 - BlendVtop1;
      dBlendVtop1dVout = -TCAmpVar->Kn2 * BlendVtop1 * BlendVtop1 * BlendVtop1Temp;

      dItop1Temp1 = TCAmpVar->Kp1 * ExpoVtop1;
      dItop1Temp2 = TCAmpVar->Kp4 * Vtop1 * Vtop1;
      dItop1Temp3 = dItop1Temp1 * BlendVtop1;
      Itop1 = dItop1Temp3 + InvBlendVtop1 * dItop1Temp2;
      dItop1dVout = TCAmpVar->Kn2 * dItop1Temp3 +
		    (dItop1Temp1 - dItop1Temp2) * dBlendVtop1dVout +
		    TCAmpVar->TwoKp4 * InvBlendVtop1 * Vtop1;
      Ihigh = -Itop1;
      dIhighdV1 = 0.0;
      dIhighdV2 = 0.0;
      dIhighdVout = -dItop1dVout;
      dIhighdVset = 0.0;
    }
  }



  Ires = 1 + (AnaVdd - VPin3) * TCAmpVar->InvVcross;
  dIresdVout = -TCAmpVar->InvVcross;


  TCRes = Ires * Absv;
  dTCTemp = Ires * dAbsvdV1;
  dTCResdV1 = dTCTemp;
  dTCResdV2 = -dTCTemp;
  dTCResdVout = dIresdVout * Absv;


  MinI3 = Iset * TCRes + Ihigh;
  dI3dV1 = -(Iset * dTCResdV1 + TCRes * dIsetdV1 + dIhighdV1);
  dI3dV2 = -(Iset * dTCResdV2 + TCRes * dIsetdV2 + dIhighdV2);
  dI3dV3 = -(Iset * dTCResdVout + dIhighdVout);
  dI3dV4 = -(TCRes * dIsetdVset + dIhighdVset);



  Delta1 = VPin1 - VMem1;
  Delta2 = VPin2 - VMem2;
  Delta3 = VPin3 - VMem3;
  Delta4 = VPin4 - VMem4;

  if (AnaChangedDt)
    TCAmpVar->TauVset = WITH->attr[N_CapVset - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta1);   /*CapVset - Vplus*/
  MinI4 = -Icap;
  dI4dV1 = -TCAmpVar->TauVset;
  dI4dV4 = TCAmpVar->TauVset;
  MinI1 = Icap;
  dI1dV1 = TCAmpVar->TauVset;
  dI1dV4 = -TCAmpVar->TauVset;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta2);   /*CapVset - Vminus*/
  MinI4 -= Icap;
  dI4dV2 = -TCAmpVar->TauVset;
  dI4dV4 += TCAmpVar->TauVset;
  MinI2 = Icap;
  dI2dV2 = TCAmpVar->TauVset;
  dI2dV4 = -TCAmpVar->TauVset;
  Icap = TCAmpVar->TauVset * (Delta4 - Delta3);   /*CapVset - Vout*/
  MinI4 -= Icap;
  dI4dV3 = -TCAmpVar->TauVset;
  dI4dV4 += TCAmpVar->TauVset;
  MinI3 += Icap;
  dI3dV3 += TCAmpVar->TauVset;
  dI3dV4 -= TCAmpVar->TauVset;

  if (AnaChangedDt)
    TCAmpVar->TauVout = WITH->attr[N_CapVout - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVout * (Delta3 - Delta2);   /*CapVout Minus*/
  MinI3 -= Icap;
  dI3dV2 -= TCAmpVar->TauVout;
  dI3dV3 += TCAmpVar->TauVout;
  MinI2 += Icap;
  dI2dV2 += TCAmpVar->TauVout;
  dI2dV3 = -TCAmpVar->TauVout;
  Icap = TCAmpVar->TauVout * (Delta3 - Delta1);   /*CapVout Vplus*/
  MinI3 -= Icap;
  dI3dV1 -= TCAmpVar->TauVout;
  dI3dV3 += TCAmpVar->TauVout;
  MinI1 += Icap;
  dI1dV1 += TCAmpVar->TauVout;
  dI1dV3 = -TCAmpVar->TauVout;

  if (AnaChangedDt)
    TCAmpVar->TauVdiff = WITH->attr[N_CapVdiff - 1].UU.r / AnaDt;
  Icap = TCAmpVar->TauVdiff * (Delta2 - Delta1);   /*CapVdiff*/
  MinI2 -= Icap;
  dI2dV1 = -TCAmpVar->TauVdiff;
  dI2dV2 += TCAmpVar->TauVdiff;
  MinI1 += Icap;
  dI1dV1 += TCAmpVar->TauVdiff;
  dI1dV2 = -TCAmpVar->TauVdiff;
  WITH1 = AnaSystem;
  WITH1->Mat[Pin1][AnaSysCol] += MinI1;
  WITH1->Mat[Pin1][Pin1] += dI1dV1;
  WITH1->Mat[Pin1][Pin2] += dI1dV2;
  WITH1->Mat[Pin1][Pin3] += dI1dV3;
  WITH1->Mat[Pin1][Pin4] += dI1dV4;
  WITH1->Mat[Pin2][AnaSysCol] += MinI2;
  WITH1->Mat[Pin2][Pin1] += dI2dV1;
  WITH1->Mat[Pin2][Pin2] += dI2dV2;
  WITH1->Mat[Pin2][Pin3] += dI2dV3;
  WITH1->Mat[Pin2][Pin4] += dI2dV4;
  WITH1->Mat[Pin3][AnaSysCol] += MinI3;
  WITH1->Mat[Pin3][Pin1] += dI3dV1;
  WITH1->Mat[Pin3][Pin2] += dI3dV2;
  WITH1->Mat[Pin3][Pin3] += dI3dV3;
  WITH1->Mat[Pin3][Pin4] += dI3dV4;
  WITH1->Mat[Pin4][AnaSysCol] += MinI4;
  WITH1->Mat[Pin4][Pin1] += dI4dV1;
  WITH1->Mat[Pin4][Pin2] += dI4dV2;
  WITH1->Mat[Pin4][Pin3] += dI4dV3;
  WITH1->Mat[Pin4][Pin4] += dI4dV4;
  WITH1->MatNZ[Pin1][AnaSysCol] = true;
  WITH1->MatNZ[Pin1][Pin1] = true;
  WITH1->MatNZ[Pin1][Pin2] = true;
  WITH1->MatNZ[Pin1][Pin3] = true;
  WITH1->MatNZ[Pin1][Pin4] = true;
  WITH1->MatNZ[Pin2][AnaSysCol] = true;
  WITH1->MatNZ[Pin2][Pin1] = true;
  WITH1->MatNZ[Pin2][Pin2] = true;
  WITH1->MatNZ[Pin2][Pin3] = true;
  WITH1->MatNZ[Pin2][Pin4] = true;
  WITH1->MatNZ[Pin3][AnaSysCol] = true;
  WITH1->MatNZ[Pin3][Pin1] = true;
  WITH1->MatNZ[Pin3][Pin2] = true;
  WITH1->MatNZ[Pin3][Pin3] = true;
  WITH1->MatNZ[Pin3][Pin4] = true;
  WITH1->MatNZ[Pin4][AnaSysCol] = true;
  WITH1->MatNZ[Pin4][Pin1] = true;
  WITH1->MatNZ[Pin4][Pin2] = true;
  WITH1->MatNZ[Pin4][Pin3] = true;
  WITH1->MatNZ[Pin4][Pin4] = true;
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_Abs*/


/****************************************************************************/
/*          Procedure To Supply Current Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Abs(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  AnaExt_rec *Pin3Ptr;
  double VPin3, VMem3;
  AnaExt_rec *Pin4Ptr;
  double VPin4, VMem4;
  TCAmpConst *TCAmpVar;
  Anainstlist *A_Gate;

  double Del, BlendDel, InvBlendDel, V1Offset, V2Offset, MaxV1V2, Vcmp,
	 BlendVcmp, ExpoVstr, BlendVstr, Vstr, TCtanch, Vtop1, ExpoVtop1,
	 BlendVtop1, Vtop2, ExpVtop2, BlendVtop2, Ihigh;
  Deltasign Sign;
  double TCRes, Iset, Delta1, Delta2, Delta3, Delta4, Absv;
  log_grec *WITH;



  WITH = act->inst;

  /*Implementation notes*/
  /*1. Use Dtlast, Dtlast+Accum for time values*/
  /*Numbers used below are in 'virtual pin numbers' */
  /*Pin1=Vplus*/
  /*Pin3=Vout*/
  /*Pin2=Vminus*/
  /*Pin4=Vset*/


  A_Gate = (Anainstlist *)WITH->info;
  TCAmpVar = (TCAmpConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[A_Vplus - 1]->info;
  VPin1 = Pin1Ptr->now;
  VMem1 = A_Gate->Pininfo[A_Vplus - 1].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[A_Vminus - 1]->info;
  VPin2 = Pin2Ptr->now;
  VMem2 = A_Gate->Pininfo[A_Vminus - 1].Vmem;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[A_Vout - 1]->info;
  VPin3 = Pin3Ptr->now;
  VMem3 = A_Gate->Pininfo[A_Vout - 1].Vmem;
  Pin4Ptr = (AnaExt_rec *)WITH->pin[A_Vset - 1]->info;
  VPin4 = Pin4Ptr->now;
  VMem4 = A_Gate->Pininfo[A_Vset - 1].Vmem;
  switch (act->pin) {

  case A_Vplus:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta1 = VPin1 - VMem1;
    act->Iin = TCAmpVar->TauVdiff * (Delta1 - VPin2 + VMem2) +
	       TCAmpVar->TauVout * (Delta1 - VPin3 + VMem3) +
	       TCAmpVar->TauVset * (Delta1 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case A_Vminus:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta2 = VPin2 - VMem2;
    act->Iin = TCAmpVar->TauVout * (Delta2 - VPin3 + VMem3) +
	       TCAmpVar->TauVdiff * (Delta2 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta2 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case A_Vout:   /*Allows attr^[]. access*/
    WITH = act->inst;
    V1Offset = VPin1 + TCAmpVar->HalfOffset;
    V2Offset = VPin2 - TCAmpVar->HalfOffset;
    Del = V1Offset - V2Offset;
    if (fabs(Del) < 0.000708) {
      BlendDel = 1 / (1 + AnaExpo(100000L * Del));
      InvBlendDel = 1 - BlendDel;
      MaxV1V2 = V2Offset * BlendDel + V1Offset * InvBlendDel;
      Sign = Zero;
    } else {
      if (Del > 0) {
	BlendDel = 0.0;
	InvBlendDel = 1.0;
	MaxV1V2 = V1Offset;
	Sign = Positive;
      } else {
	BlendDel = 1.0;
	InvBlendDel = 0.0;
	MaxV1V2 = V2Offset;
	Sign = Negative;
      }
    }

    Vcmp = MaxV1V2 - VPin4;
    if (fabs(Vcmp) < 0.000708) {
      BlendVcmp = 1 / (1 + AnaExpo(100000L * Vcmp));
      Vstr = MaxV1V2 * BlendVcmp + VPin4 * (1 - BlendVcmp);
    } else {
      if (Vcmp > 0)
	Vstr = VPin4;
      else
	Vstr = MaxV1V2;
    }


    ExpoVstr = AnaExpo(TCAmpVar->Kn2 * Vstr);
    BlendVstr = 1 / (1 + TCAmpVar->Kn5 * ExpoVstr);
    Iset = TCAmpVar->Kn1 * ExpoVstr * BlendVstr +
	   TCAmpVar->Kn4 * Vstr * Vstr * (1 - BlendVstr);

    TCtanch = AnaTanch(Del * TCAmpVar->HalfIKn2);
    Absv = -TCtanch * BlendDel * WITH->attr[N_Ipos - 1].UU.r;
    if (AmpRange == Fwr)
      Absv = TCtanch * InvBlendDel * WITH->attr[N_Ineg - 1].UU.r + Absv;

    if (Sign == Zero && AmpRange == Hwr) {
      Vtop1 = VPin3 - AnaVdd;
      ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
      BlendVtop1 = 1 / (1 + TCAmpVar->Kn5 * ExpoVtop1);
      Vtop2 = VPin3 + VPin4 - AnaVdd;
      ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
      BlendVtop2 = 1 / (1 + TCAmpVar->Kn5 * ExpVtop2);
      Ihigh = -(InvBlendDel * (TCAmpVar->Kp1 * ExpoVtop1 * BlendVtop1 +
		  (1 - BlendVtop1) * TCAmpVar->Kp4 * Vtop1 * Vtop1) +
	  BlendDel * (TCAmpVar->Kp1 * ExpVtop2 * BlendVtop2 +
		      (1 - BlendVtop2) * TCAmpVar->Kp4 * Vtop2 * Vtop2));
    } else {
      if (Sign == Negative && AmpRange == Hwr || AmpRange == Fwr) {
	Vtop2 = VPin3 + VPin4 - AnaVdd;
	ExpVtop2 = AnaExpo(TCAmpVar->Kn2 * Vtop2);
	BlendVtop2 = 1 / (1 + TCAmpVar->Kn5 * ExpVtop2);
	Ihigh = -(TCAmpVar->Kp1 * ExpVtop2 * BlendVtop2 +
		  (1 - BlendVtop2) * TCAmpVar->Kp4 * Vtop2 * Vtop2);
      } else {
	Vtop1 = VPin3 - AnaVdd;
	ExpoVtop1 = AnaExpo(TCAmpVar->Kn2 * Vtop1);
	BlendVtop1 = 1 / (1 + TCAmpVar->Kn5 * ExpoVtop1);
	Ihigh = -(TCAmpVar->Kp1 * ExpoVtop1 * BlendVtop1 +
		  (1 - BlendVtop1) * TCAmpVar->Kp4 * Vtop1 * Vtop1);
      }
    }

    TCRes = (1 + (AnaVdd - VPin3) * TCAmpVar->InvVcross) * Absv;

    Delta3 = VPin3 - VMem3;
    act->Iin = TCAmpVar->TauVout * (Delta3 - VPin2 + VMem2) - Ihigh -
	       Iset * TCRes + TCAmpVar->TauVout * (Delta3 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta3 - VPin4 + VMem4) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case A_Vset:   /*Allows attr^[]. access*/
    WITH = act->inst;
    Delta4 = VPin4 - VMem4;
    act->Iin = TCAmpVar->TauVset * (Delta4 - VPin1 + VMem1) +
	       TCAmpVar->TauVset * (Delta4 - VPin2 + VMem2) +
	       TCAmpVar->TauVset * (Delta4 - VPin3 + VMem3) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_Abs*/


/*==========================================================================*/
/*                 Library Listing For  Cell Rectifiers                    */
/*==========================================================================*/


Static Void Abs_System(act)
Analog_32_action *act;
{



  /****************************************************************************/
  /*            Procedure To Simulate Cell In Algorithm                        */
  /****************************************************************************/
  /*Main Procedure*/




  switch (act->action) {

  case Analog_act_newkind:
    INITLIB_TCAMP(act);
    break;

  case Analog_act_ex:
    Ex_Abs(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_TCAmp(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    Update_TCAmp(act);
    break;

  case Analog_act_display:
    Display_TCAmp(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_TCAmp(act);
    break;

  case Analog_act_reset:
    Reset_TCAmp(act);
    break;

  case Analog_act_newgate:
    Newgate_TCAmp(act);
    break;

  case Analog_act_copygate:
    Copygate_TCAmp(act);
    break;

  case Analog_act_disposegate:
    Dispose_TCAmp(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_TCAmp(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Abs(act);
    break;

  case Analog_act_erase:
    Erase_TCAmp(act);
    break;

  case Analog_act_refresh:
    Refresh_TCAmp(act);
    break;

  case Analog_act_probe:
    Probe_TCAmp(act);
    break;
  }
}


Void Log_opamp_initlib_32(act)
Analog_32_action *act;
{
  AmpRange = Normal;
  TC_System(act);
}


Void Log_wramp_initlib_32(act)
Analog_32_action *act;
{
  AmpRange = Wide;
  TC_System(act);
}


Void Log_hwr_initlib_32(act)
Analog_32_action *act;
{
  AmpRange = Hwr;
  Abs_System(act);
}


Void Log_fwr_initlib_32(act)
Analog_32_action *act;
{
  AmpRange = Fwr;
  Abs_System(act);
}





/* End. */
