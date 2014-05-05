#define NEWKBD_G

#include <p2c/p2c.h>

#ifndef NEWCRT_H
#include <p2c/newcrt.h>
#endif

#ifndef NEWKBD_H
#include <p2c/newkbd.h>
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#include <X11/Xlib.h>

static char *KeyNames[] = {
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "BackSpace",
  "Tab",
  "Linefeed",
  "Clear",
  "12",
  "Return",
  "13",
  "14",
  "15",
  "16",
  "17",
  "18",
  "Pause",
  "20",
  "21",
  "22",
  "23",
  "24",
  "25",
  "26",
  "Escape",
  "28",
  "29",
  "30",
  "31",
  "space",
  "exclam",
  "quotedbl",
  "numbersign",
  "dollar",
  "percent",
  "ampersand",
  "quoteright",
  "parenleft",
  "parenright",
  "asterisk",
  "plus",
  "comma",
  "minus",
  "period",
  "slash",
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "colon",
  "semicolon",
  "less",
  "equal",
  "greater",
  "question",
  "at",
  "A",
  "B",
  "C",
  "D",
  "E",
  "F",
  "G",
  "H",
  "I",
  "J",
  "K",
  "L",
  "M",
  "N",
  "O",
  "P",
  "Q",
  "R",
  "S",
  "T",
  "U",
  "V",
  "W",
  "X",
  "Y",
  "Z",
  "bracketleft",
  "backslash",
  "bracketright",
  "asciicircum",
  "underscore",
  "quoteleft",
  "a",
  "b",
  "c",
  "d",
  "e",
  "f",
  "g",
  "h",
  "i",
  "j",
  "k",
  "l",
  "m",
  "n",
  "o",
  "p",
  "q",
  "r",
  "s",
  "t",
  "u",
  "v",
  "w",
  "x",
  "y",
  "z",
  "braceleft",
  "bar",
  "braceright",
  "asciitilde",
};

extern void nk_crtwritestr(x, y, str, field)
short x, y;
char *str;
short field;
{
  int len, field2;
  short savehighlight = nc_highlight;

  nc_setHighlight(nk_screenhigh);
  if (x == -1) {
    if (field)
      nc_clearXY(0, y, nc_curWindow->width, 1);
    nc_putStr((nc_curWindow->width-strlen(str)) / 2, y, str);
  } else {
    if ((field2 = abs(field)) > (len = ((str == NULL)?0:strlen(str))))
      if (field < 0) {
	nc_putStr(x, y, str);
	nc_clearXY(x+len, y, field2-len, 1);
      } else {
	nc_clearXY(x, y, field2-len, 1);
	nc_putStr(x+field2-len, y, str);
      }
    else
      nc_putStr(x, y, str);
  }
  nc_setHighlight(savehighlight);
}

extern void nk_ungetstr(str)
char *str;
{
  fprintf(stderr, "NEWKBD kluge:  nk_ungetstr() not supported on UNIX\n");
}


extern void nk_deluserkey(ukr)
nk_userkeyrec *ukr;
{
  fprintf(stderr, "NEWKBD kluge:  nk_deluserkey() not supported on UNIX\n");
}


extern void nk_adduserkey(perm, keynum, keymod, proc, ukr)
int perm, keynum, keymod;
_PROCEDURE proc;
nk_userkeyrec *ukr;
{
  fprintf(stderr, "NEWKBD kluge:  nk_adduserkey() not supported on UNIX\n");
}


extern void nk_ungetkey(c)
uchar c;
{
  XEvent event;
  char buf[2];

  event.type = KeyPress;
  event.xkey.display = m_display;
  event.xkey.window = m_window;
  buf[0] = c;
  buf[1] = '\0';
  event.xkey.keycode = XKeysymToKeycode(m_display, XStringToKeysym(buf));
  XPutBackEvent(m_display, &event);
}

extern void nk_keybufclear()
{
  XEvent event;

  while (XCheckTypedEvent(m_display, KeyPress, &event))
    ;
}
	 

void nk_settransarray(mode, mat)
int mode;
nk_keytransinfo **mat;
{
  static int dummy;

  if (mode == nk_ktsupplied) {
    /* ignore it for now! */
  } else {
    *mat = (nk_keytransinfo *)(&dummy);
  }
}


