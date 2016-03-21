//
// math.c    -- compute all factors (both prime and composite) of P-1 (PM1)
//           -- compute Jacobi symbol and other functions as required
//           -- Support functions:  sieve(), initfactors(), nextfactor() are here
//
//
// $RCSfile: math.c,v $
//
// $Date: 2013/12/11 22:04:58 $
//
// $Author: chuck $
//
// $Id: math.c,v 1.1 2013/12/11 22:04:58 chuck Exp chuck $
//
//
// ChangeLog:
//
// $Log: math.c,v $
// Revision 1.1  2013/12/11 22:04:58  chuck
// Initial revision
//
//
//

static char *rcs_file_verson = "$Id: math.c,v 1.1 2013/12/11 22:04:58 chuck Exp chuck $";

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "limits.h"
#include "math.h"


// Static variable for first-time initialization
static int       First = 1;            // will clear after we are ready
                                       // otherwise calculate a new factor list for given P

//
// Eratosthenes() -- Initialize the list of prime numbers for factoring using Sieve of
//                Eratosthenes.
//
void eratosthenes()
{

    unsigned char bits[1<<PRIMEBITS];  // PRIMEBITS must be even
    int i,j;

    // Presume all candidates are primes
    memset(bits,1, 1 << PRIMEBITS);

    // manually clear 0 & 1, they aren't primes
    bits[0] = bits[1] = 0;

    // walking up to 2^PRIMEBITS/2, clear out the lower half
    for (i=0; i< 1<<PRIMEBITS/2; i++) {

      // if the this bit is set, clear every bit at a power of this above above here
      if (bits[i]) 

        // clear every bit from this bit^2 up, on 'bit' boundaries
        for (j= i*i; j< 1<<PRIMEBITS; j += i)  bits[j] = 0;
    }

    // All done sieving, now allocate the memory and collect into the list we need
    Primes = (uint64_t *) calloc(1,sizeof(uint64_t)*MAXPRIMES);

    // That's it.  Sieve is done.  Put them into an ordered list for fast, easy use
    for ( i = j = 0; i< 1<<PRIMEBITS; i++) 

      if ( bits[i]) Primes[j++] = i;

    // transfer the count of primes in Primes
    NumPrimes = j;

    First = 0;
}

//
// GetPrimeFactors() -  Compute the prime factorization and return it, with a count
//
// Note: using 64 bit int for the prime factors list is overkill but consistency rules
//

int getprimefactors(uint64_t N, PrimeFactor *List)
{
  int        length = 0;
  uint64_t   p;
  int        e;
  int        i;


#ifdef ORIG_PF
  for (i = 0; i < NumPrimes; i++) {
    p = Primes[i];
    if (p * p > N) break;
    for (e = 0; !(N % p); N /= p, e++);
    if (e) {
      List[length].p = p;
      List[length++].e = e;
    }
  }

  return N == 1 ? length : (List[length].p = N, List[length].e = 1, ++length);

#else

  for (i = 0; i < NumPrimes; i++) {
    p = Primes[i];
    if (p * p > N) break;
    if (!(N % p)){
      List[length].p = p;
      List[length].e = 1;
      N /= p;
      while(!(N % p)) {
        List[length].e++;
        N /= p;
      }
      length++;
    }  
  }

  // Is it completely factored or must we include the 'residue' (too big for us)
  if (N != 1) {
    List[length].p = N;
    List[length].e = 1;
    length++;
  }

  return length;

#endif
}

// 
// uint64Compare -- used to return -1, 0, or 1 by qsort()
//
int uint64Compare(const void *a, const void *b)
{
  return *(const uint64_t *)a < *(const uint64_t *)b 
                              ? -1 : *(const uint64_t *)a > *(const uint64_t *)b;
}

//
// GetFactors -- Factor the given N (usually P-1) and factor it the best we can (2^PRIMEBITS)
//

int getfactors(uint64_t N, uint64_t *List)
{
  int           NumFactors,Length, Length2, i,j,k;
  uint64_t      p;                  // while not likely, this could exceed 2^32 so use 64bits
  PrimeFactor   Factors[MAXFACTORS];

  // First get the list of prime factors for N
  NumFactors = getprimefactors(N, Factors);   // Factors will contain the factors on return

  // Initialize both lists and put 1 as the first factor
  Length2 = Length = List[0] = 1;

  // Now do the expansion:   L = (1); L = (L, L * p**(1 .. e)) forall((p, e))
  for (i=0; i< NumFactors; i++, Length2 = Length)

    // for each exponent of this factor, performan manual exponentiation
    for (j=0, p = Factors[i].p; j < Factors[i].e; j++, p *= Factors[i].p)

      // and multiply by everything on the list, creating the next block of factors
      for (k=0; k < Length2; k++)

        List[Length++] = List[k] * p;


  // Now let qsort put them in numerical order
  qsort(List, Length, sizeof(uint64_t), uint64Compare);

  // return how many we calculated
  return Length;
}

//
// Order() -- Compute multiplicative order under multiplication mod P
//
//   Note:  This implementation differs from the traditional by supporting an artificial
//          limit (if non-zero).  Order will not attempt search above 'limit' and return
//          zero (0), indicating not-found/no-solution.
//
//   Note:  This implemention supports multi-threading because all information is contained
//          on the stack.  The downside of this is subsequent calls for the same P will
//          return the same initial results.  Native multi-thread support will be integrated
//          permitting subsequent calls using the same P value to return the 'next' order.
//
uint64_t order(int B, uint64_t P)
{
  uint64_t      i;
  uint64_t      Factors[MAXFACTORS];
  int           NumFactors = 0;
  uint64_t      Pminus1 = P-1;
  int           order = 0;

  // very straight forward:  Get the factors of P-1 and look for B^order %P == 1
  NumFactors = getfactors(Pminus1,Factors);  

  // now test 
  for (i=0; i<NumFactors; i++) {

    // Does B^Factor % P == 1?
    if ( powmodgeneric(B,Factors[i],P) == 1)  return Factors[i];

  }

  // Fell out, No order found at this P
  return 0;
}
