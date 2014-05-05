/* Output from p2c, the Pascal-to-C translator */
/* From input file "numex.text" */


/* $debug$ {*/




#include <p2c/p2c.h>


#define NUMEX_G
#include <p2c/numex.h>






typedef ne_datatype typetab[76];


static Const typetab types = {
  ne_notype, ne_integer, ne_real, ne_string, ne_integer, ne_real, ne_string,
  ne_integer, ne_real, ne_string, ne_integer, ne_real, ne_string, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_integer, ne_real, ne_real, ne_real,
  ne_real, ne_real, ne_real, ne_string, ne_real, ne_integer, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_integer, ne_integer, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_integer, ne_integer, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_integer, ne_integer, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_integer, ne_integer, ne_integer,
  ne_integer, ne_integer, ne_integer, ne_real, ne_string, ne_integer, ne_real,
  ne_string, ne_integer, ne_real, ne_string, ne_integer, ne_integer, ne_real,
  ne_real, ne_integer, ne_real, ne_real, ne_real, ne_real, ne_real, ne_real,
  ne_real, ne_real
/* p2c: numex.text, line 259: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 37 [251] */
};






#define numbuiltins     11


typedef struct builtinrec {
  Char name[9];
  Char kind;
  ne_opkind op;
} builtinrec;

typedef builtinrec builtintab[numbuiltins];

static builtintab builtins;


#define numopersyms     22


typedef struct opersymrec {
  Char name;
  uchar lprec, rprec;
  unsigned iop : 7, rop : 7, sop : 7;
/* p2c: numex.text, line 302:
 * Note: Field width for IOP assumes enum ne_opkind has 76 elements [105] */
} opersymrec;

typedef opersymrec opersymtab[numopersyms];


static Const opersymtab opers = {
  { '?', 30, 29, ne_icond, ne_rcond, ne_scond },
  { '\b', 40, 40, ne_error, ne_rxor, ne_error },
  { '\005', 40, 40, ne_ior, ne_ror, ne_sor },
  { '\004', 50, 50, ne_iand, ne_rand, ne_sand },
  { '|', 60, 60, ne_or, ne_error, ne_error },
  { '@', 70, 70, ne_xor, ne_error, ne_error },
  { '&', 80, 80, ne_and, ne_error, ne_error },
  { '=', 90, 90, ne_ieq, ne_req, ne_seq },
  { '\001', 90, 90, ne_ine, ne_rne, ne_sne },
  { '<', 100, 100, ne_ilt, ne_rlt, ne_slt },
  { '>', 100, 100, ne_igt, ne_rgt, ne_sgt },
  { '\002', 100, 100, ne_ile, ne_rle, ne_sle },
  { '\003', 100, 100, ne_ige, ne_rge, ne_sge },
  { '\006', 110, 110, ne_lsh, ne_error, ne_error },
  { '\007', 110, 110, ne_rsh, ne_error, ne_error },
  { '+', 120, 120, ne_iadd, ne_radd, ne_sadd },
  { '-', 120, 120, ne_isub, ne_rsub, ne_error },
  { '*', 130, 130, ne_imul, ne_rmul, ne_error },
  { '/', 130, 130, ne_error, ne_rdiv, ne_error },
  { '\\', 130, 130, ne_idiv, ne_error, ne_error },
  { '%', 130, 130, ne_imod, ne_error, ne_error },
  { '^', 135, 134, ne_error, ne_rpow, ne_error }
};

#define neg_prec        140
#define not_prec        140
#define lnot_prec       140


#ifdef rs6000
char *strdup(string)
     char *string;
{
  char *ptr;
  ptr=malloc((strlen(string)+1)*sizeof(char));
  if(ptr!=NULL)strcpy(ptr,string);
  return(ptr);
}
#endif


Static boolean readreal(s, i, r, desc)
Char *s;
long *i;
double *r;
ne_desc *desc;
{
  boolean Result;
  long j;
  Char ch;
  Char buf[256];
  Char *STR1;
  Char STR2[256];

  while (s[*i - 1] == ' ')
    (*i)++;
  j = *i;
  if (s[*i - 1] == '-' || s[*i - 1] == '+')
    (*i)++;
  while (isdigit(s[*i - 1]))
    (*i)++;
  if (s[*i - 1] == '.' && isdigit(s[*i])) {
    do {
      (*i)++;
    } while (isdigit(s[*i - 1]));
  }
  if ((s[*i - 1] == 'E' || s[*i - 1] == 'e') && (isdigit(s[*i]) ||
	(s[*i] == '-' || s[*i] == '+') && isdigit(s[*i + 1]))) {
    (*i)++;
    if (s[*i - 1] == '-' || s[*i - 1] == '+')
      (*i)++;
    while (isdigit(s[*i - 1]))
      (*i)++;
  }
  if (*i <= j)
    return false;
  buf[*i - j] = '\0';
/* p2c: numex.text, line 394:
 * Note: Modification of string length may translate incorrectly [146] */
  memmove((Anyptr)buf, (Anyptr)(&s[j - 1]), *i - j);
  *r = strtod(buf, &STR1);
  j = STR1 - buf + 1;
  Result = true;
  if (!P_inset(s[*i - 1], desc->startident))
    return Result;
  *buf = '\0';
  while (P_inset(s[*i - 1], desc->ident) ||
	 P_inset(s[*i - 1], desc->startident)) {
    sprintf(buf + strlen(buf), "%c", s[*i - 1]);
    (*i)++;
  }
  strupper(buf, buf);
  if (strends(buf, strupper(STR2, desc->units))) {
    buf[strlen(buf) - strlen(desc->units)] = '\0';
/* p2c: numex.text, line 418:
 * Note: Modification of string length may translate incorrectly [146] */
  }
  if (*buf == '\0')
    return Result;
  ch = buf[0];
  if (ch == 'F') {
    *r *= 1e-15;
    return Result;
  }
  if (ch == 'P') {
    *r *= 1e-12;
    return Result;
  }
  if (ch == 'N') {
    *r *= 1e-9;
    return Result;
  }
  if (ch == 'U') {
    *r *= 1e-6;
    return Result;
  }
  if (ch == 'K') {
    *r *= 1e3;
    return Result;
  }
  if (ch == 'G') {
    *r *= 1e9;
    return Result;
  }
  if (ch == 'T') {
    *r *= 1e12;
    return Result;
  }
  if (ch != 'M')
    return Result;
  if (buf[1] == 'E' && buf[2] == 'G')
    *r *= 1e6;
  else
    *r *= 1e-3;
  return Result;
}



boolean ne_readreal(s_, r, desc)
Char *s_;
double *r;
ne_desc *desc;
{
  Char s[256];
  long i;
  Char STR1[256];

/* p2c: numex.text, line 475:
 * Note: Null character at end of sprintf control string [148] */
  strcpy(s, s_);
  strcpy(STR1, s);
  strcpy(s, STR1);
  i = 1;
  return (readreal(s, &i, r, desc));
}




Static Char *realunit(Result, r, u)
Char *Result;
double r;
Char *u;
{
  Char s[256];
  Char STR2[256];

  if (r == 0 || fabs(r) >= 1e15 || fabs(r) < 1e-16)
    *s = '\0';
  else if (fabs(r) >= 1e12) {
    strcpy(s, "T");
    r /= 1e12;
  } else if (fabs(r) >= 1e9) {
    strcpy(s, "G");
    r /= 1e9;
  } else if (fabs(r) >= 1e6) {
    strcpy(s, "Meg");
    r /= 1e6;
  } else if (fabs(r) >= 1e3) {
    strcpy(s, "K");
    r /= 1e3;
  } else if (fabs(r) >= 0.1)
    *s = '\0';
  else if (fabs(r) >= 1e-3) {
    strcpy(s, "m");
    r *= 1e3;
  } else if (fabs(r) >= 1e-6) {
    strcpy(s, "u");
    r *= 1e6;
  } else if (fabs(r) >= 1e-9) {
    strcpy(s, "n");
    r *= 1e9;
  } else if (fabs(r) >= 1e-12) {
    strcpy(s, "p");
    r *= 1e12;
  } else {
    strcpy(s, "f");
    r *= 1e15;
  }
  sprintf(s, "%g%s", r, strcpy(STR2, s));
  sprintf(Result, "%s%s", s, u);
  return Result;
}






Static Void dummysymproc(name, desc, symptr)
Char *name;
ne_desc *desc;
na_strlist **symptr;
{
  *symptr = strlist_find(desc->symtab, name);
}



Void ne_init(desc)
ne_desc *desc;
{
  strcpy(builtins[0].name,"ROUND");
  builtins[0].kind = 'A';
  builtins[0].op = ne_error;
  strcpy(builtins[1].name,"TRUNC");
  builtins[1].kind = 'A';
  builtins[1].op = ne_error;
  strcpy(builtins[2].name,"LN");
  builtins[2].kind = 'B';
  builtins[2].op = ne_error;
  strcpy(builtins[3].name,"EXP");
  builtins[3].kind = 'B';
  builtins[3].op = ne_error;
  strcpy(builtins[4].name,"ABS");
  builtins[4].kind = 'C';
  builtins[4].op = ne_error;
  strcpy(builtins[5].name,"SQRT");
  builtins[5].kind = 'B';
  builtins[5].op = ne_error;
  strcpy(builtins[6].name,"SIN");
  builtins[6].kind = 'B';
  builtins[6].op = ne_error;
  strcpy(builtins[7].name,"COS");
  builtins[7].kind = 'B';
  builtins[7].op = ne_error;
  strcpy(builtins[8].name,"TAN");
  builtins[8].kind = 'B';
  builtins[8].op = ne_error;
  strcpy(builtins[9].name,"ARCTAN");
  builtins[9].kind = 'B';
  builtins[9].op = ne_error;
  strcpy(builtins[10].name,"PI");
  builtins[10].kind = 'P';
  builtins[10].op = ne_error;
  desc->symtab = NULL;
  desc->casesens = false;
  desc->builtin = true;
  *desc->units = '\0';
  desc->scaled = false;
  desc->doubleeq = false;
  desc->error = (unsigned)ne_noerror;
  P_addsetr(P_expset(desc->startident, 0L), 'A', 'Z');
  P_addsetr(desc->startident, 'a', 'z');
  P_addset(desc->startident, '_');
  P_addsetr(P_expset(desc->ident, 0L), 'A', 'Z');
  P_addsetr(desc->ident, 'a', 'z');
  P_addsetr(desc->ident, '0', '9');
  P_addset(desc->ident, '_');
  desc->symproc.proc = (Anyptr)dummysymproc;
  desc->symproc.link = (Anyptr)NULL;
  desc->isfunc = false;
}




Static Void newnexrec(nex, nargs, op)
ne_nexrec **nex;
long nargs;
ne_opkind op;
{
/* p2c: numex.text, line 580:
 * Note: No SpecialSizeOf form known for NE_NEXREC.1 [187] */
/* p2c: numex.text, line 580:
 * Note: Suspicious mixture of sizes in NA_NEW [173] */
  *nex = (ne_nexrec *)Malloc(sizeof(ne_nexrec) + nargs * 4);
  (*nex)->nargs = nargs;
  (*nex)->op = (unsigned)op;
}


Static Void disposenexrec(nex)
ne_nexrec **nex;
{
/* p2c: numex.text, line 588:
 * Note: No SpecialSizeOf form known for NE_NEXREC.1 [187] */
  Free(*nex);
}



Void ne_copy(nex, newnex)
ne_nexrec *nex, **newnex;
{
  ne_opkind op;
  long i, FORLIM;

  if (nex == NULL) {
    *newnex = NULL;
    return;
  }
  op = (ne_opkind)nex->op;
  newnexrec(newnex, (long)nex->nargs, op);
  (*newnex)->UU.qw = nex->UU.qw;
  FORLIM = nex->nargs;
  for (i = 0; i < FORLIM; i++)
    ne_copy(nex->UU.U99.pvals[i], &(*newnex)->UU.U99.pvals[i]);
  if (op == ne_sc)
    nex->UU.s = (char *) strdup((*newnex)->UU.s);
}



Void ne_dispose(nex)
ne_nexrec **nex;
{
  long i, FORLIM;

  if (*nex == NULL)
    return;
  if ((ne_opkind)(*nex)->op == ne_sc)
    strdispose(&(*nex)->UU.s);
  FORLIM = (*nex)->nargs;
  for (i = 0; i < FORLIM; i++)
    ne_dispose(&(*nex)->UU.U99.pvals[i]);
  disposenexrec(nex);
}


Static Char *ne_errorkind_NAMES[] = {
  "NE_NOERROR", "NE_SYNTAX", "NE_OVERFLOW", "NE_UNDERFLOW", "NE_DIVZERO",
  "NE_STRLONG", "NE_BADTYPES", "NE_UNDEF", "NE_BADVAL"
} ;


Static Char *ne_opkind_NAMES[] = {
  "NE_ERROR", "NE_IC", "NE_RC", "NE_SC", "NE_IP", "NE_RP", "NE_SP", "NE_IF",
  "NE_RF", "NE_SF", "NE_IARG", "NE_RARG", "NE_SARG", "NE_IADD", "NE_ISUB",
  "NE_IMUL", "NE_IDIV", "NE_INEG", "NE_RADD", "NE_RSUB", "NE_RMUL", "NE_RDIV",
  "NE_RNEG", "NE_RPOW", "NE_SADD", "NE_ITOR", "NE_IMOD", "NE_RBOOL",
  "NE_SBOOL", "NE_IEQ", "NE_INE", "NE_ILT", "NE_ILE", "NE_IGT", "NE_IGE",
  "NE_REQ", "NE_RNE", "NE_RLT", "NE_RLE", "NE_RGT", "NE_RGE", "NE_SEQ",
  "NE_SNE", "NE_SLT", "NE_SLE", "NE_SGT", "NE_SGE", "NE_NOT", "NE_AND",
  "NE_OR", "NE_XOR", "NE_LSH", "NE_RSH", "NE_LNOT", "NE_IOR", "NE_ROR",
  "NE_SOR", "NE_IAND", "NE_RAND", "NE_SAND", "NE_ICOND", "NE_RCOND",
  "NE_SCOND", "NE_ROUND", "NE_TRUNC", "NE_LN", "NE_EXP", "NE_IABS", "NE_RABS",
  "NE_SQRT", "NE_SIN", "NE_COS", "NE_TAN", "NE_ARCTAN", "NE_RXP", "NE_RXOR"
} ;



Void ne_dump(nex)
ne_nexrec *nex;
{
  Char buf[256];
  long i;
  ne_opkind op;
  Char STR1[256];
  long FORLIM;

  if (nex == NULL) {
    printf("(nil)");
    return;
  }
  op = (ne_opkind)nex->op;
  switch (op) {

  case ne_ic:
    printf("%ld", nex->UU.i);
    break;

  case ne_rc:
    sprintf(buf, "%g", nex->UU.r);
    if (strpos2(buf, ".", 1) == 0 && strpos2(buf, "E", 1) == 0)
      strcat(buf, ".");
    fputs(buf, stdout);
    break;

  case ne_sc:
    printf("'%s'", nex->UU.s);
    break;

  case ne_ip:
    if (nex->UU.U10.ips != NULL)
      fputs(nex->UU.U10.ips->s, stdout);
    else
      printf("(i)");
    break;

  case ne_rp:
  case ne_rxp:
    if (nex->UU.U11.rps != NULL)
      fputs(nex->UU.U11.rps->s, stdout);
    else
      printf("(r)");
    break;

  case ne_sp:
    if (nex->UU.U12.sps != NULL)
      fputs(nex->UU.U12.sps->s, stdout);
    else
      printf("(s)");
    break;

  case ne_if:
  case ne_rf:
  case ne_sf:
    if (nex->UU.U15.fps != NULL)
      fputs(nex->UU.U15.fps->s, stdout);
    else
      printf("func");
    break;

  case ne_error:
    printf("error(%s)", ne_errorkind_NAMES[nex->UU.err]);
/* p2c: numex.text, line 678: Note:
 * Line breaker spent 0.0+2.00 seconds, 5000 tries on line 407 [251] */
    break;

  default:
    strcpy(buf, ne_opkind_NAMES[(long)op]);
    i = strlen(buf) + 1;
    buf[i - 1] = '\0';
/* p2c: numex.text, line 679:
 * Note: Modification of string length may translate incorrectly [146] */
    strcpy(buf, buf + 3);
    fputs(strlower(STR1, buf), stdout);
    break;
  }
  if (nex->nargs <= 0)
    return;
  FORLIM = nex->nargs;
  for (i = 1; i <= FORLIM; i++) {
    if (i == 1)
      putchar('[');
    else
      putchar(',');
    ne_dump(nex->UU.U99.pvals[i - 1]);
  }
  putchar(']');
}








Static boolean integerp(nex)
ne_nexrec *nex;
{
  return (nex != NULL && types[nex->op] == ne_integer);
}



Static boolean realp(nex)
ne_nexrec *nex;
{
  return (nex != NULL && types[nex->op] == ne_real);
}



Static boolean stringp(nex)
ne_nexrec *nex;
{
  return (nex != NULL && types[nex->op] == ne_string);
}





ne_nexrec *ne_makeerror(err)
ne_errorkind err;
{
  ne_nexrec *nex;

  newnexrec(&nex, 0L, ne_error);
  nex->UU.err = (unsigned)err;
  return nex;
}


Static Void optimize PP((ne_nexrec **nex, ne_desc *desc, na_strlist *which,
			 long mode));

/* Local variables for optimize: */
struct LOC_optimize {
  ne_desc *desc;
  na_strlist *which;
  long mode;
  ne_errorkind err;
  long ires;
  double rres;
  Char sres[256];
} ;

/* Local variables for opt: */
struct LOC_opt {
  struct LOC_optimize *LINK;
  ne_nexrec **nex;
} ;

Local boolean makeconstant(name, LINK)
Char *name;
struct LOC_opt *LINK;
{
  if (LINK->LINK->mode == 1 || LINK->LINK->mode == 2)
    return ((strlist_find(LINK->LINK->which, name) != NULL) ==
	    (LINK->LINK->mode == 1));
  else
    return false;
}

Local Void error(errk, LINK)
ne_errorkind errk;
struct LOC_opt *LINK;
{
  LINK->LINK->err = errk;
  _Escape(2345);
}

Local Void collapsehalf(px, py, LINK)
ne_nexrec *px, *py;
struct LOC_opt *LINK;
{
  /*keep px, dispose py*/
  disposenexrec(LINK->nex);
  ne_dispose(&py);
  *LINK->nex = px;
}

Local Void collapsecond(px, py, pz, LINK)
ne_nexrec *px, *py, *pz;
struct LOC_opt *LINK;
{
  /*keep py, dispose px/pz*/
  disposenexrec(LINK->nex);
  ne_dispose(&px);
  ne_dispose(&pz);
  *LINK->nex = py;
}

Local Void collapseint(res, LINK)
long res;
struct LOC_opt *LINK;
{
  ne_dispose(LINK->nex);
  newnexrec(LINK->nex, 0L, ne_ic);
  (*LINK->nex)->UU.i = res;
}

Local Void collapsereal(res, LINK)
double res;
struct LOC_opt *LINK;
{
  ne_dispose(LINK->nex);
  newnexrec(LINK->nex, 0L, ne_rc);
  (*LINK->nex)->UU.r = res;
}

Local Void collapsestr(res, LINK)
Char *res;
struct LOC_opt *LINK;
{
  ne_dispose(LINK->nex);
  newnexrec(LINK->nex, 0L, ne_sc);
  (*LINK->nex)->UU.s = (char *)strdup(res);
}

Local boolean istrue(nex, LINK)
ne_nexrec *nex;
struct LOC_opt *LINK;
{
  boolean Result;

  switch ((ne_opkind)nex->op) {

  case ne_ic:
    Result = (nex->UU.i != 0);
    break;

  case ne_rc:
    Result = (nex->UU.r != 0);
    break;

  case ne_sc:
    Result = (*nex->UU.s != '\0');
    break;

  default:
    Result = false;
    break;
  }
  return Result;
}

Local boolean isfalse(nex, LINK)
ne_nexrec *nex;
struct LOC_opt *LINK;
{
  boolean Result;

  switch ((ne_opkind)nex->op) {

  case ne_error:
    Result = ((ne_errorkind)nex->UU.err == ne_badval);
    break;

  case ne_ic:
    Result = (nex->UU.i == 0);
    break;

  case ne_rc:
    Result = (nex->UU.r == 0);
    break;

  case ne_sc:
    Result = (*nex->UU.s == '\0');
    break;

  default:
    Result = false;
    break;
  }
  return Result;
}

Local boolean foldfunction(LINK)
struct LOC_opt *LINK;
{
  boolean Result;
  long i1;
  ne_nexrec *WITH;
  long FORLIM;

  WITH = *LINK->nex;
  if (!WITH->UU.U15.fp->static_ || WITH->UU.U15.fp->subnex)
    return false;
  Result = true;
  FORLIM = WITH->nargs;
  for (i1 = 0; i1 < FORLIM; i1++) {
    if (WITH->UU.U99.pvals[i1]->op >= 32 ||
	((1L << WITH->UU.U99.pvals[i1]->op) & ((1L << ((long)ne_ic)) |
	   (1L << ((long)ne_rc)) | (1L << ((long)ne_sc)))) == 0)
      Result = false;
  }
  return Result;
}

Local ne_nexrec *opt(nex_, LINK)
ne_nexrec **nex_;
struct LOC_optimize *LINK;
{
  struct LOC_opt V;
  long i1;
  ne_nexrec *WITH;
  long FORLIM;
  Char STR1[256];

  V.LINK = LINK;
  V.nex = nex_;
  if (*V.nex == NULL)
    return (*V.nex);
  WITH = *V.nex;
  if ((ne_opkind)WITH->op != (int)ne_scond &&
      (ne_opkind)WITH->op != (int)ne_sor &&
      (ne_opkind)WITH->op != (int)ne_sand &&
      (ne_opkind)WITH->op != (int)ne_rcond &&
      (ne_opkind)WITH->op != (int)ne_ror &&
      (ne_opkind)WITH->op != (int)ne_rand &&
      (ne_opkind)WITH->op != (int)ne_icond &&
      (ne_opkind)WITH->op != (int)ne_ior &&
      (ne_opkind)WITH->op != (int)ne_iand &&
      (ne_opkind)WITH->op != (int)ne_rxor &&
      (ne_opkind)WITH->op != (int)ne_lnot) {
    FORLIM = WITH->nargs;
    for (i1 = 0; i1 < FORLIM; i1++)
      WITH->UU.U99.pvals[i1] = opt(&WITH->UU.U99.pvals[i1], LINK);
  }
  /* optimize(pvals[i1], desc, which, mode); */
  switch ((ne_opkind)WITH->op) {

  case ne_error:
    error((ne_errorkind)WITH->UU.err, &V);
    break;

  case ne_ip:
    if (WITH->UU.U10.ips != NULL && makeconstant(WITH->UU.U10.ips->s, &V)) {
      WITH->op = (unsigned)ne_ic;
      WITH->UU.i = *WITH->UU.U10.ip;
    }
    break;

  case ne_rp:
  case ne_rxp:
    if (WITH->UU.U11.rps != NULL && makeconstant(WITH->UU.U11.rps->s, &V)) {
      WITH->op = (unsigned)ne_rc;
      WITH->UU.r = *WITH->UU.U11.rp;
      if ((ne_opkind)WITH->op == ne_rxp && WITH->UU.r == ne_badvalue)
	error(ne_badval, &V);
    }
    break;

  case ne_sp:
    if (WITH->UU.U12.sps != NULL && makeconstant(WITH->UU.U12.sps->s, &V)) {
      WITH->op = (unsigned)ne_sc;
      WITH->UU.s = (char *)strdup(*WITH->UU.U12.sp);
    }
    break;

  case ne_if:
    if (foldfunction(&V)) {
      if (WITH->UU.U15.fp->UU.U5.ipr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, long *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.U5.ipr.proc)(*V.nex, &LINK->ires,
					   WITH->UU.U15.fp->UU.U5.ipr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, long *res)))
	  WITH->UU.U15.fp->UU.U5.ipr.proc)(*V.nex, &LINK->ires);
      collapseint(LINK->ires, &V);
    }
    break;

  case ne_rf:
    if (foldfunction(&V)) {
      if (WITH->UU.U15.fp->UU.rpr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, double *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.rpr.proc)(*V.nex, &LINK->rres,
					WITH->UU.U15.fp->UU.rpr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, double *res)))
	  WITH->UU.U15.fp->UU.rpr.proc)(*V.nex, &LINK->rres);
      collapsereal(LINK->rres, &V);
    }
    break;

  case ne_sf:
    if (foldfunction(&V)) {
      if (WITH->UU.U15.fp->UU.spr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, Char *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.spr.proc)(*V.nex, LINK->sres,
					WITH->UU.U15.fp->UU.spr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, Char *res)))
	  WITH->UU.U15.fp->UU.spr.proc)(*V.nex, LINK->sres);
      collapsestr(LINK->sres, &V);
    }
    break;

  case ne_iadd:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i + WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_isub:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i - WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_imul:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i * WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 1)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    break;

  case ne_idiv:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i / WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      error(ne_divzero, &V);
    break;

  case ne_ineg:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic)
      collapseint(-WITH->UU.U10.p1->UU.i, &V);
    break;

  case ne_radd:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapsereal(WITH->UU.U10.p1->UU.r + WITH->UU.U10.p2->UU.r, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	     WITH->UU.U10.p1->UU.r == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_rsub:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapsereal(WITH->UU.U10.p1->UU.r - WITH->UU.U10.p2->UU.r, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_rmul:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapsereal(WITH->UU.U10.p1->UU.r * WITH->UU.U10.p2->UU.r, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	     WITH->UU.U10.p1->UU.r == 1)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	     WITH->UU.U10.p1->UU.r == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    break;

  case ne_rdiv:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapsereal(WITH->UU.U10.p1->UU.r / WITH->UU.U10.p2->UU.r, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	     WITH->UU.U10.p1->UU.r == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_rc &&
	     WITH->UU.U10.p2->UU.r == 0)
      error(ne_divzero, &V);
    break;

  case ne_rpow:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapsereal(ma_ytox(WITH->UU.U10.p1->UU.r, WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rneg:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(-WITH->UU.U10.p1->UU.r, &V);
    break;

  case ne_sadd:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc) {
      if (strlen(WITH->UU.U10.p1->UU.s) + strlen(WITH->UU.U10.p2->UU.s) > 255)
	error(ne_strlong, &V);
      else {
	sprintf(STR1, "%s%s", WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s);
	collapsestr(STR1, &V);
      }
    } else if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	       *WITH->UU.U10.p1->UU.s == '\0')
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_sc &&
	     *WITH->UU.U10.p2->UU.s == '\0')
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_itor:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic)
      collapsereal((double)WITH->UU.U10.p1->UU.i, &V);
    break;

  case ne_imod:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic) {
/* p2c: numex.text, line 991:
 * Note: Using % for possibly-negative arguments [317] */
      collapseint(WITH->UU.U10.p1->UU.i % WITH->UU.U10.p2->UU.i, &V);
    }
    break;

  case ne_rbool:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r != 0), &V);
    break;

  case ne_sbool:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc)
      collapseint((long)(*WITH->UU.U10.p1->UU.s != '\0'), &V);
    break;

  case ne_ieq:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i == WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_ine:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i != WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_ilt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i < WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_ile:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i <= WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_igt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i > WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_ige:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint((long)(WITH->UU.U10.p1->UU.i >= WITH->UU.U10.p2->UU.i), &V);
    break;

  case ne_req:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r == WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rne:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r != WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rlt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r < WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rle:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r <= WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rgt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r > WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_rge:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_rc)
      collapseint((long)(WITH->UU.U10.p1->UU.r >= WITH->UU.U10.p2->UU.r), &V);
    break;

  case ne_seq:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) == 0),
	&V);
    break;

  case ne_sne:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) != 0),
	&V);
    break;

  case ne_slt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) < 0), &V);
    break;

  case ne_sle:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) <= 0),
	&V);
    break;

  case ne_sgt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) > 0), &V);
    break;

  case ne_sge:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_sc &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_sc)
      collapseint(
	(long)(strcmp(WITH->UU.U10.p1->UU.s, WITH->UU.U10.p2->UU.s) >= 0),
	&V);
    break;

  case ne_not:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic)
      collapseint(~WITH->UU.U10.p1->UU.i, &V);
    break;

  case ne_and:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i & WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == -1)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == -1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    break;

  case ne_or:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i | WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == -1)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == -1)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    break;

  case ne_xor:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic)
      collapseint(WITH->UU.U10.p1->UU.i ^ WITH->UU.U10.p2->UU.i, &V);
    else if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	     WITH->UU.U10.p1->UU.i == 0)
      collapsehalf(WITH->UU.U10.p2, WITH->UU.U10.p1, &V);
    else if ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
	     WITH->UU.U10.p2->UU.i == 0)
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_lsh:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic) {
/* p2c: numex.text, line 1086:
 * Note: Assuming count for NA_ASL is positive [504] */
      collapseint(WITH->UU.U10.p1->UU.i << WITH->UU.U10.p2->UU.i, &V);
    } else if (((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
		WITH->UU.U10.p1->UU.i == 0) ||
	       ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
		WITH->UU.U10.p2->UU.i == 0))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_rsh:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
	(ne_opkind)WITH->UU.U10.p2->op == ne_ic) {
/* p2c: numex.text, line 1092:
 * Note: Assuming count for NA_ASL is negative [504] */
/* p2c: numex.text, line 1092:
 * Note: Assuming >> is an arithmetic shift [505] */
      collapseint(WITH->UU.U10.p1->UU.i >> WITH->UU.U10.p2->UU.i, &V);
    } else if (((ne_opkind)WITH->UU.U10.p1->op == ne_ic &&
		WITH->UU.U10.p1->UU.i == 0) ||
	       ((ne_opkind)WITH->UU.U10.p2->op == ne_ic &&
		WITH->UU.U10.p2->UU.i == 0))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_lnot:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (istrue(WITH->UU.U10.p1, &V))
      collapseint(0L, &V);
    else if (isfalse(WITH->UU.U10.p1, &V))
      collapseint(1L, &V);
    break;

  case ne_iand:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (isfalse(WITH->UU.U10.p1, &V))
      collapseint(0L, &V);
    break;

  case ne_ior:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (isfalse(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_rand:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (isfalse(WITH->UU.U10.p1, &V))
      collapsereal(0.0, &V);
    break;

  case ne_ror:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (isfalse(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_rxor:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_error &&
	(ne_errorkind)WITH->UU.U10.p1->UU.err == ne_badval)
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_sand:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (isfalse(WITH->UU.U10.p1, &V))
      collapsestr("", &V);
    break;

  case ne_sor:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (isfalse(WITH->UU.U10.p1, &V))
      collapsehalf(opt(&WITH->UU.U10.p2, LINK), WITH->UU.U10.p1, &V);
    else if (istrue(WITH->UU.U10.p1, &V))
      collapsehalf(WITH->UU.U10.p1, WITH->UU.U10.p2, &V);
    break;

  case ne_icond:
  case ne_rcond:
  case ne_scond:
    optimize(&WITH->UU.U10.p1, LINK->desc, LINK->which, LINK->mode);
    if (isfalse(WITH->UU.U10.p1, &V))
      collapsecond(WITH->UU.U10.p1, opt(&WITH->UU.U10.p3, LINK),
		   WITH->UU.U10.p2, &V);
    else if (istrue(WITH->UU.U10.p1, &V))
      collapsecond(WITH->UU.U10.p1, opt(&WITH->UU.U10.p2, LINK),
		   WITH->UU.U10.p3, &V);
    break;

  case ne_round:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapseint((long)floor(WITH->UU.U10.p1->UU.r + 0.5), &V);
    break;

  case ne_trunc:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapseint((long)WITH->UU.U10.p1->UU.r, &V);
    break;

  case ne_ln:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(log(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_exp:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(exp(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_iabs:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_ic)
      collapseint(labs(WITH->UU.U10.p1->UU.i), &V);
    break;

  case ne_rabs:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(fabs(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_sqrt:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(sqrt(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_sin:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(sin(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_cos:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(cos(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_tan:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(ma_tan(WITH->UU.U10.p1->UU.r), &V);
    break;

  case ne_arctan:
    if ((ne_opkind)WITH->UU.U10.p1->op == ne_rc)
      collapsereal(atan(WITH->UU.U10.p1->UU.r), &V);
    break;
  }
  return (*V.nex);
}



Static Void optimize(nex, desc_, which_, mode_)
ne_nexrec **nex;
ne_desc *desc_;
na_strlist *which_;
long mode_;
{
  struct LOC_optimize V;

  V.desc = desc_;
  V.which = which_;
  V.mode = mode_;
  V.err = ne_noerror;
  TRY(try1);
    *nex = opt(nex, &V);
  RECOVER(try1);
    if (P_escapecode == -20)
      _Escape(P_escapecode);
    if (V.err == ne_noerror) {
      switch (P_escapecode) {

      case -5:
	V.err = ne_divzero;
	break;

      case -7:
	V.err = ne_underflow;
	break;

      default:
	V.err = ne_overflow;
	break;
      }
    }
    ne_dispose(nex);
    *nex = ne_makeerror(V.err);
  ENDTRY(try1);
  if ((ne_opkind)(*nex)->op == ne_error)
    V.desc->error = (unsigned)((ne_errorkind)(*nex)->UU.err);
  else
    V.desc->error = (unsigned)ne_noerror;
}




Void ne_constant(nex, desc, which_)
ne_nexrec **nex;
ne_desc *desc;
Char *which_;
{
  Char which[256];
  na_strlist *l1, *l2;
  Char buf[256];

  strcpy(which, which_);
  if (!desc->casesens)
    strupper(which, which);
  l1 = NULL;
  do {
    strword(which, buf);
    if (*buf != '\0')
      l2 = strlist_add(&l1, buf);
  } while (*which != '\0');
  optimize(nex, desc, l1, 1L);
  strlist_empty(&l1);
}



Void ne_notconstant(nex, desc, which_)
ne_nexrec **nex;
ne_desc *desc;
Char *which_;
{
  Char which[256];
  na_strlist *l1, *l2;
  Char buf[256];

  strcpy(which, which_);
  if (!desc->casesens)
    strupper(which, which);
  l1 = NULL;
  do {
    strword(which, buf);
    if (*buf != '\0')
      l2 = strlist_add(&l1, buf);
  } while (*which != '\0');
  optimize(nex, desc, l1, 2L);
  strlist_empty(&l1);
}



Void ne_constantlist(nex, desc, which)
ne_nexrec **nex;
ne_desc *desc;
na_strlist *which;
{
  optimize(nex, desc, which, 1L);
}



Void ne_notconstantlist(nex, desc, which)
ne_nexrec **nex;
ne_desc *desc;
na_strlist *which;
{
  optimize(nex, desc, which, 2L);
}



Static ne_nexrec *make2(op, nex, nex2)
ne_opkind op;
ne_nexrec *nex, *nex2;
{
  ne_nexrec *n;

  newnexrec(&n, (long)((nex != NULL) + (nex2 != NULL)), op);
  if (nex != NULL)
    n->UU.U10.p1 = nex;
  if (nex2 != NULL)
    n->UU.U10.p2 = nex2;
  return n;
}


Static ne_nexrec *make3(op, nex, nex2, nex3)
ne_opkind op;
ne_nexrec *nex, *nex2, *nex3;
{
  ne_nexrec *n;

  newnexrec(&n, 3L, op);
  n->UU.U10.p1 = nex;
  n->UU.U10.p2 = nex2;
  n->UU.U10.p3 = nex3;
  return n;
}


ne_nexrec *ne_typecast(nex, typ)
ne_nexrec *nex;
ne_datatype typ;
{
  switch (typ) {

  case ne_notype:
    ne_dispose(&nex);
    nex = ne_makeerror(ne_badtypes);
    break;

  case ne_integer:
    if (realp(nex))
      nex = make2(ne_round, nex, NULL);
    else if (!integerp(nex)) {
      ne_dispose(&nex);
      nex = ne_makeerror(ne_badtypes);
    }
    break;

  case ne_real:
    if (integerp(nex))
      nex = make2(ne_itor, nex, NULL);
    else if (!realp(nex)) {
      ne_dispose(&nex);
      nex = ne_makeerror(ne_badtypes);
    }
    break;

  case ne_string:
    if (!stringp(nex)) {
      ne_dispose(&nex);
      nex = ne_makeerror(ne_badtypes);
    }
    break;

  case ne_boolean:
    if (realp(nex))
      nex = make2(ne_rbool, nex, NULL);
    else if (stringp(nex))
      nex = make2(ne_sbool, nex, NULL);
    break;
  }
  return nex;
}


/* Local variables for ne_intcompile: */
struct LOC_ne_intcompile {
  Char *ex;
  ne_desc *desc;
  long p;
} ;

Local ne_nexrec *expr PP((long prec, struct LOC_ne_intcompile *LINK));

Local Void seterror(errk, LINK)
ne_errorkind errk;
struct LOC_ne_intcompile *LINK;
{
  if ((ne_errorkind)LINK->desc->error == ne_noerror)
    LINK->desc->error = (unsigned)errk;
  LINK->p = 0;
  while (LINK->ex[LINK->p - 1] != '\0')
    LINK->p++;
}

Local Void error_(errk, nex1, nex2, LINK)
ne_errorkind errk;
ne_nexrec **nex1, *nex2;
struct LOC_ne_intcompile *LINK;
{
  ne_dispose(nex1);
  ne_dispose(&nex2);
  seterror(errk, LINK);
}

Local Char skipspc(LINK)
struct LOC_ne_intcompile *LINK;
{
  while (LINK->ex[LINK->p - 1] == ' ')
    LINK->p++;
  return (LINK->ex[LINK->p - 1]);
}

/* Local variables for expr: */
struct LOC_expr {
  struct LOC_ne_intcompile *LINK;
} ;

Local ne_nexrec *parexp(LINK)
struct LOC_expr *LINK;
{
  ne_nexrec *nex;
  Char ch, ch2;

  nex = NULL;
  ch = skipspc(LINK->LINK);
  if (ch != '(' && ch != '[') {
    error_(ne_syntax, &nex, NULL, LINK->LINK);
    return nex;
  }
  LINK->LINK->p++;
  nex = expr(0L, LINK->LINK);
  ch2 = skipspc(LINK->LINK);
  if (ch == '(' && ch2 == ')' || ch == '[' && ch2 == ']')
    LINK->LINK->p++;
  else
    error_(ne_syntax, &nex, NULL, LINK->LINK);
  return nex;
}

Local ne_nexrec *factor(LINK)
struct LOC_expr *LINK;
{
  Char ch, ch2, kind;
  Char name[256];
  ne_nexrec *nex;
  ne_nexrec **args;
  ne_datatype atype;
  na_long value;
  na_strlist *l1;
  double *rp;
  ne_functionrec *fp;
  long i, p0;
  double r;
  boolean done;
  Char TEMP;
  long FORLIM;
  int allocdone; /* flag for deallocation */
  Anyptr tryalloc;

  nex = NULL;
  allocdone = 0;
  ch = skipspc(LINK->LINK);
  if (P_inset(ch, LINK->LINK->desc->startident)) {
    *name = '\0';
    while (P_inset(LINK->LINK->ex[LINK->LINK->p - 1], LINK->LINK->desc->ident) ||
	   P_inset(LINK->LINK->ex[LINK->LINK->p - 1],
		   LINK->LINK->desc->startident)) {
      sprintf(name + strlen(name), "%c", LINK->LINK->ex[LINK->LINK->p - 1]);
      LINK->LINK->p++;
    }
    if (!LINK->LINK->desc->casesens)
      strupper(name, name);
    ch = skipspc(LINK->LINK);
    LINK->LINK->desc->isfunc = (ch == '[' || ch == '(');
    kind = ' ';
    l1 = NULL;
    if (LINK->LINK->desc->symproc.link != NULL)
      (*(Void(*) PP((Char *name, ne_desc *desc, na_strlist **symptr,
		     Anyptr _link)))LINK->LINK->desc->symproc.proc)(name,
	LINK->LINK->desc, &l1, LINK->LINK->desc->symproc.link);
    else
      (*(Void(*) PP((Char *name, ne_desc *desc, na_strlist **symptr)))
	LINK->LINK->desc->symproc.proc)(name, LINK->LINK->desc, &l1);
    if (l1 != NULL) {
      kind = l1->kind;
      value = l1->value;
    } else {
      kind = 255;
/* p2c: numex.text, line 1448: Note: Character >= 128 encountered [281] */
    }
    switch (kind) {

    case ne_kind_intconst:
      nex = make2(ne_ic, NULL, NULL);
      nex->UU.i = (long)value;
      break;

    case ne_kind_intvalue:
      nex = make2(ne_ip, NULL, NULL);
      nex->UU.U10.ip = (long*)(&l1->value);
      nex->UU.U10.ips = l1;
      break;

    case ne_kind_intptr:
      nex = make2(ne_ip, NULL, NULL);
      nex->UU.U10.ip = (long *)value;
      nex->UU.U10.ips = l1;
      break;

    case ne_kind_realconst:
      nex = make2(ne_rc, NULL, NULL);
      rp = (double *)value;
      nex->UU.r = *rp;
      break;

    case ne_kind_srealconst:
      nex = make2(ne_rc, NULL, NULL);
      nex->UU.r = na_srtor(value);
      break;

    case ne_kind_realptr:
      nex = make2(ne_rp, NULL, NULL);
      nex->UU.U11.rp = (double *)value;
      nex->UU.U11.rps = l1;
      break;

    case ne_kind_realxptr:
      nex = make2(ne_rxp, NULL, NULL);
      nex->UU.U11.rp = (double *)value;
      nex->UU.U11.rps = l1;
      break;

    case ne_kind_strconst:
      nex = make2(ne_sc, NULL, NULL);
      nex->UU.s = (char *)strdup((Char *)value);
      break;

    case ne_kind_strvalue:
      nex = make2(ne_sp, NULL, NULL);
      nex->UU.U12.sp = (Char **)(&l1->value);
      nex->UU.U12.sps = l1;
      break;

    case ne_kind_strptr:
      nex = make2(ne_sp, NULL, NULL);
      nex->UU.U12.sp = (Char **)value;
      nex->UU.U12.sps = l1;
      break;

    case ne_kind_intfunc:
    case ne_kind_realfunc:
    case ne_kind_strfunc:
      fp = (ne_functionrec *)value;
/* p2c: numex.text, line 1510: Note: Call to NA_ALLOC [501] */
      tryalloc = (Anyptr *)(&args); /*G. Liu + */
      /* na_alloc(&args, fp->maxargs * sizeof(ne_nexrec *));*/
      na_alloc(tryalloc,fp->maxargs *sizeof(ne_nexrec *));  /* G. Liu + */
      allocdone = 1;
      i = 0;
      if (ch == '[' || ch == '(') {
	LINK->LINK->p++;
	TEMP = skipspc(LINK->LINK);
	if (!(TEMP == ']' || TEMP == ')')) {
	  done = false;
	  while (!done && i < fp->maxargs) {
	    i++;
	    if (fp->nargs > 0) {
	      atype = fp->UU.U99.ptypes[P_imin2(i, (long)fp->nargs) - 1];
	      args[i - 1] = ne_typecast(expr(0L, LINK->LINK), atype);
	    } else if (fp->subnex)
	      args[i - 1] = ne_typecast(expr(0L, LINK->LINK), ne_real);
	    else
	      args[i - 1] = expr(0L, LINK->LINK);
	    if ((ne_opkind)args[i - 1]->op == ne_error)
	      error_((ne_errorkind)args[i - 1]->UU.err, &args[i - 1], NULL,
		     LINK->LINK);
	    if (skipspc(LINK->LINK) == ',')
	      LINK->LINK->p++;
	    else
	      done = true;
	  }
	  if (!done)
	    seterror(ne_syntax, LINK->LINK);
	}
	ch2 = skipspc(LINK->LINK);
	if (ch == '(' && ch2 == ')' || ch == '[' && ch2 == ']')
	  LINK->LINK->p++;
	else
	  seterror(ne_syntax, LINK->LINK);
      }
      if (i < fp->minargs)
	seterror(ne_syntax, LINK->LINK);
      if ((ne_errorkind)LINK->LINK->desc->error == ne_noerror) {
	switch (kind) {

	case ne_kind_intfunc:
	  newnexrec(&nex, i, ne_if);
	  break;

	case ne_kind_realfunc:
	  newnexrec(&nex, i, ne_rf);
	  break;

	case ne_kind_strfunc:
	  newnexrec(&nex, i, ne_sf);
	  break;
	}
	FORLIM = nex->nargs;
	for (i = 0; i < FORLIM; i++)
	  nex->UU.U99.pvals[i] = args[i];
	nex->UU.U15.fp = fp;
	nex->UU.U15.fps = l1;
      }
      break;

    case ne_kind_intarg:
      nex = make2(ne_iarg, NULL, NULL);
      nex->UU.i = (long)value;
      break;

    case ne_kind_realarg:
      nex = make2(ne_rarg, NULL, NULL);
      nex->UU.i = (long)value;
      break;

    case ne_kind_strarg:
      nex = make2(ne_sarg, NULL, NULL);
      nex->UU.i = (long)value;
      break;

    case ne_kind_nex:
      if (value == NULL)
	error_(ne_undef, &nex, NULL, LINK->LINK);
      else
	ne_copy((ne_nexrec *)value, &nex);
      break;

    default:
      if (LINK->LINK->desc->builtin) {
	i = numbuiltins;
	while (i >= 1 && strcmp(builtins[i - 1].name, name))
	  i--;
      } else
	i = 0;
      if (i >= 1) {
	switch (builtins[i - 1].kind) {

	case 'A':
	  nex = parexp(LINK);
	  if (realp(nex))
	    nex = make2(builtins[i - 1].op, nex, NULL);
	  else if (!integerp(nex))
	    error_(ne_badtypes, &nex, NULL, LINK->LINK);
	  break;

	case 'B':
	  nex = ne_typecast(parexp(LINK), ne_real);
	  if ((ne_opkind)nex->op == ne_error)
	    error_((ne_errorkind)nex->UU.err, &nex, NULL, LINK->LINK);
	  nex = make2(builtins[i - 1].op, nex, NULL);
	  break;

	case 'C':
	  nex = parexp(LINK);
	  if (integerp(nex))
	    nex = make2(builtins[i - 1].op, nex, NULL);
	  else if (realp(nex))
	    nex = make2((ne_opkind)((long)builtins[i - 1].op + 1), nex, NULL);
	  else
	    error_(ne_badtypes, &nex, NULL, LINK->LINK);
	  break;

	case 'P':
	  nex = make2(ne_rc, NULL, NULL);
	  nex->UU.r = 3.1415926536;
	  break;
	}
      } else
	error_(ne_undef, &nex, NULL, LINK->LINK);
      break;
    }
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '+') {
    LINK->LINK->p++;
    nex = expr((long)neg_prec, LINK->LINK);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '-') {
    LINK->LINK->p++;
    nex = expr((long)neg_prec, LINK->LINK);
    if (realp(nex)) {
      nex = make2(ne_rneg, nex, NULL);
      if (allocdone) 
	na_free((Anyptr)&args);
      return nex;
    }
    if (integerp(nex))
      nex = make2(ne_ineg, nex, NULL);
    else
      error_(ne_badtypes, &nex, NULL, LINK->LINK);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '!') {
    LINK->LINK->p++;
    nex = ne_typecast(expr((long)lnot_prec, LINK->LINK), ne_boolean);
    nex = make2(ne_lnot, nex, NULL);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '~') {
    LINK->LINK->p++;
    nex = expr((long)not_prec, LINK->LINK);
    if (realp(nex))
      nex = make2(ne_round, nex, NULL);
    if (integerp(nex))
      nex = make2(ne_not, nex, NULL);
    else
      error_(ne_badtypes, &nex, NULL, LINK->LINK);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '#' || ch == '\'' || ch == '"') {
    *name = '\0';
    done = false;
    do {
      ch = skipspc(LINK->LINK);
      if (ch == '#') {
	ch = toupper(LINK->LINK->ex[LINK->LINK->p]);
	if (ch >= '@' && ch <= '_') {
	  sprintf(name + strlen(name), "%c", ch - 64);
	  LINK->LINK->p += 2;
	} else if (isdigit(ch)) {
	  i = 0;
	  LINK->LINK->p++;
	  while (isdigit(LINK->LINK->ex[LINK->LINK->p - 1])) {
	    i = i * 10 + LINK->LINK->ex[LINK->LINK->p - 1] - 48;
	    LINK->LINK->p++;
	  }
	  sprintf(name + strlen(name), "%c", (Char)(i & 255));
	} else
	  done = true;
      } else if (ch == '\'' || ch == '"') {
	done = false;
	do {
	  LINK->LINK->p++;
	  if (LINK->LINK->ex[LINK->LINK->p - 1] == ch) {
	    if (LINK->LINK->ex[LINK->LINK->p] == ch) {
	      sprintf(name + strlen(name), "%c", ch);
	      LINK->LINK->p++;
	    } else
	      done = true;
	  } else if (LINK->LINK->ex[LINK->LINK->p - 1] == '\0')
	    done = true;
	  else
	    sprintf(name + strlen(name), "%c",
		    LINK->LINK->ex[LINK->LINK->p - 1]);
	} while (!done);
	if (LINK->LINK->ex[LINK->LINK->p - 1] == ch)
	  LINK->LINK->p++;
	done = false;
      } else
	done = true;
    } while (!done);
    nex = make2(ne_sc, nex, NULL);
    nex->UU.s = (char *)strdup(name);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (ch == '[' || ch == '(') {
    nex = parexp(LINK);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  if (!(ch == '.' || isdigit(ch))) {
    error_(ne_syntax, &nex, NULL, LINK->LINK);
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  i = 0;
  p0 = LINK->LINK->p;
  while (isdigit(LINK->LINK->ex[LINK->LINK->p - 1]) && i < 214748364L ||
	 (i == 214748364L && LINK->LINK->ex[LINK->LINK->p - 1] >= '0' &&
	  LINK->LINK->ex[LINK->LINK->p - 1] <= '7')) {
    i = i * 10 + LINK->LINK->ex[LINK->LINK->p - 1] - '0';
    LINK->LINK->p++;
  }
  TEMP = skipspc(LINK->LINK);
  if (!(TEMP == '.' || isalnum(TEMP))) {
    nex = make2(ne_ic, NULL, NULL);
    nex->UU.i = i;
    if (allocdone) 
      na_free((Anyptr)&args);
    return nex;
  }
  LINK->LINK->p = p0;
  if (readreal(LINK->LINK->ex, &LINK->LINK->p, &r, LINK->LINK->desc)) {
    nex = make2(ne_rc, NULL, NULL);
    nex->UU.r = r;
  } else
    error_(ne_syntax, &nex, NULL, LINK->LINK);
  if (allocdone) 
    na_free((Anyptr)&args);
  return nex;
}

Local ne_nexrec *expr(prec, LINK)
long prec;
struct LOC_ne_intcompile *LINK;
{
  struct LOC_expr V;
  ne_nexrec *nex, *nex2, *nex3;
  Char ch;
  long opn, dp;

  V.LINK = LINK;
  nex = factor(&V);
  do {
    ch = skipspc(LINK);
    dp = 1;
    if (ch == '=' && LINK->ex[LINK->p] == '=')
      dp = 2;
    else if (ch == '!' && LINK->ex[LINK->p] == '=') {
      ch = '\001';   /*ne*/
      dp = 2;
    } else if (ch == '<') {
      if (LINK->ex[LINK->p] == '>') {
	ch = '\001';   /*ne*/
	dp = 2;
      } else if (LINK->ex[LINK->p] == '=') {
	ch = '\002';   /*le*/
	dp = 2;
      } else if (LINK->ex[LINK->p] == '<') {
	ch = '\006';   /*lsh*/
	dp = 2;
      }
    } else if (ch == '>') {
      if (LINK->ex[LINK->p] == '=') {
	ch = '\003';   /*ge*/
	dp = 2;
      } else if (LINK->ex[LINK->p] == '>') {
	ch = '\007';   /*rsh*/
	dp = 2;
      }
    } else if (ch == '&' && LINK->ex[LINK->p] == '&') {
      ch = '\004';   /*log. and*/
      dp = 2;
    } else if (ch == '|' && LINK->ex[LINK->p] == '|') {
      ch = '\005';   /*log. or*/
      dp = 2;
    } else if (ch == '!' && LINK->ex[LINK->p] == '!') {
      ch = '\b';   /*log. X-checking or*/
      dp = 2;
    }
    opn = numopersyms;
    while (opn >= 1 && opers[opn - 1].name != ch)
      opn--;
    if (opn >= 1 && opers[opn - 1].lprec > prec) {
      if (ch == '?') {
	LINK->p++;
	nex = ne_typecast(nex, ne_boolean);
	nex2 = expr(0L, LINK);
	if (skipspc(LINK) == ':') {
	  LINK->p++;
	  nex3 = expr((long)opers[opn - 1].rprec, LINK);
	  if (stringp(nex2) && stringp(nex3))
	    nex = make3(ne_scond, nex, nex2, nex3);
	  else if (!stringp(nex2) && !stringp(nex3)) {
	    if (realp(nex2) || realp(nex3))
	      nex = make3(ne_rcond, nex, ne_typecast(nex2, ne_real),
			  ne_typecast(nex3, ne_real));
	    else
	      nex = make3(ne_icond, nex, nex2, nex3);
	  } else {
	    ne_dispose(&nex3);
	    error_(ne_badtypes, &nex, nex2, LINK);
	  }
	} else
	  error_(ne_syntax, &nex, nex2, LINK);
      } else {
	LINK->p += dp;
	nex2 = expr((long)opers[opn - 1].rprec, LINK);
	if (stringp(nex) && stringp(nex2) &&
	    (ne_opkind)opers[opn - 1].sop != ne_error)
	  nex = make2((ne_opkind)opers[opn - 1].sop, nex, nex2);
	else if (stringp(nex) || stringp(nex2))
	  error_(ne_badtypes, &nex, nex2, LINK);
	else if (integerp(nex) && integerp(nex2) &&
		 (ne_opkind)opers[opn - 1].iop != ne_error)
	  nex = make2((ne_opkind)opers[opn - 1].iop, nex, nex2);
	else if ((ne_opkind)opers[opn - 1].rop != ne_error) {
	  if (integerp(nex))
	    nex = make2(ne_itor, nex, NULL);
	  if (integerp(nex2))
	    nex2 = make2(ne_itor, nex2, NULL);
	  nex = make2((ne_opkind)opers[opn - 1].rop, nex, nex2);
	} else {
	  if (realp(nex))
	    nex = make2(ne_round, nex, NULL);
	  if (realp(nex2))
	    nex2 = make2(ne_round, nex2, NULL);
	  nex = make2((ne_opkind)opers[opn - 1].iop, nex, nex2);
	}
      }
    }
  } while (opn >= 1 && opers[opn - 1].lprec > prec);
  return nex;
}



Void ne_intcompile(ex_, endp, nex, desc_)
Char *ex_;
long *endp;
ne_nexrec **nex;
ne_desc *desc_;
{
  struct LOC_ne_intcompile V;
  long startp;

  V.ex = ex_;
  V.desc = desc_;
  *nex = NULL;
  V.desc->error = (unsigned)ne_noerror;
  V.p = *endp;
  startp = *endp;
  *nex = expr(0L, &V);
  if ((ne_errorkind)V.desc->error == ne_noerror) {
    *endp = V.p;
    return;
  }
  ne_dispose(nex);
  *nex = ne_makeerror((ne_errorkind)V.desc->error);
  *endp = startp;
}


Void ne_compile(ex_, nex, desc)
Char *ex_;
ne_nexrec **nex;
ne_desc *desc;
{
  Char ex[256];
  long len, endp;
  Char STR1[256];

  strcpy(ex, ex_);
  if (strlen(ex) == 255) {
    desc->error = (unsigned)ne_strlong;
    *nex = ne_makeerror(ne_strlong);
    return;
  }
  len = strlen(ex);
/* p2c: numex.text, line 1913:
 * Note: Null character at end of sprintf control string [148] */
  strcpy(STR1, ex);
  strcpy(ex, STR1);
  endp = 1;
  ne_intcompile(ex, &endp, nex, desc);
  if (endp == len + 1) {
    if ((ne_errorkind)desc->error == ne_noerror)
      optimize(nex, desc, NULL, 0L);
    return;
  }
  desc->error = (unsigned)ne_syntax;
  ne_dispose(nex);
  *nex = ne_makeerror(ne_syntax);
}


/* Local variables for ne_uncompile: */
struct LOC_ne_uncompile {
  Char *ex;
  ne_desc *desc;
  ne_opkind lastop;
  Char ch;
  boolean flag;
} ;

Local Void uncompile PP((ne_nexrec *nex, int prec,
			 struct LOC_ne_uncompile *LINK));

Local Void append(s, LINK)
Char *s;
struct LOC_ne_uncompile *LINK;
{
  long i;

  i = strlen(LINK->ex) + strlen(s) - 255;
/* p2c: numex.text, line 1940:
 * Note: STRMAX of "ex" wants VarStrings=1 [151] */
  if (i < 1) {
    strcat(LINK->ex, s);
    return;
  }
  if (i > 1)
    sprintf(LINK->ex + strlen(LINK->ex), "%.*s", (int)(i - 1), s);
  LINK->desc->error = (unsigned)ne_strlong;
}

Local Void appendc(ch, LINK)
Char ch;
struct LOC_ne_uncompile *LINK;
{
  if (strlen(LINK->ex) < 255)
    sprintf(LINK->ex + strlen(LINK->ex), "%c", ch);
  else
    LINK->desc->error = (unsigned)ne_strlong;
/* p2c: numex.text, line 1953:
 * Note: STRMAX of "ex" wants VarStrings=1 [151] */
}

Local Void appenderrname(nex, LINK)
ne_nexrec *nex;
struct LOC_ne_uncompile *LINK;
{
  Char buf[256];
  long i;
  Char STR1[256];

  strcpy(buf, ne_errorkind_NAMES[nex->UU.err]);
  i = strlen(buf) + 1;
  buf[i - 1] = '\0';
/* p2c: numex.text, line 1966:
 * Note: Modification of string length may translate incorrectly [146] */
  strcpy(buf, buf + 3);
  append(strlower(STR1, buf), LINK);
}

/* Local variables for uncompile: */
struct LOC_uncompile {
  struct LOC_ne_uncompile *LINK;
  ne_nexrec *nex;
} ;

Local Void args(LINK)
struct LOC_uncompile *LINK;
{
  long i1;
  ne_nexrec *WITH;
  long FORLIM;

  WITH = LINK->nex;
  if (WITH->nargs == 0)
    return;
  appendc('(', LINK->LINK);
  FORLIM = WITH->nargs;
  for (i1 = 1; i1 <= FORLIM; i1++) {
    if (i1 > 1)
      appendc(',', LINK->LINK);
    uncompile(WITH->UU.U99.pvals[WITH->UU.i - 1], 0, LINK->LINK);
  }
  appendc(')', LINK->LINK);
}

Local Void func(name, LINK)
Char *name;
struct LOC_uncompile *LINK;
{
  append(name, LINK->LINK);
  args(LINK);
}

Local Void uncompile(nex_, prec, LINK)
ne_nexrec *nex_;
short prec;
struct LOC_ne_uncompile *LINK;
{
  struct LOC_uncompile V;
  ne_nexrec *nex2;
  long j;
  ne_nexrec *WITH;
  Char STR1[256];
  long FORLIM;

  V.LINK = LINK;
  V.nex = nex_;
  WITH = V.nex;
  LINK->lastop = (ne_opkind)WITH->op;
  switch ((ne_opkind)WITH->op) {

  case ne_error:
    append("error(", LINK);
    appenderrname(V.nex, LINK);
    appendc(')', LINK);
    break;

  case ne_ic:
    if (LINK->desc->scaled)
      append(realunit(STR1, (double)WITH->UU.i, LINK->desc->units), LINK);
    else {
      sprintf(STR1, "%ld", WITH->UU.i);
      append(STR1, LINK);
    }
    break;

  case ne_rc:
    if (LINK->desc->scaled)
      append(realunit(STR1, WITH->UU.r, LINK->desc->units), LINK);
    else {
      sprintf(STR1, "%g", WITH->UU.r);
      append(STR1, LINK);
    }
    break;

  case ne_sc:
    if (*WITH->UU.s == '\0')
      append("''", LINK);
    else {
      LINK->flag = false;
      FORLIM = strlen(WITH->UU.s);
      for (j = 0; j < FORLIM; j++) {
	if (WITH->UU.s[j] < ' ' || WITH->UU.s[j] > '~') {
	  if (LINK->flag)
	    appendc('\'', LINK);
	  appendc('#', LINK);
	  sprintf(STR1, "%d", WITH->UU.s[j]);
	  append(STR1, LINK);
	  LINK->flag = false;
	} else {
	  if (!LINK->flag)
	    appendc('\'', LINK);
	  appendc(WITH->UU.s[j], LINK);
	  if (WITH->UU.s[j] == '\'')
	    appendc('\'', LINK);
	  LINK->flag = true;
	}
      }
      if (LINK->flag)
	appendc('\'', LINK);
    }
    break;

  case ne_ip:
  case ne_rp:
  case ne_sp:
  case ne_rxp:
    if (WITH->UU.U10.ips != NULL)
      append(WITH->UU.U10.ips->s, LINK);
    else
      append("(var)", LINK);
    break;

  case ne_if:
  case ne_rf:
  case ne_sf:
    if (WITH->UU.U10.ips != NULL)
      append(WITH->UU.U15.fps->s, LINK);
    else
      append("(func)", LINK);
    args(&V);
    break;

  case ne_itor:
  case ne_rbool:
  case ne_sbool:
    uncompile(WITH->UU.U10.p1, prec, LINK);
    break;

  case ne_iarg:
    sprintf(STR1, "i_arg(%ld)", WITH->UU.i);
    append(STR1, LINK);
    break;

  case ne_rarg:
    sprintf(STR1, "r_arg(%ld)", WITH->UU.i);
    append(STR1, LINK);
    break;

  case ne_sarg:
    sprintf(STR1, "s_arg(%ld)", WITH->UU.i);
    append(STR1, LINK);
    break;

  case ne_ineg:
  case ne_rneg:
    if (neg_prec < prec)
      appendc('(', LINK);
    appendc('-', LINK);
    uncompile(WITH->UU.U10.p1, (int)(neg_prec + 1), LINK);
    if (neg_prec < prec)
      appendc(')', LINK);
    break;

  case ne_not:
    if (not_prec < prec)
      appendc('(', LINK);
    appendc('~', LINK);
    uncompile(WITH->UU.U10.p1, (int)(not_prec + 1), LINK);
    if (not_prec < prec)
      appendc(')', LINK);
    break;

  case ne_lnot:
    if (lnot_prec < prec)
      appendc('(', LINK);
    appendc('!', LINK);
    uncompile(WITH->UU.U10.p1, (int)(lnot_prec + 1), LINK);
    if (lnot_prec < prec)
      appendc(')', LINK);
    break;

  case ne_round:
    func("round", &V);
    break;

  case ne_trunc:
    func("trunc", &V);
    break;

  case ne_ln:
    func("ln", &V);
    break;

  case ne_exp:
    func("exp", &V);
    break;

  case ne_iabs:
  case ne_rabs:
    func("abs", &V);
    break;

  case ne_sqrt:
    func("sqrt", &V);
    break;

  case ne_sin:
    func("sin", &V);
    break;

  case ne_cos:
    func("cos", &V);
    break;

  case ne_tan:
    func("tan", &V);
    break;

  case ne_arctan:
    func("arctan", &V);
    break;

  default:
    j = numopersyms;
    while (j >= 1 && (ne_opkind)opers[j - 1].iop != (ne_opkind)WITH->op &&
	   (ne_opkind)opers[j - 1].rop != (ne_opkind)WITH->op &&
	   (ne_opkind)opers[j - 1].sop != (ne_opkind)WITH->op)
      j--;
    if (opers[j - 1].lprec < prec)
      appendc('(', LINK);
    if (opers[j - 1].lprec == opers[j - 1].rprec)
      uncompile(WITH->UU.U10.p1, opers[j - 1].lprec, LINK);
    else
      uncompile(WITH->UU.U10.p1, opers[j - 1].lprec + 1, LINK);
    LINK->ch = opers[j - 1].name;
    nex2 = WITH->UU.U10.p2;
    switch (LINK->ch) {

    case '\001':
      if (LINK->desc->doubleeq)
	append("!=", LINK);
      else
	append("<>", LINK);
      break;

    case '\002':
      append("<=", LINK);
      break;

    case '\003':
      append(">=", LINK);
      break;

    case '\004':
      append("&&", LINK);
      break;

    case '\005':
      append("||", LINK);
      break;

    case '\006':
      append("<<", LINK);
      break;

    case '\007':
      append(">>", LINK);
      break;

    case '\b':
      append("!!", LINK);
      break;

    case '?':
      appendc(LINK->ch, LINK);
      uncompile(WITH->UU.U10.p2, 0, LINK);
      appendc(':', LINK);
      nex2 = WITH->UU.U10.p3;
      break;

    default:
      if (LINK->ch == '=' && LINK->desc->doubleeq)
	appendc(LINK->ch, LINK);
      appendc(LINK->ch, LINK);
      break;
    }
    if (opers[j - 1].lprec == opers[j - 1].rprec)
      uncompile(nex2, opers[j - 1].lprec + 1, LINK);
    else
      uncompile(nex2, opers[j - 1].lprec, LINK);
    if (opers[j - 1].lprec < prec)
      appendc(')', LINK);
    break;
  }
}



Void ne_uncompile(nex, ex_, desc_)
ne_nexrec *nex;
Char *ex_;
ne_desc *desc_;
{
  struct LOC_ne_uncompile V;

  V.ex = ex_;
  V.desc = desc_;
  V.desc->error = (unsigned)ne_noerror;
  *V.ex = '\0';
  if (nex == NULL) {
    append("(nil)", &V);
    return;
  }
  uncompile(nex, 0, &V);
  if (*V.desc->units != '\0' && (unsigned long)V.lastop < 32 &&
      ((1L << ((long)V.lastop)) &
       ((1L << ((long)ne_ic)) | (1L << ((long)ne_rc)))) != 0)
    append(V.desc->units, &V);
}


#define framesize       4   /*for na_outeralloc*/








Static Void evalargs(nex)
ne_nexrec *nex;
{
  long i;
  Char buf[256];
  long FORLIM;

/* p2c: numex.text, line 2201: Note: Call to NA_OUTERALLOC [502] */
  na_outeralloc((Anyptr *)(&ne_argarray), nex->nargs * 8L, (long)framesize);
  FORLIM = nex->nargs;
  for (i = 0; i < FORLIM; i++) {
    if (nex->UU.U15.fp->nargs == 0)
      ne_argarray[i].r = ne_reval(nex->UU.U99.pvals[i]);
    else {
      switch (nex->UU.U15.fp->UU.U99.ptypes[P_imin2(i + 1,
					      (long)nex->UU.U15.fp->nargs) - 1]) {

      case ne_integer:
      case ne_boolean:
	ne_argarray[i].U1.i1 = ne_ieval(nex->UU.U99.pvals[i]);
	break;

      case ne_real:
	ne_argarray[i].r = ne_reval(nex->UU.U99.pvals[i]);
	break;

      case ne_string:
	ne_seval(buf, nex->UU.U99.pvals[i]);
/* p2c: numex.text, line 2215: Note: Call to NA_OUTERALLOC [502] */
	na_outeralloc((Anyptr *)(&ne_argarray[i].U16.sp1), strlen(buf) + 1L,
		      (long)framesize);
	strcpy(ne_argarray[i].U16.sp1, buf);
	break;
      }
    }
  }
}

#undef framesize


boolean ne_condeval(nex)
ne_nexrec *nex;
{
  boolean Result;

  TRY(try2);
    Result = (ne_ieval(nex) != 0);
  RECOVER(try2);
    if (P_escapecode == ne_badescape)
      return false;
    else
      _Escape(P_escapecode);
  ENDTRY(try2);
  return Result;
}


long ne_ieval(nex)
ne_nexrec *nex;
{
  long Result, i1;
  na_quadword *saveargarr;
  ne_nexrec *WITH;
  Char STR1[256], STR2[256];

  WITH = nex;
  switch ((ne_opkind)nex->op) {

  case ne_ic:
    Result = WITH->UU.i;
    break;

  case ne_ip:
    Result = *WITH->UU.U10.ip;
    break;

  case ne_if:
    if (WITH->UU.U15.fp->subnex) {
      saveargarr = ne_argarray;
      TRY(try3);
	evalargs(nex);
	switch ((int)types[(*WITH->UU.U15.fp->UU.nexp)->op]) {

	case ne_integer:
	  Result = ne_ieval(*WITH->UU.U15.fp->UU.nexp);
	  break;

	case ne_real:   /*in case of $range off$*/
	  Result = (long)floor(ne_reval(*WITH->UU.U15.fp->UU.nexp) + 0.5);
	  break;

	default:
	  P_escapecode = -9;
	  goto _Ltry3;
	  break;
	}
	ne_argarray = saveargarr;
      RECOVER2(try3,_Ltry3);
	ne_argarray = saveargarr;
	_Escape(P_escapecode);
      ENDTRY(try3);
    } else {
      if (WITH->UU.U15.fp->UU.U5.ipr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, long *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.U5.ipr.proc)(nex, &i1,
					   WITH->UU.U15.fp->UU.U5.ipr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, long *res)))
	  WITH->UU.U15.fp->UU.U5.ipr.proc)(nex, &i1);
      Result = i1;
    }
    break;

  case ne_iarg:
    if (ne_argarray == NULL)
      Result = 0;
    else
      Result = ne_argarray[WITH->UU.i - 1].U1.i1;
    break;

  case ne_iadd:
    Result = ne_ieval(WITH->UU.U10.p1) + ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_isub:
    Result = ne_ieval(WITH->UU.U10.p1) - ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_imul:
    Result = ne_ieval(WITH->UU.U10.p1) * ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_idiv:
    Result = ne_ieval(WITH->UU.U10.p1) / ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_ineg:
    Result = -ne_ieval(WITH->UU.U10.p1);
    break;

  case ne_imod:
    Result = ne_ieval(WITH->UU.U10.p1) % ne_ieval(WITH->UU.U10.p2);
/* p2c: numex.text, line 2289:
 * Note: Using % for possibly-negative arguments [317] */
    break;

  case ne_rbool:
    TRY(try4);
      Result = (ne_reval(WITH->UU.U10.p1) != 0);
    RECOVER(try4);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      Result = 0;
    ENDTRY(try4);
    break;

  case ne_sbool:
    TRY(try5);
      Result = (*ne_seval(STR1, WITH->UU.U10.p1) != '\0');
    RECOVER(try5);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      Result = 0;
    ENDTRY(try5);
    break;

  case ne_ieq:
    Result = (ne_ieval(WITH->UU.U10.p1) == ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_ine:
    Result = (ne_ieval(WITH->UU.U10.p1) != ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_ilt:
    Result = (ne_ieval(WITH->UU.U10.p1) < ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_ile:
    Result = (ne_ieval(WITH->UU.U10.p1) <= ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_igt:
    Result = (ne_ieval(WITH->UU.U10.p1) > ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_ige:
    Result = (ne_ieval(WITH->UU.U10.p1) >= ne_ieval(WITH->UU.U10.p2));
    break;

  case ne_req:
    Result = (ne_reval(WITH->UU.U10.p1) == ne_reval(WITH->UU.U10.p2));
    break;

  case ne_rne:
    Result = (ne_reval(WITH->UU.U10.p1) != ne_reval(WITH->UU.U10.p2));
    break;

  case ne_rlt:
    Result = (ne_reval(WITH->UU.U10.p1) < ne_reval(WITH->UU.U10.p2));
    break;

  case ne_rle:
    Result = (ne_reval(WITH->UU.U10.p1) <= ne_reval(WITH->UU.U10.p2));
    break;

  case ne_rgt:
    Result = (ne_reval(WITH->UU.U10.p1) > ne_reval(WITH->UU.U10.p2));
    break;

  case ne_rge:
    Result = (ne_reval(WITH->UU.U10.p1) >= ne_reval(WITH->UU.U10.p2));
    break;

  case ne_seq:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) == 0);
    break;

  case ne_sne:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) != 0);
    break;

  case ne_slt:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) < 0);
    break;

  case ne_sle:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) <= 0);
    break;

  case ne_sgt:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) > 0);
    break;

  case ne_sge:
    Result = (strcmp(ne_seval(STR1, WITH->UU.U10.p1),
		     ne_seval(STR2, WITH->UU.U10.p2)) >= 0);
    break;

  case ne_not:
    Result = ~ne_ieval(WITH->UU.U10.p1);
    break;

  case ne_and:
    Result = ne_ieval(WITH->UU.U10.p1) & ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_or:
    Result = ne_ieval(WITH->UU.U10.p1) | ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_xor:
    Result = ne_ieval(WITH->UU.U10.p1) ^ ne_ieval(WITH->UU.U10.p2);
    break;

  case ne_lsh:
    Result = ne_ieval(WITH->UU.U10.p1) << ne_ieval(WITH->UU.U10.p2);
/* p2c: numex.text, line 2351:
 * Note: Assuming count for NA_ASL is positive [504] */
    break;

  case ne_rsh:
    Result = ne_ieval(WITH->UU.U10.p1) >> ne_ieval(WITH->UU.U10.p2);
/* p2c: numex.text, line 2353:
 * Note: Assuming count for NA_ASL is negative [504] */
/* p2c: numex.text, line 2353:
 * Note: Assuming >> is an arithmetic shift [505] */
    break;

  case ne_lnot:
    Result = !ne_condeval(WITH->UU.U10.p1);
    break;

  case ne_iand:
    if (ne_condeval(WITH->UU.U10.p1))
      Result = ne_ieval(WITH->UU.U10.p2);
    else
      Result = 0;
    break;

  case ne_ior:
    TRY(try6);
      i1 = ne_ieval(WITH->UU.U10.p1);
    RECOVER(try6);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      i1 = 0;
    ENDTRY(try6);
    if (i1 == 0)
      Result = ne_ieval(WITH->UU.U10.p2);
    else
      Result = i1;
    break;

  case ne_icond:
    if (ne_condeval(WITH->UU.U10.p1))
      Result = ne_ieval(WITH->UU.U10.p2);
    else
      Result = ne_ieval(WITH->UU.U10.p3);
    break;

  case ne_round:
    Result = (long)floor(ne_reval(WITH->UU.U10.p1) + 0.5);
    break;

  case ne_trunc:
    Result = (long)ne_reval(WITH->UU.U10.p1);
    break;

  case ne_iabs:
    Result = labs(ne_ieval(WITH->UU.U10.p1));
    break;

  default:
    _Escape(-9);
    break;
  }
  return Result;
}


double ne_reval(nex)
ne_nexrec *nex;
{
  double Result, r1;
  na_quadword *saveargarr;
  ne_nexrec *WITH;

  WITH = nex;
  switch ((ne_opkind)nex->op) {

  case ne_rc:
    Result = WITH->UU.r;
    break;

  case ne_rp:
    Result = *WITH->UU.U11.rp;
    break;

  case ne_rxp:
    if (*WITH->UU.U11.rp == ne_badvalue)
      _Escape(ne_badescape);
    Result = *WITH->UU.U11.rp;
    break;

  case ne_rf:
    if (WITH->UU.U15.fp->subnex) {
      saveargarr = ne_argarray;
      TRY(try7);
	evalargs(nex);
	switch ((int)types[(*WITH->UU.U15.fp->UU.nexp)->op]) {

	case ne_integer:
	  Result = ne_ieval(*WITH->UU.U15.fp->UU.nexp);
	  break;

	case ne_real:   /*in case of $range off$*/
	  Result = ne_reval(*WITH->UU.U15.fp->UU.nexp);
	  break;

	default:
	  P_escapecode = -9;
	  goto _Ltry7;
	  break;
	}
	ne_argarray = saveargarr;
      RECOVER2(try7,_Ltry7);
	ne_argarray = saveargarr;
	_Escape(P_escapecode);
      ENDTRY(try7);
    } else {
      if (WITH->UU.U15.fp->UU.rpr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, double *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.rpr.proc)(nex, &r1,
					WITH->UU.U15.fp->UU.rpr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, double *res)))
	  WITH->UU.U15.fp->UU.rpr.proc)(nex, &r1);
      Result = r1;
    }
    break;

  case ne_rarg:
    if (ne_argarray == NULL)
      Result = 0.0;
    else
      Result = ne_argarray[WITH->UU.i - 1].r;
    break;

  case ne_radd:
    Result = ne_reval(WITH->UU.U10.p1) + ne_reval(WITH->UU.U10.p2);
    break;

  case ne_rsub:
    Result = ne_reval(WITH->UU.U10.p1) - ne_reval(WITH->UU.U10.p2);
    break;

  case ne_rmul:
    Result = ne_reval(WITH->UU.U10.p1) * ne_reval(WITH->UU.U10.p2);
    break;

  case ne_rdiv:
    Result = ne_reval(WITH->UU.U10.p1) / ne_reval(WITH->UU.U10.p2);
    break;

  case ne_rneg:
    Result = -ne_reval(WITH->UU.U10.p1);
    break;

  case ne_rpow:
    Result = ma_ytox(ne_reval(WITH->UU.U10.p1), ne_reval(WITH->UU.U10.p2));
    break;

  case ne_itor:
    Result = ne_ieval(WITH->UU.U10.p1);
    break;

  case ne_ln:
    Result = log(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_exp:
    Result = exp(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_rabs:
    Result = fabs(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_sqrt:
    Result = sqrt(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_sin:
    Result = sin(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_cos:
    Result = cos(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_tan:
    Result = ma_tan(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_arctan:
    Result = atan(ne_reval(WITH->UU.U10.p1));
    break;

  case ne_rand:
    TRY(try8);
      r1 = ne_reval(WITH->UU.U10.p1);
    RECOVER(try8);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      r1 = 0.0;
    ENDTRY(try8);
    if (r1 != 0)
      Result = ne_reval(WITH->UU.U10.p2);
    else
      Result = 0.0;
    break;

  case ne_ror:
    TRY(try9);
      r1 = ne_reval(WITH->UU.U10.p1);
    RECOVER(try9);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      r1 = 0.0;
    ENDTRY(try9);
    if (r1 == 0)
      Result = ne_reval(WITH->UU.U10.p2);
    else
      Result = r1;
    break;

  case ne_rxor:
    TRY(try10);
      Result = ne_reval(WITH->UU.U10.p1);
    RECOVER(try10);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      Result = ne_reval(WITH->UU.U10.p2);
    ENDTRY(try10);
    break;

  case ne_rcond:
    if (ne_condeval(WITH->UU.U10.p1))
      Result = ne_reval(WITH->UU.U10.p2);
    else
      Result = ne_reval(WITH->UU.U10.p3);
    break;

  default:
    _Escape(-9);
    break;
  }
  return Result;
}


Char *ne_seval(Result, nex)
Char *Result;
ne_nexrec *nex;
{
  Char s1[256];
  na_quadword *saveargarr;
  ne_nexrec *WITH;
  Char STR1[256], STR2[256];

  WITH = nex;
  switch ((ne_opkind)nex->op) {

  case ne_sc:
    strcpy(Result, WITH->UU.s);
    break;

  case ne_sp:
    strcpy(Result, *WITH->UU.U12.sp);
    break;

  case ne_sf:
    if (WITH->UU.U15.fp->subnex) {
      saveargarr = ne_argarray;
      TRY(try11);
	evalargs(nex);
	ne_seval(Result, *WITH->UU.U15.fp->UU.nexp);
	ne_argarray = saveargarr;
      RECOVER(try11);
	ne_argarray = saveargarr;
	_Escape(P_escapecode);
      ENDTRY(try11);
    } else {
      if (WITH->UU.U15.fp->UU.spr.link != NULL)
	(*(Void(*) PP((ne_nexrec *nex, Char *res, Anyptr _link)))
	  WITH->UU.U15.fp->UU.spr.proc)(nex, s1, WITH->UU.U15.fp->UU.spr.link);
      else
	(*(Void(*) PP((ne_nexrec *nex, Char *res)))
	  WITH->UU.U15.fp->UU.spr.proc)(nex, s1);
      strcpy(Result, s1);
    }
    break;

  case ne_sarg:
    if (ne_argarray == NULL)
      strcpy(Result, "");
    else
      strcpy(Result, ne_argarray[WITH->UU.i - 1].U16.sp1);
    break;

  case ne_sadd:
    sprintf(Result, "%s%s",
	    ne_seval(STR1, WITH->UU.U10.p1), ne_seval(STR2, WITH->UU.U10.p2));
    break;

  case ne_sand:
    TRY(try12);
      ne_seval(s1, WITH->UU.U10.p1);
    RECOVER(try12);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      *s1 = '\0';
    ENDTRY(try12);
    if (*s1 != '\0')
      ne_seval(Result, WITH->UU.U10.p2);
    else
      strcpy(Result, "");
    break;

  case ne_sor:
    TRY(try13);
      ne_seval(s1, WITH->UU.U10.p1);
    RECOVER(try13);
      if (P_escapecode != ne_badescape)
	_Escape(P_escapecode);
      *s1 = '\0';
    ENDTRY(try13);
    if (*s1 == '\0')
      ne_seval(Result, WITH->UU.U10.p2);
    else
      strcpy(Result, s1);
    break;

  case ne_scond:
    if (ne_condeval(WITH->UU.U10.p1))
      ne_seval(Result, WITH->UU.U10.p2);
    else
      ne_seval(Result, WITH->UU.U10.p3);
    break;

  default:
    _Escape(-9);
    break;
  }
  return Result;
}




ne_datatype ne_exprtype(nex)
ne_nexrec *nex;
{
  if (nex == NULL)
    return ne_notype;
  else
    return (types[nex->op]);
}




Void ne_evaluate(nex, res)
ne_nexrec *nex, *res;
{
  Char STR1[256];

  TRY(try14);
    switch ((int)types[nex->op]) {

    case ne_notype:
      *res = *nex;
      break;

    case ne_integer:
      res->op = (unsigned)ne_ic;
      res->UU.i = ne_ieval(nex);
      break;

    case ne_real:
      res->op = (unsigned)ne_rc;
      res->UU.r = ne_reval(nex);
      break;

    case ne_string:
      res->op = (unsigned)ne_sc;
      res->UU.s = (char *)strdup(ne_seval(STR1, nex));
      break;
    }
  RECOVER(try14);
    if (P_escapecode == -20)
      _Escape(P_escapecode);
    res->op = (unsigned)ne_error;
    switch (P_escapecode) {

    case -3:
      res->UU.err = (unsigned)ne_undef;
      break;

    case -5:
      res->UU.err = (unsigned)ne_divzero;
      break;

    case -7:
      res->UU.err = (unsigned)ne_underflow;
      break;

    case ne_badescape:
      res->UU.err = (unsigned)ne_badval;
      break;

    default:
      res->UU.err = (unsigned)ne_overflow;
      break;
    }
  ENDTRY(try14);
}




Static Void faileval(nex, desc, esc)
ne_nexrec *nex;
ne_desc *desc;
long esc;
{
  desc->error = (unsigned)ne_noerror;
  if (nex == NULL || (ne_opkind)nex->op != ne_error)
    _Escape((int)esc);
  desc->error = (unsigned)((ne_errorkind)nex->UU.err);
  switch ((ne_errorkind)nex->UU.err) {

  case ne_underflow:
    _Escape(-7);
    break;

  case ne_divzero:
    _Escape(-5);
    break;

  case ne_strlong:
    _Escape(-8);
    break;

  default:
    _Escape((int)esc);
    break;
  }
}



long ne_ievaluate(nex, desc)
ne_nexrec *nex;
ne_desc *desc;
{
  if (integerp(nex))
    return (ne_ieval(nex));
  if (realp(nex))
    return ((long)floor(ne_reval(nex) + 0.5));
  faileval(nex, desc, -4L);
}


double ne_revaluate(nex, desc)
ne_nexrec *nex;
ne_desc *desc;
{
  if (realp(nex))
    return (ne_reval(nex));
  if (integerp(nex))
    return (ne_ieval(nex));
  faileval(nex, desc, -6L);
}


Char *ne_sevaluate(Result, nex, desc)
Char *Result;
ne_nexrec *nex;
ne_desc *desc;
{
  if (stringp(nex))
    return (ne_seval(Result, nex));
  faileval(nex, desc, -8L);
  return Result;
}




long ne_ievalexpr(ex, desc)
Char *ex;
ne_desc *desc;
{
  long Result;
  ne_nexrec *nex;
  long esc;

  ne_compile(ex, &nex, desc);
  TRY(try15);
    Result = ne_ievaluate(nex, desc);
    ne_dispose(&nex);
  RECOVER(try15);
    esc = P_escapecode;
    ne_dispose(&nex);
    _Escape((int)esc);
  ENDTRY(try15);
  return Result;
}


double ne_revalexpr(ex, desc)
Char *ex;
ne_desc *desc;
{
  double Result;
  ne_nexrec *nex;
  long esc;

  ne_compile(ex, &nex, desc);
  TRY(try16);
    Result = ne_revaluate(nex, desc);
    ne_dispose(&nex);
  RECOVER(try16);
    esc = P_escapecode;
    ne_dispose(&nex);
    _Escape((int)esc);
  ENDTRY(try16);
  return Result;
}


Char *ne_sevalexpr(Result, ex, desc)
Char *Result, *ex;
ne_desc *desc;
{
  ne_nexrec *nex;
  long esc;

  ne_compile(ex, &nex, desc);
  TRY(try17);
    ne_sevaluate(Result, nex, desc);
    ne_dispose(&nex);
  RECOVER(try17);
    esc = P_escapecode;
    ne_dispose(&nex);
    _Escape((int)esc);
  ENDTRY(try17);
  return Result;
}





Void ne_makeintconst(sym, i)
na_strlist *sym;
long i;
{
  sym->kind = ne_kind_intconst;
  sym->value = (na_long)i;
}


Void ne_makesrealconst(sym, r)
na_strlist *sym;
double r;
{
  sym->kind = ne_kind_srealconst;
  sym->value = na_rtosr(r);
}


Void ne_makerealconst(sym, r)
na_strlist *sym;
double r;
{
  double *rp;

  sym->kind = ne_kind_realconst;
  rp = (double *)Malloc(sizeof(double));
  *rp = r;
  sym->value = (Anyptr)rp;
}


Void ne_makestrconst(sym, s)
na_strlist *sym;
Char *s;
{
  sym->kind = ne_kind_strconst;
  *(Char **)((Char **)(&sym->value)) = (char *)strdup(s);
}


Void ne_makeintvar(sym, i)
na_strlist *sym;
long *i;
{
  sym->kind = ne_kind_intptr;
  sym->value = (Anyptr)i;
}


Void ne_makerealvar(sym, r)
na_strlist *sym;
double *r;
{
  sym->kind = ne_kind_realptr;
  sym->value = (Anyptr)r;
}


Void ne_makerealxvar(sym, r)
na_strlist *sym;
double *r;
{
  sym->kind = ne_kind_realxptr;
  sym->value = (Anyptr)r;
}


Void ne_makestrvar(sym, s)
na_strlist *sym;
Char *s;
{
  sym->kind = ne_kind_strvalue;
  sym->value = (Anyptr)s;
}


Void ne_makestrptrvar(sym, sp)
na_strlist *sym;
Char **sp;
{
  sym->kind = ne_kind_strptr;
  sym->value = (Anyptr)sp;
}



Static Void setupfunc(sym, fp, nargs, args)
na_strlist *sym;
ne_functionrec **fp;
long nargs;
ne_datatype *args;
{
  long i;

/* p2c: numex.text, line 2837:
 * Note: No SpecialSizeOf form known for NE_FUNCTIONREC.1 [187] */
/* p2c: numex.text, line 2837:
 * Note: Suspicious mixture of sizes in NA_NEW [173] */
  *fp = (ne_functionrec *)Malloc(sizeof(ne_functionrec) + nargs * 2);
  (*fp)->nargs = nargs;
  for (i = 0; i < nargs; i++)
    (*fp)->UU.U99.ptypes[i] = args[i];
  (*fp)->static_ = false;
  (*fp)->minargs = nargs;
  (*fp)->maxargs = nargs;
  sym->value = (Anyptr)(*fp);
}


Void ne_makeintmfunc(sym, nargs, args, pr)
na_strlist *sym;
long nargs;
ne_datatype *args;
_PROCEDURE pr;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_intfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = false;
  fp->UU.U5.ipr = pr;
}


Void ne_makerealmfunc(sym, nargs, args, pr)
na_strlist *sym;
long nargs;
ne_datatype *args;
_PROCEDURE pr;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_realfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = false;
  fp->UU.rpr = pr;
}


Void ne_makestrmfunc(sym, nargs, args, pr)
na_strlist *sym;
long nargs;
ne_datatype *args;
_PROCEDURE pr;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_strfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = false;
  fp->UU.spr = pr;
}


Void ne_makeintfunc(sym, t1, t2, t3, pr)
na_strlist *sym;
ne_datatype t1, t2, t3;
_PROCEDURE pr;
{
  ne_datatype args[3];

  args[0] = t1;
  args[1] = t2;
  args[2] = t3;
  ne_makeintmfunc(sym,
    (long)((t1 != ne_notype) + (t2 != ne_notype) + (t3 != ne_notype)), args,
    pr);
}


Void ne_makerealfunc(sym, t1, t2, t3, pr)
na_strlist *sym;
ne_datatype t1, t2, t3;
_PROCEDURE pr;
{
  ne_datatype args[3];

  args[0] = t1;
  args[1] = t2;
  args[2] = t3;
  ne_makerealmfunc(sym,
    (long)((t1 != ne_notype) + (t2 != ne_notype) + (t3 != ne_notype)), args,
    pr);
}


Void ne_makestrfunc(sym, t1, t2, t3, pr)
na_strlist *sym;
ne_datatype t1, t2, t3;
_PROCEDURE pr;
{
  ne_datatype args[3];

  args[0] = t1;
  args[1] = t2;
  args[2] = t3;
  ne_makestrmfunc(sym,
    (long)((t1 != ne_notype) + (t2 != ne_notype) + (t3 != ne_notype)), args,
    pr);
}


Void ne_makeintsfunc(sym, nargs, args, nex)
na_strlist *sym;
long nargs;
ne_datatype *args;
ne_nexrec **nex;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_intfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = true;
  fp->UU.nexp = nex;
}


Void ne_makerealsfunc(sym, nargs, args, nex)
na_strlist *sym;
long nargs;
ne_datatype *args;
ne_nexrec **nex;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_realfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = true;
  fp->UU.nexp = nex;
}


Void ne_makestrsfunc(sym, nargs, args, nex)
na_strlist *sym;
long nargs;
ne_datatype *args;
ne_nexrec **nex;
{
  ne_functionrec *fp;

  sym->kind = ne_kind_strfunc;
  setupfunc(sym, &fp, nargs, args);
  fp->subnex = true;
  fp->UU.nexp = nex;
}


Void ne_makeintarg(sym, i)
na_strlist *sym;
long i;
{
  sym->kind = ne_kind_intarg;
  sym->value = (na_long)i;
}


Void ne_makerealarg(sym, i)
na_strlist *sym;
long i;
{
  sym->kind = ne_kind_realarg;
  sym->value = (na_long)i;
}


Void ne_makestrarg(sym, i)
na_strlist *sym;
long i;
{
  sym->kind = ne_kind_strarg;
  sym->value = (na_long)i;
}



Void ne_setminargs(sym, minargs)
na_strlist *sym;
long minargs;
{
  ne_functionrec *fp;

  fp = (ne_functionrec *)sym->value;
  fp->minargs = P_imin2((long)fp->nargs, minargs);
}


Void ne_setmaxargs(sym, maxargs)
na_strlist *sym;
long maxargs;
{
  ne_functionrec *fp;

  fp = (ne_functionrec *)sym->value;
  fp->maxargs = P_imin2(P_imax2((long)fp->nargs, maxargs), (long)ne_maxargs);
}


Void ne_makestaticfunc(sym)
na_strlist *sym;
{
  ne_functionrec *fp;

  fp = (ne_functionrec *)sym->value;
  fp->static_ = true;
}



Void ne_disposesym(sym)
na_strlist *sym;
{
  double *rp;
  ne_functionrec *fp;

  switch (sym->kind) {

  case ne_kind_realconst:
    rp = (double *)sym->value;
    Free(rp);
    break;

  case ne_kind_intfunc:
  case ne_kind_realfunc:
  case ne_kind_strfunc:
    fp = (ne_functionrec *)sym->value;
/* p2c: numex.text, line 3026:
 * Note: No SpecialSizeOf form known for NE_FUNCTIONREC.1 [187] */
    Free(fp);
    break;
  }
  sym->kind = '\0';
  sym->value = (na_long)0;
}


Void ne_disposesymtab(symtab)
na_strlist **symtab;
{
  na_strlist *l1;

  l1 = *symtab;
  while (l1 != NULL) {
    ne_disposesym(l1);
    l1 = l1->next;
  }
  strlist_empty(symtab);
}











/* End. */
