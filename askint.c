//
// askint.c  -  ask for an integer (with [min] [max] boundaries)
//
// $RCSfile: askint.c,v $
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: askint.c,v 1.5 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: askint.c,v $
// Revision 1.5  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.4  2013/11/21 04:22:54  chuck
// updated for new output
//
// Revision 1.3  2013/11/15 21:18:36  chuck
// more appropriate user input handling
//
// Revision 1.2  2013/11/10 00:48:37  chuck
// changed gets() to scanf()
//
// Revision 1.1  2013/11/08 03:53:45  chuck
// Initial revision
//
//
//
static char *rcs_file_verson = "$Id: askint.c,v 1.5 2013/12/02 19:16:57 chuck Exp $";
//
//

#include <stdio.h>
#include <string.h>

int askint(char *prompt, int min, int max)
{

  char reply[80];
  int  gotanswer = 0;
  long answer = 0;


  while (!gotanswer) {

    // read console input
    printf("%s: ",prompt); fflush(stdout);
    //else            printf("%s [%d-%d]: ",prompt,min,max); fflush(stdout);

    // get the input
    scanf("%d",&answer);


    // now range check and issue error message if appropriate.
    // Boundary ignore (unbounded) flag value is '-1'   (WARNING)

    if ( (min != -1) && (max != -1) && ( answer < min | answer > max ) ) {

      printf("Your reply (%d) must be >= %d and <= %d\n",answer,min,max);
      printf("Please try again\n");

    } else {

      gotanswer = 1;
      return answer;

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

  // now perform number tests
  count = askint("how many times to I have to ask?",1,100);

  printf(" '%d' ???  That's it? \n",count);

}
#endif
