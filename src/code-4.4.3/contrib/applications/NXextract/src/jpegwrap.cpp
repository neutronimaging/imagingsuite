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

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "base.h"
#include "file.h"
#include "membuf.h"
#include "jpegwrap.h"

using namespace soleil;
//=============================================================================
// JpegEncoder
//=============================================================================

/// Declaration of static map between jpeg_compress_struct and DestMemBuf object pointers
map<struct jpeg_compress_struct *, class JpegEncoder::DestMemBuf *> JpegEncoder::s_mapCinfoToOutput;

//------------------------------------------------------------------------
// JpegEncoder::EncodeGrayScaleToFile
//------------------------------------------------------------------------
void JpegEncoder::EncodeGrayScaleToFile(MemBufPtr ptrMembuf, const String &strOutputFile)
{
  // Allocate jpeg structures
  struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);

  // Create output file
  FILE * outfile;
  if ((outfile = fopen(PSZ(strOutputFile), "wb")) == NULL) 
  {
	  fprintf(stderr, "can't open %s\n", PSZ(strOutputFile));
	  exit(1);
	}
  
  // Generic initialization
  InitCompressor(ptrMembuf, &cinfo);

  // comress to file
  jpeg_stdio_dest(&cinfo, outfile);

  // Do the compression
  Compress(ptrMembuf, &cinfo);

  // Close output file
  fclose(outfile);
}

//------------------------------------------------------------------------
// JpegEncoder::EncodeGrayScaleToMemBuf
//------------------------------------------------------------------------
void JpegEncoder::EncodeGrayScaleToMemBuf(MemBufPtr ptrmbSrc, MemBufPtr ptrmbDst)
{
  // Allocate jpeg structures
  struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);

  // Generic initialization
  InitCompressor(ptrmbSrc, &cinfo);

  // Prepare encoder destination
  JpegEncoder::DestMemBuf mbEncoderDest(ptrmbDst);
  cinfo.dest = mbEncoderDest.libjpegStruct();

  // Add Destination buffer object to static map
  s_mapCinfoToOutput[&cinfo] = &mbEncoderDest;

  // Do the compression
  Compress(ptrmbSrc, &cinfo);

  // Remove Destination buffer object from map
  map<struct jpeg_compress_struct *,  class DestMemBuf *>::iterator it;
  it = s_mapCinfoToOutput.find(&cinfo);
  if( it != s_mapCinfoToOutput.end() )
    s_mapCinfoToOutput.erase(it);
  else
    throw JpegEncoder::Exception("INVALID_STATE", "Destination buffer not found", "JpegEncoder::EncodeGrayScaleToMemBuf");
}

//------------------------------------------------------------------------
// JpegEncoder::InitCompressor
//------------------------------------------------------------------------
void JpegEncoder::InitCompressor(MemBufPtr ptrmbSrc, struct jpeg_compress_struct *pcinfo)
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
    throw JpegEncoder::Exception(PSZ(e.Error()), PSZ(e.Reason()), "JpegEncoder::InitCompressor");
  }

  if( iWidth < 1 ||iWidth > JPEG_MAX_DIMENSION )
    throw JpegEncoder::Exception("BAD_PARAMETER", "Wrongs image dimensions. Must be in [1, 65000]", "JpegEncoder::Encode");

  // create context
  jpeg_create_compress(pcinfo);

  // Put cinfo values
  pcinfo->image_width = iWidth;
  pcinfo->image_height = iHeight;
  pcinfo->input_components = 1; // grayscale image
  pcinfo->in_color_space = JCS_GRAYSCALE;

  // Set compression parameters to default values
  jpeg_set_defaults(pcinfo);

  // Set quality to max
  jpeg_set_quality(pcinfo, 100, TRUE);
}

//------------------------------------------------------------------------
// JpegEncoder::Compress
//------------------------------------------------------------------------
void JpegEncoder::Compress(MemBufPtr ptrmbSrc, struct jpeg_compress_struct *pcinfo)
{
  // Start compressor
  jpeg_start_compress(pcinfo, TRUE);

  JSAMPROW *row_pointer = new JSAMPROW[pcinfo->image_height];
  for( uint ui = 0; ui < pcinfo->image_height; ui++ )
    row_pointer[ui] = (unsigned char *)(ptrmbSrc->Buf() + ui * pcinfo->image_width);
  // Write all rows in one single pass
  jpeg_write_scanlines(pcinfo, row_pointer, pcinfo->image_height);

  // Terminate encoding process
  jpeg_finish_compress(pcinfo);
  jpeg_destroy_compress(pcinfo);
}

//------------------------------------------------------------------------
// JpegEncoder::GetDestBuf
//------------------------------------------------------------------------
JpegEncoder::DestMemBuf *JpegEncoder::GetDestBuf(struct jpeg_compress_struct *pcinfo)
{
  map<struct jpeg_compress_struct *,  class DestMemBuf *>::iterator it;
  it = s_mapCinfoToOutput.find(pcinfo);
  if( it != s_mapCinfoToOutput.end() )
    return it->second;
  return NULL;
}

//=============================================================================
// JpegEncoder::DestMemBuf
//=============================================================================
//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::DestinationManager
//------------------------------------------------------------------------
JpegEncoder::DestMemBuf::DestMemBuf(MemBufPtr ptrmbOuput, int iBufferSize)
{
  m_ptrmbOutput = ptrmbOuput;

  // Allocate destination buffer
  m_mbEncoderDest.SetLen(iBufferSize);

  // Allocation libjpeg structure
  m_pDestinationManager = new jpeg_destination_mgr;

  // Set callback function pointers
  m_pDestinationManager->init_destination = init_destination_wrapper;
  m_pDestinationManager->empty_output_buffer = empty_output_buffer_wrapper;
  m_pDestinationManager->term_destination = term_destination_wrapper;
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::init_destination_wrapper
//------------------------------------------------------------------------
void JpegEncoder::DestMemBuf::init_destination_wrapper(struct jpeg_compress_struct *pcinfo)
{
  // Search in JpegEncoder::s_mapCinfoToOutput on which instance of DestMemBuf we should call InitDestination
  JpegEncoder::DestMemBuf *pOutputBuf = JpegEncoder::GetDestBuf(pcinfo);
  if( pOutputBuf != NULL )
    pOutputBuf->InitDestination(pcinfo);
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::InitDestination
//------------------------------------------------------------------------
void JpegEncoder::DestMemBuf::InitDestination(struct jpeg_compress_struct *pcinfo)
{
  pcinfo->dest->next_output_byte = (unsigned char *)m_mbEncoderDest.Buf();
  pcinfo->dest->free_in_buffer = m_mbEncoderDest.Len();
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::empty_output_buffer_wrapper
//------------------------------------------------------------------------
#ifdef __WIN32__
uint8 JpegEncoder::DestMemBuf::empty_output_buffer_wrapper(struct jpeg_compress_struct *pcinfo)
#else
int JpegEncoder::DestMemBuf::empty_output_buffer_wrapper(struct jpeg_compress_struct *pcinfo)
#endif
{
  // Search in JpegEncoder::s_mapCinfoToOutput on which instance of DestMemBuf
  // we should call EmptyOutputBuffer
  JpegEncoder::DestMemBuf *pOutputBuf = JpegEncoder::GetDestBuf(pcinfo);
  if( pOutputBuf != NULL )
    return uint8(pOutputBuf->EmptyOutputBuffer(pcinfo));
  return 0;
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::EmptyOutputBuffer
//------------------------------------------------------------------------
int JpegEncoder::DestMemBuf::EmptyOutputBuffer(struct jpeg_compress_struct *pcinfo)
{
  m_ptrmbOutput->PutBloc(m_mbEncoderDest.Buf(), m_mbEncoderDest.Len());
  pcinfo->dest->next_output_byte = (unsigned char *)m_mbEncoderDest.Buf();
  pcinfo->dest->free_in_buffer = m_mbEncoderDest.Len();
  return 1;
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::term_destination_wrapper
//------------------------------------------------------------------------
void JpegEncoder::DestMemBuf::term_destination_wrapper(struct jpeg_compress_struct *pcinfo)
{
  // Search in JpegEncoder::s_mapCinfoToOutput on which instance of DestMemBuf
  // we should call TermDestination
  JpegEncoder::DestMemBuf *pOutputBuf = JpegEncoder::GetDestBuf(pcinfo);
  if( pOutputBuf != NULL )
    pOutputBuf->TermDestination(pcinfo);
}

//------------------------------------------------------------------------
// JpegEncoder::DestMemBuf::TermDestination
//------------------------------------------------------------------------
void JpegEncoder::DestMemBuf::TermDestination(struct jpeg_compress_struct *pcinfo)
{
  m_ptrmbOutput->PutBloc(m_mbEncoderDest.Buf(), m_mbEncoderDest.Len() - pcinfo->dest->free_in_buffer);
}
