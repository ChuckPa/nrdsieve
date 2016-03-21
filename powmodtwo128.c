//
//
//  powmodtwo128.c  --- support optimizations for Base 2 primes (2^n, etc) in 128 bits
//
//  Simple routine to perform integer modular exponentiation in a deterministic manner
//  using successive squaring in binary for arbitary size base & exponent 
//  Results only bounded by the bitwidth of 'P'.  (64 bits w/ 128 bit temp during multiply)
//
//  15-Nov-2013, C. Lasher
//
//  Initial version:  Copied from existing powmodtwo.c (64 bit native) and promoted
//
// ChangeLog:
//
// $Log: powmodtwo128.c,v $
// Revision 1.5  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.4  2013/11/21 04:27:00  chuck
// Created new 128 bit gcc native routine using __int128 type for 64x64 bit math.
// Use only in the absense of SSE / CMOV routines where speed is not a factor
//
// Revision 1.3  2013/11/16 04:56:40  chuck
// Added early out and expression folding for performance
//
// Revision 1.2  2013/11/15 21:18:00  chuck
// correct misnamed variable
//
// Revision 1.1  2013/11/15 20:57:25  chuck
// Initial revision
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: powmodtwo128.c,v 1.5 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "powmod.h"

uint64_t powmodtwo128(unsigned int Base, unsigned int Exp, uint64_t Prime)
{
  register uint64_t  res = 1;
  register uint64_t  B = Base;  // get the base off the stack;
  register uint64_t  E = Exp;   // get the exponent off the stack;
  register uint64_t  P = Prime; // get the prime (modulus) off the stack

  // new type for 128 bit int
  __int128 verylong = 1;


  // loop through each bit of the exponent until all bits done
  while (E) {

    // If this bit is set, include b by multipy/mod.
    if ( E & 0x01 ) verylong = (verylong * B) % P;

    // Square the base and take the mod for the next bit position
    B = (B * B) % P;

    // if the res is 0, we're done
    if (!verylong) return 0;

    // Shift right
    E >>= 1;
  }

  res = verylong & 0xFFFFFFFFFFFFFFFFUL;
  return (res);

}

#ifdef UNIT_TEST
main(int ac, char **av)
{
  uint64_t  k,b,n,p;
  uint64_t  c;
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
