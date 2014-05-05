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
/* From input file "swiss.text" */



#include <p2c/p2c.h>


#define NFET5_G
#include "nfet5.h"


#define Cstart          1
#define N_Cgd           10
#define N_Cgs           11
#define N_Ratio         12
#define N_Vt            13
#define N_Beta          14
#define N_Kappa         15
#define N_Ktq           16
#define N_Early         17

#define Gatename        "NFET5"


typedef struct NfetConst {
  double k1;   /*2*Ratio*Beta*Vo*Vo/Kappa*/
  double k2;   /*Kappa/(2*Vo)*/
  double k3;   /*1/Kappa*/
  double k4;   /*2*Ratio*Beta*Vo*/
  double k5;   /*(Kappa*Vt)/(2*Vo)*/
  double k6;   /*1/(2*Vo)*/
  double InvVcross;   /*Inverse Early Voltage*/
  double TauCgd;   /*Tau for capacitor*/
  double TauCgs;   /*Tau for capacitor*/
} NfetConst;


typedef struct ICNfet {
  double InitRatio, InitVt, InitBeta, InitKappa, InitKtq, InitCgd, InitCgs,
	 InitCgb, InitCdb, InitCsb, InitEarly;
} ICNfet;



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_NFET5(act)
Analog_32_action *act;
{
  double MinI2, dI2dV1, dI2dV2, dI2dV3, MinI3, dI3dV1, dI3dV2, dI3dV3, MinI1,
	 dI1dV1, dI1dV2, dI1dV3;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  long Pin1, Pin2, Pin3;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;   /*1*/
  double ICap;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double Vds, Vgs, Vgd, Vg, Vd, Vs, Ids, dIdsdG, dIdsdS, dIdsdD;

  /*new model*/

  double K2Vg, ExpFor, OnePlusExpFor, LogOnePlusExpFor, Ifor, DIforDVg,
	 DIforDVs, ExpRev, OnePlusExpRev, LogOnePlusExpRev, Irev, DIrevDVg,
	 DIrevDVd;

  /*early voltage*/

  double ExpVds, Blend, InvBlend, VdsInvVcross, ResForward, ResReverse, Early,
	 dBlenddS, dEarlydS, dEarlydD;

  double Itotal, dItotaldG, dItotaldS, dItotaldD;
  log_grec *WITH;
  AnaMatRec *WITH1;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  Pin1 = Pin1Ptr->nodenum;
  Pin2 = Pin2Ptr->nodenum;
  Pin3 = Pin3Ptr->nodenum;
  VPin1 = Pin1Ptr->last;   /*Vpin1 is the gate*/
  VPin2 = Pin2Ptr->last;   /*Vpin2 is the drain*/
  VPin3 = Pin3Ptr->last;   /*Vpin3 is the source*/
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*Memories for capacitors*/
  VMem2 = A_Gate->Pininfo[1].Vmem;   /*...*/
  VMem3 = A_Gate->Pininfo[2].Vmem;   /*...*/
  Vg = VPin1;
  Vd = VPin2;
  Vs = VPin3;
  Vds = VPin2 - VPin3;
  Vgs = VPin1 - VPin3;
  Vgd = VPin1 - VPin2;

  K2Vg = NfetVar->k2 * Vg;

  ExpFor = AnaExpo(K2Vg - NfetVar->k5 - NfetVar->k6 * Vs);
  OnePlusExpFor = 1 + ExpFor;
  LogOnePlusExpFor = log(OnePlusExpFor);
  Ifor = NfetVar->k1 * LogOnePlusExpFor * LogOnePlusExpFor;
  DIforDVg = NfetVar->k4 * LogOnePlusExpFor * (1 / OnePlusExpFor) * ExpFor;
  DIforDVs = -NfetVar->k3 * DIforDVg;

  ExpRev = AnaExpo(K2Vg - NfetVar->k5 - NfetVar->k6 * Vd);
  OnePlusExpRev = 1 + ExpRev;
  LogOnePlusExpRev = log(OnePlusExpRev);
  Irev = NfetVar->k1 * LogOnePlusExpRev * LogOnePlusExpRev;
  DIrevDVg = NfetVar->k4 * LogOnePlusExpRev * (1 / OnePlusExpRev) * ExpRev;
  DIrevDVd = -NfetVar->k3 * DIrevDVg;

  Ids = Ifor - Irev;
  dIdsdG = DIforDVg - DIrevDVg;
  dIdsdD = -DIrevDVd;
  dIdsdS = DIforDVs;


  if (fabs(Vds) < 0.000708) {
    ExpVds = AnaExpo(1000000L * Vds);
    Blend = 1 / (1 + ExpVds);
    InvBlend = 1 - Blend;
    VdsInvVcross = Vds * NfetVar->InvVcross;
    ResForward = 1 + VdsInvVcross;
    ResReverse = 1 - VdsInvVcross;
    Early = Blend * ResReverse + InvBlend * ResForward;
    dBlenddS = 1000000L * (ExpVds * Blend * Blend);    /* bug fix */
    dEarlydS = dBlenddS * ResReverse + Blend * NfetVar->InvVcross -
	       dBlenddS * ResForward - NfetVar->InvVcross * InvBlend;
    dEarlydD = -dEarlydS;
  } else {
    if (Vds > 0) {
      Early = 1 + Vds * NfetVar->InvVcross;
      dEarlydS = -NfetVar->InvVcross;
      dEarlydD = NfetVar->InvVcross;
    } else {
      Early = 1 - Vds * NfetVar->InvVcross;
      dEarlydS = NfetVar->InvVcross;
      dEarlydD = -NfetVar->InvVcross;
    }
  }

  Itotal = Ids * Early;
  dItotaldG = dIdsdG * Early;
  dItotaldS = dIdsdS * Early + Ids * dEarlydS;
  dItotaldD = dIdsdD * Early + Ids * dEarlydD;

  MinI3 = Itotal;
  MinI2 = -Itotal;
  dI2dV1 = dItotaldG;
  dI3dV1 = -dItotaldG;
  dI2dV3 = dItotaldS;
  dI3dV3 = -dItotaldS;
  dI2dV2 = dItotaldD;
  dI3dV2 = -dItotaldD;
  MinI1 = 0.0;
  dI1dV1 = 0.0;
  dI1dV2 = 0.0;
  dI1dV3 = 0.0;
  if (WITH->attr[N_Cgd - 1].UU.r != 0) {   /*C gate-drain*/
    if (AnaChangedDt)
      NfetVar->TauCgd = WITH->attr[N_Cgd - 1].UU.r / AnaDt;
    ICap = NfetVar->TauCgd * (VMem1 - VMem2 - Vgd);   /*4*/
    MinI2 -= ICap;
    dI2dV1 -= NfetVar->TauCgd;
    dI2dV2 += NfetVar->TauCgd;
    MinI1 += ICap;
    dI1dV1 += NfetVar->TauCgd;
    dI1dV2 -= NfetVar->TauCgd;
  }
  /*3*/
  if (WITH->attr[N_Cgs - 1].UU.r != 0) {   /*C gate-source*/
    if (AnaChangedDt)
      NfetVar->TauCgs = WITH->attr[N_Cgs - 1].UU.r / AnaDt;
    ICap = NfetVar->TauCgs * (VMem1 - VMem3 - Vgs);
    MinI3 -= ICap;
    dI3dV1 -= NfetVar->TauCgs;
    dI3dV3 += NfetVar->TauCgs;
    MinI1 += ICap;
    dI1dV1 += NfetVar->TauCgs;
    dI1dV3 -= NfetVar->TauCgs;
  }
  WITH1 = AnaSystem;
  if (Pin1 != 0) {
    WITH1->Mat[Pin1][AnaSysCol] += MinI1;
    WITH1->Mat[Pin1][Pin1] += dI1dV1;
    WITH1->Mat[Pin1][Pin2] += dI1dV2;
    WITH1->Mat[Pin1][Pin3] += dI1dV3;
    WITH1->MatNZ[Pin1][AnaSysCol] = true;
    WITH1->MatNZ[Pin1][Pin1] = true;
    WITH1->MatNZ[Pin1][Pin2] = true;
    WITH1->MatNZ[Pin1][Pin3] = true;
  }
  if (Pin2 != 0) {
    WITH1->Mat[Pin2][AnaSysCol] += MinI2;
    WITH1->Mat[Pin2][Pin1] += dI2dV1;
    WITH1->Mat[Pin2][Pin2] += dI2dV2;
    WITH1->Mat[Pin2][Pin3] += dI2dV3;
    WITH1->MatNZ[Pin2][AnaSysCol] = true;
    WITH1->MatNZ[Pin2][Pin1] = true;
    WITH1->MatNZ[Pin2][Pin2] = true;
    WITH1->MatNZ[Pin2][Pin3] = true;
  }
  if (Pin3 != 0) {
    WITH1->Mat[Pin3][AnaSysCol] += MinI3;
    WITH1->Mat[Pin3][Pin1] += dI3dV1;
    WITH1->Mat[Pin3][Pin2] += dI3dV2;
    WITH1->Mat[Pin3][Pin3] += dI3dV3;
    WITH1->MatNZ[Pin3][AnaSysCol] = true;
    WITH1->MatNZ[Pin3][Pin1] = true;
    WITH1->MatNZ[Pin3][Pin2] = true;
    WITH1->MatNZ[Pin3][Pin3] = true;
  }
  AnaCapex(act->inst);   /*5*/
}  /*Ex_NFET5*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/

Local Void Iin_NFET5(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double Ids, Vg, Vd, Vs, Vds, K2Vg, ExpFor, OnePlusExpFor, LogOnePlusExpFor,
	 Ifor, ExpRev, OnePlusExpRev, LogOnePlusExpRev, Irev, Blend,
	 VdsInvVcross, Early, Itotal;
  log_grec *WITH1;

  switch (act->pin) {

  case 1:
    WITH1 = act->inst;
    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    NfetVar = (NfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VMem2 = A_Gate->Pininfo[1].Vmem;
    VMem3 = A_Gate->Pininfo[2].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    if (WITH1->attr[N_Cgd - 1].UU.r != 0)   /*C gate-drain*/
      act->Iin = NfetVar->TauCgd * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    if (WITH1->attr[N_Cgs - 1].UU.r != 0)   /*C gate-source*/
      act->Iin = NfetVar->TauCgs * (VPin1 - VPin3 - VMem1 + VMem3) + act->Iin;
    AnaCapIin(act);
    break;

  case 2:
  case 3:
    WITH1 = act->inst;

    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    NfetVar = (NfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    switch (act->pin) {

    case 2:
      if (WITH1->attr[N_Cgd - 1].UU.r != 0) {
	VMem2 = A_Gate->Pininfo[1].Vmem;
	act->Iin = NfetVar->TauCgd * (VPin2 - VPin1 - VMem2 + VMem1) + act->Iin;
      }
      break;

    case 3:
      if (WITH1->attr[N_Cgs - 1].UU.r != 0) {
	VMem3 = A_Gate->Pininfo[2].Vmem;
	act->Iin = NfetVar->TauCgs * (VPin3 - VPin1 - VMem3 + VMem1) + act->Iin;
      }
      break;
    }
    Vg = VPin1;
    Vd = VPin2;
    Vs = VPin3;
    Vds = Vd - Vs;

    K2Vg = NfetVar->k2 * Vg;

    ExpFor = AnaExpo(K2Vg - NfetVar->k5 - NfetVar->k6 * Vs);
    OnePlusExpFor = 1 + ExpFor;
    LogOnePlusExpFor = log(OnePlusExpFor);
    Ifor = NfetVar->k1 * LogOnePlusExpFor * LogOnePlusExpFor;

    ExpRev = AnaExpo(K2Vg - NfetVar->k5 - NfetVar->k6 * Vd);
    OnePlusExpRev = 1 + ExpRev;
    LogOnePlusExpRev = log(OnePlusExpRev);
    Irev = NfetVar->k1 * LogOnePlusExpRev * LogOnePlusExpRev;

    Ids = Ifor - Irev;

    if (fabs(Vds) < 0.000708) {
      Blend = 1 / (1 + AnaExpo(1000000L * Vds));
      VdsInvVcross = Vds * NfetVar->InvVcross;
      Early = Blend * (1 - VdsInvVcross) + (1 - Blend) * (1 + VdsInvVcross);
    } else {
      if (Vds > 0)
	Early = 1 + Vds * NfetVar->InvVcross;
      else
	Early = 1 - Vds * NfetVar->InvVcross;
    }

    Itotal = Ids * Early;

    switch (act->pin) {

    case 2:
      act->Iin += Itotal;
      break;

    case 3:
      act->Iin -= Itotal;
      break;
    }
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_NFET5*/


Local Void GetCnf_Nfet5(NewIC)
ICNfet *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICNfet *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("NFET5", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "W/L")) {
	TRY(try1);
	  WITH->InitRatio = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->InitRatio = 1.0;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "VT")) {
	TRY(try2);
	  WITH->InitVt = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->InitVt = 0.9;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "BETA")) {
	TRY(try3);
	  WITH->InitBeta = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitBeta = 50e-6;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "KAPPA")) {
	TRY(try4);
	  WITH->InitKappa = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitKappa = 1.0;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "KT/Q")) {
	TRY(try5);
	  WITH->InitKtq = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitKtq = 0.025;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "CGS")) {
	TRY(try6);
	  WITH->InitCgs = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try6);
	  WITH->InitCgs = 10e-15;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "CGD")) {
	TRY(try7);
	  WITH->InitCgd = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try7);
	  WITH->InitCgd = 10e-15;
	ENDTRY(try7);
      }
      if (!strcmp(Keyword, "CGB")) {
	TRY(try8);
	  WITH->InitCgb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try8);
	  WITH->InitCgb = 10e-15;
	ENDTRY(try8);
      }
      if (!strcmp(Keyword, "CDB")) {
	TRY(try9);
	  WITH->InitCdb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try9);
	  WITH->InitCdb = 10e-15;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "CSB")) {
	TRY(try10);
	  WITH->InitCsb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try10);
	  WITH->InitCsb = 10e-15;
	ENDTRY(try10);
      }
      if (!strcmp(Keyword, "EARLY")) {
	TRY(try11);
	  WITH->InitEarly = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try11);
	  WITH->InitEarly = 50.0;
	ENDTRY(try11);
      }
    }
  } while (Found);
}

/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_NFET5(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICNfet *NewIC;

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
  NewCell->plist_cell[2].standalone = false;
  NewCell->plist_cell[2].active = true;
  NewIC = (ICNfet *)Malloc(sizeof(ICNfet));
  NewIC->InitRatio = 1.0;
  NewIC->InitVt = 0.9;
  NewIC->InitBeta = 50e-6;
  NewIC->InitKappa = 1.0;
  NewIC->InitKtq = 0.025;
  NewIC->InitCgs = 10e-15;
  NewIC->InitCgd = 10e-15;
  NewIC->InitCgb = 10e-15;
  NewIC->InitCdb = 10e-15;
  NewIC->InitCsb = 10e-15;
  NewIC->InitEarly = 50.0;
  GetCnf_Nfet5(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*NFET5_Initlib*/


Local boolean Nfetcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Ratio:
    if (Inst->attr[N_Ratio - 1].UU.r < 0 || Inst->attr[N_Ratio - 1].blnk ||
	Inst->attr[N_Ratio - 1].UU.r > 1e20)
      Result = false;
    else {
      NfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      NfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else
      NfetVar->k5 = Inst->attr[N_Kappa - 1].UU.r * Inst->attr[N_Vt - 1].UU.r /
		    (2 * Inst->attr[N_Ktq - 1].UU.r);
    break;

  case N_Beta:
    if ((unsigned)Inst->attr[N_Beta - 1].UU.r > 1 ||
	Inst->attr[N_Beta - 1].blnk)
      Result = false;
    else {
      NfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      NfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
    }
    break;

  case N_Kappa:
    if (Inst->attr[N_Kappa - 1].UU.r <= 0 ||
	Inst->attr[N_Kappa - 1].UU.r > 1 || Inst->attr[N_Kappa - 1].blnk)
      Result = false;
    else {
      NfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      NfetVar->k2 = Inst->attr[N_Kappa - 1].UU.r / (2 * Inst->attr[N_Ktq - 1].UU.r);
      NfetVar->k3 = 1 / Inst->attr[N_Kappa - 1].UU.r;
      NfetVar->k5 = Inst->attr[N_Kappa - 1].UU.r * Inst->attr[N_Vt - 1].UU.r /
		    (2 * Inst->attr[N_Ktq - 1].UU.r);
    }
    break;

  case N_Ktq:
    if (Inst->attr[N_Ktq - 1].UU.r <= 0 || Inst->attr[N_Ktq - 1].UU.r > 1 ||
	Inst->attr[N_Ktq - 1].blnk)
      Result = false;
    else {
      NfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      NfetVar->k2 = Inst->attr[N_Kappa - 1].UU.r / (2 * Inst->attr[N_Ktq - 1].UU.r);
      NfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
      NfetVar->k5 = Inst->attr[N_Kappa - 1].UU.r * Inst->attr[N_Vt - 1].UU.r /
		    (2 * Inst->attr[N_Ktq - 1].UU.r);
      NfetVar->k6 = 1 / (2 * Inst->attr[N_Ktq - 1].UU.r);
    }
    break;

  case N_Early:
    if (Inst->attr[N_Early - 1].UU.r <= 0 ||
	Inst->attr[N_Early - 1].UU.r > AnaVTooBig)
      Result = false;
    else {
      if (Inst->attr[N_Early - 1].blnk)
	NfetVar->InvVcross = 0.0;
      else
	NfetVar->InvVcross = 1 / Inst->attr[N_Early - 1].UU.r;
    }
    break;
  }
  return Result;
}


Local Void Attr_Nfet5(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 8) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 &&
      ((1L << Attrnum) & ((1L << N_Cgs) | (1L << N_Cgd))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = Nfetcheck(act->inst, Attrnum);
}


Local Void Newgate_Nfet5(act)
Analog_32_action *act;
{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNfet *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICNfet *)Cellptr->Ainfo;
  NfetVar = (NfetConst *)Malloc(sizeof(NfetConst));
  NfetVar->k1 = 2 * ICptr->InitRatio * ICptr->InitBeta * ICptr->InitKtq *
		ICptr->InitKtq / ICptr->InitKappa;
  NfetVar->k2 = ICptr->InitKappa / (2 * ICptr->InitKtq);
  NfetVar->k3 = 1 / ICptr->InitKappa;
  NfetVar->k4 = 2 * ICptr->InitRatio * ICptr->InitBeta * ICptr->InitKtq;
  NfetVar->k5 = ICptr->InitKappa * ICptr->InitVt / (2 * ICptr->InitKtq);
  NfetVar->k6 = 1 / (2 * ICptr->InitKtq);
  if (ICptr->InitEarly == 0)
    NfetVar->InvVcross = 0.0;
  else
    NfetVar->InvVcross = 1 / ICptr->InitEarly;
  NfetVar->TauCgd = 0.0;
  NfetVar->TauCgs = 0.0;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NfetVar;
  AnaCapInit(act->inst);   /*6*/
  A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
  A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
  A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
  WITH = act->inst;
  WITH->attr[N_Cgd - 1].UU.r = ICptr->InitCgd;   /*Cgd*/
  WITH->attr[N_Cgd - 1].blnk = false;
  WITH->attr[N_Cgd - 1].changed = true;
  WITH->attr[N_Cgs - 1].UU.r = ICptr->InitCgs;   /*Cgs*/
  WITH->attr[N_Cgs - 1].blnk = false;
  WITH->attr[N_Cgs - 1].changed = true;
  WITH->attr[N_Ratio - 1].UU.r = ICptr->InitRatio;   /*W/L*/
  WITH->attr[N_Ratio - 1].blnk = false;
  WITH->attr[N_Ratio - 1].changed = true;
  WITH->attr[N_Vt - 1].UU.r = ICptr->InitVt;   /*Vt*/
  WITH->attr[N_Vt - 1].blnk = false;
  WITH->attr[N_Vt - 1].changed = true;
  WITH->attr[N_Beta - 1].UU.r = ICptr->InitBeta;   /*Beta*/
  WITH->attr[N_Beta - 1].blnk = false;
  WITH->attr[N_Beta - 1].changed = true;
  WITH->attr[N_Kappa - 1].UU.r = ICptr->InitKappa;   /*Kappa*/
  WITH->attr[N_Kappa - 1].blnk = false;
  WITH->attr[N_Kappa - 1].changed = true;
  WITH->attr[N_Ktq - 1].UU.r = ICptr->InitKtq;   /*Kt/q*/
  WITH->attr[N_Ktq - 1].blnk = false;
  WITH->attr[N_Ktq - 1].changed = true;
  WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;
  WITH->attr[N_Early - 1].blnk = false;
  WITH->attr[N_Early - 1].changed = true;
}

Local Void Copygate_Nfet5(act)
Analog_32_action *act;
{
  NfetConst *NfetVar, *Old_Nfetvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Nfetvar = (NfetConst *)A_Oldgate->InstVar;
  NfetVar = (NfetConst *)Malloc(sizeof(NfetConst));
  *NfetVar = *Old_Nfetvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NfetVar;
  AnaCapCopy(act->inst);   /*6*/
}

Local Void Dispose_Nfet5(act)
Analog_32_action *act;
{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  Free(NfetVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Nfet5(act)
Analog_32_action *act;
{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNfet *ICptr;
  log_grec *WITH;

  AnaCapattrread(1L, act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICNfet *)Cellptr->Ainfo;
    A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
    A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
    A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
    WITH = act->inst;
    WITH->attr[N_Cgd - 1].UU.r = ICptr->InitCgd;   /*Cgd*/
    WITH->attr[N_Cgd - 1].blnk = false;
    WITH->attr[N_Cgd - 1].changed = true;
    WITH->attr[N_Cgs - 1].UU.r = ICptr->InitCgs;   /*Cgs*/
    WITH->attr[N_Cgs - 1].blnk = false;
    WITH->attr[N_Cgs - 1].changed = true;
    WITH->attr[N_Vt - 1].UU.r = ICptr->InitVt;   /*Vt*/
    WITH->attr[N_Vt - 1].blnk = false;
    WITH->attr[N_Vt - 1].changed = true;
    WITH->attr[N_Beta - 1].UU.r = ICptr->InitBeta;   /*Beta*/
    WITH->attr[N_Beta - 1].blnk = false;
    WITH->attr[N_Beta - 1].changed = true;
    WITH->attr[N_Kappa - 1].UU.r = ICptr->InitKappa;   /*Kappa*/
    WITH->attr[N_Kappa - 1].blnk = false;
    WITH->attr[N_Kappa - 1].changed = true;
    WITH->attr[N_Ktq - 1].UU.r = ICptr->InitKtq;   /*Kt/q*/
    WITH->attr[N_Ktq - 1].blnk = false;
    WITH->attr[N_Ktq - 1].changed = true;
    WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;   /*Early*/
    WITH->attr[N_Early - 1].blnk = false;
    WITH->attr[N_Early - 1].changed = true;
  }
  WITH = act->inst;
  NfetVar->k1 = 2 * WITH->attr[N_Ratio - 1].UU.r *
		WITH->attr[N_Beta - 1].UU.r * WITH->attr[N_Ktq - 1].UU.r *
		WITH->attr[N_Ktq - 1].UU.r / WITH->attr[N_Kappa - 1].UU.r;
  NfetVar->k2 = WITH->attr[N_Kappa - 1].UU.r / (2 * WITH->attr[N_Ktq - 1].UU.r);
  NfetVar->k3 = 1 / WITH->attr[N_Kappa - 1].UU.r;
  NfetVar->k4 = 2 * WITH->attr[N_Ratio - 1].UU.r *
		WITH->attr[N_Beta - 1].UU.r * WITH->attr[N_Ktq - 1].UU.r;
  NfetVar->k5 = WITH->attr[N_Kappa - 1].UU.r * WITH->attr[N_Vt - 1].UU.r /
		(2 * WITH->attr[N_Ktq - 1].UU.r);
  NfetVar->k6 = 1 / (2 * WITH->attr[N_Ktq - 1].UU.r);
  if (WITH->attr[N_Early - 1].blnk)
    NfetVar->InvVcross = 0.0;
  else
    NfetVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
  NfetVar->TauCgd = 0.0;
  NfetVar->TauCgs = 0.0;
}

Local Void Probe_Nfet5(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr, *Node3Ptr;
  double Node1, Node2, Node3, Pin1, Pin2, Pin3, d1, d2;
  Char Name2[256], Name3[256];
  log_grec *WITH;
  Char STR1[18];
  Char STR2[14];
  Char STR3[256];
  Char STR4[256];

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Pin1 = A_Gate->Pininfo[0].Vmem;
  Pin2 = A_Gate->Pininfo[1].Vmem;
  Pin3 = A_Gate->Pininfo[2].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Node3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  Node3 = Node3Ptr->ltimestep;
  if (Node1 != Pin1 || Node2 != Pin2 || Node3 != Pin3 || Node1 == AnaNotyet ||
      Node2 == AnaNotyet || Node3 == AnaNotyet) {
    sprintf(STR1, "%s <undefined>", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  d1 = Pin2 - Pin3;
  if (d1 >= 0) {
    d2 = Pin1 - Pin3;
    strcpy(Name2, " (drain)");
    strcpy(Name3, " (source)");
  } else {
    d1 = -d1;
    d2 = Pin1 - Pin2;
    strcpy(Name2, " (source)");
    strcpy(Name3, " (drain)");
  }
  switch (AnaLogglobals->probepin) {

  case 0:
    AnaScoreboard(Gatename, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s (gate)", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR3, "%s%s", Gatename, Name2);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;

  case 3:
    sprintf(STR3, "%s%s", Gatename, Name3);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;
  }
  sprintf(STR3, "Vds = %s", AnaProbeVoltStr(STR4, d1));
  AnaScoreboard(STR3, (long)AnaMessGate2);
  sprintf(STR3, "Vgs = %s", AnaProbeVoltStr(STR4, d2));
  AnaScoreboard(STR3, (long)AnaMessGate3);
}




/*==========================================================================*/
/*                 Library Listing For   Cell NFET5                    */
/*==========================================================================*/


Void Log_NFET5_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_NFET5(act);
    break;

  case Analog_act_ex:
    Ex_NFET5(act);
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
    Attr_Nfet5(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Nfet5(act);
    break;

  case Analog_act_copygate:
    Copygate_Nfet5(act);
    break;

  case Analog_act_disposegate:
    Dispose_Nfet5(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Nfet5(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_NFET5(act);
    break;

  case Analog_act_probe:
    Probe_Nfet5(act);
    break;
  }
}

#undef Cstart
#undef N_Cgd
#undef N_Cgs
#undef N_Ratio
#undef N_Vt
#undef N_Beta
#undef N_Kappa
#undef N_Ktq
#undef N_Early
#undef Gatename







/* End. */
