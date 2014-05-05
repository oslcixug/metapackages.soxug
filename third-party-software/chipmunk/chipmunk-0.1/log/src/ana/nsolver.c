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
/* From input file "nsolver.text" */


/*  This module does solutions of simultaneous equations of a finite order


    Procedure Solve;

    To solve the equations: (A ->constant,  X->variable)

        A1,1*x1  + A1,2*x2 .... A1,n*xn  =  A1,n+1
        A2,1*x1  + A2,2*x2 .... A2,n*xn  =  A2,n+1
                         .
                         .
                         .
        An,1*x1  + An,2*x2 .... An,n*xn  =  An,n+1


    Give this routine the N*(N+1) Array

        A1,1   A1,2  ....  A1,n  A1,n+1
        A2,1   A2,2  ....  A2,n  A2,n+1

                         .
                         .
                         .

        An,1   An,2 ....   An,n  An,n+1


     The routine will return : Error=True      If unsolvable equation

            If Not(Error), It will return in the matrix:

                              Col N+1

                               X1
                               X2
                               .
                               .
                               .
                               Xn    */



#include <p2c/p2c.h>


#define NSOLVER_G
#include "nsolver.h"



/* Given A matrix M, with row and column indices Sysrow and Syscol,
  the routine finds the row (>=col) which has the largest absolute value
  in the col column.  It then exchanges this row with this current col
  row.

If no row >= col has a non-zero col value, then the system is
  insolvable and Abort:=True.*/

Static Void BestRow(Col)
long Col;
{
  long I, Gr_index;
  double Greatest, Temp;
  boolean BTemp;
  AnaMatRec *WITH;
  long FORLIM;

  WITH = AnaSystem;
  Gr_index = Col;
  if (WITH->MatNZ[Col][Col])
    Greatest = fabs(WITH->Mat[Col][Col]);
  else
    Greatest = 0.0;
  FORLIM = AnaSysRow;
  for (I = Col + 1; I <= FORLIM; I++) {   /*Make Gr_Index Point to Pivot*/
    if (WITH->MatNZ[I][Col]) {
      Temp = fabs(WITH->Mat[I][Col]);
      if (Temp > Greatest) {
	Greatest = Temp;
	Gr_index = I;
      }
    }
  }
  if (Greatest < AnaEqn_Epsilon) {   /*If a bad matrix given*/
    AnaError = true;
    return;
  }
  if (Gr_index == Col)
    return;
  FORLIM = AnaSysCol;
  for (I = Col; I <= FORLIM; I++) {
    Temp = WITH->Mat[Col][I];
    BTemp = WITH->MatNZ[Col][I];
    WITH->Mat[Col][I] = WITH->Mat[Gr_index][I];
    WITH->MatNZ[Col][I] = WITH->MatNZ[Gr_index][I];
    WITH->Mat[Gr_index][I] = Temp;
    WITH->MatNZ[Gr_index][I] = BTemp;
  }
}


/********************************************************************************/

Void Solve()
{

  /********************************************************************************/
  long I, J, K;
  double Temp;
  AnaMatRec *WITH;
  long FORLIM, FORLIM1;

  WITH = AnaSystem;
  if (AnaEpoch != AnaE_Solve || AnaQuit())
    return;
  if (AnaS_Section == AnaS_Start) {
    AnaError = false;
    if (AnaSysCol == 2)
      AnaS_Section = AnaS_Finish;
    else {
      AnaS_Section = AnaS_Bestrow;
      AnaLastJ = 1;
    }
  }
  if ((AnaS_Section == AnaS_Bestrow || AnaS_Section == AnaS_Divide ||
       AnaS_Section == AnaS_Subtract) && !AnaQuit()) {
    J = AnaLastJ;
    while (J <= AnaSysCol - 2 && !AnaError && !AnaQuit()) {
      if (AnaS_Section == AnaS_Bestrow && !AnaQuit()) {
	BestRow(J);
	if (AnaError)
	  AnaS_Section = AnaS_Finish;
	else
	  AnaS_Section = AnaS_Divide;
      }
      if (AnaS_Section == AnaS_Divide && !AnaQuit()) {
	FORLIM = AnaSysRow;
	for (I = J; I <= FORLIM; I++) {   /*Divide Throught to get one's*/
	  if (WITH->MatNZ[I][J])   /*Worst case J=1 : sysrow^2 Mult*/
	  {  /*Best case j=Sysrow: 1 Mult*/
	    Temp = WITH->Mat[I][J];
	    if (fabs(Temp) > AnaEqn_Epsilon) {
	      Temp = 1 / Temp;
	      FORLIM1 = AnaSysCol;
	      for (K = J + 1; K <= FORLIM1; K++) {
		if (WITH->MatNZ[I][K])
		  WITH->Mat[I][K] *= Temp;
	      }
	    }
	  }
	}
	AnaS_Section = AnaS_Subtract;
      }
      if (AnaS_Section != AnaS_Subtract || AnaQuit())
	continue;
      FORLIM = AnaSysRow;
      for (I = J + 1; I <= FORLIM; I++) {
	if (WITH->MatNZ[I][J] && fabs(WITH->Mat[I][J]) > AnaEqn_Epsilon) {
	  FORLIM1 = AnaSysCol;
	  for (K = J + 1; K <= FORLIM1; K++) {
	    if (WITH->MatNZ[J][K])   /*Subtract*/
	    {  /*Bestcase j=Sysrow:1*/
	      if (WITH->MatNZ[I][K])   /*Worstcase J=1:sysrow^2*/
		WITH->Mat[I][K] -= WITH->Mat[J][K];
	      else {
		WITH->Mat[I][K] = -WITH->Mat[J][K];
		WITH->MatNZ[I][K] = true;
	      }
	    }
	  }
	}
      }
      if (AnaDebugset) {
	WITH->Mat[J][J] = 1.0;
	WITH->MatNZ[J][J] = true;
	FORLIM = AnaSysRow;
	for (I = J + 1; I <= FORLIM; I++) {
	  WITH->Mat[I][J] = 0.0;
	  WITH->MatNZ[I][J] = false;
	}
      }
      J++;
      AnaLastJ = J;
      if (J <= AnaSysCol - 2)
	AnaS_Section = AnaS_Bestrow;
      else
	AnaS_Section = AnaS_Finish;
    }
  }
  if (AnaS_Section != AnaS_Finish || AnaQuit())
    return;
  if (!AnaError) {
    Temp = WITH->Mat[AnaSysRow][AnaSysRow];
    if (fabs(Temp) < AnaEqn_Epsilon)
      AnaError = true;
    else {
      if (WITH->MatNZ[AnaSysRow][AnaSysCol])
	WITH->Mat[AnaSysRow][AnaSysCol] /= Temp;
      if (AnaDebugset) {
	WITH->Mat[AnaSysRow][AnaSysRow] = 1.0;
	WITH->MatNZ[AnaSysRow][AnaSysRow] = true;
      }
    }
  }
  if (!AnaError) {
    for (I = AnaSysRow - 1; I >= 1; I--) {   /*Total of N^2/2 Multiplies*/
      FORLIM1 = AnaSysRow;
      for (J = I + 1; J <= FORLIM1; J++) {
	if (WITH->MatNZ[J][AnaSysCol] && WITH->MatNZ[I][J]) {
	  if (WITH->MatNZ[I][AnaSysCol])
	    WITH->Mat[I]
	      [AnaSysCol] -= WITH->Mat[J][AnaSysCol] * WITH->Mat[I][J];
	  else {
	    WITH->Mat[I]
	      [AnaSysCol] = -WITH->Mat[J][AnaSysCol] * WITH->Mat[I][J];
	    WITH->MatNZ[I][AnaSysCol] = true;
	  }
	}
      }
    }
  }
  if (AnaError)
    AnaEpoch = AnaE_Adapt;
  else
    AnaEpoch = AnaE_Calculate;
}




/* End. */
