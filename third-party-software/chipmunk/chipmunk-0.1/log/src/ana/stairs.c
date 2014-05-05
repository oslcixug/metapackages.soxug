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


#define STAIRS_G
#include "stairs.h"


/*For Attributes*/

#define N_Freeze        1
#define N_Res           2
#define N_Type          3
#define N_tVstart       4
#define N_tVend         5
#define N_tNumsteps     6
#define N_tSteptime     7
#define N_tTrantime     8
#define N_tRdelay       9

#define Cstart          10

#define Gatename        "Stairstep Generator"


typedef struct StairsConst {
  double Conductance, Timez;
  boolean First, Limit, OldLimit, NewLimit;
  double Videal, Reset_Time;
  boolean Reset_First, Reset_Freeze;
} StairsConst;


typedef struct ICStairs {
  boolean ICFreeze;   /*Default Off*/
  double ICRes;   /*Default 50ohm*/
  long ICType;   /*Default 0*/
  double ICtVstart;   /*Default 0*/
  double ICtVend;   /*Default Vdd*/
  long ICtNumsteps;   /*Default 5*/
  double ICtSteptime;   /*Default Dtmax*10*/
  double ICtTrantime;   /*Default Dtmax*2*/
  double ICtRdelay;   /*Default 0*/
} ICStairs;



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



Local double Stairmaker(Inst, Ftime, First)
log_grec *Inst;
double Ftime;
boolean First;
{
  double Result;
  boolean Found;
  double Period;
  long Step;
  double Steplength, Vinc;

  Found = false;
  if (First) {
    if (Ftime < Inst->attr[N_tRdelay - 1].UU.r) {
      Result = Inst->attr[N_tVstart - 1].UU.r;
      Found = true;
    } else
      Ftime -= Inst->attr[N_tRdelay - 1].UU.r;
  }
  if (Found)
    return Result;
  if (Inst->attr[N_tNumsteps - 1].UU.U73.i1 == 0)
    Vinc = 0.0;
  else
    Vinc = (Inst->attr[N_tVend - 1].UU.r - Inst->attr[N_tVstart - 1].UU.r) /
	   Inst->attr[N_tNumsteps - 1].UU.U73.i1;
  Steplength = Inst->attr[N_tSteptime - 1].UU.r + Inst->attr[N_tTrantime - 1].UU.r;
  Period = (Inst->attr[N_tNumsteps - 1].UU.U73.i1 + 1) * Steplength;
  if (Ftime > Period)
    Ftime -= Period * (long)(Ftime / Period);
  Step = (long)(Ftime / Steplength);
  Ftime -= Step * Steplength;
  if (Ftime <= Inst->attr[N_tSteptime - 1].UU.r)
    return (Inst->attr[N_tVstart - 1].UU.r + Step * Vinc);
  else {
    Ftime -= Inst->attr[N_tSteptime - 1].UU.r;
    if (Step < Inst->attr[N_tNumsteps - 1].UU.U73.i1)
      return (Ftime / Inst->attr[N_tTrantime - 1].UU.r *
	      (Inst->attr[N_tVstart - 1].UU.r + (Step + 1) * Vinc) +
	      (1 - Ftime / Inst->attr[N_tTrantime - 1].UU.r) *
	      (Inst->attr[N_tVstart - 1].UU.r + Step * Vinc));
    else
      return (Ftime / Inst->attr[N_tTrantime - 1].UU.r *
	      Inst->attr[N_tVstart - 1].UU.r +
	      (1 - Ftime / Inst->attr[N_tTrantime - 1].UU.r) *
	      (Inst->attr[N_tVstart - 1].UU.r + Step * Vinc));
  }
  return Result;
}




/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local Void Ex_Stairs(act)
Analog_32_action *act;
{
  /*Stairserent*/
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  long Pin1, Pin2;
  double VPin1, VPin2;
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  double Fout, Ftime;
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;   /*Use with statement to get to internal variables*/
  if (WITH->attr[N_Freeze - 1].UU.b)
    Ftime = StairsVar->Timez;
  else
    Ftime = StairsVar->Timez + AnaDt;
  switch (WITH->attr[N_Type - 1].UU.nv) {

  case 0:
    Fout = Stairmaker(act->inst, Ftime, StairsVar->First);
    break;
  }
  StairsVar->Videal = Fout;
  MinI1 = StairsVar->Conductance * (VPin2 + Fout - VPin1);
  MinI2 = -MinI1;
  dI1dV1 = StairsVar->Conductance;
  dI1dV2 = -StairsVar->Conductance;
  dI2dV1 = -StairsVar->Conductance;
  dI2dV2 = StairsVar->Conductance;
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
}  /*Ex_Stairs*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_Stairs(act)
Analog_32_action *act;
{
  /*Stairserent*/
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, Vpin2;
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  double Fout, Ftime;
  log_grec *WITH;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Vpin2 = Pin2Ptr->now;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    if (WITH->attr[N_Freeze - 1].UU.b)
      Ftime = StairsVar->Timez;
    else
      Ftime = StairsVar->Timez + AnaDtlast;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case 0:
      Fout = Stairmaker(act->inst, Ftime, StairsVar->First);
      break;
    }
    act->Iin = StairsVar->Conductance * (VPin1 - Vpin2 - Fout) + act->Iin;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    if (WITH->attr[N_Freeze - 1].UU.b)
      Ftime = StairsVar->Timez;
    else
      Ftime = StairsVar->Timez + AnaDtlast;
    switch (WITH->attr[N_Type - 1].UU.nv) {

    case 0:
      Fout = Stairmaker(act->inst, Ftime, StairsVar->First);
      break;
    }
    act->Iin = StairsVar->Conductance * (Vpin2 + Fout - VPin1) + act->Iin;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_Stairs*/

Local Void GetCnf_Stairs(NewIC)
ICStairs *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICStairs *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("STAIRS", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "FREEZE")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	WITH->ICFreeze = false;
	if (!strcmp(Keyword, "ON"))
	  WITH->ICFreeze = true;
      }
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
	if (!strcmp(Keyword, "STAIRS"))
	  WITH->ICType = 0;
	else
	  WITH->ICType = 0;
      }
      if (!strcmp(Keyword, "STAIRSTEP")) {
	(*AnaLogglobals->hook.getword)(Arg, Keyword);
	if (!strcmp(Keyword, "VSTART")) {
	  TRY(try2);
	    WITH->ICtVstart = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try2);
	    WITH->ICtVstart = 0.0;
	  ENDTRY(try2);
	}
	if (!strcmp(Keyword, "VEND")) {
	  TRY(try3);
	    WITH->ICtVend = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try3);
	    WITH->ICtVend = AnaVdd;
	  ENDTRY(try3);
	}
	if (!strcmp(Keyword, "NUMSTEPS")) {
	  TRY(try4);
	    WITH->ICtNumsteps = strtol(Arg, &STR1, 10);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try4);
	    WITH->ICtNumsteps = 5;
	  ENDTRY(try4);
	}
	if (!strcmp(Keyword, "STEPTIME")) {
	  TRY(try5);
	    WITH->ICtSteptime = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try5);
	    WITH->ICtSteptime = AnaDtmax * 10;
	  ENDTRY(try5);
	}
	if (!strcmp(Keyword, "TRAN")) {
	  TRY(try6);
	    WITH->ICtTrantime = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try6);
	    WITH->ICtTrantime = AnaDtmax * 2;
	  ENDTRY(try6);
	}
	if (!strcmp(Keyword, "RDELAY")) {
	  TRY(try7);
	    WITH->ICtRdelay = strtod(Arg, &STR1);
	    Dummy = STR1 - Arg + 1;
	  RECOVER(try7);
	    WITH->ICtRdelay = 0.0;
	  ENDTRY(try7);
	}
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_Stairs(act)
Analog_32_action *act;
{
  /*Stairserent*/
  AnaCell_rec *NewCell;
  ICStairs *NewIC;

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
  NewIC = (ICStairs *)Malloc(sizeof(ICStairs));
  NewIC->ICFreeze = false;
  NewIC->ICRes = 50.0;
  NewIC->ICType = 0;
  NewIC->ICtVstart = 0.0;
  NewIC->ICtVend = AnaVdd;
  NewIC->ICtNumsteps = 5;
  NewIC->ICtSteptime = AnaDtmax * 10;
  NewIC->ICtTrantime = AnaDtmax * 2;
  NewIC->ICtRdelay = 0.0;
  GetCnf_Stairs(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Stairs_Initlib*/


Local boolean Stairscheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Freeze:
    Result = false;
    break;

  case N_Type:
    if (Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_Res:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].UU.r >
	  AnaResTooBig || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else
      StairsVar->Conductance = 1 / Inst->attr[Attrnum - 1].UU.r;
    break;

  case N_tVstart:
    if (Inst->attr[Attrnum - 1].UU.r < AnaLowest ||
	Inst->attr[Attrnum - 1].UU.r > AnaHighest || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_tVend:
    if (Inst->attr[Attrnum - 1].UU.r < AnaLowest ||
	Inst->attr[Attrnum - 1].UU.r > AnaHighest || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_tNumsteps:
    if (Inst->attr[N_tNumsteps - 1].UU.U73.i1 < 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

  case N_tSteptime:
  case N_tTrantime:
  case N_tRdelay:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;
  }
  return Result;
}




Local Void Attr_Stairs(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = Stairscheck(act->inst, Attrnum);
}


Local Void Newgate_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICStairs *ICptr;
  long Index;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICStairs *)Cellptr->Ainfo;
  StairsVar = (StairsConst *)Malloc(sizeof(StairsConst));
  StairsVar->Conductance = 1 / ICptr->ICRes;
  StairsVar->Reset_Time = 0.0;
  StairsVar->Reset_First = true;
  StairsVar->Reset_Freeze = ICptr->ICFreeze;
  StairsVar->Timez = StairsVar->Reset_Time;
  StairsVar->First = StairsVar->Reset_First;
  StairsVar->Limit = false;
  StairsVar->OldLimit = false;
  StairsVar->NewLimit = false;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)StairsVar;
  AnaCapInit(act->inst);
  WITH = act->inst;
  WITH->attr[N_Freeze - 1].UU.b = ICptr->ICFreeze;
  WITH->attr[N_Res - 1].UU.r = ICptr->ICRes;
  WITH->attr[N_Type - 1].UU.nv = ICptr->ICType;
  WITH->attr[N_tVstart - 1].UU.r = ICptr->ICtVstart;
  WITH->attr[N_tVend - 1].UU.r = ICptr->ICtVend;
  WITH->attr[N_tNumsteps - 1].UU.U73.i1 = ICptr->ICtNumsteps;
  WITH->attr[N_tSteptime - 1].UU.r = ICptr->ICtSteptime;
  WITH->attr[N_tTrantime - 1].UU.r = ICptr->ICtTrantime;
  WITH->attr[N_tRdelay - 1].UU.r = ICptr->ICtRdelay;
  for (Index = 0; Index <= Cstart - 2; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar, *Old_Stairsvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Stairsvar = (StairsConst *)A_Oldgate->InstVar;
  StairsVar = (StairsConst *)Malloc(sizeof(StairsConst));
  *StairsVar = *Old_Stairsvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)StairsVar;
  AnaCapCopy(act->inst);
}


Local Void Dispose_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Free(StairsVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;
  Char STR1[256];

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  StairsVar->Conductance = 1 / WITH1->attr[N_Res - 1].UU.r;
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &StairsVar->Timez);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  StairsVar->First = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  StairsVar->Limit = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &StairsVar->Videal);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &StairsVar->Reset_Time);
  getc(*AnaLogglobals->actfile);
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  StairsVar->Reset_First = (*STR1 == 'T' || *STR1 == 't');
  fscanf(*AnaLogglobals->actfile, " %[a-zA-Z]%*[^\n]", STR1);
  getc(*AnaLogglobals->actfile);
  StairsVar->Reset_Freeze = (*STR1 == 'T' || *STR1 == 't');
}

Local Void Writegate_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  AnaCapattrwrite((long)Cstart, act->inst);   /*Write capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  fprintf(*AnaLogglobals->actfile, "% .5E\n", StairsVar->Timez);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  StairsVar->First ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  StairsVar->Limit ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "% .5E\n", StairsVar->Videal);
  fprintf(*AnaLogglobals->actfile, "% .5E\n", StairsVar->Reset_Time);
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  StairsVar->Reset_First ? " TRUE" : "FALSE");
  fprintf(*AnaLogglobals->actfile, "%s\n",
	  StairsVar->Reset_Freeze ? " TRUE" : "FALSE");
}



Local Void Reset_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  StairsVar->Timez = StairsVar->Reset_Time;
  StairsVar->First = StairsVar->Reset_First;
  act->inst->attr[N_Freeze - 1].UU.b = StairsVar->Reset_Freeze;
  StairsVar->Limit = false;
  StairsVar->OldLimit = false;
  StairsVar->NewLimit = false;
  if (AnaLogglobals->showpage == act->page) {
    Limitfill(act->inst, StairsVar->Limit, false);
    Switchfill(act->inst, false);
  }
}

Local Void Set_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  StairsVar->Reset_Time = StairsVar->Timez;
  StairsVar->Reset_First = StairsVar->First;
  StairsVar->Reset_Freeze = act->inst->attr[N_Freeze - 1].UU.b;
}

Local Void Clear_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICStairs *ICptr;

  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICStairs *)Cellptr->Ainfo;
  StairsVar->Reset_Time = 0.0;
  StairsVar->Reset_First = true;
  StairsVar->Reset_Freeze = ICptr->ICFreeze;
}

Local Void PreUpdate_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, VPin2;
  log_grec *WITH;

  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  WITH = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  VPin2 = Pin2Ptr->now;
  StairsVar->NewLimit = (fabs(VPin1 - VPin2 - StairsVar->Videal) > 0.1);
}

Local Void Update_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  StairsVar->OldLimit = StairsVar->Limit;
  StairsVar->Limit = StairsVar->NewLimit;
  if (!WITH1->attr[N_Freeze - 1].UU.b)
    StairsVar->Timez += AnaDtlast;
}

Local Void Display_Stairs(act)
Analog_32_action *act;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;
  double Period;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (!WITH1->attr[N_Freeze - 1].UU.b) {
    if (WITH1->attr[N_Type - 1].UU.nv == 1) {
      if (StairsVar->First) {
	if (StairsVar->Timez > WITH1->attr[N_tRdelay - 1].UU.r) {
	  StairsVar->First = false;
	  StairsVar->Timez -= WITH1->attr[N_tRdelay - 1].UU.r;
	}
      }
      if (!StairsVar->First) {
	Period = (WITH1->attr[N_tNumsteps - 1].UU.U73.i1 + 1) *
	    (WITH1->attr[N_tSteptime - 1].UU.r + WITH1->attr[N_tTrantime - 1].UU.r);
	if (StairsVar->Timez > Period)
	  StairsVar->Timez -= Period * (long)(StairsVar->Timez / Period);
      }
    }
  }
  if (StairsVar->OldLimit != StairsVar->Limit &&
      AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, StairsVar->Limit, false);
}


Local Void Touch_Stairs(act)
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


Local Void Erase_Stairs(Inst)
log_grec *Inst;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)Inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Switchfill(Inst, true);
  Limitfill(Inst, StairsVar->Limit, true);
}

Local Void Refresh_Stairs(Inst)
log_grec *Inst;
{
  StairsConst *StairsVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)Inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  Switchfill(Inst, false);
  Limitfill(Inst, StairsVar->Limit, false);
}

Local Void Probe_Stairs(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  StairsConst *StairsVar;
  AnaExt_rec *Node1Ptr, *Node2Ptr;
  double Node1, Node2, Pin1, Pin2, d;
  log_grec *WITH;
  Char STR1[256];
  Char STR2[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Pin2 = A_Gate->Pininfo[1].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  A_Gate = (Anainstlist *)act->inst->info;
  StairsVar = (StairsConst *)A_Gate->InstVar;
  AnaScoreboard(Gatename, (long)AnaMessGate1);
  if (Node1 != Pin1 || Node1 == AnaNotyet || Node2 != Pin2 || Node2 == AnaNotyet) {
    AnaScoreboard(" <undefined state>", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  d = Node1 - Node2;
  sprintf(STR2, "dV = %s", AnaProbeVoltStr(STR1, d));
  AnaScoreboard(STR2, (long)AnaMessGate2);
  if (StairsVar->Limit)
    AnaScoreboard("Current limited", (long)AnaMessGate3);
  else
    AnaScoreboard("Full voltage delivered", (long)AnaMessGate3);
}


/*$If Float$
$float_hdw on$
$End$*/
/*$If Debugflag$
$Debug$
$End$*/

/*=============================================================================*/
/*                 Library Listing For Cell Stairs                            */
/*=============================================================================*/


Void Log_stairs_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/

  switch (act->action) {

  case Analog_act_newkind:
    initlib_Stairs(act);
    break;

  case Analog_act_ex:
    Ex_Stairs(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Stairs(act);
    break;

  case Analog_act_update:
    Update_Stairs(act);
    break;

  case Analog_act_display:
    Display_Stairs(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Stairs(act);
    break;

  case Analog_act_reset:
    Reset_Stairs(act);
    break;

  case Analog_act_newgate:
    Newgate_Stairs(act);
    break;

  case Analog_act_copygate:
    Copygate_Stairs(act);
    break;

  case Analog_act_disposegate:
    Dispose_Stairs(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Stairs(act);
    break;

  case Analog_act_writegate:
    Writegate_Stairs(act);
    break;

  case Analog_act_Iin:
    Iin_Stairs(act);
    break;

  case Analog_act_erase:
    Erase_Stairs(act->inst);
    break;

  case Analog_act_refresh:
    Refresh_Stairs(act->inst);
    break;

  case Analog_act_touch:
    Touch_Stairs(act);
    break;

  case Analog_act_set:
    Set_Stairs(act);
    break;

  case Analog_act_clear:
    Clear_Stairs(act);
    break;

  case Analog_act_probe:
    Probe_Stairs(act);
    break;
  }
}

#undef N_Freeze
#undef N_Res
#undef N_Type
#undef N_tVstart
#undef N_tVend
#undef N_tNumsteps
#undef N_tSteptime
#undef N_tTrantime
#undef N_tRdelay
#undef Cstart
#undef Gatename





/* End. */
