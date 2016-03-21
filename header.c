//
// $RCSfile: header.c,v $  - Decode the supplied header string, returning values in the supplied variables
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: header.c,v 1.5 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: header.c,v $
// Revision 1.5  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.4  2013/11/21 04:23:25  chuck
// changed data save/load format to compily with new bitmap
//
// Revision 1.3  2013/11/15 21:18:36  chuck
// corrected file read/write problem formats
//
// Revision 1.2  2013/11/10 00:49:05  chuck
// changed gets() to scanf()
//
// Revision 1.1  2013/11/08 15:25:58  chuck
// Initial revision
//
//
static char *rcs_file_verson = "$Id: header.c,v 1.5 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int setheader(char *string, int B1, int N, int B2, int C, uint64_t P)
{
  sprintf(string,"ABCx %d:%d:%d:%d:%lu",B1,N,B2,C,P);
}

int getheader(char *string, int *B1, int *N, int *B2, int *C, uint64_t *P)
{

  int b1,b2,n,c;
  uint64_t p;
  int error = 0;

  // this is relativly simple.  Check the signature then return sscanf the values
  if (strncmp(string,"ABCx ",5)) return (0);  // not valid so fail

  // ok, valid so far
  b1 = b2 = n = c = p = 0;
  sscanf(&string[5],"%d:%d:%d:%d:%lu",&b1,&n,&b2,&c,&p);

  // now perform minimal validation

  if (b1 < 2) error++;
  if (b2 < 2) error++;
  if (n < 2 || n > 100000000) error++;
  if (p < 3) error++;


  // push back the values
  *B1 = b1;
  *B2 = b2;
  *N  = n;
  *C  = c;
  *P  = p;

  return error;  // correct return is '0'
}

#ifdef UNIT_TEST
main ()
{

  char buffer[80];
  int b1,b2,n,c;
  uint64_t p;
  int error;

  while (scanf("%s",buffer)) {
    error = getheader(buffer,&b1,&n,&b2,&c,&p);

    printf("decoding '%s':  %d errors, b1=%d, b2=%d, n=%d, c=%d, p=%ld\n",
           buffer,error,b1,b2,n,c,p);
  }
}
#endif // UNIT_TEST
