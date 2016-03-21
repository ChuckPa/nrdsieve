//
//
//  powmod.c
//
//  Simple routine to perform integer modular exponentiation in a deterministic manner
//  using successive squaring in binary for arbitary size base & exponent 
//  Results only bounded by the bitwidth of 'b', 'e', & 'P'.  (64 bits here)
//
//  17-Sep-2013, C. Lasher
//
// ChangeLog:
//
// $Log: powmodgeneric.c,v $
// Revision 1.6  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.5  2013/11/21 04:27:00  chuck
// changed to right->left walk.  More logical and uses fewer instructions (faster)
//
// Revision 1.4  2013/11/16 04:56:40  chuck
// Added early out and expression folding for performance
//
// Revision 1.3  2013/11/15 21:01:28  chuck
// Correct algorithm flaw (incorrect squaring order).
// Updated names to reflect companion 128 bit implementation.
// Moved all operations to registers where possible.
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: powmodgeneric.c,v 1.6 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "powmod.h"

//uint64_t mulmod(uint64_t a, uint64_t b, uint64_t p)
//{
//    return ( ((a%p)*(b%p))%p );
//}

uint64_t powmodgeneric(unsigned int Base, unsigned int Exp, uint64_t Prime)
{

  register uint64_t  res = 1;
  register uint64_t  B = Base;  // get the base off the stack;
  register uint64_t  E = Exp;   // get the exponent off the stack;
  register uint64_t  P = Prime; // get the prime (modulus) off the stack


  // loop through each bit of the exponent until all bits done
  while (E) {

    // If this bit is set, include b by multipy/mod.
    if ( E & 0x01 ) res = (res * B) % P;

    // Square the base and take the mod for the next bit position
    B = (B * B) % P;

    // if the res is 0, we're done
    if (!res) return 0;

    // Shift right
    E >>= 1;
  }

  return (res);
}

#ifdef UNIT_TEST
main(int ac, char **av)
{
  uint64_t  k,b,n,p;
  int64_t   c;
  uint64_t r;
  int i;

  if (ac < 4) {
    printf("usage: %s k b n c p\n",av[0]);
    exit(1);
  }

  // convert arguments
  k = atoi(av[1]);
  b = atoi(av[2]);
  n = atoi(av[3]);
  p = atoi(av[4]);

  
  r = powmodgeneric(b,n,p);
  printf("%d^%d | %d = %d\n", b,n,p,r);

  printf("2^2 %3 = %d\n", powmodgeneric(2,2,3));
  printf("2^3 %3 = %d\n", powmodgeneric(2,3,3));
  printf("2^4 %3 = %d\n", powmodgeneric(2,4,3));
  printf("2^5 %3 = %d\n", powmodgeneric(2,5,3));
  printf("2^6 %3 = %d\n", powmodgeneric(2,6,3));
}

#endif // UNIT_TEST
