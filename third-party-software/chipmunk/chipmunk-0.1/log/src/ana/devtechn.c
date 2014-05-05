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


#define DEVTECHN_G
#include "devtechn.h"

#define N_process      1
#define N_lambda       2
#define N_deltaL       3
#define N_deltaW       4

#define N_Tox          5
#define N_Cox          6
#define N_mu0          7
#define N_mu           8

#define N_Na           9
#define N_del_NaS      10
#define N_psi          11

#define N_Early_s      12
#define N_L_0          13

#define N_aCactive     14
#define N_linCgs       15
#define N_aCgw         16
#define N_aCwell       17


#define GateName        "Device Technology (N-Channel)"





/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/


Local Void initlib_Devtechn(act)
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


Local Void Reload_Devtechn(act)
Analog_32_action *act;
{
  log_grec *WITH;


  WITH = act->inst;

  strcpy(WITH->attr[N_process - 1].UU.c, AnaGenFab.process);
  WITH->attr[N_process - 1].blnk = false;
  WITH->attr[N_process - 1].changed = true;

  WITH->attr[N_Tox -1].UU.r = AnaNFab.Tox;
  WITH->attr[N_Tox - 1].blnk = false;
  WITH->attr[N_Tox - 1].changed = true;

  WITH->attr[N_Cox -1].UU.r = AnaNFab.Cox;
  WITH->attr[N_Cox - 1].blnk = false;
  WITH->attr[N_Cox - 1].changed = true;

  WITH->attr[N_mu0 -1].UU.r = AnaNFab.mu0;
  WITH->attr[N_mu0 - 1].blnk = false;
  WITH->attr[N_mu0 - 1].changed = true;

  WITH->attr[N_mu -1].UU.r = AnaNFab.mu;
  WITH->attr[N_mu - 1].blnk = false;
  WITH->attr[N_mu - 1].changed = true;

  WITH->attr[N_Na -1].UU.r = AnaNFab.Na;
  WITH->attr[N_Na - 1].blnk = false;
  WITH->attr[N_Na - 1].changed = true;

  WITH->attr[N_del_NaS -1].UU.r = AnaNFab.del_NaS;
  WITH->attr[N_del_NaS - 1].blnk = false;
  WITH->attr[N_del_NaS - 1].changed = true;

  WITH->attr[N_psi -1].UU.r = AnaNFab.psi;
  WITH->attr[N_psi - 1].blnk = false;
  WITH->attr[N_psi - 1].changed = true;

  WITH->attr[N_lambda - 1].UU.r = AnaGenFab.lambda;
  WITH->attr[N_lambda - 1].blnk = false;
  WITH->attr[N_lambda - 1].changed = true;

  WITH->attr[N_Early_s -1].UU.r = AnaNFab.Early_s;
  WITH->attr[N_Early_s - 1].blnk = false;
  WITH->attr[N_Early_s - 1].changed = true;

  WITH->attr[N_L_0 -1].UU.r = AnaNFab.L_0;
  WITH->attr[N_L_0 - 1].blnk = false;
  WITH->attr[N_L_0 - 1].changed = true;

  WITH->attr[N_deltaW -1].UU.r = AnaNFab.deltaW;
  WITH->attr[N_deltaW - 1].blnk = false;
  WITH->attr[N_deltaW - 1].changed = true;

  WITH->attr[N_deltaL -1].UU.r = AnaNFab.deltaL;
  WITH->attr[N_deltaL - 1].blnk = false;
  WITH->attr[N_deltaL - 1].changed = true;

  WITH->attr[N_aCactive -1].UU.r = AnaNFab.aCactive;
  WITH->attr[N_aCactive - 1].blnk = false;
  WITH->attr[N_aCactive - 1].changed = true;

  WITH->attr[N_linCgs -1].UU.r = AnaNFab.linCgs;
  WITH->attr[N_linCgs - 1].blnk = false;
  WITH->attr[N_linCgs - 1].changed = true;

  WITH->attr[N_aCgw -1].UU.r = AnaNFab.aCgw;
  WITH->attr[N_aCgw - 1].blnk = false;
  WITH->attr[N_aCgw - 1].changed = true;

  WITH->attr[N_aCwell - 1].UU.r = AnaNFab.aCwell;
  WITH->attr[N_aCwell - 1].blnk = false;
  WITH->attr[N_aCwell - 1].changed = true;


}

Local Void Attr_Devtechn(act)
Analog_32_action *act;
{
  long Attrnum;
  log_grec *WITH;

  WITH = act->inst;
  Attrnum = act->pin;
  act->ok = true;

  switch (Attrnum) {

  case N_Cox:
  case N_mu:
    act->ok = false;
    break;

  case N_Tox:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.Tox ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.Tox ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.Tox = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_mu0:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.mu0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.mu0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.mu0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_Na:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.Na ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.Na ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.Na = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_del_NaS:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.del_NaS ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.del_NaS ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.del_NaS = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_psi:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.psi ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.psi ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.psi = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;


  case N_lambda:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxGenFab.lambda ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinGenFab.lambda ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaGenFab.lambda = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_process:
    if (WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	strcpy(AnaGenFab.process, WITH->attr[N_process - 1].UU.c);
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_Early_s:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.Early_s ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.Early_s ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.Early_s = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_L_0:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.L_0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.L_0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.L_0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_deltaW:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.deltaW ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.deltaW ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.deltaW = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_deltaL:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.deltaL ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.deltaL ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.deltaL = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_aCactive:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.aCactive ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.aCactive ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.aCactive = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_linCgs:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.linCgs ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.linCgs ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.linCgs = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_aCgw:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.aCgw ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.aCgw ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.aCgw = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;

  case N_aCwell:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxNFab.aCwell ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinNFab.aCwell ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaNFab.aCwell = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechn(act);
      }
    break;


  }
}

Local Void Openconfig_Devtechn(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  Reload_Devtechn(act);
}

Local Void Readgate_Devtechn(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaChangedP = true;
    AnaNFab.Tox = WITH->attr[N_Tox -1].UU.r;
    AnaNFab.Cox = WITH->attr[N_Cox -1].UU.r;
    AnaNFab.mu0 = WITH->attr[N_mu0 -1].UU.r;
    AnaNFab.mu = WITH->attr[N_mu -1].UU.r;
    AnaNFab.Na = WITH->attr[N_Na -1].UU.r;
    AnaNFab.del_NaS = WITH->attr[N_del_NaS -1].UU.r;
    AnaNFab.psi = WITH->attr[N_psi -1].UU.r;
    AnaGenFab.lambda = WITH->attr[N_lambda -1].UU.r;
    strcpy(AnaGenFab.process, WITH->attr[N_process - 1].UU.c);
    AnaNFab.Early_s = WITH->attr[N_Early_s -1].UU.r;
    AnaNFab.L_0 = WITH->attr[N_L_0 -1].UU.r;
    AnaNFab.deltaW = WITH->attr[N_deltaW -1].UU.r;
    AnaNFab.deltaL = WITH->attr[N_deltaL -1].UU.r;
    AnaNFab.aCactive = WITH->attr[N_aCactive -1].UU.r;
    AnaNFab.linCgs = WITH->attr[N_linCgs -1].UU.r;
    AnaNFab.aCgw = WITH->attr[N_aCgw -1].UU.r;
    AnaNFab.aCwell = WITH->attr[N_aCwell - 1].UU.r;
    AnaParamUpdate();
  }
}


Local Void Newgate_Devtechn(act)
Analog_32_action *act;
{
  Analog_32_action Newact;
  Char Newout[256];

  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Devtechn(&Newact);

}



/*==========================================================================*/
/*                 Library Listing For Cell Devtechn                        */
/*==========================================================================*/

Void Log_DEVTECHN_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Devtechn(act);
    break;

  case Analog_act_attrchange:
    Attr_Devtechn(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Devtechn(act);
    break;

  case Analog_act_readgate:
    Readgate_Devtechn(act);
    break;

  case Analog_act_newgate:
    Newgate_Devtechn(act);
    break;


  }
}



#undef N_process
#undef N_Tox          
#undef N_Cox          
#undef N_mu0          
#undef N_mu           

#undef N_Na           
#undef N_del_NaS      
#undef N_psi          

#undef N_lambda       
#undef N_Early_s      
#undef N_L_0          
#undef N_deltaW       
#undef N_deltaL       

#undef N_aCactive     
#undef N_linCgs       
#undef N_aCgw         
#undef N_aCwell       





/* End. */
