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




#define PFET6_G
#include "pfet6.h"


#define Cstart          1
#define N_Cgd           13
#define N_Cgs           14
#define N_Ratio         15
#define N_Vt            16
#define N_Beta          17
#define N_Kappa         18
#define N_Ktq           19
#define N_Early         20

#define Gatename        "PFET6"


typedef struct PfetConst {
  double k1;   /*2*Ratio*Beta*Vo*Vo/Kappa*/
  double k2;   /*Kappa/(2*Vo)*/
  double k3;   /*1/Kappa*/
  double k4;   /*2*Ratio*Beta*Vo*/
  double k5;   /*(Kappa*Vt)/(2*Vo)*/
  double k6;   /*1/(2*Vo)*/
  double k7;   /*(1-Kappa)/2Vo*/
  double k8;   /*2*Ratio*Beta*Vo*((1/Kappa) -1)*/
  double InvVcross;   /*Inverse Early Voltage*/
  double TauCgd;   /*Tau for capacitor*/
  double TauCgs;   /*Tau for capacitor*/
} PfetConst;


typedef struct ICPfet {
  double InitRatio, InitVt, InitBeta, InitKappa, InitKtq, InitCgd, InitCgs,
	 InitCgb, InitCdb, InitCsb, InitCwb, InitEarly;
} ICPfet;



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_PFET6(act)
Analog_32_action *act;
{
  double MinI2, dI2dV1, dI2dV2, dI2dV3, dI2dV4, 
         MinI3, dI3dV1, dI3dV2, dI3dV3, dI3dV4,
         MinI1, dI1dV1, dI1dV2, dI1dV3, dI1dV4,
         MinI4, dI4dV1, dI4dV2, dI4dV3, dI4dV4;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr, *Pin4Ptr;
  long Pin1, Pin2, Pin3, Pin4;
  double VPin1, VPin2, VPin3, VPin4, VMem1, VMem2, VMem3, VMem4;   /*1*/
  double ICap;
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  double Vds, Vgs, Vgd, Vg, Vd, Vs, Vw, Ids, dIdsdG, dIdsdS, dIdsdD, dIdsdW;

  /*new model*/

  double K2Vg, K7Vw, ExpFor, OnePlusExpFor, LogOnePlusExpFor, Ifor, DIforDVg,
	 DIforDVs, ExpRev, OnePlusExpRev, LogOnePlusExpRev, Irev, DIrevDVg,
	 DIrevDVd, DIforMath, DIrevMath, DIforDVw, DIrevDVw;

  /*early voltage*/

  double ExpVds, Blend, InvBlend, VdsInvVcross, ResForward, ResReverse, Early,
	 dBlenddS, dEarlydS, dEarlydD;

  double Itotal, dItotaldG, dItotaldS, dItotaldD, dItotaldW;
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  Pin4Ptr = (AnaExt_rec *)WITH->pin[3]->info;
  Pin1 = Pin1Ptr->nodenum;
  Pin2 = Pin2Ptr->nodenum;
  Pin3 = Pin3Ptr->nodenum;
  Pin4 = Pin4Ptr->nodenum;
  VPin1 = Pin1Ptr->last;   /*Vpin1 is the gate*/
  VPin2 = Pin2Ptr->last;   /*Vpin2 is the drain*/
  VPin3 = Pin3Ptr->last;   /*Vpin3 is the source*/
  VPin4 = Pin4Ptr->last;   /*Vpin4 is the well*/
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*Memories for capacitors*/
  VMem2 = A_Gate->Pininfo[1].Vmem;   /*...*/
  VMem3 = A_Gate->Pininfo[2].Vmem;   /*...*/
  VMem4 = A_Gate->Pininfo[3].Vmem;   /*...*/
  Vg = VPin1;
  Vd = VPin2;
  Vs = VPin3;
  Vw = VPin4;
  Vds = VPin2 - VPin3;
  Vgs = VPin1 - VPin3;
  Vgd = VPin1 - VPin2;

  K2Vg = PfetVar->k2 * Vg;
  K7Vw = PfetVar->k7 * Vw;

  ExpFor = AnaExpo(PfetVar->k6 * Vs - PfetVar->k5 - K7Vw - K2Vg);
  OnePlusExpFor = 1 + ExpFor;
  LogOnePlusExpFor = log(OnePlusExpFor);
  Ifor = PfetVar->k1 * LogOnePlusExpFor * LogOnePlusExpFor;
  DIforMath = LogOnePlusExpFor * (-1 / OnePlusExpFor) * ExpFor;
  DIforDVg = PfetVar->k4 * DIforMath;
  DIforDVs = PfetVar->k3 * DIforDVg;
  DIforDVw = PfetVar->k8 * DIforMath;

  ExpRev = AnaExpo(PfetVar->k6 * Vd - PfetVar->k5 - K7Vw - K2Vg);
  OnePlusExpRev = 1 + ExpRev;
  LogOnePlusExpRev = log(OnePlusExpRev);
  Irev = PfetVar->k1 * LogOnePlusExpRev * LogOnePlusExpRev;
  DIrevMath = LogOnePlusExpRev * (-1 / OnePlusExpRev) * ExpRev;
  DIrevDVg = PfetVar->k4 * DIrevMath;
  DIrevDVd = PfetVar->k3 * DIrevDVg;
  DIrevDVw = PfetVar->k8 * DIrevMath;

  Ids = Irev - Ifor;
  dIdsdG = DIrevDVg - DIforDVg;
  dIdsdD = -DIrevDVd;  /*Sign correct despite appearances*/
  dIdsdS = DIforDVs;   /*Sign correct despite appearances*/
  dIdsdW = DIrevDVw - DIforDVw;

  if (fabs(Vds) < 0.000708) {
    ExpVds = AnaExpo(1000000L * Vds);
    Blend = 1 / (1 + ExpVds);
    InvBlend = 1 - Blend;
    VdsInvVcross = Vds * PfetVar->InvVcross;
    ResForward = 1 + VdsInvVcross;
    ResReverse = 1 - VdsInvVcross;
    Early = Blend * ResReverse + InvBlend * ResForward;
    dBlenddS = 1000000L * (ExpVds * Blend * Blend);
    dEarlydS = dBlenddS * ResReverse + Blend * PfetVar->InvVcross -
	       dBlenddS * ResForward - PfetVar->InvVcross * InvBlend;
    dEarlydD = -dEarlydS;
  } else {
    if (Vds > 0) {
      Early = 1 + Vds * PfetVar->InvVcross;
      dEarlydS = -PfetVar->InvVcross;
      dEarlydD = PfetVar->InvVcross;
    } else {
      Early = 1 - Vds * PfetVar->InvVcross;
      dEarlydS = PfetVar->InvVcross;
      dEarlydD = -PfetVar->InvVcross;
    }
  }

  Itotal = Ids * Early;
  dItotaldG = dIdsdG * Early;
  dItotaldS = dIdsdS * Early + Ids * dEarlydS;
  dItotaldD = dIdsdD * Early + Ids * dEarlydD;
  dItotaldW = dIdsdW * Early;

  MinI3 = Itotal;
  MinI2 = -Itotal;
  dI2dV1 = dItotaldG;
  dI3dV1 = -dItotaldG;
  dI2dV3 = dItotaldS;
  dI3dV3 = -dItotaldS;
  dI2dV2 = dItotaldD;
  dI3dV2 = -dItotaldD;
  dI2dV4 = dItotaldW;
  dI3dV4 = -dItotaldW;

  MinI1 = 0.0;
  dI1dV1 = 0.0;
  dI1dV2 = 0.0;
  dI1dV3 = 0.0;
  dI1dV4 = 0.0;

  MinI4 = 0.0;
  dI4dV1 = 0.0;
  dI4dV2 = 0.0;
  dI4dV3 = 0.0;
  dI4dV4 = 0.0;

  if (WITH->attr[N_Cgd - 1].UU.r != 0) {   /*C gate-drain*/
    if (AnaChangedDt)
      PfetVar->TauCgd = WITH->attr[N_Cgd - 1].UU.r / AnaDt;
    ICap = PfetVar->TauCgd * (VMem1 - VMem2 - Vgd);   /*4*/
    MinI2 -= ICap;
    dI2dV1 -= PfetVar->TauCgd;
    dI2dV2 += PfetVar->TauCgd;
    MinI1 += ICap;
    dI1dV1 += PfetVar->TauCgd;
    dI1dV2 -= PfetVar->TauCgd;
  }
  /*3*/
  if (WITH->attr[N_Cgs - 1].UU.r != 0) {   /*C gate-source*/
    if (AnaChangedDt)
      PfetVar->TauCgs = WITH->attr[N_Cgs - 1].UU.r / AnaDt;
    ICap = PfetVar->TauCgs * (VMem1 - VMem3 - Vgs);
    MinI3 -= ICap;
    dI3dV1 -= PfetVar->TauCgs;
    dI3dV3 += PfetVar->TauCgs;
    MinI1 += ICap;
    dI1dV1 += PfetVar->TauCgs;
    dI1dV3 -= PfetVar->TauCgs;
  }
  WITH1 = AnaSystem;
  if (Pin1 != 0) {
    WITH1->Mat[Pin1][AnaSysCol] += MinI1;
    WITH1->Mat[Pin1][Pin1] += dI1dV1;
    WITH1->Mat[Pin1][Pin2] += dI1dV2;
    WITH1->Mat[Pin1][Pin3] += dI1dV3;
    WITH1->Mat[Pin1][Pin4] += dI1dV4;
    WITH1->MatNZ[Pin1][AnaSysCol] = true;
    WITH1->MatNZ[Pin1][Pin1] = true;
    WITH1->MatNZ[Pin1][Pin2] = true;
    WITH1->MatNZ[Pin1][Pin3] = true;
    WITH1->MatNZ[Pin1][Pin4] = true;
  }
  if (Pin2 != 0) {
    WITH1->Mat[Pin2][AnaSysCol] += MinI2;
    WITH1->Mat[Pin2][Pin1] += dI2dV1;
    WITH1->Mat[Pin2][Pin2] += dI2dV2;
    WITH1->Mat[Pin2][Pin3] += dI2dV3;
    WITH1->Mat[Pin2][Pin4] += dI2dV4;
    WITH1->MatNZ[Pin2][AnaSysCol] = true;
    WITH1->MatNZ[Pin2][Pin1] = true;
    WITH1->MatNZ[Pin2][Pin2] = true;
    WITH1->MatNZ[Pin2][Pin3] = true;
    WITH1->MatNZ[Pin2][Pin4] = true;
  }
  if (Pin3 != 0) {
    WITH1->Mat[Pin3][AnaSysCol] += MinI3;
    WITH1->Mat[Pin3][Pin1] += dI3dV1;
    WITH1->Mat[Pin3][Pin2] += dI3dV2;
    WITH1->Mat[Pin3][Pin3] += dI3dV3;
    WITH1->Mat[Pin3][Pin4] += dI3dV4;
    WITH1->MatNZ[Pin3][AnaSysCol] = true;
    WITH1->MatNZ[Pin3][Pin1] = true;
    WITH1->MatNZ[Pin3][Pin2] = true;
    WITH1->MatNZ[Pin3][Pin3] = true;
    WITH1->MatNZ[Pin3][Pin4] = true;
  }
  if (Pin4 != 0) {
    WITH1->Mat[Pin4][AnaSysCol] += MinI4;
    WITH1->Mat[Pin4][Pin1] += dI4dV1;
    WITH1->Mat[Pin4][Pin2] += dI4dV2;
    WITH1->Mat[Pin4][Pin3] += dI4dV3;
    WITH1->Mat[Pin4][Pin4] += dI4dV4;
    WITH1->MatNZ[Pin4][AnaSysCol] = true;
    WITH1->MatNZ[Pin4][Pin1] = true;
    WITH1->MatNZ[Pin4][Pin2] = true;
    WITH1->MatNZ[Pin4][Pin3] = true;
    WITH1->MatNZ[Pin4][Pin4] = true;
  }
  AnaCapex(act->inst);   /*5*/
}  /*Ex_PFET6*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/

Local Void Iin_PFET6(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr, *Pin4Ptr;
  double VPin1, VPin2, VPin3, VPin4, VMem1, VMem2, VMem3, VMem4 ;
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  double Ids, Vg, Vd, Vs, Vw, Vds, K2Vg, K7Vw, ExpFor, OnePlusExpFor,
         LogOnePlusExpFor, Ifor, ExpRev, OnePlusExpRev, LogOnePlusExpRev, 
         Irev, Blend, VdsInvVcross, Early, Itotal;
  log_grec *WITH1;

  switch (act->pin) {

  case 1:
    WITH1 = act->inst;
    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    PfetVar = (PfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VMem2 = A_Gate->Pininfo[1].Vmem;
    VMem3 = A_Gate->Pininfo[2].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    if (WITH1->attr[N_Cgd - 1].UU.r != 0)   /*C gate-drain*/
      act->Iin = PfetVar->TauCgd * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    if (WITH1->attr[N_Cgs - 1].UU.r != 0)   /*C gate-source*/
      act->Iin = PfetVar->TauCgs * (VPin1 - VPin3 - VMem1 + VMem3) + act->Iin;
    AnaCapIin(act);
    break;

  case 4:
    AnaCapIin(act);
    break;

  case 2:
  case 3:
    WITH1 = act->inst;

    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    Pin4Ptr = (AnaExt_rec *)WITH1->pin[3]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    PfetVar = (PfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    VPin4 = Pin4Ptr->now;
    switch (act->pin) {

    case 2:
      if (WITH1->attr[N_Cgd - 1].UU.r != 0) {
	VMem2 = A_Gate->Pininfo[1].Vmem;
	act->Iin = PfetVar->TauCgd * (VPin2 - VPin1 - VMem2 + VMem1) + act->Iin;
      }
      break;

    case 3:
      if (WITH1->attr[N_Cgs - 1].UU.r != 0) {
	VMem3 = A_Gate->Pininfo[2].Vmem;
	act->Iin = PfetVar->TauCgs * (VPin3 - VPin1 - VMem3 + VMem1) + act->Iin;
      }
      break;
    }

    Vg = VPin1;
    Vd = VPin2;
    Vs = VPin3;
    Vw = VPin4;
    Vds = Vd - Vs;

    K2Vg = PfetVar->k2 * Vg;
    K7Vw = PfetVar->k7 * Vw;

    ExpFor = AnaExpo(PfetVar->k6 * Vs - PfetVar->k5 - K7Vw - K2Vg);
    OnePlusExpFor = 1 + ExpFor;
    LogOnePlusExpFor = log(OnePlusExpFor);
    Ifor = PfetVar->k1 * LogOnePlusExpFor * LogOnePlusExpFor;

    ExpRev = AnaExpo(PfetVar->k6 * Vd - PfetVar->k5  -K7Vw - K2Vg);
    OnePlusExpRev = 1 + ExpRev;
    LogOnePlusExpRev = log(OnePlusExpRev);
    Irev = PfetVar->k1 * LogOnePlusExpRev * LogOnePlusExpRev;

    Ids = Irev - Ifor;

    if (fabs(Vds) < 0.000708) {
      Blend = 1 / (1 + AnaExpo(1000000L * Vds));
      VdsInvVcross = Vds * PfetVar->InvVcross;
      Early = Blend * (1 - VdsInvVcross) + (1 - Blend) * (1 + VdsInvVcross);
    } else {
      if (Vds > 0)
	Early = 1 + Vds * PfetVar->InvVcross;
      else
	Early = 1 - Vds * PfetVar->InvVcross;
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
}  /*Iin_PFET6*/


Local Void GetCnf_Pfet6(NewIC)
ICPfet *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICPfet *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("PFET6", Arg, &Found);
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
      if (!strcmp(Keyword, "CWB")) {
	TRY(try11);
	  WITH->InitCwb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try11);
	  WITH->InitCwb = 10e-15;
	ENDTRY(try11);
      }
      if (!strcmp(Keyword, "EARLY")) {
	TRY(try12);
	  WITH->InitEarly = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try12);
	  WITH->InitEarly = 50.0;
	ENDTRY(try12);
      }
    }
  } while (Found);
}

/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_PFET6(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICPfet *NewIC;

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
  NewCell->plist_cell[3].standalone = false;
  NewCell->plist_cell[3].active = true;
  NewIC = (ICPfet *)Malloc(sizeof(ICPfet));
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
  NewIC->InitCwb = 10e-15;
  NewIC->InitEarly = 50.0;
  GetCnf_Pfet6(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*PFET6_Initlib*/


Local boolean Pfetcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  PfetConst *PfetVar;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Ratio:
    if (Inst->attr[N_Ratio - 1].UU.r < 0 || Inst->attr[N_Ratio - 1].blnk ||
	Inst->attr[N_Ratio - 1].UU.r > 1e20)
      Result = false;
    else {
      PfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      PfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
      PfetVar->k8 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r
                    *((1/Inst->attr[N_Kappa - 1].UU.r) - 1);
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else
      PfetVar->k5 = (Inst->attr[N_Kappa - 1].UU.r * Inst->attr[N_Vt - 1].UU.r)
		     /(2 * Inst->attr[N_Ktq - 1].UU.r);
    break;

  case N_Beta:
    if ((unsigned)Inst->attr[N_Beta - 1].UU.r > 1 ||
	Inst->attr[N_Beta - 1].blnk)
      Result = false;
    else {
      PfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      PfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
      PfetVar->k8 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r
                    *((1/Inst->attr[N_Kappa - 1].UU.r) - 1);
    }
    break;

  case N_Kappa:
    if (Inst->attr[N_Kappa - 1].UU.r <= 0 ||
	Inst->attr[N_Kappa - 1].UU.r > 1 || Inst->attr[N_Kappa - 1].blnk)
      Result = false;
    else {
      PfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      PfetVar->k2 = Inst->attr[N_Kappa - 1].UU.r / (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k3 = 1 / Inst->attr[N_Kappa - 1].UU.r;
      PfetVar->k5 = (Inst->attr[N_Kappa - 1].UU.r * Inst->attr[N_Vt - 1].UU.r)/
		    (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k7 = (1 - Inst->attr[N_Kappa - 1].UU.r)/
                    (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k8 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r
                    *((1/Inst->attr[N_Kappa - 1].UU.r) - 1);
    }
    break;

  case N_Ktq:
    if ((unsigned)Inst->attr[N_Ktq - 1].UU.r > 1 || Inst->attr[N_Ktq - 1].blnk)
      Result = false;
    else {
      PfetVar->k1 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r *
		    Inst->attr[N_Ktq - 1].UU.r / Inst->attr[N_Kappa - 1].UU.r;
      PfetVar->k2 = Inst->attr[N_Kappa - 1].UU.r / (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k4 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r;
      PfetVar->k5 = (Inst->attr[N_Kappa - 1].UU.r*Inst->attr[N_Vt - 1].UU.r)/ 
		    (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k6 = 1 / (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k7 = (1 - Inst->attr[N_Kappa - 1].UU.r)/
                    (2 * Inst->attr[N_Ktq - 1].UU.r);
      PfetVar->k8 = 2 * Inst->attr[N_Ratio - 1].UU.r *
		    Inst->attr[N_Beta - 1].UU.r * Inst->attr[N_Ktq - 1].UU.r
                    *((1/Inst->attr[N_Kappa - 1].UU.r) - 1);
    }
    break;

  case N_Early:
    if (Inst->attr[N_Early - 1].UU.r <= 0 ||
	Inst->attr[N_Early - 1].UU.r > AnaVTooBig)
      Result = false;
    else {
      if (Inst->attr[N_Early - 1].blnk)
	PfetVar->InvVcross = 0.0;
      else
	PfetVar->InvVcross = 1 / Inst->attr[N_Early - 1].UU.r;
    }
    break;
  }
  return Result;
}


Local Void Attr_Pfet6(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 11) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 &&
      ((1L << Attrnum) & ((1L << N_Cgs) | (1L << N_Cgd))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = Pfetcheck(act->inst, Attrnum);
}


Local Void Newgate_Pfet6(act)
Analog_32_action *act;
{
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICPfet *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICPfet *)Cellptr->Ainfo;
  PfetVar = (PfetConst *)Malloc(sizeof(PfetConst));
  PfetVar->k1 = 2 * ICptr->InitRatio * ICptr->InitBeta * ICptr->InitKtq *
		ICptr->InitKtq / ICptr->InitKappa;
  PfetVar->k2 = ICptr->InitKappa / (2 * ICptr->InitKtq);
  PfetVar->k3 = 1 / ICptr->InitKappa;
  PfetVar->k4 = 2 * ICptr->InitRatio * ICptr->InitBeta * ICptr->InitKtq;
  PfetVar->k5 = (ICptr->InitKappa * ICptr->InitVt)/
		(2 * ICptr->InitKtq);
  PfetVar->k6 = 1 / (2 * ICptr->InitKtq);
  PfetVar->k7 = (1-ICptr->InitKappa)/(2 * ICptr->InitKtq);
  PfetVar->k8 = 2 * ICptr->InitRatio * ICptr->InitBeta * ICptr->InitKtq*
                ((1/ICptr->InitKappa)-1);
  if (ICptr->InitEarly == 0)
    PfetVar->InvVcross = 0.0;
  else
    PfetVar->InvVcross = 1 / ICptr->InitEarly;
  PfetVar->TauCgd = 0.0;
  PfetVar->TauCgs = 0.0;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)PfetVar;
  AnaCapInit(act->inst);   /*6*/
  A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
  A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
  A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
  A_Gate->Pininfo[3].Cap = ICptr->InitCwb;
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

Local Void Copygate_Pfet6(act)
Analog_32_action *act;
{
  PfetConst *PfetVar, *Old_Pfetvar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_Pfetvar = (PfetConst *)A_Oldgate->InstVar;
  PfetVar = (PfetConst *)Malloc(sizeof(PfetConst));
  *PfetVar = *Old_Pfetvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)PfetVar;
  AnaCapCopy(act->inst);   /*6*/
}

Local Void Dispose_Pfet6(act)
Analog_32_action *act;
{
  PfetConst *PfetVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  Free(PfetVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Pfet6(act)
Analog_32_action *act;
{
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICPfet *ICptr;
  log_grec *WITH;

  AnaCapattrread(1L, act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICPfet *)Cellptr->Ainfo;
    A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
    A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
    A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
    A_Gate->Pininfo[3].Cap = ICptr->InitCwb;
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
  PfetVar->k1 = 2 * WITH->attr[N_Ratio - 1].UU.r *
		WITH->attr[N_Beta - 1].UU.r * WITH->attr[N_Ktq - 1].UU.r *
		WITH->attr[N_Ktq - 1].UU.r / WITH->attr[N_Kappa - 1].UU.r;
  PfetVar->k2 = WITH->attr[N_Kappa - 1].UU.r/(2 * WITH->attr[N_Ktq - 1].UU.r);
  PfetVar->k3 = 1 / WITH->attr[N_Kappa - 1].UU.r;
  PfetVar->k4 = 2 * WITH->attr[N_Ratio - 1].UU.r *
		WITH->attr[N_Beta - 1].UU.r * WITH->attr[N_Ktq - 1].UU.r;
  PfetVar->k5 = (WITH->attr[N_Kappa - 1].UU.r * WITH->attr[N_Vt - 1].UU.r)/ 
		(2 * WITH->attr[N_Ktq - 1].UU.r);
  PfetVar->k6 = 1 / (2 * WITH->attr[N_Ktq - 1].UU.r);
  PfetVar->k7 = (1 - WITH->attr[N_Kappa - 1].UU.r)/
                (2 * WITH->attr[N_Ktq - 1].UU.r);
  PfetVar->k8 = 2 * WITH->attr[N_Ratio - 1].UU.r *
		WITH->attr[N_Beta - 1].UU.r * WITH->attr[N_Ktq - 1].UU.r
                *((1/WITH->attr[N_Kappa - 1].UU.r) - 1);
  if (WITH->attr[N_Early - 1].blnk)
    PfetVar->InvVcross = 0.0;
  else
    PfetVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
  PfetVar->TauCgd = 0.0;
  PfetVar->TauCgs = 0.0;
}

Local Void Probe_Pfet6(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr, *Node3Ptr, *Node4Ptr;
  double Node1, Node2, Node3, Node4, Pin1, Pin2, Pin3, Pin4, d1, d2;
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
  Pin4 = A_Gate->Pininfo[3].Vmem;
  Node1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Node2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Node3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  Node4Ptr = (AnaExt_rec *)WITH->pin[3]->info;
  Node1 = Node1Ptr->ltimestep;
  Node2 = Node2Ptr->ltimestep;
  Node3 = Node3Ptr->ltimestep;
  Node4 = Node4Ptr->ltimestep;
  if (Node1 != Pin1 || Node2 != Pin2 || Node3 != Pin3 || Node4 != Pin4 ||
      Node1 == AnaNotyet || Node2 == AnaNotyet || Node3 == AnaNotyet ||
      Node4 == AnaNotyet) {
    sprintf(STR1, "%s <undefined>", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  d1 = Pin2 - Pin3;
  if (d1 >= 0) {
    d2 = Pin2 - Pin1;
    strcpy(Name2, " (drain)");
    strcpy(Name3, " (source)");
  } else {
    d1 = -d1;
    d2 = Pin3 - Pin1;
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

  case 4:
    sprintf(STR2, "%s (well)", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  }
  sprintf(STR3, "Vsd = %s", AnaProbeVoltStr(STR4, d1));
  AnaScoreboard(STR3, (long)AnaMessGate2);
  sprintf(STR3, "Vsg = %s", AnaProbeVoltStr(STR4, d2));
  AnaScoreboard(STR3, (long)AnaMessGate3);
}




/*==========================================================================*/
/*                 Library Listing For   Cell PFET6                    */
/*==========================================================================*/


Void Log_PFET6_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_PFET6(act);
    break;

  case Analog_act_ex:
    Ex_PFET6(act);
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
    Attr_Pfet6(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Pfet6(act);
    break;

  case Analog_act_copygate:
    Copygate_Pfet6(act);
    break;

  case Analog_act_disposegate:
    Dispose_Pfet6(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Pfet6(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_PFET6(act);
    break;

  case Analog_act_probe:
    Probe_Pfet6(act);
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
