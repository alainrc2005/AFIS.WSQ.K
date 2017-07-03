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

      FILE:    TABLEIO.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/24/1999
      UPDATED: 02/24/2005 by MDG
      UPDATED: 03/08/2005 by MDG

      Contains routines responsible for reading and writing the various
      tables and blocks used by the WSQ encoder/decoder.

      ROUTINES:
#cat: read_marker_wsq - Reads a specified WSQ marker from an open file.
#cat:
#cat: getc_marker_wsq - Reads a specified WSQ marker from a memory buffer.
#cat:
#cat: read_table_wsq - Reads a specified WSQ table from an open file.
#cat:
#cat: getc_table_wsq - Reads a specified WSQ table from a memory buffer.
#cat:
#cat: read_transform_table - Reads in a WSQ transform table from an
#cat:                   open file.
#cat: getc_transform_table - Reads in a WSQ transform table from a
#cat:                   memory buffer.
#cat: write_transform_table - Writes a WSQ transform table to an
#cat:                   open file.
#cat: putc_transform_table - Writes a WSQ transform table to a
#cat:                   memory buffer.
#cat: read_quantization_table - Reads a WSQ quantization table from an
#cat:                   open file.
#cat: getc_quantization_table - Reads a WSQ quantization table from a
#cat:                   memory buffer.
#cat: write_quantization_table - Writes a WSQ quantization table to an
#cat:                   open file.
#cat: putc_quantization_table - Writes a WSQ quantization table to a
#cat:                   memory buffer.
#cat: read_huffman_table_wsq - Reads a WSQ huffman table from an
#cat:                   open file.
#cat: getc_huffman_table_wsq - Reads a WSQ huffman table from a
#cat:                   memory buffer.
#cat: read_frame_header_wsq - Reads a WSQ Frame header from an open file.
#cat:
#cat: getc_frame_header_wsq - Reads a WSQ Frame header from a memory buffer.
#cat:
#cat: write_frame_header_wsq - Writes a WSQ Frame header to an open file.
#cat:
#cat: putc_frame_header_wsq - Writes a WSQ Frame header to a memory buffer.
#cat:
#cat: read_block_header - Reads a WSQ Block header from an open file.
#cat:
#cat: getc_block_header - Reads a WSQ Block header from a memory buffer.
#cat:
#cat: write_block_header - Writes a WSQ Block header to an open file.
#cat:
#cat: putc_block_header - Writes a WSQ Block header to a memory buffer.
#cat:
#cat: add_comment_wsq - Inserts a NISTCOM comment block into a
#cat:                   WSQ compressed datastream through an open file.
#cat: putc_nistcom_wsq - Inserts a NISTCOM comment block into a
#cat:                   WSQ compressed datastream through a memory buffer.
#cat: read_nistcom_wsq - Gets and returns the first NISTCOM comment block
#cat:                   in an open file.
#cat: getc_nistcom_wsq - Gets and returns the first NISTCOM comment block
#cat:                   in a memory buffer.
#cat: print_comments_wsq - Gets and prints the first NISTOCM comment block
#cat:                   in a WSQ compressed memory buffer to a specified
#cat:                   file pointer.

***********************************************************************/

#include "tableio.h"
#include "computil.h"
#include "dataio.h"
#include "swap.h"
#include "util.h"
#include <string.h>

/******************************************************/
/* Routine to read in WSQ markers from memory buffer. */
/******************************************************/
int getc_marker_wsq(
   unsigned short *omarker,  /* marker read */
   const int type,   /* type of markers that could be found */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned short marker;    /* WSQ marker */
	
   if((ret = getc_ushort(&marker, cbufptr, ebufptr)))
      return(ret);

   switch(type){
   case SOI_WSQ:
      if(marker != SOI_WSQ) {
         fprintf(stderr,
         "ERROR : getc_marker_wsq : No SOI marker. {%04X}\n", marker);
         return(-88);
      }
      break;
   case TBLS_N_SOF:
      if(marker != DTT_WSQ && marker != DQT_WSQ && marker != DHT_WSQ
         && marker != SOF_WSQ && marker != COM_WSQ) {
         fprintf(stderr,
         "ERROR : getc_marker_wsq : No SOF, Table, or comment markers.\n");
         return(-89);
      }
      break;
   case TBLS_N_SOB:
      if(marker != DTT_WSQ && marker != DQT_WSQ && marker != DHT_WSQ
         && marker != SOB_WSQ && marker != COM_WSQ) {
         fprintf(stderr,
         "ERROR : getc_marker_wsq : No SOB, Table, or comment markers.{%04X}\n",
                 marker);
         return(-90);
      }
      break;
   case ANY_WSQ:
      if((marker & 0xff00) != 0xff00){
	fprintf(stderr,"ERROR : getc_marker_wsq : no marker found {%04X}\n",
                marker);
         return(-91);
      }
      /* Added by MDG on 03-07-05 */
      if((marker < SOI_WSQ) || (marker > COM_WSQ)){
	fprintf(stderr,"ERROR : getc_marker_wsq : {%04X} not a valid marker\n",
                marker);
         return(-92);
      }
      break;
   default:
      fprintf(stderr,
      "ERROR : getc_marker_wsq : Invalid marker -> {%4X}\n", marker);
      return(-93);
   }

   *omarker = marker;
   return(0);
}

/*******************************************************/
/* Routine to read specified table from memory buffer. */
/*******************************************************/
int getc_table_wsq(
   unsigned short marker,         /* WSQ marker */
   DTT_TABLE *dtt_table,  /* transform table structure */
   DQT_TABLE *dqt_table,  /* quantization table structure */
   DHT_TABLE *dht_table,  /* huffman table structure */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr, /* end of input buffer */
   WSQContext *context)
{
   int ret;
   unsigned char *comment;

   switch(marker){
   case DTT_WSQ:
      if((ret = getc_transform_table(dtt_table, cbufptr, ebufptr)))
         return(ret);
      break;
   case DQT_WSQ:
      if((ret = getc_quantization_table(dqt_table, cbufptr, ebufptr)))
         return(ret);
      break;
   case DHT_WSQ:
      if((ret = getc_huffman_table_wsq(dht_table, cbufptr, ebufptr)))
         return(ret);
      break;
   case COM_WSQ:
      if((ret = getc_comment(&comment, cbufptr, ebufptr)))
         return(ret);
#ifdef PRINT_COMMENT
      fprintf(stderr, "COMMENT:\n%s\n\n", comment);
#endif
      free(comment);
      break;
   default:
      fprintf(stderr,"ERROR: getc_table_wsq : Invalid table defined -> {%u}\n",
              marker);
      return(-93);
   }

   return(0);
}

/*********************************************************************/
/* Routine to read in transform table parameters from memory buffer. */
/*********************************************************************/
int getc_transform_table(
   DTT_TABLE *dtt_table,  /* transform table structure */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned short hdr_size;              /* header size */
   float *a_lofilt, *a_hifilt;  /* unexpanded filter coefficients */
   unsigned char a_size;                 /* size of unexpanded coefficients */
   unsigned int cnt, shrt_dat;           /* counter and temp short data */
   unsigned char scale, sign;            /* scaling and sign parameters */


   if((ret = getc_ushort(&hdr_size, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&(dtt_table->hisz), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&(dtt_table->losz), cbufptr, ebufptr)))
      return(ret);


   /* Added 02-24-05 by MDG */
   /* If lofilt member previously allocated ... */
   if(dtt_table->lofilt != (float *)NULL){
      /* Deallocate the member prior to new allocation */
      free(dtt_table->lofilt);
      dtt_table->lofilt = (float *)NULL;
   }

   dtt_table->lofilt = (float *)calloc(dtt_table->losz,sizeof(float));
   if(dtt_table->lofilt == (float *)NULL) {
      fprintf(stderr,
      "ERROR : getc_transform_table : calloc : lofilt\n");
      return(-94);
   }

   /* Added 02-24-05 by MDG */
   /* If hifilt member previously allocated ... */
   if(dtt_table->hifilt != (float *)NULL){
      /* Deallocate the member prior to new allocation */
      free(dtt_table->hifilt);
      dtt_table->hifilt = (float *)NULL;
   }

   dtt_table->hifilt = (float *)calloc(dtt_table->hisz,sizeof(float));
   if(dtt_table->hifilt == (float *)NULL) {
      free(dtt_table->lofilt);
      fprintf(stderr,
      "ERROR : getc_transform_table : calloc : hifilt\n");
      return(-95);
   }

   if(dtt_table->hisz % 2)
      a_size = (dtt_table->hisz + 1) / 2;
   else
      a_size = dtt_table->hisz / 2;

   a_lofilt = (float *) calloc(a_size, sizeof(float));
   if(a_lofilt == (float *)NULL) {
      free(dtt_table->lofilt);
      free(dtt_table->hifilt);
      fprintf(stderr,
      "ERROR : getc_transform_table : calloc : a_lofilt\n");
      return(-96);
   }

   a_size--;
   for(cnt = 0; cnt <= a_size; cnt++) {
      if((ret = getc_byte(&sign, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_lofilt);
         return(ret);
      }
      if((ret = getc_byte(&scale, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_lofilt);
         return(ret);
      }
      if((ret = getc_uint(&shrt_dat, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_lofilt);
         return(ret);
      }
      a_lofilt[cnt] = (float)shrt_dat;
      while(scale > 0) {
         a_lofilt[cnt] /= 10.0;
         scale--;
      }
      if(sign != 0)
         a_lofilt[cnt] *= -1.0;


      if(dtt_table->hisz % 2) {
         dtt_table->hifilt[cnt + a_size] = (float)((float)int_sign(cnt)
                                         * a_lofilt[cnt]);
         if(cnt > 0)
            dtt_table->hifilt[a_size - cnt] = dtt_table->hifilt[cnt + a_size];
      }
      else {
         dtt_table->hifilt[cnt + a_size + 1] = (float)((float)int_sign(cnt)
                                             * a_lofilt[cnt]);
         dtt_table->hifilt[a_size - cnt] = -1.0 *
                                    dtt_table->hifilt[cnt + a_size + 1];
      }

   }
   free(a_lofilt);

   if(dtt_table->losz % 2)
      a_size = (dtt_table->losz + 1) / 2;
   else
      a_size = dtt_table->losz / 2;

   a_hifilt = (float *) calloc(a_size, sizeof(float));
   if(a_hifilt == (float *)NULL) {
      free(dtt_table->lofilt);
      free(dtt_table->hifilt);
      fprintf(stderr,
      "ERROR : getc_transform_table : calloc : a_hifilt\n");
      return(-97);
   }

   a_size--;
   for(cnt = 0; cnt <= a_size; cnt++) {
      if((ret = getc_byte(&sign, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_hifilt);
         return(ret);
      }
      if((ret = getc_byte(&scale, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_hifilt);
         return(ret);
      }
      if((ret = getc_uint(&shrt_dat, cbufptr, ebufptr))){
         free(dtt_table->lofilt);
         free(dtt_table->hifilt);
         free(a_hifilt);
         return(ret);
      }
      a_hifilt[cnt] = (float)shrt_dat;
      while(scale > 0) {
         a_hifilt[cnt] /= 10.0;
         scale--;
      }
      if(sign != 0)
         a_hifilt[cnt] *= -1.0;


      if(dtt_table->losz % 2) {
         dtt_table->lofilt[cnt + a_size] = (float)((float)int_sign(cnt)
                                         * a_hifilt[cnt]);
         if(cnt > 0)
            dtt_table->lofilt[a_size - cnt] = dtt_table->lofilt[cnt + a_size];
      }
      else {
         dtt_table->lofilt[cnt + a_size + 1] = (float)((float)int_sign(cnt+1)
                                             * a_hifilt[cnt]);
         dtt_table->lofilt[a_size - cnt] = dtt_table->lofilt[cnt + a_size + 1];
      }


   }
   free(a_hifilt);

   dtt_table->lodef = 1;
   dtt_table->hidef = 1;


   return(0);
}

/************************************************/
/* Stores transform table to the output buffer. */
/************************************************/
int putc_transform_table(
   float *lofilt,     /* filter coefficients      */
   const int losz,
   float *hifilt,
   const int hisz,
   unsigned char *odata,      /* output byte buffer       */
   const int oalloc,  /* allocated size of buffer */
   int   *olen)       /* filled length of buffer  */
{
   int ret;
   unsigned int coef;           /* filter coefficient indicator */
   unsigned int int_dat;        /* temp variable */
   float dbl_tmp;       /* temp variable */
   char scale_ex, sign; /* exponent scaling and sign parameters */

   if((ret = putc_ushort(DTT_WSQ, odata, oalloc, olen)))
      return(ret);
   /* table size */
   if((ret = putc_ushort(58, odata, oalloc, olen)))
      return(ret);
   /* number analysis lowpass coefficients */
   if((ret = putc_byte(losz, odata, oalloc, olen)))
      return(ret);
   /* number analysis highpass coefficients */
   if((ret = putc_byte(hisz, odata, oalloc, olen)))
      return(ret);

   for(coef = (losz>>1); coef < losz; coef++) {
      dbl_tmp = lofilt[coef];
      if(dbl_tmp >= 0.0) {
         sign = 0;
      }
      else {
         sign = 1;
         dbl_tmp *= -1.0;
      }
      scale_ex = 0;
      if(dbl_tmp == 0.0)
         int_dat = 0;
      else if(dbl_tmp < 4294967295.0) {
         while(dbl_tmp < 4294967295.0) {
            scale_ex += 1;
            dbl_tmp *= 10.0;
         }
         scale_ex -= 1;
         int_dat = (unsigned int)sround_uint(dbl_tmp / 10.0);
      }
      else {
         dbl_tmp = lofilt[coef];
         fprintf(stderr,
         "ERROR: putc_transform_table : lofilt[%d] to high at %f\n",
         coef, dbl_tmp);
         return(-82);
      }

      if((ret = putc_byte(sign, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_byte(scale_ex, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_uint(int_dat, odata, oalloc, olen)))
         return(ret);
   }

   for(coef = (hisz>>1); coef < hisz; coef++) {
      dbl_tmp = hifilt[coef];
      if(dbl_tmp >= 0.0) {
         sign = 0;
      }
      else {
         sign = 1;
         dbl_tmp *= -1.0;
      }
      scale_ex = 0;
      if(dbl_tmp == 0.0)
         int_dat = 0;
      else if(dbl_tmp < 4294967295.0) {
         while(dbl_tmp < 4294967295.0) {
            scale_ex += 1;
            dbl_tmp *= 10.0;
         }
         scale_ex -= 1;
         int_dat = (unsigned int)sround_uint(dbl_tmp / 10.0);
      }
      else {
         dbl_tmp = hifilt[coef];
         fprintf(stderr,
         "ERROR: putc_transform_table : hifilt[%d] to high at %f\n",
         coef, dbl_tmp);
         return(-83);
      }


      if((ret = putc_byte(sign, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_byte(scale_ex, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_uint(int_dat, odata, oalloc, olen)))
         return(ret);
   }

   return(0);
}

/************************************************************************/
/* Routine to read in quantization table parameters from memory buffer. */
/************************************************************************/
int getc_quantization_table(
   DQT_TABLE *dqt_table,  /* quatization table structure */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned short hdr_size;       /* header size */
   unsigned short cnt, shrt_dat;  /* counter and temp short data */
   unsigned char scale;           /* scaling parameter */

   if((ret = getc_ushort(&hdr_size, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&scale, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&shrt_dat, cbufptr, ebufptr)))
      return(ret);
   dqt_table->bin_center = (float)shrt_dat;
   while(scale > 0) {
      dqt_table->bin_center /= 10.0;
      scale--;
   }

   for(cnt = 0; cnt < 64; cnt++) {
      if((ret = getc_byte(&scale, cbufptr, ebufptr)))
         return(ret);
      if((ret = getc_ushort(&shrt_dat, cbufptr, ebufptr)))
         return(ret);
      dqt_table->q_bin[cnt] = (float)shrt_dat;
      while(scale > 0) {
         dqt_table->q_bin[cnt] /= 10.0;
         scale--;
      }
      if((ret = getc_byte(&scale, cbufptr, ebufptr)))
         return(ret);
      if((ret = getc_ushort(&shrt_dat, cbufptr, ebufptr)))
         return(ret);
      dqt_table->z_bin[cnt] = (float)shrt_dat;
      while(scale > 0) {
         dqt_table->z_bin[cnt] /= 10.0;
         scale--;
      }

   }
   dqt_table->dqt_def = 1;

   return(0);
}

/***************************************************/
/* Stores quantization table in the output buffer. */
/***************************************************/
int putc_quantization_table(
   QUANT_VALS *quant_vals,   /* quantization parameters  */
   unsigned char *odata,             /* output byte buffer       */
   const int oalloc,         /* allocated size of buffer */
   int   *olen)              /* filled length of buffer  */
{
   int ret, sub;               /* subband indicators */
   char scale_ex, scale_ex2;   /* exponent scaling parameters */
   unsigned short shrt_dat, shrt_dat2; /* temp variables */
   float flt_tmp;              /* temp variable */


   if((ret = putc_ushort(DQT_WSQ, odata, oalloc, olen)))
      return(ret);
   /* table size */
   if((ret = putc_ushort(389, odata, oalloc, olen)))
      return(ret);
   /* exponent scaling value */
   if((ret = putc_byte(2, odata, oalloc, olen)))
      return(ret);
   /* quantizer bin center parameter */
   if((ret = putc_ushort(44, odata, oalloc, olen)))
      return(ret);

   for(sub = 0; sub < 64; sub ++) {
      if(sub >= 0 && sub < 60) {
         if(quant_vals->qbss[sub] != 0.0) {
            flt_tmp = quant_vals->qbss[sub];
            scale_ex = 0;
            if(flt_tmp < 65535) {
               while(flt_tmp < 65535) {
                  scale_ex += 1;
                  flt_tmp *= 10;
               }
               scale_ex -= 1;
               shrt_dat = (unsigned short)sround(flt_tmp / 10.0);
            }
            else {
               flt_tmp = quant_vals->qbss[sub];
               fprintf(stderr,
               "ERROR : putc_quantization_table : Q[%d] to high at %f\n",
               sub, flt_tmp);
               return(-86);
            }

            flt_tmp = quant_vals->qzbs[sub];
            scale_ex2 = 0;
            if(flt_tmp < 65535) {
               while(flt_tmp < 65535) {
                  scale_ex2 += 1;
                  flt_tmp *= 10;
               }
               scale_ex2 -= 1;
               shrt_dat2 = (unsigned short)sround(flt_tmp / 10.0);
            }
            else {
               flt_tmp = quant_vals->qzbs[sub];
               fprintf(stderr,
               "ERROR : putc_quantization_table : Z[%d] to high at %f\n",
               sub, flt_tmp);
               return(-87);
            }
         }
         else {
            scale_ex = 0;
            scale_ex2 = 0;
            shrt_dat = 0;
            shrt_dat2 = 0;
         }
      }
      else {
         scale_ex = 0;
         scale_ex2 = 0;
         shrt_dat = 0;
         shrt_dat2 = 0;
      }

      if((ret = putc_byte(scale_ex, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_ushort(shrt_dat, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_byte(scale_ex2, odata, oalloc, olen)))
         return(ret);
      if((ret = putc_ushort(shrt_dat2, odata, oalloc, olen)))
         return(ret);
   }

   return(0);
}

/*******************************************************************/
/* Routine to read in huffman table parameters from memory buffer. */
/*******************************************************************/
int getc_huffman_table_wsq(
   DHT_TABLE *dht_table,  /* huffman table structure */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned char table_id;        /* huffman table indicator */
   unsigned char *huffbits;
   unsigned char *huffvalues;
   int bytes_left;

   /* First time, read table len. */
   if((ret = getc_huffman_table(&table_id, &huffbits, &huffvalues,
                               MAX_HUFFCOUNTS_WSQ, cbufptr, ebufptr,
                               READ_TABLE_LEN, &bytes_left)))
      return(ret);

   /* Store table into global structure list. */
   memcpy((dht_table+table_id)->huffbits, huffbits, MAX_HUFFBITS);
   memcpy((dht_table+table_id)->huffvalues, huffvalues,
          MAX_HUFFCOUNTS_WSQ+1);
   (dht_table+table_id)->tabdef = 1;
   free(huffbits);
   free(huffvalues);

   while(bytes_left){
      /* Read next table without rading table len. */
      if((ret = getc_huffman_table(&table_id, &huffbits, &huffvalues,
                                  MAX_HUFFCOUNTS_WSQ, cbufptr, ebufptr,
                                  NO_READ_TABLE_LEN, &bytes_left)))
         return(ret);

      /* If table is already defined ... */
      if((dht_table+table_id)->tabdef){
         free(huffbits);
         free(huffvalues);
         fprintf(stderr, "ERROR : getc_huffman_table_wsq : ");
         fprintf(stderr, "huffman table ID = %d already defined\n", table_id);
         return(-2);
      }

      /* Store table into global structure list. */
      memcpy((dht_table+table_id)->huffbits, huffbits, MAX_HUFFBITS);
      memcpy((dht_table+table_id)->huffvalues, huffvalues,
             MAX_HUFFCOUNTS_WSQ+1);
      (dht_table+table_id)->tabdef = 1;
      free(huffbits);
      free(huffvalues);
   }

   return(0);
}

/******************************************************************/
/* Routine to read in frame header parameters from memory buffer. */
/******************************************************************/
int getc_frame_header_wsq(
   FRM_HEADER_WSQ *frm_header,  /* frame header structure */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned short hdr_size, shrt_dat;  /* header size and data pointer */
   unsigned char scale;                /* exponent scaling parameter */

   if((ret = getc_ushort(&hdr_size, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&(frm_header->black), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&(frm_header->white), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&(frm_header->height), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&(frm_header->width), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(&scale, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&shrt_dat, cbufptr, ebufptr)))
      return(ret);
   frm_header->m_shift = (float) shrt_dat;
   while(scale > 0) {
      frm_header->m_shift /= 10.0;
      scale--;
   }
   if((ret = getc_byte(&scale, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&shrt_dat, cbufptr, ebufptr)))
      return(ret);
   frm_header->r_scale = (float) shrt_dat;
   while(scale > 0) {
      frm_header->r_scale /= 10.0;
      scale--;
   }

   if((ret = getc_byte(&(frm_header->wsq_encoder), cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_ushort(&(frm_header->software), cbufptr, ebufptr)))
      return(ret);

   return(0);
}

/*********************************************/
/* Stores frame header to the output buffer. */
/*********************************************/
int putc_frame_header_wsq(
   const int width,       /* image width              */
   const int height,      /* image height             */
   const float m_shift,   /* image shifting parameter */
   const float r_scale,   /* image scaling parameter  */
   unsigned char *odata,          /* output byte buffer       */
   const int oalloc,      /* allocated size of buffer */
   int   *olen)           /* filled length of buffer  */
{
   int ret;
   float flt_tmp;         /* temp variable */
   char scale_ex;         /* exponent scaling parameter */
   unsigned short shrt_dat;       /* temp variable */

   if((ret = putc_ushort(SOF_WSQ, odata, oalloc, olen)))
      return(ret);
   /* size of frame header */
   if((ret = putc_ushort(17, odata, oalloc, olen)))
      return(ret);
   /* black pixel */
   if((ret = putc_byte(0, odata, oalloc, olen)))
      return(ret);
   /* white pixel */
   if((ret = putc_byte(255, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_ushort(height, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_ushort(width, odata, oalloc, olen)))
      return(ret);

   flt_tmp = m_shift;
   scale_ex = 0;
   if(flt_tmp != 0.0) {
      while(flt_tmp < 65535) {
         scale_ex += 1;
         flt_tmp *= 10;
      }
      scale_ex -= 1;
      shrt_dat = (unsigned short)sround(flt_tmp / 10.0);
   }
   else
      shrt_dat = 0;
   if((ret = putc_byte(scale_ex, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_ushort(shrt_dat, odata, oalloc, olen)))
      return(ret);

   flt_tmp = r_scale;
   scale_ex = 0;
   if(flt_tmp != 0.0) {
      while(flt_tmp < 65535) {
         scale_ex += 1;
         flt_tmp *= 10;
      }
      scale_ex -= 1;
      shrt_dat = (unsigned short)sround(flt_tmp / 10.0);
   }
   else
      shrt_dat = 0;
   if((ret = putc_byte(scale_ex, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_ushort(shrt_dat, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_byte(0, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_ushort(0, odata, oalloc, olen)))
      return(ret);

   return(0);
}

/******************************************************************/
/* Routine to read in block header parameters from memory buffer. */
/******************************************************************/
int getc_block_header(
   unsigned char *huff_table,   /* huffman table indicator */
   unsigned char **cbufptr,  /* current byte in input buffer */
   unsigned char *ebufptr)   /* end of input buffer */
{
   int ret;
   unsigned short hdr_size;     /* block header size */

   if((ret = getc_ushort(&hdr_size, cbufptr, ebufptr)))
      return(ret);
   if((ret = getc_byte(huff_table, cbufptr, ebufptr)))
      return(ret);

   return(0);
}

/*********************************************/
/* Stores block header to the output buffer. */
/*********************************************/
int putc_block_header(
   const int table,   /* huffman table indicator  */
   unsigned char *odata,      /* output byte buffer       */
   const int oalloc,  /* allocated size of buffer */
   int   *olen)       /* filled length of buffer  */
{
   int ret;

   if((ret = putc_ushort(SOB_WSQ, odata, oalloc, olen)))
      return(ret);
   /* block header size */
   if((ret = putc_ushort(3, odata, oalloc, olen)))
      return(ret);
   if((ret = putc_byte((unsigned char)table, odata, oalloc, olen)))
      return(ret);

   return(0);
}

/*******************************************/
int add_comment_wsq(unsigned char **ocdata, int *oclen, unsigned char *idata,
                    const int ilen, unsigned char *comment)
{
   int ret, nlen, nalloc;
   unsigned short marker;
   unsigned char *ndata, *cbufptr, *ebufptr;
   unsigned char *ocomment;

   if((comment == (unsigned char *)NULL) ||
      (strlen((char *)comment) == 0)){
      fprintf(stderr,
             "ERROR : add_comment_wsq : empty comment passed\n");
      return(-2);
   }

   /* New compressed byte stream length including:                */
   /*    orig byte strem + marker + header length + comment bytes */
   nalloc = ilen + (sizeof(unsigned short) << 1) + strlen((char *)comment);
   /* Initialize current filled length to 0. */
   nlen = 0;

   /* Allocate new compressed byte stream. */
   if((ndata = (unsigned char *)malloc(nalloc * sizeof(unsigned char)))
             == (unsigned char *)NULL){
      fprintf(stderr, "ERROR : add_comment_wsq : malloc : ndata\n");
      return(-3);
   }
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Parse idata and determine comment destination in byte stream. */
   
   /* Parse SOI */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }

   /* Copy SOI */
   if((ret = putc_ushort(marker, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   /* Read next marker. */
   if((ret = getc_ushort(&marker, &cbufptr, ebufptr))){
      free(ndata);
      return(ret);
   }

   /* If COM segment ... */
   if (marker == COM_WSQ){
      /* Do ... while COM segments exist in input byte stream ... */
      do{
         /* Read COM_WSQ segment. */
         if((ret = getc_comment(&ocomment, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }
         /* Copy COM segment. */
         if((ret = putc_comment(COM_WSQ, ocomment, strlen((char *)ocomment),
                                ndata, nalloc, &nlen))){
            free(ndata);
            free(ocomment);
            return(ret);
         }
         free(ocomment);
         /* Read next marker. */
         if((ret = getc_ushort(&marker, &cbufptr, ebufptr))){
            free(ndata);
            return(ret);
         }
      }while(marker == COM_WSQ);
   }

   /* Back up to start of last marker read. */
   cbufptr -= sizeof(unsigned short);

   /* Insert Comment Segment */
   if((ret = putc_comment(COM_WSQ, comment, strlen((char *)comment),
                         ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   /* Append remaining byte stream */
   if((ret = putc_bytes(cbufptr, ebufptr - cbufptr, ndata, nalloc, &nlen))){
      free(ndata);
      return(ret);
   }

   *ocdata = ndata;
   *oclen = nalloc;

   return(0);
}

/*******************************************/
int putc_nistcom_wsq(char *comment_text, const int w, const int h,
                     const int d, const int ppi, const int lossyflag,
                     const float r_bitrate,
                     unsigned char *odata, const int oalloc, int *olen)
{
   int ret, gencomflag;
   NISTCOM *nistcom;
   char *comstr;

   /* Add Comment(s) here. */
   nistcom = (NISTCOM *)NULL;
   gencomflag = 0;
   if(comment_text != (char *)NULL){
      /* if NISTCOM ... */
      if(strncmp(comment_text, NCM_HEADER, strlen(NCM_HEADER)) == 0){
         if((ret = string2fet(&nistcom, comment_text))){
            return(ret);
         }
      }
      /* If general comment ... */
      else{
         gencomflag = 1;
      }
   }
   /* Otherwise, no comment passed ... */

   /* Combine image attributes to NISTCOM. */
   if((ret = combine_wsq_nistcom(&nistcom, w, h, d, ppi, lossyflag, r_bitrate))){
      if(nistcom != (NISTCOM *)NULL)
         freefet(nistcom);
      return(ret);
   }

   /* Put NISTCOM ... */
   /* NISTCOM to string. */
   if((ret = fet2string(&comstr, nistcom))){
      freefet(nistcom);
      return(ret);
   }
   /* Put NISTCOM comment string. */
   if((ret = putc_comment(COM_WSQ, (unsigned char *)comstr, strlen(comstr),
                        odata, oalloc, olen))){
      freefet(nistcom);
      free(comstr);
      return(ret);
   }
   freefet(nistcom);
   free(comstr);

   /* If general comment exists ... */
   if(gencomflag){
      /* Put general comment to its own segment. */
      if((ret = putc_comment(COM_WSQ, (unsigned char *)comment_text,
                             strlen(comment_text), odata, oalloc, olen)))
         return(ret);
   }

   return(0);
}

/*****************************************************************/
/* Get and return first NISTCOM from encoded data stream.        */
/*****************************************************************/
int getc_nistcom_wsq(NISTCOM **onistcom, unsigned char *idata,
                        const int ilen)
{
   int ret;
   unsigned short marker;
   unsigned char *cbufptr, *ebufptr;
   NISTCOM *nistcom;
   char *comment_text;
   unsigned char *ucharptr;

   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Get SOI */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr)))
      return(ret);

   /* Get next marker. */
   if((ret = getc_marker_wsq(&marker, ANY_WSQ, &cbufptr, ebufptr)))
      return(ret);

   /* While not at Start of Block (SOB) -     */
   /*    the start of encoded image data ... */
   while(marker != SOB_WSQ){
      if(marker == COM_WSQ){
         if(strncmp((char *)cbufptr+2 /* skip Length */,
                    NCM_HEADER, strlen(NCM_HEADER)) == 0){
            if((ret = getc_comment(&ucharptr, &cbufptr,
                                   ebufptr)))
               return(ret);
            comment_text = (char *)ucharptr;
            if((ret = string2fet(&nistcom, comment_text))){
			   free(comment_text);
               return(ret);
			}
            *onistcom = nistcom;
			free(comment_text);
            return(0);
         }
      }
      /* Skip marker segment. */
      if((ret = getc_skip_marker_segment(marker, &cbufptr, ebufptr)))
         return(ret);
      /* Get next marker. */
      if((ret = getc_marker_wsq(&marker, ANY_WSQ, &cbufptr, ebufptr)))
         return(ret);
   }

   /* NISTCOM not found ... */
   *onistcom = (NISTCOM *)NULL;
   return(0);
}
