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
/* From input file "gates.text" */




#include <p2c/p2c.h>





#define CAPFLOAT_G
#include "capfloat.h"


#define N_Cap           1
#define Cstart          2

#define GateName        "Capacitor"


typedef struct CapfloatConst {
  double Tau;
} CapfloatConst;


typedef struct ICCapfloat {
  double ICCap;
} ICCapfloat;




/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_Capfloat(act)
Analog_32_action *act;
{
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2, VMem2;
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;
  log_grec *WITH;
  AnaMatRec *WITH2;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  CapfloatVar = (CapfloatConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  VMem1 = A_Gate->Pininfo[0].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;
  VMem2 = A_Gate->Pininfo[1].Vmem;
      /*Use with statement to get to internal variables*/
  if (AnaChangedDt)
    CapfloatVar->Tau = WITH->attr[N_Cap - 1].UU.r / AnaDt;
  MinI1 = CapfloatVar->Tau * (VPin2 - VPin1 + VMem1 - VMem2);
  MinI2 = -MinI1;
  dI1dV1 = CapfloatVar->Tau;
  dI1dV2 = -CapfloatVar->Tau;
  dI2dV1 = -CapfloatVar->Tau;
  dI2dV2 = CapfloatVar->Tau;
  WITH2 = AnaSystem;
  WITH2->Mat[Pin1][AnaSysCol] += MinI1;
  WITH2->Mat[Pin2][AnaSysCol] += MinI2;
  WITH2->Mat[Pin1][Pin1] += dI1dV1;
  WITH2->Mat[Pin1][Pin2] += dI1dV2;
  WITH2->Mat[Pin2][Pin1] += dI2dV1;
  WITH2->Mat[Pin2][Pin2] += dI2dV2;
  WITH2->MatNZ[Pin1][AnaSysCol] = true;
  WITH2->MatNZ[Pin2][AnaSysCol] = true;
  WITH2->MatNZ[Pin1][Pin1] = true;
  WITH2->MatNZ[Pin1][Pin2] = true;
  WITH2->MatNZ[Pin2][Pin1] = true;
  WITH2->MatNZ[Pin2][Pin2] = true;
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_Capfloat*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Capfloat(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  CapfloatVar = (CapfloatConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  VMem1 = A_Gate->Pininfo[0].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  VPin2 = Pin2Ptr->now;
  VMem2 = A_Gate->Pininfo[1].Vmem;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    act->Iin = CapfloatVar->Tau * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    act->Iin = CapfloatVar->Tau * (VPin2 - VPin1 + VMem1 - VMem2) + act->Iin;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_Capfloat*/


Local Void GetCnf_Capfloat(NewIC)
ICCapfloat *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICCapfloat *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("CAPFLOAT", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "CAP")) {
	TRY(try1);
	  WITH->ICCap = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICCap = 1e-12;
	ENDTRY(try1);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_Capfloat(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICCapfloat *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anasimple;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewCell->plist_cell[1].standalone = false;
  NewCell->plist_cell[1].active = true;
  NewIC = (ICCapfloat *)Malloc(sizeof(ICCapfloat));
  NewIC->ICCap = 1e-12;
  GetCnf_Capfloat(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Capfloat_Initlib*/



Local boolean Capfloatcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  CapfloatVar = (CapfloatConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Cap:
    if (Inst->attr[Attrnum - 1].UU.r < 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaCapTooBig)
      Result = false;
    break;
  }
  return Result;

  /*Recompute all internal variables that depend on it*/
}


Local Void Attr_Capfloat(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Capfloatcheck(act->inst, Attrnum);
}


Local Void Newgate_Capfloat(act)
Analog_32_action *act;
{
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICCapfloat *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICCapfloat *)Cellptr->Ainfo;
  CapfloatVar = (CapfloatConst *)Malloc(sizeof(CapfloatConst));
  /*Give all internal variables values derived from initial attribute values*/
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)CapfloatVar;
  AnaCapInit(act->inst);
  WITH = act->inst;
  WITH->attr[N_Cap - 1].UU.r = ICptr->ICCap;
  WITH->attr[N_Cap - 1].blnk = false;
  WITH->attr[N_Cap - 1].changed = true;
}

Local Void Copygate_Capfloat(act)
Analog_32_action *act;
{
  CapfloatConst *CapfloatVar, *Old_Capfloatvar;
  Anainstlist *A_Gate, *A_Oldgate;

  /*If any internal variables then*/
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Capfloatvar = (CapfloatConst *)A_Oldgate->InstVar;
  CapfloatVar = (CapfloatConst *)Malloc(sizeof(CapfloatConst));
  *CapfloatVar = *Old_Capfloatvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)CapfloatVar;
  /*Always Do*/
  AnaCapCopy(act->inst);
}

Local Void Dispose_Capfloat(act)
Analog_32_action *act;
{
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  CapfloatVar = (CapfloatConst *)A_Gate->InstVar;
  Free(CapfloatVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Capfloat(act)
Analog_32_action *act;
{
  CapfloatConst *CapfloatVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  CapfloatVar = (CapfloatConst *)A_Gate->InstVar;
  /*Set any internals variables to */
  WITH1 = act->inst;
}

Local Void Probe_Capfloat(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr;
  double Node1, Node2, Pin1, Pin2, d;
  Char Name1[256], Name2[256];
  log_grec *WITH;
  Char STR1[22];
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
  if (Node1 != Pin1 || Node2 != Pin2 || Node1 == AnaNotyet || Node2 == AnaNotyet) {
    sprintf(STR1, "%s <undefined>", GateName);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    return;
  }
  d = Pin1 - Pin2;
  if (d >= 0) {
    strcpy(Name1, " (+)");
    strcpy(Name2, " (-)");
  } else {
    d = -d;
    strcpy(Name1, " (-)");
    strcpy(Name2, " (+)");
  }
  switch (AnaLogglobals->probepin) {

  case 0:
    AnaScoreboard(GateName, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s%s", GateName, Name1);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR2, "%s%s", GateName, Name2);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;
  }
  sprintf(STR2, "dV = %s", AnaProbeVoltStr(STR3, d));
  AnaScoreboard(STR2, (long)AnaMessGate2);
}


/*==========================================================================*/
/*                 Library Listing For  Cell Capfloat                    */
/*==========================================================================*/


Void Log_capfloat_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Capfloat(act);
    break;

  case Analog_act_ex:
    Ex_Capfloat(act);
    break;

  case Analog_act_update:   /*8*/
    AnaCapUpdate(act->inst);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Capfloat(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Capfloat(act);
    break;

  case Analog_act_copygate:
    Copygate_Capfloat(act);
    break;

  case Analog_act_disposegate:
    Dispose_Capfloat(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Capfloat(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_Capfloat(act);
    break;

  case Analog_act_probe:
    Probe_Capfloat(act);
    break;
  }
}

#undef N_Cap
#undef Cstart
#undef GateName




/* End. */
