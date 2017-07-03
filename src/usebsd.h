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
      FILE:    USEBSD.H
      AUTHOR:  Michael Garris
      DATE:    06/09/2005

      Controls definitions to facilitate the portable use of
      BSD-based library routines on both Linux and Cygwin platforms.
      This file must be included when code is calling for example:

         strdup()
         rindex()
         setlinebuf()

      and the strict ANSI flag "-ansi" is used.  The including of
      this file is intended to have no effect when the "-ansi"
      compiler flag is not in use.

***********************************************************************/
#ifndef _USEBSD_H
#define _USEBSD_H

#if defined(__linux__) && !defined(_BSD_SOURCE)
#define _BSD_SOURCE
#elif defined(__CYGWIN__) && defined(__STRICT_ANSI__)
#undef __STRICT_ANSI__
#endif

/* The setting of _BSD_SOURCE under Linux sets up subsequent */
/* definitions in <stdio.h>.  Therefore this file should be  */
/* included first, so to help ensure this, <stdio.h> is      */
/* included here at the end of this file */
#include <stdio.h>

#endif /* !_USEBSD_H */
