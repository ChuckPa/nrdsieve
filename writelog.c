//
// writelog.c - write information to the logfile
//
// 		- this file contains support functions for writing other info to the logfile
//                which utilize the core functionality of writelog()
//              
//              - functions defined are:
//
//                  writelog(text);
//                  writePdivK(P,K);   // P divides K in target expression
//                  writePdivN(P,N);   // P divides N in target expression
//
//
// 13-Oct-2013 - C. Lasher
//
// ChangeLog:
//
// ver 1.0.0   - initial development
//
// $Log: writelog.c,v $
// Revision 1.7  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.6  2013/11/21 04:27:00  chuck
// Correct incorrect output (.del file) format when recording a deleted factor
//
// Revision 1.5  2013/11/16 19:00:39  chuck
// Changed delete writes to immediate fflush().  Will monitor performance impact.
//
// Revision 1.4  2013/11/16 17:08:49  chuck
// Changed logfile format to make easier to parse.
//
// Revision 1.3  2013/11/15 21:18:36  chuck
// Updated for new file i/o and interface to core variables
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: writelog.c,v 1.7 2013/12/02 19:16:57 chuck Exp $";
//


// global includes
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include "sieve.h"

// local (saved) stuff
  static FILE   *log = (FILE *) 0;
  static FILE   *del = (FILE *) 0;

writedel(char *string)
{

  // If delfile not open, generate filename based on output name & open it
  if (!del) {

    // open the log, exit if error
    del = fopen(DelFilename,"w+");

    if (del == (FILE *) 0) {

      fprintf(stderr,"ERROR: cannot open '%s' as LogDelete\n",DelFilename);
      exit(98);

    }

  }

  // log is presumed still open and write to it.  Balk if unable to
  if (! fprintf(del,"%s\n", string) ) {

    fprintf(stderr,"ERROR: Unable to write to delfile. Exiting\n");
    exit(97);
  }

  fflush(del);

}


writelog(char *string) 
{


  // If logfile not open, generate filename based on output name & open it
  if (!log) {

    // append '.log' to base name
    strcpy(LogFilename, OutputFilename);
    strcat(LogFilename,".log");

    // open the log, exit if error
    log = fopen(LogFilename,"w+");

    if (log == (FILE *) 0) {

      fprintf(stderr,"ERROR: cannot open '%s' as logfile\n",LogFilename);
      exit(98);

    }

  }

  // log is presumed still open and write to it.  Balk if unable to
  if (! fprintf(log,"%s\n", string) ) {

    fprintf(stderr,"ERROR: Unable to write to logfile. Exiting\n");
    exit(97);
  }

  fflush(log);

}

writePdivKN(uint64_t P, int K, int N)
{

  char message[128];

  sprintf(message," %d^%d-%d^%d-%d | %lu", B,N,B,K,C,P);
  writedel(message);
}

writePdivK(uint64_t P, uint64_t K)
{

  char   message[128];

  // sprintf(message,"P= %ld divides K= %ld ",P,K);
  //sprintf(message,"K= %ld divided at P= %ld ",K,P);  // THIS IS FOR DEBUG ONLY

  sprintf(message," %d*%d^%d-%d | %lu", K,B,N,C,P);
  writedel(message);

}

writePdivN(uint64_t P, uint64_t N)
{

  char   message[128];

  sprintf(message,"P= %ld divides N= %ld ",P,N);
  // (future) sprintf(message," %d*%d^%d-%d | %lu\n", K,B,N,C,P);
  writedel(message);

}
