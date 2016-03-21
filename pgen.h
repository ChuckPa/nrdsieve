//
// pgen.h -- global variables & function definitions for pgen.c.
//
//           Some variables used in Pgen's functions are not exposed to avoid potential
//           conflicts.  
//
// $RCSfile: pgen.h,v $
//
// $Date: 2013/11/29 21:47:23 $
//
// $Author: chuck $
//
// $Id: pgen.h,v 1.1 2013/11/29 21:47:23 chuck Exp $
//
//
// ChangeLog:
//
// $Log: pgen.h,v $
// Revision 1.1  2013/11/29 21:47:23  chuck
// Initial revision
//
//
//

#ifndef _havepgenh_
#define _havepgenh_


  // PgenInit() -- Initialize the Pgen multi-thread safe layer
  int PgenInit();

  // PgenSkipto() -- Skip ahead from the current P to the supplied P
  int PgenSkipto(uint64_t);

  // PgenNext(Tid) -- Using the caller's Thread Id (Tid), mark the currently 'pending'
  //                  P for that thread as done and issue the next in line to do.
  //                  ALSO:  Perform any housekeeping which is needed to keep sync.
  uint64_t PgenNext(int);

  // PgenLowtide() -- Return the lowest P known to be complete of all running threads
  uint64_t PgenLowtide();

#ifdef _am_pgen_internals_h_

  // Pgen's single thread control mutex
  static pthread_mutex_t  PgenLock = PTHREAD_MUTEX_INITIALIZER;

  // The PG counting / maintaining instance;
  static primegen         PG;

  // How many threads have been initialized for (this must be >= number of real threads)
  static int              PgenNumThreads;

  // Pgen 'lowtide' (completed) values
  static uint64_t         Lowtide[MAXTHREADS];

  // Pgen 'hightide' (pending) values
  static uint64_t         Hightide[MAXTHREADS];

#endif // _am_pgen_internals_h_

#endif // _havepgenh_
