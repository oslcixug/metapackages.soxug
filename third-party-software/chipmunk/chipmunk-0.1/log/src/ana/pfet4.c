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


#define PFET4_G
#include "pfet4.h"


#define Cstart          1
#define N_Cgd           10
#define N_Cgs           11
#define N_Ratio         12
#define N_Vt            13
#define N_Leakage       14
#define N_FitFactor     15
#define N_Ktqn          16
#define N_Early         17

#define GateName        "PFET"


typedef struct PfetConst {
  double V0;   /*kt/qn*/
  double Kp1;   /*Pfet diode leakage current*(W/L)*/
  double Kp2;   /*qn/kt*/
  double Kp3;   /*Tanch(Kp3*x) ~=.99 when x=1*/
  double Kp4;   /*Kp1*Expo(Kp2*Vtp)/(Vtp*Vtp)*/
  double TwoKp4;   /*2*Kp4*/
  double Kp5;   /*Expo(-Kp2*Vtp)*/
  double InvVcross;   /*Inverse Early Voltage*/
  double TauCgd, TauCgs;
} PfetConst;


typedef struct ICPfet {
  double InitRatio, InitVt, InitLeakage, InitFitFactor, InitKtqn, InitCgd,
	 InitCgs, InitCgb, InitCdb, InitCsb, InitEarly;
} ICPfet;


/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_PFET4(act)
Analog_32_action *act;
{
  double MinI2, dI2dV1, dI2dV2, dI2dV3, MinI3, dI3dV1, dI3dV2, dI3dV3, MinI1,
	 dI1dV1, dI1dV2, dI1dV3;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  long Pin1, Pin2, Pin3;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;   /*1*/
  double ICap;
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  double Vds, Vsg, Vdg, ExpVsg, ExpVdg, ExpVds, BlendVsg, BlendVdg, BlendVds,
	 InvBlendVsg, InvBlendVdg, InvBlendVds, dBlendVsgdG, dBlendVdgdG,
	 dBlendVdsdS, Temp1, Temp2, Temp4, OldIforward, Iforward, OldIreverse,
	 Ireverse, dIforwarddD, dIforwarddG, dIforwarddS, dIreversedD,
	 dIreversedG, dIreversedS, Finv, F, dFinvdD, dFinvdS, dFinvdG,
	 Saturate, dTanch, Itotal, Ids, dIdsdG, dIdsdS, dIdsdD, Resforward,
	 Resreverse, dSaturatedS_Iforward, dSaturatedD_Iforward,
	 dSaturatedG_Iforward, dSaturatedD_Ireverse, dSaturatedG_Ireverse,
	 dSaturatedS_Ireverse, dIforwarddG_Saturate, dIforwarddS_Saturate,
	 dIforwarddD_Saturate, dIreversedG_Saturate, dIreversedD_Saturate,
	 dIreversedS_Saturate, Vdg_dBlendVdgdG, Vdg_InvBlendVdg, Kp1_ExpVdg,
	 Kp1_ExpVdg_BlendVdg, Kp4_Vdg, Vsg_dBlendVsgdG, Vsg_InvBlendVsg,
	 Kp1_ExpVsg, Kp1_ExpVsg_BlendVsg, Kp4_Vsg, ResForward_InvBlendVds,
	 BlendVds_ResReverse, Vsg_InvBlendVsg_dBlendVdsdS,
	 InvBlendVdg_InvBlendVds, Vdg_InvBlendVds_dBlendVdgdG,
	 Vdg_InvBlendVdg_dBlendVdsdS, InvBlendVsg_BlendVds,
	 Vsg_BlendVds_dBlendVsgdG, Vds_dTanch_Temp3_Itotal, F_dTanch_Itotal,
	 dSaturatedD_Itotal, dSaturatedG_Itotal, dSaturatedS_Itotal;
/* p2c: gates.text, line 998: 
 * Note: Line breaker spent 3.0+1.00 seconds, 5000 tries on line 79 [251] */
  log_grec *WITH;
  AnaMatRec *WITH1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
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
  Vds = VPin3 - VPin2;
  Vsg = VPin2 - VPin1;
  Vdg = VPin3 - VPin1;
  if (fabs(Vds) < 0.000708) {
    ExpVsg = AnaExpo(PfetVar->Kp2 * Vsg);
    ExpVdg = AnaExpo(PfetVar->Kp2 * Vdg);
    Temp1 = PfetVar->Kp5 * ExpVsg;
    BlendVsg = 1 / (1 + Temp1);
    InvBlendVsg = 1 - BlendVsg;
    dBlendVsgdG = Temp1 * PfetVar->Kp2 * BlendVsg * BlendVsg;
    Temp2 = PfetVar->Kp5 * ExpVdg;
    BlendVdg = 1 / (1 + Temp2);
    InvBlendVdg = 1 - BlendVdg;
    dBlendVdgdG = Temp2 * PfetVar->Kp2 * BlendVdg * BlendVdg;
    ExpVds = AnaExpo(1000000L * Vds);
    BlendVds = 1 / (1 + ExpVds);
    InvBlendVds = 1 - BlendVds;
    dBlendVdsdS = ExpVds * BlendVds * BlendVds;
    dBlendVdsdS = 1000000L * dBlendVdsdS;
    Temp4 = Vds * PfetVar->InvVcross;
    Resforward = 1 + Temp4;
    Resreverse = 1 - Temp4;
    Kp1_ExpVdg = PfetVar->Kp1 * ExpVdg;
    Kp1_ExpVdg_BlendVdg = Kp1_ExpVdg * BlendVdg;
    Kp4_Vdg = PfetVar->Kp4 * Vdg;
    Vdg_InvBlendVdg = Vdg * InvBlendVdg;
    OldIforward = Kp1_ExpVdg_BlendVdg + Vdg_InvBlendVdg * Kp4_Vdg;
    ResForward_InvBlendVds = Resforward * InvBlendVds;
    Iforward = ResForward_InvBlendVds * OldIforward;
    dIforwarddG = ResForward_InvBlendVds *
	(Kp1_ExpVdg * dBlendVdgdG - Kp1_ExpVdg_BlendVdg * PfetVar->Kp2 -
	 PfetVar->TwoKp4 * Vdg_InvBlendVdg - Kp4_Vdg * Vdg * dBlendVdgdG);
    dIforwarddS = -(dBlendVdsdS * Resforward +
		    PfetVar->InvVcross * InvBlendVds) * OldIforward;
    dIforwarddD = -(dIforwarddG + dIforwarddS);
    Kp1_ExpVsg = PfetVar->Kp1 * ExpVsg;
    Kp1_ExpVsg_BlendVsg = Kp1_ExpVsg * BlendVsg;
    Kp4_Vsg = PfetVar->Kp4 * Vsg;
    Vsg_InvBlendVsg = Vsg * InvBlendVsg;
    OldIreverse = Kp1_ExpVsg_BlendVsg + Vsg_InvBlendVsg * Kp4_Vsg;
    BlendVds_ResReverse = BlendVds * Resreverse;
    Ireverse = BlendVds_ResReverse * OldIreverse;
    dIreversedG = BlendVds_ResReverse *
	(Kp1_ExpVsg * dBlendVsgdG - Kp1_ExpVsg_BlendVsg * PfetVar->Kp2 -
	 PfetVar->TwoKp4 * Vsg_InvBlendVsg - Kp4_Vsg * Vsg * dBlendVsgdG);
    dIreversedD = -(dBlendVdsdS * Resreverse + BlendVds * PfetVar->InvVcross) *
		   OldIreverse;
    dIreversedS = -(dIreversedG + dIreversedD);
    Finv = PfetVar->V0 + Vsg_InvBlendVsg * BlendVds + Vdg_InvBlendVdg * InvBlendVds;
    F = PfetVar->Kp3 / Finv;
    Vsg_InvBlendVsg_dBlendVdsdS = Vsg_InvBlendVsg * dBlendVdsdS;
    InvBlendVdg_InvBlendVds = InvBlendVdg * InvBlendVds;
    Vdg_InvBlendVds_dBlendVdgdG = Vdg * InvBlendVds * dBlendVdgdG;
    Vdg_InvBlendVdg_dBlendVdsdS = Vdg_InvBlendVdg * dBlendVdsdS;
    InvBlendVsg_BlendVds = InvBlendVsg * BlendVds;
    Vsg_BlendVds_dBlendVsgdG = Vsg * BlendVds * dBlendVsgdG;
    dFinvdD = InvBlendVdg_InvBlendVds - Vsg_InvBlendVsg_dBlendVdsdS +
	      Vdg_InvBlendVds_dBlendVdgdG + Vdg_InvBlendVdg_dBlendVdsdS;
    dFinvdS = InvBlendVsg_BlendVds - Vdg_InvBlendVdg_dBlendVdsdS +
	      Vsg_BlendVds_dBlendVsgdG + Vsg_InvBlendVsg_dBlendVdsdS;
    dFinvdG = -InvBlendVdg_InvBlendVds - Vdg_InvBlendVds_dBlendVdgdG -
	      InvBlendVsg_BlendVds - Vsg_BlendVds_dBlendVsgdG;
    Saturate = AnaTanch(Vds * F);
    Itotal = Iforward + Ireverse;
    F_dTanch_Itotal = Itotal * F * (1 - Saturate * Saturate);
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
      ExpVdg = AnaExpo(PfetVar->Kp2 * Vdg);
      Temp2 = PfetVar->Kp5 * ExpVdg;
      BlendVdg = 1 / (1 + Temp2);
      InvBlendVdg = 1 - BlendVdg;
      dBlendVdgdG = Temp2 * PfetVar->Kp2 * BlendVdg * BlendVdg;
      Resforward = 1 + Vds * PfetVar->InvVcross;
      Vdg_dBlendVdgdG = Vdg * dBlendVdgdG;
      Vdg_InvBlendVdg = Vdg * InvBlendVdg;
      Kp1_ExpVdg = PfetVar->Kp1 * ExpVdg;
      Kp1_ExpVdg_BlendVdg = Kp1_ExpVdg * BlendVdg;
      Kp4_Vdg = PfetVar->Kp4 * Vdg;
      OldIforward = Kp1_ExpVdg_BlendVdg + Vdg_InvBlendVdg * Kp4_Vdg;
      Iforward = Resforward * OldIforward;
      Finv = 1 / (PfetVar->V0 + Vdg_InvBlendVdg);
      F = PfetVar->Kp3 * Finv;
      Saturate = AnaTanch(Vds * F);
      dIforwarddG_Saturate = Resforward * Saturate *
	  (Kp1_ExpVdg * dBlendVdgdG - Kp1_ExpVdg_BlendVdg * PfetVar->Kp2 -
	   PfetVar->TwoKp4 * Vdg_InvBlendVdg - Kp4_Vdg * Vdg_dBlendVdgdG);
      dIforwarddS_Saturate = -PfetVar->InvVcross * OldIforward * Saturate;
      dIforwarddD_Saturate = -(dIforwarddG_Saturate + dIforwarddS_Saturate);
      dTanch = 1 - Saturate * Saturate;
      dSaturatedS_Iforward = -Iforward * F * dTanch;
      dSaturatedG_Iforward = -Vds * dSaturatedS_Iforward * Finv *
			      (InvBlendVdg + Vdg_dBlendVdgdG);
      dSaturatedD_Iforward = -(dSaturatedG_Iforward + dSaturatedS_Iforward);
      Ids = Iforward * Saturate;
      dIdsdG = dIforwarddG_Saturate + dSaturatedG_Iforward;
      dIdsdS = dIforwarddS_Saturate + dSaturatedS_Iforward;
      dIdsdD = dIforwarddD_Saturate + dSaturatedD_Iforward;
    } else {
      ExpVsg = AnaExpo(PfetVar->Kp2 * Vsg);
      Temp1 = PfetVar->Kp5 * ExpVsg;
      BlendVsg = 1 / (1 + Temp1);
      InvBlendVsg = 1 - BlendVsg;
      dBlendVsgdG = Temp1 * PfetVar->Kp2 * BlendVsg * BlendVsg;
      Resreverse = 1 - Vds * PfetVar->InvVcross;
      Vsg_dBlendVsgdG = Vsg * dBlendVsgdG;
      Vsg_InvBlendVsg = Vsg * InvBlendVsg;
      Kp1_ExpVsg = PfetVar->Kp1 * ExpVsg;
      Kp1_ExpVsg_BlendVsg = Kp1_ExpVsg * BlendVsg;
      Kp4_Vsg = PfetVar->Kp4 * Vsg;
      OldIreverse = Kp1_ExpVsg_BlendVsg + Vsg_InvBlendVsg * Kp4_Vsg;
      Ireverse = Resreverse * OldIreverse;
      Finv = 1 / (PfetVar->V0 + Vsg_InvBlendVsg);
      F = PfetVar->Kp3 * Finv;
      Saturate = AnaTanch(Vds * F);
      dIreversedG_Saturate = Resreverse * Saturate *
	  (Kp1_ExpVsg * dBlendVsgdG - Kp1_ExpVsg_BlendVsg * PfetVar->Kp2 -
	   PfetVar->TwoKp4 * Vsg_InvBlendVsg - Kp4_Vsg * Vsg_dBlendVsgdG);
      dIreversedD_Saturate = -PfetVar->InvVcross * OldIreverse * Saturate;
      dIreversedS_Saturate = -(dIreversedG_Saturate + dIreversedD_Saturate);
      dTanch = 1 - Saturate * Saturate;
      dSaturatedD_Ireverse = F * dTanch * Ireverse;
      dSaturatedG_Ireverse = Vds * dSaturatedD_Ireverse * Finv *
			     (InvBlendVsg + Vsg_dBlendVsgdG);
      dSaturatedS_Ireverse = -(dSaturatedG_Ireverse + dSaturatedD_Ireverse);
      Ids = Ireverse * Saturate;
      dIdsdG = dIreversedG_Saturate + dSaturatedG_Ireverse;
      dIdsdS = dIreversedS_Saturate + dSaturatedS_Ireverse;
      dIdsdD = dIreversedD_Saturate + dSaturatedD_Ireverse;
    }
  }
  MinI3 = -Ids;
  MinI2 = Ids;
  dI2dV1 = -dIdsdG;
  dI3dV1 = dIdsdG;
  dI2dV3 = -dIdsdD;
  dI3dV3 = dIdsdD;
  dI2dV2 = -dIdsdS;
  dI3dV2 = dIdsdS;
  MinI1 = 0.0;
  dI1dV1 = 0.0;
  dI1dV2 = 0.0;
  dI1dV3 = 0.0;
  if (WITH->attr[N_Cgs - 1].UU.r != 0) {   /*C gate-source*/
    if (AnaChangedDt)
      PfetVar->TauCgs = WITH->attr[N_Cgs - 1].UU.r / AnaDt;
    ICap = PfetVar->TauCgs * (Vsg - VMem2 + VMem1);   /*4*/
    MinI2 -= ICap;
    dI2dV1 -= PfetVar->TauCgs;
    dI2dV2 += PfetVar->TauCgs;
    MinI1 += ICap;
    dI1dV1 += PfetVar->TauCgs;
    dI1dV2 -= PfetVar->TauCgs;
  }
  if (WITH->attr[N_Cgd - 1].UU.r != 0) {   /*C gate-drain*/
    if (AnaChangedDt)
      PfetVar->TauCgd = WITH->attr[N_Cgd - 1].UU.r / AnaDt;
    ICap = PfetVar->TauCgd * (Vdg - VMem3 + VMem1);
    MinI3 -= ICap;
    dI3dV1 -= PfetVar->TauCgd;
    dI3dV3 += PfetVar->TauCgd;
    MinI1 += ICap;
    dI1dV1 += PfetVar->TauCgd;
    dI1dV3 -= PfetVar->TauCgd;
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
}  /*Ex_PFET4*/

/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_PFET4(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  double Vds, Vsg, Vdg, ExpVsg, ExpVdg, BlendVsg, BlendVdg, BlendVds,
	 InvBlendVsg, InvBlendVdg, InvBlendVds, Temp1, Tempf, Tempb, Iforward,
	 Ireverse, Ids;
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
    if (WITH1->attr[N_Cgs - 1].UU.r != 0)   /*C gate-source*/
      act->Iin = PfetVar->TauCgs * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    if (WITH1->attr[N_Cgd - 1].UU.r != 0)   /*C gate-drain*/
      act->Iin = PfetVar->TauCgd * (VPin1 - VPin3 - VMem1 + VMem3) + act->Iin;
    AnaCapIin(act);
    break;

  case 2:
  case 3:
    WITH1 = act->inst;
    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    PfetVar = (PfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    switch (act->pin) {

    case 2:
      if (WITH1->attr[N_Cgs - 1].UU.r != 0) {
	VMem2 = A_Gate->Pininfo[1].Vmem;
	act->Iin = PfetVar->TauCgs * (VPin2 - VPin1 - VMem2 + VMem1) + act->Iin;
      }
      break;

    case 3:
      if (WITH1->attr[N_Cgd - 1].UU.r != 0) {
	VMem3 = A_Gate->Pininfo[2].Vmem;
	act->Iin = PfetVar->TauCgd * (VPin3 - VPin1 - VMem3 + VMem1) + act->Iin;
      }
      break;
    }
    Vds = VPin3 - VPin2;
    if (fabs(Vds) < 0.000708) {
      Vsg = VPin2 - VPin1;
      Vdg = VPin3 - VPin1;
      ExpVsg = AnaExpo(PfetVar->Kp2 * Vsg);
      BlendVsg = 1 / (1 + PfetVar->Kp5 * ExpVsg);
      InvBlendVsg = 1 - BlendVsg;
      ExpVdg = AnaExpo(PfetVar->Kp2 * Vdg);
      BlendVdg = 1 / (1 + PfetVar->Kp5 * ExpVdg);
      InvBlendVdg = 1 - BlendVdg;
      BlendVds = 1 / (1 + AnaExpo(1000000L * Vds));
      InvBlendVds = 1 - BlendVds;
      Temp1 = Vds * PfetVar->InvVcross;
      Tempf = InvBlendVdg * Vdg;
      Tempb = InvBlendVsg * Vsg;
      Iforward = (1 + Temp1) * InvBlendVds *
	  (PfetVar->Kp1 * ExpVdg * BlendVdg + Tempf * PfetVar->Kp4 * Vdg);
      Ireverse = (1 - Temp1) * BlendVds *
	  (PfetVar->Kp1 * ExpVsg * BlendVsg + Tempb * PfetVar->Kp4 * Vsg);
      Ids = (Iforward + Ireverse) * AnaTanch(Vds * PfetVar->Kp3 /
		(PfetVar->V0 + Tempb * BlendVds + Tempf * InvBlendVds));
    } else {
      if (Vds > 0) {
	Vdg = VPin3 - VPin1;
	ExpVdg = AnaExpo(PfetVar->Kp2 * Vdg);
	BlendVdg = 1 / (1 + PfetVar->Kp5 * ExpVdg);
	InvBlendVdg = 1 - BlendVdg;
	Temp1 = InvBlendVdg * Vdg;
	Ids = (1 + Vds * PfetVar->InvVcross) *
	      (PfetVar->Kp1 * ExpVdg * BlendVdg + PfetVar->Kp4 * Temp1 * Vdg) *
	      AnaTanch(Vds * PfetVar->Kp3 / (PfetVar->V0 + Temp1));
      } else {
	Vsg = VPin2 - VPin1;
	ExpVsg = AnaExpo(PfetVar->Kp2 * Vsg);
	BlendVsg = 1 / (1 + PfetVar->Kp5 * ExpVsg);
	InvBlendVsg = 1 - BlendVsg;
	Temp1 = Vsg * InvBlendVsg;
	Ids = (1 - Vds * PfetVar->InvVcross) *
	      (PfetVar->Kp1 * ExpVsg * BlendVsg + Temp1 * PfetVar->Kp4 * Vsg) *
	      AnaTanch(Vds * PfetVar->Kp3 / (PfetVar->V0 + Temp1));
      }
    }
    switch (act->pin) {

    case 2:
      act->Iin -= Ids;
      break;

    case 3:
      act->Iin += Ids;
      break;
    }
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_PFET4*/

Local Void GetCnf_Pfet4(NewIC)
ICPfet *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICPfet *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("PFET4", Arg, &Found);
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
	  WITH->InitLeakage = 1.66e-17;
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

Local Void initlib_PFET4(act)
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
  NewIC = (ICPfet *)Malloc(sizeof(ICPfet));
  NewIC->InitRatio = 1.0;
  NewIC->InitVt = 0.9;
  NewIC->InitLeakage = 1.66e-17;
  NewIC->InitFitFactor = 3.33;
  NewIC->InitKtqn = 0.04;
  NewIC->InitCgs = 10e-15;
  NewIC->InitCgd = 10e-15;
  NewIC->InitCgb = 10e-15;
  NewIC->InitCdb = 10e-15;
  NewIC->InitCsb = 10e-15;
  NewIC->InitEarly = 50.0;
  GetCnf_Pfet4(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Pfet4_Initlib*/



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
      PfetVar->Kp1 = Inst->attr[N_Ratio - 1].UU.r * Inst->attr[N_Leakage - 1].UU.r;
      PfetVar->Kp4 = PfetVar->Kp1 *
		     AnaExpo(PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      PfetVar->TwoKp4 = 2 * PfetVar->Kp4;
    }
    break;

  case N_Vt:
    if (Inst->attr[N_Vt - 1].UU.r < -AnaVdd ||
	Inst->attr[N_Vt - 1].UU.r > AnaVdd || Inst->attr[N_Vt - 1].blnk ||
	Inst->attr[N_Vt - 1].UU.r == 0)
      Result = false;
    else {
      PfetVar->Kp4 = PfetVar->Kp1 *
		     AnaExpo(PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      PfetVar->TwoKp4 = 2 * PfetVar->Kp4;
      PfetVar->Kp5 = AnaExpo(-PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r);
    }
    break;

  case N_Leakage:
    if ((unsigned)Inst->attr[N_Leakage - 1].UU.r > 1 ||
	Inst->attr[N_Leakage - 1].blnk)
      Result = false;
    else {
      PfetVar->Kp1 = Inst->attr[N_Ratio - 1].UU.r * Inst->attr[N_Leakage - 1].UU.r;
      PfetVar->Kp4 = PfetVar->Kp1 *
		     AnaExpo(PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      PfetVar->TwoKp4 = 2 * PfetVar->Kp4;
    }
    break;

  case N_FitFactor:
    if ((unsigned)Inst->attr[N_FitFactor - 1].UU.r > 20 ||
	Inst->attr[N_FitFactor - 1].blnk)
      Result = false;
    else
      PfetVar->Kp3 = Inst->attr[N_FitFactor - 1].UU.r;
    break;

  case N_Ktqn:
    if ((unsigned)Inst->attr[N_Ktqn - 1].UU.r > 1 || Inst->attr[N_Ktqn - 1].blnk)
      Result = false;
    else {
      PfetVar->V0 = Inst->attr[N_Ktqn - 1].UU.r;
      PfetVar->Kp2 = 1 / PfetVar->V0;
      PfetVar->Kp4 = PfetVar->Kp1 *
		     AnaExpo(PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r) /
		     (Inst->attr[N_Vt - 1].UU.r * Inst->attr[N_Vt - 1].UU.r);
      PfetVar->TwoKp4 = 2 * PfetVar->Kp4;
      PfetVar->Kp5 = AnaExpo(-PfetVar->Kp2 * Inst->attr[N_Vt - 1].UU.r);
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


Local Void Attr_Pfet4(act)
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
    act->ok = Pfetcheck(act->inst, Attrnum);
}


Local Void Newgate_Pfet4(act)
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
  PfetVar->V0 = ICptr->InitKtqn;   /*kt/qn*/
  PfetVar->Kp1 = ICptr->InitRatio * ICptr->InitLeakage;
      /*Diode leakage current*(W/L)*/
  PfetVar->Kp2 = 1 / PfetVar->V0;   /*qn/kt*/
  PfetVar->Kp3 = ICptr->InitFitFactor;   /*Tanch(Kp3*x) ~=.99 when x=1*/
  PfetVar->Kp4 = PfetVar->Kp1 * AnaExpo(PfetVar->Kp2 * ICptr->InitVt) /
		 (ICptr->InitVt * ICptr->InitVt);
      /*Kp1*Expo(Kp2*Vtp)/(Vtp*Vtp)*/
  PfetVar->TwoKp4 = 2 * PfetVar->Kp4;   /*2*Kp4*/
  PfetVar->Kp5 = AnaExpo(-PfetVar->Kp2 * ICptr->InitVt);   /*Expo(-Kp2*Vtp)*/
  if (ICptr->InitEarly == 0)
    PfetVar->InvVcross = 0.0;
  else
    PfetVar->InvVcross = 1 / ICptr->InitEarly;
  PfetVar->TauCgs = 0.0;
  PfetVar->TauCgd = 0.0;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)PfetVar;
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

Local Void Copygate_Pfet4(act)
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

Local Void Dispose_Pfet4(act)
Analog_32_action *act;
{
  PfetConst *PfetVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  Free(PfetVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Pfet4(act)
Analog_32_action *act;
{
  PfetConst *PfetVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICPfet *ICptr;
  log_grec *WITH;

  AnaCapattrread((long)Cstart, act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  PfetVar = (PfetConst *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICPfet *)Cellptr->Ainfo;
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
  PfetVar->V0 = WITH->attr[N_Ktqn - 1].UU.r;   /*kt/qn*/
  PfetVar->Kp1 = WITH->attr[N_Ratio - 1].UU.r * WITH->attr[N_Leakage - 1].UU.r;
      /*Diode leakage current*(W/L)*/
  PfetVar->Kp2 = 1 / PfetVar->V0;   /*qn/kt*/
  PfetVar->Kp3 = WITH->attr[N_FitFactor - 1].UU.r;
      /*Tanch(Kp3*x) ~=.99 when x=1*/
  PfetVar->Kp4 = PfetVar->Kp1 * AnaExpo(PfetVar->Kp2 * WITH->attr[N_Vt - 1].UU.r) /
		 (WITH->attr[N_Vt - 1].UU.r * WITH->attr[N_Vt - 1].UU.r);
      /*Kp1*Expo(Kp2*Vtp)/(Vtp*Vtp)*/
  PfetVar->TwoKp4 = 2 * PfetVar->Kp4;   /*2*Kp4*/
  PfetVar->Kp5 = AnaExpo(-PfetVar->Kp2 * WITH->attr[N_Vt - 1].UU.r);
      /*Expo(-Kp2*Vtp)*/
  if (WITH->attr[N_Early - 1].blnk)
    PfetVar->InvVcross = 0.0;
  else
    PfetVar->InvVcross = 1 / WITH->attr[N_Early - 1].UU.r;
  PfetVar->TauCgs = 0.0;
  PfetVar->TauCgd = 0.0;
}

Local Void Probe_Pfet4(act)
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
    sprintf(STR1, "%s <undefined>", GateName);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    return;
  }
  d1 = Pin2 - Pin3;
  if (d1 >= 0) {
    d2 = Pin2 - Pin1;
    strcpy(Name2, " (source)");
    strcpy(Name3, " (drain)");
  } else {
    d1 = -d1;
    d2 = Pin3 - Pin1;
    strcpy(Name2, " (drain)");
    strcpy(Name3, " (source)");
  }
  switch (AnaLogglobals->probepin) {

  case 0:
    AnaScoreboard(GateName, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s (gate)", GateName);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR3, "%s%s", GateName, Name2);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;

  case 3:
    sprintf(STR3, "%s%s", GateName, Name3);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;
  }
  sprintf(STR3, "Vsd = %s", AnaProbeVoltStr(STR4, d1));
  AnaScoreboard(STR3, (long)AnaMessGate2);
  sprintf(STR3, "Vsg = %s", AnaProbeVoltStr(STR4, d2));
  AnaScoreboard(STR3, (long)AnaMessGate3);
}



/*==========================================================================*/
/*                 Library Listing For Cell PFET4                           */
/*==========================================================================*/


Void Log_pfet4_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_PFET4(act);
    break;

  case Analog_act_ex:
    Ex_PFET4(act);
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
    Attr_Pfet4(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Pfet4(act);
    break;

  case Analog_act_copygate:
    Copygate_Pfet4(act);
    break;

  case Analog_act_disposegate:
    Dispose_Pfet4(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Pfet4(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_PFET4(act);
    break;

  case Analog_act_probe:
    Probe_Pfet4(act);
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
#undef GateName





/* End. */
