/* Output from P2C, the Pascal-to-C translator */
/* p2c: math.text, line 17: Note: Overflow checking is OFF */
/* p2c: math.text, line 17: Note: Range checking is OFF */
/* p2c: math.text, line 17: Note: Stack checking is OFF */
/* From input file "math.text" */


#include <p2c/p2c.h>


#define MATH_G
#include <p2c/math.h>


Const ma_complex ma_c0 = {
  0.0, 0.0
};

Const ma_complex ma_c1 = {
  1.0, 0.0
};

Const ma_complex ma_cm1 = {
  -1.0, 0.0
};

Const ma_complex ma_cI = {
  0.0, 1.0
};

Const ma_complex ma_cmI = {
  0.0, -1.0
};

Const ma_complex ma_cpi = {
  ma_pi, 0.0
};



#ifndef NEWCI_H
#include <p2c/newci.h>
#endif

#ifndef RND_H
#include <p2c/rnd.h>
#endif

#ifndef SYSGLOBALS_H
#include <p2c/sysglobals.h>
#endif


#define rshufsize       17

#define ifactmaxfast    12   /*largest n such that n! fits in an integer*/
#define lnfactmaxfast   25   /*largest n before we switch to gamma function*/
#define lnfactcachesize  25   /*size of lnfactorial cache, >= lnfactmaxfast*/


typedef struct rstaterec {
  long seed, index;
  long shuf[rshufsize];
  long n_seed;
  double n_next;
} rstaterec;


Static rstaterec sysrstaterec, *sysrstate;
Static boolean sysrstinit;   /*will be initialized to zero by the loader*/
Static double lnfactcache[lnfactcachesize - 1];





/* Global flags */

boolean ma_trapuf(flag)
boolean flag;
{
  boolean ma_Result;

  ma_Result = false;
  return ma_Result;
}





/* General operations on reals */

double ma_rround(r)
double r;
{
  boolean minus;
  double r2;
  long scale;

  if (r < 0) {
    minus = true;
    r = -r;
  } else
    minus = false;
  if (r <= LONG_MAX)
    r = (long)floor(r + 0.5);
  else {
    if (r <= 1e17) {  /*note 1e17<<sqr(maxint)*/
      r2 = r;
      scale = 0;
      while (r2 > LONG_MAX) {
	scale++;
	r2 /= 10;
      }
      r2 = (long)r2;
      while (scale > 0) {
	r2 *= 10;
	scale--;
      }
      r = (long)floor(r - r2 + 0.5) + r2;
    }
  }
  if (minus)
    return (-r);
  else
    return r;
}



double ma_rrounde(r)
double r;
{
  boolean minus;
  double r2;
  long scale;

  if (r < 0) {
    minus = true;
    r = -r;
  } else
    minus = false;
  if (r <= LONG_MAX) {
    r += 0.5;
    scale = (long)r;
    if (scale == r && (scale & 1))
      scale--;
    r = scale;
  } else {
    if (r <= 1e17) {  /*note 1e17<<sqr(maxint)*/
      r2 = r;
      scale = 0;
      while (r2 > LONG_MAX) {
	scale++;
	r2 /= 10;
      }
      r2 = (long)r2;
      while (scale > 0) {
	r2 *= 10;
	scale--;
      }
      r += 0.5 - r2;
      scale = (long)r;
      if (scale == r && (scale & 1))
	scale--;
      r = scale + r2;
    }
  }
  if (minus)
    return (-r);
  else
    return r;
}


double ma_rtrunc(r)
double r;
{
  boolean minus;
  double r2;
  long scale;

  if (r < 0) {
    minus = true;
    r = -r;
  } else
    minus = false;
  if (r <= LONG_MAX)
    r = (long)r;
  else {
    if (r <= 1e17) {  /*note 1e17<<sqr(maxint)*/
      r2 = r;
      scale = 0;
      while (r2 > LONG_MAX) {
	scale++;
	r2 /= 10;
      }
      r2 = (long)r2;
      while (scale > 0) {
	r2 *= 10;
	scale--;
      }
      r = (long)(r - r2) + r2;
    }
  }
  if (minus)
    return (-r);
  else
    return r;
}



double ma_rfloor(r)
double r;
{
  double a;

  if (r >= 0)
    return (ma_rtrunc(r));
  else {
    a = ma_rtrunc(r);
    if (a == r)
      return a;
    else
      return (a - 1);
  }
}


double ma_rceil(r)
double r;
{
  double a;

  if (r <= 0)
    return (ma_rtrunc(r));
  else {
    a = ma_rtrunc(r);
    if (a == r)
      return a;
    else
      return (a + 1);
  }
}


double ma_rfrac(r)
double r;
{
  return (r - ma_rtrunc(r));
}




/* Handy predicates */

boolean ma_isinteger(r)
double r;
{
  return (r >= LONG_MIN && r <= LONG_MAX && (long)floor(r + 0.5) == r);
}


boolean ma_isint(r)
double r;
{
  if (r >= LONG_MIN && r <= LONG_MAX)
    return ((long)floor(r + 0.5) == r);
  else
    return (ma_rround(r) == r);
}


boolean ma_rnequal(a, b, eps)
double a, b, eps;
{
  if (a == b)
    return true;
  else
    return (fabs(a - b) < eps * ma_max2(fabs(a), fabs(b)));
}




/* Operations on integers */

long ma_igcd(a, b)
long a, b;
{
  long t;
  short sa, sb, st;
  na_quadword q1;

  a = labs(a);
  b = labs(b);
  if (a < b) {
    t = a;
    a = b;
    b = t;
  }
  if (b == LONG_MIN) {
    if (a == LONG_MIN || a == 0)
      _Escape(-4);
    q1.U1.i1 = 0;
    q1.U1.i2 = LONG_MIN;
    b = (unsigned long) b % a;
  }
  while (b > 32767) {
    t = a % b;
    a = b;
    b = t;
  }
  if (b != 0) {
    if (a > 32767) {
      sa = b;
      sb = a % sa;
    } else {
      sa = a;
      sb = b;
    }
    while (sb != 0) {
      st = sa % sb;
      sa = sb;
      sb = st;
    }
    return sa;
  } else
    return a;
}




/* Handy functions for real numbers */

double ma_rmod(a, b)
double a, b;
{
  double r;

  r = ma_rfloor(a / b);
  if (r == 0)
    return a;
  else
    return (a - r * b);
}


double ma_rmod2(a, b, q)
double a, b;
long *q;
{
  double ma_Result, r;

  r = a / b;
  if (fabs(r) >= LONG_MAX) {
    r = ma_rfloor(r);
    *q = (long)floor(ma_rfrac(fabs(r) / 128) * 128 + 0.5);
    if (r < 0 && *q != 0)
      *q = 128 - *q;
    if (r == 0)
      return a;
    else
      return (a - r * b);
  }
  *q = (long)r;
  if (r < 0 && r != *q)
    (*q)--;
  if (*q == 0)
    return a;
  ma_Result = a - *q * b;
  *q &= 127;
  return ma_Result;
}


double ma_rrem(a, b)
double a, b;
{
  double r;

  r = ma_rtrunc(a / b);
  if (r == 0)
    return a;
  else
    return (a - r * b);
}


double rrem_ieee(a, b)
double a, b;
{
  double r;

  r = ma_rrounde(a / b);
  if (r == 0)
    return a;
  else
    return (a - r * b);
}


long ma_sgn(r)
double r;
{
  if (r > 0)
    return 1;
  else {
    if (r < 0)
      return -1;
    else
      return 0;
  }
}


double ma_sgn2(r, s)
double r, s;
{
  if (s > 0)
    return fabs(r);
  else {
    if (s < 0)
      return (-fabs(r));
    else
      return 0.0;
  }
}


double ma_min2(a, b)
double a, b;
{
  if (a < b)
    return a;
  else
    return b;
}


double ma_min3(a, b, c)
double a, b, c;
{
  if (a < b) {
    if (a < c)
      return a;
    else
      return c;
  } else {
    if (b < c)
      return b;
    else
      return c;
  }
}


double ma_min4(a, b, c, d)
double a, b, c, d;
{
  if (a < b) {
    if (a < c) {
      if (a < d)
	return a;
      else
	return d;
    } else {
      if (c < d)
	return c;
      else
	return d;
    }
  } else {
    if (b < c) {
      if (b < d)
	return b;
      else
	return d;
    } else {
      if (c < d)
	return c;
      else
	return d;
    }
  }
}


double ma_max2(a, b)
double a, b;
{
  if (a > b)
    return a;
  else
    return b;
}


double ma_max3(a, b, c)
double a, b, c;
{
  if (a > b) {
    if (a > c)
      return a;
    else
      return c;
  } else {
    if (b > c)
      return b;
    else
      return c;
  }
}


double ma_max4(a, b, c, d)
double a, b, c, d;
{
  if (a > b) {
    if (a > c) {
      if (a > d)
	return a;
      else
	return d;
    } else {
      if (c > d)
	return c;
      else
	return d;
    }
  } else {
    if (b > c) {
      if (b > d)
	return b;
      else
	return d;
    } else {
      if (c > d)
	return c;
      else
	return d;
    }
  }
}


double ma_mean2(a, b)
double a, b;
{
  return ((a + b) * 0.5);
}


double ma_mean3(a, b, c)
double a, b, c;
{
  return ((a + b + c) / 3);
}


double ma_mean4(a, b, c, d)
double a, b, c, d;
{
  return ((a + b + c + d) * 0.25);
}



Void ma_rswap2(a, b)
double *a, *b;
{
  double t;

  t = *a;
  *a = *b;
  *b = t;
}


Void ma_rsort2(a, b)
double *a, *b;
{
  double t;

  if (*a <= *b)
    return;
  t = *a;
  *a = *b;
  *b = t;
}


Void ma_rsort3(a, b, c)
double *a, *b, *c;
{
  double t;

  if (*a > *b) {
    t = *a;
    *a = *b;
    *b = t;
  }
  if (*b > *c) {
    t = *b;
    *b = *c;
    *c = t;
  }
  if (*a <= *b)
    return;
  t = *a;
  *a = *b;
  *b = t;
}


Void ma_rsort4(a, b, c, d)
double *a, *b, *c, *d;
{
  double t;

  ma_rsort3(a, b, c);
  if (*d >= *c)
    return;
  t = *d;
  *d = *c;
  if (t >= *b) {
    *c = t;
    return;
  }
  *c = *b;
  if (t >= *a)
    *b = t;
  else {
    *b = *a;
    *a = t;
  }
}


double ma_rbound(min, r, max)
double min, r, max;
{
  if (min <= max) {
    if (r <= min)
      return min;
    else {
      if (r >= max)
	return max;
      else
	return r;
    }
  } else {
    if (r <= max)
      return max;
    else if (r >= min)
      return min;
    else
      return r;
  }
}


boolean ma_between(min, r, max)
double min, r, max;
{
  return (min <= r && r <= max || max <= r && r <= min);
}


boolean ma_betweenm(min, r, max, margin)
double min, r, max, margin;
{
  if (min < max)
    return (min - margin <= r && r <= max + margin);
  else
    return (max - margin <= r && r <= min + margin);
}


Void ma_amin(min, r)
double *min, r;
{
  if (r < *min)
    *min = r;
}


Void ma_amax(max, r)
double *max, r;
{
  if (r > *max)
    *max = r;
}


Void ma_aminmax(min, max, r)
double *min, *max, r;
{
  if (r < *min)
    *min = r;
  if (r > *max)
    *max = r;
}




/* Epsilons */

double ma_rsucc(r)
double r;
{
  na_quadword q;

  if (r == 0) {
    q.U1.i1 = 0;
    q.U1.i2 = 0;
    q.U28.rexp = 1;
    return (q.r);
  }
  q.r = r;
  q.U28.rmant2++;
  if (q.U28.rmant2 != 0)
    return (q.r);
  if (q.U28.rmant1 < 1048575L) {
    q.U28.rmant1++;
    return (q.r);
  }
  q.U28.rmant1 = 0;
  if (q.U28.rexp >= 2046)
    _Escape(-6);
  q.U28.rexp++;
  return (q.r);
}


double ma_rpred(r)
double r;
{
  na_quadword q;

  if (r == 0)
    return 0.0;
  else {
    q.r = r;
    q.U28.rmant2--;
    if (q.U28.rmant2 != -1)
      return (q.r);
    if (q.U28.rmant1 > 0) {
      q.U28.rmant1--;
      return (q.r);
    }
    q.U28.rmant1 = 1048575L;
    if (q.U28.rexp > 1) {
      q.U28.rexp--;
      return (q.r);
    }
    if (false)
      _Escape(-7);
    q.r = 0.0;
    return (q.r);
  }
}






/* String munching */

static Void formatreal(buf, r, wid, prec, flags, minv, maxv)
Char *buf;
double r;
long wid, prec, flags;
double minv, maxv;
{
  long i, j, pwid, maxlen, expon;
  boolean nowid, carry, minus;
  Char *STR1;
  Char STR2[256];
  Char STR3[256];
  Char STR4[256];

  buf[0] = '\0';
  nowid = (wid < 0);
  minus = (r < 0);
  if (minus) {
    r = -r;
    wid--;
  }
  if (maxv == 0) {
    if (nowid)
      maxv = 1e9;
    else {
      switch (wid) {

      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
	maxv = 1e4;
	break;

      case 5:
	maxv = 1e5;
	break;

      case 6:
	maxv = 1e6;
	break;

      case 7:
	maxv = 1e7;
	break;

      case 8:
	maxv = 1e8;
	break;

      case 9:
	maxv = 1e9;
	break;

      case 10:
	maxv = 1e10;
	break;

      case 11:
	maxv = 1e11;
	break;

      case 12:
	maxv = 1e12;
	break;

      case 13:
	maxv = 1e13;
	break;

      case 14:
	maxv = 1e14;
	break;

      case 15:
	maxv = 1e15;
	break;

      default:
	maxv = 1e16;
	break;
      }
    }
  }
  if (r < minv && r != 0 || r >= maxv) {
    if (nowid) {
      sprintf(buf, "%.5E", r);
      i = strlen(buf) + 1;
    } else {
      sprintf(buf, "%50.43E", r);
      i = strlen(buf) + 1;
    }
  } else {
    if (prec >= 0) {
      sprintf(buf, "%50.*f", (int)prec, r);
      i = strlen(buf) + 1;
      if (prec == 0)
	strcat(buf, ".");
    } else if (nowid) {
      sprintf(buf, "%50.9f", r);
      i = strlen(buf) + 1;
    } else {
      sprintf(buf, "%50.18f", r);
      i = strlen(buf) + 1;
    }
  }
  i = strposc(buf, 'E', 1L);
  if (i == 0)
    expon = LONG_MAX;
  else {
    expon = strtol(buf + i, &STR1, 10);
    j = STR1 - buf + 1;
    buf[i - 1] = '\0';
/* p2c: math.text, line 1013:
 * Note: Modification of string length may translate incorrectly */
    if (expon < 0)
      wid--;
    if (labs(expon) >= 10) {
      if (labs(expon) >= 100)
	wid -= 4;
      else
	wid -= 3;
    } else
      wid -= 2;
  }
  if (strends(buf, " "))
    strcpy(buf, strrtrim(strcpy(STR2, buf)));
  if (*buf == ' ') {
    strcpy(STR3, strltrim(buf));
    strcpy(buf, STR3);
  }
  if (nowid)
    pwid = LONG_MAX;
  else {
    if (expon != LONG_MAX && prec >= 0)
      pwid = P_imin2(prec + (prec > 0) + 1, wid);
    else
      pwid = wid;
  }
  if (strlen(buf) > pwid) {
    i = strposc(buf, '.', 1L);
    maxlen = P_imax2(pwid, i);
    carry = (buf[maxlen] >= '5');
    buf[maxlen] = '\0';
/* p2c: math.text, line 1039:
 * Note: Modification of string length may translate incorrectly */
    while (carry && maxlen >= 1) {
      if (buf[maxlen - 1] == '.') {
	maxlen--;
	continue;
      }
      buf[maxlen - 1]++;
      carry = (buf[maxlen - 1] > '9');
      if (!carry)
	break;
      buf[maxlen - 1] = '0';
      maxlen--;
    }
    if (carry) {
      sprintf(buf, "1%s", strcpy(STR4, buf));
      if (strends(buf, "0"))
	buf[strlen(buf) - 1] = '\0';
    }
  }
  if (prec < 0 || (flags & ma_fmt_vprec) != 0) {
    while (buf[strlen(buf) - 1] == '0')
      buf[strlen(buf) - 1] = '\0';
  }
  if ((prec != 0 || expon != LONG_MAX) && buf[strlen(buf) - 1] == '.')
    buf[strlen(buf) - 1] = '\0';
  if (minus) {
    i = strlen(buf);
    while (i >= 1 && (buf[i - 1] == 'e' || buf[i - 1] == '.' || buf[i - 1] == '0'))
      i--;
    if (i > 0) {
      sprintf(buf, "-%s", strcpy(STR4, buf));
      wid++;
    }
  }
  if ((long)strlen(buf) < wid) {
    sprintf(STR4, "%*s%s", (int)(wid - strlen(buf)), "", buf);
    strcpy(buf, STR4);
  }
  if (expon != LONG_MAX) {
    strcat(buf, "e");
    sprintf(buf + strlen(buf), "%ld", expon);
  }

}



Char *ma_strfmtreal(ma_Result, r, wid, prec)
Char *ma_Result;
double r;
long wid, prec;
{
  Char s[256];

  formatreal(s, r, wid, prec, 0L, 1e-2, 0.0);
  return strcpy(ma_Result, s);
}


Char *ma_strfmtreal2(ma_Result, r, wid, prec, flags, min, max)
Char *ma_Result;
double r;
long wid, prec, flags;
double min, max;
{
  Char s[256];

  formatreal(s, r, wid, prec, flags, min, max);
  return strcpy(ma_Result, s);
}








/* Logarithms and roots */

double ma_log(r)
double r;
{
  return (log(r) * ma_iln10);
}


double ma_ytoi(y, i)
double y;
long i;
{
  double yy;

  if (y == 0.0) {
    if (i <= 0)   /*"divide by zero"*/
      _Escape(-5);
    else
      return 0.0;
  } else {
    if (i < 0) {
      y = 1 / y;
      i = -i;
    }
    if (i & 1)
      yy = y;
    else
      yy = 1.0;
    while (i > 1) {
      i /= 2;
      y *= y;
      if (i & 1)
	yy *= y;
    }
    return yy;
  }
}


double ma_yrooti(y, i)
double y;
long i;
{
  double ma_Result;

  if (y == 0.0) {
    if (i <= 0)   /*"divide by zero"*/
      _Escape(-5);
    else
      return 0.0;
  }
  if (i < 0) {
    y = 1 / y;
    i = -i;
  }
  switch (i) {

  case 0:
    _Escape(-5);
    break;

  case 1:
    ma_Result = y;
    break;

  case 2:
    ma_Result = sqrt(y);
    break;

  case 4:
    ma_Result = sqrt(sqrt(y));
    break;

  default:   /*should we use Newton's method?*/
    ma_Result = exp(log(y) / i);
    break;
  }
  return ma_Result;
}


double ma_ytox(y, x)
double y, x;
{
  long i;

  if (fabs(x) <= LONG_MAX) {
    if (x == 0.5)
      return sqrt(y);
    else {
      i = (long)floor(x + 0.5);
      if (i == x)
	return (ma_ytoi(y, i));
      else
	return exp(x * log(y));
    }
  } else {
    if (y == 0)
      return 0.0;
    else
      return exp(x * log(y));
  }
}


double ma_mytox(y, x)
double y, x;
{
  if (y > 0)
    return exp(x * log(y));
  else
    return (ma_ytox(y, x));
}


/* Warning: this is almost, but not quite, monotonic*/
double ma_tentox(x)
double x;
{
  long i;

  if (x < -1000)
    return 0.0;
  else {
    i = (long)floor(x + 0.5);
    if (i == x)
      return (ma_ytoi(10.0, i));
    else
      return exp(x * ma_ln10);
  }
}


double ma_mtentox(x)
double x;
{
  return exp(x * ma_ln10);
}


double ma_hypot(a, b)
double a, b;
{
  double TEMP;

  a = fabs(a);
  b = fabs(b);
  if (a < b) {
    if (a == 0)
      return b;   /*vv "is one too big, or both too small?"*/
    else {
      if (b > 1e100 || b < 1e-100) {
	TEMP = a / b;
	return (b * sqrt(1 + TEMP * TEMP));
      } else
	return sqrt(a * a + b * b);
    }
  } else {
    if (b == 0)
      return a;
    else if (a > 1e100 || a < 1e-100) {
      TEMP = b / a;
      return (a * sqrt(1 + TEMP * TEMP));
    } else
      return sqrt(a * a + b * b);
  }
}


Void ma_quadrsolve(a, b, c, x1, x2)
double a, b, c, *x1, *x2;
{
  double d, q;

  if (a == 0) {
    if (b == 0)
      *x1 = ma_maxreal_;
    else
      *x1 = -(c / b);
    *x2 = *x1;
    return;
  }
  d = b * b - 4 * a * c;
  if (d <= 0) {
    if (d == 0) {
      *x1 = b / (-2 * a);
      *x2 = *x1;
    } else {
      *x1 = ma_maxreal_;
      *x2 = ma_maxreal_;
    }
    return;
  }
  q = sqrt(d);
  if (b >= 0)
    q = -0.5 * (b + q);
  else
    q = -0.5 * (b - q);
  *x1 = q / a;
  *x2 = c / q;
}





/* Trigonometric and inverse trigonometric functions */

Void ma_sincos(r, s, c)
double r, *s, *c;
{
  *s = sin(r);
  *c = cos(r);
}


double ma_tan(r)
double r;
{
  return (sin(r) / cos(r));
}


double ma_arctan2(r1, r2)
double r1, r2;
{
  /* = arctan(r1/r2) */
  if (r1 == 0) {
    if (r2 >= 0)
      return 0.0;
    else
      return ma_pi;
  } else {
    if (r2 == 0) {
      if (r1 > 0)
	return ma_hpi;
      else
	return (-ma_hpi);
    } else if (r2 > 0)
      return atan(r1 / r2);
    else {
      if (r1 > 0)
	return (atan(r1 / r2) + ma_pi);
      else
	return (atan(r1 / r2) - ma_pi);
    }
  }
}


double ma_arcsin(r)
double r;
{
  return (ma_arctan2(r, sqrt(1 - r * r)));
}


double ma_arccos(r)
double r;
{
  return (ma_arctan2(sqrt(1 - r * r), r));
}






/* Trigonometric functions in degrees */
/* Warning: these may be slightly non-monotonic */

Void ma_sincosd(r, s, c)
double r, *s, *c;
{
  long i;

  if (fabs(r) <= LONG_MAX) {
    i = (long)floor(r + 0.5);
    if (i == r) {
      i %= 360;
      if (i == 0) {
	*s = 0.0;
	*c = 1.0;
	return;
      }
      if (i == 180) {
	*s = 0.0;
	*c = -1.0;
	return;
      } else {
	if (i == 90) {
	  *s = 1.0;
	  *c = 0.0;
	  return;
	} else if (i == 270) {
	  *s = -1.0;
	  *c = 0.0;
	  return;
	} else {
	  r *= ma_dr;
	  *s = sin(r);
	  *c = cos(r);
	}
      }
      return;
    }
    r *= ma_dr;
    *s = sin(r);
    *c = cos(r);
    return;
  }
  r *= ma_dr;
  *s = sin(r);
  *c = cos(r);
}


double ma_sind(r)
double r;
{
  long i;

  if (fabs(r) <= LONG_MAX) {
    i = (long)floor(r + 0.5);
    if (i == r) {
      i %= 360;
      if (i == 0 || i == 180)
	return 0.0;
      else if (i == 90)
	return 1.0;
      else if (i == 270)
	return -1.0;
      else
	return sin(i * ma_dr);
    } else
      return sin(r * ma_dr);
  } else
    return sin(r * ma_dr);
}


double ma_cosd(r)
double r;
{
  long i;

  if (fabs(r) <= LONG_MAX) {
    i = (long)floor(r + 0.5);
    if (i == r) {
      i %= 360;
      if (i == 90 || i == 270)
	return 0.0;
      else if (i == 0)
	return 1.0;
      else if (i == 180)
	return -1.0;
      else
	return cos(i * ma_dr);
    } else
      return cos(r * ma_dr);
  } else
    return cos(r * ma_dr);
}


double ma_tand(r)
double r;
{
  long i;

  if (fabs(r) <= LONG_MAX) {
    i = (long)floor(r + 0.5);
    if (i == r) {
      i %= 360;
      if (i == 90 || i == 270)   /*division by zero*/
	_Escape(-5);
      else if (i == 0 || i == 180)
	return 0.0;
      else if (i == 45 || i == 225)
	return 1.0;
      else if (i == 135 || i == 315)
	return -1.0;
      else
	return (ma_tan(i * ma_dr));
    } else
      return (ma_tan(r * ma_dr));
  } else
    return (ma_tan(r * ma_dr));
}


double ma_arcsind(r)
double r;
{
  return (ma_arcsin(r) * ma_rd);
}


double ma_arccosd(r)
double r;
{
  return (ma_arccos(r) * ma_rd);
}


double ma_arctand(r)
double r;
{
  return (atan(r) * ma_rd);
}


double ma_arctand2(r1, r2)
double r1, r2;
{
  /* = arctand(r1/r2) */
  if (r1 == 0) {
    if (r2 >= 0)
      return 0.0;
    else
      return 180.0;
  } else {
    if (r2 == 0) {
      if (r1 > 0)
	return 90.0;
      else
	return -90.0;
    } else if (r1 == r2) {
      if (r1 > 0)
	return 45.0;
      else
	return -135.0;
    } else if (r1 == -r2) {
      if (r1 > 0)
	return 135.0;
      else
	return -45.0;
    } else if (r2 > 0)
      return (ma_arctand(r1 / r2));
    else {
      if (r1 > 0)
	return (ma_arctand(r1 / r2) + 180.0);
      else
	return (ma_arctand(r1 / r2) - 180.0);
    }
  }
}






/* Hyperbolic functions */

double ma_sinh(r)
double r;
{
  double e;

  e = exp(r);
  return ((e * e - 1) / (2 * e));
}


double ma_cosh(r)
double r;
{
  double e;

  e = exp(r);
  return ((e * e + 1) / (2 * e));
}


double ma_tanh(r)
double r;
{
  double e;

  if (r < 0) {
    if (r < -300)
      return -1.0;
    else {
      e = exp(2 * r);
      return ((e - 1) / (e + 1));
    }
  } else {
    if (r > 300)
      return 1.0;
    else {
      e = exp(-2 * r);
      return ((1 - e) / (e + 1));
    }
  }
}





/* Inverse hyperbolic functions */

double ma_arcsinh(r)
double r;
{
  return log(r + sqrt(r * r + 1));
}


double ma_arccosh(r)
double r;
{
  if (r < 1.0)   /* bad argument to SQRT */
    _Escape(-17);
  else
    return log(r + sqrt(r * r - 1));
}


double ma_arctanh(r)
double r;
{
  return (log((1 + r) / (1 - r)) * 0.5);
}


#define c1_             76.18009173   /*from Numerical Recipes*/
#define c2_             (-86.50532033)
#define c3              24.01409822
#define c4              (-1.231739516)
#define c5              0.120858003e-2
#define c6              (-0.536282e-5)
#define stp             2.50662827465   /*sqrt(2*pi)*/






/* Special functions */

double ma_lngamma(x, minus)
double x;
boolean *minus;
{
  double ma_Result, xx, sinpix, val, val2;
  long ival, form;

  if (x <= lnfactmaxfast + 1) {
    ival = (long)floor(x + 0.5);
    if (ival == x) {
      if (ival <= 0)
	_Escape(-6);
      ma_Result = ma_lnfactorial(ival - 1);   /*yow! mutual recursion*/
      *minus = false;
      form = -1;
    } else {
      if (x >= 1) {
	form = 0;
	xx = x;
      } else if (x >= 0) {
	form = 1;
	xx = x + 1;
      } else {
	form = 2;
	xx = 1 - x;
      }
    }
  } else {
    form = 0;
    xx = x;
  }
  if (form < 0)
    return ma_Result;
  val = xx + 4.5;
  val = (xx - 0.5) * log(val) - val +
	log(stp * (1 + c1_ / xx + c2_ / (xx + 1) + c3 / (xx + 2) +
		   c4 / (xx + 3) + c5 / (xx + 4) + c6 / (xx + 5)));
/* p2c: math.text, line 1698: Note: Line breaker spent 1.0+0.33 seconds, 1807 tries on line 1578 */
  switch (form) {

  case 0:
    ma_Result = val;
    *minus = false;
    break;

  case 1:
    ma_Result = val - log(x);
    *minus = false;
    break;

  case 2:
    sinpix = sin(ma_pi * xx);
    if (sinpix == 0)
      _Escape(-6);
    val2 = ma_pi / sinpix;
    ma_Result = log(fabs(val2)) - val;
    *minus = (val2 < 0);
    break;
  }
  return ma_Result;
}

#undef c1_
#undef c2_
#undef c3
#undef c4
#undef c5
#undef c6
#undef stp


double ma_rgamma(x)
double x;
{
  boolean minus;
  double val;
  long ival;

  ival = (long)floor(x + 0.5);
  if (ival <= ifactmaxfast + 1 && ival >= 1 && ival == x)
    return (ma_ifactorial(ival - 1));
  else {
    val = ma_lngamma(x, &minus);
    if (minus)
      return (-exp(val));
    else
      return exp(val);
  }
}


typedef long tabletype[ifactmaxfast + 1];


Const tabletype table = {
  1, 1, 2, 6, 24, 120, 720, 5040, 40320L, 362880L, 3628800L, 39916800L,
  479001600L
};





/* Combinatorial functions */

long ma_ifactorial(n)
long n;
{
  if ((unsigned long)n > ifactmaxfast)
    _Escape(-4);
  else
    return (table[n]);
}


double ma_lnfactorial(n)
long n;
{
  boolean minus;
  long i;
  double val;

  if (n > lnfactcachesize)
    return (ma_lngamma(n + 1.0, &minus));
  else {
    if (n < 2) {
      if (n < 0)
	_Escape(-6);
      else
	return 0.0;
    } else {
      val = lnfactcache[n - 2];
      if (val != 0)
	return val;
      else {
	if (n <= ifactmaxfast)
	  val = log((double)ma_ifactorial(n));
	else if (n <= lnfactmaxfast) {
	  val = ma_ifactorial((long)ifactmaxfast);
	  for (i = ifactmaxfast + 1; i <= n; i++)
	    val *= i;
	  val = log(val);
	} else
	  val = ma_lngamma(n + 1.0, &minus);
	lnfactcache[n - 2] = val;
	return val;
      }
    }
  }
}


long ma_ibincoef(a, b)
long a, b;
{
  long res, i;

  if (a < b)
    _Escape(-4);
  else {
    if (a <= ifactmaxfast)
      return (ma_ifactorial(a) / ma_ifactorial(b) / ma_ifactorial(a - b));
    else if (a <= 29) {
      if (b * 2 > a)
	b = a - b;
      if (b == 0)
	return 1;
      else {
	res = a;
	for (i = 2; i <= b; i++)
	  res = res * (a - i + 1) / i;
	return res;
      }
    } else
      _Escape(-4);
  }
}


double ma_rbincoef(a, b)
long a, b;
{
  if (a < b)
    _Escape(-6);
  else {
    if (a <= 29)
      return (ma_ibincoef(a, b));
    else
      return (ma_rround(exp(ma_lnfactorial(a) - ma_lnfactorial(b) -
			    ma_lnfactorial(a - b))));
  }
}






/* Complex number operations */

ma_complex ma_cmake(r, i)
double r, i;
{
  ma_complex ma_Result;

  ma_Result.re = r;
  ma_Result.im = i;
  return ma_Result;
}


ma_complex ma_creal(r)
double r;
{
  ma_complex ma_Result;

  ma_Result.re = r;
  ma_Result.im = 0.0;
  return ma_Result;
}


ma_complex ma_cimag(i)
double i;
{
  ma_complex ma_Result;

  ma_Result.re = 0.0;
  ma_Result.im = i;
  return ma_Result;
}


ma_complex ma_cadd(a, b)
ma_complex a, b;
{
  ma_complex ma_Result;

  ma_Result.re = a.re + b.re;
  ma_Result.im = a.im + b.im;
  return ma_Result;
}


ma_complex ma_csub(a, b)
ma_complex a, b;
{
  ma_complex ma_Result;

  ma_Result.re = a.re - b.re;
  ma_Result.im = a.im - b.im;
  return ma_Result;
}


ma_complex ma_cmul(a, b)
ma_complex a, b;
{
  ma_complex ma_Result;

  ma_Result.re = a.re * b.re - a.im * b.im;
  ma_Result.im = a.im * b.re + a.re * b.im;
  return ma_Result;
}


ma_complex ma_cdiv(a, b)
ma_complex a, b;
{
  ma_complex ma_Result;
  double d;

  d = b.re * b.re + b.im * b.im;
  ma_Result.re = (a.re * b.re + a.im * b.im) / d;
  ma_Result.im = (a.im * b.re - a.re * b.im) / d;
  return ma_Result;
}


ma_complex ma_csqr(a)
ma_complex a;
{
  ma_complex ma_Result;

  ma_Result.re = a.re * a.re - a.im * a.im;
  ma_Result.im = 2 * a.re * a.im;
  return ma_Result;
}


ma_complex ma_csqrt(a)
ma_complex a;
{
  ma_complex ma_Result;
  double d;

  if (a.im == 0) {
    if (a.re < 0) {
      ma_Result.re = 0.0;
      ma_Result.im = sqrt(-a.re);
    } else {
      ma_Result.re = sqrt(a.re);
      ma_Result.im = 0.0;
    }
    return ma_Result;
  }
  d = ma_hypot(a.re, a.im);
  ma_Result.re = sqrt(0.5 * (d + a.re));
  if (a.im < 0)
    ma_Result.im = -sqrt(0.5 * (d - a.re));
  else
    ma_Result.im = sqrt(0.5 * (d - a.re));
  return ma_Result;
}


boolean ma_cequal(a, b)
ma_complex a, b;
{
  return (a.re == b.re && a.im == b.im);
}


boolean ma_cnequal(a, b, epssq)
ma_complex a, b;
double epssq;
{
  double TEMP, TEMP1;

  if (a.re == b.re && a.im == b.im)
    return true;
  else {
    TEMP = a.re - b.re;
    TEMP1 = a.im - b.im;
    return (TEMP * TEMP + TEMP1 * TEMP1 <
	    epssq * ma_max2(a.re * a.re + a.im * a.im,
			    b.re * b.re + b.im * b.im));
  }
}


ma_complex ma_csmul(r, a)
double r;
ma_complex a;
{
  ma_complex ma_Result;

  ma_Result.re = r * a.re;
  ma_Result.im = r * a.im;
  return ma_Result;
}


ma_complex ma_cconj(a)
ma_complex a;
{
  ma_complex ma_Result;

  ma_Result.re = a.re;
  ma_Result.im = -a.im;
  return ma_Result;
}


ma_complex ma_cneg(a)
ma_complex a;
{
  ma_complex ma_Result;

  ma_Result.re = -a.re;
  ma_Result.im = -a.im;
  return ma_Result;
}


double ma_cangle(a)
ma_complex a;
{
  return (ma_arctan2(a.im, a.re));
}


double ma_cabs(a)
ma_complex a;
{
  return (ma_hypot(a.im, a.re));
}


double ma_cabssqr(a)
ma_complex a;
{
  return (a.im * a.im + a.re * a.re);
}


double ma_cdiff(a, b)
ma_complex a, b;
{
  double TEMP, TEMP1;

  TEMP = a.re - b.re;
  TEMP1 = a.im - b.im;
  return (TEMP * TEMP + TEMP1 * TEMP1);
}  /*Scott Hemphill's favorite function*/


ma_complex ma_cnorm(a)
ma_complex a;
{
  double mag;

  mag = ma_cabs(a);
  if (mag != 0) {
    a.re /= mag;
    a.im /= mag;
  }
  return a;
}


ma_complex ma_cexp(a)
ma_complex a;
{
  ma_complex ma_Result;
  double e;

  e = exp(a.re);
  if (a.im == 0) {
    ma_Result.re = e;
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = e * cos(a.im);
    ma_Result.im = e * sin(a.im);
  }
  return ma_Result;
}


ma_complex ma_cexpi(a)
ma_complex a;
{
  ma_complex ma_Result;
  double e;

  e = exp(-a.im);
  if (a.im == 0) {
    ma_Result.re = e;
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = e * cos(a.re);
    ma_Result.im = e * sin(a.re);
  }
  return ma_Result;
}


ma_complex ma_cln(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0 && a.re > 0) {
    ma_Result.re = log(a.re);
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = 0.5 * log(a.re * a.re + a.im * a.im);
    ma_Result.im = ma_arctan2(a.im, a.re);
  }
  return ma_Result;
}


ma_complex ma_csin(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0) {
    ma_Result.re = sin(a.re);
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = sin(a.re) * ma_cosh(a.im);
    ma_Result.im = cos(a.re) * ma_sinh(a.im);
  }
  return ma_Result;
}


ma_complex ma_ccos(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0) {
    ma_Result.re = cos(a.re);
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = cos(a.re) * ma_cosh(a.im);
    ma_Result.im = -sin(a.re) * ma_sinh(a.im);
  }
  return ma_Result;
}


ma_complex ma_ctan(a)
ma_complex a;
{
  ma_complex ma_Result;
  double d;

  if (a.im == 0) {
    ma_Result.re = ma_tan(a.re);
    ma_Result.im = 0.0;
    return ma_Result;
  }
  a.re = 2 * a.re;
  a.im = 2 * a.im;
  d = cos(a.re) + ma_cosh(a.im);
  ma_Result.re = sin(a.re) / d;
  ma_Result.im = ma_sinh(a.im) / d;
  return ma_Result;
}


/* These definitions follow the Common Lisp standard; they are ugly, but
   they have the desired behavior around branch cuts and principal values. */

ma_complex ma_carcsin(a)
ma_complex a;
{
  if (a.im == 0 && fabs(a.re) < 1)
    return (ma_creal(ma_arcsin(a.re)));
  else
    return (ma_cmul(ma_cmI,
		    ma_cln(ma_cadd(ma_cmul(ma_cI, a),
				   ma_csqrt(ma_csub(ma_c1, ma_csqr(a)))))));
}


ma_complex ma_carccos(a)
ma_complex a;
{
  if (a.im == 0 && fabs(a.re) < 1)
    return (ma_creal(ma_arccos(a.re)));
  else
    return (ma_cmul(ma_cmI, ma_cln(ma_cadd(a,
		  ma_cmul(ma_cI, ma_csqrt(ma_csub(ma_c1, ma_csqr(a))))))));
/* p2c: math.text, line 2108: Note: Line breaker spent 1.5 seconds, 2231 tries on line 2089 */
}


ma_complex ma_carctan(a)
ma_complex a;
{
  if (a.im == 0)
    return (ma_creal(atan(a.re)));
  else
    return (ma_cmul(ma_cmI, ma_cln(ma_cmul(ma_cadd(ma_c1, ma_cmul(ma_cI, a)),
		  ma_csqrt(ma_cdiv(ma_c1, ma_cadd(ma_c1, ma_csqr(a))))))));
}


ma_complex ma_csinh(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0) {
    ma_Result.re = ma_sinh(a.re);
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = ma_sinh(a.re) * cos(a.im);
    ma_Result.im = ma_cosh(a.re) * sin(a.im);
  }
  return ma_Result;
}


ma_complex ma_ccosh(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0) {
    ma_Result.re = ma_cosh(a.re);
    ma_Result.im = 0.0;
  } else {
    ma_Result.re = ma_cosh(a.re) * cos(a.im);
    ma_Result.im = ma_sinh(a.re) * sin(a.im);
  }
  return ma_Result;
}


ma_complex ma_ctanh(a)
ma_complex a;
{
  ma_complex ma_Result;
  double d;

  if (a.im == 0) {
    ma_Result.re = ma_tanh(a.re);
    ma_Result.im = 0.0;
    return ma_Result;
  }
  a.re = 2 * a.re;
  a.im = 2 * a.im;
  d = ma_cosh(a.re) + cos(a.im);
  ma_Result.re = ma_sinh(a.re) / d;
  ma_Result.im = sin(a.im) / d;
  return ma_Result;
}


ma_complex ma_carcsinh(a)
ma_complex a;
{
  if (a.im == 0)
    return (ma_creal(ma_arcsinh(a.re)));
  else
    return (ma_cln(ma_cadd(a, ma_csqrt(ma_cadd(ma_c1, ma_csqr(a))))));
}


ma_complex ma_carccosh(a)
ma_complex a;
{
  if (a.im == 0 && a.re >= 1)
    return (ma_creal(ma_arccosh(a.re)));
  else
    return (ma_cln(ma_cadd(a, ma_cmul(ma_cadd(a, ma_c1),
		  ma_csqrt(ma_cdiv(ma_csub(a, ma_c1), ma_cadd(a, ma_c1)))))));
}


ma_complex ma_carctanh(a)
ma_complex a;
{
  ma_complex ma_Result;

  if (a.im == 0 && fabs(a.re) < 1)
    return (ma_creal(ma_arctanh(a.re)));
  ma_Result = ma_csmul(0.5,
		       ma_cln(ma_cdiv(ma_cadd(ma_c1, a), ma_csub(ma_c1, a))));
  if (a.im == 0 && a.re < -1)
    ma_Result.im = -ma_hpi;
  return ma_Result;
}



Char *ma_strfmtcomplex2(ma_Result, a, wid, prec, flags, min, max)
Char *ma_Result;
ma_complex a;
long wid, prec, flags;
double min, max;
{
  Char buf[256], buf2[256];
  long i;
  boolean rz, iz, flag, usespace, useparen;
  long FORLIM;
  Char STR1[256];

  formatreal(buf, a.re, -1L, prec, flags, min, max);
  formatreal(buf2, a.im, -1L, prec, flags, min, max);
  rz = true;
  FORLIM = strlen(buf);
  for (i = 0; i < FORLIM; i++) {
    if (buf[i] != 'e' && buf[i] != '.' && buf[i] != '0' && buf[i] != ' ')
      rz = false;
  }
  iz = true;
  FORLIM = strlen(buf2);
  for (i = 0; i < FORLIM; i++) {
    if (buf2[i] != 'e' && buf2[i] != '.' && buf2[i] != '0' && buf2[i] != ' ')
      iz = false;
  }
  usespace = ((flags & ma_fmt_usespace) != 0);
  useparen = ((flags & ma_fmt_useparen) != 0);
  if (!iz) {
    if (useparen) {
      sprintf(buf, "(%s", strcpy(STR1, buf));
      strcat(buf, ",");
      if (usespace)
	strcat(buf, " ");
      strcat(buf2, ")");
    } else {
      if (rz)
	*buf = '\0';
      if (*buf2 == '-') {
	if (!strncmp(buf2, "-1", 2L)) {
	  flag = true;
	  FORLIM = strlen(buf2);
	  for (i = 2; i < FORLIM; i++) {
	    if (buf2[i] != 'e' && buf2[i] != '.' && buf2[i] != '0')
	      flag = false;
	  }
	  if (flag)
	    strcpy(buf2, "-");
	}
	if (usespace && !rz) {
	  strcat(buf, " ");
	  sprintf(STR1, " %s", buf2 + 1);
	  strcpy(buf2 + 1, STR1);
	}
      } else {
	if (*buf != '\0') {
	  if (usespace && !rz)
	    strcat(buf, " + ");
	  else
	    strcat(buf, "+");
	}
	if (*buf2 == '1') {
	  flag = true;
	  FORLIM = strlen(buf2);
	  for (i = 1; i < FORLIM; i++) {
	    if (buf2[i] != 'e' && buf2[i] != '.' && buf2[i] != '0')
	      flag = false;
	  }
	  if (flag)
	    *buf2 = '\0';
	}
      }
      if ((flags & ma_fmt_usej) != 0)
	strcat(buf2, "j");
      else
	strcat(buf2, "i");
    }
    strcat(buf, buf2);
  }
  if (strlen(buf) < wid) {
    sprintf(STR1, "%*s%s", (int)(wid - strlen(buf)), "", buf);
    strcpy(buf, STR1);
  }
  return strcpy(ma_Result, buf);
}


Char *ma_strfmtcomplex(ma_Result, a, wid, prec)
Char *ma_Result;
ma_complex a;
long wid, prec;
{
  return (ma_strfmtcomplex2(ma_Result, a, wid, prec, 0L, 1e-2, 0.0));
}


Char *ma_strcomplex(ma_Result, a)
Char *ma_Result;
ma_complex a;
{
  return (ma_strfmtcomplex2(ma_Result, a, -1L, -1L, 0L, 1e-2, 0.0));
}


/* Local variables for ma_strsubreadcomplex: */
struct LOC_ma_strsubreadcomplex {
  Char *buf;
  double sign;
  long i;
} ;

Local Void skipspc(LINK)
struct LOC_ma_strsubreadcomplex *LINK;
{
  while (LINK->i <= strlen(LINK->buf) && LINK->buf[LINK->i - 1] == ' ')
    LINK->i++;
}

Local Void getsign(LINK)
struct LOC_ma_strsubreadcomplex *LINK;
{
  LINK->sign = 1.0;
  skipspc(LINK);
  if (LINK->i > strlen(LINK->buf))
    return;
  if (LINK->buf[LINK->i - 1] == '+') {
    LINK->i++;
    skipspc(LINK);
    return;
  }
  if (LINK->buf[LINK->i - 1] != '-')
    return;
  LINK->i++;
  skipspc(LINK);
  LINK->sign = -1.0;
}


Void ma_strsubreadcomplex(buf_, first, pos, a)
Char *buf_;
long first, *pos;
ma_complex *a;
{
  struct LOC_ma_strsubreadcomplex V;
  double r;
  ma_complex val;
  long j;
  boolean comma, parens, hasleft;
  Char *STR1;

  V.buf = buf_;
  V.i = first;
  skipspc(&V);
  if (V.i > strlen(V.buf))
    goto _L1;
  if (V.buf[V.i - 1] == '(') {
    V.i++;
    skipspc(&V);
    parens = true;
    if (V.i > strlen(V.buf))
      goto _L1;
  } else
    parens = false;
  getsign(&V);
  if (V.i > strlen(V.buf))
    goto _L1;
  if (V.buf[V.i - 1] == 'J' || V.buf[V.i - 1] == 'j' ||
      V.buf[V.i - 1] == 'I' || V.buf[V.i - 1] == 'i') {
    val.re = 0.0;
    val.im = V.sign;
    V.i++;
  } else {
    if (V.buf[V.i - 1] == ',') {
      if (V.sign < 0)
	goto _L1;
      r = 0.0;
      hasleft = false;
    } else {
      r = strtod(V.buf + V.i - 1, &STR1);
      V.i = STR1 - V.buf + 1;
      if (P_ioresult != 0)
	goto _L2;
      r *= V.sign;
      skipspc(&V);
      hasleft = true;
    }
    if (V.i <= strlen(V.buf) &&
	(V.buf[V.i - 1] == 'J' || V.buf[V.i - 1] == 'j' ||
	 V.buf[V.i - 1] == 'I' || V.buf[V.i - 1] == 'i')) {
      V.i++;
      val.re = 0.0;
      val.im = r;
    } else {
      val.re = r;
      if (V.i <= strlen(V.buf) && (V.buf[V.i - 1] == '-' ||
	    V.buf[V.i - 1] == '+' || V.buf[V.i - 1] == ',')) {
	j = V.i;
	comma = (V.buf[V.i - 1] == ',');
	if (comma)
	  V.i++;
	getsign(&V);
	if (V.i <= strlen(V.buf) &&
	    (V.buf[V.i - 1] == '.' || isdigit(V.buf[V.i - 1]))) {
	  r = strtod(V.buf + V.i - 1, &STR1);
	  V.i = STR1 - V.buf + 1;
	  if (P_ioresult != 0) {
	    if (comma && !hasleft)
	      goto _L1;
	    V.i = j;
	    r = 0.0;
	  } else {
	    r *= V.sign;
	    skipspc(&V);
	  }
	} else if (comma) {
	  if (V.i <= strlen(V.buf) &&
	      (V.buf[V.i - 1] == 'J' || V.buf[V.i - 1] == 'j' ||
	       V.buf[V.i - 1] == 'I' || V.buf[V.i - 1] == 'i'))
	    r = V.sign;
	  else {
	    if (V.sign < 0 || !hasleft)
	      goto _L1;
	    V.i = j;
	    r = 0.0;
	  }
	} else
	  r = V.sign;
	if (V.i <= strlen(V.buf) &&
	    (V.buf[V.i - 1] == 'J' || V.buf[V.i - 1] == 'j' ||
	     V.buf[V.i - 1] == 'I' || V.buf[V.i - 1] == 'i'))
	  V.i++;
	else if (!comma) {
	  V.i = j;
	  r = 0.0;
	}
	val.im = r;
      } else
	val.im = 0.0;
    }
  }
  if (parens) {
    skipspc(&V);
    if (V.i > strlen(V.buf) || V.buf[V.i - 1] != ')')
      goto _L1;
    V.i++;
  }
  *pos = V.i;
  *a = val;
  P_ioresult = 0;
  goto _L2;
_L1:
  P_ioresult = 14;
_L2: ;
}



ma_complex ma_strreadcomplex(buf_)
Char *buf_;
{
  Char buf[256];
  ma_complex c;
  long i;

  strcpy(buf, buf_);
  ma_strsubreadcomplex(buf, 1L, &i, &c);
  if (P_ioresult != 0)
    _Escape(-10);
  while (i <= strlen(buf) && buf[i - 1] == ' ')
    i++;
  if (i <= strlen(buf))
    P_ioresult = 14;
  if (P_ioresult != 0)
    _Escape(-10);
  return c;
}
/* p2c: math.text, line 2477: Note: Range checking is ON */








/* Shuffled/seeded random number generator */
/* Various algorithms taken from chapter 7 of "Numerical Recipes" */

static Void seedrand(st, seed)
rstaterec *st;
long seed;
{
  long i;

  if (seed <= 0)
    seed = timers_sysclock() + 1;
  for (i = 1; i <= 10; i++)
    P_random(&seed);
  for (i = 0; i < rshufsize; i++) {
    P_random(&seed);
    st->shuf[i] = seed;
  }
  st->index = P_rand(&seed, (long)rshufsize);
  st->seed = seed;
  st->n_seed = 0;
}


Void ma_seedrandom(s, seed)
Anyptr *s;
long seed;
{
  rstaterec *rs;

  if (*s == NULL) {
    rs = (rstaterec *)Malloc(sizeof(rstaterec));
    *s = (Anyptr)rs;
  } else
    rs = (rstaterec *)(*s);
  seedrand(rs, seed);
}


Void ma_initrandom(s)
Anyptr *s;
{
  rstaterec *rs;

  rs = (rstaterec *)Malloc(sizeof(rstaterec));
  *s = (Anyptr)rs;
  seedrand(rs, 0L);
}


Void ma_duprandom(d, s)
Anyptr d, s;
{
  rstaterec *rsd, *rss;

  rsd = (rstaterec *)d;
  rss = (rstaterec *)s;
  *rsd = *rss;
}


double ma_urandom(s)
Anyptr s;
{
  long i;
  double v;
  rstaterec *WITH;

  WITH = (rstaterec *)s;
  i = WITH->shuf[WITH->index];
  P_random(&WITH->seed);
  WITH->shuf[WITH->index] = WITH->seed;
  WITH->index = i / 1024 * rshufsize / 2097152L;
  v = i / 2147483648.0;
  i = WITH->shuf[WITH->index];
  P_random(&WITH->seed);
  WITH->shuf[WITH->index] = WITH->seed;
  WITH->index = i / 1024 * rshufsize / 2097152L;
  return ((v + i) / 2147483648.0);
}


double ma_rrandom(s, min, max)
Anyptr s;
double min, max;
{
  if (max <= min)
    return min;
  else
    return (ma_urandom(s) * (max - min) + min);
}


long ma_irandom(s, min, max)
Anyptr s;
long min, max;
{
  long i, diff;
  rstaterec *WITH;

  if (max <= min)
    return min;
  else {
    diff = max - min + 1;
    if (diff <= 0 || diff > 32767)
      return ((long)ma_rrandom(s, (double)min, (double)max));
    else {
      WITH = (rstaterec *)s;
      i = WITH->shuf[WITH->index];
      P_random(&WITH->seed);
      WITH->shuf[WITH->index] = WITH->seed;
      WITH->index = i / 1024 * rshufsize / 2097152L;
      return (i / 32768L * diff / 65536L + min);
    }
  }
}


double ma_nrandom(s, mean, sdev)
Anyptr s;
double mean, sdev;
{
  double ma_Result, v1, v2, r, fac;
  rstaterec *WITH;

  WITH = (rstaterec *)s;
  if (WITH->n_seed == WITH->seed) {
    WITH->n_seed = 0;
    return (WITH->n_next * sdev + mean);
  }
  do {
    v1 = 2 * ma_urandom(s) - 1;
    v2 = 2 * ma_urandom(s) - 1;
    r = v1 * v1 + v2 * v2;
  } while (r >= 1);
  fac = sqrt(-2 * log(r) / r);
  ma_Result = v1 * fac * sdev + mean;
  WITH->n_next = v2 * fac;   /*algorithm generates two values*/
  WITH->n_seed = WITH->seed;   /*so we return one and save the*/
  return ma_Result;

  /*other for next time*/
}






/* Non-seedable random number generator */

static Void seedsysrand()
{
  seedrand(&sysrstaterec, 0L);
  sysrstate = &sysrstaterec;
  sysrstinit = true;
}


long ma_rand2(min, max)
long min, max;
{
  if (!sysrstinit)
    seedsysrand();
  return (ma_irandom((Anyptr)sysrstate, min, max));
}


double ma_rrand()
{
  if (!sysrstinit)
    seedsysrand();
  return (ma_urandom((Anyptr)sysrstate));
}


double ma_rrand2(min, max)
double min, max;
{
  if (!sysrstinit)
    seedsysrand();
  return (ma_rrandom((Anyptr)sysrstate, min, max));
}


double ma_nrand2(mean, sdev)
double mean, sdev;
{
  if (!sysrstinit)
    seedsysrand();
  return (ma_nrandom((Anyptr)sysrstate, mean, sdev));
}









/* End. */
