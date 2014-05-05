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


#define DEVTECHP_G
#include "devtechp.h"

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


Local Void initlib_Devtechp(act)
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


Local Void Reload_Devtechp(act)
Analog_32_action *act;
{
  log_grec *WITH;


  WITH = act->inst;

  strcpy(WITH->attr[N_process - 1].UU.c, AnaGenFab.process);
  WITH->attr[N_process - 1].blnk = false;
  WITH->attr[N_process - 1].changed = true;

  WITH->attr[N_Tox -1].UU.r = AnaPFab.Tox;
  WITH->attr[N_Tox - 1].blnk = false;
  WITH->attr[N_Tox - 1].changed = true;

  WITH->attr[N_Cox -1].UU.r = AnaPFab.Cox;
  WITH->attr[N_Cox - 1].blnk = false;
  WITH->attr[N_Cox - 1].changed = true;

  WITH->attr[N_mu0 -1].UU.r = AnaPFab.mu0;
  WITH->attr[N_mu0 - 1].blnk = false;
  WITH->attr[N_mu0 - 1].changed = true;

  WITH->attr[N_mu -1].UU.r = AnaPFab.mu;
  WITH->attr[N_mu - 1].blnk = false;
  WITH->attr[N_mu - 1].changed = true;

  WITH->attr[N_Na -1].UU.r = AnaPFab.Na;
  WITH->attr[N_Na - 1].blnk = false;
  WITH->attr[N_Na - 1].changed = true;

  WITH->attr[N_del_NaS -1].UU.r = AnaPFab.del_NaS;
  WITH->attr[N_del_NaS - 1].blnk = false;
  WITH->attr[N_del_NaS - 1].changed = true;

  WITH->attr[N_psi -1].UU.r = AnaPFab.psi;
  WITH->attr[N_psi - 1].blnk = false;
  WITH->attr[N_psi - 1].changed = true;

  WITH->attr[N_lambda - 1].UU.r = AnaGenFab.lambda;
  WITH->attr[N_lambda - 1].blnk = false;
  WITH->attr[N_lambda - 1].changed = true;

  WITH->attr[N_Early_s -1].UU.r = AnaPFab.Early_s;
  WITH->attr[N_Early_s - 1].blnk = false;
  WITH->attr[N_Early_s - 1].changed = true;

  WITH->attr[N_L_0 -1].UU.r = AnaPFab.L_0;
  WITH->attr[N_L_0 - 1].blnk = false;
  WITH->attr[N_L_0 - 1].changed = true;

  WITH->attr[N_deltaW -1].UU.r = AnaPFab.deltaW;
  WITH->attr[N_deltaW - 1].blnk = false;
  WITH->attr[N_deltaW - 1].changed = true;

  WITH->attr[N_deltaL -1].UU.r = AnaPFab.deltaL;
  WITH->attr[N_deltaL - 1].blnk = false;
  WITH->attr[N_deltaL - 1].changed = true;

  WITH->attr[N_aCactive -1].UU.r = AnaPFab.aCactive;
  WITH->attr[N_aCactive - 1].blnk = false;
  WITH->attr[N_aCactive - 1].changed = true;

  WITH->attr[N_linCgs -1].UU.r = AnaPFab.linCgs;
  WITH->attr[N_linCgs - 1].blnk = false;
  WITH->attr[N_linCgs - 1].changed = true;

  WITH->attr[N_aCgw -1].UU.r = AnaPFab.aCgw;
  WITH->attr[N_aCgw - 1].blnk = false;
  WITH->attr[N_aCgw - 1].changed = true;

  WITH->attr[N_aCwell - 1].UU.r = AnaPFab.aCwell;
  WITH->attr[N_aCwell - 1].blnk = false;
  WITH->attr[N_aCwell - 1].changed = true;


}

Local Void Attr_Devtechp(act)
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
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.Tox ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.Tox ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.Tox = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_mu0:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.mu0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.mu0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.mu0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_Na:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.Na ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.Na ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.Na = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_del_NaS:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.del_NaS ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.del_NaS ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.del_NaS = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_psi:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.psi ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.psi ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.psi = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
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
	Reload_Devtechp(act);
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
	Reload_Devtechp(act);
      }
    break;

  case N_Early_s:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.Early_s ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.Early_s ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.Early_s = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_L_0:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.L_0 ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.L_0 ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.L_0 = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_deltaW:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.deltaW ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.deltaW ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.deltaW = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_deltaL:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.deltaL ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.deltaL ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.deltaL = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_aCactive:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.aCactive ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.aCactive ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.aCactive = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_linCgs:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.linCgs ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.linCgs ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.linCgs = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_aCgw:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.aCgw ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.aCgw ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.aCgw = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;

  case N_aCwell:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPFab.aCwell ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPFab.aCwell ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPFab.aCwell = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Devtechp(act);
      }
    break;


  }
}

Local Void Openconfig_Devtechp(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  Reload_Devtechp(act);
}

Local Void Readgate_Devtechp(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaChangedP = true;
    AnaPFab.Tox = WITH->attr[N_Tox -1].UU.r;
    AnaPFab.Cox = WITH->attr[N_Cox -1].UU.r;
    AnaPFab.mu0 = WITH->attr[N_mu0 -1].UU.r;
    AnaPFab.mu = WITH->attr[N_mu -1].UU.r;
    AnaPFab.Na = WITH->attr[N_Na -1].UU.r;
    AnaPFab.del_NaS = WITH->attr[N_del_NaS -1].UU.r;
    AnaPFab.psi = WITH->attr[N_psi -1].UU.r;
    AnaGenFab.lambda = WITH->attr[N_lambda -1].UU.r;
    strcpy(AnaGenFab.process, WITH->attr[N_process - 1].UU.c);
    AnaPFab.Early_s = WITH->attr[N_Early_s -1].UU.r;
    AnaPFab.L_0 = WITH->attr[N_L_0 -1].UU.r;
    AnaPFab.deltaW = WITH->attr[N_deltaW -1].UU.r;
    AnaPFab.deltaL = WITH->attr[N_deltaL -1].UU.r;
    AnaPFab.aCactive = WITH->attr[N_aCactive -1].UU.r;
    AnaPFab.linCgs = WITH->attr[N_linCgs -1].UU.r;
    AnaPFab.aCgw = WITH->attr[N_aCgw -1].UU.r;
    AnaPFab.aCwell = WITH->attr[N_aCwell - 1].UU.r;
    AnaParamUpdate();
  }
}


Local Void Newgate_Devtechp(act)
Analog_32_action *act;
{
  Analog_32_action Newact;
  Char Newout[256];

  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Devtechp(&Newact);

}



/*==========================================================================*/
/*                 Library Listing For Cell Devtechp                        */
/*==========================================================================*/

Void Log_DEVTECHP_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Devtechp(act);
    break;

  case Analog_act_attrchange:
    Attr_Devtechp(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Devtechp(act);
    break;

  case Analog_act_readgate:
    Readgate_Devtechp(act);
    break;

  case Analog_act_newgate:
    Newgate_Devtechp(act);
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
