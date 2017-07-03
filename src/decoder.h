/*
 * decoder.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef DECODER_H_
#define DECODER_H_

#include "util.h"
#include "tableio.h"
#include "ppi.h"

/* decoder.c */
int wsq_decode_mem(unsigned char *odata, int *ow, int *oh, int *od, int *oppi,
                   int *lossyflag, unsigned char *idata, const int ilen, WSQContext * context);
int huffman_decode_data_mem(short *ip, DTT_TABLE *dtt_table, DQT_TABLE *dqt_table,
                            DHT_TABLE *dht_table, unsigned char **cbufptr, unsigned char *ebufptr,
                            WSQContext *context);
int decode_data_mem(int *onodeptr, int *mincode, int *maxcode, int *valptr,
   unsigned char *huffvalues, unsigned char **cbufptr, unsigned char *ebufptr,
   int *bit_count, unsigned short *marker, WSQContext *context);
int getc_nextbits_wsq(unsigned short *obits, unsigned short *marker,
   unsigned char **cbufptr, unsigned char *ebufptr, int *bit_count,
   const int bits_req, WSQContext *context);
int wsq_get_dimensions(unsigned char *idata, const int ilen, int *ow, int *oh, WSQContext *context);
#endif /* DECODER_H_ */
