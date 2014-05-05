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
/* From input file "main.text" */


/*Version 1.5  Modular*/


#include <p2c/p2c.h>


#define MAIN_G
#include "main.h"


#define newci_findprocedure  findprocedure
extern boolean findprocedure PP((Char *name, Void (**start)()));

Static Void Update PV();


Static Void Clear_Array()
{
  long Row, Col;
  AnaMatRec *WITH;
  long FORLIM, FORLIM1;

  WITH = AnaSystem;
  FORLIM = AnaSysRow;
  for (Row = 0; Row <= FORLIM; Row++) {
    FORLIM1 = AnaSysCol;
    for (Col = 0; Col <= FORLIM1; Col++) {
      WITH->Mat[Row][Col] = 0.0;
      WITH->MatNZ[Row][Col] = false;
    }
  }
}


/*Doneproc**/

Void Analog_Status()
{
  Char Temp[256];

  if (AnaLogglobals->actflag) {
    if (AnaInuse) {
      printf("Analog Status Display\n\n");   /*4*/
      /*5*/
      printf("Analog Circuit State      :\n");   /*6*/
      printf("Simulation State Machine  :\n");   /*7*/
      printf("State of Timestep         :\n");   /*8*/
      printf("Present Timestep          :\n");   /*9*/
      printf("Present Time              :\n");   /*10*/
    } else
      printf("Analog simulator not presently active\n");
    AnaLogglobals->actflag = false;
    return;
  }
  if (!AnaInuse)
    return;
  nc_gotoXY(29, 6);   /*Row 6*/
  switch (AnaChanged) {

  case true:
    if (AnaReady)
      printf("Aborting simulation          ");
    else
      printf("Preparing for new extraction ");
    break;

  case false:
    if (AnaReady) {
      if (AnaStart)
	printf("Doing first simulation pass  ");
      else
	printf("Simulation in progress       ");
    } else
      printf("Circuit not ready to simulate");
    break;
  }
  nc_gotoXY(29, 7);   /*Row 7*/
  if (!AnaReady || AnaChanged)
    printf("                                                   ");
  else {
    switch (AnaEpoch) {

    case AnaE_Evaluate:
      printf("Computing Gates          ");
      break;

    case AnaE_Solve:
      printf("Solving Matrix           ");
      break;

    case AnaE_Calculate:
      printf("Computing New Voltages   ");
      break;

    case AnaE_Refresh:
      printf("Updating New Voltages    ");
      break;

    case AnaE_Adapt:
      printf("Testing For Convergence  ");
      break;

    case AnaE_Memory1:
      printf("Finished the timestep    ");
      break;

    case AnaE_Memory2:
      printf("Updating Capacitors      ");
      break;

    case AnaE_Update:
      printf("Not finished the timestep");
      break;
    }
  }
  nc_gotoXY(29, 8);   /*Row 8*/
  if (!AnaReady || AnaChanged)
    printf("                                                   ");
  else {
    if (AnaError)
      printf("Matrix insolvable            - gearshifting");
    else {
      if (AnaOverflow)
	printf("Solution heading to infinity - gearshifting");
      else {
	if (AnaToobig && AnaSuccess)
	  printf("Solution reached with a big voltage change ");
	else {
	  if (AnaToobig && !AnaSuccess)
	    printf("Voltage change too big       - gearshifting");
	  else {
	    if (AnaSuccess)
	      printf("Finished the timestep                      ");
	    else {
	      if (AnaItercount > AnaItermax)
		printf("Taking too long to converge  - gearshifting");
	      else
		printf("Iteration number             -  %ld            ",
		       AnaItercount);
	    }
	  }
	}
      }
    }
  }
  nc_gotoXY(29, 9);   /*Row 8*/
  if (!AnaReady || AnaChanged)
    printf("                                                  ");
  else {
    (*AnaLogglobals->hook.realunit)(AnaDt, 3, "s", Temp);
    printf("%s              ", Temp);
  }
  nc_gotoXY(29, 10);   /*Row 8*/
  if (!AnaReady || AnaChanged)
    printf("                                                  ");
  else {
    (*AnaLogglobals->hook.realunit)(AnaAccum, 3, "s", Temp);
    printf("%s              ", Temp);
  }
}

/*===========================================================================*/
/*      Procedure To initialize Technology-related constants                 */

Void Analog_InitTechnology()
{

/*===========================================================================*/


  AnaPhysical.T = 298;
  AnaMaxPhysical.T = 1000;
  AnaMinPhysical.T = 1;

  AnaPhysical.k = 1.380658e-23;
  AnaMaxPhysical.k = 1.38e-22;
  AnaMinPhysical.k = 1.38e-24;

  AnaPhysical.q = 1.60217733e-19;
  AnaMaxPhysical.q = 1.602e-18;
  AnaMinPhysical.q = 1.602e-20;

  AnaPhysical.e_v = 8.854187817e-12;
  AnaMaxPhysical.e_v = 8.85e-11;
  AnaMinPhysical.e_v = 8.85e-13;

  AnaPhysical.e_s = 11.7*AnaPhysical.e_v;
  AnaMaxPhysical.e_s = 1.03545e-9;
  AnaMinPhysical.e_s = 1.03545e-11;

  AnaPhysical.e_ox = 3.9*AnaPhysical.e_v;
  AnaMaxPhysical.e_ox = 3.4515e-10;
  AnaMinPhysical.e_ox = 3.4515e-12;

  AnaPhysical.Ut = 0.0256798;        /* T=298  */
  AnaMaxPhysical.Ut = 0.0861423 ;    /* T=1000 */
  AnaMinPhysical.Ut =  8.61423e-05;  /* T=1    */

  AnaGenFab.phi_ms =  -0.3;
  AnaMaxGenFab.phi_ms =  1.0;
  AnaMinGenFab.phi_ms =  -2.0;

  AnaGenFab.Eg =  1.80168e-19;
  AnaMaxGenFab.Eg =  1.87457e-19;
  AnaMinGenFab.Eg =  1.25491e-19;

  AnaGenFab.ni =  8.45211e+09;
  AnaMaxGenFab.ni =  2.2864e+18;
  AnaMinGenFab.ni =  0;

  AnaGenFab.lambda =  1e-6;
  AnaMaxGenFab.lambda =  1e12;
  AnaMinGenFab.lambda =  1e-12;

  strcpy(AnaGenFab.process, "MOS.CNF FILE NOT LOADED");
  strcpy(AnaGenFab.fabrun, "MOS.CNF FILE NOT LOADED");

  AnaNFab.nsign = 1;

  AnaNFab.Tox     =  216e-10;
  AnaMaxNFab.Tox  =  10000e-10;
  AnaMinNFab.Tox  =  1e-10;

  AnaNFab.psi  =  0.6;
  AnaMaxNFab.psi  =  5;
  AnaMinNFab.psi  =  0;

  AnaNFab.Na  =  3.11e16;
  AnaMaxNFab.Na  =  3.11e22;
  AnaMinNFab.Na  =  3.11e10;

  AnaNFab.mu0  =  686.6;
  AnaMaxNFab.mu0  =  1200;
  AnaMinNFab.mu0  =  300;

  AnaNFab.deltaW  =    -0.11e-6; 
  AnaMaxNFab.deltaW  =  1.0 ;
  AnaMinNFab.deltaW  = -1.0 ;

  AnaNFab.deltaL  =  0.55e-6; 
  AnaMaxNFab.deltaL  = 1.0 ;
  AnaMinNFab.deltaL  = -1.0 ;

  AnaNFab.del_NaS  =       0; 
  AnaMaxNFab.del_NaS  =  100;
  AnaMinNFab.del_NaS =  -100;

  AnaNFab.Early_s  =  0.06; 
  AnaMaxNFab.Early_s  =  1;
  AnaMinNFab.Early_s =  0.001;

  AnaNFab.L_0  =  0.04e-6; 
  AnaMaxNFab.L_0  =  1e-6;
  AnaMinNFab.L_0 =  -1e-6;

  AnaNFab.Qss  =  0.00061;
  AnaMaxNFab.Qss  = 10 ;
  AnaMinNFab.Qss  = -10 ;

  AnaNFab.del_Na0  =       1; 
  AnaMaxNFab.del_Na0 =   100;
  AnaMinNFab.del_Na0 =  -100;

  AnaNFab.del_mu  =       1; 
  AnaMaxNFab.del_mu =   100;
  AnaMinNFab.del_mu =  -100;

  AnaNFab.aCactive  =  280e-18;
  AnaMaxNFab.aCactive  =  1000e-18;
  AnaMinNFab.aCactive  =  1e-18;

  AnaNFab.aCwell  =  30e-18;
  AnaMaxNFab.aCwell  =  1000e-18;
  AnaMinNFab.aCwell  =  1e-18;

  AnaNFab.linCgs  =  3e-16;
  AnaMaxNFab.linCgs  =  100e-16;
  AnaMinNFab.linCgs  =  0.1e-16;

  AnaNFab.aCgw  =  10e-18;
  AnaMaxNFab.aCgw  =  1000e-18;
  AnaMinNFab.aCgw =  1e-18;

  AnaNFab.Cox  =  0.159792;
  AnaMaxNFab.Cox  =  0.0034515;
  AnaMinNFab.Cox  =  34.515;

  AnaNFab.mu  =  697.805;
  AnaMaxNFab.mu  =  6.78168e+08;
  AnaMinNFab.mu  =  37.2681;


  AnaPFab.nsign = -1;

  AnaPFab.Tox     =  216e-10;
  AnaMaxPFab.Tox  =  10000e-10;
  AnaMinPFab.Tox  =  1e-10;

  AnaPFab.psi  =  0.6;
  AnaMaxPFab.psi  =  5;
  AnaMinPFab.psi  =  0;

  AnaPFab.Na  =  2.69e16;
  AnaMaxPFab.Na  =  3.11e22;
  AnaMinPFab.Na  =  3.11e10;

  AnaPFab.mu0  =  205;
  AnaMaxPFab.mu0  =  1200;
  AnaMinPFab.mu0  =  300;

  AnaPFab.deltaW  =    -0.11e-6; 
  AnaMaxPFab.deltaW  =  1.0 ;
  AnaMinPFab.deltaW  = -1.0 ;

  AnaPFab.deltaL  =  0.76e-6; 
  AnaMaxPFab.deltaL  = 1.0 ;
  AnaMinPFab.deltaL  = -1.0 ;

  AnaPFab.del_NaS  =       0; 
  AnaMaxPFab.del_NaS  =  100;
  AnaMinPFab.del_NaS =  -100;

  AnaPFab.Early_s  =  0.06; 
  AnaMaxPFab.Early_s  =  1;
  AnaMinPFab.Early_s =  0.001;

  AnaPFab.L_0  =  0.04e-6; 
  AnaMaxPFab.L_0  =  1e-6;
  AnaMinPFab.L_0 =  -1e-6;

  AnaPFab.Qss  =  0.000126;
  AnaMaxPFab.Qss  = 10 ;
  AnaMinPFab.Qss  = -10 ;

  AnaPFab.del_Na0  =       1; 
  AnaMaxPFab.del_Na0 =   100;
  AnaMinPFab.del_Na0 =  -100;

  AnaPFab.del_mu  =       1; 
  AnaMaxPFab.del_mu =   100;
  AnaMinPFab.del_mu =  -100;

  AnaPFab.aCactive  =  280e-18;
  AnaMaxPFab.aCactive  =  1000e-18;
  AnaMinPFab.aCactive  =  1e-18;

  AnaPFab.aCwell  =  30e-18;
  AnaMaxPFab.aCwell  =  1000e-18;
  AnaMinPFab.aCwell  =  1e-18;

  AnaPFab.linCgs  =  3e-16;
  AnaMaxPFab.linCgs  =  100e-16;
  AnaMinPFab.linCgs  =  0.1e-16;

  AnaPFab.aCgw  =  280e-18;
  AnaMaxPFab.aCgw  =  1000e-18;
  AnaMinPFab.aCgw =  1e-18;

  AnaPFab.Cox  =  0.159792;
  AnaMaxPFab.Cox  =  0.0034515;
  AnaMinPFab.Cox  =  34.515;

  AnaPFab.mu  =  208.345;
  AnaMaxPFab.mu  =  2.02482e+08;
  AnaMinPFab.mu  =  11.1272;


}

/*===========================================================================*/
/*              Procedure To initialize constants in program                 */

Void Analog_InitVar()
{

/*===========================================================================*/

  AnaVdd = 5.0;   /*System Power*/
  AnaLimits[AnaN_Vdd - 1][(long)AnaMin].r = AnaVddmin;
  AnaLimits[AnaN_Vdd - 1][(long)AnaMax].r = AnaVddmax;
  AnaDtmax = 1e-4;   /*largest allowable timestep*/
  AnaLimits[AnaN_Dtmax - 1][(long)AnaMin].r = AnaDtmaxmin;
  AnaLimits[AnaN_Dtmax - 1][(long)AnaMax].r = AnaDtmaxmax;
  AnaVstep = 0.1;
  AnaLimits[AnaN_Vstep - 1][(long)AnaMin].r = AnaVstepmin;
  AnaLimits[AnaN_Vstep - 1][(long)AnaMax].r = AnaVstepmax;

  AnaSrange = 10.0;
  AnaLimits[AnaN_Srange - 1][(long)AnaMin].r = AnaSrangemin;
  AnaLimits[AnaN_Srange - 1][(long)AnaMax].r = AnaSrangemax;
  AnaVshift = AnaVstep / AnaSrange;

  AnaHighest = AnaVdd + 2;   /*Highest voltage allowed in simulation*/
  AnaLimits[AnaN_Highest - 1][(long)AnaMin].r = AnaVdd;
  AnaLimits[AnaN_Highest - 1][(long)AnaMax].r = AnaHighestmax;
  AnaLowest = -2.0;   /*Lowest voltage allowed in simulation*/
  AnaLimits[AnaN_Lowest - 1][(long)AnaMin].r = AnaLowestmin;
  AnaLimits[AnaN_Lowest - 1][(long)AnaMax].r = 0.0;
  AnaSmall = 0.1;   /*Guess on a capacitor when Vc=0*/
  AnaLimits[AnaN_Small - 1][(long)AnaMin].r = AnaSmallmin;
  AnaLimits[AnaN_Small - 1][(long)AnaMax].r = AnaSmallmax;


  AnaTol = 0.000001;   /*Maximum percentage difference for convergance*/
  AnaLimits[AnaN_Tol - 1][(long)AnaMin].r = AnaTolmin;
  AnaLimits[AnaN_Tol - 1][(long)AnaMax].r = AnaTolmax;
  AnaEqn_Epsilon = 1e-40;   /*epsilon for equation solver*/
  AnaLimits[AnaN_Eqn_Epsilon - 1][(long)AnaMin].r = AnaEqn_epsilonmin;
  AnaLimits[AnaN_Eqn_Epsilon - 1][(long)AnaMax].r = AnaEqn_epsilonmax;
  AnaVardone_Epsilon = 1e-9;   /*epsilon for eqn zero test*/
  AnaLimits[AnaN_Vardone_Epsilon - 1][(long)AnaMin].r = AnaVardone_epsilonmin;
  AnaLimits[AnaN_Vardone_Epsilon - 1][(long)AnaMax].r = AnaVardone_epsilonmax;


  AnaTimefactor = 2.0;   /*How much to scale timestep */
  AnaLimits[AnaN_Timefactor - 1][(long)AnaMin].r = AnaTimefactormin;
  AnaLimits[AnaN_Timefactor - 1][(long)AnaMax].r = AnaTimefactormax;

  AnaTimemin = 1e-40;   /*lowest allowable timestep */
  AnaLimits[AnaN_Timemin - 1][(long)AnaMin].r = AnaTimeminmin;
  AnaLimits[AnaN_Timemin - 1][(long)AnaMax].r = AnaTimeminmax;

  AnaIterfirstmax = 30;   /*Number of iterations for first try*/
  AnaLimits[AnaN_Iterfirstmax - 1][(long)AnaMin].i = AnaIterfirstmaxmin;
  AnaLimits[AnaN_Iterfirstmax - 1][(long)AnaMax].i = AnaIterfirstmaxmax;

  AnaTimeincrease = 2;   /*Factor to increase maximum interation by*/
  AnaLimits[AnaN_Timeincrease - 1][(long)AnaMin].i = AnaTimeincreasemin;
  AnaLimits[AnaN_Timeincrease - 1][(long)AnaMax].i = AnaTimeincreasemax;


  AnaTimerelax = 1e-20;   /*Minimum time to relax accuracy checking*/
  AnaLimits[AnaN_Timerelax - 1][(long)AnaMin].r = AnaTimerelaxmin;
  AnaLimits[AnaN_Timerelax - 1][(long)AnaMax].r = AnaTimerelaxmax;

  AnaNodecap = 10e-15;   /*Capacitance on each node default*/
  AnaLimits[AnaN_Nodecap - 1][(long)AnaMin].r = AnaNodecapmin;
  AnaLimits[AnaN_Nodecap - 1][(long)AnaMax].r = AnaNodecapmax;

  /*Glow mode levels*/

  AnaLevelmap.Level_a = 0;
  AnaLevelmap.Level_b = 7;
  AnaLevelmap.Level_c = 24;
  AnaLevelmap.Level_d = 41;
  AnaLevelmap.Level_e = 48;
  AnaLevelmap.Level_sum = 50;

  
  AnaChanged = false;
  AnaChangedP = false;
  AnaReady = false;
  AnaInuse = false;
  AnaExact = true;
  AnaErase = true;
  AnaOldmodel = true;
  AnaDimgate = false;
  AnaDebugset = false;
  AnaProbeOn = false;
  AnaDebugsize = 16;
  AnaTimermode = false;
  AnaTimerstate = false;
  AnaTimerval = 0;
  AnaInt_nodelist = NULL;
  AnaSystem = (AnaMatRec *)Malloc(sizeof(AnaMatRec));
  AnaAccum = 0.0;
  AnaClist = NULL;
  AnaKlist = NULL;
  AnaVddsig = NULL;
  Anagndsig = NULL;
  Anagndnode = NULL;
  AnaVddnode = NULL;
  strcpy(AnaLastnodeStr, "$");
  strcpy(AnaLastGate1Str, "$");
  strcpy(AnaLastGate2Str, "$");
  strcpy(AnaLastGate3Str, "$");
  strcpy(AnaLastGate4Str, "$");
  strcpy(AnaLastGate5Str, "$");
  AnaProbeprec = 3;
  AnaHysteresis = 3;
  AnaGateHystercount = 0;
  AnaNodeHystercount = 0;
  AnaTimeReset();
  Analog_InitTechnology();


}


Static Void NewAnalog_SystemInit()
{
  boolean Found;
  Char Arg[256], Keyword[256];

  do {
    AnaGetcommand("SYSTEM", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "VDD")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaVdd, 5.0);
	if (AnaVdd <= AnaVddmin || AnaVdd > AnaVddmax)
	  AnaVdd = 5.0;
      }
      if (!strcmp(Keyword, "DTMAX")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaDtmax, 1e-4);
	if (AnaDtmax <= AnaDtmaxmin || AnaDtmax > AnaDtmaxmax)
	  AnaDtmax = 1e-4;
      }
      if (!strcmp(Keyword, "VSTEP")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaVstep, 0.1);
	if (AnaVstep <= AnaVstepmin || AnaVstep > AnaVstepmax)
	  AnaVstep = 0.1;
	AnaVshift = AnaVstep / AnaSrange;
      }
      if (!strcmp(Keyword, "SRANGE")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaSrange, 10.0);
	if (AnaSrange <= AnaSrangemin || AnaSrange > AnaSrangemax)
	  AnaSrange = 10.0;
	AnaVshift = AnaVstep / AnaSrange;
      }
      if (!strcmp(Keyword, "HIGHEST")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaHighest, AnaVdd + 2);
	if (AnaHighest > AnaHighestmax)
	  AnaHighest = AnaVdd + 2;
      }
      if (!strcmp(Keyword, "LOWEST")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaLowest, -2.0);
	if (AnaLowest < AnaLowestmin || AnaLowest > 0)
	  AnaLowest = -2.0;
      }
      if (!strcmp(Keyword, "SMALL")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaSmall, 0.1);
	if (AnaSmall < AnaSmallmin || AnaSmall > AnaSmallmax)
	  AnaSmall = 0.1;
      }
      if (!strcmp(Keyword, "TOL")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaTol, 0.000001);
	if (AnaTol < AnaTolmin || AnaTol > AnaTolmax)
	  AnaTol = 0.000001;
      }
      if (!strcmp(Keyword, "EQN_EPSILON")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaEqn_Epsilon, 1e-40);
	if (AnaEqn_Epsilon < AnaEqn_epsilonmin ||
	    AnaEqn_Epsilon > AnaEqn_epsilonmax)
	  AnaEqn_Epsilon = 1e-40;
      }
      if (!strcmp(Keyword, "VARDONE_EPSILON")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaVardone_Epsilon, 1e-9);
	if (AnaVardone_Epsilon < AnaVardone_epsilonmin ||
	    AnaVardone_Epsilon > AnaVardone_epsilonmax)
	  AnaVardone_Epsilon = 1e-9;
      }
      if (!strcmp(Keyword, "TIMEFACTOR")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaTimefactor, 2.0);
	if (AnaTimefactor < AnaTimefactormin || AnaTimefactor > AnaTimefactormax)
	  AnaTimefactor = 2.0;
      }
      if (!strcmp(Keyword, "TIMEMIN")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaTimemin, 1e-40);
	if (AnaTimemin < AnaTimeminmin || AnaTimemin > AnaTimeminmax)
	  AnaTimemin = 1e-40;
      }
      if (!strcmp(Keyword, "ITERFIRSTMAX")) {
	(*AnaLogglobals->hook.getint)(Arg, &AnaIterfirstmax, 30L);
	if (AnaIterfirstmax < AnaIterfirstmaxmin ||
	    AnaIterfirstmax > AnaIterfirstmaxmax)
	  AnaIterfirstmax = 30;
      }
      if (!strcmp(Keyword, "TIMEINCREASE")) {
	(*AnaLogglobals->hook.getint)(Arg, &AnaTimeincrease, 2L);
	if (AnaTimeincrease < AnaTimeincreasemin ||
	    AnaTimeincrease > AnaTimeincreasemax)
	  AnaTimeincrease = 2;
      }
      if (!strcmp(Keyword, "TIMERELAX")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaTimerelax, 1e-20);
	if (AnaTimerelax < AnaTimerelaxmin || AnaTimerelax > AnaTimerelaxmax)
	  AnaTimerelax = 1e-20;
      }
      if (!strcmp(Keyword, "NODECAP")) {
	(*AnaLogglobals->hook.getreal)(Arg, &AnaNodecap, 10e-15);
	if (AnaNodecap < AnaNodecapmin || AnaNodecap > AnaNodecapmax)
	  AnaNodecap = 10e-15;
      }
    }
  } while (Found);
  if (AnaHighest < AnaVdd)
    AnaHighest = AnaVdd + 2;
  AnaTimeReset();
}


Void Analog_Systeminit()
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  Char *STR1;

  do {
    AnaGetcommand("SYSTEM", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "VDD")) {
	TRY(try1);
	  AnaVdd = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaVdd <= AnaVddmin || AnaVdd > AnaVddmax)
	    AnaVdd = 5.0;
	RECOVER(try1);
	  AnaVdd = 5.0;
	ENDTRY(try1);
      }
      if (!strcmp(Keyword, "DTMAX")) {
	TRY(try2);
	  AnaDtmax = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaDtmax <= AnaDtmaxmin || AnaDtmax > AnaDtmaxmax)
	    AnaDtmax = 1e-4;
	RECOVER(try2);
	  AnaDtmax = 1e-4;
	ENDTRY(try2);
      }
      if (!strcmp(Keyword, "VSTEP")) {
	TRY(try3);
	  AnaVstep = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaVstep <= AnaVstepmin || AnaVstep > AnaVstepmax)
	    AnaVstep = 0.1;
	  AnaVshift = AnaVstep / AnaSrange;
	RECOVER(try3);
	  AnaVstep = 0.1;
	  AnaVshift = AnaVstep / AnaSrange;
	ENDTRY(try3);
      }
      if (!strcmp(Keyword, "SRANGE")) {
	TRY(try4);
	  AnaSrange = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaSrange <= AnaSrangemin || AnaSrange > AnaSrangemax)
	    AnaSrange = 10.0;
	  AnaVshift = AnaVstep / AnaSrange;
	RECOVER(try4);
	  AnaSrange = 10.0;
	  AnaVshift = AnaVstep / AnaSrange;
	ENDTRY(try4);
      }
      if (!strcmp(Keyword, "HIGHEST")) {
	TRY(try5);
	  AnaHighest = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaHighest > AnaHighestmax)
	    AnaHighest = AnaVdd + 2;
	RECOVER(try5);
	  AnaHighest = AnaVdd + 2;
	ENDTRY(try5);
      }
      if (!strcmp(Keyword, "LOWEST")) {
	TRY(try6);
	  AnaLowest = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaLowest < AnaLowestmin || AnaLowest > 0)
	    AnaLowest = -2.0;
	RECOVER(try6);
	  AnaLowest = -2.0;
	ENDTRY(try6);
      }
      if (!strcmp(Keyword, "SMALL")) {
	TRY(try7);
	  AnaSmall = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaSmall < AnaSmallmin || AnaSmall > AnaSmallmax)
	    AnaSmall = 0.1;
	RECOVER(try7);
	  AnaSmall = 0.1;
	ENDTRY(try7);
      }
      if (!strcmp(Keyword, "TOL")) {
	TRY(try8);
	  AnaTol = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaTol < AnaTolmin || AnaTol > AnaTolmax)
	    AnaTol = 0.000001;
	RECOVER(try8);
	  AnaTol = 0.000001;
	ENDTRY(try8);
      }
      if (!strcmp(Keyword, "EQN_EPSILON")) {
	TRY(try9);
	  AnaEqn_Epsilon = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaEqn_Epsilon < AnaEqn_epsilonmin ||
	      AnaEqn_Epsilon > AnaEqn_epsilonmax)
	    AnaEqn_Epsilon = 1e-40;
	RECOVER(try9);
	  AnaEqn_Epsilon = 1e-40;
	ENDTRY(try9);
      }
      if (!strcmp(Keyword, "VARDONE_EPSILON")) {
	TRY(try10);
	  AnaVardone_Epsilon = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaVardone_Epsilon < AnaVardone_epsilonmin ||
	      AnaVardone_Epsilon > AnaVardone_epsilonmax)
	    AnaVardone_Epsilon = 1e-9;
	RECOVER(try10);
	  AnaVardone_Epsilon = 1e-9;
	ENDTRY(try10);
      }
      if (!strcmp(Keyword, "TIMEFACTOR")) {
	TRY(try11);
	  AnaTimefactor = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaTimefactor < AnaTimefactormin || AnaTimefactor > AnaTimefactormax)
	    AnaTimefactor = 2.0;
	RECOVER(try11);
	  AnaTimefactor = 2.0;
	ENDTRY(try11);
      }
      if (!strcmp(Keyword, "TIMEMIN")) {
	TRY(try12);
	  AnaTimemin = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaTimemin < AnaTimeminmin || AnaTimemin > AnaTimeminmax)
	    AnaTimemin = 1e-40;
	RECOVER(try12);
	  AnaTimemin = 1e-40;
	ENDTRY(try12);
      }
      if (!strcmp(Keyword, "ITERFIRSTMAX")) {
	TRY(try13);
	  AnaIterfirstmax = strtol(Arg, &STR1, 10);
	  Dummy = STR1 - Arg + 1;
	  if (AnaIterfirstmax < AnaIterfirstmaxmin ||
	      AnaIterfirstmax > AnaIterfirstmaxmax)
	    AnaIterfirstmax = 30;
	RECOVER(try13);
	  AnaIterfirstmax = 30;
	ENDTRY(try13);
      }
      if (!strcmp(Keyword, "TIMEINCREASE")) {
	TRY(try14);
	  AnaTimeincrease = strtol(Arg, &STR1, 10);
	  Dummy = STR1 - Arg + 1;
	  if (AnaTimeincrease < AnaTimeincreasemin ||
	      AnaTimeincrease > AnaTimeincreasemax)
	    AnaTimeincrease = 2;
	RECOVER(try14);
	  AnaTimeincrease = 2;
	ENDTRY(try14);
      }
      if (!strcmp(Keyword, "TIMERELAX")) {
	TRY(try15);
	  AnaTimerelax = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaTimerelax < AnaTimerelaxmin || AnaTimerelax > AnaTimerelaxmax)
	    AnaTimerelax = 1e-20;
	RECOVER(try15);
	  AnaTimerelax = 1e-20;
	ENDTRY(try15);
      }
      if (!strcmp(Keyword, "NODECAP")) {
	TRY(try16);
	  AnaNodecap = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNodecap < AnaNodecapmin || AnaNodecap > AnaNodecapmax)
	    AnaNodecap = 10e-15;
	RECOVER(try16);
	  AnaNodecap = 10e-15;
	ENDTRY(try16);
      }
    }
  } while (Found);
  if (AnaHighest < AnaVdd)
    AnaHighest = AnaVdd + 3;
  AnaTimeReset();
}

Void Analog_TechnologyCnf()
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  Char *STR1;

  do {
    AnaGetcommand("PHYSICAL", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "T")) {
	TRY(try1);
	  AnaPhysical.T = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.T <=  AnaMinPhysical.T || 
              AnaPhysical.T >   AnaMaxPhysical.T )
	    AnaPhysical.T = 300;
	RECOVER(try1);
	  AnaPhysical.T = 300;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "K")) {
	TRY(try1);
	  AnaPhysical.k = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.k <=  AnaMinPhysical.k || 
              AnaPhysical.k >   AnaMaxPhysical.k )
	    AnaPhysical.k = 1.380658e-23;
	RECOVER(try1);
	  AnaPhysical.k = 1.380658e-23;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "Q")) {
	TRY(try1);
	  AnaPhysical.q = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.q <=  AnaMinPhysical.q || 
              AnaPhysical.q >   AnaMaxPhysical.q )
	    AnaPhysical.q = 1.60217733e-19;
	RECOVER(try1);
	  AnaPhysical.q = 1.60217733e-19;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "E_V")) {
	TRY(try1);
	  AnaPhysical.e_v = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.e_v <=  AnaMinPhysical.e_v || 
              AnaPhysical.e_v >   AnaMaxPhysical.e_v )
	    AnaPhysical.e_v = 8.854187817e-12;
	RECOVER(try1);
	  AnaPhysical.e_v = 8.854187817e-12;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "E_S")) {
	TRY(try1);
	  AnaPhysical.e_s = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.e_s <=  AnaMinPhysical.e_s || 
              AnaPhysical.e_s >   AnaMaxPhysical.e_s )
	    AnaPhysical.e_s = 11.7*AnaPhysical.e_v;
	RECOVER(try1);
	  AnaPhysical.e_s = 11.7*AnaPhysical.e_v;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "E_OX")) {
	TRY(try1);
	  AnaPhysical.e_ox = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPhysical.e_ox <=  AnaMinPhysical.e_ox || 
              AnaPhysical.e_ox >   AnaMaxPhysical.e_ox )
	    AnaPhysical.e_ox = 3.9*AnaPhysical.e_v;
	RECOVER(try1);
	  AnaPhysical.e_ox = 3.9*AnaPhysical.e_v;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "PHI_MS")) {
	TRY(try1);
	  AnaGenFab.phi_ms = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaGenFab.phi_ms <=  AnaMinGenFab.phi_ms || 
              AnaGenFab.phi_ms >   AnaMaxGenFab.phi_ms )
	    AnaGenFab.phi_ms = -0.3;
	RECOVER(try1);
	  AnaGenFab.phi_ms = -0.3;
	ENDTRY(try1);
      }

    }
  } while (Found);

  do {
    AnaGetcommand("DEVTECHN", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "PROCESS")) {
	TRY(try1);
	  strcpy(AnaGenFab.process, Arg);
	RECOVER(try1);
	  strcpy(AnaGenFab.process, "PROCESS NAME LOADING ERROR");
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "LAMBDA")) {
	TRY(try1);
	  AnaGenFab.lambda = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaGenFab.lambda <=  AnaMinGenFab.lambda || 
              AnaGenFab.lambda >   AnaMaxGenFab.lambda )
	    AnaGenFab.lambda = 1e-6;
	RECOVER(try1);
	  AnaGenFab.lambda = 1e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "TOX")) {
	TRY(try1);
	  AnaNFab.Tox = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.Tox <=  AnaMinNFab.Tox || 
              AnaNFab.Tox >   AnaMaxNFab.Tox )
	    AnaNFab.Tox = 216e-10;
	RECOVER(try1);
	  AnaNFab.Tox = 216e-10;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "PSI")) {
	TRY(try1);
	  AnaNFab.psi = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.psi <=  AnaMinNFab.psi || 
              AnaNFab.psi >   AnaMaxNFab.psi )
	    AnaNFab.psi = 0.6;
	RECOVER(try1);
	  AnaNFab.psi = 0.6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "NA")) {
	TRY(try1);
	  AnaNFab.Na = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.Na <=  AnaMinNFab.Na || 
              AnaNFab.Na >   AnaMaxNFab.Na )
	    AnaNFab.Na = 3.11e16;
	RECOVER(try1);
	  AnaNFab.Na = 3.11e16;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "MU0")) {
	TRY(try1);
	  AnaNFab.mu0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.mu0 <=  AnaMinNFab.mu0 || 
              AnaNFab.mu0 >   AnaMaxNFab.mu0 )
	    AnaNFab.mu0 = 686.6;
	RECOVER(try1);
	  AnaNFab.mu0 = 686.6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DELTAW")) {
	TRY(try1);
	  AnaNFab.deltaW = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.deltaW <=  AnaMinNFab.deltaW || 
              AnaNFab.deltaW >   AnaMaxNFab.deltaW )
	    AnaNFab.deltaW = -0.11e-6;
	RECOVER(try1);
	  AnaNFab.deltaW = -0.11e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DELTAL")) {
	TRY(try1);
	  AnaNFab.deltaL = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.deltaL <=  AnaMinNFab.deltaL || 
              AnaNFab.deltaL >   AnaMaxNFab.deltaL )
	    AnaNFab.deltaL = 0.55e-6;
	RECOVER(try1);
	  AnaNFab.deltaL = 0.55e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_NAS")) {
	TRY(try1);
	  AnaNFab.del_NaS = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.del_NaS <=  AnaMinNFab.del_NaS || 
              AnaNFab.del_NaS >   AnaMaxNFab.del_NaS )
	    AnaNFab.del_NaS = 0;
	RECOVER(try1);
	  AnaNFab.del_NaS = 0;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "EARLY_S")) {
	TRY(try1);
	  AnaNFab.Early_s = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.Early_s <=  AnaMinNFab.Early_s || 
              AnaNFab.Early_s >   AnaMaxNFab.Early_s )
	    AnaNFab.Early_s = 0.06;
	RECOVER(try1);
	  AnaNFab.Early_s = 0.06;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "L_0")) {
	TRY(try1);
	  AnaNFab.L_0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.L_0 <=  AnaMinNFab.L_0 || 
              AnaNFab.L_0 >   AnaMaxNFab.L_0 )
	    AnaNFab.L_0 = 0.04e-6;
	RECOVER(try1);
	  AnaNFab.L_0 = 0.04e-6;
	ENDTRY(try1);
      }
      
      if (!strcmp(Keyword, "ACACTIVE")) {
	TRY(try1);
	  AnaNFab.aCactive = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.aCactive <=  AnaMinNFab.aCactive || 
              AnaNFab.aCactive >   AnaMaxNFab.aCactive )
	    AnaNFab.aCactive = 280e-18;
	RECOVER(try1);
	  AnaNFab.aCactive = 280e-18;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "ACWELL")) {
	TRY(try1);
	  AnaNFab.aCwell = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.aCwell <=  AnaMinNFab.aCwell || 
              AnaNFab.aCwell >   AnaMaxNFab.aCwell )
	    AnaNFab.aCwell = 30e-18;
	RECOVER(try1);
	  AnaNFab.aCwell = 30e-18;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "LINCGS")) {
	TRY(try1);
	  AnaNFab.linCgs = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.linCgs <=  AnaMinNFab.linCgs || 
              AnaNFab.linCgs >   AnaMaxNFab.linCgs )
	    AnaNFab.linCgs = 3e-16;
	RECOVER(try1);
	  AnaNFab.linCgs = 3e-16;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "ACGW")) {
	TRY(try1);
	  AnaNFab.aCgw = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.aCgw <=  AnaMinNFab.aCgw || 
              AnaNFab.aCgw >   AnaMaxNFab.aCgw )
	    AnaNFab.aCgw = 10e-18;
	RECOVER(try1);
	  AnaNFab.aCgw = 10e-18;
	ENDTRY(try1);
      }


    }
  } while (Found);


  do {
    AnaGetcommand("DEVTECHP", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "PROCESS")) {
	TRY(try1);
	  strcpy(AnaGenFab.process, Arg);
	RECOVER(try1);
	  strcpy(AnaGenFab.process, "PROCESS NAME LOADING ERROR");
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "LAMBDA")) {
	TRY(try1);
	  AnaGenFab.lambda = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaGenFab.lambda <=  AnaMinGenFab.lambda || 
              AnaGenFab.lambda >   AnaMaxGenFab.lambda )
	    AnaGenFab.lambda = 1e-6;
	RECOVER(try1);
	  AnaGenFab.lambda = 1e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "TOX")) {
	TRY(try1);
	  AnaPFab.Tox = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.Tox <=  AnaMinPFab.Tox || 
              AnaPFab.Tox >   AnaMaxPFab.Tox )
	    AnaPFab.Tox = 216e-10;
	RECOVER(try1);
	  AnaPFab.Tox = 216e-10;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "PSI")) {
	TRY(try1);
	  AnaPFab.psi = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.psi <=  AnaMinPFab.psi || 
              AnaPFab.psi >   AnaMaxPFab.psi )
	    AnaPFab.psi = 0.6;
	RECOVER(try1);
	  AnaPFab.psi = 0.6;
	ENDTRY(try1);
      }


      if (!strcmp(Keyword, "NA")) {
	TRY(try1);
	  AnaPFab.Na = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.Na <=  AnaMinPFab.Na || 
              AnaPFab.Na >   AnaMaxPFab.Na )
	    AnaPFab.Na = 2.69e16;
	RECOVER(try1);
	  AnaPFab.Na = 2.69e16;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "MU0")) {
	TRY(try1);
	  AnaPFab.mu0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.mu0 <=  AnaMinPFab.mu0 || 
              AnaPFab.mu0 >   AnaMaxPFab.mu0 )
	    AnaPFab.mu0 = 205;
	RECOVER(try1);
	  AnaPFab.mu0 = 205;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DELTAW")) {
	TRY(try1);
	  AnaPFab.deltaW = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.deltaW <=  AnaMinPFab.deltaW || 
              AnaPFab.deltaW >   AnaMaxPFab.deltaW )
	    AnaPFab.deltaW = -0.11e-6;
	RECOVER(try1);
	  AnaPFab.deltaW = -0.11e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DELTAL")) {
	TRY(try1);
	  AnaPFab.deltaL = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.deltaL <=  AnaMinPFab.deltaL || 
              AnaPFab.deltaL >   AnaMaxPFab.deltaL )
	    AnaPFab.deltaL = 0.76e-6;
	RECOVER(try1);
	  AnaPFab.deltaL = 0.76e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_NAS")) {
	TRY(try1);
	  AnaPFab.del_NaS = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.del_NaS <=  AnaMinPFab.del_NaS || 
              AnaPFab.del_NaS >   AnaMaxPFab.del_NaS )
	    AnaPFab.del_NaS = 0;
	RECOVER(try1);
	  AnaPFab.del_NaS = 0;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "EARLY_S")) {
	TRY(try1);
	  AnaPFab.Early_s = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.Early_s <=  AnaMinPFab.Early_s || 
              AnaPFab.Early_s >   AnaMaxPFab.Early_s )
	    AnaPFab.Early_s = 0.06;
	RECOVER(try1);
	  AnaPFab.Early_s = 0.06;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "L_0")) {
	TRY(try1);
	  AnaPFab.L_0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.L_0 <=  AnaMinPFab.L_0 || 
              AnaPFab.L_0 >   AnaMaxPFab.L_0 )
	    AnaPFab.L_0 = 0.04e-6;
	RECOVER(try1);
	  AnaPFab.L_0 = 0.04e-6;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "ACACTIVE")) {
	TRY(try1);
	  AnaPFab.aCactive = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.aCactive <=  AnaMinPFab.aCactive || 
              AnaPFab.aCactive >   AnaMaxPFab.aCactive )
	    AnaPFab.aCactive = 280e-18;
	RECOVER(try1);
	  AnaPFab.aCactive = 280e-18;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "ACWELL")) {
	TRY(try1);
	  AnaPFab.aCwell = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.aCwell <=  AnaMinPFab.aCwell || 
              AnaPFab.aCwell >   AnaMaxPFab.aCwell )
	    AnaPFab.aCwell = 30e-18;
	RECOVER(try1);
	  AnaPFab.aCwell = 280e-18;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "LINCGS")) {
	TRY(try1);
	  AnaPFab.linCgs = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.linCgs <=  AnaMinPFab.linCgs || 
              AnaPFab.linCgs >   AnaMaxPFab.linCgs )
	    AnaPFab.linCgs = 3e-16;
	RECOVER(try1);
	  AnaPFab.linCgs = 3e-16;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "ACGW")) {
	TRY(try1);
	  AnaPFab.aCgw = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.aCgw <=  AnaMinPFab.aCgw || 
              AnaPFab.aCgw >   AnaMaxPFab.aCgw )
	    AnaPFab.aCgw = 280e-18;
	RECOVER(try1);
	  AnaPFab.aCgw = 280e-18;
	ENDTRY(try1);
      }


    }
  } while (Found);

  do {
    AnaGetcommand("RUNSPECN", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "FABRUN")) {
	TRY(try1);
	  strcpy(AnaGenFab.fabrun, Arg);
	RECOVER(try1);
	  strcpy(AnaGenFab.fabrun, "FAB RUN NAME LOADING ERROR");
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "QSS")) {
	TRY(try1);
	  AnaNFab.Qss = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.Qss <=  AnaMinNFab.Qss || 
              AnaNFab.Qss >   AnaMaxNFab.Qss )
	    AnaNFab.Qss = 0.00061;
	RECOVER(try1);
	  AnaNFab.Qss = 0.00061;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_NA0")) {
	TRY(try1);
	  AnaNFab.del_Na0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.del_Na0 <=  AnaMinNFab.del_Na0 || 
              AnaNFab.del_Na0 >   AnaMaxNFab.del_Na0 )
	    AnaNFab.del_Na0 = 1;
	RECOVER(try1);
	  AnaNFab.del_Na0 = 1;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_MU")) {
	TRY(try1);
	  AnaNFab.del_mu = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaNFab.del_mu <=  AnaMinNFab.del_mu || 
              AnaNFab.del_mu >   AnaMaxNFab.del_mu )
	    AnaNFab.del_mu = 1;
	RECOVER(try1);
	  AnaNFab.del_mu = 1;
	ENDTRY(try1);
      }

    }
  } while (Found);


  do {
    AnaGetcommand("RUNSPECP", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);

      if (!strcmp(Keyword, "FABRUN")) {
	TRY(try1);
	  strcpy(AnaGenFab.fabrun, Arg);
	RECOVER(try1);
	  strcpy(AnaGenFab.fabrun, "FAB RUN NAME LOADING ERROR");
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "QSS")) {
	TRY(try1);
	  AnaPFab.Qss = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.Qss <=  AnaMinPFab.Qss || 
              AnaPFab.Qss >   AnaMaxPFab.Qss )
	    AnaPFab.Qss = 0.000126;
	RECOVER(try1);
	  AnaPFab.Qss = 0.000126;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_NA0")) {
	TRY(try1);
	  AnaPFab.del_Na0 = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.del_Na0 <=  AnaMinPFab.del_Na0 || 
              AnaPFab.del_Na0 >   AnaMaxPFab.del_Na0 )
	    AnaPFab.del_Na0 = 0.835;
	RECOVER(try1);
	  AnaPFab.del_Na0 = 0.835;
	ENDTRY(try1);
      }

      if (!strcmp(Keyword, "DEL_MU")) {
	TRY(try1);
	  AnaPFab.del_mu = strtod(Arg, &STR1);
	  Dummy = STR1 - Arg + 1;
	  if (AnaPFab.del_mu <=  AnaMinPFab.del_mu || 
              AnaPFab.del_mu >   AnaMaxPFab.del_mu )
	    AnaPFab.del_mu = 1;
	RECOVER(try1);
	  AnaPFab.del_mu = 1;
	ENDTRY(try1);
      }

    }
  } while (Found);

  AnaParamUpdate();

}


/*==============================================================================*/
/*         Procedure To Reset The Simulator To Initial Conditions               */
/*==============================================================================*/


Void Analog_Reset()
{
  log_nrec *NCount;
  AnaExt_rec *A_Node;
  AnaInt_rec *IntCount;
  Analog_32_action Act;
  AnaCell_rec *A_Kind;
  log_grec *GCount;
  long Pages, Numpages;
  log_grec *WITH;

  NCount = AnaLogglobals->nbase;
  while (NCount != NULL) {
    if (NCount->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)NCount->info;
      A_Node->ltimestep = AnaNotyet;
      A_Node->col_ltimestep = AnaColormap.C_Glow1;
      NCount->changed = true;
    }
    NCount = NCount->next;
  }
  IntCount = AnaInt_nodelist;
  while (IntCount != NULL) {
    IntCount->ltimestep = AnaNotyet;
    IntCount = IntCount->next;
  }
  Numpages = AnaLogglobals->numpages;
  Pages = 1;
  while (Pages <= Numpages) {
    GCount = AnaLogglobals->gbase[Pages - 1];
    while (GCount != NULL) {
      WITH = GCount;
      if (WITH->kind->simtype->simtype == 32) {
	Act.action = Analog_act_reset;
	Act.page = Pages;
	Act.inst = GCount;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	(*A_Kind->proc_cell)(&Act);
      }
      GCount = GCount->next;
    }
    Pages++;
  }
  AnaTimeReset();
  AnaAccum = 0.0;
  AnaChanged = true;
  AnaReady = true;
  if (AnaLogglobals->showpage > 0)
    (*AnaLogglobals->hook.refresh)();
}


/********************************************************************************/
/*            Procedure For updating the screen appropiately                    */

Void Analog_Refresh()
{

  /********************************************************************************/
  Analog_32_action Act;
  AnaCell_rec *A_Kind;
  log_grec *GCount;
  long Showpage;
  log_grec *WITH;

  Showpage = AnaLogglobals->showpage;
  if (Showpage <= 0)
    return;
  GCount = AnaLogglobals->gbase[Showpage - 1];
  while (GCount != NULL) {
    WITH = GCount;
    if (WITH->kind->simtype->simtype == 32) {
      Act.action = Analog_act_refresh;
      Act.inst = GCount;
      A_Kind = (AnaCell_rec *)WITH->kind->info;
      (*A_Kind->proc_cell)(&Act);
    }
    GCount = GCount->next;
  }
}


/*==============================================================================*/
/*         Procedure To Find A New Kind of Gate In  Symbol Table                */
/*==============================================================================*/

Void Analog_Newkind(newkindptr)
log_krec *newkindptr;
{
  Char Procname[256];
  long Index;
  Void (*Initproc) PP((Analog_32_action *act));
  Analog_32_action Act;
  AnaCell_rec *Temp;
  AnaKlistnode *NewKlist;

  *Procname = '\0';
  Index = 1;
  while (Index <= 255 && newkindptr->proc[Index - 1] != '\0') {
    sprintf(Procname + strlen(Procname), "%c", newkindptr->proc[Index - 1]);
    Index++;
  }
  if (!newci_findprocedure(Procname, (Void(**) PV())(&Initproc))) {
    AnaDummyInit(newkindptr);
    return;
  }
  Act.action = Analog_act_newkind;
  Act.kind = newkindptr;
  (*Initproc)(&Act);
  Temp = (AnaCell_rec *)newkindptr->info;
  Temp->proc_cell = Initproc;
  newkindptr->info = (Anyptr)Temp;
  NewKlist = (AnaKlistnode *)Malloc(sizeof(AnaKlistnode));
  NewKlist->NodeKind = newkindptr;
  NewKlist->Nextnode = AnaKlist;
  AnaKlist = NewKlist;
}


/*==============================================================================*/
/*         Section of Program Which Prepares a New Simulation                   */
/*==============================================================================*/

/********************************************************************************/
/* Procedure To Initialize the Reference Counting Fields of Nodes               */

Static Void Nodelist_init()
{

  /********************************************************************************/
  log_nrec *Count;
  AnaExt_rec *A_Node;

  Count = AnaLogglobals->nbase;
  while (Count != NULL) {
    if (Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Count->info;
      A_Node->Ok_Alone = true;
      A_Node->refcnt = 0;
    }
    Count = Count->next;
  }
}


/********************************************************************************/
/*              Procedure To Set Reference Counts Of All Nodes                  */

Static Void Nodelist_refcnt(Inuse, Ok)
boolean *Inuse, *Ok;
{

  /********************************************************************************/
  AnaExt_rec *A_Node;
  AnaCell_rec *A_Kind;
  long Index;
  log_grec *Count;
  long Pages, Numpages;
  Anainstlist *A_Gate;
  log_grec *WITH;
  Char STR1[26];
  long FORLIM1;

  *Inuse = false;
  *Ok = true;
  Numpages = AnaLogglobals->numpages;
  for (Pages = 0; Pages < Numpages; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Gate = (Anainstlist *)Count->info;
	A_Gate->NotReady = false;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	if (!A_Kind->simulatable) {
	  *Ok = false;
	  A_Gate->NotReady = true;
	  sprintf(STR1, "%s doesn't simulate", WITH->kind->name);
	  AnaScoreboard(STR1, (long)AnaMessScoreboard);
	} else {
	  FORLIM1 = WITH->kind->numpins;
	  for (Index = 0; Index < FORLIM1; Index++) {
	    if (A_Kind->plist_cell[Index].active &&
		WITH->pin[Index]->simtype->simtype == 32) {
	      *Inuse = true;
	      A_Node = (AnaExt_rec *)WITH->pin[Index]->info;
	      A_Node->refcnt++;
	      A_Node->Ok_Alone = (A_Node->Ok_Alone &&
				  A_Kind->plist_cell[Index].standalone);
	    }
	  }
	}
      }
      Count = Count->next;
    }
  }
}


/********************************************************************************/
/*              Procedure To Report Singly Connected Node                       */

Static Void Single_Find(display)
boolean display;
{

  /********************************************************************************/
  AnaExt_rec *A_Node;
  AnaCell_rec *A_Kind;
  long Index;
  log_grec *Count;
  long Pages, Numpages;
  Anainstlist *A_Gate;
  boolean FirstFound, Found;
  log_grec *WITH;
  Char STR1[24];

  Numpages = AnaLogglobals->numpages;
  FirstFound = true;
  for (Pages = 0; Pages < Numpages; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Kind = (AnaCell_rec *)Count->kind->info;
	Index = 1;
	Found = false;
	while (Index <= WITH->kind->numpins && !Found &&
	       A_Kind->plist_cell[Index - 1].active) {
	  if (WITH->pin[Index - 1]->simtype->simtype == 32 &&
	      WITH->pin[Index - 1] != Anagndnode &&
	      WITH->pin[Index - 1] != AnaVddnode) {
	    A_Node = (AnaExt_rec *)WITH->pin[Index - 1]->info;
	    if (A_Node->refcnt == 1 && !A_Node->Ok_Alone) {
	      Found = true;
	      A_Gate = (Anainstlist *)Count->info;
	      A_Gate->NotReady = true;
	      if (FirstFound && display) {
		sprintf(STR1, "%s is unconnected", WITH->kind->name);
		AnaScoreboard(STR1, (long)AnaMessScoreboard);
		FirstFound = false;
	      }
	    }
	  }
	  Index++;
	}
      }
      Count = Count->next;
    }
  }
}


/********************************************************************************/
/*        Procedure To Check Reference Counts For a Proper Net                  */

Static Void Nodelist_Check(Ok, Totnodes)
boolean *Ok;
long *Totnodes;
{

  /********************************************************************************/
  log_nrec *Count;
  AnaExt_rec *A_Node;

  Count = AnaLogglobals->nbase;
  *Ok = true;
  while (Count != NULL) {   /*And Ok*/
    if (Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Count->info;
      if (Count == Anagndnode || Count == AnaVddnode) {
	if (Count == Anagndnode && Count == AnaVddnode) {
	  *Ok = false;
	  AnaScoreboard("Vdd and Gnd connected together",
			(long)AnaMessScoreboard);
	} else {
	  A_Node->nodenum = 0;
	  if (Count == Anagndnode) {
	    A_Node->ltimestep = 0.0;
	    A_Node->col_ltimestep = AnaColormap.C_Glow1;
	    A_Node->col_now = AnaColormap.C_Glow1;
	  }
	  if (Count == AnaVddnode) {
	    A_Node->ltimestep = AnaVdd;
	    A_Node->col_ltimestep = AnaColormap.C_Glow6;
	    A_Node->col_now = AnaColormap.C_Glow6;
	  }
	  Count->changed = true;
	}
      } else {
	if (A_Node->refcnt > 1 || A_Node->refcnt == 1 && A_Node->Ok_Alone) {
	  (*Totnodes)++;
	  A_Node->nodenum = *Totnodes;
	} else {
	  if (A_Node->refcnt == 1)
	    *Ok = false;
	  else {  /*Refcnt=0*/
	    A_Node->nodenum = 0;
	    A_Node->ltimestep = 0.0;
	    A_Node->col_ltimestep = AnaColormap.C_Glow1;
	    A_Node->col_now = AnaColormap.C_Glow1;
	    Count->changed = true;
	  }
	}
      }
    }
    Count = Count->next;
  }
}


/******************************************************************************/
/*        Procedure for setting dimmed status on all gates                    */

Static Void Dimcheck()
{

  /******************************************************************************/
  log_grec *Count;
  long Pages, Numpages;
  Anainstlist *A_Gate;
  long FORLIM;
  log_grec *WITH;

  Numpages = AnaLogglobals->numpages;
  FORLIM = Numpages;
  for (Pages = 0; Pages < FORLIM; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Gate = (Anainstlist *)Count->info;
	(*AnaLogglobals->hook.setdimgate)(Count, A_Gate->NotReady);
      }
      Count = Count->next;
    }
  }
}


/******************************************************************************/
/*        Procedure for resetting dimmed status on all gates                  */

Static Void UnDim()
{

  /******************************************************************************/
  log_grec *Count;
  long Pages, Numpages;
  Anainstlist *A_Gate;
  long FORLIM;
  log_grec *WITH;

  Numpages = AnaLogglobals->numpages;
  FORLIM = Numpages;
  for (Pages = 0; Pages < FORLIM; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Gate = (Anainstlist *)Count->info;
	(*AnaLogglobals->hook.setdimgate)(Count, false);
      }
      Count = Count->next;
    }
  }
}


/********************************************************************************/
/*         Procedure To Check For a Simulatable Net                             */

Static Void Analog_Syntax_Check(Ok, Totnodes)
boolean *Ok;
long *Totnodes;
{

  /*  Ok: Set if ok to simulate net;
      Totnodes: Total number of external nodes, already numbered
      Int_Exists: Set if Any Gate has Internal Nodes                             */
  /********************************************************************************/
  Nodelist_init();
  Nodelist_refcnt(&AnaInuse, Ok);
  *Totnodes = 0;
  if (*Ok) {
    if (AnaInuse) {
      Nodelist_Check(Ok, Totnodes);
      if (*Ok) {
	if (*Totnodes == 0) {
	  *Ok = false;
	  AnaScoreboard("No nodes unconstrained", (long)AnaMessScoreboard);
	}
	if (*Totnodes > AnaRowbound) {
	  *Ok = false;
	  AnaScoreboard("Too many nodes", (long)AnaMessScoreboard);
	  (*AnaLogglobals->hook.vmessage)("Circuit too large for anaLOG");
	}
	Single_Find(false);
      } else
	Single_Find(true);
      AnaLogglobals->acttool->nexttstep = 0.0;
    } else {
      *Ok = false;
      AnaScoreboard("$", (long)AnaMessScoreboard);
      Single_Find(false);
    }
  } else
    Single_Find(false);
  if (AnaDimgate)
    Dimcheck();
}


/********************************************************************************/
/*  Procedure To Allocate Internal Node List - Updates Totnodes Appropiately    */

Static Void Internal_Number(Totnodes)
long *Totnodes;
{

  /********************************************************************************/
  AnaInt_rec *Count;


  Count = AnaInt_nodelist;
  while (Count != NULL) {
    (*Totnodes)++;
    Count->nodenum = *Totnodes;
    Count = Count->next;
  }
}


/********************************************************************************/
/* Procedure To Allocate The correct sized array For Simulation                 */

Static Void Analog_Allocate(Totnodes)
long Totnodes;
{

  /********************************************************************************/
  AnaSysRow = Totnodes;
  AnaSysCol = Totnodes + 1;
  Clear_Array();
}


/******************************************************************************/
/*         Returns true if any guesses are still needed                       */
/******************************************************************************/

Static boolean Ext_Anynil()
{
  boolean Result;
  log_nrec *Count;
  AnaExt_rec *A_Node;

  Result = false;
  Count = AnaLogglobals->nbase;
  while (Count != NULL) {
    if (Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Count->info;
      if (A_Node->ltimestep == AnaNotyet && A_Node->refcnt != 0)
	Result = true;
    }
    Count = Count->next;
  }
  return Result;
}


/******************************************************************************/
/*         Returns true if any guesses are still needed                       */
/******************************************************************************/

Static boolean Anynil()
{
  log_nrec *Count;
  AnaExt_rec *A_Node;
  boolean Onenil;
  AnaInt_rec *IntCount;

  Onenil = false;
  Count = AnaLogglobals->nbase;
  while (Count != NULL && !Onenil) {
    if (Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Count->info;
      if (A_Node->ltimestep == AnaNotyet && A_Node->refcnt != 0)
	Onenil = true;
    }
    Count = Count->next;
  }
  IntCount = AnaInt_nodelist;
  while (!Onenil && IntCount != NULL) {
    if (IntCount->ltimestep == AnaNotyet)
      Onenil = true;
    IntCount = IntCount->next;
  }
  return Onenil;
}




Static Void Analog_Guess()
{
  log_grec *Count;
  long Pages, Numpages;
  AnaCell_rec *A_Kind;
  Analog_32_action Act;
  log_grec *WITH;

  Numpages = AnaLogglobals->numpages;
  Pages = 1;
  while (Pages <= Numpages) {
    Count = AnaLogglobals->gbase[Pages - 1];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	Act.action = Analog_act_pass1;
	Act.page = Pages;
	Act.inst = Count;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	(*A_Kind->proc_cell)(&Act);
      }
      Count = Count->next;
    }
    Pages++;
  }
  Pages = 1;
  while (Pages <= Numpages) {
    Count = AnaLogglobals->gbase[Pages - 1];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	Act.action = Analog_act_pass2;
	Act.page = Pages;
	Act.inst = Count;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	(*A_Kind->proc_cell)(&Act);
      }
      Count = Count->next;
    }
    Pages++;
  }
  if (Anynil())
    _Escape(10);
}


/********************************************************************************/
/*         Procedure for fixing simulation constants                            */

Static Void Timestep()
{

  /********************************************************************************/
  AnaStart = true;
  AnaChangedDt = true;
  AnaTimeinit();
  AnaEpoch = AnaE_Update;
  AnaTimestate = Anasettime;
  AnaToomany = false;
  Update();
}


/********************************************************************************/
/*                  Main procedure for setup of simulation                      */

Void Analog_Setup()
{

  /********************************************************************************/
  long Totnodes;

  Analog_Syntax_Check(&AnaReady, &Totnodes);
  if (AnaReady) {
    Internal_Number(&Totnodes);
    Analog_Allocate(Totnodes);
    Analog_Guess();
    Timestep();
    AnaScoreboard("Starting A Simulation", (long)AnaMessScoreboard);
  }
  AnaChanged = false;
}



/*==============================================================================*/
/*         Procedures To Abort A Simulation Gracefully                           */
/*==============================================================================*/

/********************************************************************************/
/*           Main procedure to abort simulation gracefully                      */

Void Analog_Cleanup()
{

  /********************************************************************************/
  AnaChanged = true;
  AnaReady = false;
  AnaScoreboard("Halting simulation", (long)AnaMessScoreboard);
}


/*==============================================================================*/
/*             Procedures To Implement Keyboard Commands                        */
/*==============================================================================*/



Void Analog_GlobalCommand(Found, Accepted)
boolean *Found, *Accepted;
{   /*Func is command, Funcarg^ is argument*/
  long Temp;
  log_action *WITH;
  Char STR2[256];


  WITH = AnaLogglobals;
  *Found = false;
  *Accepted = false;
  if (!strcmp(WITH->func, "SET")) {
    *Found = true;
    *Accepted = true;
    AnaErase = false;
    AnaAnalog_ChangeReset(AnaErase);
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Setting reset memory");
    return;
  }
  if (!strcmp(WITH->func, "ERASE")) {
    *Found = true;
    *Accepted = true;
    AnaErase = true;
    AnaAnalog_ChangeReset(AnaErase);
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Erasing reset memory");
    return;
  }
  if (!strcmp(WITH->func, "EXACT")) {
    *Found = true;
    *Accepted = true;
    AnaExact = true;
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Restoring accuracy");
    return;
  }
  if (!strcmp(WITH->func, "RELAXED")) {
    *Found = true;
    *Accepted = true;
    AnaExact = false;
    AnaScoreboard("#", (long)AnaMessFlags);
    (*AnaLogglobals->hook.vmessage)("Relaxing accuracy");
    return;
  }
  if (!strcmp(WITH->func, "OLDMODEL")) {
    *Found = true;
    *Accepted = true;
    AnaOldmodel = true;
    (*AnaLogglobals->hook.vmessage)("Loads will use saved model parameters");
    return;
  }
  if (!strcmp(WITH->func, "NEWMODEL")) {
    *Found = true;
    *Accepted = true;
    AnaOldmodel = false;
    (*AnaLogglobals->hook.vmessage)("Loads will use present model parameters");
    return;
  }
  if (!strcmp(WITH->func, "DEBUG")) {
    *Found = true;
    *Accepted = true;
    (*AnaLogglobals->hook.getbool)(WITH->funcarg, &AnaDebugset);
    if (AnaDebugset)
      (*AnaLogglobals->hook.vmessage)("Debug mode on");
    else
      (*AnaLogglobals->hook.vmessage)("Debug mode off");
    return;
  }
  if (!strcmp(WITH->func, "TIMER")) {
    *Found = true;
    *Accepted = true;
    (*AnaLogglobals->hook.getbool)(WITH->funcarg, &AnaTimermode);
    if (AnaTimermode)
      (*AnaLogglobals->hook.vmessage)("Timer on");
    else
      (*AnaLogglobals->hook.vmessage)("Timer off");
    AnaTimerstate = false;
    AnaTimerval = 0;
    return;
  }
  if (!strcmp(WITH->func, "DEBUGSIZE")) {
    *Found = true;
    *Accepted = true;
    (*AnaLogglobals->hook.getint)(WITH->funcarg, &AnaDebugsize, AnaDebugsize);
    sprintf(STR2, "Debug size is %ld", AnaDebugsize);
    (*AnaLogglobals->hook.vmessage)(STR2);
    return;
  }
  if (!strcmp(WITH->func, "DIM")) {
    *Found = true;
    *Accepted = true;
    (*AnaLogglobals->hook.getbool)(WITH->funcarg, &AnaDimgate);
    if (AnaDimgate) {
      (*AnaLogglobals->hook.vmessage)("Unconnected gates are dimmed");
      AnaChanged = true;
    } else {
      (*AnaLogglobals->hook.vmessage)("Unconnected gates are not dimmed");
      UnDim();
    }
    return;
  }
  if (!strcmp(WITH->func, "PROBEPREC")) {
    *Found = true;
    *Accepted = true;
    (*AnaLogglobals->hook.getint)(WITH->funcarg, &Temp, AnaProbeprec);
    if (Temp < 1)
      *Found = false;
    else
      AnaProbeprec = Temp;
    sprintf(STR2, "Probe precision is %ld", AnaProbeprec);
    (*AnaLogglobals->hook.vmessage)(STR2);
    return;
  }
  if (strcmp(WITH->func, "HYSTERESIS"))
    return;
  *Found = true;
  *Accepted = true;
  (*AnaLogglobals->hook.getint)(WITH->funcarg, &AnaHysteresis, AnaHysteresis);
  sprintf(STR2, "Hysteresis is %ld", AnaHysteresis);
  (*AnaLogglobals->hook.vmessage)(STR2);
}


Void Analog_Probemode()
{
  AnaExt_rec *Probenode;
  double Value;
  Char Outstr[256];
  Analog_32_action Act;
  AnaCell_rec *A_Kind;

  if (AnaLogglobals->probenode == NULL) {
    if (AnaNodeHystercount <= AnaHysteresis)
      AnaNodeHystercount++;
    if (AnaNodeHystercount > AnaHysteresis)
      AnaScoreboard("$", (long)AnaMessNode);
  } else {
    if (AnaLogglobals->probenode->simtype->simtype == 32) {
      AnaNodeHystercount = 0;
      Probenode = (AnaExt_rec *)AnaLogglobals->probenode->info;
      Value = Probenode->ltimestep;
      if (Value == AnaNotyet)
	strcpy(Outstr, "Undefined voltage");
      else
	AnaProbeVoltStr(Outstr, Value);
      AnaScoreboard(Outstr, (long)AnaMessNode);
    } else {
      if (AnaLogglobals->probenode->simtype->simtype == 33) {
	AnaNodeHystercount = 0;
	Probenode = (AnaExt_rec *)AnaLogglobals->probenode->info;
	Value = Probenode->ltimestep;
	if (Value == AnaNotyet)
	  strcpy(Outstr, "Undefined current");
	else
	  AnaProbeAmpStr(Outstr, Value);
	AnaScoreboard(Outstr, (long)AnaMessNode);
      } else {
	AnaNodeHystercount = 0;
	AnaScoreboard("Not an anaLOG node ", (long)AnaMessNode);
      }
    }
  }
  if (AnaLogglobals->probegate == NULL) {
    if (AnaGateHystercount <= AnaHysteresis)
      AnaGateHystercount++;
    if (AnaGateHystercount <= AnaHysteresis)
      return;
    AnaScoreboard("$", (long)AnaMessGate1);
    AnaScoreboard("$", (long)AnaMessGate2);
    AnaScoreboard("$", (long)AnaMessGate3);
    AnaScoreboard("$", (long)AnaMessGate4);
    AnaScoreboard("$", (long)AnaMessGate5);
    return;
  }
  if (AnaLogglobals->probegate->kind->simtype->simtype == 32) {
    AnaGateHystercount = 0;
    Act.action = Analog_act_probe;
    Act.inst = AnaLogglobals->probegate;
    A_Kind = (AnaCell_rec *)AnaLogglobals->probegate->kind->info;
    (*A_Kind->proc_cell)(&Act);
    return;
  }
  AnaGateHystercount = 0;
  AnaScoreboard("Not an anaLOG gate", (long)AnaMessGate1);
  AnaScoreboard("$", (long)AnaMessGate2);
  AnaScoreboard("$", (long)AnaMessGate3);
  AnaScoreboard("$", (long)AnaMessGate4);
  AnaScoreboard("$", (long)AnaMessGate5);
}


/*==============================================================================*/
/*                      Procedures To Do Simulation                             */
/*==============================================================================*/


Static Void Update()
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;

  if (AnaEpoch != AnaE_Update || AnaQuit())
    return;
  Ext_Count = AnaLogglobals->nbase;
  while (Ext_Count != NULL) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      A_Node->last = A_Node->ltimestep;
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL) {
    Int_Count->last = Int_Count->ltimestep;
    Int_Count = Int_Count->next;
  }
  AnaItercount = 0;
  AnaEpoch = AnaE_Evaluate;
  AnaLastgate = NULL;
  AnaLastpage = 0;
  AnaToomany = false;
}


/*Doneproc**/
Static Void Refresh()
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;
  boolean Glowset;
  double Glowscale;

  if (AnaEpoch != AnaE_Refresh || AnaQuit())
    return;
  Glowset = (AnaDone && (!AnaToobig || AnaDtcount <= AnaRelaxcount ||
			 !AnaExact || AnaDifficult));
  if (Glowset)
    Glowscale = AnaLevelmap.Level_sum / AnaVdd;
  Ext_Count = AnaLogglobals->nbase;
  while (Ext_Count != NULL) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      A_Node->last = A_Node->now;
      if (Glowset) {
	A_Node->col_now = AnaGlowVal((long)(A_Node->now * Glowscale));
	A_Node->changed_now = (A_Node->col_now != A_Node->col_ltimestep);
      }
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL) {
    Int_Count->last = Int_Count->now;
    Int_Count = Int_Count->next;
  }
  if (AnaOverflow || AnaDone || AnaItercount > AnaItermax) {
    AnaToomany = (AnaItercount > AnaItermax);
    AnaEpoch = AnaE_Adapt;
  } else {
    AnaEpoch = AnaE_Evaluate;
    AnaLastgate = NULL;
    AnaLastpage = 0;
  }
  AnaItercount++;
}


Static Void Evaluate()
{
  log_grec *Count;
  long Pages, Numpages;
  AnaCell_rec *A_Kind;
  Analog_32_action Act;
  boolean Timeout;
  log_grec *WITH;

  Numpages = AnaLogglobals->numpages;
  if (AnaEpoch != AnaE_Evaluate || AnaQuit())
    return;
  if (AnaLastpage == 0) {
    Clear_Array();
    AnaLastpage = 1;
  }
  Pages = AnaLastpage;
  Timeout = false;
  while (Pages <= Numpages && !Timeout) {
    if (AnaLastgate == NULL)
      AnaLastgate = AnaLogglobals->gbase[Pages - 1];
    Count = AnaLastgate;
    while (Count != NULL && !Timeout) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	Act.action = Analog_act_ex;
	Act.inst = Count;
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	(*A_Kind->proc_cell)(&Act);
      }
      Count = Count->next;
      AnaLastgate = Count;
      Timeout = AnaQuit();
      if (Timeout) {
	if (Count == NULL) {
	  Pages++;
	  AnaLastpage = Pages;
	} else
	  AnaLastpage = Pages;
      }
    }
    if (!Timeout)
      Pages++;
  }
  if (Pages <= Numpages)
    return;
  AnaEpoch = AnaE_Solve;
  AnaS_Section = AnaS_Start;
  AnaChangedDt = false;
  AnaChangedP  = false;
  if (AnaDebugset)
    AnaMatdump();
}


Static boolean Vardone(Oldvar, NewVar)
double Oldvar, NewVar;
{
  if (fabs(Oldvar) > AnaVardone_Epsilon)
    return (fabs(1 - NewVar / Oldvar) < AnaTol);
  else {
    if (fabs(NewVar) > AnaVardone_Epsilon)
      return (fabs(1 - Oldvar / NewVar) < AnaTol);
    else
      return true;
  }
}


/*Doneproc**/
Static Void Calculate(Overflow, Done)
boolean *Overflow, *Done;
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;
  double Change;
  AnaMatRec *WITH;

  if (AnaEpoch != AnaE_Calculate || AnaQuit())
    return;
  WITH = AnaSystem;
  if (AnaDebugset)
    AnaDiffdump();
  *Overflow = false;
  *Done = true;
  AnaToobig = false;
  AnaToosmall = true;
  Ext_Count = AnaLogglobals->nbase;
  while (Ext_Count != NULL) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      if (A_Node->nodenum == 0)
	A_Node->now = A_Node->last;
      else {
	A_Node->now = A_Node->last + WITH->Mat[A_Node->nodenum][AnaSysCol];
	if (A_Node->now > AnaHighest || A_Node->now < AnaLowest) {
	  *Overflow = true;
	  *Done = false;
	} else {
	  if (*Done)
	    *Done = Vardone(A_Node->now, A_Node->last);
	  if (*Done) {
	    Change = fabs(A_Node->now - A_Node->ltimestep);
	    if (Change > AnaVstep)
	      AnaToobig = true;
	    else {
	      if (Change > AnaVshift)
		AnaToosmall = false;
	    }
	  }
	}
      }
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL) {
    Int_Count->now = Int_Count->last + WITH->Mat[Int_Count->nodenum]
		     [AnaSysCol];
    if (Int_Count->now > AnaHighest || Int_Count->now < AnaLowest) {
      *Overflow = true;
      *Done = false;
    } else {
      if (*Done)
	*Done = Vardone(Int_Count->now, Int_Count->last);
      if (*Done) {
	Change = fabs(Int_Count->now - Int_Count->ltimestep);
	if (Change > AnaVstep)
	  AnaToobig = true;
	else {
	  if (Change > AnaVshift)
	    AnaToosmall = false;
	}
      }
    }
    Int_Count = Int_Count->next;
  }
  AnaEpoch = AnaE_Refresh;
}



Static Void Backtrack()
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;

  Ext_Count = AnaLogglobals->nbase;
  while (Ext_Count != NULL) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      A_Node->last = A_Node->ltimestep;
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL) {
    Int_Count->last = Int_Count->ltimestep;
    Int_Count = Int_Count->next;
  }
}


/*Doneproc**/

Static Void OldAdapt(Done)
boolean Done;
{
  if (AnaEpoch != AnaE_Adapt || AnaQuit())
    return;
  if (Done && (!AnaToobig || AnaDtcount <= AnaRelaxcount || !AnaExact)) {
    AnaSuccess = true;
    AnaDtlast = AnaDt;
    if (AnaDtcount < 0 && AnaToobig && AnaDtcount <= AnaRelaxcount && AnaExact) {
      while (AnaDtcount <= AnaRelaxcount) {
	AnaDt *= AnaTimefactor;
	AnaDtcount++;
      }
      AnaChangedDt = true;
      (*AnaLogglobals->hook.vmessage)("Relaxing accuracy for one timestep");
    } else {
      if (AnaDtcount < 0 && (AnaToosmall || !AnaExact)) {
	AnaDt *= AnaTimefactor;
	AnaChangedDt = true;
	AnaDtcount++;
      }
    }
    AnaItermax = AnaTimeincrease * AnaItercount;
  } else {
    AnaSuccess = false;
    AnaDt /= AnaTimefactor;
    AnaDtcount--;
    AnaChangedDt = true;
    if (AnaDtcount <= AnaRelaxcount)
      AnaItermax = AnaIterfirstmax;
    if (AnaDt < AnaTimemin) {
      (*AnaLogglobals->hook.vmessage)("Numerics unable to continue - restarting");
      AnaScoreboard("Numerics unable to continue - restarting",
		    (long)AnaMessScoreboard);
      AnaTimeReset();
      AnaChanged = true;
    }
    Backtrack();
  }
  if (!AnaSuccess) {
    AnaEpoch = AnaE_Update;
    return;
  }
  AnaEpoch = AnaE_Memory1;
  AnaLastgate = NULL;
  AnaLastpage = 1;
}


/*Doneproc**/

Static Void Adapt(Done)
boolean Done;
{
  if (AnaEpoch != AnaE_Adapt || AnaQuit())
    return;
  if (Done && (!AnaToobig || AnaDtcount <= AnaRelaxcount || !AnaExact ||
	       AnaDifficult)) {
    AnaSuccess = true;
    AnaDtlast = AnaDt;
    if (AnaDtcount < 0 && AnaToobig && AnaDtcount <= AnaRelaxcount && AnaExact) {
      while (AnaDtcount <= AnaRelaxcount) {
	AnaDt *= AnaTimefactor;
	AnaDtcount++;
      }
      AnaChangedDt = true;
    } else {
      if (AnaDtcount < 0 && (AnaToosmall || !AnaExact)) {
	AnaDt *= AnaTimefactor;
	AnaChangedDt = true;
	AnaDtcount++;
      }
    }
    AnaItermax = AnaTimeincrease * AnaItercount;
    AnaDifficult = false;
    if (AnaExact && AnaToobig)
      (*AnaLogglobals->hook.vmessage)("Relaxing accuracy for one timestep");
  } else {
    if (AnaDebugset && AnaError)
      AnaDiffdump();
    AnaSuccess = false;
    AnaDt /= AnaTimefactor;
    AnaDtcount--;
    AnaChangedDt = true;
    if (AnaDtcount == AnaRelaxcount)
      (*AnaLogglobals->hook.vmessage)("A difficult section - stand by");
    if (AnaDtcount <= AnaRelaxcount)
      AnaItermax = AnaIterfirstmax;
    if (AnaDt < AnaTimemin) {
      if (!AnaDifficult) {
	(*AnaLogglobals->hook.vmessage)(
	  "A very difficult section - please stand by");
	AnaDt = AnaDtmax;
	AnaDtcount = 0;
	AnaItermax = AnaIterfirstmax;
	AnaChanged = true;
	AnaDifficult = true;
      } else {
	(*AnaLogglobals->hook.vmessage)(
	  "An impossible section - please reset or change circuit");
	AnaTimeReset();
	AnaChanged = true;
      }
    }
    Backtrack();
  }
  if (!AnaSuccess) {
    AnaEpoch = AnaE_Update;
    return;
  }
  AnaEpoch = AnaE_Memory1;
  AnaLastgate = NULL;
  AnaLastpage = 1;
}


Static Void Timestep_Update()
{
  log_nrec *Ext_Count;
  AnaExt_rec *A_Node;
  AnaInt_rec *Int_Count;

  Ext_Count = AnaLogglobals->nbase;
  while (Ext_Count != NULL) {
    if (Ext_Count->simtype->simtype == 32) {
      A_Node = (AnaExt_rec *)Ext_Count->info;
      A_Node->ltimestep = A_Node->now;
      Ext_Count->changed = A_Node->changed_now;
      A_Node->col_ltimestep = A_Node->col_now;
    } else {
      if (Ext_Count->simtype->simtype == 33) {
	A_Node = (AnaExt_rec *)Ext_Count->info;
	A_Node->ltimestep = A_Node->now;
	A_Node->now = 0.0;
      }
    }
    Ext_Count = Ext_Count->next;
  }
  Int_Count = AnaInt_nodelist;
  while (Int_Count != NULL) {
    Int_Count->ltimestep = Int_Count->now;
    Int_Count = Int_Count->next;
  }
}



Static Void Memory()
{
  log_grec *Count;
  long Pages, Numpages;
  AnaCell_rec *A_Kind;
  Analog_32_action Act;
  boolean Timeout;
  log_grec *WITH;
  log_action *WITH1;
  log_tool *WITH2;

  Numpages = AnaLogglobals->numpages;
  if (AnaEpoch == AnaE_Memory1 && !AnaQuit()) {
    Pages = AnaLastpage;
    Timeout = false;
    while (Pages <= Numpages && !Timeout) {
      if (AnaLastgate == NULL)
	AnaLastgate = AnaLogglobals->gbase[Pages - 1];
      Count = AnaLastgate;
      while (Count != NULL && !Timeout) {
	WITH = Count;
	if (WITH->kind->simtype->simtype == 32) {
	  A_Kind = (AnaCell_rec *)WITH->kind->info;
	  if (A_Kind->phase == Anacomplex) {
	    Act.page = Pages;
	    Act.action = Analog_act_preupdate;
	    Act.inst = Count;
	    (*A_Kind->proc_cell)(&Act);
	  }
	}
	Count = Count->next;
	AnaLastgate = Count;
	Timeout = AnaQuit();
	if (Timeout) {
	  if (Count == NULL) {
	    Pages++;
	    AnaLastpage = Pages;
	  } else
	    AnaLastpage = Pages;
	}
      }
      if (!Timeout)
	Pages++;
    }
    if (Pages > Numpages) {
      AnaEpoch = AnaE_Memory2;
      AnaLastgate = NULL;
      AnaLastpage = 1;
      AnaTimestate = Anasettime;
    }
  }
  if (AnaEpoch != AnaE_Memory2 || AnaQuit())
    return;
  switch (AnaTimestate) {

  case Anasettime:
    WITH1 = AnaLogglobals;
    WITH2 = AnaLogglobals->acttool;
    WITH2->nexttstep = AnaDtlast;
    AnaTimestate = Anawait;
    break;

  case Anawait:
    WITH1 = AnaLogglobals;
    WITH2 = AnaLogglobals->acttool;
    WITH2->nexttstep = AnaDtlast;
    break;

  case Anacontinue:
    Pages = AnaLastpage;
    Timeout = false;
    while (Pages <= Numpages && !Timeout) {
      if (AnaLastgate == NULL)
	AnaLastgate = AnaLogglobals->gbase[Pages - 1];
      Count = AnaLastgate;
      while (Count != NULL && !Timeout) {
	WITH = Count;
	if (WITH->kind->simtype->simtype == 32) {
	  A_Kind = (AnaCell_rec *)WITH->kind->info;
	  if (A_Kind->phase == Anacomplex) {
	    Act.page = Pages;
	    Act.action = Analog_act_display;
	    Act.inst = Count;
	    (*A_Kind->proc_cell)(&Act);
	  }
	}
	Count = Count->next;
	AnaLastgate = Count;
	Timeout = AnaQuit();
	if (Timeout) {
	  if (Count == NULL) {
	    Pages++;
	    AnaLastpage = Pages;
	  } else
	    AnaLastpage = Pages;
	}
      }
      if (!Timeout)
	Pages++;
    }
    if (Pages > Numpages) {
      AnaTimestate = Anasettime;
      AnaEpoch = AnaE_Evaluate;
      AnaLastgate = NULL;
      AnaLastpage = 0;
      if (AnaStart) {
	AnaStart = false;
	AnaScoreboard("Simulation In Progress", (long)AnaMessScoreboard);
      }
      AnaItercount = 0;
      AnaToomany = false;
      AnaSuccess = false;
    }
    break;
  }
}


Void Analog_Tstep()
{
  log_grec *Count;
  AnaCell_rec *A_Kind;
  Analog_32_action Act;
  long Pages, FORLIM;
  log_grec *WITH;

  if (!(AnaLogglobals->actflag && AnaTimestate == Anawait))
    return;
  AnaTimestate = Anacontinue;
  AnaAccum = AnaLogglobals->time;
  FORLIM = AnaLogglobals->numpages;
  for (Pages = 0; Pages < FORLIM; Pages++) {
    Count = AnaLogglobals->gbase[Pages];
    while (Count != NULL) {
      WITH = Count;
      if (WITH->kind->simtype->simtype == 32) {
	A_Kind = (AnaCell_rec *)WITH->kind->info;
	Act.action = Analog_act_update;
	Act.inst = Count;
	(*A_Kind->proc_cell)(&Act);
      }
      Count = Count->next;
    }
  }
  Timestep_Update();
}


/*Doneproc**/

Void Analog_Simulate()
{
  AnaTimeinit();
  Evaluate();
  Solve();
  if (!AnaError) {
    Calculate(&AnaOverflow, &AnaDone);
    Refresh();
  } else
    AnaDone = false;
  if (!(AnaDone || AnaOverflow || AnaError || AnaToomany))
    return;
  Adapt(AnaDone);
  if (AnaSuccess)
    Memory();
  else
    Update();
}





/* End. */
