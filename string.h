//
//
// Define local functions useful when handling arbitary user input with unknown format
//
// $RCSfile: string.h,v $
//
// $Date: 2013/11/21 04:18:41 $
//
// $Author: chuck $
//
// $Id: string.h,v 1.4 2013/11/21 04:18:41 chuck Exp $
//
//
// ChangeLog:
//
// $Log: string.h,v $
// Revision 1.4  2013/11/21 04:18:41  chuck
// corrected duplicate include deficiency
//
// Revision 1.3  2013/11/10 00:50:59  chuck
// updated per glibc
//
// Revision 1.2  2013/10/21 14:48:43  chuck
// updated header
//
// Revision 1.1  2013/10/21 14:46:27  chuck
// Initial revision
//
//
//


	int   nocrlf(char *);

	int   locase(char *);

	int   upcase(char *);

	char *ltrim(char *);

	char *rtrim(char *);
