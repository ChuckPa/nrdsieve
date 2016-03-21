//
//
//  powmodtwo.c  --- support optimizations for Base 2 primes (2^n, etc)
//
//  Simple routine to perform integer modular exponentiation in a deterministic manner
//  using successive squaring in binary for arbitary size base & exponent 
//  Results only bounded by the bitwidth of 'P'.  (64 bits here)
//
//  11-Oct-2013, C. Lasher
//
//  Initial version:  Lifted from generalized form and optimized
//
// ChangeLog:
//
// $Log: powmodtwo.c,v $
// Revision 1.8  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.7  2013/11/29 21:49:59  chuck
// improved performance by reinstating tweak
//
// Revision 1.6  2013/11/21 04:27:00  chuck
// changed to right->left walk.  More logical and uses fewer instructions (faster)
//
// Revision 1.5  2013/11/16 04:56:40  chuck
// Added early out and expression folding for performance
//
// Revision 1.4  2013/11/15 21:17:44  chuck
// correct misnamed variable
//
// Revision 1.3  2013/11/15 21:00:17  chuck
// Correct algorithm flaw (incorrect squaring order).
// Updated names to reflect companion 128 bit implementation.
// Moved all operations to registers where possible.
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: powmodtwo.c,v 1.8 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "powmod.h"

uint64_t powmodtwo(uint32_t Base, uint32_t Exp, uint64_t Prime)
{

  register uint64_t  res = 1;
  register uint64_t  B = Base;  // get the base off the stack;
  register uint64_t  E = Exp;   // get the exponent off the stack;
  register uint64_t  P = Prime; // get the prime (modulus) off the stack


  // loop through each bit of the exponent until all bits done
  while (E) {

    // If this bit is set, include b by multipy/mod.
    if (E & 0x01 )   res = (res * B) % P;

    // if the res is 0, we're done
    if (!res) return 0;

    // Square the base and take the mod for the next bit position
    B *= B;
    B %= P;

    // Shift right
    E >>= 1;
  }

  return (res);

}

#ifdef UNIT_TEST
main(int ac, char **av)
{
  uint64_t  p;
  uint64_t   b,n;
  uint64_t r;
  uint64_t i;
  char line[80];
  
  printf("b n p? "); fflush(stdout);
  while ( gets(line) > 0 ) {
    sscanf(line,"%d %d %lu", &b, &n, &p);
    r = powmodtwo(b,n,p);
    printf("%d^%d | %ld = %d\n", b,n,p,r);
  }
}

#endif // UNIT_TEST
