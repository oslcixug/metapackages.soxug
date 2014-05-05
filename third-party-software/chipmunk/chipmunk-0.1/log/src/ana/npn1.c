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


#define NPN1_G
#include "npn1.h"


#define Cstart          1
#define N_Cbc           10
#define N_Cbe           11
#define N_betaF         12
#define N_betaR         13
#define N_Is            14
#define N_Ktq           15
#define N_Vcrow         16

#define Gatename        "NPN1"


typedef struct NPN1Const {
  double betaF, betaR;   /*Transistor Parameters*/
  double For1, For2, R1, R2, Is, IECO, ICEO, lam;
  boolean Limit;   /*Booleans for indicator light*/
  boolean OldLimit, NewLimit;
  double TauCbc;   /*Taus for capacitors*/
  double TauCbe;   /*Tau for capacitor*/
} NPN1Const;


typedef struct ICNPN1 {
  double ICbetaF, ICbetaR, ICIs, ICKtq, ICVcrow, InitCbc, InitCbe, InitCgb,
	 InitCdb, InitCsb;
} ICNPN1;


/****************************************************************************/
/*            Procedure To Light 'Limiting' Light                           */
/****************************************************************************/


Local Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
boolean Limitflag, Erase;
{
  short LLx, LLy, URx, URy;

  (*AnaLogglobals->hook.hidecursor)();
  LLx = 1;
  URx = 3;
  LLy = -5;
  URy = 5;
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
/*            Procedure To Simulate Cell In Algorithm                       */
/*                                                                          */
/*   Model used:                                                            */
/*                                                                          */
/*       Base = VPin1, Emitter = VPin3 Collector = VPin2                    */
/*                                                                          */
/*  IBase: ICEO*(Exp((Vbase-Vcoll)/Vo)-1)+IECO*(Exp((Vbase-Vemit)/Vo)-1)    */
/*  IColl:  betaF*IECO*(Exp((Vbase-Vemit)/Vo)-1)                            */
/*         -(1+betaR)*ICEO*(Exp((Vbase-Vcoll)/Vo)-1)                        */
/*  IEmit: -(IBase + IColl)                                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/



Local Void Ex_NPN1(act)
Analog_32_action *act;
{
  double MinI1, MinI2, MinI3, dI1dV1, dI1dV2, dI1dV3, dI2dV1, dI2dV2, dI2dV3,
	 dI3dV1, dI3dV2, dI3dV3;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  long Pin1, Pin2, Pin3;
  double VPin1, VPin2, VPin3, VMem1, VMem2, VMem3;
  NPN1Const *Npn1Var;
  Anainstlist *A_Gate;
  double Vbe, Vbc, Icap, ExpVbe, For2ExpVbe, ICEOExpVbe, ICEOExpVbeMinOne,
	 FBlend, InvFBlend, For1Vbe, lamFBlend, Ifor, dFBlenddS, dIfdC, dIfdB,
	 dIfdE, ExpVbc, For2ExpVbc, IECOExpVbc, IECOExpVbcMinOne, RBlend,
	 InvRBlend, R1Vbc, lamRBlend, Irev, dRBlenddS, dIrdE, dIrdB, dIrdC,
	 Ib, dIbdE, dIbdB, dIbdC, Ic, dIcdE, dIcdC, dIcdB, Ie, dIedE, dIedB,
	 dIedC;
  log_grec *WITH;
  AnaMatRec *WITH1;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  Npn1Var = (NPN1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  Pin1 = Pin1Ptr->nodenum;
  Pin2 = Pin2Ptr->nodenum;
  Pin3 = Pin3Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  VPin2 = Pin2Ptr->last;
  VPin3 = Pin3Ptr->last;
  VMem1 = A_Gate->Pininfo[0].Vmem;   /*Memories for capacitors*/
  VMem2 = A_Gate->Pininfo[1].Vmem;   /*...*/
  VMem3 = A_Gate->Pininfo[2].Vmem;   /*...*/
  /*Use with statement to get to internal variables*/

  /*Vbase -- VPin1, Vcoll -- VPin2, Vemit -- VPin3*/

  Vbe = VPin1 - VPin3;
  Vbc = VPin1 - VPin2;

  ExpVbe = AnaExpo(Npn1Var->lam * Vbe);
  For2ExpVbe = Npn1Var->For2 * ExpVbe;
  ICEOExpVbe = Npn1Var->ICEO * ExpVbe;
  ICEOExpVbeMinOne = ICEOExpVbe - Npn1Var->ICEO;
  FBlend = 1 / (1 + For2ExpVbe);
  InvFBlend = 1 - FBlend;

  For1Vbe = Npn1Var->For1 * Vbe;
  lamFBlend = Npn1Var->lam * FBlend;

  Ifor = ICEOExpVbeMinOne * FBlend + InvFBlend * For1Vbe;

  dFBlenddS = lamFBlend * For2ExpVbe * FBlend;

  dIfdC = 0.0;
  dIfdB = ICEOExpVbe * lamFBlend - ICEOExpVbeMinOne * dFBlenddS +
	  Npn1Var->For1 * InvFBlend + For1Vbe * dFBlenddS;
  dIfdE = -dIfdB;


  ExpVbc = AnaExpo(Npn1Var->lam * Vbc);
  For2ExpVbc = Npn1Var->For2 * ExpVbc;
  IECOExpVbc = Npn1Var->IECO * ExpVbc;
  IECOExpVbcMinOne = IECOExpVbc - Npn1Var->IECO;
  RBlend = 1 / (1 + For2ExpVbc);
  InvRBlend = 1 - RBlend;

  R1Vbc = Npn1Var->R1 * Vbc;
  lamRBlend = Npn1Var->lam * RBlend;

  Irev = IECOExpVbcMinOne * RBlend + InvRBlend * R1Vbc;

  dRBlenddS = lamRBlend * For2ExpVbc * RBlend;

  dIrdE = 0.0;
  dIrdB = IECOExpVbc * lamRBlend - IECOExpVbcMinOne * dRBlenddS +
	  Npn1Var->R1 * InvRBlend + R1Vbc * dRBlenddS;
  dIrdC = -dIrdB;


  Ib = Irev + Ifor;
  dIbdE = dIfdE;
  dIbdB = dIrdB + dIfdB;
  dIbdC = dIrdC;

  Ic = Npn1Var->betaF * Ifor - (1 + Npn1Var->betaR) * Irev;
  dIcdE = Npn1Var->betaF * dIfdE;
  dIcdC = -(1 + Npn1Var->betaR) * dIrdC;
  dIcdB = Npn1Var->betaF * dIfdB - (1 + Npn1Var->betaR) * dIrdB;

  Ie = Ib + Ic;
  dIedE = dIbdE + dIcdE;
  dIedB = dIbdB + dIcdB;
  dIedC = dIbdC + dIcdC;

  MinI1 = -Ib;
  MinI2 = -Ic;
  MinI3 = Ie;
  dI1dV1 = dIbdB;
  dI1dV2 = dIbdC;
  dI1dV3 = dIbdE;
  dI2dV1 = dIcdB;
  dI2dV2 = dIcdC;
  dI2dV3 = dIcdE;
  dI3dV1 = -dIedB;
  dI3dV2 = -dIedC;
  dI3dV3 = -dIedE;
  if (WITH->attr[N_Cbc - 1].UU.r != 0) {   /*C base-collector*/
    if (AnaChangedDt)
      Npn1Var->TauCbc = WITH->attr[N_Cbc - 1].UU.r / AnaDt;
    Icap = Npn1Var->TauCbc * (VMem1 - VMem2 - Vbc);   /*4*/
    MinI2 -= Icap;
    dI2dV1 -= Npn1Var->TauCbc;
    dI2dV2 += Npn1Var->TauCbc;
    MinI1 += Icap;
    dI1dV1 += Npn1Var->TauCbc;
    dI1dV2 -= Npn1Var->TauCbc;
  }
  /*3*/
  if (WITH->attr[N_Cbe - 1].UU.r != 0) {   /*C base-emitter*/
    if (AnaChangedDt)
      Npn1Var->TauCbe = WITH->attr[N_Cbe - 1].UU.r / AnaDt;
    Icap = Npn1Var->TauCbe * (VMem1 - VMem3 - Vbe);
    MinI3 -= Icap;
    dI3dV1 -= Npn1Var->TauCbe;
    dI3dV3 += Npn1Var->TauCbe;
    MinI1 += Icap;
    dI1dV1 += Npn1Var->TauCbe;
    dI1dV3 -= Npn1Var->TauCbe;
  }
  WITH1 = AnaSystem;
  if (Pin1 != 0) {
    WITH1->Mat[Pin1][AnaSysCol] += MinI1;
    WITH1->Mat[Pin1][Pin1] += dI1dV1;
    WITH1->Mat[Pin1][Pin2] += dI1dV2;
    WITH1->Mat[Pin1][Pin3] += dI1dV3;
    WITH1->MatNZ[Pin1][Pin1] = true;
    WITH1->MatNZ[Pin1][Pin2] = true;
    WITH1->MatNZ[Pin1][Pin3] = true;
    WITH1->MatNZ[Pin1][AnaSysCol] = true;
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
    WITH1->Mat[Pin3][Pin1] += dI3dV1;
    WITH1->Mat[Pin3][Pin2] += dI3dV2;
    WITH1->Mat[Pin3][Pin3] += dI3dV3;
    WITH1->Mat[Pin3][AnaSysCol] += MinI3;
    WITH1->MatNZ[Pin3][AnaSysCol] = true;
    WITH1->MatNZ[Pin3][Pin1] = true;
    WITH1->MatNZ[Pin3][Pin2] = true;
    WITH1->MatNZ[Pin3][Pin3] = true;
  }
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_NPN1*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_NPN1(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  AnaExt_rec *Pin3Ptr;
  double VPin3, VMem3;
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;
  double Vbe, Vbc, Ib, Ic, Ie, ExpVbe, FBlend, Ifor, ExpVbc, RBlend, Irev;
  log_grec *WITH;


  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin3Ptr = (AnaExt_rec *)WITH->pin[2]->info;
  VPin1 = Pin1Ptr->now;
  VPin2 = Pin2Ptr->now;
  VPin3 = Pin3Ptr->now;
  VMem1 = A_Gate->Pininfo[0].Vmem;
  VMem2 = A_Gate->Pininfo[1].Vmem;
  VMem3 = A_Gate->Pininfo[2].Vmem;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    if (WITH->attr[N_Cbc - 1].UU.r != 0)   /*C base-collector*/
      act->Iin = NPN1Var->TauCbc * (VPin1 - VPin2 - VMem1 + VMem2) + act->Iin;
    if (WITH->attr[N_Cbe - 1].UU.r != 0)   /*C base-emitter*/
      act->Iin = NPN1Var->TauCbe * (VPin1 - VPin3 - VMem1 + VMem3) + act->Iin;

    Vbe = VPin1 - VPin3;
    Vbc = VPin1 - VPin2;

    ExpVbe = AnaExpo(NPN1Var->lam * Vbe);
    FBlend = 1 / (1 + NPN1Var->For2 * ExpVbe);
    Ifor = (NPN1Var->ICEO * ExpVbe - NPN1Var->ICEO) * FBlend +
	   (1 - FBlend) * NPN1Var->For1 * Vbe;

    ExpVbc = AnaExpo(NPN1Var->lam * Vbc);
    RBlend = 1 / (1 + NPN1Var->For2 * ExpVbc);
    Irev = (NPN1Var->IECO * ExpVbc - NPN1Var->IECO) * RBlend +
	   (1 - RBlend) * NPN1Var->R1 * Vbc;

    Ib = Irev + Ifor;

    act->Iin += Ib;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    if (WITH->attr[N_Cbc - 1].UU.r != 0)
      act->Iin = NPN1Var->TauCbc * (VPin2 - VPin1 - VMem2 + VMem1) + act->Iin;

    Vbe = VPin1 - VPin3;
    Vbc = VPin1 - VPin2;

    ExpVbe = AnaExpo(NPN1Var->lam * Vbe);
    FBlend = 1 / (1 + NPN1Var->For2 * ExpVbe);
    Ifor = (NPN1Var->ICEO * ExpVbe - NPN1Var->ICEO) * FBlend +
	   (1 - FBlend) * NPN1Var->For1 * Vbe;

    ExpVbc = AnaExpo(NPN1Var->lam * Vbc);
    RBlend = 1 / (1 + NPN1Var->For2 * ExpVbc);
    Irev = (NPN1Var->IECO * ExpVbc - NPN1Var->IECO) * RBlend +
	   (1 - RBlend) * NPN1Var->R1 * Vbc;

    Ic = NPN1Var->betaF * Ifor - (1 + NPN1Var->betaR) * Irev;

    act->Iin += Ic;
    AnaCapIin(act);
    break;

  case 3:
    WITH = act->inst;
    if (WITH->attr[N_Cbe - 1].UU.r != 0)
      act->Iin = NPN1Var->TauCbe * (VPin3 - VPin1 - VMem3 + VMem1) + act->Iin;

    Vbe = VPin1 - VPin3;
    Vbc = VPin1 - VPin2;

    ExpVbe = AnaExpo(NPN1Var->lam * Vbe);
    FBlend = 1 / (1 + NPN1Var->For2 * ExpVbe);
    Ifor = (NPN1Var->ICEO * ExpVbe - NPN1Var->ICEO) * FBlend +
	   (1 - FBlend) * NPN1Var->For1 * Vbe;

    ExpVbc = AnaExpo(NPN1Var->lam * Vbc);
    RBlend = 1 / (1 + NPN1Var->For2 * ExpVbc);
    Irev = (NPN1Var->IECO * ExpVbc - NPN1Var->IECO) * RBlend +
	   (1 - RBlend) * NPN1Var->R1 * Vbc;

    Ib = Irev + Ifor;
    Ic = NPN1Var->betaF * Ifor - (1 + NPN1Var->betaR) * Irev;
    Ie = Ib + Ic;

    act->Iin -= Ie;
    AnaCapIin(act);
    break;

  default:
    printf("Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_NPN1*/

Local Void GetCnf_NPN1(NewIC)
ICNPN1 *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICNPN1 *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("NPN1", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "BETAF")) {
	TRY(try1);
	  WITH->ICbetaF = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICbetaF = 100.0;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "VCROW")) {
	TRY(try2);
	  WITH->ICVcrow = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try2);
	  WITH->ICVcrow = 0.8;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "BETAR")) {
	TRY(try3);
	  WITH->ICbetaR = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try3);
	  WITH->ICbetaR = 1.0;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "IS")) {
	TRY(try4);
	  WITH->ICIs = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try4);
	  WITH->ICIs = 1e-15;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "KT/Q")) {
	TRY(try5);
	  WITH->ICKtq = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try5);
	  WITH->ICKtq = 0.025;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "CBE")) {
	TRY(try6);
	  WITH->InitCbe = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try6);
	  WITH->InitCbe = 10e-15;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "CBC")) {
	TRY(try7);
	  WITH->InitCbc = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try7);
	  WITH->InitCbc = 10e-15;
	ENDTRY(try7);
      }
      if (!strcmp(Keyword, "CBSUB")) {
	TRY(try8);
	  WITH->InitCgb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try8);
	  WITH->InitCgb = 10e-15;
	ENDTRY(try8);
      }
      if (!strcmp(Keyword, "CCSUB")) {
	TRY(try9);
	  WITH->InitCdb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try9);
	  WITH->InitCdb = 10e-15;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "CESUB")) {
	TRY(try10);
	  WITH->InitCsb = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try10);
	  WITH->InitCsb = 10e-15;
	ENDTRY(try10);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_NPN1(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICNPN1 *NewIC;

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
  NewCell->plist_cell[2].standalone = false;
  NewCell->plist_cell[2].active = true;
  NewIC = (ICNPN1 *)Malloc(sizeof(ICNPN1));
  NewIC->ICbetaF = 100.0;
  NewIC->ICbetaR = 1.0;
  NewIC->ICIs = 1e-15;
  NewIC->ICKtq = 0.025;
  NewIC->ICVcrow = 0.8;
  NewIC->InitCbe = 10e-15;
  NewIC->InitCbc = 10e-15;
  NewIC->InitCgb = 10e-15;
  NewIC->InitCdb = 10e-15;
  NewIC->InitCsb = 10e-15;
  GetCnf_NPN1(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*NPN1_Initlib*/

#define MAKE_ICEO(n)	((n)->ICEO=(n)->Is/(n)->betaR)
#define MAKE_IECO(n)	((n)->IECO=(n)->Is/(n)->betaF)

/* #define MAKE_ICEO(n)	((n)->ICEO=(n)->Is*(n)->betaF/((n)->betaR+(n)->betaF))
   #define MAKE_IECO(n)	((n)->IECO=(n)->Is*(n)->betaR/((n)->betaR+(n)->betaF))
*/

Local boolean NPN1check(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_betaF:
    if (Inst->attr[Attrnum - 1].UU.r < 1e-10 ||
	Inst->attr[Attrnum - 1].blnk || Inst->attr[Attrnum - 1].UU.r > 1e10)
      Result = false;
    else {
      NPN1Var->betaF = Inst->attr[N_betaF - 1].UU.r;
      MAKE_ICEO(NPN1Var);
      MAKE_IECO(NPN1Var);
      NPN1Var->For1 = NPN1Var->ICEO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->R1 = NPN1Var->IECO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
    }
    break;

  case N_betaR:
    if (Inst->attr[Attrnum - 1].UU.r < 1e-10 ||
	Inst->attr[Attrnum - 1].blnk || Inst->attr[Attrnum - 1].UU.r > 1e10)
      Result = false;
    else {
      NPN1Var->betaR = Inst->attr[N_betaR - 1].UU.r;
      MAKE_ICEO(NPN1Var);
      MAKE_IECO(NPN1Var);
      NPN1Var->For1 = NPN1Var->ICEO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->R1 = NPN1Var->IECO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
    }
    break;

  case N_Is:
    if (Inst->attr[Attrnum - 1].UU.r < AnaITooSmall ||
	Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else {
      NPN1Var->Is = Inst->attr[N_Is - 1].UU.r;
      MAKE_ICEO(NPN1Var);
      MAKE_IECO(NPN1Var);
      NPN1Var->For1 = NPN1Var->ICEO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->R1 = NPN1Var->IECO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
    }
    break;

  case N_Ktq:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > 1)
      Result = false;
    else {
      NPN1Var->lam = 1 / Inst->attr[N_Ktq - 1].UU.r;
      NPN1Var->For1 = NPN1Var->ICEO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->R1 = NPN1Var->IECO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->For2 = AnaExpo(-NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
      NPN1Var->R2 = AnaExpo(-NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
    }
    break;

  case N_Vcrow:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > 1.5)
      Result = false;
    else {
      NPN1Var->For1 = NPN1Var->ICEO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->R1 = NPN1Var->IECO *
	  (AnaExpo(NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r) - 1) /
	  Inst->attr[N_Vcrow - 1].UU.r;
      NPN1Var->For2 = AnaExpo(-NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
      NPN1Var->R2 = AnaExpo(-NPN1Var->lam * Inst->attr[N_Vcrow - 1].UU.r);
    }
    break;
  }
  return Result;
}


Local Void Attr_NPN1(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 8) {
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
    return;
  }
  if ((unsigned long)Attrnum < 32 &&
      ((1L << Attrnum) & ((1L << N_Cbe) | (1L << N_Cbc))) != 0)
    act->ok = AnaCrange(act->inst, Attrnum, true);
  else
    act->ok = NPN1check(act->inst, Attrnum);
}


Local Void Newgate_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNPN1 *ICptr;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICNPN1 *)Cellptr->Ainfo;
  NPN1Var = (NPN1Const *)Malloc(sizeof(NPN1Const));
  NPN1Var->betaF = ICptr->ICbetaF;
  NPN1Var->betaR = ICptr->ICbetaR;
  NPN1Var->Is = ICptr->ICIs;
  MAKE_ICEO(NPN1Var);
  MAKE_IECO(NPN1Var);
  NPN1Var->lam = 1 / ICptr->ICKtq;
  NPN1Var->For1 = NPN1Var->ICEO *
      (AnaExpo(NPN1Var->lam * ICptr->ICVcrow) - 1) / ICptr->ICVcrow;
  NPN1Var->R1 = NPN1Var->IECO * (AnaExpo(NPN1Var->lam * ICptr->ICVcrow) - 1) /
		ICptr->ICVcrow;
  NPN1Var->For2 = AnaExpo(-NPN1Var->lam * ICptr->ICVcrow);
  NPN1Var->R2 = AnaExpo(-NPN1Var->lam * ICptr->ICVcrow);
  NPN1Var->Limit = false;
  NPN1Var->OldLimit = false;
  NPN1Var->NewLimit = false;
  NPN1Var->TauCbc = 0.0;
  NPN1Var->TauCbe = 0.0;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NPN1Var;
  AnaCapInit(act->inst);
  A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
  A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
  A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
  WITH = act->inst;
  WITH->attr[N_Cbc - 1].UU.r = ICptr->InitCbc;   /*Cbc*/
  WITH->attr[N_Cbc - 1].blnk = false;
  WITH->attr[N_Cbc - 1].changed = true;
  WITH->attr[N_Cbe - 1].UU.r = ICptr->InitCbe;   /*Cbe*/
  WITH->attr[N_Cbe - 1].blnk = false;
  WITH->attr[N_Cbe - 1].changed = true;
  WITH->attr[N_betaF - 1].UU.r = ICptr->ICbetaF;
  WITH->attr[N_betaF - 1].blnk = false;
  WITH->attr[N_betaF - 1].changed = true;
  WITH->attr[N_betaR - 1].UU.r = ICptr->ICbetaR;
  WITH->attr[N_betaR - 1].blnk = false;
  WITH->attr[N_betaR - 1].changed = true;
  WITH->attr[N_Is - 1].UU.r = ICptr->ICIs;
  WITH->attr[N_Is - 1].blnk = false;
  WITH->attr[N_Is - 1].changed = true;
  WITH->attr[N_Ktq - 1].UU.r = ICptr->ICKtq;
  WITH->attr[N_Ktq - 1].blnk = false;
  WITH->attr[N_Ktq - 1].changed = true;
  WITH->attr[N_Vcrow - 1].UU.r = ICptr->ICVcrow;
  WITH->attr[N_Vcrow - 1].blnk = false;
  WITH->attr[N_Vcrow - 1].changed = true;
}

Local Void Copygate_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var, *Old_NPN1var;
  Anainstlist *A_Gate, *A_Oldgate;

  /*If any internal variables then*/
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_NPN1var = (NPN1Const *)A_Oldgate->InstVar;
  NPN1Var = (NPN1Const *)Malloc(sizeof(NPN1Const));
  *NPN1Var = *Old_NPN1var;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NPN1Var;
  /*Always Do*/
  AnaCapCopy(act->inst);
}

Local Void Dispose_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  Free(NPN1Var);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_Npn1(act)
Analog_32_action *act;
{
  NPN1Const *Npn1Var;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICNPN1 *ICptr;
  log_grec *WITH;

  AnaCapattrread(1L, act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  Npn1Var = (NPN1Const *)A_Gate->InstVar;
  if (!AnaOldmodel) {
    Cellptr = (AnaCell_rec *)act->inst->kind->info;
    ICptr = (ICNPN1 *)Cellptr->Ainfo;
    A_Gate->Pininfo[0].Cap = ICptr->InitCgb;
    A_Gate->Pininfo[1].Cap = ICptr->InitCdb;
    A_Gate->Pininfo[2].Cap = ICptr->InitCsb;
    WITH = act->inst;
    WITH->attr[N_Cbc - 1].UU.r = ICptr->InitCbc;   /*Cbc*/
    WITH->attr[N_Cbc - 1].blnk = false;
    WITH->attr[N_Cbc - 1].changed = true;
    WITH->attr[N_Cbe - 1].UU.r = ICptr->InitCbe;   /*Cbe*/
    WITH->attr[N_Cbe - 1].blnk = false;
    WITH->attr[N_Cbe - 1].changed = true;
    WITH->attr[N_betaF - 1].UU.r = ICptr->ICbetaF;
    WITH->attr[N_betaF - 1].blnk = false;
    WITH->attr[N_betaF - 1].changed = true;
    WITH->attr[N_betaR - 1].UU.r = ICptr->ICbetaR;
    WITH->attr[N_betaR - 1].blnk = false;
    WITH->attr[N_betaR - 1].changed = true;
    WITH->attr[N_Is - 1].UU.r = ICptr->ICIs;
    WITH->attr[N_Is - 1].blnk = false;
    WITH->attr[N_Is - 1].changed = true;
    WITH->attr[N_Ktq - 1].UU.r = ICptr->ICKtq;
    WITH->attr[N_Ktq - 1].blnk = false;
    WITH->attr[N_Ktq - 1].changed = true;
    WITH->attr[N_Vcrow - 1].UU.r = ICptr->ICVcrow;
    WITH->attr[N_Vcrow - 1].blnk = false;
    WITH->attr[N_Vcrow - 1].changed = true;
  }
  WITH = act->inst;
  Npn1Var->betaF = WITH->attr[N_betaF - 1].UU.r;
  Npn1Var->betaR = WITH->attr[N_betaR - 1].UU.r;
  Npn1Var->Is = WITH->attr[N_Is - 1].UU.r;
  MAKE_ICEO(Npn1Var);
  MAKE_IECO(Npn1Var);
  Npn1Var->lam = 1 / WITH->attr[N_Ktq - 1].UU.r;
  Npn1Var->For1 = Npn1Var->ICEO *
		  (AnaExpo(Npn1Var->lam * WITH->attr[N_Vcrow - 1].UU.r) - 1) /
		  WITH->attr[N_Vcrow - 1].UU.r;
  Npn1Var->R1 = Npn1Var->IECO *
		(AnaExpo(Npn1Var->lam * WITH->attr[N_Vcrow - 1].UU.r) - 1) /
		WITH->attr[N_Vcrow - 1].UU.r;
  Npn1Var->For2 = AnaExpo(-Npn1Var->lam * WITH->attr[N_Vcrow - 1].UU.r);
  Npn1Var->R2 = AnaExpo(-Npn1Var->lam * WITH->attr[N_Vcrow - 1].UU.r);
  Npn1Var->TauCbc = 0.0;
  Npn1Var->TauCbe = 0.0;
}

Local Void Probe_Npn1(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaExt_rec *Node1Ptr, *Node2Ptr, *Node3Ptr;
  double Node1, Node2, Node3, Pin1, Pin2, Pin3, d1, d2;
  log_grec *WITH;
  Char STR1[18];
  Char STR2[12];
  Char STR3[16];
  Char STR4[256];
  Char STR5[256];

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
  d2 = Pin1 - Pin3;
  switch (AnaLogglobals->probepin) {

  case 0:
    AnaScoreboard(Gatename, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s (gate)", Gatename);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR1, "%s (collector)", Gatename);
    AnaScoreboard(STR1, (long)AnaMessGate1);
    break;

  case 3:
    sprintf(STR3, "%s (emitter)", Gatename);
    AnaScoreboard(STR3, (long)AnaMessGate1);
    break;
  }
  sprintf(STR5, "Vce = %s", AnaProbeVoltStr(STR4, Pin2 - Pin3));
  AnaScoreboard(STR5, (long)AnaMessGate2);
  if (Pin1 - Pin3 > WITH->attr[N_Vcrow - 1].UU.r ||
      Pin1 - Pin2 > WITH->attr[N_Vcrow - 1].UU.r)
    AnaScoreboard("Too much base current", (long)AnaMessGate3);
  else {
    sprintf(STR5, "Vbe = %s", AnaProbeVoltStr(STR4, Pin1 - Pin3));
    AnaScoreboard(STR5, (long)AnaMessGate3);
  }
}

Local Void Reset_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  NPN1Var->Limit = false;
  NPN1Var->OldLimit = false;
  NPN1Var->NewLimit = false;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, NPN1Var->Limit, false);
}

Local Void PreUpdate_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr, *Pin3Ptr;
  double VPin1, Vpin2, Vpin3;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
  Pin3Ptr = (AnaExt_rec *)WITH1->pin[2]->info;
  VPin1 = Pin1Ptr->last;
  Vpin2 = Pin2Ptr->last;
  Vpin3 = Pin3Ptr->last;
  NPN1Var->NewLimit = (VPin1 - Vpin3 >= WITH1->attr[N_Vcrow - 1].UU.r ||
		       VPin1 - Vpin2 >= WITH1->attr[N_Vcrow - 1].UU.r);
}

Local Void Update_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  NPN1Var->OldLimit = NPN1Var->Limit;
  NPN1Var->Limit = NPN1Var->NewLimit;
}

Local Void Display_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (NPN1Var->OldLimit != NPN1Var->Limit)
      Limitfill(act->inst, NPN1Var->Limit, false);
  }
}


Local Void Refresh_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  Limitfill(act->inst, NPN1Var->Limit, false);
}

Local Void Erase_NPN1(act)
Analog_32_action *act;
{
  NPN1Const *NPN1Var;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  NPN1Var = (NPN1Const *)A_Gate->InstVar;
  Limitfill(act->inst, NPN1Var->Limit, true);
}


/*==========================================================================*/
/*                 Library Listing For  Cell NPN1                    */
/*==========================================================================*/


Void Log_NPN1_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_NPN1(act);
    break;

  case Analog_act_ex:
    Ex_NPN1(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_NPN1(act);
    break;

  case Analog_act_update:
    Update_NPN1(act);
    break;

  case Analog_act_display:
    Display_NPN1(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_NPN1(act);
    break;

  case Analog_act_reset:
    Reset_NPN1(act);
    break;

  case Analog_act_newgate:
    Newgate_NPN1(act);
    break;

  case Analog_act_copygate:
    Copygate_NPN1(act);
    break;

  case Analog_act_disposegate:
    Dispose_NPN1(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_Npn1(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_NPN1(act);
    break;

  case Analog_act_erase:
    Erase_NPN1(act);
    break;

  case Analog_act_refresh:
    Refresh_NPN1(act);
    break;

  case Analog_act_probe:
    Probe_Npn1(act);
    break;
  }
}

#undef Cstart
#undef N_Cbc
#undef N_Cbe
#undef N_betaF
#undef N_betaR
#undef N_Is
#undef N_Ktq
#undef N_Vcrow
#undef Gatename







/* End. */
