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

      FILE:    DECODER.C
      AUTHORS: Craig Watson
               Michael Garris
      DATE:    12/02/1999
      UPDATED: 02/24/2005 by MDG

      Contains routines responsible for decoding a WSQ compressed
      datastream.

      ROUTINES:
#cat: wsq_decode_mem - Decodes a datastream of WSQ compressed bytes
#cat:                  from a memory buffer, returning a lossy
#cat:                  reconstructed pixmap.
#cat: huffman_decode_data_mem - Decodes a block of huffman encoded
#cat:                  data from a memory buffer.
#cat: huffman_decode_data_file - Decodes a block of huffman encoded
#cat:                  data from an open file.
#cat: decode_data_mem - Decodes huffman encoded data from a memory buffer.
#cat:
#cat: decode_data_file - Decodes huffman encoded data from an open file.
#cat:
#cat: nextbits_wsq - Gets next sequence of bits for data decoding from
#cat:                    an open file.
#cat: getc_nextbits_wsq - Gets next sequence of bits for data decoding
#cat:                    from a memory buffer.

***********************************************************************/

#include "decoder.h"
#include "tree.h"
#include "huff.h"
#include "dataio.h"

int wsq_get_dimensions(unsigned char *idata, const int ilen, int *ow, int *oh, WSQContext *context)
{
  int ret, i;
  unsigned short marker;         /* WSQ marker */
  unsigned char *cbufptr;        /* points to current byte in buffer */
  unsigned char *ebufptr;        /* points to end of buffer */

   /* Added by MDG on 02-24-05 */
   init_wsq_decoder_resources(context);

   /* Set memory buffer pointers. */
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Init DHT Tables to 0. */
   for(i = 0; i < MAX_DHT_TABLES; i++)
      (context->dht_table + i)->tabdef = 0;

   /* Read the SOI marker. */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }

   /* Read in supporting tables up to the SOF marker. */
   if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = getc_table_wsq(marker, &context->dtt_table, &context->dqt_table, context->dht_table,
                          &cbufptr, ebufptr, context))){
         free_wsq_decoder_resources(context);
         return(ret);
      }
      if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
         free_wsq_decoder_resources(context);
         return(ret);
      }
   }

   /* Read in the Frame Header. */
   if((ret = getc_frame_header_wsq(&context->frm_header_wsq, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }
   *ow = context->frm_header_wsq.width;
   *oh = context->frm_header_wsq.height;
   free_wsq_decoder_resources(context);
   return 0;
}
/************************************************************************/
/*              This is an implementation based on the Crinimal         */
/*              Justice Information Services (CJIS) document            */
/*              "WSQ Gray-scale Fingerprint Compression                 */
/*              Specification", Dec. 1997.                              */
/***************************************************************************/
/* WSQ Decoder routine.  Takes an WSQ compressed memory buffer and decodes */
/* it, returning the reconstructed pixmap.                                 */
/***************************************************************************/
int wsq_decode_mem(unsigned char *odata, int *ow, int *oh, int *od, int *oppi,
                   int *lossyflag, unsigned char *idata, const int ilen, WSQContext * context)
{
   int ret, i;
   unsigned short marker;         /* WSQ marker */
   int num_pix;                   /* image size and counter */
   int width, height, ppi;        /* image parameters */
   float *fdata;                  /* image pointers */
   short *qdata;                  /* image pointers */
   unsigned char *cbufptr;        /* points to current byte in buffer */
   unsigned char *ebufptr;        /* points to end of buffer */

   /* Added by MDG on 02-24-05 */
   init_wsq_decoder_resources(context);

   /* Set memory buffer pointers. */
   cbufptr = idata;
   ebufptr = idata + ilen;

   /* Init DHT Tables to 0. */
   for(i = 0; i < MAX_DHT_TABLES; i++)
      (context->dht_table + i)->tabdef = 0;

   /* Read the SOI marker. */
   if((ret = getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }

   /* Read in supporting tables up to the SOF marker. */
   if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }
   while(marker != SOF_WSQ) {
      if((ret = getc_table_wsq(marker, &context->dtt_table, &context->dqt_table, context->dht_table,
                          &cbufptr, ebufptr, context))){
         free_wsq_decoder_resources(context);
         return(ret);
      }
      if((ret = getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))){
         free_wsq_decoder_resources(context);
         return(ret);
      }
   }

   /* Read in the Frame Header. */
   if((ret = getc_frame_header_wsq(&context->frm_header_wsq, &cbufptr, ebufptr))){
      free_wsq_decoder_resources(context);
      return(ret);
   }
   width = context->frm_header_wsq.width;
   height = context->frm_header_wsq.height;
   num_pix = width * height;

   if((ret = getc_ppi_wsq(&ppi, idata, ilen))){
      free_wsq_decoder_resources(context);
      return(ret);
   }


   /* Build WSQ decomposition trees. */
   build_wsq_trees(context->w_tree, W_TREELEN, context->q_tree, Q_TREELEN, width, height);


   /* Allocate working memory. */
   qdata = (short *) malloc(num_pix * sizeof(short));
   if(qdata == (short *)NULL) {
      fprintf(stderr,"ERROR: wsq_decode_mem : malloc : qdata1\n");
      free_wsq_decoder_resources(context);
      return(-20);
   }
   /* Decode the Huffman encoded data blocks. */
   if((ret = huffman_decode_data_mem(qdata, &context->dtt_table, &context->dqt_table, context->dht_table,
                                    &cbufptr, ebufptr, context))){
      free(qdata);
      free_wsq_decoder_resources(context);
      return(ret);
   }

   /* Decode the quantize wavelet subband data. */
   if((ret = unquantize(&fdata, &context->dqt_table, context->q_tree, Q_TREELEN,
                         qdata, width, height))){
      free(qdata);
      free_wsq_decoder_resources(context);
      return(ret);
   }

   /* Done with quantized wavelet subband data. */
   free(qdata);

   if((ret = wsq_reconstruct(fdata, width, height, context->w_tree, W_TREELEN,
                              &context->dtt_table))){
      free(fdata);
      free_wsq_decoder_resources(context);
      return(ret);
   }

   /* Convert floating point pixels to unsigned char pixels. */
   conv_img_2_uchar(odata, fdata, width, height,
                      context->frm_header_wsq.m_shift, context->frm_header_wsq.r_scale);

   /* Done with floating point pixels. */
   free(fdata);

   /* Added by MDG on 02-24-05 */
   free_wsq_decoder_resources(context);

   /* Assign reconstructed pixmap and attributes to output pointers. */
   *ow = width;
   *oh = height;
   *od = 8;
   *oppi = ppi;
   *lossyflag = 1;

   /* Return normally. */
   return(0);
}

/***************************************************************************/
/* Routine to decode an entire "block" of encoded data from memory buffer. */
/***************************************************************************/
int huffman_decode_data_mem(
   short *ip,               /* image pointer */
   DTT_TABLE *dtt_table,    /*transform table pointer */
   DQT_TABLE *dqt_table,    /* quantization table */
   DHT_TABLE *dht_table,    /* huffman table */
   unsigned char **cbufptr, /* points to current byte in input buffer */
   unsigned char *ebufptr,  /* points to end of input buffer */
   WSQContext *context)
{
   int ret;
   int blk = 0;           /* block number */
   unsigned short marker; /* WSQ markers */
   int bit_count;         /* bit count for getc_nextbits_wsq routine */
   int n;                 /* zero run count */
   int nodeptr;           /* pointers for decoding */
   int last_size;         /* last huffvalue */
   unsigned char hufftable_id;    /* huffman table number */
   HUFFCODE *hufftable;   /* huffman code structure */
   int maxcode[MAX_HUFFBITS+1]; /* used in decoding data */
   int mincode[MAX_HUFFBITS+1]; /* used in decoding data */
   int valptr[MAX_HUFFBITS+1];     /* used in decoding data */
   unsigned short tbits;


   if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, cbufptr, ebufptr)))
      return(ret);

   bit_count = 0;

   while(marker != EOI_WSQ) {

      if(marker != 0) {
         blk++;
         while(marker != SOB_WSQ) {
            if((ret = getc_table_wsq(marker, dtt_table, dqt_table,
                                dht_table, cbufptr, ebufptr, context)))
               return(ret);
            if((ret = getc_marker_wsq(&marker, TBLS_N_SOB, cbufptr, ebufptr)))
               return(ret);
         }
         if((ret = getc_block_header(&hufftable_id, cbufptr, ebufptr)))
            return(ret);

         if((dht_table+hufftable_id)->tabdef != 1) {
            fprintf(stderr, "ERROR : huffman_decode_data_mem : ");
            fprintf(stderr, "huffman table {%d} undefined.\n", hufftable_id);
            return(-51);
         }

         /* the next two routines reconstruct the huffman tables */
         if((ret = build_huffsizes(&hufftable, &last_size,
                                  (dht_table+hufftable_id)->huffbits,
                                  MAX_HUFFCOUNTS_WSQ)))
            return(ret);

         build_huffcodes(hufftable);
         if((ret = check_huffcodes_wsq(hufftable, last_size)))
            fprintf(stderr, "         hufftable_id = %d\n", hufftable_id);

         /* this routine builds a set of three tables used in decoding */
         /* the compressed data*/
         gen_decode_table(hufftable, maxcode, mincode, valptr,
                          (dht_table+hufftable_id)->huffbits);
         free(hufftable);
         bit_count = 0;
         marker = 0;
      }

      /* get next huffman category code from compressed input data stream */
      if((ret = decode_data_mem(&nodeptr, mincode, maxcode, valptr,
                            (dht_table+hufftable_id)->huffvalues,
                            cbufptr, ebufptr, &bit_count, &marker, context)))
         return(ret);

      if(nodeptr == -1) {
         while(marker == COM_WSQ && blk == 3) {
            if((ret = getc_table_wsq(marker, dtt_table, dqt_table,
                                dht_table, cbufptr, ebufptr, context)))
               return(ret);
            if((ret = getc_marker_wsq(&marker, ANY_WSQ, cbufptr, ebufptr)))
               return(ret);
         }
         continue;
      }

      if(nodeptr > 0 && nodeptr <= 100)
         for(n = 0; n < nodeptr; n++) {
            *ip++ = 0; /* z run */
         }
      else if(nodeptr > 106 && nodeptr < 0xff)
         *ip++ = nodeptr - 180;
      else if(nodeptr == 101){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8, context)))
            return(ret);
         *ip++ = tbits;
      }
      else if(nodeptr == 102){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8, context)))
            return(ret);
         *ip++ = -tbits;
      }
      else if(nodeptr == 103){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16, context)))
            return(ret);
         *ip++ = tbits;
      }
      else if(nodeptr == 104){
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16, context)))
            return(ret);
         *ip++ = -tbits;
      }
      else if(nodeptr == 105) {
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 8, context)))
            return(ret);
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else if(nodeptr == 106) {
         if((ret = getc_nextbits_wsq(&tbits, &marker, cbufptr, ebufptr,
                                &bit_count, 16, context)))
            return(ret);
         n = tbits;
         while(n--)
            *ip++ = 0;
      }
      else {
         fprintf(stderr,
                "ERROR: huffman_decode_data_mem : Invalid code %d (%x).\n",
                nodeptr, nodeptr);
         return(-52);
      }

   }

   return(0);
}

/**********************************************************/
/* Routine to decode the encoded data from memory buffer. */
/**********************************************************/
int decode_data_mem(
   int *onodeptr,       /* returned huffman code category        */
   int *mincode,        /* points to minimum code value for      */
                        /*    a given code length                */
   int *maxcode,        /* points to maximum code value for      */
                        /*    a given code length                */
   int *valptr,         /* points to first code in the huffman   */
                        /*    code table for a given code length */
   unsigned char *huffvalues,   /* defines order of huffman code          */
                                /*    lengths in relation to code sizes   */
   unsigned char **cbufptr,     /* points to current byte in input buffer */
   unsigned char *ebufptr,      /* points to end of input buffer          */
   int *bit_count,      /* marks the bit to receive from the input byte */
   unsigned short *marker,
   WSQContext *context)
{
   int ret;
   int inx, inx2;       /*increment variables*/
   unsigned short code, tbits;  /* becomes a huffman code word
                                   (one bit at a time)*/

   if((ret = getc_nextbits_wsq(&code, marker, cbufptr, ebufptr, bit_count, 1, context)))
      return(ret);

   if(*marker != 0){
      *onodeptr = -1;
      return(0);
   }

   for(inx = 1; (int)code > maxcode[inx]; inx++) {
      if((ret = getc_nextbits_wsq(&tbits, marker, cbufptr, ebufptr, bit_count, 1, context)))
         return(ret);

      code = (code << 1) + tbits;
      if(*marker != 0){
         *onodeptr = -1;
         return(0);
      }
   }
   inx2 = valptr[inx];
   inx2 = inx2 + code - mincode[inx];

   *onodeptr = huffvalues[inx2];
   return(0);
}

/****************************************************************/
/* Routine to get nextbit(s) of data stream from memory buffer. */
/****************************************************************/
int getc_nextbits_wsq(
   unsigned short *obits,       /* returned bits */
   unsigned short *marker,      /* returned marker */
   unsigned char **cbufptr,     /* points to current byte in input buffer */
   unsigned char *ebufptr,      /* points to end of input buffer */
   int *bit_count,      /* marks the bit to receive from the input byte */
   const int bits_req,  /* points to end of input buffer */
   WSQContext *context)
{
   int ret;
   unsigned short bits, tbits;  /*bits of current data byte requested*/
   int bits_needed;     /*additional bits required to finish request*/

                              /*used to "mask out" n number of
                                bits from data stream*/
   static unsigned char bit_mask[9] = {0x00,0x01,0x03,0x07,0x0f,
                                       0x1f,0x3f,0x7f,0xff};
   if(*bit_count == 0) {
      if((ret = getc_byte(&context->code, cbufptr, ebufptr))){
         return(ret);
      }
      *bit_count = 8;
      if(context->code == 0xFF) {
         if((ret = getc_byte(&context->code2, cbufptr, ebufptr))){
            return(ret);
         }
         if(context->code2 != 0x00 && bits_req == 1) {
            *marker = (context->code << 8) | context->code2;
            *obits = 1;
            return(0);
         }
         if(context->code2 != 0x00) {
            fprintf(stderr, "ERROR: getc_nextbits_wsq : No stuffed zeros\n");
            return(-41);
         }
      }
   }
   if(bits_req <= *bit_count) {
      bits = (context->code >>(*bit_count - bits_req)) & (bit_mask[bits_req]);
      *bit_count -= bits_req;
      context->code &= bit_mask[*bit_count];
   }
   else {
      bits_needed = bits_req - *bit_count;
      bits = context->code << bits_needed;
      *bit_count = 0;
      if((ret = getc_nextbits_wsq(&tbits, (unsigned short *)NULL, cbufptr,
                             ebufptr, bit_count, bits_needed, context)))
         return(ret);
      bits |= tbits;
   }

   *obits = bits;
   return(0);
}
