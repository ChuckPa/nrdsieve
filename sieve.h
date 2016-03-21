//
// sieve.h  - for generalized sieve of the form:  b^n - b^k - c 
//
// 4-Oct-2013:  C. Lasher 
// 
// ChangeLog:
//
// $Log: sieve.h,v $
// Revision 1.8  2013/11/30 17:57:15  chuck
// Updated for multiple thread support, new options, and as part of complete overhaul
//
// Revision 1.7  2013/11/21 04:18:41  chuck
// added new bitmap structure, threads, and mulmod
//
// Revision 1.6  2013/11/15 21:18:36  chuck
// entering primegen.h into the library
//
// Revision 1.5  2013/11/11 03:06:37  chuck
// Corrected enumeration mismatch and centralized definition into one module.
//
// Revision 1.4  2013/11/10 18:06:30  chuck
// Cleaned up things no longer needed
//
// Revision 1.3  2013/11/08 03:50:47  chuck
// Changed 64 bit B,N,K to 32 bits for speed.  P and all math still done in 64/96 bits.
//
// Revision 1.2  2013/10/21 06:11:09  chuck
// Added RCS header info
//
//

#ifndef _HAVE_SIEVE_H_
#define _HAVE_SIEVE_H_	// inclusion control

#include <inttypes.h>

#ifndef int64
#define int64 int64_t
#endif

// Brute Force but keeps it out of memory
#define AutosaveDefault 10              // 10 minutes (console updates every minute)

  // Input, output and logfiles plus their affiliated variables

  char          InputFilename[512];	// Sieve input file
  char          OutputFilename[512];	// Sieve output file
  char          LogFilename[512];	// Where we log the good stuff
  char          DelFilename[512];       // Where we log the deleted candidates 

  FILE         *Input, *Output, *Log;

  // Auto-save Timer
  int           Autosave;               // Number of intervals (minutes) between saves

  // Runtime options 
  int		Resume;			// resuming a previous sieve
  int		Quiet;			// minimal (terse) output
  int		Help;			// command line option help requested
  int		New;			// true if new sieve specified

  // Exit flag (signalled by Ctrl-C or other means)
  int           ExitSieve;

  // Logging and output flags
  int           LogDelete;
  char         *DeleteFile[512];

  int           Verbose;


  // Global operational runtime flags
  int           DeltaP;
  int           NeedStats;
  int           NeedSave;
  int           NumThreads;

  // If cache misses / overrun is a problem reducing this to a byte will improve at the cost
  // of the type of instruction used to access it (and any associated penalty)
  typedef   unsigned char type_k;

  // Sieve variables & candidate lists
  int    	B;	   // the base (b)
  int    	N;	   // the exponent (n)
  int           Kmin;      // Starting K
  int           Kmax;      // Ending K
  type_k       *Kbitmap;   // the 'bitmap' list of candidate K's.
  int		Ksize;	   // the number of elements in Kbitmap;
  int    	C;         // Constant C to subtract (unless it's negative)
  int64         Pmin,Pmax; // Where to start & stop sieving at (P)
  int64         LastP;     // where we last stoped (or 0 if a fresh sieve)
  int          *Ksremoved; // How many K's have been removed during this run (per thread)

// Function Prototypes




#endif // _HAVE_SIEVE_H_
