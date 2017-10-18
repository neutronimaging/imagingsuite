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

#ifndef __TEMPLATEFILEPARSOR_H__
#define __TEMPLATEFILEPARSOR_H__


//-----------------------------------------------------------------------------
// Referencable String
//
//-----------------------------------------------------------------------------
class RefString:public String, MReferencable {};

//-----------------------------------------------------------------------------
// class TemplateLine
//
//-----------------------------------------------------------------------------
class TemplateLine
{
public:
  int iLine;
  RefString refFileName;
  String strLine;
};

//-----------------------------------------------------------------------------
// Class TemplateFileParsor
//
//-----------------------------------------------------------------------------
class TemplateFileParsor
{
private:
  VecToken           &m_vecToken;       // Tokenized template file
  bool                m_bSilentMode;    // If true disable warnings
  IVariableEvaluator *m_pVarEval;       // Variable evaluator
  bool                m_bHeaderParsed;  // 'true' if header was parsed
  FileName            m_fnMainTemplate; // Main template file
  String              m_strCurrentTemplateFile;

  void SuppressComments(String *pstrLine);
  bool IsLoopToken(const TemplateToken &aToken);

  // Get type id according to specified print format spec
  DataBuf::Type GetTypeFromFormat(char cType, char cMod);

  // Parsing
  TemplateToken::Type GetLineToken(String &strLine);
  void Tokenize(ifstream &fs, int &iLine, TemplateToken::Type eBlockType=TemplateToken::NO_TYPE);
  bool BuildDataFragments(TemplateToken *pToken, const String &strData);
  void ParsePrintData(String *pstrLine, int iLine);
  void ParseBinary(String *pstrLine, int iLine);
  void ParseOutput(String *pstrLine, int iLine, bool bBinary);
  void ParseSet(String *pstrLine, int iLine);
  int  ParseLoop(String *pstrLine, int iLine);
  int  ParseIf(String *pstrLine, int iLine);
  int  ParseIfExists(String *pstrLine, int iLine);
  int  ParseIfCond(String *pstrLine, int iLine);
  int  ParseBlockStart(String *pstrLine);
  int  ParsePadding(String *pstrLine, int iLine);
  void ParseEndBlock(TemplateToken::Type eExpectedType, 
                     TemplateToken::Type eTokenType,
                     int iLine);
  void ParsePreprocessorInclude(String *pstrLine);

public:
  // @param pstrVar Variable to evaluate
  // @return true if evaluation is done, or false
  TemplateFileParsor(VecToken *pvecToken);

  // Set silent mode
  void SetSilent(bool bSilent=true) { m_bSilentMode = bSilent; }

  // Parsing and script executing
  void Parse(const string &strTemplateName, IVariableEvaluator *pVarEval);
  void Parse(const string &strTemplateName);
  void ReadHeader(const string &strTemplateName, String *pstrParams);


  static ExpressionPtr ParseExpression(const String &strExpr);
};

#endif
