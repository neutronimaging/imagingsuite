//*****************************************************************************
/// Synchrotron SOLEIL
///
/// NeXus C++ API over NAPI
///
/// Creation : 16/02/2005
/// Author   : Stephane Poirier
///
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the GNU General Public License as published by the Free Software
/// Foundation; version 2 of the License.
/// 
/// This program is distributed in the hope that it will be useful, but WITHOUT 
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
/// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
///
//*****************************************************************************

#ifndef NEXUSAPI
  #ifdef WIN32
    #include "napi.h"
  #else // LINUX
    #include <napi.h>
  #endif
#endif

#ifdef WIN32
  #pragma warning(disable:4786)
#endif

// Use enum instead of #define for NeXus DataTypes
#undef NX_FLOAT32
#undef NX_FLOAT64
#undef NX_INT8   
#undef NX_UINT8  
#undef NX_INT16  
#undef NX_UINT16 
#undef NX_INT32  
#undef NX_UINT32 
#undef NX_INT64  
#undef NX_UINT64 
#undef NX_CHAR   
#undef NX_BINARY

// Use enum instead of #define for Nexus Status codes
#undef NX_OK
#undef NX_EOD

#include "base.h"
#include "file.h"
#include <cstring>

#include "nxfile.h"

// STL headers
#include <string>
#include <list>
#include <map>
#include <vector>
#include <stack>

#include <sstream>

using namespace soleil;

// Error msgs
const char CREATE_FILE_ERR[]        = "Cannot create file";
const char OPENING_FILE_ERR[]       = "Cannot open file";
const char OPENING_FILE_READ_ERR[]  = "Cannot open file for reading";
const char OPENING_FILE_RW_ERR[]    = "Cannot open file for reading and writing";
const char CREATE_DATA_ERR[]        = "Cannot create dataset '%s' (group '%s', file '%s')";
const char BAD_GROUP_NAME_ERR[]     = "Null value passed as group name";
const char OPEN_GROUP_ERR[]         = "Cannot open group '%s' (file '%s')";
const char MAKE_GROUP_ERR[]         = "Cannot create group '%s' (file '%s')";
const char OPEN_DATA_ERR[]          = "Cannot open dataset '%s' (group '%s', file '%s')";
const char CLOSE_DATA_ERR[]         = "Cannot close dataset (group '%s', file '%s')";
const char CLOSE_GROUP_ERR[]        = "Cannot close group '%s' in file '%s'";
const char PUT_DATA_ERR[]           = "Cannot put data in dataset '%s' (group '%s', file '%s')";
const char PUT_DATASUBSET_ERR[]     = "Cannot put data subset in dataset '%s' (group '%s', file '%s')";
const char PUT_ATTR_ERR[]           = "Cannot write attribute '%s' (group '%s', file '%s')";
const char GET_ATTR_ERR[]           = "Cannot read attribute '%s' (group '%s', file '%s')";
const char GETINFO_ERR[]            = "Cannot read data set infos (dataset '%s', group '%s', file '%s')";
const char GETDATA_ERR[]            = "Cannot read data set (dataset '%s', group '%s', file '%s')";
const char FLUSH_ERR[]              = "Cannot flush data (file '%s')";
const char GET_INIT_ENUM_ITEM[]     = "Cannot initialize group items enumeration (file '%s')";
const char GET_RETREIVE_ITEM[]      = "Cannot retreive group items information (file '%s')";
const char GET_INIT_ENUM_ATTR[]     = "Cannot initialize attributes enumeration (file '%s')";
const char GET_RETREIVE_ATTR[]      = "Cannot retreive attributes information (file '%s')";
const char LINK_ITEM_ERR[]          = "Cannot link item to current group (file '%s')";
const char GET_GROUPINFO_ERR[]      = "Cannot read group infos (group '%s', file '%s')";
const char GET_ATTRINFO_ERR[]       = "Cannot read attributes infos (group '%s', file '%s')";
const char NO_SIGNAL_ATTR_ERR[]     = "Cannot find 'signal' attribute";

// Reasons
const char MISSING_FILE_NAME[]      = "Missing file name !";
const char UNABLE_TO_OPEN_FILE[]    = "Unable to open '%s'";
const char NO_HANDLE[]              = "Missing file handle !";
const char OPERATION_FAILED[]       = "Operation failed !";
const char NO_DATASET[]             = "No data set !";
const char TYPES_MISMATCH[]         = "Type mismatch !";
const char BAD_DATASET[]            = "Bad dataset";

// Constantes
const int MAX_DIM = 32;              // Max rank
const int MAX_NAME_LENGTH = 256;      // For class, attributes, group names
const char DATASET_CLASS[] = "SDS";  // Class name for NeXus data set
const char ROOT_LEVEL[] = "root(NXroot)";    // Top level group of a NeXus file
const string g_strNoDataSet = "(no data set)";

//---------------------------------------------------------------------------
// Free function that return a char * from a string objet
//---------------------------------------------------------------------------
inline char *PCSZToPSZ(const char *pcsz)
{
  return (char *)((void *)pcsz);
}

//=============================================================================
// NeXus exceptions
//
//=============================================================================
NexusException::NexusException(const char *pcszError, const char *pcszReason,
                               const char *pcszMethod)
{
  m_pError = (void *)(new string(pcszError));
  m_pReason = (void *)(new string(pcszReason));
  m_pMethod = (void *)(new string(pcszMethod));
}

//---------------------------------------------------------------------------
// NexusException::PrintMessage
//---------------------------------------------------------------------------
void NexusException::PrintMessage()
{
  cerr << "Nexus exception!" << endl
       << "Desc  : " << *( string *)m_pError  << endl
       << "Reason: " << *( string *)m_pReason << endl
       << "Origin: " << *( string *)m_pMethod << endl;
}

//---------------------------------------------------------------------------
// NexusException::GetMessage
//---------------------------------------------------------------------------
void NexusException::GetMsg(char *pBuf, int iLen)
{
  string strMsg;
  strMsg  = "Nexus exception: " + *( string *)m_pError
          + ". Reason: " + *( string *)m_pReason
          + ". Origin: " + *( string *)m_pMethod + ".";
  strncpy(pBuf, strMsg.c_str(), iLen - 1);
}

//---------------------------------------------------------------------------
// NexusException::Error
//---------------------------------------------------------------------------
const char *NexusException::Error() const
{
  return (*( string *)m_pError).c_str();
}

//---------------------------------------------------------------------------
// NexusException::Reason
//---------------------------------------------------------------------------
const char *NexusException::Reason() const
{
  return (*( string *)m_pReason).c_str();
}

//---------------------------------------------------------------------------
// NexusException::Method
//---------------------------------------------------------------------------
const char *NexusException::Method() const
{
  return (*( string *)m_pMethod).c_str();
}

//=============================================================================
// NeXus item identifier
//
// This class holds a nexus links
//=============================================================================
NexusItemID::NexusItemID()
{
  m_pLink = new NXlink;
}

NexusItemID::~NexusItemID()
{
  delete (NXlink *)m_pLink;
}

//=============================================================================
// NeXus item info
//
//=============================================================================
NexusItemInfo::NexusItemInfo()
{
  m_pszItem = new char[MAX_NAME_LENGTH];
  m_pszClass = new char[MAX_NAME_LENGTH];
}

NexusItemInfo::~NexusItemInfo()
{
  delete [] m_pszItem;
  delete [] m_pszClass;
}

bool NexusItemInfo::IsDataSet() const
{
  return !strncmp(m_pszClass, DATASET_CLASS, 3);
}

bool NexusItemInfo::IsGroup() const
{
  return !strncmp(m_pszClass, "NX", 2);
}

//=============================================================================
// NeXus attribute info
//
//=============================================================================
NexusAttrInfo::NexusAttrInfo()
{
  m_pszName = new char[MAX_NAME_LENGTH];
}

NexusAttrInfo::~NexusAttrInfo()
{
  delete [] m_pszName;
}

//=============================================================================
// NeXus Data set info
//
//=============================================================================

//---------------------------------------------------------------------------
// NexusDataSetInfo::DataTypeSize
//---------------------------------------------------------------------------
int NexusDataSetInfo::DataTypeSize(NexusDataType eDataType)
{
  switch( eDataType )
  {
    case NX_INT16:
    case NX_UINT16:
      return sizeof(short);
    case NX_INT32:
    case NX_UINT32:
      return sizeof(long);
    case NX_INT64:
    case NX_UINT64:
      return sizeof(int64);
    case NX_FLOAT32:
      return sizeof(float);
    case NX_FLOAT64:
      return sizeof(double);
    default:
      return 1;
  }
}

//---------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------
NexusDataSetInfo::NexusDataSetInfo()
{
  m_eDataType = NX_NONE;
  m_iRank = 0;
  m_piDim = new int[MAX_DIM];
}

//---------------------------------------------------------------------------
// Destructors
//---------------------------------------------------------------------------
NexusDataSetInfo::~NexusDataSetInfo()
{
  Clear();
  delete [] m_piDim;
}

//---------------------------------------------------------------------------
// NexusDataSetInfo::Clear
//---------------------------------------------------------------------------
void NexusDataSetInfo::Clear()
{
  m_eDataType = NX_NONE;
  m_iRank = 0;
}

//---------------------------------------------------------------------------
// NexusDataSetInfo::IsEmpty
//---------------------------------------------------------------------------
bool NexusDataSetInfo::IsEmpty() const
{
  if( NX_NONE == m_eDataType && 0 == m_iRank )
    return true;
  return false;
}

//---------------------------------------------------------------------------
// NexusDataSetInfo::SetInfo
//---------------------------------------------------------------------------
void NexusDataSetInfo::SetInfo(NexusDataType eDataType, int iRank)
{
  m_eDataType = eDataType;
  m_iRank = iRank;
}

//---------------------------------------------------------------------------
// NexusDataSetInfo::Size
//---------------------------------------------------------------------------
int NexusDataSetInfo::Size() const
{
  int iSize = 1;
  for( int i = 0; i < m_iRank; i++ )
    iSize *= m_piDim[i];
  return iSize;
}

//---------------------------------------------------------------------------
// NexusDataSet::DatumSize
//---------------------------------------------------------------------------
unsigned int NexusDataSetInfo::DatumSize() const
{
  switch( m_eDataType )
  {
    case NX_INT16:
    case NX_UINT16:
      return sizeof(short);
    case NX_INT32:
    case NX_UINT32:
      return sizeof(long);
    case NX_INT64:
    case NX_UINT64:
      return sizeof(int64);
    case NX_FLOAT32:
      return sizeof(float);
    case NX_FLOAT64:
      return sizeof(double);
    default: // CHAR, NX_INT8, NX_UINT8
      return 1;
  }
}

//=============================================================================
// NeXus Data set
//
//=============================================================================

//---------------------------------------------------------------------------
// Constructors
//---------------------------------------------------------------------------
NexusDataSet::NexusDataSet()
{
  m_pData = NULL;
  m_piStart = new int[MAX_DIM];
}

//---------------------------------------------------------------------------
// Destructors
//---------------------------------------------------------------------------
NexusDataSet::~NexusDataSet()
{
  Clear();
  delete [] m_piStart;
}

//---------------------------------------------------------------------------
// NexusDataSet::FreeData
//---------------------------------------------------------------------------
void NexusDataSet::FreeData()
{
  if( m_pData )
  {
    switch( m_eDataType )
    {
      case NX_INT16:
      case NX_UINT16:
        delete [] (short *)m_pData;
        break;
      case NX_INT32:
      case NX_UINT32:
      case NX_FLOAT32:
        delete [] (long *)m_pData;
        break;
      case NX_INT64:
      case NX_UINT64:
        delete [] (int64 *)m_pData;
        break;
      case NX_FLOAT64:
        delete [] (double *)m_pData;
        break;
      default:  // CHAR, NX_INT8, NX_UINT8
        delete [] (char *)m_pData;
    }
  }

  m_pData = NULL;
}

//---------------------------------------------------------------------------
// NexusDataSet::Clear
//---------------------------------------------------------------------------
void NexusDataSet::Clear()
{
  FreeData();
  NexusDataSetInfo::Clear();
}

//---------------------------------------------------------------------------
// NexusDataSet::Size
//---------------------------------------------------------------------------
unsigned int NexusDataSet::Size() const
{
  unsigned int uiSize=1;
  int iRank;
  for( iRank = 0; iRank < m_iRank; iRank++ )
    uiSize *= m_piDim[iRank];
  return uiSize;
}

//---------------------------------------------------------------------------
// NexusDataSet::MemSize
//---------------------------------------------------------------------------
unsigned int NexusDataSet::MemSize() const
{
  return Size() * DatumSize();
}

//---------------------------------------------------------------------------
// NexusDataSet::Alloc
//---------------------------------------------------------------------------
void NexusDataSet::Alloc()
{
  switch( m_eDataType )
  {
    case NX_INT16:
    case NX_UINT16:
      m_pData = new short[Size()];
      break;
    case NX_INT32:
    case NX_UINT32:
    case NX_FLOAT32:
      m_pData = new long[Size()];
      break;
    case NX_INT64:
    case NX_UINT64:
      m_pData = new int64[Size()];
      break;
    case NX_FLOAT64:
      m_pData = new double[Size()];
      break;
    default:  // CHAR, NX_INT8, NX_UINT8
      m_pData = new char[Size()];
  }
}

//---------------------------------------------------------------------------
// NexusDataSet::Alloc
//---------------------------------------------------------------------------
void NexusDataSet::SetDimension(int iDim, int iSize)
{
  if( iDim < MAX_DIM )
    m_piDim[iDim] = iSize;
}

//---------------------------------------------------------------------------
// NexusDataSet::SetData
//---------------------------------------------------------------------------
void NexusDataSet::SetData(const void *pData, NexusDataType eDataType, int iRank, int *piDimArray)
{
  // Clear data
  Clear();

  // Store properties
  m_iRank = iRank;
  m_eDataType = eDataType;
  for( iRank = 0; iRank < m_iRank; iRank++ )
    m_piDim[iRank] = piDimArray[iRank];

  // Allocate memory for data block
  Alloc();

  memcpy(m_pData, pData, MemSize());
}

//=============================================================================
// NeXus File Class internal implementation
//
// Do the job
//=============================================================================
class NexusFileImpl
{
private:
  void             *m_hFile;
  string            m_strFullPath;
  string            m_strOpenDataSet;  // name of the currently opened data set
  stack<string>     m_stkstrOpenGroup; // stack of opened groups names
  bool              m_bNeedFlush;      // if true flushing data is needed before closing
  
  // Method called by CreateFile, OpenRead, OpenReadWrite
  int PrivOpenFile(const char *pcszFullPath, int iAccessMode);

  // For internal purpose
  void SetNeedFlush(bool bNeed=true) { m_bNeedFlush = bNeed; }

  // Check group name
  // Look for forbidden char and replace them by '_'
  String GetGroupName(const char *pcszName);

public:
  // Constructor
  NexusFileImpl(const char *pcszFullPath=NULL);

  // Destructor
  ~NexusFileImpl();

  // Creates file
  void Create(const char *pcszFullPath, ENexusCreateMode eMode);

  // Opens an existing file for read operations
  void OpenRead(const char *pcszFullPath);

  // Opens an existing file for read/write operations
  void OpenReadWrite(const char *pcszFullPath);

  // Closes currently opened file
  void Close();

  // Flushes all data to the file
  void Flush();

  // Adds a new group
  void CreateGroup(const char *pcszName, const char *pcszClass, bool bOpen=true);

  // Opens a existing group
  bool OpenGroup(const char *pcszName, const char *pcszClass, bool bThrowException=true);

  // CurrentGroup
  string CurrentGroup();

  // Closes current group
  void CloseGroup();

  // Closes all opened groups
  void CloseAllGroups();

  // Creates data set
  void CreateDataSet(const char *pcszName, NexusDataType eDataType, 
                     int iRank, int *piDim, int bOpen);

  // Creates compressed data set
  void CreateCompressedDataSet(const char *pcszName, NexusDataType eDataType, 
                               int iRank, int *piDim, int *piChunkDim, int bOpen);

  // Closes currenly open dataset
  void CloseDataSet();

  // Quickly creates simple data set and writes the data
  void WriteData(const char *pcszName, void *pData, NexusDataType eDataType, int iRank,
                 int *piDim, bool bCreate=true);

  // Quickly creates simple data set and writes part of the data
  void WriteDataSubSet(const char *pcszName, void *pData, NexusDataType eDataType, 
                       int iRank, int *piStart, int *piDim, bool bCreate=true, bool bNoDim = false);

  // Opens a already existing data set
  bool OpenDataSet(const char *pcszName, bool bThrowException=true);

  // Puts data in an existing data set
  void PutData(void *pData, const char *pcszName, int bFlush);

  // Puts data subset in an existing data set
  void PutDataSubSet(void *pData, int *piStart, int *piSize, const char *pcszName);

  // Put Data set attribut
  void PutAttr(const char *pcszName, void *pValue, int iLen, int iDataType);

  // Read data values from a data set in currently open group
  void GetData(NexusDataSet *DataSet, const char *pcszDataSetName);

  // Reads data values from a data set in currently open group
  void GetDataSubSet(NexusDataSet *pDataSet, const char *pcszDataSetName);

  // Gets info about a data set
  void GetDataSetInfo(NexusDataSetInfo *pDataSetInfo, const char *pcszDataSetName);

  // Read attribute
  void GetAttribute(const char *pcszAttr, int *pBufLen, void *pData, int iDataType);
  void GetAttribute(const char *pcszAttr, int *pBufLen, void *pData, int *piDataType);

  // Get info about the first item (data set or group) in the current group
  int GetFirstItem(NexusItemInfo *pItemInfo);
  
  // Get info about the next item (data set or group) in the current group
  int GetNextItem(NexusItemInfo *pItemInfo);
  
  // Get info about the first attribute of the specified data set
  int GetFirstAttribute(NexusAttrInfo *pAttrInfo, const char *pcszDataSet=NULL);

  // Get info about the next attribute of the specified data set
  int GetNextAttribute(NexusAttrInfo *pAttrInfo);

  // Get a handle on the currently open data set in order to link it with a group
  void GetDataSetLink(NexusItemID *pnxl);

  // Get a handle on the currently open group set in order to link it with a group
  void GetGroupLink(NexusItemID *pnxl);

  // Link a item to the currently open group
  void LinkToCurrentGroup(const NexusItemID &nxl);

  // Get the number of items in the current group
  int ItemCount();

  // Get the number of items in the current group
  int AttrCount();
};

//=============================================================================
//
// NeXus File Class
//
//=============================================================================

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
NexusFileImpl::NexusFileImpl(const char *pszFullPath)
{
  m_hFile = NULL;
  if( NULL != pszFullPath )
    m_strFullPath = pszFullPath;
  m_strOpenDataSet = g_strNoDataSet;
  m_stkstrOpenGroup.push(ROOT_LEVEL);
  m_bNeedFlush = false;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
NexusFileImpl::~NexusFileImpl()
{
  Close();
}

//*****************************************************************************
// Private methods
//*****************************************************************************

//---------------------------------------------------------------------------
// NexusFileImpl::PrivOpenFile
//---------------------------------------------------------------------------
int NexusFileImpl::PrivOpenFile(const char *pcszFullPath, int iAccessMode)
{
  // Close file, if open
  Close();

  if( NULL == pcszFullPath && m_strFullPath == g_strEmpty )
    throw NexusException(OPENING_FILE_ERR, MISSING_FILE_NAME, "NexusFileImpl::PrivOpenFile");
  else if( pcszFullPath && false == IsEqualsNoCase(m_strFullPath, String(pcszFullPath)) )
    m_strFullPath = pcszFullPath;

  LogVerbose("nex", "Opening file '%s' whith access mode %d", PSZ(m_strFullPath), iAccessMode);

  return NXopen(PCSZToPSZ(m_strFullPath.c_str()), (NXaccess)iAccessMode, &m_hFile);
}

//---------------------------------------------------------------------------
// NexusFileImpl::CheckGroupName
//---------------------------------------------------------------------------
String NexusFileImpl::GetGroupName(const char *pcszName)
{
  if( !pcszName )
    throw NexusException(PSZ(StrFormat(BAD_GROUP_NAME_ERR)), OPERATION_FAILED, "NexusFileImpl::GetGroupName");

  String strGroupName(pcszName);
  strGroupName.Replace('/', '_');
  strGroupName.Replace('\\', '_');
  strGroupName.Replace(' ', '_');
  strGroupName.Replace('*', '_');
  strGroupName.Replace('"', '_');
  strGroupName.Replace('\'', '_');
  strGroupName.Replace(',', '_');
  strGroupName.Replace(';', '_');
  strGroupName.Replace(':', '_');
  strGroupName.Replace('!', '_');
  strGroupName.Replace('?', '_');

  if( !String(pcszName).IsEquals(strGroupName) )
    LogWarning("nex", "Changed group name from '%s' to '%s'", pcszName, PSZ(strGroupName));

  return strGroupName;
}

//*****************************************************************************
// Publics methods
//*****************************************************************************

//---------------------------------------------------------------------------
// NexusFileImpl::Close
//---------------------------------------------------------------------------
void NexusFileImpl::Close()
{
  if( NULL != m_hFile )
  {
    // Gracefully close opened groups
    CloseAllGroups();
    // Flushing data
    Flush();

    LogVerbose("nex", "Close file '%s'", PSZ(m_strFullPath));

    // Then close the file
    NXclose(&m_hFile);
    m_hFile = NULL;
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::Flush
//---------------------------------------------------------------------------
void NexusFileImpl::Flush()
{
  if( NULL != m_hFile && m_bNeedFlush)
  {
    if( NX_OK != NXflush(&m_hFile) )
    {
      // No needs to retry since it will fail again then call Close()
      m_bNeedFlush = false;
      throw NexusException(PSZ(StrFormat(FLUSH_ERR, PSZ(m_strFullPath))),
                           OPERATION_FAILED, "NexusFileImpl::Flush");
    }
  }

  LogVerbose("nex", "Flush data");

  // Flushing data closes the currently open data set
  m_strOpenDataSet = g_strNoDataSet;
}

//---------------------------------------------------------------------------
// NexusFileImpl::Create
//---------------------------------------------------------------------------
void NexusFileImpl::Create(const char *pcszFullPath, ENexusCreateMode eMode)
{
  int iMode = 0;
  switch( eMode )
  {
    case NX_HDF4:
      iMode = NXACC_CREATE4; break;
    case NX_HDF5:
    case NX_XML: // Not implemented yet
      iMode = NXACC_CREATE5; break;
  }
  int iRc = PrivOpenFile(pcszFullPath, iMode);
  if( iRc != NX_OK )
    throw NexusException(CREATE_FILE_ERR, 
                         PSZ(StrFormat(UNABLE_TO_OPEN_FILE, PSZ(m_strFullPath))),
                         "NexusFileImpl::Create");
}

//---------------------------------------------------------------------------
// NexusFileImpl::OpenRead
//---------------------------------------------------------------------------
void NexusFileImpl::OpenRead(const char *pcszFullPath)
{
  int iRc = PrivOpenFile(pcszFullPath, NXACC_READ);
  if( iRc != NX_OK )
    throw NexusException(OPENING_FILE_READ_ERR, 
                         PSZ(StrFormat(UNABLE_TO_OPEN_FILE, PSZ(m_strFullPath))),
                         "NexusFileImpl::OpenRead");
}

//---------------------------------------------------------------------------
// NexusFileImpl::OpenReadWrite
//---------------------------------------------------------------------------
void NexusFileImpl::OpenReadWrite(const char *pcszFullPath)
{
  int iRc = PrivOpenFile(pcszFullPath, NXACC_RDWR);
  if( iRc != NX_OK )
    throw NexusException(OPENING_FILE_RW_ERR, 
                         PSZ(StrFormat(UNABLE_TO_OPEN_FILE, PSZ(m_strFullPath))),
                         "NexusFileImpl::OpenReadWrite");
}

//---------------------------------------------------------------------------
// NexusFileImpl::CreateGroup
//---------------------------------------------------------------------------
void NexusFileImpl::CreateGroup(const char *pcszName, const char *pcszClass, bool bOpen)
{
  CloseDataSet();

  if( NULL == m_hFile )
    throw NexusException(MAKE_GROUP_ERR, NO_HANDLE, "NexusFileImpl::CreateGroup");
  
  if( !pcszName )
    throw NexusException(PSZ(StrFormat(MAKE_GROUP_ERR, "<noname>", PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::CreateGroup");

  String strGroupName = GetGroupName(pcszName);
  if( NXmakegroup(m_hFile, PCSZToPSZ(PSZ(strGroupName)), PCSZToPSZ(pcszClass)) != NX_OK )
    throw NexusException(PSZ(StrFormat(MAKE_GROUP_ERR, pcszName, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::CreateGroup");
  
  if( bOpen )
    OpenGroup(PSZ(strGroupName), pcszClass);
}

//---------------------------------------------------------------------------
// NexusFileImpl::OpenGroup
//---------------------------------------------------------------------------
bool NexusFileImpl::OpenGroup(const char *pcszName, const char *pcszClass, bool bThrowException)
{
  if( NULL == m_hFile )
    throw NexusException(PSZ(StrFormat(OPEN_GROUP_ERR, pcszName, PSZ(m_strFullPath))),
                         NO_HANDLE, "NexusFileImpl::OpenGroup");


  CString strGroup;
  strGroup.Printf("%s(%s)", pcszName, pcszClass);
  LogVerbose("nex", "Opening group '%s'", PSZ(strGroup));

  // Trying to open group without filtering the requested name
  String strGroupName = pcszName;
  int iRc = NXopengroup(m_hFile, PCSZToPSZ(PSZ(strGroupName)), PCSZToPSZ(pcszClass));
  if( iRc != NX_OK )
  {
    // Trying 
    strGroupName = GetGroupName(pcszName);
    strGroup.Printf("%s(%s)", PSZ(strGroupName), pcszClass);
    iRc = NXopengroup(m_hFile, PCSZToPSZ(PSZ(strGroupName)), PCSZToPSZ(pcszClass));
  }
  if( iRc != NX_OK )
  {
    if( bThrowException )
      throw NexusException(PSZ(StrFormat(OPEN_GROUP_ERR, PSZ(strGroupName), PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::OpenGroup");
    else
      return false;
  }

  // Empiler le nom du groupe
  m_stkstrOpenGroup.push(strGroup);
  return true;
}

//---------------------------------------------------------------------------
// NexusFileImpl::CurrentGroup
//---------------------------------------------------------------------------
string NexusFileImpl::CurrentGroup()
{
  return m_stkstrOpenGroup.top();
}

//---------------------------------------------------------------------------
// NexusFileImpl::CloseGroup
//---------------------------------------------------------------------------
void NexusFileImpl::CloseGroup()
{
  CloseDataSet();

  if( m_stkstrOpenGroup.size() > 1 )
  {
    LogVerbose("nex", "Close group");

    if( NXclosegroup(m_hFile) != NX_OK )
      throw NexusException(PSZ(StrFormat(CLOSE_GROUP_ERR, PSZ(m_stkstrOpenGroup.top()),
                                                       PSZ(m_strFullPath))),
                           OPERATION_FAILED, "NexusFileImpl::CloseGroup");
    m_stkstrOpenGroup.pop();
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::CloseAllGroups
//---------------------------------------------------------------------------
void NexusFileImpl::CloseAllGroups()
{
  while( m_stkstrOpenGroup.size() > 1 )
    CloseGroup();
}

//---------------------------------------------------------------------------
// NexusFileImpl::CloseDataSet
//---------------------------------------------------------------------------
bool NexusFileImpl::OpenDataSet(const char *pcszName, bool bThrowException)
{
  if( m_strOpenDataSet != pcszName )
    CloseDataSet();

  LogVerbose("nex", "Open data set '%s'", pcszName);

  if( NXopendata(m_hFile, PCSZToPSZ(pcszName)) != NX_OK )
  {
    if( bThrowException )
      throw NexusException(PSZ(StrFormat(OPEN_DATA_ERR, pcszName, 
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::OpenDataSet");
    else
      return false;
  }
  m_strOpenDataSet = pcszName;
  return true;
}

//---------------------------------------------------------------------------
// NexusFileImpl::CloseDataSet
//---------------------------------------------------------------------------
void NexusFileImpl::CloseDataSet()
{
  if( m_strOpenDataSet != g_strNoDataSet )
  {
    LogVerbose("nex", "Close data set '%s'", PSZ(m_strOpenDataSet));

    if( NXclosedata(m_hFile) != NX_OK )
      throw NexusException(PSZ(StrFormat(CLOSE_DATA_ERR, PSZ(m_stkstrOpenGroup.top()),
                                                      PSZ(m_strFullPath))),
                           OPERATION_FAILED, "NexusFileImpl::CloseDataSet");

    m_strOpenDataSet = g_strNoDataSet;
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::CreateDataSet
//---------------------------------------------------------------------------
void NexusFileImpl::CreateCompressedDataSet(const char *pcszName, NexusDataType eDataType, 
                                        int iRank, int *piDim, int *piChunkDim, int bOpen)
{
  if( m_strOpenDataSet != pcszName )
    CloseDataSet();

  int aiChunkSize[MAX_DIM];
  if( piChunkDim )
  {
    for( int i = 0; i < iRank; i++ )
      aiChunkSize[i] = piChunkDim[i];
  }
  else
  {
    for( int i = 0; i < iRank; i++ )
      aiChunkSize[i] = piDim[i];
  }

  LogVerbose("nex", "create compressed data set '%s'", pcszName);

  // Create dataset
  if( NXcompmakedata(m_hFile, PCSZToPSZ(pcszName), (int)eDataType, iRank, 
                     piDim, NX_COMP_LZW, aiChunkSize) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(CREATE_DATA_ERR, pcszName,
                                                     PSZ(m_stkstrOpenGroup.top()),
                                                     PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::CreateCompressedDataSet");
  }

  if( bOpen )
    OpenDataSet(pcszName);
}

//---------------------------------------------------------------------------
// NexusFileImpl::CreateDataSet
//---------------------------------------------------------------------------
void NexusFileImpl::CreateDataSet(const char *pcszName, NexusDataType eDataType, 
                                  int iRank, int *piDim, int bOpen)
{
  if( m_strOpenDataSet != pcszName )
    CloseDataSet();
  
  LogVerbose("nex", "Create data set '%s'", pcszName);

  int iRc = 0;
  if( piDim )
    iRc = NXmakedata(m_hFile, PCSZToPSZ(pcszName), (int)eDataType, iRank, piDim);
  else
  {
    int iSize = NX_UNLIMITED;
    iRc = NXmakedata(m_hFile, PCSZToPSZ(pcszName), (int)eDataType, iRank, &iSize);
  }
  // Create dataset
  if( iRc != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(CREATE_DATA_ERR, pcszName,
                                                     PSZ(m_stkstrOpenGroup.top()),
                                                     PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::CreateDataSet");
  }

  if( bOpen )
    OpenDataSet(pcszName);
}

//---------------------------------------------------------------------------
// NexusFileImpl::PutData
//---------------------------------------------------------------------------
void NexusFileImpl::PutData(void *pData, const char *pcszName, int bFlush)
{
  if( NULL != pcszName && m_strOpenDataSet != pcszName )
  {
    // The DataSet 'pcszName' isn't open
    CloseDataSet();
    // Try to openit
    OpenDataSet(pcszName);
  }

  if( NXputdata(m_hFile, pData) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(PUT_DATA_ERR, PSZ(m_strOpenDataSet),
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::PutData");
  }

  if( bFlush )
    Flush();
// Not useful since flushing data is performed when calling NXclose()
//  else
//    SetNeedFlush();
}

//---------------------------------------------------------------------------
// NexusFileImpl::PutDataSubSet
//---------------------------------------------------------------------------
void NexusFileImpl::PutDataSubSet(void *pData, int *piStart, int *piSize,
                                  const char *pcszName)
{
  if( NULL != pcszName && m_strOpenDataSet != pcszName )
  {
    // The DataSet 'pcszName' isn't open
    CloseDataSet();
    // Try to openit
    OpenDataSet(pcszName);
  }

  if( NXputslab(m_hFile, pData, piStart, piSize) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(PUT_DATASUBSET_ERR, PSZ(m_strOpenDataSet),
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::PutDataSubSet");
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::WriteData
//---------------------------------------------------------------------------
void NexusFileImpl::WriteData(const char *pcszName, void *pData, 
                              NexusDataType eDataType, int iRank, int *piDim, bool bCreate)
{
  if( m_strOpenDataSet != pcszName )
    CloseDataSet();
  
  if( bCreate )
  {
    // Create dataset
    CreateDataSet(pcszName, eDataType, iRank, piDim, true);
  }
  // Put data
  PutData(pData, pcszName, false);
}

//---------------------------------------------------------------------------
// NexusFileImpl::WriteDataSubSet
//---------------------------------------------------------------------------
void NexusFileImpl::WriteDataSubSet(const char *pcszName, void *pData, NexusDataType eDataType, 
                                    int iRank, int *piStart, int *piSize, bool bCreate, bool bNoDim)
{
  if( m_strOpenDataSet != pcszName )
    CloseDataSet();
  
  if( bCreate )
  {
    // Create dataset
    if( !bNoDim )
      CreateDataSet(pcszName, eDataType, iRank, piSize, true);
    else
    {
      // Create unlimited size dataset
      int iSize = NX_UNLIMITED;
      CreateDataSet(pcszName, eDataType, iRank, &iSize, true);
    }
  }
  // Put data
  PutDataSubSet(pData, piStart, piSize, pcszName);
}

//---------------------------------------------------------------------------
// NexusFileImpl::PutAttr
//---------------------------------------------------------------------------
void NexusFileImpl::PutAttr(const char *pcszName, void *pValue, int iLen, int iDataType)
{
  if( NXputattr(m_hFile, PCSZToPSZ(pcszName), pValue, iLen, iDataType) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(PUT_DATA_ERR, pcszName,
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::PutAttr");
  }

  // Not useful since flushing data is performed when calling NXclose()
  // SetNeedFlush();
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetDataSetInfo
//---------------------------------------------------------------------------
void NexusFileImpl::GetDataSetInfo(NexusDataSetInfo *pDataSetInfo, 
                                   const char *pcszDataSet)
{
  if( m_strOpenDataSet != pcszDataSet )
  {
    // The DataSet 'pcszName' isn't open
    CloseDataSet();
    // Try to open correct data set
    OpenDataSet(pcszDataSet);
    // Clear the NexusDataSetInfo object
    pDataSetInfo->Clear();
  }

  if( pDataSetInfo->IsEmpty() )
  {
    // Read Data set info
    int iRank, iDataType;
    if( NX_OK != NXgetinfo(m_hFile, &iRank, pDataSetInfo->DimArray(), &iDataType) )
    {
      throw NexusException(PSZ(StrFormat(GETINFO_ERR, pcszDataSet,
                                                   PSZ(m_stkstrOpenGroup.top()),
                                                   PSZ(m_strFullPath))),
                           OPERATION_FAILED, "NexusFileImpl::GetDataSetInfo");
    }

    pDataSetInfo->SetInfo((NexusDataType)iDataType, iRank);
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetData
//---------------------------------------------------------------------------
void NexusFileImpl::GetData(NexusDataSet *pDataSet, const char *pcszDataSet)
{
  if( NULL != pcszDataSet )
    // First, get info about the data set
    GetDataSetInfo(pDataSet, pcszDataSet);

  // Allocate dataset
  pDataSet->Alloc();

  // Read data
  if( NX_OK != NXgetdata(m_hFile, pDataSet->Data()) )
  {
    throw NexusException(PSZ(StrFormat(GETDATA_ERR, pcszDataSet,
                                                 PSZ(m_stkstrOpenGroup.top()),
                                                 PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetData");
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetDataSubSet
//---------------------------------------------------------------------------
void NexusFileImpl::GetDataSubSet(NexusDataSet *pDataSet, const char *pcszDataSet)
{
  // Allocate dataset
  pDataSet->Alloc();

  // Read data
  if( NX_OK != NXgetslab(m_hFile, pDataSet->Data(), pDataSet->StartArray(),
                                                     pDataSet->DimArray()) )
  {
    throw NexusException(PSZ(StrFormat(GETDATA_ERR, pcszDataSet,
                                                 PSZ(m_stkstrOpenGroup.top()),
                                                 PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetDataSubSet");
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetAttribute
//---------------------------------------------------------------------------
void NexusFileImpl::GetAttribute(const char *pcszAttr, int *piBufLen, 
                                 void *pData, int iDataType)
{
  int _iDataType = iDataType;

  // Read attribute
  if( NXgetattr(m_hFile, PCSZToPSZ(pcszAttr), pData, piBufLen, &_iDataType) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(GET_ATTR_ERR, pcszAttr,
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetAttribute");
  }

  // Check type, except if NX_NONE is the passed type
  if( NX_NONE != iDataType && _iDataType != iDataType )
  {
    throw NexusException(PSZ(StrFormat(GET_ATTR_ERR, pcszAttr,
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         TYPES_MISMATCH, "NexusFileImpl::GetAttribute");

  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetAttribute
//---------------------------------------------------------------------------
void NexusFileImpl::GetAttribute(const char *pcszAttr, int *piBufLen, 
                                 void *pData, int *piDataType)
{
  // Read attribute
  if( NXgetattr(m_hFile, PCSZToPSZ(pcszAttr), pData, piBufLen, piDataType) != NX_OK )
  {
    throw NexusException(PSZ(StrFormat(GET_ATTR_ERR, pcszAttr,
                                                  PSZ(m_stkstrOpenGroup.top()),
                                                  PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetAttribute");
  }
}

//---------------------------------------------------------------------------
// NexusFileImpl::ItemCount
//---------------------------------------------------------------------------
int NexusFileImpl::ItemCount()
{
  int iCount;
  if( NXgetgroupinfo(m_hFile, &iCount, g_acScratchBuf, g_acScratchBuf) != NX_OK )
    throw NexusException(PSZ(StrFormat(GET_GROUPINFO_ERR, PSZ(m_stkstrOpenGroup.top()),
                                                       PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::ItemCount");
  return iCount;
}

//---------------------------------------------------------------------------
// NexusFile::AttrCount
//---------------------------------------------------------------------------
int NexusFileImpl::AttrCount()
{
  int iCount;
  if( NXgetattrinfo(m_hFile, &iCount) != NX_OK )
    throw NexusException(PSZ(StrFormat(GET_ATTRINFO_ERR, PSZ(m_stkstrOpenGroup.top()),
                                                       PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::AttrCount");
  return iCount;
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetFirstItem
//---------------------------------------------------------------------------
int NexusFileImpl::GetFirstItem(NexusItemInfo *pItemInfo)
{
  int iStatus = NX_OK;

  // Initiate enumeration
  iStatus = NXinitgroupdir(m_hFile);
  if( NX_ERROR == iStatus )
  {
    throw NexusException(PSZ(StrFormat(GET_INIT_ENUM_ITEM, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetFirstItem");
  }

  // Get first item
  return GetNextItem(pItemInfo);
}
  
//---------------------------------------------------------------------------
// NexusFileImpl::GetNextItem
//---------------------------------------------------------------------------
int NexusFileImpl::GetNextItem(NexusItemInfo *pItemInfo)
{
  int iStatus = NXgetnextentry(m_hFile, pItemInfo->m_pszItem, pItemInfo->m_pszClass, 
                           (int *)&(pItemInfo->m_eDataType));
  if( NX_ERROR == iStatus )
  {
    throw NexusException(PSZ(StrFormat(GET_RETREIVE_ITEM, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetNextItem");
  }

  return iStatus;
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetFirstAttribute
//---------------------------------------------------------------------------
int NexusFileImpl::GetFirstAttribute(NexusAttrInfo *pAttrInfo, const char *pcszDataSet)
{
  int iStatus = NX_OK;

  if( pcszDataSet )
    OpenDataSet(pcszDataSet);

  // Initiate enumeration
  iStatus = NXinitattrdir(m_hFile);
  if( NX_ERROR == iStatus )
  {
    throw NexusException(PSZ(StrFormat(GET_INIT_ENUM_ATTR, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetFirstAttribute");
  }

  // Get first attribute
  return GetNextAttribute(pAttrInfo);
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetNextAttribute
//---------------------------------------------------------------------------
int NexusFileImpl::GetNextAttribute(NexusAttrInfo *pAttrInfo)
{
  int iStatus = NXgetnextattr(m_hFile, pAttrInfo->m_pszName, &pAttrInfo->m_iLen,
                           (int *)&(pAttrInfo->m_eDataType));
  if( NX_ERROR == iStatus )
  {
    throw NexusException(PSZ(StrFormat(GET_RETREIVE_ATTR, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::GetNextAttribute");
  }

  return iStatus;
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetDataSetHandle
//---------------------------------------------------------------------------
void NexusFileImpl::GetDataSetLink(NexusItemID *pnxl)
{
  NXgetdataID(m_hFile, (NXlink *)(pnxl->m_pLink));
}

//---------------------------------------------------------------------------
// NexusFileImpl::GetGroupHandle
//---------------------------------------------------------------------------
void NexusFileImpl::GetGroupLink(NexusItemID *pnxl)
{
  NXgetgroupID(m_hFile, (NXlink *)(pnxl->m_pLink));
}

//---------------------------------------------------------------------------
// NexusFileImpl::LinkToCurrentGroup
//---------------------------------------------------------------------------
void NexusFileImpl::LinkToCurrentGroup(const NexusItemID &nxl)
{
  if( NX_OK != NXmakelink(m_hFile, (NXlink *)(nxl.m_pLink)) )
  {
    throw NexusException(PSZ(StrFormat(LINK_ITEM_ERR, PSZ(m_strFullPath))),
                         OPERATION_FAILED, "NexusFileImpl::LinkToCurrentGroup");
  }
}

//=============================================================================
//
// NeXus File Class - wrapper class
//
//=============================================================================

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
NexusFile::NexusFile(const char *pcszFullPath)
{
  m_pImpl = new NexusFileImpl(pcszFullPath);
  m_pUserPtr = NULL;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
NexusFile::~NexusFile()
{
  Close();
  delete m_pImpl;
}

//---------------------------------------------------------------------------
// NexusFile::Close
//---------------------------------------------------------------------------
void NexusFile::Close()
{
  m_pImpl->Close();
}

//---------------------------------------------------------------------------
// NexusFile::Flush
//---------------------------------------------------------------------------
void NexusFile::Flush()
{
  m_pImpl->Flush();
}

//---------------------------------------------------------------------------
// NexusFile::Create
//---------------------------------------------------------------------------
void NexusFile::Create(const char *pcszFullPath, ENexusCreateMode eMode)
{
  m_pImpl->Create(pcszFullPath, eMode);
}

//---------------------------------------------------------------------------
// NexusFile::OpenRead
//---------------------------------------------------------------------------
void NexusFile::OpenRead(const char *pcszFullPath)
{
  m_pImpl->OpenRead(pcszFullPath);
}

//---------------------------------------------------------------------------
// NexusFile::OpenReadWrite
//---------------------------------------------------------------------------
void NexusFile::OpenReadWrite(const char *pcszFullPath)
{
  m_pImpl->OpenReadWrite(pcszFullPath);
}

//---------------------------------------------------------------------------
// NexusFile::CreateGroup
//---------------------------------------------------------------------------
void NexusFile::CreateGroup(const char *pcszName, const char *pcszClass, bool bOpen)
{
  m_pImpl->CreateGroup(pcszName, pcszClass, bOpen);
}

//---------------------------------------------------------------------------
// NexusFile::OpenGroup
//---------------------------------------------------------------------------
bool NexusFile::OpenGroup(const char *pcszName, const char *pcszClass, bool bThrowException)
{
  return m_pImpl->OpenGroup(pcszName, pcszClass, bThrowException);
}

//---------------------------------------------------------------------------
// NexusFile::CloseGroup
//---------------------------------------------------------------------------
void NexusFile::CloseGroup()
{
  m_pImpl->CloseGroup();
}

//---------------------------------------------------------------------------
// NexusFile::CloseAllGroups
//---------------------------------------------------------------------------
void NexusFile::CloseAllGroups()
{
  m_pImpl->CloseAllGroups();
}

//---------------------------------------------------------------------------
// NexusFile::CloseDataSet
//---------------------------------------------------------------------------
bool NexusFile::OpenDataSet(const char *pcszName, bool bThrowException)
{
  return m_pImpl->OpenDataSet(pcszName, bThrowException);
}

//---------------------------------------------------------------------------
// NexusFile::CloseDataSet
//---------------------------------------------------------------------------
void NexusFile::CloseDataSet()
{
  m_pImpl->CloseDataSet();
}

//---------------------------------------------------------------------------
// NexusFile::CreateCompressedDataSet
//---------------------------------------------------------------------------
void NexusFile::CreateCompressedDataSet(const char *pcszName, NexusDataType eDataType, 
                                        int iRank, int *piDim, int *piChunkDim, int bOpen)
{
  m_pImpl->CreateCompressedDataSet(pcszName, eDataType, iRank, piDim, piChunkDim, bOpen);
}

//---------------------------------------------------------------------------
// NexusFile::CreateDataSet
//---------------------------------------------------------------------------
void NexusFile::CreateDataSet(const char *pcszName, NexusDataType eDataType, 
                              int iRank, int *piDim, int bOpen)
{
  m_pImpl->CreateDataSet(pcszName, eDataType, iRank, piDim, bOpen);
}

//---------------------------------------------------------------------------
// NexusFile::PutData
//---------------------------------------------------------------------------
void NexusFile::PutData(void *pData, const char *pcszName, int bFlush)
{
  m_pImpl->PutData(pData, pcszName, bFlush);
}

//---------------------------------------------------------------------------
// NexusFile::PutDataSubSet
//---------------------------------------------------------------------------
void NexusFile::PutDataSubSet(void *pData, int *piStart, int *piSize,
                              const char *pcszName)
{
  m_pImpl->PutDataSubSet(pData, piStart, piSize, pcszName);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, void *pData, 
                          NexusDataType eDataType, int iRank, int *piDim,
                          bool bCreate)
{
  m_pImpl->WriteData(pcszName, pData, eDataType, iRank, piDim, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::WriteDataSubSet
//---------------------------------------------------------------------------
void NexusFile::WriteDataSubSet(const char *pcszName, void *pData, NexusDataType eDataType, 
                       int iRank, int *piStart, int *piSize, bool bCreate, bool bNoDim)
{
  m_pImpl->WriteDataSubSet(pcszName, pData, eDataType, iRank, piStart, piSize, bCreate, bNoDim);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData | float version
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, float fValue, bool bCreate)
{
  int iLen = 1;
  m_pImpl->WriteData(pcszName, &fValue, NX_FLOAT32, 1, &iLen, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData | double version
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, double dValue, bool bCreate)
{
  int iLen = 1;
  m_pImpl->WriteData(pcszName, &dValue, NX_FLOAT64, 1, &iLen, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData | long version
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, long lValue, bool bCreate)
{
  int iLen = 1;
  m_pImpl->WriteData(pcszName, &lValue, NX_INT32, 1, &iLen, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData | string version
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, const char *pcszValue, bool bCreate)
{
  int iLen = strlen(pcszValue);
  m_pImpl->WriteData(pcszName, PCSZToPSZ(pcszValue), NX_CHAR, 1, &iLen, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::WriteData | binary version
//---------------------------------------------------------------------------
void NexusFile::WriteData(const char *pcszName, void *pData, int _iLen, bool bCreate)
{
  int iLen = _iLen;
  m_pImpl->WriteData(pcszName, pData, NX_BINARY, 1, &iLen, bCreate);
}

//---------------------------------------------------------------------------
// NexusFile::PutAttr
//---------------------------------------------------------------------------
void NexusFile::PutAttr(const char *pcszName, void *pValue, int iLen, 
                        NexusDataType eDataType)
{
  m_pImpl->PutAttr(pcszName, pValue, iLen, (int)eDataType);
}

//---------------------------------------------------------------------------
// NexusFile::PutAttr | 'long' version
//---------------------------------------------------------------------------
void NexusFile::PutAttr(const char *pcszName, long lValue)
{
  m_pImpl->PutAttr(pcszName, &lValue, 1, NX_INT32);
}

//---------------------------------------------------------------------------
// NexusFile::PutAttr | 'pcsz' version
//---------------------------------------------------------------------------
void NexusFile::PutAttr(const char *pcszName, const char *pcszValue)
{
  m_pImpl->PutAttr(pcszName, (void *)pcszValue, strlen(pcszValue), NX_CHAR);
}

//---------------------------------------------------------------------------
// NexusFile::PutAttr | 'double' version
//---------------------------------------------------------------------------
void NexusFile::PutAttr(const char *pcszName, double dValue)
{
  m_pImpl->PutAttr(pcszName, &dValue, 1, NX_FLOAT64);
}

//---------------------------------------------------------------------------
// NexusFile::PutAttr | 'float' version
//---------------------------------------------------------------------------
void NexusFile::PutAttr(const char *pcszName, float fValue)
{
  m_pImpl->PutAttr(pcszName, &fValue, 1, NX_FLOAT32);
}

//---------------------------------------------------------------------------
// NexusFile::GetData
//---------------------------------------------------------------------------
void NexusFile::GetData(NexusDataSet *pDataSet, const char *pcszDataSet)
{
  m_pImpl->GetData(pDataSet, pcszDataSet);
}

//---------------------------------------------------------------------------
// NexusFile::GetDataSubSet
//---------------------------------------------------------------------------
void NexusFile::GetDataSubSet(NexusDataSet *pDataSet, const char *pcszDataSet)
{
  m_pImpl->GetDataSubSet(pDataSet, pcszDataSet);
}

//---------------------------------------------------------------------------
// NexusFile::GetDataSetInfo
//---------------------------------------------------------------------------
void NexusFile::GetDataSetInfo(NexusDataSetInfo *pDataSetInfo, const char *pcszDataSet)
{
  m_pImpl->GetDataSetInfo(pDataSetInfo, pcszDataSet);
}

//---------------------------------------------------------------------------
// NexusFile::GetAttribute
//---------------------------------------------------------------------------
void NexusFile::GetAttribute(const char *pcszAttr, int *piBufLen, 
                             void *pData, NexusDataType eDataType)
{
  m_pImpl->GetAttribute(pcszAttr, piBufLen, pData, (int)eDataType);
}

//---------------------------------------------------------------------------
// NexusFile::GetAttribute 'long' version
//---------------------------------------------------------------------------
void NexusFile::GetAttribute(const char *pcszAttr, long *plValue)
{
  int iLen = 1;
  m_pImpl->GetAttribute(pcszAttr, &iLen, plValue, NX_INT32);
}

//---------------------------------------------------------------------------
// NexusFile::GetAttribute 'double' version
//---------------------------------------------------------------------------
void NexusFile::GetAttribute(const char *pcszAttr, double *pdValue)
{
  int iLen = 1;
  m_pImpl->GetAttribute(pcszAttr, &iLen, pdValue, NX_FLOAT64);
}

//---------------------------------------------------------------------------
// NexusFile::GetAttribute 'float' version
//---------------------------------------------------------------------------
void NexusFile::GetAttribute(const char *pcszAttr, float *pfValue)
{
  int iLen = 1;
  m_pImpl->GetAttribute(pcszAttr, &iLen, pfValue, NX_FLOAT32);
}

//---------------------------------------------------------------------------
// NexusFile::GetAttribute 'string' version
//---------------------------------------------------------------------------
void NexusFile::GetAttribute(const char *pcszAttr, string *pstrValue)
{
  char szBuf[512];
  int iLen = 512;
  m_pImpl->GetAttribute(pcszAttr, &iLen, szBuf, NX_CHAR);
  (*pstrValue) = szBuf;
}
void NexusFile::GetAttribute(const char *pcszAttr, char *pszValue, int iBufLen)
{
  int iLen = iBufLen;
  m_pImpl->GetAttribute(pcszAttr, &iLen, pszValue, NX_CHAR);
}

//---------------------------------------------------------------------------
// NexusFile::GetFirstItem
//---------------------------------------------------------------------------
int NexusFile::GetFirstItem(NexusItemInfo *pItemInfo)
{
  return m_pImpl->GetFirstItem(pItemInfo);
}
  
//---------------------------------------------------------------------------
// NexusFile::GetNextItem
//---------------------------------------------------------------------------
int NexusFile::GetNextItem(NexusItemInfo *pItemInfo)
{
  return m_pImpl->GetNextItem(pItemInfo);
}

//---------------------------------------------------------------------------
// NexusFile::GetFirstAttribute
//---------------------------------------------------------------------------
int NexusFile::GetFirstAttribute(NexusAttrInfo *pAttrInfo, const char *pcszDataSet)
{
  return m_pImpl->GetFirstAttribute(pAttrInfo, pcszDataSet);
}

//---------------------------------------------------------------------------
// NexusFile::GetNextAttribute
//---------------------------------------------------------------------------
int NexusFile::GetNextAttribute(NexusAttrInfo *pAttrInfo)
{
  return m_pImpl->GetNextAttribute(pAttrInfo);
}

//---------------------------------------------------------------------------
// NexusFile::GetDataSetLink
//---------------------------------------------------------------------------
void NexusFile::GetDataSetLink(NexusItemID *pnxl)
{
  m_pImpl->GetDataSetLink(pnxl);
}

//---------------------------------------------------------------------------
// NexusFile::GetGroupHandle
//---------------------------------------------------------------------------
void NexusFile::GetGroupLink(NexusItemID *pnxl)
{
  m_pImpl->GetGroupLink(pnxl);
}

//---------------------------------------------------------------------------
// NexusFile::LinkToCurrentGroup
//---------------------------------------------------------------------------
void NexusFile::LinkToCurrentGroup(const NexusItemID &nxl)
{
  m_pImpl->LinkToCurrentGroup(nxl);
}

//---------------------------------------------------------------------------
// NexusFile::ItemCount
//---------------------------------------------------------------------------
int NexusFile::ItemCount()
{
  return m_pImpl->ItemCount();
}

//---------------------------------------------------------------------------
// NexusFile::AttrCount
//---------------------------------------------------------------------------
int NexusFile::AttrCount()
{
  return m_pImpl->AttrCount();
}

//-----------------------------------------------------------------------------
// NexusFile::OpenGroupPath
//-----------------------------------------------------------------------------
bool NexusFile::OpenGroupPath(const char *pszPath, bool bThrowException)
{
  LogVerbose("nex", "Open group from path '%s'", pszPath);

  String strPath = pszPath;
  if( strPath.StartWith('/') )
  {
    // Sarting from root level => ensure that no group is already opened
    CloseAllGroups();
    strPath.erase(0, 1);
  }

  if( NULL == pszPath )
    // No path => nothing to do
    return false;

  while( strPath.size() > 0 )
  {
    String strGroup;
    String strClass;

    // we extract the name and the class's name of the Item.
    soleil::ExtractToken(&strPath, '/', &strGroup);
    soleil::ExtractTokenRight(&strGroup, '<', '>', &strClass);
    if( strClass.empty() )
      // 2nd chance !
      soleil::ExtractTokenRight(&strGroup, '(', ')', &strClass);
    if( strClass.empty() )
      // last chance !
      soleil::ExtractTokenRight(&strGroup, '{', '}', &strClass);

    if( strGroup.IsEquals("..") )
    {
      // Up one level
      CloseGroup();
    }

    else if( strGroup.IsEquals(".") )
    {
      // do nothing
    }

    // Provide groupe name and class
    else if( !strClass.empty() && !strGroup.empty() )
    {
      // Open the group
      bool bOpened = OpenGroup( PSZ(strGroup), PSZ(strClass), bThrowException);
      if( !bOpened )
        return false;
    }

    // Provide only class name
    else if( strGroup.empty() && !strClass.empty() )
    {     
      // Find first group of class {strClass}
      NexusItemInfo ItemInfo;
      int iRc = GetFirstItem(&ItemInfo);
      // we start our research.
      while( iRc != NX_EOD )
      {
        if( ItemInfo.IsGroup() && IsEquals(ItemInfo.ClassName(), strClass) )
        {
          strGroup = ItemInfo.ItemName();
          break;
        }
        iRc = GetNextItem(&ItemInfo);
      }
      if( !strGroup.empty() )
        // we open the group found.
        OpenGroup(PSZ(strGroup), PSZ(strClass), bThrowException);
      else
        return false;
    }

    // Provide only group name
    else if( strClass.empty() && !strGroup.empty() )
    {
      // Find first group of class {strClass}
      NexusItemInfo ItemInfo;
      int iRc = GetFirstItem(&ItemInfo);
      // we start our research.
      while( iRc != NX_EOD )
      {
        if( ItemInfo.IsGroup() && IsEquals(ItemInfo.ItemName(), strGroup) )
        {
          strClass = ItemInfo.ClassName();
          break;
        }
        iRc = GetNextItem(&ItemInfo);
      }
      if( !strClass.empty() )
        // we open the group found.
        OpenGroup(PSZ(strGroup), PSZ(strClass), bThrowException);
      else
        return false;
    }
  }
  
  return true;
}

//-----------------------------------------------------------------------------
// NexusFile::SearchGroup
//-----------------------------------------------------------------------------
int NexusFile::SearchGroup(const char *pszGroupName, const char *pszClassName,
                vector<string> *pvecPaths, const char *pszStartPath)
{
  static bool bReEnter = false;
  bool bFirstCall = !bReEnter;
  bReEnter = true;

  if( bFirstCall && pszStartPath )
    OpenGroupPath(pszStartPath);

  NexusItemInfo aItemInfo;
  int iRc = GetFirstItem(&aItemInfo);
  while( NX_OK == iRc )
  {
    if( aItemInfo.IsGroup() )
    {
      CString strPath = pszStartPath;
      strPath += '/' + CString(aItemInfo.ItemName()) + '<' + CString(aItemInfo.ClassName()) + '>';
      if( stricmp(pszClassName, aItemInfo.ClassName()) == 0 &&
          (!pszGroupName || stricmp(pszGroupName, aItemInfo.ItemName()) == 0) )
      {
        pvecPaths->push_back(strPath);
      }
      
      OpenGroup(aItemInfo.ItemName(), aItemInfo.ClassName());
      SearchGroup(pszGroupName, pszClassName, pvecPaths, PSZ(strPath));
      CloseGroup();
    }

    iRc = GetNextItem(&aItemInfo);
  }

  if( bFirstCall )
  {
    bReEnter = false;
    if( pvecPaths->size() == 0 )
      return NX_EOD;
  }
  return NX_OK;
}

//-----------------------------------------------------------------------------
// NexusFile::SearchDataSetFromAttr
//-----------------------------------------------------------------------------
int NexusFile::SearchDataSetFromAttr(const char *pszAttrName, vector<string> *pvecDataSets,
                                     const string &strAttrVal)
{
  soleil::CString strName(pszAttrName);
  // browse through items into current group
  NexusItemInfo aItemInfo;
  int iRc = GetFirstItem(&aItemInfo);
  while( NX_OK == iRc )
  {
    if( aItemInfo.IsDataSet() )
    {
      // Open data set and through its attributs 
      NexusAttrInfo aAttrInfo;
      int iRc = GetFirstAttribute(&aAttrInfo, aItemInfo.ItemName());
      while( NX_OK == iRc )
      {
        if( strName.IsEqualsNoCase(aAttrInfo.AttrName()) )
        {
          if( !strAttrVal.empty() )
          {
            // Get attr value
            string strValue = GetAttributeAsString(aAttrInfo);
            if( strValue == strAttrVal )
              // Ok add data set name
              pvecDataSets->push_back(aItemInfo.ItemName());
          }
          else
            // Ok add data set name
            pvecDataSets->push_back(aItemInfo.ItemName());
          break;
        }
        iRc = GetNextAttribute(&aAttrInfo);
      }
    }
    iRc = GetNextItem(&aItemInfo);
  }
  if( pvecDataSets->size() )
    return NX_OK;
  return NX_EOD;
}

//-----------------------------------------------------------------------------
// NexusFile::SearchFirstDataSetFromAttr
//-----------------------------------------------------------------------------
int NexusFile::SearchFirstDataSetFromAttr(const char *pszAttrName, string *pstrDataSet,
                                          const string &strAttrVal)
{
  vector<string> vecstrDataSet;
  int rc = SearchDataSetFromAttr(pszAttrName, &vecstrDataSet, strAttrVal);
  if( NX_OK == rc )
    *pstrDataSet = vecstrDataSet[0];
  else
    *pstrDataSet = "";
  return rc;
}
//-----------------------------------------------------------------------------
// NexusFile::SearchDataSetFromAttr
//-----------------------------------------------------------------------------
string NexusFile::GetAttributeAsString(const NexusAttrInfo &aAttrInfo)
{
  // Get attribute
  int iBufLen = 1024;
  char szBuf[1024];
  int iDataType = aAttrInfo.DataType();
  m_pImpl->GetAttribute(aAttrInfo.AttrName(), &iBufLen, szBuf, &iDataType);

  ostringstream ossVal;
  // Setting floating point precision
  ossVal.precision(10);

  switch( iDataType )
  {
    case NX_INT8:
    {
      int iVal = *((char *)szBuf);
      ossVal << iVal;
    }
    break;
    case NX_UINT8:
    {
      int iVal = *((unsigned char *)szBuf);
      ossVal << iVal;
    }
    break;
    case NX_INT16:
      ossVal << *((short *)szBuf);
    break;
    case NX_UINT16:
      ossVal << *((unsigned short *)szBuf);
    break;
    case NX_INT32:
      ossVal << *((long *)szBuf);
    break;
    case NX_UINT32:
      ossVal << *((unsigned long *)szBuf);
    break;
    case NX_FLOAT32:
      ossVal << *((float *)szBuf);
    break;
    case NX_FLOAT64:
      ossVal << *((double *)szBuf);
    break;
    case NX_CHAR:
      ossVal.write(szBuf, iBufLen);
    break;
    default:
      //## Error...
      break;
  }
  return ossVal.str();
}

//-----------------------------------------------------------------------------
// NexusFile::CurrentGroupName
//-----------------------------------------------------------------------------
string NexusFile::CurrentGroupName()
{
  string strGroup = m_pImpl->CurrentGroup();
	return strGroup.substr(0, strGroup.find('(') - 1);
}

//-----------------------------------------------------------------------------
// NexusFile::CurrentGroupClass
//-----------------------------------------------------------------------------
string NexusFile::CurrentGroupClass()
{
  string strGroup = m_pImpl->CurrentGroup();  
	return strGroup.substr(strGroup.find('(') + 1, strGroup.find(')') - strGroup.find('(') - 1);
}

//-----------------------------------------------------------------------------
// NexusFile::HasAttribute
//-----------------------------------------------------------------------------
bool NexusFile::HasAttribute(const char *pszAttrName, const char *pcszDataSet, const string &strAttrVal)
{
	NexusAttrInfo attrInfo;
  int iStatus = GetFirstAttribute(&attrInfo, pcszDataSet);
	while( iStatus != NX_EOD )
	{
		if( strAttrVal.empty() && stricmp(attrInfo.AttrName(), pszAttrName) == 0 )
			return true;
    else if( stricmp(attrInfo.AttrName(), pszAttrName) == 0 )
    {
      string strValue = GetAttributeAsString(attrInfo);
      if( strValue == strAttrVal )
			  return true;
    }
		iStatus = GetNextAttribute(&attrInfo);
	}
	return false;
}

//-----------------------------------------------------------------------------
// NexusFile::BuildAxisDict
//-----------------------------------------------------------------------------
bool NexusFile::BuildAxisDict(map<string, string> *pdict, const char *pszGroupPath, const char *pcszDataSet)
{
  // Open group and dataset if needed
  if( pszGroupPath )
    OpenGroupPath(pszGroupPath);
  if( pcszDataSet )
    m_pImpl->OpenDataSet(pcszDataSet);

  // Check 'signal' attribute
  if( !HasAttribute("signal") )
  {
    throw NexusException(NO_SIGNAL_ATTR_ERR, NO_DATASET, "NexusFile::GetAxisList");
  }

  String strAxes;
  // Check for 'axes' attribute
  if( HasAttribute("axes") )
  {
    // The 'axes' attribute immediately give the axis list
    GetAttribute("axes", &strAxes);
  }

  if( !strAxes.empty() && !(strAxes.StartWith("[") || strAxes.EndWith('?') || strAxes.EndWith(']')) )
  {
    // Retreive separator
    unsigned int iSepPos =  strAxes.find_first_of(",:");
    char cSep = ':';
    if( iSepPos != string::npos )
      cSep = strAxes[iSepPos];

    int iDim=1;
    CString strAxeName;
    while( !strAxes.empty() )
    {
      ostringstream oss;
      oss << "axis_" << iDim << "_1";
      strAxes.ExtractTokenRight(cSep, &strAxeName);
      (*pdict)[oss.str()] = strAxeName;
    }
  }
  else
  {
    // Search for datasets with 'axis' attribute
    NexusItemInfo aItemInfo;
    int iRc = GetFirstItem(&aItemInfo);
    while( NX_EOD != iRc )
    {
      if( aItemInfo.IsDataSet() )
      {
        OpenDataSet(aItemInfo.ItemName());
        if( HasAttribute("axis") )
        {
          // We found a axis
          // Get axis (dimension) and order (primary)
          long lAxis, lPrimary=1;
          GetAttribute("axis", &lAxis);
          // Look for 'primary' attribute
          if( HasAttribute("primary") )
            GetAttribute("primary", &lPrimary);
          // Fill dictionnary
          ostringstream oss;
          oss << "axis_" << lAxis << "_" << lPrimary;
          (*pdict)[oss.str()] = aItemInfo.ItemName();
        }
        CloseDataSet();
      }
      iRc = GetNextItem(&aItemInfo);
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// NexusFile::BuildScanAxisDict
//-----------------------------------------------------------------------------
bool NexusFile::BuildScanAxisDict(map<string, string> *pdict, const char *pszGroupPath, const char *pcszDataSet)
{
  // Open group and dataset if needed
  if( pszGroupPath )
    OpenGroupPath(pszGroupPath);
  if( pcszDataSet )
    m_pImpl->OpenDataSet(pcszDataSet);

  // Check 'signal' attribute
  if( !HasAttribute("signal") )
  {
    throw NexusException(NO_SIGNAL_ATTR_ERR, NO_DATASET, "NexusFile::GetAxisList");
  }

  String strAxes;
  // Check for 'axes' attribute
  if( HasAttribute("axes") )
  {
    // The 'axes' attribute immediately give the axis list
    GetAttribute("axes", &strAxes);
  }

  // Search for datasets with 'axis' attribute
  NexusItemInfo aItemInfo;
  int iRc = GetFirstItem(&aItemInfo);
  while( NX_EOD != iRc )
  {
    if( aItemInfo.IsDataSet() )
    {
      OpenDataSet(aItemInfo.ItemName());
      if( HasAttribute("axis") )
      {
        // We found a axis
        // Get axis (dimension) and order (primary)
        long lAxis, lPrimary=1;
        GetAttribute("axis", &lAxis);
        // Look for 'primary' attribute
        if( HasAttribute("primary") )
          GetAttribute("primary", &lPrimary);

        // Look for trajectory pointer
        if( HasAttribute("trajectory") )
        {
          String strTrajectory;
          GetAttribute("trajectory", &strTrajectory);

          // Fill dictionnary with readed axis values (e.g. actuator at Soleil)
          ostringstream oss1;
          oss1 << "axis-readed_" << lAxis << "_" << lPrimary;
          (*pdict)[oss1.str()] = aItemInfo.ItemName();

          // Get trajectory dataset name
          GetAttribute("trajectory", &strTrajectory);

          // Fill dictionnary with setted values  (e.g. trajectory at Soleil)
          ostringstream oss2;
          oss2 << "axis-setted_" << lAxis << "_" << lPrimary;
          (*pdict)[oss2.str()] = strTrajectory;
        }
        else
        {
          // Fill dictionnary
          ostringstream oss;
          oss << "axis_" << lAxis << "_" << lPrimary;
          (*pdict)[oss.str()] = aItemInfo.ItemName();
        }
      }
      CloseDataSet();
    }
    iRc = GetNextItem(&aItemInfo);
  }
  return true;
}

//-----------------------------------------------------------------------------
// NexusFile::GetScanDim
//-----------------------------------------------------------------------------
int NexusFile::GetScanDim(const char *pszGroupPath)
{
  // Open group and dataset if needed
  if( pszGroupPath )
    OpenGroupPath(pszGroupPath);

  int iMaxAxis = -1;
  // Search for datasets with 'axis' attribute
  NexusItemInfo aItemInfo;
  int iRc = GetFirstItem(&aItemInfo);
  while( NX_EOD != iRc )
  {
    if( aItemInfo.IsDataSet() )
    {
      OpenDataSet(aItemInfo.ItemName());
      if( HasAttribute("axis") )
      {
        // We found a axis
        // Get axis (dimension) and order (primary)
        long lAxis, lPrimary=1;
        GetAttribute("axis", &lAxis);
        if( lAxis > iMaxAxis )
          iMaxAxis = (int) lAxis;
      }
      CloseDataSet();
    }
    iRc = GetNextItem(&aItemInfo);
  }

  if( 0 == iMaxAxis ) 
    // Time scan
    return 1;

  if( -1 == iMaxAxis )
    // No axis dataset found => not a scan
    return 0;

  return iMaxAxis;
}
