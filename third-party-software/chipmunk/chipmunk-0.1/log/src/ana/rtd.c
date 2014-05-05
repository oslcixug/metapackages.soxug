/*	File Name:		rtd.c
 *	Version:		1.0
 *	Project Name:		AnaLog Simulator Module
 *	Author:			Harold Levy
 *	Date:			August 1, 1991
 *	Compiler:		Sparcstation
 *
 *	Utility:		This module simulates resonant-tunneling
 *				devices for use with AnaLOG.
 *
 *	Notes:			har@caltech.edu
 *				128-95 Caltech/Pasadena, CA 91125
 *				Please send questions/bug fixes regarding this
 *				module to the above addresses.
 *
 ******************************************************************************/
/*
   The licensing agreement for this software is identical to that of the rest
   of the LOG/DigLog/AnaLog source code:

   "LOG", a circuit editing and simulation system,

   "DigLOG", the digital simulator for LOG.

   Copyright (C) 1985, 1990 California Institute of Technology.
   Author: David Gillespie
   Maintainers: John Lazzaro and Dave Gillespie
   Maintainers's address: lazzaro@hobiecat.cs.caltech.edu;
                          CB 425/ CU Boulder/Boulder CO 91125.
                          daveg@csvax.caltech.edu;
                          256-80 Caltech/Pasadena CA 91125.
                          Send questions, bug fixes, to these addresses.

   "AnaLOG", the analog simulator for LOG.
   Copyright (C) 1985, 1990 California Institute of Technology.
   Author: John Lazzaro
   Maintainer: John Lazzaro
   Maintainers's address: lazzaro@hobiecat.cs.caltech.edu;
                          CB 425/ CU Boulder/Boulder CO 91125.
                          Send questions, bug fixes, to this address.


This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (Version 1, 1989).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <math.h>
#include <p2c/p2c.h>

#define RTD_G
#include "rtd.h"

/* Defaults */
#define	D_Sharpness	100.0
#define D_NDRSV		0.1
#define D_NDREV		0.2
#define D_VllyC		0.0002
#define D_TailC		1e-6
#define D_TailV		0.1

#define	D_Mode		0
#define D_eOnsRes       100.0
#define D_aOnsC         1e-5
#define D_aOnsV         0.1


/* Parameter Indices */
#define N_Sharpness	1
#define N_NDRSV		2
#define N_NDREV		3
#define N_VllyC		4
#define N_TailC		5
#define N_TailV		6

#define N_Mode		7
#define N_eOnsRes	8
#define N_aOnsC		9
#define N_aOnsV		10

#define Cstart          11

/* Device name when probed: */
#define Gatename        "RTD"


typedef struct RTDConst {
  double g0;
  double y0;
  double g1;
  double b;
} RTDConst;


typedef struct ICRTD {
  double	ICSharpness;
  double	ICNDRSV;
  double	ICNDREV;
  double	ICVllyC;
  double	ICTailC;
  double	ICTailV;
  long		ICMode;
  double	ICeOnsRes;
  double	ICaOnsC;
  double	ICaOnsV;
} ICRTD;




/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local double SafeExp (x)
double	x;
{
  if (x < -700.0)
    return (0.0);
  else if (x > 700.0)
    return (1e38);
  else
    return (exp (x));
}

Local Void Ex_RTD(act)
Analog_32_action *act;
{
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2;
  RTDConst *RTDVar;
  Anainstlist *A_Gate;
  log_grec *WITH;
  AnaMatRec *WITH2;

  double	deltaV, signDV, x;
  double	g0, g1, x0, x1, y0, y1, b, u, s, m;
  double	b1x0, b2x0, b1x1, b2x1, d1x0, d2x0, d1x1, d2x1;
  double	ex0, ex1, exm, tail, dtail, t1, t2;
  double	tailC, tailV, aOnsC, aOnsV, onset, dOnset;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  RTDVar = (RTDConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;   /*Use with statement to get to internal variables*/

  deltaV = VPin2 - VPin1;  /*  VPin1 is typically > VPin2 == 0  */
  if (deltaV > 0.0)
    signDV = 1.0;
  else if (deltaV == 0.0)
    signDV = 0.0;
  else
    signDV = -1.0;

  x = fabs (deltaV);

/*
  Givens:  x0, x1, y1, g0
  Computed:
    g0 = 1 / r0;
    y0 = g0 * x0; or y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1);
    g1 = (y0 - y1) / (x0 - x1);
    b = y1 - (g1 * x1);
*/

  s = WITH->attr[N_Sharpness - 1].UU.r;
  x0 = WITH->attr[N_NDRSV - 1].UU.r;
  x1 = WITH->attr[N_NDREV - 1].UU.r;
  y1 = WITH->attr[N_VllyC - 1].UU.r;
  tailC = WITH->attr[N_TailC - 1].UU.r;
  tailV = WITH->attr[N_TailV - 1].UU.r;

  aOnsC = WITH->attr[N_aOnsC - 1].UU.r;
  aOnsV = WITH->attr[N_aOnsV - 1].UU.r;

  g0 = RTDVar->g0;
  y0 = RTDVar->y0;
  g1 = RTDVar->g1;
  b = RTDVar->b;

  m = 0.01;

  ex0 = SafeExp (s * (x - x0));
  ex1 = SafeExp (s * (x - x1));
  exm = SafeExp (-x / m);

  b1x0 = 1 / (1 + ex0);
  b1x1 = 1 / (1 + ex1);
  b2x0 = (1 - exm) * (1 - b1x0);
  b2x1 = (1 - exm) * (1 - b1x1);

  d1x0 = -s * ex0 * b1x0 * b1x0;
  d1x1 = -s * ex1 * b1x1 * b1x1;
  d2x0 = (exm / m) * (1 - b1x0) - d1x0 * (1 - exm);
  d2x1 = (exm / m) * (1 - b1x1) - d1x1 * (1 - exm);

  tail = y1 + tailC * (SafeExp ((x - x1) / tailV) - 1);
  dtail = (tailC / tailV) * SafeExp ((x - x1) / tailV);

  switch (WITH->attr[N_Mode - 1].UU.nv)
  {
    case 0:

      t1 = (g0*x)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;

      t2 = g0*(b1x0+x*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;

      if (signDV == -1.0) MinI1 = -MinI1;
      
      break;

    case 1:
      
      onset = aOnsC * (SafeExp (x / aOnsV) - 1);
      dOnset = (aOnsC / aOnsV) * SafeExp (x / aOnsV);

      t1 = (onset)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;

      t2 = (dOnset*b1x0+onset*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;

      if (signDV == -1.0) MinI1 = -MinI1;
      
      break;
  }

  MinI2 = -MinI1;
  dI1dV2 = -dI1dV1;
  dI2dV1 = -dI1dV1;
  dI2dV2 = dI1dV1;

  WITH2 = AnaSystem;
  if (Pin1 != 0)
  {
    WITH2->Mat[Pin1][AnaSysCol] += MinI1;
    WITH2->Mat[Pin1][Pin1] += dI1dV1;
    WITH2->Mat[Pin1][Pin2] += dI1dV2;
    WITH2->MatNZ[Pin1][AnaSysCol] = true;
    WITH2->MatNZ[Pin1][Pin1] = true;
    WITH2->MatNZ[Pin1][Pin1] = true;
  }
  if (Pin2 != 0)
  {
    WITH2->Mat[Pin2][AnaSysCol] += MinI2;
    WITH2->Mat[Pin2][Pin1] += dI2dV1;
    WITH2->Mat[Pin2][Pin2] += dI2dV2;
    WITH2->MatNZ[Pin2][AnaSysCol] = true;
    WITH2->MatNZ[Pin2][Pin1] = true;
    WITH2->MatNZ[Pin2][Pin2] = true;
  }
  AnaCapex(act->inst);   /*Does execution of node capacitors*/
}  /*Ex_RTD*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_RTD(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  RTDConst *RTDVar;
  Anainstlist *A_Gate;
  log_grec *WITH;

  double        deltaV, signDV, x;
  double        g0, g1, x0, x1, y0, y1, b, u, s, m;
  double        b1x0, b2x0, b1x1, b2x1, d1x0, d2x0, d1x1, d2x1;
  double        ex0, ex1, exm, tail, dtail, t1, t2;
  double	tailC, tailV, aOnsC, aOnsV, onset, dOnset;
  double	MinI1, dI1dV1;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  RTDVar = (RTDConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  VPin1 = Pin1Ptr->now;
  VMem1 = A_Gate->Pininfo[0].Vmem;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  VPin2 = Pin2Ptr->now;
  VMem2 = A_Gate->Pininfo[1].Vmem;
  switch (act->pin) {

  case 1:
    WITH = act->inst;
    deltaV = VPin1 - VPin2;

  if (deltaV > 0.0)
    signDV = 1.0;
  else if (deltaV == 0.0)
    signDV = 0.0;
  else
    signDV = -1.0;
  x = fabs (deltaV);

  x0 = WITH->attr[N_NDRSV - 1].UU.r;
  x1 = WITH->attr[N_NDREV - 1].UU.r;
  y1 = WITH->attr[N_VllyC - 1].UU.r;
  tailC = WITH->attr[N_TailC - 1].UU.r;
  tailV = WITH->attr[N_TailV - 1].UU.r;
  aOnsC = WITH->attr[N_aOnsC - 1].UU.r;
  aOnsV = WITH->attr[N_aOnsV - 1].UU.r;
  g0 = RTDVar->g0;
  y0 = RTDVar->y0;
  g1 = RTDVar->g1;
  b = RTDVar->b;
  s = WITH->attr[N_Sharpness - 1].UU.r;
  m = 0.01;
  ex0 = SafeExp (s * (x - x0));
  ex1 = SafeExp (s * (x - x1));
  exm = SafeExp (-x / m);
  b1x0 = 1 / (1 + ex0);
  b1x1 = 1 / (1 + ex1);
  b2x0 = (1 - exm) * (1 - b1x0);
  b2x1 = (1 - exm) * (1 - b1x1);
  d1x0 = -s * ex0 * b1x0 * b1x0;
  d1x1 = -s * ex1 * b1x1 * b1x1;
  d2x0 = (exm / m) * (1 - b1x0) - d1x0 * (1 - exm);
  d2x1 = (exm / m) * (1 - b1x1) - d1x1 * (1 - exm);
  tail = y1 + tailC * (SafeExp ((x - x1) / tailV) - 1);
  dtail = (tailC / tailV) * SafeExp ((x - x1) / tailV);
  switch (WITH->attr[N_Mode - 1].UU.nv)
  {
    case 0:
      t1 = (g0*x)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;
      t2 = g0*(b1x0+x*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;
      if (signDV == -1.0) MinI1 = -MinI1;
      break;
    case 1:
      onset = aOnsC * (SafeExp (x / aOnsV) - 1);
      dOnset = (aOnsC / aOnsV) * SafeExp (x / aOnsV);
      t1 = (onset)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;
      t2 = (dOnset*b1x0+onset*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;
      if (signDV == -1.0) MinI1 = -MinI1;
      break;
  }
    act->Iin += MinI1;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    deltaV = VPin2 - VPin1;

  if (deltaV > 0.0)
    signDV = 1.0;
  else if (deltaV == 0.0)
    signDV = 0.0;
  else
    signDV = -1.0;
  x = fabs (deltaV);
  
  x0 = WITH->attr[N_NDRSV - 1].UU.r;
  x1 = WITH->attr[N_NDREV - 1].UU.r;
  y1 = WITH->attr[N_VllyC - 1].UU.r;
  tailC = WITH->attr[N_TailC - 1].UU.r;
  tailV = WITH->attr[N_TailV - 1].UU.r;
  aOnsC = WITH->attr[N_aOnsC - 1].UU.r;
  aOnsV = WITH->attr[N_aOnsV - 1].UU.r;
  g0 = RTDVar->g0;
  y0 = RTDVar->y0;
  g1 = RTDVar->g1;
  b = RTDVar->b;
  s = WITH->attr[N_Sharpness - 1].UU.r;
  m = 0.01;
  ex0 = SafeExp (s * (x - x0));
  ex1 = SafeExp (s * (x - x1));
  exm = SafeExp (-x / m);
  b1x0 = 1 / (1 + ex0);
  b1x1 = 1 / (1 + ex1);
  b2x0 = (1 - exm) * (1 - b1x0);
  b2x1 = (1 - exm) * (1 - b1x1);
  d1x0 = -s * ex0 * b1x0 * b1x0;
  d1x1 = -s * ex1 * b1x1 * b1x1;
  d2x0 = (exm / m) * (1 - b1x0) - d1x0 * (1 - exm);
  d2x1 = (exm / m) * (1 - b1x1) - d1x1 * (1 - exm);
  tail = y1 + tailC * (SafeExp ((x - x1) / tailV) - 1);
  dtail = (tailC / tailV) * SafeExp ((x - x1) / tailV);
  switch (WITH->attr[N_Mode - 1].UU.nv)
  {
    case 0:
      t1 = (g0*x)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;
      t2 = g0*(b1x0+x*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;
      if (signDV == -1.0) MinI1 = -MinI1;
      break;
    case 1:
      onset = aOnsC * (SafeExp (x / aOnsV) - 1);
      dOnset = (aOnsC / aOnsV) * SafeExp (x / aOnsV);
      t1 = (onset)*b1x0 + (g1*x+b)*b2x0;
      MinI1 = t1*b1x1 + tail*b2x1;
      t2 = (dOnset*b1x0+onset*d1x0) + g1*(b2x0+x*d2x0) + b*d2x0;
      dI1dV1 = t2*b1x1 + t1*d1x1 + dtail*b2x1 + tail*d2x1;
      if (signDV == -1.0) MinI1 = -MinI1;
      break;
  }
    act->Iin += MinI1;
    AnaCapIin(act);
    break;

  default:
    fprintf(stderr, "Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_RTD*/


Local Void GetCnf_RTD(NewIC)
ICRTD *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICRTD *WITH;
  Char *STR1;

  WITH = NewIC;
  do {
    AnaGetcommand("RTD", Arg, &Found);
    if (Found)
    {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "MODE"))
      {
        (*AnaLogglobals->hook.getword) (Arg, Keyword);
        if (!strcmp(Keyword, "INTRABAND"))
          WITH->ICMode = 1; /* set mode to intRAband */
        else
          WITH->ICMode = 0; /* set mode to intERband */
      }

      if (!strcmp(Keyword, "SHARPNESS"))
      {
        TRY(try1);
          WITH->ICSharpness = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICSharpness = D_Sharpness;
        ENDTRY(try1);
      }

      if (!strcmp(Keyword, "NDRSV"))
      {    
        TRY(try1)
          WITH->ICNDRSV = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICNDRSV = D_NDRSV;
        ENDTRY(try1);
      }

      if (!strcmp(Keyword, "NDREV"))
      {    
        TRY(try1);
          WITH->ICNDREV = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICNDREV = D_NDREV;
        ENDTRY(try1);
      }

      if (!strcmp(Keyword, "VLLYC"))
      {
        TRY(try1);
          WITH->ICVllyC = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICVllyC = D_VllyC;
        ENDTRY(try1);
      }

      if (!strcmp(Keyword, "TAILC"))
      {
        TRY(try1);
          WITH->ICTailC = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICTailC = D_TailC;
        ENDTRY(try1);
      }
 
      if (!strcmp(Keyword, "TAILV"))
      {
        TRY(try1);
          WITH->ICTailV = strtod(Arg, &STR1);
          Dummy = STR1 - Arg + 1;
        RECOVER(try1);
          WITH->ICTailV = D_TailV;
        ENDTRY(try1);
      }

      if (!strcmp(Keyword, "INTERBAND"))
      {
 	(*AnaLogglobals->hook.getword) (Arg, Keyword);

        if (!strcmp(Keyword, "EONSRES"))
        {
          TRY(try1);
            WITH->ICeOnsRes = strtod(Arg, &STR1);
            Dummy = STR1 - Arg + 1;
          RECOVER(try1);
            WITH->ICeOnsRes = D_eOnsRes;
          ENDTRY(try1);
        }
      }

      if (!strcmp(Keyword, "INTRABAND"))
      {
        (*AnaLogglobals->hook.getword) (Arg, Keyword);

        if (!strcmp(Keyword, "AONSC"))
        {
          TRY(try1);
            WITH->ICaOnsC = strtod(Arg, &STR1);
            Dummy = STR1 - Arg + 1;
          RECOVER(try1);
            WITH->ICaOnsC = D_aOnsC;
          ENDTRY(try1);
        }

        if (!strcmp(Keyword, "AONSV"))
        {
          TRY(try2);
             WITH->ICaOnsV = strtod(Arg, &STR1);
            Dummy = STR1 - Arg + 1;
          RECOVER(try2); 
            WITH->ICaOnsV = D_aOnsV;
          ENDTRY(try2);
        }
      }

    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_RTD(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICRTD *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anasimple;
  NewCell->plist_cell =(Anapinrec*)Malloc(sizeof(Anapinrec)*act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewCell->plist_cell[1].standalone = false;
  NewCell->plist_cell[1].active = true;
  NewIC = (ICRTD *)Malloc(sizeof(ICRTD));

  NewIC->ICSharpness = D_Sharpness;
  NewIC->ICNDRSV = D_NDRSV;
  NewIC->ICNDREV = D_NDREV;
  NewIC->ICVllyC = D_VllyC;
  NewIC->ICTailC = D_TailC;
  NewIC->ICTailV = D_TailV;

  NewIC->ICMode = 0;
  NewIC->ICeOnsRes = D_eOnsRes;
  NewIC->ICaOnsC = D_aOnsC;
  NewIC->ICaOnsV = D_aOnsV;

  GetCnf_RTD(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*RTD_Initlib*/



Local boolean RTDcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  RTDConst *RTDVar;
  Anainstlist *A_Gate;

  double	x0, x1, y1, aOnsC, aOnsV;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  RTDVar = (RTDConst *)A_Gate->InstVar;
  switch (Attrnum) {

  case N_Mode:
    if (Inst->attr[Attrnum - 1].blnk)
      Result = false;
    else {
      if (Inst->attr[N_Mode - 1].UU.nv == 0) {}
      else {}
    }
    break;

  case N_Sharpness:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk)
      Result = false;
    break;

/* Here's how to do time variables:
  case N_TTime:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 || Inst->attr[Attrnum - 1].blnk ||
        Inst->attr[Attrnum - 1].UU.r > AnaTimeTooBig)
      Result = false;
    break;
*/

  case N_eOnsRes:
    if (Inst->attr[Attrnum - 1].UU.r <= 0 ||
        Inst->attr[Attrnum - 1].blnk ||
	Inst->attr[Attrnum - 1].UU.r > AnaResTooBig)
      Result = false;
    else {
      x0 = Inst->attr[N_NDRSV - 1].UU.r;
      x1 = Inst->attr[N_NDREV - 1].UU.r;
      y1 = Inst->attr[N_VllyC - 1].UU.r;
      aOnsC = Inst->attr[N_aOnsC - 1].UU.r;
      aOnsV = Inst->attr[N_aOnsV - 1].UU.r;
      RTDVar->g0 = 1 / Inst->attr[N_eOnsRes - 1].UU.r;
      switch (Inst->attr[N_Mode - 1].UU.nv) 
      { 
        case 0: 
          RTDVar->y0 = RTDVar->g0 * x0;
          break;
        case 1: 
          RTDVar->y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1); 
          break; 
      }
      RTDVar->g1 = (RTDVar->y0 - y1) / (x0 - x1);
      RTDVar->b = y1 - (RTDVar->g1 * x1);
    }
    break;

  case N_NDRSV:
  case N_NDREV:
  case N_TailV:
  case N_aOnsV:
    if (Inst->attr[Attrnum - 1].UU.r < AnaLowest ||
        Inst->attr[Attrnum - 1].blnk ||
        Inst->attr[Attrnum - 1].UU.r > AnaHighest)
      Result = false;
    else {
      x0 = Inst->attr[N_NDRSV - 1].UU.r;
      x1 = Inst->attr[N_NDREV - 1].UU.r;
      y1 = Inst->attr[N_VllyC - 1].UU.r;
      aOnsC = Inst->attr[N_aOnsC - 1].UU.r;
      aOnsV = Inst->attr[N_aOnsV - 1].UU.r;
      RTDVar->g0 = 1 / Inst->attr[N_eOnsRes - 1].UU.r;
      switch (Inst->attr[N_Mode - 1].UU.nv) 
      { 
        case 0: 
          RTDVar->y0 = RTDVar->g0 * x0;
          break;
        case 1: 
          RTDVar->y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1); 
          break; 
      }
      RTDVar->g1 = (RTDVar->y0 - y1) / (x0 - x1);
      RTDVar->b = y1 - (RTDVar->g1 * x1);
    }
    break;

  case N_VllyC:
  case N_TailC:
  case N_aOnsC:
    if (Inst->attr[Attrnum - 1].UU.r < 0 ||
        Inst->attr[Attrnum - 1].blnk ||
        Inst->attr[Attrnum - 1].UU.r > AnaITooBig) 
      Result = false;
    else {
      x0 = Inst->attr[N_NDRSV - 1].UU.r;
      x1 = Inst->attr[N_NDREV - 1].UU.r;
      y1 = Inst->attr[N_VllyC - 1].UU.r;
      aOnsC = Inst->attr[N_aOnsC - 1].UU.r;
      aOnsV = Inst->attr[N_aOnsV - 1].UU.r;
      RTDVar->g0 = 1 / Inst->attr[N_eOnsRes - 1].UU.r;
      switch (Inst->attr[N_Mode - 1].UU.nv)
      {
        case 0:
          RTDVar->y0 = RTDVar->g0 * x0;
          break;
        case 1:
          RTDVar->y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1);
          break;
      }
      RTDVar->g1 = (RTDVar->y0 - y1) / (x0 - x1);
      RTDVar->b = y1 - (RTDVar->g1 * x1);
    }
    break;

  }

  return Result;
}


Local Void Attr_RTD(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = RTDcheck(act->inst, Attrnum);
}


Local Void Newgate_RTD(act)
Analog_32_action *act;
{
  RTDConst *RTDVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICRTD *ICptr;
  log_grec *WITH;

  double	x0, x1, y1, aOnsC, aOnsV;
  long		Index;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICRTD *)Cellptr->Ainfo;
  RTDVar = (RTDConst *)Malloc(sizeof(RTDConst));

  x0 = ICptr->ICNDRSV;
  x1 = ICptr->ICNDREV;
  y1 = ICptr->ICVllyC;
  aOnsC = ICptr->ICaOnsC;
  aOnsV = ICptr->ICaOnsV;
 
  RTDVar->g0 = 1 / ICptr->ICeOnsRes;
  switch (ICptr->ICMode)
  {
    case 0:
      RTDVar->y0 = RTDVar->g0 * x0;
      break;
    case 1:
      RTDVar->y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1);
      break;
  }
  RTDVar->g1 = (RTDVar->y0 - y1) / (x0 - x1);
  RTDVar->b = y1 - (RTDVar->g1 * x1);

  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)RTDVar;
  AnaCapInit(act->inst);
  WITH = act->inst;

  WITH->attr[N_Mode - 1].UU.nv = ICptr->ICMode;
  WITH->attr[N_Sharpness - 1].UU.r = ICptr->ICSharpness;
  WITH->attr[N_NDRSV - 1].UU.r = ICptr->ICNDRSV;
  WITH->attr[N_NDREV - 1].UU.r = ICptr->ICNDREV;
  WITH->attr[N_VllyC - 1].UU.r = ICptr->ICVllyC;
  WITH->attr[N_TailC - 1].UU.r = ICptr->ICTailC;
  WITH->attr[N_TailV - 1].UU.r = ICptr->ICTailV;
  WITH->attr[N_eOnsRes - 1].UU.r = ICptr->ICeOnsRes;
  WITH->attr[N_aOnsC - 1].UU.r = ICptr->ICaOnsC;
  WITH->attr[N_aOnsV - 1].UU.r = ICptr->ICaOnsV;

  for (Index = 0; Index <= Cstart - 2; Index++) {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_RTD(act)
Analog_32_action *act;
{
  RTDConst *RTDVar, *Old_RTDvar;
  Anainstlist *A_Gate, *A_Oldgate;

  /*If any internal variables then*/
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_RTDvar = (RTDConst *)A_Oldgate->InstVar;
  RTDVar = (RTDConst *)Malloc(sizeof(RTDConst));
  *RTDVar = *Old_RTDvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)RTDVar;
  /*Always Do*/
  AnaCapCopy(act->inst);
}

Local Void Dispose_RTD(act)
Analog_32_action *act;
{
  RTDConst *RTDVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  RTDVar = (RTDConst *)A_Gate->InstVar;
  Free(RTDVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_RTD(act)
Analog_32_action *act;
{
  RTDConst *RTDVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  double	x0, x1, y1;
  double	aOnsC, aOnsV;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  RTDVar = (RTDConst *)A_Gate->InstVar;
  WITH1 = act->inst;

  x0 = WITH1->attr[N_NDRSV - 1].UU.r;
  x1 = WITH1->attr[N_NDREV - 1].UU.r;
  y1 = WITH1->attr[N_VllyC - 1].UU.r;
  aOnsC = WITH1->attr[N_aOnsC - 1].UU.r;
  aOnsV = WITH1->attr[N_aOnsV - 1].UU.r;
  RTDVar->g0 = 1.0 / WITH1->attr[N_eOnsRes - 1].UU.r;
  switch (WITH1->attr[N_Mode - 1].UU.nv)
  {
    case 0:
      RTDVar->y0 = RTDVar->g0 * x0;
      break;
    case 1:
      RTDVar->y0 = aOnsC * (SafeExp (x0 / aOnsV) - 1);
      break;
    default:
      fprintf (stderr, "Readgate: N_Mode error.\n");
      break;
  }
  RTDVar->g1 = (RTDVar->y0 - y1) / (x0 - x1);
  RTDVar->b = y1 - (RTDVar->g1 * x1);
}


Local Void Probe_RTD(act)
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
    sprintf(STR1, "%s <undefined>", Gatename);
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
    AnaScoreboard(Gatename, (long)AnaMessGate1);
    break;

  case 1:
    sprintf(STR2, "%s%s", Gatename, Name1);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;

  case 2:
    sprintf(STR2, "%s%s", Gatename, Name2);
    AnaScoreboard(STR2, (long)AnaMessGate1);
    break;
  }
  sprintf(STR2, "dV = %s", AnaProbeVoltStr(STR3, d));
  AnaScoreboard(STR2, (long)AnaMessGate2);
}


/*==========================================================================*/
/*                 Library Listing For  Cell RTD                    */
/*==========================================================================*/


Void Log_rtd_initlib_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_RTD(act);
    break;

  case Analog_act_ex:
    Ex_RTD(act);
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
    Attr_RTD(act);
    break;

  case Analog_act_reset:
    AnaCapReset(act->inst);
    break;

  case Analog_act_newgate:
    Newgate_RTD(act);
    break;

  case Analog_act_copygate:
    Copygate_RTD(act);
    break;

  case Analog_act_disposegate:
    Dispose_RTD(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_RTD(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_RTD(act);
    break;

  case Analog_act_probe:
    Probe_RTD(act);
    break;
  }
}


#undef D_Sharpness
#undef D_eOnsRes
#undef D_eNDRSV
#undef D_eNDREV
#undef D_eVllyC
#undef D_eTailC
#undef D_eTailV
#undef D_aOnsC
#undef D_aOnsV
#undef D_aVllyC
#undef D_aTailC
#undef D_aTailV

#undef N_Mode
#undef N_eOnsRes
#undef N_eNDRSV
#undef N_eNDREV
#undef N_eTailC
#undef N_eTailV
#undef N_aOnsC
#undef N_aOnsV
#undef N_aVllyC
#undef N_aTailC
#undef N_aTailV
#undef Cstart
#undef Gatename



/* End. */
