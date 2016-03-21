//
// $RCSfile: sieveworker.c,v $  - sieve for prp of the general form b^n - b^k - c
//
// $Date: 2013/12/11 22:04:58 $
//
// $Author: chuck $
//
// $Id: sieveworker.c,v 1.5 2013/12/11 22:04:58 chuck Exp chuck $
//
// ChangeLog:
//
// 04-Oct-2013  - Initial dev version
//
// $Log: sieveworker.c,v $
// Revision 1.5  2013/12/11 22:04:58  chuck
// Implemented multiplicative order test at top of loop to skip obvious no-solution P
//
// Revision 1.3  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.2  2013/11/30 18:08:29  chuck
// Portion of original sieve.c which is used for multi-threaded work.
// All prior sieve.c log entries transferred to here.
//
// Revision 1.12  2013/11/21 04:27:00  chuck
// Major changes implemented.
// a.  candidates changed to bitmap from linked list from array.
// b.  implementation of Mark, Dot, and Peek (skip over) true sieving logic based on res.
// c.  implemented newsieve() export.  New sieves are now setup in function.
// d.  isolation of sieve logic in prep for relocation into
// e.  interrupt tasks no longer perform i/o.  Only set flags and let main logic handle it per standards
// f.  runtime selection of mulmod() based on Pmax / MaxP requested.
//
// Revision 1.11  2013/11/17 01:15:52  chuck
// checking in preparing for K list overhaul
//
// Revision 1.10  2013/11/16 17:08:49  chuck
// VT-100 escape code EL0.
// Alarm task flag order changed to prevent possible loss of sieve data during concurrent interrupts.
//
// Revision 1.9  2013/11/16 06:35:39  chuck
// FINALLY corrected losing OutputFilename when optOutput specified.
//
// Revision 1.8  2013/11/16 04:59:04  chuck
// removed i/o from signal handlers (never should have been here)
//
// Revision 1.7  2013/11/15 21:18:36  chuck
// Rewrite of new sieve creation logic, sieve resume logic, auto-save/update,
// and added support for linked-link candidates (major speed increase)
// and 128 bit multiplies (for large P>2^32 which isn't really that big)
//
// Revision 1.6  2013/11/11 03:17:52  chuck
// fixed reference to gotint -> askint()
//
// Revision 1.5  2013/11/11 03:06:37  chuck
// Corrected enumeration mismatch and centralized definition into one module.
//
// Revision 1.4  2013/11/10 05:35:33  chuck
// reconstructed initialization code from printouts after source lost
//
// Revision 1.3  2013/11/10 01:01:29  chuck
// duplicate load/save removed
//
// Revision 1.2  2013/11/10 00:58:26  chuck
// fixed askyn() reference.. sb: askyesno()
//
// Revision 1.1  2013/10/21 05:59:46  chuck
// Initial revision
//
static char *rcs_file_verson = "$Id: sieveworker.c,v 1.5 2013/12/11 22:04:58 chuck Exp chuck $";
//

// global includes
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


// local includes
#include "limits.h"
#include "sieve.h"                      // global data contained here
#include "powmod.h"
#include "primegen.h"
#include "loadsaveini.h"
#include "option.h"
#include "signals.h"
#include "threads.h"

// WARNING:  MANUAL PROTOTYPE Order()
uint64_t order(int, uint64_t);

// =========================== WARNING:  Inline mulmod =============================

#ifdef INLINE_MULMOD
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
static inline uint64_t mulmod32 (uint64_t a, uint64_t b, uint64_t mod)
{
  return (a * b) % mod;
}

//
// mulmod64 relies on the underlying 128 bit quasi-intrinsic type provided by gcc 4.8.2+
//
//
static inline uint64_t mulmod64 (uint64_t a, uint64_t b, uint64_t mod)
{

   register montgomery A,B;
   register uint64_t t;

   unsigned __int128   T;

   T =  ( a * b) % mod;
   t = T & 0xFFFFFFFFFFFFFFFF ;
   return t;
}

// now the strange one, inline 'selector' (I *HATE* doing this!)
uint64_t Plarge;

static inline uint64_t mulmod(uint64_t a,uint64_t b,uint64_t p)
{
  if (Plarge) return mulmod64(a,b,p);
  else        return mulmod32(a,b,p);
}

#endif // INLINE_MULMOD


// =========================== END: inline mulmod ==================================

void sieveworker(ThreadInfo *Tcb)
{
  int     Tid;

  int     i;
  uint64  kres, nres;
  uint64  NresMinusC, Kres;
  uint64  MarkRes, DotRes;  // Incremental residues computed as exponents increment
  uint64  P;
  uint64  R;  // Multiplicative order under multiplication (mod P)

  // Indicies and speed pointers to those locations
  type_k *Kmark, *Kdot, *Kpeek;
  int     Mark,   Dot,   Peek,   Skip;   // The serious sievers :)

  int     Kmid = (Kmax + Kmin)>>1 + 1;       // mid K (favor being 1 above vs premature kill)

  // weird use of a register.. maintain a pointer to a function
  // uint64_t (*mulmod)(uint64_t, uint64_t, uint64_t);


  // Keep track of how many K's removed during this run
  int *Kremoved;


  // Who am I ?
  Tid = Tcb->Number;

  // Initialize my Kcounter
  Kremoved = &Ksremoved[Tid];
  *Kremoved = 0;

  // Initialize (*mulmod)() -- relocated from sieve.c
  // Set mulmod appropriately (it will dynamically adjust)
  //mulmod = mulmod32;
  //if (LastP & SIEVEMASK32) mulmod = mulmod64;
  Plarge = LastP & SIEVEMASK32;


  // Start with a dummy P
  P = 1;

  // Run until it's time to exit
  while (P <  Pmax) {

    // Again, I *HATE* this!
    Plarge = P & SIEVEMASK32;

    // Run at least 1 P value, which might be just over the limit but
    // better than being under
    P = PgenNext(Tid);

    // Since we routinely sieve small P values, run with faster math as long as possible
    // Are we over the bit limit of a 32 bit operand?

    // next no longer required (Plarge handles this)
    //if ( P & SIEVEMASK32 ) mulmod = mulmod64;


    // compute N's res at this P
    nres = powmod(B,N,P);
    NresMinusC = nres - C;

    // WARNING:  This is a hack until I restructure it!
    R = order(B,P);

    // if R != 0 and res^R % P != 1  SKIP EVERYTHING at this P as there are no solutions
    if ( R && powmod(NresMinusC,R,P) != 1) goto SkippedEverything;


    // Start at the beginning (Mark and Dot are the bitmap index, not the 'true' K value)
    Mark  = 0;
    Dot   = 0;
    Skip  = 0;
    Kmark = Kbitmap;

    // Compute the residue long hand ONE time this P
    MarkRes = powmod(B,Kmin,P);

    // While within the bounds of our data
    while ( (Mark < Ksize) && (Dot < Ksize) ) {

      if ( *Kmark ) {

        // If Nres - Kres - C == 0 we have a divisor
        if (NresMinusC == MarkRes) {

          // Delete Mark
          *Kmark = 0;

          if (LogDelete)  writePdivKN(P,Mark+Kmin,N);
          (*Kremoved)++;

          // Now search for 'Dot' if 
          Dot = Mark + 1;
          Kdot = Kmark + 1;


          // If we are below the midpoint, there is the possibility of Dot existing
          if (Dot < Kmid ) {


            // Increment and go looking

#ifdef MULMOD
            DotRes = mulmod(MarkRes,B,P);
#else
            DotRes = MarkRes * B;
            DotRes %= P;
#endif

            while ( Dot < Ksize ) {

              if ( *Kdot ) {

                // If this residue matches, we found our 'dot'
                if (NresMinusC == DotRes) {

                  // we found our 'dot' so compute 'skip'
                  Skip = Dot - Mark;

                  // Now delete Dot
                  *Kdot = 0;

                  if (LogDelete) writePdivKN(P,Dot+Kmin,N);
                  (*Kremoved)++;

                  // Lastly, let's do 'The Sieve Thing' and see what else shakes out
                  Peek = Dot + Skip;
                  Kpeek = Kdot + Skip;

                  while (Peek < Ksize) {

                    if (*Kpeek) {

                      // Delete this one too because it's a multiple
                      *Kpeek = 0;

                      if (LogDelete) writePdivKN(P,Peek+Kmin,N);

                      (*Kremoved)++;
                    }

                    // Step to the next multiple
                    Peek += Skip;
                    Kpeek += Skip;

                  }

                }
              }

              // Advance Dot and keep looking
              Dot++;
              Kdot++;

#ifdef MULMOD
              DotRes = mulmod(DotRes,B,P);
#else
              DotRes *= B;
              DotRes %= P;
#endif
            }
          }

          // Dot not possible or found nothing and we are above the top, so stop searching
          Dot = Mark = Ksize + 1;
        }
      }

      // Now step to the next K
      Mark++;
      Kmark++;

#ifdef MULMOD
      MarkRes = mulmod(MarkRes,B,P);
#else
      MarkRes *= B;
      MarkRes %= P;
#endif

    }

// Hack return point (goto label).. There is no solution at this P so we skipped it

SkippedEverything:

    // If an interrupt is asking us to exit
    if (ExitSieve) {

      // Mark this P as done
      PgenDone(Tid);

      // If we are the main thread (0), simply return
      if (Tid) ThreadExit(&Tid);

      // else
      return;     

    }

    // Handle stats and autosave if we are the main thread
    if (!Tid) {

      // Handle stats & saves here (before we advance)
      if (NeedSave) autosaveworker();
      else if (NeedStats) statsworker();
    }


#if 0   // this test is useless here now that Kremoved is being totaled elsewhere
    // Have we removed all the K's?  DARN! 
    if (Kremoved >= Ksize)  {

      // Yes, print value, force clear and write empty output file
      printf("\nAll values removed!\n");
      printf("Sieve Completed. P = %lu\n",P);
      savedata(); saveini();
      exit(0);

    }
#endif


  }

  // If we are exiting between ticks and my progress (P) exceeds LastP, update it
  if (P > LastP) LastP = P;

  // Normal exit/return, range complete
  if (Tid) ThreadExit(&Tid);

  return;
}
