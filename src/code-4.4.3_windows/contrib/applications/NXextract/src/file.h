//*****************************************************************************
// Synchrotron SOLEIL
//
// Files manipulations
//
// Création : 25/05/2005
// Auteur   : S. Poirier
//
//*****************************************************************************

#ifndef __FILE_H__
#define __FILE_H__

#ifndef __BASE_H__
  #include "base.h"
#endif

#ifndef __DATE_H__
  #include "date.h"
#endif

#ifndef __MEMBUF_H__
  #include "membuf.h"
#endif

#if defined(_MSC_VER)
  #include <sys\utime.h>
#else
  #include <utime.h>
#endif

#if defined (__WIN32__) && !defined(_CSTDIO_)
  #include <cstdio>
#endif

#ifdef __UNIX__
	#include <dirent.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <errno.h>
  #include <sys/vfs.h>
#else
  #include <sys\stat.h>
  #include <ctype.h>
  #include <direct.h>
  #include <errno.h>
  #ifdef __WIN32__
    #include <windows.h>
  #endif
#endif

namespace gdshare
{

#define SEP_PATHDOS     '\\'
#define SEP_PATHUNIX    '/'
#ifdef __UNIX__
  #define SEP_PATH      SEP_PATHUNIX
#else
  #define SEP_PATH      SEP_PATHDOS
#endif
#define SEP_EXT         '.'
#define SEP_DSK         ':'

#ifndef _MAX_PATH
  #define _MAX_PATH 260
#endif

//=============================================================================
// Free functions
//=============================================================================

/// Check for a path (pszPath peut etre termine ou non par un /)
///
/// @param pcszPath Path to check, can be ended by a '/'
///
bool PathExists(pcsz pcszPath);

/// Check for a file
///
/// @param pcszPath File to check
bool FileExists(pcsz pcszFullName);

/// Get access mode from a string in the form "rwxrwxrwx"
///
/// @param strAccess Input string
mode_t AccessFromString(const String &strAccess);


/// Make symbolic link
///
/// @param strFileName opbject to link
int MakeLink(const String &strLink, const String &strTarget);

//=============================================================================
///
/// Exceptions
///
//=============================================================================
class FileException : public ExceptionBase
{
protected:
  const char *ErrorTitle()
  {
    return "File Exception: ";
  }

public:
  FileException(const char *pcszError=NULL, const char *pcszReason=NULL, const char *pcszMethod=NULL):
  ExceptionBase(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// Exceptions
///
//=============================================================================
class IOException : public FileException
{
public:
  IOException(const char *pcszError=NULL, const char *pcszReason=NULL, const char *pcszMethod=NULL):
  FileException(pcszError, pcszReason, pcszMethod)
  {  }
};

//=============================================================================
///
/// creating, accessing file/folder failure
///
//=============================================================================
class FileFailureException : public FileException
{
public:
  FileFailureException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// Bad path specification
///
//=============================================================================
class BadPathException : public FileException
{
public:
  BadPathException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// Current conditions doesn't allow to perform the action
///
//=============================================================================
class BadPathConditionException : public FileException
{
public:
  BadPathConditionException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// Path or file not found
///
//=============================================================================
class FileNotFoundException : public FileException
{
public:
  FileNotFoundException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// Permission
///
//=============================================================================
class PermissionException : public FileException
{
public:
  PermissionException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
///
/// bad drive name
///
//=============================================================================
class BadDriveException : public FileException
{
public:
  BadDriveException(const char *pcszError, const char *pcszReason, const char *pcszMethod):
  FileException(pcszError, pcszReason, pcszMethod)
  { }
};


//============================================================================
/// Free function
//============================================================================
inline int IsSepPath(char c)
 { return c == SEP_PATHDOS || c == SEP_PATHUNIX; }

//============================================================================
/// Classe FileName
///
/// File name manipulations
//============================================================================
class FileName
{
protected:
  String m_strFile;  /// File name
public:
  
  // File system types
  enum FSType
  {
    // Defaut value for Microsoft partition type
    MS = 0,

    // Microsoft partitions types
    FAT16 = 0x04,
    FAT32 = 0x0B,
    IFS = 0x07,
    
    // Unix file system types
    AFFS = 0xADFF,
    EFS = 0x00414A53,
    EXT = 0x137D,
    EXT2 = 0xEF53,
    HPFS = 0xF995E849,
    ISOFS = 0x9660,
    MSDOS = 0x4d44,
    NFS = 0x6969,
    PROC = 0x9fa0,
    SMB = 0x517B
  };

private:

  void ThrowExceptionFromErrno(const char *pszError, const char *pszMethod) const;

public:
  /// Constructor
  FileName()
  { }
  FileName(const String &strFileName)
  { Set(strFileName); }
  FileName(const String &strPath, const String &strName)
  { Set(strPath, strName); }

  /// Returns true if the filename is a path
  bool IsPathName() const;

  /// Returns true if the file name is a existing path
  bool PathExists() const;

  /// Returns true if file exists
  bool FileExists() const;

  /// Set file name
  /// Convert separators
  void Set(const String &strFullName);
  void Set(const String &strPath, const String &strName);
  void Set(const String &strPath, const String &strName, const  String &strExt);

  void SetFullName(pcsz pszFileName);

  /// Convert separator according to the current operating system
  static void ConvertSeparators(String *pstr);

  /// Return full name
  const String &FullName() const { return m_strFile; }

  /// Return path
  String Path() const;

  /// Return filename without path and extension
  String Name() const;

  /// Return directory name (last path component before file name)
  String DirName() const;

  /// Return filename relative to specified path
  String RelName(const char* pszPath) const;

  /// Return filename without path
  String NameExt() const;

  /// Return extension
  String Ext() const;

  /// Create directory
  void MkDir(mode_t mode = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const;

  /// Return true the file name is a existing link
  bool LinkExists() const;

  /// Make a symbolic link
  void MakeSymLink(const String &strTarget, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const;

  /// Delete file
  void Delete();

  /// Remove directory
  void Rmdir(bool bRecursive=false, bool bContentOnly=false);

  /// Copy a directory and its whole content inside the destination dir
  /// if bCreateDir is false the destination directory must already exists
  void DirCopy(const String &strDest, bool bCreateDir=false, mode_t modeDir = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1);

  /// Return size in octets
  ulong Size() const;

  /// Return size in octets: 64bits version for files bigger than 2GB
  int64 Size64() const;
  
  /// Rename the file
  void Rename(const String &strNewName);
  
  /// Copy the file to the specified destination with given 
  void Copy(const String &strDest, bool bKeepMetaData=false);
  
  /// Move file
  void Move(const String &strDest);

  // Returns last modification date & time
  void ModTime(Time *pTm, bool bLocalTime=false) const;

  // Sets last modification date & time
  void SetModTime(const Time& tm) const;

  /// Change file rights
  void Chmod(mode_t mode);

  /// Recursively change rights for directory and files
  void RecursiveChmod(mode_t modeFile, mode_t modeDir, bool bCurrentLevel=true);

  /// Recursively change directory rights
  void RecursiveChmodDir(mode_t mode);

  /// Recursively change file rights
  void RecursiveChmodFile(mode_t mode);

  /// Change file owner
  void Chown(uid_t uid, gid_t gid = (uid_t)-1);

  /// Recursively change file owner
  void RecursiveChown(uid_t uid, gid_t gid = (uid_t)-1);

  // Returns filesystem type
  FSType FileSystemType() const;

  // Returns filesystem type
  fsid_t FileSystemId() const;
};

///===========================================================================
/// Classe File
///
/// Name for temporary file
///===========================================================================
class File: public FileName
{
  public:
  File()
  { }
  File(const String &strFileName)
  { Set(strFileName); }
  File(const String &strPath, const String &strName)
  { Set(strPath, strName); }

  // Load file to the given MemBuf object
  void Load(MemBuf *pMemBuf);

  // Load file to the given String
  void Load(String *pString);

  // Save file from the given String
  void Save(const String &strContent);
};


///===========================================================================
/// Classe TempFileName
///
/// Name for temporary file
///===========================================================================
class TempFileName : public FileName
{
private:
  static long s_lLastNumber;

  // Generate file name
  String GenerateRandomName();

public:
  /// Constructors
  TempFileName();
  TempFileName(const String &strPath);
};  

///===========================================================================
/// Classe FileEnum
///
/// Enumerate all file of a given path
///===========================================================================
class FileEnum : public FileName
{
public:
  enum EEnumMode
  {
     ENUM_FILE = 1,
     ENUM_DIR = 2,
     ENUM_ALL = 3
  };

protected: 

  EEnumMode m_eMode;  // Find mode (FILE or DIR)

#ifdef __UNIX__
  DIR *m_dirDir;
#else
  #ifdef __WIN32__
    void* m_hFind;
    /// WIN32_FIND_DATA pointer
    void* m_pfindData;
    /// Indicate if a error occured since last find action
    int m_bErrorOnLastFind;
  #else
    struct find_t mscDir;
  #endif
  bool m_bFirst;
#endif

  /// Path
  String m_strPath;    

public:
  ///-------------------------------------------------------------------------
  /// @group Constructeur/Destructeur
  ///-------------------------------------------------------------------------

  FileEnum(const String &strPath, EEnumMode eMode=ENUM_FILE);
  ~FileEnum();

  ///-------------------------------------------------------------------------
  /// @group Méthodes standards
  ///-------------------------------------------------------------------------

  /// Initializing search mask
  /// 
  /// @param strPath Path to enumerate
  /// @param eMode enum mode: ENUM_FILE => files, ENUM_DIR => directories, ENUM_ALL => files & directories
  void Init(const String &strPath, EEnumMode eMode=ENUM_FILE);

  /// Find next file
  /// @param pointer to a string to put the file name
  /// @return true if a file has been found, false if not
  bool Find();

  // Ferme enumeration
  void Close();
};


}

#endif
