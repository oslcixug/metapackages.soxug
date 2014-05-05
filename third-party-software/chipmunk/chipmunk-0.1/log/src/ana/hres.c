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




#define HRES_G
#include "hres.h"


#define N_Voffset       1
#define N_Ipos          2
#define N_Ineg          3
#define N_IKtqn         4

#define N_Leakage       5
#define N_Vt            6
#define N_Early         7
#define N_Ktqn          8
#define N_CapVlow       9
#define N_CapVhigh      10
#define N_CapVdiff      11


#define Cstart          12

/*Actual pin coordinates*/

#define H_Vin           1
#define H_Vout          2
#define H_Vlow          3
#define H_Vhigh         4

#define Gatename        "Horizontal Resistor"



typedef struct HresConst {

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

  double TauVlow, TauVhigh, TauVdiff;

  boolean Limit, OldLimit, NewLimit;

  /*For every 'state variable'*/
  /*OldState:Statetype;*/
  /*Newstate:Statetype;*/
  /*State:statetype;*/
} HresConst;


typedef struct ICHres {
  double InitVoffset, InitIpos, InitIneg, InitIKtqn;

  double InitLeakage, InitVt, InitEarly, InitKtqn, InitCapVlow, InitCapVhigh,
	 InitCapVdiff;

  double InitCgbVin, InitCgbVout, InitCdbVlow, InitCgbVhigh;
} ICHres;


typedef enum {
  Positive, Negative, Zero
} Deltasign;


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
  LLy = 11;
  URy = 7;
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

Local Void GetCnf_Hres(NewIC)
ICHres *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICHres *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("HRES", Arg, &Found);
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
      if (!strcmp(Keyword, "CAPVLOW")) {
	TRY(try9);
	  WITH->InitCapVlow = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try9);
	  WITH->InitCapVlow = 10e-15;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "CAPVHIGH")) {
	TRY(try10);
	  WITH->InitCapVhigh = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try10);
	  WITH->InitCapVhigh = 10e-15;
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
      if (!strcmp(Keyword, "CGBVIN")) {
	TRY(try12);
	  WITH->InitCgbVin = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try12);
	  WITH->InitCgbVin = 10e-15;
	ENDTRY(try12);
      }
      if (!strcmp(Keyword, "CGBVOUT")) {
	TRY(try13);
	  WITH->InitCgbVout = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try13);
	  WITH->InitCgbVout = 10e-15;
	ENDTRY(try13);
      }
      if (!strcmp(Keyword, "CDBVLOW")) {
	TRY(try14);
	  WITH->InitCdbVlow = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try14);
	  WITH->InitCdbVlow = 10e-15;
	ENDTRY(try14);
      }
      if (!strcmp(Keyword, "CGBVHIGH")) {
	TRY(try15);
	  WITH->InitCgbVhigh = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try15);
	  WITH->InitCgbVhigh = 10e-15;
	ENDTRY(try15);
      }
    }
  } while (Found);
}


Local Void INITLIB_Hres(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICHres *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[H_Vin - 1].standalone = false;
  NewCell->plist_cell[H_Vin - 1].active = true;
  NewCell->plist_cell[H_Vout - 1].standalone = false;
  NewCell->plist_cell[H_Vout - 1].active = true;
  NewCell->plist_cell[H_Vlow - 1].standalone = false;
  NewCell->plist_cell[H_Vlow - 1].active = true;
  NewCell->plist_cell[H_Vhigh - 1].standalone = false;
  NewCell->plist_cell[H_Vhigh - 1].active = true;
  NewIC = (ICHres *)Malloc(sizeof(ICHres));
  NewIC->InitVoffset = 0.0;
  NewIC->InitIpos = 1.0;
  NewIC->InitIneg = 1.0;
  NewIC->InitIKtqn = 0.04;
  NewIC->InitLeakage = 5e-17;
  NewIC->InitVt = 0.9;
  NewIC->InitEarly = 50.0;
  NewIC->InitKtqn = 0.04;
  NewIC->InitCapVlow = 10e-15;
  NewIC->InitCapVhigh = 10e-15;
  NewIC->InitCapVdiff = 10e-15;
  NewIC->InitCgbVin = 10e-15;
  NewIC->InitCgbVout = 10e-15;
  NewIC->InitCdbVlow = 10e-15;
  NewIC->InitCgbVhigh = 10e-15;
  GetCnf_Hres(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Hres_Initlib*/


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


Local boolean Hrescheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  HresConst *HresVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Voffset:
    if (fabs(Inst->attr[Attrnum - 1].UU.r) > AnaVdd || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      HresVar->HalfOffset = Inst->attr[Attrnum - 1].UU.r / 2;
    break;

  case N_Ipos:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Ineg:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_IKtqn:
    if ((unsigned)Inst->attr[N_IKtqn - 1].UU.r > 1 || Inst->attr[N_IKtqn - 1].blnk)
      Result = false;
    else {
      HresVar->IV0 = Inst->attr[N_IKtqn - 1].UU.r;
      HresVar->IKn2 = 1 / HresVar->IV0;
      HresVar->HalfIKn2 = HresVar->IKn2 / 2;
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else {
      HresVar->Kn4 = HresVar->Kn1 *
		     AnaExpo(HresVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      HresVar->Kp4 = HresVar->Kn4 / 3;
      HresVar->TwoKn4 = 2 * HresVar->Kn4;
      HresVar->TwoKp4 = 2 * HresVar->Kp4;
      HresVar->Kn5 = AnaExpo(-HresVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Leakage:
    if ((unsigned)Inst->attr[N_Leakage - 1].UU.r > 1 ||
	Inst->attr[N_Leakage - 1].blnk)
      Result = false;
    else {
      HresVar->Kn1 = Inst->attr[N_Leakage - 1].UU.r;
      HresVar->Kp1 = HresVar->Kn1 / 3;
      HresVar->Kn4 = HresVar->Kn1 *
		     AnaExpo(HresVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      HresVar->Kp4 = HresVar->Kn4 / 3;
      HresVar->TwoKn4 = 2 * HresVar->Kn4;
      HresVar->TwoKp4 = 2 * HresVar->Kp4;
    }
    break;

  case N_Early:
    if (Inst->attr[N_Early - 1].UU.r <= 0 ||
	Inst->attr[N_Early - 1].UU.r > AnaVTooBig)
      Result = false;
    else {
      if (Inst->attr[N_Early - 1].blnk)
	HresVar->InvVcross = 0.0;
      else
	HresVar->InvVcross = 1 / Inst->attr[N_Early - 1].UU.r;
    }
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[N_Ktqn - 1].UU.r > 1 || Inst->attr[N_Ktqn - 1].blnk)
      Result = false;
    else {
      HresVar->V0 = Inst->attr[N_Ktqn - 1].UU.r;
      HresVar->Kn2 = 1 / HresVar->V0;
      HresVar->Kn4 = HresVar->Kn1 *
		     AnaExpo(HresVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      HresVar->Kp4 = HresVar->Kn4 / 3;
      HresVar->TwoKn4 = 2 * HresVar->Kn4;
      HresVar->TwoKp4 = 2 * HresVar->Kp4;
      HresVar->Kn5 = AnaExpo(-HresVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;
  }
  return Result;

  /*Recompute all internal variables that depend on it*/
  /*Recompute all internal variables that depend on it*/
}


Local Void Attr_Hres(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 11) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 && ((1L << Attrnum) &
	  ((1L << N_CapVlow) | (1L << N_CapVhigh) | (1L << N_CapVdiff))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = Hrescheck(act->inst, Attrnum);
}


Local Void Newgate_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICHres *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICHres *)Cellptr->Ainfo;
  HresVar = (HresConst *)Malloc(sizeof(HresConst));
  HresVar->HalfOffset = ICptr->InitVoffset / 2;
  HresVar->IV0 = ICptr->InitIKtqn;
  HresVar->IKn2 = 1 / HresVar->IV0;
  HresVar->HalfIKn2 = HresVar->IKn2 / 2;
  HresVar->V0 = ICptr->InitKtqn;   /*kt/qn*/
  HresVar->Kn1 = ICptr->InitLeakage;   /*nfet diode leakage current*1*/
  HresVar->Kp1 = HresVar->Kn1 / 3;   /*pfet diode leakage current*1*/
  HresVar->Kn2 = 1 / HresVar->V0;   /*qn/kt*/
  HresVar->Kn4 = HresVar->Kn1 * AnaExpo(HresVar->Kn2 * ICptr->InitVt) /
		 (ICptr->InitVt * ICptr->InitVt);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  HresVar->Kp4 = HresVar->Kn4 / 3;   /*p equivalent*/
  HresVar->TwoKn4 = 2 * HresVar->Kn4;   /*2*Kn4*/
  HresVar->TwoKp4 = 2 * HresVar->Kp4;   /*2*Kp4*/
  HresVar->Kn5 = AnaExpo(-HresVar->Kn2 * ICptr->InitVt);   /*Expo(-Kn2*Vtn)*/
  if (ICptr->InitEarly == 0)
    HresVar->InvVcross = 0.0;
  else
    HresVar->InvVcross = 1 / ICptr->InitEarly;
  HresVar->Limit = false;
  HresVar->OldLimit = false;
  HresVar->NewLimit = false;
  /*Give internal variables values derived from initial attribute values*/
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)HresVar;
  AnaCapInit(act->inst);   /*Initialize attributes of Cstart*/
  A_Gate->Pininfo[H_Vin - 1].Cap = ICptr->InitCgbVin;
  A_Gate->Pininfo[H_Vout - 1].Cap = ICptr->InitCgbVout;
  A_Gate->Pininfo[H_Vlow - 1].Cap = ICptr->InitCdbVlow;
  A_Gate->Pininfo[H_Vhigh - 1].Cap = ICptr->InitCgbVhigh;
  WITH = act->inst;
  WITH->attr[N_Voffset - 1].UU.r = ICptr->InitVoffset;
  WITH->attr[N_Voffset - 1].blnk = false;
  WITH->attr[N_Voffset - 1].changed = true;
  WITH->attr[N_Ipos - 1].UU.r = ICptr->InitIpos;
  WITH->attr[N_Ipos - 1].blnk = false;
  WITH->attr[N_Ipos - 1].changed = true;
  WITH->attr[N_Ineg - 1].UU.r = ICptr->InitIneg;
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
  WITH->attr[N_CapVlow - 1].UU.r = ICptr->InitCapVlow;
  WITH->attr[N_CapVlow - 1].blnk = false;
  WITH->attr[N_CapVlow - 1].changed = true;
  WITH->attr[N_CapVhigh - 1].UU.r = ICptr->InitCapVhigh;
  WITH->attr[N_CapVhigh - 1].blnk = false;
  WITH->attr[N_CapVhigh - 1].changed = true;
  WITH->attr[N_CapVdiff - 1].UU.r = ICptr->InitCapVdiff;
  WITH->attr[N_CapVdiff - 1].blnk = false;
  WITH->attr[N_CapVdiff - 1].changed = true;
}

Local Void Copygate_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar, *Old_Hresvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Hresvar = (HresConst *)A_Oldgate->InstVar;
  HresVar = (HresConst *)Malloc(sizeof(HresConst));
  *HresVar = *Old_Hresvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)HresVar;
  AnaCapCopy(act->inst);
}


Local Void Dispose_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  Free(HresVar);
  AnaCapDispose(act->inst);
}


Local Void Readgate_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICHres *ICptr;
  log_grec *WITH;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICHres *)Cellptr->Ainfo;
    A_Gate->Pininfo[H_Vin - 1].Cap = ICptr->InitCgbVin;
    A_Gate->Pininfo[H_Vout - 1].Cap = ICptr->InitCgbVout;
    A_Gate->Pininfo[H_Vlow - 1].Cap = ICptr->InitCdbVlow;
    A_Gate->Pininfo[H_Vhigh - 1].Cap = ICptr->InitCgbVhigh;
    WITH = act->inst;
    WITH->attr[N_Voffset - 1].UU.r = ICptr->InitVoffset;
    WITH->attr[N_Voffset - 1].blnk = false;
    WITH->attr[N_Voffset - 1].changed = true;
    WITH->attr[N_Ipos - 1].UU.r = ICptr->InitIpos;
    WITH->attr[N_Ipos - 1].blnk = false;
    WITH->attr[N_Ipos - 1].changed = true;
    WITH->attr[N_Ineg - 1].UU.r = ICptr->InitIneg;
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
    WITH->attr[N_CapVlow - 1].UU.r = ICptr->InitCapVlow;
    WITH->attr[N_CapVlow - 1].blnk = false;
    WITH->attr[N_CapVlow - 1].changed = true;
    WITH->attr[N_CapVhigh - 1].UU.r = ICptr->InitCapVhigh;
    WITH->attr[N_CapVhigh - 1].blnk = false;
    WITH->attr[N_CapVhigh - 1].changed = true;
    WITH->attr[N_CapVdiff - 1].UU.r = ICptr->InitCapVdiff;
    WITH->attr[N_CapVdiff - 1].blnk = false;
    WITH->attr[N_CapVdiff - 1].changed = true;
  }
  WITH = act->inst;
  HresVar->HalfOffset = WITH->attr[N_Voffset - 1].UU.r / 2;
  HresVar->IV0 = WITH->attr[N_IKtqn - 1].UU.r;   /*kt/qn for tanh*/
  HresVar->IKn2 = 1 / HresVar->IV0;   /*qn/kt for tanh*/
  HresVar->HalfIKn2 = HresVar->IKn2 / 2;
  HresVar->V0 = WITH->attr[N_Ktqn - 1].UU.r;   /*kt/qn*/
  HresVar->Kn1 = WITH->attr[N_Leakage - 1].UU.r;
      /*n diode leakage current*(W/L)*/
  HresVar->Kp1 = HresVar->Kn1 / 3;   /*p diode leakage current*(W/L)*/
  HresVar->Kn2 = 1 / HresVar->V0;   /*qn/kt*/
  HresVar->Kn4 = HresVar->Kn1 * AnaExpo(HresVar->Kn2 * WITH->attr[N_Vt - 1].UU.r) /
		 (WITH->attr[N_Vt - 1].UU.r * WITH->attr[N_Vt - 1].UU.r);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  HresVar->Kp4 = HresVar->Kn4 / 3;   /*p equivalent*/
  HresVar->TwoKn4 = 2 * HresVar->Kn4;   /*2*Kn4*/
  HresVar->TwoKp4 = 2 * HresVar->Kp4;   /*2*Kp4*/
  HresVar->Kn5 = AnaExpo(-HresVar->Kn2 * WITH->attr[N_Vt - 1].UU.r);
      /*Expo(-Kn2*Vtn)*/
  if (WITH->attr[N_Early - 1].blnk)
    HresVar->InvVcross = 0.0;
  else
    HresVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
}


Local Void Reset_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  HresVar->Limit = false;
  HresVar->OldLimit = false;
  HresVar->NewLimit = false;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, HresVar->Limit, false);
}

Local Void PreUpdate_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;
  AnaExt_rec *PinInPtr;
  double VPinIn;
  AnaExt_rec *PinOutPtr;
  double VPinOut;
  AnaExt_rec *PinLowPtr;
  double VPinLow;
  AnaExt_rec *PinHighPtr;
  double VPinHigh;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  PinInPtr = (AnaExt_rec *)WITH1->pin[H_Vin - 1]->info;
  VPinIn = PinInPtr->last;
  PinOutPtr = (AnaExt_rec *)WITH1->pin[H_Vout - 1]->info;
  VPinOut = PinOutPtr->last;
  PinLowPtr = (AnaExt_rec *)WITH1->pin[H_Vlow - 1]->info;
  VPinLow = PinLowPtr->last;
  PinHighPtr = (AnaExt_rec *)WITH1->pin[H_Vhigh - 1]->info;
  VPinHigh = PinHighPtr->last;
  HresVar->NewLimit = (VPinLow >= WITH1->attr[N_Vt - 1].UU.r ||
		       AnaVdd - VPinHigh >= WITH1->attr[N_Vt - 1].UU.r);
}


Local Void Update_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  HresVar->OldLimit = HresVar->Limit;
  HresVar->Limit = HresVar->NewLimit;
}


Local Void Display_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (HresVar->OldLimit != HresVar->Limit)
      Limitfill(act->inst, HresVar->Limit, false);
  }
}


Local Void Refresh_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  Limitfill(act->inst, HresVar->Limit, false);
}



Local Void Erase_Hres(act)
Analog_32_action *act;
{
  HresConst *HresVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  Limitfill(act->inst, HresVar->Limit, true);
}


/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_Hres(act)
Analog_32_action *act;
{
  double MinIin, dIindVin, dIindVout, dIindVlow, dIindVhigh, MinIout,
	 dIoutdVin, dIoutdVout, dIoutdVlow, dIoutdVhigh, MinIlow, dIlowdVin,
	 dIlowdVout, dIlowdVlow, dIlowdVhigh, MinIhigh, dIhighdVin,
	 dIhighdVout, dIhighdVlow, dIhighdVhigh;
  AnaExt_rec *PinInPtr;
  long PinIn;
  double VPinIn, VMemIn;
  AnaExt_rec *PinOutPtr;
  long PinOut;
  double VPinOut, VMemOut;
  AnaExt_rec *PinLowPtr;
  long PinLow;
  double VPinLow, VMemLow;
  AnaExt_rec *PinHighPtr;
  long PinHigh;
  double VPinHigh, VMemHigh;
  HresConst *HresVar;
  Anainstlist *A_Gate;
  double Icap, DeltaVin, DeltaVout, DeltaVlow, DeltaVhigh;
  Deltasign Sign;
  double VInOffset, VOutOffset, Del, ExpoDel, BlendDel, InvBlendDel,
	 dBlendDeldVout, MaxVinVout, dMaxVinVoutdVout, dMaxVinVoutdVin,
	 MinVinVout, dMinVinVoutdVin, dMinVinVoutdVout, ExpoBot, BlendBot,
	 dBlendBotdVlow, dBlendBotdVin, dBlendBotdVout, ExpoTop, BlendTop,
	 dBlendTopdVhigh, dBlendTopdVin, dBlendTopdVout, ExpoVlow,
	 Kn5_ExpoVlow, BlendVlow, InvBlendVlow, dBlendVlowdVlow, ExpoVhigh,
	 Kn5_ExpoVhigh, BlendVhigh, InvBlendVhigh, dBlendVhighdVhigh,
	 Kn1_ExpoVlow, Kn1_ExpoVlow_BlendVlow, Kn4_Vlow_Vlow, PureLow,
	 dPureLowdVlow, pVhigh, Kp1_ExpoVhigh, Kp1_ExpoVhigh_BlendVhigh,
	 Kp4_pVhigh_pVhigh, PureHigh, dPureHighdVhigh, Ineg_PureLow, Isetlow,
	 dIsetlowdVlow, dIsetlowdVin, dIsetlowdVout, Ipos_PureHigh, Isethigh,
	 dIsethighdVhigh, dIsethighdVin, dIsethighdVout, HresTanch,
	 dHresTanchdVin, Vdd_InvVcross, Vin_InvVcross, Vout_InvVcross,
	 ResVinHigh, ResVinLow, ResVoutHigh, ResVoutLow,
	 InvBlendDel_ResVinLow, BlendDel_ResVinHigh, Isetlow_ResVinLow,
	 IsetHigh_ResVinHigh, dBlendDeldVout_Isetlow_ResVinLow,
	 dBlendDeldVout_IsetHigh_ResVinHigh, IsetHigh_InvVcross,
	 Isetlow_InvVcross, IinRes, dIinResdVin, dIinResdVout, dIinResdVlow,
	 dIinResdVhigh, BlendDel_ResVoutLow, InvBlendDel_ResVoutHigh,
	 IsetHigh_ResVoutHigh, Isetlow_ResVoutLow,
	 dBlendDeldVout_IsetHigh_ResVoutHigh,
	 dBlendDeldVout_Isetlow_ResVoutLow, IoutRes, dIoutResdVin,
	 dIoutResdVout, dIoutResdVlow, dIoutResdVhigh;
/* p2c: stdlib.text, line 3079: Note:
 * Line breaker spent 2.0+1.00 seconds, 5000 tries on line 790 [251] */
  log_grec *WITH;
  AnaMatRec *WITH2;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  PinInPtr = (AnaExt_rec *)WITH->pin[H_Vin - 1]->info;
  PinIn = PinInPtr->nodenum;
  VPinIn = PinInPtr->last;
  VMemIn = A_Gate->Pininfo[H_Vin - 1].Vmem;
  PinOutPtr = (AnaExt_rec *)WITH->pin[H_Vout - 1]->info;
  PinOut = PinOutPtr->nodenum;
  VPinOut = PinOutPtr->last;
  VMemOut = A_Gate->Pininfo[H_Vout - 1].Vmem;
  PinLowPtr = (AnaExt_rec *)WITH->pin[H_Vlow - 1]->info;
  PinLow = PinLowPtr->nodenum;
  VPinLow = PinLowPtr->last;
  VMemLow = A_Gate->Pininfo[H_Vlow - 1].Vmem;
  PinHighPtr = (AnaExt_rec *)WITH->pin[H_Vhigh - 1]->info;
  PinHigh = PinHighPtr->nodenum;
  VPinHigh = PinHighPtr->last;
  VMemHigh = A_Gate->Pininfo[H_Vhigh - 1].Vmem;
      /*Use with statement to get to internal variables*/

  VInOffset = VPinIn + HresVar->HalfOffset;
  VOutOffset = VPinOut - HresVar->HalfOffset;
  Del = VInOffset - VOutOffset;
  if (fabs(Del) < 0.000708) {
    ExpoDel = AnaExpo(100000L * Del);
    BlendDel = 1 / (1 + ExpoDel);
    InvBlendDel = 1 - BlendDel;
    dBlendDeldVout = 100000L * ExpoDel * BlendDel * BlendDel;
    MaxVinVout = VOutOffset * BlendDel + VInOffset * InvBlendDel;
    dMaxVinVoutdVout = BlendDel - Del * dBlendDeldVout;
    dMaxVinVoutdVin = 1 - dMaxVinVoutdVout;
    MinVinVout = VInOffset * BlendDel + VOutOffset * InvBlendDel;
    dMinVinVoutdVin = dMaxVinVoutdVout;
    dMinVinVoutdVout = dMaxVinVoutdVin;
    Sign = Zero;
  } else {
    if (Del > 0) {
      BlendDel = 0.0;
      InvBlendDel = 1.0;
      dBlendDeldVout = 0.0;
      MaxVinVout = VInOffset;
      dMaxVinVoutdVout = 0.0;
      dMaxVinVoutdVin = 1.0;
      MinVinVout = VOutOffset;
      dMinVinVoutdVin = 0.0;
      dMinVinVoutdVout = 1.0;
      Sign = Positive;
    } else {
      BlendDel = 1.0;
      InvBlendDel = 0.0;
      dBlendDeldVout = 0.0;
      MaxVinVout = VOutOffset;
      dMaxVinVoutdVout = 1.0;
      dMaxVinVoutdVin = 0.0;
      MinVinVout = VInOffset;
      dMinVinVoutdVin = 1.0;
      dMinVinVoutdVout = 0.0;
      Sign = Negative;
    }
  }






  ExpoVlow = AnaExpo(HresVar->Kn2 * VPinLow);
  Kn5_ExpoVlow = HresVar->Kn5 * ExpoVlow;
  BlendVlow = 1 / (1 + Kn5_ExpoVlow);
  InvBlendVlow = 1 - BlendVlow;
  dBlendVlowdVlow = -HresVar->Kn2 * BlendVlow * BlendVlow * Kn5_ExpoVlow;

  Kn1_ExpoVlow = HresVar->Kn1 * ExpoVlow;
  Kn1_ExpoVlow_BlendVlow = Kn1_ExpoVlow * BlendVlow;
  Kn4_Vlow_Vlow = HresVar->Kn4 * VPinLow * VPinLow;
  PureLow = Kn1_ExpoVlow_BlendVlow + InvBlendVlow * Kn4_Vlow_Vlow;
  dPureLowdVlow = HresVar->Kn2 * Kn1_ExpoVlow_BlendVlow +
		  dBlendVlowdVlow * (Kn1_ExpoVlow - Kn4_Vlow_Vlow) +
		  InvBlendVlow * HresVar->TwoKn4 * VPinLow;



  ExpoVhigh = AnaExpo(HresVar->Kn2 * (AnaVdd - VPinHigh));
  Kn5_ExpoVhigh = HresVar->Kn5 * ExpoVhigh;
  BlendVhigh = 1 / (1 + Kn5_ExpoVhigh);
  InvBlendVhigh = 1 - BlendVhigh;
  dBlendVhighdVhigh = HresVar->Kn2 * BlendVhigh * BlendVhigh * Kn5_ExpoVhigh;

  pVhigh = AnaVdd - VPinHigh;
  Kp1_ExpoVhigh = HresVar->Kp1 * ExpoVhigh;
  Kp1_ExpoVhigh_BlendVhigh = Kp1_ExpoVhigh * BlendVhigh;
  Kp4_pVhigh_pVhigh = HresVar->Kp4 * pVhigh * pVhigh;
  PureHigh = Kp1_ExpoVhigh_BlendVhigh + InvBlendVhigh * Kp4_pVhigh_pVhigh;
  dPureHighdVhigh = dBlendVhighdVhigh * (Kp1_ExpoVhigh - Kp4_pVhigh_pVhigh) -
		    HresVar->Kn2 * Kp1_ExpoVhigh_BlendVhigh -
		    InvBlendVhigh * HresVar->TwoKp4 * pVhigh;



  ExpoBot = AnaExpo(HresVar->Kn2 * (VPinLow - MaxVinVout));
  BlendBot = 1 / (1 + ExpoBot);
  dBlendBotdVlow = -HresVar->Kn2 * BlendBot * BlendBot * ExpoBot;
  dBlendBotdVin = -dBlendBotdVlow * dMaxVinVoutdVin;
  dBlendBotdVout = -dBlendBotdVlow * dMaxVinVoutdVout;

  Ineg_PureLow = WITH->attr[N_Ineg - 1].UU.r * PureLow;
  Isetlow = BlendBot * Ineg_PureLow;
  dIsetlowdVlow = Ineg_PureLow * dBlendBotdVlow +
		  WITH->attr[N_Ineg - 1].UU.r * BlendBot * dPureLowdVlow;
  dIsetlowdVin = Ineg_PureLow * dBlendBotdVin;
  dIsetlowdVout = Ineg_PureLow * dBlendBotdVout;



  ExpoTop = AnaExpo(HresVar->Kn2 * (MinVinVout - VPinHigh));
  BlendTop = 1 / (1 + ExpoTop);
  dBlendTopdVhigh = HresVar->Kn2 * BlendTop * BlendTop * ExpoTop;
  dBlendTopdVin = -dBlendTopdVhigh * dMinVinVoutdVin;
  dBlendTopdVout = -dBlendTopdVhigh * dMinVinVoutdVout;

  Ipos_PureHigh = WITH->attr[N_Ipos - 1].UU.r * PureHigh;
  Isethigh = BlendTop * Ipos_PureHigh;
  dIsethighdVhigh = Ipos_PureHigh * dBlendTopdVhigh +
		    WITH->attr[N_Ipos - 1].UU.r * BlendTop * dPureHighdVhigh;
  dIsethighdVin = Ipos_PureHigh * dBlendTopdVin;
  dIsethighdVout = Ipos_PureHigh * dBlendTopdVout;



  HresTanch = AnaTanch(Del * HresVar->HalfIKn2);
  dHresTanchdVin = (1 - HresTanch * HresTanch) * HresVar->HalfIKn2;


  Vdd_InvVcross = AnaVdd * HresVar->InvVcross;
  Vin_InvVcross = VPinIn * HresVar->InvVcross;
  Vout_InvVcross = VPinOut * HresVar->InvVcross;
  ResVinHigh = 1 + Vdd_InvVcross - Vin_InvVcross;
  ResVinLow = 1 + Vin_InvVcross;
  ResVoutHigh = 1 + Vdd_InvVcross - Vout_InvVcross;
  ResVoutLow = 1 + Vout_InvVcross;

  InvBlendDel_ResVinLow = InvBlendDel * ResVinLow;
  BlendDel_ResVinHigh = BlendDel * ResVinHigh;
  Isetlow_ResVinLow = Isetlow * ResVinLow;
  IsetHigh_ResVinHigh = Isethigh * ResVinHigh;
  dBlendDeldVout_Isetlow_ResVinLow = dBlendDeldVout * Isetlow_ResVinLow;
  dBlendDeldVout_IsetHigh_ResVinHigh = dBlendDeldVout * IsetHigh_ResVinHigh;
  IsetHigh_InvVcross = Isethigh * HresVar->InvVcross;
  Isetlow_InvVcross = Isetlow * HresVar->InvVcross;

  IinRes = InvBlendDel_ResVinLow * Isetlow + BlendDel_ResVinHigh * Isethigh;
  dIinResdVin =
    dBlendDeldVout_Isetlow_ResVinLow + InvBlendDel_ResVinLow * dIsetlowdVin +
    InvBlendDel * Isetlow_InvVcross - dBlendDeldVout_IsetHigh_ResVinHigh +
    BlendDel_ResVinHigh * dIsethighdVin - BlendDel * IsetHigh_InvVcross;
  dIinResdVout =
    InvBlendDel_ResVinLow * dIsetlowdVout - dBlendDeldVout_Isetlow_ResVinLow +
    dBlendDeldVout_IsetHigh_ResVinHigh + BlendDel_ResVinHigh * dIsethighdVout;
  dIinResdVlow = InvBlendDel_ResVinLow * dIsetlowdVlow;
  dIinResdVhigh = BlendDel_ResVinHigh * dIsethighdVhigh;



  BlendDel_ResVoutLow = BlendDel * ResVoutLow;
  InvBlendDel_ResVoutHigh = InvBlendDel * ResVoutHigh;
  IsetHigh_ResVoutHigh = Isethigh * ResVoutHigh;
  Isetlow_ResVoutLow = Isetlow * ResVoutLow;
  dBlendDeldVout_IsetHigh_ResVoutHigh = dBlendDeldVout * IsetHigh_ResVoutHigh;
  dBlendDeldVout_Isetlow_ResVoutLow = dBlendDeldVout * Isetlow_ResVoutLow;

  IoutRes = InvBlendDel_ResVoutHigh * Isethigh + BlendDel_ResVoutLow * Isetlow;
  dIoutResdVin = dBlendDeldVout_IsetHigh_ResVoutHigh +
      InvBlendDel_ResVoutHigh * dIsethighdVin -
      dBlendDeldVout_Isetlow_ResVoutLow + BlendDel_ResVoutLow * dIsetlowdVin;
  dIoutResdVout = InvBlendDel_ResVoutHigh * dIsethighdVout -
      dBlendDeldVout_IsetHigh_ResVoutHigh - InvBlendDel * IsetHigh_InvVcross +
      dBlendDeldVout_Isetlow_ResVoutLow +
      BlendDel_ResVoutLow * dIsetlowdVout + BlendDel * Isetlow_InvVcross;
  dIoutResdVlow = BlendDel_ResVoutLow * dIsetlowdVlow;
  dIoutResdVhigh = InvBlendDel_ResVoutHigh * dIsethighdVhigh;

  MinIin = -IinRes * HresTanch;
  MinIout = IoutRes * HresTanch;
  dIindVin = dIinResdVin * HresTanch + IinRes * dHresTanchdVin;
  dIindVout = dIinResdVout * HresTanch - IinRes * dHresTanchdVin;
  dIindVlow = dIinResdVlow * HresTanch;
  dIindVhigh = dIinResdVhigh * HresTanch;
  dIoutdVin = -dIoutResdVin * HresTanch - IoutRes * dHresTanchdVin;
  dIoutdVout = IoutRes * dHresTanchdVin - dIoutResdVout * HresTanch;
  dIoutdVlow = -dIoutResdVlow * HresTanch;
  dIoutdVhigh = -dIoutResdVhigh * HresTanch;

  DeltaVin = VPinIn - VMemIn;
  DeltaVout = VPinOut - VMemOut;
  DeltaVlow = VPinLow - VMemLow;
  DeltaVhigh = VPinHigh - VMemHigh;

  if (AnaChangedDt)
    HresVar->TauVlow = WITH->attr[N_CapVlow - 1].UU.r / AnaDt;
  Icap = HresVar->TauVlow * (DeltaVlow - DeltaVin);
  MinIlow = -Icap;
  dIlowdVin = -HresVar->TauVlow;
  dIlowdVlow = HresVar->TauVlow;
  MinIin += Icap;
  dIindVin += HresVar->TauVlow;
  dIindVlow -= HresVar->TauVlow;
  Icap = HresVar->TauVlow * (DeltaVlow - DeltaVout);
  MinIlow -= Icap;
  dIlowdVout = -HresVar->TauVlow;
  dIlowdVlow += HresVar->TauVlow;
  MinIout += Icap;
  dIoutdVout += HresVar->TauVlow;
  dIoutdVlow -= HresVar->TauVlow;

  if (AnaChangedDt)
    HresVar->TauVhigh = WITH->attr[N_CapVhigh - 1].UU.r / AnaDt;
  Icap = HresVar->TauVhigh * (DeltaVhigh - DeltaVin);
  MinIhigh = -Icap;
  dIhighdVin = -HresVar->TauVhigh;
  dIhighdVhigh = HresVar->TauVhigh;
  MinIin += Icap;
  dIindVin += HresVar->TauVhigh;
  dIindVhigh -= HresVar->TauVhigh;
  Icap = HresVar->TauVhigh * (DeltaVhigh - DeltaVout);
  MinIhigh -= Icap;
  dIhighdVout = -HresVar->TauVhigh;
  dIhighdVhigh += HresVar->TauVhigh;
  MinIout += Icap;
  dIoutdVout += HresVar->TauVhigh;
  dIoutdVhigh -= HresVar->TauVhigh;

  if (AnaChangedDt)
    HresVar->TauVdiff = WITH->attr[N_CapVdiff - 1].UU.r / AnaDt;
  Icap = HresVar->TauVdiff * (DeltaVin - DeltaVout);
  MinIin -= Icap;
  dIindVout -= HresVar->TauVdiff;
  dIindVin += HresVar->TauVdiff;
  MinIout += Icap;
  dIoutdVout += HresVar->TauVdiff;
  dIoutdVin -= HresVar->TauVdiff;
  Icap = HresVar->TauVdiff * (DeltaVlow - DeltaVhigh);
  MinIlow -= Icap;
  dIlowdVhigh = -HresVar->TauVdiff;
  dIlowdVlow += HresVar->TauVdiff;
  MinIhigh += Icap;
  dIhighdVhigh += HresVar->TauVdiff;
  dIhighdVlow = -HresVar->TauVdiff;


  WITH2 = AnaSystem;
  WITH2->Mat[PinIn][AnaSysCol] += MinIin;
  WITH2->Mat[PinIn][PinIn] += dIindVin;
  WITH2->Mat[PinIn][PinOut] += dIindVout;
  WITH2->Mat[PinIn][PinLow] += dIindVlow;
  WITH2->Mat[PinIn][PinHigh] += dIindVhigh;
  WITH2->Mat[PinOut][AnaSysCol] += MinIout;
  WITH2->Mat[PinOut][PinIn] += dIoutdVin;
  WITH2->Mat[PinOut][PinOut] += dIoutdVout;
  WITH2->Mat[PinOut][PinLow] += dIoutdVlow;
  WITH2->Mat[PinOut][PinHigh] += dIoutdVhigh;
  WITH2->Mat[PinLow][AnaSysCol] += MinIlow;
  WITH2->Mat[PinLow][PinIn] += dIlowdVin;
  WITH2->Mat[PinLow][PinOut] += dIlowdVout;
  WITH2->Mat[PinLow][PinLow] += dIlowdVlow;
  WITH2->Mat[PinLow][PinHigh] += dIlowdVhigh;
  WITH2->Mat[PinHigh][AnaSysCol] += MinIhigh;
  WITH2->Mat[PinHigh][PinIn] += dIhighdVin;
  WITH2->Mat[PinHigh][PinOut] += dIhighdVout;
  WITH2->Mat[PinHigh][PinLow] += dIhighdVlow;
  WITH2->Mat[PinHigh][PinHigh] += dIhighdVhigh;
  WITH2->MatNZ[PinIn][AnaSysCol] = true;
  WITH2->MatNZ[PinIn][PinIn] = true;
  WITH2->MatNZ[PinIn][PinOut] = true;
  WITH2->MatNZ[PinIn][PinLow] = true;
  WITH2->MatNZ[PinIn][PinHigh] = true;
  WITH2->MatNZ[PinOut][AnaSysCol] = true;
  WITH2->MatNZ[PinOut][PinIn] = true;
  WITH2->MatNZ[PinOut][PinOut] = true;
  WITH2->MatNZ[PinOut][PinLow] = true;
  WITH2->MatNZ[PinOut][PinHigh] = true;
  WITH2->MatNZ[PinLow][AnaSysCol] = true;
  WITH2->MatNZ[PinLow][PinIn] = true;
  WITH2->MatNZ[PinLow][PinOut] = true;
  WITH2->MatNZ[PinLow][PinLow] = true;
  WITH2->MatNZ[PinLow][PinHigh] = true;
  WITH2->MatNZ[PinHigh][AnaSysCol] = true;
  WITH2->MatNZ[PinHigh][PinIn] = true;
  WITH2->MatNZ[PinHigh][PinOut] = true;
  WITH2->MatNZ[PinHigh][PinLow] = true;
  WITH2->MatNZ[PinHigh][PinHigh] = true;
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_Hres*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Hres(act)
Analog_32_action *act;
{
  AnaExt_rec *PinInPtr;
  double VPinIn, VMemIn;
  AnaExt_rec *PinOutPtr;
  double VPinOut, VMemOut;
  AnaExt_rec *PinLowPtr;
  double VPinLow, VMemLow;
  AnaExt_rec *PinHighPtr;
  double VPinHigh, VMemHigh;
  HresConst *HresVar;
  Anainstlist *A_Gate;
  double DeltaVin, DeltaVout, DeltaVlow, DeltaVhigh;
  Deltasign Sign;
  double VInOffset, VOutOffset, Del, BlendDel, InvBlendDel, MaxVinVout,
	 MinVinVout, ExpoVlow, BlendVlow, PureLow, ExpoVhigh, BlendVhigh,
	 pVhigh, PureHigh, Isetlow, Isethigh;
  log_grec *WITH;


  WITH = act->inst;   /*Use Dtlast, Dtlast+Accum for time values*/
  A_Gate = (Anainstlist *)WITH->info;
  HresVar = (HresConst *)A_Gate->InstVar;
  PinInPtr = (AnaExt_rec *)WITH->pin[H_Vin - 1]->info;
  VPinIn = PinInPtr->now;
  VMemIn = A_Gate->Pininfo[H_Vin - 1].Vmem;
  PinOutPtr = (AnaExt_rec *)WITH->pin[H_Vout - 1]->info;
  VPinOut = PinOutPtr->now;
  VMemOut = A_Gate->Pininfo[H_Vout - 1].Vmem;
  PinLowPtr = (AnaExt_rec *)WITH->pin[H_Vlow - 1]->info;
  VPinLow = PinLowPtr->now;
  VMemLow = A_Gate->Pininfo[H_Vlow - 1].Vmem;
  PinHighPtr = (AnaExt_rec *)WITH->pin[H_Vhigh - 1]->info;
  VPinHigh = PinHighPtr->now;
  VMemHigh = A_Gate->Pininfo[H_Vhigh - 1].Vmem;
  switch (act->pin) {

  case H_Vin:   /*Allows attr^[]. access*/
    WITH = act->inst;
    VInOffset = VPinIn + HresVar->HalfOffset;
    VOutOffset = VPinOut - HresVar->HalfOffset;
    Del = VInOffset - VOutOffset;
    if (fabs(Del) < 0.000708) {
      BlendDel = 1 / (1 + AnaExpo(100000L * Del));
      InvBlendDel = 1 - BlendDel;
      MaxVinVout = VOutOffset * BlendDel + VInOffset * InvBlendDel;
      MinVinVout = VInOffset * BlendDel + VOutOffset * InvBlendDel;
      Sign = Zero;
    } else {
      if (Del > 0) {
	BlendDel = 0.0;
	InvBlendDel = 1.0;
	MaxVinVout = VInOffset;
	MinVinVout = VOutOffset;
	Sign = Positive;
      } else {
	BlendDel = 1.0;
	InvBlendDel = 0.0;
	MaxVinVout = VOutOffset;
	MinVinVout = VInOffset;
	Sign = Negative;
      }
    }
    ExpoVlow = AnaExpo(HresVar->Kn2 * VPinLow);
    BlendVlow = 1 / (1 + HresVar->Kn5 * ExpoVlow);
    PureLow = HresVar->Kn1 * ExpoVlow * BlendVlow +
	      (1 - BlendVlow) * HresVar->Kn4 * VPinLow * VPinLow;
    ExpoVhigh = AnaExpo(HresVar->Kn2 * (AnaVdd - VPinHigh));
    BlendVhigh = 1 / (1 + HresVar->Kn5 * ExpoVhigh);
    pVhigh = AnaVdd - VPinHigh;
    PureHigh = HresVar->Kp1 * ExpoVhigh * BlendVhigh +
	       (1 - BlendVhigh) * HresVar->Kp4 * pVhigh * pVhigh;
    Isetlow = 1 / (1 + AnaExpo(HresVar->Kn2 * (VPinLow - MaxVinVout))) *
	      WITH->attr[N_Ineg - 1].UU.r * PureLow;
    Isethigh = 1 / (1 + AnaExpo(HresVar->Kn2 * (MinVinVout - VPinHigh))) *
	       WITH->attr[N_Ipos - 1].UU.r * PureHigh;

    DeltaVin = VPinIn - VMemIn;
    act->Iin =
      AnaTanch(Del * HresVar->HalfIKn2) *
      (InvBlendDel * (1 + VPinIn * HresVar->InvVcross) * Isetlow +
       BlendDel * (1 + (AnaVdd - VPinIn) * HresVar->InvVcross) * Isethigh) +
      HresVar->TauVhigh * (DeltaVin - VPinHigh + VMemHigh) +
      HresVar->TauVlow * (DeltaVin - VPinLow + VMemLow) +
      HresVar->TauVdiff * (DeltaVin - VPinOut + VMemOut) + act->Iin;
/* p2c: stdlib.text, line 3079: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 1182 [251] */
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case H_Vout:   /*Allows attr^[]. access*/
    WITH = act->inst;
    VInOffset = VPinIn + HresVar->HalfOffset;
    VOutOffset = VPinOut - HresVar->HalfOffset;
    Del = VInOffset - VOutOffset;
    if (fabs(Del) < 0.000708) {
      BlendDel = 1 / (1 + AnaExpo(100000L * Del));
      InvBlendDel = 1 - BlendDel;
      MaxVinVout = VOutOffset * BlendDel + VInOffset * InvBlendDel;
      MinVinVout = VInOffset * BlendDel + VOutOffset * InvBlendDel;
      Sign = Zero;
    } else {
      if (Del > 0) {
	BlendDel = 0.0;
	InvBlendDel = 1.0;
	MaxVinVout = VInOffset;
	MinVinVout = VOutOffset;
	Sign = Positive;
      } else {
	BlendDel = 1.0;
	InvBlendDel = 0.0;
	MaxVinVout = VOutOffset;
	MinVinVout = VInOffset;
	Sign = Negative;
      }
    }
    ExpoVlow = AnaExpo(HresVar->Kn2 * VPinLow);
    BlendVlow = 1 / (1 + HresVar->Kn5 * ExpoVlow);
    PureLow = HresVar->Kn1 * ExpoVlow * BlendVlow +
	      (1 - BlendVlow) * HresVar->Kn4 * VPinLow * VPinLow;
    ExpoVhigh = AnaExpo(HresVar->Kn2 * (AnaVdd - VPinHigh));
    BlendVhigh = 1 / (1 + HresVar->Kn5 * ExpoVhigh);
    pVhigh = AnaVdd - VPinHigh;
    PureHigh = HresVar->Kp1 * ExpoVhigh * BlendVhigh +
	       (1 - BlendVhigh) * HresVar->Kp4 * pVhigh * pVhigh;
    Isetlow = 1 / (1 + AnaExpo(HresVar->Kn2 * (VPinLow - MaxVinVout))) *
	      WITH->attr[N_Ineg - 1].UU.r * PureLow;
    Isethigh = 1 / (1 + AnaExpo(HresVar->Kn2 * (MinVinVout - VPinHigh))) *
	       WITH->attr[N_Ipos - 1].UU.r * PureHigh;
    DeltaVout = VPinOut - VMemOut;
    act->Iin = HresVar->TauVhigh * (DeltaVout - VPinHigh + VMemHigh) -
	AnaTanch(Del * HresVar->HalfIKn2) *
	(InvBlendDel * (1 + (AnaVdd - VPinOut) * HresVar->InvVcross) *
	 Isethigh + BlendDel * (1 + VPinOut * HresVar->InvVcross) * Isetlow) +
	HresVar->TauVlow * (DeltaVout - VPinLow + VMemLow) +
	HresVar->TauVdiff * (DeltaVout - VPinIn + VMemIn) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case H_Vlow:   /*Allows attr^[]. access*/
    WITH = act->inst;
    DeltaVlow = VPinLow - VMemLow;
    act->Iin = HresVar->TauVlow * (DeltaVlow - VPinIn + VMemIn) +
	HresVar->TauVlow * (DeltaVlow - VPinOut + VMemOut) +
	HresVar->TauVdiff * (DeltaVlow - VPinHigh + VMemHigh) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  case H_Vhigh:   /*Allows attr^[]. access*/
    WITH = act->inst;
    DeltaVhigh = VPinHigh - VMemHigh;
    act->Iin = HresVar->TauVhigh * (DeltaVhigh - VPinOut + VMemOut) +
	HresVar->TauVhigh * (DeltaVhigh - VPinIn + VMemIn) +
	HresVar->TauVdiff * (DeltaVhigh - VPinLow + VMemLow) + act->Iin;
    AnaCapIin(act);   /*Add nodal capacitance*/
    break;
    /*Allows internal variable access*/

  default:
    printf("Data Structure Corruption\n");
    _Escape(0);
    break;
  }
}  /*Iin_Hres*/



Local Void Probe_Hres(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *NodeInPtr, *NodeOutPtr, *NodeHighPtr, *NodeLowPtr;
  double NodeIn, NodeOut, NodeHigh, NodeLow, PinIn, PinOut, PinHigh, PinLow;
  HresConst *HresVar;
  double ExpoVhigh, BlendVhigh, Ihigh, pVhigh, ExpoVlow, BlendVlow, Ilow;
  log_grec *WITH;
  Char STR1[256];
  Char STR2[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  PinIn = A_Gate->Pininfo[H_Vin - 1].Vmem;
  PinOut = A_Gate->Pininfo[H_Vout - 1].Vmem;
  PinHigh = A_Gate->Pininfo[H_Vhigh - 1].Vmem;
  PinLow = A_Gate->Pininfo[H_Vlow - 1].Vmem;
  NodeInPtr = (AnaExt_rec *)WITH->pin[H_Vin - 1]->info;
  NodeOutPtr = (AnaExt_rec *)WITH->pin[H_Vout - 1]->info;
  NodeHighPtr = (AnaExt_rec *)WITH->pin[H_Vhigh - 1]->info;
  NodeLowPtr = (AnaExt_rec *)WITH->pin[H_Vlow - 1]->info;
  NodeIn = NodeInPtr->ltimestep;
  NodeOut = NodeOutPtr->ltimestep;
  NodeHigh = NodeHighPtr->ltimestep;
  NodeLow = NodeLowPtr->ltimestep;
  HresVar = (HresConst *)A_Gate->InstVar;
  AnaScoreboard(Gatename, (long)AnaMessGate1);
  if (NodeIn != PinIn || NodeOut != PinOut || NodeIn == AnaNotyet ||
      NodeOut == AnaNotyet || NodeLow != PinLow || NodeHigh != PinHigh ||
      NodeLow == AnaNotyet || NodeHigh == AnaNotyet) {
    AnaScoreboard(" <undefined>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    return;
  }
  if (PinLow > WITH->attr[N_Vt - 1].UU.r)
    AnaScoreboard("Ilow out of subthreshold", (long)AnaMessGate2);
  else {
    if (PinLow > PinIn && PinLow > PinOut)
      AnaScoreboard("Ilow limited by inputs", (long)AnaMessGate2);
    else {
      ExpoVlow = AnaExpo(HresVar->Kn2 * PinLow);
      BlendVlow = 1 / (1 + HresVar->Kn5 * ExpoVlow);
      Ilow = HresVar->Kn1 * ExpoVlow * BlendVlow +
	     HresVar->Kn4 * PinLow * PinLow * (1 - BlendVlow);
      sprintf(STR2, "Ilow = %s", AnaProbeAmpStr(STR1, Ilow));
      AnaScoreboard(STR2, (long)AnaMessGate2);
    }
  }
  if (PinHigh < AnaVdd - WITH->attr[N_Vt - 1].UU.r)
    AnaScoreboard("Ihigh out of subthreshold", (long)AnaMessGate3);
  else {
    if (PinHigh < PinIn && PinHigh < PinOut)
      AnaScoreboard("Ihigh limited by inputs", (long)AnaMessGate3);
    else {
      ExpoVhigh = AnaExpo(HresVar->Kn2 * (AnaVdd - PinHigh));
      BlendVhigh = 1 / (1 + HresVar->Kn5 * ExpoVhigh);
      pVhigh = AnaVdd - PinHigh;
      Ihigh = HresVar->Kp1 * ExpoVhigh * BlendVhigh +
	      (1 - BlendVhigh) * HresVar->Kp4 * pVhigh * pVhigh;
      sprintf(STR2, "Ihigh = %s", AnaProbeAmpStr(STR1, Ihigh));
      AnaScoreboard(STR2, (long)AnaMessGate3);
    }
  }
  if (fabs(PinIn - PinOut) < 2 * WITH->attr[N_Ktqn - 1].UU.r)
    AnaScoreboard("Tanh linear", (long)AnaMessGate4);
  else
    AnaScoreboard("Tanh saturated", (long)AnaMessGate4);
}



Void Log_hres_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    INITLIB_Hres(act);
    break;

  case Analog_act_ex:
    Ex_Hres(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Hres(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    Update_Hres(act);
    break;

  case Analog_act_display:
    Display_Hres(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Hres(act);
    break;

  case Analog_act_reset:
    Reset_Hres(act);
    break;

  case Analog_act_newgate:
    Newgate_Hres(act);
    break;

  case Analog_act_copygate:
    Copygate_Hres(act);
    break;

  case Analog_act_disposegate:
    Dispose_Hres(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Hres(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Hres(act);
    break;

  case Analog_act_erase:
    Erase_Hres(act);
    break;

  case Analog_act_refresh:
    Refresh_Hres(act);
    break;

  case Analog_act_probe:
    Probe_Hres(act);
    break;
  }
}

#undef N_Voffset
#undef N_Ipos
#undef N_Ineg
#undef N_IKtqn
#undef N_Leakage
#undef N_Vt
#undef N_Early
#undef N_Ktqn
#undef N_CapVlow
#undef N_CapVhigh
#undef N_CapVdiff
#undef Cstart
#undef H_Vin
#undef H_Vout
#undef H_Vlow
#undef H_Vhigh
#undef Gatename





/* End. */
