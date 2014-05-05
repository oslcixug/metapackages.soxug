/* Output from p2c, the Pascal-to-C translator */
/* From input file "logsimed.text" */


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


/*

      LOG 4.1   Digital logic simulator definition editor

      David Gillespie  7/18/88

      The following module is subject to change at any time.
*/


/* $debug$ {*/


#include <p2c/p2c.h>


#define LOGSIMED_G
#include "logsimed.h"


/* Local variables for dasm_16: */
struct LOC_dasm_16 {
  Char *buf;
  int bufsiz;
  uchar *proc;
  long *pc;
} ;

Local Void dasmop PP((struct LOC_dasm_16 *LINK));

/* Local variables for dasmop: */
struct LOC_dasmop {
  struct LOC_dasm_16 *LINK;
} ;

Local Void append PP((struct LOC_dasm_16 *LINK, Char *s));

/* append to the local buffer. */
/* the buf and bufsiz are initialized in dasm_16() */
Local Void append(LINK, s)
struct LOC_dasm_16 *LINK;
Char *s;
{
#define GROW_SIZE 1024
Char *tmp;

  if (strlen(LINK->buf) + strlen(s) + 1 > LINK->bufsiz) {
    tmp = strdup(LINK->buf);
    Free(LINK->buf);
    LINK->bufsiz = strlen(tmp) + strlen(s) + 1 + GROW_SIZE;  
    LINK->buf = (Char*)Malloc(LINK->bufsiz);
    strcpy(LINK->buf, tmp);
    Free(tmp);
  }

  strcat(LINK->buf, s);
  return;
}


Local Char vr(i, LINK)
long i;
struct LOC_dasmop *LINK;
{
  return ((Char)((i & 15) + 65));
}

Local long getpseudo(LINK)
struct LOC_dasmop *LINK;
{
  long Result;

  if (LINK->LINK->proc[*LINK->LINK->pc - 1] < 128) {
/* p2c: logsimed.text, line 184: Note: Character >= 128 encountered [281] */
    Result = LINK->LINK->proc[*LINK->LINK->pc - 1] - 64;
    (*LINK->LINK->pc)++;
  } else {
    Result = LINK->LINK->proc[*LINK->LINK->pc - 1] +
	     (LINK->LINK->proc[*LINK->LINK->pc] - 32) * 128 - 64;
    *LINK->LINK->pc += 2;
  }
  return Result;
}

Local Void dasmoppar(z, LINK)
uchar z;
struct LOC_dasmop *LINK;
{
  uchar ch;

  ch = LINK->LINK->proc[*LINK->LINK->pc - 1];
  if (160 > ch || ch > 164 || ch == z) {
    dasmop(LINK->LINK);
    return;
  }
/* p2c: logsimed.text, line 202: Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 202: Note: Character >= 128 encountered [281] */
  append(LINK->LINK, "(");
  dasmop(LINK->LINK);
  append(LINK->LINK, ")");
}

Local Void dasmop(LINK)
struct LOC_dasm_16 *LINK;
{
  struct LOC_dasmop V;
  uchar ch;
  long i;
  Char STR1[256];

  V.LINK = LINK;
  ch = LINK->proc[*LINK->pc - 1];
  i = ch;
  (*LINK->pc)++;
  switch (i / 16) {

  case 0:
    switch (i) {

    case 0:
      (*LINK->pc)--;
      break;

    case 1:
      /* blank case */
      break;

    case 2:
      append(LINK, "END");
      break;

    case 3:
      append(LINK, "IF ");
      dasmop(LINK);
      break;

    case 4:
      append(LINK, "IFNONE ");
      dasmop(LINK);
      break;

    case 5:
      append(LINK, "IFZERO ");
      dasmop(LINK);
      break;

    case 6:
      append(LINK, "IFONE ");
      dasmop(LINK);
      break;

    case 7:
      append(LINK, "IFCONN ");
      dasmop(LINK);
      break;

    case 8:
      append(LINK, "IFZN ");
      dasmop(LINK);
      break;

    case 15:
      append(LINK, "ELSE");
      break;

    default:
      sprintf(STR1, "<%ld>", i);
      append(LINK, STR1);
      break;
    }
    break;

  case 1:
    switch (i & 15) {

    case 0:
      append(LINK, "##");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " = ");
      dasmop(LINK);
      break;

    case 1:
      append(LINK, "##");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " < ");
      dasmop(LINK);
      break;

    case 2:
      append(LINK, "CALL ");
      i = LINK->proc[*LINK->pc];
      *LINK->pc += 2;
      while (i > 128) {
	sprintf(STR1, "%c", LINK->proc[*LINK->pc - 1]);
        append(LINK, STR1);
	(*LINK->pc)++;
	i--;
      }
      break;

    case 6:
      append(LINK, "# ");
      i = LINK->proc[*LINK->pc - 1];
      (*LINK->pc)++;
      while (i > 32) {
	sprintf(STR1, "%c", LINK->proc[*LINK->pc - 1]);
        append(LINK, STR1);
	(*LINK->pc)++;
	i--;
      }
      break;

    case 7:
      append(LINK, "INST");
      if (LINK->proc[*LINK->pc - 1] > '"') {
	append(LINK, " ");
	sprintf(STR1, "%ld",
	  (LINK->proc[*LINK->pc + 1] - 32L) * 128 + LINK->proc[*LINK->pc] - 64);
        append(LINK, STR1);
      }
      if (LINK->proc[*LINK->pc - 1] > '$') {
	append(LINK, ",");
	sprintf(STR1, "%ld",
		(LINK->proc[*LINK->pc + 3] - 32L) * 128 +
		LINK->proc[*LINK->pc + 2] - 64);
        append(LINK, STR1);
      }
      *LINK->pc += LINK->proc[*LINK->pc - 1] - 33;
      break;

    case 8:
      append(LINK, "V");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " = ");
      dasmop(LINK);
      break;

    case 9:
      append(LINK, "V");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " = NOT V");
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      break;

    case 10:
      append(LINK, "V");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " = ZERO");
      break;

    case 11:
      append(LINK, "V");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      append(LINK, " = ONE");
      break;

    case 12:
      dasmop(LINK);
      append(LINK, " = PULLDN");
      break;

    case 13:
      dasmop(LINK);
      append(LINK, " = PULLUP");
      break;

    case 14:
      append(LINK, "#");
      sprintf(STR1, "%ld", LINK->proc[*LINK->pc - 1] + 1L);
      append(LINK, STR1);
      (*LINK->pc)++;
      append(LINK, " = ");
      dasmop(LINK);
      break;

    case 15:
      append(LINK, "#");
      sprintf(STR1, "%ld", LINK->proc[*LINK->pc - 1] + 1L);
      append(LINK, STR1);
      (*LINK->pc)++;
      append(LINK, " < ");
      dasmop(LINK);
      break;

    default:
      sprintf(STR1, "<%ld>", i);
      append(LINK, STR1);
      break;
    }
    break;

  case 2:
  case 3:
    append(LINK, "#");
    sprintf(STR1, "%ld", (i & 31) + 1);
    append(LINK, STR1);
    append(LINK, " = ");
    dasmop(LINK);
    break;

  case 4:
  case 5:
    append(LINK, "#");
    sprintf(STR1, "%ld", (i & 31) + 1);
    append(LINK, STR1);
    append(LINK, " < ");
    dasmop(LINK);
    break;

  case 6:
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    append(LINK, " = ");
    dasmop(LINK);
    break;

  case 7:
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    append(LINK, " = NOT ");
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    break;

  case 8:
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    append(LINK, " = ZERO");
    break;

  case 9:
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    append(LINK, " = ONE");
    break;

  case 10:
  case 11:
    switch (i & 31) {

    case 0:
      dasmoppar(ch, &V);
      append(LINK, " AND ");
      dasmoppar(ch, &V);
      break;

    case 1:
      dasmoppar('\0', &V);
      while (LINK->proc[*LINK->pc - 1] == i - 1) {
	(*LINK->pc)++;
	append(LINK, " NAND ");
	dasmoppar('\0', &V);
      }
      append(LINK, " NAND ");
      dasmoppar('\0', &V);
      break;

    case 2:
      dasmoppar(ch, &V);
      append(LINK, " OR ");
      dasmoppar(ch, &V);
      break;

    case 3:
      dasmoppar('\0', &V);
      while (LINK->proc[*LINK->pc - 1] == i - 1) {
	(*LINK->pc)++;
	append(LINK, " NOR ");
	dasmoppar('\0', &V);
      }
      append(LINK, " NOR ");
      dasmoppar('\0', &V);
      break;

    case 4:
      dasmoppar(ch, &V);
      append(LINK, " XOR ");
      dasmoppar(ch, &V);
      break;

    case 5:
      append(LINK, "NOT ");
      dasmoppar('\0', &V);
      break;

    case 6:
      append(LINK, "RISE ");
      dasmoppar('\0', &V);
      break;

    case 7:
      append(LINK, "FALL ");
      dasmoppar('\0', &V);
      break;

    case 8:
      append(LINK, "ZERO");
      break;

    case 9:
      append(LINK, "ONE");
      break;

    case 10:
      dasmop(LINK);
      append(LINK, " SAME ");
      dasmop(LINK);
      break;

    case 11:
      append(LINK, "##");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      break;

    case 12:
      append(LINK, "V");
      i = getpseudo(&V);
      sprintf(STR1, "%ld", i);
      append(LINK, STR1);
      break;

    case 13:
      append(LINK, "FIX ");
      dasmoppar('\0', &V);
      break;

    case 14:
      append(LINK, "AMP ");
      dasmoppar('\0', &V);
      break;

    case 15:
      append(LINK, "WONE");
      break;

    case 16:
      append(LINK, "#");
      sprintf(STR1, "%ld", LINK->proc[*LINK->pc - 1] + 1L);
      append(LINK, STR1);
      (*LINK->pc)++;
      break;

    case 17:
      append(LINK, "STRONG ");
      dasmoppar('\0', &V);
      break;

    default:
      sprintf(STR1, "<%ld>", i);
      append(LINK, STR1);
      break;
    }
    break;

  case 12:
  case 13:
    append(LINK, "#");
    sprintf(STR1, "%ld", (i & 31) + 1);
    append(LINK, STR1);
    break;

  case 14:
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    break;

  case 15:
    append(LINK, "NOT ");
    sprintf(STR1, "%c", vr(i, &V));
    append(LINK, STR1);
    break;
  }
}




/* The following routine forms the interface between simtype 16 and LOGED.


      This is the format of simtype 16 program strings:

         instr1 instr2 instr3 instr4 <0>

      The ASSEM and DASMINST procedures symbolically manipulate the
      Gate Definition Language used by the digital simulator.
      The GDL language is as follows:


                    GDL - Gate Definition Language

         Instruction  LOGED Syntax     Meaning of instruction
         -----------  ----- ------     ------- -- -----------
         00000000     end-of-program
         00000001     blank-line       Ignored
         00000010     END              End of IF section
         00000011     IF      x        Do if ONE or NONE
         00000100     IFNONE  x        Do if NONE
         00000101     IFZERO  x        Do if ZERO
         00000110     IFONE   x        Do if ONE
         00000111     IFCONN  x        Do if ZERO or ONE
         00001000     IFZN    x        Do if ZERO or NONE
         00001001                        (unused IF)
         0000101x                        (unused IF's)
         0000110x                        (unused IF's)
         00001110                        (unused IF's)
         00001111     ELSE             Beginning of ELSE section
         00010000     ppin =  x        Output to pseudo-pin
         00010001     ppin <  x        Open collector output to pseudo-pin
         00010010     CALL    name     Pascal procedure for gate
         00010011                        (unused)
         00010100                        (unused)
         00010101
         00010110     #                Comment
         00010111     INST             Instance-gate header
         00011000     pvar =  x        Store in pseudo-variable
         00011001     pvar =  NOT pvar
         00011010     pvar =  ZERO
         00011011     pvar =  ONE
         00011100     pin =   PULLDN   Pull-down pin or pseudo-pin
         00011101     pin =   PULLUP   Pull-up pin or pseudo-pin
         00011110     pin =   x        Output to high-numbered pin
         00011111     pin <   x        Open collector output to high pin
         001ppppp     pin =   x        Output to pin
         010ppppp     pin <   x        Open collector output
         0110vvvv     var =   x        Store in variable (NONE => ONE)
         0111vvvv     var =   NOT var  Short for 0110vvvv 10100101 1110vvvv
         1000vvvv     var =   ZERO     Short for 0110vvvv 10101000
         1001vvvv     var =   ONE      Short for 0110vvvv 10101001
         10100000   * AND     x,y      Logical AND, etc.
         10100001   * NAND    x,y
         10100010   * OR      x,y
         10100011   * NOR     x,y
         10100100   * XOR     x,y
         10100101   * NOT     x
         10100110   * RISE    p        ONE if pin has rising edge, else ZERO
         10100111   * FALL    p        Falling edge on pin
         10101000   * ZERO             Constants
         10101001   * ONE
         10101010   * SAME    p,q      ONE if pins are on same node
         10101011   * ppin             Input from pseudo-pin
         10101100   * pvar             Value of pseudo-variable
         10101101   * FIX     x        Fix (NONE => ZERO, others stay same)
         10101110   * AMP     x          (obsolete, copies x)
         10101111   *                    (obsolete, same as ONE)
         10110000   * pin              Input high-numbered pin
         10110001   * STRONG  p        Value of pin, NONE if weakly driven
         1011001x                        (unused)
         101101xx                        (unused)
         10111xxx                        (unused)
         110ppppp   * pin              Input pin
         1110vvvv   * var              Value of variable, ONE or ZERO
         1111vvvv   * NOT     var      Short for 10100101 1110vvvv

      Those codes marked with  *  are functions; those without are opcodes.
      A statement consists of an opcode followed by necessary functions
      and/or pin numbers.  Functions may take other functions as arguments.
      In the ASSEM procedure, two-argument codes are taken as infix operators.

      The simulator uses three-state logic.  ONE and ZERO are as usual.
      NONE means "no connection," or no current flowing.  Pins are numbered
      #1 through #255.  Internal variables are A through P, and store only
      ZERO or ONE.

      Gates which are compiled instances of circuits may include "pseudo-pins",
      which are referred to by number, ##0 through ##28735.  Pseudo-pin P is
      compiled as an opcode followed by the byte (64 + P) if P <= 63, or
      by bytes (128+P1) and (32+P2) where P1 = (P-64) MOD 128 and
      P2 = (P-64) DIV 128.

      The codes for operating on high-numbered pins are followed by a single
      byte P >= 32; the pin number is P+1.

      The "pull-up", "pull-down", "rise", "fall", "same", and "strong"
      instructions take one or two pins as arguments; these arguments
      should be one of the codes for reading pins or pseudo-pins.

      Instance gates may also include "pseudo-variables", which are referred
      to as V0 through V28735.  Pseudo-variables act just like the regular
      variables A through P, and are compiled in the same way as
      pseudo-pins (but with the appropriate opcodes).

      Instance gates contain INST as the first opcode of their program.
      This opcode is followed by the byte 32+N, where N is the total number
      of bytes in the INST instruction.  (For a fully-specified
      instruction, this will by 32+6=38.) The next two bytes, if specified,
      are the number of pseudo-pins; then comes the two-byte number of
      pseudo-variables used by the gate.  The two-byte numbers are stored
      in the same format as the operands of pseudo-pin and -variable
      references, where the one-byte form is padded out by adding a 32
      byte.

      The CALL opcode is followed by a byte 255, then a byte (128 + N),
      then an N-character symbol name.  During execution, the 255 byte is
      used to store a hash code identifying the procedure.

      The "comment" opcode is followed by a 32+N byte, then by N text
      characters.

      For example,  00100000 10100001 11000001 11110001  means
                      #1  =   NAND of    #2,      B      or in LOGED:
                      #1 = #2 NAND B
*/


/* dasm_16() returns a pointer to allocated memory. */
/* the caller must ensure it is freed. */
Char *dasm_16(proc_, pc_)
uchar *proc_;
long *pc_;
{
  struct LOC_dasm_16 V;

  V.bufsiz = 256;
  V.buf = (Char*)Malloc(V.bufsiz);
  V.proc = proc_;
  V.pc = pc_;
  *V.buf = '\0';
  TRY(try1);
    dasmop(&V);
    if (V.proc[*V.pc - 1] == '\026') {   /*add comment*/
      if (*V.buf != '\0')
	append(&V, "  ");
      dasmop(&V);
    }
  RECOVER(try1);
    if (P_escapecode == -20)
      _Escape(P_escapecode);
    *V.buf = '\0';
  ENDTRY(try1);
  return V.buf;
}


long indent_16(proc, pc)
uchar *proc;
long pc;
{
  long Result;

  switch (proc[pc - 1]) {

  case 2:
    Result = -3;
    break;

  default:
    if (proc[pc - 1] >= 3 && proc[pc - 1] <= 14)
      Result = 3;
    else
      Result = 0;
    break;
  }
  return Result;
}


/* Local variables for length_16: */
struct LOC_length_16 {
  uchar *proc;
  long pc;
} ;

Local Void instlen(LINK)
struct LOC_length_16 *LINK;
{
  uchar ch;

  ch = LINK->proc[LINK->pc - 1];
  LINK->pc++;
  switch (ch / 16) {

  case 0:
    switch (ch) {

    case 0:
      LINK->pc--;
      break;

    case 1:
    case 2:
    case 15:
      /* blank case */
      break;

    default:
      if (ch >= 3 && ch <= 14)
	instlen(LINK);
      break;
    }
    break;

  case 1:
    switch (ch & 15) {

    case 0:
    case 1:
    case 8:
      LINK->pc += (LINK->proc[LINK->pc - 1] >= 128) + 1;
/* p2c: logsimed.text, line 605: Note: Character >= 128 encountered [281] */
      instlen(LINK);
      break;

    case 9:
    case 10:
    case 11:
      LINK->pc += (LINK->proc[LINK->pc - 1] >= 128) + 1;
/* p2c: logsimed.text, line 609: Note: Character >= 128 encountered [281] */
      break;

    case 6:
      LINK->pc += LINK->proc[LINK->pc - 1] - 31;
      break;

    case 7:
      LINK->pc += LINK->proc[LINK->pc - 1] - 33;
      break;

    case 2:
      LINK->pc += LINK->proc[LINK->pc] - 126;
      break;

    case 12:
    case 13:
      instlen(LINK);
      break;

    case 14:
    case 15:
      LINK->pc++;
      instlen(LINK);
      break;
    }
    break;

  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    instlen(LINK);
    break;

  case 7:
  case 8:
  case 9:
  case 12:
  case 13:
  case 14:
  case 15:
    /* blank case */
    break;

  case 10:
  case 11:
    switch (ch & 31) {

    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 10:
      instlen(LINK);
      instlen(LINK);
      break;

    case 5:
    case 6:
    case 7:
    case 13:
    case 14:
    case 17:
      instlen(LINK);
      break;

    case 8:
    case 9:
    case 15:
      /* blank case */
      break;

    case 16:
      LINK->pc++;
      break;

    case 11:
    case 12:
      LINK->pc += (LINK->proc[LINK->pc - 1] >= 128) + 1;
/* p2c: logsimed.text, line 643: Note: Character >= 128 encountered [281] */
      break;
    }
    break;
  }
}


long length_16(proc_, pc_)
uchar *proc_;
long pc_;
{
  struct LOC_length_16 V;
  long savepc;

  V.proc = proc_;
  V.pc = pc_;
  savepc = V.pc;
  instlen(&V);
  if (V.proc[V.pc - 1] == '\026')   /*comment*/
    instlen(&V);
  return (V.pc - savepc);
}


/* Local variables for assem_16: */
struct LOC_assem_16 {
  uchar *proc;
  long *pc;
  Char *s;
  boolean snerr;
  long procsiz;
} ;

Local Void assem PP((struct LOC_assem_16 *LINK));

Local Void store(b, LINK)
short b;
struct LOC_assem_16 *LINK;
{
  (*LINK->pc)++;
  LINK->proc[*LINK->pc - 1] = b;
}

Local boolean strcheck(s, pref, LINK)
Char *s;
Char *pref;
struct LOC_assem_16 *LINK;
{
  if (strbegins(s, pref)) {
    strcpy(s, s + strlen(pref));
    return true;
  } else
    return false;
}

/* Local variables for assem: */
struct LOC_assem {
  struct LOC_assem_16 *LINK;
} ;

Local Void factor(LINK)
struct LOC_assem *LINK;
{
  long i, j, FORLIM;
  Char STR2[6];
  Char STR3[256];

  if (LINK->LINK->s[0] == '\0') {
    LINK->LINK->snerr = true;
    return;
  }
  if (strcheck(LINK->LINK->s, "(", LINK->LINK)) {
    assem(LINK->LINK);
    if (!strcheck(LINK->LINK->s, ")", LINK->LINK))
      LINK->LINK->snerr = true;
    return;
  }
  if (!strncmp(LINK->LINK->s, "##", 2L) && isdigit(LINK->LINK->s[2])) {
    strcpy(LINK->LINK->s, LINK->LINK->s + 2);
    i = 0;
    while (isdigit(LINK->LINK->s[0])) {
      i = i * 10 + LINK->LINK->s[0] - 48;
      strcpy(LINK->LINK->s, LINK->LINK->s + 1);
    }
    if (i > 28735) {
      LINK->LINK->snerr = true;
      return;
    }
    store(171, LINK->LINK);
    j = *LINK->LINK->pc;
    if (i < 64)
      store((int)(i + 64), LINK->LINK);
    else {
      store((int)(((i - 64) & 127) + 128), LINK->LINK);
      store((int)((i - 64) / 128 + 32), LINK->LINK);
    }
    if (strcheck(LINK->LINK->s, "=PULLDN", LINK->LINK)) {
      for (i = *LINK->LINK->pc; i >= j; i--)
	LINK->LINK->proc[i] = LINK->LINK->proc[i - 1];
      (*LINK->LINK->pc)++;
      LINK->LINK->proc[j - 1] = '\034';
      return;
    }
    if (strcheck(LINK->LINK->s, "=PULLUP", LINK->LINK) ||
	strcheck(LINK->LINK->s, "=WONE", LINK->LINK)) {
      for (i = *LINK->LINK->pc; i >= j; i--)
	LINK->LINK->proc[i] = LINK->LINK->proc[i - 1];
      (*LINK->LINK->pc)++;
      LINK->LINK->proc[j - 1] = '\035';
      return;
    }
    if (strcheck(LINK->LINK->s, "=", LINK->LINK)) {
      LINK->LINK->proc[j - 1] = '\020';
      assem(LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "<", LINK->LINK)) {
      LINK->LINK->proc[j - 1] = '\021';
      assem(LINK->LINK);
    }
    return;
  }
  if (strcheck(LINK->LINK->s, "#", LINK->LINK)) {
    i = 0;
    while (isdigit(LINK->LINK->s[0])) {
      i = i * 10 + LINK->LINK->s[0] - 48;
      strcpy(LINK->LINK->s, LINK->LINK->s + 1);
    }
    if (i < 1 || i > 256) {
      LINK->LINK->snerr = true;
      return;
    }
    if (strcheck(LINK->LINK->s, "=PULLDN", LINK->LINK)) {
      store(28, LINK->LINK);
      if (i > 32) {
	store(176, LINK->LINK);
	store((int)(i - 1), LINK->LINK);
      } else
	store((int)(i + 191), LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "=PULLUP", LINK->LINK) ||
	strcheck(LINK->LINK->s, "=WONE", LINK->LINK)) {
      store(29, LINK->LINK);
      if (i > 32) {
	store(176, LINK->LINK);
	store((int)(i - 1), LINK->LINK);
      } else
	store((int)(i + 191), LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "=", LINK->LINK)) {
      if (i > 32) {
	store(30, LINK->LINK);
	store((int)(i - 1), LINK->LINK);
      } else
	store((int)(i + 31), LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "<", LINK->LINK)) {
      if (i > 32) {
	store(31, LINK->LINK);
	store((int)(i - 1), LINK->LINK);
      } else
	store((int)(i + 63), LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (i > 32) {
      store(176, LINK->LINK);
      store((int)(i - 1), LINK->LINK);
    } else
      store((int)(i + 191), LINK->LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "END", LINK->LINK)) {
    store(2, LINK->LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "ELSE", LINK->LINK)) {
    store(15, LINK->LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "IF", LINK->LINK)) {
    if (strcheck(LINK->LINK->s, "NONE", LINK->LINK)) {
      store(4, LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (!strncmp(LINK->LINK->s, "ZERO", 4L) && strlen(LINK->LINK->s) > 4) {
      strcpy(LINK->LINK->s, LINK->LINK->s + 4);
      store(5, LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (!strncmp(LINK->LINK->s, "ONE", 3L) && strlen(LINK->LINK->s) > 3) {
      strcpy(LINK->LINK->s, LINK->LINK->s + 3);
      store(6, LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "CONN", LINK->LINK)) {
      store(7, LINK->LINK);
      assem(LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "ZN", LINK->LINK)) {
      store(8, LINK->LINK);
      assem(LINK->LINK);
    } else {
      store(3, LINK->LINK);
      assem(LINK->LINK);
    }
    return;
  }
  if (strcheck(LINK->LINK->s, "CALL", LINK->LINK)) {
    if (LINK->LINK->s[0] == '\0') {
      LINK->LINK->snerr = true;
      return;
    }
    store(18, LINK->LINK);
    store(255, LINK->LINK);
    store((int)(strlen(LINK->LINK->s) + 128), LINK->LINK);
    FORLIM = strlen(LINK->LINK->s) - 1;
    for (i = 0; i <= FORLIM; i++)
      store(LINK->LINK->s[i], LINK->LINK);
    strcpy(LINK->LINK->s, "\0");
    return;
  }
  if (strcheck(LINK->LINK->s, "INST", LINK->LINK)) {
    store(23, LINK->LINK);
    store(34, LINK->LINK);
    j = *LINK->LINK->pc;
    if (LINK->LINK->s[0] != '\0' && !LINK->LINK->snerr) {
      i = 0;
      while (*LINK->LINK->s != '\0' && isdigit(LINK->LINK->s[0])) {
	i = i * 10 + LINK->LINK->s[0] - 48;
	strcpy(LINK->LINK->s, LINK->LINK->s + 1);
      }
      if (i > 28735)
	LINK->LINK->snerr = true;
      else {
	if (i < 64) {
	  store((int)(i + 64), LINK->LINK);
	  store(32, LINK->LINK);
	} else {
	  store((int)(((i - 64) & 127) + 128), LINK->LINK);
	  store((int)((i - 64) / 128 + 32), LINK->LINK);
	}
      }
      LINK->LINK->proc[j - 1] += 2;
    }
    if (!(strcheck(LINK->LINK->s, ",", LINK->LINK) &&
	  LINK->LINK->s[0] != '\0') || LINK->LINK->snerr)
      return;
    i = 0;
    while (*LINK->LINK->s != '\0' && isdigit(LINK->LINK->s[0])) {
      i = i * 10 + LINK->LINK->s[0] - 48;
      strcpy(LINK->LINK->s, LINK->LINK->s + 1);
    }
    if (i > 28735)
      LINK->LINK->snerr = true;
    else {
      if (i < 64) {
	store((int)(i + 64), LINK->LINK);
	store(32, LINK->LINK);
      } else {
	store((int)(((i - 64) & 127) + 128), LINK->LINK);
	store((int)((i - 64) / 128 + 32), LINK->LINK);
      }
    }
    LINK->LINK->proc[j - 1] += 2;
    return;
  }
  if (strcheck(LINK->LINK->s, "NOT", LINK->LINK)) {
    store(165, LINK->LINK);
    i = *LINK->LINK->pc;
    factor(LINK);
    if (*LINK->LINK->pc == i + 1 &&
	LINK->LINK->proc[*LINK->LINK->pc - 1] / 16 == 14) {
      LINK->LINK->proc[i - 1] = LINK->LINK->proc[*LINK->LINK->pc - 1] + 16;
      *LINK->LINK->pc = i;
    }
    return;
  }
  if (strcheck(LINK->LINK->s, "RISE", LINK->LINK)) {
    store(166, LINK->LINK);
    factor(LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "FALL", LINK->LINK)) {
    store(167, LINK->LINK);
    factor(LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "0", LINK->LINK) || strcheck(LINK->LINK->s,
	"ZERO", LINK->LINK) || strcheck(LINK->LINK->s, "FALSE", LINK->LINK)) {
    store(168, LINK->LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "1", LINK->LINK) || strcheck(LINK->LINK->s,
	"ONE", LINK->LINK) || strcheck(LINK->LINK->s, "TRUE", LINK->LINK)) {
    store(169, LINK->LINK);
    return;
  }
  if (strcheck(LINK->LINK->s, "FIX", LINK->LINK)) {
    store(173, LINK->LINK);
    factor(LINK);
    return;
  }
  /*     else if strcheck(s, 'AMP') then
          begin
             store(174);
             factor;
          end
       else if strcheck(s, 'WONE') then
          begin
             store(175);
          end   */
  if (strcheck(LINK->LINK->s, "STRONG", LINK->LINK)) {
    store(177, LINK->LINK);
    factor(LINK);
    return;
  }
  if (LINK->LINK->s[0] >= 'A' && LINK->LINK->s[0] <= 'P') {
    i = LINK->LINK->s[0] - 65;
    strcpy(LINK->LINK->s, LINK->LINK->s + 1);
    if (!strcheck(LINK->LINK->s, "=", LINK->LINK)) {
      store((int)(i + 224), LINK->LINK);
      return;
    }
    sprintf(STR2, "NOT%c", (Char)(i + 65));
    if (strcheck(LINK->LINK->s, STR2, LINK->LINK)) {
      store((int)(i + 112), LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "0", LINK->LINK) || strcheck(LINK->LINK->s,
	  "ZERO",
	  LINK->LINK) || strcheck(LINK->LINK->s, "FALSE", LINK->LINK)) {
      store((int)(i + 128), LINK->LINK);
      return;
    }
    if (strcheck(LINK->LINK->s, "1", LINK->LINK) || strcheck(LINK->LINK->s,
	  "ONE", LINK->LINK) || strcheck(LINK->LINK->s, "TRUE", LINK->LINK))
      store((int)(i + 144), LINK->LINK);
    else {
      store((int)(i + 96), LINK->LINK);
      assem(LINK->LINK);
    }
    return;
  }
  if (!(LINK->LINK->s[0] == 'V' && isdigit(LINK->LINK->s[1]))) {
    LINK->LINK->snerr = true;
    return;
  }
  i = 0;
  strcpy(LINK->LINK->s, LINK->LINK->s + 1);
  while (isdigit(LINK->LINK->s[0])) {
    i = i * 10 + LINK->LINK->s[0] - 48;
    strcpy(LINK->LINK->s, LINK->LINK->s + 1);
  }
  if (i > 28735) {
    LINK->LINK->snerr = true;
    return;
  }
  store(172, LINK->LINK);
  j = *LINK->LINK->pc;
  if (i < 64)
    store((int)(i + 64), LINK->LINK);
  else {
    store((int)(((i - 64) & 127) + 128), LINK->LINK);
    store((int)((i - 64) / 128 + 32), LINK->LINK);
  }
  if (!strcheck(LINK->LINK->s, "=", LINK->LINK))
    return;
  sprintf(STR3, "NOTV%ld", i);
  if (strcheck(LINK->LINK->s, STR3, LINK->LINK)) {
    LINK->LINK->proc[j - 1] = '\031';
    return;
  }
  if (strcheck(LINK->LINK->s, "0", LINK->LINK) || strcheck(LINK->LINK->s,
	"ZERO", LINK->LINK) || strcheck(LINK->LINK->s, "FALSE", LINK->LINK)) {
    LINK->LINK->proc[j - 1] = '\032';
    return;
  }
  if (strcheck(LINK->LINK->s, "1", LINK->LINK) || strcheck(LINK->LINK->s,
	"ONE", LINK->LINK) || strcheck(LINK->LINK->s, "TRUE", LINK->LINK))
    LINK->LINK->proc[j - 1] = '\033';
  else {
    LINK->LINK->proc[j - 1] = '\030';
    assem(LINK->LINK);
  }
}

Local Void assem(LINK)
struct LOC_assem_16 *LINK;
{
  struct LOC_assem V;
  long i, j, i0;

  V.LINK = LINK;
  i = -1;
  i0 = -1;
  do {
    store(1, LINK);
    if (i0 == -1)
      i0 = i;
    i = *LINK->pc;
    factor(&V);
    j = -1;
    if (strcheck(LINK->s, "AND", LINK))
      j = 0;
    else if (strcheck(LINK->s, "NAND", LINK))
      j = 1;
    else if (strcheck(LINK->s, "OR", LINK))
      j = 2;
    else if (strcheck(LINK->s, "NOR", LINK))
      j = 3;
    else if (strcheck(LINK->s, "XOR", LINK))
      j = 4;
    else if (strcheck(LINK->s, "SAME", LINK))
      j = 10;
    if (j != -1) {
      if (i0 != -1 && LINK->proc[i0 - 1] == j + 160) {
	if (j == 1 || j == 3)
	  j--;
      }
      LINK->proc[i - 1] = (Char)(j + 160);
    }
  } while (j != -1);
}



long assem_16(buf_, proc_, pc_)
Char *buf_;
uchar *proc_;
long *pc_;
{
  struct LOC_assem_16 V;
  long Result;
  Char *buf;
  long i, j, savepc;
  Char *STR2;
  long FORLIM;


  buf = strdup(buf_);
  V.s = strdup(buf_);
  V.proc = proc_;
  V.pc = pc_;
  (*V.pc)--;
  savepc = *V.pc;
  *V.s = '\0';
  i = 1;
  while (i <= strlen(buf) && (buf[i - 1] != '#' ||
	   (strlen(buf) > i && ((buf[i] == '#' && strlen(buf) > i + 1 &&
				 isdigit(buf[i + 1])) || isdigit(buf[i]))))) {
    if (buf[i - 1] > ' ')
      sprintf(V.s + strlen(V.s), "%c", toupper(buf[i - 1]));
    i++;
  }
/* p2c: logsimed.text, line 1093:
 * Note: Null character at end of sprintf control string [148] */
  STR2 = strdup(buf + i - 1);
  strcpy(buf, STR2);
  Free(STR2);
  V.snerr = false;
  if (V.s[0] == '\0') {
    store(1, &V);
    Result = 1;
  } else {
    assem(&V);
    i = 1;
    while (strcheck(V.s, ";", &V)) {
      assem(&V);
      i++;
    }
    Result = i;
    j = savepc;
    FORLIM = *V.pc;
    for (i = savepc; i < FORLIM; i++) {
      if (V.proc[i] != '\001') {
	j++;
	V.proc[j - 1] = V.proc[i];
      }
    }
    *V.pc = j;
  }
  if (*buf != '\0') {
    strcpy(buf, buf + 1);
    strcheck(buf, " ", &V);
    store(22, &V);
    store((int)(strlen(buf) + 32), &V);
    FORLIM = strlen(buf);
    for (i = 0; i < FORLIM; i++)
      store(buf[i], &V);
  }
  if (V.s[0] != '\0' || V.snerr) {
    *V.pc = savepc;
    Result = 0;
  }
  V.proc[*V.pc] = '\0';
  Free(buf);
  Free(V.s);
  return Result;
}


/* Local variables for edit_16: */
struct LOC_edit_16 {
   uchar **proc;
   long *proclen;
   Char name[9];
   long x, y, pc, pctop, indtop, indent, instrptr, height;
   uchar instr[1023];
   uchar ch;
} ;

Local Void resizeproc(len, LINK)
long len;
struct LOC_edit_16 *LINK;
{
  long newlen;
  uchar *newproc;

  if (*LINK->proclen >= len)
    return;
  newlen = *LINK->proclen;
  while (newlen < len)
    newlen *= 2;
  newproc = (uchar *)Malloc(newlen);
  memmove((Anyptr)newproc, (Anyptr)(*LINK->proc), *LINK->proclen);
  Free(*LINK->proc);
  *LINK->proclen = newlen;
  *LINK->proc = newproc;
}

Local Void dasminst(LINK)
struct LOC_edit_16 *LINK;
{
  long ind;
  Char *buf;

  ind = indent_16(*LINK->proc, LINK->pc);
  if (ind < 0) {
    if (LINK->indent >= -ind) {
      printf("\b\b\b");
      LINK->indent += ind;
    }
  } else if (ind > 0)
    LINK->indent += ind;
  else if ((*LINK->proc)[LINK->pc - 1] == '\017' && LINK->indent >= 3)
    printf("\b\b\b");
  buf = dasm_16(*LINK->proc, &LINK->pc);
  printf("%s\t", buf);
  Free(buf);
}

Local Void showdef(LINK)
struct LOC_edit_16 *LINK;
{
  long i, pc0, FORLIM;

  LINK->x = 0;
  LINK->y = -1;
  pc0 = LINK->pc;
  LINK->pc = LINK->pctop;
  if ((*LINK->proc)[LINK->pctop - 1] == '\002')
    LINK->indent = LINK->indtop + 3;
  else
    LINK->indent = LINK->indtop;
  FORLIM = LINK->height;
  for (i = 0; i <= FORLIM; i++) {
    if (LINK->pc == pc0 && LINK->y < 0) {
      if ((*LINK->proc)[LINK->pc - 1] == '\002')
	LINK->x = LINK->indent - 3;
      else
	LINK->x = LINK->indent;
      LINK->y = i;
    }
    nk_gotoxy(0, (int)i);
    if (LINK->indent != 0)
      printf("%*c", (int)LINK->indent, ' ');
    dasminst(LINK);
    if (i == 0) {
      nk_gotoxy(nc_curWindow->width - 10, 0);
      fputs(LINK->name, stdout);
    }
  }
  if (LINK->y < 0)
    LINK->y = 0;
  LINK->pc = pc0;
}

Local boolean insertinst(p, n, LINK)
long p, n;
struct LOC_edit_16 *LINK;
{
  long i;

  i = p;
  while ((*LINK->proc)[i - 1] != '\0')
    i++;
  if (i + n <= 1023) {
    resizeproc(i + n, LINK);
    do {
      (*LINK->proc)[i + n - 1] = (*LINK->proc)[i - 1];
      i--;
    } while (i >= p);
    return true;
  } else
    return false;
}

Local Void deleteinst(p, n, LINK)
long p, n;
struct LOC_edit_16 *LINK;
{
  long i;

  i = p;
  do {
    (*LINK->proc)[i - 1] = (*LINK->proc)[i + n - 1];
    i++;
  } while ((*LINK->proc)[i - 2] != '\0');
}

Local long doassem(s, repl, LINK)
Char *s;
boolean repl;
struct LOC_edit_16 *LINK;
{
  long Result, i, FORLIM;

  LINK->instrptr = 1;
  Result = assem_16(s, LINK->instr, &LINK->instrptr);
  if (LINK->instrptr == 0)
    return Result;
  if (repl)
    deleteinst(LINK->pc, length_16(*LINK->proc, LINK->pc), LINK);
  if (insertinst(LINK->pc, LINK->instrptr, LINK)) {
    FORLIM = LINK->instrptr;
    for (i = 1; i <= FORLIM; i++)
      (*LINK->proc)[i + LINK->pc - 2] = LINK->instr[i - 1];
  }
  return Result;
}

Local Void bogusreadln(s, hasdef, LINK)
Char *s;
boolean hasdef;
struct LOC_edit_16 *LINK;
{
  long pos;

  if (s && !hasdef)
    *s = '\0';
  nk_ungetkey(LINK->ch);
  pos = 1;

/* p2c: logsimed.text, line 1282:
 * Warning: Type mismatch in VAR parameter term [295] */
  newci_inputstring(s, im_keep, "\015\n\037\003\004", 
		    (Char *)&LINK->ch, false, &pos);
}



Void edit_16(proc_, proclen_, name_)
uchar **proc_;
long *proclen_;
Char *name_;
{
  struct LOC_edit_16 V;
  long x1, y1, pc0, pc1, sx, sy;
  Char *sbuf=NULL;
  Char *recallbuf=NULL;
  boolean editing;
  Char *STR1;
  Char *STR2;

  V.proc = proc_;
  V.proclen = proclen_;
  strcpy(V.name, name_);
  V.height = nc_curWindow->height - 1;
  V.pc = 1;
  V.pctop = 1;
  V.indtop = 0;
  showdef(&V);
  do {
    newci_inputmap();
    if ((*V.proc)[V.pc - 1] == '\017')
      nc_gotoXY((int)P_imax2(V.x - 3, 0L), (int)V.y);
    else
      nc_gotoXY((int)V.x, (int)V.y);
    V.ch = nk_getkey();
    if (V.ch == '?') {
      pc0 = 1;
      while ((*V.proc)[pc0 - 1] != '\0')
	pc0++;
      nc_gotoXY(0,
		(int)P_imax2(nc_curWindow->height - (pc0 + 15) / 16 - 1, 0L));
      putchar('\013');
      pc0 = 1;
      pc1 = length_16(*V.proc, pc0);
      while ((*V.proc)[pc0 - 1] != '\0') {
	if ((pc0 & 15) == 1 && pc0 > 1)
	  putchar('\n');
	if (pc0 == V.pc)
	  putchar(137);
/* p2c: logsimed.text, line 1313:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1313:
 * Note: WRITE statement contains color/attribute characters [203] */
	printf("%.2X", (*V.proc)[pc0 - 1] & 0xff);
	if (pc0 == V.pc)
	  putchar(136);
/* p2c: logsimed.text, line 1316:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1316:
 * Note: WRITE statement contains color/attribute characters [203] */
	pc0++;
	pc1--;
	if (pc1 <= 0) {
	  putchar('|');
	  pc1 = length_16(*V.proc, pc0);
	} else
	  putchar(' ');
      }
      if (pc0 == V.pc)
	printf("\21100\210\n");
      else
	printf("00\n");
/* p2c: logsimed.text, line 1328:
 * Note: Characters >= 128 encountered [281] */
/* p2c: logsimed.text, line 1328:
 * Note: WRITE statement contains color/attribute characters [203] */
      printf("Press any key to continue...");
      V.ch = nk_getkey();
      V.ch = '\0';
      showdef(&V);              /* was 152 */
    } else if (V.ch == 25) {  /* changed to 25 for recall buffer == ^Y */
      if (recallbuf && *recallbuf != '\0') {
	STR2 = (Char*)Malloc(strlen(recallbuf));
	x1 = strposb(recallbuf, "\0", strlen(recallbuf) - 1L);
	y1 = doassem(strpart(STR2, recallbuf, (int)(x1 + 1),
			     (int)(strlen(recallbuf) - 1L)), false, &V);
	recallbuf[x1] = '\0';
/* p2c: logsimed.text, line 1342:
 * Note: Modification of string length may translate incorrectly [146] */
	Free(STR2);
	showdef(&V);
      }
    } else if (V.ch == '\b' || V.ch >= 150 && V.ch <= 162 ||
	       V.ch >= ' ' && V.ch <= '\177') {
/* p2c: logsimed.text, line 1346:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1346:
 * Note: Character >= 128 encountered [281] */
      editing = (V.ch == '\b' || V.ch >= 157 && V.ch <= 162 || V.ch == 154 ||
		 V.ch == 153 || V.ch == 151 || V.ch == 150);
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
/* p2c: logsimed.text, line 1349:
 * Note: Character >= 128 encountered [281] */
      if (editing) {
	pc0 = V.pc;
        if (sbuf)
          Free(sbuf);
	sbuf = dasm_16(*V.proc, &pc0);
	if (V.ch == 161) {   /*EDIT key*/
	  V.ch = 150;
/* p2c: logsimed.text, line 1355:
 * Note: Character >= 128 encountered [281] */
	}
/* p2c: logsimed.text, line 1354:
 * Note: Character >= 128 encountered [281] */
      } else {
	nc_getXY(&sx, &sy);
	nc_insLine((int)sy, 1); 
      }
      if (sbuf == NULL) {
        sbuf = (Char*)Malloc(1024);
      }
      bogusreadln(sbuf, editing, &V);
      y1 = doassem(sbuf, editing, &V);
      if (y1 == 0) {
	putchar('\007');
        STR1 = (Char*)Malloc(strlen(sbuf) + strlen("# ") + 1);
	sprintf(STR1, "# %s", sbuf);
	y1 = doassem(STR1, false, &V);
        Free(STR1);
      }
      while (V.ch == '\015' && y1 > 1) {
	nk_ungetkey('\015');
	y1--;
      }
      showdef(&V);
    }
    if (V.ch < 32 && ((1L << V.ch) & 0x2400) != 0 && (*V.proc)[V.pc - 1] != '\0') {
      V.y++;
      if ((*V.proc)[V.pc - 1] < 32 && ((1L << (*V.proc)[V.pc - 1]) & 0x7ff8) != 0)
	V.x += 3;
      V.pc += length_16(*V.proc, V.pc);
      if ((*V.proc)[V.pc - 1] == '\002')
	V.x -= 3;
      if (V.x < 0)
	V.x = 0;
      if (V.y >= V.height) {
	nk_gotoxy(0, (int)V.height);
	putchar('\n');
	if ((*V.proc)[V.pctop - 1] < 32 &&
	    ((1L << (*V.proc)[V.pctop - 1]) & 0x7ff8) != 0)
	  V.indtop += 3;
	V.pctop += length_16(*V.proc, V.pctop);
	if ((*V.proc)[V.pctop - 1] == '\002')
	  V.indtop -= 3;
	if (V.indtop < 0)
	  V.indtop = 0;
	V.y = V.height - 1;
	if ((*V.proc)[V.pc - 1] < 32 && ((1L << (*V.proc)[V.pc - 1]) & 0x7ff8) != 0)
	  nk_gotoxy((int)(V.x + 3), (int)V.height);
	else
	  nk_gotoxy((int)V.x, (int)V.height);
	pc0 = V.pc;
	V.pc += length_16(*V.proc, V.pc);
	V.indent = V.x;
	showdef(&V);
	V.pc = pc0;
      }
    } else if (V.ch == '\037' && V.pc > 1) {
      V.y--;
      if (V.y < 1 && V.pctop > 1) {
	V.x = V.indtop;
	if ((*V.proc)[V.pctop - 1] == '\002')
	  V.indtop += 3;
	pc0 = V.pctop;
	V.pc = 1;
	V.pctop = 1;
	while (V.pc < pc0) {
	  V.pctop = V.pc;
	  V.pc += length_16(*V.proc, V.pc);
	}
	if ((*V.proc)[V.pctop - 1] < 32 &&
	    ((1L << (*V.proc)[V.pctop - 1]) & 0x7ff8) != 0)
	  V.indtop -= 3;
	if (V.indtop < 0)
	  V.indtop = 0;
	if (V.pctop == 1 && V.indtop != 0) {
	  V.indtop = 0;
	  showdef(&V);
	} else {
	  nk_gotoxy(nc_curWindow->width - 10, 0);
	  printf("%8c", ' ');
	  nk_gotoxy(0, 0);
	  putchar('\037');
	  V.y = 1;
	  nk_gotoxy((int)V.indtop, 0);
	  if ((*V.proc)[V.pctop - 1] == '\002')
	    printf("   ");
	  V.pc = V.pctop;
	  V.indent = V.indtop + 3;
	  showdef(&V);
	}
      } else {
	if ((*V.proc)[V.pc - 1] == '\002')
	  V.x += 3;
	pc0 = V.pc;
	V.pc = V.pctop;
	pc1 = V.pctop;
	while (pc1 < pc0) {
	  V.pc = pc1;
	  pc1 += length_16(*V.proc, pc1);
	}
	if ((*V.proc)[V.pc - 1] < 32 && ((1L << (*V.proc)[V.pc - 1]) & 0x7ff8) != 0)
	  V.x -= 3;
	if (V.x < 0)
	  V.x = 0;
      }
    } else if (V.ch == 9) {       /* was 163, now 9 (^I), insert */
      if (insertinst(V.pc, 1L, &V))
	(*V.proc)[V.pc - 1] = '\001';
      showdef(&V);
    } else if (V.ch == 11) {      /* was 165, now 11 (^K), delete */
      if ((*V.proc)[V.pc - 1] != '\0') {
	pc0 = V.pc;
        if (sbuf)
          Free(sbuf);
	sbuf = dasm_16(*V.proc, &pc0);
	if (recallbuf == NULL) {
	  recallbuf = strdup(sbuf);
        } else {
          STR2 = strdup(recallbuf);
          Free(recallbuf);
	  recallbuf = (Char*)Malloc(strlen(STR2) + strlen(sbuf) + 1 );  
	  strcpy(recallbuf,STR2);
	  strcat(recallbuf, sbuf);
          Free(STR2);
        }
	deleteinst(V.pc, length_16(*V.proc, V.pc), &V);
	showdef(&V);
      }
    }
  } while (V.ch != '\003' && V.ch != '\004');
  newci_inputunmap();

/* p2c: logsimed.text, line 1336:
 * Note: Character >= 128 encountered [281] */
  if (sbuf)
    Free(sbuf);
  if (recallbuf)
    Free(recallbuf);
  /*RECALL*/
/* p2c: logsimed.text, line 1454:
 * Note: Character >= 128 encountered [281] */
  /*INS LN*/
/* p2c: logsimed.text, line 1460:
 * Note: Character >= 128 encountered [281] */
  /*DEL LN*/
}




Void dump_16(proc, sl, indenting)
uchar **proc;
na_strlist **sl;
boolean indenting;
{
  long pc, indent, oindent, proclen;
  na_strlist *l1;
  Char *buf;
  Char *STR1;
  Char STR2[256];

  proclen = 1;
  while ((*proc)[proclen - 1] != '\0')
    proclen++;
  pc = 1;
  indent = 0;
  oindent = 0;
  while (pc < proclen) {
    if ((*proc)[pc - 1] < 32 && ((1L << (*proc)[pc - 1]) & 0x8004L) != 0)
      indent -= 3;
    if (indenting)
      oindent = indent;
    if ((*proc)[pc - 1] < 32 && ((1L << (*proc)[pc - 1]) & 0xfff8L) != 0)
      indent += 3;
    buf = dasm_16(*proc, &pc);
    STR1 = (Char*)Malloc(strlen(buf) + oindent +1);
    sprintf(STR1, "%*s%s", (int)oindent, "", buf);
    l1 = strlist_append(sl, STR1);
    Free(STR1);
    Free(buf);
  }
  if (!indenting)
    return;
  if (pc != proclen)
    l1 = strlist_append(sl, "# Warning: terminator is not #0");
  if (indent != 0) {
    sprintf(STR2, "# Warning: final indentation level was %ld", indent / 3);
    l1 = strlist_append(sl, STR2);
  }
}



Void read_16(proc, proclen, sl)
uchar **proc;
long *proclen;
na_strlist *sl;
{
  long pc, instrptr, i, newlen;
  uchar instr[1023];
  na_strlist *l1;
  Char *STR1;

  pc = 1;
  l1 = sl;
  while (l1 != NULL) {
    instrptr = 1;
    i = assem_16(l1->s, instr, &instrptr);
    if (instrptr != 0)
      pc += instrptr;
    else {
      instrptr = 1;
      STR1 = (Char*)Malloc(strlen(l1->s) + strlen("# ") + 1);
      sprintf(STR1, "# %s", l1->s);
      i = assem_16(STR1, instr, &instrptr);
      pc += instrptr;
      Free(STR1);
    }
    l1 = l1->next;
  }
  newlen = 32;
  while (newlen < pc)
    newlen *= 2;
  if (newlen > *proclen) {
    if (*proclen > 0)
      Free(*proc);
    *proclen = newlen;
    *proc = (uchar *)Malloc(*proclen);
  }
  pc = 1;
  l1 = sl;
  while (l1 != NULL) {
    instrptr = pc;
    i = assem_16(l1->s, *proc, &instrptr);
    if (instrptr < pc) {
      instrptr = pc;
      STR1 = (Char*)Malloc(strlen(l1->s) +2 +1);
      sprintf(STR1, "# %s", l1->s);
      i = assem_16(STR1, *proc, &instrptr);
      Free(STR1);
    }
    pc = instrptr + 1;
    l1 = l1->next;
  }
  (*proc)[pc - 1] = '\0';
}











/* End. */
