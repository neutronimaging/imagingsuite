//*****************************************************************************
// Synchrotron SOLEIL
//
//
// Creation : 01/12/2006
// Author   : S. Poirier
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; version 2 of the License.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//*****************************************************************************

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "base.h"
#include "file.h"
#include "membuf.h"
#include "bmp.h"

#define BMP_MAX_DIMENSION 65536

using namespace gdshare;
//=============================================================================
// BmpEncoder
//=============================================================================

//------------------------------------------------------------------------
// BmpEncoder::EncodeGrayScaleToFile
//------------------------------------------------------------------------
void BmpEncoder::EncodeGrayScaleToFile(MemBufPtr ptrMembuf, const String &strOutputFile)
{
  // Create output file
  FILE * outfile;
  if ((outfile = fopen(PSZ(strOutputFile), "wb")) == NULL) 
  {
	  fprintf(stderr, "can't open %s\n", PSZ(strOutputFile));
	  exit(1);
	}
  
  // Encode into a MemBuf object
  MemBufPtr ptrDestBuf(new MemBuf);
  EncodeGrayScaleToMemBuf(ptrMembuf, ptrDestBuf);

  // flush Data to the file
  fwrite(ptrDestBuf->Buf(), 1, ptrDestBuf->Len(), outfile);

  // Close output file
  fclose(outfile);
}

//------------------------------------------------------------------------
// BmpEncoder::EncodeHeader
//------------------------------------------------------------------------
void BmpEncoder::EncodeHeader(MemBufPtr ptrmbDst, int iBitDepth, int iWidth, int iHeight, int *piBytePaddingPerRow)
{
  int iBytesPerPixel = iBitDepth / 8;
  int iBytesPerRow = iBytesPerPixel * iWidth;

  *piBytePaddingPerRow = (4 - iBytesPerRow % 4) % 4;
  int iActualBytesPerRow = iBytesPerRow + *piBytePaddingPerRow;

  int iTotalPixelBytes = iHeight * iActualBytesPerRow;

  int iPaletteSize = 0;
  if( iBitDepth == 8 )
    iPaletteSize = (1 << iBitDepth) * 4;

  // leave some room for 16-bit masks 
  if( iBitDepth == 16 )
    iPaletteSize = 3*4;

  int iTotalFileSize = 14 + 40 + iPaletteSize + iTotalPixelBytes;
  int iOffBits = 14 + 40 + iPaletteSize;
  
  // Encode file header
  (*ptrmbDst) << 'B';                  // file marker
  (*ptrmbDst) << 'M';                  // file marker
  (*ptrmbDst) << (long)iTotalFileSize;  // Size
  (*ptrmbDst) << (short)0;              // Reserved1
  (*ptrmbDst) << (short)0;              // Reserved2
  (*ptrmbDst) << (long)iOffBits;        // Offset bits

  // Encode info header
  (*ptrmbDst) << (long)40;              // Header size
  (*ptrmbDst) << (long)iWidth;          // Image width
  (*ptrmbDst) << (long)iHeight;         // Image height
  (*ptrmbDst) << (short)1;              // Planes
  (*ptrmbDst) << (short)iBitDepth;      // Bit count per pixel
  (*ptrmbDst) << (long)0;               // Compression
  (*ptrmbDst) << (long)iTotalPixelBytes;// Image size
  (*ptrmbDst) << (long)3780;            // X pixels per meter
  (*ptrmbDst) << (long)3780;            // Y pixels per meter
  (*ptrmbDst) << (long)0;               // Colors used
  (*ptrmbDst) << (long)0;               // Important colors

  // Create B&W palette
  for( int i = 0; i < 256; i++ )
  {
    (*ptrmbDst) << (uchar)i;               // Red
    (*ptrmbDst) << (uchar)i;               // Green
    (*ptrmbDst) << (uchar)i;               // Blue
    (*ptrmbDst) << (uchar)0;               // Alpha
  }
}

//------------------------------------------------------------------------
// BmpEncoder::EncodeGrayScaleToMemBuf
//------------------------------------------------------------------------
void BmpEncoder::EncodeGrayScaleToMemBuf(MemBufPtr ptrmbSrc, MemBufPtr ptrmbDst)
{
  int iWidth, iHeight;
  try
  {
    ptrmbSrc->GetIntegerMetadata("width", &iWidth);
    ptrmbSrc->GetIntegerMetadata("height", &iHeight);
  }
  catch( NoDataException e )
  {
    // Transform exception into a jpeg exception
    throw BmpEncoder::Exception(PSZ(e.Error()), PSZ(e.Reason()), "BmpEncoder::EncodeGrayScaleToMemBuf");
  }

  if( iWidth < 1 || iWidth > BMP_MAX_DIMENSION )
    throw Exception("BAD_PARAMETER", "Wrongs image dimensions. Must be in [1, 65000]", "BmpEncoder::EncodeGrayScaleToMemBuf");

  int iBytePaddingPerRow = 0;
  EncodeHeader(ptrmbDst, 8, iWidth, iHeight, &iBytePaddingPerRow);

  // Image data
  for( int y = 0; y < iHeight; y++ )
  {
    ptrmbDst->PutBloc((uint8 *)(ptrmbSrc->Buf()) + (iHeight - y - 1) * iWidth, iWidth);
    for( int i = 0; i < iBytePaddingPerRow; i++ )
      (*ptrmbDst) << (uint8)0;
  }
}

