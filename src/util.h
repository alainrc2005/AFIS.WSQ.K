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

#ifndef _UTIL_H
#define _UTIL_H

#include "wsqInternal.h"

/* UPDATED: 03/15/2005 by MDG */


void conv_img_2_flt(float *, float *, float *, unsigned char *,
                 const int);
void conv_img_2_uchar(unsigned char *, float *, const int, const int,
                 const float, const float);
void variance( QUANT_VALS *quant_vals, Q_TREE q_tree[], const int,
                 float *, const int, const int);
int quantize(short **, int *, QUANT_VALS *, Q_TREE qtree[], const int,
                 float *, const int, const int);
void quant_block_sizes(int *, int *, int *,
                 QUANT_VALS *, W_TREE w_tree[], const int,
                 Q_TREE q_tree[], const int);
int unquantize(float **, const DQT_TABLE *,
                 Q_TREE q_tree[], const int, short *, const int, const int);
int wsq_decompose(float *, const int, const int,
                 W_TREE w_tree[], const int, float *, const int,
                 float *, const int);
void get_lets(float *, float *, const int, const int, const int,
                 const int, float *, const int, float *, const int, const int);
int wsq_reconstruct(float *, const int, const int,
                 W_TREE w_tree[], const int, const DTT_TABLE *);
void  join_lets(float *, float *, const int, const int,
                 const int, const int, float *, const int,
                 float *, const int, const int);
int int_sign(const int);
int image_size(const int, short *, short *);
void init_wsq_decoder_resources(WSQContext *);
void free_wsq_decoder_resources(WSQContext *);

int delete_comments_wsq(unsigned char **, int *, unsigned char *, int);
#endif /* !_UTIL_H */
