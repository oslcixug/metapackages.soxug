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
/* From input file "scott.text" */




#include <p2c/p2c.h>




#define DIODE1_G
#include "diode1.h"


#define N_Is            1
#define N_Ktqn          2
#define N_Vcrow         3
#define Cstart          4

#define Gatename        "DIODE1"


typedef struct Diode1Const {
  double Is, lam, K1, K2;

  boolean Limit, OldLimit, NewLimit;
} Diode1Const;


typedef struct ICDiode1 {
  double ICIs, ICKtqn, ICVcrow, InitCab, InitCcb;
} ICDiode1;


/****************************************************************************/
/*            Procedure To Light 'Limiting' Light                           */
/****************************************************************************/


Local Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
boolean Limitflag, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = -1;
  URx = 1;
  LLy = -2;
  URy = 1;
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
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local Void Ex_Diode1(act)
Analog_32_action *act;
{
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2;
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  double Vgs, ExpVgs, K2ExpVgs, IsExpVgs, IsExpVgsMinOne, Blend, InvBlend,
	 K1Vgs, lamBlend, Ifor, dBlenddS, dIfordG;
  log_grec *WITH;
  AnaMatRec *WITH2;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin1 = Pin1Ptr->nodenum;
  Pin2 = Pin2Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  VPin2 = Pin2Ptr->last;   /*Use with statement to get to internal variables*/

  Vgs = VPin1 - VPin2;
  ExpVgs = AnaExpo(Diode1Var->lam * Vgs);
  K2ExpVgs = Diode1Var->K2 * ExpVgs;
  IsExpVgs = Diode1Var->Is * ExpVgs;
  IsExpVgsMinOne = IsExpVgs - Diode1Var->Is;
  Blend = 1 / (1 + K2ExpVgs);
  InvBlend = 1 - Blend;

  K1Vgs = Diode1Var->K1 * Vgs;
  lamBlend = Diode1Var->lam * Blend;

  Ifor = IsExpVgsMinOne * Blend + InvBlend * K1Vgs;

  dBlenddS = lamBlend * K2ExpVgs * Blend;
  dIfordG = IsExpVgs * lamBlend - IsExpVgsMinOne * dBlenddS +
	    Diode1Var->K1 * InvBlend + K1Vgs * dBlenddS;

  MinI1 = -Ifor;
  MinI2 = Ifor;
  dI1dV1 = dIfordG;
  dI1dV2 = -dIfordG;
  dI2dV1 = -dIfordG;
  dI2dV2 = dIfordG;

  WITH2 = AnaSystem;
  if (Pin1 != 0) {
    WITH2->Mat[Pin1][AnaSysCol] += MinI1;
    WITH2->Mat[Pin1][Pin1] += dI1dV1;
    WITH2->Mat[Pin1][Pin2] += dI1dV2;
    WITH2->MatNZ[Pin1][AnaSysCol] = true;
    WITH2->MatNZ[Pin1][Pin1] = true;
    WITH2->MatNZ[Pin1][Pin2] = true;
  }
  if (Pin2 != 0) {
    WITH2->Mat[Pin2][AnaSysCol] += MinI2;
    WITH2->Mat[Pin2][Pin1] += dI2dV1;
    WITH2->Mat[Pin2][Pin2] += dI2dV2;
    WITH2->MatNZ[Pin2][AnaSysCol] = true;
    WITH2->MatNZ[Pin2][Pin1] = true;
    WITH2->MatNZ[Pin2][Pin2] = true;
  }
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_Diode1*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Diode1(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  double Vgs, ExpVgs, Blend, Ifor;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  VMem1 = A_Gate->Pininfo[0].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  VPin2 = Pin2Ptr->now;
  VMem2 = A_Gate->Pininfo[1].Vmem;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    Vgs = VPin1 - VPin2;
    ExpVgs = AnaExpo(Diode1Var->lam * Vgs);
    Blend = 1 / (1 + Diode1Var->K2 * ExpVgs);
    Ifor = Diode1Var->Is * (ExpVgs - 1) * Blend + (1 - Blend) * Diode1Var->K1 * Vgs;
    act->Iin = Ifor + act->Iin;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    Vgs = VPin1 - VPin2;
    ExpVgs = AnaExpo(Diode1Var->lam * Vgs);
    Blend = 1 / (1 + Diode1Var->K2 * ExpVgs);
    Ifor = Diode1Var->Is * (ExpVgs - 1) * Blend + (1 - Blend) * Diode1Var->K1 * Vgs;
    act->Iin = act->Iin - Ifor;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_Diode1*/

Local Void GetCnf_Diode1(NewIC)
ICDiode1 *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICDiode1 *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("DIODE1", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "IS")) {
	TRY(try1);
	  WITH->ICIs = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICIs = 1e-15;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "KT/QN")) {
	TRY(try2);
	  WITH->ICKtqn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->ICKtqn = 0.04;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "VCROW")) {
	TRY(try3);
	  WITH->ICVcrow = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->ICVcrow = 0.8;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "CCSUB")) {
	TRY(try4);
	  WITH->InitCcb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitCcb = 10e-15;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "CASUB")) {
	TRY(try5);
	  WITH->InitCab = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitCab = 10e-15;
	ENDTRY(try5);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_Diode1(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICDiode1 *NewIC;

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
  NewIC = (ICDiode1 *)Malloc(sizeof(ICDiode1));
  NewIC->ICIs = 1e-15;
  NewIC->ICKtqn = 0.04;
  NewIC->ICVcrow = 0.8;
  NewIC->InitCab = 10e-15;
  NewIC->InitCcb = 10e-15;
  GetCnf_Diode1(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Diode1_Initlib*/



Local boolean Diode1check(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Is:
    if (Inst->attr[Attrnum - 1].UU.r < AnaITooSmall ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else {
      Diode1Var->Is = Inst->attr[N_Is - 1].UU.r;
      Diode1Var->K1 = Diode1Var->Is *
	  (AnaExpo(Diode1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
    }
    break;

  case N_Ktqn:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > 1)
      Result = false;
    else {
      Diode1Var->lam = 1 / Inst->attr[N_Ktqn - 1].UU.r;
      Diode1Var->K1 = Diode1Var->Is *
	  (AnaExpo(Diode1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      Diode1Var->K2 = AnaExpo(-Diode1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
    }
    break;

  case N_Vcrow:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > 1.5)
      Result = false;
    else {
      Diode1Var->K1 = Diode1Var->Is *
	  (AnaExpo(Diode1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      Diode1Var->K2 = AnaExpo(-Diode1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
    }
    break;
  }
  return Result;
}


Local Void Attr_Diode1(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Diode1check(act->inst, Attrnum);
}


Local Void Newgate_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICDiode1 *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICDiode1 *)Cellptr->Ainfo;
  Diode1Var = (Diode1Const *)Malloc(sizeof(Diode1Const));
  Diode1Var->Is = ICptr->ICIs;
  Diode1Var->lam = 1 / ICptr->ICKtqn;
  Diode1Var->K1 = Diode1Var->Is *
      (AnaExpo(Diode1Var->lam * ICptr->ICVcrow) - 1) / ICptr->ICVcrow;
  Diode1Var->K2 = AnaExpo(-Diode1Var->lam * ICptr->ICVcrow);
  Diode1Var->Limit = false;
  Diode1Var->OldLimit = false;
  Diode1Var->NewLimit = false;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)Diode1Var;
  AnaCapInit(act->inst);
  A_Gate->Pininfo[0].Cap = ICptr->InitCcb;
  A_Gate->Pininfo[1].Cap = ICptr->InitCab;
  WITH = act->inst;
  WITH->attr[N_Is - 1].UU.r = ICptr->ICIs;
  WITH->attr[N_Is - 1].blnk = false;
  WITH->attr[N_Is - 1].changed = true;
  WITH->attr[N_Ktqn - 1].UU.r = ICptr->ICKtqn;
  WITH->attr[N_Ktqn - 1].blnk = false;
  WITH->attr[N_Ktqn - 1].changed = true;
  WITH->attr[N_Vcrow - 1].UU.r = ICptr->ICVcrow;
  WITH->attr[N_Vcrow - 1].blnk = false;
  WITH->attr[N_Vcrow - 1].changed = true;
}

Local Void Copygate_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var, *Old_Diode1var;
  Anainstlist *A_Gate, *A_Oldgate;

  /*If any internal variables then*/
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Diode1var = (Diode1Const *)A_Oldgate->InstVar;
  Diode1Var = (Diode1Const *)Malloc(sizeof(Diode1Const));
  *Diode1Var = *Old_Diode1var;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)Diode1Var;
  /*Always Do*/
  AnaCapCopy(act->inst);
}

Local Void Dispose_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Free(Diode1Var);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICDiode1 *ICptr;
  log_grec *WITH;

  AnaCapattrread(1L, act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICDiode1 *)Cellptr->Ainfo;
    A_Gate->Pininfo[0].Cap = ICptr->InitCcb;
    A_Gate->Pininfo[1].Cap = ICptr->InitCab;
    WITH = act->inst;
    WITH->attr[N_Is - 1].UU.r = ICptr->ICIs;
    WITH->attr[N_Is - 1].blnk = false;
    WITH->attr[N_Is - 1].changed = true;
    WITH->attr[N_Ktqn - 1].UU.r = ICptr->ICKtqn;
    WITH->attr[N_Ktqn - 1].blnk = false;
    WITH->attr[N_Ktqn - 1].changed = true;
    WITH->attr[N_Vcrow - 1].UU.r = ICptr->ICVcrow;
    WITH->attr[N_Vcrow - 1].blnk = false;
    WITH->attr[N_Vcrow - 1].changed = true;
  }
  WITH = act->inst;
  Diode1Var->Is = WITH->attr[N_Is - 1].UU.r;
  Diode1Var->lam = 1 / WITH->attr[N_Ktqn - 1].UU.r;
  Diode1Var->K1 = Diode1Var->Is *
      (AnaExpo(Diode1Var->lam * WITH->attr[N_Vcrow - 1].UU.r) - 1) /
      WITH->attr[N_Vcrow - 1].UU.r;
  Diode1Var->K2 = AnaExpo(-Diode1Var->lam * WITH->attr[N_Vcrow - 1].UU.r);
}

Local Void Probe_Diode1(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr;
  double Node1, Node2, Pin1, Pin2, d;
  Char Name1[256], Name2[256];
  log_grec *WITH;
  Char STR1[20];
  Char STR2[256];
  Char STR3[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Pin2 = A_Gate->Pininfo[1].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  if (Node1 != Pin1 || Node2 != Pin2 || Node1 == AnaNotyet ||
      Node2 == AnaNotyet) {
    sprintf(STR1, "%s <undefined>", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  d = Pin1 - Pin2;
  strcpy(Name1, " (+)");
  strcpy(Name2, " (-)");
  switch (AnaLogglobals->probepin) {

  case 0:
    AnaScoreboard(Gatename, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s%s", Gatename, Name1);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR2, "%s%s", Gatename, Name2);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;
  }
  sprintf(STR2, "dV = %s", AnaProbeVoltStr(STR3, d));
  AnaScoreboard(STR2, (long)AnaMessGate2);
  if (d >= WITH->attr[N_Vcrow - 1].UU.r)
    AnaScoreboard("Diode in Ohmic Region", (long)AnaMessGate3);
  else
    AnaScoreboard("$", (long)AnaMessGate3);
}

Local Void Reset_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Diode1Var->Limit = false;
  Diode1Var->OldLimit = false;
  Diode1Var->NewLimit = false;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, Diode1Var->Limit, false);
}

Local Void PreUpdate_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, Vpin2;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
  VPin1 = Pin1Ptr->last;
  Vpin2 = Pin2Ptr->last;
  Diode1Var->NewLimit = (VPin1 - Vpin2 >= WITH1->attr[N_Vcrow - 1].UU.r);
}

Local Void Update_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Diode1Var->OldLimit = Diode1Var->Limit;
  Diode1Var->Limit = Diode1Var->NewLimit;
}

Local Void Display_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (Diode1Var->OldLimit != Diode1Var->Limit)
      Limitfill(act->inst, Diode1Var->Limit, false);
  }
}


Local Void Refresh_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Limitfill(act->inst, Diode1Var->Limit, false);
}

Local Void Erase_Diode1(act)
Analog_32_action *act;
{
  Diode1Const *Diode1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  Diode1Var = (Diode1Const *)A_Gate->InstVar;
  Limitfill(act->inst, Diode1Var->Limit, true);
}


/*==========================================================================*/
/*                 Library Listing For  Cell Diode1                    */
/*==========================================================================*/


Void Log_Diode1_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Diode1(act);
    break;

  case Analog_act_ex:
    Ex_Diode1(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Diode1(act);
    break;

  case Analog_act_update:   /*For complex gates - see IDIFF*/
    Update_Diode1(act);
    break;

  case Analog_act_display:
    Display_Diode1(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Diode1(act);
    break;

  case Analog_act_reset:
    Reset_Diode1(act);
    break;

  case Analog_act_newgate:
    Newgate_Diode1(act);
    break;

  case Analog_act_copygate:
    Copygate_Diode1(act);
    break;

  case Analog_act_disposegate:
    Dispose_Diode1(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Diode1(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Diode1(act);
    break;

  case Analog_act_erase:
    Erase_Diode1(act);
    break;

  case Analog_act_refresh:
    Refresh_Diode1(act);
    break;

  case Analog_act_probe:
    Probe_Diode1(act);
    break;
  }
}

#undef N_Is
#undef N_Ktqn
#undef N_Vcrow
#undef Cstart
#undef Gatename







/* End. */
