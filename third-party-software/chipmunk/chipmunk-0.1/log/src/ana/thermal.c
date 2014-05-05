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
/* From input file "tools.text" */



#include <p2c/p2c.h>


#define THERMAL_G
#include "thermal.h"

#define N_Kelvin       1
#define N_Celsius      2
#define N_Fahrenheit   3
#define N_Selection    4

#define Kdisplay       0
#define Cdisplay       1
#define Fdisplay       2

#define GateName        "Die Temperature"




typedef struct ThermalConst {
  Char OldDisplay[256], Display[256];
  boolean Changed;
} ThermalConst;


typedef struct ICThermal {
  long InitUnits;
} ICThermal;




Local Void GetCnf_Thermal(NewIC)
ICThermal *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  ICThermal *WITH;

  WITH = NewIC;
  do {
    AnaGetcommand("THERMAL", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "UNITS")) {
	TRY(try1);
	   (*AnaLogglobals->hook.getword)(Arg, Keyword);
	 if (!strcmp(Keyword, "CELSIUS")) 
	   {
	     WITH->InitUnits = Cdisplay;
	   }
	 else
	   {
	     if (!strcmp(Keyword, "FAHRENHEIT")) 
	       {
		 WITH->InitUnits = Fdisplay;
	       }
	     else
	       {
		 WITH->InitUnits = Kdisplay;
	       }
	   }
	RECOVER(try1);
	   {
	     WITH->InitUnits = Kdisplay;
           }
	ENDTRY(try1);
      }
    }
  } while (Found);
}


/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/


Local Void initlib_Thermal(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICThermal *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->plist_cell = NULL;
  NewCell->phase = Anasimple;
  NewIC = (ICThermal *)Malloc(sizeof(ICThermal));
  NewIC->InitUnits = Kdisplay;
  GetCnf_Thermal(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*NFET4_Initlib*/

Local Void Refresh_Thermal(act)
Analog_32_action *act;
{
  short Tx, Ty;
  Anainstlist *A_Gate;
  ThermalConst *ThermalVar, *WITH;
  Char Newout[256];

  A_Gate = (Anainstlist *)act->inst->info;
  ThermalVar = (ThermalConst *)A_Gate->InstVar;
  WITH = ThermalVar;
  (*AnaLogglobals->hook.hidecursor)();
  Tx = 0;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  if (ThermalVar->Changed)
    {
      ThermalVar->Changed = false;
      m_color((long)AnaLogglobals->color.backgr);
      m_centerstr((long)Tx, (long)Ty, NULL, ThermalVar->OldDisplay);
      switch (act->inst->attr[N_Selection - 1].UU.nv)
	{
	case Kdisplay:
	  {
	    (*AnaLogglobals->hook.realunit)(AnaPhysical.T, (int)1, "K", Newout);
	    strcpy(ThermalVar->OldDisplay, Newout);
	  }
	  break;
	case Cdisplay:
	  {
	    (*AnaLogglobals->hook.realunit)((AnaPhysical.T-273), (int)1, "C", Newout);
	    strcpy(ThermalVar->OldDisplay, Newout);
	  }
	  break;
	case Fdisplay:
	  {
	    (*AnaLogglobals->hook.realunit)(((AnaPhysical.T - 273)*(9.0/5.0) + 32), 
					    (int)1, "F", Newout);
	    strcpy(ThermalVar->OldDisplay, Newout);
	  }
	  break;
	}
    }
  m_color((long)AnaColormap.C_Imeter);
  m_centerstr((long)Tx, (long)Ty, NULL, ThermalVar->OldDisplay);
  (*AnaLogglobals->hook.unhidecursor)();

}


Local Void Erase_Thermal(act)
Analog_32_action *act;
{
  short Tx, Ty;
  Anainstlist *A_Gate;
  ThermalConst *ThermalVar, *WITH;


  A_Gate = (Anainstlist *)act->inst->info;
  ThermalVar = (ThermalConst *)A_Gate->InstVar;
  WITH = ThermalVar;
  (*AnaLogglobals->hook.hidecursor)();
  m_color((long)AnaLogglobals->color.backgr);
  Tx = 0;
  Ty = 0;
  (*AnaLogglobals->hook.xform)(act->inst, &Tx, &Ty);
  m_centerstr((long)Tx, (long)Ty, NULL, ThermalVar->OldDisplay);
  (*AnaLogglobals->hook.unhidecursor)();
  

}


 
Local Void Attr_Thermal(act)
Analog_32_action *act;
{
  long Attrnum;
  log_grec *WITH;
  log_gattrrec *WITH1;
  ThermalConst *ThermalVar;
  Anainstlist *A_Gate;

  WITH = act->inst;
  Attrnum = act->pin;
  act->ok = true;
  A_Gate = (Anainstlist *)WITH->info;
  ThermalVar = (ThermalConst *)A_Gate->InstVar;

  switch (Attrnum) {

  case N_Kelvin:
    if (WITH->attr[Attrnum - 1].UU.r > 10000 ||
	WITH->attr[Attrnum - 1].UU.r <= 0 || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.T = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	WITH->attr[N_Kelvin-1].UU.r = AnaPhysical.T;
	WITH->attr[N_Celsius-1].UU.r = AnaPhysical.T - 273;
	WITH->attr[N_Fahrenheit-1].UU.r= (AnaPhysical.T - 273)*(9.0/5.0) + 32;
	WITH->attr[N_Kelvin - 1].blnk = false;
	WITH->attr[N_Fahrenheit - 1].blnk = false;
	WITH->attr[N_Celsius - 1].blnk = false;
	WITH->attr[N_Kelvin - 1].changed = true;
	WITH->attr[N_Fahrenheit - 1].changed = true;
	WITH->attr[N_Celsius - 1].changed = true;
	ThermalVar->Changed = true;
	Refresh_Thermal(act);
      }
    break;

  case N_Celsius:
    if (WITH->attr[Attrnum - 1].UU.r > 10000 ||
	WITH->attr[Attrnum - 1].UU.r <= -273 || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.T = WITH->attr[Attrnum - 1].UU.r + 273;
	AnaParamUpdate();
	WITH->attr[N_Kelvin-1].UU.r = AnaPhysical.T;
	WITH->attr[N_Celsius-1].UU.r = AnaPhysical.T - 273;
	WITH->attr[N_Fahrenheit-1].UU.r= (AnaPhysical.T - 273)*(9.0/5.0) + 32;
	WITH->attr[N_Kelvin - 1].blnk = false;
	WITH->attr[N_Fahrenheit - 1].blnk = false;
	WITH->attr[N_Celsius - 1].blnk = false;
	WITH->attr[N_Kelvin - 1].changed = true;
	WITH->attr[N_Fahrenheit - 1].changed = true;
	WITH->attr[N_Celsius - 1].changed = true;
	ThermalVar->Changed = true;
	Refresh_Thermal(act);
      }
    break;


  case N_Fahrenheit:
    if (WITH->attr[Attrnum - 1].UU.r > 10000 ||
	WITH->attr[Attrnum - 1].UU.r <= -459.4 || WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.T = (WITH->attr[Attrnum - 1].UU.r-32)*(5.0/9.0) + 273;
	AnaParamUpdate();
	WITH->attr[N_Kelvin-1].UU.r = AnaPhysical.T;
	WITH->attr[N_Celsius-1].UU.r = AnaPhysical.T - 273;
	WITH->attr[N_Fahrenheit-1].UU.r= (AnaPhysical.T - 273)*(9.0/5.0) + 32;
	WITH->attr[N_Kelvin - 1].blnk = false;
	WITH->attr[N_Fahrenheit - 1].blnk = false;
	WITH->attr[N_Celsius - 1].blnk = false;
	WITH->attr[N_Kelvin - 1].changed = true;
	WITH->attr[N_Fahrenheit - 1].changed = true;
	WITH->attr[N_Celsius - 1].changed = true;
	ThermalVar->Changed = true;
	Refresh_Thermal(act);
      }
    break;

  case N_Selection:
    {
	ThermalVar->Changed = true;
	Refresh_Thermal(act);
    }
    break;
  }
}

Local Void Openconfig_Thermal(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  WITH->attr[N_Kelvin-1].UU.r = AnaPhysical.T;
  WITH->attr[N_Celsius-1].UU.r = AnaPhysical.T - 273;
  WITH->attr[N_Fahrenheit-1].UU.r= (AnaPhysical.T - 273)*(9.0/5.0) + 32;
  WITH->attr[N_Kelvin - 1].blnk = false;
  WITH->attr[N_Fahrenheit - 1].blnk = false;
  WITH->attr[N_Celsius - 1].blnk = false;
  WITH->attr[N_Kelvin - 1].changed = true;
  WITH->attr[N_Fahrenheit - 1].changed = true;
  WITH->attr[N_Celsius - 1].changed = true;
}

Local Void Readgate_Thermal(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaChangedP = true;
    AnaPhysical.T = WITH->attr[N_Kelvin - 1].UU.r;
    AnaParamUpdate();
  }
}


Local Void Newgate_Thermal(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  AnaCell_rec *Cellptr;
  ICThermal *ICptr;
  ThermalConst *ThermalVar;
  Analog_32_action Newact;
  Char Newout[256];

  ThermalVar = (ThermalConst *)Malloc(sizeof(ThermalConst));
  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICThermal *)Cellptr->Ainfo;
  act->inst->attr[N_Selection - 1].UU.nv = ICptr->InitUnits;
  act->inst->attr[N_Selection - 1].blnk = false;
  act->inst->attr[N_Selection - 1].changed = true;

  /* strcpy(ThermalVar->OldMessScoreboard, ICptr->InitMessScoreboard); */
  /*ThermalVar->OldErase = AnaErase;*/

  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)ThermalVar;
  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Thermal(&Newact);
  ThermalVar->Changed = false;
  
  switch (act->inst->attr[N_Selection - 1].UU.nv)
    {
      case Kdisplay:
	{
	  (*AnaLogglobals->hook.realunit)(AnaPhysical.T, (int)0, "K", Newout);
	  strcpy(ThermalVar->OldDisplay, Newout);
	}
	break;
      case Cdisplay:
	{
	  (*AnaLogglobals->hook.realunit)((AnaPhysical.T-273), (int)0, "C", Newout);
	  strcpy(ThermalVar->OldDisplay, Newout);
	}
	break;
      case Fdisplay:
	{
	  (*AnaLogglobals->hook.realunit)(((AnaPhysical.T - 273)*(9.0/5.0) + 32), 
                                          (int)0, "F", Newout);
	  strcpy(ThermalVar->OldDisplay, Newout);
	}
	break;
    }
}

Local Void Copygate_Thermal(act)
Analog_32_action *act;
{
  ThermalConst *ThermalVar, *Old_ThermalVar;
  Anainstlist *A_Gate, *A_Oldgate;

  A_Oldgate = (Anainstlist *)AnaLogglobals->actgate2->info;
  Old_ThermalVar = (ThermalConst *)A_Oldgate->InstVar;
  ThermalVar = (ThermalConst *)Malloc(sizeof(ThermalConst));
  *ThermalVar = *Old_ThermalVar;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)ThermalVar;
}

Local Void Dispose_Thermal(act)
Analog_32_action *act;
{
  Anainstlist *A_Gate;
  ThermalConst *ThermalVar;

  A_Gate = (Anainstlist *)act->inst->info;
  ThermalVar = (ThermalConst *)A_Gate->InstVar;
  Free(ThermalVar);
}



/*==========================================================================*/
/*                 Library Listing For Cell Thermal                    */
/*==========================================================================*/

Void Log_THERMAL_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Thermal(act);
    break;

  case Analog_act_attrchange:
    Attr_Thermal(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Thermal(act);
    break;

  case Analog_act_readgate:
    Readgate_Thermal(act);
    break;

  case Analog_act_newgate:
    Newgate_Thermal(act);
    break;

  case Analog_act_copygate:
    Copygate_Thermal(act);
    break;

  case Analog_act_disposegate:
    Dispose_Thermal(act);
    break;

  case Analog_act_erase:
    Erase_Thermal(act);
    break;

  case Analog_act_refresh:
    Refresh_Thermal(act);
    break;

  }
}


#undef N_Kelvin       
#undef N_Celsius      
#undef N_Farenheit    
#undef N_Selection    
#undef Kdisplay
#undef Cdisplay
#undef Fdisplay





/* End. */
