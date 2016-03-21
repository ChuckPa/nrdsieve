//
// option.c:      Get all command line options specified and parse accordingly
//
// Date:	$Date: 2013/12/02 19:16:57 $
//
// Author:	$Author: chuck $
//
// RCSfile:	$RCSfile: option.c,v $
//
// Id:		$Id: option.c,v 1.17 2013/12/02 19:16:57 chuck Exp $
//
// Invocation:    getoptions(ac,av)  // uses 'argc' and 'argv' as supplied to main() directly
//
// ChangeLog:
//
// $Log: option.c,v $
// Revision 1.17  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.16  2013/11/29 21:39:10  chuck
// added -threads as primary new option.  Threads are now supported and enabled.
// added aliases p0 = pmin, p1 = pmax, k0 = kmin, k1 = kmax
// added -nodelete to suppress asking about logging deletes if -yes or -no without logging Q.
//
// Revision 1.15  2013/11/21 21:36:53  chuck
// Forgot to remove debug statements.  OOPS
//
// Revision 1.14  2013/11/21 20:47:18  chuck
// Corrected gcc ambiguity while exponentiating xEnnn notation.
//
// Revision 1.13  2013/11/21 19:46:18  chuck
// Added support for 'E' notation in Pmin & Pmax.  Now may be 2E9 == 2 * 10^9
//
// Revision 1.12  2013/11/21 04:27:00  chuck
// completed implementation of -yes and -no options.  YES and NO implied reply to questions
//
// Revision 1.11  2013/11/16 17:08:22  chuck
// cleaned up source
//
// Revision 1.10  2013/11/15 21:18:36  chuck
// Removed '--' option format, added minimal unique support (cheat names)
//
// Revision 1.9  2013/11/11 01:12:25  chuck
// Eliminated a race condition that will occur after figuring out we need the data
// from the INI file (the names preloaded into destination variables) and then being
// overwritten by the command line processing.  While it is almost certain to be the
// desired outcome due to sequencing, this guarantees any user combination will work
//
// Revision 1.8  2013/11/10 19:30:31  chuck
// Added Yes/No default action support.
//
// Revision 1.7  2013/11/10 19:03:13  chuck
// variable name conflict corrected
//
// Revision 1.6  2013/11/10 18:50:51  chuck
// Added 'implied' file name specification support (relaxed user input requirements)
// to permit input, output and log filenames to be 'implied' if not previously explicitly
// defined on the command line.
//
// However, once used by the main logic and flags are set, there is no turning back.
//
// Revision 1.5  2013/11/10 00:49:30  chuck
// rewrite entire option module for proper option w/ detached value handling
//
// Revision 1.4  2013/11/08 04:01:33  chuck
// added '-N[ew]', support for abbreviated options, and upper/lower case control
//
// Revision 1.3  2013/11/07 16:35:34  chuck
// added printoptions()
//
// Notes:
// revision history
// ----------------
// 16-Oct-2013  - Installed Keyword structure to simplify coding
// 12-Oct-2013  - update (upper/lower case support)
// 06-Oct-2013  - Keyword validation & error reporting (forces exit)
// 04-Oct-2013  - Initial version
//
//
static char *rcs_file_verson = "$Id: option.c,v 1.17 2013/12/02 19:16:57 chuck Exp $";
//


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "sieve.h"
#include "string.h"
#include "option.h"


// Command line options and decoding/processing table

struct_opt opttab[] = {
  // option,        upper,  bool,   type,   flag,          dest,
  { "H",                1,     1,   null,   &optHelp,      &Help },
  { "HE",               1,     1,   null,   &optHelp,      &Help },
  { "HEL",              1,     1,   null,   &optHelp,      &Help },
  { "HELP",             1,     1,   null,   &optHelp,      &Help },

  { "A",                1,     0,   intger, &optAutosave,  &Autosave },
  { "AU",               1,     0,   intger, &optAutosave,  &Autosave },
  { "AUT",              1,     0,   intger, &optAutosave,  &Autosave },
  { "AUTO",             1,     0,   intger, &optAutosave,  &Autosave },
  { "AUTOS",            1,     0,   intger, &optAutosave,  &Autosave },
  { "AUTOSA",           1,     0,   intger, &optAutosave,  &Autosave },
  { "AUTOSAV",          1,     0,   intger, &optAutosave,  &Autosave },
  { "AUTOSAVE",         1,     0,   intger, &optAutosave,  &Autosave },

  { "B",                1,     0,   intger, &optB,         &B },
  { "BA",               1,     0,   intger, &optB,         &B },
  { "BAS",              1,     0,   intger, &optB,         &B },
  { "BASE",             1,     0,   intger, &optB,         &B },

  { "C",                1,     0,   intger, &optC,         &C },

  { "D",                1,     1,   null,   &optDelete,    &LogDelete },
  { "DE",               1,     1,   null,   &optDelete,    &LogDelete },
  { "DEL",              1,     1,   null,   &optDelete,    &LogDelete },
  { "DELE",             1,     1,   null,   &optDelete,    &LogDelete },
  { "DELET",            1,     1,   null,   &optDelete,    &LogDelete },
  { "DELETE",           1,     1,   null,   &optDelete,    &LogDelete },

  { "I",                1,     0,   string, &optInput,     &optInputName },
  { "IN",               1,     0,   string, &optInput,     &optInputName },
  { "INP",              1,     0,   string, &optInput,     &optInputName },
  { "INPU",             1,     0,   string, &optInput,     &optInputName },
  { "INPUT",            1,     0,   string, &optInput,     &optInputName },

  { "K0",               1,     0,   intger, &optKmin,      &Kmin },
  { "KMI",              1,     0,   intger, &optKmin,      &Kmin },
  { "KMIN",             1,     0,   intger, &optKmin,      &Kmin },

  { "K1",               1,     0,   intger, &optKmax,      &Kmax },
  { "KMA",              1,     0,   intger, &optKmax,      &Kmax },
  { "KMAX",             1,     0,   intger, &optKmax,      &Kmax },

  { "N",                1,     0,   intger, &optN,         &N },

  { "NE",               1,     1,   null,   &optNew,       &optNew },
  { "NEW",              1,     1,   null,   &optNew,       &optNew },

  { "NO",               1,     1,   null,   &optNo,        &optNo },

  { "NOD",              1,     1,   null,   &optNoDelete,  &optNoDelete },
  { "NODE",             1,     1,   null,   &optNoDelete,  &optNoDelete },
  { "NODEL",            1,     1,   null,   &optNoDelete,  &optNoDelete },
  { "NODELE",           1,     1,   null,   &optNoDelete,  &optNoDelete },
  { "NODELET",          1,     1,   null,   &optNoDelete,  &optNoDelete },
  { "NODELETE",         1,     1,   null,   &optNoDelete,  &optNoDelete },

  { "O",                1,     0,   string, &optOutput,    &optOutputName },
  { "OU",               1,     0,   string, &optOutput,    &optOutputName },
  { "OUT",              1,     0,   string, &optOutput,    &optOutputName },
  { "OUTP",             1,     0,   string, &optOutput,    &optOutputName },
  { "OUTPU",            1,     0,   string, &optOutput,    &optOutputName },
  { "OUTPUT",           1,     0,   string, &optOutput,    &optOutputName },

  { "L",                1,     0,   string, &optLogfile,   &optLogName },
  { "LO",               1,     0,   string, &optLogfile,   &optLogName },
  { "LOG",              1,     0,   string, &optLogfile,   &optLogName },
  { "LOGF",             1,     0,   string, &optLogfile,   &optLogName },
  { "LOGFI",            1,     0,   string, &optLogfile,   &optLogName },
  { "LOGFIL",           1,     0,   string, &optLogfile,   &optLogName },
  { "LOGFILE",          1,     0,   string, &optLogfile,   &optLogName },

  { "p",                0,     0,   powr,   &optPmin,      &optMinP }, //* LOWERCASE 'p'
  { "P0",               1,     0,   powr,   &optPmin,      &optMinP },
  { "PMI",              1,     0,   powr,   &optPmin,      &optMinP },
  { "PMIN",             1,     0,   powr,   &optPmin,      &optMinP },

  { "P",                0,     0,   powr,   &optPmax,      &optMaxP },
  { "P1",               1,     0,   powr,   &optPmax,      &optMaxP },
  { "PMA",              1,     0,   powr,   &optPmax,      &optMaxP },
  { "PMAX",             1,     0,   powr,   &optPmax,      &optMaxP },
  
  { "Q",                1,     1,   null,   &optQuiet,     &optQuiet },
  { "QU",               1,     1,   null,   &optQuiet,     &optQuiet },
  { "QUI",              1,     1,   null,   &optQuiet,     &optQuiet },
  { "QUIE",             1,     1,   null,   &optQuiet,     &optQuiet },
  { "QUIET",            1,     1,   null,   &optQuiet,     &optQuiet },

  { "R",                1,     1,   null,   &optResume,    &optResume },
  { "RE",               1,     1,   null,   &optResume,    &optResume },
  { "RES",              1,     1,   null,   &optResume,    &optResume },
  { "RESU",             1,     1,   null,   &optResume,    &optResume },
  { "RESUM",            1,     1,   null,   &optResume,    &optResume },
  { "RESUME",           1,     1,   null,   &optResume,    &optResume },

  { "S",                1,     0,   intger, &optAutosave,  &Autosave },
  { "SA",               1,     0,   intger, &optAutosave,  &Autosave },
  { "SAV",              1,     0,   intger, &optAutosave,  &Autosave },
  { "SAVE",             1,     0,   intger, &optAutosave,  &Autosave },

  { "T",                1,     0,   intger, &optThreads,   &optNumThreads },
  { "TH",               1,     0,   intger, &optThreads,   &optNumThreads },
  { "THR",              1,     0,   intger, &optThreads,   &optNumThreads },
  { "THRE",             1,     0,   intger, &optThreads,   &optNumThreads },
  { "THREA",            1,     0,   intger, &optThreads,   &optNumThreads },
  { "THREAD",           1,     0,   intger, &optThreads,   &optNumThreads },
  { "THREADS",          1,     0,   intger, &optThreads,   &optNumThreads },

  { "V",                1,     1,   null,   &optVerbose,   &optVerbose },
  { "VE",               1,     1,   null,   &optVerbose,   &optVerbose },
  { "VER",              1,     1,   null,   &optVerbose,   &optVerbose },
  { "VERB",             1,     1,   null,   &optVerbose,   &optVerbose },
  { "VERBO",            1,     1,   null,   &optVerbose,   &optVerbose },
  { "VERBOS",           1,     1,   null,   &optVerbose,   &optVerbose },
  { "VERBOSE",          1,     1,   null,   &optVerbose,   &optVerbose },

  { "YE",               1,     1,   null,   &optYes,       &optYes},
  { "YES",              1,     1,   null,   &optYes,       &optYes},

  { 0,0,0,0,0 }
};

// support function for power() since C has no power operator and I don't want the library
uint64_t power(int a, int b)
{
  register uint64_t result = 1;

  while (b--) result *= a;

  return result;
}

#define ISSTRING(a,b)  ( (strlen(a)==strlen(b)) && (!strcmp(a,b)) )

// here we go

int printoptions()
{
  int i;
  char keyword[64];


  printf("command line options:\n\n");

  printf("  ");

  i=0;
  while ( *opttab[i].keyword ) {

    strcpy(keyword,opttab[i].keyword);
    if (opttab[i].upper) locase(keyword);

    (keyword[1]) ? printf("--%s",keyword) : printf("-%s",keyword) ;

    i++;
  }

}

int getoptions(ac, av)
  int   ac;
  char *av[];
{

  int   arg;
  char  *a, *keyword, *value, argstring[512];
  char  upperkey[512];

  long  ord, exp;    // the ordinal x for x*10^exp
  char  newvalue[128]; // temp string space to construct new value to convert back normally

  int   argcount = 0;  // how many did we find/recognize

  int   i, id,found;
  int   errors = 0;    // how many errors did we encounter during processing.

  char  *TRUE = "1";  // for conversion from Y to true
  uint64_t newlong;   // make gcc happy (silly promotion rules)


  // clear all opt flags
  optResume = optAutosave = optDelete = optQuiet = optVerbose = 0;
  optInput = optOutput = optLogfile = optN = optB = optKmin = optKmax = optC = 0;
  optPmin = optPmax =  optNew = optThreads = optYes = optNo =  0;
  optNoDelete = optHelp = 0;

  // remember, argv[0] is the command name :P //
  for (arg=1; arg < ac; arg++) {

    // make sure the user didn't try to screw up the command line on purpose
    strcpy(argstring,av[arg]);

    a = rtrim(argstring);
    a = ltrim(a);

    // is this a real option?
    if ( *a == '-' ) {

//    // see if '--' form
      a++;

//    if ( *a != '-') {

//      // Simple '-letter' or '-keyword'.  Need for a value will be checked later
//      keyword = a;
//      value = (char *) 0;

//    } else {

//      // We are expecting 'keyword=value' (compound argument)
//      // but will accept '--keyword value' (pair as 1 arg), which is handled below
        
//      // skip the 2nd '-'
//      a++;

        // is there a value attached to this?
        if (!strstr(a,"=")) {

          // No, so use this argument as the keyword part.  Don't bother the value part.
          keyword = rtrim(a);
          value = (char *) 0;

        } else {

          // Yes, keep the keyword and seperate into two pieces
          keyword = a;
          value   = strstr(a,"=")+1;

          // seperate the two pieces
          *(strstr(a,"=")) = '\0';

          keyword = ltrim(rtrim(a));
          value   = ltrim(rtrim(value));
        }

//    }

      // now validate and process
      strcpy(upperkey,keyword);
      upcase(upperkey);

      found = 0;
      id = 0;

      while (!found && opttab[id].keyword) {

        //Is this it?
        if ( ( opttab[id].upper && ISSTRING(upperkey,opttab[id].keyword)) ||
             (!opttab[id].upper && ISSTRING(keyword ,opttab[id].keyword)) ) {


          // Is this a duplicate?
          if (*opttab[id].flag) {
            printf("Error:  Duplicate '%s' option found\n",keyword);
            errors++;
          }

          // Tag it as found
          *opttab[id].flag = 1;              // we found it

          // Is it boolean in opttab?
          if (opttab[id].boolean) {

            sscanf(TRUE,"%d",opttab[id].dest); // let conversion handle the cast

          } else {

            // No. We are expecting the general form '--keyword=value' but also accept
            // --keyword <space> value

            // We only need to look here and see if a 'value' is required.  If it is
            // but not already 'attached' at 'value', then look at the next arg (if one)

            // Is value present?
            if (!value) {

              // no, so see if we can find it.
              if (arg < ac-1) {

                // these IS another arg that we can try to attach as this arg's value
                // unless it's another option spec (starts with '-')
                if (*av[arg+1] == '-') {

                   printf("Error:  Option '%s' needs a value but wasn't supplied\n",keyword);
                   errors++;
                    
                } else {

                   // Ok, it's not another arg, simply tag it as the 'value' and skip over
                   value = av[++arg];

                }

              } else {

                // no argument to use but is needed. Error
                printf("Error: Option '%s' needs a value but wasn't supplied\n",keyword);
                errors++;

              }
            }
                 
            // Using the format spec (fmtspec) use appropriate logic based on ther
            if (*value) {

              switch (opttab[id].type) {

                case (string):
                  if (sscanf(value,"%s",opttab[id].dest) != 1) errors++;
                  break;

                case (intger):
                  if (sscanf(value,"%d",opttab[id].dest) != 1) errors++;
                  break;

                case (lngint):
                  if (sscanf(value,"%lu",opttab[id].dest) != 1) errors++;
                  break;


                case (powr):
                  if ( strstr(value,"e") || strstr(value,"E") ) {
                    strcpy(newvalue,value);
                    upcase(newvalue);

                    exp = atoi(strstr(newvalue,"E")+1);
                    *strstr(newvalue,"E") = '\0';
                    ord   = atoi(newvalue);

                    newlong = ord * power(10,exp);
                    sprintf(newvalue,"%lu", newlong );

                    value = newvalue;
                  }

                  if (sscanf(value,"%lu",opttab[id].dest) != 1) errors++;
                  break;
     

                default:
                  if (sscanf(value,"%d",opttab[id].dest) != 1) errors++;
               }
            }
          }

          // flag it as found
          found = 1;

          // Flag it found
          *opttab[id].flag = 1;

          // increment the args found count
          argcount++;

        }

        // step to the next keyword
        if (!found) id++;

      } // yes, it was a real option and it's been handled, now get the next one

    } else {

      // Don't know what was specified, presume it's a name until we run out of space
      if (!optImp1) {

        strcpy(optName1,argstring);
        optImp1 = 1;

      } else if (!optImp2) {

        strcpy(optName2,argstring);
        optImp2 = 1;

      } else if (!optImp3) {

        strcpy(optName3,argstring);
        optImp3 = 1;

      } else {

        fprintf(stderr,"WARNING:  [%s] unrecognized. ignored.\n",argstring);
        fprintf(stderr,"          No more usable 'implied name' slots available.\n\n");
        errors++;
      }
    }

  } //


  // if errors were encountered, don't bother returning.  Exit here.  No recovery possible.
  if (errors) {
    printf("\n  %d errors encountered during processing.  Recovery not possible. Exiting\n",
           errors);
    exit(errors);
  }

  // All done with the command line options
  return argcount;
}


#ifdef UNIT_TEST
main(int ac, char *av[])
{

  getoptions(ac,av);

}
#endif
