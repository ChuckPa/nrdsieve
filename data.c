//
// $RCSfile: data.c,v $
//
// $Date: 2013/12/11 22:04:58 $
//
// $Author: chuck $
//
// $Id: data.c,v 1.12 2013/12/11 22:04:58 chuck Exp chuck $
//
// loaddata():  loads/decodes header, allocates memory & loads prior sieve data
//
//      invocation:   count = loaddata(filename);  
//      returns:      # of items loaded or '0' if unsuccessful
//
// savedata():  saves/endocdes header, writes current sieve candidates to file
//
//      invocation:   count = savedata(filename);  // data taken from global structures
//      returns:      # of items written or '0' if unsuccessful
//
//
//
// Warning:	No error detection / reporting is made for corrupted candidates.
//		Any candidates which do not convert to proper integers are silently ignored.
//
//
static char *rcs_file_verson = "$Id: data.c,v 1.12 2013/12/11 22:04:58 chuck Exp chuck $";
//
// ChangeLog:
//
//  Initial:  14-Oct-2013
//  Merged:   06-Nov-2013  (loaddata & savedata) 
//
// $Log: data.c,v $
// Revision 1.12  2013/12/11 22:04:58  chuck
// updated i/o structure and lockboxes
//
// Revision 1.11  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.10  2013/11/29 21:35:50  chuck
// pthread support added
//
// Revision 1.9  2013/11/21 04:23:25  chuck
// changed data save/load format to compily with new bitmap
//
// Revision 1.8  2013/11/16 18:31:05  chuck
// Corrected for 'count overrun' which is no longer applicable using linked lists.
// Removed the 'correction' as it was now wrong
//
// Revision 1.7  2013/11/16 17:08:49  chuck
// Stopped status message (when saving candidates) from scribbling too much (VT-100)
//
// Revision 1.6  2013/11/16 04:59:04  chuck
// correct list walking
//
// Revision 1.5  2013/11/15 21:18:36  chuck
// changed candidate list to linked list format (input & output)
//
// Revision 1.4  2013/11/10 05:13:13  chuck
// Changed from using hard coded ini name to #define constant
//
// Revision 1.3  2013/11/10 00:48:57  chuck
// changed gets() to scanf()
//
// Revision 1.2  2013/11/08 04:08:06  chuck
// Created module, having pulled the functions out of sieve.c
//
// Revision 1.1  2013/11/07 15:54:55  chuck
// Initial revision
//
//
//
//
//


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#include "sieve.h"
#include "loadsaveini.h"

int loaddata(char *filename)
{
  FILE    *input;
  int      count;
  char     header[80];
  char     line[128];
  int      i;
  int      b1,n,b2,c;
  uint64_t P;
  int      value;


//printf("Called loaddata(%s)\n",filename);

  // determine how many, allocate, then load
  input = fopen (filename,"r");

  if (input == (FILE *) 0)  return 0;

  Ksize = count = 0;
  Kmax = 0;
  Kmin = 1<<30;

  // read first record, verify it's a header record then 
  // fscanf(input,"%s",header);
  fgets(header,80,input);
  nocrlf(header);

  // if the header decoded cleanly, trust what we got
  if (!getheader(header,&b1,&n,&b2,&c,&P)) {
  
    // Now count the number of records to load, allocate memory and load it
    while (fgets(line,128,input)) {

      count++;

      // find Kmin and Kmax while counting records
      if (sscanf(line,"%d",&i) > 0) {

        if (i < Kmin)  Kmin = i;
        if (i > Kmax)  Kmax = i;
      }
    }

    // go back to the beginning
    fseek(input, 0L, SEEK_SET);

    // skip header record
    fgets(line,sizeof(line),input); // fscanf(input,"%s",line);

    // allocate memory
    Ksize = Kmax - Kmin + 1;
    Kbitmap = calloc(1, sizeof(type_k)*(Ksize+2));

    // start reading
    count = 0;
    while (fscanf(input,"%d",&value) == 1) {
      if (value >= Kmin && value <= Kmax) {
        Kbitmap[value-Kmin] = 1;
        count++;
      }
    }

    // report how many found/loaded.
    printf("Loaded %d candidates from %s\n",count,filename);


  }

  // Push back the values retrieved 
  B = b1;
  N = n;
  C = c;
  LastP = P;

  fclose(input);

  return(count);
}

// was: static void .. possible problem with signal handler
void savedata()
{
  FILE *output;
  char  PrevName[512];  // name of old output file with '~' appended.. powerfail recovery
  int   i;
  int   count = 0;
  int   b1,n,b2,c;
  int   newKmin, newKmax;
  char  header[80];
  type_k  *K;

  // create name of file to mv() to
  strcpy(PrevName,OutputFilename);
  strcat(PrevName,"~");

  // delete the previous version of the save file, if it exists
  unlink(PrevName);

  // mv (rename) the current output to the new one
  rename (OutputFilename,PrevName);


  // Now save the data normally
  output = fopen(OutputFilename,"w");

  // Warn if we can't write it
  if (output == (FILE *) 0) {
   fprintf(stderr,"\nERROR: Cannot save sieve output to '%s'\n",OutputFilename);
  }

  // output the header record
  setheader(header,B,N,B,C,LastP);
  fprintf(output,"%s\n",header);

  for (i=0; i<Ksize; i++) {
    if (Kbitmap[i]) {
      fprintf(output,"%d\n",i+Kmin);
      count++;
    }
  }

  // Update Ksize for our return (reduced / updated Ksize)
  Ksize = Kmax - Kmin + 1;

  // save our current values (.ini)
  saveini();

  // flush and close
  fflush(output);
  fclose(output);
 

  printf("\rWrote %d K's to %s\033[K                \r",count,OutputFilename);
  fflush(stdout);
}
