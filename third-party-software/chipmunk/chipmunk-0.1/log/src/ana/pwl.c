/******************************************************************************
 *
 *	File Name:		pwl.c
 *	Version:		2.0
 *	Project Name:		AnaLog Simulator Module
 *	Author:			Harold Levy
 *	Date:			August 1, 1991
 *	Compiler:		gcc
 *
 *	Utility:		This module simulates arbitrary current-voltage
 *				characteristics from piecewise-linear descrip-
 *                              tions.  The module reads in (V, I) data pairs
 *                              from a specified file and blends the line seg-
 *                              ments together by circular interpolation with a
 *                              specified radius.
 *
 *                              A data file must have
 *                                   3 <= # of valid data lines <= MAX_PWL
 *                              each containing two numbers separated by any
 *                              combination of spaces and/or tabs.  Column 1
 *                              is voltage and column 2 is current.  The con-
 *                              stant MAX_PWL is #defined below, and is thusly
 *                              hardwired at compile time.  Comment lines begin
 *                              with '#', and empty lines are ignored.
 *
 *                              The PWL device will light up red if the applied
 *                              bias exceeds the definition range (the position
 *                              of the light indicates which range limit is
 *                              being exceeded), and yellow if the device can-
 *                              not find the definition file (in which case the
 *                              device will act as an open circuit).
 *                              
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

#define PWL_G
#include "pwl.h"


/* Defaults */
#define	D_NoDataFile	"none"
#define	D_Radius	1.0

#define MAX_PWL 32

/* Parameter Indices */
#define	N_DataFile	1
#define	N_Radius	2

/* "Cstart" = last parameter index + 1 */
#define	Cstart		3

/* "Gatename" = device name when probed */
#define Gatename        "PWL I-V"

typedef struct PWLConst {
  char		status;
  double	r[MAX_PWL - 2];
  double	x[2*(MAX_PWL - 2)];
  int		npwlx;
  double	m[MAX_PWL - 1];
  double	b[MAX_PWL - 1];
  double	xc[MAX_PWL - 2];
  double	yc[MAX_PWL - 2];
  double	sd[MAX_PWL - 2];
  char		Limit, OldLimit, NewLimit;	/* range light stuff */
  double	low, high;
  boolean	badFile;
} PWLConst;

typedef struct ICPWL {
  char		ICDataFile[256];
  double	ICRadius;
} ICPWL;

#define   sqr(x)  ((x) * (x))


/****************************************************************************/
/*            Procedure To Light 'Limiting' Light                           */
/****************************************************************************/


Local Void Limitfill(Inst, Limitflag, Erase)
log_grec *Inst;
char Limitflag;
boolean Erase;
{
  Anainstlist *A_Gate;
  PWLConst *PWLVar;
  short pLLx = -8, pLLy = -10, pURx = -5, pURy = -13;
  short nLLx =  5, nLLy =  13, nURx =  8, nURy =  10;

  A_Gate = (Anainstlist *)Inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;

  (*AnaLogglobals->hook.hidecursor)();

  (*AnaLogglobals->hook.xform)(Inst, &pLLx, &pLLy);
  (*AnaLogglobals->hook.xform)(Inst, &pURx, &pURy);
  (*AnaLogglobals->hook.xform)(Inst, &nLLx, &nLLy);
  (*AnaLogglobals->hook.xform)(Inst, &nURx, &nURy);

  if (Erase)
  {
    m_color((long)AnaLogglobals->color.backgr);
    m_fillrect((long)pLLx, (long)pLLy, (long)pURx, (long)pURy);
    m_fillrect((long)nLLx, (long)nLLy, (long)nURx, (long)nURy);
  }
  else
  {
    if (PWLVar->badFile)
    {
      m_color((long)m_yellow);
      m_fillrect((long)pLLx, (long)pLLy, (long)pURx, (long)pURy);
      m_fillrect((long)nLLx, (long)nLLy, (long)nURx, (long)nURy);
    }
    else if (Limitflag == 1)
    {
      m_color((long)AnaColormap.C_Limiton);
      m_fillrect((long)pLLx, (long)pLLy, (long)pURx, (long)pURy);
      m_color((long)AnaLogglobals->color.backgr);
      m_fillrect((long)nLLx, (long)nLLy, (long)nURx, (long)nURy);
    }
    else if (Limitflag == 2)
    {
      m_color((long)AnaColormap.C_Limiton);
      m_fillrect((long)nLLx, (long)nLLy, (long)nURx, (long)nURy);
      m_color((long)AnaLogglobals->color.backgr);
      m_fillrect((long)pLLx, (long)pLLy, (long)pURx, (long)pURy);
    }
    else
    {
      m_color((long)AnaLogglobals->color.backgr);
      m_fillrect((long)pLLx, (long)pLLy, (long)pURx, (long)pURy);
      m_fillrect((long)nLLx, (long)nLLy, (long)nURx, (long)nURy);
    }
  }

  (*AnaLogglobals->hook.unhidecursor)();
}


/****************************************************************************/
/*            Procedure To Simulate Cell In Algorithm                        */
/****************************************************************************/

Local void locate (x, n, v, j)
double  x[], v;
int     n, *j;
{
        int ascnd,ju,jm,jl;

        jl=0;
        ju=n+1;
        ascnd=x[n-1] > x[0];
        while (ju-jl > 1)
        {
                jm=(ju+jl) >> 1;
                if (v > x[jm-1] == ascnd)
                  jl=jm;
                else
                  ju=jm;
        }
        *j=jl-1;
}


Local void interp (v, j, PWLVar, i, didv)
  double        v, *i, *didv;
  int           j;
  PWLConst	*PWLVar;
{
  double        s;

  if (j == -1) /* left region */
  {
    *i = PWLVar->m[0] * v + PWLVar->b[0];
    *didv = PWLVar->m[0];
  }
  else if (j == PWLVar->npwlx - 1) /* right region */
  {
    *i = PWLVar->m[PWLVar->npwlx/2] * v + PWLVar->b[PWLVar->npwlx/2];
    *didv = PWLVar->m[PWLVar->npwlx/2];
  }
  else if (j%2) /* odd --> linear */
  {
    *i = PWLVar->m[1+(j/2)]*v + PWLVar->b[1+(j/2)];
    *didv = PWLVar->m[1+(j/2)];
  }
  else /* even --> circular */
  {
    s = -PWLVar->sd[j/2] * sqrt(sqr(PWLVar->r[j/2]) - sqr(v - PWLVar->xc[j/2]));
    *i = s + PWLVar->yc[j/2];
    *didv = (PWLVar->xc[j/2] - v) / s;
  }
}


Local Void Ex_PWL(act)
Analog_32_action *act;
{
  double MinI1, MinI2, dI1dV1, dI1dV2, dI2dV1, dI2dV2;
  AnaExt_rec *Pin1Ptr;
  long Pin1;
  double VPin1;
  AnaExt_rec *Pin2Ptr;
  long Pin2;
  double VPin2;
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  log_grec *WITH;
  AnaMatRec *WITH2;

  double	deltaV;
  int		j;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  Pin1Ptr = (AnaExt_rec *)WITH->pin[0]->info;
  Pin1 = Pin1Ptr->nodenum;
  VPin1 = Pin1Ptr->last;
  Pin2Ptr = (AnaExt_rec *)WITH->pin[1]->info;
  Pin2 = Pin2Ptr->nodenum;
  VPin2 = Pin2Ptr->last;

  deltaV = VPin2 - VPin1;

  if (PWLVar->badFile)
  {
    MinI1 = 0.0;
    dI1dV1 = 0.0;
  }
  else
  {
    locate (PWLVar->x, PWLVar->npwlx, deltaV, &j);
    interp (deltaV, j, PWLVar, &MinI1, &dI1dV1);
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
}  /*Ex_PWL*/


/****************************************************************************/
/*          Procedure To Supply Current  Out of a Cell To Current Meters    */
/****************************************************************************/


Local Void Iin_PWL(act)
Analog_32_action *act;
{
  AnaExt_rec *Pin1Ptr;
  double VPin1, VMem1;
  AnaExt_rec *Pin2Ptr;
  double VPin2, VMem2;
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  log_grec *WITH;

  double        deltaV, MinI1, dI1dV1;
  int		j;

  WITH = act->inst;
  A_Gate = (Anainstlist *)WITH->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
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
    if (PWLVar->badFile)
    {
      MinI1 = 0.0;
      dI1dV1 = 0.0;
    }
    else
    {
      locate (PWLVar->x, PWLVar->npwlx, deltaV, &j);
      interp (deltaV, j, PWLVar, &MinI1, &dI1dV1);
    }
    act->Iin += MinI1;
    AnaCapIin(act);
    break;

  case 2:
    WITH = act->inst;
    deltaV = VPin2 - VPin1;
    if (PWLVar->badFile)
    {
      MinI1 = 0.0;
      dI1dV1 = 0.0;
    }
    else
    {
      locate (PWLVar->x, PWLVar->npwlx, deltaV, &j);
      interp (deltaV, j, PWLVar, &MinI1, &dI1dV1);
    }
    act->Iin += MinI1;
    AnaCapIin(act);
    break;

  default:
    fprintf(stderr, "Iin_PWL: Data Structure Corruption\n");
    _Escape(1002);
    break;
  }
}  /*Iin_PWL*/


Local void findcircle (x1, y1, x2, y2, x3, y3, rd,
                       xc, yc, xl, xr, sd, ml, mr, bl, br)
  double        x1, y1, x2, y2, x3, y3, *rd;
  double        *xc, *yc, *xl, *xr, *sd, *ml, *mr, *bl, *br;
{
  double        r;
  double        pxl, pyl, pxr, pyr;
  double        a, b, c;
  double        al, ar;

  *ml = (y2 - y1) / (x2 - x1);
  *mr = (y3 - y2) / (x3 - x2);
  *sd = *mr - *ml;

  *bl = y1 - (*ml * x1);
  *br = y2 - (*mr * x2);

  r = *rd;

retry:

  pxl = fabs ( (r * (*ml)) / sqrt (1.0 + sqr(*ml)) );
  if (*sd >= 0.0)
  {
    if (*ml > 0.0) pxl = -pxl;
  }
  else
  {
    if (*ml < 0.0) pxl = -pxl;
  }
  pyl = ((-1.0/(*ml)) * pxl) + (*bl);

  pxr = fabs ( (r * (*mr)) / sqrt (1.0 + sqr(*mr)) );
  if (*sd >= 0.0)
  {
    if (*mr > 0.0) pxr = -pxr;
  }
  else
  {
    if (*mr < 0.0) pxr = -pxr;
  }
  pyr = ((-1.0/(*mr)) * pxr) + (*br);

  *xc = ( ((*mr)*pxr - (*ml)*pxl) - (pyr - pyl) ) / (*sd);
  *yc = (*ml) * (*xc) + (pyl - ((*ml) * pxl));

  a = 1.0 + sqr(*ml);
  b = (2.0 * (*bl) * (*ml)) - (2.0 * (*yc) * (*ml)) - (2.0 * (*xc));
  c = sqr(*xc) - (2.0 * (*bl) * (*yc)) + sqr(*bl) + sqr(*yc) - sqr(r);
  *xl = -b / (2.0 * a);

  a = 1.0 + sqr(*mr);
  b = (2.0 * (*br) * (*mr)) - (2.0 * (*yc) * (*mr)) - (2.0 * (*xc));
  c = sqr(*xc) - (2.0 * (*br) * (*yc)) + sqr(*br) * sqr(*yc) - sqr(r);
  *xr = -b / (2.0 * a);

  al = (x1 + x2) / 2.0;
  ar = (x2 + x3) / 2.0;

  if ((*xl <= al) || (*xr >= ar))
  {
    r *= 0.9;
    goto retry;
  }
  *rd = r;
}


Local void pwl2c (x, y, n, radius, PWLVar)
  double        x[], y[], radius;
  int           n;
  PWLConst      *PWLVar;
{
  double        r[MAX_PWL-2];
  double        xc[MAX_PWL-2], yc[MAX_PWL-2], xl[MAX_PWL-2], xr[MAX_PWL-2];
  double        sd[MAX_PWL-2];
  double        ml[MAX_PWL-2], mr[MAX_PWL-2], bl[MAX_PWL-2], br[MAX_PWL-2];
  int           i, npwlx;

  PWLVar->low = x[0];
  PWLVar->high = x[n-1];

  for (i = 1; i <= n-2; i ++)
  {
    r[i-1] = radius;
    findcircle (x[i-1],         y[i-1],
                x[i],           y[i],
                x[i+1],         y[i+1],
                &r[i-1],
                &xc[i-1],       &yc[i-1],
                &xl[i-1],       &xr[i-1],
                &sd[i-1],
                &ml[i-1],       &mr[i-1],
                &bl[i-1],       &br[i-1]);
  }

  npwlx = 0;
  for (i = 0; i < n-2; i ++)
  {
    PWLVar->r[i] = r[i];
    PWLVar->x[2*i]   = xl[i];
    PWLVar->x[2*i+1] = xr[i];
    npwlx += 2;
    PWLVar->m[i] = ml[i];
    PWLVar->b[i] = bl[i];
    PWLVar->xc[i] = xc[i];
    PWLVar->yc[i] = yc[i];
    PWLVar->sd[i] = (sd[i] >= 0.0) ? 1.0 : -1.0;
  }
  PWLVar->npwlx = npwlx;
  PWLVar->m[n-2] = mr[n-3];
  PWLVar->b[n-2] = br[n-3];
}


Local char CheckPWLFile (datafile)
  FILE		*datafile;
{
  int		n;
  double        x[MAX_PWL], y[MAX_PWL];
  char		str[256];

  if (!datafile)
    return (0);
  else
  {
    n = 0;
    while (fgets (str, 255, datafile))
    {
      if (str[0] == '#')
        continue;
      else if (sscanf (str, "%lf %lf", &x[n], &y[n]) == 2)
        n ++;
    }
    if ((n < 3) || (n > MAX_PWL)) return (0);
  }
  rewind (datafile);
  return (1);
}


Local void LoadPWLVar (PWLVar, datafileName, radius)
  PWLConst	*PWLVar;
  char		*datafileName;
  double	radius;
{
  double        x[MAX_PWL], y[MAX_PWL];
  int           n, i;
  FILE          *datafile;
  char		badPWLFile = 0;
  char		str[256];

  datafile = fopen (datafileName, "r");
  if (datafile && CheckPWLFile (datafile))
  {
      n = 0;
      while (fgets (str, 255, datafile))
      {
        if (str[0] == '#')
          continue;
        else if (sscanf (str, "%lf %lf", &x[n], &y[n]) == 2)
          n ++;
      }
      fclose (datafile);
      pwl2c (x, y, n, radius, PWLVar);
      PWLVar->badFile = false;
  }
  else
    PWLVar->badFile = true;
}


Local Void GetCnf_PWL(NewIC)
ICPWL *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICPWL *WITH;
  Char *STR1;

  FILE		*datafile;
  char		badPWLFile = 0;

/* Read in defaults from models.cnf */

  WITH = NewIC;
  do {
    AnaGetcommand("PWL", Arg, &Found);
    if (Found)
    {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "DATAFILE"))
      {
	TRY(try1);
	  if (!strcmp (Arg, D_NoDataFile))
	    strcpy (WITH->ICDataFile, D_NoDataFile);
	  else
	  {
	    datafile = fopen (Arg, "r");
            if (datafile && CheckPWLFile (datafile))
	    {
	      strcpy (WITH->ICDataFile, Arg);
            }
	    if (datafile) fclose (datafile);
	  }
	RECOVER(try1);
	  strcpy (WITH->ICDataFile, D_NoDataFile);
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "RADIUS"))
      {
        TRY(try1);
	  WITH->ICRadius = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	RECOVER(try1);
	  WITH->ICRadius = D_Radius;
	ENDTRY(try1);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/

Local Void initlib_PWL(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICPWL *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell =(Anapinrec*)Malloc(sizeof(Anapinrec)*act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = true;
  NewCell->plist_cell[1].standalone = false;
  NewCell->plist_cell[1].active = true;
  NewIC = (ICPWL *)Malloc(sizeof(ICPWL));

  strcpy (NewIC->ICDataFile, D_NoDataFile);
  NewIC->ICRadius = D_Radius;

  GetCnf_PWL(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*PWL_Initlib*/



Local boolean PWLcheck(Inst, Attrnum)
log_grec *Inst;
long Attrnum;
{
  boolean Result;
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  FILE		*datafile;

  Result = true;
  A_Gate = (Anainstlist *)Inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  switch (Attrnum)
  {
    case N_DataFile:
      if (strcmp (Inst->attr[N_DataFile - 1].UU.c, D_NoDataFile))
      {
        datafile = fopen (Inst->attr[N_DataFile - 1].UU.c, "r");
        if (!datafile || !CheckPWLFile (datafile) ||
            Inst->attr[Attrnum - 1].blnk)
          Result = false;
        else
	  LoadPWLVar (PWLVar, Inst->attr[N_DataFile - 1].UU.c,
			      Inst->attr[N_Radius - 1].UU.r);
        if (datafile) fclose (datafile);
      }
      else
        PWLVar->badFile = true;
      Limitfill (Inst, PWLVar->Limit, false);
      break;

    case N_Radius:
      if (Inst->attr[N_Radius - 1].UU.r <= 0 || Inst->attr[N_Radius - 1].blnk)
        Result = false;
      else
	LoadPWLVar (PWLVar, Inst->attr[N_DataFile - 1].UU.c, 
                            Inst->attr[N_Radius - 1].UU.r);
      break;
  }

  return Result;
}


Local Void Attr_PWL(act)
Analog_32_action *act;
{
  long Attrnum;

  Attrnum = act->pin;
  if (Attrnum >= Cstart && Attrnum <= Cstart + 5)
    AnaCapattrinsert((long)Cstart, Attrnum, act->inst, &act->ok);
  else
    act->ok = PWLcheck(act->inst, Attrnum);
}


Local Void Newgate_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICPWL *ICptr;
  log_grec *WITH;

  double	x[MAX_PWL], y[MAX_PWL];
  int		n, i;
  FILE		*datafile;
  long		Index;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICPWL *)Cellptr->Ainfo;
  PWLVar = (PWLConst *)Malloc(sizeof(PWLConst));
  if (!PWLVar)
  {
    fprintf (stderr, "\nNewgate: malloc error for PWLVar.\n");
    exit (1);
  }
  LoadPWLVar (PWLVar, ICptr->ICDataFile, ICptr->ICRadius);

  PWLVar->Limit = 0;
  PWLVar->OldLimit = 0;
  PWLVar->NewLimit = 0;

  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)PWLVar;
  AnaCapInit(act->inst);
  WITH = act->inst;

  strcpy (WITH->attr[N_DataFile - 1].UU.c, ICptr->ICDataFile);
  WITH->attr[N_Radius - 1].UU.r = ICptr->ICRadius;

  for (Index = 0; Index <= Cstart - 2; Index++)
  {
    WITH->attr[Index].blnk = false;
    WITH->attr[Index].changed = true;
  }
}

Local Void Copygate_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar, *Old_PWLvar;
  Anainstlist *A_Gate, *A_Oldgate;

  /*If any internal variables then*/
  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_PWLvar = (PWLConst *)A_Oldgate->InstVar;
  PWLVar = (PWLConst *)Malloc(sizeof(PWLConst));
  *PWLVar = *Old_PWLvar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)PWLVar;
  /*Always Do*/
  AnaCapCopy(act->inst);
}

Local Void Dispose_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  Free(PWLVar);
  AnaCapDispose(act->inst);   /*7*/
}

Local Void Readgate_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  AnaCapattrread((long)Cstart, act->inst);   /*Read capacitors*/
  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  WITH1 = act->inst;

  LoadPWLVar (PWLVar, WITH1->attr[N_DataFile - 1].UU.c,
                      WITH1->attr[N_Radius - 1].UU.r);
}


Local Void Probe_PWL(act)
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


Local Void Reset_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  AnaCapReset(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  PWLVar->Limit = 0;
  PWLVar->OldLimit = 0;
  PWLVar->NewLimit = 0;
  if (AnaLogglobals->showpage == act->page)
    Limitfill(act->inst, PWLVar->Limit, false);
}


Local Void PreUpdate_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  AnaExt_rec *Pin1Ptr, *Pin2Ptr;
  double VPin1, VPin2, deltaV;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  Pin1Ptr = (AnaExt_rec *)WITH1->pin[0]->info;
  Pin2Ptr = (AnaExt_rec *)WITH1->pin[1]->info;
  VPin1 = Pin1Ptr->last;
  VPin2 = Pin2Ptr->last;
  deltaV = VPin1 - VPin2;
  if (deltaV >= PWLVar->high)
    PWLVar->NewLimit = 1;
  else if (deltaV <= PWLVar->low)
    PWLVar->NewLimit = 2;
  else
    PWLVar->NewLimit = 0;
}


Local Void Update_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  AnaCapUpdate(act->inst);
  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  PWLVar->OldLimit = PWLVar->Limit;
  PWLVar->Limit = PWLVar->NewLimit;
}


Local Void Display_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;
  log_grec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  WITH1 = act->inst;
  if (AnaLogglobals->showpage == act->page) {
    if (PWLVar->OldLimit != PWLVar->Limit)
      Limitfill(act->inst, PWLVar->Limit, false);
  }
}


Local Void Refresh_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  Limitfill(act->inst, PWLVar->Limit, false);
}

Local Void Erase_PWL(act)
Analog_32_action *act;
{
  PWLConst *PWLVar;
  Anainstlist *A_Gate;

  A_Gate = (Anainstlist *)act->inst->info;
  PWLVar = (PWLConst *)A_Gate->InstVar;
  Limitfill(act->inst, PWLVar->Limit, true);
}


/*==========================================================================*/
/*                 Library Listing For  Cell PWL                    */
/*==========================================================================*/


Void Log_PWL_INITLIB_32(act)
Analog_32_action *act;
{
  /*Main Procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_PWL(act);
    break;

  case Analog_act_ex:
    Ex_PWL(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_PWL(act);
    break;

  case Analog_act_update:
    Update_PWL(act);
    break;

  case Analog_act_display:
    Display_PWL(act);
    break;

  case Analog_act_pass1:
    AnaCappass1(act->inst);
    break;

  case Analog_act_pass2:
    AnaCappass2(act->inst);
    break;

  case Analog_act_attrchange:
    Attr_PWL(act);
    break;

  case Analog_act_reset:
    Reset_PWL(act);
    break;

  case Analog_act_newgate:
    Newgate_PWL(act);
    break;

  case Analog_act_copygate:
    Copygate_PWL(act);
    break;

  case Analog_act_disposegate:
    Dispose_PWL(act);
    break;

  case Analog_act_openconfig:
    AnaCapattrload((long)Cstart, act->inst, act->ok);
    break;

  case Analog_act_readgate:
    Readgate_PWL(act);
    break;

  case Analog_act_writegate:
    AnaCapattrwrite((long)Cstart, act->inst);
    break;

  case Analog_act_Iin:
    Iin_PWL(act);
    break;

  case Analog_act_erase:
    Erase_PWL(act);
    break;

  case Analog_act_refresh:
    Refresh_PWL(act);
    break;

  case Analog_act_probe:
    Probe_PWL(act);
    break;
  }
}


#undef Cstart
#undef Gatename



/* End. */
