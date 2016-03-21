//
//
// $RCSfile: threads.c,v $
//           -- Thread library (very thin layer) on top of the local host multi-thread/
//              multi-process services.
//
//              The sole intent is to mimize host OS intrusion into the application.
//
//              Threads are implemented using the standard POSIX thread library (pthread)
//              but may be implemented using multple processes and shared memory equally well.
//
// $Date: 2013/12/02 19:16:57 $
//
// $Author: chuck $
//
// $Id: threads.c,v 1.2 2013/12/02 19:16:57 chuck Exp $
//
//
// ChangeLog:
//
// $Log: threads.c,v $
// Revision 1.2  2013/12/02 19:16:57  chuck
// added rcs_file_version identification string
//
// Revision 1.1  2013/11/30 18:02:47  chuck
// Initial revision
//
//
//
static char *rcs_file_verson = "$Id: threads.c,v 1.2 2013/12/02 19:16:57 chuck Exp $";
//

#include <stdio.h>
#include <strings.h>


// Local includes
#include "limits.h"
#include "threads.h"

// Start a new thread at the address pointed to by 'Module' and arguments pointed to by Args
//
// 'Args' is passed as given with no checking of any kind
//


// Private (static) variable for the very first invocation (internal initialization)
static int First = 1;

static int ThreadSelfInit()
{

  // Make sure the thread control blocks are zero 
  bzero(ThreadControlBlock,sizeof(ThreadInfo)*MAXTHREADS);


  // Initialize Tcb to point to ThreadControlBlock
  Tcb = ThreadControlBlock;

  // Clear the flag
  First = 0;
}


int ThreadStart( void *Start, void *Args)
{

  int   ThreadNum;


  // Init?
  if (First) ThreadSelfInit();

  // Find a slot, assign it, and go
  for (ThreadNum=1; ThreadNum<MAXTHREADS; ThreadNum++) {

    // If this slot is not in use, use it
    if ( !(Tcb[ThreadNum].State & ThreadRun) && !(Tcb[ThreadNum].State & ThreadDone) ) {

      // This is available, let's go
      Tcb[ThreadNum].Number  = ThreadNum;  // a formality
      Tcb[ThreadNum].State   = ThreadRun;
      Tcb[ThreadNum].Info    = (struct threadinfo *) &Tcb[ThreadNum];
      Tcb[ThreadNum].Arg     = Args;
      
      // Init the Attributes block
      pthread_attr_init(&Tcb[ThreadNum].Attr);

      // increase stack size
      pthread_attr_setstacksize (&Tcb[ThreadNum].Attr, 8388608);

      // Launch it
      pthread_create(  &Tcb[ThreadNum].ThreadId,
                       &Tcb[ThreadNum].Attr,
                       Start,
                       &Tcb[ThreadNum]              );

      // Mark this as active
      Tcb[ThreadNum].State   = ThreadRun;

      return ThreadNum;
    }
  }

  // No thread slots available, (Thread '0' is the main thread but doesn't need to be)
  return 0;
}

// Ask thread to finish and exit nicely
int ThreadFini(int ThreadNum)
{

  // Init?  Better not be
  if (First) return 0;

  // Range check thread number
  if (ThreadNum < 1 || ThreadNum >= MAXTHREADS) return 0; // not a valid thread number

  // Simply insert the Quit request.  The thread should watch this flag
  // ThreadCancel() coming soon

  Tcb[ThreadNum].State |= ThreadQuit;
}

int ThreadJoin(int ThreadNum)
{

  // Init?  Better not be!
  if (First) return;

  // Range check thread number
  if (ThreadNum < 1 || ThreadNum >= MAXTHREADS)  return 0;  // not a valid thread number

  // If it's listed as running or exited (we launched something in that slot)
  if ( (Tcb[ThreadNum].State & ThreadRun) || (Tcb[ThreadNum].State & ThreadDone) ) {

    // Join the thread (we WILL suspend if it's not yet exited)
    // accepting what it passes back
    pthread_join(Tcb[ThreadNum].ThreadId, &Tcb[ThreadNum].Arg);

    return (ThreadNum);
  }

  // Could not join
  return 0;
}

// Flag myself as exited and then pthread_exit()
int ThreadExit(void *ret)
{
  pthread_t  threadid = pthread_self();
  int        id;


  // find myself in the task list and set the flag
  for (id=0; id<MAXTHREADS; id++) {

    if (Tcb[id].ThreadId == threadid) {

      Tcb[id].State = ThreadDone;
      Tcb[id].Arg   = ret;

      // outta here!
      pthread_exit(ret);

    }
  }

  return -1 ;   // Caller is not a registered thread
}

// Simple test routines included below


#ifdef UNIT_TEST

#include <stdio.h>
#include <strings.h>


// -- This was/is the pthreads test code.  basis for threads.c library

// A dummy worker thread for education purposes only.  use your own real one.
void *WorkerTask( void *t )
{
  ThreadInfo       *tcb = (ThreadInfo *) t;
  int               i;
  char             *retstring = "@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  int  tid = tcb->Number;

  // inform the main I am here
  tcb->State = ThreadRun;

  sleep(1) ; // just to pause
  printf("Hello from Task id %d (%c)\n",tid,64+tid); fflush(stdout);

  // Passed a single string to print upon entry as the sole argument for test
  if (tcb->Arg) printf("%s\n",tcb->Arg);

  //printf("sleeping for %d seconds\n", tid+2);

  for (i=0; i< 50; i++) {
    printf("%c", 96+tid);  fflush(stdout);  // lowercase
    usleep(1000*(330+tid));
  }

  while (! (tcb->State & ThreadQuit) )  usleep (1000*(330+tid));

  // we are done
  tcb->State &= ~ThreadRun;
  tcb->State &= ~ThreadQuit;
  tcb->State &= ~ThreadDone;

  printf("Thread id %d exiting\n",tcb->Number);
  fflush(stdout);

  ThreadExit(&retstring[tid]);
  // pthread_exit(&retstring[tid]);  // ascii character code
}


main ()
{
  int i,id;
  int threadno[64], threads;
  char *ret, retchar;

  for (i=0; i<8; i++) {

    //printf("Starting thread %d...\n",i+1); 

    threadno[i] = ThreadStart(&WorkerTask,"I was told to say 'hello'");

    if (threadno[i]) printf("Started thread %d\n",threadno[i]);
  }

  // waste some time
  sleep(10);


  // ask every thread to exit
  for (i=0; i<8; i++) ThreadFini(threadno[i]);



  printf("letting everyone finish nicely\n");
  sleep(10);


  // Now join up with each one and grab their exit status
  for (i=1; i<9; i++) {

    printf("joining %d\n",Tcb[i].Number);
    pthread_join(Tcb[i].ThreadId, &Tcb[i].Arg); //ThreadJoin(threadno[i]);
    printf(".. status:  %8.8x\n",Tcb[i].State);
    ret = Tcb[i].Arg;
    retchar = *ret;

    printf(".. return:  %8.8x (%c)\n",Tcb[i].Arg, retchar);


  }

  // That's it, all done
  printf("Done.\n");
}

#endif //  UNIT_TEST
