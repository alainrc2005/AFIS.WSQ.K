/*
 * wsq.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef WSQ_H_
#define WSQ_H_

#include "Config.h"
#include "wsqInternal.h"
/***************************************************************************
****************************************************************************
   WSQ Decoder routine.  Takes an WSQ compressed memory buffer and decodes
   it, returning the reconstructed pixmap.

 Input
  ps    - WSQ information data
  ilen  - size of WSQ
  context - context WSQ library for multi-process thread
 Output
  w     - image width
  h     - image height
  depth - bits per pixel (8)
  ppi   - pixel per inch
  odata - image pointer

************************************************************************/
EXTERNC int API WSQToRawImage(unsigned char * ps, const int ilen, int* w ,int* h, int* depth, int* ppi, unsigned char** odata, WSQContext *context);

EXTERNC int API WSQGetDimensions(unsigned char *ps, const int ilen, int *w ,int *h, WSQContext *context);

/***************************************************************************
****************************************************************************
 WSQ encodes/compresses an image pixmap.

 Input
  ps    - image pointer
  w     - image width
  h     - image height
  context - context WSQ library for multi-process thread
 Output
  depth - bits per pixel (8)
  ppi   - pixel per inch
  odata - compressed data buffer WSQ
  size  - compressed data buffer length

************************************************************************/
EXTERNC int API RawImageToWSQ(unsigned char * ps, int w, int h, int* size, unsigned char* odata, WSQContext *context);

/***************************************************************************
****************************************************************************
 Create context for multi-process thread

 Return code
  unsigned long - WSQContext* pointer

****************************************************************************/
EXTERNC API  WSQContext* WSQCreateContext(void);

/***************************************************************************
****************************************************************************
 Free context

Input
 context - context to free

****************************************************************************/
EXTERNC void API WSQFreeContext(WSQContext *context);

#if PLATFORM_WIN32 || PLATFORM_WIN64
 typedef enum _PixelFormatType
  {
 	 Format32bppArgb = 0x26200a,
 	 Format32bppRgb = 0x22009,
	 Format24bppRgb = 0x21808
  } PixelFormatType;

EXTERNC void API FastBitmapToRaw(PixelFormatType pixelformat, int width, int height, int stride, unsigned char *scan, unsigned char *barray);
EXTERNC void API FastRawToBitmap(int width, int height, int stride, unsigned char *scan, unsigned char *barray);
#endif

#endif /* WSQ_H_ */
