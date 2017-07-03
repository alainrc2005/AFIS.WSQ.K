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
      LIBRARY: WSQ - Grayscale Image Compression

      FILE:    HUFF.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/02/1999

      Checks that huffman codes are WSQ compliant. The specification
      does not allow for an all 1's code in the code table.

      ROUTINES:
#cat: check_huffcodes_wsq - Checks for an all 1's code in the code table.

***********************************************************************/

#include "huff.h"

int check_huffcodes_wsq(HUFFCODE *hufftable, int last_size)
{
   int i, k;
   int all_ones;

   for(i = 0; i < last_size; i++){
      all_ones = 1;
      for(k = 0; (k < (hufftable+i)->size) && all_ones; k++)
         all_ones = (all_ones && (((hufftable+i)->code >> k) & 0x0001));
      if(all_ones) {
         fprintf(stderr, "WARNING: A code in the hufftable contains an ");
         fprintf(stderr, "all 1's code.\n         This image may still be ");
         fprintf(stderr, "decodable.\n         It is not compliant with ");
         fprintf(stderr, "the WSQ specification.\n");
         return(-1);
      }
   }
   return(0);
}
