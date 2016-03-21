//
// askyn.c  -  ask for a yes/no answer from the console, return 0 if no, 1 if yes
//
// $RCSfile: askyesno.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: askyesno.c,v 1.9 2013/12/02 19:16:57 chuck Exp $
//
// Initial Version:  	10-Oct-2013 - C. Lasher
//
// ChangeLog:
//
// $Log: askyesno.c,v $
// Revision 1.9  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.8  2013/11/21 04:23:25  chuck
// updated user i/o
//
// Revision 1.7  2013/11/16 04:55:39  chuck
// Integrated optYes and optNo feature
//
// Revision 1.6  2013/11/10 00:48:50  chuck
// changed gets() to scanf()
//
// Revision 1.5  2013/11/08 03:55:26  chuck
// minor tweak to reply handling (bug prevention)
//
// Revision 1.4  2013/10/31 19:17:57  chuck
// Changed askyn() to askyesno()
//
// Revision 1.3  2013/10/21 15:15:38  chuck
// Corrected header
//
// Revision 1.2  2013/10/21 06:07:55  chuck
// Added RCS header info
//
static char *rcs_file_verson = "$Id: askyesno.c,v 1.9 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <string.h>

#include "option.h"

int askyesno(char *prompt)
{

  char reply[80];
  int  gotyn = 0;


  // use provided yes/no if present
  if (optYes) {
    printf("%s [y/n] ",prompt);
    printf("YES\n"); fflush(stdout);
    return 1;
  }

  if (optNo) {
    printf("%s [y/n] ",prompt);
    printf("NO\n");  fflush(stdout);
    return 0;
  }

  while (!gotyn) {

    strcpy(reply,"");

    // read console input
    reply[0] = 0;
    printf("%s [y/n] ",prompt); fflush(stdout);
    
    scanf("%s",reply);

    // convert to uppercase
    if (*reply >= 'a' && *reply <= 'z') *reply -= 32;

    if (*reply != 'Y' && *reply != 'N') {

      printf("Please answer Y[es] or N[o]\n");

    } else {

      if (*reply == 'Y') return 1;
      if (*reply == 'N') return 0;

      printf("Invalid reply.  Please try again.");

    }
  }
}


#ifdef UNIT_TEST

main(ac,av)
{
  
  int i;
  int count = 0;

  for (i=0; i< 5 ; i++) {

    count = askyesno("Do you wear boots to bed?");
    askyesno("Are you SURE?");
  }
  if (count == 0 ) printf("OK OK!  I heard you!\n");
  if (count > 0 && count < 3) printf("Hmmm, you don't sound to sure of yourself\n");
  if (count > 3) printf("Fine.  I promise not to tell.\n");

}
#endif
