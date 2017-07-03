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

#ifndef _FET_H
#define _FET_H

#include "syserr.h"

#define FET_EXT		"fet"
#define MAXFETS		100
#define MAXFETLENGTH	512

typedef struct fetstruct{
   int alloc;
   int num;
   char **names;
   char **values;
} FET;

/* allocfet.c */
FET  *allocfet(int);
int  allocfet_ret(FET **, int);
FET  *reallocfet(FET *, int);
int  reallocfet_ret(FET **, int);
/* delfet.c */
void deletefet(char *, FET *);
int  deletefet_ret(char *, FET *);
/* extfet.c */
char *extractfet(char *, FET *);
int  extractfet_ret(char **, char *, FET *);
/* freefet.c */
void freefet(FET *);
/* lkupfet.c */
int  lookupfet(char **, char *, FET *);
/* printfet.c */
void printfet(FILE *, FET *);
/* strfet.c */
int fet2string(char **, FET *);
int string2fet(FET **, char *);
/* updatfet.c */
void updatefet(char *, char *, FET *);
int  updatefet_ret(char *, char *, FET *);

#endif  /* !_FET_H */
