#define RND_G

#include <stdlib.h>
#include <p2c/rnd.h>


void P_random(seed)
long *seed;
{
  static short xsubi[3];
  
  srand48(*seed);
  *seed = lrand48();
}


long P_rand(seed, limit)
long *seed;
long limit;
{
  long ret;

  srand48(*seed);
  *seed = lrand48();
  if (limit != 0)
    return(abs(*seed % limit)); /* I know, I know.  This skews probabilities */
			        /* slightly toward low numbers unless limit  */
			        /* is a divisor of 2^32.  Tough.             */
  else
    return(0);
}  
  
