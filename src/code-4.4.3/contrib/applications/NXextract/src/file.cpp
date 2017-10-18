//*****************************************************************************
// Synchrotron SOLEIL
//
// Files manipulations
//
// Creation : 25/05/2005
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
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base.h"
#include "membuf.h"
#include "date.h"
#include "file.h"

using namespace gdshare;

// Error msgs
const char ERR_CANNOT_CREATE_FOLDER[]     = "Cannot create directory '%s'";
const char ERR_CANNOT_CREATE_LINK[]       = "Cannot create link; name = '%s', target='%s'";
const char ERR_CANNOT_ENUM_DIR[]          = "Cannot enumerate directory '%s'";
const char ERR_CANNOT_REMOVE_FILE[]       = "Cannot remove file '%s'";
const char ERR_CANNOT_FETCH_INFO[]        = "Cannot fetch informations for file '%s'";
const char ERR_CANNOT_RENAME_FILE[]       = "Cannot rename file '%s'";
const char ERR_FILE_NOT_FOUND[]           = "File '%s' not found";
const char ERR_DIR_NOT_FOUND[]            = "Directory '%s' not found";
const char ERR_COPY_FAILED[]              = "A error has occured while copying from '%s' to '%s'";
const char ERR_OPEN_FILE[]                = "Cannot open file '%s'";
const char ERR_CANNOT_CREATE_WIN32[]      = "Cannot get handle for '%s' (Win32 API)";
const char ERR_CANNOT_CHANGE_FILE_TIME[]  = "Cannot change file time for '%s'";
const char ERR_CANNOT_GET_FILE_TIME[]     = "Cannot get file time for '%s'";
const char ERR_READING_FILE[]             = "Error while reading file '%s'";
const char ERR_WRITING_FILE[]             = "Error while writing file '%s'";
const char ERR_STAT_FAILED[]              = "Cannot get informations about file '%s'";
const char ERR_CHMOD_FAILED[]             = "Cannot change access for '%s' to '%o'";
const char ERR_CHOWN_FAILED[]             = "Cannot change owner for '%s' to %d:%d";
const char ERR_FSTYPE[]                   = "Error gathering file system information on '%s'";
const char ERR_NOT_A_DIRECTORY[]          = "Is not a directory";
const char ERR_DELETE_DIRECTORY[]         = "Cannot delete directory '%s'";
const char ERR_BAD_DEST_PATH[]            = "Bad destination path '%s'";

// Begining of cygwin absolute file names
const char FILE_CYGDRIVE[] = "\\cygdrive\\";

//-------------------------------------------------------------------
// PathExists
//-------------------------------------------------------------------
bool gdshare::PathExists(pcsz pszPath)
{
  if( strchr(pszPath, '*') || strchr(pszPath, '?') )
    // there are wildcard. this is not a valid path
    return false;
 
  uint uiLen = strlen(pszPath) ;
  if (uiLen == 0)
     return false;
 
#ifdef __WIN32__
  WIN32_FIND_DATA find;
  HANDLE          h;
  if( pszPath[uiLen-1] == '\\' )
  {
    if( uiLen >= 2 && pszPath[uiLen-2] == ':' )
    {
      // Path is a disque drive name
      uint uiRc = ::GetDriveType( pszPath );
      if( uiRc == 0 || uiRc == 1 )
        return false;
      return true;
    }
    // Path ends with '\' => remove '\'
    String strPath = pszPath;
    strPath = strPath.substr(0, strPath.size()-1);
    h = FindFirstFile( PSZ(strPath), &find );
  }
  else
    h = FindFirstFile( pszPath, &find );
 
  if( h == INVALID_HANDLE_VALUE )
    return false;
  FindClose( h );
  return MAKEBOOL(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat  st ;
  int          iRc ;

  if( uiLen>=2 && IsSepPath(pszPath[uiLen-1]) && pszPath[uiLen-2] != ':' )
  {
    // Path ends with '\' => remove '\'
    String strPath = pszPath;
    strPath = strPath.substr(0, strPath.size()-1);
    iRc = stat(PSZ(strPath), &st);
  }
  else
    iRc = stat(pszPath, &st);
  return !iRc && (st.st_mode & S_IFDIR);
#endif
}

//----------------------------------------------------------------------------
// FileExists
//----------------------------------------------------------------------------
bool gdshare::FileExists(pcsz pcszFullName)
{
#ifdef __WIN32__ // Windows version

  WIN32_FIND_DATA find;
  HANDLE h = FindFirstFile(pcszFullName, &find);
  if( h == INVALID_HANDLE_VALUE )
    return false;
  FindClose( h );
  return MAKEBOOL(!(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));

#else // Linux version

  struct stat st;
  return MAKEBOOL(!access(pcszFullName, 0) &&
         !stat(pcszFullName, &st) &&
         (st.st_mode & S_IFREG));

#endif
}

//----------------------------------------------------------------------------
// AccessFromString
//----------------------------------------------------------------------------
mode_t gdshare::AccessFromString(const String &strAccess)
{
  mode_t mode = 0;
#ifdef __LINUX__
  sscanf(PSZ(strAccess), "%o", &mode);
#endif
	return mode;
}

///===========================================================================
/// FileName
///
/// File name manipulations
///===========================================================================

//----------------------------------------------------------------------------
// FileName::Set
//----------------------------------------------------------------------------
void FileName::Set(const String &strFullName)
{
  SetFullName(PSZ(strFullName));

  if( PathExists() && !IsPathName() )
    // Add missing path separator
    m_strFile += SEP_PATH;
}

void FileName::Set(const String &_strPath, const String &_strName)
{
  String strPath = _strPath;
  String strName = _strName;

  // Evaluate env variables
  CTemplateProcessor tproc;
  CEnvVariableEvaluator aObject;
  tproc.AddEvaluator(&aObject);
  tproc.Process(&strName);
  tproc.Process(&strPath);

  ConvertSeparators(&strPath);
  ConvertSeparators(&strName);

  String strFullName;

  if( EndWith(strPath, SEP_PATH) )
    strFullName = strPath + strName;
  else
    strFullName = strPath + SEP_PATH + strName;
  Set(strFullName);
}

void FileName::Set(const String &strPath, const String &strName, const  String &strExt)
{
  Set(strPath, strName + "." + strExt);
}

//----------------------------------------------------------------------------
// FileName::IsPathName
//----------------------------------------------------------------------------
bool FileName::IsPathName() const
{
  if( EndWith(m_strFile, SEP_PATH) )
    return true;
  return false;
}

//-------------------------------------------------------------------
// Indique si le nom entre correspond a un path existant
//-------------------------------------------------------------------
bool FileName::PathExists() const
{
  return gdshare::PathExists(PSZ(FullName()));
}

//-------------------------------------------------------------------
// Indique si le nom correspond a un fichier existant
//-------------------------------------------------------------------
bool FileName::FileExists() const
{
  return gdshare::FileExists(PSZ(FullName()));
}

//----------------------------------------------------------------------------
// FileName::SetFullName
//----------------------------------------------------------------------------
void FileName::SetFullName(pcsz pszFileName)
{
  if( !pszFileName || !pszFileName[0] )
  {
    m_strFile = g_strEmpty;
    return;
  }

  String strFileName = pszFileName;

  // Evaluate env variables
  CTemplateProcessor tproc;
  CEnvVariableEvaluator aObject;
  tproc.AddEvaluator(&aObject);
  tproc.Process(&strFileName);

  // Convert separators
  ConvertSeparators(&strFileName);

  #ifdef __UNIX__

    if (IsSepPath(strFileName[0u]))
    {
      // Absolute name
      m_strFile = strFileName;
    }
    else
    {
      // relative name: add current working directory
      char cbuf[_MAX_PATH];
      getcwd(cbuf, _MAX_PATH);
      m_strFile = StrFormat("%s/%s", cbuf, PSZ(strFileName));
    }

  #elif defined(__WIN32__)

    m_strFile = strFileName;
   
    // Warning: strFile may be empty
    if( m_strFile.size() > 0 && 
          ((IsSepPath(m_strFile[0u] && IsSepPath(m_strFile[1u])) || // UNC '\\'
          m_strFile[1u] == SEP_DSK )))                              // Drive letter: 'X:'
    { 
      // Absolute name

    }
    else
    {
      _getcwd(g_acScratchBuf, _MAX_PATH);
      String strDir = g_acScratchBuf;
 
      // Convert path into full name in windows format.
      // 3 cases :
      // - path begin with /cygdrive/<letter>/ => absolute path name
      //   We replace this part by the drive letter
      // - The path name begins with a '/' => UNIX absolute path name.
      //   We insert current media name at begining
      // - For all other cases it's a relative path name
      if( StartWith(m_strFile, FILE_CYGDRIVE) && m_strFile[11u] == '\\' )
      {
        // Case of paths like '/cygdrive/<letter>/'
        strDir = StrFormat("%c:\\", m_strFile[10u]);
        m_strFile = StrFormat("%c:\\%s", m_strFile[10u], m_strFile.substr(12, std::string::npos));
      }
      else if( IsSepPath(m_strFile[0u]) )
      {
        // Absolute path without drive letter:
        // - add drive letter coming from getcwd
        m_strFile = StrFormat("%c:\\%s", strDir[0u], PSZ(m_strFile));
      }
      else
      {
        // getcwd may not add the traling path separator
        if( !EndWith(strDir, SEP_PATH) )
          strDir += SEP_PATH;

        if( IsEquals(m_strFile, ".") )
          m_strFile = strDir;
        else
          m_strFile = strDir + m_strFile;
      }
    }
  #endif
}

//----------------------------------------------------------------------------
// FileName::Path
//----------------------------------------------------------------------------
String FileName::Path() const
{
  String strPath = g_strEmpty;

  // Backward search for first separator
  String::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if( String::npos != iLastSepPos )
   // Found
   strPath = m_strFile.substr(0, iLastSepPos + 1);

  return strPath;
}

//----------------------------------------------------------------------------
// FileName::Name
//----------------------------------------------------------------------------
String FileName::Name() const
{
  String strName = g_strEmpty;

  // Backward search for first separator
  String::size_type iLastSepPos = m_strFile.find_last_of(SEP_PATH);
  if( String::npos == iLastSepPos )
    // If not found  : no path
    iLastSepPos = 0;

  String::size_type iExtPos = m_strFile.find_last_of(SEP_EXT);
  if( String::npos == iExtPos )
    iExtPos = m_strFile.length();

  strName = m_strFile.substr(iLastSepPos + 1, iExtPos - iLastSepPos - 1);

  return strName;
}

//----------------------------------------------------------------------------
// FileName::DirName
//----------------------------------------------------------------------------
String FileName::DirName() const
{
  String strName = g_strEmpty;

  // Backward search for last separator
  int iLastSepPos = (int)m_strFile.find_last_of(SEP_PATH);
  if( String::npos == iLastSepPos )
    return g_strEmpty;
  int iPreviousSepPos = (int)m_strFile.rfind(SEP_PATH, iLastSepPos-1);
  if( String::npos == iPreviousSepPos )
    iPreviousSepPos = -1;

  strName = m_strFile.substr(iPreviousSepPos + 1, iLastSepPos - iPreviousSepPos - 1);

  return strName;
}

//-------------------------------------------------------------------
// FileName::RelName
//-------------------------------------------------------------------
String FileName::RelName(const char* pszPath) const
{
  FileName fnRef(pszPath);

  // On windows check if both paths are on the same disk
  // Otherwise we return the full name
  #ifdef __WIN__
    // On ne peut pas comparer les caracteres a cause du case
    if( strnicmp(m_strFile.Buf(), fnRef.FullName(), 1) )
      return m_strFile;
  #endif

  // Search for first separator. If not => return full name
  const char* p = strchr(PSZ(m_strFile), SEP_PATH);
  const char* pRef = strchr(PSZ(fnRef.FullName()), SEP_PATH);
  if (!p || !pRef)
    return m_strFile;

  String str;
  bool bClimbStarted = false;
  for(;;)
  {
    const char* p1 = strchr(p+1, SEP_PATH);
    const char* pRef1 = strchr(pRef+1, SEP_PATH);

    if( !p1 )
    {
      // No more parts in file name
      while( pRef1 )
      {
        #ifdef __UNIX__
          str = string("../") + str;
        #else
          str = string("..\\") + str;
        #endif
        pRef1 = strchr(pRef1+1, SEP_PATH);
      }
      str += string(p+1);
      return str;
    }

    if( !pRef1 )
    {
      // No more reference
      str += string(p+1);
      return str;
    }

    // Compare directories
    if( (p1-p != pRef1-pRef) || bClimbStarted ||
        #ifdef __UNIX__
          // Unix : le case est important
          strncmp(p, pRef, p1-p) )
        #else
          _strnicmp(p, pRef, p1-p) )
        #endif
    {
      // Different directory
      #ifdef __UNIX__
        str = string("../") + str;
      #else
        str = string("..\\") + str;
      #endif
      bClimbStarted = true;
      str.append(p+1, p1-p);
    }
    p = p1;
    pRef = pRef1;
  }
}

//----------------------------------------------------------------------------
// FileName::NameExt
//----------------------------------------------------------------------------
String FileName::NameExt() const
{
  String strFileName = Name();
  if( Ext().size() > 0 )
    strFileName += '.' + Ext();
  return strFileName;
}

//----------------------------------------------------------------------------
// FileName::Ext
//----------------------------------------------------------------------------
String FileName::Ext() const
{
  String strExt = g_strEmpty;

  // Search backward for extension separator
  String::size_type iExtPos = m_strFile.find_last_of(SEP_EXT);
  if( String::npos != iExtPos )
    // Separator found
    strExt = m_strFile.substr(iExtPos + 1);

  return strExt;
}

//----------------------------------------------------------------------------
// FileName::ConvertSeparators
//----------------------------------------------------------------------------
void FileName::ConvertSeparators(String *pstr)
{
  char *ptc = new char[pstr->length()+1];
  char *pStart = ptc;
  strcpy(ptc, PSZ(*pstr));

  #ifdef __UNIX__

    // Convert from DOS to UNIX
    while (*ptc)
    {
      if (*ptc == SEP_PATHDOS)
        *ptc = SEP_PATHUNIX;
      ptc++;
    }

  #elif defined(__WIN32__)

    // Convert from UNIX to DOS
    char* ptcStart = ptc;
    while (*ptc)
    {
      if (*ptc == SEP_PATHUNIX)
        *ptc = SEP_PATHDOS;

      // If there are two consecutive separators, remove the second but not if it's the begining of the path => UNC
      if (*ptc == SEP_PATHDOS && (ptc - 1) > ptcStart && *(ptc - 1) == SEP_PATHDOS)
      {
        // Removing second sep
        strcpy(ptc - 1, ptc);
      }
      ptc++;
    }
  #endif

  *pstr = pStart;
  delete [] pStart;
}

//----------------------------------------------------------------------------
// FileName::MkDir
//----------------------------------------------------------------------------
void FileName::MkDir(mode_t mode, uid_t uid, gid_t gid) const
{
  LogVerbose("file", "Creating directory '%s'...", PSZ(Path()));
  String str = Path();
  if( str.empty() )
    return;

  const char   *p;
#ifdef __WIN32__
  // Skeep UNC starting if it exists
  if( str[0u] == SEP_PATH && str[1u] == SEP_PATH )
  {
    // Saute le nom de la machine (qui suit les '\\' du debut)
    char *p1 = const_cast<char*>(strchr( PSZ(str) + 2, SEP_PATH ));
    if( p1 == NULL || p1 == PSZ(str) + 2 )
    {
      String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
      throw BadPathException(PSZ(strErr), "Bad path", "FileName::MkDir");
    }
    // Saute le nom du share
    p = strchr( p1 + 1, SEP_PATH );
    if( p == NULL || p == p1 + 1 )
    {
      String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
      throw BadPathException(PSZ(strErr), "Bad path", "FileName::MkDir");
    }
  }
  else
  {
    p = const_cast<char*>(strchr(PSZ(str), SEP_PATH));
  }
#else
  p = strchr(PSZ(str), SEP_PATH);
#endif

  if( !p )
  {
    String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
    throw BadPathException(PSZ(strErr), "Bad path", "FileName::MkDir");
  }
  p = strchr(p+1, SEP_PATH);
  if( !p )
  {
    #ifdef __WIN32__
      // Check drive
      if( str[1u] != ':' )
      {
        String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
        throw BadDriveException(PSZ(strErr), "Bad path", "FileName::MkDir");
      }

      if( ::GetDriveType(PSZ(str)) <= 1 )
      {
        String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
        throw BadDriveException(PSZ(strErr), "Bad path", "FileName::MkDir");
      }

    #endif
    // path = racine ; exist
    return;
  }

  do
  {
    str[p - PSZ(str)] = '\0';  // *p = 0;
    struct stat st;
    if( ::stat(PSZ(str), &st) )
    {
      #ifdef __UNIX__

        if( errno != ENOENT )
          // stat call report error != file not found
          ThrowExceptionFromErrno(PSZ(StrFormat(ERR_STAT_FAILED, PSZ(str))), "FileName::MkDir");
        else
          LogInfo("file", "Create directory '%s'", PSZ(str));

        if( mkdir(PSZ(str), 0000777) )
        {
          // failure
          ThrowExceptionFromErrno(PSZ(StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str))), "FileName::MkDir");
        }

        // Change access mode if needed
        if( mode != 0 )
        {
          if( chmod(PSZ(str), mode) )
          {
            // changing access mode failed
            ThrowExceptionFromErrno(PSZ(StrFormat(ERR_CHMOD_FAILED, PSZ(str), mode)), "FileName::MkDir");
          }
        }
        // Change owner if needed
        if( (int)uid != -1 || (int)gid != -1 )
        {
          if( chown(PSZ(str), uid, gid) )
          {
            // changing owner mode failed
            ThrowExceptionFromErrno(PSZ(StrFormat(ERR_CHOWN_FAILED, PSZ(str), uid, gid)), "FileName::MkDir");
          }
        }

      #else // !__UNIX__

        if( mkdir(PSZ(str)) )
        {
          // creation error
          String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
          throw FileFailureException(PSZ(strErr), "Operation failed", "FileName::MkDir");
        }

      #endif
    }
    else
    {
      if( !(st.st_mode & S_IFDIR) )
      {
        // c'est un fichier : erreur
        String strErr = StrFormat(ERR_CANNOT_CREATE_FOLDER, PSZ(str));
        throw BadPathException(PSZ(strErr), "Path is a file path", "FileName::MkDir");
      }
      // Directory : ok
    }
    // Next path component
    str[p - PSZ(str)] = SEP_PATH;   // *p = SEP_PATH;
    p = strchr(p+1, SEP_PATH);
  } while( p );
}

//----------------------------------------------------------------------------
// FileName::LinkExists
//----------------------------------------------------------------------------
bool FileName::LinkExists() const
{
#ifdef __LINUX__
  struct stat st;
  String strFullName = FullName();
  if( IsPathName() )
    strFullName.erase(strFullName.size()-1, 1);
  int iRc = lstat(PSZ(strFullName), &st);
  if( !iRc && S_ISLNK(st.st_mode) )
    return true;
#endif
  return false;
}

//----------------------------------------------------------------------------
// FileName::MakeSymLink
//----------------------------------------------------------------------------
void FileName::MakeSymLink(const String &strTarget, uid_t uid, gid_t gid) const
{
#ifdef __LINUX__
  int iRc = symlink(PSZ(strTarget), PSZ(FullName()));
  if( iRc )
  {
    String strErr = StrFormat(ERR_CANNOT_CREATE_LINK, PSZ(FullName()), PSZ(strTarget));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::MakeSymLink");
  }
  // Change owner if needed
  if( (int)uid != -1 || (int)gid != -1 )
  {
    if( lchown(PSZ(FullName()), uid, gid) )
    {
      // changing owner mode failed
      String strErr = StrFormat(ERR_CHOWN_FAILED, PSZ(FullName()), uid, gid);
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::MakeSymLink");
    }
  }

#endif
}

//----------------------------------------------------------------------------
// FileName::Delete
//----------------------------------------------------------------------------
void FileName::Delete()
{
  if( !m_strFile.empty() )
  {
    LogVerbose("file", "Deleting file '%s'...", PSZ(FullName()));
    if( unlink(PSZ(FullName())) )
    {
      String strErr = StrFormat(ERR_CANNOT_REMOVE_FILE, PSZ(FullName()));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::Delete");
    }
  }
}

//----------------------------------------------------------------------------
// FileName::Rmdir
//----------------------------------------------------------------------------
void FileName::Rmdir(bool bRecursive, bool bContentOnly)
{
  if( !IsPathName() )
    throw BadPathException(PSZ(StrFormat(ERR_DELETE_DIRECTORY, PSZ(FullName()))),
                           ERR_NOT_A_DIRECTORY, "FileName::Rmdir");

  if( !m_strFile.empty() )
  {
    LogVerbose("file", "Deleting content of directory '%s'...", PSZ(FullName()));
    if( !bRecursive )
    {
      if( rmdir(PSZ(FullName())) )
      {
        String strErr = StrFormat(ERR_CANNOT_REMOVE_FILE, PSZ(FullName()));
        ThrowExceptionFromErrno(PSZ(strErr), "FileName::Rmdir");
      }
    }
    else
    { // Recursively delete directory content
      FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
      while( dirEnum.Find() )
        dirEnum.Rmdir(true);
      // Delete files & symbolic links
      FileEnum fileEnum(FullName(), FileEnum::ENUM_FILE);
      while( fileEnum.Find() )
        fileEnum.Delete();

      if( !bContentOnly )
      {
        LogVerbose("file", "Deleting directory '%s'...", PSZ(FullName()));
        // And finally the direcory himself
        Rmdir();
      }
    }
  }
}

//----------------------------------------------------------------------------
// FileName::Size
//----------------------------------------------------------------------------
ulong FileName::Size() const
{
#ifdef __WIN32__
  WIN32_FIND_DATA find;
  HANDLE h = FindFirstFile(PSZ(m_strFile), &find);
  if( h == INVALID_HANDLE_VALUE )
  {
    String strErr = StrFormat(ERR_CANNOT_FETCH_INFO, PSZ(m_strFile));
    throw FileFailureException(PSZ(strErr), "Invalid handle", "FileName::Size");
  }
  FindClose(h);
  return (ulong)find.nFileSizeLow;
#else
  struct stat sStat;
  if( stat(PSZ(FullName()), &sStat) == -1 )
  {
    String strErr = StrFormat(ERR_CANNOT_FETCH_INFO, PSZ(m_strFile));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Size");
  }
  return sStat.st_size;
#endif
}

//----------------------------------------------------------------------------
// FileName::Size64
//----------------------------------------------------------------------------
int64 FileName::Size64() const
{
#ifdef __WIN32__
  WIN32_FIND_DATA find;
  HANDLE h = FindFirstFile(PSZ(m_strFile), &find);
  if( h == INVALID_HANDLE_VALUE )
  {
    String strErr = StrFormat(ERR_CANNOT_FETCH_INFO, PSZ(m_strFile));
    throw FileFailureException(PSZ(strErr), "Invalid handle", "FileName::Size");
  }
  FindClose(h);
  return (ulong)find.nFileSizeLow;
#else
  struct stat sStat;
  if( stat(PSZ(FullName()), &sStat) == -1 )
  {
    String strErr = StrFormat(ERR_CANNOT_FETCH_INFO, PSZ(m_strFile));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Size");
  }
  return sStat.st_size;
#endif
}

//----------------------------------------------------------------------------
// FileName::Rename
//----------------------------------------------------------------------------
void FileName::Rename(const String &strNewName)
{
  if( !m_strFile.empty() )
    if( rename(PSZ(m_strFile), PSZ(strNewName)) )
    {
      String strErr = StrFormat(ERR_CANNOT_RENAME_FILE, PSZ(m_strFile));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::Rename");
    }
 
  // Change internal name
  Set(strNewName);
}

//----------------------------------------------------------------------------
// FileName::ModTime
//----------------------------------------------------------------------------
void FileName::ModTime(Time *pTm, bool bLocalTime) const
{
#ifdef __WIN32__
  HANDLE hFile = CreateFile(PSZ(FullName()), GENERIC_READ, FILE_SHARE_READ,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if( hFile == INVALID_HANDLE_VALUE )
	{
      String strErr = StrFormat(ERR_CANNOT_CREATE_WIN32, PSZ(m_strFile));
      throw FileFailureException(PSZ(strErr), "Invalid handle", "FileName::ModTime");
	}
  FILETIME fileTime;
  GetFileTime(hFile, NULL, NULL, &fileTime);
  FileTimeToLocalFileTime(&fileTime, &fileTime);
  CloseHandle(hFile);
  SYSTEMTIME sysTime;
  FileTimeToSystemTime(&fileTime, &sysTime);
  pTm->Set(sysTime.wYear, sysTime.wMonth, sysTime.wDay,
           sysTime.wHour, sysTime.wMinute, 
           (double)sysTime.wSecond + sysTime.wMilliseconds/1000.);
#else
  struct stat sStat;
  if( stat(PSZ(FullName()), &sStat) == -1 )
	{
      String strErr = StrFormat(ERR_CANNOT_GET_FILE_TIME, PSZ(m_strFile));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::ModTime");
	}
  
  if( bLocalTime )
  {
    struct tm tmLocal;       
    localtime_r(&sStat.st_mtime, &tmLocal);
    pTm->SetLongUnix(mktime(&tmLocal) + tmLocal.tm_gmtoff);
  }
  else
    pTm->SetLongUnix(sStat.st_mtime);

#endif
}

//----------------------------------------------------------------------------
// FileName::SetModTime
//----------------------------------------------------------------------------
void FileName::SetModTime(const Time& tm) const
{
#ifdef __WIN32__
  SDateFields sTm;
  tm.Get(&sTm);
  SYSTEMTIME sysTime;
  sysTime.wDay    = (ushort)sTm.uiDay;
  sysTime.wYear   = (ushort)sTm.iYear;
  sysTime.wMonth  = (ushort)sTm.uiMonth;
  sysTime.wHour   = (ushort)sTm.uiHour;
  sysTime.wMinute = (ushort)sTm.uiMin;
  sysTime.wSecond = (ushort)sTm.dSec;
  sysTime.wMilliseconds = (ushort)((long)(sTm.dSec*1000) % 1000);
  FILETIME fileTime;
  SystemTimeToFileTime(&sysTime, &fileTime);
  LocalFileTimeToFileTime(&fileTime, &fileTime);

  int iAttribute;
  // In order to modify a directory date under Win32, we have to position the correct flag
  // otherwise windows returns a invalid handle.
  // Then we can call SetFileTime even if the handle is not a file handle 
  if( IsPathName() )
    iAttribute = FILE_FLAG_BACKUP_SEMANTICS;
  else 
    iAttribute = FILE_ATTRIBUTE_NORMAL;
  
  HANDLE hFile = CreateFile(PSZ(FullName()), GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, OPEN_EXISTING, iAttribute, NULL);
    
  if( hFile == INVALID_HANDLE_VALUE )
	{
      String strErr = StrFormat(ERR_CANNOT_CREATE_WIN32, PSZ(m_strFile));
      throw FileFailureException(PSZ(strErr), "Invalid handle", "FileName::SetModTime");
	}
  if( !SetFileTime(hFile, NULL, NULL, &fileTime) )
	{
      CloseHandle(hFile);
      String strErr = StrFormat(ERR_CANNOT_CHANGE_FILE_TIME, PSZ(m_strFile));
      throw FileFailureException(PSZ(strErr), "Operation failed", "FileName::SetModTime");
	}
  CloseHandle(hFile);

#else
  struct utimbuf sTm;
  struct stat sStat;

  if( stat(PSZ(FullName()), &sStat) != -1 )
    // Get access time, in order to preserve it
    sTm.actime = sStat.st_atime;
  else
    // stat function failed, use the new mod time
    sTm.actime = tm.LongUnix();

  sTm.modtime = tm.LongUnix();
  if( utime(PSZ(FullName()), &sTm) )
	{
      String strErr = StrFormat(ERR_CANNOT_CHANGE_FILE_TIME, PSZ(m_strFile));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::SetModTime");
	}

#endif
}

// 1Mo
#define MAX_SIZE  1048576LL
//-------------------------------------------------------------------
// FileName::Copy
//-------------------------------------------------------------------
void FileName::Copy(const String &strDst, bool bKeepMetaData)
{
  LogVerbose("file", "Copying file '%s' to '%s'...", PSZ(FullName()), PSZ(strDst));
  if( bKeepMetaData )
    LogVerbose("file", "Keep Metadata");

  if( !FileExists() )
  { // File doesn't exists
      String strErr = StrFormat(ERR_FILE_NOT_FOUND, PSZ(m_strFile));
      throw FileNotFoundException(PSZ(strErr), "File not found", "FileName::Copy");
  }

  FileName fDst(strDst);
  if( fDst.IsPathName() )
    // Take source name
    fDst.Set(fDst.Path(), NameExt());

  LogVerbose("file", "Destination file is '%s'", PSZ(fDst.FullName()));

#ifdef __WIN32__
  if (!CopyFile(PSZ(FullName()), PSZ(fDst.FullName()), FALSE))
  {
    String strErr = StrFormat(ERR_COPY_FAILED, PSZ(FullName()), PSZ(fDst.FullName()), (long)GetLastError());
    throw FileFailureException(PSZ(strErr), "Operation failed", "FileName::Copy");
  }
#else

  // Self copy ?
  if( FullName().IsEquals(fDst.FullName()) )
  {
    String strErr = StrFormat(ERR_COPY_FAILED, PSZ(FullName()), PSZ(fDst.FullName()));
    throw FileFailureException(PSZ(strErr), "Operation failed", "FileName::Copy");
  }

  struct stat st;
  if( bKeepMetaData && IsRoot() )
  {
    int iRc = stat(PSZ(FullName()), &st);
    if( iRc )
    {
      String strErr = StrFormat(ERR_COPY_FAILED, PSZ(FullName()), PSZ(fDst.FullName()));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::Copy");
    }

  }

  // Open source file
  FILE *fiSrc = fopen(PSZ(FullName()), "r");
  if( NULL == fiSrc )
  {
    String strErr = StrFormat(ERR_OPEN_FILE, PSZ(FullName()));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Copy");
  }

  // Buffer
  char aBuf[MAX_SIZE];

  // Get last modified time
  Time tmLastMod;
  ModTime(&tmLastMod);
  
  // Opens destination file
  FILE *fiDst = fopen(PSZ(fDst.FullName()), "w");
  if( NULL == fiDst )
  {
    String strErr = StrFormat(ERR_OPEN_FILE, PSZ(fDst.FullName()));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Copy");
  }
  
  // Copy by blocs
  int64 llSize = Size64();
  long lReaded=0, lWritten=0;
  while( llSize )
  {
    long lToRead = 0;

    if( llSize > MAX_SIZE )
      lToRead = MAX_SIZE;
    else
      lToRead = (long)llSize;
      
    lReaded = fread(aBuf, 1, lToRead, fiSrc);
    if( ferror(fiSrc) )
    {
      String strErr = StrFormat(ERR_READING_FILE, PSZ(FullName()));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::Copy");
    }
	
    lWritten = fwrite(aBuf, 1, lToRead, fiDst);
    if( ferror(fiDst) || lWritten != lReaded )
    {
      String strErr = StrFormat(ERR_WRITING_FILE, PSZ(fDst.FullName()));
      ThrowExceptionFromErrno(PSZ(strErr), "FileName::Copy");
    }
    
    llSize -= lReaded;
  }

  fclose(fiSrc);
  fclose(fiDst);

  // Copy last modifitation date
  fDst.SetModTime(tmLastMod);

  // Copy file metadata: access mode, owner & group
  if( bKeepMetaData && IsRoot() )
  {
    fDst.Chown(st.st_uid, st.st_gid);
    fDst.Chmod(st.st_mode);
  }
#endif // !WIN32
}

//-------------------------------------------------------------------
// FileName::DirCopy
//-------------------------------------------------------------------
void FileName::DirCopy(const String &strDest, bool bCreateDir, mode_t modeDir, uid_t uid, gid_t gid)
{
  LogVerbose("file", "Copying directory '%s' to '%s'...", PSZ(FullName()), PSZ(strDest));
  LogVerbose("file", "Set directory mode '%o'", modeDir);
  LogVerbose("file", "Set ownership to %d:%d", uid, gid);
  FileName fnDst;

  // Create destination path
  fnDst.Set(strDest);
  fnDst.MkDir(modeDir, uid, gid);

  if( bCreateDir )
  {
    // Create source directory name inside destination path
    fnDst.Set(strDest + DirName() + SEP_PATH);
    fnDst.MkDir(modeDir, uid, gid);
  }

  if( !fnDst.IsPathName() )
    throw BadPathException(PSZ(StrFormat(ERR_BAD_DEST_PATH, PSZ(fnDst.FullName()))),
                           ERR_NOT_A_DIRECTORY, "FileName::DirCopy");

  // Recursively copying sub-directories
  FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
  while( dirEnum.Find() )
    dirEnum.DirCopy(fnDst.Path(), true, modeDir, uid, gid);

  // Copying directory files
  FileEnum fileEnum(FullName(), FileEnum::ENUM_FILE);
  while( fileEnum.Find() )
    // Copy with metadata
    fileEnum.Copy(fnDst.Path(), true);
}

//-------------------------------------------------------------------
// FileName::RecursiveChmod
//-------------------------------------------------------------------
void FileName::RecursiveChmod(mode_t modeFile, mode_t modeDir, bool bCurrentLevel)
{
  LogVerbose("file", "Recursively set access mode for content of directory '%s'...", PSZ(FullName()));
  LogVerbose("file", "Set file mode to '%o'", modeFile);
  LogVerbose("file", "Set directory mode '%o'", modeDir);

  if( !PathExists() )
  { // File doesn't exists
    String strErr = StrFormat(ERR_DIR_NOT_FOUND, PSZ(FullName()));
    throw FileNotFoundException(PSZ(strErr), "Directory not found", "FileName::RecursiveChmod");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
  while( dirEnum.Find() )
    dirEnum.RecursiveChmod(modeFile, modeDir, true);

  // Change mode on files
  FileEnum fileEnum(FullName(), FileEnum::ENUM_FILE);
  while( fileEnum.Find() )
    // Copy with metadata
    fileEnum.Chmod(modeFile);

  if( bCurrentLevel )
    // Change mode to directory itself
    Chmod(modeDir);
}

//-------------------------------------------------------------------
// FileName::RecursiveChmodFile
//-------------------------------------------------------------------
void FileName::RecursiveChmodFile(mode_t mode)
{
  LogVerbose("file", "Recusrsively set access mode for directory '%s' to '%o'...", PSZ(FullName()), mode);

  if( !PathExists() )
  { // File doesn't exists
    String strErr = StrFormat(ERR_DIR_NOT_FOUND, PSZ(FullName()));
    throw FileNotFoundException(PSZ(strErr), "Directory not found", "FileName::RecursiveChmodFile");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
  while( dirEnum.Find() )
    dirEnum.RecursiveChmodFile(mode);

  // Change mode on files
  FileEnum fileEnum(FullName(), FileEnum::ENUM_FILE);
  while( fileEnum.Find() )
    // Copy with metadata
    fileEnum.Chmod(mode);
}

//-------------------------------------------------------------------
// FileName::RecursiveChmodDir
//-------------------------------------------------------------------
void FileName::RecursiveChmodDir(mode_t mode)
{
  LogVerbose("file", "Recusrsively set access mode for directory '%s' to '%o'...", PSZ(FullName()), mode);

  if( !PathExists() )
  { // File doesn't exists
    String strErr = StrFormat(ERR_DIR_NOT_FOUND, PSZ(FullName()));
    throw FileNotFoundException(PSZ(strErr), "Directory not found", "FileName::RecursiveChmodDir");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
  while( dirEnum.Find() )
    dirEnum.RecursiveChmodDir(mode);

  // Change mode to directory itself
  Chmod(mode);
}

//-------------------------------------------------------------------
// FileName::RecursiveChown
//-------------------------------------------------------------------
void FileName::RecursiveChown(uid_t uid, gid_t gid)
{
  LogVerbose("file", "Recursively set owner mode for directory '%s' to '%d':'%d'...", PSZ(FullName()), uid, gid);

  if( !PathExists() )
  { // File doesn't exists
    String strErr = StrFormat(ERR_DIR_NOT_FOUND, PSZ(FullName()));
    throw FileNotFoundException(PSZ(strErr), "Directory not found", "FileName::RecursiveChmod");
  }

  // Recursively change rights on sub-directories
  FileEnum dirEnum(FullName(), FileEnum::ENUM_DIR);
  while( dirEnum.Find() )
    dirEnum.RecursiveChown(uid, gid);

  // Change mode on files
  FileEnum fileEnum(FullName(), FileEnum::ENUM_FILE);
  while( fileEnum.Find() )
    // Copy with metadata
    fileEnum.Chown(uid, gid);

  // Change mode to directory itself
  Chown(uid, gid);
}

//-------------------------------------------------------------------
// FileName::Move
//-------------------------------------------------------------------
void FileName::Move(const String &strDest)
{
  if( !FileExists() )
  { // File doesn't exists
      String strErr = StrFormat(ERR_FILE_NOT_FOUND, PSZ(m_strFile));
      throw FileNotFoundException(PSZ(strErr), "File not found", "FileName::Move");
  }

  FileName fDst(strDest);
  if( fDst.IsPathName() )
    // Take source name
    fDst.Set(fDst.Path(), NameExt());

  // Remove destination
  if( fDst.FileExists() )
    fDst.Delete();

#ifdef __UNIX__
  // Check filesystem id, if it's the same, we can try to rename file
  fsid_t idSrc = FileSystemId();
  fsid_t idDst = fDst.FileSystemId();
  if( idSrc.__val[0] == idDst.__val[0] && idSrc.__val[1] == idDst.__val[1] )
  {
    try
    {
      CErrorStack::SetIgnoreErrors();
      Rename(fDst.FullName());
      CErrorStack::SetIgnoreErrors(false);
    }
    catch( FileFailureException e )
    {
      CErrorStack::SetIgnoreErrors(false);
      // Unable to rename => make a copy
      Copy(fDst.FullName(), true);

      // Deletes source file and changes name
      Delete();
      Set(fDst.FullName());
    }
  }
  else
#endif
  {
    CErrorStack::SetIgnoreErrors(false);
    Copy(fDst.FullName(), true);

    // Deletes source file and changes name
    Delete();
    Set(fDst.FullName());
  }
}

//-------------------------------------------------------------------
// FileName::FileSystemType
//-------------------------------------------------------------------
FileName::FSType FileName::FileSystemType() const
{
#ifdef __UNIX__
  struct statfs buf;
  int iRc = statfs(PSZ(Path()), &buf);
  if( iRc )
  {
    String strErr = StrFormat(ERR_FSTYPE, PSZ(Path()));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::FileSystemType");
  }
  return FSType(buf.f_type);
  
#else
  // Not implemented yet on windows
  return FSType(MS);
#endif

}

//-------------------------------------------------------------------
// FileName::FileSystemId
//-------------------------------------------------------------------
fsid_t FileName::FileSystemId() const
{
#ifdef __UNIX__
  struct statfs buf;
  int iRc = statfs(PSZ(Path()), &buf);
  if( iRc )
  {
    String strErr = StrFormat(ERR_FSTYPE, PSZ(Path()));
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::FileSystemType");
  }
  return buf.f_fsid;
  
#else
  // Not implemented yet on windows
  return 0;
#endif

}

//-------------------------------------------------------------------
// FileName::Chmod
//-------------------------------------------------------------------
void FileName::Chmod(mode_t mode)
{
  LogVerbose("file", "Set access mode for '%s' to '%o'...", PSZ(FullName()), mode);

#ifdef __UNIX__
  int iRc = chmod(PSZ(FullName()), mode);
  if( iRc )
  {
    String strErr = StrFormat(ERR_CHMOD_FAILED, PSZ(FullName()), mode);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Chmod");
  }
#else
  // Not implemented yet on windows
#endif
}

//-------------------------------------------------------------------
// FileName::Chown
//-------------------------------------------------------------------
void FileName::Chown(uid_t uid, gid_t gid)
{
  LogVerbose("file", "Set owner mode for '%s' to '%d:%d'...", PSZ(FullName()), uid, gid);
#ifdef __UNIX__
  int iRc = chown(PSZ(FullName()), uid, gid);
  if( iRc )
  {
    String strErr = StrFormat(ERR_CHOWN_FAILED, PSZ(FullName()), uid, gid);
    ThrowExceptionFromErrno(PSZ(strErr), "FileName::Chown");
  }
#else
  // Not implemented yet on windows
#endif
}

//-------------------------------------------------------------------
// FileName::ThrowExceptionFromErrno
//-------------------------------------------------------------------
void FileName::ThrowExceptionFromErrno(const char *pszError, const char *pszMethod) const
{
#ifdef __UNIX__
  switch( errno )
  {
    case EIO:
      throw IOException(pszError, strerror(errno), pszMethod);
    case EPERM:
    case EACCES:
    case EROFS:
      throw PermissionException(pszError, strerror(errno), pszMethod);
    case ENOTEMPTY:
      throw BadPathConditionException(pszError, strerror(errno), pszMethod);
    case ENAMETOOLONG:
    case ELOOP:
    case EISDIR:
    case ENOTDIR:
      throw BadPathException(pszError, strerror(errno), pszMethod);
    case ENOENT:
      throw FileNotFoundException(pszError, strerror(errno), pszMethod);
    default:
      throw FileFailureException(pszError, strerror(errno), pszMethod);     
  }
#else
      throw FileFailureException(pszError, "IO error", pszMethod);     
#endif
}

//===========================================================================
// Class FileEnum
//===========================================================================

#ifdef __UNIX__

//-------------------------------------------------------------------
// Initialisation
//-------------------------------------------------------------------
FileEnum::FileEnum(const String &strPath, EEnumMode eMode)
{
  m_dirDir = NULL;
  Init(strPath, eMode);
}
//-------------------------------------------------------------------
// Destructeur
//-------------------------------------------------------------------
FileEnum::~FileEnum()
{
  Close();
}

//-------------------------------------------------------------------
// FileEnum::Init
//-------------------------------------------------------------------
void FileEnum::Init(const String &strPath, EEnumMode eMode)
{
  Close();
  m_eMode = eMode;
  Set(PSZ(strPath));
  
  // Initialize enumeration
  m_dirDir = opendir(PSZ(Path()));
  if( NULL == m_dirDir )
  {
    String strErr = StrFormat(ERR_CANNOT_ENUM_DIR, PSZ(Path()));
    throw BadPathException(PSZ(strErr), "Bad path", "FileEnum::Init");
  }
  m_strPath = strPath; // Save initial path.
}

//-------------------------------------------------------------------
// FileEnum::Find
//-------------------------------------------------------------------
bool FileEnum::Find()
{
  struct dirent *dirEntry;
  String str;
  while( (dirEntry = readdir(m_dirDir)) != NULL )
  {
    str = dirEntry->d_name;
    if( IsEquals(str, ".") == false && IsEquals(str, "..") == false )
    {
      // Set new file name
      Set(m_strPath, dirEntry->d_name);

      // Check file
      if( (m_eMode & ENUM_FILE) && FileExists() )
        return true;
      if( (m_eMode & ENUM_DIR) && PathExists() )
        return true;
    }
  }

  // Not found
  return false;
}

void FileEnum::Close()
{
  if(m_dirDir)
    closedir(m_dirDir);
}
 
#elif defined(__WIN32__)

//-------------------------------------------------------------------
// FileEnum::FileEnum
//-------------------------------------------------------------------
FileEnum::FileEnum(const String &strPath, EEnumMode eMode)
{
  // Dynamic allocation of WIN32_FIND_DATA => no need to include winbase.h
  m_pfindData = new WIN32_FIND_DATA;

  m_bFirst = true;
  m_hFind = INVALID_HANDLE_VALUE;
  Init(strPath, eMode);
}

//-------------------------------------------------------------------
// FileEnum::~FileEnum
//-------------------------------------------------------------------
FileEnum::~FileEnum()
{
  Close();
  delete m_pfindData;
}

//-------------------------------------------------------------------
// FileEnum::Init
//-------------------------------------------------------------------
void FileEnum::Init(const String &strPath, EEnumMode eMode)
{
  Close();
  m_eMode = eMode;
  m_strPath = strPath;
  m_strFile = strPath;
}

//-------------------------------------------------------------------
// FileEnum::Find()
//-------------------------------------------------------------------
bool FileEnum::Find()
{
  String str;

  while(true)
  {
    if( m_bFirst )
    {
      str = Path();
      str += '*' ;
      m_hFind = (HANDLE)FindFirstFile(PSZ(str), (WIN32_FIND_DATA *)m_pfindData);
      if( m_hFind == INVALID_HANDLE_VALUE )
        // not fount
        return false;
      m_bFirst = false;
    }
    else if( !FindNextFile((HANDLE)m_hFind, (WIN32_FIND_DATA *)m_pfindData) )
      // not found
      return false;

    str = ((WIN32_FIND_DATA *)m_pfindData)->cFileName;
    if( IsEquals(str, ".") == false && IsEquals(str, "..") == false )
    {
      // Set new file name
      Set(m_strPath, str);

      // Check file type
      if( (m_eMode & ENUM_FILE) &&
          !(((WIN32_FIND_DATA *)m_pfindData)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
        return true;

      // Add directory separator if needed
      if( (m_eMode & ENUM_DIR) &&
          (((WIN32_FIND_DATA *)m_pfindData)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
      {
        if( !IsSepPath(m_strFile[m_strFile.size()-1]) )
          m_strFile += (char)SEP_PATH;
        return true;
      }
    }
  }
}

void FileEnum::Close()
{
   if( m_hFind != INVALID_HANDLE_VALUE )
   {
     FindClose((HANDLE)m_hFind);
     m_hFind = INVALID_HANDLE_VALUE;
   }
   m_bFirst = true;
}
#endif

//===========================================================================
// Class TempFileName
//===========================================================================
long TempFileName::s_lLastNumber = 0;

//-------------------------------------------------------------------
// TempFileName::GenerateRandomName
//-------------------------------------------------------------------
String TempFileName::GenerateRandomName()
{
  if( !s_lLastNumber )
    // Initialize random sequence
    s_lLastNumber = CurrentTime().LongUnix();

  return StrFormat("temp%lx", s_lLastNumber++);
}

//-------------------------------------------------------------------
// TempFileName::TempFileName
//-------------------------------------------------------------------
TempFileName::TempFileName()
{
#ifdef __WIN32__
  Set("C:\\Temp", GenerateRandomName());
#else
  Set("/tmp", GenerateRandomName());
#endif
}

//-------------------------------------------------------------------
// TempFileName::TempFileName(path)
//-------------------------------------------------------------------
TempFileName::TempFileName(const String &strPath)
{
  Set(strPath, GenerateRandomName());
}

//===========================================================================
// Class File
//===========================================================================

//-------------------------------------------------------------------
// File::Load
//-------------------------------------------------------------------
void File::Load(MemBuf *pBuf)
{
  // Open source file
  FILE *fi = fopen(PSZ(FullName()), "r");
  if( NULL == fi )
  {
    String strErr = StrFormat(ERR_OPEN_FILE, PSZ(FullName()));
    throw FileFailureException(PSZ(strErr), "Operation failed", "File::Load");
  }

  // Buffer
  pBuf->SetLen(Size()+1);

  // Read
  long lSize = Size();
  long lReaded = fread(pBuf->Buf(), 1, lSize, fi);
  if( ferror(fi) || lSize != lReaded )
  {
    String strErr = StrFormat(ERR_READING_FILE, PSZ(FullName()));
    throw FileFailureException(PSZ(strErr), "Operation failed", "File::Load");
  }
  memset(pBuf->Buf() + lSize, 0, 1);
  fclose(fi);
}
void File::Load(String *pString)
{
  MemBuf buf;
  Load(&buf);
  buf >> (*pString);
}

//-------------------------------------------------------------------
// File::Save
//-------------------------------------------------------------------
void File::Save(const String &strContent)
{
  // Open destination file
  FILE *fi = fopen(PSZ(FullName()), "wb");
  if( NULL == fi )
  {
    String strErr = StrFormat(ERR_OPEN_FILE, PSZ(FullName()));
    throw FileFailureException(PSZ(strErr), "Operation failed", "File::Load");
  }

  // Write text content
  int iRc = fputs(PSZ(strContent), fi);
  if( EOF == iRc )
  {
    String strErr;
    strErr.Printf("Cannot write in file '%s'", PSZ(FullName()));
    throw FileFailureException(PSZ(strErr), "Operation failed", "File::Save");
  }
  fclose(fi);
}
