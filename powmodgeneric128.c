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
// $Log: powmodgeneric128.c,v $
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
// Revision 1.2  2013/11/15 21:18:21  chuck
// correct misnamed variable
//
// Revision 1.1  2013/11/15 21:05:06  chuck
// Initial revision
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: powmodgeneric128.c,v 1.5 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include "powmod.h"

uint64_t powmodgeneric128(uint32_t Base, uint32_t Exp, uint64_t Prime)
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
  int64_t   c;
  uint64_t r;
  int i;

  if (ac < 5) {
    printf("usage: %s k b n c p iterations\n",av[0]);
    exit(1);
  }

  // convert arguments
  k = atoi(av[1]);
  b = atoi(av[2]);
  n = atoi(av[3]);
  p = atoi(av[4]);

  
  for (i=0; i< atoi(av[5]); i++); r = powmulmod(k,b,n,p);

  printf("%d^%d | %d = %d\n", b,n,p,r);

  printf("2^2-1 %3 = %d\n", mulmod(1,(2*2-1),3));
  printf("2^3-1 %3 = %d\n", mulmod(1,(4*2-1),3));
  printf("2^4-1 %3 = %d\n", mulmod(1,(8*2-1),3));
  printf("2^4-2 %3 = %d\n", mulmod(1,(8*2-2),3));
  printf("2^4-3 %3 = %d\n", mulmod(1,(8*2-3),3));
}

#endif // UNIT_TEST
