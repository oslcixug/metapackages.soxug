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


#define RUNSPEC_G
#include "runspec.h"

#define N_fabrun       1
#define N_N_Na         2
#define N_N_Mu         3
#define N_N_Qss        4

#define N_P_Na         5
#define N_P_Mu         6
#define N_P_Qss        7


#define GateName        "Fabrication Parameters -- Adjustment Factors"





/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/


Local Void initlib_Runspec(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->plist_cell = NULL;
  NewCell->phase = Anasimple;
  act->kind->info = (Anyptr)NewCell;
}  /*NFET4_Initlib*/


Local Void Reload_Runspec(act)
Analog_32_action *act;
{
  log_grec *WITH;


  WITH = act->inst;

  strcpy(WITH->attr[N_fabrun - 1].UU.c, AnaGenFab.fabrun);
  WITH->attr[N_fabrun - 1].blnk = false;
  WITH->attr[N_fabrun - 1].changed = true;

  WITH->attr[N_N_Na - 1].UU.r = AnaNFab.del_Na0;
  WITH->attr[N_N_Na - 1].blnk = false;
  WITH->attr[N_N_Na - 1].changed = true;

  WITH->attr[N_N_Mu - 1].UU.r = AnaNFab.del_mu;
  WITH->attr[N_N_Mu - 1].blnk = false;
  WITH->attr[N_N_Mu - 1].changed = true;

  WITH->attr[N_N_Qss - 1].UU.r = AnaNFab.Qss;
  WITH->attr[N_N_Qss - 1].blnk = false;
  WITH->attr[N_N_Qss - 1].changed = true;

  WITH->attr[N_P_Na - 1].UU.r = AnaPFab.del_Na0;
  WITH->attr[N_P_Na - 1].blnk = false;
  WITH->attr[N_P_Na - 1].changed = true;

  WITH->attr[N_P_Mu - 1].UU.r = AnaPFab.del_mu;
  WITH->attr[N_P_Mu - 1].blnk = false;
  WITH->attr[N_P_Mu - 1].changed = true;

  WITH->attr[N_P_Qss - 1].UU.r = AnaPFab.Qss;
  WITH->attr[N_P_Qss - 1].blnk = false;
  WITH->attr[N_P_Qss - 1].changed = true;

}

Local Void Attr_Runspec(act)
Analog_32_action *act;
{
  long Attrnum;
  log_grec *WITH;

  WITH = act->inst;
  Attrnum = act->pin;
  act->ok = true;

  switch (Attrnum) {

  case N_fabrun:
    if (WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	strcpy(AnaGenFab.fabrun, WITH->attr[N_fabrun - 1].UU.c);
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_N_Na:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.del_Na0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.del_Na0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.del_Na0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_N_Mu:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.del_mu ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.del_mu ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.del_mu = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_N_Qss:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.Qss ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.Qss ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.Qss = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_P_Na:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.del_Na0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.del_Na0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.del_Na0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_P_Mu:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.del_mu ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.del_mu ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.del_mu = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;

  case N_P_Qss:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.Qss ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.Qss ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.Qss = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Runspec(act);
      }
    break;



  }
}

Local Void Openconfig_Runspec(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  Reload_Runspec(act);
}

Local Void Readgate_Runspec(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaChangedP = true;
    strcpy(AnaGenFab.fabrun, WITH->attr[N_fabrun - 1].UU.c);
    AnaNFab.del_Na0  = WITH->attr[N_N_Na -1].UU.r;
    AnaNFab.del_mu   = WITH->attr[N_N_Mu -1].UU.r;
    AnaNFab.Qss      = WITH->attr[N_N_Qss -1].UU.r;
    AnaPFab.del_Na0  = WITH->attr[N_P_Na -1].UU.r;
    AnaPFab.del_mu   = WITH->attr[N_P_Mu -1].UU.r;
    AnaPFab.Qss      = WITH->attr[N_P_Qss -1].UU.r;
    AnaParamUpdate();
  }
}


Local Void Newgate_Runspec(act)
Analog_32_action *act;
{
  Analog_32_action Newact;
  Char Newout[256];

  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Runspec(&Newact);

}



/*==========================================================================*/
/*                 Library Listing For Cell Runspec                          */
/*==========================================================================*/

Void Log_RUNSPEC_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Runspec(act);
    break;

  case Analog_act_attrchange:
    Attr_Runspec(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Runspec(act);
    break;

  case Analog_act_readgate:
    Readgate_Runspec(act);
    break;

  case Analog_act_newgate:
    Newgate_Runspec(act);
    break;


  }
}

#undef N_fabrun       
#undef N_N_Na         
#undef N_N_Mu         
#undef N_N_Qss        

#undef N_P_Na         
#undef N_P_Mu         
#undef N_P_Qss        







/* End. */
