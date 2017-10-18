//*****************************************************************************
// Synchrotron SOLEIL
//
//
// Creation : 20/07/2005
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

#include "base.h"
#include "file.h"
#include "membuf.h"
#include "nxfile.h"
#include "variant.h"

#ifdef __JPEG_SUPPORT__
  #include "jpegwrap.h"
#endif

#include "bmp.h"

#include <iostream>
#include <fstream>
#include <sstream>  
#include <vector>
#include <cstring>
#include <cstdlib>

#include "nexusevaluator.h"
#include "extractor.h"
#include "templateparsor.h"

using namespace gdshare;

//-----------------------------------------------------------------------------
// PrintFormat::PrintFormat
//-----------------------------------------------------------------------------
PrintFormat::PrintFormat()
{
  Init();
}

//-----------------------------------------------------------------------------
// PrintFormat::PrintFormat
//-----------------------------------------------------------------------------
PrintFormat::PrintFormat(const String &strFormat)
{
  Init();
  Set(strFormat);
}

//-----------------------------------------------------------------------------
// PrintFormat::Set
//-----------------------------------------------------------------------------
void PrintFormat::Set(const String &strFormat)
{
  // First search for format type
  int iTypePos = strFormat.find_first_of("cdieEfgGosuxXpn?");
  if( string::npos == (uint)iTypePos )
  {
    cerr << "Error : bad format: '" << strFormat << "'; Exiting." << endl;
  }
  m_cType = strFormat[iTypePos];

  int iWidthBegPos = 1;
  int iWidthEndPos = iTypePos;

  // Look for a modifier
  if( strFormat.find_first_of("hlL", iTypePos - 1, 2) != string::npos )
  {
    m_cModifier = strFormat[iTypePos-1];
    iWidthEndPos--;
  }

  // Look for precision
  int iPrecisionPos = strFormat.find('.');
  if( (uint)iPrecisionPos != string::npos && iPrecisionPos < iTypePos )
  {
    m_iPrecision = atoi(strFormat.substr(iPrecisionPos + 1, iTypePos - iPrecisionPos - 1).c_str());
    iWidthEndPos = iPrecisionPos;
  }

  // Look for a flag
  int iFlagPos = strFormat.find_first_of("-+ #");
  if( 1 == iFlagPos )
  {
    m_cFlag = strFormat[1];
    iWidthBegPos++;
  }

  // Width argument
  m_strWidth = strFormat.substr(iWidthBegPos, iWidthEndPos - iWidthBegPos);

  // Rest
  m_strRest = strFormat.substr(iTypePos + 1);
}

//-----------------------------------------------------------------------------
// PrintFormat::Get
//-----------------------------------------------------------------------------
String PrintFormat::Get()
{
  ostringstream ossFmt;
  ossFmt << '%';
  if( m_cFlag != -1 )
    ossFmt << m_cFlag;
  ossFmt << m_strWidth;
  if( m_iPrecision != -1 )
    ossFmt << '.' << m_iPrecision;
  if( m_cModifier != -1 )
    ossFmt << m_cModifier;
  ossFmt << m_cType << m_strRest;
  return ossFmt.str();
}

//-----------------------------------------------------------------------------
// PrintFormat::GetNoRest
//-----------------------------------------------------------------------------
String PrintFormat::GetNoRest()
{
  ostringstream ossFmt;
  ossFmt << '%';
  if( m_cFlag != -1 )
    ossFmt << m_cFlag;
  ossFmt << m_strWidth;
  if( m_iPrecision != -1 )
    ossFmt << '.' << m_iPrecision;
  if( m_cModifier != -1 )
    ossFmt << m_cModifier;
  ossFmt << m_cType;
  return ossFmt.str();
}

//-----------------------------------------------------------------------------
// DataBuf:SetTypeFromNeXusType
//-----------------------------------------------------------------------------
void DataBuf::SetTypeFromNeXusType(int iNxType)
{
  switch( iNxType )
  {
    case NX_UINT8:
    case NX_INT8:
      m_eType = DataBuf::BYTE;
      m_uiTypeSize = 1;
      break;
    case NX_INT16:
      m_eType = DataBuf::SHORT;
      m_uiTypeSize = sizeof(short);
      break;
    case NX_UINT16:
      m_eType = DataBuf::USHORT;
      m_uiTypeSize = sizeof(unsigned short);
      break;
    case NX_INT32:
      m_eType = DataBuf::LONG;
      m_uiTypeSize = sizeof(long);
      break;
    case NX_UINT32:
      m_eType = DataBuf::ULONG;
      m_uiTypeSize = sizeof(unsigned long);
      break;
    case NX_FLOAT32:
      m_eType = DataBuf::FLOAT;
      m_uiTypeSize = sizeof(float);
      break;
    case NX_FLOAT64:
      m_eType = DataBuf::DOUBLE;
      m_uiTypeSize = sizeof(double);
      break;
    case NX_CHAR:
      m_eType = DataBuf::CHAR;
      m_uiTypeSize = 1;
      break;
    default:
      m_eType = DataBuf::UNKNOWN;
      m_uiTypeSize = 0;
   }
}

//-----------------------------------------------------------------------------
// Extractor::Extractor
//-----------------------------------------------------------------------------
Extractor::Extractor(const String &strNeXusFile, VecToken *pvecToken,
                     IVariableEvaluator *pAppContext,
                     const String &strMode, const String &strOwnership)
          :m_vecToken(*pvecToken), m_strMode(strMode), m_strOwnership(strOwnership)
{
  m_bSilentMode = false;

  // Default output stream is stdout
  m_fiOut = NULL;

  m_pNxEval = new NexusEvaluator(strNeXusFile, this);
  m_TempParsor.AddEvaluator(this);
  m_TempParsor.AddEvaluator(pAppContext);
}

//-----------------------------------------------------------------------------
// Extractor::Extractor
//-----------------------------------------------------------------------------
Extractor::~Extractor()
{
  delete m_pNxEval;
}

//-----------------------------------------------------------------------------
// Extractor::Uid
//-----------------------------------------------------------------------------
uid_t Extractor::Uid()
{
  String strUser = m_strOwnership, strGroup;
  strUser.ExtractTokenRight(':', &strGroup);
  if( strUser.empty() )
      return uid_t(-1);
  return uid_t(atoi(PSZ(strUser)));
}

//-----------------------------------------------------------------------------
// Extractor::Uid
//-----------------------------------------------------------------------------
gid_t Extractor::Gid()
{
  String strUser = m_strOwnership, strGroup;
  strUser.ExtractTokenRight(':', &strGroup);
  if( strGroup.empty() )
      return gid_t(-1);
  return gid_t(atoi(PSZ(strGroup)));
}

//-----------------------------------------------------------------------------
// Extractor::ConvertBinaryValue
//-----------------------------------------------------------------------------
#define CASE(TDstName, TDST, TSRC) \
        case TDstName: \
        *(TDST*)(pDst) = (TDST)(*(TSRC*)(pSrc));\
        return true;
bool Extractor::ConvertBinaryValue(void *pSrc, DataBuf::Type eSrcType, void *pDst, TemplateToken::BinaryDataType eDstType)
{
  switch( eSrcType )
  {
    case DataBuf::BYTE:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::BYTE, uint8, uint8)
        CASE(TemplateToken::SHORT, short, uint8)
        CASE(TemplateToken::LONG, long, uint8)
        CASE(TemplateToken::FLOAT, float, uint8)
        CASE(TemplateToken::DOUBLE, double, uint8)
        default:
          return false;
      }
      break;
    }
    case DataBuf::SHORT:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::SHORT, short, short)
        CASE(TemplateToken::LONG, long, short)
        CASE(TemplateToken::FLOAT, float, short)
        CASE(TemplateToken::DOUBLE, double, short)
        default:
          return false;
      }
      break;
    }
    case DataBuf::USHORT:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::SHORT, short, unsigned short)
        CASE(TemplateToken::USHORT, unsigned short, unsigned short)
        CASE(TemplateToken::LONG, long, unsigned short)
        CASE(TemplateToken::FLOAT, float, unsigned short)
        CASE(TemplateToken::DOUBLE, double, unsigned short)
        default:
          return false;
      }
      break;
    }
    case DataBuf::INT:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::SHORT, short, int)
        CASE(TemplateToken::LONG, long, int)
        CASE(TemplateToken::FLOAT, float, int)
        CASE(TemplateToken::DOUBLE, double, int)
        default:
          return false;
      }
      break;
    }
    case DataBuf::LONG:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::LONG, long, long)
        CASE(TemplateToken::FLOAT, float, long)
        CASE(TemplateToken::DOUBLE, double, long)
        default:
          return false;
      }
      break;
    }
    case DataBuf::ULONG:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::LONG, long, unsigned long)
        CASE(TemplateToken::FLOAT, float, unsigned long)
        CASE(TemplateToken::DOUBLE, double, unsigned long)
        default:
          return false;
      }
      break;
    }
    case DataBuf::FLOAT:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::LONG, long, float)
        CASE(TemplateToken::FLOAT, float, float)
        CASE(TemplateToken::DOUBLE, double, float)
        default:
          return false;
      }
      break;
    }
    case DataBuf::DOUBLE:
    {
      switch( eDstType )
      {
        CASE(TemplateToken::LONG, long, double)
        CASE(TemplateToken::FLOAT, float, double)
        CASE(TemplateToken::DOUBLE, double, double)
        default:
          return false;
      }
      break;
    }
    default:
      return false;
  }
  return false;
}

//------------------------------------------------------------------------
// ExtractorApp::ConvertImageToByte
//------------------------------------------------------------------------
#define CONVERT_IMAGE_TO_BYTE(T, buf, buf_size)\
{\
  T MaxValue=T(0), MinValue=T(0);\
  T *pInBuf = (T *)(buf);\
  uint ui;\
  for( ui = 0; ui < buf_size; ui++ )\
  {\
    if( 0 == ui )\
    { \
      MinValue = pInBuf[ui];\
      MaxValue = pInBuf[ui];\
    }\
    else\
    {\
      if( MaxValue < pInBuf[ui] )\
        MaxValue = pInBuf[ui];\
      if( MinValue > pInBuf[ui] )\
        MinValue = pInBuf[ui];\
    }\
  }\
  double dRange = double(MaxValue - MinValue);\
  uint8 uiVal;\
  for( ui = 0; ui < buf_size; ui++ )\
  {\
    uiVal = uint8(((double)(pInBuf[ui] - MinValue) * 255.) / dRange);\
    puiBuf[ui] = uiVal;\
  }\
}

//------------------------------------------------------------------------
// ExtractorApp::ConvertImageToByte
//------------------------------------------------------------------------
#define RAW_CONVERT_IMAGE_TO_BYTE(T, buf, buf_size)\
{\
  T *pInBuf = (T *)(buf);\
  uint ui;\
  for( ui = 0; ui < buf_size; ui++ )\
  {\
    if( pInBuf[ui] < 0 )\
      puiBuf[ui] = 0;\
    else if( pInBuf[ui] > 255 )\
      puiBuf[ui] = 255;\
    else\
    puiBuf[ui] = (uint8)(pInBuf[ui]);\
  }\
}

#ifdef __JPEG_SUPPORT__
//------------------------------------------------------------------------
// Extractor::ValueToJpeg
//------------------------------------------------------------------------
void Extractor::ValueToJpeg(DataBuf *pDataBuf, MemBufPtr ptrmbDest)
{
  // Allocate membuf with metadata capability
  MemBufPtr ptrMemBuf(new CMemBuf);
  uint8 *puiBuf = new uint8[pDataBuf->Len() / pDataBuf->TypeSize()];

  // Converting data into 0..255 range
  switch( pDataBuf->DataType() )
  {
    case DataBuf::FLOAT:
      CONVERT_IMAGE_TO_BYTE(float, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::DOUBLE:
      CONVERT_IMAGE_TO_BYTE(double, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::LONG:
      CONVERT_IMAGE_TO_BYTE(long, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::ULONG:
      CONVERT_IMAGE_TO_BYTE(ulong, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::SHORT:
      CONVERT_IMAGE_TO_BYTE(short, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::USHORT:
      CONVERT_IMAGE_TO_BYTE(ushort, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    default:
      LogError("data", "Bad image datatype. Unable to convert");
      break;   
  } 

  // Attach dataset buffer to our buffer to avoid a allocation and a copy
  ptrMemBuf->Attach((char *)puiBuf, pDataBuf->Len() / pDataBuf->TypeSize(), false);

  // Set metadata
  ptrMemBuf->AddMetadata("width", pDataBuf->GetDimSize(0));
  ptrMemBuf->AddMetadata("height", pDataBuf->GetDimSize(1));

  // Invoke jpeg encoder
  JpegEncoder::EncodeGrayScaleToMemBuf(ptrMemBuf, ptrmbDest);
}
#endif

//------------------------------------------------------------------------
// Extractor::ValueToBmp
//------------------------------------------------------------------------
void Extractor::ValueToBmp(DataBuf *pDataBuf, MemBufPtr ptrmbDest)
{
  // Allocate membuf with metadata capability
  MemBufPtr ptrMemBuf(new CMemBuf);
  uint8 *puiBuf = new uint8[pDataBuf->Len() / pDataBuf->TypeSize()];

  // Converting data into 0..255 range
  switch( pDataBuf->DataType() )
  {
    case DataBuf::FLOAT:
      RAW_CONVERT_IMAGE_TO_BYTE(float, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::DOUBLE:
      RAW_CONVERT_IMAGE_TO_BYTE(double, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::LONG:
      RAW_CONVERT_IMAGE_TO_BYTE(long, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::ULONG:
      RAW_CONVERT_IMAGE_TO_BYTE(ulong, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::SHORT:
      RAW_CONVERT_IMAGE_TO_BYTE(short, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    case DataBuf::USHORT:
      RAW_CONVERT_IMAGE_TO_BYTE(ushort, pDataBuf->Buf(), pDataBuf->Len() / pDataBuf->TypeSize());
      break;
    default:
      LogError("data", "Bad image datatype. Unable to convert");
      break;   
  } 

  // Attach dataset buffer to our buffer to avoid a allocation and a copy
  ptrMemBuf->Attach((char *)puiBuf, pDataBuf->Len() / pDataBuf->TypeSize(), false);

  // Set metadata
  ptrMemBuf->AddMetadata("width", pDataBuf->GetDimSize(0));
  ptrMemBuf->AddMetadata("height", pDataBuf->GetDimSize(1));

  // Invoke bmp encoder
  BmpEncoder::EncodeGrayScaleToMemBuf(ptrMemBuf, ptrmbDest);
}

//-----------------------------------------------------------------------------
// Extractor::ValueToInteger
//-----------------------------------------------------------------------------
int Extractor::ValueToInteger()
{
  switch( m_aValue.DataType() )
  {
    case DataBuf::BYTE:
      return int(*((uint8 *)(m_aValue.Buf())));
    case DataBuf::UBYTE:
      return int(*((uint8 *)(m_aValue.Buf())));
    case DataBuf::CHAR:
      return 0;
    case DataBuf::SHORT:
      return int(*((short *)(m_aValue.Buf())));
    case DataBuf::USHORT:
      return int(*((short *)(m_aValue.Buf())));
    case DataBuf::LONG:
      return int(*((long *)(m_aValue.Buf())));
    case DataBuf::ULONG:
      return int(*((long *)(m_aValue.Buf())));
    case DataBuf::INT:
      return int(*((int *)(m_aValue.Buf())));
    case DataBuf::FLOAT:
      return int(*((float *)(m_aValue.Buf())));
    case DataBuf::DOUBLE:
      return int(*((double *)(m_aValue.Buf())));
    case DataBuf::NO_TYPE:
    case DataBuf::UNKNOWN:
      break;
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Extractor::ValueToString
//-----------------------------------------------------------------------------
String Extractor::ValueToString(DataBuf &aValue)
{
  String strValue;
  switch( aValue.DataType() )
  {
    case DataBuf::BYTE:
      strValue.Printf("%d", int(*((uint8 *)(aValue.Buf()))));
      break;
    case DataBuf::UBYTE:
      strValue.Printf("%d", (*((uint8 *)(aValue.Buf()))));
      break;
    case DataBuf::CHAR:
      strValue.Printf("%s", m_aValue.Buf());
      break;
    case DataBuf::SHORT:
      strValue.Printf("%h", (*((short *)(aValue.Buf()))));
      break;
    case DataBuf::USHORT:
      strValue.Printf("%h", (*((unsigned short *)(aValue.Buf()))));
      break;
    case DataBuf::LONG:
      strValue.Printf("%ld", (*((long *)(aValue.Buf()))));
      break;
    case DataBuf::ULONG:
      strValue.Printf("%ld", (*((unsigned long *)(aValue.Buf()))));
      break;
    case DataBuf::INT:
      strValue.Printf("%d", (*((int *)(aValue.Buf()))));
      break;
    case DataBuf::FLOAT:
      strValue.Printf("%g", (*((float *)(aValue.Buf()))));
      break;
    case DataBuf::DOUBLE:
      strValue.Printf("%g", (*((double *)(aValue.Buf()))));
      break;
    case DataBuf::NO_TYPE:
    case DataBuf::UNKNOWN:
      break;
  }
  return strValue;
}

//-----------------------------------------------------------------------------
// Extractor::ExecLoopOver
//-----------------------------------------------------------------------------
int Extractor::ExecLoopOver(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);

  String strCollection = Token.m_strParam2;
  m_TempParsor.ProcessNoRecursion(&strCollection);
  ItemList *pList = NULL;
  ItemList *pCachedList = NULL;
  ItemList lstItems;

  if( strCollection.StartWith("nxs:") )
    // NeXus collection
    pCachedList = m_pNxEval->GetItemsList(&strCollection, Token.m_strParam1);
  else
  {
    // The collection is a variable containing a list of value with '|' as separator
    m_TempParsor.ProcessNoRecursion(&strCollection);
    String strItem;
    while( !strCollection.empty() )
    {
      strCollection.ExtractToken('|', &strItem);
      lstItems.push_back(NxItem(strItem, strItem));
    }
  }

  if( pCachedList )
    pList = pCachedList;
  else
    pList = &lstItems;

  // First variable name is whoise indicated in script
  // Second (and implicite) variable is the same followed by "_name"
  String strName = Token.m_strParam1 + "_name";

  ItemList::iterator it = pList->begin();
  // Loop over list items pairs
  for(; it != pList->end(); it++)
  {
    String strValue = (*it).first;
    m_dictVar[strName] = strValue;
    if( !(*it).second.empty() )
    {
      String strValue = (*it).second;
      m_dictVar[Token.m_strParam1] = strValue;
    }
    Exec(iCurrentPos+1, Token.m_iEndBlockPos+1);
  }

  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::ExecBlock
//-----------------------------------------------------------------------------
int Extractor::ExecBlock(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);
  String strLimit = Token.m_strParam2;
  m_TempParsor.ProcessNoRecursion(&strLimit);

  // Get parameters
  String strBlockName = Token.m_strParam1;
  uint uiMaxSize = (uint)atoi(PSZ(strLimit));
  
  // Get Membuf
  MemBufPtr ptrBuf = GetBuf(strBlockName);
  ptrBuf->Reset();

  // Push new output object
  MemBufOutput *pOutBuf = new MemBufOutput(ptrBuf);
  m_stkOut.push(pOutBuf);

  Exec(iCurrentPos+1, Token.m_iEndBlockPos+1);

  // Check buffer size
  if( uiMaxSize != 0 )
  {
    if( uiMaxSize < ptrBuf->Len() )
      // Buffer size exceed limit, cut !
      ptrBuf->SetLen(uiMaxSize);
    
    if( uiMaxSize > ptrBuf->Len() )
    {
      // Padding
      int iInsertPos = pOutBuf->PaddingPos() == -1 ? ptrBuf->Len() : pOutBuf->PaddingPos();
      while( uiMaxSize - ptrBuf->Len() >= pOutBuf->PaddingPattern().size() )
      {
        ptrBuf->InsertBloc(PSZ(pOutBuf->PaddingPattern()), pOutBuf->PaddingPattern().size(), iInsertPos);
        iInsertPos += pOutBuf->PaddingPattern().size();
      }

      if( uiMaxSize > ptrBuf->Len() )
        ptrBuf->InsertBloc(PSZ(pOutBuf->PaddingPattern()), uiMaxSize - ptrBuf->Len(), iInsertPos);
    }
  }

  // Remove output object
  m_stkOut.pop();

  // Make new variables with bloc name and size
  String strVar = strBlockName + "_size";
  String strSize;
  strSize.Printf("%d", ptrBuf->Len());
  m_dictVar[strVar] = strSize;
  m_dictVar[strBlockName] = String(ptrBuf->Buf(), ptrBuf->Len());

  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::ExecPadding
//-----------------------------------------------------------------------------
void Extractor::ExecPadding(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);
  // Check Output object type
  MemBufOutput *pBufOutput = dynamic_cast<MemBufOutput *>(m_stkOut.top().ObjectPtr());
  if( !pBufOutput )
  {
    cerr << "Padding don't make sense in the current context (at line " << Token.m_iTemplateLine << ")." << endl;
  }

  // Store current buffer position for padding and pattern
  pBufOutput->SetPaddingPattern(Token.m_strParam1);
}

//-----------------------------------------------------------------------------
// Extractor::OutBinaryScalarFromString
//-----------------------------------------------------------------------------
void Extractor::OutBinaryScalarFromString(const TemplateToken &Token, const String &strData)
{
  int iBase = 10;
  if( strData.StartWith("0x") )
    iBase = 16;

  switch( Token.m_eBinaryDataType )
  {
    case TemplateToken::CHAR:
    {
      char c = strData[0];
      m_stkOut.top()->Out(NULL, c);
      break;
    }
    case TemplateToken::BYTE:
    {
      uint8 ui8 = strtol(PSZ(strData), (char **)NULL, iBase);
      m_stkOut.top()->Out(NULL, ui8);
      break;
    }
    case TemplateToken::SHORT:
    {
      short s = strtol(PSZ(strData), (char **)NULL, iBase);
      m_stkOut.top()->Out(NULL, s);
      break;
    }
    case TemplateToken::LONG:
    {
      long l = strtol(PSZ(strData), (char **)NULL, iBase);
      m_stkOut.top()->Out(NULL, l);
      break;
    }
    case TemplateToken::FLOAT:
    {
      float f = atof(PSZ(strData));
      m_stkOut.top()->Out(NULL, f);
      break;
    }
    case TemplateToken::DOUBLE:
    {
      double d = atof(PSZ(strData));
      m_stkOut.top()->Out(NULL, d);
      break;
    }
    case TemplateToken::RAW:
    {
      // Search for buffer
      MemBufPtr ptrBuf = GetBuf(strData, false);
      if( ptrBuf.IsNull() )
      {
        cerr << "Error : bloc '" << strData << "' not found; Exiting." << endl;
      }
      m_stkOut.top()->Out((void *)ptrBuf->Buf(), ptrBuf->Len());
      break;  
    }
    default:
      cerr << "Error: bad syntax at line " << Token.m_iTemplateLine << " : bloc '" << strData << "' not found; Exiting." << endl;
  }
}

//-----------------------------------------------------------------------------
// Extractor::ExecBinary
//-----------------------------------------------------------------------------
void Extractor::ExecBinary(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);

  // Evaluate target
  String strData = GetTokenArgument(&Token);

  // Evaluate data by NeXus evaluator
  bool bEvaluation = m_pNxEval->Evaluate(strData, &m_aValue);
  if( bEvaluation )
  {
    if( TemplateToken::RAW == Token.m_eBinaryDataType )
      // Raw data type => just push value content to output buffer
      m_stkOut.top()->Out((void *)m_aValue.Buf(), m_aValue.Len());

#ifdef __JPEG_SUPPORT__
    else if( TemplateToken::JPEG_IMAGE == Token.m_eBinaryDataType )
    {
      // Write a jpeg image
      MemBufPtr ptrMemBuf(new CMemBuf);
      try
      {
        ValueToJpeg(&m_aValue, ptrMemBuf);
        m_stkOut.top()->Out((void *)ptrMemBuf->Buf(), ptrMemBuf->Len());
      }
      catch( JpegEncoder::Exception e )
      {
        cerr << e.Message();
      }
    }
#endif

    else if( TemplateToken::BMP_IMAGE == Token.m_eBinaryDataType )
    {
      // Write a bmp image
      MemBufPtr ptrMemBuf(new CMemBuf);
      try
      {
        ValueToBmp(&m_aValue, ptrMemBuf);
        m_stkOut.top()->Out((void *)ptrMemBuf->Buf(), ptrMemBuf->Len());
      }
      catch( BmpEncoder::Exception e )
      {
        cerr << e.Message();
      }
    }
    else
    {
      // Convert buffer to asked type
      CMemBuf mbDst;
      // Pre-allocation
      mbDst.SetLen(Token.m_uiTypeSize * m_aValue.Len() / m_aValue.TypeSize());
    
      // Get pointers to data
      char *pDst = mbDst.Buf();
      char *pSrc = (char *)m_aValue.Buf();

      bool bConverted = true;

      if( m_aValue.DataType() == Token.m_eBinaryDataType )
        // no conversion
        mbDst.Attach(pSrc, mbDst.Len());
      else
      { // conversion needed
        for( pSrc = (char *)m_aValue.Buf(); pSrc < (char *)m_aValue.Buf() + m_aValue.Len() && bConverted; 
             pSrc += m_aValue.TypeSize(), pDst += Token.m_uiTypeSize )
        {
          bConverted = ConvertBinaryValue(pSrc, m_aValue.DataType(), pDst, Token.m_eBinaryDataType);
        }
      }

      if( !bConverted )
      {
        cerr << "Error: conversion not possible at line " << Token.m_iTemplateLine << ". Extraction aborted." << endl;
      }
      m_stkOut.top()->Out((void *)mbDst.Buf(), mbDst.Len());
    }
  }
  else 
    OutBinaryScalarFromString(Token, strData);    
}

//-----------------------------------------------------------------------------
// Extractor::ExecComp
//-----------------------------------------------------------------------------
int Extractor::ExecComp(int iCurrentPos, bool bInf, bool bNot)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);

  DataBuf aValue1, aValue2;
  String strParam1 = Token.m_strParam1;
  m_TempParsor.ProcessNoRecursion(&strParam1);
  if( m_pNxEval->Evaluate(strParam1, &aValue1) )
    strParam1 = ValueToString(aValue1);
  String strParam2 = Token.m_strParam2;
  m_TempParsor.ProcessNoRecursion(&strParam2);
  if( m_pNxEval->Evaluate(strParam2, &aValue2) )
    strParam2 = ValueToString(aValue2);

  bool bComp = false;
  if( !bInf )
    bComp = atoi(PSZ(strParam1)) > atoi(PSZ(strParam2));
  else
    bComp = atoi(PSZ(strParam1)) < atoi(PSZ(strParam2));

  if( (bComp && !bNot) || (!bComp && bNot ) )
    // Condition is satisfyed
    Exec(iCurrentPos+1, Token.m_iEndBlockPos+1);

  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::ExecIfEq
//-----------------------------------------------------------------------------
int Extractor::ExecIfEq(int iCurrentPos, bool bNot)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);

  DataBuf aValue1, aValue2;
  String strParam1 = Token.m_strParam1;
  m_TempParsor.ProcessNoRecursion(&strParam1);
  if( m_pNxEval->Evaluate(strParam1, &aValue1) )
    strParam1 = ValueToString(aValue1);
  String strParam2 = Token.m_strParam2;
  m_TempParsor.ProcessNoRecursion(&strParam2);
  if( m_pNxEval->Evaluate(strParam2, &aValue2) )
    strParam2 = ValueToString(aValue2);

  bool bCond = strParam1.Match(PSZ(strParam2));
  if( (bCond && !bNot) || (!bCond && bNot ) )
    // Condition is satisfyed
    Exec(iCurrentPos+1, Token.m_iEndBlockPos+1);

  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::ExecIfExists
//-----------------------------------------------------------------------------
int Extractor::ExecIfExists(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);
  bool bExists = false;
  if( !Token.m_strData.empty() )
  {
    String strItemPath = Token.m_strData;
    m_TempParsor.ProcessNoRecursion(&strItemPath);
    bExists = m_pNxEval->CheckItem(strItemPath);
  }
  else // Check for variable
    bExists = (m_dictVar.find(Token.m_strParam1) != m_dictVar.end());

  if( bExists )
    Exec(iCurrentPos+1, Token.m_iEndBlockPos+1);

  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::ExecSet
//-----------------------------------------------------------------------------
void Extractor::ExecSet(int iCurrentPos)
{
  TokenSet &Token = *((TokenSet *)(m_vecToken[iCurrentPos].ObjectPtr()));
  String strValue = Token.m_strParam2;
  if( Token.m_bEvalArgument )
    m_TempParsor.Process(&strValue);
  m_dictVar[Token.m_strParam1] = strValue;
}

//-----------------------------------------------------------------------------
// Extractor::ExecSetList
//-----------------------------------------------------------------------------
void Extractor::ExecSetList(int iCurrentPos)
{
  TokenSet &Token = *((TokenSet *)(m_vecToken[iCurrentPos].ObjectPtr()));
  String strCollection = Token.m_strParam2;
  m_TempParsor.ProcessNoRecursion(&strCollection);
  ItemList *pList = NULL;
  ItemList *pCachedList=NULL;
  ItemList lstItems;
  bool bNexusItem = false;

  if( strCollection.StartWith("nxs:") )
  {
    // NeXus collection
    pList = m_pNxEval->GetItemsList(&strCollection, Token.m_strParam1);
    bNexusItem = true;
  }

  String strValueList;
  ItemList::iterator it = pList->begin();
  // Loop over list items pairs
  for(; it != pList->end(); it++)
  {
    String strValue = (*it).first;
    if( !(*it).second.empty() )
    {
      if( !strValueList.empty() )
        strValueList += "|";  // List separator
      if( !Token.m_strParam3.empty() && bNexusItem )
      {
        // Value to be evaluated
        String strToEvaluate = "nxs:" + (*it).second + Token.m_strParam3;
        m_pNxEval->Evaluate(strToEvaluate, &m_aValue);
        strValueList += ScalarValueToString();

      }
      else
        strValueList += (*it).second;
    }
  }

  m_dictVar[Token.m_strParam1] = strValueList;
}

//-----------------------------------------------------------------------------
// Extractor::ExecLoop
//-----------------------------------------------------------------------------
int Extractor::ExecLoop(int iCurrentPos)
{
  TemplateToken &Token = *(m_vecToken[iCurrentPos]);
  TokenLoop *pToken = dynamic_cast<TokenLoop *>(&Token);
  Expression::s_pValueEval = this;

  int iInit = pToken->ptrExprBegin->Eval();
  int iEnd = pToken->ptrExprEnd->Eval();
  int iStep = 1;
  if( iInit > iEnd )
    iStep = -1;
  if( !pToken->ptrExprStep.IsNull() )
    iStep = pToken->ptrExprStep->Eval();

  // Add counter
  pair<MapCounters::iterator, bool> 
    prItBool = m_dictCounters.insert(MapCounters::value_type(Token.m_strParam1, 0));

  if( iInit < iEnd )
    for(int i = iInit; i < iEnd; i += iStep)
    {
      (prItBool.first)->second = i;              // Update counter value
      Exec(iCurrentPos+1, Token.m_iEndBlockPos+1); // Execute code until next end-loop
    }
  else
    for(int i = iInit; i > iEnd; i += iStep)
    {
      (prItBool.first)->second = i;              // Update counter value
      Exec(iCurrentPos+1, Token.m_iEndBlockPos+1); // Execute code until next end-loop
    }

  // Remove counter
  m_dictCounters.erase(prItBool.first);
  // Return next token pos
  return Token.m_iEndBlockPos;
}

//-----------------------------------------------------------------------------
// Extractor::GetTokenArgument
//-----------------------------------------------------------------------------
String Extractor::GetTokenArgument(TemplateToken *pToken)
{
  String strTxt = pToken->m_strData;
  if( strTxt.empty() )
  {
    strTxt.reserve(256);
    // Build request from fragments
    for( DataFragmentsList::const_iterator it = pToken->lstDataFragments.begin();
         it != pToken->lstDataFragments.end(); it++ )
    {
      if( DataFragment::TEXT == it->Type() )
        // Text fragment
        strTxt += it->Content();
      else
      { // Fragment is a variable name to replace with its current value
        String strVar = it->Content();
        Evaluate(&strVar);
        strTxt += strVar;
      }
    }
  }
  return strTxt;
}

//-----------------------------------------------------------------------------
// Extractor::ExecPrintData
//-----------------------------------------------------------------------------
void Extractor::ExecPrintData(TemplateToken *pToken)
{
  String strData = GetTokenArgument(pToken);
  try
  {
    bool bEvaluation = m_pNxEval->Evaluate(strData, &m_aValue);
    if( bEvaluation )
      PrintData(pToken);
    else
    {
      switch( pToken->m_Format.Type() )
      {
        case 'd':
          m_stkOut.top()->Out(PSZ(pToken->m_Format.Get()), atol(PSZ(strData)));
          break;
        case 'f':
        case 'e':
        case 'g':
          m_stkOut.top()->Out(PSZ(pToken->m_Format.Get()), atof(PSZ(strData)));
          break;
        default:
          m_stkOut.top()->Out(PSZ(pToken->m_Format.Get()), PSZ(strData));
      }
    }
  }
  catch(...)
  {
    m_stkOut.top()->Out("%s", PSZ(strData));
  }
}

//-----------------------------------------------------------------------------
// Extractor::Execute
//-----------------------------------------------------------------------------
void Extractor::Execute()
{
  m_stkOut.push(new StandardOutput);
  Exec(0, m_vecToken.size());
  SetOutputFile(g_strEmpty, false); // To set properties and close current file
}

//-----------------------------------------------------------------------------
// Extractor::Exec
//-----------------------------------------------------------------------------
void Extractor::Exec(int iStartPos, int iEndPos)
{
  TemplateToken *pToken = NULL;
  
  for(int iPos = iStartPos; iPos < iEndPos; iPos++ )
  {
    pToken = m_vecToken[iPos];

    switch( pToken->m_TokenType )
    {
      case TemplateToken::PRINT_DATA:
        ExecPrintData(pToken);
        break;

      case TemplateToken::PRINT_TEXT:
      {
        String strTxt = pToken->m_strData;
        m_TempParsor.Process(&strTxt, false);
        m_stkOut.top()->Out("%s", PSZ(strTxt));
        break;
      }
      case TemplateToken::LOOP:
        iPos = ExecLoop(iPos);
        break;

      case TemplateToken::LOOP_OVER:
        iPos = ExecLoopOver(iPos);
        break;

      case TemplateToken::BINARY:
        ExecBinary(iPos);
        break;

      case TemplateToken::BLOCK_START:
        iPos = ExecBlock(iPos);
        break;

      case TemplateToken::IF_EXISTS:
        iPos = ExecIfExists(iPos);
        break;

      case TemplateToken::IF_SUP:
        iPos = ExecComp(iPos, false, false);
        break;

      case TemplateToken::IF_INF:
        iPos = ExecComp(iPos, true, false);
        break;

      case TemplateToken::IF_EQ:
        iPos = ExecIfEq(iPos, false);
        break;

      case TemplateToken::IF_NOT_EQ:
        iPos = ExecIfEq(iPos, true);
        break;

      case TemplateToken::SET:
        ExecSet(iPos);
        break;

      case TemplateToken::SET_LIST:
        ExecSetList(iPos);
        break;

      case TemplateToken::OUTPUT:
      {
        String strFile = pToken->m_strParam1;
        m_TempParsor.Process(&strFile);
        SetOutputFile(strFile, pToken->m_iParam1 ? true : false);
        break;
      }

      case TemplateToken::PADDING:
        ExecPadding(iPos);
        break;

      case TemplateToken::END_LOOP:
      case TemplateToken::BLOCK_END:
      case TemplateToken::END_IF:
      default:
        break;
    }
  }
}

//-----------------------------------------------------------------------------
// Extractor::SetOutputFile
//-----------------------------------------------------------------------------
void Extractor::SetOutputFile(const String &strFile, bool bBinary)
{
  if( m_fiOut != NULL )
  {
    if( m_stkOut.size() > 1 )
      m_stkOut.pop();
    fclose(m_fiOut);
  }

  if( !m_strCurrentFile.empty() )
  {
    // Set file ownership and access mode
#ifdef __LINUX__
    if( !m_strMode.empty() )
      chmod(PSZ(m_strCurrentFile), AccessFromString(m_strMode));
    chown(PSZ(m_strCurrentFile), Uid(), Gid());
#endif
    m_strCurrentFile = strFile;
  }

  if( strFile.empty() )
  {
    // Output stream redirection on console
#ifdef __WIN32__
    freopen_s(&m_fiOut, "CON", "w", stdout);
#else // UNIX
    m_fiOut = freopen("/dev/tty", "w", stdout);
#endif
    return;
  }

  m_strCurrentFile = strFile;

  // Replace blank characters to avoid problems
  m_strCurrentFile.Replace(' ', '_');

  // Create folder tree, with correct rights
  FileName fn(m_strCurrentFile);

  long lMode = 0;
  if( !m_strMode.empty() )
    // Allow directory access for everyone
    lMode = atoi(PSZ(m_strMode)) + 111;

  fn.MkDir(AccessFromString(StrFormat("%d", lMode)), Uid(), Gid());

  if( !bBinary )
    // Output stream redirection
#ifdef __WIN32__
    freopen_s(&m_fiOut, PSZ(m_strCurrentFile), "w", stdout);
#else // UNIX
    m_fiOut = freopen(PSZ(m_strCurrentFile), "w", stdout);
#endif
  else
  {
    // Open binary file
#ifdef __WIN32__
    fopen_s(&m_fiOut, PSZ(m_strCurrentFile), "wb");
#else // UNIX
    m_fiOut = fopen(PSZ(m_strCurrentFile), "wb");
#endif
    BinaryFileOutput *pOut = new BinaryFileOutput(m_fiOut);
    m_stkOut.push(pOut);
  }
}

//-----------------------------------------------------------------------------
// Extractor::AddVar
//-----------------------------------------------------------------------------
void Extractor::AddVar(const String &strVar, const String &strVal)
{
  // add the string strVar to our dict with the value strVal.
  m_dictVar.insert(StringDict::value_type(strVar, strVal));
}

//-----------------------------------------------------------------------------
// Extractor::SetVar
//-----------------------------------------------------------------------------
void Extractor::SetVar(const String &strVar, const String &strVal)
{
  // Search variable
  StringDict::iterator it = m_dictVar.find(strVar);
  if( it != m_dictVar.end())
    (it->second) = strVal;
  else 
    m_dictVar.insert(StringDict::value_type(strVar, strVal));
}

//-----------------------------------------------------------------------------
// Extractor::HasVar
//-----------------------------------------------------------------------------
bool Extractor::HasVar(const String &strVar)
{
  // Search variable
  StringDict::iterator it = m_dictVar.find(strVar);
  if( it != m_dictVar.end() )
    return true;
  else 
    return false;
}

//-----------------------------------------------------------------------------
// Extractor::RemoveVar
//-----------------------------------------------------------------------------
void Extractor::RemoveVar(const String &strVar)
{
  // we remove the string var from our dict.
  m_dictVar.erase(strVar);
}

//-----------------------------------------------------------------------------
// Extractor::Evalutate
//-----------------------------------------------------------------------------
bool Extractor::Evaluate(String *pstrVar)
{ 
  // To preserve performance, use pointer to avoid useless allocation of a big object
  String *pstrDefValue = NULL;
  if( pstrVar->find('=') != string::npos )
  {
    // Use def value if variable is not found
    pstrDefValue = new String;
    pstrVar->ExtractTokenRight('=', pstrDefValue);
  }

  // evalute a variable by giving its value stocked in the dict.
  StringDict::iterator it = m_dictVar.find(*pstrVar);
  if( it != m_dictVar.end())
  {
    *pstrVar=(it->second);
    return true;
  }
  // Search in counters
  MapCounters::const_iterator it2 = m_dictCounters.find(*pstrVar);
  if( it2 != m_dictCounters.end())
  {
    (*pstrVar).Printf("%d", (it2->second));
    return true;
  }

  if( pstrDefValue )
  {
    // Apply default value
    *pstrVar = *pstrDefValue;
    delete pstrDefValue;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Extractor::GetBuf
//-----------------------------------------------------------------------------
MemBufPtr Extractor::GetBuf(const String &strName, bool bCreate)
{
  MapBufPtr::iterator it = m_dictBuf.find(strName);
  if( it != m_dictBuf.end() )
    // found
    return it->second;

  // Create new buffer
  if( bCreate )
  {
    MemBufPtr ptrBuf(new CMemBuf);
    m_dictBuf[strName] = ptrBuf;
    return ptrBuf;
  }
  
  return NULL;
}

//-----------------------------------------------------------------------------
// Extractor::ScalarValueToString
//-----------------------------------------------------------------------------
String Extractor::ScalarValueToString()
{
  String strValue;
  switch( m_aValue.DataType() )
  {
    // if we have a string format.
    case DataBuf::CHAR:
      strValue.append((char *)m_aValue.Buf(), m_aValue.Len());
      break;
    // the case of int,long,unsigned char or short.
    case DataBuf::BYTE:
      strValue.Printf("%d", *((uint8 *)m_aValue.Buf()));
      break;
    case DataBuf::SHORT:
      strValue.Printf("%h", *((short *)m_aValue.Buf()));
      break;
    case DataBuf::USHORT:
      strValue.Printf("%hu", *((unsigned short *)m_aValue.Buf()));
      break;
    case DataBuf::LONG:
      strValue.Printf("%ld", *((long *)m_aValue.Buf()));
      break;
    case DataBuf::INT:
      strValue.Printf("%ld", *((int *)m_aValue.Buf()));
      break;
    case DataBuf::ULONG:
      strValue.Printf("%lu", *((unsigned long *)m_aValue.Buf()));
      break;
    case DataBuf::FLOAT:
      strValue.Printf("%g", *((float *)m_aValue.Buf()));
      break;
    case DataBuf::DOUBLE:
      strValue.Printf("%lg", *((double *)m_aValue.Buf()));
      break;
    default:
      break;
  }
  return strValue;
}

//-----------------------------------------------------------------------------
// Extractor::PrintData
//-----------------------------------------------------------------------------
void Extractor::PrintData(TemplateToken *pToken)
{
  // Check format
  if( m_aValue.DataType() != pToken->m_eOutputType )
  {
    ostringstream ossWarn;
    ossWarn << "Warning: Mismatching format types at line " <<
                pToken->m_iTemplateLine << ". Format " << pToken->m_Format.GetNoRest();
    // Set the right type according to the value to be printed
    switch( m_aValue.DataType() )
    {
      // if we have a string format.
      case DataBuf::CHAR:
        pToken->m_Format.SetType('s');
        pToken->m_eOutputType = DataBuf::CHAR;
        break;
        // the case of int,long,unsigned char or short.
      case DataBuf::BYTE:
        pToken->m_Format.SetType('d');
        pToken->m_Format.SetModifier('h');
        pToken->m_eOutputType = DataBuf::BYTE;
        break;
      case DataBuf::SHORT:
        pToken->m_Format.SetType('d');
        pToken->m_Format.SetModifier('h');
        pToken->m_eOutputType = DataBuf::SHORT;
        break;
      case DataBuf::USHORT:
        pToken->m_Format.SetType('d');
        pToken->m_Format.SetModifier('h');
        pToken->m_eOutputType = DataBuf::USHORT;
        break;
      case DataBuf::LONG:
        pToken->m_Format.SetType('d');
        pToken->m_Format.SetModifier('l');
        pToken->m_eOutputType = DataBuf::LONG;
        break;
      case DataBuf::ULONG:
        pToken->m_Format.SetType('d');
        pToken->m_Format.SetModifier('l');
        pToken->m_eOutputType = DataBuf::ULONG;
        break;
     case DataBuf::INT:
        pToken->m_Format.SetType('d');
        pToken->m_eOutputType = DataBuf::INT;
        break;
      case DataBuf::FLOAT:
      {
        if( pToken->m_eOutputType != DataBuf::FLOAT &&
            pToken->m_eOutputType != DataBuf::DOUBLE)
          pToken->m_Format.SetType('f');
        pToken->m_Format.SetModifier(-1);
        pToken->m_eOutputType = DataBuf::FLOAT;
        break;
      }
      case DataBuf::DOUBLE:
      {
        if( pToken->m_eOutputType != DataBuf::FLOAT &&
            pToken->m_eOutputType != DataBuf::DOUBLE)
          pToken->m_Format.SetType('f');
        pToken->m_Format.SetModifier('l');
        pToken->m_eOutputType = DataBuf::DOUBLE;
        break;
      }
      default:
        break;
    }
    ossWarn << " replaced with " << pToken->m_Format.GetNoRest() << "." << endl;
    if( !m_bSilentMode )
      // Show warning only if silent mode is off
      cerr << ossWarn.str();

    // Update format string
    pToken->m_strPrintFmt = pToken->m_Format.Get();
  }

  switch( m_aValue.DataType() )
  {
    // if we have a string format.
    case DataBuf::CHAR:
    {
      String strBuf;
      strBuf.append((char *)m_aValue.Buf(), m_aValue.Len());
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), PSZ(strBuf));
      break;
    }
      // the case of int,long,unsigned char or short.
    case DataBuf::BYTE:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((uint8 *)m_aValue.Buf()));
      break;
    case DataBuf::SHORT:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((short *)m_aValue.Buf()));
      break;
    case DataBuf::USHORT:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((unsigned short *)m_aValue.Buf()));
      break;
    case DataBuf::LONG:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((long *)m_aValue.Buf()));
      break;
    case DataBuf::ULONG:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((unsigned long *)m_aValue.Buf()));
      break;
    case DataBuf::INT:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((int *)m_aValue.Buf()));
      break;
    case DataBuf::FLOAT:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((float *)m_aValue.Buf()));
      break;
    case DataBuf::DOUBLE:
      m_stkOut.top()->Out(PSZ(pToken->m_strPrintFmt), *((double *)m_aValue.Buf()));
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
// Extractor::GetValue
//-----------------------------------------------------------------------------
bool Extractor::GetValue(const String &strVar, int *piValue)
{
  // evalute a variable by giving its value stocked in the dict.
  MapCounters::const_iterator it = m_dictCounters.find(strVar);
  if( it != m_dictCounters.end())
  {
    *piValue = it->second;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Extractor::Evaluate
//-----------------------------------------------------------------------------
Variant Extractor::Evaluate(const String &str)
{
  String strToEval = str;
  int iValue = 0;
  m_TempParsor.ProcessNoRecursion(&strToEval);
  if( m_pNxEval->Evaluate(strToEval, &m_aValue) )
    iValue = ValueToInteger();
  else
    iValue = atoi(PSZ(strToEval));
  return iValue;
}

//===========================================================================
/// class StandardOutput
//===========================================================================
void StandardOutput::Out(const char *pcszFormat, const char *psz)
{
  printf(pcszFormat, psz);
}

void StandardOutput::Out(const char *pcszFormat, char c)
{
  printf(pcszFormat, c);
}

void StandardOutput::Out(const char *pcszFormat, uint8 ui8)
{
  printf(pcszFormat, ui8);
}

void StandardOutput::Out(const char *pcszFormat, short s)
{
  printf(pcszFormat, s);
}

void StandardOutput::Out(const char *pcszFormat, unsigned short s)
{
  printf(pcszFormat, s);
}

void StandardOutput::Out(const char *pcszFormat, long l)
{
  printf(pcszFormat, l);
}

void StandardOutput::Out(const char *pcszFormat, unsigned long l)
{
  printf(pcszFormat, l);
}

void StandardOutput::Out(const char *pcszFormat, int i)
{
  printf(pcszFormat, i);
}

void StandardOutput::Out(const char *pcszFormat, float f)
{
  printf(pcszFormat, f);
}

void StandardOutput::Out(const char *pcszFormat, double d)
{
  printf(pcszFormat, d);
}

void StandardOutput::Out(void * /*pData*/, int /*iDataLen*/)
{
  cerr << "Error: Cannot print binary data as ascii text" << endl;
}

//===========================================================================
/// class MemBufOutput
//===========================================================================
void MemBufOutput::Out(const char *pcszFormat, const char *psz)
{
  sprintf(g_acScratchBuf, pcszFormat, psz);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, char c)
{
  sprintf(g_acScratchBuf, pcszFormat, c);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, uint8 ui8)
{
  sprintf(g_acScratchBuf, pcszFormat, ui8);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, short s)
{
  sprintf(g_acScratchBuf, pcszFormat, s);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, unsigned short us)
{
  sprintf(g_acScratchBuf, pcszFormat, us);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, long l)
{
  sprintf(g_acScratchBuf, pcszFormat, l);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, unsigned long ul)
{
  sprintf(g_acScratchBuf, pcszFormat, ul);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, int i)
{
  sprintf(g_acScratchBuf, pcszFormat, i);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, float f)
{
  sprintf(g_acScratchBuf, pcszFormat, f);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(const char *pcszFormat, double d)
{
  sprintf(g_acScratchBuf, pcszFormat, d);
  m_ptrMemBuf->PutBloc(g_acScratchBuf, strlen((const char *)g_acScratchBuf));
}

void MemBufOutput::Out(void *pData, int iDataLen)
{
  m_ptrMemBuf->PutBloc(pData, iDataLen);
}

//===========================================================================
/// class BinaryFileOutput
//===========================================================================
BinaryFileOutput::~BinaryFileOutput()
{
//  if( m_pFile )
//    fclose( m_pFile);
}
void BinaryFileOutput::Out(const char *pcszFormat, const char *psz)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, psz);
  else
    fwrite(pcszFormat, strlen(psz), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, char c)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, c);
  else
    fwrite(&c, sizeof(char), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, uint8 ui8)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, ui8);
  else
    fwrite(&ui8, sizeof(uint8), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, short s)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, s);
  else
    fwrite(&s, sizeof(short), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, unsigned short us)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, us);
  else
    fwrite(&us, sizeof(unsigned short), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, long l)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, l);
  else
    fwrite(&l, sizeof(long), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, unsigned long ul)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, ul);
  else
    fwrite(&ul, sizeof(unsigned long), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, int i)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, i);
  else
    fwrite(&i, sizeof(int), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, float f)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, f);
  else
    fwrite(&f, sizeof(float), 1, m_pFile);
}

void BinaryFileOutput::Out(const char *pcszFormat, double d)
{
  if( pcszFormat )
    fprintf(m_pFile, pcszFormat, d);
  else
    fwrite(&d, sizeof(double), 1, m_pFile);
}

void BinaryFileOutput::Out(void *pData, int iDataLen)
{
  fwrite(pData, iDataLen, 1, m_pFile);
}

//===========================================================================
/// class Constant
//===========================================================================
//-----------------------------------------------------------------------------
// Expression::Constant::GetValue
//-----------------------------------------------------------------------------
Variant Expression::Constant::GetValue()
{
  return m_vValue;
}
Expression::Constant::~Constant()
{}

//===========================================================================
/// class Expression
//===========================================================================
IValueEvaluator *Expression::s_pValueEval = NULL;

Expression::~Expression()
{}

//-----------------------------------------------------------------------------
// Expression::Eval
//-----------------------------------------------------------------------------
double Expression::Eval()
{
  return double(GetValue());
}

//-----------------------------------------------------------------------------
// Expression::GetValue
//-----------------------------------------------------------------------------
Variant Expression::GetValue()
{
  double dResult = 0;
  for( list<FragmentPtr>::iterator it = m_lstFragment.begin(); it != m_lstFragment.end(); it++ )
  {
    Variant v = (*it)->Value();
    double dVal = 0;
    if( v.Type() == Variant::STRING )
    {
      if( s_pValueEval != NULL )
        dVal = s_pValueEval->Evaluate(v);
    }

    if( (*it)->UnaryOp() != NOP )
    {
      // Unary evaluation: not implemented yet
    }

    switch( (*it)->BinaryOp() )
    {
      case ADD:
        dResult += dVal;
        break;
      case SUB:
        dResult -= dVal;
        break;
      case MUL:
        dResult *= dVal;
        break;
      case DIV:
        dResult /= dVal;
        break;
      case NOP:
      default:
        dResult = dVal;
    }
  }
  return dResult;
}

//-----------------------------------------------------------------------------
// Expression::CharToOp
//-----------------------------------------------------------------------------
Expression::Operation Expression::CharToOp(char c)
{
  switch( c )
  {
    case '+':
      return ADD;
    case '-':
      return SUB;
    case '*':
      return MUL;
    case '/':
      return DIV;
    case 'N':
      return NOP;
    default:
      //## should throw an exception
      return NOP;
  }
}
