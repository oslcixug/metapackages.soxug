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
/* From input file "globals.text" */


/*******************************************************************************/
/*     This routine holds global variable declarations for entire system       */
/*******************************************************************************/



#include <p2c/p2c.h>


#define GLOBALS_G
#include "globals.h"

#ifndef NEWCRT_H
#include <p2c/newcrt.h>
#endif


Char *AnaProbeVoltStr(AnaResult, Val)
Char *AnaResult;
double Val;
{
  Char Outstr[256];

  if (fabs(Val) < AnaVardone_Epsilon)
    (*AnaLogglobals->hook.realunit)(0.0, (int)AnaProbeprec, "V", Outstr);
  else
    (*AnaLogglobals->hook.realunit)(Val, (int)AnaProbeprec, "V", Outstr);
  return strcpy(AnaResult, Outstr);
}


Char *AnaProbeAmpStr(AnaResult, Val)
Char *AnaResult;
double Val;
{
  Char Outstr[256];

  if (fabs(Val) < 1e-15)
    (*AnaLogglobals->hook.realunit)(0.0, (int)AnaProbeprec, "A", Outstr);
  else
    (*AnaLogglobals->hook.realunit)(Val, (int)AnaProbeprec, "A", Outstr);
  return strcpy(AnaResult, Outstr);
}



long AnaGlowVal(Intensity)
long Intensity;
{
  if (Intensity > AnaLevelmap.Level_c) {
    if (Intensity > AnaLevelmap.Level_d) {
      if (Intensity > AnaLevelmap.Level_e)
	return (AnaColormap.C_Glow6);
      else
	return (AnaColormap.C_Glow5);
    } else
      return (AnaColormap.C_Glow4);
  } else {
    if (Intensity > AnaLevelmap.Level_b)
      return (AnaColormap.C_Glow3);
    else {
      if (Intensity > AnaLevelmap.Level_a)
	return (AnaColormap.C_Glow2);
      else
	return (AnaColormap.C_Glow1);
    }
  }
}



Void AnaTimeReset()
{

  /*Should be called whenever: 1)Resetting simulation.
                              2)Initializing analog.
                              3)Changing any of the following variables:
                                 Dtmax
                                 Timerelax
                                 Timefactor                                  */
  if (AnaDtmax <= AnaTimerelax || AnaTimefactor <= 1)
    _Escape(1001);
  AnaRelaxcount = 0;
  AnaDt = AnaDtmax;
  while (AnaDt > AnaTimerelax) {
    AnaRelaxcount--;
    AnaDt /= AnaTimefactor;
  }
  AnaDt *= AnaTimefactor;
  AnaDtcount = AnaRelaxcount + 1;
  AnaChangedDt = true;
  AnaItermax = AnaIterfirstmax;
  AnaDifficult = false;
}


/*Execution procedure for analog gates that cannot presently simulate*/


Void AnaDummyGate(act)
Analog_32_action *act;
{
  switch (act->action) {

  case Analog_act_newgate:
    AnaCapInit(act->inst);
    break;

  case Analog_act_copygate:
    AnaCapCopy(act->inst);
    break;

  case Analog_act_disposegate:
    AnaCapDispose(act->inst);
    break;

  case Analog_act_attrchange:
    act->ok = false;
    break;
  }
}


Void AnaDummyInit(dummykindptr)
log_krec *dummykindptr;
{
  AnaCell_rec *NewCell;
  long Index, FORLIM;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = false;
  NewCell->proc_cell = AnaDummyGate;
  NewCell->intr_cell = 0;
  NewCell->phase = Anasimple;
  NewCell->plist_cell = (Anapinrec *)
			Malloc(sizeof(Anapinrec) * dummykindptr->numpins);
  FORLIM = dummykindptr->numpins;
  for (Index = 0; Index < FORLIM; Index++) {
    NewCell->plist_cell[Index].standalone = false;
    NewCell->plist_cell[Index].active = true;
  }
  NewCell->Ainfo = NULL;
  dummykindptr->info = (Anyptr)NewCell;
}


/********************************************************************************/
/*                   Procedure to set/clear reset memory                        */

Void AnaAnalog_ChangeReset(Erase)
boolean Erase;
{

  /********************************************************************************/
  log_grec *Count;
  AnaCell_rec *A_Kind;
  long Pages, Index;
  Anainstlist *A_Gate;
  Analog_32_action Act;
  long FORLIM;
  log_grec *WITH;
  long FORLIM1;
  AnaInstpinrec *WITH1;

  if (Erase)
    Act.action = Analog_act_clear;
  else
    Act.action = Analog_act_set;
  FORLIM = AnaLogglobals->numpages;
  for (Pages = 0; Pages < FORLIM; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	FORLIM1 = WITH->kind->numpins;
	for (Index = 0; Index < FORLIM1; Index++) {
	  if (A_Kind->plist_cell[Index].active &&
	      WITH->pin[Index]->simtype->simtype == 32) {
	    A_Gate = (Anainstlist *)Count->info;
	    WITH1 = &A_Gate->Pininfo[Index];
	    if (Erase)
	      WITH1->Vreset = AnaNotyet;
	    else
	      WITH1->Vreset = WITH1->Vmem;
	  }
	}
	Act.inst = Count;
	(*A_Kind->proc_cell)(&Act);
      }
      Count = Count->next;
    }
  }
}




/*******************************************************************************/
/*            Procedure For Changing Scoreboard Message                        */

Void AnaScoreboard(Newmessage_, purpose)
Char *Newmessage_;
long purpose;
{

  /*******************************************************************************/
  Char Newmessage[256];
  Analog_32_action Act;
  AnaCell_rec *A_Kind;
  log_grec *GCount;
  long Pages, Numpages;
  log_grec *WITH;

  strcpy(Newmessage, Newmessage_);
  Numpages = AnaLogglobals->numpages;
  Pages = 1;
  while (Pages <= Numpages) {
    GCount = AnaLogglobals->gbase[Pages - 1];
    while (GCount != NULL) {
      WITH = GCount;
      if (WITH->kind->simtype->simtype == 32) {
	Act.action = Analog_act_message;
	Act.page = Pages;
	Act.inst = GCount;
	strcpy(Act.message, Newmessage);
	Act.messagenum = purpose;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	(*A_Kind->proc_cell)(&Act);
      }
      GCount = GCount->next;
    }
    Pages++;
  }
}


/********************************************************************************/
/*     Procedure for handling LOG.CNF calls                                     */

Void AnaGetcommand(Keyword, Arg, Found)
Char *Keyword;
Char *Arg;
boolean *Found;
{
  /*Key to search under*/
  /*command if found*/
  /*Reports finding of command*/
  AnaCommand_rec *Count, *Last;

  Count = AnaClist;
  *Found = false;
  Last = NULL;
  while (Count != NULL && !*Found) {
    if (strcmp(Count->Command, Keyword)) {
      Last = Count;
      Count = Count->Next;
      continue;
    }
    *Found = true;
    strcpy(Arg, Count->Arguments);
    if (Last == NULL)
      AnaClist = Count->Next;
    else
      Last->Next = Count->Next;
    Free(Count);
  }
}


Void AnaAddcommand(Keyword, Arg)
Char *Keyword;
Char *Arg;
{
  /*Key to search under*/
  /*command if found*/
  AnaCommand_rec *Newnode;


  Newnode = (AnaCommand_rec *)Malloc(sizeof(AnaCommand_rec));
  strcpy(Newnode->Command, Keyword);
  strcpy(Newnode->Arguments, Arg);
  Newnode->Next = AnaClist;
  AnaClist = Newnode;
}


Void AnaPopcommand()
{
  AnaCommand_rec *Temp;

  if (AnaClist == NULL)
    return;
  Temp = AnaClist;
  AnaClist = AnaClist->Next;
  Free(Temp);
}


/******************************************************************************/
/*         Utility procedure for data input                                   */
/******************************************************************************/

Void AnaGetvar(Varname, Value)
Char *Varname;
double *Value;
{
  long Temp;
  Char Name[256];
  double Newval;
  Char *STR3;

  m_alpha_on();
  m_graphics_on();
  printf("\f");
  printf("%s[% .5E]:", Varname, *Value);
  gets(Name);
  TRY(try1);
    Newval = strtod(Name, &STR3);
    Temp = STR3 - Name + 1;
    *Value = Newval;
  RECOVER(try1);
    ;
  ENDTRY(try1);
}


/*==============================================================================*/
/*       The following procedures handle the watchdog timer  (new)              */
/*==============================================================================*/

/*$if UsingAlarm$

{****************************************************************************}
{                       The watchdog timer                                     }
{****************************************************************************}


   Procedure Watchdog(T:Timers_ptr);

    Begin
     Quit:=True;
     Timers_Remove(T^);
    End;

{******************************************************************************}
{               Procedure For resetting watchdog count                         }
{******************************************************************************}


  Procedure Timeinit;


   Begin
    Logglobals^.acttool^.nexttstep:=0;
    Quit:=False;
    T1.rate:=Logglobals^.fastspeed;        {Used to be Simtime}
    T1.count:=T1.rate;
    T1.proc:=watchdog;
    timers_add(T1);
   End;

$end$*/

/*=============================================================================*/
/*   Mathmatical library of transcendental functions for cell library          */
/*=============================================================================*/

double AnaTanch(V)
double V;
{
  double Temp;

  if (V > 5)
    return 1.0;
  else {
    if (V < -5)
      return -1.0;
    else {
      if (fabs(V) < 0.001)
	return V;
      else {
	Temp = exp(2 * V);
	return ((Temp - 1) / (Temp + 1));
      }
    }
  }
}


double AnaExpo(V)
double V;
{
  if (V < -700)
    return 0.0;
  else
    return exp(V);
}

double AnaSqrt(V)
double V;
{
  if (V < 0)
    return 0.0;
  else
    return sqrt(V);
}


/*=============================================================================*/
/*     General purpose procedures for attribute range checking                */
/*=============================================================================*/

boolean AnaVrange(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean AnaResult;

  AnaResult = true;
  if (!Inst->attr[Attrnum - 1].blnk) {
    if (Inst->attr[Attrnum - 1].UU.r > AnaHighest ||
	Inst->attr[Attrnum - 1].UU.r < AnaLowest)
      return false;
  }
  return AnaResult;
}


boolean AnaCrange(Inst, Attrnum, OkZero)
log_grec *Inst;
long Attrnum;
boolean OkZero;
{
  boolean AnaResult;

  AnaResult = true;
  if (OkZero) {
    if (Inst->attr[Attrnum - 1].blnk ||
	(unsigned)Inst->attr[Attrnum - 1].UU.r > AnaCapTooBig)
      return false;
  } else {
    if (Inst->attr[Attrnum - 1].blnk || Inst->attr[Attrnum - 1].UU.r <
	  AnaCapTooSmall || Inst->attr[Attrnum - 1].UU.r > AnaCapTooBig)
      return false;
  }
  return AnaResult;
}


boolean AnaDisplayrange(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean AnaResult;

  AnaResult = true;
  if (!Inst->attr[Attrnum - 1].blnk && (Inst->attr[Attrnum - 1].UU.U73.i1 < 1 ||
	Inst->attr[Attrnum - 1].UU.U73.i1 > 3))
    return false;
  return AnaResult;
}



boolean AnaGate_On_Display(Inst)
log_grec *Inst;
{
  boolean AnaResult;
  log_grec *GCount;
  long Showpage;
  boolean Found;

  AnaResult = false;
  Showpage = AnaLogglobals->showpage;
  if (Showpage <= 0)
    return AnaResult;
  Found = false;
  GCount = AnaLogglobals->gbase[Showpage - 1];
  while (GCount != NULL && !Found) {
    if (GCount == Inst) {
      Found = true;
      AnaResult = true;
    } else
      GCount = GCount->next;
  }
  return AnaResult;
}


/*=============================================================================*/
/*        General purpose library to handle pin to ground capacitance          */
/*=============================================================================*/

Void AnaCapInit(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaInstpinrec *WITH1;

  A_Gate = (Anainstlist *)inst->info;
  Numpins = inst->kind->numpins;
  A_Gate->Pininfo = (AnaInstpinrec *)Malloc(sizeof(AnaInstpinrec) * Numpins);
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    WITH1->Cap = AnaNodecap;
    WITH1->Tau = 0.0;
    WITH1->Vmem = AnaNotyet;
    WITH1->Vreset = AnaNotyet;
  }
}



Void AnaCapCopy(inst)
log_grec *inst;
{
  Anainstlist *A_Gate, *A_Oldgate;
  long Index, Numpins;
  AnaInstpinrec *WITH1;

  A_Gate = (Anainstlist *)inst->info;
  Numpins = inst->kind->numpins;
  A_Gate->Pininfo = (AnaInstpinrec *)Malloc(sizeof(AnaInstpinrec) * Numpins);
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    WITH1->Cap = A_Oldgate->Pininfo[Index].Cap;
    WITH1->Tau = A_Oldgate->Pininfo[Index].Tau;
    WITH1->Vmem = A_Oldgate->Pininfo[Index].Vmem;
    WITH1->Vreset = A_Oldgate->Pininfo[Index].Vreset;
  }
}


Void AnaCapDispose(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Numpins;

  A_Gate = (Anainstlist *)inst->info;
  Numpins = inst->kind->numpins;
  Free(A_Gate->Pininfo);
}


Void AnaCapReset(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaInstpinrec *WITH1;

  A_Gate = (Anainstlist *)inst->info;
  Numpins = inst->kind->numpins;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    WITH1->Vmem = WITH1->Vreset;
  }
}


Void AnaCappass1(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaExt_rec *Extnodeptr;
  AnaInstpinrec *WITH1;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    if (WITH1->Vmem != AnaNotyet) {   /*If an initial value exists*/
      Extnodeptr = (AnaExt_rec *)inst->pin[Index]->info;
      if (Extnodeptr->ltimestep == AnaNotyet)
	Extnodeptr->ltimestep = WITH1->Vmem;
    }
  }
}


Void AnaCappass2(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaExt_rec *Extnodeptr;
  AnaInstpinrec *WITH1;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    Extnodeptr = (AnaExt_rec *)inst->pin[Index]->info;
    WITH1 = &A_Gate->Pininfo[Index];
    if (WITH1->Vmem != AnaNotyet) {   /*If Vmem full, guess if neccessary*/
      if (Extnodeptr->ltimestep == AnaNotyet)
	Extnodeptr->ltimestep = WITH1->Vmem;
    } else {  /*If Vmem empty,fill it somehow*/
      if (Extnodeptr->ltimestep != AnaNotyet)
	WITH1->Vmem = Extnodeptr->ltimestep;
      else {
	WITH1->Vmem = 0.0;
	Extnodeptr->ltimestep = AnaSmall;
      }
    }
  }
}


Void AnaCapUpdate(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaExt_rec *Extnodeptr;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    Extnodeptr = (AnaExt_rec *)inst->pin[Index]->info;
    A_Gate->Pininfo[Index].Vmem = Extnodeptr->now;
  }
}


Void AnaCapex(inst)
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaExt_rec *Extnodeptr;
  AnaInstpinrec *WITH1;
  AnaMatRec *WITH3;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    Extnodeptr = (AnaExt_rec *)inst->pin[Index]->info;
	/*Vmem,Last,Cap,Nodenum,SysCol*/
    WITH1 = &A_Gate->Pininfo[Index];
    if (Extnodeptr->nodenum != 0) {   /*For constant Vdd,Gnd only!!!*/
      if (AnaChangedDt)
	WITH1->Tau = WITH1->Cap / AnaDt;
      WITH3 = AnaSystem;
      WITH3->Mat[Extnodeptr->nodenum]
	[AnaSysCol] += WITH1->Tau * (WITH1->Vmem - Extnodeptr->last);
      WITH3->Mat[Extnodeptr->nodenum][Extnodeptr->nodenum] += WITH1->Tau;
      WITH3->MatNZ[Extnodeptr->nodenum][AnaSysCol] = true;
      WITH3->MatNZ[Extnodeptr->nodenum][Extnodeptr->nodenum] = true;
    } else
      WITH1->Tau = 0.0;
  }
}


Void AnaCapIin(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Extnodeptr;
  AnaInstpinrec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Extnodeptr = (AnaExt_rec *)act->inst->pin[act->pin - 1]->info;
  WITH1 = &A_Gate->Pininfo[act->pin - 1];
  act->Iin += WITH1->Tau * (Extnodeptr->now - WITH1->Vmem);
}


Void AnaCapattrload(base, inst, first)
long base;
log_grec *inst;
boolean first;
{
  Anainstlist *A_Gate;
  long Index, Address, Numpins;
  AnaInstpinrec *WITH1;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    Address = base + Index * 3;
    if (inst->attr[Address - 1].UU.r != WITH1->Vmem) {
      inst->attr[Address - 1].UU.r = WITH1->Vmem;
      inst->attr[Address - 1].changed = true;
    }
    if (first) {
      inst->attr[Address].UU.r = WITH1->Vreset;
      inst->attr[Address + 1].UU.r = WITH1->Cap;
      inst->attr[Address].changed = true;
      inst->attr[Address + 1].changed = true;
    }
  }
  for (Index = -1; Index <= Numpins * 3 - 2; Index++) {
    if (inst->attr[base + Index].UU.r == AnaNotyet)
      inst->attr[base + Index].blnk = true;
    else
      inst->attr[base + Index].blnk = false;
  }
}


Void AnaCapattrwrite(base, inst)
long base;
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaInstpinrec *WITH1;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    fprintf(*AnaLogglobals->actfile, "% .5E\n", WITH1->Vmem);
    fprintf(*AnaLogglobals->actfile, "% .5E\n", WITH1->Vreset);
    fprintf(*AnaLogglobals->actfile, "% .5E\n", WITH1->Cap);
  }
}


Void AnaCapattrread(base, inst)
long base;
log_grec *inst;
{
  Anainstlist *A_Gate;
  long Index, Numpins;
  AnaInstpinrec *WITH1;

  Numpins = inst->kind->numpins;
  A_Gate = (Anainstlist *)inst->info;
  for (Index = 0; Index < Numpins; Index++) {
    WITH1 = &A_Gate->Pininfo[Index];
    fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &WITH1->Vmem);
    getc(*AnaLogglobals->actfile);
    fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &WITH1->Vreset);
    getc(*AnaLogglobals->actfile);
    fscanf(*AnaLogglobals->actfile, "%lg%*[^\n]", &WITH1->Cap);
    getc(*AnaLogglobals->actfile);
  }
}


Static Void NodevalSet(Setnode, Value, ResetVal)
log_nrec *Setnode;
double Value;
boolean ResetVal;
{
  AnaCell_rec *A_Kind;
  Anainstlist *A_Gate;
  long Index;
  log_grec *Count;
  long Pages, Numpages;
  boolean Erased;
  log_grec *WITH;
  long FORLIM1;

  Numpages = AnaLogglobals->numpages;
  Erased = (ResetVal && Value == AnaNotyet);
  for (Pages = 0; Pages < Numpages; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	FORLIM1 = WITH->kind->numpins;
	for (Index = 0; Index < FORLIM1; Index++) {
	  if (WITH->pin[Index]->simtype->simtype == 32 &&
	      A_Kind->plist_cell[Index].active) {
	    A_Gate = (Anainstlist *)Count->info;
	    if (WITH->pin[Index] == Setnode) {
	      if (ResetVal)
		A_Gate->Pininfo[Index].Vreset = Value;
	      else
		A_Gate->Pininfo[Index].Vmem = Value;
	    }
	    if (Erased)
	      Erased = (A_Gate->Pininfo[Index].Vreset == AnaNotyet);
	  }
	}
      }
      Count = Count->next;
    }
  }
  if (Erased) {
    AnaErase = true;
    AnaScoreboard("#", (long)AnaMessFlags);
  }
}



Void AnaCapattrinsert(base, attrnum, inst, Ok)
long base, attrnum;
log_grec *inst;
boolean *Ok;
{
  Anainstlist *A_Gate;
  long Nodenum, Param;
  AnaExt_rec *CapNodeptr;


  *Ok = true;
  A_Gate = (Anainstlist *)inst->info;
  Nodenum = (attrnum - base) / 3 + 1;
  Param = (attrnum - base) % 3;
/* p2c: globals.text, line 1450:
 * Note: Using % for possibly-negative arguments [317] */
  switch (Param) {

  case 0:
    if (!inst->attr[attrnum - 1].blnk &&
	(inst->attr[attrnum - 1].UU.r < AnaLowest ||
	 inst->attr[attrnum - 1].UU.r > AnaHighest))
      *Ok = false;
    else {
      CapNodeptr = (AnaExt_rec *)inst->pin[Nodenum - 1]->info;
      if (inst->attr[attrnum - 1].blnk) {
	NodevalSet(inst->pin[Nodenum - 1], (double)AnaNotyet, false);
	CapNodeptr->ltimestep = AnaNotyet;
      } else {
	NodevalSet(inst->pin[Nodenum - 1], inst->attr[attrnum - 1].UU.r,
		   false);
	CapNodeptr->ltimestep = inst->attr[attrnum - 1].UU.r;
      }
      AnaCapattrload(base, inst, true);
    }
    break;

  case 1:
    if (!inst->attr[attrnum - 1].blnk &&
	(inst->attr[attrnum - 1].UU.r < AnaLowest ||
	 inst->attr[attrnum - 1].UU.r > AnaHighest))
      *Ok = false;
    else {
      if (inst->attr[attrnum - 1].blnk)
	NodevalSet(inst->pin[Nodenum - 1], (double)AnaNotyet, true);
      else {
	NodevalSet(inst->pin[Nodenum - 1], inst->attr[attrnum - 1].UU.r, true);
	AnaErase = false;
	AnaScoreboard("#", (long)AnaMessFlags);
      }
      AnaCapattrload(base, inst, true);
    }
    break;

  case 2:
    if (inst->attr[attrnum - 1].blnk || inst->attr[attrnum - 1].UU.r <
	  AnaCapTooSmall || inst->attr[attrnum - 1].UU.r > AnaCapTooBig)
      *Ok = false;
    else {
      inst->attr[attrnum - 1].changed = true;
      A_Gate->Pininfo[Nodenum - 1].Cap = inst->attr[attrnum - 1].UU.r;
    }
    break;
  }
}


Static Void Dataout(data)
Char *data;
{
  if (AnaLogglobals->traceflag)
    fputs(data, *AnaLogglobals->tracefile);
}


Static Void Linefeed()
{
  if (AnaLogglobals->traceflag)
    putc('\n', *AnaLogglobals->tracefile);
}


Static Void Openfiles()
{
  if (!AnaLogglobals->traceopen)
    (*AnaLogglobals->hook.trace)("anaLOG opening tracefile");
}


Void AnaMatdump()
{
  long Row, Col, Dump;
  Char Newdata[256];
  Char STR3[256];
  long FORLIM, FORLIM1;
  AnaMatRec *WITH;

  if (!AnaLogglobals->traceflag)
    return;
  Openfiles();
  sprintf(STR3, "Iteration # %ld Timestep %g", AnaItercount, AnaDt);
  Dataout(STR3);
  Linefeed();
  FORLIM = AnaSysRow;
  for (Row = 1; Row <= FORLIM; Row++) {
    FORLIM1 = AnaSysCol;
    for (Col = 1; Col <= FORLIM1; Col++) {
      *Newdata = '\0';
      WITH = AnaSystem;
      if (WITH->Mat[Row][Col] >= 0) {
	sprintf(Newdata, "+% .*E ",
		P_max((int)(AnaDebugsize - 1) - 7, 1), WITH->Mat[Row][Col]);
	Dump = strlen(Newdata) + 1;
      } else {
	sprintf(Newdata, "% .*E ",
		P_max((int)AnaDebugsize - 7, 1), WITH->Mat[Row][Col]);
	Dump = strlen(Newdata) + 1;
      }
      Dataout(Newdata);
    }
    Linefeed();
  }
  Linefeed();
  Linefeed();
  FORLIM = AnaSysRow;
  for (Row = 1; Row <= FORLIM; Row++) {
    FORLIM1 = AnaSysCol;
    for (Col = 1; Col <= FORLIM1; Col++) {
      *Newdata = '\0';
      WITH = AnaSystem;
      if (WITH->MatNZ[Row][Col]) {
	sprintf(Newdata, "F");
	Dump = strlen(Newdata) + 1;
      } else {
	sprintf(Newdata, "Z");
	Dump = strlen(Newdata) + 1;
      }
      Dataout(Newdata);
    }
    Linefeed();
  }
  Linefeed();
}


Static Void PrintResult(Rownum)
long Rownum;
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;
  boolean Found;
  long Dump;
  Char Newdata[256];
  AnaMatRec *WITH1;

  Ext_Count = AnaLogglobals->nbase;
  Found = false;
  while (Ext_Count != NULL && !Found) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      if (A_Node->nodenum == Rownum) {
	Found = true;
	*Newdata = '\0';
	WITH1 = AnaSystem;
	if (A_Node->last + WITH1->Mat[A_Node->nodenum][AnaSysCol] >= 0) {
	  sprintf(Newdata, " +% .*E ",
		  P_max((int)(AnaDebugsize - 1) - 7, 1),
		  A_Node->last + WITH1->Mat[A_Node->nodenum][AnaSysCol]);
	  Dump = strlen(Newdata) + 1;
	} else {
	  sprintf(Newdata, " % .*E ",
		  P_max((int)AnaDebugsize - 7, 1),
		  A_Node->last + WITH1->Mat[A_Node->nodenum][AnaSysCol]);
	  Dump = strlen(Newdata) + 1;
	}
	Dataout(Newdata);
      }
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL && !Found) {
    if (Int_Count->nodenum == Rownum) {
      Found = true;
      *Newdata = '\0';
      WITH1 = AnaSystem;
      if (Int_Count->last + WITH1->Mat[Int_Count->nodenum][AnaSysCol] >= 0) {
	sprintf(Newdata, " +% .*E ",
		P_max((int)(AnaDebugsize - 1) - 7, 1),
		Int_Count->last + WITH1->Mat[Int_Count->nodenum][AnaSysCol]);
	Dump = strlen(Newdata) + 1;
      } else {
	sprintf(Newdata, " % .*E ",
		P_max((int)AnaDebugsize - 7, 1),
		Int_Count->last + WITH1->Mat[Int_Count->nodenum][AnaSysCol]);
	Dump = strlen(Newdata) + 1;
      }
      Dataout(Newdata);
    }
    Int_Count = Int_Count->next;
  }
}


Void AnaDiffdump()
{
  long Row, Col, Dump;
  Char Newdata[256];
  long FORLIM, FORLIM1;
  AnaMatRec *WITH;

  if (!AnaLogglobals->traceflag)
    return;
  Openfiles();
  FORLIM = AnaSysRow;
  for (Row = 1; Row <= FORLIM; Row++) {
    FORLIM1 = AnaSysCol;
    for (Col = 1; Col <= FORLIM1; Col++) {
      *Newdata = '\0';
      WITH = AnaSystem;
      if (WITH->Mat[Row][Col] >= 0) {
	sprintf(Newdata, "+% .*E ",
		P_max((int)(AnaDebugsize - 1) - 7, 1), WITH->Mat[Row][Col]);
	Dump = strlen(Newdata) + 1;
      } else {
	sprintf(Newdata, "% .*E ",
		P_max((int)AnaDebugsize - 7, 1), WITH->Mat[Row][Col]);
	Dump = strlen(Newdata) + 1;
      }
      Dataout(Newdata);
    }
    if (!AnaError)
      PrintResult(Row);
    Linefeed();
  }
  FORLIM = AnaSysRow;
  for (Row = 1; Row <= FORLIM; Row++) {
    FORLIM1 = AnaSysCol;
    for (Col = 1; Col <= FORLIM1; Col++) {
      *Newdata = '\0';
      WITH = AnaSystem;
      if (WITH->MatNZ[Row][Col]) {
	sprintf(Newdata, "F");
	Dump = strlen(Newdata) + 1;
      } else {
	sprintf(Newdata, "Z");
	Dump = strlen(Newdata) + 1;
      }
      Dataout(Newdata);
    }
    Linefeed();
  }
  if (AnaError)
    Dataout("(Matrix aborted)>");
  else
    Dataout("(Matrix solved)>");
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*    The code for a 'safe but slow' way of doing timouts
      The use requires commenting out variable 'Quit' and adding
      the following variable declaration. In addition, comment out
      the watchdog and timinit procedures and add the following                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*==============================================================================*/
/*       The following procedures handle the watchdog timer  (old)              */
/*==============================================================================*/



/******************************************************************************/
/*                 Old Procedure for checking timer                           */
/******************************************************************************/

int stepcount = 0;


boolean AnaQuit()
{
  boolean AnaResult;

  AnaResult = false;
  stepcount++;
  if (timers_sysclock() > AnaStoptime) {
   /* printf("Count = %d\n", stepcount); */
    stepcount = 0;
    return true;
  } else {
    return false;
  }
}


/********************************************************************************/
/*         Old Procedure For resetting watchdog count                           */
/********************************************************************************/


Void AnaTimeinit()
{
  AnaLogglobals->acttool->nexttstep = 0.0;
  AnaStoptime = timers_sysclock() + AnaLogglobals->fastspeed;
  stepcount = 0;
}

/********************************************************************************/
/*         Updates calculated parameters after changes                          */
/********************************************************************************/

Void AnaParamUpdate()
{
  AnaPhysical.Ut = (AnaPhysical.k*AnaPhysical.T)/AnaPhysical.q;

  if (AnaPhysical.T < 150)
    AnaGenFab.Eg = (1.1700 + 1.059e-5*AnaPhysical.T - 
	  	      6.05e-7*AnaPhysical.T*AnaPhysical.T)*AnaPhysical.q;
  else
    AnaGenFab.Eg = (1.1785 - 9.025e-5*AnaPhysical.T - 
	  	      3.05e-7*AnaPhysical.T*AnaPhysical.T)*AnaPhysical.q;
  AnaGenFab.ni = 1.640e15*pow(AnaPhysical.T,1.706)*
                 exp(-AnaGenFab.Eg/(2*AnaPhysical.k*AnaPhysical.T));
    
  AnaNFab.Cox = AnaPhysical.e_ox/AnaNFab.Tox;
  AnaPFab.Cox = AnaPhysical.e_ox/AnaPFab.Tox;

  AnaNFab.mu = AnaNFab.del_mu*AnaNFab.mu0*pow((AnaPhysical.T/300),-2.42);
  AnaPFab.mu = AnaPFab.del_mu*AnaPFab.mu0*pow((AnaPhysical.T/300),-2.30);

}



/* End. */
