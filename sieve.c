//
// $RCSfile: sieve.c,v $  - sieve for prp of the general form b^n - b^k - c
//
// $Date: 2013/12/11 22:04:58 $
//
// $Author: chuck $
//
// $Id: sieve.c,v 1.16 2013/12/11 22:04:58 chuck Exp chuck $
//
// ChangeLog:
//
// 04-Oct-2013  - Initial dev version
//
// $Log: sieve.c,v $
// Revision 1.16  2013/12/11 22:04:58  chuck
// Implemented multiplicative order test at top of loop to skip obvious no-solution P
//
// Revision 1.15  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.14  2013/11/30 18:04:04  chuck
// change to mulmod / mulmod32 / mulmod64.  Now quietly assigns.
//
// Revision 1.13  2013/11/29 21:50:55  chuck
// COMPLETELY overhauled main sieve module.
// New sieve creation now in newsieve.c
// Sieve setup, whether newsieve or existing sieve reload, now in sievesetup.c
// Sieve 'work' now done in sieveworker.c
// Added pthread support (launch and join/recovery)
// improved performance of 32/64 bit mulmod by using arithmetic-and trigger
// Changed array references to pointers & pointer math (addition) were cost effective
// Main sieve now 'thread 0' & default thread if no -t defined or -t 1 (1 thread) defined
// All Stats & save logic kept here.
// Many other smaller 'tweaks'
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
static char *rcs_file_verson = "$Id: sieve.c,v 1.16 2013/12/11 22:04:58 chuck Exp chuck $";
//

// global includes
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


// Force undefine specific flags
#undef INLINE_MULMOD

// local includes
#include "limits.h"
#include "sieve.h"     			// global data contained here
#include "powmod.h"
#include "primegen.h"
#include "loadsaveini.h"
#include "option.h"
#include "signals.h"
#include "threads.h"


// Manual function prototype to handle issue I can't remember how to without embedding .h's
   extern void sieveworker(ThreadInfo *);



// Actual 'do autosave' function (can't put printf inside a signal handler)
void autosaveworker()
{

//  printf("Hello from AUTOSAVEWORKER\n");

  // Pause Pgen so we can safely save data
  PgenPause();

  // Give everything a chance to go idle
  sleep(1);

  // Update LastP;
  LastP = PgenLowtide();

  // simply save the data
  savedata();

  // and the ini
  saveini();

  // clear the NeedSave flag
  NeedSave = 0;

  // Now resume
  PgenResume();

}

static int StatsSamples = 1;

int statsworker()
{
    int Removed,i;


    // Sum the number of K removed in total
    Removed = 0;
    for (i=0; i<NumThreads; i++) Removed += Ksremoved[i];

    // Update LastP by calling PgenLowtide() and checking
    LastP = PgenLowtide();

    // output stats (temp) of how many removed
    printf("P=%lu, %d p/sec, factors found: %d (%d/min avg)",
               LastP,DeltaP/60,          Removed,  (Removed/StatsSamples));

    // clear the line to EOL
    printf("%c[K\r", 27);
    fflush(stdout);


    // Clear the stats related flags
    StatsSamples++;
    DeltaP = 0;
    NeedStats = 0;

}

void saveandexit()
{

  printf("\nCtrl-C received, saving K's\n");

  // Save the data then exit
  savedata();

  // Save the ini
  saveini();

  printf("\nExiting.\n");
  exit(0);
}


main(int ac, char* av[])
{

  int     i;

  // Initialze core variables before loading
  ExitSieve = Quiet = Resume = New = Help = LogDelete = Verbose = 0;

  // Set our auto-save timer (default  interval = 10 min (600 sec))
  Autosave = AutosaveDefault;

  // Initialize our Stats and Save flags
  AutosaveInterval = 0;   // which 'tick' of the autosave count are we at
  DeltaP = 0;             // How many P's did we do.  Incremented each time P is changed.
  NeedSave = 0;           // We don't need to save the data yet
  NeedStats = 0;          // We don't need to print stats yet

  // No set limit for sieve unless specified
  Pmax = 0;
  Pmin = 0;
  LastP = 0;

  // get command line arguments (if any) and do what we can
  // Options override ANYTHING that was specified in the ini (if it's applicable)
  getoptions(ac,av);


  // If help requested, display help then exit
  if (optHelp) {
    help();
    exit(1);
  }


  // Have sievesetup perform all option handling and data loading or new sieve creation
  sievesetup(ac,av);


  // Default to no additional threads (single threaded operation)
  NumThreads = 0;

  // Were multiple threads requested?  If so, validate how many
  if (optThreads) {

    if (optNumThreads >= MAXTHREADS) {
      printf("Warning: Number of requested threads (%d) exceeds max (%d). Reduced.\n",
              optNumThreads, MAXTHREADS-1);
      optNumThreads = MAXTHREADS;
    }

    if (optNumThreads < 1) {
      printf("Warning: Number of requested threads (%d) invalid.  Defaulting to 1 thread\n");
      optNumThreads = 1;
    }

    NumThreads = optNumThreads;
      
  }

  // If no threads requested, default to 1 (main thread only)
  if (!NumThreads) NumThreads = 1;

  // Initialize Kremoved stats counters
  Ksremoved = calloc(1,sizeof(int)*(NumThreads+1));

  // Establish signal handlers
  InitSignals();

 
// =====================================================================
// ----  All data loaded and flags set.  Ready to begin processing. ----
// =====================================================================

  // Initialize Pgen
  PgenInit(NumThreads);

  // Initialize Eratosthenes
  eratosthenes();

  // BEFORE we start, are we picking up at a previous P?  if so, skipto it
  if (LastP) {
    if (!Quiet) printf("Resuming sieve at P = %lu\n",LastP);
    PgenSkipto(LastP);
  }

//  // Set mulmod appropriately (it will dynamically adjust)
//  mulmod = mulmod32;
//  if (LastP & SIEVEMASK32) mulmod = mulmod64;


  // Start each thread
  for (i=1; i< NumThreads; i++) ThreadStart(sieveworker,(void *)0 );

  // Now make myself thread 0 (the only thread guaranteed to run)
  Tcb = ThreadControlBlock;
  Tcb[0].Number = 0;
  Tcb[0].State  = ThreadRun;
  sieveworker(&Tcb[0]);


  // We have returned from the worker, so sieving must be done or interrupted

  // Make sure all threads have exited by asking nicely
  for (i=0; i<NumThreads; i++) ThreadFini(i);

  // Wait for them to finish
  usleep(200000);  // give them 200 milliseconds to clean up

  // join and get the returns
  for (i=1; i<NumThreads; i++) ThreadJoin(i);

  // Now perform all final save & exit
  printf("\nSieve Completed. P = %lu\n",LastP);

  // Save the data
  savedata(); 

  // Save the ini
  saveini();

  // Done
  printf("\nExiting.\n");
  exit(0); 
}
