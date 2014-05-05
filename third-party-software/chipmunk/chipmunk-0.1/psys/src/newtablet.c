#define NEWTABLET_G

#include <stdio.h>
#include <p2c/p2c.h>
#include <p2c/newtablet.h>

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

boolean nt_init(device)
long device;
{
  return(1);
}

void nt_info(s, xl, yl, xh, yh)
char *s;
long *xl, *yl, *xh, *yh;
{
  if (! m_initialized)
    m_init_graphics();

  strcpy(s, "mouse");
  *xl = 0;
  *yl = 0;
  *xh = m_across;
  *yh = m_down;
}

boolean nt_read(x, y, status)
long *x, *y, *status;
{
  m_tablet_info pen;

  m_readpen(&pen);
  *x = pen.x;
  *y = pen.y;
  *status = pen.depressed * nt_down + pen.near_ * nt_near;

  return (1);
}




