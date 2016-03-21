//
// loadsaveini.h 
//
// 	Define all variables used for loading & saving the .ini file (and flagging what we got)
//
//
// ChangeLog:
//
// $Log: loadsaveini.h,v $
// Revision 1.6  2013/11/21 04:18:41  chuck
// updated for new bitmap and data structure.
//
// Revision 1.5  2013/11/15 21:18:36  chuck
// corrected inputfile / output filename getting lost in certain cases
//
// Revision 1.4  2013/11/11 03:06:37  chuck
// Corrected enumeration mismatch and centralized definition into one module.
//
// Revision 1.3  2013/11/10 05:15:32  chuck
// Changed from using hard coded ini name to #define constant
//
// Revision 1.2  2013/10/21 06:11:09  chuck
// Added RCS header info
//
//

// make sure we have these
#include "sievenums.h"

// Now for the rest of the variables
#ifndef _HAVE_LOADSAVEINI_H
#define _HAVE_LOADSAVEINI_H

// Name of the INI file
#define SIEVE_INI_NAME "sieve.ini"     // adjust this for your OS as desired

// master structure used to hold all ini keywords, formats, locations and flags

typedef struct ini_struct {

  char *keyword; // keyword to be tested for (a const char *)
  int  boolean;  // does keyword use a boolean value or a formatted value
                 //  Note: Booleans that do not accept alternate input forms have a NULL format
  int  format;   // format used for conversion by sscanf()
  void *dest;    // destination variable addresss (&xxxx) .  It is presumed this is properly aligned
  int  *flag;    // flag to set showing this variable was found in the ini file

} struct_ini;


  // Sieve Creation parameters (also loaded when reading .ini file)
  int           gotB;
  int           gotN;
  int           gotKmin, gotKmax;
  int           gotC;
  int           gotLastP;
  int           gotPmin,gotPmax;
  int           gotKsize;
  int           gotAutosave;
  int           gotInput;
  int           gotOutput;
  int           gotSaveInterval;
  int           gotLogDelete;
  int           gotFilename;
  int           gotLogfile;
  int           gotVerbose;


#endif // _HAVE_LOADSAVEINI_H
