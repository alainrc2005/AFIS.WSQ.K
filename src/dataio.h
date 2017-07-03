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

#ifndef _DATA_IO_H
#define _DATA_IO_H

/* dataio.c */
int read_byte(unsigned char *, FILE *);
int getc_byte(unsigned char *, unsigned char **, unsigned char *);
int getc_bytes(unsigned char **, const int, unsigned char **,
                 unsigned char *);
int write_byte(const unsigned char, FILE *);
int putc_byte(const unsigned char, unsigned char *, const int, int *);
int putc_bytes(unsigned char *, const int, unsigned char *,
                 const int, int *);
int read_ushort(unsigned short *, FILE *);
int getc_ushort(unsigned short *, unsigned char **, unsigned char *);
int write_ushort(unsigned short, FILE *);
int putc_ushort(unsigned short, unsigned char *, const int, int *);
int read_uint(unsigned int *, FILE *);
int getc_uint(unsigned int *, unsigned char **, unsigned char *);
int write_uint(unsigned int, FILE *);
int putc_uint(unsigned int, unsigned char *, const int, int *);
void write_bits(unsigned char **, const unsigned short, const short,
                 int *, unsigned char *, int *);
void flush_bits(unsigned char **, int *, unsigned char *, int *);

#endif /* !_DATA_IO_H */
