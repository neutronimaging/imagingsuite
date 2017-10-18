//*****************************************************************************
// Synchrotron SOLEIL
//
// Some useful definitions and shortcuts
//
// Creation : 18/02/2005
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

#ifndef __BASE_H__
#define __BASE_H__

#ifdef WIN32
  #define __WIN32__
  #pragma warning(disable:4786)
#elif defined(__linux__)
  #define __UNIX__
  #define __LINUX__
  #define __WANT_STRICMP__  // undefined function
#endif

#include <iostream>

// STL headers
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <set>

#ifdef __WANT_TREE_CLASS__
  // include stl-like 'tree' class from Kasper Peeters (k.peeters@damtp.cam.ac.uk)
  // (http://www.damtp.cam.ac.uk/user/kp229/index.html)
  #ifdef  __WIN32__
    #if _MSC_VER < 1300 // 1200 => vc 6.0
      #include "tree_msvc.h"
    #else
      #include "tree.h"
    #endif
  #else
    #include "tree.h"
  #endif
#endif

#if defined(__WIN32__) && _MSC_VER > 1200
    // Deprecated POSX definitions with VC8+
    #define stricmp   _stricmp
    #define strnicmp  _strnicmp
    #define mkdir     _mkdir
#endif

#include <sys/types.h>

#ifdef __WIN32__
  // Fake definitions
  typedef int mode_t;
  typedef long uid_t;
  typedef long gid_t;
  typedef int fsid_t;
#endif

// Cast a string object to a const char *
#define PSZ(s) (s).c_str()

#if defined(__UNIX__) || (defined(__WIN32__) && _MSC_VER > 1200)
  // Variadic macro supported starting in C99
  #define PSZ_FMT(...) StrFormat(__VA_ARGS__).c_str()
#endif

// Usefull macro
#define MAKEBOOL(a) (a)?true:false

#ifdef __WANT_STRICMP__
  int stricmp(const char *s1, const char *s2);
  int strnicmp(const char *s1, const char *s2, int maxlen);
#endif

#ifdef _DEBUG
  void PrintfDebug(const char *pcszFormat, ...);
  void OutputDebug(const char *pcszString);
  void checkmem();
#endif

// To ensure backward compatibility of the name space
#define soleil gdshare

using namespace std;

namespace gdshare
{

//===========================================================================
// Usual types
//===========================================================================

// Mapping 64 bit integers
#ifdef _MSC_VER
  #ifdef __WIN32__
    typedef __int64 int64;
  #endif
#elif defined (__LINUX__)
  typedef long long int64;
#endif

/// Other types
typedef unsigned char  uint8;
typedef unsigned char  byte;
typedef unsigned char  uchar;
typedef const char *   pcsz;
typedef char *         psz;

#ifdef __WIN32__
  typedef unsigned short ushort;
  typedef unsigned long  ulong;
  typedef unsigned int   uint;
#endif

/// Make int64 from the long integers pair [highpart, lowpart]
inline int64 int64FromHLPair(long lHigh, unsigned long ulLow) 
{ 
  int64 i64 = 1;
  i64 = i64 << 32;
  i64 = i64 * lHigh + ulLow;
  return i64;
}

// Generic pointer to obj
typedef void * obj_ptr;

// Useful values for interpret 'ExtractToken' family methods
#define EXTRACT_TOKEN_FOUND 1
#define EXTRACT_TOKEN_NOT_FOUND 2
#define EXTRACT_TOKEN_EMPTY_SOURCE 0

//=============================================================================
/// free functions
//=============================================================================

/// Returns true if process is running as root
bool IsRoot();

//=============================================================================
/// Extended string class
///
//=============================================================================
class String : public string
{
public:

  /// ExtractToken methods result
  enum ExtractRes
  {
    EMPTY_STRING=0,
    SEP_FOUND,
    SEP_NOT_FOUND
  };
	
  //@{ Constructors
  String() : string()
  {}

  String(const char *psz)
  {
    if( NULL == psz )
      erase();
    else
      append(psz);
  }

  String(const char *psz, int iSize)
  {
    if( NULL == psz )
      erase();
    else
      append(psz, iSize);
  }

  String(const String &str) : string(str)
  {}

  String(const string &str) : string(str)
  {}
  //@}

  /// Compare strings
  bool IsEquals(const String &str) const;

  /// Compare string in a no case sensitive way
  bool IsEqualsNoCase(const String &str) const;

  /// Test first character
  bool StartWith(char c) const;
  /// Test first characters with other string
  bool StartWith(pcsz pcszStart, bool bNoCase=false) const;
  /// Test last character
  bool EndWith(char c) const;
  /// Test last character
  bool EndWith(pcsz pcszEnd, bool bNoCase=false) const;

  /// @name Extracting token from a string
  ///
  /// @param pstrSource The string to extract from
  /// @param c Separator
  /// @param pstrToken string receiving the extracted token
  /// @return
  /// 0 : nothing extracted
  /// 1 : string extracted and separator found
  /// 2 : string extracted and separator not found
  ///

  //@{ Extraction methods
  /// Search token from left to right
  int ExtractToken(char c, String *pstrToken);
  /// Search token from right to left
  int ExtractTokenRight(char c, String *pstrToken);
  /// Search enclosed token from left to right
  int ExtractToken(char cLeft, char cRight, String *pstrToken);
  /// Search enclosed token from right to left
  int ExtractTokenRight(char cLeft, char cRight, String *pstrToken);
  //@}

  /// Remove characters that enclose string: quotes, paranthesis, etc...
  /// ex: RemoveEnclosure("'", "'") -> removes quotes in a string like 'string'
  /// ex: RemoveEnclosure("([", ")]") -> removes paranthesis in a string like (string) or [string]
  ///                                    but not in string like (string]
  /// @param pcszLeft list of possible left enclosure chars
  /// @param pcszRight list of possible right enclosure chars
  /// @return true if enclosure was removed
  bool RemoveEnclosure(pcsz pszLeft, pcsz pszRight);
  bool RemoveEnclosure(char cLeft, char cRight);

  /// Match string with mask containing '*' and '?' jokers
  bool Match(pcsz pszMask) const;
  bool Match(const String &strMask) const;

  /// Remove white space and begining and end of string
  void Trim();

  /// Build a string with format
  int Printf(pcsz pszFormat, ...);

  /// Split string
  ///
  /// @param c Separator
  /// @param pvecstr pointer to a vector of strings
  ///
  void Split(char c, vector<String> *pvecstr);
  void Split(char c, vector<String> *pvecstr) const;
  void Split(char c, String *pstrLeft, String *pstrRight, bool bPreserve=false);

  /// Join strings from string vector
  ///
  /// @param cSep Items separator
  ///
  void Join(const vector<String> &vecStr, char cSep=',');

  /// Remove item in a string like "item1,item2,item3,..."
  ///
  /// @param cSep Items separator
  ///
  /// @return true if the item was found, otherwise false
  bool RemoveItem(const String &strItem, char cSep=',');

  /// Convert characters to lowercase
  void ToLower();

  /// Convert string to uppercase
  void ToUpper();

  /// Find and replace
  ///
  /// @param pszSrc Substring to replace
  /// @param pszDst Substitution string
  ///
  void Replace(pcsz pszSrc, pcsz pszDst);

  /// Find and replace one character
  ///
  /// @param cSrc Character to replace
  /// @param cDst Substitution Character
  ///
  void Replace(char cSrc, char cDst);

  /// Crypting
  ///
  /// @param pszXorKey XOR key
  /// @param pszRotKey ROT key
  ///
  void Crypt(const char* pszXorKey=NULL, const char* pszRotKey=NULL);

  /// Decrypting
  ///
  /// @param pszXorKey XOR key
  /// @param pszRotKey ROT key
  ///
  void Decrypt(const char* pszXorKey=NULL, const char* pszRotKey=NULL);

  /// Returns hash code
  ///
  ulong Hash() const;
};

//## Will be removed when all String object will be replaced with String
typedef String CString;

// Empty string - useful when need a const string &
extern const String g_strEmpty;

// Simple scratch buffer (not thread safe)
extern const int g_iScratchLen;
extern char g_acScratchBuf[];

//==============================================================================
/// Error handling
///
//==============================================================================
class Error
{
public:
  enum ELevel
  {
    _WARNING = 0,
    _ERROR = 1,
    _CRITICAL = 2,
    _ALERT = 3,
    _EMERGENCY = 4,
    _LEVEL_COUNT  // Not a error level, just the total number of levels
  };

private:
  String m_strReason;  /**< Error reason (e.g. "OUT_OF_MEMORY") */
  String m_strDesc;    /**< Error description */
  String m_strOrigin;  /**< Origin of error */
  ELevel  m_eLevel;     /**< Error level */

public:
  Error(const String &strReason, const String &strDesc, const String &strOrigin, ELevel eLevel = _ERROR)
  : m_strReason(strReason), m_strDesc(strDesc), m_strOrigin(strOrigin), m_eLevel(eLevel)
  {}

  const String &Reason() const { return m_strReason; }
  const String &Desc() const { return m_strDesc; }
  const String &Origin() const { return m_strOrigin; }
  const ELevel Level() const { return m_eLevel; }
  String ToString() const;
};

// For deprecated class name CError
typedef Error CError;

//==============================================================================
/// Error stack : singleton object
///
//==============================================================================
class ErrorStack 
{
friend class IgnoreErrors;
private:
  static ErrorStack *ms_pTheInstance;
  static Error s_EmptyError;
  std::stack<Error> m_stkErrors;
  bool   m_bLogError;
  bool   m_bIgnoreError;

  /// Give back a real object
  static ErrorStack *Instance();
  

public:
  static void Push(const String &strReason, const String &strDesc, 
                   const String &strOrigin, CError::ELevel eLevel = CError::_ERROR);

  /// Activate logging when a error is pushed
  static void SetLogError(bool bLog = true);

  /// Ignore errors
  static void SetIgnoreErrors(bool bIgnore = true);

  /// Return top error
  static CError &Top();

  /// Pop top error
  static void Pop();

  /// Returns true if stack is empty
  static bool IsEmpty();

  /// Clear stack
  static void Clear();

  /// Returns stack size
  static int Size();

  /// Returns all error msg in a single string
  static String ToString(char cSep='\n');
};
// For deprecated class name CError
#define CErrorStack ErrorStack

//==============================================================================
/// Instance this object to suspend error collecting from the errors stack
///
//==============================================================================
class IgnoreErrors
{
  IgnoreErrors()  { CErrorStack::SetIgnoreErrors(); }
  ~IgnoreErrors() { CErrorStack::SetIgnoreErrors(false); }
};

//==============================================================================
/// Basic exception class
///
//==============================================================================
class ExceptionBase
{
protected:
  String m_strError;   // Error text
  String m_strReason;  // Reason
  String m_strMethod;  // Method name

  virtual const char *ErrorTitle();

public:
  ExceptionBase(const char *pcszError, const char *pcszReason, const char *pcszMethod);

  virtual ~ExceptionBase();

  /// Prints error message on console
  void PrintMessage();

  /// Copies error message in a pre-allocated string buffer
  ///
  /// @param pBuf Buffer (allocated by caller) to copy the message in
  /// @param iLen Buffer length
  ///
  String Message();

  const String &Error() const { return m_strError; }
  const String &Reason() const { return m_strReason; }
  const String &Method() const { return m_strMethod; }
};

//==============================================================================
/// Simple exception class
///
//==============================================================================
class Exception : public ExceptionBase
{
protected:
  const char *ErrorTitle()
  {
    return "A exception occured";
  }
public:
  Exception(const char *pcszError=NULL, const char *pcszReason=NULL, const char *pcszMethod=NULL):
  ExceptionBase(pcszError, pcszReason, pcszMethod)
  { }
};

//==============================================================================
/// Null pointer exception
///
//==============================================================================
class NullPointerException : public ExceptionBase
{
protected:
  const char *ErrorTitle()
  {
    return "Null pointer exception";
  }
public:
  NullPointerException(const char *pcszReason, const char *pcszMethod):
  ExceptionBase("NULL_POINTER", pcszReason, pcszMethod)
  { }
};

//==============================================================================
/// No data exception
///
//==============================================================================
class NoDataException : public ExceptionBase
{
protected:
  const char *ErrorTitle()
  {
    return "No data exception";
  }
public:
  NoDataException(const char *pcszReason, const char *pcszMethod):
  ExceptionBase("NO_DATA", pcszReason, pcszMethod)
  { }
};

//===========================================================================
/// A few string algorithms
//===========================================================================

/// Compare strings
bool IsEquals(const String &str1, const String &str2);

/// Compare string in a no case sensitive way
bool IsEqualsNoCase(const String &str1, const String &str2);

/// @name Extracting token from a string
///
/// @param pstrSource The string to extract from
/// @param c Separator
/// @param pstrToken string receiving the extracted token
/// @return
/// 0 : nothing extracted
/// 1 : string extracted and separator found
/// 2 : string extracted and separator not found
///

//@{ Token extraction functions
/// Search token from left to right
int ExtractToken(String *pstrSource, char c, String *pstrToken);
/// Search token from right to left
int ExtractTokenRight(String *pstrSource, char c, String *pstrToken);
/// Search enclosed token from left to right
int ExtractToken(String *pstrSource, char cLeft, char cRight, String *pstrToken);
/// Search enclosed token from right to left
int ExtractTokenRight(String *pstrSource, char cLeft, char cRight, String *pstrToken);
//@}

/// Remove white space and begining and end of string
void Trim(String *pstr);

/// Test first character
bool StartWith(const String &str, char c);
/// Test first characters with other string
bool StartWith(const String &str, pcsz pcszStart, bool bNoCase=false);
/// Test last character
bool EndWith(const String &str, char c);

// Build a string with format
String StrFormat(pcsz pszFormat, ...);

//=============================================================================
/// Simple strings dictionnary
//=============================================================================
class StringDict : public std::map<String, String>
{
public:
  /// Export dictionary items to a string
  ///
  /// @param pstrDest destination string
  ///        string format is : "name,<value size>=value|..."
  ///
  void Export(String *pstrDest);

  /// Build a dictionnary from a string
  ///
  /// @param pstrSrc Source string
  ///
  void Import(const String &strSrc);

  /// Complete StringDict entry
  ///
  /// @param strKey Entry name
  /// @param strValue Value to add
  /// @param cSep Values separator
  ///
  void ConcatValue(const String &strKey, const String &strValue, char cSep='|');

  /// Return the value associated with the given key
  ///
  /// @param strKey Entry name
  const String &Value(const String &strKey) const;

  /// Return true if the given key exists in the dictionnary
  ///
  /// @param strKey Key name
  ///
  bool HasKey(const String &strKey) const;
};

//=============================================================================
/// Command line option
//=============================================================================
struct SCommandLineOpt
{
  char   cShortName;
  String strLongName;
  bool   bMandatory;
  String strDesc;
  String strValue;
};
typedef std::vector<SCommandLineOpt> vecOpts;

//=============================================================================
/// Command line argument
//=============================================================================
struct SCommandLineArg
{
  String strDesc;
  bool   bMandatory;
  bool   bSingle;
};

typedef std::vector<SCommandLineArg> vecArgs;

//=============================================================================
/// Command line argument
//=============================================================================
class CommandLine
{
private:
  static CommandLine *ms_pTheInstance;
  static CommandLine *Instance();

  StringDict m_dictOptValues;     // Current options values
  std::vector<String> m_vecArgs;  // Simples arguments (not options)
  vecOpts    m_vecOptDefs;        // Valid options list
  vecArgs    m_vecArgDefs;        // Valid arguments list
  StringDict m_dictOpts;          // Argument dictionnary
  String     m_strCmdName;        // Command name
  String     m_strCmdVersion;     // Version

  /// Constructor
  CommandLine() {}

  /// Find option def from short form
  SCommandLineOpt *FindShortOpt(const char cOpt);

  /// Find option definition from long form
  SCommandLineOpt *FindLongOpt(const String &strOpt);

  /// Display "bad option" error message
  ///
  /// @return always false
  static bool BadOption(const String &strOpt);

  /// Show usage
  ///
  /// @return false
  static bool ShowUsage(std::ostream &os = std::cout);

public:
  /// Set command name (displayed in ShowUsage() )
  ///
  static void SetCmdNameVersion(const String &strName, const String &strVersion);

  /// Add a option
  static void AddOpt(char cShortName, pcsz pszLongName, pcsz pszValue, pcsz pszDesc);

  /// Add a argument
  static void AddArg(pcsz pszDesc, bool bSingle=true);

  /// Initializing with command line arguments
  ///
  /// @param iArgc, ppszArgv main() arguments
  /// @return true if arguments are valid, otherwise false
  ///
  static bool Read(int iArgc, char **ppszArgv);

  /// Showing Usage
  ///
  /// @param strAppInfo Additionnal information about the application
  ///
  static void ShowUsage(const String &strAppInfo);

  /// Retreiving options, arguments and theirs values
  static bool IsOption(const String &strArg);
  static String OptionValue(const String &strOpt);
  static int ArgCount();
  static String Arg(int i);
};

// For deprecated class name CCommandLine
typedef CommandLine CCommandLine;

//=============================================================================
/// Application object
//=============================================================================
class App
{
private:
  String m_strName;

protected:

  /// Constructor
  App(const String &strName);

public:

  virtual ~App() {}

  /// Application entry point
  virtual int OnRun()=0;
  
  /// Make the application a system deamon
  void Deamonize();
};

//=============================================================================
/// Logging messages
///
/// TODO : sending syslog messages
//=============================================================================

//=============================================================================
///
/// LogException
///
//=============================================================================
class LogException : public ExceptionBase
{
protected:
  const char *ErrorTitle()
  {
    return "Exception in logging system";
  }
public:
  LogException(const char *pcszError=NULL, const char *pcszReason=NULL, const char *pcszMethod=NULL):
  ExceptionBase(pcszError, pcszReason, pcszMethod)
  { }
};

//=============================================================================
/// ELogLevel
/// Define severity types using syslog levels definitions
//=============================================================================
enum ELogLevel
{
  LOG_VERBOSE = 0,
  LOG_INFO,
  LOG_NOTICE,
  LOG_WARNING,
  LOG_ERROR,
  LOG_CRITICAL,
  LOG_ALERT,
  LOG_EMERGENCY,
  LOG_LEVEL_COUNT  // Not a log level, just the total number of levels
};

//=============================================================================
/// ILogTarget log target interface 
///
//=============================================================================
class ILogTarget
{
public:
  /// Log message
  virtual void Log(ELogLevel eLevel, pcsz pszType, const String &strMsg)=0;
};

/// Log target stack, for logging redirection
typedef std::stack<class ILogTarget *> LogTargetStack;

//=============================================================================
/// default log handler : print log on console using clog stream
///
//=============================================================================
class DefaultLogHandler: public ILogTarget
{
public:
  /// Log message
  virtual void Log(ELogLevel eLevel, pcsz pszType, const String &strMsg);
};

/// Log forwarding function type declaration
typedef void (*pfn_log_fwd)(int iLevel, const char *pszType, const char *pszMsg);

// For deprecated class name CDefaultLogHandler
typedef DefaultLogHandler CDefaultLogHandler;

//=============================================================================
/// log forwarding object : forward log to a specified function
///
//=============================================================================
class LogForward: public ILogTarget
{
private:
	/// Function to forward log to
  pfn_log_fwd m_pfn_log_fwd;

public:
	/// Constructor
  LogForward(pfn_log_fwd pfn_log_fwd);

  /// ILogTarget
  virtual void Log(ELogLevel eLevel, pcsz pszType, const String &strMsg);
};

// For deprecated class name CLogForward
typedef LogForward CLogForward;

//=============================================================================
/// log utility class
///
//=============================================================================
class LogManager
{
friend class LogCatcher;
private:
  static LogManager *ms_pTheInstance;
  static LogManager *Instance();
  DefaultLogHandler  m_defLogHandler;

  /// Log target
  LogTargetStack      m_stkCatchLogTarget;

  /// Min severity level of logged messages
  int         m_iMinLevel;

  /// Logged messages types
  std::set<String>  m_setTypes;

  /// Add a new log target to the stack
  static void PushLogTarget(ILogTarget *pLogTarget);

  /// Remove top log target
  static void PopLogTarget();

public:
  /// Constructor
  LogManager();

  /// Initialize LogHandler.
  ///
  /// @param eMinLevel Min severity level
  /// @param strFilter List (separator = '|') of source types used for message filtering
  ///
  static void Init(int iMinLevel, const String &strFilter=g_strEmpty);

  /// Log message
  static void Log(ELogLevel eLevel, pcsz pszType, const String &strMsg);

  /// Min log level take in account
  static int MinLevel() { return Instance()->m_iMinLevel; }

  /// Return current log target
  static ILogTarget *CurrentLogTarget();
};

// For deprecated class name CLogManager
typedef LogManager CLogManager;

//=============================================================================
/// Class used to define object that catch log, during CLogCatcher live time
///
//=============================================================================
class LogCatcher
{
public:
  /// Constructor : Push log target in ClogHandler stack
  LogCatcher(ILogTarget *pLogTarget);

  /// Destructor : remove top log target from the stack
  ~LogCatcher();
};

// For deprecated class name CLogManager
typedef LogCatcher CLogCatcher;

//=============================================================================
///
/// Log functions
///
//=============================================================================

void LogVerbose(pcsz pszType, pcsz pszFormat, ...);
void LogInfo(pcsz pszType, pcsz pszFormat, ...);
void LogNotice(pcsz pszType, pcsz pszFormat, ...);
void LogWarning(pcsz pszType, pcsz pszFormat, ...);
void LogError(pcsz pszType, pcsz pszFormat, ...);
void LogCritical(pcsz pszType, pcsz pszFormat, ...);
void LogAlert(pcsz pszType, pcsz pszFormat, ...);
void LogEmergency(pcsz pszType, pcsz pszFormat, ...);

//=============================================================================
/// Free functions
///
//=============================================================================

/// Reading a environnement variable
///
/// @param pszVar Variable name
/// @param pstrValue string to store result to
/// @param bBackground if true execute shell command in background (i.e. don't wait for a return code)
/// @param uid used for shell execution (don't have any effect under Windows OS)
/// @param gid used for shell execution (don't have any effect under Windows OS)
/// @param bThrow if true may throw a exception in case if failure
/// @param pszDef default value
/// @return true in case of a defined variable, otherwire false
///
bool GetEnv(const char *pszVar, String *pstrValue, const char *pszDef = NULL);

/// Execute command line
int Exec(const char* pszCmdLine, const char *pszDefDir = NULL,
         int bBackground = true, bool bThrow = true, ulong* pulReturnCode = NULL);
int ExecAs(const char* pszCmdLine, const char *pszDefDir = NULL,
         int bBackground = true, bool bThrow = true, ulong* pulReturnCode = NULL, uid_t uid = -1, gid_t gid = -1);

//===========================================================================
/// A simple template evaluator
///
/// This class is a interface so it can't be directly instancied
/// @see TemplateString
//===========================================================================
class IVariableEvaluator
{
protected:
  /// Constructor
  IVariableEvaluator() { }

public:
  /// Attempts to evaluate a variable 
  ///
  /// @param pstrVar Variable to evaluate
  /// @return true if template has been evaluated, or false
  ///
  virtual bool Evaluate(String *pstrVar)=0;
};

//===========================================================================
/// Template processor
///
/// A TemplateString is a string that contains items to be replace with
/// there real value
/// example : in the string 'date is $(date)', '$(date)' will be replaced
/// with the current date when processed
//===========================================================================
class TemplateProcessor
{
public:
  enum NotFoundReplacement
  {
    EMPTY_STRING,
    VARIABLE_NAME,
    UNCHANGE_STRING
  };

private:
  std::list<IVariableEvaluator *> m_lstEvaluator;
  NotFoundReplacement m_eNotFoundReplacement;
  bool PrivProcess(String *pstrTemplate, bool bRecurse, set<String> &setEvaluatedSymbols);
  bool PrivProcessVar(String *pstrVar, bool bRecurse, bool bDeepEvaluation, set<String> &setEvaluatedSymbols);

public:
  ///
  /// Constructor
  ///
  TemplateProcessor(NotFoundReplacement eNotFoundReplacement = VARIABLE_NAME) : m_eNotFoundReplacement(eNotFoundReplacement) {}

  /// Add evaluator
  ///
  /// @param pEvaluator Evaluator object
  ///
  void AddEvaluator(IVariableEvaluator *pEvaluator);

  /// Remove evaluator
  ///
  /// @param pEvaluator Evaluator object
  ///
  void RemoveEvaluator(IVariableEvaluator *pEvaluator);

  /// Evaluate a variable
  ///
  /// @param pstrVar Variable to evaluate, will contains the result
  /// @return true if evaluation is done, or false
  ///
  bool ProcessVar(String *pstrVar);

  /// Process a template string
  ///
  /// @param strTemplate String to evaluate
  /// @param pstrEvaluated Resulting string
  /// @return true if evaluation is done, or false
  ///
  bool Process(String *pstrTemplate, bool bRecurse=true);
  bool ProcessNoRecursion(String *pstrTemplate);
};

// CTemplateProcessor => deprecated name
typedef TemplateProcessor CTemplateProcessor;

//===========================================================================
/// A template evaluator based of environnement variables
///
//===========================================================================
class EnvVariableEvaluator : public IVariableEvaluator
{
public:
  /// @param pstrVar Variable to evaluate
  /// @return true if evaluation is done, or false
  virtual bool Evaluate(String *pstrVar);
};

// CEnvVariableEvaluator => deprecated name
#define CEnvVariableEvaluator EnvVariableEvaluator

//=============================================================================
///
/// Basic tree node
///
/// note : already obsolete, rather use stl-like 'tree' class
///
//=============================================================================
class BasicTreeNode
{
protected:
  BasicTreeNode  *m_pFirstChildNode;
  BasicTreeNode  *m_pLastChildNode;
  BasicTreeNode  *m_pNextSiblingNode;
  BasicTreeNode  *m_pParentNode;

public:
  /// Constructor
  BasicTreeNode();

  /// Destructor
  virtual ~BasicTreeNode();

  /// Add a node to the tree
  void AddNode(BasicTreeNode *pNode);

  /// Navigation
  BasicTreeNode  *FirstChild() { return m_pFirstChildNode; }
  BasicTreeNode  *NextSibling() { return m_pNextSiblingNode; }
  BasicTreeNode  *Parent() { return m_pParentNode; }
};


//=============================================================================
/// Mix-in class for pointer with use counts. Make your class derive from
/// this mixin to make it support intelligent pointers
//=============================================================================
class MReferencable
{
private:
  /// number of references on this instance
  mutable int m_iUseCount;

protected:
  /// constructor / destructor
  MReferencable();
  virtual ~MReferencable();

public:
  /// Increase the ref count (const since mutable like)
  void AddRef() const;
  /// Decrease the ref count (const since mutable like)
  void Release() const;
  /// Returns the use count
  int UseCount() const { return m_iUseCount; }
};

//=============================================================================
/// The reference pointer itself
/// To be used as
///   RefPtr<CMyClass> ref;
///   ref = new CMyClass;
///   ref->Method();
//=============================================================================
template <class TYPE> class RefPtr
{
private:
  TYPE* m_pRefObject;
  void Free()
  {
    if( m_pRefObject )
    {
      m_pRefObject->Release();
      m_pRefObject = NULL;
    }
  }

public:
  RefPtr(TYPE *pObj = NULL)             
    : m_pRefObject(pObj)
  {
    if( m_pRefObject )
      m_pRefObject->AddRef();
  }

  RefPtr(const RefPtr<TYPE>& ref)
    : m_pRefObject(NULL)

  {
    operator=(ref.m_pRefObject);
  }

  ~RefPtr()                                   
  {  Free(); }                                        
                                                      
  TYPE* operator->()                             
  {                                                   
    return m_pRefObject;                              
  }                                                   
                                                      
  const TYPE* operator->() const                 
  {                                                   
    return (const TYPE*)m_pRefObject;           
  }                                                   
                                                      
  bool operator==(const RefPtr<TYPE>& ref) const     
    { return m_pRefObject == ref.m_pRefObject; }      
                                                      
  /// Comparaison operator that allow to use STL objects to maintain
  /// a reference collection of uniques pointers
  bool operator<(const RefPtr<TYPE>& ref) const     
    { return (int)m_pRefObject < (int)ref.m_pRefObject; }      
                                                      
  RefPtr<TYPE>& operator=(TYPE* pObj)
  {
    if( m_pRefObject != pObj )
    {
      Free();
      m_pRefObject = pObj;
      if( m_pRefObject )
        m_pRefObject->AddRef();
    }
    return *this;
  }
                                                      
  RefPtr<TYPE>& operator=(const RefPtr<TYPE>& ref)
    { return operator=(ref.m_pRefObject); }           
                                                      
  TYPE& operator*()                              
    { return *m_pRefObject; }                         
                                                      
  operator TYPE*() const                        
    { return m_pRefObject; }                          

  TYPE *ObjectPtr() const                        
    { return m_pRefObject; }                          
                                                      
  int IsNull() const                                 
    { return m_pRefObject == NULL; }
                      
  void Clear()                                        
    { Free(); }                                       
};

//=============================================================================
/// Mix-in that add metadata capabilitie to objects
//=============================================================================
class MMetadata
{
private:
  std::map<String, String> m_mapString; // "String" type metadata
  std::map<String, int> m_mapInteger;   // "Integer" type metadata
  std::map<String, double> m_mapDouble; // "Float" type metadata

public:
  /// Add String metadata
  void AddMetadata(const String &strKey, const String &strValue);
  /// Add C-ansi string metadata
  void AddMetadata(const String &strKey, const char *pszValue);
  /// Add integer matedata
  void AddMetadata(const String &strKey, int iValue);
  /// Add float metadata
  void AddMetadata(const String &strKey, double dValue);

  /// Check metadata
  bool HasMetadata(const String &strKey) const;

  /// Get metadata as string
  /// Implicitely convert integer and float metadata as string value
  /// @param  bThrow if true throw a exception in case of no data
  /// @return true if metadata was found, otherwise false (if bThrow == false)
  bool GetMetadata(const String &strKey, String *pstrValue, bool bThrow=true) const;

  /// Get string metadata
  /// @param  bThrow if true throw a exception in case of no data
  /// @return true if metadata was found, otherwise false (if bThrow == false)
  bool GetStringMetadata(const String &strKey, String *pstrValue, bool bThrow=true) const;

  /// Get integer metadata
  /// @param  bThrow if true throw a exception in case of no data
  /// @return true if metadata was found, otherwise false (if bThrow == false)
  bool GetIntegerMetadata(const String &strKey, int *piValue, bool bThrow=true) const;

  /// Get float metadata
  /// @param  bThrow if true throw a exception in case of no data
  /// @return true if metadata was found, otherwise false (if bThrow == false)
  bool GetDoubleMetadata(const String &strKey, double *pdValue, bool bThrow=true) const;
};

} // namespace soleil

#ifdef _THREAD_SUPPORT
	// Threading supported through YAT library
	#define _YAT_THREADS
	#include <yat/threading/Thread.h>
	#include "threadutil.h"
#endif

#endif // __BASE_H__
