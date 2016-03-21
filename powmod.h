//
// powmod.h	Define prototypes and macros used for powmod, mulmod, and powmulmod
//              modular exponentiation
//
//
// ChangeLog:
//
// $Log: powmod.h,v $
// Revision 1.5  2013/12/11 22:08:00  chuck
// updated to support new inline structure
// ,
//
// Revision 1.4  2013/11/21 02:54:14  chuck
// added mulmod, mulmod32 (generic 2^32 limit), mulmod64 (full width)
//
// Revision 1.3  2013/11/15 20:58:39  chuck
// Added 128bit support using a P<2^32 version (64 bits) and a P>2^32 version.
// Renamed functions to reflect new naming.
// Moved all operations to registers where possible.
//
// Revision 1.2  2013/10/21 06:11:09  chuck
// Added RCS header info
//
//


#ifndef _HAVE_POWMOD_H__
#define _HAVE_POWMOD_H__

#include <inttypes.h>
#include <math.h>

uint64_t (*powmod)(uint32_t, uint32_t, uint64_t);   // runtime indirect powmod() mechanism


// Use powmodtwo & powmodgeneric when P < 2^32 for speed
uint64_t powmodtwo          ( unsigned int, unsigned int, uint64_t);
uint64_t powmodgeneric      ( unsigned int, unsigned int, uint64_t);

// Use powmodtwo128 & powmodgeneric128 when P > 2^32 (to avoid losing upper bits in squaring)
// (this is currently very slow in gcc but will improve)
uint64_t powmodtwo128       ( unsigned int, unsigned int, uint64_t);
uint64_t powmodgeneric128   ( unsigned int, unsigned int, uint64_t);

#ifdef INLINE_MULMOD

  static uint64_t mulmod             ( uint64_t, uint64_t, uint64_t);
  static uint64_t mulmod32           ( uint64_t, uint64_t, uint64_t);
  static uint64_t mulmod64           ( uint64_t, uint64_t, uint64_t);

#else
 
  uint64_t mulmod             ( uint64_t, uint64_t, uint64_t); 
  uint64_t mulmod32           ( uint64_t, uint64_t, uint64_t);
  uint64_t mulmod64           ( uint64_t, uint64_t, uint64_t);

#endif // INLINE_MULMOD

#endif //_HAVE_POWMOD_H__

