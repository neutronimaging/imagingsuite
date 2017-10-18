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

#ifndef __JPEG_WRAP_H__
#define __JPEG_WRAP_H__

namespace soleil
{

extern "C"
{
  #undef FAR
  #include "jpeglib.h"
}

#ifdef __WIN32__
  #define CONVCALL __cdecl
#else
  #define CONVCALL
#endif

//==============================================================================
/// Class JpegEncoder
/// Class that take a MemBuxEx and a output file name to encode a grayscale jpeg
//==============================================================================
class JpegEncoder
{
public:

  //---------------------------------------------------------------------------
  /// Jpeg encoder exception
  //---------------------------------------------------------------------------
  class Exception : public ExceptionBase
  {
    protected:
      const char *ErrorTitle()
      {
        return "Jpeg encoder exception";
      }

    public:
      Exception(const char *pcszError, const char *pcszReason, const char *pcszMethod):
      ExceptionBase(pcszError, pcszReason, pcszMethod)
      { }
  };

  //---------------------------------------------------------------------------
  /// DestMemBuf
  /// manage output to a MemBuf object
  //---------------------------------------------------------------------------
  class DestMemBuf
  {
    private:
      /// Destination buffer for libjpeg encoder
      CMemBuf m_mbEncoderDest;

      /// Reference to the final destination
      MemBufPtr m_ptrmbOutput;

      /// libjpeg structure who maintain pointer to next output byte 
      /// and free bytes in buffer
      struct jpeg_destination_mgr *m_pDestinationManager;

      /// Method called by init_destination_wrapper
      void InitDestination(struct jpeg_compress_struct * cinfo);

      /// Method called by empty_output_buffer_wrapper
      int EmptyOutputBuffer(struct jpeg_compress_struct * cinfo);

      /// Method called by term_destination_wrapper
      void TermDestination(struct jpeg_compress_struct * cinfo);

      /// Method called by libjpeg at compressor initialization
      static void CONVCALL init_destination_wrapper(struct jpeg_compress_struct * cinfo);

      /// Method called by libjpeg when output buffer is full
#ifdef __WIN32__
      static uint8 CONVCALL empty_output_buffer_wrapper(struct jpeg_compress_struct * cinfo);
#else
      static int CONVCALL empty_output_buffer_wrapper(struct jpeg_compress_struct * cinfo);
#endif

      /// Method called by libjpeg when compressor has finished
      static void CONVCALL term_destination_wrapper(struct jpeg_compress_struct * cinfo);

    public:
      /// Constructor
      /// @param iBufferSize destination buffer size on bytes
      DestMemBuf(MemBufPtr mbOuput, int iBufferSize = 65536);
      
      /// Accessors
      jpeg_destination_mgr *libjpegStruct() { return m_pDestinationManager; }
  };

private:
  ///# In a multithread environment, we should use a lock for controlling the access to this map
  static map<struct jpeg_compress_struct *, class DestMemBuf *> s_mapCinfoToOutput;

  /// Method called to perform generic initialization of the compressor
  static void InitCompressor(MemBufPtr ptrmbSrc, struct jpeg_compress_struct * cinfo);

  /// Method called to perform generic initialization of the compressor
  static void Compress(MemBufPtr ptrmbSrc, struct jpeg_compress_struct * cinfo);

public:
  JpegEncoder();

  /// Methode called from JpegEncoder::DestMemBuf static callback wrappers class to retreive
  /// right instance of JpegEncoder::DestMemBuf object
  static DestMemBuf *GetDestBuf(struct jpeg_compress_struct * cinfo);

  static void EncodeGrayScaleToFile(MemBufPtr ptrmbSrc, const String &strOutputFile);
  static void EncodeGrayScaleToMemBuf(MemBufPtr ptrmbSrc, MemBufPtr ptrmbDst);
};

} // namespace

#endif
