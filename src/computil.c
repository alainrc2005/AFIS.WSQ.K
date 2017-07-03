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

      FILE:    COMPUTIL.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/24/1999
      UPDATED: 04/25/2005 by MDG

      Contains general purpose routines responsible for processing
      compression algorithm markers in a compressed datastream.

      ROUTINES:
#cat: read_skip_marker_segment - skips the segment data following a
#cat:           JPEGB, JPEGL, or WSQ marker in the open filestream.
#cat: getc_skip_marker_segment - skips the segment data following a
#cat:           JPEGB, JPEGL, or WSQ marker in the given memory buffer.

***********************************************************************/
#include <stdio.h>
#include "dataio.h"
#include "computil.h"

/*****************************************************************/
/* Skips the segment data following a JPEGB, JPEGL, or WSQ       */
/* marker in the open filestream.                                */
/*****************************************************************/
int read_skip_marker_segment(const unsigned short marker, FILE *infp)
{
   int ret;
   unsigned short length;

   /* Get ushort Length. */
   if((ret = read_ushort(&length, infp)))
      return(ret);

   length -= 2;

   /* Bump file pointer forward. */
   if(fseek(infp, length, SEEK_CUR) < 0){
      fprintf(stderr, "ERROR : read_skip_marker_segment : ");
      fprintf(stderr, "unable to advance file pointer to skip ");
      fprintf(stderr, "marker %d segment of length %d\n", marker, length);
      return(-2);
   }

   return(0);
}

/*****************************************************************/
/* Skips the segment data following a JPEGB, JPEGL, or WSQ       */
/* marker in the given memory buffer.                            */
/*****************************************************************/
int getc_skip_marker_segment(const unsigned short marker,
                     unsigned char **cbufptr, unsigned char *ebufptr)
{
   int ret;
   unsigned short length;

   /* Get ushort Length. */
   if((ret = getc_ushort(&length, cbufptr, ebufptr)))
      return(ret);

   length -= 2;

   /* Check for EOB ... */
   if(((*cbufptr)+length) >= ebufptr){
      fprintf(stderr, "ERROR : getc_skip_marker_segment : ");
      fprintf(stderr, "unexpected end of buffer when parsing ");
      fprintf(stderr, "marker %d segment of length %d\n", marker, length);
      return(-2);
   }

   /* Bump buffer pointer. */
   (*cbufptr) += length;

   return(0);
}

