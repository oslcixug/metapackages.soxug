/* Output from p2c, the Pascal-to-C translator */
/* From input file "plot.text" */


/* Chipmunk plotting library.  David Gillespie  12/27/85 */

/*caged_date='I{ Last edit by $U on $X (To add things for lplot) $]'*/
/* Last edit by dave on Mar 5, 1989 9:06 pm (To add things for lplot) */
/* Last edit by dave on Jan 6, 1989 11:14 pm (To add things for lplot) */
/* Last edit by dave on Nov 27, 1988 6:46 pm (To add things for lplot) */
/* Last edit by dave on Aug 9, 1988 4:54 am (To add things for lplot) */
/* Last edit by dave on Aug 5, 1988 5:15 pm (To add things for lplot) */
/* Last edit by dave on Feb 29, 1988 11:50 pm (To make imports internal) */
/* Last edit by dave on Dec 9, 1987 6:11 am (To make imports internal) */
/* Last edit by dave on Nov 8, 1987 9:29 am (To make imports internal) */
/* Last edit by dave on Oct 21, 1987 5:28 pm (To add this line) */
/* Last edit by maryann on Dec 11, 1986 11:16 pm (To add this line) */
/* Last edit by dave on Nov 15, 1986 2:04 am (To add this line) */
/* Last edit by dave on Nov 5, 1986 12:23 pm (To add this line) */


/*$debug${*/

#ifndef CHIPLIB
# define CHIPLIB "/usr/chipmunk/lib" 
#endif

#ifndef PSLIB
# define PSLIB CHIPLIB
#endif

#ifndef FONTLIB
# define FONTLIB CHIPLIB
#endif



#include <p2c/p2c.h>


#define PLOT_G
#include <p2c/plot.h>



#ifndef GENERAL_1_H
#include <p2c/general_1.h>
#endif

#ifndef GENERAL_2_H
#include <p2c/general_2.h>
#endif

#ifndef HPIB_2_H
#include <p2c/hpib_2.h>
#endif

/*file support*/

#ifndef FS_H
#include <p2c/fs.h>
#endif

#ifndef NEWCI_H
#include <p2c/newci.h>
#endif

/*graphics,*/

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif


#define scale_          200

#define pi              3.14159265

#define unitspcm        402
#define qualvel         10

#define ps_scale        10

#define ps_fudge        (ps_scale * 30)
#define ps_maxx         (ps_scale * 791)
#define ps_maxy         (ps_scale * 611)

#define ff_scale        10




typedef Char pac8[8];


typedef enum {
  out_normal, out_hpgl, out_ps, out_ff
} outmodekind;

typedef struct point {
  unsigned circ : 1;
  Signed int px : 7;
  unsigned drw : 1;
  Signed int py : 7;
} point;

typedef point ptarr[1000000L];

typedef struct chardesc {
  char xleft, xright;
  uchar width;
  char ybase;
  uchar reserved, numpts;
  ptarr v;
} chardesc;

typedef chardesc *indexarr[1000000L];

typedef union swap2 {
        short ival;
        unsigned char c[2];
} swap2;

typedef union swap4 {
        long ival;
        unsigned char c[4];
} swap4;


Local short getshortsw(c)
  char *c;
  {
    swap2 s;

    if (bigendian)
      {
        s.c[1] = c[1];
        s.c[0] = c[0];
      }
    else
      {
        s.c[0] = c[1];
        s.c[1] = c[0];
      }

     return s.ival;
}

Local long getintsw(c)
char *c;
{
    swap4 s;

    if (bigendian)
      {
        s.c[0] = c[0];
        s.c[1] = c[1];
        s.c[2] = c[2];
        s.c[3] = c[3];
      }
    else
      {
	s.c[0] = c[3];
        s.c[1] = c[2];
        s.c[2] = c[1];
        s.c[3] = c[0];
      }

     return s.ival;
}


Static short w1x, w1y, w2x, w2y, pp1x, pp1y, pp2x, pp2y, p1x, p1y, p2x, p2y,
	     pltc, pltcolr, pltpat, plotter;
Static long csnum, csden, csn, csnxx, csnxy, csnyx, csnyy, sbo, spo, sod,
	    csns, csnc, csah, csav, csad, csd, csdsc, cssc, cscsc, orgx,
            orgy, pltx, plty, xpos, ypos, sc;
Static long b1x, b1y, b2x, b2y;
Static double csth;
Static boolean quality, fast, rotate, fancy, pendn, spendn, hasellipse;
Static Char prefchar[11], ctrlchars[11], ctrllchars[11];
Static outmodekind outmode;

Static long ps_curx, ps_cury, ps_curstyle, ps_veccount;
Static boolean ps_flag, ps_needinit;
Static Char *ff_title;

Static _PROCEDURE proc;
Static plot_devrec rec;

Static chardesc **ix;
Static plot_fontrec *curft;
Static long loadft[9];
Static Char fontfn[256], headerfn[256]=PSLIB"/texstuff.ps";
Static boolean headerincflag;
Static FILE **outf, **foutf;
Static boolean foutfopen;
Static double ps_linewidth = 7.0;


/*$if false$
procedure graphics_crt_zlptn(c1, c2 : char; n : shortint);
   external;
$end$*/


Static Void copyfile(fn)
Char *fn;
{
  FILE *f;
  Char buf[256];
  boolean okay;
  Char *TEMP;

  f = NULL;
  TRY(try1);
    if (f != NULL)
      f = freopen(fn, "r", f);
    else
      f = fopen(fn, "r");
    if (f == NULL) {
      P_escapecode = -10;
      P_ioresult = FileNotFound;
      goto _Ltry1;
    }
    okay = true;
  RECOVER2(try1,_Ltry1);
    okay = false;
    printf("Unable to open %s\n", fn);
  ENDTRY(try1);
  if (okay) {
    while (fgets(buf, 256, f) != NULL) {
      TEMP = (char *) strchr(buf, '\n');
      if (TEMP != NULL)
	*TEMP = 0;
      plot_out(buf);
    }
    if (f != NULL)
      fclose(f);
    f = NULL;
  }
  if (f != NULL)
    fclose(f);
}


Void plot_out(s)
Char *s;
{
  Char STR1[256];

  if (outf == NULL) {
    if (outmode == out_hpgl)
      P_writestring(plotter + 700L, s);
    return;
  }
  if (!headerincflag && !strncmp(s, "% $ Include", 11L))
    copyfile(strcpy(STR1, s + 12));
  else
    fprintf(*outf, "%s\n", s);
}



Void plot_in(s)
Char *s;
{
}





Static Void hpgl_proc(rec)
plot_devrec *rec;
{
  Char s[256];
  long i;

  switch (rec->act) {

  case plot_act_finish:
    plot_out("SP0;DF;");
    break;

  case plot_act_clip:
    sprintf(s, "IW%ld,%ld,%ld,%ld;", rec->x1, rec->y1, rec->x2, rec->y2);
    i = strlen(s);
    s[i] = '\0';
    plot_out(s);
    break;

  case plot_act_raise:
    plot_out("PU;");
    break;

  case plot_act_lower:
    plot_out("PD;");
    break;

  case plot_act_view:
    plot_out("PA0,32767;SP0;");
    break;

  case plot_act_quality:
    plot_velocity((double)qualvel);
    break;

  case plot_act_move:
  case plot_act_draw:
    sprintf(s, "PA%ld,%ld;", rec->x1, rec->y1);
    i = strlen(s);
    s[i] = '\0';
    plot_out(s);
    break;

  case plot_act_box:
    if (fancy) {
      sprintf(s, "PA%ld,%ld;", rec->x1, rec->y1);
      i = strlen(s);
      s[i] = '\0';
      plot_out(s);
      sprintf(s, "EA%ld,%ld;", rec->x2, rec->y2);
      i = strlen(s);
      s[i] = '\0';
      plot_out(s);
      s[0] = 'R';
      if (rec->q1.U1.i1 != 0 && !fast)
	plot_out(s);
      rec->q1.U1.i1 = -1;
    }
    break;

  case plot_act_color:
    sprintf(s, "SP%ld;", rec->x1);
    i = strlen(s);
    s[i] = '\0';
    plot_out(s);
    break;

  case plot_act_style:
    if (rec->x1 == 0)
      plot_out("LT;");
    else {
      sprintf(s, "LT%ld,2;", rec->x1);
      i = strlen(s);
      s[i] = '\0';
      plot_out(s);
    }
    break;

  case plot_act_pattern:
    if (fancy && rec->x1 != pltpat && rec->x1 != 0) {
      pltpat = rec->x1;
      if (rotate) {
	switch (rec->x1) {

	case 2:
	  plot_out("FT2;");
	  break;

	case 3:
	  plot_out("FT3,60,90;");
	  break;

	case 4:
	  plot_out("FT3,60,45;");
	  break;

	case 5:
	  plot_out("FT3,60,0;");
	  break;

	case 6:
	  plot_out("FT4,60,0;");
	  break;

	case 7:
	  plot_out("FT4,60,45;");
	  break;

	default:
	  plot_out("FT1;");
	  break;
	}
      } else {
	switch (rec->x1) {

	case 2:
	  plot_out("FT2;");
	  break;

	case 3:
	  plot_out("FT3,60,0;");
	  break;

	case 4:
	  plot_out("FT3,60,45;");
	  break;

	case 5:
	  plot_out("FT3,60,90;");
	  break;

	case 6:
	  plot_out("FT4,60,0;");
	  break;

	case 7:
	  plot_out("FT4,60,45;");
	  break;

	default:
	  plot_out("FT1;");
	  break;
	}
      }
    }
    break;
  }
}


#define vectorlimit     1000

#define sizefudge       1.4


/* Local variables for ps_proc: */
struct LOC_ps_proc {

  Char s[256];
} ;

Local Void init(LINK)
struct LOC_ps_proc *LINK;
{
  Char STR1[256], STR3[256];

  plot_out("save");
  sprintf(STR1, "%% $ Include %s", headerfn);
  plot_out(STR1);
  sprintf(LINK->s, "[%ld %ld %ld %ld]",
	  (long)(pp1y / ps_scale), (long)(pp1x / ps_scale),
	  (long)(pp2y / ps_scale), (long)(pp2x / ps_scale));
  if (rotate)
    plot_out(LINK->s);
  else {
    sprintf(STR3, "{%s} Not-In-TeX", LINK->s);
    plot_out(STR3);
    sprintf(STR1, "{[%ld %ld %ld %ld]} In-TeX",
	    (long)(pp1x / ps_scale), (long)(pp1y / ps_scale),
	    (long)(pp2x / ps_scale), (long)(pp2y / ps_scale));
    plot_out(STR1);
  }
  plot_out("/My-MBB exch def");
  plot_out("My-MBB TeX-do-square-scale");
  plot_out(
    "/M {moveto} def /D {lineto} def /S {stroke} def /F {fill} def /G {setgray} def");
  plot_out(
    "/E {matrix currentmatrix 5 1 roll translate scale 0 0 1 0 360 arc setmatrix} def");
  plot_out("/TF {findfont setfont} def /TB {gsave} def");
  plot_out("/TY {currentpoint grestore currentpoint pop exch pop -.8} def");
  plot_out("/TX {3 1 roll exch sub} def /TJ {exch rmoveto} def");
  plot_out("/TNJ {pop 0} def /TCJ {.5 mul} def /TW {stringwidth rmoveto} def");
  plot_out(
    "/TBP {0 .5 rmoveto .75 .75 scale} def /TBB {0 -.2 rmoveto .75 .75 scale} def");
  plot_out(
    "/TFP {1.3333 dup scale 0 -.5 rmoveto} def /TFB {1.3333 dup scale 0 .2 rmoveto} def");
  plot_out(
    "/BOX { 2 copy M 3 index exch D 3 -1 roll 2 index D exch D closepath } def");
  sprintf(STR1, "%g setlinewidth 1 setlinejoin", ps_linewidth);
  plot_out(STR1);
  sprintf(STR1, "1 %ld div dup scale newpath", (long)ps_scale);
  plot_out(STR1);
  if (rotate) {
    sprintf(STR3, "{180 rotate %ld %ld translate} In-TeX",
	    pp1y - pp2y - ps_fudge * 2L, pp1x - pp2x - ps_fudge * 2L);
    plot_out(STR3);
  } else {
    sprintf(STR3, "{90 rotate 0 %ld translate} In-TeX",
	    pp1x - pp2x - ps_fudge * 2L);
    plot_out(STR3);
  }
  ps_curstyle = 0;
  ps_veccount = 0;
  ps_needinit = false;
}

Local Void dostroke(LINK)
struct LOC_ps_proc *LINK;
{
  if (ps_needinit)
    init(LINK);
  if (!ps_flag)
    plot_out("S");
  ps_veccount = 0;
  ps_flag = true;
}

Local Void writecoord(x, LINK)
long x;
struct LOC_ps_proc *LINK;
{
  sprintf(LINK->s + strlen(LINK->s), "%ld", x);
  strcat(LINK->s, " ");
}

Local Void writereal(x, LINK)
double x;
struct LOC_ps_proc *LINK;
{
  long i;

  i = strlen(LINK->s) + 1;
  LINK->s[255] = '\0';
  sprintf(LINK->s + i - 1, "%1.2f", x);
  i = strlen(LINK->s);
  LINK->s[i] = '\0';
  strcat(LINK->s, " ");
}

/* Local variables for showstring: */
struct LOC_showstring {
  struct LOC_ps_proc *LINK;
  Char msg[256];
  boolean hassubscr;
} ;

/* Local variables for showstr: */
struct LOC_showstr {
  struct LOC_showstring *LINK;
  boolean showit;
  long where;
  Char buf[256];
} ;

Local Void dumpbuf(LINK)
struct LOC_showstr *LINK;
{
  Char STR1[256];

  if (*LINK->buf == '\0')
    return;
  sprintf(LINK->buf, "(%s", strcpy(STR1, LINK->buf));
  switch (LINK->where) {

  case 1:
    sprintf(LINK->buf, "TBP %s", strcpy(STR1, LINK->buf));
    break;

  case -1:
    sprintf(LINK->buf, "TBB %s", strcpy(STR1, LINK->buf));
    break;
  }
  strcat(LINK->buf, ")");
  if (LINK->showit)
    strcat(LINK->buf, " show");
  else
    strcat(LINK->buf, " TW");
  switch (LINK->where) {

  case 1:
    strcat(LINK->buf, " TFP");
    break;

  case -1:
    strcat(LINK->buf, " TFB");
    break;
  }
  plot_out(LINK->buf);
  *LINK->buf = '\0';
}

Local Void showstr(showit_, LINK)
boolean showit_;
struct LOC_showstring *LINK;
{
  /*cheap subset of $ formats*/
  struct LOC_showstr V;
  long i;

  V.LINK = LINK;
  V.showit = showit_;
  V.where = 0;
  *V.buf = '\0';
  i = 1;
  while (i <= strlen(LINK->msg)) {
    if (LINK->msg[i - 1] == '$' && LINK->hassubscr) {
      i++;
      if (LINK->msg[i - 1] == '$')
	strcat(V.buf, "$");
      else {
	dumpbuf(&V);
	switch (LINK->msg[i - 1]) {

	case 'P':
	case 'p':
	  V.where = 1;
	  break;

	case 'B':
	case 'b':
	  V.where = -1;
	  break;

	case 'X':
	case 'x':   /*ignore*/
	  V.where = 0;
	  break;
	}
      }
    } else if (LINK->msg[i - 1] == '\\' || LINK->msg[i - 1] == ')' ||
	       LINK->msg[i - 1] == '(') {
      strcat(V.buf, "\\");
      sprintf(V.buf + strlen(V.buf), "%c", LINK->msg[i - 1]);
    } else if (LINK->msg[i - 1] < ' ' || (LINK->msg[i - 1] & 255) >= '\177') {
      strcat(V.buf, "\\");
      sprintf(V.buf + strlen(V.buf), "%c", LINK->msg[i - 1] / 64 + 48);
      sprintf(V.buf + strlen(V.buf), "%c", ((LINK->msg[i - 1] / 8) & 7) + 48);
      sprintf(V.buf + strlen(V.buf), "%c", (LINK->msg[i - 1] & 7) + 48);
    } else
      sprintf(V.buf + strlen(V.buf), "%c", LINK->msg[i - 1]);
    i++;
  }
  dumpbuf(&V);
}

Local Void showstring(msg_, x, y, mode, font, num, den, sc, theta, LINK)
Char *msg_;
long x, y, mode, font, num, den, sc;
double theta;
struct LOC_ps_proc *LINK;
{
  struct LOC_showstring V;

  V.LINK = LINK;
  strcpy(V.msg, msg_);
  dostroke(LINK);
  V.hassubscr = (!strcmp(prefchar, "$") && strlen(ctrlchars) >= 7 &&
		 ctrlchars[0] == 'X' && ctrlchars[5] == 'B' &&
		 ctrlchars[6] == 'P');
  strcpy(LINK->s, "gsave /");
  switch (font) {

  case 2:
    strcat(LINK->s, "Times-Roman");
    break;

  case 3:
    strcat(LINK->s, "Times-Italic");
    break;

  case 4:
    strcat(LINK->s, "Times-Bold");
    break;

  case 5:
    strcat(LINK->s, "Times-BoldItalic");
    break;

  case 7:
  case 8:
  case 9:
  case 10:
  case 12:
  case 13:
    strcat(LINK->s, "ZapfChancery-MediumItalic");
    break;

  case 11:
    strcat(LINK->s, "AvantGarde-Book");
    break;

  default:
    strcat(LINK->s, "Helvetica");
    break;
  }
  strcat(LINK->s, " TF ");
  writecoord(y, LINK);
  writecoord(ps_maxx - x, LINK);
  strcat(LINK->s, "M ");
  sprintf(LINK->s + strlen(LINK->s), "%g", theta - 90);
  strcat(LINK->s, " rotate ");
  sprintf(LINK->s + strlen(LINK->s), "%g",
	  (double)num / den * 21 * sc / scale_ * sizefudge);
  strcat(LINK->s, " dup scale");
  plot_out(LINK->s);
  if (mode != 1) {
    plot_out("TB");
    showstr(false, &V);
    strcpy(LINK->s, "TY");
    switch (mode) {

    case 1:
    case 2:
    case 3:
      strcat(LINK->s, " TNJ");
      break;

    case 4:
    case 8:
    case 9:
      strcat(LINK->s, " TCJ");
      break;
    }
    strcat(LINK->s, " TX");
    switch (mode) {

    case 1:
    case 7:
    case 8:
      strcat(LINK->s, " TNJ");
      break;

    case 2:
    case 6:
    case 9:
      strcat(LINK->s, " TCJ");
      break;
    }
    strcat(LINK->s, " TJ");
    plot_out(LINK->s);
  }
  showstr(true, &V);
  plot_out("grestore");
}



Static Void ps_proc(rec)
plot_devrec *rec;
{
  struct LOC_ps_proc V;
  long i, FORLIM;

  switch (rec->act) {

  case plot_act_init:
    plot_out("%!PS-Adobe-2.0");
    plot_out("% PostScript output from PLOT library");
    ps_needinit = true;
    ps_flag = true;
    break;

  case plot_act_finish:
    dostroke(&V);
    plot_out("{showpage} Not-In-TeX");
    plot_out("restore");
    plot_out("% End of PLOT-generated PostScript");
    break;

  case plot_act_move:
    dostroke(&V);
    ps_curx = rec->x1;
    ps_cury = rec->y1;
    break;

  case plot_act_draw:
    ps_veccount++;
    if (ps_veccount == vectorlimit)   /*DAVE 10/31/86*/
      dostroke(&V);
    *V.s = '\0';
    if (ps_flag) {
      writecoord(ps_cury, &V);
      writecoord(ps_maxx - ps_curx, &V);
      strcat(V.s, "M ");
    }
    writecoord(rec->y1, &V);
    writecoord(ps_maxx - rec->x1, &V);
    strcat(V.s, "D");
    plot_out(V.s);
    ps_flag = false;
    ps_curx = rec->x1;
    ps_cury = rec->y1;
    break;

  case plot_act_color:
    dostroke(&V);
    *V.s = '\0';
/* p2c: plot.text, line 663:
 * Note: Using % for possibly-negative arguments [317] */
    writereal((rec->x1 - 1) % 6 / 6.0, &V);
    strcat(V.s, "G");
    plot_out(V.s);
    break;

  case plot_act_ellipse:
    dostroke(&V);
    *V.s = '\0';
    writereal(rec->q2.r, &V);
    writereal(rec->q1.r, &V);
    writecoord(rec->y1, &V);
    writecoord(ps_maxx - rec->x1, &V);
    strcat(V.s, "E");
    plot_out(V.s);
    if (rec->x2 != 0)
      plot_out("F");
    else
      plot_out("S");
    break;

  case plot_act_text:
    if (rec->y2 < 14) {
      showstring(rec->q1.U16.sp1, rec->x1, rec->y1, rec->x2, rec->y2,
		 rec->q3.U1.i1, rec->q3.U1.i2, rec->q1.U1.i2, rec->q2.r, &V);
      rec->q1.U1.i1 = 0;
    }
    break;

  case plot_act_box:
    dostroke(&V);
    *V.s = '\0';
    writecoord(rec->y1, &V);
    writecoord(ps_maxx - rec->x1, &V);
    writecoord(rec->y2, &V);
    writecoord(ps_maxx - rec->x2, &V);
    strcat(V.s, "BOX ");
    if (rec->q1.U1.i1 == 0)
      strcat(V.s, "S");
    else
      strcat(V.s, "F");
    plot_out(V.s);
    rec->q1.U1.i1 = -1;
    break;

  case plot_act_polygon:
    dostroke(&V);
    *V.s = '\0';
    writecoord((long)rec->q1.U25.lap2[0], &V);
    writecoord(ps_maxx - (long)rec->q1.U25.lap1[0], &V);
    strcat(V.s, "M");
    plot_out(V.s);
    FORLIM = rec->x1;
    for (i = 1; i < FORLIM; i++) {
      *V.s = '\0';
      writecoord((long)rec->q1.U25.lap2[i], &V);
      writecoord(ps_maxx - (long)rec->q1.U25.lap1[i], &V);
      strcat(V.s, "D");
      plot_out(V.s);
    }
    if (rec->x2 == 0)
      plot_out("closepath S");
    else
      plot_out("closepath F");
    rec->x1 = 0;
    break;

  case plot_act_style:
    if (rec->x1 != ps_curstyle) {
      dostroke(&V);
      switch (rec->x1) {

      case 1:   /*#               */
	plot_out("[10 90] 0 setdash");
	break;

      case 2:   /*########        */
	plot_out("[100] 0 setdash");
	break;

      case 3:   /*############    */
	plot_out("[150 50] 0 setdash");
	break;

      case 4:   /*############# # */
	plot_out("[150 20 10 20] 0 setdash");
	break;

      case 5:   /*############ ## */
	plot_out("[150 10 30 10] 0 setdash");
	break;

      case 6:   /*######### ## ## */
	plot_out("[100 10 30 20 30 10] 0 setdash");
	break;

      default:
	plot_out("[] 0 setdash");
	break;
      }
      ps_curstyle = rec->x1;
    }
    break;
  }
}

#undef vectorlimit
#undef sizefudge


/* Local variables for ff_proc: */
struct LOC_ff_proc {
  Char s[256];
} ;

Local Void dofigs(col, c1, c2, c3)
long col;
Char c1, c2, c3;
{
  Char STR1[12];
  Char STR2[256];

  sprintf(STR1, "F ``%c` [](", c1);
  plot_out(STR1);
  sprintf(STR2, "O (%ld )", col);
  plot_out(STR2);
  plot_out("B (-160 -160 160 160 )");
  plot_out(")");
  sprintf(STR1, "F ``%c` [](", c2);
  plot_out(STR1);
  sprintf(STR2, "O (%ld )", col);
  plot_out(STR2);
  plot_out("C (0 0 160 )");
  plot_out(")");
  sprintf(STR1, "F ``%c` [](", c3);
  plot_out(STR1);
  sprintf(STR2, "O (%ld )", col);
  plot_out(STR2);
  plot_out("L [7 ](0 160 -160 -160 160 -160 )");
  plot_out(")");
}

Local Void init_(LINK)
struct LOC_ff_proc *LINK;
{
  Char STR2[256];

  dofigs(2L, 'A', 'C', 'E');
  dofigs(5L, 'B', 'D', 'F');
  sprintf(STR2, "F ``%s` [](", ff_title);
  plot_out(STR2);
  ps_needinit = false;
}

Local Void dostroke_(LINK)
struct LOC_ff_proc *LINK;
{
  if (ps_needinit)
    init_(LINK);
  if (!ps_flag)
    plot_out(")");
  ps_flag = true;
}

Local Void writecoord_(x, LINK)
long x;
struct LOC_ff_proc *LINK;
{
  sprintf(LINK->s + strlen(LINK->s), "%ld", x);
  strcat(LINK->s, " ");
}

Local Void writereal_(x, LINK)
double x;
struct LOC_ff_proc *LINK;
{
  writecoord_((long)floor(x + 0.5), LINK);
}

Local Void writestr(s1, LINK)
Char *s1;
struct LOC_ff_proc *LINK;
{
  Char ch;

  ch = '`';
  while (strposc(s1, ch, 1L) != 0)
    ch--;
  sprintf(LINK->s + strlen(LINK->s), "%c", ch);
  strcat(LINK->s, s1);
  sprintf(LINK->s + strlen(LINK->s), "%c", ch);
}



Static Void ff_proc(rec)
plot_devrec *rec;
{
  struct LOC_ff_proc V;
  Char STR1[256];

  switch (rec->act) {

  case plot_act_init:
    ps_needinit = true;
    ps_flag = true;
    ff_title = (Char *)Malloc(256);
    strcpy(ff_title, "UNTITLED");
    break;

  case plot_act_finish:
    dostroke_(&V);
    plot_out(")");
    break;

  case plot_act_move:
    dostroke_(&V);
    ps_curx = rec->x1;
    ps_cury = rec->y1;
    break;

  case plot_act_draw:
    *V.s = '\0';
    if (ps_flag) {
      strcpy(V.s, "L (");
      writecoord_(ps_curx, &V);
      writecoord_(ps_cury, &V);
    }
    writecoord_(rec->x1, &V);
    writecoord_(rec->y1, &V);
    plot_out(strrtrim(strcpy(STR1, V.s)));
    ps_flag = false;
    ps_curx = rec->x1;
    ps_cury = rec->y1;
    break;

  case plot_act_color:
    dostroke_(&V);
    strcpy(V.s, "O (");
    writecoord_(rec->x1, &V);
    strcat(V.s, ")");
    plot_out(V.s);
    break;

  case plot_act_ellipse:
    dostroke_(&V);
    if (rec->q1.r == rec->q2.r) {
      strcpy(V.s, "C (");
      writecoord_(rec->x1, &V);
      writecoord_(rec->y1, &V);
      writereal_(rec->q1.r, &V);
      strcat(V.s, ")");
    } else {
      strcpy(V.s, "E (");
      writecoord_(rec->x1, &V);
      writecoord_(rec->y1, &V);
      writereal_(rec->q1.r, &V);
      writereal_(rec->q2.r, &V);
      strcat(V.s, " 0 360 0)");
    }
    plot_out(V.s);
    break;

  case plot_act_text:
    dostroke_(&V);
    if (rec->y2 == 14 && strlen(rec->q1.U16.sp1) == 1) {   /*ugh!*/
      strcpy(V.s, "i (``");
      strcat(V.s, rec->q1.U16.sp1);
      strcat(V.s, "` 0 ");
      writecoord_(rec->x1, &V);
      writecoord_(rec->y1, &V);
      strcat(V.s, "1 1 1 )");
    } else {
      strcpy(V.s, "T [");
      if (rec->x2 != 1) {
	writecoord_(1L, &V);
	writecoord_(rec->x2, &V);
      }
      if (rec->q2.r != 0) {
	writecoord_(4L, &V);
	writereal_(rec->q2.r, &V);
      }
      strcat(V.s, "](``");
      strcat(V.s, rec->q1.U16.sp1);
      strcat(V.s, "` ");
      writecoord_(rec->x1, &V);
      writecoord_(rec->y1, &V);
      strcat(V.s, ")");
    }
    plot_out(V.s);
    rec->q1.U1.i1 = 0;
    break;
  }
}



/*$if false$
procedure graphics_proc(var rec : plot_devrec);
   var
      i : integer;
   begin
      with rec do
         case rec.act of
            plot_act_clip:
               begin
                  crt_zdlim(x1, x2, y1, y2);
               end;
            plot_act_move:
               begin
                  crt_zdmov(x1, y1);
                  if x1 < plot_bx1 then plot_bx1 := x1;
                  if x1 > plot_bx2 then plot_bx2 := x1;
                  if y1 < plot_by1 then plot_by1 := y1;
                  if y1 > plot_by2 then plot_by2 := y1;
               end;
            plot_act_draw:
               begin
                  crt_zddrw(x1, y1);
                  if x1 < plot_bx1 then plot_bx1 := x1;
                  if x1 > plot_bx2 then plot_bx2 := x1;
                  if y1 < plot_by1 then plot_by1 := y1;
                  if y1 > plot_by2 then plot_by2 := y1;
               end;
            plot_act_color:
               begin
                  if (x1 >= 1) and (x1 <= plot_numscreenpens) then
                     i := plot_screenpen[x1]
                  else
                     i := x1;
                  crt_zcolr(i);
               end;
            plot_act_style:
               begin
                  case x1 of
                     1: i := hex('8000');       {#               }
                     2: i := hex('FF00');       {########        }
                     3: i := hex('FFF0');       {############    }
                     4: i := hex('FFFA');       {############# # }
                     5: i := hex('FFF6');       {############ ## }
                     6: i := hex('FFD6');       {######### ## ## }
                     otherwise
                        i := 0;
                  end;
                  if i = 0 then
                     crt_zlstl(0)
                  else
                     begin
                        graphics_crt_zlptn(chr(i div 256), chr(i mod 256), 15);
                        crt_zlstl(15);
                     end;
               end;
            otherwise ;
         end;
   end;
$end$*/




Static Void graphics_proc(rec)
plot_devrec *rec;
{
  long i;

  switch (rec->act) {

  case plot_act_clip:
    m_clip(rec->x1, rec->y1, rec->x2, rec->y2);
    break;

  case plot_act_move:
    m_move(rec->x1, rec->y1);
    if (rec->x1 < plot_bx1)
      plot_bx1 = rec->x1;
    if (rec->x1 > plot_bx2)
      plot_bx2 = rec->x1;
    if (rec->y1 < plot_by1)
      plot_by1 = rec->y1;
    if (rec->y1 > plot_by2)
      plot_by2 = rec->y1;
    break;

  case plot_act_draw:
    m_draw(rec->x1, rec->y1);
    if (rec->x1 < plot_bx1)
      plot_bx1 = rec->x1;
    if (rec->x1 > plot_bx2)
      plot_bx2 = rec->x1;
    if (rec->y1 < plot_by1)
      plot_by1 = rec->y1;
    if (rec->y1 > plot_by2)
      plot_by2 = rec->y1;
    break;

  case plot_act_polygon:
    if (rec->x2 == 0)
      m_drawpoly(rec->x1, (int *)rec->q1.U25.lap1, (int *)rec->q1.U25.lap2);
    else
      m_fillpoly(rec->x1, (int *)rec->q1.U25.lap1, (int *)rec->q1.U25.lap2);
    rec->x1 = 0;
    break;

  case plot_act_color:
    if (rec->x1 >= 1 && rec->x1 <= plot_numscreenpens)
      i = plot_screenpen[rec->x1 - 1];
    else
      i = rec->x1;
    m_color(i);
    break;

  case plot_act_style:
    switch (rec->x1) {

    case 1:   /*#               */
      i = 0x8000L;
      break;

    case 2:   /*########        */
      i = 0xff00L;
      break;

    case 3:   /*############    */
      i = 0xfff0L;
      break;

    case 4:   /*############# # */
      i = 0xfffaL;
      break;

    case 5:   /*############ ## */
      i = 0xfff6L;
      break;

    case 6:   /*######### ## ## */
      i = 0xffd6L;
      break;

    default:
      i = 0;
      break;
    }
    if (i == 0)
      m_linestyle(0L);
    else {
      m_setlinestyle(15L, i);
      m_linestyle(15L);
    }
    break;
  }
}






Static Void ucase(s)
Char *s;
{
  long i, FORLIM;

  FORLIM = strlen(s);
  for (i = 0; i < FORLIM; i++) {
    if (islower(s[i]))
      s[i] -= 32;
  }
}



Static Void lcase(s)
Char *s;
{
  long i, FORLIM;

  FORLIM = strlen(s);
  for (i = 0; i < FORLIM; i++) {
    if (isupper(s[i]))
      s[i] += 32;
  }
}



Static Void init__()
{
  char STR1[256];
  curft = NULL;
  w1x = pp1x;
  w1y = pp1y;
  w2x = pp2x;
  w2y = pp2y;
  rotate = false;
  sc = scale_;
  p1x = 0;
  p1y = 0;
  orgx = 0;
  orgy = 0;
  pltc = -1;
  pltcolr = 1;
  pltpat = -1;
  pltx = LONG_MAX;
  plty = LONG_MAX;
  sbo = 1;
  spo = 2;
  sod = 4;
  csnum = 1;
  csden = 1;
  cssc = 1;
  cscsc = 1;
  csns = 0;
  csnc = scale_;
  csth = 0.0;
  csah = 1;
  csav = 1;
  csad = 1;
  csd = 0;
  pendn = false;
  spendn = false;
  quality = false;
  fast = false;
  fancy = false;
  *prefchar = '\0';
  strcpy(ctrlchars, "\0\001\002\003\004\005\006\007\b");
  strcpy(ctrllchars, ctrlchars);
  /* sprintf(STR1,"%s/%s",PSLIB,"texstuff.ps"); */
  /* strcpy(headerfn, STR1); */
  /* headerincflag = false; */
  foutfopen = false;
  hasellipse = false;
  ff_title = NULL;
}



Static Void setsize(sizecode)
Char sizecode;
{
  switch (sizecode) {

  case 'A':
  case 'a':
    pp1x = 250;
    pp1y = 596;
    pp2x = 10250;
    pp2y = 7796;
    break;

  case 'B':
  case 'b':
    pp1x = 522;
    pp1y = 256;
    pp2x = 15722;
    pp2y = 10259;
    break;

  case 'U':
  case 'u':
    /* blank case */
    break;

  default:
    printf("Error in PLOT_INIT: Size code %c not supported\n", sizecode);
    _Escape(-1);
    break;
  }
}



Void plot_p1p2(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  pp1x = x1;
  pp1y = y1;
  pp2x = x2;
  pp2y = y2;
}




Void plot_figtitle(title_)
Char *title_;
{
  Char title[256];
  long i;

  strcpy(title, title_);
  if (ff_title == NULL)
    return;
  i = strlen(title);
  while (i > 0 && title[i - 1] != '/' && title[i - 1] != ':')
    i--;
  if (i > 0)
    strcpy(title, title + i);
  i = strlen(title);
  while (i > 0 && title[i - 1] != '.')
    i--;
  if (i > 0) {
    title[i - 1] = '\0';
/* p2c: plot.text, line 1186:
 * Note: Modification of string length may translate incorrectly [146] */
  }
  if (*title != '\0')
    strcpy(ff_title, title);
}



Void plot_init(addr, sizecode)
long addr;
Char sizecode;
{
  long i;
  Char s[256];
  Char ch;
  boolean exists;

  proc.proc = (Anyptr)hpgl_proc;
  proc.link = (Anyptr)NULL;
  outmode = out_hpgl;
  outf = NULL;
  if (addr == 0)
    plotter = 5;
  else
    plotter = addr;
  exists = false;
  TRY(try2);
    set_timeout(7, 0.5);
    plot_out("DF;");
    exists = true;
  RECOVER(try2);
    if (P_escapecode != -26)
      _Escape(P_escapecode);
  ENDTRY(try2);
  set_timeout(7, 60.0);
  abort_hpib(7);
  if (!exists)
    _Escape(1);
  if (sizecode == 'x' || sizecode == 'X') {
    plot_out("OP;");
    P_readstring(plotter + 700L, s);
    sscanf(s, "%hd%c%hd%c%hd%c%hd%ln", &pp1x, &ch, &pp1y, &ch, &pp2x, &ch,
	   &pp2y, &i);
    i++;
  } else
    setsize(sizecode);
  /*   plot_out('OO;');
     readstring(700+plotter, s);   */
  plot_out("PU;SP0;");
  init__();
  /*   if (strlen(s) >= 9) and (s[9] = '1') then
        fancy := true;    */
}



Void plot_init_fast()
{
  plot_init(0L, 'X');
  plot_initfonts();
  plot_selfont(2L);
}





Void plot_warminitscreen(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  long i;

  proc.proc = (Anyptr)graphics_proc;
  proc.link = (Anyptr)NULL;
  outmode = out_normal;
  outf = NULL;
  pp1x = x1;
  pp1y = y1;
  pp2x = x2;
  pp2y = y2;
  /*$if false$
      crt_zlstl(0);
      crt_zcolr(1);
      crt_zdlim(pp1x, pp2x, pp1y, pp2y);
$end$*/
  m_nolinestyle();
  m_nolinewidth();
  m_colormode((long)m_normal);
  m_color(1L);
  m_notransform();
  m_clip(x1, y1, x2, y2);
  for (i = 1; i <= plot_numscreenpens; i++)
    plot_screenpen[i - 1] = i;
  init__();
}



Void plot_initscreen(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  /*$if false$
var k : short_int;
      crt_setup(k);
      crt_init;
$end$*/
  m_init_graphics_nopen();
  if (x2 <= 0)
    x2 += m_across;
  if (y2 <= 0)
    y2 += m_down;
  plot_warminitscreen(x1, y1, x2, y2);
}


Void plot_initscreen_fast()
{
  plot_initscreen(0L, 0L, 0L, 0L);
  plot_initfonts();
  plot_selfont(2L);
}




Void plot_initps(f)
FILE **f;
{
  proc.proc = (Anyptr)ps_proc;
  proc.link = (Anyptr)NULL;
  outmode = out_ps;
  plotter = 0;
  plot_p1p2(ps_fudge, ps_fudge, ps_maxx - ps_fudge, ps_maxy - ps_fudge);
  init__();
  outf = f;
  fancy = false;
  rec.act = plot_act_init;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  hasellipse = true;
}


#define maxz             1000



Void plot_initff(f)
FILE **f;
{
  proc.proc = (Anyptr)ff_proc;
  proc.link = (Anyptr)NULL;
  outmode = out_ff;
  plotter = 0;
  plot_p1p2((long)(-maxz), (long)(-maxz), (long)maxz, (long)maxz);
  init__();
  sc = ff_scale * scale_;
  outf = f;
  fancy = false;
  rec.act = plot_act_init;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  hasellipse = true;
}

#undef maxz



Void plot_initfile(f, sizecode, hascircles)
FILE **f;
Char sizecode;
boolean hascircles;
{
  proc.proc = (Anyptr)hpgl_proc;
  proc.link = (Anyptr)NULL;
  outmode = out_hpgl;
  plotter = 0;
  if (sizecode == 'x' || sizecode == 'X') {
    printf("\007Error in PLOT_INIT: Auto-sizing not available for files\n");
    _Escape(-1);
  }
  setsize(sizecode);
  init__();
  outf = f;
  fancy = hascircles;
  plot_out("DF;PU;");
}


Void plot_initgen(fn_)
Char *fn_;
{
  Char fn[256];

  strcpy(fn, fn_);
  foutf = (FILE **)Malloc(sizeof(FILE *));
  *foutf = NULL;
  if (*foutf != NULL)
    *foutf = freopen(fn, "w", *foutf);
  else
    *foutf = fopen(fn, "w");
  if (*foutf == NULL)
    _EscIO(FileNotFound);
  strupper(fn, fn);
  if (strends(fn, ".PS"))
    plot_initps(foutf);
  else if (strends(fn, ".FF"))
    plot_initff(foutf);
  else
    plot_initfile(foutf, 'A', false);
  plot_figtitle(fn);
  foutfopen = true;
}


Void plot_initgen_fast(fn)
Char *fn;
{
  plot_initgen(fn);
  plot_initfonts();
  plot_selfont(2L);
}


Void plot_initfile_fast(fn_)
Char *fn_;
{
  Char fn[256];

  strcpy(fn, fn_);
  if (*fn == '\0')
    strcpy(fn, "plot.hpgl");
  newci_fixfname(fn, "hpgl", "");
  plot_initgen_fast(fn);
}


Void plot_initps_fast(fn_)
Char *fn_;
{
  Char fn[256];

  strcpy(fn, fn_);
  if (*fn == '\0') {
    sprintf(fn, "plot%ld.ps", timers_sysclock() % 1000);
/* p2c: plot.text, line 1418:
 * Note: Using % for possibly-negative arguments [317] */
  }
  newci_fixfname(fn, "ps", "");
  plot_initgen_fast(fn);
}



Void plot_inituser(devproc)
_PROCEDURE devproc;
{
  proc = devproc;
  outmode = out_normal;
  outf = NULL;
  rec.q1.U1.i1 = 0;
  rec.q1.U1.i2 = 0;
  rec.act = plot_act_init;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  pp1x = rec.x1;
  pp1y = rec.y1;
  pp2x = rec.x2;
  pp2y = rec.y2;
  init__();
  hasellipse = P_getbits_UB(rec.q1.b, 0, 0, 3);
}



Static Void window_(x1, y1, x2, y2, rot)
long x1, y1, x2, y2;
boolean rot;
{
  long sc1, i;
  short tx;

  w1x = x1;
  w1y = y1;
  w2x = x2;
  w2y = y2;
  if (x1 > x2) {
    i = x1;
    x1 = x2;
    x2 = i;
  }
  if (y1 > y2) {
    i = y1;
    y1 = y2;
    y2 = i;
  }
  rotate = (rot && outmode != out_ff);
  if (rotate) {
    p1y = pp1x;
    p1x = pp1y;
    p2y = pp2x;
    p2x = pp2y;
  } else {
    p1x = pp1x;
    p1y = pp1y;
    p2x = pp2x;
    p2y = pp2y;
  }
  orgx = x1;
  orgy = y1;
  sc = labs((p2x - p1x) * scale_ / (x2 - x1));
  sc1 = labs((p2y - p1y) * scale_ / (y2 - y1));
  if (sc1 < sc) {
    if (rotate) {
      tx = ((p2x - p1x) - (x2 - x1) * (p2y - p1y) / (y2 - y1)) / 2;
      p1x += tx; 
      p2x -= tx;
    } else
      p1x = (p1x + p2x - (x2 - x1) * (p2y - p1y) / (y2 - y1)) / 2;
    sc = sc1;
    return;
  }
  if (rotate)
    p1y = (p1y + p2y - (y2 - y1) * (p2x - p1x) / (x2 - x1)) / 2;
  else
    p1y = (p1y + p2y - (y2 - y1) * (p2x - p1x) / (x2 - x1)) / 2;
}



Void plot_window(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  window_(x1, y1, x2, y2, labs(x2 - x1) < labs(y2 - y1));
}



Void plot_nrwindow(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  window_(x1, y1, x2, y2, false);
}



Void plot_pwindow(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  window_(x1, y1, x2, y2, pp2x - pp1x >= pp2y - pp1y);
}



Void plot_lwindow(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  window_(x1, y1, x2, y2, pp2x - pp1x < pp2y - pp1y);
}




Void plot_getwindow(x1, y1, x2, y2)
long *x1, *y1, *x2, *y2;
{
  *x1 = w1x;
  *y1 = w1y;
  *x2 = w2x;
  *y2 = w2y;
}



Void plot_quality()
{
  quality = true;
  rec.act = plot_act_quality;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



Void plot_fast()
{
  fast = true;
  rec.act = plot_act_fast;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



Void plot_velocity(vel)
double vel;
{
  Char s[21];
  long i;
  Char STR1[256];
  Char STR3[256];

  if (outmode != out_hpgl)
    return;
  if (vel == 0) {
    plot_out("VS;");
    return;
  }
  sprintf(s, "%20.2f", vel);
  i = strlen(s) + 1;
  s[i - 1] = '\0';
  sprintf(STR3, "VS%s;", strcpy(STR1, strltrim(s)));
/* p2c: plot.text, line 1572:
 * Note: Modification of string length may translate incorrectly [146] */
  plot_out(STR3);
}



Static Void raisepen()
{
  if (pendn) {
    rec.act = plot_act_raise;
    if (proc.link != NULL)
      (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
    else
      (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  }
  pendn = false;
}



Static Void lowerpen()
{
  if (!pendn) {
    rec.act = plot_act_lower;
    if (proc.link != NULL)
      (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
    else
      (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  }
  pendn = true;
}



Void plot_lift()
{
  if (outf == NULL)
    raisepen();
}



Void plot_view()
{
  raisepen();
  rec.act = plot_act_view;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  pltc = -1;
  pltx = LONG_MAX;
  plty = LONG_MAX;
}



Void plot_finish()
{
  raisepen();
  rec.act = plot_act_finish;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  if (foutfopen) {
    if (*foutf != NULL)
      fclose(*foutf);
    *foutf = NULL;
  }
  pltc = -1;
}



Static Void pattern(pat)
long pat;
{
  rec.act = plot_act_pattern;
  rec.x1 = pat;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



Static Void pmove(x, y)
long x, y;
{
  if (x == pltx && y == plty)
    return;
  raisepen();
  if (rotate) {
    rec.x1 = p1y + y;
    rec.y1 = p2x - x;
  } else {
    rec.x1 = p1x + x;
    rec.y1 = p1y + y;
  }
  rec.act = plot_act_move;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  pltx = x;
  plty = y;
}



Static Void pdraw(x, y)
long x, y;
{
  lowerpen();
  if (rotate) {
    rec.x1 = p1y + y;
    rec.y1 = p2x - x;
  } else {
    rec.x1 = p1x + x;
    rec.y1 = p1y + y;
  }
  rec.act = plot_act_draw;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  pltx = x;
  plty = y;
}



Static Void pcircle(pat, rx, ry)
long pat;
double rx, ry;
{
  Char s[256];
  long nseg, i, cx, cy, r;
  double theta, dth;

  rx = fabs(rx);
  ry = fabs(ry);
  r = (long)floor(rx + 0.5);
  if (ry > rx)
    r = (long)floor(ry + 0.5);
  if (r < 2)
    r = 2;
  if (fast)
    nseg = r / 16 + 4;
  else
    nseg = r / 8 + 5;
  if (quality)
    nseg *= 3;
  if (nseg > 90)
    nseg = 90;
  if (fancy && outmode == out_hpgl && pat != 0 && rx == ry && !fast) {
    pattern(pat);
    sprintf(s, "CI%ld,%ld;", r, 360 / nseg);
    i = strlen(s);
    s[i] = '\0';
    plot_out(s);
    sprintf(s, "WG%ld,0,360,%ld;", r, 360 / nseg);
    i = strlen(s);
    s[i] = '\0';
    plot_out(s);
    return;
  }
  if (hasellipse) {
    raisepen();
    if (rotate) {
      rec.x1 = p1y + plty;
      rec.y1 = p2x - pltx;
      rec.q1.r = ry;
      rec.q2.r = rx;
    } else {
      rec.x1 = p1x + pltx;
      rec.y1 = p1y + plty;
      rec.q1.r = rx;
      rec.q2.r = ry;
    }
    rec.x2 = pat;
    rec.act = plot_act_ellipse;
    if (proc.link != NULL)
      (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
    else
      (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
    return;
  }
  cx = pltx;
  cy = plty;
  dth = (pi + pi) / nseg;
  theta = 0.0;
  pmove(cx + (long)rx, cy);
  for (i = 1; i <= nseg; i++) {
    theta += dth;
    pdraw(cx + (long)(cos(theta) * rx), cy + (long)(sin(theta) * ry));
  }
  pmove(cx, cy);
}



Void plot_color(c)
long c;
{
  pltcolr = c;
}



Static Void defpen()
{
  if (pltc == pltcolr)
    return;
  plot_lift();
  rec.act = plot_act_color;
  rec.x1 = pltcolr;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  pltc = pltcolr;
}


Void plot_prepare()
{
  defpen();
}



Void plot_clip()
{
  long x1, y1, x2, y2;

  x1 = (w1x - orgx) * sc / scale_;
  x2 = (w2x - orgx) * sc / scale_;
  y1 = (w1y - orgy) * sc / scale_;
  y2 = (w2y - orgy) * sc / scale_;
  if (rotate) {
    rec.x1 = p1y + y1;
    rec.x2 = p1y + y2;
    rec.y1 = p1x - x1;
    rec.y2 = p1x + x2;
  } else {
    rec.x1 = p1x + x1;
    rec.x2 = p1x + x2;
    rec.y1 = p1y + y1;
    rec.y2 = p1y + y2;
  }
  rec.act = plot_act_clip;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



Void plot_noclip()
{
  rec.x1 = pp1y;
  rec.x2 = pp1y;
  rec.y1 = pp1x;
  rec.y2 = pp1x;
  rec.act = plot_act_clip;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



long plot_cm(cm)
double cm;
{
  long i;

  i = (long)(cm * unitspcm * scale_ / sc);
  if (i <= 0)
    i = 1;
  return i;
}



Void plot_xform(x, y, xx, yy)
long x, y, *xx, *yy;
{
  x = (x - orgx) * sc / scale_;
  y = (y - orgy) * sc / scale_;
  if (rotate) {
    *xx = p1y + y;
    *yy = p2x - x;
  } else {
    *xx = p1x + x;
    *yy = p1y + y;
  }
}


Void plot_rxform(x, y, xx, yy)
double x, y;
long *xx, *yy;
{
  long ix, iy;

  ix = (long)floor((x - orgx) * sc / scale_ + 0.5);
  iy = (long)floor((y - orgy) * sc / scale_ + 0.5);
  if (rotate) {
    *xx = p1y + iy;
    *yy = p2x - ix;
  } else {
    *xx = p1x + ix;
    *yy = p1y + iy;
  }
}



Void plot_unxform(xx, yy, x, y)
long xx, yy, *x, *y;
{
  if (rotate) {
    *y = xx - p1y;
    *x = p2x - yy;
  } else {
    *x = xx - p1x;
    *y = yy - p1y;
  }
  *x = (*x * scale_ + sc / 2) / sc + orgx;
  *y = (*y * scale_ + sc / 2) / sc + orgy;
}



Void plot_relxform(x, y, xx, yy)
long x, y, *xx, *yy;
{
  x = x * sc / scale_;
  y = y * sc / scale_;
  if (rotate) {
    *xx = y;
    *yy = -x;
  } else {
    *xx = x;
    *yy = y;
  }
}


Void plot_rrelxform(x, y, xx, yy)
double x, y;
long *xx, *yy;
{
  long ix, iy;

  ix = (long)floor(x * sc / scale_ + 0.5);
  iy = (long)floor(y * sc / scale_ + 0.5);
  if (rotate) {
    *xx = iy;
    *yy = -ix;
  } else {
    *xx = ix;
    *yy = iy;
  }
}



Void plot_relunxform(xx, yy, x, y)
long xx, yy, *x, *y;
{
  if (rotate) {
    *y = xx;
    *x = -yy;
  } else {
    *x = xx;
    *y = yy;
  }
  *x = (*x * scale_ + sc / 2) / sc;
  *y = (*y * scale_ + sc / 2) / sc;
}



Void plot_linestyle(num)
long num;
{
  rec.act = plot_act_style;
  rec.x1 = num;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
}



Void plot_linewidth(n)
double n;
{
  ps_linewidth = n;
}



Void plot_move(x, y)
long x, y;
{
  xpos = x;
  ypos = y;
  pmove((x - orgx) * sc / scale_, (y - orgy) * sc / scale_);
}


Void plot_rmove(x, y)
double x, y;
{
  xpos = (long)floor(x + 0.5);
  ypos = (long)floor(y + 0.5);
  pmove((long)floor((x - orgx) * sc / scale_ + 0.5),
	(long)floor((y - orgy) * sc / scale_ + 0.5));
}



Void plot_draw(x, y)
long x, y;
{
  defpen();
  xpos = x;
  ypos = y;
  pdraw((x - orgx) * sc / scale_, (y - orgy) * sc / scale_);
}


Void plot_rdraw(x, y)
double x, y;
{
  defpen();
  xpos = (long)floor(x + 0.5);
  ypos = (long)floor(y + 0.5);
  pdraw((long)floor((x - orgx) * sc / scale_ + 0.5),
	(long)floor((y - orgy) * sc / scale_ + 0.5));
}



Void plot_moverel(dx, dy)
long dx, dy;
{
  plot_move(xpos + dx, ypos + dy);
}



Void plot_drawrel(dx, dy)
long dx, dy;
{
  plot_draw(xpos + dx, ypos + dy);
}



Void plot_line(x1, y1, x2, y2)
long x1, y1, x2, y2;
{
  defpen();
  plot_move(x1, y1);
  plot_draw(x2, y2);
}



Void plot_roundbox(x1, y1, x2, y2, rx, ry, pat)
long x1, y1, x2, y2, rx, ry, pat;
{
  defpen();
  pattern(pat);
  rec.act = plot_act_box;
  plot_xform(x1, y1, &rec.x1, &rec.y1);
  plot_xform(x2, y2, &rec.x2, &rec.y2);
  plot_relxform(rx, ry, &rec.q2.U1.i1, &rec.q2.U1.i2);
  rec.q1.U1.i1 = pat;
  rec.q1.U1.i2 = 0;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  if (rec.q1.U1.i1 != pat)
    return;
  plot_move(x1, y1);
  plot_draw(x1, y2);
  plot_draw(x2, y2);
  plot_draw(x2, y1);
  plot_draw(x1, y1);
}


Void plot_rroundbox(x1, y1, x2, y2, rx, ry, pat)
double x1, y1, x2, y2, rx, ry;
long pat;
{
  defpen();
  pattern(pat);
  rec.act = plot_act_box;
  plot_rxform(x1, y1, &rec.x1, &rec.y1);
  plot_rxform(x2, y2, &rec.x2, &rec.y2);
  plot_rrelxform(rx, ry, &rec.q2.U1.i1, &rec.q2.U1.i2);
  rec.q1.U1.i1 = pat;
  rec.q1.U1.i2 = 0;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  if (rec.q1.U1.i1 != pat)
    return;
  plot_rmove(x1, y1);
  plot_rdraw(x1, y2);
  plot_rdraw(x2, y2);
  plot_rdraw(x2, y1);
  plot_rdraw(x1, y1);
}


Void plot_box(x1, y1, x2, y2, pat)
long x1, y1, x2, y2, pat;
{
  plot_roundbox(x1, y1, x2, y2, 0L, 0L, pat);
}



Void plot_circle(x1, y1, r, pat)
long x1, y1, r, pat;
{
  defpen();
  plot_move(x1, y1);
  r = r * sc / scale_;
  pcircle(pat, (double)r, (double)r);
}



Void plot_rellipse(x1, y1, rx, ry, pat)
long x1, y1;
double rx, ry;
long pat;
{
  defpen();
  plot_move(x1, y1);
  rx = rx * sc / scale_;
  ry = ry * sc / scale_;
  pcircle(pat, rx, ry);
}


Void plot_rrellipse(x1, y1, rx, ry, pat)
double x1, y1, rx, ry;
long pat;
{
  defpen();
  plot_rmove(x1, y1);
  rx = rx * sc / scale_;
  ry = ry * sc / scale_;
  pcircle(pat, rx, ry);
}


/* Local variables for plot_rbezier: */
struct LOC_plot_rbezier {
  long epsilon;
} ;

Local Void bezier(x1, y1, x2, y2, x3, y3, x4, y4, LINK)
long x1, y1, x2, y2, x3, y3, x4, y4;
struct LOC_plot_rbezier *LINK;
{
  long x5, y5, x7, y7, x8, y8, x9, y9, x10, y10;
  double y43, x43, y42, x42;

  /*   if (abs(x1 - x2) < epsilon) and (abs(y1 - y2) < epsilon) and
        (abs(x1 - x3) < epsilon) and (abs(y1 - y3) < epsilon) and
        (abs(x1 - x4) < epsilon) and (abs(y1 - y4) < epsilon) then  */
  y43 = y4 - y3;
  x43 = x4 - x3;   /*avoid overflows*/
  y42 = y4 - y2;
  x42 = x4 - x2;
  if (fabs(y43 * (x3 - x1) - x43 * (y3 - y1)) < LINK->epsilon &&
      fabs(y42 * (x2 - x1) - x42 * (y2 - y1)) < LINK->epsilon)
  {   /*Ron Goodman's method*/
    pdraw((x4 + 8) / 16, (y4 + 8) / 16);
    return;
  }
  x5 = (x1 + x2) / 2;
  y5 = (y1 + y2) / 2;
  x7 = (x3 + x4) / 2;
  y7 = (y3 + y4) / 2;
  x8 = (x1 + x2 * 2 + x3) / 4;
  y8 = (y1 + y2 * 2 + y3) / 4;
  x9 = (x2 + x3 * 2 + x4) / 4;
  y9 = (y2 + y3 * 2 + y4) / 4;
  x10 = (x8 + x9) / 2;
  y10 = (y8 + y9) / 2;
  bezier(x1, y1, x5, y5, x8, y8, x10, y10, LINK);
  bezier(x10, y10, x9, y9, x7, y7, x4, y4, LINK);
}




Void plot_rbezier(x1, y1, x2, y2, x3, y3, x4, y4, epsilon_)
double x1, y1, x2, y2, x3, y3, x4, y4;
long epsilon_;
{
  struct LOC_plot_rbezier V;
  long smalle, xx, yy, ix1, iy1, ix2, iy2, ix3, iy3, ix4, iy4;
  double reps;
  Char STR1[256], STR2[256];
  Char STR3[256];

  V.epsilon = epsilon_;
  defpen();
  if (outmode == out_ps) {
    plot_rxform(x1, y1, &xx, &yy);
    sprintf(STR1, "%ld %ld M", yy, ps_maxx - xx);
    plot_out(STR1);
    plot_rxform(x2, y2, &xx, &yy);
    sprintf(STR2, "%ld %ld", yy, ps_maxx - xx);
    plot_out(STR2);
    plot_rxform(x3, y3, &xx, &yy);
    sprintf(STR3, "%ld %ld", yy, ps_maxx - xx);
    plot_out(STR3);
    plot_rxform(x4, y4, &xx, &yy);
    sprintf(STR1, "%ld %ld", yy, ps_maxx - xx);
    plot_out(STR1);
    plot_out("curveto");
    return;
  }
  xpos = (long)floor(x4 + 0.5);
  ypos = (long)floor(y4 + 0.5);
  ix1 = (long)floor((x1 - orgx) * 16 * sc / scale_ + 0.5) - 8;
  ix2 = (long)floor((x2 - orgx) * 16 * sc / scale_ + 0.5) - 8;
  ix3 = (long)floor((x3 - orgx) * 16 * sc / scale_ + 0.5) - 8;
  ix4 = (long)floor((x4 - orgx) * 16 * sc / scale_ + 0.5) - 8;
  iy1 = (long)floor((y1 - orgy) * 16 * sc / scale_ + 0.5) - 8;
  iy2 = (long)floor((y2 - orgy) * 16 * sc / scale_ + 0.5) - 8;
  iy3 = (long)floor((y3 - orgy) * 16 * sc / scale_ + 0.5) - 8;
  iy4 = (long)floor((y4 - orgy) * 16 * sc / scale_ + 0.5) - 8;
  pmove((ix1 + 8) / 16, (iy1 + 8) / 16);
  /*    smalle := imax(20, (imax4(x1,x2,x3,x4) - imin4(x1,x2,x3,x4) +
                          imax4(y1,y2,y3,y4) - imin4(y1,y2,y3,y4)) div 3200);   */
  smalle = abs(pp1x - pp2x) + abs(pp1y - pp2y);
  if (V.epsilon == 0)
    V.epsilon = smalle * 3;
  else if (V.epsilon < smalle)
    V.epsilon = smalle;
  reps = V.epsilon;
  bezier(ix1, iy1, ix2, iy2, ix3, iy3, ix4, iy4, &V);
}



Void plot_bezier(x1, y1, x2, y2, x3, y3, x4, y4, epsilon)
long x1, y1, x2, y2, x3, y3, x4, y4, epsilon;
{
  plot_rbezier((double)x1, (double)y1, (double)x2, (double)y2, (double)x3,
	       (double)y3, (double)x4, (double)y4, epsilon);
}



Void plot_polygon(n, x, y, pat)
long n;
long *x, *y;
long pat;
{
  long *xx, *yy;
  long i;
  Anyptr xxptr=&xx,yyptr=&yy;

  if (n < 2)
    return;
  defpen();
  pattern(pat);
/* p2c: plot.text, line 2219: Note: Call to NA_ALLOC [501] */
  na_alloc(xxptr, n * sizeof(long));
/* p2c: plot.text, line 2220: Note: Call to NA_ALLOC [501] */
  na_alloc(yyptr, n * sizeof(long));
  for (i = 0; i < n; i++)
    plot_xform(x[i], y[i], &xx[i], &yy[i]);
  rec.act = plot_act_polygon;
  rec.x1 = n;
  rec.x2 = pat;
  rec.q1.U17.p1 = (Anyptr)xx;
  rec.q1.U17.p2 = (Anyptr)yy;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  if (rec.x1 != n)
    return;
  plot_move(x[n - 1], y[n - 1]);
  for (i = 0; i < n; i++)
    plot_draw(x[i], y[i]);
}





Void plot_initfonts()
{
  char STR1[256];

  plot_lastchar = 0;
  ix = NULL;
  plot_fontbase = NULL;
  plot_namebase = NULL;
  sprintf(STR1,"%s/%s",FONTLIB,"plot.font");
  strcpy(fontfn, STR1);
  P_expset(loadft, 0L);
}



Void plot_fontfile(fn)
Char *fn;
{
  strcpy(fontfn, fn);
}


Void plot_headerfile(fn, incflag)
Char *fn;
boolean incflag;
{
  strcpy(headerfn, fn);
  headerincflag = incflag;
}



Static Void warning(msg)
Char *msg;
{
  puts(msg);
  printf("\007Press return to continue.\n");
  scanf("%*[^\n]");
  getchar();
}


typedef struct _REC_findexarr {
  long ptr;
  short siz;
} _REC_findexarr;

typedef _REC_findexarr findexarr[1000000L];

typedef struct fontdesc {
  long ptr;
  uchar num, height, attr, numchrs;
} fontdesc;

typedef fontdesc ffontarr[1000000L];

typedef struct fchardesc {
  Char ch;
  uchar reserved;
  short num;
} fchardesc;

typedef struct fname {
  pac8 nam;
  short num;
} fname;

typedef fname fnamearr[1000000L];

 Void readchardesc(f,fd,siz)
 int f;
 chardesc *fd;
 int siz;
 {
    uchar d;
     point p1,*pp1;
     int i;
     uchar x[2];
     if (read(f, fd, 6)<=0)
             fprintf(stderr,"file read error (readchardesc)\n");
     pp1 = fd->v;
     siz -= 6;
     for (i=0;i<(siz/2);i++) {
         if (read(f, x, 2) <= 0)
             fprintf(stderr,"file read error (readchardesc)\n");
         pp1->drw = (x[1] >>7) & 1;
         pp1->py = (Signed int)(x[1] & 0x7f);
   pp1->circ = (x[0] >>7) & 1;
         pp1->px = (Signed int)(x[0]  & 0x7f);
         pp1 += 1;
   }
 }


Void plot_loadfonts(which_)
long *which_;
{
  long which[9];
  FILE *f;
  _REC_findexarr *fix;
  fname *fnm;
  fontdesc *fft;
  plot_namerec *nm, *nm1;
  plot_fontrec *ft, *ft1;
  fchardesc cd;
  short i, j;
  uchar ch;
  long ver, ixp, nmp, ftp, offset, p, p2;
  chardesc **oldix;
  short oldnc, newnc, nn, nf;       
  chardesc *loadme;
  Char fn[256], fn2[256];
  boolean all, warned, failed;
  long SET[257];
  Char STR1[256];
  short FORLIM;
  long SET1[257];
  short TEMP;
  short FORLIM1;
  Char STR4[256];
  point *pp1;
  chardesc *cd1;

  P_setcpy(which, which_);
  f = NULL;
  all = (*which == 0L);
  P_setdiff(which, which, loadft);
  strcpy(fn, fontfn);
  while (*fn != '\0' && (all || *which != 0L)) {
    strword(fn, fn2);
    newci_fixfname(fn2, "font", "");
    failed = true;
    TRY(try3);
      if (f != NULL)
#ifdef OS2
        f = freopen(fn2, "rb", f);
#else
        f = freopen(fn2, "r", f);
#endif  
      else
#ifdef OS2
        f = fopen(fn2, "rb");
#else
        f = fopen(fn2, "r");
#endif
      if (f == NULL) {
	P_escapecode = -10;
	P_ioresult = FileNotFound;
	goto _Ltry3;
      }
      if (f != NULL) {
/* p2c: plot.text, line 2335: Note: OPEN does not specify a name [181] */
	rewind(f);
      } else
	f = tmpfile();
      if (f == NULL) {
	P_escapecode = -10;
	P_ioresult = FileNotFound;
	goto _Ltry3;
      }
      failed = false;
    RECOVER2(try3,_Ltry3);
      if (P_escapecode != -10)
	_Escape(P_escapecode);
      sprintf(STR1, "Warning:  Unable to open font file %s", fn2);
      warning(STR1);
    ENDTRY(try3);
    if (failed)
      continue;
/* p2c: plot.text, line 2344: Note: Can't interpret size in NA_NEW [174] */
    loadme = (chardesc *)Malloc(4L);   /*dummy address*/
    fread(STR1, sizeof(long), 1, f);
    ver = (long)getintsw(STR1);
    fread(STR1, sizeof(long), 1, f);
    ixp = (long)getintsw(STR1);
    fread(STR1, sizeof(long), 1, f);
    nmp = (long)getintsw(STR1);
    fread(STR1, sizeof(long), 1, f);
    ftp = (long)getintsw(STR1);
    if (ver != 1) {
      printf("Font file %s is incorrect version\n", fn2);
      _Escape(-1);
    }
    fseek(f, ixp - 1, 0);
    oldnc = plot_lastchar;
    fread(STR1, 2, 1, f);
    newnc = getshortsw(STR1);
    plot_lastchar += newnc;
    oldix = ix;
    ix = (chardesc **)Malloc(plot_lastchar * sizeof(chardesc *));
    if (oldix != NULL) {
      for (i = 0; i < oldnc; i++)
	ix[i] = oldix[i];
      Free(oldix);
    }
    offset = oldnc;
/* p2c: plot.text, line 2367: Note: Can't interpret size in NA_NEW [174] */
    fix = (_REC_findexarr *)Malloc(newnc * sizeof(_REC_findexarr));
/* p2c: plot.text, line 2368:
 * Note: Can't interpret size in FREADBYTES [174] */
    for (i=0;i<newnc;i++) {
     fread(STR1, 4, 1, f);
     fix[i].ptr = (long)getintsw(STR1);
     fread(STR1, 2, 1, f);
     fix[i].siz = getshortsw(STR1);
    }
    if (all) {
      FORLIM = plot_lastchar;
      for (i = offset; i < FORLIM; i++)
	ix[i] = loadme;
    } else {
      FORLIM = plot_lastchar;
      for (i = offset; i < FORLIM; i++)
	ix[i] = NULL;
    }
    warned = false;
    fseek(f, ftp - 1, 0);
    fread(STR1, 2, 1, f);
    nf = getshortsw(STR1);
/* p2c: plot.text, line 2378: Note: Can't interpret size in NA_NEW [174] */
    fft = (fontdesc *)Malloc(nf * sizeof(fontdesc));
/* p2c: plot.text, line 2379:
 * Note: Can't interpret size in FREADBYTES [174] */
    if (bigendian)
      {
	for (i=0;i<nf;i++) {
	  fread(&(fft[i].ptr), 4, 1, f);
	  fread(&(fft[i].num),1,1,f);
	  fread(&(fft[i].height),1,1,f);
	  fread(&(fft[i].attr),1,1,f);
	  fread(&(fft[i].numchrs),1,1,f);
	}
      }
    else
      {
	for (i=0;i<nf;i++) {
	  fread(STR1, 4, 1, f);
	  fft[i].ptr = getintsw(STR1);
	  fread(&(fft[i].num),1,1,f);
	  fread(&(fft[i].height),1,1,f);
	  fread(&(fft[i].attr),1,1,f);
	  fread(&(fft[i].numchrs),1,1,f);
	}
      }
    for (i = 0; i < nf; i++) {
      if (all || P_inset(fft[i].num, which)) {
	P_remset(which, fft[i].num);
	P_addset(loadft, fft[i].num);
	ft = (plot_fontrec *)Malloc(sizeof(plot_fontrec));
	ft->next = plot_fontbase;
	plot_fontbase = ft;
	ft->num = fft[i].num;
	ft->height = fft[i].height;
	ft->attr = fft[i].attr;
	for (TEMP = '\0'; TEMP <= 255; TEMP++) {
	  ch = TEMP;
/* p2c: plot.text, line 2392: Note: Character >= 128 encountered [281] */
	  ft->arr[ch] = -1;
	}
	fseek(f, fft[i].ptr - 1, 0);
	FORLIM1 = fft[i].numchrs;
	for (j = 1; j <= FORLIM1; j++) {
          fread(&(cd.ch),1,1,f);
          fread(&(cd.reserved),1,1,f);
          fread(STR1,2,1,f);
          cd.num = getshortsw(STR1);
	  ft->arr[cd.ch] = cd.num + offset;
	  ix[cd.num + offset - 1] = loadme;
	}
/* p2c: plot.text, line 2397:
 * Note: Can't interpret size in FREADBYTES [174] */
	ft1 = plot_fontbase->next;
	while (ft1 != NULL && ft1->num != ft->num)
	  ft1 = ft1->next;
	if (ft1 != NULL) {
	  if (!warned)
	    printf("Warning: two fonts named");
	  warned = true;
	  printf(" %d", ft->num);
	}
      }
    }
    if (warned)
      printf(".\n");
    Free(fft);
    warned = false;
    if (all || P_inset(0, which)) {
      fseek(f, nmp - 1, 0);
      fread(STR1, 2, 1, f);
      nn = getshortsw(STR1);
/* p2c: plot.text, line 2420: Note: Can't interpret size in NA_NEW [174] */
      fnm = (fname *)Malloc(nn * sizeof(fname));
/* p2c: plot.text, line 2421:
 * Note: Can't interpret size in FREADBYTES [174] */
      for (i=0;i<nn;i++) {
             fread(fnm[i].nam,8,1,f); /* read chars */
             fread(STR1, 2, 1, f);
             fnm[i].num = getshortsw(STR1);
          }
      for (i = 0; i < nn; i++) {
	nm = (plot_namerec *)Malloc(sizeof(plot_namerec));
	nm->next = plot_namebase;
	plot_namebase = nm;
	nm->num = fnm[i].num + offset;
	ix[nm->num - 1] = loadme;
	nm->s[8] = '\0';
	strmove(8, fnm[i].nam, 1, nm->s, 1);
	strcpy(nm->s, strrtrim(strcpy(STR1, nm->s)));
	nm1 = plot_namebase->next;
	while (nm1 != NULL && strcmp(nm1->s, nm->s))
	  nm1 = nm1->next;
	if (nm1 != NULL) {
	  if (!warned)
	    printf("Warning: two characters named");
	  warned = true;
	  printf(" %s", nm->s);
	}
      }
      Free(fnm);
    }
    if (warned)
      printf(".\n");
    p = -1;
    for (i = 0; i < newnc; i++) {
      if (ix[i + offset] == loadme) {
	p2 = fix[i].ptr;
     if (p2 != p) {
       fseek(f, p2 - 1, 0);
       p = p2;
	}
/* p2c: plot.text, line 2455: Note: Can't interpret size in NA_NEW [174] */
	ix[i + offset] = (chardesc *)Malloc(sizeof(point)*fix[i].siz+1);
/* p2c: plot.text, line 2456:
 * Note: Can't interpret size in FREADBYTES [174] */

	/* readchardesc(fileno(f),(chardesc *)ix[i+offset],fix[i].siz); */
	/* code below replaces above commented-out line */

	cd1 = (chardesc *)ix[i + offset];
	fread(cd1,6,1,f); /* read chars */
	pp1 = cd1->v;
	for (j=0;j<((fix[i].siz-6)/2);j++) {
	  fread(STR1,2,1,f); /* reading fields */
	  pp1[j].drw = (STR1[1] >>7) & 1;
	  pp1[j].py = (Signed int)(STR1[1] & 0x7f);
	  pp1[j].circ = (STR1[0] >>7) & 1;
	  pp1[j].px = (Signed int)(STR1[0]  & 0x7f);
	}

	/* end of replaced code */    

	p += fix[i].siz;
      }
    }
    Free(fix);
    if (f != NULL)
      fclose(f);
    f = NULL;
  }
  for (i = 0; i <= 255; i++) {
    if (P_inset(i, which)) {
      sprintf(STR1, "Warning:  Font %d was not found!", i);
      warning(STR1);
    }
  }
  if (f != NULL)
    fclose(f);
}  /*loadfonts*/



Void plot_morefontset(fn, which)
Char *fn;
long *which;
{
  plot_fontfile(fn);
  plot_loadfonts(which);
}



Void plot_readfontset(fn, which)
Char *fn;
long *which;
{
  plot_initfonts();
  plot_morefontset(fn, which);
}



Void plot_readfont(fn)
Char *fn;
{
  long SET[9];

  plot_readfontset(fn, P_expset(SET, 0L));
}



Void plot_ctrlchars(pref, chrs_)
Char *pref, *chrs_;
{
  Char chrs[256];

  strcpy(chrs, chrs_);
  if (strlen(prefchar) > 1)
    prefchar[1] = '\0';
  strcpy(prefchar, pref);
  if (strlen(chrs) > 10)
    chrs[10] = '\0';
  strcpy(ctrlchars, chrs);
  ucase(ctrlchars);
  strcpy(ctrllchars, ctrlchars);
  lcase(ctrllchars);
}



Void plot_selfont(num)
long num;
{
  plot_fontrec *ft;
  long SET[257];

  plot_loadfonts(P_addset(P_expset(SET, 0L), (int)num));
  ft = plot_fontbase;
  while (ft != NULL && ft->num != num)
    ft = ft->next;
  if (ft != NULL)
    curft = ft;
}



long plot_height()
{
  if (curft != NULL)
    return (curft->height);
  else
    return 1;
}



Static Void plotsize()
{
  csn = cscsc * csnum;
  csd = cssc * csden;
  csnyy = csn * csnc * csav / (scale_ * csad);
  csnxy = csn * csns * csav / (scale_ * csad);
  csnyx = -(csn * csns * csah / (scale_ * csad));
  csnxx = csn * csnc * csah / (scale_ * csad);
  csdsc = csd * scale_;
}



Void plot_charsize(num, den, scale)
long num, den, scale;
{
  if (den == 0)
    _Escape(-5);
  csnum = num;
  csden = den;
  if (scale != 0)
    cssc = labs(scale);
  else
    cssc = 1;
  cscsc = cssc;
  plotsize();
}



Void plot_charscale(scale)
long scale;
{
  cscsc = scale;
  plotsize();
}



Void plot_charorient(theta, mirrorx)
double theta;
boolean mirrorx;
{
  if (theta == 0) {
    csnc = scale_;
    csns = 0;
  } else if (theta == 90) {
    csnc = 0;
    csns = scale_;
  } else if (theta == 180) {
    csnc = -scale_;
    csns = 0;
  } else if (theta == 270) {
    csnc = 0;
    csns = -scale_;
  } else {
    csnc = (long)(cos(theta * pi / 180) * scale_);
    csns = (long)(sin(theta * pi / 180) * scale_);
  }
  if (mirrorx)
    csah = -1;
  else
    csah = 1;
  csth = theta;
  csav = 1;
  csad = 1;
  plotsize();
}



Void plot_charaspect(horiz, vert, den)
long horiz, vert, den;
{
  csah = horiz;
  csav = vert;
  csad = den;
  plotsize();
}



long plot_charnum(ch)
Char ch;
{
  if (curft == NULL)
    return -1;
  else
    return (curft->arr[ch]);
}



long plot_charname(s)
Char *s;
{
  plot_namerec *nm;

  nm = plot_namebase;
  while (nm != NULL && strcmp(nm->s, s))
    nm = nm->next;
  if (nm == NULL)
    return -1;
  else
    return (nm->num);
}



Void plot_ssoffset(sub, sup, den)
long sub, sup, den;
{
  sbo = sub;
  spo = sup;
  sod = den;
}



Static Void plotchar(x, y, num)
long *x, *y, num;
{
  short i;
  long xx, yy, x0, y0, x1, y1, x2, y2, x3, y3;
  double rad;
  chardesc *WITH;
  short FORLIM;
  point *WITH1;

  defpen();
  x3 = -1000;
  y3 = -1000;
  xx = *x * sc;
  yy = *y * sc;
  if (1 > num || num > plot_lastchar)
    return;
  WITH = ix[num - 1];
  FORLIM = WITH->numpts;
  for (i = 0; i < FORLIM; i++) {
    WITH1 = &WITH->v[i];
    if (WITH1->circ) {
      rad = (double)((labs(x0 - SEXT(WITH1->px, 7)) + labs(
			y0 - SEXT(WITH1->py, 7))) * csn * sc) / csdsc;
      pcircle(0L, rad, rad);
      x3 = -1000;
    } else {
      x0 = SEXT(WITH1->px, 7);
      y0 = SEXT(WITH1->py, 7);
      x1 = (SEXT(WITH1->px, 7) - SEXT(WITH->xleft, 8)) * sc;
      y1 = (SEXT(WITH->ybase, 8) - SEXT(WITH1->py, 7)) * sc;
      x2 = (xx + x1 * csnxx + y1 * csnyx) / csdsc;
      y2 = (yy + x1 * csnxy + y1 * csnyy) / csdsc;
      if (WITH1->drw)
	pdraw(x2, y2);
      else if (quality || labs(x2 - x3) + labs(y2 - y3) > 1)
	pmove(x2, y2);
      x3 = x2;
      y3 = y2;
    }
  }
  *x += WITH->width * csnxx;
  *y += WITH->width * csnxy;
}



Void plot_char(x, y, num)
long x, y, num;
{
  if (csd == 0)
    plotsize();
  x = (x - orgx) * csd;
  y = (y - orgy) * csd;
  plotchar(&x, &y, num);
}


/* Local variables for plotstring: */
struct LOC_plotstring {
  long *x, *y;
  Char *s;
  boolean plotit;
  short i, j;
} ;

Local long strnum(binary, term, LINK)
boolean binary;
Char term;
struct LOC_plotstring *LINK;
{
  long Result, n;

  if (binary) {
    if (LINK->i > strlen(LINK->s))
      return 0;
    Result = LINK->s[LINK->i - 1];
    LINK->i++;
    return Result;
  } else {
    n = 0;
    while (LINK->i <= strlen(LINK->s) && LINK->s[LINK->i - 1] != term) {
      if (isdigit(LINK->s[LINK->i - 1]))
	n = n * 10 + LINK->s[LINK->i - 1] - 48;
      LINK->i++;
    }
    if (LINK->i <= strlen(LINK->s))
      LINK->i++;
    return n;
  }
  return Result;
}

Local Void substr_(LINK)
struct LOC_plotstring *LINK;
{
  Char ch;
  long x0, y0, num;
  plot_fontrec *orgft;
  boolean done, flag;
  chardesc *WITH;

  done = false;
  while (LINK->i <= strlen(LINK->s) && !done) {
    ch = LINK->s[LINK->i - 1];
    LINK->i++;
    LINK->j = strlen(ctrlchars);
    if (*prefchar != '\0') {
      if (ch == prefchar[0] && LINK->i <= strlen(LINK->s)) {
	ch = LINK->s[LINK->i - 1];
	LINK->i++;
      } else
	LINK->j = 0;
    }
    if (ch == '\0' && LINK->j > 1)
      LINK->j = 1;
    while (LINK->j >= 1 && ch != ctrlchars[LINK->j - 1] &&
	   ch != ctrllchars[LINK->j - 1])
      LINK->j--;
    switch (LINK->j) {

    case 1:   /*end*/
      done = true;
      break;

    case 2:
    case 3:   /*font*/
      orgft = curft;
      num = strnum(LINK->j & 1, ';', LINK);
      if (!fast)
	plot_selfont(num);
      substr_(LINK);
      curft = orgft;
      break;

    case 4:
    case 5:   /*scale*/
      x0 = cscsc;
      cscsc = strnum(LINK->j & 1, ';', LINK);
      plotsize();
      substr_(LINK);
      cscsc = x0;
      plotsize();
      break;

    case 6:   /*subscript*/
      flag = (cscsc > 1);
      if (flag)
	cscsc--;
      plotsize();
      *LINK->x -= curft->height * csnyx * sbo / sod;
      *LINK->y -= curft->height * csnyy * sbo / sod;
      substr_(LINK);
      *LINK->x += curft->height * csnyx * sbo / sod;
      *LINK->y += curft->height * csnyy * sbo / sod;
      if (flag)
	cscsc++;
      plotsize();
      break;

    case 7:   /*superscript*/
      flag = (cscsc > 1);
      if (flag)
	cscsc--;
      plotsize();
      *LINK->x += curft->height * csnyx * spo / sod;
      *LINK->y += curft->height * csnyy * spo / sod;
      substr_(LINK);
      *LINK->x -= curft->height * csnyx * spo / sod;
      *LINK->y -= curft->height * csnyy * spo / sod;
      if (flag)
	cscsc++;
      plotsize();
      break;

    case 8:   /*underline*/
      x0 = *LINK->x;
      y0 = *LINK->y;
      substr_(LINK);
      if (LINK->plotit) {
	pmove((x0 - curft->height * csnyx / 5) * sc / csdsc,
	      (y0 - curft->height * csnyy / 5) * sc / csdsc);
	pdraw((*LINK->x - curft->height * csnyx / 5) * sc / csdsc,
	      (*LINK->y - curft->height * csnyy / 5) * sc / csdsc);
      }
      break;

    case 9:   /*overline*/
      x0 = *LINK->x;
      y0 = *LINK->y;
      substr_(LINK);
      if (LINK->plotit) {
	pmove((x0 + curft->height * csnyx * 6 / 5) * sc / csdsc,
	      (y0 + curft->height * csnyy * 6 / 5) * sc / csdsc);
	pdraw((*LINK->x + curft->height * csnyx * 6 / 5) * sc / csdsc,
	      (*LINK->y + curft->height * csnyy * 6 / 5) * sc / csdsc);
      }
      break;

    default:
      LINK->j = plot_charnum(ch);
      if (LINK->j >= 0) {
	if (LINK->plotit)
	  plotchar(LINK->x, LINK->y, (long)LINK->j);
	else {
	  WITH = ix[LINK->j - 1];
	  *LINK->x += WITH->width * csnxx;
	  *LINK->y += WITH->width * csnxy;
	}
      }
      break;
    }
  }
}



Static Void plotstring(x_, y_, s_, plotit_)
long *x_, *y_;
Char *s_;
boolean plotit_;
{
  struct LOC_plotstring V;

  V.x = x_;
  V.y = y_;
  V.s = s_;
  V.plotit = plotit_;
  if (csd == 0)
    plotsize();
  TRY(try4);
    if (curft != NULL) {
      V.i = 1;
      do {
	substr_(&V);
      } while (V.i <= strlen(V.s));
    }
  RECOVER(try4);
    if (P_escapecode == -20)
      _Escape(P_escapecode);
    printf("\007Error %d in PLOT_STRING\n", P_escapecode);
  ENDTRY(try4);
}


Static Void stringlen(x, y, s_)
long *x, *y;
Char *s_;
{
  Char s[256];

  strcpy(s, s_);
  *x = 0;
  *y = 0;
  plotstring(x, y, s, false);
}



Void plot_stringrel(s_)
Char *s_;
{
  Char s[256];
  long x, y;

  strcpy(s, s_);
  x = (xpos - orgx) * csd;
  y = (ypos - orgy) * csd;
  plotstring(&x, &y, s, true);
  xpos = x / csd + orgx;
  ypos = y / csd + orgy;
}



Static boolean candotext(x, y, s, mode)
long x, y;
Char *s;
long mode;
{
  defpen();
  rec.act = plot_act_text;
  plot_xform(x, y, &rec.x1, &rec.y1);
  if (rotate) {
    rec.x2 = mode;
    rec.q2.r = csth - 90.0;
  } else {
    rec.x2 = mode;
    rec.q2.r = csth;
  }
  if (curft == NULL)
    rec.y2 = 0;
  else
    rec.y2 = curft->num;
  rec.q1.U16.sp1 = s;
  rec.q1.U1.i2 = cscsc * scale_ / cssc;
  rec.q3.U1.i1 = csnum * sc;
  rec.q3.U1.i2 = csden * scale_;
  if (proc.link != NULL)
    (*(Void(*) PP((plot_devrec *rec, Anyptr _link)))proc.proc)(&rec, proc.link);
  else
    (*(Void(*) PP((plot_devrec *rec)))proc.proc)(&rec);
  return (rec.q1.U1.i1 == 0);
}


Void plot_string(x, y, s_)
long x, y;
Char *s_;
{
  Char s[256];

  strcpy(s, s_);
  if (candotext(x, y, s, 1L))
    return;
  x = (x - orgx) * csd;
  y = (y - orgy) * csd;
  plotstring(&x, &y, s, true);
}


Void plot_rightstring(x, y, s_)
long x, y;
Char *s_;
{
  Char s[256];
  long x1, y1;

  strcpy(s, s_);
  if (candotext(x, y, s, 3L))
    return;
  stringlen(&x1, &y1, s);
  x = (x - orgx) * csd - x1;
  y = (y - orgy) * csd - y1;
  plotstring(&x, &y, s, true);
}


Void plot_centerstring(x, y, s_)
long x, y;
Char *s_;
{
  Char s[256];
  long x1, y1;

  strcpy(s, s_);
  if (candotext(x, y, s, 2L))
    return;
  stringlen(&x1, &y1, s);
  x = (x - orgx) * csd - x1 / 2;
  y = (y - orgy) * csd - y1 / 2;
  plotstring(&x, &y, s, true);
}


Void plot_genstring(x, y, orient_, s_)
long x, y;
Char *orient_, *s_;
{
  Char orient[256], s[256];
  long i, x1, y1;

  strcpy(orient, orient_);
  strcpy(s, s_);
  strlower(orient, orient);
  if (!strcmp(orient, "ll"))
    i = 1;
  else if (!strcmp(orient, "lc"))
    i = 2;
  else if (!strcmp(orient, "lr"))
    i = 3;
  else if (!strcmp(orient, "cr"))
    i = 4;
  else if (!strcmp(orient, "ur"))
    i = 5;
  else if (!strcmp(orient, "uc"))
    i = 6;
  else if (!strcmp(orient, "ul"))
    i = 7;
  else if (!strcmp(orient, "cl"))
    i = 8;
  else if (!strcmp(orient, "cc"))
    i = 9;
  else {
    strcpy(orient, "ll");
    i = 1;
  }
  if (candotext(x, y, s, i))
    return;
  x = (x - orgx) * csd;
  y = (y - orgy) * csd;
  if (strcmp(orient, "ll")) {
    stringlen(&x1, &y1, s);
    if (orient[0] == 'u') {
      x -= curft->height * csnyx;
      y -= curft->height * csnyy;
    } else if (orient[0] == 'c') {
      x -= curft->height * csnyx / 2;
      y -= curft->height * csnyy / 2;
    }
    if (orient[1] == 'r') {
      x -= x1;
      y -= y1;
    } else if (orient[1] == 'c') {
      x -= x1 / 2;
      y -= y1 / 2;
    }
  }
  plotstring(&x, &y, s, true);
}


long plot_stringwidth(s)
Char *s;
{
  long x1, y1;

  stringlen(&x1, &y1, s);
  return ((x1 + y1) / csd);
}



/*plot*/








/* End. */
