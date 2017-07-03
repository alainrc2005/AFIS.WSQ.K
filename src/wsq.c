/*
 * wsq.c
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#include "wsq.h"
#include "decoder.h"
#include "encoder.h"

int WSQToRawImage(unsigned char * ps, const int ilen, int* w ,int* h, int* depth, int* ppi, unsigned char* odata, WSQContext *context)
{
  int lossyflag;
	return wsq_decode_mem(odata, w, h, depth, ppi, &lossyflag, ps, ilen, context);
}

int WSQGetDimensions(unsigned char *ps, const int ilen, int *w ,int *h, WSQContext *context)
{
	return wsq_get_dimensions(ps, ilen, w, h, context);
}

int RawImageToWSQ(unsigned char * ps, int w, int h, int* size, unsigned char* odata, WSQContext *context)
{
	return wsq_encode_mem(odata, size, ps, w, h, 8, 0, context);
}

WSQContext *WSQCreateContext(void)
{
	WSQContext *context;
	context = malloc(sizeof(WSQContext));
	if (!context) return 0;
	return context;
}

void WSQFreeContext(WSQContext *context)
{
	if (context) free(context);
}

#if PLATFORM_WIN32 || PLATFORM_WIN64
void FastBitmapToRaw(PixelFormatType pixelformat, int width, int height, int stride, unsigned char *scan, unsigned char *barray)
{
	int step, offset, x, y;
	step = ((pixelformat == Format32bppArgb) ||
		(pixelformat == Format32bppRgb) ? 4 :
		(pixelformat == Format24bppRgb) ? 3 : 1);
	offset = stride - width * step;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(barray++) = *scan;
			scan += step;
		}
		scan += offset;
	}
}

#define CopyMem(dst, src, count) memcpy(dst, src, count)
#define CopyRow(type, dstRow, srcRow, length) CopyMem(dstRow, srcRow, (length) * sizeof(type))
#define CopyByteRow(dstRow, srcRow, length) CopyRow(unsigned char, dstRow, srcRow, length)

void FastRawToBitmap(int width, int height, int stride, unsigned char *scan, unsigned char *barray)
{
	int offset, y;
	offset = stride - width;
	for (y = 0; y < height; y++)
	{
		CopyByteRow(scan, barray, width);
		scan += width + offset;
		barray += width;
	}
}
#endif
