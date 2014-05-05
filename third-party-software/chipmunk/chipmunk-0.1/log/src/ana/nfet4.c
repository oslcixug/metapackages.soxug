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





#define NFET4_G
#include "nfet4.h"


#define Cstart          1
#define N_Cgd           10
#define N_Cgs           11
#define N_Ratio         12
#define N_Vt            13
#define N_Leakage       14
#define N_FitFactor     15
#define N_Ktqn          16
#define N_Early         17

#define Gatename        "NFET"


typedef struct NfetConst {
  double V0;   /*kt/qn*/
  double Kn1;   /*nfet diode leakage current*(W/L)*/
  double Kn2;   /*qn/kt*/
  double Kn3;   /*Tanch(kn3*x) ~=.99 when x=1*/
  double Kn4;   /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  double TwoKn4;   /*2*Kn4*/
  double Kn5;   /*Expo(-Kn2*Vtn)*/
  double InvVcross;   /*Inverse Early Voltage*/
  double TauCgd;   /*Tau for capacitor*/
  double TauCgs;   /*Tau for capacitor*/
} NfetConst;


typedef struct ICNfet {
  double InitRatio, InitVt, InitLeakage, InitFitFactor, InitKtqn, InitCgd,
	 InitCgs, InitCgb, InitCdb, InitCsb, InitEarly;
} ICNfet;



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_NFET4(act)
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
  double Vds, Vgs, Vgd, ExpVgs, ExpVgd, ExpVds, BlendVgs, BlendVgd, BlendVds,
	 InvBlendVgs, InvBlendVgd, InvBlendVds, dBlendVgsdS, dBlendVgddD,
	 dBlendVdsdS, Temp1, Temp2, Temp4, OldIforward, Iforward, OldIreverse,
	 Ireverse, dIforwarddD, dIforwarddG, dIforwarddS, dIreversedD,
	 dIreversedG, dIreversedS, Finv, F, dFinvdD, dFinvdS, dFinvdG,
	 Saturate, dTanch, Itotal, Ids, dIdsdG, dIdsdS, dIdsdD, Resforward,
	 Resreverse, dSaturatedD_Iforward, dSaturatedG_Iforward,
	 dSaturatedS_Iforward, dSaturatedS_Ireverse, dSaturatedG_Ireverse,
	 dSaturatedD_Ireverse, dIforwarddG_Saturate, dIforwarddD_Saturate,
	 dIforwarddS_Saturate, dIreversedG_Saturate, dIreversedD_Saturate,
	 dIreversedS_Saturate, Vgs_InvBlendVgs, Vgs_dBlendVgsds, Kn1_ExpVgs,
	 Kn1_ExpVgs_BlendVgs, Kn4_Vgs, Vgd_InvBlendVgd, Vgd_dBlendVgddD,
	 Kn1_ExpVgd, Kn1_ExpVgd_BlendVgd, Kn4_Vgd, ResForward_InvBlendVds,
	 BlendVds_ResReverse, Vgs_InvBlendVgs_dBlendVdsdS,
	 InvBlendVgd_BlendVds, Vgd_BlendVds_dBlendVgddD,
	 Vgd_InvBlendVgd_dBlendVdsdS, InvBlendVgs_InvBlendVds,
	 Vgs_InvBlendVds_dBlendVgsdS, Vds_dTanch_Temp3_Itotal,
	 F_dTanch_Itotal, dSaturatedD_Itotal, dSaturatedG_Itotal,
	 dSaturatedS_Itotal;
/* p2c: gates.text, line 1996: 
 * Note: Line breaker spent 4.0 seconds, 5000 tries on line 85 [251] */
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
  VPin1 = Pin1Ptr->last;
  VPin2 = Pin2Ptr->last;
  VPin3 = Pin3Ptr->last;
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*2*/
  VMem2 = A_Gate->Pininfo[1].Vmem;
  VMem3 = A_Gate->Pininfo[2].Vmem;
  Vds = VPin2 - VPin3;
  Vgs = VPin1 - VPin3;
  Vgd = VPin1 - VPin2;
  if (fabs(Vds) < 0.000708) {
    ExpVgs = AnaExpo(NfetVar->Kn2 * Vgs);
    ExpVgd = AnaExpo(NfetVar->Kn2 * Vgd);
    Temp1 = NfetVar->Kn5 * ExpVgs;
    BlendVgs = 1 / (1 + Temp1);
    InvBlendVgs = 1 - BlendVgs;
    dBlendVgsdS = Temp1 * NfetVar->Kn2 * BlendVgs * BlendVgs;
    Temp2 = NfetVar->Kn5 * ExpVgd;
    BlendVgd = 1 / (1 + Temp2);
    InvBlendVgd = 1 - BlendVgd;
    dBlendVgddD = Temp2 * NfetVar->Kn2 * BlendVgd * BlendVgd;
    ExpVds = AnaExpo(1000000L * Vds);
    BlendVds = 1 / (1 + ExpVds);
    InvBlendVds = 1 - BlendVds;
    dBlendVdsdS = ExpVds * BlendVds * BlendVds;
    dBlendVdsdS = 1000000L * dBlendVdsdS;
    Temp4 = Vds * NfetVar->InvVcross;
    Resforward = 1 + Temp4;
    Resreverse = 1 - Temp4;
    Kn4_Vgs = NfetVar->Kn4 * Vgs;
    Kn1_ExpVgs = NfetVar->Kn1 * ExpVgs;
    Kn1_ExpVgs_BlendVgs = Kn1_ExpVgs * BlendVgs;
    Vgs_InvBlendVgs = Vgs * InvBlendVgs;
    OldIforward = Kn1_ExpVgs_BlendVgs + Vgs_InvBlendVgs * Kn4_Vgs;
    ResForward_InvBlendVds = Resforward * InvBlendVds;
    Iforward = ResForward_InvBlendVds * OldIforward;
    dIforwarddG = ResForward_InvBlendVds *
	(Kn1_ExpVgs_BlendVgs * NfetVar->Kn2 - Kn1_ExpVgs * dBlendVgsdS +
	 NfetVar->TwoKn4 * Vgs_InvBlendVgs + Kn4_Vgs * Vgs * dBlendVgsdS);
    dIforwarddD = (dBlendVdsdS * Resforward +
		   NfetVar->InvVcross * InvBlendVds) * OldIforward;
    dIforwarddS = -(dIforwarddG + dIforwarddD);
    Kn1_ExpVgd = NfetVar->Kn1 * ExpVgd;
    Kn1_ExpVgd_BlendVgd = Kn1_ExpVgd * BlendVgd;
    Kn4_Vgd = NfetVar->Kn4 * Vgd;
    Vgd_InvBlendVgd = Vgd * InvBlendVgd;
    OldIreverse = Kn1_ExpVgd_BlendVgd + Vgd_InvBlendVgd * Kn4_Vgd;
    BlendVds_ResReverse = BlendVds * Resreverse;
    Ireverse = BlendVds_ResReverse * OldIreverse;
    dIreversedG = BlendVds_ResReverse *
	(Kn1_ExpVgd_BlendVgd * NfetVar->Kn2 - Kn1_ExpVgd * dBlendVgddD +
	 NfetVar->TwoKn4 * Vgd_InvBlendVgd + Kn4_Vgd * Vgd * dBlendVgddD);
    dIreversedS = (dBlendVdsdS * Resreverse + BlendVds * NfetVar->InvVcross) *
		  OldIreverse;
    dIreversedD = -(dIreversedG + dIreversedS);
    Finv = NfetVar->V0 + Vgs_InvBlendVgs * InvBlendVds + Vgd_InvBlendVgd * BlendVds;
    F = NfetVar->Kn3 / Finv;
    Vgs_InvBlendVgs_dBlendVdsdS = Vgs_InvBlendVgs * dBlendVdsdS;
    InvBlendVgd_BlendVds = InvBlendVgd * BlendVds;
    Vgd_BlendVds_dBlendVgddD = Vgd * BlendVds * dBlendVgddD;
    Vgd_InvBlendVgd_dBlendVdsdS = Vgd_InvBlendVgd * dBlendVdsdS;
    InvBlendVgs_InvBlendVds = InvBlendVgs * InvBlendVds;
    Vgs_InvBlendVds_dBlendVgsdS = Vgs * InvBlendVds * dBlendVgsdS;
    dFinvdD = Vgs_InvBlendVgs_dBlendVdsdS - InvBlendVgd_BlendVds -
	      Vgd_BlendVds_dBlendVgddD - Vgd_InvBlendVgd_dBlendVdsdS;
    dFinvdS = Vgd_InvBlendVgd_dBlendVdsdS - Vgs_InvBlendVds_dBlendVgsdS -
	      Vgs_InvBlendVgs_dBlendVdsdS - InvBlendVgs_InvBlendVds;
    dFinvdG = InvBlendVgs_InvBlendVds + Vgs_InvBlendVds_dBlendVgsdS +
	      InvBlendVgd_BlendVds + Vgd_BlendVds_dBlendVgddD;
    Saturate = AnaTanch(Vds * F);
    Itotal = Iforward + Ireverse;
    F_dTanch_Itotal = F * Itotal * (1 - Saturate * Saturate);
    Vds_dTanch_Temp3_Itotal = -(Vds * F_dTanch_Itotal / Finv);
    dSaturatedD_Itotal = Vds_dTanch_Temp3_Itotal * dFinvdD + F_dTanch_Itotal;
    dSaturatedG_Itotal = Vds_dTanch_Temp3_Itotal * dFinvdG;
    dSaturatedS_Itotal = Vds_dTanch_Temp3_Itotal * dFinvdS - F_dTanch_Itotal;
    Ids = Itotal * Saturate;
    dIdsdG = (dIforwarddG + dIreversedG) * Saturate + dSaturatedG_Itotal;
    dIdsdS = (dIforwarddS + dIreversedS) * Saturate + dSaturatedS_Itotal;
    dIdsdD = (dIforwarddD + dIreversedD) * Saturate + dSaturatedD_Itotal;
  } else {
    if (Vds > 0) {
      ExpVgs = AnaExpo(NfetVar->Kn2 * Vgs);
      Temp1 = NfetVar->Kn5 * ExpVgs;
      BlendVgs = 1 / (1 + Temp1);
      InvBlendVgs = 1 - BlendVgs;
      dBlendVgsdS = Temp1 * NfetVar->Kn2 * BlendVgs * BlendVgs;
      Vgs_InvBlendVgs = Vgs * InvBlendVgs;
      Vgs_dBlendVgsds = Vgs * dBlendVgsdS;
      Kn4_Vgs = NfetVar->Kn4 * Vgs;
      Kn1_ExpVgs = NfetVar->Kn1 * ExpVgs;
      Kn1_ExpVgs_BlendVgs = Kn1_ExpVgs * BlendVgs;
      Resforward = 1 + Vds * NfetVar->InvVcross;
      OldIforward = Kn1_ExpVgs_BlendVgs + Vgs_InvBlendVgs * Kn4_Vgs;
      Iforward = Resforward * OldIforward;
      Finv = 1 / (NfetVar->V0 + Vgs_InvBlendVgs);
      F = NfetVar->Kn3 * Finv;
      Saturate = AnaTanch(Vds * F);
      dIforwarddG_Saturate = Resforward * Saturate *
	  (Kn1_ExpVgs_BlendVgs * NfetVar->Kn2 - Kn1_ExpVgs * dBlendVgsdS +
	   NfetVar->TwoKn4 * Vgs_InvBlendVgs + Kn4_Vgs * Vgs_dBlendVgsds);
      dIforwarddD_Saturate = NfetVar->InvVcross * OldIforward * Saturate;
      dIforwarddS_Saturate = -(dIforwarddG_Saturate + dIforwarddD_Saturate);
      dTanch = 1 - Saturate * Saturate;
      dSaturatedD_Iforward = F * dTanch * Iforward;
      dSaturatedG_Iforward = -Vds * dSaturatedD_Iforward * Finv *
			      (InvBlendVgs + Vgs_dBlendVgsds);
      dSaturatedS_Iforward = -(dSaturatedG_Iforward + dSaturatedD_Iforward);
      Ids = Iforward * Saturate;
      dIdsdG = dIforwarddG_Saturate + dSaturatedG_Iforward;
      dIdsdS = dIforwarddS_Saturate + dSaturatedS_Iforward;
      dIdsdD = dIforwarddD_Saturate + dSaturatedD_Iforward;
    } else {
      ExpVgd = AnaExpo(NfetVar->Kn2 * Vgd);
      Temp2 = NfetVar->Kn5 * ExpVgd;
      BlendVgd = 1 / (1 + Temp2);
      InvBlendVgd = 1 - BlendVgd;
      dBlendVgddD = Temp2 * NfetVar->Kn2 * BlendVgd * BlendVgd;
      Vgd_InvBlendVgd = Vgd * InvBlendVgd;
      Vgd_dBlendVgddD = Vgd * dBlendVgddD;
      Kn1_ExpVgd = NfetVar->Kn1 * ExpVgd;
      Kn1_ExpVgd_BlendVgd = Kn1_ExpVgd * BlendVgd;
      Kn4_Vgd = NfetVar->Kn4 * Vgd;
      Resreverse = 1 - Vds * NfetVar->InvVcross;
      OldIreverse = Kn1_ExpVgd_BlendVgd + Vgd_InvBlendVgd * Kn4_Vgd;
      Ireverse = Resreverse * OldIreverse;
      Finv = 1 / (NfetVar->V0 + Vgd_InvBlendVgd);
      F = NfetVar->Kn3 * Finv;
      Saturate = AnaTanch(Vds * F);
      dIreversedG_Saturate = Resreverse * Saturate *
	  (Kn1_ExpVgd_BlendVgd * NfetVar->Kn2 - Kn1_ExpVgd * dBlendVgddD +
	   NfetVar->TwoKn4 * Vgd_InvBlendVgd + Kn4_Vgd * Vgd_dBlendVgddD);
      dIreversedS_Saturate = NfetVar->InvVcross * OldIreverse * Saturate;
      dIreversedD_Saturate = -(dIreversedG_Saturate + dIreversedS_Saturate);
      dTanch = 1 - Saturate * Saturate;
      dSaturatedS_Ireverse = -F * dTanch * Ireverse;
      dSaturatedG_Ireverse = Vds * dSaturatedS_Ireverse * Finv *
			     (InvBlendVgd + Vgd_dBlendVgddD);
      dSaturatedD_Ireverse = -(dSaturatedG_Ireverse + dSaturatedS_Ireverse);
      Ids = Ireverse * Saturate;
      dIdsdG = dIreversedG_Saturate + dSaturatedG_Ireverse;
      dIdsdS = dIreversedS_Saturate + dSaturatedS_Ireverse;
      dIdsdD = dIreversedD_Saturate + dSaturatedD_Ireverse;
    }
  }
  MinI3 = Ids;
  MinI2 = -Ids;
  dI2dV1 = dIdsdG;
  dI3dV1 = -dIdsdG;
  dI2dV3 = dIdsdS;
  dI3dV3 = -dIdsdS;
  dI2dV2 = dIdsdD;
  dI3dV2 = -dIdsdD;
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
}  /*Ex_NFET4*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/

Local Void Iin_NFET4(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double Vds, Vgs, Vgd, ExpVgs, ExpVgd, BlendVgs, BlendVgd, BlendVds,
	 InvBlendVgs, InvBlendVgd, InvBlendVds, Temp1, Tempf, Tempb, Iforward,
	 Ireverse, Ids;
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
    Vds = VPin2 - VPin3;
    if (fabs(Vds) < 0.000708) {
      Vgs = VPin1 - VPin3;
      Vgd = VPin1 - VPin2;
      ExpVgs = AnaExpo(NfetVar->Kn2 * Vgs);
      BlendVgs = 1 / (1 + NfetVar->Kn5 * ExpVgs);
      InvBlendVgs = 1 - BlendVgs;
      ExpVgd = AnaExpo(NfetVar->Kn2 * Vgd);
      BlendVgd = 1 / (1 + NfetVar->Kn5 * ExpVgd);
      InvBlendVgd = 1 - BlendVgd;
      BlendVds = 1 / (1 + AnaExpo(1000000L * Vds));
      InvBlendVds = 1 - BlendVds;
      Temp1 = Vds * NfetVar->InvVcross;
      Tempf = InvBlendVgs * Vgs;
      Tempb = InvBlendVgd * Vgd;
      Iforward = (1 + Temp1) * InvBlendVds *
	  (NfetVar->Kn1 * ExpVgs * BlendVgs + Tempf * NfetVar->Kn4 * Vgs);
      Ireverse = (1 - Temp1) * BlendVds *
	  (NfetVar->Kn1 * ExpVgd * BlendVgd + Tempb * NfetVar->Kn4 * Vgd);
      Ids = (Iforward + Ireverse) * AnaTanch(Vds * NfetVar->Kn3 /
		(NfetVar->V0 + Tempb * BlendVds + Tempf * InvBlendVds));
    } else {
      if (Vds > 0) {
	Vgs = VPin1 - VPin3;
	ExpVgs = AnaExpo(NfetVar->Kn2 * Vgs);
	BlendVgs = 1 / (1 + NfetVar->Kn5 * ExpVgs);
	InvBlendVgs = 1 - BlendVgs;
	Temp1 = InvBlendVgs * Vgs;
	Ids = (1 + Vds * NfetVar->InvVcross) *
	      (NfetVar->Kn1 * ExpVgs * BlendVgs + Temp1 * NfetVar->Kn4 * Vgs) *
	      AnaTanch(Vds * NfetVar->Kn3 / (NfetVar->V0 + Temp1));
      } else {
	Vgd = VPin1 - VPin2;
	ExpVgd = AnaExpo(NfetVar->Kn2 * Vgd);
	BlendVgd = 1 / (1 + NfetVar->Kn5 * ExpVgd);
	InvBlendVgd = 1 - BlendVgd;
	Temp1 = InvBlendVgd * Vgd;
	Ids = (1 - Vds * NfetVar->InvVcross) *
	      (NfetVar->Kn1 * ExpVgd * BlendVgd + Temp1 * NfetVar->Kn4 * Vgd) *
	      AnaTanch(Vds * NfetVar->Kn3 / (NfetVar->V0 + Temp1));
      }
    }
    switch (act->pin) {

    case 2:
      act->Iin += Ids;
      break;

    case 3:
      act->Iin -= Ids;
      break;
    }
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_NFET4*/


Local Void GetCnf_Nfet4(NewIC)
ICNfet *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICNfet *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("NFET4", Arg, &Found);
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
      if (!strcmp(Keyword, "I0")) {
	TRY(try3);
	  WITH->InitLeakage = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitLeakage = 5e-17;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "FITFACTOR")) {
	TRY(try4);
	  WITH->InitFitFactor = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitFitFactor = 3.33;
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

Local Void initlib_NFET4(act)
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
  NewIC->InitLeakage = 5e-17;
  NewIC->InitFitFactor = 3.33;
  NewIC->InitKtqn = 0.04;
  NewIC->InitCgs = 10e-15;
  NewIC->InitCgd = 10e-15;
  NewIC->InitCgb = 10e-15;
  NewIC->InitCdb = 10e-15;
  NewIC->InitCsb = 10e-15;
  NewIC->InitEarly = 50.0;
  GetCnf_Nfet4(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*NFET4_Initlib*/



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
      NfetVar->Kn1 = Inst->attr[N_Ratio - 1].UU.r * Inst->attr[N_Leakage - 1].UU.r;
      NfetVar->Kn4 = NfetVar->Kn1 *
		     AnaExpo(NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      NfetVar->TwoKn4 = 2 * NfetVar->Kn4;
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else {
      NfetVar->Kn4 = NfetVar->Kn1 *
		     AnaExpo(NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      NfetVar->TwoKn4 = 2 * NfetVar->Kn4;
      NfetVar->Kn5 = AnaExpo(-NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Leakage:
    if ((unsigned)Inst->attr[N_Leakage - 1].UU.r > 1 ||
	Inst->attr[N_Leakage - 1].blnk)
      Result = false;
    else {
      NfetVar->Kn1 = Inst->attr[N_Ratio - 1].UU.r * Inst->attr[N_Leakage - 1].UU.r;
      NfetVar->Kn4 = NfetVar->Kn1 *
		     AnaExpo(NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      NfetVar->TwoKn4 = 2 * NfetVar->Kn4;
    }
    break;

  case N_FitFactor:
    if ((unsigned)Inst->attr[N_FitFactor - 1].UU.r > 20 ||
	Inst->attr[N_FitFactor - 1].blnk)
      Result = false;
    else
      NfetVar->Kn3 = Inst->attr[N_FitFactor - 1].UU.r;
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[N_Ktqn - 1].UU.r > 1 || Inst->attr[N_Ktqn - 1].blnk)
      Result = false;
    else {
      NfetVar->V0 = Inst->attr[N_Ktqn - 1].UU.r;
      NfetVar->Kn2 = 1 / NfetVar->V0;
      NfetVar->Kn4 = NfetVar->Kn1 *
		     AnaExpo(NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      NfetVar->TwoKn4 = 2 * NfetVar->Kn4;
      NfetVar->Kn5 = AnaExpo(-NfetVar->Kn2 * Inst->attr[N_Vt - 1].UU.r);
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


Local Void Attr_Nfet4(act)
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


Local Void Newgate_Nfet4(act)
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
  NfetVar->V0 = ICptr->InitKtqn;   /*kt/qn*/
  NfetVar->Kn1 = ICptr->InitRatio * ICptr->InitLeakage;
      /*nfet diode leakage current*(W/L)*/
  NfetVar->Kn2 = 1 / NfetVar->V0;   /*qn/kt*/
  NfetVar->Kn3 = ICptr->InitFitFactor;   /*Tanch(kn3*x) ~=.99 when x=1*/
  NfetVar->Kn4 = NfetVar->Kn1 * AnaExpo(NfetVar->Kn2 * ICptr->InitVt) /
		 (ICptr->InitVt * ICptr->InitVt);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  NfetVar->TwoKn4 = 2 * NfetVar->Kn4;   /*2*Kn4*/
  NfetVar->Kn5 = AnaExpo(-NfetVar->Kn2 * ICptr->InitVt);   /*Expo(-Kn2*Vtn)*/
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
  WITH->attr[N_Leakage - 1].UU.r = ICptr->InitLeakage;   /*leakage current*/
  WITH->attr[N_Leakage - 1].blnk = false;
  WITH->attr[N_Leakage - 1].changed = true;
  WITH->attr[N_FitFactor - 1].UU.r = ICptr->InitFitFactor;   /*fit factor*/
  WITH->attr[N_FitFactor - 1].blnk = false;
  WITH->attr[N_FitFactor - 1].changed = true;
  WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;   /*Kt/qn*/
  WITH->attr[N_Ktqn - 1].blnk = false;
  WITH->attr[N_Ktqn - 1].changed = true;
  WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;
  WITH->attr[N_Early - 1].blnk = false;
  WITH->attr[N_Early - 1].changed = true;
}

Local Void Copygate_Nfet4(act)
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

Local Void Dispose_Nfet4(act)
Analog_32_action *act;
{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  Free(NfetVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Nfet4(act)
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
    WITH->attr[N_Leakage - 1].UU.r = ICptr->InitLeakage;   /*leakage current*/
    WITH->attr[N_Leakage - 1].blnk = false;
    WITH->attr[N_Leakage - 1].changed = true;
    WITH->attr[N_FitFactor - 1].UU.r = ICptr->InitFitFactor;   /*fit factor*/
    WITH->attr[N_FitFactor - 1].blnk = false;
    WITH->attr[N_FitFactor - 1].changed = true;
    WITH->attr[N_Ktqn - 1].UU.r = ICptr->InitKtqn;   /*Kt/qn*/
    WITH->attr[N_Ktqn - 1].blnk = false;
    WITH->attr[N_Ktqn - 1].changed = true;
    WITH->attr[N_Early - 1].UU.r = ICptr->InitEarly;
    WITH->attr[N_Early - 1].blnk = false;
    WITH->attr[N_Early - 1].changed = true;
  }
  WITH = act->inst;
  NfetVar->V0 = WITH->attr[N_Ktqn - 1].UU.r;   /*kt/qn*/
  NfetVar->Kn1 = WITH->attr[N_Ratio - 1].UU.r * WITH->attr[N_Leakage - 1].UU.r;
      /*diode leakage current*(W/L)*/
  NfetVar->Kn2 = 1 / NfetVar->V0;   /*qn/kt*/
  NfetVar->Kn3 = WITH->attr[N_FitFactor - 1].UU.r;
      /*Tanch(kn3*x) ~=.99 when x=1*/
  NfetVar->Kn4 = NfetVar->Kn1 * AnaExpo(NfetVar->Kn2 * WITH->attr[N_Vt - 1].UU.r) /
		 (WITH->attr[N_Vt - 1].UU.r * WITH->attr[N_Vt - 1].UU.r);
      /*Kn1*Expo(Kn2*Vtn)/(Vtn*Vtn)*/
  NfetVar->TwoKn4 = 2 * NfetVar->Kn4;   /*2*Kn4*/
  NfetVar->Kn5 = AnaExpo(-NfetVar->Kn2 * WITH->attr[N_Vt - 1].UU.r);
      /*Expo(-Kn2*Vtn)*/
  if (WITH->attr[N_Early - 1].blnk)
    NfetVar->InvVcross = 0.0;
  else
    NfetVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
  NfetVar->TauCgd = 0.0;
  NfetVar->TauCgs = 0.0;
}

Local Void Probe_Nfet4(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr, *Node3Ptr;
  double Node1, Node2, Node3, Pin1, Pin2, Pin3, d1, d2;
  Char Name2[256], Name3[256];
  log_grec *WITH;
  Char STR1[18];
  Char STR2[12];
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
/*                 Library Listing For   Cell NFET4                    */
/*==========================================================================*/


Void Log_nfet4_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_NFET4(act);
    break;

  case Analog_act_ex:
    Ex_NFET4(act);
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
    Attr_Nfet4(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Nfet4(act);
    break;

  case Analog_act_copygate:
    Copygate_Nfet4(act);
    break;

  case Analog_act_disposegate:
    Dispose_Nfet4(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Nfet4(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_NFET4(act);
    break;

  case Analog_act_probe:
    Probe_Nfet4(act);
    break;
  }
}

#undef Cstart
#undef N_Cgd
#undef N_Cgs
#undef N_Ratio
#undef N_Vt
#undef N_Leakage
#undef N_FitFactor
#undef N_Ktqn
#undef N_Early
#undef Gatename




/* End. */
