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
#include "file.h"
#include "membuf.h"
#include "nxfile.h"
#include "variant.h"

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

typedef struct _LineToken
{
  pcsz pszToken1;                // Token
  TemplateToken::Type TokenType; // Token type
  bool bRemoveToken;
} SLineToken;

const int g_iTokenCount = 13;
SLineToken g_LineToken[g_iTokenCount] = 
{ 
  {"@(", TemplateToken::LOOP, true},
  {"@)", TemplateToken::END_LOOP, true},
  {"?(", TemplateToken::IF, true},
  {"?)", TemplateToken::END_IF, true},
  {"\"", TemplateToken::PRINT, false},
  {">", TemplateToken::OUTPUT, true},
  {"b>", TemplateToken::BINARY_OUTPUT, true},
  {"%", TemplateToken::SET, true},
  {"(", TemplateToken::BLOCK_START, true},
  {")", TemplateToken::BLOCK_END, true},
  {"...", TemplateToken::PADDING, true},
  {"'", TemplateToken::BINARY, false},
  {"+", TemplateToken::INCLUDE, true}
};

//-----------------------------------------------------------------------------
// TemplateFileParsor::TemplateFileParsor
//-----------------------------------------------------------------------------
TemplateFileParsor::TemplateFileParsor(VecToken *pvecToken)
                    :m_vecToken(*pvecToken), m_pVarEval(NULL)
{
  m_bHeaderParsed = false;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::SuppressComments
//-----------------------------------------------------------------------------
void TemplateFileParsor::SuppressComments(String *pstrLine)
{
  // Extract payload charge
  int iSearchPos = 0;
  while( true )
  {
    int iCom = pstrLine->find("//", iSearchPos);
    if( (uint)iCom != string::npos )
    {
      if( ((*pstrLine)[iCom - 1] == ' ' || (*pstrLine)[iCom - 1] == '\t') )
      {
        pstrLine->erase(iCom, pstrLine->size()-iCom);
          break;
      }
      iSearchPos = iCom + 1;
    }
    else
      break;
  }
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseSet
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParseSet(String *pstrLine, int iLine)
{
  TokenSet *pToken = new TokenSet;
  pToken->m_TokenType = TemplateToken::SET;

  // Looking for the commentaries to eliminate them.
  SuppressComments(pstrLine);

  // Parsing variable name
  String strVarName;
  pstrLine->ExtractToken('=' , &strVarName);

  pToken->m_strParam1 = strVarName;
  pToken->m_strParam1.Trim();
  (*pstrLine).Trim();
  pToken->m_bEvalArgument = true;
  if( pstrLine->Match("'*'") || pstrLine->Match("\"*\"") )
  {
    pToken->m_iTemplateLine = iLine;
    (*pstrLine).RemoveEnclosure("'\"", "'\"");
    pToken->m_bEvalArgument = false;
    pToken->m_strParam2 = *pstrLine; // value
  }
  else if( pstrLine->Match("[*]*") )
  {
    // This is a list
    pToken->m_TokenType = TemplateToken::SET_LIST;
    pstrLine->ExtractToken('[', ']', &pToken->m_strParam2);
    pToken->m_strParam3 = *pstrLine;
  }
  else
  {
    // Standard argument
    pToken->m_iTemplateLine = iLine;
    pToken->m_strParam2 = *pstrLine; // value
  }
  m_vecToken.push_back(TemplateTokenPtr(pToken));
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseOutput
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParseOutput(String *pstrLine, int iLine, bool bBinary)
{
  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_TokenType = TemplateToken::OUTPUT;

  // we look for the commentaries to eliminate them.
  SuppressComments(pstrLine);

  // Store parameters
  ptrToken->m_strParam1 = *pstrLine;
  ptrToken->m_strParam1.Trim();
  ptrToken->m_iTemplateLine = iLine;
  ptrToken->m_iParam1 = bBinary ? 1 : 0;

  m_vecToken.push_back(ptrToken);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseLoop
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParseLoop(String *pstrLine, int iLine)
{
  TokenLoop *pToken = new TokenLoop;

  pToken->m_iTemplateLine = iLine;
  
  // we look for the commentaries to eliminate them.
  SuppressComments(pstrLine);

  // Extract the name of iteration variable
  pstrLine->ExtractToken('=', &pToken->m_strParam1);
  // Next parameter
  int rc = pstrLine->ExtractToken(',', &pToken->m_strParam2);
  if( pstrLine->find(',') != string::npos )
    // Loop step
    pstrLine->ExtractTokenRight(',', &pToken->m_strParam4);

  // Suppress white spaces
  pToken->m_strParam1.Trim();
  pToken->m_strParam2.Trim();

  if( 2 == rc ) // token not found
  {
    pToken->m_TokenType = TemplateToken::LOOP_OVER;
    if( pToken->m_strParam1.empty() )
    {
      cerr << "Error: Missed variable name at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }

    if( pToken->m_strParam2.empty() )
    {
      cerr << "Error: Missed NeXus path at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }
    if( pToken->m_strParam2.Match("[*]") )
      pToken->m_strParam2 = pToken->m_strParam2.substr(1, pToken->m_strParam2.size()-2 );
    else
    {
      cerr << "Error: Bad loop syntax at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }
  }
  else
  {
    pToken->m_TokenType = TemplateToken::LOOP;
    // End value 
    pToken->m_strParam3 = *pstrLine;
    pToken->m_strParam3.Trim();
    if( pToken->m_strParam1.empty() )
    {
      cerr << "Error: Missed counter name at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }

    if( pToken->m_strParam2.empty() )
    {
      cerr << "Error: Missed initial value for counter at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }

    if( pToken->m_strParam3.empty() )
    {
      cerr << "Error: Missed final value for counter at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
      exit(1);
    }

    // Parse expressions
    pToken->ptrExprBegin = ParseExpression(pToken->m_strParam2);
    pToken->ptrExprEnd = ParseExpression(pToken->m_strParam3);
    if( !pToken->m_strParam4.empty() )
      pToken->ptrExprStep = ParseExpression(pToken->m_strParam4);
  }

  m_vecToken.push_back(pToken);
  return m_vecToken.size()-1;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseIfCond
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParseIfCond(String *pstrLine, int iLine)
{
  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_iTemplateLine = iLine;
  
  // Extract payload charge
  SuppressComments(pstrLine);
  pstrLine->Trim();

  String strCond, strParam2;
  uint uiPos = string::npos;
  if( string::npos == uiPos )
  {
    uiPos = pstrLine->rfind("!=");
    if( uiPos != string::npos )
    {
      ptrToken->m_TokenType = TemplateToken::IF_NOT_EQ;
      ptrToken->m_strParam1 = pstrLine->substr(0, uiPos);
      strParam2 = pstrLine->substr(uiPos + 2);
    }
  }
  if( string::npos == uiPos )
  {
    uiPos = pstrLine->rfind("=");
    if( uiPos != string::npos )
    {
      ptrToken->m_TokenType = TemplateToken::IF_EQ;
      ptrToken->m_strParam1 = pstrLine->substr(0, uiPos);
      strParam2 = pstrLine->substr(uiPos + 1);
    }
  }
  if( string::npos == uiPos )
  {
    uiPos = pstrLine->rfind(">");
    if( uiPos != string::npos )
    {
      ptrToken->m_TokenType = TemplateToken::IF_SUP;
      ptrToken->m_strParam1 = pstrLine->substr(0, uiPos);
      strParam2 = pstrLine->substr(uiPos + 1);
    }
  }
  if( string::npos == uiPos )
  {
    uiPos = pstrLine->rfind("<");
    if( uiPos != string::npos )
    {
      ptrToken->m_TokenType = TemplateToken::IF_INF;
      ptrToken->m_strParam1 = pstrLine->substr(0, uiPos);
      strParam2 = pstrLine->substr(uiPos + 1);
    }
  }

  ptrToken->m_strParam1.Trim();
  strParam2.Trim();

  // Remove quotes or double quotes if any
  strParam2.ExtractToken('\'', '\'', &ptrToken->m_strParam2);
  if( ptrToken->m_strParam2.empty() )
    strParam2.ExtractToken('"', '"', &ptrToken->m_strParam2);
  if( ptrToken->m_strParam2.empty() )
    // No quotes
    ptrToken->m_strParam2 = strParam2;

  m_vecToken.push_back(ptrToken);
  return m_vecToken.size()-1;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseIfExists
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParseIfExists(String *pstrLine, int iLine)
{
  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_TokenType = TemplateToken::IF_EXISTS;
  ptrToken->m_iTemplateLine = iLine;

  // Extract payload charge
  SuppressComments(pstrLine);

  // we parse the name of the NeXus Object.
  pstrLine->Trim();
  if( pstrLine->StartWith("nxs:") )
    ptrToken->m_strData = *pstrLine;
  else // Variable
    ptrToken->m_strParam1 = *pstrLine;
  m_vecToken.push_back(ptrToken);
  return m_vecToken.size()-1;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseIf
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParseIf(String *pstrLine, int iLine)
{
  // Search kind of 'if' statement
  pcsz pszLine = PSZ(*pstrLine);
  if( strstr(pszLine, "=") || strstr(pszLine, "!=") || strstr(pszLine, ">") || strstr(pszLine, "<") )
    return ParseIfCond(pstrLine, iLine);

  return ParseIfExists(pstrLine, iLine);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseBlockStart
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParseBlockStart(String *pstrLine)
{
  // Extract payload charge
  SuppressComments(pstrLine);
  pstrLine->Trim();

  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_TokenType = TemplateToken::BLOCK_START;

  // Get block name
  pstrLine->ExtractToken(' ', &ptrToken->m_strParam1); 
  // Get max block length
  ptrToken->m_strParam2 = *pstrLine;

  m_vecToken.push_back(ptrToken);
  return m_vecToken.size()-1;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParsePadding
//-----------------------------------------------------------------------------
int TemplateFileParsor::ParsePadding(String *pstrLine, int iLine)
{
  // Extract payload charge
  SuppressComments(pstrLine);
  pstrLine->Trim();

  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_TokenType = TemplateToken::PADDING;

  // Get Padding pattern
  ptrToken->m_strParam1 = *pstrLine;
  if( pstrLine->empty() )
    // default padding pattern : 80 blanks characters and a LF
    ptrToken->m_strParam1 = "                                                                                \n";
  else
  {
    int iRc = pstrLine->ExtractToken('"', '"', &ptrToken->m_strParam1);
    if( iRc != String::SEP_FOUND )
    {
      cerr << "Error: bad syntax at line " << iLine << " in file " << m_strCurrentTemplateFile << "; quotes missing." << endl;
      exit(1);
    }
  }

  m_vecToken.push_back(ptrToken);
  return m_vecToken.size()-1;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::BuildDataFragments
//-----------------------------------------------------------------------------
bool TemplateFileParsor::BuildDataFragments(TemplateToken *pToken, const String &strData)
{
  String strToFragment = strData;
  bool bVarDetected = false;

  while( strToFragment.size() > 0 )
  {
    // Search for a variable
    uint uiFirstPos = strToFragment.find("$(");
    if( String::npos != uiFirstPos )
    {
      // Search for matching ')'. Take care of nested variables
      uint uiMatchPos = strToFragment.find_first_of(')', uiFirstPos + 2);

      if( String::npos != uiMatchPos )
      {
        // New fragment
        if( uiFirstPos > 0 )
        {
          DataFragment fragText(DataFragment::TEXT, strToFragment.substr(0, uiFirstPos));
          pToken->lstDataFragments.push_back(fragText);
        }

        // Delete up to '$(' characters
        strToFragment.erase(0, uiFirstPos + 2);

        // Extract variable content
        DataFragment fragVar(DataFragment::VARIABLE, strToFragment.substr(0, uiMatchPos - uiFirstPos - 2));
        pToken->lstDataFragments.push_back(fragVar);

        // Delete up to matching end parenthesis
        strToFragment.erase(0, uiMatchPos - uiFirstPos - 1);

        bVarDetected = true;
      }
      else
      {
        // Missing close bracket
        // Copying up to the end of template string
        DataFragment fragText(DataFragment::TEXT, strToFragment);
        pToken->lstDataFragments.push_back(fragText);
        strToFragment.erase();
      }
    }
    else
    {
      // Copying up to the end of template string
      DataFragment fragText(DataFragment::TEXT, strToFragment);
      pToken->lstDataFragments.push_back(fragText);
      strToFragment.erase();
    }
  }
  return bVarDetected;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::GetTypeFromFormat
//-----------------------------------------------------------------------------
DataBuf::Type TemplateFileParsor::GetTypeFromFormat(char cType, char cMod)
{
  switch( cType )
  {
    case 's':
      return DataBuf::CHAR;
    case 'd':
      if( cMod == 'l')
        return DataBuf::LONG;
      else if( cMod == 'h')
        return DataBuf::SHORT;
      return DataBuf::INT;
    case 'e':
    case 'E':
    case 'f':
    case 'g':
    case 'G':
      return DataBuf::DOUBLE;
    case '?':
      return DataBuf::UNKNOWN;
    default:
      return DataBuf::UNKNOWN;
  };

  return DataBuf::UNKNOWN;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParsePrintData
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParsePrintData(String *pstrLine, int iLine)
{
  pstrLine->Trim();
  String strToPrint;
  int iRc = pstrLine->ExtractToken('"', '"', &strToPrint);

  CTemplateProcessor VarProc(CTemplateProcessor::EMPTY_STRING);
  if( m_pVarEval )
    VarProc.AddEvaluator(m_pVarEval);
  CEnvVariableEvaluator EnvVar;
  VarProc.AddEvaluator(&EnvVar);

  // Variable substitution in quoted strings !!
  VarProc.Process(&strToPrint);

  uint uiPos = 0;
  // looking for new line characters.
  for( uiPos = strToPrint.find("\\n"); uiPos != String::npos; uiPos = strToPrint.find("\\n"))
    strToPrint.replace(uiPos, 2, "\n");

  // looking for carriage return characters.
  for( uiPos = strToPrint.find("\\r"); uiPos != String::npos; uiPos = strToPrint.find("\\r"))
    strToPrint.replace(uiPos, 2, "\r");

  // looking for tab characters.
  for( uiPos = strToPrint.find("\\t"); uiPos != String::npos; uiPos = strToPrint.find("\\t"))
    strToPrint.replace(uiPos, 2, "\t");

  if( 1 == iRc )
  {
    while( !strToPrint.empty() )
    {
      String strTxt, strTmp;
      // Search for next format directive
      bool bParse = true;
      while( bParse )
      {
        iRc = strToPrint.ExtractToken('%', &strTmp);
        // Skip directive indicator if it's a '%%' pair
        if( strToPrint.StartWith('%') )
        {
          strTmp += '%';
          strToPrint.erase(0, 1);
        }
        else
          bParse = false;
        strTxt += strTmp;
      }

      if( !strTxt.empty() )
      {
        // Text to print before format directive
        TemplateTokenPtr ptrToken(new TemplateToken);
        ptrToken->m_iTemplateLine = iLine;
        ptrToken->m_TokenType = TemplateToken::PRINT_TEXT;
        ptrToken->m_strData = strTxt;
        ptrToken->m_Format.Set("%s");
        m_vecToken.push_back(ptrToken);
      }

      if( 1 == iRc )
      {
        // A format directive was found
        SuppressComments(pstrLine);
        String strArgument;
        int iRc2 = pstrLine->ExtractToken(',', &strArgument);
        if( 0 == iRc2 )
        {
          cerr << "Error: Argument missing at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
          exit(1);
        }
        // Remove blanks
        strArgument.Trim();

        // Check argument type
        if( strArgument.Match("[nxs:*<*>]*") )
        {
          // This is a array type argument
          // Erase '[' and ']' enclosers
          strArgument.erase(0, 1); // '['
          uint uiMatchingPos = strArgument.find(']');
          strArgument.erase(uiMatchingPos, 1); // ']'

          // Insert a loop/end-loop couple
          TemplateTokenPtr ptrLoopToken(new TemplateToken);
          ptrLoopToken->m_iTemplateLine = iLine;
          ptrLoopToken->m_TokenType = TemplateToken::LOOP_OVER;
          ptrLoopToken->m_strParam1 = "loopvar";

          // Extract loop part of the argument
          String strLastPart;
          int iLastPart = strArgument.rfind('>');
          strLastPart = strArgument.substr(iLastPart+1);
          ptrLoopToken->m_strParam2 = strArgument.substr(0, iLastPart+1);
          ptrLoopToken->m_iEndBlockPos = m_vecToken.size() + 1;
          // Add 'loop' Token
          m_vecToken.push_back(ptrLoopToken);

          // Replace loop element with variable access
          String strTmp;
          strArgument.ExtractTokenRight('<', '>', &strTmp);
          strArgument += "$(loopvar_name)" + strLastPart;
        }
        else if( strArgument.Match("*[$(*)]*") )
        {
          uint uiMatchingPos = strArgument.find("[$(");
          uint uiLastPos = strArgument.find("]", uiMatchingPos);
          String strBeforePattern = strArgument.substr(0, uiMatchingPos);
          String strAfterPattern = strArgument.substr(uiLastPos + 1);
          String strMatchedPattern = strArgument.substr(uiMatchingPos + 1, uiLastPos - uiMatchingPos - 1);

          // Insert a loop/end-loop couple
          TemplateTokenPtr ptrLoopToken(new TemplateToken);
          ptrLoopToken->m_iTemplateLine = iLine;
          ptrLoopToken->m_TokenType = TemplateToken::LOOP_OVER;
          ptrLoopToken->m_strParam1 = "loopvar";

          ptrLoopToken->m_strParam2 = strMatchedPattern;
          ptrLoopToken->m_iEndBlockPos = m_vecToken.size() + 1;
          // Add 'loop' Token
          m_vecToken.push_back(ptrLoopToken);
          // Rebuild argument for next token (inside loop)
          strArgument = strBeforePattern + "$(loopvar_name)" + strAfterPattern;
        }

        TemplateTokenPtr ptrToken(new TemplateToken);
        ptrToken->m_iTemplateLine = iLine;
        ptrToken->m_TokenType = TemplateToken::PRINT_DATA;

        if( false == BuildDataFragments(ptrToken.ObjectPtr(), strArgument) )
          // If no variable are presents in the argument string then fill strData member
          ptrToken->m_strData = strArgument;
        
        // Extend format string up to next format directive
        int iPos = strToPrint.find('%');
        // Parse format
        ptrToken->m_Format.Set('%' + strToPrint.substr(0, iPos));
        // Search for format type in first 10 characters
        DataBuf::Type eType = GetTypeFromFormat(ptrToken->m_Format.Type(), ptrToken->m_Format.Modifier());
        if( DataBuf::NO_TYPE == eType )
        {
          cerr << "Error: Bad type specification at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
          exit(1);
        }
        ptrToken->m_eOutputType = eType;
        ptrToken->m_strPrintFmt = ptrToken->m_Format.Get();

        m_vecToken.push_back(ptrToken);
        strToPrint.erase(0, iPos);
      }
    }
  }
  else
  {
    cerr << "Error: Missed '\"' at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseBinary
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParseBinary(String *pstrLine, int iLine)
{
  if( pstrLine->StartWith("binary", true) )
    pstrLine->erase(0, strlen("binary"));
  pstrLine->Trim();

  String strBinaryFormat;
  int iRc = pstrLine->ExtractToken('\'', '\'', &strBinaryFormat);
  pstrLine->Trim();

  if( iRc != EXTRACT_TOKEN_FOUND )
  {
    cerr << "Error: Syntax error at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
    exit(1);
  }

  TemplateTokenPtr ptrToken(new TemplateToken);
  ptrToken->m_iTemplateLine = iLine;
  ptrToken->m_TokenType = TemplateToken::BINARY;

  if( false == BuildDataFragments(ptrToken.ObjectPtr(), *pstrLine) )
    // If no variable are presents in the argument string then fill strData member
    ptrToken->m_strData = *pstrLine;

  if( strBinaryFormat == "c" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::CHAR;
    ptrToken->m_uiTypeSize = 1;
  }
  else if( strBinaryFormat == "i1" || strBinaryFormat == "bt" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::BYTE;
    ptrToken->m_uiTypeSize = 1;
  }
  else if( strBinaryFormat == "i2" || strBinaryFormat == "si")
  {
    ptrToken->m_eBinaryDataType = TemplateToken::SHORT;
    ptrToken->m_uiTypeSize = sizeof(short);
  }
  else if( strBinaryFormat == "ui2" || strBinaryFormat == "usi")
  {
    ptrToken->m_eBinaryDataType = TemplateToken::USHORT;
    ptrToken->m_uiTypeSize = sizeof(unsigned short);
  }
  else if( strBinaryFormat == "i4" || strBinaryFormat == "li" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::LONG;
    ptrToken->m_uiTypeSize = sizeof(long);
  }
  else if( strBinaryFormat == "ui4" || strBinaryFormat == "uli" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::LONG;
    ptrToken->m_uiTypeSize = sizeof(unsigned long);
  }
  else if( strBinaryFormat == "f4" || strBinaryFormat == "f" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::FLOAT;
    ptrToken->m_uiTypeSize = sizeof(float);
  }
  else if( strBinaryFormat == "f8" || strBinaryFormat == "lf" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::DOUBLE;
    ptrToken->m_uiTypeSize = sizeof(double);
  }
  else if( strBinaryFormat == "b" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::RAW;
    ptrToken->m_uiTypeSize = 0;
  }
  else if( strBinaryFormat == "jpeg" )
  {
#ifdef __JPEG_SUPPORT__
    ptrToken->m_eBinaryDataType = TemplateToken::JPEG_IMAGE;
    ptrToken->m_uiTypeSize = 0;
#else
    cerr << "Error: jpeg output not supported in this version (line: " << iLine << " in file " << m_strCurrentTemplateFile << ")." << endl;
    exit(1);
#endif
  }
  else if( strBinaryFormat == "bmp" )
  {
    ptrToken->m_eBinaryDataType = TemplateToken::BMP_IMAGE;
    ptrToken->m_uiTypeSize = 0;
  }
  else
  {
    cerr << "Error: unknown binary format '" << strBinaryFormat << "' at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
    exit(1);
  }
  m_vecToken.push_back(ptrToken);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::Parse
//-----------------------------------------------------------------------------
bool TemplateFileParsor::IsLoopToken(const TemplateToken &aToken)
{
  if( TemplateToken::LOOP_OVER == aToken.m_TokenType ||
      TemplateToken::LOOP == aToken.m_TokenType )
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::Parse
//-----------------------------------------------------------------------------
void TemplateFileParsor::Parse(const string &strTemplateName, IVariableEvaluator *pVarEval)
{
  m_pVarEval = pVarEval;
  m_fnMainTemplate.Set(strTemplateName);
  m_strCurrentTemplateFile = strTemplateName;
  Parse(strTemplateName);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::Parse
//-----------------------------------------------------------------------------
void TemplateFileParsor::Parse(const string &strTemplateName)
{
  m_strCurrentTemplateFile = strTemplateName;
  if( !FileExists(PSZ(m_strCurrentTemplateFile)) )
  {
    // Try with name relative to main template
    m_strCurrentTemplateFile = FileName(m_fnMainTemplate.Path() + m_strCurrentTemplateFile).FullName();
    if( !FileExists(PSZ(m_strCurrentTemplateFile)) )
    {
      cerr << "File " << strTemplateName << "don't exists" << endl;
      exit(1);
    }
  }

  // Build tokens tree
  ifstream fs(PSZ(m_strCurrentTemplateFile));

  int iLine = 0;
  // Tokenize
  Tokenize(fs, iLine);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ReadHeader
//-----------------------------------------------------------------------------
void TemplateFileParsor::ReadHeader(const string &strTemplateName, String *pstrParams)
{
  // Build template lines list
  ifstream fs(PSZ(strTemplateName));
  int iLine = 0;
  
  while( !fs.eof() )
  {
    iLine++;
    fs.getline(g_acScratchBuf, g_iScratchLen);
    String strLine = g_acScratchBuf;
    strLine.Trim();

    if( !strLine.StartWith("#") )
      // End of header reached
      return;

    // Header parsing
    // Search for lines begining with "# @param..."
    strLine.erase(0, 1);
    strLine.Trim();
    if( strLine.StartWith("@param") )
    {
      strLine.erase(0, strlen("@param"));
      strLine.Trim();

      // Get name
      String strName;
      strLine.ExtractToken(' ', &strName);
      strName.Trim();
      strLine.Trim();
      
      // Pass type;
      String strTmp;
      strLine.ExtractToken(' ', &strTmp);
      strLine.Trim();

      // Pass description
      strLine.ExtractToken('\'', '\'', &strTmp);
      strLine.Trim();
      *pstrParams += strName + string("=") + strLine + string(",");
    }
  }
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseEndBlock
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParseEndBlock(TemplateToken::Type eExpectedType, 
                                        TemplateToken::Type eTokenType,
                                        int iLine)
{
  if( eExpectedType != eTokenType )
  {
    cerr << "Error : end block mismatching at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParsePreprocessorInclude
//-----------------------------------------------------------------------------
void TemplateFileParsor::ParsePreprocessorInclude(String *pstrLine)
{
  if( pstrLine->StartWith("include", true) )
    pstrLine->erase(0, strlen("include"));
  if( pstrLine->StartWith("+", true) )
    pstrLine->erase(0, strlen("+"));
  pstrLine->Trim();

  String strFileName;
  pstrLine->ExtractToken('\'', '\'', &strFileName);
  Parse(strFileName);
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::GetLineToken
//-----------------------------------------------------------------------------
TemplateToken::Type TemplateFileParsor::GetLineToken(String &strLine)
{
  for( int i = 0; i < g_iTokenCount; i++ )
  {
    if( strLine.StartWith(g_LineToken[i].pszToken1) )
    {
      if( g_LineToken[i].bRemoveToken )
        strLine.erase(0, strlen(g_LineToken[i].pszToken1) );
      return g_LineToken[i].TokenType;
    }
  }
  return TemplateToken::NO_TYPE;
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::Tokenize
//-----------------------------------------------------------------------------
void TemplateFileParsor::Tokenize(ifstream &fs, int &iLine, TemplateToken::Type eEndBlockToken)
{
  while( !fs.eof() )
  {
    iLine++;
    fs.getline(g_acScratchBuf, g_iScratchLen);
    String strLine = g_acScratchBuf;
    Trim(&strLine);
    // Merge splitted lines (with '\')
    while( EndWith(strLine, '\\') )
    {
      iLine++;
      strLine.erase(strLine.size()-1);
      fs.getline(g_acScratchBuf, g_iScratchLen);
      String strPartLine = g_acScratchBuf;
      Trim(&strPartLine);
      strLine += strPartLine;
    }

    // Replace tabs with spaces
    strLine.Replace('\t', ' ');

    if( strLine.size() == 0 || strLine.StartWith("//") || strLine.StartWith("#") )
      // Empty line
      continue;

    int iTokenType = (int)GetLineToken(strLine);
    switch( iTokenType )
    {
      case TemplateToken::PRINT:
        ParsePrintData(&strLine, iLine);
        break;
      case TemplateToken::LOOP:
      {
        int iPos = ParseLoop(&strLine, iLine);
        Tokenize(fs, iLine, TemplateToken::END_LOOP);
        // Store end block position in token vector
        m_vecToken[iPos]->m_iEndBlockPos = m_vecToken.size() - 1;
        break;
      }
      case TemplateToken::IF:
      {
        int iPos = ParseIf(&strLine, iLine);
        Tokenize(fs, iLine, TemplateToken::END_IF);
        // Store end block position in token vector
        m_vecToken[iPos]->m_iEndBlockPos = m_vecToken.size() - 1;
        break;
      }
      case TemplateToken::BLOCK_START:
        {
          int iPos = ParseBlockStart(&strLine);
          Tokenize(fs, iLine, TemplateToken::BLOCK_END);
          // Store end block position in token vector
          m_vecToken[iPos]->m_iEndBlockPos = m_vecToken.size() - 1;
          break;
        }
      case TemplateToken::END_LOOP:
        ParseEndBlock(eEndBlockToken, TemplateToken::END_LOOP, iLine);
        return;
      case TemplateToken::END_IF:
        ParseEndBlock(eEndBlockToken, TemplateToken::END_IF, iLine);
        return;
      case TemplateToken::BLOCK_END:
        ParseEndBlock(eEndBlockToken, TemplateToken::BLOCK_END, iLine);
        return;
      case TemplateToken::SET:
        ParseSet(&strLine, iLine);
        break;
      case TemplateToken::OUTPUT:
        ParseOutput(&strLine, iLine, false);
        break;
      case TemplateToken::BINARY_OUTPUT:
        ParseOutput(&strLine, iLine, true);
        break;
      case TemplateToken::BINARY:
        ParseBinary(&strLine, iLine);
        break;
      case TemplateToken::PADDING:
        ParsePadding(&strLine, iLine);
        break;
      case TemplateToken::INCLUDE:
        ParsePreprocessorInclude(&strLine);
        break;
      default:
        cerr << "Error: Bad syntax at line " << iLine << " in file " << m_strCurrentTemplateFile << "." << endl;
        exit(1);
    }
  }

  if( eEndBlockToken != TemplateToken::NO_TYPE )
  {
    // End of file reached while parsing a 'loop' or 'if' block
    cerr << "Error : Unexpected end of template file (" << m_strCurrentTemplateFile << ")." << endl;
    exit(1);
  }
}

//-----------------------------------------------------------------------------
// TemplateFileParsor::ParseExpression
//-----------------------------------------------------------------------------
ExpressionPtr TemplateFileParsor::ParseExpression(const String &_strExpr)
{
  // Very basic math expression parsor:
  // - can only parse expression like 'a + b - c * d / e' without parenthesis
  // - spaces characters must enclose operators
  ExpressionPtr ptrExpression = new Expression();

  String strExpr = _strExpr;
  Expression::Operation opPrevious = Expression::CharToOp('N');
  uint uiStartSearch = 0;
  while( !strExpr.empty() )
  {
    int iOpPos = strExpr.find_first_of("+-/*", uiStartSearch);

    if( string::npos != iOpPos )
    {
      String strMatching = strExpr.substr(iOpPos-1,3);
      // Operator must be enclosed by spaces characters
      if( strMatching.Match(" * ") )
      { 
        char cOp = strExpr[iOpPos];
        String strToken;
        // Extract first operand
        strExpr.ExtractToken(cOp, &strToken);
        strToken.Trim();
        ptrExpression->AddFragment(new Expression::Fragment(new Expression::Constant(strToken), opPrevious));
        opPrevious = Expression::CharToOp(cOp);
      }
      else
      { // Look forward
        uiStartSearch = iOpPos + 1;
      }
    }
    else
    {
      strExpr.Trim();
      ptrExpression->AddFragment(new Expression::Fragment(new Expression::Constant(strExpr), opPrevious));
      break;
    }
  }
  return ptrExpression;
}
