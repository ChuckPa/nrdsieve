//
// inifile.c:	read/load or save the .ini file
//
// Invocation:	loadini(ini-filename)
//
// $RCSfile: loadsaveini.c,v $
//
// $Date: 2013/12/11 22:04:58 $
//
// $Author: chuck $
//
// $Id: loadsaveini.c,v 1.10 2013/12/11 22:04:58 chuck Exp chuck $
//
// 	Compile-time options
//
//  	WARN_NOTFOUND   -- Warning if a keyword specified is NOT found when reading the ini
//  	WARN_DUP        -- Warning if a keyword is specified more than once
//
// ChangeLog:
//
// Notes:
// revision history
// ----------------
// 04-Oct-2013  - Initial version
// 06-Oct-2013  - Keyword validation & error reporting (forces exit)
// 12-Oct-2013  - update (upper/lower case support)
// 14-Oct-2013  - Installed Keyword structure to simplify coding
//
// $Log: loadsaveini.c.v $
// Revision 1.11  2017/12/08 18:52:04  chuck
// Corrected optOutput processing
//
// $Log: loadsaveini.c,v $
// Revision 1.10  2013/12/11 22:04:58  chuck
// updated for new structure
//
// Revision 1.9  2013/11/21 04:23:25  chuck
// changed data save/load format to compily with new bitmap
//
// Revision 1.8  2013/11/15 21:18:36  chuck
// reflect changed internal variable names
//
// Revision 1.7  2013/11/10 05:26:52  chuck
// Reinstalled data type & validity checking of ini file parameter values
//
// Revision 1.6  2013/11/10 00:57:12  chuck
// corrected typo in structure member name
//
// Revision 1.5  2013/11/10 00:49:13  chuck
// changed gets() to scanf().  updated io.
//
// Revision 1.4  2013/11/08 03:52:10  chuck
// Updated to return TRUE when ini loaded, FALSE otherwise
//
// Revision 1.3  2013/10/21 15:20:11  chuck
// Updated header & includes
//
//
static char *rcs_file_verson = "$Id: loadsaveini.c,v 1.10 2013/12/11 22:04:58 chuck Exp chuck $";
//


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <strings.h>
#include "sieve.h"
#include "loadsaveini.h"
#include "string.h"
#include "limits.h"

// --- put compile-time flags here (if used)
#define WARN_NOTFOUND
#define WARN_DUP


extern char *ltrim(char *);
extern char *rtrim(char *);

// struct_ini and ini_struct are defined in loadsaveini.h

// Reference all INI keywords, destination, and trigger variables here
struct_ini initable[] = {
   // keyword        bool,  format,     dest,          flag

   { "AUTOSAVE",        1,  intger,   &Autosave,       &gotAutosave },
   { "LASTP",           0,  lngint,   &LastP,          &gotLastP },
   { "FILENAME",        0,  string,   &InputFilename,  &gotInput },
   { "INPUT",           0,  string,   &InputFilename,  &gotInput },   // *Alias for Filename
   { "INPUTNAME",       0,  string,   &InputFilename,  &gotInput },   // *Alias for FILENAME
   { "OUTPUTNAME",      0,  string,   &OutputFilename, &gotOutput },
   { "OUTPUT",          0,  string,   &OutputFilename, &gotOutput },  // *Alias for OutputName
   { "LOGFILE",         0,  string,   &LogFilename,    &gotLogfile },
   { "VERBOSE",         1,  intger,   &Verbose,        &gotVerbose },
   { "LOGDELETE",       0,  intger,   &LogDelete,      &gotLogDelete },
   { "BASE",            0,  intger,   &B,              &gotB },
   { "N",               0,  intger,   &N,              &gotN },
   { "KMIN",            0,  intger,   &Kmin,           &gotKmin },
   { "KMAX",            0,  intger,   &Kmax,           &gotKmax },
   { "KSIZE",           0,  intger,   &Ksize,          &gotKsize },
   { "C",               0,  intger,   &C,              &gotC },
   { "PMIN",            0,  lngint,   &Pmin,           &gotPmin },
   { "PMAX",            0,  lngint,   &Pmax,           &gotPmax },

   { "LASTP",           0,  lngint,   &Pmin,           &gotPmin },    // Transfer LastP to Pmin

   { 0,0,0,0,0 }
 };

#define ISSTRING(a,b)  ( (strlen(a)==strlen(b)) && (!strcmp(a,b)) )

int loadini(char *filename)
{
  FILE *ini = fopen(filename,"r");

  char  *l,*keyword, *value;
  char  line[512];

  int   id,found;

  int   errors = 0;
  int   fatal = 0;

  char  *YES = "1", *NO="0";  // for booleans



  // Clear all 'got' flags
  gotB = gotN = gotKmin = gotKmax = gotKsize = gotC = gotLastP = gotVerbose = 0;
  gotLogDelete = gotInput = gotOutput = gotAutosave = gotLogfile = gotPmin = gotPmax = 0;


  if (ini == (FILE *) 0) {
    return(0);
    // printf("Error: cannot open ini file\n");
  }

  // Make sure the buffer is empty
  bzero(line,sizeof(line));

  // read each record from the ini file

  while ( fgets(line, 256, ini)) {

    nocrlf(line);

    // remove leading & trailing whitespece
    l = rtrim(line);
    l = ltrim(l);

    // parse simple and paired entries
    if (!strstr(l,"=")) {

      keyword = rtrim(l);
      value  = '\0';

    } else {

      keyword = l;
      value   = strstr(l,"=")+1;

      *(strstr(l,"=")) = '\0';

      keyword = ltrim(rtrim(l));
      value   = ltrim(rtrim(value));

#ifdef DEBUG
      printf("Keyword='%s', Value='%s'\n",keyword,value);
#endif

      // zero it again (sse is that fussy)
      bzero(line,sizeof(line));
    }

#ifdef DEBUG
    printf("INI: keyword = [%s], value = [%s]\n",keyword,value);
#endif
    // Convert to uppercase and go from there
    upcase(keyword);

    found = 0;
    id    = 0;

    // Validate the supplied keyword and handle it
    while ( (!found) && initable[id].keyword ) {

      // see if it matches
      if (ISSTRING(keyword,initable[id].keyword)) {

        // convert ini value to internal format
        if (initable[id].boolean) {

          if (*value == 'y' || *value == 'Y')  sscanf(YES,"%d",initable[id].dest);

          else if (*value == 'n' || *value == 'N') sscanf(NO,"%d",initable[id].dest);

          else sscanf(value,"%d", initable[id].dest);

        } else {

          // Using the format spec (fmtspec) use appropriate logic based on ther
          switch (initable[id].format) {

            case (string):
#ifdef DEBUG
              printf("capturing '%s' for '%s'\n", value, initable[id].keyword);
#endif
              if (sscanf(value,"%s",initable[id].dest) != 1) 
                printf("Error: INI file parameter '%s' invalid [%s]\n",keyword,value);
              break;

            case (intger):
              if (sscanf(value,"%d",initable[id].dest) != 1)
                printf("Error: INI file parameter '%s' not an integer [%s]\n",keyword,value);

#ifdef DEBUG
              // DEBUG:
              int dbg = *( (int *) initable[id].dest);
              printf("%s got  %d\n", initable[id].keyword, dbg );
#endif
                break;

            case (lngint):
              if (sscanf(value,"%lu",initable[id].dest) != 1)
                printf("Error: INI file parameter '%s' not an integer [%s]\n",keyword,value);
              break;

            default:
              sscanf(value,"%d",initable[id].dest);
                printf("Error: INI file parameter '%s' invalid [%s]\n",keyword,value);
          }

          // ORIG - sscanf(value,initable[id].fmtspec,initable[id].dest);

        }

        // flag it as found
        found = 1;

#ifdef WARN_DUP
        if (*initable[id].flag) printf("Warning: Duplicate '%s' overrides first reference\n",keyword);
#endif
        // Flag it found
        *initable[id].flag = 1;

      }

      // step to the next keyword
      id++;

    } // end checking keyword list

    // if !found we got junk somehow
#ifdef WARN_NOTFOUND
    if (!found) {
      printf("Unknown keyword '%s' found in .ini file, ignored\n",keyword);
    }
#endif

    // zero (null) out the input buffer.. it's squirrely from time to time
    bzero(line,512);

  } // while(fgets)


  // Now validate (and range check if appropriate) all loaded values

  // Autosave
  if (gotAutosave) {
    if (Autosave <= 0 || Autosave > 60000) {
      fprintf(stderr,"Warning: Autosave specified is way out of range [%s]\n",value);
      fprintf(stderr,"         Using default.");
      errors++;
      Autosave = AutosaveDefault;
    }
  }



  // InputFile the input filename, copy it to the output filename
  if (gotInput) {

      // Only copy input filename to output name if no Output filename yet encountered
      if (!gotOutput) strcpy(OutputFilename,InputFilename);

#ifdef DEBUG
      printf("DBG: copying Input -> Output\n");
#endif

  }


  // Verbose
  if (gotVerbose)
    if (Verbose < 0 || Verbose > 5) Verbose = 1;


  // B
  if (gotB) {
    if (B < 2 || B > 10000) {

      fprintf(stderr,"Warning: 'Base' in .ini file is wildly out of range [%d] - Ignored\n",B);
      errors++;
      fatal++;
    }
  }


  // N
  if (gotN) {
    if (N < 2 || N > 100000000) {

      fprintf(stderr,"Warning: 'N' in .ini file is out of range [%d]\n",N);
      errors++;
      fatal++;

    }
  }

  // Kmin
  if (gotKmin) {
    if (Kmin < 1 || Kmin > 1000000000) {

      fprintf(stderr,"Warning: 'Kmin' in .ini file is out of range [%d]\n",Kmin);
      errors++;
      fatal++;

    } 
  }

  // Kmax
  if (gotKmax) {
    if (Kmax < 1 || Kmax > 1000000000) {

      fprintf(stderr,"Warning: 'Kmax' in .ini file is out of range [%d]\n",Kmax);

      errors++;
      fatal++;
    }
  }

  // Ksize (for resume)
  if (gotKsize) {
    if (Ksize < 1 || Ksize > 1000000000) {
      fprintf(stderr,"Warning: 'Ksize' in .ini file is wildly out of range [%d]\n",Ksize);
      errors++;
    }
  }


  // C
  if (gotC) {
    if (!C) {

      fprintf(stderr,"Warning: 'C' in .ini file is out of range [%s]\n",value);
      fprintf(stderr,"         Value overridden.  Default of '1' assigned.\n");
      errors++;
      C = 1;
    }
  }

  // LastP
  if (gotLastP || gotPmin) {
    if (Pmin < 2 ) {

      fprintf(stderr,"Warning: 'Pmin' in .ini file is out of range [%s]\n",value);
      fprintf(stderr,"         Value overridden.  Default of '2' assigned.\n");
      
      errors++;
      Pmin = 2;
    }

    // If not yet encountered Pmax,  set to SIEVELIMIT for now
    if (! gotPmax ) Pmax = SIEVELIMIT;
   
  }

  // Make sure Pmin & Pmax are sane if specified 
  if (gotPmax && gotPmin) {
    if (Pmax <= Pmin) {
      fprintf(stderr,"Warning: Pmax <= Pmin.  Both values ignored.\n");
      errors++;

      Pmax = 0;
      Pmin = 0;
      gotPmax = gotPmin = 0;
    }
  }

  // Close the ini file
  fclose(ini);


  // Decide what to do 
  if (errors) printf("Warning:  %s errors encountered in ini file\n",errors);

  if (fatal)  {
    printf("ERROR:    Fatal errors in ini file prevent further processing.  Exiting\n");
    exit(99);
  }

  // return TRUE because we found & loaded the .ini.  We use this result
  return (1);
}

int saveini(char *Filename)
{
  FILE *ini = fopen(Filename, "w");


  // if we cannot open the .ini file to write to, silently exit
  if ( ini == (FILE *) 0) {
    printf("WARNING: unable to open .ini file [%s] to write to.\n",SIEVE_INI_NAME);
    return 0;
  }


  // now dump it all out, in order EXCEPT, use LIVE values and not 'ini' prefixed ones

  fprintf(ini,"Input     = %s\n"  ,InputFilename);
  fprintf(ini,"Output    = %s\n"  ,OutputFilename);
  fprintf(ini,"Base      = %d\n"  ,B );
  fprintf(ini,"N         = %d\n"  ,N );
  fprintf(ini,"Ksize     = %d\n"  ,Ksize);
  fprintf(ini,"Kmin      = %d\n"  ,Kmin );
  fprintf(ini,"Kmax      = %d\n"  ,Kmax );
  fprintf(ini,"C         = %d\n"  ,C    );
  fprintf(ini,"LastP     = %lu\n" ,LastP);

  // Do not expose optional keywords that are not at default value

  if (Autosave)          fprintf(ini,"Autosave  = %d\n", Autosave);
  if (LogDelete)         fprintf(ini,"LogDelete = %d\n", LogDelete);
  if (Verbose)           fprintf(ini,"Verbose   = %d\n", Verbose);
  if (*LogFilename)      fprintf(ini,"Logfile   = %s\n", LogFilename);

  // Although coded here, Pmax & Pmin are not retained by default
  //if (Pmax)              fprintf(ini,"Pmax = %lu\n",     Pmax);
  //if (Pmin)              fprintf(ini,"Pmin = %lu\n",     Pmin);


  fflush(ini);
  fclose(ini);
}




#ifdef UNIT_TEST
int main()
{


    loadini("loadini.test");


    // now print what's in global memory (dump it out)
    printf("After loading loadini.test, values are: \n");

    //B = N = 1;
    //Kmin = Kmax = C = LastP = Autosave = LogDelete = Verbose = 2;
    //strcpy(InputFilename,"THIS=IS=WRONG");

    printf("Base         = %ld\n",B    );
    printf("N            = %ld\n",N    );
    printf("Kmin         = %ld\n",Kmin );
    printf("Kmax         = %ld\n",Kmax );
    printf("C            = %ld\n",C    );
    printf("LastP        = %lu\n",LastP);

    printf("\n");
    printf("Autosave     = %d\n",Autosave);
    printf("LogDelete    = %d\n",LogDelete);
    printf("Verbose      = %d\n",Verbose);
    printf("Logfile      = %s\n",LogFilename);
    printf("Input        = %s\n",InputFilename);
    printf("Output       = %s\n",OutputFilename);


    // final test -- save a test .ini file
    saveini("loadini.save");
}
#endif // UNIT_TEST
