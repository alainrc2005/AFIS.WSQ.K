/*
 * encoder.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "wsqInternal.h"

/* encoder.c */
int wsq_encode_mem(unsigned char *, int *, unsigned char *, int ,
				   int, int, int, WSQContext *);
int gen_hufftable_wsq(HUFFCODE **, unsigned char **, unsigned char **,
                 short *, const int *, const int);
int compress_block(unsigned char *, int *, short *,
                 const int, const int, const int, HUFFCODE *);
int count_block(int **, const int, short *,
                 const int, const int, const int);

#endif /* ENCODER_H_ */
