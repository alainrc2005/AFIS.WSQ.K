/*
 * tableio.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef TABLEIO_H_
#define TABLEIO_H_

#include "wsqInternal.h"

/* tableio.c */
int getc_marker_wsq(unsigned short *, const int, unsigned char **, unsigned char *);
int getc_table_wsq(unsigned short, DTT_TABLE *, DQT_TABLE *, DHT_TABLE *, unsigned char **, unsigned char *, WSQContext *);
int getc_transform_table(DTT_TABLE *, unsigned char **, unsigned char *);
int putc_transform_table(float *, const int, float *, const int, unsigned char *, const int, int *);
int getc_quantization_table(DQT_TABLE *, unsigned char **, unsigned char *);
int putc_quantization_table(QUANT_VALS *, unsigned char *, const int, int *);
int getc_huffman_table_wsq(DHT_TABLE *, unsigned char **, unsigned char *);
int getc_frame_header_wsq(FRM_HEADER_WSQ *, unsigned char **, unsigned char *);
int putc_frame_header_wsq(const int, const int, const float, const float, unsigned char *, const int, int *);
int getc_block_header(unsigned char *, unsigned char **, unsigned char *);
int putc_block_header(const int, unsigned char *, const int, int *);
int add_comment_wsq(unsigned char **, int *, unsigned char *, const int, unsigned char *);
int putc_nistcom_wsq(char *, const int, const int, const int, const int, const int, const float, unsigned char *, const int,
		int *);
int getc_nistcom_wsq(NISTCOM **, unsigned char *, const int);

#endif /* TABLEIO_H_ */
