//
//  gcd.h -- inline header-file function to compute Greatest Common Divisor of A and B
//
// $RCSfile: gcd.h,v $
//
// $Date: 2013/12/16 21:24:19 $
//
// $Author: chuck $
//
// $Id: gcd.h,v 1.1 2013/12/16 21:24:19 chuck Exp chuck $
//
//
// ChangeLog:
//
// $Log: gcd.h,v $
// Revision 1.1  2013/12/16 21:24:19  chuck
// Initial revision
//
//
//
//
#ifndef _havegcdh_
#define _havegcdh_

static inline gcd(register int a, register int b)
{
  register int c;

  while (a) {  c = a; a = b%a ; b = c; }

  return b;
}

#define UNIT_TEST
#ifdef UNIT_TEST
#include <stdio.h>
main()
{
 
#endif // _havegcdh_
