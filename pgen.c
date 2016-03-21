//
// pgen.c  -- support and interface to primegen library with multi-threaded applications
//
// Usage:
//          These functions perform the actual call to primegen_next, primegen_skipto,
//          and primegen_init appropriately.
//
//          Secondarily, as a caller 'posts' it's completion of the 'P' it previous
//          retrieved, these routines perform the necessary housekeeping to properly
//          update 'LastP' if appropriate and 'fold' any pending 'posts' which arrived before
//          it into LastP as well.  This eliminates the need to have a seperate rendezvous
//          thread, which is itself a problem in async multi-threading.
//
// $RCSfile: pgen.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: pgen.c,v 1.3 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: pgen.c,v $
// Revision 1.3  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.2  2013/11/30 17:55:50  chuck
// Wrapper for primegen library for use with pthreads.  Primegen is single-threaded only
//
// Revision 1.1  2013/11/29 21:46:08  chuck
// Initial revision
//
//
static char *rcs_file_verson = "$Id: pgen.c,v 1.3 2013/12/02 19:16:57 chuck Exp $";
//

// Special 'internal' flag for pgen.h
#define _am_pgen_internals_h_


// global includes
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <inttypes.h>

// local includes
#include "limits.h"
#include "primegen.h"
#include "pgen.h"

// include Sieve.h for 'LastP' and 'DeltaP' ntegration only
// #include "sieve.h"
uint64_t LastP;
int      DeltaP;

// pgeninit() - initialize the Pgen buffers for keeping track of 'NumThread' threads of P
//
PgenInit( Threads )
  int     Threads;
{

  int        i,j;
  uint64_t  *P;

  
  // call primegen and init PG 
  primegen_init(&PG);

  // validate the Number of threads requested (these are 'worker threads')
  if (Threads < 1 || Threads > (MAXTHREADS-1)) return (-1);

  // initialize the Pgen tide markers
  for (i=0; i<MAXTHREADS; i++)  Lowtide[i] = Hightide[i] = 0;

  // Remember how many threads were initialized / activated
  PgenNumThreads = Threads;

  // return now many we created 
  return (Threads);
}


// pgenskipto() - Skip to the P specified
//
int PgenSkipto(uint64_t Skipto)
{

  // Acquire the lock
  pthread_mutex_lock(&PgenLock);

  // Skip ahead
  primegen_skipto(&PG, Skipto);

  // release the lock
  pthread_mutex_unlock(&PgenLock);

}

//
// PgenPause - Simply Pause all processing by pausing Pgen
// 
int PgenPause()
{
  
  // Acquire the lock
  pthread_mutex_lock(&PgenLock);

}

//
// PgenResume() - Unlock Pgen so processing can resume
//
int PgenResume()
{
  // Release the lock
  pthread_mutex_unlock(&PgenLock);

}

//  PgenDone() - Report this thread's P as done but do not ask for another
//
//


int PgenDone(int Tid)
{

  // Move Hightide -> Lowtide
  Lowtide[Tid] = Hightide[Tid];

  // and return
  return;

}

uint64_t PgenNext(int Tid)
{

  // The P value we will return
  register uint64_t       P;


  // Move HighTide -> LowTide
  Lowtide[Tid] = Hightide[Tid];

  // Acquire the mutex 
  pthread_mutex_lock(&PgenLock);

  // Get and account for P;
  Hightide[Tid] = P = primegen_next(&PG);

  //  Increment P-acquired counter
  DeltaP++;

  // Release the mutex and set pointers
  pthread_mutex_unlock(&PgenLock);

//printf("Tid %d gets %d\n",Tid,P);

  // and return
  return (P);
}

//
// PgenLowtide -- Return the lowtide of all lowtides currently active
//                (called by statsworker and savedata only)

uint64_t PgenLowtide()
{
  uint64_t tide = SIEVELIMIT;
  int i;

  for (i=0; i<MAXTHREADS; i++) if (Lowtide[i] && (Lowtide[i] < tide)) tide = Lowtide[i];

  LastP = tide;
  return tide;
}
