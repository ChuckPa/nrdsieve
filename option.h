//
// 	
//
// $RCSfile: option.h,v $ - Define all variables used for getting command line options 
//             (and flagging what we got)
//
// $Date: 2013/11/29 21:41:48 $
//
// $Author: chuck $
//
// $Id: option.h,v 1.13 2013/11/29 21:41:48 chuck Exp $
//
//
// ChangeLog:
//
// $Log: option.h,v $
// Revision 1.13  2013/11/29 21:41:48  chuck
// added option optNoDelete
// corrected missing global flags (were static)
//
// Revision 1.12  2013/11/21 04:18:41  chuck
// updated for new bitmap and data structure.
//
// Revision 1.11  2013/11/15 21:18:36  chuck
// Removed '--' option handler, switched to '-'
// Added support for multiple aliases (includes shorted option names by cheating)
//
// Revision 1.10  2013/11/11 03:06:37  chuck
// Corrected enumeration mismatch and centralized definition into one module.
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
// corrected variable name conflict.
//
// Revision 1.6  2013/11/10 18:50:51  chuck
// Added 'implied' file name specification support (relaxed user input requirements)
// to permit input, output and log filenames to be 'implied' if not previously explicitly
// defined on the command line.
//
// However, once used by the main logic and flags are set, there is no turning back.
//
// Revision 1.5  2013/11/10 00:50:37  chuck
// added support for new variables and opttab colums
//
// Revision 1.4  2013/11/08 04:07:01  chuck
// added header
//
//
//
//

// get enum definitions if not already present (required)
#include "sievenums.h"

#ifndef _HAVE_OPTIONS_H
#define _HAVE_OPTIONS_H


// master structure used to hold all ini keywords, formats, locations and flags

typedef struct opt_struct {

  char *keyword; // keyword to be tested for (a const char *)
  int  upper;    // Is mixed case acceptable and converted to all uppercase for usage?
  int  boolean;  // does keyword use a boolean value or a formatted value
                 // Note: Booleans that do not accept alternate input forms have a NULL format
  int  type;     // data type (format) used for conversion
  int  *flag;    // flag to set showing this variable was found on the command line
  void *dest;    // destination variable addresss (&xxxx) .  It is presumed this is aligned

} struct_opt;


  // Sieve command line parameters  (similar to ini options but not saved between runs)
  int           OptionCount;		// How many command line options found
  int           optB;			// Base specied
  int           optN;			// 'N' specified
  int           optKmin, optKmax;	// 'K' min & max range specified
  int           optC;			// 'C' specied
  int           optPmin,optPmax;	// 'P' min (start) and max (end) range specified
  int           optNew;			// True if requesting new sieve creation
  int           optResume;		// True if requestin resume of an existing sieve  
  int           optAutosave;		// Automatic save interval (default available)
  int           optInput;		// Name of input file to use specified
  int           optOutput;		// Name of output file to use specified
  int           optDelete;		// True if logging deleted candidates requested.
  int           optNoDelete;            // True forces 'optDelete' off in '-yes' cases
  int           optFilename;		// True if only one filename specified (not I & O)
  int           optLogfile;		// Logfile name specified
  int           optVerbose;		// Verbose runtime info requested
  int           optHelp;		// Help requested
  int           optQuiet;		// Quiet (terse) output requested
  int           optYes; 		// Assume all questions are 'YES' answers
  int		optNo;			// Assume all questions are 'NO' answers
  int           optThreads;             // Multiple threads requested
  int           optNumThreads;          // How many were requested/granted

  long          optMinP,optMaxP;        // Hold Pmin,Pmax here because they aren't 'int

  // Place to hold the filenames for resume operations when redirecting input & output
  // while keeping all other parameters
  
  // these variables are used for managing unspecified filenames found on command line
  // If invocation was: 'sieve -r name1 name2 name3'  then
  //                    name1 = input, name2 = output, name3 = logfile

  char          optName1[512], optName2[512], optName3[512];
  int           optImp1,       optImp2,       optImp3;

  // These variables keep the names from entering a race condition while deciding to 
  // continue an existing sieve using the ini but overriding the output or other filenames
  // as well as any name specified on the command line.

  char          optInputName[512], optOutputName[512], optLogName[512];

#endif // _HAVE_OPTIONS_H
