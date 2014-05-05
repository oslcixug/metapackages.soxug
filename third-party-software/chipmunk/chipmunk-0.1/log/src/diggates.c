/* Output from p2c, the Pascal-to-C translator */
/* From input file "diggates.text" */


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


/* Pascal module for SRAM8K and other gates */

/*$debug${*/



#include <p2c/p2c.h>


#define MYGATES_G
#include "mygates.h"




Static short crtcolor;
Static m_chpicrec **crtfont;


#define ramsize         8191
#define attr_mode       1
#define attr_addrhex    2
#define attr_addrdec    3
#define attr_valhex     4
#define attr_valdec     5
#define attr_saveit     6
#define attr_loadname   7
#define attr_savename   8
#define attr_savemin    9
#define attr_savemax    10


typedef uchar ramarray[ramsize + 1];


/* Local variables for Log_mygates_sram8k: */
struct LOC_Log_mygates_sram8k {
  log_16_action *act;
  uchar *ram;
} ;

Local Void peek(n, LINK)
long n;
struct LOC_Log_mygates_sram8k *LINK;
{
  log_16_action *WITH;
  log_action *WITH1;
  log_grec *WITH2;

  WITH = LINK->act;
  WITH1 = WITH->lact;
  WITH2 = WITH1->actgate;
  LINK->ram = (uchar *)WITH2->info;
  if (n < 0)
    n = 0;
  else if (n > ramsize)
    n = ramsize;
  if (n != WITH2->attr[attr_addrhex - 1].UU.U73.i1)
    WITH2->attr[attr_addrhex - 1].changed = true;
  if (n != WITH2->attr[attr_addrdec - 1].UU.U73.i1)
    WITH2->attr[attr_addrdec - 1].changed = true;
  if (LINK->ram[n] != WITH2->attr[attr_valhex - 1].UU.U73.i1)
    WITH2->attr[attr_valhex - 1].changed = true;
  if (LINK->ram[n] != WITH2->attr[attr_valdec - 1].UU.U73.i1)
    WITH2->attr[attr_valdec - 1].changed = true;
  WITH2->attr[attr_addrhex - 1].UU.U73.i1 = n;
  WITH2->attr[attr_addrdec - 1].UU.U73.i1 = n;
  WITH2->attr[attr_valhex - 1].UU.U73.i1 = LINK->ram[n];
  WITH2->attr[attr_valdec - 1].UU.U73.i1 = LINK->ram[n];
}

Local Void poke(n, LINK)
long n;
struct LOC_Log_mygates_sram8k *LINK;
{
  log_16_action *WITH;
  log_action *WITH1;
  log_grec *WITH2;

  WITH = LINK->act;
  WITH1 = WITH->lact;
  WITH2 = WITH1->actgate;
  LINK->ram = (uchar *)WITH2->info;
  if (n < 0)
    n = 0;
  else if (n > 255)
    n = 255;
  LINK->ram[WITH2->attr[attr_addrdec - 1].UU.U73.i1] = n;
  if (WITH2->attr[attr_mode - 1].UU.nv != 1) {
    peek(WITH2->attr[attr_addrdec - 1].UU.U73.i1 + 1, LINK);
    return;
  }
  WITH2->attr[attr_valdec - 1].UU.U73.i1 = n;
  WITH2->attr[attr_valdec - 1].changed = true;
  WITH2->attr[attr_valhex - 1].UU.U73.i1 = n;
  WITH2->attr[attr_valhex - 1].changed = true;
}

Local Void configchgate(LINK)
struct LOC_Log_mygates_sram8k *LINK;
{
  long index_;
  boolean lineflag;
  Char buf[256];
  FILE *textfile;
  log_16_action *WITH;
  log_action *WITH1;
  log_grec *WITH2;
  log_gattrrec *WITH3;
  long FORLIM;
  Char STR3[256];
  Char *TEMP;

  textfile = NULL;
  WITH = LINK->act;
  WITH1 = WITH->lact;
  WITH2 = WITH1->actgate;
  WITH3 = &WITH2->attr[WITH1->actx - 1];
  switch (WITH1->actx) {

  case attr_addrdec:
  case attr_addrhex:
    peek(WITH3->UU.U73.i1, LINK);
    break;

  case attr_valdec:
  case attr_valhex:
    if (WITH2->attr[attr_mode - 1].UU.nv != 0)
      poke(WITH3->UU.U73.i1, LINK);
    else
      WITH1->actflag = false;
    break;

  case attr_savename:
    strcpy(buf, WITH3->UU.c);
    newci_fixfname(buf, "ram", "");
    TRY(try1);
      if (textfile != NULL)
	textfile = freopen(buf, "w", textfile);
      else
	textfile = fopen(buf, "w");
      if (textfile == NULL) {
	P_escapecode = -10;
	P_ioresult = FileNotFound;
	goto _Ltry1;
      }
      lineflag = true;
      FORLIM = WITH2->attr[attr_savemax - 1].UU.U73.i1;
      for (index_ = WITH2->attr[attr_savemin - 1].UU.U73.i1;
	   index_ <= FORLIM;
	   index_++) {
	if (lineflag)
	  fprintf(textfile, "%.4lX:", index_ & 0xffffL);
	lineflag = false;
	fprintf(textfile, "%.2X", LINK->ram[index_] & 0xff);
	if ((index_ & 15) == 15) {
	  putc('\n', textfile);
	  lineflag = true;
	}
      }
      if (!lineflag)
	putc('\n', textfile);
      if (textfile != NULL)
	fclose(textfile);
      textfile = NULL;
    RECOVER2(try1,_Ltry1);
      if (P_escapecode == -20)
	_Escape(P_escapecode);
    ENDTRY(try1);
    *WITH3->UU.c = '\0';
    break;

  case attr_loadname:
    strcpy(buf, WITH3->UU.c);
    newci_fixfname(buf, "ram", "");
    TRY(try2);
      if (textfile != NULL)
	textfile = freopen(buf, "r", textfile);
      else
	textfile = fopen(buf, "r");
      if (textfile == NULL) {
	P_escapecode = -10;
	P_ioresult = FileNotFound;
	goto _Ltry2;
      }
      while (fgets(buf, 256, textfile) != NULL) {
	TEMP = (char *) strchr(buf, '\n');
	if (TEMP != NULL)
	  *TEMP = 0;
	sprintf(STR3, "%.4s", buf);
	index_ = strtol(STR3, NULL, 16);
	while (strlen(buf) > 6) {
	  LINK->ram[index_] = strtol(strpart(STR3, buf, 6, 7), NULL, 16);
	  strcpy(buf, buf + 2);
	  index_ = (index_ + 1) % (ramsize + 1);
/* p2c: diggates.text, line 160:
 * Note: Using % for possibly-negative arguments [317] */
	}
      }
      if (textfile != NULL)
	fclose(textfile);
      textfile = NULL;
    RECOVER2(try2,_Ltry2);
      if (P_escapecode == -20)
	_Escape(P_escapecode);
    ENDTRY(try2);
    *WITH3->UU.c = '\0';
    peek(WITH2->attr[attr_addrdec - 1].UU.U73.i1, LINK);
    break;
  }
  if (textfile != NULL)
    fclose(textfile);
}



Void Log_mygates_sram8k(act_)
log_16_action *act_;
{  /*sram8k*/
  struct LOC_Log_mygates_sram8k V;
  log_16_value v;
  uchar *ram2;
  long addr, ramval;
  log_16_action *WITH;
  log_action *WITH1;
  log_grec *WITH2;
  long FORLIM;
  int TEMP;

  V.act = act_;
  WITH = V.act;
  WITH1 = WITH->lact;
  WITH2 = WITH1->actgate;
  switch (V.act->action) {

  case act_16_new:
    V.ram = (uchar *)Malloc(sizeof(ramarray));
    WITH2->info = (Anyptr)V.ram;
    break;

  case act_16_copy:
    V.ram = (uchar *)Malloc(sizeof(ramarray));
    WITH2->info = (Anyptr)V.ram;
    ram2 = (uchar *)WITH1->actgate2->info;
    memcpy(V.ram, ram2, sizeof(ramarray));
    break;

  case act_16_dispose:
    V.ram = (uchar *)WITH2->info;
    Free(V.ram);
    break;

  case act_16_sim:
    if ((((unsigned long)WITH2->vars) & (1L << 13)) == 0) {
      V.ram = (uchar *)WITH2->info;
      /*   addr := vars.u1 div 8;  */
      addr = ((((unsigned long)WITH2->vars) & (1L << 0)) != 0) * 4096 +
	     ((((unsigned long)WITH2->vars) & (1L << 1)) != 0) * 2048 +
	     ((((unsigned long)WITH2->vars) & (1L << 2)) != 0) * 1024 +
	     ((((unsigned long)WITH2->vars) & (1L << 3)) != 0) * 512 +
	     ((((unsigned long)WITH2->vars) & (1L << 4)) != 0) * 256 +
	     ((((unsigned long)WITH2->vars) & (1L << 5)) != 0) * 128;
      addr += ((((unsigned long)WITH2->vars) & (1L << 6)) != 0) * 64 +
	      ((((unsigned long)WITH2->vars) & (1L << 7)) != 0) * 32 +
	      ((((unsigned long)WITH2->vars) & (1L << 8)) != 0) * 16 +
	      ((((unsigned long)WITH2->vars) & (1L << 9)) != 0) * 8 +
	      ((((unsigned long)WITH2->vars) & (1L << 10)) != 0) * 4 +
	      ((((unsigned long)WITH2->vars) & (1L << 11)) != 0) * 2 +
	      ((((unsigned long)WITH2->vars) & (1L << 12)) != 0);
      if ((((unsigned long)WITH2->vars) & (1L << 14)) != 0 &&
	  (((unsigned long)WITH2->vars) & (1L << 15)) == 0) {
	ramval = V.ram[addr];
	(*WITH->hook_output)(WITH->lact, WITH2->pin[13],
			     log_16_bv[((ramval / 128) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[14],
			     log_16_bv[((ramval / 64) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[15],
			     log_16_bv[((ramval / 32) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[16],
			     log_16_bv[((ramval / 16) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[17],
			     log_16_bv[((ramval / 8) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[18],
			     log_16_bv[((ramval / 4) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[19],
			     log_16_bv[((ramval / 2) & 1) - false]);
	(*WITH->hook_output)(WITH->lact, WITH2->pin[20],
			     log_16_bv[(ramval & 1) - false]);
      } else if ((((unsigned long)WITH2->vars) & (1L << 14)) == 0) {
	(*WITH->hook_input)(WITH->lact, WITH2->pin[13], &v);
	ramval = log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[14], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[15], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[16], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[17], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[18], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[19], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	(*WITH->hook_input)(WITH->lact, WITH2->pin[20], &v);
	ramval = ramval * 2 + log_16_vb[(long)v - (long)log_none];
	V.ram[addr] = ramval;
      }
    }
    break;

  case act_16_configgate:
    peek(WITH2->attr[attr_addrdec - 1].UU.U73.i1, &V);
    break;

  case act_16_configchgate:
    V.ram = (uchar *)WITH2->info;
    configchgate(&V);
    break;

  case act_16_write:
    if (WITH2->attr[attr_saveit - 1].UU.b) {
      V.ram = (uchar *)WITH2->info;
      FORLIM = WITH2->attr[attr_savemax - 1].UU.U73.i1;
      for (addr = WITH2->attr[attr_savemin - 1].UU.U73.i1; addr <= FORLIM; addr++)
	fprintf(*WITH1->actfile, "%d\n", V.ram[addr]);
    }
    break;

  case act_16_read:
    if (WITH2->attr[attr_saveit - 1].UU.b) {
      V.ram = (uchar *)WITH2->info;
      FORLIM = WITH2->attr[attr_savemax - 1].UU.U73.i1;
      for (addr = WITH2->attr[attr_savemin - 1].UU.U73.i1; addr <= FORLIM; addr++) {
	fscanf(*WITH1->actfile, "%d%*[^\n]", &TEMP);
	getc(*WITH1->actfile);
	V.ram[addr] = TEMP;
      }
    }
    break;
  }
}

#undef ramsize
#undef attr_mode
#undef attr_addrhex
#undef attr_addrdec
#undef attr_valhex
#undef attr_valdec
#undef attr_saveit
#undef attr_loadname
#undef attr_savename
#undef attr_savemin
#undef attr_savemax




Static long byte_reverse(n)
long n;
{
  return (((n / 128) & 1) + ((n / 64) & 1) * 2 + ((n / 32) & 1) * 4 +
	  ((n / 16) & 1) * 8 + ((n / 8) & 1) * 16 + ((n / 4) & 1) * 32 +
	  ((n / 2) & 1) * 64 + (n & 1) * 128);
}


#define width_          64
#define height_         16
#define xspc            6
#define yspc            9
#define xtoff           0
#define ytoff           0


typedef struct dispinfo {
  long xpos, ypos;
  boolean savetrig, saveclr, chgflag;
  Char scrn[height_][width_];
} dispinfo;


/* Local variables for Log_mygates_ascdisp: */
struct LOC_Log_mygates_ascdisp {
  log_16_action *act;
  dispinfo *dip;
} ;

Local Void findorg(xorg, yorg, LINK)
long *xorg, *yorg;
struct LOC_Log_mygates_ascdisp *LINK;
{
  short tx, ty;
  dispinfo *WITH;
  log_action *WITH1;

  WITH = LINK->dip;
  WITH1 = LINK->act->lact;
  tx = 0;
  ty = 0;
  (*WITH1->hook.xform)(WITH1->actgate, &tx, &ty);
  *xorg = tx - 21;
  *yorg = ty - yspc * height_ + 22;
}

Local Void xorcursor(LINK)
struct LOC_Log_mygates_ascdisp *LINK;
{
  long xorg, yorg;
  dispinfo *WITH;
  log_action *WITH1;

  WITH = LINK->dip;
  WITH1 = LINK->act->lact;
  findorg(&xorg, &yorg, LINK);
  m_color((long)crtcolor);
  m_colormode((long)m_xor);
  m_fillrect(xorg + WITH->xpos * xspc, yorg + WITH->ypos * yspc,
	     xorg + (WITH->xpos + 1) * xspc - 1,
	     yorg + (WITH->ypos + 1) * yspc - 1);
  m_colormode((long)m_normal);
}

Local Void erasedisp(LINK)
struct LOC_Log_mygates_ascdisp *LINK;
{
  long xorg, yorg;
  dispinfo *WITH;
  log_action *WITH1;

  WITH = LINK->dip;
  WITH1 = LINK->act->lact;
  (*WITH1->hook.hidecursor)();
  findorg(&xorg, &yorg, LINK);
  m_color((long)WITH1->color.backgr);
  m_fillrect(xorg - 2, yorg - 2, xorg + xspc * width_ + 1,
	     yorg + yspc * height_ + 1);
  (*WITH1->hook.unhidecursor)();
}

Local Void refrdisp(LINK)
struct LOC_Log_mygates_ascdisp *LINK;
{
  long x, y, yy, xorg, yorg;
  dispinfo *WITH;
  log_action *WITH1;
  Char STR1[2];

  WITH = LINK->dip;
  WITH1 = LINK->act->lact;
  (*WITH1->hook.hidecursor)();
  findorg(&xorg, &yorg, LINK);
  m_color((long)WITH1->color.gate);
  m_drawrect(xorg - 2, yorg - 2, xorg + xspc * width_ + 1,
	     yorg + yspc * height_ + 1);
  for (y = 0; y < height_; y++) {
    yy = yorg + y * yspc;
    m_color((long)WITH1->color.backgr);
    m_fillrect(xorg, yy, xorg + xspc * width_ - 1, yy + yspc - 1);
    m_color((long)crtcolor);
    for (x = 0; x < width_; x++) {
      if (WITH->scrn[y][x] != ' ') {
	sprintf(STR1, "%c", WITH->scrn[y][x]);
	m_drawstr(xorg + x * xspc, yy, (char *) crtfont, STR1);
      }
    }
  }
  xorcursor(LINK);
  (*WITH1->hook.unhidecursor)();
}

Local Void cleardisp(LINK)
struct LOC_Log_mygates_ascdisp *LINK;
{
  log_action *WITH;
  dispinfo *WITH1;

  WITH = LINK->act->lact;
  WITH1 = LINK->dip;
  memset((Anyptr)WITH1->scrn, ' ', (long)(width_ * height_));
  WITH1->xpos = 0;
  WITH1->ypos = 0;
  WITH1->chgflag = true;
}

Local Void scrollup(LINK)
struct LOC_Log_mygates_ascdisp *LINK;
{
  log_action *WITH;
  dispinfo *WITH1;

  WITH = LINK->act->lact;
  WITH1 = LINK->dip;
  memmove((Anyptr)WITH1->scrn[0], (Anyptr)WITH1->scrn[1],
	  width_ * (height_ - 1L));
  memset((Anyptr)WITH1->scrn[height_ - 1], ' ', (long)width_);
  WITH1->chgflag = true;
}

Local Void drawchar(ch, LINK)
Char ch;
struct LOC_Log_mygates_ascdisp *LINK;
{
  short attrnum;
  log_action *WITH;
  dispinfo *WITH1;

  WITH = LINK->act->lact;
  WITH1 = LINK->dip;
  if (ch < ' ') {
    switch (ch) {

    case '\001':
      WITH1->xpos = 0;
      WITH1->ypos = 0;
      WITH1->chgflag = true;
      break;

    case '\007':
      putchar('\007');
      break;

    case '\b':
      if (WITH1->xpos > 0) {
	WITH1->xpos--;
	WITH1->chgflag = true;
	(*WITH->hook2->findattrnum)(WITH->actgate->kind, "erase-bs", "B",
				    &attrnum);
	if (attrnum != 0 && WITH->actgate->attr[attrnum - 1].UU.b)
	  WITH1->scrn[WITH1->ypos][WITH1->xpos] = ' ';
      }
      break;

    case '\t':
      memset((Anyptr)(&WITH1->scrn[WITH1->ypos][WITH1->xpos]), ' ',
	     width_ - WITH1->xpos);
      WITH1->chgflag = true;
      break;

    case '\n':
      if (WITH1->ypos < height_ - 1) {
	WITH1->ypos++;
	WITH1->chgflag = true;
      } else
	scrollup(LINK);
      break;

    case '\013':
      memset((Anyptr)(&WITH1->scrn[WITH1->ypos][WITH1->xpos]), ' ',
	     width_ * height_ - WITH1->ypos * width_ - WITH1->xpos);
      WITH1->chgflag = true;
      break;

    case '\f':
      cleardisp(LINK);
      break;

    case '\015':
      WITH1->xpos = 0;
      WITH1->chgflag = true;
      (*WITH->hook2->findattrnum)(WITH->actgate->kind, "auto-lf", "B",
				  &attrnum);
      if (attrnum != 0 && WITH->actgate->attr[attrnum - 1].UU.b)
	drawchar('\n', LINK);
      break;

    case '\034':
      if (WITH1->xpos < width_ - 1) {
	WITH1->xpos++;
	WITH1->chgflag = true;
      }
      break;

    case '\037':
      if (WITH1->ypos > 0) {
	WITH1->ypos--;
	WITH1->chgflag = true;
      }
      break;
    }
    return;
  }
  if ((ch & (~127)) != 0)
    return;
/* p2c: diggates.text, line 472: Note: Character >= 128 encountered [281] */
  WITH1->scrn[WITH1->ypos][WITH1->xpos] = ch;
  WITH1->xpos++;
  if (WITH1->xpos >= width_) {
    (*WITH->hook2->findattrnum)(WITH->actgate->kind, "auto-wrap", "B",
				&attrnum);
    if (attrnum != 0 && WITH->actgate->attr[attrnum - 1].UU.b) {
      WITH1->xpos = 0;
      if (WITH1->ypos < height_ - 1)
	WITH1->ypos++;
      else
	scrollup(LINK);
    } else
      WITH1->xpos--;
  }
  WITH1->chgflag = true;
}


Void Log_mygates_ascdisp(act_)
log_16_action *act_;
{  /*ascdisp*/
  struct LOC_Log_mygates_ascdisp V;
  dispinfo *dip2;
  m_chpicrec **savef;
  log_16_action *WITH;
  log_action *WITH1;
  log_grec *WITH2;

  V.act = act_;
  WITH = V.act;
  WITH1 = WITH->lact;
  WITH2 = WITH1->actgate;
  switch (V.act->action) {

  case act_16_new:
    (*WITH1->hook.getcolor)("CRT", &crtcolor, log_green);
    m_seefont(&savef);
    m_choosefont(0L);
    m_seefont(&crtfont);
    m_setfont(savef);
    V.dip = (dispinfo *)Malloc(sizeof(dispinfo));
    WITH2->info = (Anyptr)V.dip;
    cleardisp(&V);
    V.dip->savetrig = true;
    V.dip->saveclr = true;
    break;

  case act_16_copy:
    V.dip = (dispinfo *)Malloc(sizeof(dispinfo));
    WITH2->info = (Anyptr)V.dip;
    dip2 = (dispinfo *)WITH1->actgate2->info;
    *V.dip = *dip2;
    break;

  case act_16_dispose:
    V.dip = (dispinfo *)WITH2->info;
    Free(V.dip);
    break;

  case act_16_sim:
    V.dip = (dispinfo *)WITH2->info;
    if ((((unsigned long)WITH2->vars) & (1L << 6)) != 0) {
      if (!V.dip->saveclr)
	cleardisp(&V);
    } else if ((((unsigned long)WITH2->vars) & (1L << 7)) != 0 && !V.dip->savetrig)
      drawchar((Char)byte_reverse(((long)WITH2->vars / 256L) & 255), &V);
    V.dip->saveclr = ((((unsigned long)WITH2->vars) & (1L << 6)) != 0);
    V.dip->savetrig = ((((unsigned long)WITH2->vars) & (1L << 7)) != 0);
    break;

  case act_16_draw:
    V.dip = (dispinfo *)WITH2->info;
    if (WITH1->refrflag || V.dip->chgflag) {
      refrdisp(&V);
      V.dip->chgflag = false;
    }
    break;

  case act_16_erase:
    V.dip = (dispinfo *)WITH2->info;
    erasedisp(&V);
    break;
  }
}

#undef width_
#undef height_
#undef xspc
#undef yspc
#undef xtoff
#undef ytoff


#define map             "1234567890-=qwertyuiop[]asdfghjkl;'\015zxcvbnm,./ \b            "
#define map_s           "!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:\"\015ZXCVBNM<>? \177            "
/* p2c: diggates.text, line 569: 
 * Warning: Multi-element string constructors not yet supported [136] */
#define map_c           "12345\036789\0\037\036\021\027\005\022\024\031\025\t\017\020\033\035\001\023\004\006\007\b\n\013\f~`\015"




Void Log_mygates_asckbd(act)
log_16_action *act;
{
  Char ch;
  long keyx, keyy;
  boolean asciimode;
  log_action *WITH;
  log_grec *WITH1;
  int TEMP;

  WITH = act->lact;
  WITH1 = WITH->actgate;
  switch (act->action) {

  case act_16_sim:
    if ((long)WITH1->info <= 0) {
      if ((((unsigned long)WITH1->vars) & (1L << 7)) != 0)
	WITH1->info = (na_long)6;
    } else {
      WITH1->info = (na_long)((long)WITH1->info - 1);
      if ((long)WITH1->info <= 0)
	WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 7)));
    }
    break;

  case act_16_touch:
    if (P_ibetween(-11L, WITH->actx, 12L) && P_ibetween(-5L, WITH->acty, 4L)) {
      keyx = (WITH->actx + 11) / 2;
      keyy = (WITH->acty + 5) / 2;
      if (keyy != 4 || (unsigned long)keyx < 32 && ((1L << keyx) & 0x1f9) != 0) {
	asciimode = (WITH1->attr[0].UU.nv == 0);
	if (asciimode && keyy == 3 && keyx == 10) {
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 6)) == 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 6)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 6));
	  /*shift key*/
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 5)));
	} else if (asciimode && keyy == 4 && keyx == 0) {
	  TEMP = ((((unsigned long)WITH1->vars) & (1L << 5)) == 0);
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 5)));
	  WITH1->vars = (na_long)
			(((unsigned long)WITH1->vars) | (((long)TEMP) << 5));
	  /*ctrl key*/
	  WITH1->vars = (na_long)(((unsigned long)WITH1->vars) & (~(1L << 6)));
	} else {
	  if (keyy == 4 && keyx >= 3 && keyx <= 8)
	    keyx = 3;
	  if (asciimode) {
	    if ((((unsigned long)WITH1->vars) & (1L << 5)) != 0)
	      ch = map_c[keyy * 12 + keyx];
	    else if ((((unsigned long)WITH1->vars) & (1L << 6)) != 0)
	      ch = map_s[keyy * 12 + keyx];
	    else
	      ch = map[keyy * 12 + keyx];
	  } else
	    ch = (Char)(keyy * 16 + keyx);
	  WITH1->vars = (na_long)(byte_reverse((long)ch) * 256 + 128);
	}
	WITH->actflag = true;
      }
    }
    break;
  }
}

#undef map
#undef map_s
#undef map_c









/* End. */
