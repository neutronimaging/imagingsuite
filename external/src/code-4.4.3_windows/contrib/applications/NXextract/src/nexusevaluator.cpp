//*****************************************************************************
// Synchrotron SOLEIL
//
//
// Creation : 20/07/2005
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

#include "base.h"
#include "date.h"
#include "file.h"
#include "nxfile.h"
#include "membuf.h"
#include "variant.h"

#include <sstream>
#include <cstdlib>
#include <cstring>
#include "nexusevaluator.h"
#include "extractor.h"
#include "templateparsor.h"

// special attributes
const char SIZE_ATTR[] = "_size_";
const char RANK_ATTR[] = "_rank_";
const char NAME_ATTR[] = "_name_";
const char SCANDIM_ATTR[] = "_scandim_";
const char COUNT_ATTR[] = "_count_";

const int  MAX_RANK = 32; // Maximum array rank in NeXus files

//-----------------------------------------------------------------------------
// NexusEvaluator::NexusEvaluator
//-----------------------------------------------------------------------------
NexusEvaluator::NexusEvaluator(const String &strNeXusFile, ICounterVars *pCounterVars)
{
  m_nxf.OpenRead(PSZ(strNeXusFile));
  m_pCounterVars = pCounterVars;
}

//-----------------------------------------------------------------------------
// NexusEvaluator::~NexusEvaluator
//-----------------------------------------------------------------------------
NexusEvaluator::~NexusEvaluator()
{
  m_nxf.Close();
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetSizeAttribute
//-----------------------------------------------------------------------------
bool NexusEvaluator::GetSizeAttribute(const NexusDataSet &nds, const String &_strAttrSpec, DataBuf *pValue)
{
  String strDim, strAttrSpec = _strAttrSpec;
  // get dim
  int iRc = ExtractToken(&strAttrSpec, '(', ')', &strDim);
  if( 1 != iRc )
    return false;
  int iDim = atoi(PSZ(strDim))-1;
  if( iDim < nds.Rank() && iDim >= 0 )
  {
    int iValue = nds.DimArray()[iDim];
    pValue->DataType() = DataBuf::INT;
    pValue->Reset();
    *(pValue->MemBufRef()) << (long)iValue;
    return true;
  }
  else
  {
    cerr << "Warning: Dimension(s) out of bounds!" << endl;
    return false;
  }
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetDataSetInfo
//-----------------------------------------------------------------------------
bool NexusEvaluator::GetDataSetInfo(SSplittedRequest *pRequest, NexusDataSet **ppNxDataSet)
{
  // Get data set from NeXus data file
  *ppNxDataSet = new NexusDataSet;

  // Open data group in NeXus file
  bool bOpened = m_nxf.OpenGroupPath(PSZ(pRequest->strGroupPath), false);
  if( !bOpened )
  {
    cerr << "Warning: Cannot open group from path '" << pRequest->strGroupPath << "'" << endl;
    return false;
  }

  String strDataSet = pRequest->strDataSet;

  // open data set, if needed
  if( !pRequest->strDataSet.empty() )
  {
    if( pRequest->strDataSet.StartWith('<') && pRequest->strDataSet.EndWith('>') )
    {
      // Special case : signal or axis dataset
      strDataSet = pRequest->strDataSet.substr(1, pRequest->strDataSet.size()-2);
      if( strDataSet.IsEqualsNoCase("SDS-signal") )
        // Search for first signal
        m_nxf.SearchFirstDataSetFromAttr("signal", &strDataSet);
      else if( strDataSet.IsEqualsNoCase("SDS-axis") )
        // Search for first axis
        m_nxf.SearchFirstDataSetFromAttr("axis", &strDataSet);
      else if( strDataSet.StartWith("SDS-axis", true) )
      {
        // Search for first axis along given dimension
        String strDim = strDataSet.substr(strlen("SDS-axis"));
        m_nxf.SearchFirstDataSetFromAttr("axis", &strDataSet, strDim);
      }
    }
    // May be empty if 'SearchFirstDataSetFromAttr' didn't find any dataset
    if( strDataSet.empty() )
      return false;

    bOpened = m_nxf.OpenDataSet(PSZ(strDataSet), false);

    if( !bOpened )
      return false;
  }

  if( SSplittedRequest::DATASET_ATTRIBUTE != pRequest->eType &&
      SSplittedRequest::GLOBAL_ATTRIBUTE != pRequest->eType )
  {
    // Read data set content
    m_nxf.GetDataSetInfo(*ppNxDataSet, PSZ(strDataSet));
  }
  return true;
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetDataSet
//-----------------------------------------------------------------------------
bool NexusEvaluator::GetDataSet(SSplittedRequest *pRequest, NexusDataSet **ppNxDataSet, bool bUseCache)
{
  // Check if requested dataset in the same as previous time
 /* if( pRequest->strDataSetPath.IsEquals(m_pairLastNamedDataSet.first) )
  {
      *ppNxDataSet = m_pairLastNamedDataSet.second;
      return true;
  }
  else*/ 
  if( !m_mapDataSetCache.empty() )
  { // Search in cache.
    DataSetMap::iterator i = m_mapDataSetCache.find(pRequest->strDataSetPath);
    if( i != m_mapDataSetCache.end() )
    {
      // if we find it: we get the associated NexusDataSet and return true.  
      *ppNxDataSet = (i->second);
      return true;
    }
  }

  // Get data set from NeXus data file
  *ppNxDataSet = new NexusDataSet;

  // Open data group in NeXus file
  bool bOpened = m_nxf.OpenGroupPath(PSZ(pRequest->strGroupPath), false);
  if( !bOpened )
  {
    cerr << "Warning: Cannot open group from path '" << pRequest->strGroupPath << "'" << endl;
    return false;
  }

  String strDataSet = pRequest->strDataSet;

  // open data set, if needed
  if( !pRequest->strDataSet.empty() )
  {
    if( pRequest->strDataSet.StartWith('<') && pRequest->strDataSet.EndWith('>') )
    {
      // Special case : signal or axis dataset
      strDataSet = pRequest->strDataSet.substr(1, pRequest->strDataSet.size()-2);
      if( strDataSet.IsEqualsNoCase("SDS-signal") )
        // Search for first signal
        m_nxf.SearchFirstDataSetFromAttr("signal", &strDataSet);
      else if( strDataSet.IsEqualsNoCase("SDS-axis") )
        // Search for first axis
        m_nxf.SearchFirstDataSetFromAttr("axis", &strDataSet);
      else if( strDataSet.StartWith("SDS-axis", true) )
      {
        // Search for first axis along given dimension
        String strDim = strDataSet.substr(strlen("SDS-axis"));
        m_nxf.SearchFirstDataSetFromAttr("axis", &strDataSet, strDim);
      }
    }
    // May be empty if 'SearchFirstDataSetFromAttr' didn't find any dataset
    if( strDataSet.empty() )
      return false;

    bOpened = m_nxf.OpenDataSet(PSZ(strDataSet), false);

    if( !bOpened )
      return false;
  }

  if( SSplittedRequest::DATASET_ATTRIBUTE != pRequest->eType &&
      SSplittedRequest::GLOBAL_ATTRIBUTE != pRequest->eType )
  {
    // Read data set content
    m_nxf.GetData(*ppNxDataSet, PSZ(strDataSet));

    if( bUseCache )
      // Add dataset in cache
      m_mapDataSetCache.insert(DataSetMap::value_type(pRequest->strDataSetPath, *ppNxDataSet));
//    m_pairLastNamedDataSet.first = pRequest->strDataSetPath;
//    m_pairLastNamedDataSet.second = *ppNxDataSet;
  }
  return true;
}

//-----------------------------------------------------------------------------
// NexusEvaluator::Evaluate
//-----------------------------------------------------------------------------
bool NexusEvaluator::Evaluate(const String &strRequest, DataBuf *pValue)
{
  try
  {
    if( StartWith(strRequest, "nxs") == false )
      return false;

    // Search for request
    SSplittedRequest *pRequest = GetSplittedRequest(strRequest); // 4 = strlen("nxs:")

    // For array access  
    int aiDim[MAX_RANK] = {1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    NexusDataSet *pNxData = NULL;

    // Emptying output buffer
    pValue->Reset();

    bool bUseCache = false;
    switch( pRequest->eType )
    {
      case SSplittedRequest::GLOBAL_ATTRIBUTE:
      case SSplittedRequest::DATASET_ATTRIBUTE:
      case SSplittedRequest::DATASET_ARRAY_ELEMENT:
        bUseCache = true;
        if( !GetDataSet(pRequest, &pNxData) )
          return false;
        break;
      case SSplittedRequest::DATASET:
        // In case of reading a entire dataset, don't put it in cache after reading
        if( !GetDataSet(pRequest, &pNxData, false) )
          return false;
        break;
      case SSplittedRequest::NAME_ATTRIBUTE:
      case SSplittedRequest::RANK_ATTRIBUTE:
      case SSplittedRequest::SIZE_ATTRIBUTE:
        if( !GetDataSetInfo(pRequest, &pNxData) )
          return false;
        break;
      default:
        break;
    }

    switch( pRequest->eType )
    {
      case SSplittedRequest::DATASET_ARRAY_ELEMENT:
        // Fill dim array
        GetCoordinates(*pRequest, aiDim, pNxData->Rank());
        GetDataEx(*pNxData, aiDim, pValue);
        break;

      case SSplittedRequest::DATASET:
        // get datas
        GetData(*pNxData, aiDim, pValue, true);
        break;

      case SSplittedRequest::GLOBAL_ATTRIBUTE:
      case SSplittedRequest::DATASET_ATTRIBUTE:
      {
        if( pNxData->Data() == NULL )
        {
          NexusDataType nxType = NX_NONE;
          if( ReadAttribute(pRequest->strAttr, pValue, &nxType) )
          {
            // Put value into a fake data set
            pNxData->SetInfo(nxType, 1);
            if( NX_CHAR == nxType )
              pNxData->SetDimension(0, pValue->MemBufRef()->Len());
            else
              pNxData->SetDimension(0, 1);
            char *pBuf = new char[pValue->MemBufRef()->Len()];
            memcpy(pBuf, pValue->Buf(), pValue->MemBufRef()->Len());
            pNxData->SetData(pBuf);
          }
          else
            return false;
        }
        else
          GetData(*pNxData, aiDim, pValue);
        break;
      }

      case SSplittedRequest::NAME_ATTRIBUTE:
      {
        pValue->DataType() = DataBuf::CHAR;
        pValue->MemBufRef()->PutBloc(PSZ(pRequest->strDataSet), pRequest->strDataSet.size());
        *(pValue->MemBufRef()) << char('\0');
        break;
      }

      case SSplittedRequest::RANK_ATTRIBUTE:
      {
        int iRank = pNxData->Rank();
        pValue->DataType() = DataBuf::INT;
        pValue->MemBufRef()->PutBloc(&iRank, sizeof(int));
        break;
      }

      case SSplittedRequest::SIZE_ATTRIBUTE:
        return GetSizeAttribute(*pNxData, pRequest->strLastPart, pValue);

      case SSplittedRequest::SCANDIM_ATTRIBUTE:
      {
        int iDim = m_nxf.GetScanDim(PSZ(pRequest->strGroupPath));
        pValue->DataType() = DataBuf::INT;
        pValue->MemBufRef()->PutBloc(&iDim, sizeof(int));
        break;
      }

      default:
        break;
    }

    // If the NeXus dataset is not put in cache it ban be deleted since no longer referenced
    if( !bUseCache && pNxData )
      delete pNxData;
  }
  catch(Exception e)
  {
    e.PrintMessage();
    throw e;
  }
  catch(NexusException e)
  {
    e.PrintMessage();
    throw e;
  }

  return true;
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetCoordinates
//-----------------------------------------------------------------------------
void NexusEvaluator::GetCoordinates(const SSplittedRequest &Request, int *piDim, int iRank)
{
  for(uint ui = 0; ui < Request.vecArrayCoordinates.size(); ui++ )
  {
    if( ui > (uint)iRank )
    {
      cerr << "Warning: Ignore coordinates in excessive number" << endl;
      break;
    }
    bool bVal = m_pCounterVars->GetValue(Request.vecArrayCoordinates[ui], &piDim[ui]);
    if( !bVal )
      piDim[ui] = atoi(PSZ(Request.vecArrayCoordinates[ui]));
  }
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetSplittedRequest
//-----------------------------------------------------------------------------
SSplittedRequest *NexusEvaluator::GetSplittedRequest(const String &strRequest)
{
 /* if( strRequest.IsEquals(m_pairLastRequest.first) )
    return &(*m_pairLastRequest.second).second;
  else */
  if( !m_mapRequestCache.empty() )
  { // Search in cache first
    MapRequests::iterator it = m_mapRequestCache.find(strRequest);
    if( it != m_mapRequestCache.end() )
      return &(it->second);
  }

  SSplittedRequest aRequest;
  aRequest.strGroupPath = strRequest.substr(strlen("nxs:"));
  uint uiSep = aRequest.strGroupPath.find_last_of('/');
  // Extract Attribute part
  uint uiLastDot = aRequest.strGroupPath.find_last_of('.');
  if( uiLastDot != String::npos && 
      ((uiLastDot > 0 && '\\' != aRequest.strGroupPath[uiLastDot-1]) 
      || 0 == uiLastDot) && (uiSep == String::npos || (uiSep != String::npos && uiSep < uiLastDot)) )
  {
    // Get Attribute name
    aRequest.strAttr = aRequest.strGroupPath.substr(uiLastDot+1);
    if( uiLastDot != String::npos )
      aRequest.strGroupPath.erase(uiLastDot);
    // Extract extra information
    uint uiBrace = aRequest.strAttr.find_first_of('(');
    if( uiBrace != String::npos && uiBrace > 0 )
    {
      aRequest.strLastPart = aRequest.strAttr.substr(uiBrace);
      aRequest.strAttr.erase(uiBrace);
    }
  }

  // Extract data set part
  if( uiSep != String::npos )
  {
    aRequest.strDataSet = aRequest.strGroupPath.substr(uiSep+1);
    if( !aRequest.strDataSet.empty() )
    {
      aRequest.strGroupPath.erase(uiSep);
      uint uiBrace = aRequest.strDataSet.find_first_of('[');
      if( uiBrace != String::npos && uiBrace > 0 )
      {
        aRequest.strLastPart = aRequest.strDataSet.substr(uiBrace);
        aRequest.strDataSet.erase(uiBrace);
      }
    }
  }

  // Check for request type
  if( !aRequest.strAttr.empty() )
  {
    if( IsEqualsNoCase(aRequest.strAttr, SIZE_ATTR) )
      aRequest.eType = SSplittedRequest::SIZE_ATTRIBUTE;
    else if( IsEqualsNoCase(aRequest.strAttr, RANK_ATTR) )
      aRequest.eType = SSplittedRequest::RANK_ATTRIBUTE;
    else if( IsEqualsNoCase(aRequest.strAttr, NAME_ATTR) )
      aRequest.eType = SSplittedRequest::NAME_ATTRIBUTE;
    else if( IsEqualsNoCase(aRequest.strAttr, SCANDIM_ATTR) )
      aRequest.eType = SSplittedRequest::SCANDIM_ATTRIBUTE;
    else if( !aRequest.strDataSet.empty() )
      aRequest.eType = SSplittedRequest::DATASET_ATTRIBUTE;
    else
      aRequest.eType = SSplittedRequest::GLOBAL_ATTRIBUTE;
  }
  else if( !aRequest.strDataSet.empty() )
  {
    if( !aRequest.strLastPart.empty() )
      aRequest.eType = SSplittedRequest::DATASET_ARRAY_ELEMENT;
    else
      aRequest.eType = SSplittedRequest::DATASET;
  }
  else
    // Bad request
    return NULL;

  // Compute data set path
  aRequest.strDataSetPath = aRequest.strGroupPath;
  if( !aRequest.strDataSet.empty() )
    aRequest.strDataSetPath += '/' + aRequest.strDataSet;

  if( SSplittedRequest::DATASET_ATTRIBUTE == aRequest.eType ||
      SSplittedRequest::GLOBAL_ATTRIBUTE == aRequest.eType )
    aRequest.strDataSetPath += '.' + aRequest.strAttr;

  if( SSplittedRequest::DATASET_ARRAY_ELEMENT == aRequest.eType )
  {
    // Extract coordinates for array access
    String strCoord, str = aRequest.strLastPart;
    while( !str.empty() )
    {
      ExtractToken(&str, '[', ']', &strCoord);
      aRequest.vecArrayCoordinates.insert(aRequest.vecArrayCoordinates.begin(), strCoord);
    }
  }

  // Insert request in cache
  pair<MapRequests::iterator, bool> pair = m_mapRequestCache.insert(MapRequests::value_type(strRequest, aRequest));
//  m_pairLastRequest.first = strRequest;
//  m_pairLastRequest.second = pair.first;

  return &((pair.first)->second);
  //## todo: Replace escaped '.' characters
}

#define TData(T, type) {\
  pValue->DataType() = type;\
  pValue->MemBufRef()->PutBloc((T *)(nxDataSet.Data()) + iOffset,  sizeof(T));}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetScalarDataFromArray
//-----------------------------------------------------------------------------
void NexusEvaluator::GetScalarDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue)
{
  // Check coordinates & calculate rank
  int iRank = nxDataSet.Rank();
  // Calculate data offset in case of array access
  int iOffset = 0;
  for( int i = 0; i < iRank; i++ )
  {
    int iBase = 1;
    for( int j = 0; j < i; j++ )
      iBase *= nxDataSet.DimArray()[iRank - j - 1];
    iOffset += iBase * piDim[i];
  }

  // Get data according to its type
  switch( nxDataSet.DataType() )
  {
    case NX_INT8:
      TData(uint8, DataBuf::BYTE);
      break;
    case NX_INT16:
      TData(short, DataBuf::SHORT)
      break;
    case NX_UINT16:
      TData(unsigned short, DataBuf::USHORT)
      break;
    case NX_INT32:
      TData(long, DataBuf::LONG)
      break;
    case NX_UINT32:
      TData(unsigned long, DataBuf::ULONG)
      break;
    case NX_FLOAT32:
      TData(float, DataBuf::FLOAT)
      break;
    case NX_FLOAT64:
      TData(double, DataBuf::DOUBLE)
      break;
    case NX_CHAR:
    {
      if( nxDataSet.Data() != NULL )
      {
        pValue->DataType() = DataBuf::CHAR;
        // Copy char array
        pValue->MemBufRef()->PutBloc(nxDataSet.Data(), nxDataSet.DimArray()[0]);
        *(pValue->MemBufRef()) << char('\0');
      }
    }
    break;
    default:
	  break;
  }

  pValue->SetRank(1);
  pValue->SetSize(0, 1);
}

#define TData1D(T, type) {\
  pValue->DataType() = type;\
  pValue->MemBufRef()->PutBloc((T *)(nxDataSet.Data()) + iOffset,  iDataDim * sizeof(T));}

//-----------------------------------------------------------------------------
// NexusEvaluator::Get1DDataFromArray
//-----------------------------------------------------------------------------
void NexusEvaluator::Get1DDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue)
{
  // Check coordinates & calculate rank
  int iRank = nxDataSet.Rank();
  // Calculate data offset
  int iOffset = 0;
  int iDataDim = nxDataSet.DimArray()[iRank - 1];
  for( int i = 0; i < iRank-1; i++ )
  {
    int iBase = iDataDim;
    for( int j = 0; j < i; j++ )
      iBase *= nxDataSet.DimArray()[iRank - j - 2];
    iOffset += iBase * piDim[i];
  }

  // Get data according to its type
  switch( nxDataSet.DataType() )
  {
    case NX_INT8:
      TData1D(uint8, DataBuf::BYTE);
      break;
    case NX_INT16:
      TData1D(short, DataBuf::SHORT)
      break;
    case NX_UINT16:
      TData1D(unsigned short, DataBuf::USHORT)
      break;
    case NX_INT32:
      TData1D(long, DataBuf::LONG)
      break;
    case NX_UINT32:
      TData1D(unsigned long, DataBuf::ULONG)
      break;
    case NX_FLOAT32:
      TData1D(float, DataBuf::FLOAT)
      break;
    case NX_FLOAT64:
      TData1D(double, DataBuf::DOUBLE)
      break;
    break;
    default:
	  break;
  }

  pValue->SetRank(1);
  pValue->SetSize(0, iDataDim);
}

#define TData2D(T, type) {\
  pValue->DataType() = type;\
  pValue->MemBufRef()->PutBloc((T *)(nxDataSet.Data()) + iOffset,  iDataDim * sizeof(T));}

//-----------------------------------------------------------------------------
// NexusEvaluator::Get2DDataFromArray
//-----------------------------------------------------------------------------
void NexusEvaluator::Get2DDataFromArray(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue)
{
  // Check coordinates & calculate rank
  int iRank = nxDataSet.Rank();
  // Calculate data offset
  int iOffset = 0;
  int iDataDim = nxDataSet.DimArray()[iRank - 1] * nxDataSet.DimArray()[iRank - 2];
  for( int i = 0; i < iRank-2; i++ )
  {
    int iBase = iDataDim;
    for( int j = 0; j < i; j++ )
      iBase *= nxDataSet.DimArray()[iRank - j - 3];
    iOffset += iBase * piDim[i];
  }

  // Get data according to its type
  switch( nxDataSet.DataType() )
  {
    case NX_INT8:
      TData2D(uint8, DataBuf::BYTE);
      break;
    case NX_INT16:
      TData2D(short, DataBuf::SHORT)
      break;
    case NX_UINT16:
      TData2D(unsigned short, DataBuf::USHORT)
      break;
    case NX_INT32:
      TData2D(long, DataBuf::LONG)
      break;
    case NX_UINT32:
      TData2D(unsigned long, DataBuf::ULONG)
      break;
    case NX_FLOAT32:
      TData2D(float, DataBuf::FLOAT)
      break;
    case NX_FLOAT64:
      TData2D(double, DataBuf::DOUBLE)
      break;
    break;
    default:
	  break;
  }

  pValue->SetRank(1);
  pValue->SetSize(0, nxDataSet.DimArray()[iRank - 1]);
  pValue->SetSize(1, nxDataSet.DimArray()[iRank - 2]);
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetDataEx
//-----------------------------------------------------------------------------
void NexusEvaluator::GetDataEx(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue, bool bGetAllBuffer)
{
  // Emptying buffer first
  pValue->Reset();

  if( bGetAllBuffer )
  { //
    // In the case of number values array with no given index get entire dataset
    //
    // Emptying buffer
    pValue->Reset();

    // Fill buffer with data
    pValue->MemBufRef()->PutBloc(nxDataSet.Data(), nxDataSet.BufferSize());

    pValue->SetTypeFromNeXusType(nxDataSet.DataType());
    pValue->SetRank(nxDataSet.Rank());
    for( int i = 0; i < nxDataSet.Rank(); i++ )
      pValue->SetSize(i, nxDataSet.DimArray()[i]);
    return;
  }

  // Check coordinates & calculate rank
  int iRank = nxDataSet.Rank(), i=0;

  for(i = 0; i < iRank; i++ )
  {
    if( -1 == piDim[i] )
      break;


//## 29/01/10 : à vérifier
//    if( piDim[i] < 0 || piDim[i] > nxDataSet.DimArray()[iRank - i - 1] )
    if( piDim[i] < 0 || piDim[i] > nxDataSet.DimArray()[i] )
    {
      throw Exception(PSZ(StrFormat("Cannot read array at index %d on dimension %d. Dimension size is %d", piDim[i], i+1, nxDataSet.DimArray()[i])), "ARRAY_OUT_OF_BOUNDS", "NexusEvaluator::GetData");
    }
  }
  int iDimMax = i;
  // Dimension of data returned
  int iDataDim = iRank - i;

  switch( iDataDim )
  {
    case 0:
      GetScalarDataFromArray(nxDataSet, piDim, pValue);
      break;
    case 1:
      Get1DDataFromArray(nxDataSet, piDim, pValue);
      break;
    case 2:
      Get2DDataFromArray(nxDataSet, piDim, pValue);
      break;
    default:
      throw Exception(PSZ(StrFormat("Array slicing in more than 2D is not yet supported", "BAD_PARAMETERS", "NexusEvaluator::GetData")));
  }
  pValue->SetTypeFromNeXusType(nxDataSet.DataType());
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetData
//-----------------------------------------------------------------------------
void NexusEvaluator::GetData(const NexusDataSet &nxDataSet, int *piDim, DataBuf *pValue, bool bGetAllBuffer)
{
  // Emptying buffer first
  pValue->Reset();

  if( bGetAllBuffer )
  { //
    // In the case of number values array with no given index get entire dataset
    //
    // Emptying buffer
    pValue->Reset();

    // Fill buffer with data
    pValue->MemBufRef()->PutBloc(nxDataSet.Data(), nxDataSet.BufferSize());

    pValue->SetTypeFromNeXusType(nxDataSet.DataType());
    pValue->SetRank(nxDataSet.Rank());
    for( int i = 0; i < nxDataSet.Rank(); i++ )
      pValue->SetSize(i, nxDataSet.DimArray()[i]);
    return;
  }

  // Check coordinates & calculate rank
  int iRank = nxDataSet.Rank();

  for( int i = 0; i < iRank; i++ )
  {
    if( -1 == piDim[i] )
      break;

//## 29/01/10 : à vérifier
//    if( piDim[i] < 0 || piDim[i] > nxDataSet.DimArray()[iRank - i - 1] )
    if( piDim[i] < 0 || piDim[i] > nxDataSet.DimArray()[i] )
    {
      throw Exception(PSZ(StrFormat("Cannot read array at index %d on dimension %d. Dimension size is %d", piDim[i], i+1, nxDataSet.DimArray()[iRank])), "ARRAY_OUT_OF_BOUNDS", "NexusEvaluator::GetData");
    }
  }

  // Calculate data offset in case of array access
  int iOffset = 0;
  switch( iRank )
  {
    case 1:
      iOffset = piDim[0];
      break;
    case 2:
      iOffset = nxDataSet.DimArray()[iRank-1] * (piDim[1]) + (piDim[0]);
      break;
    default:
    {    
      int iBase, j;
      for( int i = 0; i < iRank; i++ )
      {
        iBase = 1;
        for( j = 0; j < i; j++ )
          iBase *= nxDataSet.DimArray()[iRank - j - 1];
        iOffset += iBase * piDim[i];
      }
      break;
    }
  }

  // Get data according to its type
  switch( nxDataSet.DataType() )
  {
    case NX_INT8:
      TData(uint8, DataBuf::BYTE);
      break;
    case NX_INT16:
      TData(short, DataBuf::SHORT)
      break;
    case NX_UINT16:
      TData(unsigned short, DataBuf::USHORT)
      break;
    case NX_INT32:
      TData(long, DataBuf::LONG)
      break;
    case NX_UINT32:
      TData(unsigned long, DataBuf::ULONG)
      break;
    case NX_FLOAT32:
      TData(float, DataBuf::FLOAT)
      break;
    case NX_FLOAT64:
      TData(double, DataBuf::DOUBLE)
      break;
    case NX_CHAR:
    {
      if( nxDataSet.Data() != NULL )
      {
        pValue->DataType() = DataBuf::CHAR;
        // Copy char array
        pValue->MemBufRef()->PutBloc(nxDataSet.Data(), nxDataSet.DimArray()[0]);
        *(pValue->MemBufRef()) << char('\0');
      }
    }
    break;
    default:
	  break;
  }

  pValue->SetRank(1);
  pValue->SetSize(0, 1);
}

//-----------------------------------------------------------------------------
// NexusEvaluator::GetItems
//-----------------------------------------------------------------------------
ItemList *NexusEvaluator::GetItemsList(String *pstrPath, const String &strName)
{
  // First search in cache
  ListCache::iterator itCache = m_mapListCache.find(*pstrPath);
  if( itCache != m_mapListCache.end() )
    return &(itCache->second);

  else
  {
    String strTypeName;
    String strAxisDimension;
    String strPath = *pstrPath;
    int    iSignalDimension = -1;

    // NeXus dataset ?
    if( pstrPath->StartWith("nxs:") )
      pstrPath->erase(0, strlen("nxs:"));

    while( pstrPath->EndWith('/') )
      // Suppress leading slashes
      pstrPath->erase(pstrPath->size()-1);

    // Get item on which iterate
    pstrPath->ExtractTokenRight('<', '>', &strTypeName);

    bool bAxis = false, bSignal = false, bGroup = false;
    if( strTypeName.IsEqualsNoCase("SDS-signal") || strTypeName.IsEqualsNoCase("scan-data"))
      bSignal = true;
    else if( strTypeName.StartWith("SDS-signal", true) )
    {
      bSignal = true;
      // Read the specified signal dimension
      String strSignalDimension = strTypeName.substr(strlen("SDS-signal"));
      iSignalDimension = atoi(PSZ(strSignalDimension));
    }
    else if( strTypeName.StartWith("scan-data", true) )
    {
      bSignal = true;
      // Read the specified signal dimension
      String strSignalDimension = strTypeName.substr(strlen("scan-data"));
      iSignalDimension = atoi(PSZ(strSignalDimension)) + m_nxf.GetScanDim(PSZ(*pstrPath));
    }
    else if( strTypeName.IsEqualsNoCase("SDS-axis") )
      bAxis = true;
    else if( strTypeName.StartWith("SDS-axis", true) )
    {
      bAxis = true;
      // Read the specified dimension
      strAxisDimension = strTypeName.substr(strlen("sds-axis"));
    }
    else if( strTypeName.IsEqualsNoCase("Dataset") || strTypeName.IsEqualsNoCase("SDS") )
     ;
    else
      bGroup = true;

    // Get Items
    ItemList lstItems;

    // Check for wildcard
    uint uiQMarkPos = pstrPath->find("/?/");
    if( uiQMarkPos != string::npos )
    {
      if( bAxis || bSignal )
      {
        cerr << "To Iterate over SDS-Axis or SDS-signal, parent group type must be fully determined" << endl;
        return NULL;
      }

      vector<string> vecPaths;
      m_nxf.SearchGroup(NULL, PSZ(strTypeName), &vecPaths, PSZ(pstrPath->substr(0, uiQMarkPos)));
      for( uint ui = 0; ui < vecPaths.size(); ui++ )
      {
        // Build item pair : (name ,path)
        String strTmp = vecPaths[ui], strName;
        strTmp.ExtractTokenRight('<', '>', &strName);
        strTmp.ExtractTokenRight('/', &strName);
        lstItems.push_back(NxItem(strName, vecPaths[ui]));
      }
    }
    else
    {
      // Open parent group
      m_nxf.OpenGroupPath(PSZ(*pstrPath));

      // Check opened group type
      if( !bGroup && !String(m_nxf.CurrentGroupClass()).IsEqualsNoCase("NXdata") )
      {
        cerr << "To Iterate over SDS-Axis or SDS-signal, parent group type must be 'NXdata'" << endl;
        return NULL;
      }

      NexusItemInfo  ItemInfo;
      int  iStatus = m_nxf.GetFirstItem(&ItemInfo);
      while( iStatus != NX_EOD )
      {
        if( bGroup && ItemInfo.IsGroup() && strTypeName.IsEquals(ItemInfo.ClassName()) )
          // Store the item name and path
          lstItems.push_back(NxItem(ItemInfo.ItemName(), (*pstrPath) + ItemInfo.ItemName()));

        // Retreive all signal regardless dimensions
        else if( bSignal && ItemInfo.IsDataSet() && m_nxf.HasAttribute("signal", ItemInfo.ItemName()) && -1 == iSignalDimension )
          lstItems.push_back(NxItem(ItemInfo.ItemName(), (*pstrPath) + ItemInfo.ItemName()));

        // Retreive signal of specified dimension size
        else if( bSignal && ItemInfo.IsDataSet() && m_nxf.HasAttribute("signal", ItemInfo.ItemName()) )
        {
          NexusDataSetInfo info;
          m_nxf.GetDataSetInfo(&info, ItemInfo.ItemName());
          if( info.Rank() == iSignalDimension )
            lstItems.push_back(NxItem(ItemInfo.ItemName(), (*pstrPath) + ItemInfo.ItemName()));
        }

        // Retreive axis datasets of given dimension
        else if( bAxis && ItemInfo.IsDataSet() && m_nxf.HasAttribute("axis", ItemInfo.ItemName(), strAxisDimension) )
          lstItems.push_back(NxItem(ItemInfo.ItemName(), (*pstrPath) + ItemInfo.ItemName()));

        iStatus = m_nxf.GetNextItem(&ItemInfo);
      }
    }
    m_nxf.CloseAllGroups();

    // Add list in cache and return pointer to inserted object
    return &(m_mapListCache.insert(ListCache::value_type(strPath, lstItems)).first->second);
  }
}

//-----------------------------------------------------------------------------
// NexusEvaluator::ReadAttribute
//-----------------------------------------------------------------------------
bool NexusEvaluator::ReadAttribute(const String &strAttr, DataBuf *pValue, NexusDataType *pNxType)
{
  NexusAttrInfo AttrInfo;
  int iStatus = m_nxf.GetFirstAttribute(&AttrInfo);
  // we look for the attribute strAttr.
  while( iStatus != NX_EOD )
  {
    if( IsEqualsNoCase(strAttr, AttrInfo.AttrName()) )
    {
      pValue->MemBufRef()->SetLen(4096);
      pValue->Empty();
      int iBufLen = pValue->MemBufRef()->BufLen();
      m_nxf.GetAttribute(AttrInfo.AttrName(), &iBufLen, pValue->Buf(), AttrInfo.DataType());
      // Put real length
      pValue->MemBufRef()->SetLen(iBufLen);

      ostringstream oss;
      switch (AttrInfo.DataType()) 
      {
        case NX_CHAR:
          pValue->DataType() = DataBuf::CHAR;
          break;
        case NX_INT:
          pValue->DataType() = DataBuf::INT;
          break;
        case NX_INT32:
          pValue->DataType() = DataBuf::LONG;
          break;
        case NX_INT16:
          pValue->DataType() = DataBuf::SHORT;
          break;
        case NX_INT8:
          pValue->DataType() = DataBuf::BYTE;
          break;
        case NX_FLOAT32:
          pValue->DataType() = DataBuf::FLOAT;
          break;
        case NX_FLOAT64:
          pValue->DataType() = DataBuf::DOUBLE;
          break;
        default:
          return false;
      }
      *pNxType = AttrInfo.DataType();
      return true;
    }
    iStatus = m_nxf.GetNextAttribute(&AttrInfo);
  }
  return false;
}

//-----------------------------------------------------------------------------
// NexusEvaluator::CheckItem
//-----------------------------------------------------------------------------
bool NexusEvaluator::CheckItem(const String &_strPath)
{
  String strPath = _strPath, strDataSet, strAttribut;
  if( strPath.StartWith("nxs:") )
    strPath.erase(0, strlen("nxs:"));

  // Attribute part ?
  if( strPath.find_last_of('.') != string::npos )
    strPath.ExtractTokenRight('.', &strAttribut);

  if( strPath.EndWith('/') )
  {
    // It is a group path
    if( !m_nxf.OpenGroupPath(PSZ(strPath), false) )
      return false;
    // Group attribute ?
    if( !strAttribut.empty() && !m_nxf.HasAttribute(PSZ(strAttribut)) )
      return false;
    return true;
  }

  // Extract DataSet part

  strPath.ExtractTokenRight('/', &strDataSet);

  if( !m_nxf.OpenGroupPath(PSZ(strPath), false) )
    return false;

  // Try to opening dataset
  if( !strDataSet.empty() && m_nxf.OpenDataSet(PSZ(strDataSet), false)  == false )
     return false;

  // Check attribute
  if( !strAttribut.empty() && !m_nxf.HasAttribute(PSZ(strAttribut)) )
    return false;

  return true;
}
