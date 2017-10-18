//*****************************************************************************
// Synchrotron SOLEIL
//
//
// Creation : 01/12/2006
// Author   : Stephane Poirier
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

#ifndef __BMP_H__
#define __BMP_H__

namespace gdshare
{


#ifdef __WIN32__
  #define CONVCALL __cdecl
#else
  #define CONVCALL
#endif

//==============================================================================
/// Class BmpEncoder
/// Class that take a MemBuxEx and a output file name to encode a grayscale bmp
//==============================================================================
class BmpEncoder
{
public:

  //---------------------------------------------------------------------------
  /// Bmp encoder exception
  //---------------------------------------------------------------------------
  class Exception : public ExceptionBase
  {
    protected:
      const char *ErrorTitle()
      {
        return "Bmp encoder exception";
      }

    public:
      Exception(const char *pcszError, const char *pcszReason, const char *pcszMethod):
      ExceptionBase(pcszError, pcszReason, pcszMethod)
      { }
  };

private:
  static void EncodeHeader(MemBufPtr ptrmbDst, int iBitDepth, int iWidth, int iHeight, int *piBytePaddingPerRow);

public:
  BmpEncoder();

  static void EncodeGrayScaleToMemBuf(MemBufPtr ptrmbSrc, MemBufPtr ptrmbDst);
  static void EncodeGrayScaleToFile(MemBufPtr ptrMembuf, const String &strOutputFile);
};

} // namespace

#endif
