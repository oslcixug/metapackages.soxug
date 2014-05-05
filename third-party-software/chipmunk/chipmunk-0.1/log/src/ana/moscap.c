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


#define MOSCAP_G
#include "moscap.h"


#define N_Model         1
#define N_LinCap        2
#define N_NonCap        3
#define N_Vtn           4
#define N_Vtp           5
#define N_Ktqn          6

#define Cstart          7

#define Linear          0
#define NonLinear       1

#define Gatename        "Capacitor"


typedef struct MoscapConst {
  double Tau, Kn2, HalfCap;
} MoscapConst;


typedef struct ICMoscap {
  long InitModel;
  double InitLinCap, InitNonCap, InitVtn, InitVtp, InitKtqn;
} ICMoscap;




/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_Moscap(act)
Analog_32_action *act;
{
  double MinI1, dI1dV1;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1, VMem1;
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;
  double ExpoBot, BlendBot, dBlendBotdVpin1, ExpoTop, BlendTop,
	 dBlendTopdVpin1, Cnonlin, dCnonlindVpin1;
  log_grec *WITH;
  AnaMatRec *WITH2;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  MoscapVar = (MoscapConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;   /*The matrix index for each pin*/
  VPin1 = Pin1Ptr->last;   /*The voltage value for each pin*/
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*If implementing capacitors yourself,*/
  /*use this memory value*/
  /*use Dt, Accum+Dt for timevalue*/
  /*attributes may be referred to directly by attr^[]. syntax*/

  /*Use with statement to get to internal variables*/
  if (WITH->attr[N_Model - 1].UU.nv == Linear) {
    if (AnaChangedDt)
      MoscapVar->Tau = WITH->attr[N_LinCap - 1].UU.r / AnaDt;
    MinI1 = MoscapVar->Tau * (VMem1 - VPin1);
    dI1dV1 = MoscapVar->Tau;
  } else {
    ExpoBot = AnaExpo(MoscapVar->Kn2 * (WITH->attr[N_Vtn - 1].UU.r - VPin1));
    BlendBot = 1 / (1 + ExpoBot);
    dBlendBotdVpin1 = MoscapVar->Kn2 * BlendBot * BlendBot * ExpoBot;

    ExpoTop = AnaExpo(
	MoscapVar->Kn2 * (VPin1 - AnaVdd + WITH->attr[N_Vtp - 1].UU.r));
    BlendTop = 1 / (1 + ExpoTop);
    dBlendTopdVpin1 = -MoscapVar->Kn2 * BlendTop * BlendTop * ExpoTop;

    Cnonlin = MoscapVar->HalfCap * (BlendBot + BlendTop);
    dCnonlindVpin1 = MoscapVar->HalfCap * (dBlendBotdVpin1 + dBlendTopdVpin1);

    MinI1 = Cnonlin * (VMem1 - VPin1) / AnaDt;
    dI1dV1 = (Cnonlin + (VPin1 - VMem1) * dCnonlindVpin1) / AnaDt;

  }
  WITH2 = AnaSystem;
  WITH2->Mat[Pin1][AnaSysCol] += MinI1;
  WITH2->Mat[Pin1][Pin1] += dI1dV1;
  WITH2->MatNZ[Pin1][AnaSysCol] = true;
  WITH2->MatNZ[Pin1][Pin1] = true;
  AnaCapex(act->inst);
}  /*Ex_Moscap*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Moscap(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;
  log_grec *WITH;

  WITH = act->inst;   /*Use Dtlast, Dtlast+Accum for time values*/
  A_Gate = (Anainstlist *)WITH->info;
  MoscapVar = (MoscapConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;   /*The voltage value for each pin*/
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*If implementing capacitors yourself,*/
  /*use this memory value*/
  switch (act->pin) {

  case 1:   /*Allows attr^[]. access*/
    WITH = act->inst;
    if (WITH->attr[N_Model - 1].UU.nv == Linear)
      act->Iin = MoscapVar->Tau * (VPin1 - VMem1) + act->Iin;
    else
      act->Iin = MoscapVar->HalfCap * (VPin1 - VMem1) * (1 / (1 + AnaExpo(
		    MoscapVar->Kn2 * (WITH->attr[N_Vtn - 1].UU.r - VPin1))) +
	      1 / (1 + AnaExpo(MoscapVar->Kn2 * (VPin1 - AnaVdd +
			WITH->attr[N_Vtp - 1].UU.r)))) / AnaDtlast + act->Iin;
/* p2c: stdlib.text, line 560: Note:
 * Line breaker spent 3.0+1.00 seconds, 5000 tries on line 141 [251] */
    AnaCapIin(act);
    break;
    /*Allows internal variable access*/

  default:
    printf("Data Structure Corruption\n");
    _Escape(0);
    break;
  }
}  /*Iin_Moscap*/


Local Void GetCnf_Moscap(NewIC)
ICMoscap *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICMoscap *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("MOSCAP", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "NONLINEAR"))
	WITH->InitModel = NonLinear;
      if (!strcmp(Keyword, "LINEAR"))
	WITH->InitModel = Linear;
      if (!strcmp(Keyword, "LINCAP")) {
	TRY(try1);
	  WITH->InitLinCap = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->InitLinCap = 2e-12;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "NONLINCAP")) {
	TRY(try2);
	  WITH->InitNonCap = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->InitNonCap = 2e-12;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "VTN")) {
	TRY(try3);
	  WITH->InitVtn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitVtn = 0.9;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "VTP")) {
	TRY(try4);
	  WITH->InitVtp = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitVtp = 0.9;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "KT/QN")) {
	TRY(try5);
	  WITH->InitKtqn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitKtqn = 0.04;
	ENDTRY(try5);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_Moscap(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICMoscap *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anasimple;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewIC = (ICMoscap *)Malloc(sizeof(ICMoscap));
  NewIC->InitModel = Linear;
  NewIC->InitLinCap = 2e-12;
  NewIC->InitNonCap = 2e-12;
  NewIC->InitVtn = 0.9;
  NewIC->InitVtp = 0.9;
  NewIC->InitKtqn = 0.04;
  GetCnf_Moscap(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Moscap_Initlib*/


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



Local boolean Moscapcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  MoscapVar = (MoscapConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Model:
    if (Inst->attr[Attrnum - 1].blnk)   /*Or (Attr^[attrnum].nv=Nonlinear)*/
      Result = false;
    break;

  case N_LinCap:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].UU.r >
	  AnaCapTooBig || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_NonCap:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].UU.r >
	  AnaCapTooBig || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      MoscapVar->HalfCap = Inst->attr[N_NonCap - 1].UU.r / 2;
    break;

  case N_Vtn:
    if ((unsigned)Inst->attr[Attrnum - 1].UU.r > AnaVdd ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Vtp:
    if ((unsigned)Inst->attr[Attrnum - 1].UU.r > AnaVdd ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[Attrnum - 1].UU.r > 1 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      MoscapVar->Kn2 = 1 / Inst->attr[N_Ktqn - 1].UU.r;
    break;
  }
  return Result;

  /*Recompute all internal variables that depend on it*/
  /*Recompute all internal variables that depend on it*/
  /*Recompute all internal variables that depend on it*/
}


Local Void Attr_Moscap(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 2)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Moscapcheck(act->inst, Attrnum);
}


Local Void Newgate_Moscap(act)
Analog_32_action *act;
{
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICMoscap *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICMoscap *)Cellptr->Ainfo;
  MoscapVar = (MoscapConst *)Malloc(sizeof(MoscapConst));
  MoscapVar->HalfCap = ICptr->InitNonCap / 2;
  MoscapVar->Kn2 = 1 / ICptr->InitKtqn;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)MoscapVar;
  AnaCapInit(act->inst);   /*Initialize attributes of Cstart*/
  WITH = act->inst;
  WITH->attr[N_Model - 1].UU.nv = ICptr->InitModel;
  WITH->attr[N_Model - 1].blnk = false;
  WITH->attr[N_Model - 1].changed = true;
  WITH->attr[N_LinCap - 1].UU.r = ICptr->InitLinCap;
  WITH->attr[N_LinCap - 1].blnk = false;
  WITH->attr[N_LinCap - 1].changed = true;
  WITH->attr[N_NonCap - 1].UU.r = ICptr->InitNonCap;
  WITH->attr[N_NonCap - 1].blnk = false;
  WITH->attr[N_NonCap - 1].changed = true;
  WITH->attr[N_Vtn - 1].UU.r = ICptr->InitVtn;
  WITH->attr[N_Vtn - 1].blnk = false;
  WITH->attr[N_Vtn - 1].changed = true;
  WITH->attr[N_Vtp - 1].UU.r = ICptr->InitVtp;
  WITH->attr[N_Vtp - 1].blnk = false;
  WITH->attr[N_Vtp - 1].changed = true;
  WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;
  WITH->attr[N_Ktqn - 1].blnk = false;
  WITH->attr[N_Ktqn - 1].changed = true;
}

Local Void Copygate_Moscap(act)
Analog_32_action *act;
{
  MoscapConst *MoscapVar, *Old_Moscapvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Moscapvar = (MoscapConst *)A_Oldgate->InstVar;
  MoscapVar = (MoscapConst *)Malloc(sizeof(MoscapConst));
  *MoscapVar = *Old_Moscapvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)MoscapVar;
  AnaCapCopy(act->inst);   /*Initialize attributes 1 to 3*#*/
}

Local Void Dispose_Moscap(act)
Analog_32_action *act;
{
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  MoscapVar = (MoscapConst *)A_Gate->InstVar;
  Free(MoscapVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Moscap(act)
Analog_32_action *act;
{
  MoscapConst *MoscapVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICMoscap *ICptr;
  log_grec *WITH;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  MoscapVar = (MoscapConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICMoscap *)Cellptr->Ainfo;
    WITH = act->inst;
    WITH->attr[N_Model - 1].UU.nv = ICptr->InitModel;
    WITH->attr[N_Model - 1].blnk = false;
    WITH->attr[N_Model - 1].changed = true;
    WITH->attr[N_LinCap - 1].UU.r = ICptr->InitLinCap;
    WITH->attr[N_LinCap - 1].blnk = false;
    WITH->attr[N_LinCap - 1].changed = true;
    WITH->attr[N_NonCap - 1].UU.r = ICptr->InitNonCap;
    WITH->attr[N_NonCap - 1].blnk = false;
    WITH->attr[N_NonCap - 1].changed = true;
    WITH->attr[N_Vtn - 1].UU.r = ICptr->InitVtn;
    WITH->attr[N_Vtn - 1].blnk = false;
    WITH->attr[N_Vtn - 1].changed = true;
    WITH->attr[N_Vtp - 1].UU.r = ICptr->InitVtp;
    WITH->attr[N_Vtp - 1].blnk = false;
    WITH->attr[N_Vtp - 1].changed = true;
    WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;
    WITH->attr[N_Ktqn - 1].blnk = false;
    WITH->attr[N_Ktqn - 1].changed = true;
  }
  WITH = act->inst;
  MoscapVar->HalfCap = WITH->attr[N_NonCap - 1].UU.r / 2;
  MoscapVar->Kn2 = 1 / WITH->attr[N_Ktqn - 1].UU.r;
}



Local Void Probe_MosCap(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr;
  double Node1, Pin1;
  log_grec *WITH;
  Char STR1[14];
  Char STR2[18];
  Char STR3[256];
  Char STR4[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node1 = Node1Ptr->ltimestep;
  switch (WITH->attr[N_Model - 1].UU.nv) {

  case NonLinear:
    sprintf(STR1, "MOS %s", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    break;

  case Linear:
    sprintf(STR2, "Linear %s", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;
  }
  if (Node1 != Pin1 || Node1 == AnaNotyet) {
    AnaScoreboard(" <undefined>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  sprintf(STR4, "dV = %s", AnaProbeVoltStr(STR3, Pin1));
  AnaScoreboard(STR4, (long)AnaMessGate2);
  if (WITH->attr[N_Model - 1].UU.nv != NonLinear) {
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  if (Pin1 < WITH->attr[N_Vtn - 1].UU.r ||
      Pin1 > AnaVdd - WITH->attr[N_Vtp - 1].UU.r)
    AnaScoreboard("Half Capacitance", (long)AnaMessGate3);
  else
    AnaScoreboard("Full Capacitance", (long)AnaMessGate3);
}


/*==========================================================================*/
/*                 Library Listing For  Cell Moscap                    */
/*==========================================================================*/


Void Log_moscap_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Moscap(act);
    break;

  case Analog_act_ex:
    Ex_Moscap(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    AnaCapUpdate(act->inst);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Moscap(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Moscap(act);
    break;

  case Analog_act_copygate:
    Copygate_Moscap(act);
    break;

  case Analog_act_disposegate:
    Dispose_Moscap(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Moscap(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Moscap(act);
    break;

  case Analog_act_probe:
    Probe_MosCap(act);
    break;
  }
}

#undef N_Model
#undef N_LinCap
#undef N_NonCap
#undef N_Vtn
#undef N_Vtp
#undef N_Ktqn
#undef Cstart
#undef Linear
#undef NonLinear
#undef Gatename




/* End. */
