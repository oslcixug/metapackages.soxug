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


#define NFET7F_G
#include "nfet7f.h"


#define Cstart          1
#define N_Wdrawn        13
#define N_Ldrawn        14
#define N_SArea         15
#define N_DArea         16
#define N_WArea         17
#define N_NaOffset      18
#define N_MuOffset      19
#define N_QssOffset     20
#define N_Vt            21
#define N_Kappa         22


#define Gatename        "NFET7F"


typedef struct NfetConst {
  double k1;  /* Vfb + phi_b */ 
  double k2;  /* 1/Cox */ 
  double k3;  /* 2*e_s*rho*1e10*phi_b */
  double k4;  /* 2*e_s*rho*1e10 */
  double k5;  /* phi_f */
  double k6;  /* 1.66667 */  
  double k7;  /* - Vfb */
  double k8;  /* 0.5*rho*e_s*1e10 */
  double k9;  /* (1/(2*Ut)) */
  double k10; /* 2*1e-6*beta*Ut^2 */
  double k11; /* Early_s/(Leff - L_0) */
  double k12; /* phi_f/10 */
  double k13; /* 1/(1 + k2*sqrt(k8/k12)) */
  double k14; /* (k2*k2*k4)/2 */
  double k15; /* (k2*k8*k5*k6)/2 */
  double k16; /* (1/k5)^2 */
  double k17; /* 2*k9 */
  double k18; /* Early_s */
  double k19; /* Leff - L_0 */

  double Cgd; /* computed capacitance */  
  double Cgs; /* computed capacitance */  
  double TauCgd;   /*Tau for capacitor*/
  double TauCgs;   /*Tau for capacitor*/
  double NewVt;
  double Newkappa;
} NfetConst;


typedef struct ICNfet {
  double InitWdrawn, InitLdrawn, InitSArea, InitDArea, InitWArea, InitNaOffset,
         InitMuOffset, InitQssOffset;
} ICNfet;



/****************************************************************************/
/*            Procedure To Update Internal Gate Constants                   */
/****************************************************************************/


Local boolean Nfetconsts(Inst)
log_grec *Inst;

{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double rho, phi_f, phi_b,  Leff, Weff, Vfb, beta, lambda, del_Na;
  double Wdrawn, Ldrawn;

  A_Gate = (Anainstlist *)Inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;

  Wdrawn  = (Inst->attr[N_Wdrawn - 1].UU.r*AnaGenFab.lambda)*1e6;
  Ldrawn  = (Inst->attr[N_Ldrawn - 1].UU.r*AnaGenFab.lambda)*1e6;
  Leff    = (Ldrawn - AnaNFab.deltaL*1e6);
  Weff    = (Wdrawn - AnaNFab.deltaW*1e6);
  del_Na  = Inst->attr[N_NaOffset - 1].UU.r*AnaNFab.del_Na0*
            (1 - Leff*AnaNFab.del_NaS);
  rho     = AnaPhysical.q*AnaNFab.Na*del_Na;
  phi_f   = AnaPhysical.Ut*log((AnaNFab.Na*del_Na)/AnaGenFab.ni);
  phi_b   = 2*phi_f;
  Vfb   = AnaGenFab.phi_ms - 
          (AnaNFab.Qss + Inst->attr[N_QssOffset - 1].UU.r)/AnaNFab.Cox;
  beta  = AnaNFab.mu*1e-4*(Inst->attr[N_MuOffset - 1].UU.r)*
          AnaNFab.Cox*(Weff/Leff);

  NfetVar->Cgs = Wdrawn*AnaNFab.linCgs;
  NfetVar->Cgd = Wdrawn*AnaNFab.linCgs;
  Inst->attr[2].UU.r = Wdrawn*Ldrawn*AnaNFab.aCgw;
  A_Gate->Pininfo[0].Cap = Inst->attr[2].UU.r;
  Inst->attr[2].blnk = false;
  Inst->attr[2].changed = true;
  Inst->attr[5].UU.r = (Inst->attr[N_DArea - 1].UU.r*1e6)*AnaNFab.aCactive;
  A_Gate->Pininfo[1].Cap = Inst->attr[5].UU.r;
  Inst->attr[5].blnk = false;
  Inst->attr[5].changed = true;
  Inst->attr[8].UU.r = (Inst->attr[N_SArea - 1].UU.r*1e6)*AnaNFab.aCactive;
  A_Gate->Pininfo[2].Cap = Inst->attr[8].UU.r;
  Inst->attr[8].blnk = false;
  Inst->attr[8].changed = true;
  Inst->attr[11].UU.r = (Inst->attr[N_WArea - 1].UU.r*1e6)*AnaNFab.aCwell;
  A_Gate->Pininfo[3].Cap = Inst->attr[11].UU.r;
  Inst->attr[11].blnk = false;
  Inst->attr[11].changed = true;

  NfetVar->k1 = Vfb + phi_b;
  NfetVar->k2 = 1/AnaNFab.Cox;
  NfetVar->k3 = 2*AnaPhysical.e_s*rho*1e6*phi_b;
  NfetVar->k4 = 2*AnaPhysical.e_s*rho*1e6;
  NfetVar->k5 = phi_f;
  NfetVar->k6 = 1.66667;
  NfetVar->k7 = - Vfb;
  NfetVar->k8 = 0.5*rho*AnaPhysical.e_s*1e6;
  NfetVar->k9 = (1/(2*AnaPhysical.Ut));
  NfetVar->k10= 2*beta*AnaPhysical.Ut*AnaPhysical.Ut;
  NfetVar->k11 = AnaNFab.Early_s/(Leff - AnaNFab.L_0*1e6);
  NfetVar->k12 = phi_f/10;
  NfetVar->k13 = 1/(1 + NfetVar->k2*AnaSqrt(NfetVar->k8/NfetVar->k12));
  NfetVar->k14 = (NfetVar->k2*NfetVar->k2*NfetVar->k4)/2;
  NfetVar->k15 = (NfetVar->k2*NfetVar->k8*NfetVar->k5*NfetVar->k6)/2;
  NfetVar->k16 = (1/(NfetVar->k5*NfetVar->k5));
  NfetVar->k17 = 2*NfetVar->k9;
  NfetVar->k18 = AnaNFab.Early_s;
  NfetVar->k19 = Leff - AnaNFab.L_0*1e6;

  NfetVar->NewVt = 0;
  NfetVar->Newkappa = 0;

  Inst->attr[N_Vt - 1].blnk = true;
  Inst->attr[N_Vt - 1].changed = true;
  Inst->attr[N_Kappa - 1].blnk = true;
  Inst->attr[N_Kappa - 1].changed = true;
}



/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/


Local Void Ex_NFET7F(act)
Analog_32_action *act;
{
  log_grec *WITH;
  AnaMatRec *WITH1;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double MinI2, dI2dV1, dI2dV2, dI2dV3, dI2dV4, MinI3, dI3dV1, dI3dV2,
         dI3dV3, dI3dV4, MinI1, dI1dV1, dI1dV2, dI1dV3, dI1dV4,
         MinI4, dI4dV1, dI4dV2, dI4dV3, dI4dV4;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr, *Pin4Ptr;
  long Pin1, Pin2, Pin3, Pin4;
  double VPin1, VPin2, VPin3, VPin4, VMem1, VMem2, VMem3, VMem4;   /*1*/
  double ICap;
  double Vg, Vd, Vs, Vb, Blend, InvBlend, Vnear, Vfar;
  double dVneardVs, dVneardVd, dVfardVs, dVfardVd, sign;
  double Vt, dVtdVd, dVtdVs, dVtdVg, dVtdVb;
  double phi_s, kappa, dkappadVg, dkappadVs, dkappadVd, dkappadVb;
  double Ifor, Irev, Idsi, Ids;
  double lambda, dlambdadVg, dlambdadVs, dlambdadVd, dlambdadVb;
  double dIfordVg, dIfordVs, dIfordVd, dIfordVb;
  double dIrevdVg, dIrevdVs, dIrevdVd, dIrevdVb;
  double dIdsdVg, dIdsdVs, dIdsdVd, dIdsdVb;
  double dIdsidVg, dIdsidVs, dIdsidVd, dIdsidVb;
  double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
  double tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19;
  double tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;

  WITH = act->inst;
  if (AnaChangedP)
    {
      Nfetconsts(WITH);
    }
  A_Gate = (Anainstlist *)WITH->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
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
  VPin4 = Pin4Ptr->last;   /*Vpin3 is the well*/
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*Memories for capacitors*/
  VMem2 = A_Gate->Pininfo[1].Vmem;   /*...*/
  VMem3 = A_Gate->Pininfo[2].Vmem;   /*...*/
  VMem4 = A_Gate->Pininfo[3].Vmem;   /*...*/
  Vg = VPin1;
  Vd = VPin2;
  Vs = VPin3;
  Vb = VPin4;

  if (Vd > Vs)
    {
      Blend = 1;
      InvBlend = 0;
      Vnear = Vs;
      Vfar = Vd;
      dVneardVs = 1;
      dVneardVd = 0;
      dVfardVs = 0;
      dVfardVd = 1;
      sign = 1;
    }
  else
    {
      Blend = 0;
      InvBlend = 1;
      Vnear = Vd;
      Vfar = Vs;
      dVneardVs = 0;
      dVneardVd = 1;
      dVfardVs = 1;
      dVfardVd = 0;
      sign = -1;
    }
  if ((NfetVar->k3 + NfetVar->k4*(Vnear - Vb)) > 0)
    {
      Vt = NfetVar->k1 + NfetVar->k2*AnaSqrt(NfetVar->k3 + 
           NfetVar->k4*(Vnear-Vb));
      tmp1 = NfetVar->k14*(1/(Vt - NfetVar->k1));
      dVtdVd =  tmp1*dVneardVd;
      dVtdVs =  tmp1*dVneardVs;
      dVtdVb = -tmp1;
    }
  else
    {
      Vt = NfetVar->k1;
      dVtdVd = 0;
      dVtdVs = 0;
      dVtdVb = 0;
    }
  dVtdVg = 0;
  phi_s = NfetVar->k5*tanh((NfetVar->k6/Vt)*(Vg - Vb + NfetVar->k7));
  if (phi_s > NfetVar->k12)
    {
      tmp0 = 1/phi_s;
      tmp2 = AnaSqrt(tmp0*NfetVar->k8);
      tmp3 = 1/Vt;
      kappa = 1/(1 + NfetVar->k2*tmp2);
      tmp4 = NfetVar->k15*kappa*kappa*(1/tmp2)*
	     (tmp0*tmp0 - NfetVar->k16)*tmp3;
      tmp5 = -tmp4*tmp3*(Vg - Vb + NfetVar->k7);
      dkappadVg = tmp4;
      dkappadVs = tmp5*dVtdVs;
      dkappadVd = tmp5*dVtdVd;
      dkappadVb = tmp5*dVtdVb - tmp4;
    }
  else
    {
      kappa = NfetVar->k13;
      dkappadVg = 0;
      dkappadVs = 0;
      dkappadVd = 0;
      dkappadVb = 0;
    }
  tmp6 = (Vg - Vnear - Vt);
  tmp7 = kappa*tmp6;
  tmp9 = AnaExpo(NfetVar->k9*tmp7);
  tmp10 = 1 + tmp9;
  tmp11 = log(tmp10);
  tmp12 = AnaExpo(NfetVar->k9*(tmp7 + Vnear - Vfar));
  tmp13 = 1 + tmp12;
  tmp14 = log(tmp13);
  tmp15 = NfetVar->k17*tmp11*(1/tmp10)*tmp9;
  tmp16 = NfetVar->k17*tmp14*(1/tmp13)*tmp12;
  Ifor = tmp11*tmp11;
  Irev = tmp14*tmp14;
  dIfordVg = tmp15*(dkappadVg*tmp6 + kappa);
  dIfordVs = tmp15*(dkappadVs*tmp6 - kappa*(dVneardVs + dVtdVs));
  dIfordVd = tmp15*(dkappadVd*tmp6 - kappa*(dVneardVd + dVtdVd));
  dIfordVb = tmp15*(dkappadVb*tmp6 - kappa*dVtdVb);
  dIrevdVg = tmp16*(dkappadVg*tmp6 + kappa);
  dIrevdVs = tmp16*(dkappadVs*tmp6 - kappa*(dVneardVs + dVtdVs) 
             + dVneardVs - dVfardVs);
  dIrevdVd = tmp16*(dkappadVd*tmp6 - kappa*(dVneardVd + dVtdVd) 
             + dVneardVd -dVfardVd );
  dIrevdVb= tmp16*(dkappadVb*tmp6 - kappa*dVtdVb);

  tmp17 = (1/kappa);
  tmp18 = NfetVar->k10*sign*tmp17;
  tmp19 = (Ifor - Irev);
  tmp20 = tmp17*tmp19;
  Idsi = tmp18*tmp19;
  dIdsidVg = tmp18*(dIfordVg - dIrevdVg  - tmp20*dkappadVg);
  dIdsidVs = tmp18*(dIfordVs - dIrevdVs  - tmp20*dkappadVs);
  dIdsidVd = tmp18*(dIfordVd - dIrevdVd  - tmp20*dkappadVd);
  dIdsidVb = tmp18*(dIfordVb - dIrevdVb  - tmp20*dkappadVb);

  if (Vg > Vnear)
    {
      tmp21 = 1/(Vg - Vnear + NfetVar->k19);
      tmp22 = NfetVar->k18*tmp21;
      tmp23 = tmp22*tmp21;
      lambda = tmp22;
      dlambdadVg = -tmp23;
      dlambdadVs = tmp23*dVneardVs;
      dlambdadVd = tmp23*dVneardVd;
    }
  else
    {
      lambda = NfetVar->k11;
      dlambdadVg = 0;
      dlambdadVs = 0;
      dlambdadVd = 0;
    }

  tmp24 = (Vfar - Vb);
  tmp25 = (1 + lambda*tmp24);
  Ids = Idsi*tmp25;
  dIdsdVg = dIdsidVg*tmp25 + Idsi*dlambdadVg*tmp24; 
  dIdsdVs = dIdsidVs*tmp25 + Idsi*(dlambdadVs*tmp24 + lambda*dVfardVs);
  dIdsdVd = dIdsidVd*tmp25 + Idsi*(dlambdadVd*tmp24 + lambda*dVfardVd);
  dIdsdVb = dIdsidVb*tmp25 - Idsi*lambda;

  NfetVar->NewVt = Vt;
  NfetVar->Newkappa = kappa;

  MinI3 = Ids;
  MinI2 = -Ids;
  dI2dV1 = dIdsdVg;
  dI3dV1 = -dIdsdVg;
  dI2dV3 = dIdsdVs;
  dI3dV3 = -dIdsdVs;
  dI2dV2 = dIdsdVd;
  dI3dV2 = -dIdsdVd;
  dI2dV4 = dIdsdVb;
  dI3dV4 = -dIdsdVb;
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

  if (AnaChangedDt) /* might also need to catch changes in Cgd */
    NfetVar->TauCgd = NfetVar->Cgd/ AnaDt;
  ICap = NfetVar->TauCgd * (VMem1 - VMem2 - Vg + Vd);   
  MinI2 -= ICap;
  dI2dV1 -= NfetVar->TauCgd;
  dI2dV2 += NfetVar->TauCgd;
  MinI1 += ICap;
  dI1dV1 += NfetVar->TauCgd;
  dI1dV2 -= NfetVar->TauCgd;

  if (AnaChangedDt) /* might also need to catch changes in Cgs */
      NfetVar->TauCgs = NfetVar->Cgs/ AnaDt;
  ICap = NfetVar->TauCgs * (VMem1 - VMem3 - Vg + Vs);
  MinI3 -= ICap;
  dI3dV1 -= NfetVar->TauCgs;
  dI3dV3 += NfetVar->TauCgs;
  MinI1 += ICap;
  dI1dV1 += NfetVar->TauCgs;
  dI1dV3 -= NfetVar->TauCgs;

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
}  /*Ex_NFET7F*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/

Local Void Iin_NFET7F(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr, *Pin4Ptr;
  double VPin1, VPin2, VPin3, VPin4, VMem1, VMem2, VMem3, VMem4;
  NfetConst *NfetVar;
  Anainstlist *A_Gate;
  double Vg, Vd, Vs, Vb, Blend, InvBlend, Vnear, Vfar;
  double sign;
  double Vt;
  double phi_s, kappa, lambda;
  double Ifor, Irev, Idsi, Ids;
  double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
  double tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19;
  double tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;
  log_grec *WITH1;

  switch (act->pin) {

  case 1:
    WITH1 = act->inst;
    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    Pin4Ptr = (AnaExt_rec *)WITH1->pin[3]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    NfetVar = (NfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VMem2 = A_Gate->Pininfo[1].Vmem;
    VMem3 = A_Gate->Pininfo[2].Vmem;
    VMem4 = A_Gate->Pininfo[3].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    VPin4 = Pin4Ptr->now;
    act->Iin = NfetVar->TauCgd * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    act->Iin = NfetVar->TauCgs * (VPin1 - VPin3 - VMem1 + VMem3) + act->Iin;
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
    NfetVar = (NfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    VPin4 = Pin4Ptr->now;
    switch (act->pin) {

    case 2:
      {
	VMem2 = A_Gate->Pininfo[1].Vmem;
	act->Iin = NfetVar->TauCgd * (VPin2 - VPin1 - VMem2 + VMem1) 
	           + act->Iin;
      }
      break;

    case 3:
      {
	VMem3 = A_Gate->Pininfo[2].Vmem;
	act->Iin = NfetVar->TauCgs * (VPin3 - VPin1 - VMem3 + VMem1) 
	           + act->Iin;
      }
      break;
    }
    Vg = VPin1;
    Vd = VPin2;
    Vs = VPin3;
    Vb = VPin4;

    if (Vd > Vs)
      {
	Blend = 1;
	InvBlend = 0;
	Vnear = Vs;
	Vfar = Vd;
	sign = 1;
      }
    else
      {
	Blend = 0;
	InvBlend = 1;
	Vnear = Vd;
	Vfar = Vs;
	sign = -1;
      }
    if ((NfetVar->k3 + NfetVar->k4*(Vnear-Vb)) > 0)
      {
	Vt = NfetVar->k1 + NfetVar->k2*AnaSqrt(NfetVar->k3 
	     + NfetVar->k4*(Vnear-Vb));
      }
    else
      {
	Vt = NfetVar->k1;
      }
    phi_s = NfetVar->k5*tanh((NfetVar->k6/Vt)*(Vg - Vb + NfetVar->k7));
    
    if (phi_s > NfetVar->k12)
      {
	tmp0 = 1/phi_s;
	tmp2 = AnaSqrt(tmp0*NfetVar->k8);
	tmp3 = 1/Vt;
	kappa = 1/(1 + NfetVar->k2*tmp2);
      }
    else
      {
	kappa = NfetVar->k13;
      }
    tmp6 = (Vg - Vnear - Vt);
    tmp7 = kappa*tmp6;
    tmp9 = AnaExpo(NfetVar->k9*tmp7);
    tmp10 = 1 + tmp9;
    tmp11 = log(tmp10);
    tmp12 = AnaExpo(NfetVar->k9*(tmp7 + Vnear - Vfar));
    tmp13 = 1 + tmp12;
    tmp14 = log(tmp13);
    Ifor = tmp11*tmp11;
    Irev = tmp14*tmp14;
    Idsi = NfetVar->k10*sign*(1/kappa)*(Ifor - Irev);
    if (Vg > Vnear)
      {
	lambda =  NfetVar->k18*(1/(Vg - Vnear + NfetVar->k19));
      }
    else
      {
	lambda = NfetVar->k11;
      }
    Ids = Idsi*(1 + lambda*(Vfar - Vb));

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

  case 4:
    WITH1 = act->inst;
    Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
    Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
    Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
    Pin4Ptr = (AnaExt_rec *)WITH1->pin[3]->info;
    A_Gate = (Anainstlist *)WITH1->info;
    NfetVar = (NfetConst *)A_Gate->InstVar;
    VMem1 = A_Gate->Pininfo[0].Vmem;
    VMem2 = A_Gate->Pininfo[1].Vmem;
    VMem3 = A_Gate->Pininfo[2].Vmem;
    VMem4 = A_Gate->Pininfo[3].Vmem;
    VPin1 = Pin1Ptr->now;
    VPin2 = Pin2Ptr->now;
    VPin3 = Pin3Ptr->now;
    VPin4 = Pin4Ptr->now;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_NFET7F*/


Local Void GetCnf_Nfet7F(NewIC)
ICNfet *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICNfet *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("NFET7F", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "WDRAWN")) {
	TRY(try1);
	  WITH->InitWdrawn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->InitWdrawn = 28;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "LDRAWN")) {
	TRY(try2);
	  WITH->InitLdrawn = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->InitLdrawn = 14;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "SAREA")) {
	TRY(try3);
	  WITH->InitSArea = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->InitSArea = 36e-6;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "DAREA")) {
	TRY(try4);
	  WITH->InitDArea = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->InitDArea = 36e-6;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "WAREA")) {
	TRY(try5);
	  WITH->InitWArea = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitWArea = 100e-6;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "NAOFFSET")) {
	TRY(try6);
	  WITH->InitNaOffset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try6);
	  WITH->InitNaOffset = 1.0;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "MUOFFSET")) {
	TRY(try5);
	  WITH->InitMuOffset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitMuOffset = 1;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "QSSOFFSET")) {
	TRY(try5);
	  WITH->InitQssOffset = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->InitQssOffset = 0;
	ENDTRY(try5);
      }

    }
  } while (Found);
}

/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_NFET7F(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICNfet *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anasimple;
  NewCell->plist_cell = (Anapinrec *)
                        Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewCell->plist_cell[1].standalone = false;
  NewCell->plist_cell[1].active = true;
  NewCell->plist_cell[2].standalone = false;
  NewCell->plist_cell[2].active = true;
  NewCell->plist_cell[3].standalone = false;
  NewCell->plist_cell[3].active = true;
  NewIC = (ICNfet *)Malloc(sizeof(ICNfet));
  NewIC->InitWdrawn = 28;
  NewIC->InitLdrawn = 14;
  NewIC->InitSArea = 36e-6;
  NewIC->InitDArea = 36e-6;
  NewIC->InitWArea = 100e-6;
  NewIC->InitNaOffset = 1.0;
  NewIC->InitMuOffset = 1.0;
  NewIC->InitQssOffset = 0.0;
  GetCnf_Nfet7F(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*NFET7F_Initlib*/



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

  case N_Wdrawn:
    if (Inst->attr[N_Wdrawn - 1].UU.r < 0.01 || 
        Inst->attr[N_Wdrawn - 1].blnk ||
	Inst->attr[N_Wdrawn - 1].UU.r > 1e6)
      Result = false;
    else 
	Nfetconsts(Inst, Attrnum);
    break;

  case N_Ldrawn:
    if (Inst->attr[N_Ldrawn - 1].UU.r < 0.01 || 
        Inst->attr[N_Ldrawn - 1].blnk ||
	Inst->attr[N_Ldrawn - 1].UU.r > 1e6)
      Result = false;
    else
	Nfetconsts(Inst);
    break;

  case N_SArea:
    if (Inst->attr[N_SArea - 1].UU.r < 1e-12 || 
        Inst->attr[N_SArea - 1].blnk ||
	Inst->attr[N_SArea - 1].UU.r > 1e6)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_DArea:
    if (Inst->attr[N_DArea - 1].UU.r < 1e-12 || 
        Inst->attr[N_DArea - 1].blnk ||
	Inst->attr[N_DArea - 1].UU.r > 1e6)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_WArea:
    if (Inst->attr[N_WArea - 1].UU.r < 1e-12 || 
        Inst->attr[N_WArea - 1].blnk ||
	Inst->attr[N_WArea - 1].UU.r > 1e6)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_NaOffset:
    if (Inst->attr[N_NaOffset - 1].UU.r < 1e-10 || 
        Inst->attr[N_NaOffset - 1].blnk ||
	Inst->attr[N_NaOffset - 1].UU.r > 1e6)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_MuOffset:
    if (Inst->attr[N_MuOffset - 1].UU.r < 1e-10 || 
        Inst->attr[N_MuOffset - 1].blnk ||
	Inst->attr[N_MuOffset - 1].UU.r > 1e6)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_QssOffset:
    if (Inst->attr[N_QssOffset - 1].UU.r < -1000 || 
        Inst->attr[N_QssOffset - 1].blnk ||
	Inst->attr[N_QssOffset - 1].UU.r > 1000)
      Result = false;
    else 
      Nfetconsts(Inst, Attrnum);
    break;

  case N_Vt:
    Result = false;
    break;

  case N_Kappa:
    Result = false;
    break;


  }

  return Result;
}


Local Void Attr_Nfet7F(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;

  if (Attrnum >= Cstart && Attrnum <= Cstart + 11) {
    if ((Attrnum == 3) || (Attrnum == 6) || (Attrnum == 9) || (Attrnum == 12))
      {
	/* capacitances computed for this gate */
	act->ok = false;
      }
    else
      {
	AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
      }
    return;
  }
  act->ok = Nfetcheck(act->inst, Attrnum);
}


Local Void Newgate_Nfet7F(act)
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
  NfetVar->TauCgd = 0.0;
  NfetVar->TauCgs = 0.0;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NfetVar;
  AnaCapInit(act->inst);   /*6*/
  WITH = act->inst;
  WITH->attr[N_Wdrawn - 1].UU.r = ICptr->InitWdrawn;   /**/
  WITH->attr[N_Wdrawn - 1].blnk = false;
  WITH->attr[N_Wdrawn - 1].changed = true;
  WITH->attr[N_Ldrawn - 1].UU.r = ICptr->InitLdrawn;   /**/
  WITH->attr[N_Ldrawn - 1].blnk = false;
  WITH->attr[N_Ldrawn - 1].changed = true;
  WITH->attr[N_SArea  - 1].UU.r = ICptr->InitSArea;   /**/
  WITH->attr[N_SArea  - 1].blnk = false;
  WITH->attr[N_SArea  - 1].changed = true;
  WITH->attr[N_DArea  - 1].UU.r = ICptr->InitDArea;   /**/
  WITH->attr[N_DArea  - 1].blnk = false;
  WITH->attr[N_DArea  - 1].changed = true;
  WITH->attr[N_WArea  - 1].UU.r = ICptr->InitWArea;   /**/
  WITH->attr[N_WArea  - 1].blnk = false;
  WITH->attr[N_WArea  - 1].changed = true;
  WITH->attr[N_NaOffset - 1].UU.r = ICptr->InitNaOffset;   /**/
  WITH->attr[N_NaOffset - 1].blnk = false;
  WITH->attr[N_NaOffset - 1].changed = true;
  WITH->attr[N_MuOffset - 1].UU.r = ICptr->InitMuOffset;   /**/
  WITH->attr[N_MuOffset - 1].blnk = false;
  WITH->attr[N_MuOffset - 1].changed = true;
  WITH->attr[N_QssOffset - 1].UU.r = ICptr->InitQssOffset;   /**/
  WITH->attr[N_QssOffset - 1].blnk = false;
  WITH->attr[N_QssOffset - 1].changed = true;
  WITH->attr[N_Vt - 1].UU.r = 0;   /**/
  WITH->attr[N_Vt - 1].blnk = true;
  WITH->attr[N_Vt - 1].changed = true;
  WITH->attr[N_Kappa - 1].UU.r = 0;   /**/
  WITH->attr[N_Kappa - 1].blnk = true;
  WITH->attr[N_Kappa - 1].changed = true;
  Nfetconsts(WITH);
}

Local Void Copygate_Nfet7F(act)
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

Local Void Dispose_Nfet7F(act)
Analog_32_action *act;
{
  NfetConst *NfetVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  Free(NfetVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Nfet7F(act)
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
    WITH = act->inst;
    WITH->attr[N_Wdrawn - 1].UU.r = ICptr->InitWdrawn;   /**/
    WITH->attr[N_Wdrawn - 1].blnk = false;
    WITH->attr[N_Wdrawn - 1].changed = true;
    WITH->attr[N_Ldrawn - 1].UU.r = ICptr->InitLdrawn;   /**/
    WITH->attr[N_Ldrawn - 1].blnk = false;
    WITH->attr[N_Ldrawn - 1].changed = true;
    WITH->attr[N_SArea  - 1].UU.r = ICptr->InitSArea;   /**/
    WITH->attr[N_SArea  - 1].blnk = false;
    WITH->attr[N_SArea  - 1].changed = true;
    WITH->attr[N_DArea  - 1].UU.r = ICptr->InitDArea;   /**/
    WITH->attr[N_DArea  - 1].blnk = false;
    WITH->attr[N_DArea  - 1].changed = true;
    WITH->attr[N_WArea  - 1].UU.r = ICptr->InitWArea;   /**/
    WITH->attr[N_WArea  - 1].blnk = false;
    WITH->attr[N_WArea  - 1].changed = true;
    WITH->attr[N_NaOffset - 1].UU.r = ICptr->InitNaOffset;   /**/
    WITH->attr[N_NaOffset - 1].blnk = false;
    WITH->attr[N_NaOffset - 1].changed = true;
    WITH->attr[N_MuOffset - 1].UU.r = ICptr->InitMuOffset;   /**/
    WITH->attr[N_MuOffset - 1].blnk = false;
    WITH->attr[N_MuOffset - 1].changed = true;
    WITH->attr[N_QssOffset - 1].UU.r = ICptr->InitQssOffset;   /**/
    WITH->attr[N_QssOffset - 1].blnk = false;
    WITH->attr[N_QssOffset - 1].changed = true;
    WITH->attr[N_Vt - 1].UU.r = 0;   /**/
    WITH->attr[N_Vt - 1].blnk = true;
    WITH->attr[N_Vt - 1].changed = true;
    WITH->attr[N_Kappa - 1].UU.r = 0;   /**/
    WITH->attr[N_Kappa - 1].blnk = true;
    WITH->attr[N_Kappa - 1].changed = true;
  }
  WITH = act->inst;
  Nfetconsts(WITH);
  NfetVar->TauCgd = 0.0;
  NfetVar->TauCgs = 0.0;
}

Local Void Probe_Nfet7F(act)
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
      Node1 == AnaNotyet || Node2 == AnaNotyet ||
      Node3 == AnaNotyet || Node4 == AnaNotyet) {
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

  case 4:
    sprintf(STR2, "%s (well)", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  }
  sprintf(STR3, "Vds = %s", AnaProbeVoltStr(STR4, d1));
  AnaScoreboard(STR3, (long)AnaMessGate2);
  sprintf(STR3, "Vgs = %s", AnaProbeVoltStr(STR4, d2));
  AnaScoreboard(STR3, (long)AnaMessGate3);
}


Local Void Update_Nfet7F(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  NfetConst *NfetVar;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  NfetVar = (NfetConst *)A_Gate->InstVar;
  act->inst->attr[N_Vt - 1].blnk = false;
  act->inst->attr[N_Vt - 1].changed = true;
  act->inst->attr[N_Vt - 1].UU.r = NfetVar->NewVt;
  act->inst->attr[N_Kappa - 1].blnk = false;
  act->inst->attr[N_Kappa - 1].changed = true;
  act->inst->attr[N_Kappa - 1].UU.r = NfetVar->Newkappa;

}



/*==========================================================================*/
/*                 Library Listing For   Cell NFET7F                    */
/*==========================================================================*/


Void Log_NFET7F_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_NFET7F(act);
    break;

  case Analog_act_ex:
    Ex_NFET7F(act);
    break;

  case Analog_act_update:   /*8*/
    Update_Nfet7F(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_Nfet7F(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_Nfet7F(act);
    break;

  case Analog_act_copygate:
    Copygate_Nfet7F(act);
    break;

  case Analog_act_disposegate:
    Dispose_Nfet7F(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Nfet7F(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_NFET7F(act);
    break;

  case Analog_act_probe:
    Probe_Nfet7F(act);
    break;
  }
}

#undef Cstart          
#undef N_Wdrawn             
#undef N_Ldrawn             
#undef N_SArea         
#undef N_DArea         
#undef N_NaOffset      
#undef N_MuOffset      
#undef N_QssOffset      
#undef N_Vt            
#undef N_Kappa         
#undef Gatename        









/* End. */
