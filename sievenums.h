//
// SieveEnum -- (sievenum.h) -- Enum specifications to keep gcc happy
//
// These are done with #define to avoid the double init hassle in nesting
// inside includes which use them
//
//
// $RCSfile: sievenums.h,v $
//
// $Date: 2013/11/21 19:46:18 $
//
// $Author: chuck $
//
// $Id: sievenums.h,v 1.4 2013/11/21 19:46:18 chuck Exp $
//
//
// ChangeLog:
//
// $Log: sievenums.h,v $
// Revision 1.4  2013/11/21 19:46:18  chuck
// Added support for 'E' notation in Pmin & Pmax.  Now may be 2E9 == 2 * 10^9
//
// Revision 1.3  2013/11/21 04:18:41  chuck
// corrected duplicate enums when included multiple times
//
// Revision 1.2  2013/11/11 03:07:26  chuck
// completed merge and initialization/naming misalignment.
//
// Revision 1.1  2013/11/11 02:32:28  chuck
// Initial revision
//
//
//

#ifndef _HAVE_SIEVE_ENUMS_H
#define _HAVE_SIEVE_ENUMS_H

enum fmtspec {null, string, intger, lngint, bool, powr};

#endif
