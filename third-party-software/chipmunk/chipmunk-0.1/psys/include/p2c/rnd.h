#ifndef RND_H
#define RND_H

#ifdef RND_G
# define vextern
#else
# define vextern extern
#endif


extern void P_random();
extern long P_rand();

#undef vextern

#endif /* RND_H */
