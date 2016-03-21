//
// mulmod.c  - perform 64x64 multiply mod 64
//
// $RCSfile: mulmod.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: mulmod.c,v 1.3 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: mulmod.c,v $
// Revision 1.3  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.2  2013/11/29 21:36:54  chuck
// improved performance
// added pthread support
//
// Revision 1.1  2013/11/21 04:23:25  chuck
// Initial revision
//
//
static char *rcs_file_verson = "$Id: mulmod.c,v 1.3 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

//
// local struct/union to facilite multiply
//
  typedef struct {
    uint32_t  lo;
    uint32_t  hi;
  } mont;

  typedef union {
    mont      hilo;
    uint64_t  whole;
  } montgomery;



// mulmod32 relies on the underlying 64 bit os and simply does a * %.  the overhead
// of the function call cannot be avoided at this time since the selection of routines
// is made based on Pmax.
uint64_t mulmod32 (uint64_t a, uint64_t b, uint64_t mod)
{
  return (a * b) % mod;
}

uint64_t mulmod64 (uint64_t a, uint64_t b, uint64_t mod) 
{

  register uint64_t ah, al, bh, bl, ch, cl, chl, clh;
  uint64_t mask = 0xFFFFFFFF;


  a %= mod;
  b %= mod;

#define WANT_ORIGINAL
#ifdef WANT_ORIGINAL

  ah = a >>32;
  bh = b >>32;
  al = a & mask;
  bl = b & mask;
  cl = (al * bl) % mod;
  ch = (ah * bh) % mod;
  ch = (ch <<  32) % mod;
  chl = (ah * bl) % mod;
  ch += chl;
  ch %= mod;
  clh = (al * bh) % mod;
  ch += clh;
  ch %= mod;
  ch = (ch << 32) % mod;
  ch += cl;
  ch %= mod;
  return ch;

#else    // simple folded lines together to help the compiler

  a.whole = A;
  b.whole = B;

  c.whole = (a.hilo.lo * b.hilo.lo) & mod;

  t1.whole = (a.hilo.hi & b*.hilo.hi) & mod;
#endif
  
}


#ifdef MAYBE_FUTURE
uint64_t mulmod64 (uint64_t A, uint64_t B, uint64_t MOD)
{

  // register montgomery   a,b,c,t1,t2;
  register uint64_t     a = A % MOD;
  register uint64_t     b = B % MOD;
  register uint64_t     mod = MOD;

  // load A and B from the stack
  // a.whole = A;
  // b.whole = B;

  // 
  // c.hilo.low = (a.hilo.lo * b.hilo.lo) % mod;

  // t1.whole = (a.hili.hi * b.hilo.hi) % mod;
  // t1.whole = (t1.whole << 32) 

  register uint64_t    ah, al, bh, bl, ch, cl, chl, clh;

  // uint64_t ah, al, bh, bl, ch, cl, chl, clh;
  uint64_t mask = 0xFFFFFFFF;


  // done at initialization :)
  // a %= mod;
  // b %= mod;

  ah = a >>32;
  bh = b >>32;
  al = a & mask;
  bl = b & mask;

                                  // 1 - cl = (al * bl) % mod;
  // ch = (ah * bh) % mod;
  // ch = (ch <<  32) % mod;
  // folded together
  ch = (((ah * bh) % mod) << 32) % mod;

  ch += (ah * bl) % mod;
  ch %= mod;

  ch += (al * bh) % mod;
  ch %= mod;

  ch = ((ch << 32) % mod) + ((al * bl) % mod);          // 1 - +=cl;

  ch %= mod;
  return ch;
}

#endif  // MAYBE_FUTURE  (mixed 32-64 bit register ops via structures/unions)


#ifdef UNIT_TEST
main()
{
  uint64_t  a,b,c;

  uint64_t  C;

  uint64_t  P;

 
  // exhaustively regression test for 64 bits on a, b, and P   (yes it will take a while)

  for (a = 1; a< 0x7FFFFFFFFFFFFFFF; a+=83) {
    printf(".") ; fflush(stdout);
    for (b = 1; b< 0x7FFFFFFFFFFFFFFF; b+=90) {
      for (P = 1; P< 0x7FFFFFFFFFFFFFFF; P+=51) {

        c = mulmod(a,b,P);
        C = mulmod64(a,b,P);

        if (C != c) {
          printf("ERROR: %ul != %ul @ (%ul, %ul, %ul (%x))\n",c,C,a,b,P);
          exit(1);
        }
      }
    }
  }
}
#endif // UNIT_TEST

