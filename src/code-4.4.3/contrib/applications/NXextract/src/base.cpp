//*****************************************************************************
// Synchrotron SOLEIL
//
// Some useful definitions and shortcuts
//
// Creation : 18/02/2005
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
#include "date.h"

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __WIN32__
  #include <windows.h>
  #include <shellapi.h>
  #include <stdio.h>
#elif defined (__LINUX__)
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <pwd.h>
  #include <signal.h>
#endif

using namespace gdshare;

// Empty string
const String gdshare::g_strEmpty = "";

// Simple scratch buffer (not thread safe)
const int gdshare::g_iScratchLen = 262144;
char gdshare::g_acScratchBuf[g_iScratchLen];

// vsnprintf function
#ifdef __WIN32__
  #define VSNPRINTF _vsnprintf_s
#else
  #define VSNPRINTF vsnprintf
#endif

// stricmp function
#ifdef __WANT_STRICMP__
int stricmp(pcsz s1, pcsz s2)
{
  while( *s1 )
  {
    if( *s2 == 0 )
      return 1;

    if( toupper(*s1) < toupper(*s2) )
      return -1;
    if( toupper(*s1) > toupper(*s2) )
      return 1;
    s1++;
    s2++;
}

  if( *s2 == 0 )
    return 0;
  return -1;
}

int strnicmp(pcsz s1, pcsz s2, int maxlen)
{
  int i = 0;
  while( i<maxlen )
  {
    if( *s2 == 0 || *s1 == 0 )
      return 1;

    if( toupper(*s1) < toupper(*s2) )
      return -1;
    if( toupper(*s1) > toupper(*s2) )
      return 1;
    s1++;
    s2++;
    i++;
  }
  return 0;
}
#endif

#ifdef _DEBUG
//---------------------------------------------------------------------------
// Output formatting debug info
//---------------------------------------------------------------------------
  void PrintfDebug(pcsz pszFormat, ...)
  {
#ifdef _THREAD_SUPPORT
	LOCK(&g_acScratchBuf);
#endif
    va_list argptr;
    va_start(argptr, pszFormat);
    VSNPRINTF(g_acScratchBuf, g_iScratchLen, pszFormat, argptr);
    va_end(argptr);
    OutputDebug(g_acScratchBuf);
  }
  void OutputDebug(pcsz pcsz)
  {
    #ifdef __WIN32__
      OutputDebugString(pcsz);
      OutputDebugString("\n");
    #else
      fprintf(stderr, pcsz);
      fprintf(stderr, "\n");
    #endif
  }
#else
  void PrintfDebug(pcsz /*pszFormat*/, ...)
  {
  }
  void OutputDebug(pcsz /*psz*/)
  {
  }
#endif

//----------------------------------------------------------------------------
// IsRoot
//----------------------------------------------------------------------------
bool gdshare::IsRoot()
{
#ifdef __LINUX__
  if( 0 == geteuid() )
    return true;
  return false;
#endif
  return false;
}

//=============================================================================
// Error
//
//=============================================================================
String Error::ToString() const
{
  return StrFormat("%s. Reason: %s. Origin: %s.", PSZ(m_strDesc), PSZ(m_strReason), PSZ(m_strOrigin));
}

//=============================================================================
// ErrorStack
//
//=============================================================================
ErrorStack *ErrorStack::ms_pTheInstance = NULL;
Error ErrorStack::s_EmptyError("OK", "No error", "nowhere");

//----------------------------------------------------------------------------
// ErrorStack::Push
//----------------------------------------------------------------------------
void ErrorStack::Push(const String &strReason, const String &strDesc, 
                  const String &strOrigin, Error::ELevel eLevel)
{
  if( !Instance()->m_bIgnoreError )
  {
    Instance()->m_stkErrors.push(Error(strReason, strDesc, strOrigin, eLevel));
    if( Instance()->m_bLogError )
      LogError("gdshare", "%s. Reason: %s. Origin: %s.", PSZ(strDesc), PSZ(strReason), PSZ(strOrigin));
  }
}

//----------------------------------------------------------------------------
// ErrorStack::SetLogError
//----------------------------------------------------------------------------
void ErrorStack::SetLogError(bool bLog)
{
  Instance()->m_bLogError = bLog;
}

//----------------------------------------------------------------------------
// ErrorStack::SetIgnoreErrors
//----------------------------------------------------------------------------
void ErrorStack::SetIgnoreErrors(bool bIgnore)
{
  Instance()->m_bIgnoreError = bIgnore;
}

//----------------------------------------------------------------------------
// ErrorStack::Instance
//----------------------------------------------------------------------------
ErrorStack *ErrorStack::Instance()
{
  if( NULL == ms_pTheInstance )
  {
    ms_pTheInstance = new ErrorStack;
    ms_pTheInstance->m_bIgnoreError = false;
    ms_pTheInstance->m_bLogError = true;
  }
  return ms_pTheInstance;
}

//----------------------------------------------------------------------------
// ErrorStack::Top
//----------------------------------------------------------------------------
Error &ErrorStack::Top()
{
  if( false == Instance()->IsEmpty() )
    return Instance()->m_stkErrors.top();
  return s_EmptyError;
}

//----------------------------------------------------------------------------
// ErrorStack::Pop
//----------------------------------------------------------------------------
void ErrorStack::Pop()
{
  if( false == Instance()->IsEmpty() )
    Instance()->m_stkErrors.pop();
}

//----------------------------------------------------------------------------
// ErrorStack::IsEmpty
//----------------------------------------------------------------------------
bool ErrorStack::IsEmpty()
{
  return Instance()->m_stkErrors.empty();
}

//----------------------------------------------------------------------------
// ErrorStack::Clear
//----------------------------------------------------------------------------
void ErrorStack::Clear()
{
  while( !IsEmpty() )
    Instance()->m_stkErrors.pop();
}

//----------------------------------------------------------------------------
// ErrorStack::Size
//----------------------------------------------------------------------------
int ErrorStack::Size()
{
  return Instance()->m_stkErrors.size();
}

//----------------------------------------------------------------------------
// ErrorStack::Size
//----------------------------------------------------------------------------
String ErrorStack::ToString(char cSep)
{
  String strStack;
  while( !IsEmpty() )
  {
    strStack = StrFormat("%s%s%c", PSZ(strStack), PSZ(Top().ToString()), cSep);
    Pop();
  }
  return strStack;
}

//=============================================================================
// Basic exception class
//
//=============================================================================
//---------------------------------------------------------------------------
// ExceptionBase::ExceptionBase
//---------------------------------------------------------------------------
ExceptionBase::ExceptionBase(const char *pcszError, const char *pcszReason,
                             const char *pcszMethod)
{
  if( pcszError )
    m_strError = pcszError;
  if( pcszReason )
    m_strReason = pcszReason;
  if( pcszMethod )
  m_strMethod = pcszMethod;

  // put error on top of stack
  gdshare::ErrorStack::Push(pcszReason, pcszError, pcszMethod);
}

//---------------------------------------------------------------------------
// ExceptionBase::~ExceptionBase
//---------------------------------------------------------------------------
ExceptionBase::~ExceptionBase()
{

}

//---------------------------------------------------------------------------
// ExceptionBase::ErrorTitle
//---------------------------------------------------------------------------
const char *ExceptionBase::ErrorTitle()
{
  return "A exception occured";
}

//---------------------------------------------------------------------------
// ExceptionBase::PrintMessage
//---------------------------------------------------------------------------
void ExceptionBase::PrintMessage()
{
  std::cout << ErrorTitle() << std::endl
            << m_strError  << std::endl
            << "Reason: " << m_strReason << std::endl
            << "Origin: " << m_strMethod  << '.' << std::endl;
}

//---------------------------------------------------------------------------
// ExceptionBase::Message
//---------------------------------------------------------------------------
String ExceptionBase::Message()
{
  String strMsg;
  strMsg  = ErrorTitle() + string(": ") + m_strError 
          + ". Cause: " + m_strReason
          + ". In: " + m_strMethod
          + ".";
  return strMsg;
}

//---------------------------------------------------------------------------
// ExtractToken
//---------------------------------------------------------------------------
int gdshare::ExtractToken(String *pstrSource, char c, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( pstrSource == pstrToken )
    return 0;

  int iSrcLength = pstrSource->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for separator
  int iPos = pstrSource->find_first_of(c);
  if( iPos < 0 )
  {
    // Not found
    *pstrToken = *pstrSource;
    *pstrSource = g_strEmpty;
    return 2;
  }

  // Separator found
  *pstrToken = pstrSource->substr(0, iPos);
  pstrSource->erase(0, iPos+1);
  return 1;
}

//---------------------------------------------------------------------------
// ExtractTokenRight
//---------------------------------------------------------------------------
int gdshare::ExtractTokenRight(String *pstrSource, char c, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( pstrSource == pstrToken )
    return 0;

  int iSrcLength = pstrSource->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for separator
  int iPos = pstrSource->find_last_of(c);
  if( iPos < 0 )
  {
    // Not found
    *pstrToken = *pstrSource;
    *pstrSource = g_strEmpty;
    return 2;
  }

  // Separator found
  *pstrToken = pstrSource->substr(iPos+1);
  pstrSource->erase(iPos);
  return 1;
}

//---------------------------------------------------------------------------
// ExtractToken
//---------------------------------------------------------------------------
int gdshare::ExtractToken(String *pstrSource, char cLeft, char cRight, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( pstrSource == pstrToken )
    return 0;

  int iSrcLength = pstrSource->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for enclosing characters
  int iLeftPos = pstrSource->find(cLeft);
  int iRightPos = pstrSource->find(cRight, iLeftPos + 1);
  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    *pstrToken = g_strEmpty;
    return 2;
  }

  // Enclosing characters found
  *pstrToken = pstrSource->substr(iLeftPos + 1, iRightPos - iLeftPos - 1);
  pstrSource->erase(0, iRightPos + 1);
  return 1;
}

//---------------------------------------------------------------------------
// ExtractTokenRight
//---------------------------------------------------------------------------
int gdshare::ExtractTokenRight(String *pstrSource, char cLeft, char cRight, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( pstrSource == pstrToken )
    return 0;

  int iSrcLength = pstrSource->length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for enclosing characters
  int iRightPos = pstrSource->rfind(cRight);
  int iLeftPos = pstrSource->rfind(cLeft, iRightPos - 1);
  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    *pstrToken = g_strEmpty;
    return 2;
  }

  // Enclosing characters found
  *pstrToken = pstrSource->substr(iLeftPos+1, iRightPos - iLeftPos - 1);
  pstrSource->erase(iLeftPos);
  return 1;
}

//---------------------------------------------------------------------------
// IsEquals
//---------------------------------------------------------------------------
bool gdshare::IsEquals(const String &str1, const String &str2)
{
  return MAKEBOOL(str1 == str2);
}

//---------------------------------------------------------------------------
// IsEqualsNoCase
//---------------------------------------------------------------------------
bool gdshare::IsEqualsNoCase(const String &str1, const String &str2)
{
  return MAKEBOOL(!stricmp(str1.c_str(), str2.c_str()));
}

//---------------------------------------------------------------------------
// StartWith - first character only
//---------------------------------------------------------------------------
bool gdshare::StartWith(const String &str, char c)
{
  if( str.size() == 0 )
    return false;

  if( c == str[0] )
    return true;

  return false;
}

//---------------------------------------------------------------------------
// StartWith - string comparaison
//---------------------------------------------------------------------------
bool gdshare::StartWith(const String &str, pcsz pcszStart, bool bNoCase)
{
  if( str.size() < strlen(pcszStart) )
    return false;

  if( bNoCase )
  {
    return MAKEBOOL(!strnicmp(PSZ(str), pcszStart, strlen(pcszStart)));
  }
  else
  {
    return MAKEBOOL(!strncmp(PSZ(str), pcszStart, strlen(pcszStart)));
  }
}

//---------------------------------------------------------------------------
// EndWith - last character only
//---------------------------------------------------------------------------
bool gdshare::EndWith(const String &str, char c)
{
  if( str.size() == 0 )
    return false;

  if( c == str[str.size()-1] )
    return true;

  return false;
}


//---------------------------------------------------------------------------
// Remove white space and begining and end of string
//---------------------------------------------------------------------------
void gdshare::Trim(String *pstr)
{
  if( pstr->size() > 0 )
  {
    int iFirstNoWhite = 0;
    int iLastNoWhite = pstr->size()-1;

    // Search for first non-white character
    for( ; iFirstNoWhite <= iLastNoWhite; iFirstNoWhite++ )
    {
      if( !isspace((*pstr)[iFirstNoWhite]) )
        break;
    }

    // Search for last non-white character
    for( ; iLastNoWhite > iFirstNoWhite; iLastNoWhite-- )
    {
      if( !isspace((*pstr)[iLastNoWhite]) )
        break;
    }

    // Extract sub-string
    (*pstr) = pstr->substr(iFirstNoWhite, iLastNoWhite - iFirstNoWhite + 1);
  }
}

//---------------------------------------------------------------------------
// Build a string with format
//---------------------------------------------------------------------------
String gdshare::StrFormat(pcsz pszFormat, ...)
{
  va_list argptr;
  va_start(argptr, pszFormat);
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
  VSNPRINTF(g_acScratchBuf, g_iScratchLen, pszFormat, argptr);
  va_end(argptr);

  String str = g_acScratchBuf;
  return str;
}

//---------------------------------------------------------------------------
// ExportDict
//---------------------------------------------------------------------------
void StringDict::Export(String *pstrDest)
{
  pstrDest->erase();

  const_iterator it = begin();
  for( ; it != end(); it++ )
  {
    String strName = it->first;
    String strValue = it->second;

    strName.Trim();

    if( !pstrDest->empty() )
      (*pstrDest) += '|';

    pstrDest->Printf("%s%s=%d,%s", PSZ(*pstrDest), PSZ(strName), strValue.size(), PSZ(strValue));
  }
}

//---------------------------------------------------------------------------
// ImportDict
//---------------------------------------------------------------------------
void StringDict::Import(const String &_strSrc)
{
  clear();
  String strSrc = _strSrc;
  String strSize, strName, strValue;
  int iStatus = 0;
  while( strSrc.size() > 0 )
  {
    // Extrat name
    iStatus = strSrc.ExtractToken('=', &strName);
    if( iStatus )
    {
      // Extract size
      iStatus = strSrc.ExtractToken(',', &strSize);
      if( iStatus )
      {
        // Extract value
        strValue = strSrc.substr(0, atoi(PSZ(strSize)));
        (*this)[strName] = strValue;
        // Suppress separator
        strSrc.erase(0, 1);
      }
    }
  }
}

//---------------------------------------------------------------------------
// StringDict::ConcatValue
//---------------------------------------------------------------------------
void StringDict::ConcatValue(const String &strKey, const String &strValue, char cSep)
{
  // Get already stored value
  String strStoredValue;
  if( find(strKey) != end() )
    strStoredValue = (*this)[strKey] + cSep;

  // Concat with given value
  strStoredValue += strValue;

  // Store new value
  (*this)[strKey] = strStoredValue;
}

//---------------------------------------------------------------------------
// StringDict::Value
//---------------------------------------------------------------------------
const String &StringDict::Value(const String &strKey) const
{
  StringDict::const_iterator itDict;

  itDict = find(strKey);
  if( itDict != end() )
    return itDict->second;
 
  return g_strEmpty;
}

//---------------------------------------------------------------------------
// StringDict::HasKey
//---------------------------------------------------------------------------
bool StringDict::HasKey(const String &strKey) const
{
  StringDict::const_iterator itDict = find(strKey);
  if( itDict != end() )
    return true;
  return false;
}

//=============================================================================
//
// String
//
//=============================================================================
//---------------------------------------------------------------------------
// String::ExtractToken
//---------------------------------------------------------------------------
int String::ExtractToken(char c, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( this == pstrToken )
    return 0;

  int iSrcLength = length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for separator
  int iPos = find_first_of(c);
  if( iPos < 0 )
  {
    // Not found
    *pstrToken = *this;
    *this = g_strEmpty;
    return 2;
  }

  // Separator found
  *pstrToken = substr(0, iPos);
  erase(0, iPos+1);
  return 1;
}

//---------------------------------------------------------------------------
// String::Split
//---------------------------------------------------------------------------
void String::Split(char c, String *pstrLeft, String *pstrRight, bool bPreserve)
{
  if( bPreserve )
  {
    String strSaved(*this);
    ExtractTokenRight(c, pstrRight);
    (*pstrLeft) = *this;
    *this = strSaved;
  }
  else
  {
    ExtractTokenRight(c, pstrRight);
    (*pstrLeft) = *this;
  }
}

//---------------------------------------------------------------------------
// String::ExtractTokenRight
//---------------------------------------------------------------------------
int String::ExtractTokenRight(char c, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( this == pstrToken )
    return 0;

  int iSrcLength = length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for separator
  int iPos = find_last_of(c);
  if( iPos < 0 )
  {
    // Not found
    *pstrToken = *this;
    *this = g_strEmpty;
    return 2;
  }

  // Separator found
  *pstrToken = substr(iPos+1);
  erase(iPos);
  return 1;
}

//---------------------------------------------------------------------------
// String::ExtractToken
//---------------------------------------------------------------------------
int String::ExtractToken(char cLeft, char cRight, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( this == pstrToken )
    return 0;

  int iSrcLength = length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for enclosing characters
  int iLeftPos = find(cLeft);
  int iRightPos = find(cRight, iLeftPos + 1);
  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    *pstrToken = g_strEmpty;
    return 2;
  }

  // Enclosing characters found
  *pstrToken = substr(iLeftPos + 1, iRightPos - iLeftPos - 1);
  erase(0, iRightPos + 1);
  return 1;
}

//---------------------------------------------------------------------------
// String::ExtractTokenRight
//---------------------------------------------------------------------------
int String::ExtractTokenRight(char cLeft, char cRight, String *pstrToken)
{
  // Cannot extract a substring a put it in the same string !
  if( this == pstrToken )
    return 0;

  int iSrcLength = length();

  if( 0 == iSrcLength )
  {
    // Nothing else
    pstrToken->erase();
    return 0;
  }

  // Search for enclosing characters
  int iRightPos = rfind(cRight);
  int iLeftPos = iRightPos > 0 ? (int)rfind(cLeft, iRightPos - 1) : -1;
  if( iLeftPos < 0 || iRightPos < 0 || iRightPos < iLeftPos )
  {
    // Not found
    *pstrToken = g_strEmpty;
    return 2;
  }

  // Enclosing characters found
  *pstrToken = substr(iLeftPos+1, iRightPos - iLeftPos - 1);
  erase(iLeftPos);
  return 1;
}

//---------------------------------------------------------------------------
// String::RemoveEnclosure
//---------------------------------------------------------------------------
bool String::RemoveEnclosure(pcsz pszLeft, pcsz pszRight)
{
  // pcszLeft & pcszRight must have the same length
  if( strlen(pszLeft) != strlen(pszRight) )
    return false;
  
  for( uint ui = 0; ui < strlen(pszLeft); ui++ )
  {
    string strMask;
    strMask += pszLeft[ui];
    strMask += '*';
    strMask += pszRight[ui];
    if( Match(strMask) )
    {
      *this = substr(strlen(pszLeft), size() - (strlen(pszLeft) + strlen(pszRight)));
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
// String::RemoveEnclosure
//---------------------------------------------------------------------------
bool String::RemoveEnclosure(char cLeft, char cRight)
{
  string strMask;
  strMask += cLeft;
  strMask += '*';
  strMask += cRight;
  if( Match(strMask) )
  {
    *this = substr(1, size() - 2);
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
// String::IsEquals
//---------------------------------------------------------------------------
bool String::IsEquals(const String &str) const
{
  return MAKEBOOL(*this == str);
}

//---------------------------------------------------------------------------
// String::IsEqualsNoCase
//---------------------------------------------------------------------------
bool String::IsEqualsNoCase(const String &str) const
{
  return MAKEBOOL(!stricmp(c_str(), str.c_str()));
}

//---------------------------------------------------------------------------
// String::StartWith
//---------------------------------------------------------------------------
bool String::StartWith(char c) const
{
  if( size() == 0 )
    return false;

  if( c == (*this)[0] )
    return true;

  return false;
}

//---------------------------------------------------------------------------
// String::StartWith
//---------------------------------------------------------------------------
bool String::StartWith(pcsz pcszStart, bool bNoCase) const
{
  if( size() < strlen(pcszStart) )
    return false;

  if( bNoCase )
  {
    return MAKEBOOL(!strnicmp(c_str(), pcszStart, strlen(pcszStart)));
  }
  else
  {
    return MAKEBOOL(!strncmp(c_str(), pcszStart, strlen(pcszStart)));
  }
}

//---------------------------------------------------------------------------
// String::EndWith
//---------------------------------------------------------------------------
bool String::EndWith(pcsz pcszEnd, bool bNoCase) const
{
  if( size() < strlen(pcszEnd) )
    return false;

  if( bNoCase )
  {
    return MAKEBOOL(!strnicmp(c_str()+strlen(c_str())-strlen(pcszEnd), pcszEnd, strlen(pcszEnd)));
  }
  else
  {
    return MAKEBOOL(!strncmp(c_str()+strlen(c_str())-strlen(pcszEnd), pcszEnd, strlen(pcszEnd)));
  }
}

//---------------------------------------------------------------------------
// String::EndWith
//---------------------------------------------------------------------------
bool String::EndWith(char c) const
{
  if( size() == 0 )
    return false;

  if( c == (*this)[size()-1] )
    return true;

  return false;
}

//---------------------------------------------------------------------------
// Look for occurence for a string in another
// Take care of '?' that match any character
//---------------------------------------------------------------------------
static pcsz FindSubStrWithJoker(pcsz pszSrc, pcsz pMask, uint uiLenMask)
{
  if (strlen(pszSrc) < uiLenMask)
    return NULL; // No hope

  // while mask len < string len 
  while( *(pszSrc + uiLenMask - 1) ) 
  {
    uint uiOffSrc = 0; // starting offset in mask and sub-string
    
    // Tant qu'on n'est pas au bout du masque
    while (uiOffSrc < uiLenMask)
    {
      char cMask = pMask[uiOffSrc];
      
      if (cMask != '?') // In case of '?' it always match
      {
        if (pszSrc[uiOffSrc] != cMask)
          break;
      }
  
      // Next char
      uiOffSrc++;
    }

    // String matched !
    if (uiOffSrc == uiLenMask)
      return pszSrc + uiLenMask;

    // Next sub-string
    pszSrc++;
  }

  // Not found
  return NULL;
}

//---------------------------------------------------------------------------
// String::Match
//---------------------------------------------------------------------------
bool String::Match(const String &strMask) const
{
  return Match(PSZ(strMask));
}

//---------------------------------------------------------------------------
// String::Match
//---------------------------------------------------------------------------
bool String::Match(pcsz pszMask) const
{
  pcsz pszTxt = c_str();
  while (*pszMask)
  {
    switch (*pszMask)
    {

      case '\\':
        // escape next special mask char (e.g. '?' or '*')
        pszMask++;
        if( *pszMask )
        {
          if( *(pszMask++) != *(pszTxt++) )
            return false;
        }
        break;

      case '?': // joker at one position
        if (!*pszTxt)
          return true; // no match

        pszTxt++;
        pszMask++;
        break;

      case '*': // joker on one or more positions
        {
          // Pass through others useless joker characters
          while (*pszMask == '*' || *pszMask == '?')
            pszMask++;

          if (!*pszMask)
            return true; // Fin

          // end of mask
          uint uiLenMask;
          const char *pEndMask = strchr(pszMask, '*');

          if (pEndMask)
            // other jokers characters => look for bloc between the two jokers in source string
            uiLenMask = pEndMask - pszMask;
          else
            // string must be end with mask
            return (NULL != FindSubStrWithJoker(pszTxt + strlen(pszTxt)-strlen(pszMask), pszMask, strlen(pszMask)))?
                           true : false;

          // Search first uiLenMask characters from mask in text
          pcsz pEnd = FindSubStrWithJoker(pszTxt, pszMask, uiLenMask);

          if (!pEnd)
            // Mask not found
            return false;

          pszTxt = pEnd;
          pszMask += uiLenMask;
        }
        break;

      default:
        if( *(pszMask++) != *(pszTxt++) )
          return false;
        break;
    }
  }

  if( *pszTxt )
    // End of string not reached
    return false;

  return true;
}

//---------------------------------------------------------------------------
// String::Trim
//---------------------------------------------------------------------------
void String::Trim()
{
  if( size() > 0 )
  {
    int iFirstNoWhite = 0;
    int iLastNoWhite = size()-1;

    // Search for first non-white character
    for( ; iFirstNoWhite <= iLastNoWhite; iFirstNoWhite++ )
    {
      if( !isspace((*this)[iFirstNoWhite]) )
        break;
    }

    // Search for last non-white character
    for( ; iLastNoWhite > iFirstNoWhite; iLastNoWhite-- )
    {
      if( !isspace((*this)[iLastNoWhite]) )
        break;
    }

    // Extract sub-string
    (*this) = substr(iFirstNoWhite, iLastNoWhite - iFirstNoWhite + 1);
  }
}

//---------------------------------------------------------------------------
// String::Printf
//---------------------------------------------------------------------------
int String::Printf(pcsz pszFormat, ...)
{
  va_list argptr;
  va_start(argptr, pszFormat);
#ifdef THREAD_SUPPORT
	LOCK(&g_acScratchBuf)
#endif
  VSNPRINTF(g_acScratchBuf, g_iScratchLen, pszFormat, argptr);
  va_end(argptr);

  *this = g_acScratchBuf;
  return size();
}

//---------------------------------------------------------------------------
// String::Split
//---------------------------------------------------------------------------
void String::Split(char c, vector<String> *pvecstr)
{
  // Clear vector
  pvecstr->clear();
  String strToken;
  while( !empty() )
  {
    if( 0 != ExtractToken(c, &strToken) )
      pvecstr->push_back(strToken);
  }
}

//---------------------------------------------------------------------------
// String::Split (const version)
//---------------------------------------------------------------------------
void String::Split(char c, vector<String> *pvecstr) const
{
  // Clear vector
  pvecstr->clear();
  String strToken;
  String strTmp(*this);
  while( !strTmp.empty() )
  {
    if( 0 != strTmp.ExtractToken(c, &strToken) )
      pvecstr->push_back(strToken);
  }
}

//---------------------------------------------------------------------------
// String::Join
//---------------------------------------------------------------------------
void String::Join(const vector<String> &vecStr, char cSep)
{
  erase();
  for( uint ui=0; ui < vecStr.size(); ui++ )
  {
    if( 0 < ui )
      *this += cSep;
    *this += vecStr[ui];
  }
}

//---------------------------------------------------------------------------
// String::RemoveItem
//---------------------------------------------------------------------------
bool String::RemoveItem(const String &strItem, char cSep)
{
  uint uiPos = find(strItem);
  if( uiPos == string::npos )
    return false;

  if( *this == strItem )
  {
    erase();
    return true;
  }

  vector<String> vecstr;
  Split(cSep, &vecstr);
  for( vector<String>::iterator it = vecstr.begin(); it != vecstr.end(); it++ )
  {
    if( *it == strItem )
    {
      vecstr.erase(it);
      Join(vecstr, cSep);
      return true;
    }
  }
  Join(vecstr, cSep);
  return false;
}

//---------------------------------------------------------------------------
// String::ToLower
//---------------------------------------------------------------------------
void String::ToLower()
{
  for(uint ui=0; ui < size(); ui++)
    replace(ui, 1, 1, tolower(c_str()[ui]));
}

//---------------------------------------------------------------------------
// String::ToUpper
//---------------------------------------------------------------------------
void String::ToUpper()
{
  for(uint ui=0; ui < size(); ui++)
    replace(ui, 1, 1, toupper(c_str()[ui]));
}

//---------------------------------------------------------------------------
// String::Replace
//---------------------------------------------------------------------------
void String::Replace(pcsz pszSrc, pcsz pszDst)
{
  pcsz pBeg = NULL, pFind = NULL;

  int iLenSrc = strlen(pszSrc);
  if( iLenSrc == 0 )
    // Avoid infinite loop
    return;

  int iLenDst = strlen(pszDst);
  int iOffset = 0;
  for(;;)
  {
    pBeg = c_str() + iOffset;
    pFind = strstr(pBeg, pszSrc);

    if( !pFind )
      return;

    // Do replace
    replace(pFind - pBeg + iOffset, iLenSrc, pszDst);
    // replace again after replacement
    iOffset += pFind - pBeg + iLenDst;
  }
}

//---------------------------------------------------------------------------
// String::Replace
//---------------------------------------------------------------------------
void String::Replace(char cSrc, char cDst)
{
  if (!strchr(c_str(), cSrc))
    return; // Nothing to do

  for( uint uiPos = 0; uiPos < size(); uiPos++ )
    if( cSrc == (*this)[uiPos] )
      replace(uiPos, 1, 1, cDst);
}

//---------------------------------------------------------------------------
// String::Hash
//---------------------------------------------------------------------------
ulong String::Hash() const
{
  int64 i64Modulo = (int64(2) << 31) - 1;
  int64 Hash64 = 0;
  uint length = size();
  for( uint uiPos = 0; uiPos < length; uiPos++ )
    Hash64 = (31 * Hash64 + (*this)[uiPos]) % i64Modulo;

  return (ulong)(Hash64); 
}

//=============================================================================
//
// Other free functions
//
//=============================================================================

//----------------------------------------------------------------------------
// GetEnv
//----------------------------------------------------------------------------
bool gdshare::GetEnv(const char *pszVar, String *pstrValue, const char *pszDef)
{
#ifdef __WIN32__
  #ifdef _THREAD_SUPPORT
    LOCK(&g_acScratchBuf)
  #endif
  if ( ::GetEnvironmentVariable(pszVar, g_acScratchBuf, g_iScratchLen) == 0 )
#else
  char *pszBuf = ::getenv(pszVar);
  if ( pszBuf == NULL )
#endif
  { // if the variable  is undefined, use default value
    if ( pszDef )
      *pstrValue = pszDef;
    return false;
  }

#ifdef __WIN32__
  *pstrValue = g_acScratchBuf;
#else
  *pstrValue = pszBuf;
#endif
  return true;
}

//----------------------------------------------------------------------------
// Exec
//----------------------------------------------------------------------------
int gdshare::Exec(const char* pszCmdLine, const char *pszDefDir, int bBackground, bool bThrow, ulong *pulReturnCode)
{
  int rc = 0;

#ifdef __WIN32__
  // JA 1996/05/02 Support des documents, avec les associations (.TXT lance NOTEPAD,...)
  // A priori ShellExecute fait exactement ce que l'on veut, sauf pour
  //  le probleme du GetMessage() -> c'est pourquoi on veut savoir
  //  *a priori* si la commande est un executable ou un fichier. Note:
  //  si pas d'extension on considere que c'est un executable)
  bool bIsExecutable;
  String strCommandLine = pszCmdLine;
  String strCmd;
  strCommandLine.Trim();

  if ( strCommandLine.StartWith('"') )
  { // the executable name of path may contains spaces. Normaly the method caller have
    // demilited the command with quotes.
    int iQuotIndex = strCmd.find('"');
    if( iQuotIndex == -1 )
    {
      if ( bThrow )
        throw Exception("ERR_FILE", "Missing quotes", "Exec");
    }

    strCmd = strCommandLine.substr(1, iQuotIndex);
  }
  else
    strCommandLine.ExtractToken(' ', &strCmd);

  // strCmd vaut le premier argument (= le nom de l'exe ou du document)
  // extraction de l'extension :
  String strExt;
  String strCmdDummy = strCmd;
  rc = strCmdDummy.ExtractTokenRight('.', &strExt );
  // Seul le cas rc = 1 (chaine extraite et separateur trouvé) indique
  //  que l'on a une extension.
  if( rc != 1 )
  {
    strExt = g_strEmpty;
    bIsExecutable = true;
  }
  else
  {
    // On pourrait aussi tester l'entree programs= du win.ini pour les
    //  extensions correspondant a des executables, mais bon, cela
    //  ralentirait pour des extensions futures de l'os tres lointaines...
    if(   stricmp( PSZ(strExt), "exe" ) == 0
       || stricmp( PSZ(strExt), "com" ) == 0
       || stricmp( PSZ(strExt), "bat" ) == 0
       || stricmp( PSZ(strExt), "pif" ) == 0
       || stricmp( PSZ(strExt), "cmd" ) == 0
      )
    {
      // On exclue les URL
      if (strstr(PSZ(strCmd), "://") == NULL)
        bIsExecutable = true;
      else
        bIsExecutable = false;
    }
    else
      bIsExecutable = false;
  }

  bool bIsError = false;

  // WinExec attend que le process fils appelle GetMessage()
  // On utilise plutot CreateProcess, dans le cas d'un executable
  PROCESS_INFORMATION aProcessInformation;
  memset(&aProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  if( bIsExecutable )
  {
    STARTUPINFO         aStartupInfo;
    memset(&aStartupInfo, 0, sizeof(aStartupInfo));
    aStartupInfo.cb = sizeof(aStartupInfo);
    rc = 0;
    if( !::CreateProcess(NULL, (char*)pszCmdLine, NULL, NULL,
                         FALSE, CREATE_NO_WINDOW, NULL,
                         pszDefDir,
                         &aStartupInfo, &aProcessInformation) )
    {
      rc = ::GetLastError();
      bIsError = true;
    }
  }
  else
  {
    // Ce n'est pas un executable -> il faut forcement utiliser l'API
    //  ShellExecute
    rc = (int)::ShellExecute( HWND_DESKTOP, NULL,
                              PSZ(strCmd),
                              PSZ(strCommandLine),
                              pszDefDir,
                              SW_SHOW );
    if( rc >= 0 && rc < 32 )
      bIsError = true;
  }

  // Une erreur est intervenue dans le ShellExecute
  if( bIsError && bThrow )
    throw Exception("ERR_FILE", "Error while executing command", "Exec");

  if( !bBackground && bIsExecutable)
  {
    // Attente de la fin du process puis on récupère son code retour
    ulong ulReturnCode;
    ::WaitForSingleObject( aProcessInformation.hProcess, INFINITE);
    ::GetExitCodeProcess(aProcessInformation.hProcess, &ulReturnCode);
    if( pulReturnCode )
      *pulReturnCode = ulReturnCode;
    return 0;
  }

  if ( bIsExecutable )
  { // Il faut décrémenter le usecount des handles contenues dans
    // PROCESS_INFORMATION, sinon les informations sur le process
    // ne seront jamais libérées par le système...
    ::CloseHandle(aProcessInformation.hProcess);
    ::CloseHandle(aProcessInformation.hThread);
  }
  return 0;

#else
  String sCmd = pszCmdLine;
  if( bBackground )
    // Background task
    sCmd += "&";

  // Execute shell command then exit
  int iChildReturnCode = system(PSZ(sCmd));

  // Parent process, let's wait for the child to finish
  LogVerbose("sys", "return code %d", iChildReturnCode);
  return iChildReturnCode;
#endif
}

//----------------------------------------------------------------------------
// Exec
//----------------------------------------------------------------------------
int gdshare::ExecAs(const char* pszCmdLine, const char *pszDefDir, int bBackground, bool bThrow, ulong *pulReturnCode, uid_t uid, gid_t gid)
{
  int rc = 0;

#ifdef __WIN32__
  // JA 1996/05/02 Support des documents, avec les associations (.TXT lance NOTEPAD,...)
  // A priori ShellExecute fait exactement ce que l'on veut, sauf pour
  //  le probleme du GetMessage() -> c'est pourquoi on veut savoir
  //  *a priori* si la commande est un executable ou un fichier. Note:
  //  si pas d'extension on considere que c'est un executable)
  bool bIsExecutable;
  String strCommandLine = pszCmdLine;
  String strCmd;
  strCommandLine.Trim();

  if ( strCommandLine.StartWith('"') )
  { // the executable name of path may contains spaces. Normaly the method caller have
    // demilited the command with quotes.
    int iQuotIndex = strCmd.find('"');
    if( iQuotIndex == -1 )
    {
      if ( bThrow )
        throw Exception("ERR_FILE", "Missing quotes", "Exec");
    }

    strCmd = strCommandLine.substr(1, iQuotIndex);
  }
  else
    strCommandLine.ExtractToken(' ', &strCmd);

  // strCmd vaut le premier argument (= le nom de l'exe ou du document)
  // extraction de l'extension :
  String strExt;
  String strCmdDummy = strCmd;
  rc = strCmdDummy.ExtractTokenRight('.', &strExt );
  // Seul le cas rc = 1 (chaine extraite et separateur trouvé) indique
  //  que l'on a une extension.
  if( rc != 1 )
  {
    strExt = g_strEmpty;
    bIsExecutable = true;
  }
  else
  {
    // On pourrait aussi tester l'entree programs= du win.ini pour les
    //  extensions correspondant a des executables, mais bon, cela
    //  ralentirait pour des extensions futures de l'os tres lointaines...
    if(   stricmp( PSZ(strExt), "exe" ) == 0
       || stricmp( PSZ(strExt), "com" ) == 0
       || stricmp( PSZ(strExt), "bat" ) == 0
       || stricmp( PSZ(strExt), "pif" ) == 0
       || stricmp( PSZ(strExt), "cmd" ) == 0
      )
    {
      // On exclue les URL
      if (strstr(PSZ(strCmd), "://") == NULL)
        bIsExecutable = true;
      else
        bIsExecutable = false;
    }
    else
      bIsExecutable = false;
  }

  bool bIsError = false;

  // WinExec attend que le process fils appelle GetMessage()
  // On utilise plutot CreateProcess, dans le cas d'un executable
  PROCESS_INFORMATION aProcessInformation;
  memset(&aProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  if( bIsExecutable )
  {
    STARTUPINFO         aStartupInfo;
    memset(&aStartupInfo, 0, sizeof(aStartupInfo));
    aStartupInfo.cb = sizeof(aStartupInfo);
    rc = 0;
    if( !::CreateProcess(NULL, (char*)pszCmdLine, NULL, NULL,
                         FALSE, CREATE_NO_WINDOW, NULL,
                         pszDefDir,
                         &aStartupInfo, &aProcessInformation) )
    {
      rc = ::GetLastError();
      bIsError = true;
    }
  }
  else
  {
    // Ce n'est pas un executable -> il faut forcement utiliser l'API
    //  ShellExecute
    rc = (int)::ShellExecute( HWND_DESKTOP, NULL,
                              PSZ(strCmd),
                              PSZ(strCommandLine),
                              pszDefDir,
                              SW_SHOW );
    if( rc >= 0 && rc < 32 )
      bIsError = true;
  }

  // Une erreur est intervenue dans le ShellExecute
  if( bIsError && bThrow )
    throw Exception("ERR_FILE", "Error while executing command", "Exec");

  if( !bBackground && bIsExecutable)
  {
    // Attente de la fin du process puis on récupère son code retour
    ulong ulReturnCode;
    ::WaitForSingleObject( aProcessInformation.hProcess, INFINITE);
    ::GetExitCodeProcess(aProcessInformation.hProcess, &ulReturnCode);
    if( pulReturnCode )
      *pulReturnCode = ulReturnCode;
    return 0;
  }

  if ( bIsExecutable )
  { // Il faut décrémenter le usecount des handles contenues dans
    // PROCESS_INFORMATION, sinon les informations sur le process
    // ne seront jamais libérées par le système...
    ::CloseHandle(aProcessInformation.hProcess);
    ::CloseHandle(aProcessInformation.hThread);
  }
  return 0;

#else
  String sCmd = pszCmdLine;
  if( bBackground )
    // Background task
    sCmd += "&";

  LogVerbose("sys", "Shell exec with effective uid %d", uid);
  LogVerbose("sys", "Shell exec with effective gid %d", gid);
  int iChildReturnCode = 0;
  // fork a new process
  pid_t pid = fork();
  if( 0 == pid )
  {
    // Child process
    if( gid > 0 )
    {
      // Set new gid
      rc = setgid(gid);
      if( rc )
        exit(-1);
    }
    if( uid > 0 )
    {
      // Set new uid
      rc = setuid(uid);
      if( rc )
        exit(-1);
    }
    // Execute shell command then exit
    exit(system(PSZ(sCmd)));
  }
  else if( pid < 0 )
    throw Exception("ERR_FILE", "Error cannot execute shell command", "Exec");
//  iChildReturnCode = system(PSZ(sCmd));

  // Parent process, let's wait for the child to finish
  waitpid(pid, &iChildReturnCode, WEXITSTATUS(iChildReturnCode));
  LogVerbose("sys", "return code %d", iChildReturnCode);
  return iChildReturnCode;
#endif
}

//=============================================================================
//
// Command line argument
//
//=============================================================================
CommandLine *CommandLine::ms_pTheInstance = NULL;

//----------------------------------------------------------------------------
// CommandLine::Instance
//----------------------------------------------------------------------------
CommandLine *CommandLine::Instance()
{
  if( NULL == ms_pTheInstance )
    ms_pTheInstance = new CommandLine;

  return ms_pTheInstance;
}

//----------------------------------------------------------------------------
// CommandLine::FindShortOpt
//----------------------------------------------------------------------------
SCommandLineOpt *CommandLine::FindShortOpt(const char cOpt)
{
  vecOpts::iterator itOpt;
  for( itOpt = m_vecOptDefs.begin(); itOpt != m_vecOptDefs.end(); itOpt++ )
  {
    if( (*itOpt).cShortName == cOpt )
      return &(*itOpt);
  }
  return NULL;
}

//----------------------------------------------------------------------------
// CommandLine::FindLongOpt
//----------------------------------------------------------------------------
SCommandLineOpt *CommandLine::FindLongOpt(const String &strOpt)
{
  vecOpts::iterator itOpt;
  for( itOpt = m_vecOptDefs.begin(); itOpt != m_vecOptDefs.end(); itOpt++ )
  {
    if( IsEquals((*itOpt).strLongName, strOpt) )
      return &(*itOpt);
  }
  return NULL;
}

//----------------------------------------------------------------------------
// CommandLine::AddOpt
//----------------------------------------------------------------------------
void CommandLine::AddOpt(char cShortName, pcsz pszLongName, pcsz pszValue, pcsz pszDesc)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();

  SCommandLineOpt opt;
  opt.cShortName = cShortName;
  opt.strLongName = pszLongName;
  opt.strDesc = pszDesc;
  opt.strValue = pszValue ? pszValue : g_strEmpty;

  o.m_vecOptDefs.push_back(opt);
}

//----------------------------------------------------------------------------
// CommandLine::AddArg
//----------------------------------------------------------------------------
void CommandLine::AddArg(pcsz pszDesc, bool bSingle)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();

  SCommandLineArg arg;
  arg.strDesc = pszDesc;
  arg.bSingle = bSingle; // if not single Several argument can follow

  o.m_vecArgDefs.push_back(arg);
}

//----------------------------------------------------------------------------
// CommandLine::BadOption
//----------------------------------------------------------------------------
bool CommandLine::BadOption(const String &strOpt)
{
  std::cerr << "Unrecognized option '" << strOpt << "'" << std::endl;
  return ShowUsage(std::cerr);
}

//----------------------------------------------------------------------------
// CommandLine::Read
//----------------------------------------------------------------------------
bool CommandLine::Read(int iArgc, char **ppszArgv)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();

  // Special options that ignore all others
  CommandLine::AddOpt('h', "help", "", "Show this usage");
  CommandLine::AddOpt('v', "version", "", "Program version");
  
  for(int i = 1; i < iArgc; i++ )
  {
    String strArg = ppszArgv[i];
    if( StartWith(strArg, "--") )
    {
      // Supress "--" before option name
      strArg.erase(0, 2);
      SCommandLineOpt *pOpt = o.FindLongOpt(strArg);
      if( !pOpt )
        // Bad option
        return BadOption("--" + strArg);
      if( !pOpt->strValue.empty() && i < iArgc - 1 )
        // Store value
        o.m_dictOptValues[pOpt->strLongName] = ppszArgv[++i];
      else
        o.m_dictOptValues[pOpt->strLongName] = "dummy";
    }
    else if( StartWith(strArg, '-') )
    {
      // Supress "-" before option name
      SCommandLineOpt *pOpt = o.FindShortOpt(strArg[1]);
      if( NULL == pOpt )
        // Bad option
        return BadOption(strArg);
      if( !pOpt->strValue.empty() && i < iArgc - 1 )
        // Store value
        o.m_dictOptValues[pOpt->strLongName] = ppszArgv[++i];
      else
        o.m_dictOptValues[pOpt->strLongName] = "dummy";
    }
    else
    {
      if( o.m_vecArgDefs.empty() )
      {
        std::cerr << "Unexpected argument \"" << strArg << "\"" << std::endl;
        return ShowUsage(std::cerr);
      }

      // Simple argument
      o.m_vecArgs.push_back(strArg);
    }
  }

  // Check for "version" or "help" options
  if( IsOption("version") )
  {
    std::cout << o.m_strCmdName << " V" << o.m_strCmdVersion << std::endl;
    return false;
  }

  else if( IsOption("help") )
    return ShowUsage(std::cout);

  // Check arguments
  else if( o.m_vecArgDefs.size() > o.m_vecArgs.size() )
  {
    std::cerr << "Argument missing." << std::endl;
    return ShowUsage(std::cerr);
  }


  return true;
}

//----------------------------------------------------------------------------
// CommandLine::IsOption
//----------------------------------------------------------------------------
bool CommandLine::IsOption(const String &strOpt)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();

  StringDict::iterator it = o.m_dictOptValues.find(strOpt);
  if( it != o.m_dictOptValues.end() )
    return true;
  return false;
}

//----------------------------------------------------------------------------
// CommandLine::OptionValue
//----------------------------------------------------------------------------
String CommandLine::OptionValue(const String &strOpt)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();

  StringDict::iterator it = o.m_dictOptValues.find(strOpt);
  if( o.m_dictOptValues.end() == it )
    return g_strEmpty;

  return it->second;
}

//----------------------------------------------------------------------------
// CommandLine::ArgCount
//----------------------------------------------------------------------------
int CommandLine::ArgCount()
{
  return Instance()->m_vecArgs.size();
}

//----------------------------------------------------------------------------
// CommandLine::Arg
//----------------------------------------------------------------------------
String CommandLine::Arg(int i)
{
  if( i >= 0 || i < ArgCount() )
    return Instance()->m_vecArgs[i];
  return g_strEmpty;
}

//----------------------------------------------------------------------------
// CommandLine::SetCmdNameVersion
//----------------------------------------------------------------------------
void CommandLine::SetCmdNameVersion(const String &strName, const String &strVersion)
{
  Instance()->m_strCmdName = strName;
  Instance()->m_strCmdVersion = strVersion;
}

//----------------------------------------------------------------------------
// CommandLine::ShowUsage
//----------------------------------------------------------------------------
bool CommandLine::ShowUsage(std::ostream &os)
{
  // Get a reference to the singleton object
  CommandLine &o = *Instance();
  
  os << "Usage:" << std::endl;
  os << o.m_strCmdName;
  if( o.m_vecOptDefs.size() > 0 )
    os << " [options]";
  if( o.m_vecArgDefs.size() > 0 )
    for( uint ui = 0; ui < o.m_vecArgDefs.size(); ui++ )
    {
      os << " " << o.m_vecArgDefs[ui].strDesc;
      if( !o.m_vecArgDefs[ui].bSingle )
        os << "...";
    }
  os << std::endl;
  if( o.m_vecOptDefs.size() > 0 )
  {
    os << "Options:";
    for( uint ui = 0; ui < o.m_vecOptDefs.size(); ui++ )
    {
      std::ostringstream oss;
      oss << std::endl << "-" << o.m_vecOptDefs[ui].cShortName << ", --" << o.m_vecOptDefs[ui].strLongName;
      if( !o.m_vecOptDefs[ui].strValue.empty() )
        oss << " <" << o.m_vecOptDefs[ui].strValue << ">";
      // Format Output
      os.width(40);
      os << std::left << oss.str();
      os << o.m_vecOptDefs[ui].strDesc;
    }
  }
  os << std::endl;
  return false;
}

//----------------------------------------------------------------------------
// CommandLine::ShowUsage
//----------------------------------------------------------------------------
void CommandLine::ShowUsage(const String &strAppInfo)
{
  if( !strAppInfo.empty() )
    cout << strAppInfo << endl;
  ShowUsage(cout);
}

//=============================================================================
//
// App
//
//=============================================================================
App::App(const String &strName)
{
  static App *s_pApp = NULL;
  if( s_pApp )
    throw  Exception("Can't allocate more than one application object", "App object already allocated", "App::App");

  m_strName = strName;
  s_pApp = this;
}

#ifdef __LINUX__
// Signal handler
static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}
#define RUN_AS_USER "root"
#endif
//----------------------------------------------------------------------------
// App::Deamonize
//----------------------------------------------------------------------------
void App::Deamonize()
{
#ifdef __LINUX__
  LogInfo("app", "Deamonize process");

  pid_t pid, sid, parent;
  int lfp = -1;

  String strLockFile = String("/var/lock/subsys/") + m_strName;
  String strPidFile = String("/var/run/") + m_strName + String(".pid");

  /* already a daemon */
  if( getppid() == 1 ) return;

  /* Drop user if there is one, and we were run as root */
  if( getuid() == 0 || geteuid() == 0 )
  {
    struct passwd *pw = getpwnam(RUN_AS_USER);
    if( pw ) 
    {
      LogNotice("app", "setting user to " RUN_AS_USER );
      setuid( pw->pw_uid );
    }
  }

  /* Trap signals that we expect to recieve */
  signal(SIGCHLD,child_handler);
  signal(SIGUSR1,child_handler);
  signal(SIGALRM,child_handler);

  /* Fork off the parent process */
  pid = fork();
  if( pid < 0 )
  {
    LogError("app", "unable to fork daemon, code=%d (%s)",
              errno, strerror(errno) );
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then we can exit the parent process. */
  if( pid > 0 )
  {
    /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
       for two seconds to elapse (SIGALRM).  pause() should not return. */
    alarm(2);
    pause();
    exit(EXIT_FAILURE);
  }

  /* Create the lock and pid files */
  lfp = open(PSZ(strLockFile),O_RDWR|O_CREAT,0640);
  if( lfp < 0 )
  {
    LogError("app", "unable to create lock file %s, code=%d (%s)",
            PSZ(strLockFile), errno, strerror(errno) );
    exit(EXIT_FAILURE);
  }
  if( lockf(lfp, F_TLOCK, 0) < 0 )
  {
    LogError("app", "Cannot lock file '%s'. Exiting", PSZ(strLockFile));
    exit(EXIT_FAILURE);
  }
  int iPidFile = open(PSZ(strPidFile), O_RDWR|O_CREAT, 0640);
  if( iPidFile < 0 )
  {
    LogError("app", "unable to create pid file %s, code=%d (%s)",
            PSZ(strPidFile), errno, strerror(errno) );
    exit(EXIT_FAILURE);
  }
  String strPid;
  strPid.Printf("%d\n",getpid());
	if( write(iPidFile,PSZ(strPid),strPid.size()) < 0 )
  {
    LogError("app", "unable to write into pid file %s, code=%d (%s)",
            PSZ(strPidFile), errno, strerror(errno) );
    exit(EXIT_FAILURE);
  }

  /* At this point we are executing as the child process */
  parent = getppid();

  /* Cancel certain signals */
  signal(SIGCHLD,SIG_DFL); /* A child process dies */
  signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
  signal(SIGTTOU,SIG_IGN);
  signal(SIGTTIN,SIG_IGN);
  signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
  signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

  /* Change the file mode mask */
  umask(0);

  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0)
  {
    LogError( "app", "unable to create a new session, code %d (%s)",
            errno, strerror(errno) );
    exit(EXIT_FAILURE);
  }

  /* Change the current working directory.  This prevents the current
     directory from being locked; hence not being able to remove it. */
  if ((chdir("/")) < 0)
  {
      LogError("app", "unable to change directory to %s, code %d (%s)",
              "/", errno, strerror(errno) );
      exit(EXIT_FAILURE);
  }

  /* Redirect standard files to /dev/null */
  freopen( "/dev/null", "r", stdin);
  freopen( "/dev/null", "w", stdout);
  freopen( "/dev/null", "w", stderr);

  /* Tell the parent process that we are A-okay */
  kill( parent, SIGUSR1 );

  LogNotice("app", "Process started");
#endif
}

//=============================================================================
//
// Basic tree node
//
//=============================================================================
//----------------------------------------------------------------------------
// BasicTreeNode::BasicTreeNode
//----------------------------------------------------------------------------
BasicTreeNode::BasicTreeNode()
{
  m_pFirstChildNode = NULL;
  m_pLastChildNode = NULL;
  m_pNextSiblingNode = NULL;
  m_pParentNode = NULL;
}

//----------------------------------------------------------------------------
// BasicTreeNode::~BasicTreeNode
//----------------------------------------------------------------------------
BasicTreeNode::~BasicTreeNode()
{
  if( NULL != m_pFirstChildNode )
    delete m_pFirstChildNode;
  if( NULL != m_pNextSiblingNode )
    delete m_pNextSiblingNode;
}

//----------------------------------------------------------------------------
// BasicTreeNode::AddNode
//----------------------------------------------------------------------------
void BasicTreeNode::AddNode(BasicTreeNode *pNode)
{
  if( NULL == m_pFirstChildNode )
  {
    m_pFirstChildNode = pNode;
    m_pLastChildNode = pNode;
  }
  else
  {
    m_pLastChildNode->m_pNextSiblingNode = pNode;
    m_pLastChildNode = pNode;
  }

  pNode->m_pParentNode = this;
  pNode->m_pNextSiblingNode = NULL;
}

//=============================================================================
//
// CEnvVariableEvaluator
//
//=============================================================================
bool CEnvVariableEvaluator::Evaluate(String *pstrVar)
{
  return GetEnv(PSZ(*pstrVar), pstrVar);
}

//=============================================================================
//
// LogManager
//
//=============================================================================
LogManager *LogManager::ms_pTheInstance = NULL;

//----------------------------------------------------------------------------
// LogManager::LogManager
//----------------------------------------------------------------------------
LogManager::LogManager() : m_iMinLevel(LOG_INFO)
{
}

//----------------------------------------------------------------------------
// LogManager::Instance
//----------------------------------------------------------------------------
LogManager *LogManager::Instance()
{
  if( NULL == ms_pTheInstance )
    ms_pTheInstance = new LogManager;

  return ms_pTheInstance;
}

//----------------------------------------------------------------------------
// LogManager::PushLogTarget
//----------------------------------------------------------------------------
void LogManager::PushLogTarget(ILogTarget *pLogTarget)
{
  Instance()->m_stkCatchLogTarget.push(pLogTarget);
}

//----------------------------------------------------------------------------
// LogManager::PopLogTarget
//----------------------------------------------------------------------------
void LogManager::PopLogTarget()
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  if( !o.m_stkCatchLogTarget.empty() )
    o.m_stkCatchLogTarget.pop();
  else
    throw LogException("Can't pop log target", "Log target stack is empty", "LogManager::PopLogTarget");
}

//----------------------------------------------------------------------------
// LogManager::CurrentLogTarget
//----------------------------------------------------------------------------
ILogTarget *LogManager::CurrentLogTarget()
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  if( !o.m_stkCatchLogTarget.empty() )
    return o.m_stkCatchLogTarget.top();

  return &o.m_defLogHandler;
}

//----------------------------------------------------------------------------
// LogManager::Init
//----------------------------------------------------------------------------
void LogManager::Init(int iMinLevel, const String &_strFilter)
{
  // Get a reference to the singleton object
  LogManager &o = *Instance();

  o.m_iMinLevel = iMinLevel;

  String strFilter = _strFilter, strType;
  while( strFilter.size() > 0 )
  {
    ExtractToken(&strFilter, '|', &strType);
    if( strType.size() > 0 )
      o.m_setTypes.insert(strType);
  }
}

//----------------------------------------------------------------------------
// LogManager::Log
//----------------------------------------------------------------------------
void LogManager::Log(ELogLevel eLevel, pcsz pszType, const String &strMsg)
{
  // Get a reference to the singleton
  LogManager &o = *Instance();

  if( int(eLevel) < o.m_iMinLevel )
    // Do nothing
    return;

  if( o.m_setTypes.size() > 0 &&
      o.m_setTypes.find(String(pszType)) == o.m_setTypes.end() && 0 == eLevel )
    // Type not found in filter set
    return;
  
  // Notify log tarteg
  if( CurrentLogTarget() )
    CurrentLogTarget()->Log(eLevel, pszType, strMsg);
}

//=============================================================================
//
// DefaultLogHandler
//
//=============================================================================
//----------------------------------------------------------------------------
// DefaultLogHandler::Log
//----------------------------------------------------------------------------
void DefaultLogHandler::Log(ELogLevel eLevel, pcsz pszType, const String &strMsg)
{
  // Formatting message
  CurrentDate dtCur;
  String strLogDate = StrFormat("%4d-%02d-%02d,%02d:%02d:%02.3f", 
                            dtCur.Year(), dtCur.Month(), dtCur.Day(),
                            dtCur.Hour(), dtCur.Min(), dtCur.Sec());
  
  String strLevel;
  switch( eLevel )
  {
    case LOG_EMERGENCY:
      strLevel = "EMERGENCY";
      break;
    case LOG_ALERT:
      strLevel = "ALERT";
      break;
    case LOG_CRITICAL:
      strLevel = "CRITICAL";
      break;
    case LOG_ERROR:
      strLevel = "ERROR";
      break;
    case LOG_WARNING:
      strLevel = "WARNING";
      break;
    case LOG_NOTICE:
      strLevel = "NOTICE";
      break;
    case LOG_INFO:
      strLevel = "INFO";
      break;
    case LOG_VERBOSE:
      strLevel = "DEBUG";
      break;
    case LOG_LEVEL_COUNT:
    default:
      break;
  }
  std::clog << strLogDate << ' ' << strLevel << ' ' << '[' << pszType << "]:" << strMsg << '\n';
};

//=============================================================================
//
// LogCatcher
//
//=============================================================================
//----------------------------------------------------------------------------
// LogCatcher::LogCatcher
//----------------------------------------------------------------------------
LogCatcher::LogCatcher(ILogTarget *pLogTarget)
{
  LogManager::PushLogTarget(pLogTarget);
};

//----------------------------------------------------------------------------
// LogCatcher::~LogCatcher
//----------------------------------------------------------------------------
LogCatcher::~LogCatcher()
{
  LogManager::PopLogTarget();
};

//=============================================================================
//
// LogForward
//
//=============================================================================
//----------------------------------------------------------------------------
// LogForward::LogForward
//----------------------------------------------------------------------------
LogForward::LogForward(pfn_log_fwd pfn_log_fwd)
{
  m_pfn_log_fwd = pfn_log_fwd;
}

//----------------------------------------------------------------------------
// LogForward::Log
//----------------------------------------------------------------------------
void LogForward::Log(ELogLevel eLevel, pcsz pszType, const String &strMsg)
{
	if( m_pfn_log_fwd != NULL )
		m_pfn_log_fwd(eLevel, pszType, PSZ(strMsg));
}

//=============================================================================
// Macro for Log functions
//=============================================================================
#define LOG_MSG(level)                                          \
  va_list argptr;                                               \
  va_start(argptr, pszFormat);                                  \
  VSNPRINTF(g_acScratchBuf, g_iScratchLen, pszFormat, argptr);  \
  va_end(argptr);                                               \
  String strErr = g_acScratchBuf;                               \
  LogManager::Log(level, pszType, strErr);

// Log functions
void gdshare::LogEmergency(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_EMERGENCY) 
}

void gdshare::LogAlert(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_ALERT) 
}

void gdshare::LogCritical(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_CRITICAL) 
}

void gdshare::LogError(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_ERROR) 
}

void gdshare::LogWarning(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_WARNING) 
}

void gdshare::LogNotice(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_NOTICE) 
}

void gdshare::LogInfo(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_INFO) 
}

void gdshare::LogVerbose(pcsz pszType, pcsz pszFormat, ...)
{
#ifdef _THREAD_SUPPORT
  LOCK(&g_acScratchBuf)
#endif
 LOG_MSG(LOG_VERBOSE) 
}


//=============================================================================
//
// TemplateProcessor
//
//=============================================================================
//----------------------------------------------------------------------------
// TemplateProcessor::AddEvaluator
//----------------------------------------------------------------------------
void TemplateProcessor::AddEvaluator(IVariableEvaluator *pEvaluator)
{
  m_lstEvaluator.push_back(pEvaluator);
}

//----------------------------------------------------------------------------
// TemplateProcessor::RemoveEvaluator
//----------------------------------------------------------------------------
void TemplateProcessor::RemoveEvaluator(IVariableEvaluator *pEvaluator)
{
  std::list<IVariableEvaluator *>::iterator itEvaluator = m_lstEvaluator.begin();

  while( m_lstEvaluator.end() != itEvaluator )
  {
    if( *itEvaluator == pEvaluator )
    {
      m_lstEvaluator.erase(itEvaluator);
      break;
    }
    itEvaluator++;
  }
}

//----------------------------------------------------------------------------
// TemplateProcessor::ProcessNoRecursion
//----------------------------------------------------------------------------
bool TemplateProcessor::ProcessNoRecursion(String *pstrTemplate)
{
  String strEval, strTmp;
  String strTmpl = *pstrTemplate;
  String strVar, strValue;
  bool bNotReturnValue = false;

  while( strTmpl.size() > 0 )
  {
    // Search for a variable
    uint uiFirstPos = strTmpl.find("$(");
    if( String::npos != uiFirstPos )
    {
      // Search for matching ')'. Take care of nested variables
      uint uiMatchPos = strTmpl.find_first_of(')', uiFirstPos + 2);

      if( String::npos != uiMatchPos )
      {
        // complete result string
        strEval += strTmpl.substr(0, uiFirstPos);

        // Delete up to '$(' characters
        strTmpl.erase(0, uiFirstPos + 2);

        // Extract variable content
        strVar = strTmpl.substr(0, uiMatchPos - uiFirstPos - 2);
        // Delete up to matching end parenthesis
        strTmpl.erase(0, uiMatchPos - uiFirstPos - 1);

        // Variable evaluation
        bNotReturnValue = !ProcessVar(&strVar);
        strEval += strVar;
      }
      else
      {
        // Missing close bracket
        // Copying up to the end of template string
        strEval += strTmpl;
        strTmpl.erase();
      }
    }
    else
    {
      // Copying up to the end of template string
      strEval += strTmpl;
      strTmpl.erase();
    }
  }

  (*pstrTemplate) = strEval;
  return !bNotReturnValue;
}

//----------------------------------------------------------------------------
// TemplateProcessor::Process
//----------------------------------------------------------------------------
bool TemplateProcessor::Process(String *pstrTemplate, bool bRecurse)
{
  set<String> setSymbols;
  return PrivProcess(pstrTemplate, bRecurse, setSymbols);
}

//----------------------------------------------------------------------------
// TemplateProcessor::Process
//----------------------------------------------------------------------------
bool TemplateProcessor::PrivProcess(String *pstrTemplate, bool bRecurse, set<String> &setPrevSymbols)
{
  String strEval, strTmp;
  String strTmpl = *pstrTemplate;
  String strVar, strValue;
  bool bNotReturnValue = false;

  set<String> setSymbols = setPrevSymbols;

  while( strTmpl.size() > 0 )
  {
    // Search for a variable
    uint uiFirstPos = strTmpl.find("$(");
    if( String::npos != uiFirstPos )
    {
      // Search for matching ')'. Take care of nested variables
      uint uiMatchPos = uiFirstPos+1, nVar = 1;
      while( nVar > 0 )
      {
        uiMatchPos = strTmpl.find_first_of("()", uiMatchPos + 1);
        if( String::npos == uiMatchPos )
          break;
        if( ')' == strTmpl[uiMatchPos] )
          nVar--;
        else if( '$' == strTmpl[uiMatchPos-1] && '(' == strTmpl[uiMatchPos] )
          nVar++;
      }

      if( String::npos != uiMatchPos )
      {
        // complete result string
        strEval += strTmpl.substr(0, uiFirstPos);

        // Delete up to '$(' characters
        strTmpl.erase(0, uiFirstPos + 2);

        // Extract variable content
        strVar = strTmpl.substr(0, uiMatchPos - uiFirstPos - 2);
        // Delete up to matching end parenthesis
        strTmpl.erase(0, uiMatchPos - uiFirstPos - 1);

        if( bRecurse )
          // Evaluate variable name himself
          // Ignore return value since the important is the evaluation below
          PrivProcess(&strVar, true, setPrevSymbols);

        // Keep track of searched symbol for subsequents recursives calls
        setSymbols.insert(strVar);

        // Evaluate variable only if it hasn't been evaluated in a previous call during recursive process
        if( setPrevSymbols.find(strVar) == setPrevSymbols.end() )
          bNotReturnValue |= !PrivProcessVar(&strVar, bRecurse, true, setSymbols);

        // Variable evaluation
        strEval += strVar;
      }
      else
      {
        // Missing close bracket
        // Copying up to the end of template string
        strEval += strTmpl;
        strTmpl.erase();
      }
    }
    else
    {
      // Copying up to the end of template string
      strEval += strTmpl;
      strTmpl.erase();
    }
  }

  (*pstrTemplate) = strEval;
  return !bNotReturnValue;
}

//----------------------------------------------------------------------------
// TemplateProcessor::Process
//----------------------------------------------------------------------------
bool TemplateProcessor::ProcessVar(String *pstrVar)
{
  set<String> emptySet;
  return PrivProcessVar(pstrVar, false, false, emptySet);
}

//----------------------------------------------------------------------------
// TemplateProcessor::Process
//----------------------------------------------------------------------------
bool TemplateProcessor::PrivProcessVar(String *pstrVar, bool bRecurse, 
                          bool bDeepEvaluation, set<String> &setEvaluatedSymbols)
{
  std::list<IVariableEvaluator *>::iterator itEvaluator = m_lstEvaluator.begin();

  while( m_lstEvaluator.end() != itEvaluator )
  {
    if( (*itEvaluator)->Evaluate(pstrVar) )
    {
      if( bDeepEvaluation )
        // Evaluate result himself
        return PrivProcess(pstrVar, bRecurse, setEvaluatedSymbols);
      else
        return true;
    }
    itEvaluator++;
  }

  switch( m_eNotFoundReplacement )
  {
    case EMPTY_STRING:
      *pstrVar = "";
      break;
    case VARIABLE_NAME:
      break;
    case UNCHANGE_STRING:
      *pstrVar = string("$(") + *pstrVar + ')';
      break;
  }
  return false;
}

//=============================================================================
// MReferencable
//=============================================================================

//---------------------------------------------------------------------------
// MReferencable::MReferencable
//---------------------------------------------------------------------------
MReferencable::MReferencable()
{
  m_iUseCount = 0;
}

//---------------------------------------------------------------------------
// MReferencable::~MReferencable
//---------------------------------------------------------------------------
MReferencable::~MReferencable()
{
}

//---------------------------------------------------------------------------
// MReferencable::AddRef
//---------------------------------------------------------------------------
void MReferencable::AddRef() const
{
#ifdef _THREAD_SUPPORT
    LOCK((void *)this);
#endif
  ++m_iUseCount;
}

//---------------------------------------------------------------------------
// MReferencable::Release
//---------------------------------------------------------------------------
void MReferencable::Release() const
{
#ifdef _THREAD_SUPPORT
    LOCK((void *)this);
#endif

  if( 0 == --m_iUseCount )
    delete this;
}

//=============================================================================
// MMetadata
//=============================================================================
//------------------------------------------------------------------------
// MMetadata::AddMetadata
//------------------------------------------------------------------------
void MMetadata::AddMetadata(const String &strKey, const String &strValue)
{
  m_mapString[strKey] = strValue;
}
void MMetadata::AddMetadata(const String &strKey, const char *pszValue)
{
  if( pszValue )
    m_mapString[strKey] = String(pszValue);
  else
    throw NullPointerException("Null pointer passed as metadata value", "MMetadata::AddMetadata");
}
void MMetadata::AddMetadata(const String &strKey, int iValue)
{
  m_mapInteger[strKey] = iValue;
  // Also store metadata as string
  String strTmp;
  strTmp.Printf("%d", iValue);
  m_mapString[strKey] = strTmp;
}
void MMetadata::AddMetadata(const String &strKey, double dValue)
{
  m_mapDouble[strKey] = dValue;
  // Also store metadata as string
  String strTmp;
  strTmp.Printf("%g", dValue);
  m_mapString[strKey] = strTmp;
}

//------------------------------------------------------------------------
// MMetadata::HasMetadata
//------------------------------------------------------------------------
bool MMetadata::HasMetadata(const String &strKey) const
{
  std::map<String, String>::const_iterator it = m_mapString.find(strKey);
  if( it != m_mapString.end() )
    return true;
  return false;
}

//------------------------------------------------------------------------
// MMetadata::GetMetadata
//------------------------------------------------------------------------
bool MMetadata::GetMetadata(const String &strKey, String *pstrValue, bool bThrow) const
{
  std::map<String, String>::const_iterator it = m_mapString.find(strKey);
  if( it == m_mapString.end() )
  {
    if( bThrow )
      throw NoDataException("metadata not found", "MMetadata::GetMetadata");
    else
      return false;
  }
  *pstrValue = it->second;
  return true;
}

//------------------------------------------------------------------------
// MMetadata::GetIntegerMetadata
//------------------------------------------------------------------------
bool MMetadata::GetIntegerMetadata(const String &strKey, int *piValue, bool bThrow) const
{
  std::map<String, int>::const_iterator it = m_mapInteger.find(strKey);
  if( it == m_mapInteger.end() )
  {
    if( bThrow )
      throw NoDataException("metadata not found", "MMetadata::GetMetadata");
    else
      return false;
  }
  *piValue = it->second;
  return true;
}

//------------------------------------------------------------------------
// MMetadata::GetDoubleMetadata
//------------------------------------------------------------------------
bool MMetadata::GetDoubleMetadata(const String &strKey, double *pdValue, bool bThrow) const
{
  std::map<String, double>::const_iterator it = m_mapDouble.find(strKey);
  if( it == m_mapDouble.end() )
  {
    if( bThrow )
      throw NoDataException("metadata not found", "MMetadata::GetMetadata");
    else
      return false;
  }
  *pdValue = it->second;
  return true;
}

