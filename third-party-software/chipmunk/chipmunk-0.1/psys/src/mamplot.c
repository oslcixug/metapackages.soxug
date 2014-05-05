/* Output from p2c, the Pascal-to-C translator */
/* From input file "mamplot.text" */


/* shouldn't this be the FIRST line in the file? - DAVE */

/*caged_date='I{ Last edit by $U on $X (Various fixes) $]'*/
/* Last edit by dave on Mar 5, 1989 5:13 pm (Various fixes) */
/* Last edit by dave on Mar 5, 1989 5:12 pm (Various fixes) */
/* Last edit by dave on Mar 5, 1989 4:55 pm (Various fixes) */
/* Last edit by dave on Mar 5, 1989 4:52 pm (Various fixes) */
/* Last edit by dave on Oct 21, 1988 1:34 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:36 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:28 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:27 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:26 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:24 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 6:21 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 5:22 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 4:44 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 3:54 pm (Various fixes) */
/* Last edit by dave on Feb 29, 1988 3:25 pm (Various fixes) */
/* Last edit by dave on Jan 21, 1988 1:12 am (Fix TeX mode) */
/* Last edit by dave on Nov 30, 1987 5:28 pm (Various fixes for View) */
/* Last edit by dave on Nov 30, 1987 2:53 pm (Various fixes for View) */
/* Last edit by dave on Nov 24, 1987 4:57 pm (Various fixes for View) */
/* Last edit by dave on Nov 11, 1987 4:03 am (Various fixes for View) */
/* Last edit by dave on Nov 8, 1987 9:29 am (Changed 2*pi to pi+pi to avoid compiler bug) */
/* Last edit by dave on Nov 2, 1987 9:23 pm (Changed 2*pi to pi+pi to avoid compiler bug) */
/* Last edit by dave on Oct 21, 1987 4:35 pm (Changed 2*pi to pi+pi to avoid compiler bug) */
/* Last edit by mass on Dec 8, 1986 2:32 pm (To add the above comment) */
/* Last edit by mass on Dec 8, 1986 2:30 pm (To add the above comment) */
/* Last edit by mass on Dec 8, 1986 2:15 pm (To add the above comment) */
/* Last edit by dave on Nov 15, 1986 3:12 am (To add the above comment) */


/* DAVE 3/5/89 fixes:
    * changed mam_date to print current directory instead
*/
/* DAVE 2/29/88 fixes:
    * changed origins of certain labels
    * added mam_mirroraxis
    * added mam_fltlabel, mam_fixlabel
    * fixed subticks on log plots when plot_interval > 1.0
*/
/* DAVE 11/10/87 fixes:
    * completed the half-implemented mam_fullrange
    * fixed the infinite loop when datamin=datamax
    * engineering mode uses 10^3 if no units, K if units
    * engineering mode does not round values to integers!
    * added mam_transform and mam_untransform
    * added mam_gridaxis stuff
    * added mam_tex mode
    * added username to mam_date display
    * added mam_drawborder and mam_drawaxes
*/


/*$debug on${*/




#include <p2c/p2c.h>


#define PLOT_ROUTINES_G
#include <p2c/plot_routines.h>

/*file support*/

#ifndef FS_H
#include <p2c/fs.h>
#endif

#ifndef PASSWORDMOD_H
#include <p2c/passmod.h>
#endif

#ifndef CITINFOMOD_H
#include <p2c/citinfomod.h>
#endif

#ifndef SYSDEVS_H
#include <p2c/sysdevs.h>
#endif

#ifndef MATH_H
#include <p2c/math.h>
#endif


typedef enum {
  pt_log, pt_linear
} plottype;

typedef struct _REC_device_limits {
  long plotmin, plotmax, titlemax, titlemin;
} _REC_device_limits;

typedef _REC_device_limits device_limits[2];

typedef struct _REC_axis_limits {
  double absdatamax, absdatamin, datamax, datamin;
  long dpower;
  double plot_interval;
  long intpower;
  Char title[256], units[256];
  plottype dplotype;
  double scale, offset;
  boolean integerlabel, engineering, selfscale, floating, fixed;
} _REC_axis_limits;

typedef _REC_axis_limits axis_limits[3];

typedef struct char_limits {
  long height, width;
  double height_frac;
  long font, color;
} char_limits;


typedef double clip_bcarray[4];   /*left, right, bottom, top*/


Static axis_limits DATALIMIT;
Static device_limits PLOTLIMIT;

Static char_limits axischar, labelchar, titlechar, symbolchar;
Static double symbolxoff, symbolyoff, ticksize;
Static long bordercolor, tickcolor, TICKFRAC, datacolor, datastyle;

Static double logdif[8];

Static long num_axes;
Static boolean wallpaper;
Static boolean grid[3], mirrored[3];
Static boolean drewborder, drewaxes, texmode;

Static double clip_oldx, clip_oldy;
Static clip_bcarray clip_oldbc;
Static long clip_oldoc;
Static boolean clip_there;




double mam_mean(x, len)
double **x;
long len;
{
  double result;
  long i;

  result = 0.0;
  for (i = 0; i < len; i++)
    result += (*x)[i];
  result /= len;
  return result;
}


double mam_stddev(x, len)
double **x;
long len;
{
  double result, sumxsquared, sumsquaredx;
  long i;

  result = 0.0;
  sumxsquared = 0.0;
  sumsquaredx = 0.0;
  for (i = 0; i < len; i++) {
    sumxsquared += (*x)[i] * (*x)[i];
    sumsquaredx += (*x)[i];
  }
  sumsquaredx *= sumsquaredx;
  return sqrt((sumxsquared - sumsquaredx / len) / (len - 1));
}


#define pi              3.1415926


double mam_normal(mean, stddev, x)
double mean, stddev, x;
{
  /* return the probability density of value x, with a distribution
     of mean and stddev */
  double TEMP;

  TEMP = (x - mean) / stddev;
  return (exp(TEMP * TEMP / -2) / (stddev * sqrt(pi + pi)));
}

#undef pi


double mam_log(x)
double x;
{
  double Result;

  if (x != 0.0)
    return (ma_log(fabs(x)));
  Result = -1000.0;
  if (wallpaper)
    printf("Argument of mam_log is 0, returning -1000\n");
  return Result;
}



/* Convert from fraction of paper height to window units */
Static long fraction(frac)
double frac;
{
  return ((long)((PLOTLIMIT[1].titlemax - PLOTLIMIT[1].titlemin) * frac));
}





/*Procedures for User defined stuff here*/
/*****/


/*Vector memory management*/
Void mam_new(vec, size)
double **vec;
long size;
{
  *vec = (double *)Malloc(size * sizeof(double));
}


Void mam_dispose(vec, size)
double **vec;
long size;
{
  Free(*vec);
}


Void mam_gridaxis(axis)
long axis;
{
  /*DAVE 11/10/87*/
  grid[axis - 1] = true;
}


Void mam_grid()
{
  long i;

  for (i = 1; i <= 3; i++)
    mam_gridaxis(i);
}


Void mam_mirroraxis(axis)
long axis;
{
  mirrored[axis - 1] = true;
}


Void mam_tex()
{
  texmode = true;
}


/*Specify Integer Labels*/
Void mam_intlabel(axis)
long axis;
{
  DATALIMIT[axis - 1].integerlabel = true;
}


Void mam_englabel(axis)
long axis;
{
  DATALIMIT[axis - 1].engineering = true;
}


Void mam_fltlabel(axis)
long axis;
{
  DATALIMIT[axis - 1].floating = true;
}


Void mam_fixlabel(axis)
long axis;
{
  DATALIMIT[axis - 1].fixed = true;
}



/*Let the user define the scaling parameters and set flag*/
Void mam_range(axis, min, max)
long axis;
double min, max;
{
  DATALIMIT[axis - 1].datamin = min;
  DATALIMIT[axis - 1].datamax = max;
  DATALIMIT[axis - 1].absdatamin = fabs(min);
  DATALIMIT[axis - 1].absdatamax = fabs(max);
}


Void mam_fullrange(axis, min, max, interval)
long axis;
double min, max, interval;
{
  DATALIMIT[axis - 1].datamin = min;
  DATALIMIT[axis - 1].datamax = max;
  DATALIMIT[axis - 1].absdatamin = fabs(min);   /*DAVE 11/10/87*/
  DATALIMIT[axis - 1].absdatamax = fabs(max);
  DATALIMIT[axis - 1].plot_interval = interval;
  DATALIMIT[axis - 1].selfscale = true;
}


Void mam_getrange(axis, min, max, interval)
long axis;
double *min, *max, *interval;
{
  *min = DATALIMIT[axis - 1].datamin;
  if (DATALIMIT[axis - 1].dplotype == pt_log)   /*from MATH module*/
    *min = ma_tentox(*min);
  *max = DATALIMIT[axis - 1].datamax;
  if (DATALIMIT[axis - 1].dplotype == pt_log)
    *max = ma_tentox(*max);
  *interval = DATALIMIT[axis - 1].plot_interval;
}



Void mam_quality()
{
  plot_quality();
}





Void mam_axissize(frac)
double frac;
{
  axischar.height_frac = frac;
}


Void mam_axisfont(num)
long num;
{
  axischar.font = num;
}


Void mam_axiscolor(col)
long col;
{
  axischar.color = col;
}


Void mam_labelsize(frac)
double frac;
{
  labelchar.height_frac = frac;
}


Void mam_labelfont(num)
long num;
{
  labelchar.font = num;
}


Void mam_labelcolor(col)
long col;
{
  labelchar.color = col;
}


Void mam_titlesize(frac)
double frac;
{
  titlechar.height_frac = frac;
}


Void mam_titlefont(num)
long num;
{
  titlechar.font = num;
}


Void mam_titlecolor(col)
long col;
{
  titlechar.color = col;
}


Void mam_symbolsize(frac)
double frac;
{
  symbolchar.height_frac = frac;
}


Void mam_symbolfont(num)
long num;
{
  symbolchar.font = num;
}


Void mam_symbolcolor(col)
long col;
{
  symbolchar.color = col;
}


Void mam_symboloffset(xfrac, yfrac)
double xfrac, yfrac;
{
  symbolxoff = xfrac;
  symbolyoff = yfrac;
}




Void mam_setfontfile(fn)
Char *fn;
{
  plot_fontfile(fn);
}



Void mam_ticksize(frac)
double frac;
{
  ticksize = frac;
}


Void mam_tickcolor(col)
long col;
{
  tickcolor = col;
}


Void mam_bordercolor(col)
long col;
{
  bordercolor = col;
}



Void mam_datacolor(col)
long col;
{
  datacolor = col;
}


Void mam_datastyle(style)
long style;
{
  datastyle = style;
}




Void mam_charsize(frac)
double frac;
{
  mam_axissize(frac);
  mam_labelsize(frac);
  mam_titlesize(frac);
}


Void mam_font(num)
long num;
{
  mam_axisfont(num);
  mam_labelfont(num);
  mam_titlefont(num);
  mam_symbolfont(num);
}


Void mam_color(col)
long col;
{
  mam_axiscolor(col);
  mam_labelcolor(col);
  mam_titlecolor(col);
  mam_symbolcolor(col);
  mam_datacolor(col);
  mam_tickcolor(col);
  mam_bordercolor(col);
}





Static Void initcharlimits(rec, size, font, color)
char_limits *rec;
double size;
long font, color;
{
  rec->height_frac = size;
  rec->font = font;
  rec->color = color;
}


Static Void computecharlimits(rec)
char_limits *rec;
{
  rec->height = fraction(rec->height_frac);
  plot_selfont(rec->font);
  if (plot_height() == 1)
    printf("MAMPLOT:  Font %ld does not exist!\n", rec->font);
  plot_charsize(rec->height, plot_height(), 4L);
  rec->width = plot_stringwidth("0");
}


Static Void setupcharlimits(rec)
char_limits *rec;
{
  computecharlimits(rec);
  plot_charorient(0.0, false);
  plot_color(rec->color);
}





/*Initialization routines go here*/
/*****/
/**************************************************/

Void mam_setup_generic()
{
  long i;

  lowerfudge = 0;
  for (i = 0; i <= 2; i++) {
    grid[i] = false;
    mirrored[i] = false;
  }
  texmode = false;
  wallpaper = false;
  ticksize = 0.01;   /*How big tick marks are*/
  tickcolor = 1;
  bordercolor = 1;
  initcharlimits(&axischar, mam_defcharsize, 2L, 1L);
  initcharlimits(&labelchar, mam_defcharsize, 2L, 1L);
  initcharlimits(&titlechar, mam_deftitlesize, 2L, 1L);
  initcharlimits(&symbolchar, mam_defcharsize, 2L, 1L);
  symbolxoff = mam_defsymbolxoff;
  symbolyoff = mam_defsymbolyoff;
  datacolor = 2;
  datastyle = 0;
  drewborder = false;
  drewaxes = false;
}





Void mam_init_generic()
{
  long i;
  _REC_axis_limits *WITH;

  num_axes = 2;

  plot_getwindow(&PLOTLIMIT[0].titlemin, &PLOTLIMIT[1].titlemin,
		 &PLOTLIMIT[0].titlemax, &PLOTLIMIT[1].titlemax);

  /*$if false$
   plot_nrwindow (0,0, 10000, 8000);
   PLOTLIMIT[2].titlemin := 1;
   {PLOTLIMIT[1].titlemin := 1;    old MAS }
   PLOTLIMIT[1].titlemin := 600;
   {PLOTLIMIT[1].titlemax := 10500;}
   PLOTLIMIT[1].titlemax := 10000;  {quick fix for small plotter}
$end$*/


  plot_ctrlchars("$", "XF\001S\001BPUO");


  /*Set up the log constants for labeling the log part*/
  for (i = 2; i <= 9; i++)
    logdif[i - 2] = ma_log((double)i) - ma_log(i - 1.0);

  /*Set up the default values for the datalimits*/
  for (i = 0; i <= 2; i++) {
    WITH = &DATALIMIT[i];
    WITH->selfscale = false;
    WITH->integerlabel = false;
    WITH->engineering = false;
    WITH->floating = false;
    WITH->fixed = false;
    WITH->absdatamin = LONG_MAX;
    WITH->absdatamax = -LONG_MAX;
    WITH->datamax = -LONG_MAX;
    WITH->datamin = LONG_MAX;
    *WITH->title = '\0';
    *WITH->units = '\0';
    WITH->dplotype = pt_linear;
  }

}






/**************************************************/
Void mam_init_plotter()
{
  mam_setup_generic();
  plot_init_fast();
  mam_init_generic();
}




Void mam_init_file(fname)
Char *fname;
{
  mam_setup_generic();
  plot_initfile_fast(fname);
  mam_init_generic();
}



Void mam_init_ps(fname)
Char *fname;
{
  mam_setup_generic();
  plot_initps_fast(fname);
  mam_init_generic();
}



Void mam_init_screen()
{
  mam_setup_generic();
  plot_initscreen_fast();
  mam_init_generic();
}





Void mam_close()
{
  mam_close_generic();
  plot_view();
  plot_finish();
}







/************************************************************/
Static Void transform(from_origin, from_corner, to_origin, to_corner, scale,
		      offset)
double from_origin, from_corner, to_origin, to_corner, *scale, *offset;
{
  *scale = (to_corner - to_origin) / (from_corner - from_origin);
  *offset = (to_corner * from_origin - to_origin * from_corner) /
	    (from_origin - from_corner);

}


/**************************************************/
/*finds the max and min of the input vector of    */
/*length len returns sorted = true if the input is*/
/*not monotonically increasing                    */

Static Void maxmin(x, xlen, max, min, amax, amin, sorted)
double *x;
long xlen;
double *max, *min, *amax, *amin;
boolean *sorted;
{
  long i;

  *sorted = true;   /*Bug fix by DAVE  1/19/86*/
  if (xlen <= 0)
    return;
  *max = x[0];
  *min = x[0];
  *amax = fabs(x[0]);
  *amin = fabs(x[0]);
  i = 1;
  while (i <= xlen) {
    if (x[i - 1] >= *max)
      *max = x[i - 1];
    else {
      *sorted = false;
      if (x[i - 1] < *min)
	*min = x[i - 1];
    }
    if ((fabs(x[i - 1]) >= *amax || *amax == 0.0) && x[i - 1] != 0.0)
	  /*^^^^^^^^^^^^ added 10/10/86, DAVE */
	    *amax = fabs(x[i - 1]);
    else if ((fabs(x[i - 1]) < *amin || *amin == 0.0) && x[i - 1] != 0.0)
      *amin = fabs(x[i - 1]);
    i++;
  }
}  /*maxmin*/



/**************************************************/

Static long mag(x)
double x;
{
  /*returns the power needed to normalize x*/
  double logval;

  if (x != 0.0) {
    logval = ma_log(fabs(x));
    if (logval < 0)
      return ((long)logval - 1);
    else
      return ((long)logval);
  }

}


/**************************************************/

Static double expon(x, power)
double x;
long power;
{
  /*return x times ten to the power power*/
  long i;
  double temp;

  temp = x;
  if (power > 0) {
    for (i = 1; i <= power; i++)
      temp *= 10;
    return temp;
  } else {
    power = -power;
    for (i = 1; i <= power; i++)
      temp /= 10;
    return temp;
  }
}


/*******************************************************************/

Static double normal(x, power)
double x;
long power;
{
  /*yow!  normal(x,i) = expon(x,-i)*/
  long i;

  if (power > 0) {
    for (i = 1; i <= power; i++)
      x /= 10.0;
    return x;
  }
  power = -power;
  for (i = 1; i <= power; i++)
    x *= 10.0;
  return x;
}



/*Scaling routines go here*/
Static Void scale_data(x, y, ytype, len)
double **x, **y;
long ytype, len;
{
  double localmax, localmin, localamax, localamin;
  boolean sorted;
  _REC_axis_limits *WITH;

  WITH = DATALIMIT;
  maxmin(*x, len, &localmax, &localmin, &localamax, &localamin, &sorted);
  if (localmax > WITH->datamax)
    WITH->datamax = localmax;
  if (localmin < WITH->datamin)
    WITH->datamin = localmin;
  if (localamax > WITH->absdatamax)
    WITH->absdatamax = localamax;
  if (localamin < WITH->absdatamin)
    WITH->absdatamin = localamin;
  WITH = &DATALIMIT[ytype - 1];
  /*
      writeln('xdatamax,xdatamin = ',datalimit[1].datamax,' ',datalimit[1].datamin);
      writeln('ydatamax,ydatamin = ',datalimit[ytype].datamax,' ',  datalimit[ytype].datamin);
  */
  /*if sorted = false then sort(x,y,len);*/
  maxmin(*y, len, &localmax, &localmin, &localamax, &localamin, &sorted);
  if (localmax > WITH->datamax)
    WITH->datamax = localmax;
  if (localmin < WITH->datamin)
    WITH->datamin = localmin;
  if (localamax > WITH->absdatamax)
    WITH->absdatamax = localamax;
  if (localamin < WITH->absdatamin)
    WITH->absdatamin = localamin;
}


/**************************************************/

Void mam_scale_generic(axis, x, len)
long axis;
double **x;
long len;
{
  double localmax, localmin, localamax, localamin;
  boolean sorted;
  _REC_axis_limits *WITH;

  WITH = &DATALIMIT[axis - 1];
  maxmin(*x, len, &localmax, &localmin, &localamax, &localamin, &sorted);
  if (localmax > WITH->datamax)
    WITH->datamax = localmax;
  if (localmin < WITH->datamin)
    WITH->datamin = localmin;
  if (localamax > WITH->absdatamax)
    WITH->absdatamax = localamax;
  if (localamin < WITH->absdatamin)
    WITH->absdatamin = localamin;
}


/**************************************************/

Void mam_scale(x, y, len)
double **x, **y;
long len;
{
  scale_data(x, y, 2L, len);
}


Void mam_scalenormal(x, y, len, normalx)
double **x, **y;
long len;
double **normalx;
{
  scale_data(x, y, 2L, len);
}


/**************************************************/

Void mam_rscale(x, y, len)
double **x, **y;
long len;
{
  /*scale data for right axis*/
  num_axes = 3;
  scale_data(x, y, 3L, len);
}


/***************************************************/

Static double rtran(x, xscale, xoffset)
double x, xscale, xoffset;
{
  return (xscale * x + xoffset);
}


Static double rtranx(x)
double x;
{
  return (rtran(x, DATALIMIT[0].scale, DATALIMIT[0].offset));
}


Static double rtrany(y, yaxis)
double y;
long yaxis;
{
  return (rtran(y, DATALIMIT[yaxis - 1].scale, DATALIMIT[yaxis - 1].offset));
}


Static long tranx(x)
double x;
{
  return ((long)rtranx(x));
}


Static long trany(y, yaxis)
double y;
long yaxis;
{
  return ((long)rtrany(y, yaxis));
}



/**************************************************/

Void mam_transform(x, y, yaxis, tx, ty)
double x, y;
long yaxis;
double *tx, *ty;
{
  if (DATALIMIT[0].dplotype == pt_log)
    x = mam_log(x);
  if (DATALIMIT[yaxis - 1].dplotype == pt_log)
    y = mam_log(y);
  *tx = rtranx(x);
  *ty = rtrany(y, yaxis);
}


Void mam_rtransform(dx, dy, yaxis, tdx, tdy)
double dx, dy;
long yaxis;
double *tdx, *tdy;
{
  *tdx = dx * DATALIMIT[0].scale;
  *tdy = dy * DATALIMIT[yaxis - 1].scale;
}


Void mam_untransform(tx, ty, yaxis, x, y)
double tx, ty;
long yaxis;
double *x, *y;
{
  *x = (tx - DATALIMIT[0].offset) / DATALIMIT[0].scale;
  *y = (ty - DATALIMIT[yaxis - 1].offset) / DATALIMIT[yaxis - 1].scale;
  if (DATALIMIT[0].dplotype == pt_log)
    *x = ma_tentox(*x);
  if (DATALIMIT[yaxis - 1].dplotype == pt_log)
    *y = ma_tentox(*y);
}


Void mam_runtransform(tdx, tdy, yaxis, dx, dy)
double tdx, tdy;
long yaxis;
double *dx, *dy;
{
  *dx = tdx / DATALIMIT[0].scale;
  *dy = tdy / DATALIMIT[yaxis - 1].scale;
}



/**************************************************/

Static Void movabs(x, y)
long x, y;
{
  plot_move(x, y);
}


Static Void drawabs(x, y)
long x, y;
{
  plot_draw(x, y);
}




Static Void rmovabs(x, y)
double x, y;
{
  plot_move((long)x, (long)y);
}


Static Void rdrawabs(x, y)
double x, y;
{
  plot_draw((long)x, (long)y);
}




/*Clipper from Dave G.'s CS274 project*/
Static Void clippoint(x, y, pendn)
double x, y;
boolean pendn;
{
  clip_bcarray bc;   /* Boundary conditions */
  long oc;   /* Out code */
  double a, a1, a2;   /* Assorted "alpha"'s */
  long e;   /* Loop index */

  bc[0] = x - PLOTLIMIT[0].plotmin;
  bc[1] = PLOTLIMIT[0].plotmax - x;
  bc[2] = y - PLOTLIMIT[1].plotmin;
  bc[3] = PLOTLIMIT[1].plotmax - y;
  oc = (bc[0] < 0) * 8 + (bc[1] < 0) * 4 + (bc[2] < 0) * 2 + (bc[3] < 0);
  /* ord(true) = 1, ord(false) = 0 */
  if (pendn) {  /*draw the line*/
    if ((clip_oldoc & oc) == 0) {  /*not trivial reject*/
      a1 = 0.0;
      a2 = 1.0;
      if (clip_oldoc + oc != 0)   /* Both >=0, so same as OR, faster */
      {  /*not trivial accept*/
	for (e = 0; e <= 3; e++) {
	  if (clip_oldbc[e] < 0 || bc[e] < 0) {  /*clip against edge*/
	    a = clip_oldbc[e] / (clip_oldbc[e] - bc[e]);
	    if (clip_oldbc[e] < 0) {
	      if (a > a1)
		a1 = a;
	    } else {
	      if (a < a2)
		a2 = a;
	    }
	  }
	}
	if (a1 <= a2) {  /*partway on screen*/
	  if (a1 != 0) {
	    rmovabs(clip_oldx * (1 - a1) + x * a1,
		    clip_oldy * (1 - a1) + y * a1);
	  }  /* First endpoint was clipped */
	  else {
	    if (!clip_there)
	      rmovabs(clip_oldx, clip_oldy);
	  }
	  if (a2 != 1) {
	    rdrawabs(clip_oldx * (1 - a2) + x * a2,
		     clip_oldy * (1 - a2) + y * a2);
	    clip_there = false;   /* Second end was clipped */
	  } else {
	    rdrawabs(x, y);
	    clip_there = true;   /* Second end not clipped */
	  }
	} else  /*non-trivial reject*/
	  clip_there = false;
      } else {  /*trivial accept*/
	if (!clip_there)
	  rmovabs(clip_oldx, clip_oldy);
	rdrawabs(x, y);
	clip_there = true;
      }
    } else  /*trivial reject*/
      clip_there = false;
  } else  /*move without drawing*/
    clip_there = false;
  clip_oldx = x;
  clip_oldy = y;
  memcpy(clip_oldbc, bc, sizeof(clip_bcarray));
  clip_oldoc = oc;
}




/**************************************************/

Void move_(xto, yto, yaxis)
double xto, yto;
long yaxis;
{
  clippoint(rtranx(xto), rtrany(yto, yaxis), false);
}


Void draw(xto, yto, yaxis)
double xto, yto;
long yaxis;
{
  clippoint(rtranx(xto), rtrany(yto, yaxis), true);
}


Void mam_move(xto, yto, yaxis)
double xto, yto;
long yaxis;
{
  move_(xto, yto, yaxis);
}


Void mam_draw(xto, yto, yaxis)
double xto, yto;
long yaxis;
{
  draw(xto, yto, yaxis);
}






Void mam_lower_title(s)
Char *s;
{
  long xmid;

  xmid = (PLOTLIMIT[0].plotmax + PLOTLIMIT[0].plotmin) / 2;
  setupcharlimits(&titlechar);
  plot_centerstring(xmid, PLOTLIMIT[1].titlemin, s);
}



Void mam_title(s)
Char *s;
{
  long xmid;

  xmid = (PLOTLIMIT[0].plotmax + PLOTLIMIT[0].plotmin) / 2;
  setupcharlimits(&titlechar);
  plot_centerstring(xmid, PLOTLIMIT[1].plotmax + titlechar.height, s);
}


typedef Char monlist[16][4];


Const monlist months = {
  "000", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
  "Nov", "Dec", "013", "014", "015"
};


Static Char *get_date_string(Result)
Char *Result;
{
  long pos;
  Char ampm[4];
  datetimerec datetime;
  Char date_string[256];

  sysdate(&datetime.date);
  systime(&datetime.time);

  sprintf(date_string, "%s%3d%5d",
	  months[datetime.date.month], datetime.date.day,
	  datetime.date.year + 1900);
  pos = strlen(date_string) + 1;
  strcpy(ampm, " am");
  if (datetime.time.hour > 11) {
    datetime.time.hour -= 12;
    strcpy(ampm, " pm");
  }
  if (datetime.time.hour == 0)
    datetime.time.hour = 12;
  sprintf(date_string + pos - 1, "%3d:", datetime.time.hour);
  pos = strlen(date_string) + 1;
  if (datetime.time.minute < 10) {
    sprintf(date_string + pos - 1, "0");
    pos = strlen(date_string) + 1;
  }
  sprintf(date_string + pos - 1, "%d%s", datetime.time.minute, ampm);
  pos = strlen(date_string) + 1;
  return strcpy(Result, date_string);
}


Static Char *pwd(Result)
Char *Result;
{
     getcwd(Result, 255);
  return Result;
}


Void mam_date()
{
  Char STR1[256], STR3[256];
  Char STR4[256];

  setupcharlimits(&titlechar);
  plot_charscale(2L);
  sprintf(STR4, "%s from %s", get_date_string(STR1), pwd(STR3));
  /*     get_date_string + ' by ' + citinfo^.username);     */
  plot_string(PLOTLIMIT[0].titlemin,
	      PLOTLIMIT[1].titlemin + titlechar.height / 4, STR4);
  plot_charscale(4L);
}


Void mam_plotstring(s, x, y)
Char *s;
double x, y;
{
  /* plot a string, centered at x,y, where x and y are in the range (0,1) */
  long xmid, ymid;

  xmid = (long)((PLOTLIMIT[0].plotmax + PLOTLIMIT[0].plotmin) * x);
  ymid = (long)((PLOTLIMIT[1].plotmax + PLOTLIMIT[1].plotmin) * y);
  setupcharlimits(&titlechar);
  plot_string(xmid, ymid + titlechar.height, s);
  /*    writeln('calling mam_plotstring with: ', xmid, ymid);    */
}


Static Char *plottype_NAMES[] = {
  "PT_LOG", "PT_LINEAR"
} ;




/*Axis definition Procedures go here*/
/*****/
/**************************************************/
Static Void define_axis(axis_type)
long axis_type;
{
  char plot_range;
  double diff, rdiv;
  long ndiv;
  _REC_axis_limits *WITH;

  WITH = &DATALIMIT[axis_type - 1];   /* with */
  if (WITH->dplotype == pt_log) {
    WITH->datamin = mam_log(WITH->absdatamin);
    WITH->datamax = mam_log(WITH->absdatamax);
  }

  if (WITH->datamax == WITH->datamin) {   /*DAVE 11/10/87*/
    if (WITH->datamax == 0) {
      WITH->datamax = 1.0;   /*arbitrary*/
      WITH->datamin = -1.0;
    } else {
      WITH->datamax *= 1.5;
      WITH->datamin *= 0.5;
    }
  }

  diff = fabs(WITH->datamax - WITH->datamin);

  WITH->intpower = 0;
  if (WITH->dplotype == pt_log) {
    if (!WITH->selfscale) {   /*DAVE 11/10/87*/
      if (diff <= 10)
	WITH->plot_interval = 1.0;
      else if (diff > 10 && diff <= 30)
	WITH->plot_interval = 3.0;
      else if (diff > 30 && diff <= 50)
	WITH->plot_interval = 5.0;
      else if (diff > 50 && diff < 100)
	WITH->plot_interval = 10.0;
      else
	WITH->plot_interval = 20.0;
    }
    WITH->dpower = 0;
  } else if (WITH->selfscale)
    WITH->intpower = mag(WITH->plot_interval);   /*DAVE 11/10/87*/
  else if (WITH->integerlabel && diff < 10)
    WITH->plot_interval = 1.0;
  else {
    while (diff < 5) {
      WITH->intpower--;
      diff *= 10;
    }
    while (diff >= 50) {
      WITH->intpower++;
      diff /= 10;
    }

    plot_range = (long)(diff + 0.499999);
    switch (plot_range) {

    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      WITH->plot_interval = 1.0;
      break;

    case 21:
    case 22:
    case 23:
    case 24:
      WITH->plot_interval = 2.5;
      break;

    default:
      if (plot_range >= 11 && plot_range <= 20)
	WITH->plot_interval = 2.0;
      else
	WITH->plot_interval = 5.0;
      break;
    }
    if (WITH->integerlabel)
      WITH->plot_interval = ma_rtrunc(WITH->plot_interval);
    WITH->plot_interval = expon(WITH->plot_interval, WITH->intpower);
  }

  rdiv = WITH->datamin / WITH->plot_interval;
  ndiv = (long)rdiv;
  if (WITH->datamin < 0 && rdiv - ndiv != 0)
    ndiv--;
  WITH->datamin = ndiv * WITH->plot_interval;
  rdiv = WITH->datamax / WITH->plot_interval;
  ndiv = (long)rdiv;
  if (WITH->datamax > 0 && rdiv - ndiv != 0)
    ndiv++;
  WITH->datamax = ndiv * WITH->plot_interval;

  if (WITH->dplotype == pt_linear) {
    WITH->dpower = mag(ma_max2(fabs(WITH->datamax), fabs(WITH->datamin)));
    if (WITH->engineering)
      WITH->dpower = (long)floor((WITH->dpower - 1.0) / 3 + 0.5) * 3;
    else if (WITH->floating && WITH->dpower > -2 && WITH->dpower < 3 ||
	     WITH->fixed) {
      WITH->dpower = 0;
      if (WITH->intpower > 0)
	WITH->intpower = 0;
    }
    if (wallpaper)
      printf("linear %12ld  %12ld\n", mag(WITH->datamax), mag(WITH->datamin));
  }
  if (wallpaper)
    printf("%s  %12ld  %12ld  %12ld% .5E  % .5E\n",
	   plottype_NAMES[(long)WITH->dplotype], axis_type, WITH->dpower,
	   WITH->intpower, WITH->datamax, WITH->datamin);

}



/***************************************************************************/

Static Void tick(tickpos, axis_type, tickstart, tickstart2, ticklength)
double tickpos;
long axis_type, tickstart, tickstart2, ticklength;
{
  long ty, tx;
  _REC_device_limits *WITH;

  plot_color(tickcolor);
  if (axis_type == 2 || axis_type == 3) {
    ty = trany(tickpos, axis_type);
    if (grid[axis_type - 1]) {
      WITH = PLOTLIMIT;
      movabs(WITH->plotmin, ty);
      drawabs(WITH->plotmax, ty);
      return;
    }
    if (!mirrored[axis_type - 1]) {
      movabs(tickstart - ticklength, ty);
      drawabs(tickstart + ticklength, ty);
      return;
    }
    movabs(tickstart - ticklength, ty);
    drawabs(tickstart + ticklength, ty);
    if (axis_type == 2) {
      movabs(tickstart2 - ticklength, ty);
      drawabs(tickstart2, ty);
    } else {
      movabs(tickstart2 + ticklength, ty);
      drawabs(tickstart2, ty);
    }
    return;
  }
  tx = tranx(tickpos);
  if (grid[0]) {
    WITH = &PLOTLIMIT[1];
    movabs(tx, WITH->plotmin);
    drawabs(tx, WITH->plotmax);
    return;
  }
  if (!mirrored[0]) {
    movabs(tx, tickstart - ticklength);
    drawabs(tx, tickstart + ticklength);
    return;
  }
  movabs(tx, tickstart - ticklength);
  drawabs(tx, tickstart + ticklength);
  movabs(tx, tickstart2 - ticklength);
  drawabs(tx, tickstart2);
}


Static Void subtick(labelpos, axis_type, labelstart, labelstart2)
double labelpos;
long axis_type, labelstart, labelstart2;
{
  long i, j;
  double lpos2;
  _REC_axis_limits *WITH;
  long FORLIM;

  WITH = &DATALIMIT[axis_type - 1];
  FORLIM = (long)floor(WITH->plot_interval + 0.5);
  for (j = 1; j <= FORLIM; j++) {
    if (j > 1)
      tick(labelpos, axis_type, labelstart, labelstart2, TICKFRAC * 3 / 4);
    lpos2 = labelpos;
    for (i = 0; i <= 7; i++) {
      lpos2 += logdif[i];   /**plot_interval*/
      tick(lpos2, axis_type, labelstart, labelstart2, TICKFRAC / 2);
    }
    labelpos += 1.0;
  }
}



/*****************************************************************/

Static Char *superscript(Result, base, sup)
Char *Result, *base, *sup;
{
  if (texmode) {
    if (strlen(sup) > 1) {
      sprintf(Result, "$%s^{%s}$", base, sup);
      return Result;
    } else {
      sprintf(Result, "$%s^%s$", base, sup);
      return Result;
    }
  } else {
    sprintf(Result, "%s$P%s$X", base, sup);
    return Result;
  }
}


Static Void plotter_label(label_pos, axis_type, labelstart, label_string,
			  ndigs, plotype)
double label_pos;
long axis_type, labelstart;
Char *label_string;
long ndigs;
plottype plotype;
{
  Char STR1[256];

  setupcharlimits(&labelchar);
  switch (axis_type) {

  case 3:  /* put numeric labels on right-hand y-axis */
    if (plotype == pt_log)
      plot_genstring(labelstart + TICKFRAC + labelchar.width / 2,
		     trany(label_pos, 3L), "cl",
		     superscript(STR1, "10", label_string));
    else {
      /* Used to be this:   (DAVE 5/22/86)
               plot_string(labelstart + TICKFRAC + labelchar.width div 2,
                           trany(label_pos, 3) - labelchar.height div 2,
                           superscript('10', label_string))
         else  plot_string(labelstart + TICKFRAC + labelchar.width div 2,
                           trany(label_pos, 3) - labelchar.height div 2,
                           label_string)    */

      plot_genstring(labelstart + TICKFRAC + labelchar.width / 2,
		     trany(label_pos, 3L), "cl", label_string);
    }
    break;

  case 2:  /* put numeric labels on left-hand y-axis */
    if (plotype == pt_linear)
      plot_genstring(labelstart - TICKFRAC - labelchar.width / 2,
		     trany(label_pos, 2L), "cr", label_string);
    else {
      /* Used to be this:  (DAVE 5/22/86)
          plot_rightstring(labelstart - TICKFRAC - labelchar.width div 2,
           trany(label_pos, 2) - labelchar.height div 2,
           label_string)
           else  plot_rightstring(labelstart - TICKFRAC - labelchar.width div 2,
           trany(label_pos, 2) - labelchar.height div 2,
           superscript('10', label_string))    */
      plot_genstring(labelstart - TICKFRAC - labelchar.width / 2,
		     trany(label_pos, 2L), "cr",
		     superscript(STR1, "10", label_string));
    }
    break;

  case 1:  /* put numeric labels on x axis */
    if (plotype == pt_linear)
      plot_genstring(tranx(label_pos),
		     labelstart - TICKFRAC - labelchar.height / 2, "uc",
		     label_string);
    else {
      /* Used to be this:  (DAVE 5/22/86)
          plot_centerstring(tranx(label_pos),
                  labelstart - TICKFRAC - labelchar.height * 3 div 2,
                  label_string)
      else plot_centerstring(tranx(label_pos),
                  labelstart - TICKFRAC - labelchar.height * 3 div 2,
                  superscript('10', label_string))    */
      plot_genstring(tranx(label_pos),
		     labelstart - TICKFRAC - labelchar.height / 2, "uc",
		     superscript(STR1, "10", label_string));
    }
    break;
  }




}





/************************************************************/
Static long needdigits(axis)
long axis;
{
  long ndigs;
  _REC_axis_limits *WITH;

  WITH = &DATALIMIT[axis - 1];
  ndigs = labs(WITH->dpower - WITH->intpower);
  if (fabs(normal(WITH->plot_interval, WITH->intpower) - 2.5) < 1e-3)
    ndigs++;
  return ndigs;
}




/************************************************************/
Static Void label_axis(plotype, axis_type, labelstart, labelstart2)
plottype plotype;
long axis_type, labelstart, labelstart2;
{
  Char labelstr[256];
  long ndigs, len, ilabel;
  double labelpos, labelval;
  _REC_axis_limits *WITH;
  long FORLIM;
  Char STR1[256];

  ndigs = needdigits(axis_type);
  ilabel = 1;
  WITH = &DATALIMIT[axis_type - 1];
  FORLIM = (long)((WITH->datamax - WITH->datamin) / WITH->plot_interval + 0.001);
  for (ilabel = 0; ilabel <= FORLIM; ilabel++) {
    labelpos = WITH->datamin + WITH->plot_interval * ilabel;
    tick(labelpos, axis_type, labelstart, labelstart2, TICKFRAC);
    if (plotype == pt_log && labelpos < WITH->datamax)
      subtick(labelpos, axis_type, labelstart, labelstart2);
    if (WITH->integerlabel)
      sprintf(labelstr, "%g", labelpos);
    else if (WITH->engineering) {
      labelval = normal(labelpos, WITH->dpower);
      sprintf(labelstr, "%g", labelval);
    } else {
      labelval = normal(labelpos, WITH->dpower);
      sprintf(labelstr, "%*.*f", (int)(ndigs + 3), (int)ndigs, labelval);
      len = strlen(labelstr) + 1;
      labelstr[len - 1] = '\0';
      strcpy(STR1, strltrim(labelstr));
      strcpy(labelstr, STR1);
/* p2c: mamplot.text, line 1599:
 * Note: Modification of string length may translate incorrectly [146] */
      /*DAVE 5/22/86*/
      if (wallpaper)
	printf("ndigs=%12ld\n", ndigs);
    }
    plotter_label(labelpos, axis_type, labelstart, labelstr, ndigs, plotype);
  }
}




Static Void set_axis()
{
  long ndigs, i, j, FORLIM;
  _REC_axis_limits *WITH;
  _REC_device_limits *WITH1;


  TICKFRAC = fraction(ticksize);

  computecharlimits(&axischar);
  computecharlimits(&labelchar);
  computecharlimits(&titlechar);

  PLOTLIMIT[0].plotmin = PLOTLIMIT[0].titlemin;
  PLOTLIMIT[0].plotmax = PLOTLIMIT[0].titlemax;
  PLOTLIMIT[1].plotmin = PLOTLIMIT[1].titlemin + axischar.height * 3 +
			 labelchar.height * 2 + TICKFRAC;
  PLOTLIMIT[1].plotmax = PLOTLIMIT[1].titlemax - titlechar.height * 2;


  FORLIM = num_axes;
  for (i = 1; i <= FORLIM; i++)
    define_axis(i);


  if (num_axes == 3) {
    WITH = &DATALIMIT[2];
    ndigs = needdigits(3L);

    PLOTLIMIT[0].plotmax +=
      -axischar.height * 3 - (ndigs + 4) * labelchar.width - TICKFRAC;
  } else
    PLOTLIMIT[0].plotmax -= axischar.width * 3;


  ndigs = needdigits(2L);

  PLOTLIMIT[0].plotmin += axischar.height * 3 +
			  (ndigs + 4) * labelchar.width + TICKFRAC;

  FORLIM = num_axes;
  for (i = 1; i <= FORLIM; i++) {
    WITH = &DATALIMIT[i - 1];
    if (i == 1)
      j = 1;
    else
      j = 2;
    WITH1 = &PLOTLIMIT[j - 1];
    transform(WITH->datamin, WITH->datamax, (double)WITH1->plotmin,
	      (double)WITH1->plotmax, &WITH->scale, &WITH->offset);
  }

}


Local Char *findstr(Result, i)
Char *Result;
long i;
{
  switch (i) {

  case -12:
    strcpy(Result, "p");
    break;

  case -9:
    strcpy(Result, "n");
    break;

  case -6:
    if (texmode)   /*DAVE 11/10/87*/
      strcpy(Result, "$\\mu$ ");
    else
      strcpy(Result, "u");
    break;

  case -3:
    strcpy(Result, "m");
    break;

  case 3:
    strcpy(Result, "K");
    break;

  case 6:
    strcpy(Result, "M");
    break;

  case 9:
    strcpy(Result, "T");
    break;

  default:
    strcpy(Result, "");
    break;
  }
  return Result;
}





Void mam_drawaxes()
{
  long xmid, ymid, len;
  Char out[256];
  _REC_device_limits *WITH;
  _REC_axis_limits *WITH1;
  Char STR1[256], STR2[256], STR3[256];


  if (drewaxes) {
    return;
  }  /*if*/
  drewaxes = true;


  WITH = &PLOTLIMIT[1];
  WITH1 = DATALIMIT;
  /*label the x-axis*/

  xmid = (PLOTLIMIT[0].plotmax - PLOTLIMIT[0].plotmin) / 2 + WITH->plotmin;
  label_axis(WITH1->dplotype, 1L, WITH->plotmin, WITH->plotmax);
  setupcharlimits(&axischar);
  if (WITH1->engineering && *WITH1->units != '\0') {   /*DAVE 11/10/87*/
    sprintf(STR3, "%s  (%s%s)",
	    WITH1->title, findstr(STR2, WITH1->dpower), WITH1->units);

    plot_genstring(xmid, WITH->titlemin + axischar.height * 2, "uc", STR3);
  } else if (WITH1->dplotype == pt_linear && WITH1->dpower != 0 &&
	     !WITH1->integerlabel) {
    sprintf(out, "%ld", WITH1->dpower);
    len = strlen(out) + 1;
    out[len - 1] = '\0';
    sprintf(STR2, "%s  (%s %s)",
	    WITH1->title, superscript(STR3, "10", out), WITH1->units);
/* p2c: mamplot.text, line 1718:
 * Note: Modification of string length may translate incorrectly [146] */
    plot_genstring(xmid, WITH->titlemin + axischar.height * 2 +
			 lowerfudge * (axischar.height * 3 / 2), "uc", STR2);
  } else if (*WITH1->units != '\0') {
    sprintf(STR3, "%s  (%s)", WITH1->title, WITH1->units);

    plot_genstring(xmid, WITH->titlemin + axischar.height * 2, "uc", STR3);
  } else
    plot_genstring(xmid, WITH->titlemin + axischar.height * 2, "uc",
		   WITH1->title);

  out[255] = '\0';

  /*label the y-axis*/
  ymid = (PLOTLIMIT[1].plotmax - PLOTLIMIT[1].plotmin) / 2 + PLOTLIMIT[1].plotmin;

  WITH1 = &DATALIMIT[1];
  WITH = PLOTLIMIT;
  label_axis(WITH1->dplotype, 2L, WITH->plotmin, WITH->plotmax);
  setupcharlimits(&axischar);
  plot_charorient(90.0, false);

  if (WITH1->engineering && *WITH1->units != '\0') {   /*DAVE 11/10/87*/
    sprintf(STR1, "%s  (%s%s)",
	    WITH1->title, findstr(STR2, WITH1->dpower), WITH1->units);

    plot_centerstring(WITH->titlemin + axischar.height * 2, ymid, STR1);
  } else if (WITH1->dplotype == pt_linear && WITH1->dpower != 0 &&
	     !WITH1->integerlabel) {
    sprintf(out, "%ld", WITH1->dpower);
    len = strlen(out) + 1;
    out[len - 1] = '\0';
    sprintf(STR2, "%s  (%s %s)",
	    WITH1->title, superscript(STR1, "10", out), WITH1->units);
/* p2c: mamplot.text, line 1757:
 * Note: Modification of string length may translate incorrectly [146] */
    plot_centerstring(WITH->titlemin + axischar.height * 2, ymid, STR2);
  } else if (*WITH1->units != '\0') {
    sprintf(STR1, "%s  (%s)", WITH1->title, WITH1->units);
    /*The user specified string for units was blank and there is no exponent*/
    plot_centerstring(WITH->titlemin + axischar.height * 2, ymid, STR1);
  } else
    plot_centerstring(WITH->titlemin + axischar.height * 2, ymid,
		      WITH1->title);

  plot_charorient(0.0, false);

  if (num_axes != 3)
    return;

  WITH1 = &DATALIMIT[2];
  WITH = PLOTLIMIT;
  if (wallpaper)
    printf("calling define axis with %s\n",
	   plottype_NAMES[(long)WITH1->dplotype]);
  label_axis(WITH1->dplotype, 3L, WITH->plotmax, WITH->plotmin);
  setupcharlimits(&axischar);
  plot_charorient(90.0, false);
  if (WITH1->dplotype == pt_linear && WITH1->dpower != 0 &&
      !WITH1->integerlabel) {
    sprintf(out, "%ld", WITH1->dpower);
    len = strlen(out) + 1;
    out[len - 1] = '\0';
    sprintf(STR2, "%s (%s %s)",
	    WITH1->title, superscript(STR3, "10", out), WITH1->units);
/* p2c: mamplot.text, line 1785:
 * Note: Modification of string length may translate incorrectly [146] */
    plot_centerstring(WITH->titlemax - axischar.height, ymid, STR2);
  } else if (*WITH1->units != '\0') {
    sprintf(STR3, "%s (%s)", WITH1->title, WITH1->units);
    plot_centerstring(WITH->titlemax - axischar.height, ymid, STR3);
  } else
    plot_centerstring(WITH->titlemax - axischar.height, ymid, WITH1->title);
  plot_charorient(0.0, false);


  /*This is the normal case there is an exponent, ie dpower <> 0*/
  /*It is a linear graph and we are not doing integer labels*/


  /*This is a log or linear graph with no exponent with units*/



  /*This is the normal case there is an exponent, ie dpower <> 0*/
  /*It is a linear graph and we are not doing integer labels*/


  /*This is a log or linear graph with no exponent with units*/


}


Void mam_drawborder()
{
  if (drewborder)
    return;
  drewborder = true;
  plot_color(bordercolor);
  if (mirrored[0]) {
    movabs(PLOTLIMIT[0].plotmax, PLOTLIMIT[1].plotmax);
    drawabs(PLOTLIMIT[0].plotmin, PLOTLIMIT[1].plotmax);
  } else
    movabs(PLOTLIMIT[0].plotmin, PLOTLIMIT[1].plotmax);
  drawabs(PLOTLIMIT[0].plotmin, PLOTLIMIT[1].plotmin);
  drawabs(PLOTLIMIT[0].plotmax, PLOTLIMIT[1].plotmin);
  if (num_axes == 3 || mirrored[1])
    drawabs(PLOTLIMIT[0].plotmax, PLOTLIMIT[1].plotmax);
}



Void mam_close_generic()
{
  mam_drawaxes();
  mam_drawborder();
}








Static plottype readplottype(name)
Char *name;
{
  Char STR1[256];

  if (!strcmp(strlower(STR1, name), "log"))
    return pt_log;
  else
    return pt_linear;
}


/**************************************************/
Void mam_axis(xtitle, xunits, xplottype, ytitle, yunits, yplottype)
Char *xtitle, *xunits, *xplottype, *ytitle, *yunits, *yplottype;
{
  num_axes = 2;
  strcpy(DATALIMIT[0].title, xtitle);
  strcpy(DATALIMIT[0].units, xunits);
  DATALIMIT[0].dplotype = readplottype(xplottype);
  strcpy(DATALIMIT[1].title, ytitle);
  strcpy(DATALIMIT[1].units, yunits);
  DATALIMIT[1].dplotype = readplottype(yplottype);
  set_axis();
}


/*******************************************************************/


/**************************************************/
Void mam_axis3(xtitle, xunits, xplottype, y1title, y1units, y1plottype,
	       y2title, y2units, y2plottype)
Char *xtitle, *xunits, *xplottype, *y1title, *y1units, *y1plottype, *y2title,
     *y2units, *y2plottype;
{
  num_axes = 3;
  strcpy(DATALIMIT[0].title, xtitle);
  strcpy(DATALIMIT[0].units, xunits);
  DATALIMIT[0].dplotype = readplottype(xplottype);
  strcpy(DATALIMIT[1].title, y1title);
  strcpy(DATALIMIT[1].units, y1units);
  DATALIMIT[1].dplotype = readplottype(y1plottype);
  strcpy(DATALIMIT[2].title, y2title);
  strcpy(DATALIMIT[2].units, y2units);
  DATALIMIT[2].dplotype = readplottype(y2plottype);
  set_axis();
}


/*******************************************************************/

Void mam_plotnormal(x, y, len, normalx)
double **x, **y;
long len;
double **normalx;
{
  long i;
  double xval, yval;

  plot_color(datacolor);
  plot_linestyle(datastyle);
  if (len > 0) {
    if (DATALIMIT[1].dplotype == pt_log)
      yval = mam_log((*y)[0]);
    else
      yval = (*y)[0];
    if (DATALIMIT[0].dplotype == pt_log)
      xval = mam_log((*x)[0]) / ((*normalx)[0]);
    else
      xval = (*x)[0];
    move_(xval, yval, 2L);


    for (i = 1; i < len; i++) {
      if (DATALIMIT[1].dplotype == pt_log)
	yval = mam_log((*y)[i]);
      else
	yval = (*y)[i];
      if (DATALIMIT[0].dplotype == pt_log)
	xval = mam_log((*x)[i]) / ((*normalx)[0]);
      else
	xval = (*x)[i];
      printf("% .5E\n", xval);
      draw(xval, yval, 2L);
    }
  }
  plot_linestyle(0L);
}


/*******************************************************************/

Void mam_plot(x, y, len)
double **x, **y;
long len;
{
  long i;
  double xval, yval;

  plot_color(datacolor);
  plot_linestyle(datastyle);
  if (len > 0) {
    if (DATALIMIT[1].dplotype == pt_log)
      yval = mam_log((*y)[0]);
    else
      yval = (*y)[0];
    if (DATALIMIT[0].dplotype == pt_log)
      xval = mam_log((*x)[0]);
    else
      xval = (*x)[0];
    move_(xval, yval, 2L);


    for (i = 1; i < len; i++) {
      if (DATALIMIT[1].dplotype == pt_log)
	yval = mam_log((*y)[i]);
      else
	yval = (*y)[i];
      if (DATALIMIT[0].dplotype == pt_log)
	xval = mam_log((*x)[i]);
      else
	xval = (*x)[i];
      draw(xval, yval, 2L);
    }
  }
  plot_linestyle(0L);
}


/*******************************************************************/
Void mam_plot3(x, y, len)
double **x, **y;
long len;
{
  long i;
  double xval, yval;

  plot_color(datacolor);
  plot_linestyle(datastyle);
  if (len > 0) {
    if (DATALIMIT[2].dplotype == pt_log)
      yval = mam_log((*y)[0]);
    else
      yval = (*y)[0];
    if (DATALIMIT[0].dplotype == pt_log)
      xval = mam_log((*x)[0]);
    else
      xval = (*x)[0];
    move_(xval, yval, 3L);
    for (i = 1; i < len; i++) {
      if (DATALIMIT[2].dplotype == pt_log)
	yval = mam_log((*y)[i]);
      else
	yval = (*y)[i];
      if (DATALIMIT[0].dplotype == pt_log)
	xval = mam_log((*x)[i]);
      else
	xval = (*x)[i];
      draw(xval, yval, 3L);
    }
  }
  plot_linestyle(0L);
}


/*******************************************************************/

Void mam_splot(x, y, len, symbol)
double **x, **y;
long len;
Char symbol;
{
  long i, fudgex, fudgey;
  double xval, yval;
  Char STR1[2];

  setupcharlimits(&symbolchar);
  /*fudgex := trunc(symbolchar.height * symbolxoff);*/
  /*fudgey := trunc(symbolchar.height * symbolyoff);*/
  fudgex = 0;
  fudgey = 0;
  for (i = 0; i < len; i++) {
    if (DATALIMIT[1].dplotype == pt_log)
      yval = mam_log((*y)[i]);
    else
      yval = (*y)[i];
    if (DATALIMIT[0].dplotype == pt_log)
      xval = mam_log((*x)[i]);
    else
      xval = (*x)[i];

    if (xval >= DATALIMIT[0].datamin && xval <= DATALIMIT[0].datamax &&
	yval >= DATALIMIT[1].datamin && yval <= DATALIMIT[1].datamax) {
      sprintf(STR1, "%c", symbol);
      plot_string(tranx(xval) + fudgex, trany(yval, 2L) + fudgey, STR1);
    }
  }

}


/*******************************************************************/



Void mam_splot3(x, y, len, symbol)
double **x, **y;
long len;
Char symbol;
{
  long i, fudgex, fudgey;
  double xval, yval;
  Char STR2[2];

  setupcharlimits(&symbolchar);
  /*fudgex := trunc(symbolchar.height * symbolxoff);*/
  /*fudgey := trunc(symbolchar.height * symbolyoff);*/
  fudgex = 0;
  fudgey = 0;
  for (i = 0; i < len; i++) {
    if (wallpaper)
      printf("in loop\n");

    if (DATALIMIT[2].dplotype == pt_log)
      yval = mam_log((*y)[i]);
    else
      yval = (*y)[i];
    if (DATALIMIT[0].dplotype == pt_log)
      xval = mam_log((*x)[i]);
    else
      xval = (*x)[i];

    if (xval >= DATALIMIT[0].datamin && xval <= DATALIMIT[0].datamax &&
	yval >= DATALIMIT[2].datamin && yval <= DATALIMIT[2].datamax) {
      sprintf(STR2, "%c", symbol);
      plot_string(tranx(xval) + fudgex, trany(yval, 3L) + fudgey, STR2);
    }
  }
  if (wallpaper)
    printf("got through splot3\n");
}




Void mam_drawlabel(x, y, lbl)
double x, y;
Char *lbl;
{
  setupcharlimits(&axischar);
  plot_string(tranx(x), trany(y, 2L), lbl);
}











/* End. */
