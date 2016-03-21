//
// sievesetup.c  -- Perform all setup functions for a new sieve, prepared to run
//
//
// $RCSfile: sievesetup.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: sievesetup.c,v 1.2 2013/12/02 19:16:57 chuck Exp $
//
//
// Revision History:
//
// $Log: sievesetup.c,v $
// Revision 1.2  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.1  2013/11/30 18:06:27  chuck
// Initial revision
//
//
//
static char *rcs_file_verson = "$Id: sievesetup.c,v 1.2 2013/12/02 19:16:57 chuck Exp $";


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

sievesetup(int ac, char **av)
{

  // Initialze core variables before loading
  ExitSieve = Quiet = Resume = New = Help = LogDelete = Verbose = 0;

  // Set our auto-save timer (default  interval = 10 min (600 sec))
  Autosave = AutosaveDefault;

  // No set limit for sieve unless specified
  Pmax = 0;
  Pmin = 0;
  LastP = 0;

  // get command line arguments (if any) and do what we can
  // Options override ANYTHING that was specified in the ini (if it's applicable)
  getoptions(ac,av);

  // if we have a B, K, N, or C specified or NEW, it's a new sieve.
  if (optNew  || optN || optB || optKmin || optKmax || optC ) {

    // if Resume also specified, it's an ambiguity error
    if (optResume) {

      printf("\nError: 'New' and 'Resume' specified together.  Refusing to continue. \n");
      exit(1);

    }


    // If we can't initialize for a new sieve, then we exit
    if (! newsieve()) {

      fprintf(stderr,"\nErrors force termination.  Exiting\n");
      exit(2);
    }


  } else {

    // If there is an input name or an implied name, use it
    if (optInput) {

      strcpy(InputFilename,optInputName);

      // If there is no output name, use the input as the output)
      if (optOutput) strcpy(OutputFilename,optOutputName);
      else           strcpy(OutputFilename,optInputName);

    } else {

      // No input specified, pull it from the INI if it's there, fail otherwise.
        if (!loadini()) {

        fprintf(stderr,"ERROR: Resume without INI or input file to resume with\n");
        exit(7);


      }

      // Getting here means we loaded it. now be certain we got a useable name
      if (gotOutput) {

        strcpy (InputFilename,OutputFilename);

      } else {

        printf("Error: No valid filename in INI file to use.\n");
        printf("       Cannot continue.\n");
        exit(8);

      }


      // if a new output filename overrides, use it here
      if (optOutput)  strcpy(OutputFilename, optOutputName);

    }

    // Now make sure we have a valid data file to actually sieve.
    if (!loaddata(InputFilename)) {

      printf("Error:  Cannot load sieve input from '%s'. Cannot continue.\n",InputFilename);
      exit(9);
    }

  } // Data initialization complete.  Set program flags & controls then start


  // Same for the logfile name
  if (optLogfile) {

    strcpy(LogFilename,optLogName);

  } else {

    // This is precautionary only (for now)
    strcpy(LogFilename,OutputFilename);
    strcat(LogFilename,".log");

  }

  // if no Pmax specified, sieve to SieveLimit (the default condition)
  Pmax = SIEVELIMIT;
  if (optPmax) Pmax = optMaxP;

  // if Pmin specified, override if a conflict with existing LastP)
  if (optPmin && (optMinP != LastP)) {
     if (!Quiet) printf("Warning: Pmin=%lu overrides P of %lu\n",optMinP,LastP);
     LastP = optMinP;
  }

  // Setup autosave if desired
  if (Autosave)  {
    if (!Quiet) printf("Automatic save every %d minutes\n", Autosave);
    alarm(60);
  }

  // setup the runtime link for powmod
  if (B == 2) {
                if (!Quiet) printf("Using base-2 optimizations\n");
                powmod = powmodtwo;

  } else {
                if (!Quiet) printf("Using generic-base math\n");
                powmod = powmodgeneric;
  }


  // If Logging is enable, report where
  if (LogDelete) {
    strcpy(DelFilename,OutputFilename);
    strcat(DelFilename,".del");
  }

  if (!Quiet) if (LogDelete) printf("Logging removed values to: %s\n",DelFilename);

  // Guess we're all done, return ok
  return 1;

}
