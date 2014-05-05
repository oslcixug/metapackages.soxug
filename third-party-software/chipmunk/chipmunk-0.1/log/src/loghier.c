/* Output from p2c, the Pascal-to-C translator */
/* From input file "loghier.text" */


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


/*caged_process='newcom -g $F$M'*/

/* LOG hierarchy module           David Gillespie 1/7/89 */
/* version for LOG 4.1 */






#include <p2c/p2c.h>


#define LOGHIER_G
#include "loghier.h"



Static log_action *lact;

Static log_krec *attrkind;
Static short attrinstof, attrinstname;




Void hier_init(act)
log_action *act;
{
  lact = act;
  attrkind = NULL;
}


boolean commandlabel(lp, cmd, args)
log_lrec *lp;
Char *cmd, *args;
{
  boolean Result;
  long i, j;

  Result = false;
  i = 1;
  j = strlen(lp->name);
  while (i < j && lp->name[i - 1] == ' ')
    i++;
  if (i >= j)
    return Result;
  while (j > i && lp->name[j - 1] == ' ')
    j--;
  if (lp->name[i - 1] == '"') {
    if (j <= i || lp->name[j - 1] != '"')
      return Result;
    strpart(args, lp->name, (int)(i + 1), (int)(j - 1));
    if (((char *) strpbrk(args, " \"")) == NULL) {
      strcpy(cmd, "NAME");
      return true;
    }
    return Result;
  } else {
    if (lp->name[i - 1] != '<' || lp->name[j - 1] != '>')
      return Result;
    do {
      i++;
    } while (lp->name[i - 1] == ' ');
    do {
      j--;
    } while (lp->name[j - 1] == ' ');
    strpart(args, lp->name, (int)i, (int)j);
    i = 1;
    while (i <= strlen(args) && args[i - 1] != ':' && args[i - 1] != ' ')
      i++;
    if (i <= strlen(args))
      args[i - 1] = ' ';
    strword(args, cmd);
    strupper(cmd, cmd);
    return true;
  }
  return Result;
}



log_brec *labelbox(lp, pg)
log_lrec *lp;
short pg;
{
  log_brec *bp, *bp2;
  Char STR2[256];

  bp2 = NULL;
  bp = lact->bbase[pg - 1];
  while (bp != NULL) {  /*find smallest box containing the label*/
    if (P_ibetween((long)bp->x1, (long)lp->x, (long)(bp->x2 - lp->w)) &&
	P_ibetween((long)bp->y1, (long)lp->y, (long)bp->y2)) {
      if (bp2 == NULL ||
	  (bp2->x1 <= bp->x1 && bp->x2 <= bp2->x2 && bp2->y1 <= bp->y1 &&
	   bp->y2 <= bp2->y2))
	bp2 = bp;
      else if (bp->x1 > bp2->x1 || bp2->x2 > bp->x2 || bp->y1 > bp2->y1 ||
	       bp2->y2 > bp->y2) {
	sprintf(STR2, "Warning: Label %s appears in more than one box",
		lp->name);
	(*lact->hook.vmessage)(STR2);
      }
    }
    bp = bp->next;
  }
  return bp2;
}



boolean pointinbox(bp, x, y)
log_brec *bp;
short x, y;
{
  return (bp == NULL || (P_ibetween((long)bp->x1, (long)x, (long)bp->x2) &&
			 P_ibetween((long)bp->y1, (long)y, (long)bp->y2)));
}


boolean gateinbox(bp, g)
log_brec *bp;
log_grec *g;
{
  return (bp == NULL || (P_ibetween((long)bp->x1, (long)g->x, (long)bp->x2) &&
			 P_ibetween((long)bp->y1, (long)g->y, (long)bp->y2)));
}


boolean labelinbox(bp, l)
log_brec *bp;
log_lrec *l;
{
  return (bp == NULL ||
	  (P_ibetween((long)bp->x1, (long)l->x, (long)(bp->x2 - l->w)) &&
	   P_ibetween((long)bp->y1, (long)l->y, (long)bp->y2)));
}


boolean boxinbox(bp, bp2)
log_brec *bp, *bp2;
{
  return (bp == NULL || (bp->x1 <= bp2->x1 && bp2->x2 <= bp->x2 &&
			 bp->y1 <= bp2->y1 && bp2->y2 <= bp->y2));
}


boolean boxtouchbox(bp, bp2)
log_brec *bp, *bp2;
{
  return (bp != NULL && bp2 != NULL &&
	  (P_ibetween((long)bp->x1, (long)bp2->x1, (long)bp->x2) ||
	   P_ibetween((long)bp->x1, (long)bp2->x2, (long)bp->x2)) &&
	  (P_ibetween((long)bp->y1, (long)bp2->y1, (long)bp->y2) ||
	   P_ibetween((long)bp->y1, (long)bp2->y2, (long)bp->y2)));
}



boolean isinstgate(g)
log_grec *g;
{
  if (g->kind == attrkind)
    return (attrinstof != 0 && *g->attr[attrinstof - 1].UU.c != '\0');
  attrkind = g->kind;
  (*lact->hook2->findattrnum)(g->kind, "inst-of", "CA", &attrinstof);
  (*lact->hook2->findattrnum)(g->kind, "gate-name", "CA", &attrinstname);
  return (attrinstof != 0 && *g->attr[attrinstof - 1].UU.c != '\0');
}


boolean isgenericinstgate(g)
log_grec *g;
{
short num;
  if (isinstgate(g)) {
    (*lact->hook2->findattrnum)(g->kind, "is-generic", "V", &num);
    if (num == 0 || g->attr[num - 1].UU.nv != 0)
      return true;
  }
  return false;
}


Char *gateinstof(Result, g)
Char *Result;
log_grec *g;
{
  log_gattrrec *WITH;

  if (isinstgate(g)) {
    WITH = &g->attr[attrinstof - 1];
    if (strlen(WITH->UU.c) > 2 && *WITH->UU.c == '"' &&
	strends(WITH->UU.c, "\""))
      return (strpart(Result, WITH->UU.c, 2, (int)(strlen(WITH->UU.c) - 1L)));
    else
      return strcpy(Result, WITH->UU.c);
  } else
    return strcpy(Result, g->kind->name);
}



Char *gateinstname(Result, g)
Char *Result;
log_grec *g;
{
  if (g->kind != attrkind) {
    attrkind = g->kind;
    (*lact->hook2->findattrnum)(g->kind, "inst-of", "CA", &attrinstof);
    (*lact->hook2->findattrnum)(g->kind, "gate-name", "CA", &attrinstname);
  }
  if (attrinstname > 0)
    return strcpy(Result, g->attr[attrinstname - 1].UU.c);
  else
    return strcpy(Result, "");
}



/* Find a list of all definitions on the page, sorted according to dependencies */
na_strlist *defndirectory(pg)
short pg;
{
  Char buf[256], buf2[256];
  na_strlist *dir, *l1, *l2, **lprev;
  log_lrec *lp;
  log_brec *bp;
  log_grec *g;
  boolean boxed, boxless, errors;
  long numdefs;
  Char STR1[256];
  Char STR3[256];

  dir = NULL;
  errors = false;
  boxed = false;
  boxless = false;
  numdefs = 0;
  lp = lact->lbase[pg - 1];
  while (lp != NULL) {
    if (commandlabel(lp, buf, buf2) && !strcmp(buf, "NAME") &&
	*strcpy(STR1, strltrim(buf2)) != '\0' && *buf2 != '(') {
      if (strlist_cifind(dir, buf2) != NULL) {
	sprintf(STR3, "More than one cell-name label for \"%s\"", buf2);
	(*lact->hook.message)(STR3);
	errors = true;
      } else {
	bp = labelbox(lp, pg);
	if (bp != NULL)
	  boxed = true;
	else
	  boxless = true;
	lprev = &dir;
	g = lact->gbase[pg - 1];
	while (g != NULL) {  /*check inter-dependencies of definitions*/
	  if (gateinbox(bp, g)) {
	    gateinstof(buf, g);
	    do {
	      l1 = strlist_cifind(*lprev, buf);
	      if (l1 != NULL)
		lprev = &l1->next;
	    } while (l1 != NULL);
	  }
	  g = g->next;
	}
	l1 = strlist_insert(lprev, buf2);
	l1->value = (Anyptr)bp;
	numdefs++;
      }
    }
    lp = lp->next;
  }
  if (boxless && numdefs > 1) {
    if (boxed)
      (*lact->hook.message)("Not all definitions have boxes!");
    else
      (*lact->hook.message)(
	"More than one <name: xxx> or \"xxx\" label on the page!");
    errors = true;
  }
  if (!boxless) {
    l1 = dir;
    l2 = NULL;
    while (l1 != NULL && l2 == NULL) {
      l2 = l1->next;
      while (l2 != NULL &&
	     !boxtouchbox((log_brec *)l1->value, (log_brec *)l2->value))
	l2 = l2->next;
      l1 = l1->next;
    }
    if (l2 != NULL) {
      (*lact->hook.message)("Definition boxes intersect!");
      errors = true;
    }
  }
  if (errors)
    strlist_empty(&dir);
  return dir;
}



Void examinetemplate(g, pp, ppsize, isgeneric, pnum, lastnorth, lasteast,
		     lastsouth, lastwest)
log_grec *g;
log_nrec **pp;
long ppsize;
boolean isgeneric;
short **pnum;
long *lastnorth, *lasteast, *lastsouth, *lastwest;
{
  long i, j;
  short xx1, xx2;
  short *numrefs;
  log_krec *kind;
  long FORLIM;

  kind = g->kind;
  xx1 = (kind->x1 + 1) / log_scale0;
  xx2 = P_imax2((kind->x2 - 1L) / log_scale0, (long)xx1);
  if (*pnum == NULL)
    *pnum = Malloc(kind->numpins * 2);
  numrefs = (short *)Malloc(kind->numpins * 2);
  FORLIM = kind->numpins;
  for (i = 0; i < FORLIM; i++)
    numrefs[i] = 1;
  FORLIM = kind->numpins;
  for (i = 0; i < FORLIM; i++) {
    if (numrefs[i] == 1) {
      j = kind->pin[i].c;
      while (j != 0 && j != i + 1) {
	numrefs[i]++;
	numrefs[j - 1] = 32767;
	j = kind->pin[j - 1].c;
      }
    }
  }
  if (isgeneric) {
    *lastnorth = 0;
    FORLIM = kind->numpins;
    for (i = 1; i <= FORLIM; i++) {
      if (g->pin[i - 1]->ref > numrefs[i - 1] &&
	  P_ibetween((long)xx1, (long)kind->pin[i - 1].x, (long)xx2) &&
	  kind->pin[i - 1].y <= 0) {
	(*lastnorth)++;
	j = *lastnorth;
	while (j > 1 && kind->pin[(*pnum)[j - 2] - 1].x > kind->pin[i - 1].x) {
	  (*pnum)[j - 1] = (*pnum)[j - 2];
	  j--;
	}
	(*pnum)[j - 1] = i;
      }
    }
    *lasteast = *lastnorth;
    FORLIM = kind->numpins;
    for (i = 1; i <= FORLIM; i++) {
      if (g->pin[i - 1]->ref > numrefs[i - 1] && kind->pin[i - 1].x > xx2) {
	(*lasteast)++;
	j = *lasteast;
	while (j > *lastnorth + 1 &&
	       kind->pin[(*pnum)[j - 2] - 1].y > kind->pin[i - 1].y) {
	  (*pnum)[j - 1] = (*pnum)[j - 2];
	  j--;
	}
	(*pnum)[j - 1] = i;
      }
    }
    *lastsouth = *lasteast;
    FORLIM = kind->numpins;
    for (i = 1; i <= FORLIM; i++) {
      if (g->pin[i - 1]->ref > numrefs[i - 1] &&
	  P_ibetween((long)xx1, (long)kind->pin[i - 1].x, (long)xx2) &&
	  kind->pin[i - 1].y > 0) {
	(*lastsouth)++;
	j = *lastsouth;
	while (j > *lasteast + 1 &&
	       kind->pin[(*pnum)[j - 2] - 1].x < kind->pin[i - 1].x) {
	  (*pnum)[j - 1] = (*pnum)[j - 2];
	  j--;
	}
	(*pnum)[j - 1] = i;
      }
    }
    *lastwest = *lastsouth;
    FORLIM = kind->numpins;
    for (i = 1; i <= FORLIM; i++) {
      if (g->pin[i - 1]->ref > numrefs[i - 1] && kind->pin[i - 1].x < xx1) {
	(*lastwest)++;
	j = *lastwest;
	while (j > *lastsouth + 1 &&
	       kind->pin[(*pnum)[j - 2] - 1].y < kind->pin[i - 1].y) {
	  (*pnum)[j - 1] = (*pnum)[j - 2];
	  j--;
	}
	(*pnum)[j - 1] = i;
      }
    }
  } else {
    j = 0;
    for (i = 1; i <= ppsize; i++) {
      if (numrefs[i - 1] < 32767) {
	j++;
	(*pnum)[j - 1] = i;
      }
    }
    *lastnorth = 0;
    *lasteast = 0;
    *lastsouth = 0;
    *lastwest = j;
  }
  if (pp != NULL) {
    FORLIM = P_imin2(*lastwest, ppsize);
    for (i = 0; i < FORLIM; i++)
      pp[i] = g->pin[(*pnum)[i] - 1];
  }
  Free(numrefs);
}


typedef struct ginstinfo {
  short instattr, nameattr, dispattr;
  long oldgattrstamp;
  Char *wasdrawn;
} ginstinfo;





Void Log_7_ginst(act)
log_action *act;
{
  ginstinfo *ii;
  Char newname[256];
  boolean drawflag;
  short drawx, drawy;
  na_strlist *sl1;
  log_action *WITH;
  log_gattrrec *WITH1;
  Char STR2[256];
  log_grec *WITH2;
  ginstinfo *WITH3;
  Char STR3[256];

  WITH = act;
  switch (WITH->action) {

  case act_newgate:
  case act_copygate:
    ii = (ginstinfo *)Malloc(sizeof(ginstinfo));
    (*WITH->hook2->findattrnum)(WITH->actgate->kind, "inst-of", "CA",
				&ii->instattr);
    (*WITH->hook2->findattrnum)(WITH->actgate->kind, "gate-name", "CA",
				&ii->nameattr);
    (*WITH->hook2->findattrnum)(WITH->actgate->kind, "disp-inst-name", "B",
				&ii->dispattr);
    ii->wasdrawn = NULL;
    ii->oldgattrstamp = 0;
    WITH->actgate->info = (Anyptr)ii;
    break;

  case act_disposegate:
    ii = (ginstinfo *)WITH->actgate->info;
    strdispose(&ii->wasdrawn);
    Free(ii);
    break;

  case act_configchgate:
    WITH1 = &WITH->actgate->attr[WITH->actx - 1];
    ii = (ginstinfo *)WITH->actgate->info;
    if (WITH->actx == ii->instattr) {
      if (*WITH1->UU.c != '\0' && *WITH1->UU.c != '"' &&
	  !strends(WITH1->UU.c, "\"")) {
	sprintf(STR2, "\"%s\"", WITH1->UU.c);
	(*WITH->hook2->setgattr)(WITH->actgate, (int)WITH->actx, STR2);
      }
    }
    break;

  case act_pass:
    if (WITH->actgate != NULL && WITH->actflag) {
      ii = (ginstinfo *)WITH->actgate->info;
      WITH2 = WITH->actgate;
      WITH3 = ii;
      if (WITH->gattrstamp != WITH3->oldgattrstamp) {
	WITH3->oldgattrstamp = WITH->gattrstamp;
	if (WITH3->nameattr > 0 &&
	    *WITH2->attr[WITH3->nameattr - 1].UU.c != '\0')
	  strcpy(newname, WITH2->attr[WITH3->nameattr - 1].UU.c);
	else if (WITH3->instattr > 0 &&
		 *WITH2->attr[WITH3->instattr - 1].UU.c != '\0')
	  strpart(newname, WITH2->attr[WITH3->instattr - 1].UU.c, 2,
		  (int)(strlen(WITH2->attr[WITH3->instattr - 1].UU.c) - 1L));
	else
	  *newname = '\0';
	if (*newname != '\0') {
	  if (WITH3->dispattr == 0 || WITH2->attr[WITH3->dispattr - 1].blnk) {
	    if (WITH2->rot & 1) {
	      if (m_strwidth(NULL, newname) >
		  WITH2->kind->y2 - WITH2->kind->y1 - 3)
		*newname = '\0';
	    } else {
	      if (m_strwidth(NULL, newname) >
		  WITH2->kind->x2 - WITH2->kind->x1 - 3)
		*newname = '\0';
	    }
	  } else if (!WITH2->attr[WITH3->dispattr - 1].UU.b)
	    *newname = '\0';
	}
	if (*newname == '\0')
	  drawflag = (WITH3->wasdrawn != NULL);
	else
	  drawflag = (WITH3->wasdrawn == NULL ||
		      strcmp(WITH3->wasdrawn, newname));
	if (drawflag && WITH3->wasdrawn != NULL) {
	  drawx = 0;
	  drawy = 0;
	  (*WITH->hook.xform)(WITH->actgate, &drawx, &drawy);
	  (*WITH->hook.hidecursor)();
	  m_color((long)WITH->color.backgr);
	  m_centerstr((long)drawx, drawy - 4L, NULL, WITH3->wasdrawn);
	  (*WITH->hook.unhidecursor)();
	}
	if (drawflag) {
	  if (*newname == '\0')
	    strdispose(&WITH3->wasdrawn);
	  else
	    strchange(&WITH3->wasdrawn, newname);
	}
      } else
	drawflag = false;
      if ((drawflag || WITH->refrflag) && WITH3->wasdrawn != NULL) {
	drawx = 0;
	drawy = 0;
	(*WITH->hook.xform)(WITH->actgate, &drawx, &drawy);
	(*WITH->hook.hidecursor)();
	m_color((long)WITH->color.labeltext);
	m_centerstr((long)drawx, drawy - 4L, NULL, WITH3->wasdrawn);
	(*WITH->hook.unhidecursor)();
      }
    }
    break;

  case act_erasegate:
    ii = (ginstinfo *)WITH->actgate->info;
    if (ii->wasdrawn != NULL) {
      drawx = 0;
      drawy = 0;
      (*WITH->hook.xform)(WITH->actgate, &drawx, &drawy);
      (*WITH->hook.hidecursor)();
      m_color((long)WITH->color.backgr);
      m_centerstr((long)drawx, drawy - 4L, NULL, ii->wasdrawn);
      (*WITH->hook.unhidecursor)();
    }
    break;

  case act_gengate:
    if (!strcmp(WITH->genfunc, "PLOT") && WITH->actgate != NULL) {
      ii = (ginstinfo *)WITH->actgate->info;
      if (ii->wasdrawn != NULL) {
	sl1 = strlist_append(&WITH->actstrlist, "color gate");
	drawx = 0;
	drawy = 0;
	(*WITH->hook2->plainxform)(WITH->actgate, &drawx, &drawy);
	sprintf(STR3, "text %d %d 60 CC \"%s\"", drawx, drawy, ii->wasdrawn);
	sl1 = strlist_append(&WITH->actstrlist, STR3);
      }
    }
    break;

  }
}











/* End. */
