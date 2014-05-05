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


#define PHYSICAL_G
#include "physical.h"

#define N_k            1
#define N_q            2
#define N_e_v          3
#define N_e_s          4
#define N_e_ox         5
#define N_ktq          6

#define N_phi_ms       7
#define N_Eg           8
#define N_ni           9
 
#define GateName        "Physical Parameters"





/****************************************************************************/
/*                          Installation Procedure                           */
/****************************************************************************/


Local Void initlib_Physical(act)
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


Local Void Reload_Physical(act)
Analog_32_action *act;
{
  log_grec *WITH;


  WITH = act->inst;

  WITH->attr[N_k -1].UU.r = AnaPhysical.k;
  WITH->attr[N_k - 1].blnk = false;
  WITH->attr[N_k - 1].changed = true;

  WITH->attr[N_q -1].UU.r = AnaPhysical.q;
  WITH->attr[N_q - 1].blnk = false;
  WITH->attr[N_q - 1].changed = true;

  WITH->attr[N_e_v -1].UU.r = AnaPhysical.e_v;
  WITH->attr[N_e_v - 1].blnk = false;
  WITH->attr[N_e_v - 1].changed = true;

  WITH->attr[N_e_s -1].UU.r = AnaPhysical.e_s;
  WITH->attr[N_e_s - 1].blnk = false;
  WITH->attr[N_e_s - 1].changed = true;

  WITH->attr[N_e_ox -1].UU.r = AnaPhysical.e_ox;
  WITH->attr[N_e_ox - 1].blnk = false;
  WITH->attr[N_e_ox - 1].changed = true;

  WITH->attr[N_ktq -1].UU.r = AnaPhysical.Ut;
  WITH->attr[N_ktq - 1].blnk = false;
  WITH->attr[N_ktq - 1].changed = true;

  WITH->attr[N_phi_ms -1].UU.r = AnaGenFab.phi_ms;
  WITH->attr[N_phi_ms - 1].blnk = false;
  WITH->attr[N_phi_ms - 1].changed = true;

  WITH->attr[N_Eg -1].UU.r = AnaGenFab.Eg;
  WITH->attr[N_Eg - 1].blnk = false;
  WITH->attr[N_Eg - 1].changed = true;

  WITH->attr[N_ni -1].UU.r = AnaGenFab.ni;
  WITH->attr[N_ni - 1].blnk = false;
  WITH->attr[N_ni - 1].changed = true;


}

Local Void Attr_Physical(act)
Analog_32_action *act;
{
  long Attrnum;
  log_grec *WITH;

  WITH = act->inst;
  Attrnum = act->pin;
  act->ok = true;

  switch (Attrnum) {

  case N_ktq:
  case N_Eg:
  case N_ni:
    act->ok = false;
    break;

  case N_k:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPhysical.k ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPhysical.k ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.k = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  case N_q:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPhysical.q ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPhysical.q ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.q = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  case N_e_v:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPhysical.e_v ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPhysical.e_v ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.e_v = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  case N_e_s:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPhysical.e_s ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPhysical.e_s ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.e_s = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  case N_e_ox:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxPhysical.e_ox ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinPhysical.e_ox ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaPhysical.e_ox = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  case N_phi_ms:
    if (WITH->attr[Attrnum - 1].UU.r > AnaMaxGenFab.phi_ms ||
	WITH->attr[Attrnum - 1].UU.r < AnaMinGenFab.phi_ms ||
	WITH->attr[Attrnum - 1].blnk)
      act->ok = false;
    else
      {
	AnaChangedP = true;
	AnaGenFab.phi_ms = WITH->attr[Attrnum - 1].UU.r;
	AnaParamUpdate();
	Reload_Physical(act);
      }
    break;

  }
}

Local Void Openconfig_Physical(act)
Analog_32_action *act;
{
  long Index;
  log_grec *WITH;

  if (!act->ok)
    return;
  WITH = act->inst;
  Reload_Physical(act);
}

Local Void Readgate_Physical(act)
Analog_32_action *act;
{
  log_grec *WITH;
  Char STR1[256];

  WITH = act->inst;
  if (AnaOldmodel && !WITH->attr[0].blnk) {
    AnaChangedP = true;
    AnaPhysical.k    = WITH->attr[N_k -1].UU.r;
    AnaPhysical.q    = WITH->attr[N_q -1].UU.r;
    AnaPhysical.e_v  = WITH->attr[N_e_v -1].UU.r;
    AnaPhysical.e_s  = WITH->attr[N_e_s -1].UU.r;
    AnaPhysical.e_ox = WITH->attr[N_e_ox -1].UU.r;
    AnaGenFab.phi_ms = WITH->attr[N_phi_ms -1].UU.r;
    AnaParamUpdate();
  }
}


Local Void Newgate_Physical(act)
Analog_32_action *act;
{
  Analog_32_action Newact;
  Char Newout[256];

  Newact.inst = act->inst;
  Newact.ok = true;
  Openconfig_Physical(&Newact);

}



/*==========================================================================*/
/*                 Library Listing For Cell Physical                    */
/*==========================================================================*/

Void Log_PHYSICAL_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    initlib_Physical(act);
    break;

  case Analog_act_attrchange:
    Attr_Physical(act);
    break;

  case Analog_act_openconfig:
    Openconfig_Physical(act);
    break;

  case Analog_act_readgate:
    Readgate_Physical(act);
    break;

  case Analog_act_newgate:
    Newgate_Physical(act);
    break;


  }
}


#undef N_k 
#undef N_q
#undef N_e_v
#undef N_e_s
#undef N_e_ox
#undef N_ktq 
#undef N_phi_ms
#undef N_Eg
#undef N_ni






/* End. */
