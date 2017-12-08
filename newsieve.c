//
// newsieve.c  -- Create and initialize a new sieve.   Sieve type selections go here.
//
// $RCSfile: newsieve.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: newsieve.c,v 1.3 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: newsieve.c,v $
// Revision 1.3  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.2  2013/11/29 21:37:52  chuck
// Completed isolation of new sieve creation for pthread support
// added support for -nodelete
//
// Revision 1.1  2013/11/21 04:23:25  chuck
// Initial revision
//
//
static char *rcs_file_verson = "$Id: newsieve.c,v 1.3 2013/12/02 19:16:57 chuck Exp $";
//

// Global includes
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

// Local includes
#include "sieve.h"
#include "option.h"

int newsieve()
{

  int     i;


  // Before asking for values, check what we got.  If illegal, ignore them
  if (optB    && (B    < 2 || B    >      65536)) optB = 0;
  if (optN    && (N    < 2 || N    > 2000000000)) optN = 0;
  if (optKmin && (Kmin < 1 || Kmin > 2000000000)) optKmin = 0;
  if (optKmax && (Kmax < 1 || Kmax > 2000000000)) optKmax = 0;

  New = 1;
  do {

    if (!optB)        B = askint("Base", 2,      65535);
    if (!optN)        N = askint("N"   , 2, 2000000000);
    if (!optKmin)  Kmin = askint("Kmin", 1, N-1);
    if (!optKmax)  Kmax = askint("Kmax", Kmin, N-1);
    if (!optC)        C = askint("C"   , -2000000000,200000000);

    if      (optDelete)   LogDelete = 1;
    else if (optNoDelete) LogDelete = 0;
    else                  LogDelete = askyesno("Log factored candidates?");

    if (!optOutput) {
      printf("Output Filename: "); fflush(stdout);
      scanf("%s",OutputFilename);
      strcpy (optOutputName,OutputFilename);
    } else {
      strcpy (OutputFilename,optOutputName);
    }

    printf("\nCreating new sieve for %d^%d - %d^K - %d  (%d <= K <= %d),\n",
              B,N,B,C,Kmin,Kmax);
    printf("writing output to: '%s'\n",OutputFilename);
    printf("with logging of factored candidates: ");
    (LogDelete) ? printf("enabled\n") : printf("disabled\n");

    if (optPmax) printf("with Pmax limited to: %lu\n", optMaxP) ;
    else         printf("with Pmax unlimited.\n");

    printf("\n");

  } while (!askyesno("ready to start?"));

  Ksize = (Kmax - Kmin)+1;
  Kbitmap = calloc(1,sizeof(type_k)*(Ksize+2));

  // If unable to allocate the memory, print message & return error
  if (Kbitmap == 0) {
    fprintf(stderr,"Error: Unable to allocate memory for candidates\n");
    fprintf(stderr,"       Requested number of candidates = %d\n",Ksize+2);
    return (0);
  }

  // Ok, initialize the K list
  for (i=0; i<Ksize; i++) Kbitmap[i] = 1;

  // All set and ready to go
  return(1);
}
