#define ASM_G

#include <stdio.h>
#include <p2c/p2c.h>
#include <p2c/asm.h>

void asm_newbytes(p, n)
Anyptr *p;
long n;
{
  *p = malloc(n);
}
