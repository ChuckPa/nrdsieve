//
//
// strings.c	Utility functions to help with C's complete lack of string support.
//		These attempt to make up for that design consideration.
//
//
// $RCSfile: string.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: string.c,v 1.2 2013/12/02 19:16:57 chuck Exp $
//
// Functions included:
//	nocrlf()	removes CR and LF (\r and \n) from a string)
//      upcase()        converts lower case to upper case as needed
//      locase()        converts upper case to lower case as needed
//	ltrim()		removes leading white space (' ' and '\t')
//      rtrim()         removes trailing white space (' ' and '\t')
//
//
// ChangeLog:
//
// $Log: string.c,v $
// Revision 1.2  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.1  2013/11/30 18:13:05  chuck
// Initial revision
//
// Revision 1.2  2013/10/21 15:11:45  chuck
// corrected includes
//
// Revision 1.1  2013/10/21 14:58:31  chuck
// Initial revision
//
//
//
static char *rcs_file_verson = "$Id: string.c,v 1.2 2013/12/02 19:16:57 chuck Exp $";
//



#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <strings.h>

#include "string.h"



int nocrlf (char *s)
{
   // scan the string, terminating it at the first CR or LF found.
   while (*s) {
     if (*s == 13) { *s = 0; return; } // CR (\r)
     if (*s == 12) { *s = 0; return; } // FF (\f)
     if (*s == 10) { *s = 0; return; } // LF (\n)
     s++;
   }
}

int upcase (char *s)
{
   // scan the string, converting lowercase letters to uppercase letters
   while (*s) {
     if (*s >= 'a' && *s <= 'z') *s -= 32; 
     s++;
   }
}

int locase (char *s)     
{
   // scan the string, converting uppercase letters to lowercase letters
   while (*s) {
     if (*s >= 'A' && *s <= 'Z') *s += 32; 
     s++;
   }
}


char *ltrim(char *s)
{
  while (*s == 32 || *s == 9 ) s++;   // space or tab
  return s;
}

char *rtrim(char *s)
{
  int i;

  // ookay, let's do this the old fashioned way since the sse version of strlen doesn't like it
  
  // take of CR, LF, trailing TAB and SPACE (whitespace)

  i = strlen(s) - 1;  //  convert length to array index

  while (i > 0) {
    if ( s[i] == 32 || s[i] ==  10 || s[i] == 13 || s[i] == 9) {
      s[i] = 0;
    } else {
      return s;
    }
    i--;
  }
  return s;
}

