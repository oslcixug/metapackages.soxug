#ifndef MARKRELEASE_H
#define MARKRELEASE_H

#ifdef MARKRELEASE_G
# define vextern
#else
# define vextern extern
#endif

extern void mark();
extern void release();
extern char *fakemalloc();

#define malloc(x) fakemalloc(x)

#undef vextern

#endif
