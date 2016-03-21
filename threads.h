//
// threads.h  -- Thread support structures & variables
//
// $RCSfile: threads.h,v $
//
// $Date: 2013/11/30 17:57:15 $
//
// $Author: chuck $
//
// $Id: threads.h,v 1.1 2013/11/30 17:57:15 chuck Exp $
//
//
// ChangeLog:
//
// $Log: threads.h,v $
// Revision 1.1  2013/11/30 17:57:15  chuck
// Initial revision
//
//
//

#ifndef _havesievethreadsh_
#define _havesievethreadsh_

#ifndef _PTHREAD_H
#include <pthread.h>
#endif

#ifndef _BITS_PTHREADTYPES_H
#include <bits/pthreadtypes.h>
#endif

// Thread state definitions (should be enums but define is better)

#define ThreadRun      0x0001   // Thread running/runnable
#define ThreadSleep    0x0002   // Thread simply sleeping (sleep() or usleep())
#define ThreadPend     0x0004   // Thread is Waiting on i/o or other async external
#define ThreadQuit     0x0008   // Thread is requested to finish
#define ThreadUsr1     0x0010   // Thread User Flag 1
#define ThreadUsr2     0x0020   // Thread User Flag 2
#define ThreadLock     0x0040   // Thread waiting for a lockbox (mutex or semaphore)
#define ThreadDone     0x0080   // Thread is done.


  // Thread Control Block (Tcb) a.k.a. ThreadInfo

typedef struct threadinfo {
  pthread_t          ThreadId;     // Thread id assigned by pthread_create
  int                Number;       // Number we assigned to it
  unsigned int       State;        // Bits defined to report the current state of the thread
  pthread_attr_t     Attr;         // Thread attributes
  struct threadinfo *Info;         // Pointer to my info (this is passed as 'Arg' )
  void              *Arg;          // void pointer to whatever we want to pass it or get back
} ThreadInfo;



  ThreadInfo    ThreadControlBlock[MAXTHREADS];
  ThreadInfo   *Tcb; // = ThreadControlBlock; // TCB is pointer to the control block


// Function prototypes
  int ThreadStart(void *, void *);
  int ThreadExit (void *);
  int ThreadFini(int);
  int ThreadJoin(int);
  int ThreadStat(int);
  int ThreadKill(int);
  int ThreadWait(int);



#endif // _havesievethreadsh_
