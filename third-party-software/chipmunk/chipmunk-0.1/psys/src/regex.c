/* Output from p2c, the Pascal-to-C translator */
/* From input file "regex.text" */


/* Regular expression search & replace */



#include <p2c/p2c.h>


#define REGEX_G
#include <p2c/regex.h>





#define checking        false

#define quote           1
#define bunch           2
#define close_bunch     3
#define invert          4
#define through         5
#define kleene          6
#define occur           7
#define some            8
#define eol             9
#define all             10
#define alternative     11
#define qnot            12
#define left_paren      13
#define right_paren     14
#define count           15
#define close_count     16
#define continue_       17
#define star            18
#define begword         19
#define endword         20
#define charsmax        20

#define end_            "\022"
#define quote_          "\001"
#define letter_         "\002"
#define bunch_          "\003"
#define all_            "\004"
#define eol_            "\005"
#define kleene_         "\006"
#define occur_          "\007"
#define some_           "\b"
#define count_          "\t"
#define not_            "\n"
#define alternative_    "\013"
#define lparen_         "\f"
#define rparen_         "\015"
#define nparen_         "\016"
#define begword_        "\017"
#define endword_        "\020"
#define ci_             "\021"


/* Local variables for compile: */
struct LOC_compile {
  Char *ex, *rex;
  Char chars[256];
  boolean cimode;
  long i, parcnt;
} ;

Local Void expr PP((int parenflag, struct LOC_compile *link));

Local Char *parsebunch(rval, link)
Char *rval;
struct LOC_compile *link;
{
  uchar a[32];
  long j;
  Char ch;
  boolean inverted, flag;
  Char s[256];
  long FORLIM;
  int TEMP;

  inverted = (link->ex[link->i - 1] == link->chars[invert - 1] &&
	      link->ex[link->i - 1] != ' ');
  if (inverted)
    link->i++;
  for (j = 0; j <= 255; j++) {
    P_clrbits_B(a, j, 0, 3);
    P_putbits_UB(a, j, inverted, 0, 3);
  }
  while (link->i <= strlen(link->ex) &&
	 link->ex[link->i - 1] != link->chars[close_bunch - 1]) {
    ch = link->ex[link->i - 1];
    link->i++;
    if (link->ex[link->i - 1] == link->chars[through - 1] &&
	link->ex[link->i - 1] != ' ' && link->ex[link->i] >= ch) {
      FORLIM = link->ex[link->i];
      for (j = ch; j <= FORLIM; j++) {
	P_clrbits_B(a, j, 0, 3);
	P_putbits_UB(a, j, (int)(!inverted), 0, 3);
      }
      link->i += 2;
    } else {
      P_clrbits_B(a, ch, 0, 3);
      P_putbits_UB(a, ch, (int)(!inverted), 0, 3);
    }
  }
  if (link->cimode) {
    for (j = 'a'; j <= 'z'; j++) {
      TEMP = (P_getbits_UB(a, j - 32, 0, 3) || P_getbits_UB(a, j, 0, 3));
      P_clrbits_B(a, j - 32, 0, 3);
      P_putbits_UB(a, j - 32, TEMP, 0, 3);
      P_clrbits_B(a, j, 0, 3);
    }
  }
  if (link->i <= strlen(link->ex))
    link->i++;
  flag = false;
  *s = '\0';
  for (j = 0; j <= 255; j++) {
    if (P_getbits_UB(a, j, 0, 3) != flag) {
      if (flag)
	sprintf(s + strlen(s), "%c", (Char)(j - 1));
      else
	sprintf(s + strlen(s), "%c", (Char)j);
      flag = P_getbits_UB(a, j, 0, 3);
    }
  }
  if (flag)
    strcat(s, "\377");
  sprintf(rval, "%c%s", (Char)(strlen(s) / 2), s);
/* p2c: regex.text, line 140: Note: Character >= 128 encountered [281] */
  return rval;
}

Local Char *parsecount(rval, link)
Char *rval;
struct LOC_compile *link;
{
  long min, max;
  Char STR1[2];
  Char *STR2;

  min = 0;
  max = 255;
  sprintf(STR1, "%c", link->chars[close_count - 1]);
  if (strpos2(link->ex, STR1, link->i) > 0) {
    if (isdigit(link->ex[link->i - 1])) {
      min = strtol(link->ex + link->i - 1, &STR2, 10);
      link->i = STR2 - link->ex + 1;
    }
    if (link->ex[link->i - 1] == link->chars[continue_ - 1] &&
	link->chars[continue_ - 1] != ' ') {
      link->i++;
      if (isdigit(link->ex[link->i - 1])) {
	max = strtol(link->ex + link->i - 1, &STR2, 10);
	link->i = STR2 - link->ex + 1;
      }
    } else
      max = min;
    if (link->ex[link->i - 1] == link->chars[close_count - 1])
      link->i++;
  }
  if (max > 255)
    max = 255;
  if (min > max)
    min = max;
  sprintf(rval, "%s%c%c", count_, (Char)min, (Char)max);
  return rval;
}

/* Local variables for expr: */
struct LOC_expr {
  struct LOC_compile *link;
} ;

/* Local variables for term: */
struct LOC_term {
  struct LOC_expr *link;
} ;

Local Void factor(parenflag, link)
boolean parenflag;
struct LOC_term *link;
{
  long save;
  Char ch;
  Char STR1[256];

  if (link->link->link->i > strlen(link->link->link->ex))
    return;
  ch = link->link->link->ex[link->link->link->i - 1];
  link->link->link->i++;
  if (ch == ' ') {
    sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c", ch);
    return;
  }
  if (ch == link->link->link->chars[quote - 1] && ch != ' ' &&
      link->link->link->i <= strlen(link->link->link->ex)) {
    if (isalpha(link->link->link->ex[link->link->link->i - 1])) {
      strcat(link->link->link->rex, "\002");
      sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c",
	      toupper(link->link->link->ex[link->link->link->i - 1]));
      link->link->link->i++;
      return;
    }
    if (isdigit(link->link->link->ex[link->link->link->i - 1])) {
      strcat(link->link->link->rex, "\016");
      if (link->link->link->ex[link->link->link->i - 1] == '0')
	strcat(link->link->link->rex, "\n");
      else
	sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c",
		link->link->link->ex[link->link->link->i - 1] - 48);
      link->link->link->i++;
      return;
    }
    if (link->link->link->ex[link->link->link->i - 1] < ' ')
      strcat(link->link->link->rex, "\001");
    sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c",
	    link->link->link->ex[link->link->link->i - 1]);
    link->link->link->i++;
    return;
  }
  if (ch == link->link->link->chars[bunch - 1] &&
      link->link->link->i <= strlen(link->link->link->ex) &&
      link->link->link->chars[close_bunch - 1] != ' ') {
    strcat(link->link->link->rex, "\003");
    strcat(link->link->link->rex, parsebunch(STR1, link->link->link));
    return;
  }
  if (ch == link->link->link->chars[all - 1]) {
    strcat(link->link->link->rex, "\004");
    return;
  }
  if (ch == link->link->link->chars[eol - 1]) {
    strcat(link->link->link->rex, "\005");
    return;
  }
  if (ch == link->link->link->chars[begword - 1]) {
    strcat(link->link->link->rex, "\017");
    return;
  }
  if (ch == link->link->link->chars[endword - 1]) {
    strcat(link->link->link->rex, "\020");
    return;
  }
  if (ch == link->link->link->chars[left_paren - 1] &&
      link->link->link->chars[right_paren - 1] != ' ') {
    link->link->link->parcnt++;
    if (link->link->link->parcnt > 10)
      expr(true, link->link->link);
    else {
      save = link->link->link->parcnt;
      strcat(link->link->link->rex, "\f");
      sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c",
	      (Char)save);
      expr(true, link->link->link);
      strcat(link->link->link->rex, "\015");
      sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c",
	      (Char)save);
    }
    if (link->link->link->i <= strlen(link->link->link->ex) &&
	link->link->link->ex[link->link->link->i - 1] == link->link->link->
	  chars[right_paren - 1])
      link->link->link->i++;
    return;
  }
  if (ch == link->link->link->chars[kleene - 1] ||
      ch == link->link->link->chars[star - 1]) {
    sprintf(link->link->link->rex + strlen(link->link->link->rex), "%s%s%s",
	    kleene_, all_, end_);
    return;
  }
  if (ch < ' ')
    strcat(link->link->link->rex, "\001");
  sprintf(link->link->link->rex + strlen(link->link->link->rex), "%c", ch);

  /*end of line*/
}  /*factor*/

Local Void term(parenflag, link)
boolean parenflag;
struct LOC_expr *link;
{
  struct LOC_term V;
  long save;
  Char ch;
  boolean notflag;
  Char STR1[256];
  Char STR2[256];

  V.link = link;
  while (link->link->i <= strlen(link->link->ex) &&
	 (link->link->ex[link->link->i - 1] != link->link->chars[alternative - 1] ||
	  link->link->ex[link->link->i - 1] == ' ') &&
	 (link->link->ex[link->link->i - 1] != link->link->chars[right_paren - 1] ||
	  !parenflag)) {
/* p2c: regex.text, line 344: 
 * Note: Line breaker spent 1.0 seconds, 5000 tries on line 299 [251] */
    notflag = false;
    while (link->link->i <= strlen(link->link->ex) &&
	   link->link->ex[link->link->i - 1] == link->link->chars[qnot - 1] &&
	   link->link->ex[link->link->i - 1] != ' ') {
      notflag = !notflag;
      link->link->i++;
    }
    if (link->link->i > strlen(link->link->ex)) {
      notflag = !notflag;
      link->link->i--;
    }
    if (notflag)
      strcat(link->link->rex, "\n");
    save = strlen(link->link->rex) + 1;
    factor(parenflag, &V);
    while (link->link->i <= strlen(link->link->ex) &&
	   link->link->ex[link->link->i - 1] != ' ' &&
	   (link->link->ex[link->link->i - 1] == link->link->chars[kleene - 1] ||
	    link->link->ex[link->link->i - 1] == link->link->chars[occur - 1] ||
	    link->link->ex[link->link->i - 1] == link->link->chars[some - 1] ||
	    link->link->ex[link->link->i - 1] == link->link->chars[count - 1])) {
/* p2c: regex.text, line 344: 
 * Note: Line breaker spent 3.0 seconds, 5000 tries on line 322 [251] */
      ch = link->link->ex[link->link->i - 1];
      link->link->i++;
      if (ch == link->link->chars[kleene - 1]) {
	sprintf(STR1, "%s%s", kleene_, link->link->rex + save - 1);
	strcpy(link->link->rex + save - 1, STR1);
      } else if (ch == link->link->chars[occur - 1]) {
	sprintf(STR1, "%s%s", occur_, link->link->rex + save - 1);
	strcpy(link->link->rex + save - 1, STR1);
      } else if (ch == link->link->chars[some - 1]) {
	sprintf(STR1, "%s%s", some_, link->link->rex + save - 1);
	strcpy(link->link->rex + save - 1, STR1);
      } else {
	sprintf(STR1, "%s%s",
		parsecount(STR2, link->link), link->link->rex + save - 1);
	strcpy(link->link->rex + save - 1, STR1);
      }
      strcat(link->link->rex, "\022");
    }
    if (notflag)
      strcat(link->link->rex, "\022");
  }

  /*count*/
}  /*term*/

Local Void expr(parenflag, link)
boolean parenflag;
struct LOC_compile *link;
{
  struct LOC_expr V;
  long save;
  Char STR1[256];

  V.link = link;
  save = strlen(link->rex) + 1;
  term(parenflag, &V);
  if (link->i > strlen(link->ex) ||
      link->ex[link->i - 1] != link->chars[alternative - 1] ||
      link->ex[link->i - 1] == ' ')
    return;
  sprintf(STR1, "%s%s", alternative_, link->rex + save - 1);
  strcpy(link->rex + save - 1, STR1);
  strcat(link->rex, "\022");
  while (link->i <= strlen(link->ex) &&
	 link->ex[link->i - 1] == link->chars[alternative - 1]) {
    link->i++;
    strcat(link->rex, "\013");
    term(parenflag, &V);
    strcat(link->rex, "\022");
  }
}  /*expr*/




Static Void compile(ex_, rex_, chars_, cimode_)
Char *ex_, *rex_;
Char *chars_;
boolean cimode_;
{
  struct LOC_compile V;

  V.ex = ex_;
  V.rex = rex_;
  strcpy(V.chars, chars_);
  V.cimode = cimode_;
  if (*V.chars == '\0')
    strcpy(V.chars, re_defchars);
  while (strlen(V.chars) < charsmax)
    strcat(V.chars, " ");
  V.parcnt = 0;
  if (V.cimode)
    strcpy(V.rex, ci_);
  else
    *V.rex = '\0';
  V.i = 1;
  expr(false, &V);
  strcat(V.rex, end_);
}



Void re_compile(ex_, rex, chars)
Char *ex_;
Char *rex;
Char *chars;
{
  Char ex[256];

  strcpy(ex, ex_);
  compile(ex, rex, chars, false);
}



Void re_cicompile(ex_, rex, chars)
Char *ex_;
Char *rex;
Char *chars;
{
  Char ex[256];

  strcpy(ex, ex_);
  compile(ex, rex, chars, true);
}


Local Char *charconv(rval, ch)
Char *rval;
Char ch;
{
  if (ch > ' ' && (ch & (~127)) == 0) {
    sprintf(rval, "%c", ch);
/* p2c: regex.text, line 370: Note: Character >= 128 encountered [281] */
    return rval;
  } else if (ch == ' ')
    return strcpy(rval, "sp");
  else {
    sprintf(rval, "#%d", ch);
    return rval;
  }
}





Void re_dump(rex)
Char *rex;
{
  long i, n;
  Char STR2[256], STR3[256];

  if (*rex == '\0')
    printf("<null>");
  i = 1;
  while (i <= strlen(rex)) {
    if (i != 1)
      putchar(' ');
    if (rex[i - 1] > ' ')
      putchar(rex[i - 1]);
    else if (rex[i - 1] == ' ')
      printf("sp");
    else {
      putchar('<');
      switch (rex[i - 1]) {

      case '\022':
	printf("end");
	break;

      case '\001':
	i++;
	printf("quote %s", charconv(STR2, rex[i - 1]));
	break;

      case '\002':
	printf("letter");
	break;

      case '\003':
	printf("bunch");
	i++;
	n = rex[i - 1];
	while (n > 0) {
	  printf(" %s-%s", charconv(STR2, rex[i]), charconv(STR3, rex[i + 1]));
	  i += 2;
	  n--;
	}
	break;

      case '\004':
	printf("all");
	break;

      case '\005':
	printf("eol");
	break;

      case '\017':
	printf("begword");
	break;

      case '\020':
	printf("endword");
	break;

      case '\006':
	printf("kleene");
	break;

      case '\007':
	printf("occur");
	break;

      case '\b':
	printf("some");
	break;

      case '\t':
	printf("count %d:%d", rex[i], rex[i + 1]);
	i += 2;
	break;

      case '\n':
	printf("not");
	break;

      case '\013':
	printf("alt");
	break;

      case '\f':
      case '\015':
      case '\016':
	switch (rex[i - 1]) {

	case '\f':
	  printf("lparen ");
	  break;

	case '\015':
	  printf("rparen ");
	  break;

	case '\016':
	  printf("nparen ");
	  break;
	}
	printf("%d", rex[i]);
	i++;
	break;

      case '\021':
	printf("ci");
	break;
      }
      putchar('>');
    }
    i++;
  }
}
/* p2c: regex.text, line 731: Note: Range checking is ON [216] */
/* p2c: regex.text, line 732: Note: Overflow checking is ON [219] */


/* Local variables for compare: */
struct LOC_compare {
  Char *s, *rex, *rpl;
  long *p, i, savep, countleft, countright;
  long par[2][10];
  boolean parflag, cimode;
  Char matchchar;
} ;

Local boolean comp PP((struct LOC_compare *link));

/* p2c: regex.text, line 462: Note: Range checking is OFF [216] */
/* p2c: regex.text, line 463: Note: Overflow checking is OFF [219] */

Local Void skiprex(rex, link)
Char *rex;
struct LOC_compare *link;
{
  Char STR1[256];

  while (strcmp((sprintf(STR1, "%c", rex[link->i - 1]), STR1), end_)) {
    switch (rex[link->i - 1]) {

    case '\006':
    case '\007':
    case '\b':
    case '\n':
    case '\013':
      link->i++;
      skiprex(rex, link);
      break;

    case '\t':
      link->i += 3;
      skiprex(rex, link);
      break;

    case '\001':
    case '\f':
    case '\015':
    case '\016':
      link->i += 2;
      break;

    case '\003':
      link->i += rex[link->i] * 2 + 2;
      break;

    default:
      link->i++;
      break;
    }
  }
  link->i++;
}

/* Local variables for comp: */
struct LOC_comp {
  struct LOC_compare *link;
  boolean wrong;
  long tempright;
} ;

/* Local variables for count__: */
struct LOC_count__ {
  struct LOC_comp *link;
  long min, max, i1;
  boolean flag;
} ;

Local boolean subcount(num, link)
long num;
struct LOC_count__ *link;
{
  long savep, savei;

  savep = *link->link->link->p;
  savei = link->link->link->i;
  link->link->link->i = link->i1;
  if (num < link->max && comp(link->link->link)) {
    link->link->link->i++;
    if (subcount(num + 1, link)) {
      link->link->tempright = *link->link->link->p;
      link->flag = (num + 1 < link->min || !comp(link->link->link));
    } else
      link->flag = false;
  } else
    link->flag = true;
  if (link->flag) {
    *link->link->link->p = savep;
    link->link->link->i = savei;
  }
  return link->flag;
}  /*subcount*/

Local Void count__(min_, max_, link)
long min_, max_;
struct LOC_comp *link;
{
  struct LOC_count__ V;

  V.link = link;
  V.min = min_;
  V.max = max_;
  link->link->countleft = *link->link->p;
  link->tempright = -1;
  V.i1 = link->link->i;
  link->wrong = subcount(0L, &V);
  if (link->wrong && V.min == 0) {
    skiprex(link->link->rex, link->link);
    link->tempright = *link->link->p;
    link->wrong = !comp(link->link);
  }
  if (!link->wrong)
    link->link->countright = link->tempright;
}  /*count*/

Local boolean comp(link)
struct LOC_compare *link;
{
  struct LOC_comp V;
  boolean flag;
  Char ch, ch1;
  long j, n, savei, savep;
  Char STR1[256], STR2[256];
  Char STR3[256], STR4[256];

  V.link = link;
  /*      write('Comp "', strsub(s,p,999), '" against "'); re_dump(strsub(rex,i,999)); writeln('"'); {****/
  V.wrong = false;
  savei = link->i;
  savep = *link->p;
  while (strcmp((sprintf(STR1, "%c", link->rex[link->i - 1]), STR1), end_) &&
	 !V.wrong) {
    ch = link->rex[link->i - 1];
    link->i++;
    if (ch >= ' ') {
      if (*link->p > strlen(link->s)) {
	V.wrong = true;
	break;
      }
      if (link->cimode)
	V.wrong = (toupper(link->s[*link->p - 1]) != toupper(ch));
      else
	V.wrong = (link->s[*link->p - 1] != ch);
      (*link->p)++;
      continue;
    }
    switch (ch) {

    case '\001':
      ch = link->rex[link->i - 1];
      link->i++;
      if (*link->p > strlen(link->s))
	V.wrong = true;
      else {
	V.wrong = (link->s[*link->p - 1] != ch);
	(*link->p)++;
      }
      break;

    case '\002':
      ch = link->rex[link->i - 1];
      link->i++;
      if (*link->p > strlen(link->s))
	V.wrong = true;
      else {
	V.wrong = (toupper(link->s[*link->p - 1]) != ch);
	(*link->p)++;
      }
      break;

    case '\003':
      n = link->rex[link->i - 1];
      link->i++;
      if (*link->p > strlen(link->s))
	V.wrong = true;
      else {
	if (link->cimode)
	  ch1 = toupper(link->s[*link->p - 1]);
	else
	  ch1 = link->s[*link->p - 1];
	while (n > 0 && ch1 > link->rex[link->i]) {
	  link->i += 2;
	  n--;
	}
	if (n == 0 || ch1 < link->rex[link->i - 1])
	  V.wrong = true;
	else
	  (*link->p)++;
      }
      link->i += n + n;
      break;

    case '\004':
      if (*link->p > strlen(link->s))
	V.wrong = true;
      else {
	link->matchchar = link->s[*link->p - 1];
	(*link->p)++;
      }
      break;

    case '\005':
      if (*link->p != 1 && *link->p != strlen(link->s) + 1)
	V.wrong = true;
      break;

    case '\017':
      if ((*link->p != 1 &&
	   (link->s[*link->p - 2] == '_' || isalnum(link->s[*link->p - 2]))) ||
	  (*link->p <= strlen(link->s) &&
	   !(link->s[*link->p - 1] == '_' || isalnum(link->s[*link->p - 1]))))
	V.wrong = true;
      break;

    case '\020':
      if (*link->p <= strlen(link->s) &&
	  (link->s[*link->p - 1] == '_' || isalnum(link->s[*link->p - 1])))
	V.wrong = true;
      break;

    case '\006':
      count__(0L, 255L, &V);
      break;

    case '\007':
      count__(0L, 1L, &V);
      break;

    case '\b':
      count__(1L, 255L, &V);
      break;

    case '\t':
      link->i += 2;
      count__((long)link->rex[link->i - 3], (long)link->rex[link->i - 2], &V);
      break;

    case '\n':
      V.wrong = comp(link);
      if (!V.wrong)
	skiprex(link->rex, link);
      break;

    case '\013':
      flag = false;
      link->i--;
      while (!strcmp((sprintf(STR2, "%c", link->rex[link->i - 1]), STR2),
		     alternative_)) {
	link->i++;
	if (!flag)
	  flag = comp(link);
	skiprex(link->rex, link);
      }
      V.wrong = !flag;
      break;

    case '\f':
      if (!link->parflag) {
	for (j = 0; j <= 9; j++) {
	  link->par[0][j] = -1;
	  link->par[1][j] = -1;
	}
	link->parflag = true;
      }
      n = link->rex[link->i - 1];
      link->i++;
      link->par[0][n - 1] = *link->p;
      break;

    case '\015':
      n = link->rex[link->i - 1];
      link->i++;
      link->par[1][n - 1] = *link->p;
      break;

    case '\016':
      n = link->rex[link->i - 1];
      link->i++;
      if (link->parflag && link->par[1][n - 1] != -1) {
	j = link->par[1][n - 1] - link->par[0][n - 1];
	if (j + *link->p - 1 <= strlen(link->s)) {
	  sprintf(STR3, "%.*s", (int)j, link->s + *link->p - 1);
	  sprintf(STR4, "%.*s", (int)j, link->s + link->par[0][n - 1] - 1);
	  V.wrong = (strcmp(STR3, STR4) != 0);
	  *link->p += j;
	} else
	  V.wrong = true;
      } else
	V.wrong = true;
      break;

    case '\021':
      link->cimode = true;
      break;
    }
  }
  if (V.wrong) {
    link->i = savei;
    *link->p = savep;
  }
  return (!V.wrong);
}  /*comp*/

/* Local variables for replace: */
struct LOC_replace {
  struct LOC_compare *link;
  Char temps[256];
  long notnum;
} ;

Local Void getrepl(link)
struct LOC_replace *link;
{
  Char ch;
  long j, n;
  Char STR1[256], STR2[256], STR3[256];
  Char FORLIM;

  while (strcmp((sprintf(STR1, "%c", link->link->rpl[link->link->i - 1]),
		 STR1), end_)) {
    ch = link->link->rpl[link->link->i - 1];
    link->link->i++;
    if (ch >= ' ') {
      sprintf(link->temps + strlen(link->temps), "%c", ch);
      continue;
    }
    switch (ch) {

    case '\001':
    case '\002':
      sprintf(link->temps + strlen(link->temps), "%c",
	      link->link->rpl[link->link->i - 1]);
      link->link->i++;
      break;

    case '\003':
      n = link->link->rpl[link->link->i - 1];
      link->link->i++;
      while (n > 0) {
	FORLIM = link->link->rpl[link->link->i];
	for (ch = link->link->rpl[link->link->i - 1]; ch <= FORLIM; ch++)
	  sprintf(link->temps + strlen(link->temps), "%c", ch);
	link->link->i += 2;
	n--;
      }
      break;

    case '\004':
      sprintf(link->temps + strlen(link->temps), "%c", link->link->matchchar);
      break;

    case '\005':
    case '\017':
    case '\020':
    case '\021':
      /* blank case */
      break;

    case '\006':
    case '\007':
    case '\b':
      skiprex(link->link->rpl, link->link);
      if (link->link->countright > 0)
	sprintf(link->temps + strlen(link->temps), "%.*s",
		(int)(link->link->countright - link->link->countleft),
		link->link->s + link->link->countleft - 1);
      break;

    case '\t':
      n = link->link->rpl[link->link->i - 1];
      j = link->link->i + 2;
      do {
	link->link->i = j;
	getrepl(link);
	n--;
      } while (n > 0);
      link->link->i++;
      break;

    case '\n':
      sprintf(STR3, "%c", link->link->rpl[link->link->i - 1]);
      if (!strcmp(STR3, nparen_) && !strcmp(
	    (sprintf(STR2, "%c", link->link->rpl[link->link->i + 1]), STR2),
	    end_))
	link->notnum = link->link->rpl[link->link->i];
      skiprex(link->link->rpl, link->link);
      break;

    case '\f':
    case '\015':
      link->link->i++;
      break;

    case '\016':
      n = link->link->rpl[link->link->i - 1];
      link->link->i++;
      if (link->link->parflag && link->link->par[1][n - 1] != -1)
	sprintf(link->temps + strlen(link->temps), "%.*s",
		(int)(link->link->par[1][n - 1] - link->link->par[0][n - 1]),
		link->link->s + link->link->par[0][n - 1] - 1);
      break;

    case '\013':
      break;
      /* process up to end_, ignore other alternatives */
    }
  }
}  /*getrepl*/



Local Void replace(link)
struct LOC_compare *link;
{
  struct LOC_replace V;
  Char STR1[256];
  Char STR2[256];

  V.link = link;
  link->i = 1;
  *V.temps = '\0';
  V.notnum = 0;
  getrepl(&V);
  if (V.notnum > 0 && link->parflag && link->par[1][V.notnum - 1] != -1) {
    sprintf(V.temps, "%s%s",
	    strpart(STR1, link->s, (int)link->savep,
		    (int)(link->par[0][V.notnum - 1] - 1)),
	    strcpy(STR2, V.temps));
    strcat(V.temps, strpart(STR1, link->s, (int)link->par[1][V.notnum - 1],
			    (int)(*link->p - 1)));
  }
  strcpy(link->s + link->savep - 1, link->s + *link->p - 1);
  sprintf(STR2, "%s%s", V.temps, link->s + link->savep - 1);
  strcpy(link->s + link->savep - 1, STR2);
  *link->p = link->savep + strlen(V.temps);
}  /*replace*/






Static Void compare(s_, rex_, rpl_, p_, found)
Char *s_, *rex_, *rpl_;
long *p_;
boolean *found;
{
  struct LOC_compare V;

  V.s = s_;
  V.rex = rex_;
  V.rpl = rpl_;
  V.p = p_;
  V.i = 1;
  V.parflag = false;
  V.cimode = false;
  if (*V.rex == '\0') {
    *found = false;
    return;
  }
  if (*V.rpl == '\0') {
    *found = comp(&V);
    return;
  }
  V.countright = -1;
  V.matchchar = ' ';
  V.savep = *V.p;
  *found = comp(&V);
  if (*found)
    replace(&V);
}  /*compare*/




Void re_search(s, rex, pos, last, dir)
Char *s, *rex;
long *pos, *last, dir;
{
  boolean found, fast, vfast;
  Char rpl[2];

  found = false;
  fast = (rex[0] >= ' ');
  vfast = (fast && dir == 1);
  rpl[0] = '\0';
  while (!found && *pos <= strlen(s) && *pos >= 1) {
    *last = *pos;
    if (!fast || s[*pos - 1] == rex[0]) {
      compare(s, rex, rpl, last, &found);
      if (*last == *pos)   /*don't match null strings in search*/
	found = false;
    }
    if (found)
      break;
    if (vfast)
      *pos = strposc(s, rex[0], *pos + 1);
    else
      *pos += dir;
  }
  if (!found) {
    *pos = 0;
    *last = 0;
  } else
    (*last)--;
}



boolean re_compare(s, rex)
Char *s, *rex;
{
  long p;
  boolean found;
  Char rpl[2];

  p = 1;
  rpl[0] = '\0';
  compare(s, rex, rpl, &p, &found);
  return (found && p == strlen(s) + 1);
}



Void re_replace(s, rex, rpl, pos, last)
Char *s, *rex, *rpl;
long *pos, *last;
{
  boolean found;

  *last = *pos;
  compare(s, rex, rpl, last, &found);
  if (!found) {
    *pos = 0;
    *last = 0;
  } else
    (*last)--;
}




boolean re_matchstr(s_, pat)
Char *s_, *pat;
{
  Char s[256], rex[256];

  strcpy(s, s_);
  re_compile(pat, rex, "");
  return (re_compare(s, rex));
}



boolean re_matchsubstr(s_, pat, start, last)
Char *s_, *pat;
long start, *last;
{
  Char s[256], rex[256], rpl[256];

  strcpy(s, s_);
  re_compile(pat, rex, "");
  *rpl = '\0';
  re_replace(s, rex, rpl, &start, last);
  return (*last > 0);
}



Void re_grepl(s, pat, rpl)
Char *s;
Char *pat, *rpl;
{
  Char srex[256], rrex[256];
  long pos, last;

  pos = 1;
  re_compile(pat, srex, "");
  re_compile(rpl, rrex, "");
  do {
    re_search(s, srex, &pos, &last, 1L);
    if (pos > 0) {
      re_replace(s, srex, rrex, &pos, &last);
      pos = last + 1;
    }
  } while (pos != 0);
}










/* End. */
