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
/* From input file "inter.text" */




#include <p2c/p2c.h>


#define INTER_G
#include "inter.h"



Static Void log_histinit_32(Globals, Voltage)
log_action *Globals;
boolean Voltage;
{
  na_strlist *Dummy;

  if (Voltage) {
    Dummy = strlist_append(&Globals->acttool->hlbl,
	"VCyan,Orange,Yellow,Pink,Green,Red,White,Medium Red,Light Gray,Dark Cyan,Dark Yellow,Dark Red:Color");
    Dummy = strlist_append(&Globals->acttool->hlbl, "BY:Phosphor Visible?");
    Dummy = strlist_append(&Globals->acttool->hlbl, "I3:Significant Digits:");
    Dummy = strlist_append(&Globals->acttool->hlbl,
			   "VLinear,Log:Scaling method:");
    Dummy = strlist_append(&Globals->acttool->hlbl,
			   "Linear;UV,5:Volts/Division:");
    Dummy = strlist_append(&Globals->acttool->hlbl,
			   "Linear;UV,0:Vertical Origin");
    Dummy = strlist_append(&Globals->acttool->hlbl, "Log;UV,1e-15:Vzero:");
    Dummy = strlist_append(&Globals->acttool->hlbl,
			   "Log;UDiv,0:Vertical Origin");
    Dummy = strlist_append(&Globals->acttool->hlbl, "");
    Dummy = strlist_append(&Globals->acttool->hlbl, "3UV:Present Voltage:");
    return;
  }
  Dummy = strlist_append(&Globals->acttool->hlbl,
      "VCyan,Orange,Yellow,Pink,Green,Red,White,Medium Red,Light Gray,Dark Cyan,Dark Yellow,Dark Red:Color");
  Dummy = strlist_append(&Globals->acttool->hlbl, "BY:Phosphor Visible?");
  Dummy = strlist_append(&Globals->acttool->hlbl, "I3:Significant Digits:");
  Dummy = strlist_append(&Globals->acttool->hlbl,
			 "VLinear,Log:Scaling method:");
  Dummy = strlist_append(&Globals->acttool->hlbl,
			 "Linear;UA,5e-9:Amps/Division:");
  Dummy = strlist_append(&Globals->acttool->hlbl,
			 "Linear;UA,0:Vertical Origin");
  Dummy = strlist_append(&Globals->acttool->hlbl, "Log;UA,1e-15:Izero:");
  Dummy = strlist_append(&Globals->acttool->hlbl, "Log;UDiv,0:Vertical Origin");
  Dummy = strlist_append(&Globals->acttool->hlbl, "");
  Dummy = strlist_append(&Globals->acttool->hlbl, "3UA:Present Current:");
}



Static Void log_init_32(act)
log_action *act;
{
  log_tool *WITH;

  AnaLogglobals = act;
  WITH = AnaLogglobals->acttool;
  WITH->ready = true;
  WITH->simulator = true;
  WITH->shortname = (Char *)Malloc(33);
  strcpy(WITH->shortname, "analog");
  WITH->comment = (Char *)Malloc(256);
  strcpy(WITH->comment, "anaLOG - a functional analog simulator");
  log_histinit_32(AnaLogglobals, true);
  Analog_InitVar();
}


Static Void log_edit_32(act)
log_action *act;
{
  Char Ch;
  long Index;
  Char Response[256];
  Char Newname[9];
  Char STR1[256];
  Char STR2[256];
  long FORLIM;

  strcpy(Newname, act->actstr);
  printf("\f");
  do {
    printf("Default[%s]? (y,n) ", Newname);
    Ch = nk_getkey();
  } while (Ch != 'Y' && Ch != 'y' && Ch != 'N' && Ch != 'n');
  if (Ch == 'Y' || Ch == 'y') {
    strcpy(STR1, strltrim(Newname));
    strcpy(Newname, STR1);
    strcpy(Newname, strrtrim(strcpy(STR2, Newname)));
    sprintf(Response, "%s_LOG_%s_INITLIB_32", Newname, Newname);
    FORLIM = strlen(Response);
    for (Index = 0; Index < FORLIM; Index++)
      act->actproc[Index] = Response[Index];
    act->actproc[strlen(Response)] = '\0';
  }
  if (Ch == 'N' || Ch == 'n') {
    putchar('\n');
    do {
      printf("<modulename>_<Procname> : ");
      gets(Response);
    } while (strlen(Response) < 1);
    strcpy(STR1, strltrim(Response));
    strcpy(Response, STR1);
    strcpy(Response, strrtrim(strcpy(STR2, Response)));
    FORLIM = strlen(Response);
    for (Index = 0; Index < FORLIM; Index++)
      act->actproc[Index] = Response[Index];
    act->actproc[strlen(Response)] = '\0';
  }
  printf("\f");
}


Static Void log_dump_32(act)
log_action *act;
{
  strlist_append(&act->actstrlist, (char *)act->actproc);
}


Static Void log_read_32(act)
log_action *act;
{
  strcpy((char *)act->actproc, act->actstrlist->s);
}


Static Void log_pass_32()
{
  short Newgatenum;
  Char STR1[256];

  if (Anagndsig == NULL) {
    (*AnaLogglobals->hook.getsig)("Gnd", &Anagndsig);
    (*AnaLogglobals->hook.getgate)("TIME", &Newgatenum);
    (*AnaLogglobals->hook.uaddgate)((int)(m_across - 61), 5, Newgatenum);
    (*AnaLogglobals->hook.getgate)("NUMBERS", &Newgatenum);
    (*AnaLogglobals->hook.uaddgate)((int)(m_across - 61), 110, Newgatenum);
  }
  Anagndnode = Anagndsig->np;
  if (AnaVddsig == NULL)
    (*AnaLogglobals->hook.getsig)("Vdd", &AnaVddsig);
  AnaVddnode = AnaVddsig->np;
  if (AnaLogglobals->probemode != AnaProbeOn) {
    AnaProbeOn = AnaLogglobals->probemode;
    AnaScoreboard("#", (long)AnaMessFlags);
  }
  if (AnaProbeOn)
    Analog_Probemode();
  if (AnaInuse)
    AnaLogglobals->acttool->nexttstep = 0.0;
  if (AnaLogglobals->resetflag) {
    Analog_Reset();
    if (AnaTimermode) {
      AnaTimerstate = true;
      AnaTimerval = timers_sysclock();
    }
  }
  if (AnaTimermode && AnaTimerstate && !AnaLogglobals->pwrflag) {
    AnaTimerval = timers_sysclock() - AnaTimerval;
    AnaTimerstate = false;
    sprintf(STR1, "Timer:%ldcsec", AnaTimerval);
    (*AnaLogglobals->hook.vmessage)(STR1);
  }
  if (AnaLogglobals->refrflag) {
    Analog_Refresh();
    return;
  }
  if (!AnaLogglobals->pwrflag)
    return;
  if (AnaReady && !AnaChanged)
    AnaLogglobals->busyflag = true;

  switch (AnaChanged) {

  case true:
    if (!AnaReady)
      Analog_Setup();
    else
      Analog_Cleanup();
    break;

  case false:
    if (AnaReady)
      Analog_Simulate();
    break;
  }
}



Static Void log_egate_32()
{
  AnaCell_rec *A_Kind;
  Analog_32_action act;
  log_grec *g;

  g = AnaLogglobals->actgate;
  A_Kind = (AnaCell_rec *)g->kind->info;
  act.action = Analog_act_erase;
  act.inst = g;
  (*A_Kind->proc_cell)(&act);
}


Static Void log_newgate_32()
{
  long Intnodes, Index;
  Anainstlist *Newgate;
  AnaGate_intnoderec *Count;
  AnaCell_rec *Gkind;
  AnaInt_rec *Newnode;
  log_grec *g;
  Analog_32_action act;
  AnaCell_rec *A_Kind;

  g = AnaLogglobals->actgate;
  Gkind = (AnaCell_rec *)g->kind->info;
  Intnodes = Gkind->intr_cell;
  Newgate = (Anainstlist *)Malloc(sizeof(Anainstlist));
  if (Intnodes == 0)
    Newgate->Gate_nodelist = NULL;
  else {
    Newgate->Gate_nodelist = (AnaGate_intnoderec *)
			     Malloc(sizeof(AnaGate_intnoderec));
    Count = Newgate->Gate_nodelist;
    for (Index = 1; Index <= Intnodes; Index++) {
      if (Index != Intnodes) {
	Count->Next = (AnaGate_intnoderec *)Malloc(sizeof(AnaGate_intnoderec));
	Newnode = (AnaInt_rec *)Malloc(sizeof(AnaInt_rec));
	Newnode->ltimestep = AnaNotyet;
	Newnode->last = 0.0;
	Newnode->now = 0.0;
	Newnode->next = AnaInt_nodelist;
	AnaInt_nodelist = Newnode;
	Count->Int_Node = Newnode;
	Count = Count->Next;
      } else {
	Newnode = (AnaInt_rec *)Malloc(sizeof(AnaInt_rec));
	Newnode->ltimestep = AnaNotyet;
	Newnode->last = 0.0;
	Newnode->now = 0.0;
	Newnode->next = AnaInt_nodelist;
	AnaInt_nodelist = Newnode;
	Count->Int_Node = Newnode;
	Count->Next = NULL;
      }
    }
  }
  g->info = (Anyptr)Newgate;
  act.action = Analog_act_newgate;
  act.inst = g;
  A_Kind = (AnaCell_rec *)act.inst->kind->info;
  (*A_Kind->proc_cell)(&act);
  AnaChanged = true;
}


Static Void log_copygate_32()
{
  long Intnodes, Index;
  Anainstlist *Newgate;
  AnaGate_intnoderec *Count;
  AnaCell_rec *Gkind;
  AnaInt_rec *Newnode;
  log_grec *g;
  Analog_32_action act;
  AnaCell_rec *A_Kind;

  g = AnaLogglobals->actgate;
  Gkind = (AnaCell_rec *)g->kind->info;
  Intnodes = Gkind->intr_cell;
  Newgate = (Anainstlist *)Malloc(sizeof(Anainstlist));
  if (Intnodes == 0)
    Newgate->Gate_nodelist = NULL;
  else {
    Newgate->Gate_nodelist = (AnaGate_intnoderec *)
			     Malloc(sizeof(AnaGate_intnoderec));
	/*Alter to allow internals to be copied*/
    Count = Newgate->Gate_nodelist;
    for (Index = 1; Index <= Intnodes; Index++) {
      if (Index != Intnodes) {
	Count->Next = (AnaGate_intnoderec *)Malloc(sizeof(AnaGate_intnoderec));
	Newnode = (AnaInt_rec *)Malloc(sizeof(AnaInt_rec));
	Newnode->ltimestep = AnaNotyet;
	Newnode->last = 0.0;
	Newnode->now = 0.0;
	Newnode->next = AnaInt_nodelist;
	AnaInt_nodelist = Newnode;
	Count->Int_Node = Newnode;
	Count = Count->Next;
      } else {
	Newnode = (AnaInt_rec *)Malloc(sizeof(AnaInt_rec));
	Newnode->ltimestep = AnaNotyet;
	Newnode->last = 0.0;
	Newnode->now = 0.0;
	Newnode->next = AnaInt_nodelist;
	AnaInt_nodelist = Newnode;
	Count->Int_Node = Newnode;
	Count->Next = NULL;
      }
    }
  }
  g->info = (Anyptr)Newgate;
  act.action = Analog_act_copygate;
  act.inst = g;
  A_Kind = (AnaCell_rec *)act.inst->kind->info;
  (*A_Kind->proc_cell)(&act);
  AnaChanged = true;
}


Static Void Node_remove(oldnode)
AnaInt_rec *oldnode;
{
  AnaInt_rec *Count, *Last;
  boolean Done;

  Done = false;
  Count = AnaInt_nodelist;
  Last = NULL;
  while (Count != NULL && !Done) {
    if (Count != oldnode) {
      Last = Count;
      Count = Count->next;
      continue;
    }
    if (Last != NULL) {
      Last->next = Count->next;
      Free(oldnode);
    } else {
      AnaInt_nodelist = AnaInt_nodelist->next;
      Free(oldnode);
    }
    Done = true;
  }
  if (!Done) {
    printf("Corrupted Internal Node List\n");
    _Escape(7);
  }
}


Static Void log_disposegate_32()
{
  Anainstlist *Oldgate;
  AnaGate_intnoderec *Count, *Oldnode;
  log_grec *g;
  Analog_32_action act;
  AnaCell_rec *A_Kind;

  g = AnaLogglobals->actgate;
  Oldgate = (Anainstlist *)g->info;
  if (Oldgate->Gate_nodelist != NULL) {
    Count = Oldgate->Gate_nodelist;
    do {
      Node_remove(Count->Int_Node);
      Oldnode = Count;
      Count = Count->Next;
      Free(Oldnode);
    } while (Count != NULL);
  }
  act.action = Analog_act_disposegate;
  act.inst = g;
  A_Kind = (AnaCell_rec *)act.inst->kind->info;
  (*A_Kind->proc_cell)(&act);
  Free(Oldgate);
  AnaChanged = true;
}


Static Void log_connectgate_32()
{
  AnaChanged = true;
}


Static Void log_disconnectgate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  AnaChanged = true;
  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_disconnect;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
}


Static Void log_configchgate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  Act.pin = AnaLogglobals->actx;
  Act.ok = AnaLogglobals->actflag;
  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_attrchange;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
  AnaLogglobals->actflag = Act.ok;
  AnaChanged = true;
}




Static Void log_newnode_32(Globals)
log_action *Globals;
{
  AnaExt_rec *Newnode;

  Newnode = (AnaExt_rec *)Malloc(sizeof(AnaExt_rec));
  Newnode->ltimestep = AnaNotyet;
  Newnode->last = 0.0;
  Newnode->now = 0.0;
  Newnode->col_ltimestep = AnaColormap.C_Glow1;
  Globals->actnode->info = (Anyptr)Newnode;
  Globals->actnode->changed = true;
  AnaChanged = true;
}


Static Void log_disposenode_32(Globals)
log_action *Globals;
{
  AnaExt_rec *Oldnode;

  Oldnode = (AnaExt_rec *)Globals->actnode->info;
  Free(Oldnode);
  AnaChanged = true;
}


Static Void log_copynode_32(Globals)
log_action *Globals;
{
  AnaExt_rec *Newnode, *Oldnode;

  Oldnode = (AnaExt_rec *)Globals->actnode2->info;
  Newnode = (AnaExt_rec *)Malloc(sizeof(AnaExt_rec));
  Newnode->ltimestep = Oldnode->ltimestep;
  Newnode->col_ltimestep = Oldnode->col_ltimestep;
  Newnode->last = 0.0;
  Newnode->now = 0.0;
  Globals->actnode->info = (Anyptr)Newnode;
  Globals->actnode->changed = true;
  AnaChanged = true;
}


Static Void log_combinenodes_32(Globals)
log_action *Globals;
{
  AnaExt_rec *Destnode, *Srcnode;

  Destnode = (AnaExt_rec *)Globals->actnode->info;
  Srcnode = (AnaExt_rec *)Globals->actnode2->info;
  if (Srcnode->ltimestep != AnaNotyet && Destnode->ltimestep == AnaNotyet) {
    Destnode->ltimestep = Srcnode->ltimestep;
    Destnode->col_ltimestep = Srcnode->col_ltimestep;
  }
  Globals->actnode->changed = true;
  AnaChanged = true;
}


Static Void log_color_32()
{
  (*AnaLogglobals->hook.getcolor)("VMETER", &AnaColormap.C_Vmeter,
				  log_dyellow);
  (*AnaLogglobals->hook.getcolor)("IMETER", &AnaColormap.C_Imeter, log_orange);
  (*AnaLogglobals->hook.getcolor)("SWITCHON", &AnaColormap.C_Switchon,
				  log_lgray);
  (*AnaLogglobals->hook.getcolor)("SWITCHOFF", &AnaColormap.C_Switchoff,
				  log_black);
  (*AnaLogglobals->hook.getcolor)("LIMITON", &AnaColormap.C_Limiton, log_red);
  (*AnaLogglobals->hook.getcolor)("PROBE", &AnaColormap.C_Probe, log_dyellow);
  (*AnaLogglobals->hook.getcolor)("GLOW1", &AnaColormap.C_Glow1, log_black);
  (*AnaLogglobals->hook.getcolor)("GLOW2", &AnaColormap.C_Glow2, log_dred);
  (*AnaLogglobals->hook.getcolor)("GLOW3", &AnaColormap.C_Glow3, log_mred);
  (*AnaLogglobals->hook.getcolor)("GLOW4", &AnaColormap.C_Glow4, log_red);
  (*AnaLogglobals->hook.getcolor)("GLOW5", &AnaColormap.C_Glow5, log_pink);
  (*AnaLogglobals->hook.getcolor)("GLOW6", &AnaColormap.C_Glow6, log_white);
}


Static long Variantmap(Variantcolor)
long Variantcolor;
{
  long Result;

  switch (Variantcolor) {

  case 0:
    Result = log_cyan;
    break;

  case 1:
    Result = log_orange;
    break;

  case 2:
    Result = log_yellow;
    break;

  case 3:
    Result = log_pink;
    break;

  case 4:
    Result = log_green;
    break;

  case 5:
    Result = log_red;
    break;

  case 6:
    Result = log_white;
    break;

  case 7:
    Result = log_mred;
    break;

  case 8:
    Result = log_lgray;
    break;

  case 9:
    Result = log_dcyan;
    break;

  case 10:
    Result = log_dyellow;
    break;

  case 11:
    Result = log_dred;
    break;
  }
  return Result;
}


Static Void log_history_32(Globals, Voltage)
log_action *Globals;
boolean Voltage;
{
  AnaExt_rec *Histnode;

  Histnode = (AnaExt_rec *)Globals->actnode->info;
  if (Voltage) {
    if (Histnode->refcnt > 0) {
      Globals->actval = Histnode->ltimestep;
      Globals->actgattr[AnaH_Present - 1].UU.r = Histnode->ltimestep;
      Globals->actgattr[AnaH_Present - 1].blnk = false;
      Globals->actgattr[AnaH_Present - 1].changed = true;
      return;
    }
    Globals->actflag = false;
    Globals->actgattr[AnaH_Present - 1].blnk = true;
    Globals->actgattr[AnaH_Present - 1].changed = true;
    return;
  }
  if (Histnode->ltimestep != AnaNotyet) {
    Globals->actval = Histnode->ltimestep;
    Globals->actgattr[AnaH_Present - 1].UU.r = Histnode->ltimestep;
    Globals->actgattr[AnaH_Present - 1].blnk = false;
    Globals->actgattr[AnaH_Present - 1].changed = true;
    return;
  }
  Globals->actflag = false;
  Globals->actgattr[AnaH_Present - 1].blnk = true;
  Globals->actgattr[AnaH_Present - 1].changed = true;
}


Static Void log_histval_32(Globals, Voltage)
log_action *Globals;
boolean Voltage;
{
  switch (Globals->actgattr[AnaH_Method - 1].UU.nv) {

  case AnaLinear:
    Globals->actval =
      (Globals->actval - Globals->actgattr[AnaH_LinOffset - 1].UU.r) /
      Globals->actgattr[AnaH_Scale - 1].UU.r;
    break;

  case AnaLog:
    if (fabs(Globals->actval) < Globals->actgattr[AnaH_Vzero - 1].UU.r)
      Globals->actval = -Globals->actgattr[AnaH_LogOffset - 1].UU.r;
    else
      Globals->actval =
	AnaInvLn10 *
	log(fabs(Globals->actval) / Globals->actgattr[AnaH_Vzero - 1].UU.r) -
	Globals->actgattr[AnaH_LogOffset - 1].UU.r;
    break;
  }
  if (Globals->actgattr[AnaH_PhosVis - 1].UU.b)
    Globals->acty = Variantmap((long)Globals->actgattr[AnaH_Color - 1].UU.nv);
  else
    Globals->acty = -1;
}


#define Delta           0
#define Absolute        1
#define Value           2
#define Slope           3


Static Void log_histstr_32(Globals, Voltage)
log_action *Globals;
boolean Voltage;
{
  double Temp;
  Char Unit[4];
  log_action *WITH;

  WITH = Globals;
  if (!WITH->actgattr[AnaH_PhosVis - 1].UU.b)
    return;
  if (Voltage)
    strcpy(Unit, "V");
  else
    strcpy(Unit, "A");
  switch (WITH->actgattr[AnaH_Method - 1].UU.nv) {

  case AnaLinear:
    switch (WITH->acty) {

    case Delta:
    case Slope:
      Temp = WITH->actval * WITH->actgattr[AnaH_Scale - 1].UU.r;
      break;

    case Absolute:
    case Value:
      Temp = WITH->actval * WITH->actgattr[AnaH_Scale - 1].UU.r +
	     WITH->actgattr[AnaH_LinOffset - 1].UU.r;
      break;
    }
    (*Globals->hook.realunit)(Temp,
			      (int)WITH->actgattr[AnaH_Sigdig - 1].UU.U73.i1,
			      Unit, WITH->actstr);
    break;

  case AnaLog:
    switch (WITH->acty) {

    case Delta:
      Temp = AnaExpo(AnaLn10 * WITH->actval);
      (*Globals->hook.realunit)(Temp,
	(int)WITH->actgattr[AnaH_Sigdig - 1].UU.U73.i1, "", WITH->actstr);
      strcat(WITH->actstr, " Ratio");
      break;

    case Absolute:
    case Value:
      if (WITH->actval <= -WITH->actgattr[AnaH_LogOffset - 1].UU.r)
	Temp = WITH->actgattr[AnaH_Vzero - 1].UU.r;
      else
	Temp = WITH->actgattr[AnaH_Vzero - 1].UU.r * AnaExpo(
	      AnaLn10 * (WITH->actval + WITH->actgattr[AnaH_LogOffset - 1].UU.r));
      (*Globals->hook.realunit)(Temp,
	(int)WITH->actgattr[AnaH_Sigdig - 1].UU.U73.i1, Unit, WITH->actstr);
      break;

    case Slope:
      Temp = AnaExpo(AnaLn10 * WITH->actval * WITH->actval2) / WITH->actval2;
      (*Globals->hook.realunit)(Temp,
	(int)WITH->actgattr[AnaH_Sigdig - 1].UU.U73.i1, "", WITH->actstr);
      strcat(WITH->actstr, " Ratio");
      break;
    }
    break;
  }
}

#undef Delta
#undef Absolute
#undef Value
#undef Slope



Static Void log_configchhist_32(Globals, voltage)
log_action *Globals;
boolean voltage;
{
  Globals->actflag = true;
  switch (Globals->actx) {

  case AnaH_Sigdig:
    if (Globals->actgattr[Globals->actx - 1].UU.U73.i1 <= 0 ||
	Globals->actgattr[Globals->actx - 1].blnk)
      Globals->actflag = false;
    break;

  case AnaH_Scale:
    if (Globals->actgattr[Globals->actx - 1].UU.r == 0 ||
	Globals->actgattr[Globals->actx - 1].blnk ||
	fabs(Globals->actgattr[AnaH_LinOffset - 1].UU.r) >
	fabs(Globals->actgattr[AnaH_Scale - 1].UU.r * 1e5))
      Globals->actflag = false;
    break;

  case AnaH_LinOffset:
    if (Globals->actgattr[Globals->actx - 1].blnk ||
	fabs(Globals->actgattr[AnaH_LinOffset - 1].UU.r) >
	fabs(Globals->actgattr[AnaH_Scale - 1].UU.r * 1e5))
      Globals->actflag = false;
    break;

  case AnaH_Vzero:
    if (Globals->actgattr[Globals->actx - 1].UU.r <= 0 ||
	Globals->actgattr[Globals->actx - 1].blnk)
      Globals->actflag = false;
    break;

  case AnaH_LogOffset:
    if (Globals->actgattr[Globals->actx - 1].blnk)
      Globals->actflag = false;
    break;
  }
}



Static Void log_confighist_32(Globals, voltage)
log_action *Globals;
boolean voltage;
{
  if (!Globals->histactive && !Globals->actgattr[AnaH_Present - 1].blnk) {
    Globals->actgattr[AnaH_Present - 1].blnk = true;
    Globals->actgattr[AnaH_Present - 1].changed = true;
  }
}



Static Void log_readgate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_readgate;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
}


Static Void log_writegate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_writegate;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
}


Static Void log_configgate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_openconfig;
  Act.ok = AnaLogglobals->actflag;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
}


Static Void log_touchgate_32()
{
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  Act.inst = AnaLogglobals->actgate;
  Act.action = Analog_act_touch;
  A_Kind = (AnaCell_rec *)Act.inst->kind->info;
  (*A_Kind->proc_cell)(&Act);
}


Static Void log_cnf_32()
{
  log_action *WITH;

  WITH = AnaLogglobals;
  AnaAddcommand(WITH->func, WITH->funcarg);
  (*WITH->hook.clearfunc)();
}


Static Void log_func_32()
{
  Analog_32_action AnaAct;
  boolean Found;
  log_action *WITH;

  WITH = AnaLogglobals;
  Analog_GlobalCommand(&Found, &AnaAct.accepted);
  if (!Found)
    return;
  (*WITH->hook.clearfunc)();
  if (!AnaAct.accepted)
    (*AnaLogglobals->hook.vmessage)("Bad syntax for ANAlog command");
}


Static Void log_writenode_32()
{
  AnaExt_rec *Writenode;

  if (!AnaLogglobals->actflag)   /*If Dump Mode*/
    return;
  Writenode = (AnaExt_rec *)AnaLogglobals->actnode->info;
  fprintf(*AnaLogglobals->actfile, "Nodenum   is %ld\n", Writenode->nodenum);
  fprintf(*AnaLogglobals->actfile, "Ltimestep is %g\n", Writenode->ltimestep);
  fprintf(*AnaLogglobals->actfile, "Last is      %g\n", Writenode->last);
  fprintf(*AnaLogglobals->actfile, "Now  is      %g\n", Writenode->now);
}


Static Void log_glowcol_32()
{
  AnaExt_rec *Glownode;

  Glownode = (AnaExt_rec *)AnaLogglobals->actnode->info;
  AnaLogglobals->actx = Glownode->col_ltimestep;
}


Void Log_32_proc(act)
log_action *act;
{
  switch (act->action) {

  case act_init:   /*Iversion*/
    log_init_32(act);
    break;

  case act_edit:
    switch (act->acty) {
    case 0:
      log_edit_32(act);
      break;
    case 1:
    case 2:
      log_dump_32(act);
      break;
    case 3:
      log_read_32(act);
      break;
    }
    break;

  case act_pass:   /*Iversion*/
    log_pass_32();
    break;

  case act_erasegate:
    log_egate_32();
    break;

  case act_newgate:
    log_newgate_32();
    break;

  case act_disposegate:
    log_disposegate_32();
    break;

  case act_copygate:   /*be suspicious*/
    log_copygate_32();
    break;

  case act_connectgate:
    log_connectgate_32();
    break;

  case act_disconnectgate:
    log_disconnectgate_32();
    break;

  case act_configchgate:
    log_configchgate_32();
    break;

  case act_newkind:
    Analog_Newkind(AnaLogglobals->actkind);
    break;

  case act_newnode:
    log_newnode_32(AnaLogglobals);
    break;

  case act_disposenode:
    log_disposenode_32(AnaLogglobals);
    break;

  case act_copynode:
    log_copynode_32(AnaLogglobals);
    break;

  case act_combinenodes:
    log_combinenodes_32(AnaLogglobals);
    break;

  case act_color:
    log_color_32();
    break;

  case act_tstep:
    Analog_Tstep();
    break;

  case act_history:
    log_history_32(AnaLogglobals, true);
    break;

  case act_histval:
    log_histval_32(AnaLogglobals, true);
    break;

  case act_histstr:
    log_histstr_32(AnaLogglobals, true);
    break;

  case act_configchhist:
    log_configchhist_32(AnaLogglobals, true);
    break;

  case act_confighist:
    log_confighist_32(AnaLogglobals, true);
    break;

  case act_readgate:
    log_readgate_32();
    break;

  case act_writegate:
    log_writegate_32();
    break;

  case act_configgate:
    log_configgate_32();
    break;

  case act_cnf:
    log_cnf_32();
    break;

  case act_endinit:
    Analog_Systeminit();
    Analog_TechnologyCnf();
    break;

  case act_touchgate:
    log_touchgate_32();
    break;

  case act_func:
    log_func_32();
    break;

  case act_status:
    Analog_Status();
    break;

  case act_combineintonode:
    AnaChanged = true;
    break;

  case act_writenode:
    log_writenode_32();
    break;

  case act_glowcol:
    log_glowcol_32();
    break;
  }
}


/******************************************************************************/
/*                Procedures to handle Type 33 Signals                        */
/******************************************************************************/

Static Void log_init_33(act)
log_action *act;
{
  log_tool *WITH;

  AnaLogIglobals = act;
  WITH = AnaLogIglobals->acttool;
  WITH->ready = true;
  WITH->simulator = true;
  WITH->shortname = (Char *)Malloc(33);
  strcpy(WITH->shortname, "current");
  WITH->comment = (Char *)Malloc(256);
  strcpy(WITH->comment, "Current simulator to enable current plotting");
  log_histinit_32(AnaLogIglobals, false);
}


Void Log_33_proc(act)
log_action *act;
{
  switch (act->action) {

  case act_init:
    log_init_33(act);
    break;

  case act_newnode:
    log_newnode_32(AnaLogIglobals);
    break;

  case act_disposenode:
    log_disposenode_32(AnaLogIglobals);
    break;

  case act_copynode:
    log_copynode_32(AnaLogIglobals);
    break;

  case act_combinenodes:
    log_combinenodes_32(AnaLogIglobals);
    break;

  case act_history:
    log_history_32(AnaLogIglobals, false);
    break;

  case act_histval:
    log_histval_32(AnaLogIglobals, false);
    break;

  case act_histstr:
    log_histstr_32(AnaLogIglobals, false);
    break;

  case act_configchhist:
    log_configchhist_32(AnaLogIglobals, false);
    break;

  case act_confighist:
    log_confighist_32(AnaLogIglobals, false);
    break;
  }
}






/* End. */
