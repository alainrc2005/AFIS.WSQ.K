/*******************************************************************************

License: 
This software was developed at the National Institute of Standards and 
Technology (NIST) by employees of the Federal Government in the course 
of their official duties. Pursuant to title 17 Section 105 of the 
United States Code, this software is not subject to copyright protection 
and is in the public domain. NIST assumes no responsibility  whatsoever for 
its use by other parties, and makes no guarantees, expressed or implied, 
about its quality, reliability, or any other characteristic. 

Disclaimer: 
This software was developed to promote biometric standards and biometric
technology testing for the Federal Government in accordance with the USA
PATRIOT Act and the Enhanced Border Security and Visa Entry Reform Act.
Specific hardware and software products identified in this software were used
in order to perform the software development.  In no case does such
identification imply recommendation or endorsement by the National Institute
of Standards and Technology, nor does it imply that the products and equipment
identified are necessarily the best available for the purpose.  

*******************************************************************************/

/***********************************************************************
      LIBRARY: UTIL - General Purpose Utility Routines

      FILE:    SYSERR.C
      AUTHOR:  Michael Garris
      DATE:    12/19/1990
      UPDATED: 04/25/2005 by MDG

      Contains routines responsible for exiting upon an system error.

      ROUTINES:
#cat: syserr - exits on error with a status of -1, printing to stderr a
#cat:          caller-defined message.

***********************************************************************/

/* LINTLIBRARY */

#include "syserr.h"

void syserr(char *funcname, char *syscall, char *msg)
{

   (void) fflush(stdout);
   if(msg == NULL)
      (void) fprintf(stderr,"ERROR: %s: %s\n",funcname,syscall);
   else
      (void) fprintf(stderr,"ERROR: %s: %s: %s\n",funcname,syscall,msg);
   (void) fflush(stderr);

   exit(-1);
}

void fatalerr(char *s1, char *s2, char *s3)
{

  (void) fflush(stdout);
   if (s2 == (char *) NULL)
	(void) fprintf(stderr,"ERROR: %s\n",s1);
   else if (s3 == (char *) NULL)
	(void) fprintf(stderr,"ERROR: %s: %s\n",s1,s2);
   else
	(void) fprintf(stderr,"ERROR: %s: %s: %s\n",s1,s2,s3);
   (void) fflush(stderr);

   exit(1);
}
