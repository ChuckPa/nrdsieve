//
//
// $RCSfile: printmenu.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: printmenu.c,v 1.3 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: printmenu.c,v $
// Revision 1.3  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.2  2013/11/21 04:27:00  chuck
// worked more on this.  may still be eliminated.  undecided on how to proceed.
//
// Revision 1.1  2013/11/10 00:49:52  chuck
// Initial revision
//
//
static char *rcs_file_verson = "$Id: printmenu.c,v 1.3 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>

static char *menu[] = {
   " ",
   " Menu choices are: ",
   " ",
   "   1.  Create a new sieve",
   "   2.  Continue an existing sieve",
   "   3.  Change sieving options [-not-implemented-yet]",
   "   4.  Exit",
   0 };

int printmenu()
{
  char *s;

  while (*s) { 
    printf("%s\n",s);
    s++;
  } ;
}
