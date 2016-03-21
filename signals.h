//
//  signals.h  --  The global variables associated with signals and timers (the flags)
//                 in addition to the normal 'exit' flag since the thread treats this
//                 as a 'soft' signal.
//
// $RCSfile: signals.h,v $
//
// $Date: 2013/11/30 17:57:15 $
//
// $Author: chuck $
//
// $Id: signals.h,v 1.1 2013/11/30 17:57:15 chuck Exp $
//
//
// ChangeLog:
//
// $Log: signals.h,v $
// Revision 1.1  2013/11/30 17:57:15  chuck
// Initial revision
//
//
//
#ifndef _havesievesignalsh_
#define _havesievesignalsh_

// Autosave alarm interval counter.
// We actually save every 'Autosave' (user-defined or AutosaveDefault) intervals
  int  AutosaveInterval;
  int  DeltaP;

// Autosave Worker control (remember, no formatted I/O in an interrupt handler)
  int  NeedSave;
  int  NeedStats;

#endif
