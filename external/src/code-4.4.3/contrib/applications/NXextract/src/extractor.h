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

#ifndef __EXTRACTOR_H__
#define __EXTRACTOR_H__

//-----------------------------------------------------------------------------
// Class PrintFormat
//-----------------------------------------------------------------------------
class PrintFormat
{
private:
  String m_strWidth;    // min chars to output
  char   m_cFlag;       // -, +, # or ' '
  int    m_iPrecision;  // for floating point representations
  char   m_cModifier;   // h, l, or L
  char   m_cType;       // one of these chars : cdieEfgGosuxXpn?
  String m_strRest;     // will be printed together with argument

  void Init() { m_cFlag = m_cModifier = m_cType = m_iPrecision = -1; }

public:
  // Constructors
  PrintFormat();
  PrintFormat(const String &strFormat);

  // Modifiers
  void Set(const String &strFormat);
  void SetFlag(char cFlag) { m_cFlag = cFlag; }
  void SetWidth(const String &strWidth) { m_strWidth = strWidth; }
  void SetPrecision(int iPrecision) { m_iPrecision = iPrecision; }
  void SetModifier(char cModifier) { m_cModifier = cModifier; }
  void SetType(char cType) { m_cType = cType; }

  char Type() const { return m_cType; }
  char Modifier() const { return m_cModifier; }

  // Get format string in the form %[flags][width][.precision][modifiers]type[rest]
  String Get();
  String GetNoRest();
};

//-----------------------------------------------------------------------------
// Class DataBuf
//-----------------------------------------------------------------------------
class DataBuf
{
public:
  enum Type
  { 
    CHAR = 0,
    UBYTE,
    BYTE,
    SHORT,
    USHORT,
    LONG,
    ULONG,
    INT,
    FLOAT,
    DOUBLE,
    UNKNOWN,
    NO_TYPE
  };

private:
  MemBufPtr m_ptrmb;
  Type m_eType;
  uint  m_uiTypeSize; // in octets
  String m_strFormat; // print format, used if no format is given
  int    m_aiDim[32];    // size of each dimensions
  int    m_iRank; // number of dimensions

public:

  DataBuf(): m_ptrmb(new CMemBuf) { memset(m_aiDim, 0, 32*sizeof(int)); }
  void SetTypeFromNeXusType(int iNxType);

  inline void *Buf() { return (void *)(m_ptrmb->Buf()); }
  inline MemBufPtr MemBufRef() { return m_ptrmb; }
  inline int Len() { return m_ptrmb->Len(); }
  inline void Reset() { m_ptrmb->Reset(); }
  inline void Empty() { m_ptrmb->Empty(); }
  inline Type &DataType() { return m_eType; }
  inline uint TypeSize() { return m_uiTypeSize; }
  inline void SetRank(int iRank) { m_iRank = iRank; }
  inline void SetSize(int iDim, int iSize) { m_aiDim[iDim] = iSize; }
  inline int Rank() { return m_iRank; }
  inline int GetDimSize(int iDim) { return m_aiDim[iDim]; }
};

//-----------------------------------------------------------------------------
// class SDataFragment
// to boost performances requests are divided in several fragments which store
// references to variables and plain text
// a full request is the concatenation of all the snippets
//-----------------------------------------------------------------------------
class DataFragment
{
public:
  enum Type
  {
    VARIABLE = 0,
    TEXT,
  };

private:
  Type   m_eType;
  String m_strContent;

public:
  DataFragment(Type eType, const String &strContent) : m_eType(eType), m_strContent(strContent) {}
  inline Type Type() const { return m_eType; }
  inline const String &Content() const { return m_strContent; }
};
typedef list<DataFragment> DataFragmentsList;

//-----------------------------------------------------------------------------
// Interface IOperand
//
// Used in Expression class (see below)
//-----------------------------------------------------------------------------
class IOperand: public MReferencable
{
public:
  virtual Variant GetValue() = 0;
};

typedef RefPtr<IOperand> OperandPtr;
typedef list<OperandPtr> OperandPtrList;

//-----------------------------------------------------------------------------
// Interface IValueEvaluator
// 
// Used in Expression class (see below)
//-----------------------------------------------------------------------------
class IValueEvaluator
{
public:
  virtual Variant Evaluate(const String &str) = 0;
};

//-----------------------------------------------------------------------------
// Class Expression
//
// Basic expression evaluator:
// - can do recursive evaluation
// - all values are converted to double float type
// - unary operators not implemented yet
//-----------------------------------------------------------------------------
class Expression : public IOperand
{
public:
  enum Operation
  {
    NOP = 0, // no operation
    ADD, 
    SUB,
    MUL,
    DIV
  };

  // Object that can evaluate a value like a variable
  static IValueEvaluator *s_pValueEval;

  // Convenience method: return Op code from char in [+-*/]
  static Operation CharToOp(char c);

  // Class Constant
  //
  class Constant: public IOperand
  {
  private:
    Variant m_vValue; // The value
    TemplateProcessor *m_pTmplProc;
  public:

    Constant(const Variant &v): m_vValue(v) {}
    virtual ~Constant();

    // IOperand
    virtual Variant GetValue();
  };

  // Class Fragment
  //
  class Fragment: public MReferencable
  {
  private:
    OperandPtr m_ptrOperand;
    Operation m_opUnary;
    Operation m_opBinary;

  public:
    Fragment(OperandPtr ptrOperand, Operation Op2=NOP, Operation Op1=NOP)
    { m_ptrOperand = ptrOperand; m_opBinary = Op2; m_opUnary = Op1; }

    Operation UnaryOp() const { return m_opUnary; }
    Operation BinaryOp() const { return m_opBinary; }
    Variant Value() { return m_ptrOperand->GetValue(); }
  };
  typedef RefPtr<Fragment> FragmentPtr;

private:
  
  list<FragmentPtr> m_lstFragment;

public:

  virtual ~Expression();

  void AddFragment(FragmentPtr ptrFragment) { m_lstFragment.push_back(ptrFragment); }

  double Eval();

  // IOperand
  virtual Variant GetValue();
};
typedef RefPtr<Expression> ExpressionPtr;

//-----------------------------------------------------------------------------
// class TemplateToken
//-----------------------------------------------------------------------------
class TemplateToken : public MReferencable
{
public:
  enum Type
  {
    PRINT = 0,
    PRINT_DATA,
    PRINT_TEXT,
    LOOP,
    LOOP_OVER,
    END_LOOP,
    OUTPUT,
    SET,
    SET_LIST,
    IF,
    IF_EXISTS,
    IF_SUP,
    IF_INF,
    IF_EQ,
    IF_NOT_EQ,
    END_IF,
    BLOCK_START,
    BLOCK_END,
    BINARY,
    BINARY_OUTPUT,
    PADDING,
    INCLUDE,
    NO_TYPE
  };

  // Used to dump in binary format
  enum BinaryDataType
  {
    CHAR = 0,
    UBYTE,
    BYTE,
    SHORT,
    USHORT,
    LONG,
    ULONG,
    INT,
    FLOAT,
    DOUBLE,
    RAW,
    JPEG_IMAGE,
    BMP_IMAGE
  };

  Type   m_TokenType;
  uint   m_uiTypeSize;
  int    m_iEndBlockPos;
  int    m_iTemplateLine;
  String m_strTemplateFile;
  int    m_iParam1;    // block max length
  BinaryDataType m_eBinaryDataType;
  String m_strPrintFmt;
  PrintFormat m_Format;
  DataBuf::Type m_eOutputType;
  DataFragmentsList lstDataFragments;
  String m_strData;
  String m_strParam1;  // loop on / output file
  String m_strParam2;  // loop begin
  String m_strParam3;  // loop end
  String m_strParam4;  // loop step

  virtual ~TemplateToken() {}
};

//-----------------------------------------------------------------------------
// struct TemplateToken
//-----------------------------------------------------------------------------
class TokenLoop : public TemplateToken
{
public:
  ExpressionPtr ptrExprBegin, ptrExprEnd, ptrExprStep;
};

typedef RefPtr<TemplateToken> TemplateTokenPtr;
typedef vector<TemplateTokenPtr> VecToken;

//-----------------------------------------------------------------------------
// class TokenSet
//-----------------------------------------------------------------------------
class TokenSet: public TemplateToken
{
public:
  bool m_bEvalArgument;
};

//===========================================================================
/// Interface for counter variables type
///
/// This class is a interface so it can't be directly instancied
//===========================================================================
class ICounterVars
{
protected:
  /// Constructor
  ICounterVars() { }

public:
  /// Attempts to evaluate a counter value 
  ///
  /// @param pstrVar Counter to get value from
  ///
  virtual bool GetValue(const String &strVar, int *piValue)=0;
};

//===========================================================================
/// Interface for data output
///
/// This class is a interface so it can't be directly instancied
//===========================================================================
class IDataOutput : public MReferencable
{
protected:
  IDataOutput() {}

public:
  virtual ~IDataOutput() {}
  virtual void Out(const char *pcszFormat, const char *psz) =0;
  virtual void Out(const char *pcszFormat, char c) =0;
  virtual void Out(const char *pcszFormat, uint8 ui8) =0;
  virtual void Out(const char *pcszFormat, short s) =0;
  virtual void Out(const char *pcszFormat, unsigned short s) =0;
  virtual void Out(const char *pcszFormat, long l) =0;
  virtual void Out(const char *pcszFormat, unsigned long l) =0;
  virtual void Out(const char *pcszFormat, int i) =0;
  virtual void Out(const char *pcszFormat, float f) =0;
  virtual void Out(const char *pcszFormat, double d) =0;
  virtual void Out(void *pData, int iDataLen) =0;
};

typedef RefPtr<IDataOutput> DataOutputPtr;
typedef stack<DataOutputPtr> DataOutputPtrStack;

//===========================================================================
/// Output data to standard output
///
//===========================================================================
class StandardOutput : public IDataOutput
{
public:
  StandardOutput() {}
  void Out(const char *pcszFormat, const char *psz);
  void Out(const char *pcszFormat, char c);
  void Out(const char *pcszFormat, uint8 ui8);
  void Out(const char *pcszFormat, short s);
  void Out(const char *pcszFormat, unsigned short s);
  void Out(const char *pcszFormat, long l);
  void Out(const char *pcszFormat, unsigned long l);
  void Out(const char *pcszFormat, int i);
  void Out(const char *pcszFormat, float f);
  void Out(const char *pcszFormat, double d);
  void Out(void *pData, int iDataLen);
};

//===========================================================================
/// Output data to standard output
///
//===========================================================================
class MemBufOutput : public IDataOutput
{
private:
  MemBufPtr m_ptrMemBuf;
  int       m_iPaddingPos;
  String   m_strPaddingPattern;

public:
  MemBufOutput(MemBufPtr ptrMemBuf) : m_ptrMemBuf(ptrMemBuf), m_iPaddingPos(-1) {}
  void Out(const char *pcszFormat, const char *psz);
  void Out(const char *pcszFormat, char c);
  void Out(const char *pcszFormat, uint8 ui8);
  void Out(const char *pcszFormat, short s);
  void Out(const char *pcszFormat, unsigned short s);
  void Out(const char *pcszFormat, long l);
  void Out(const char *pcszFormat, unsigned long l);
  void Out(const char *pcszFormat, int i);
  void Out(const char *pcszFormat, float f);
  void Out(const char *pcszFormat, double d);
  void Out(void *pData, int iDataLen);
  void SetPaddingPattern(const String &strPattern) { m_iPaddingPos = m_ptrMemBuf->Len(); m_strPaddingPattern = strPattern; }
  int PaddingPos() const { return m_iPaddingPos; }
  const String &PaddingPattern() const { return m_strPaddingPattern; }
};

//===========================================================================
/// Output data to binary file
///
//===========================================================================
class BinaryFileOutput : public IDataOutput
{
private:
  FILE    *m_pFile;

public:
  BinaryFileOutput(FILE *pFile): m_pFile(pFile) {}
  ~BinaryFileOutput();

  void Out(const char *pcszFormat, const char *psz);
  void Out(const char *pcszFormat, char c);
  void Out(const char *pcszFormat, uint8 ui8);
  void Out(const char *pcszFormat, short s);
  void Out(const char *pcszFormat, unsigned short s);
  void Out(const char *pcszFormat, long l);
  void Out(const char *pcszFormat, unsigned long l);
  void Out(const char *pcszFormat, int i);
  void Out(const char *pcszFormat, float f);
  void Out(const char *pcszFormat, double d);
  void Out(void *pData, int iDataLen);
};

typedef map<String, int> MapCounters;
typedef map<String, MemBufPtr> MapBufPtr;

//-----------------------------------------------------------------------------
// Class CTemplateFileParsor
//
//-----------------------------------------------------------------------------
class Extractor : public IVariableEvaluator, public ICounterVars, public IValueEvaluator
{
private:
  VecToken           &m_vecToken;       // Tokenized template file
  StringDict          m_dictVar;        // Variables dictionnary
  MapCounters         m_dictCounters;   // Counters dictionnary
  CTemplateProcessor  m_TempParsor;     // Template parsor (used to evaluate variables)
  NexusEvaluator     *m_pNxEval;        // Nexus data evaluator
  DataBuf             m_aValue;         // Objet used to transport output values
  FILE               *m_fiOut;          // Output File
  String              m_strMode;        // Access mode of output files
  String              m_strOwnership;   // Ownership of output files
  String              m_strCurrentFile; // Currently wrilted file
  bool                m_bSilentMode;    // If true disable warnings
  MapBufPtr           m_dictBuf;        // Named data buffers
  MemBufPtr           m_ptrCurrentBuf;  // Current buffer
  DataOutputPtrStack  m_stkOut;         // Output objects stack

  uid_t Uid();
  gid_t Gid();

  int  ValueToInteger();
  String  ValueToString(DataBuf &aValue);

  // remove the variable var from the stringDict.
  void RemoveVar(const String &strVar);
  // Get type id according to specified print format spec
  DataBuf::Type GetTypeFromFormat(char cType, char cMod);
  // Obtain a data buffer
  MemBufPtr GetBuf(const String &strName, bool bCreate=true);

  // Executing script
  int  ExecLoopOver(int iCurrentPos);
  void ExecPrintData(TemplateToken *pToken);
  int  ExecBlock(int iCurrentPos);
  int  ExecLoop(int iCurrentPos);
  void Exec(int iStartPos, int iEndPos);
  int  ExecIfExists(int iCurrentPos);
  int  ExecIfEq(int iCurrentPos, bool bNot);
  int  ExecComp(int iCurrentPos, bool bInf, bool bNot);
  void ExecPadding(int iCurrentPos);
  void ExecBinary(int iCurrentPos);
  void ExecSet(int iCurrentPos);
  void ExecSetList(int iCurrentPos);

  String GetTokenArgument(TemplateToken *pToken);
  void PrintData(TemplateToken *pToken);
  void OutBinaryScalarFromString(const TemplateToken &Token, const String &strData);
  String ScalarValueToString();

public:
  // @param pstrVar Variable to evaluate
  // @return true if evaluation is done, or false
  Extractor(const String &strNeXusFile, VecToken *pvecToken, IVariableEvaluator *pAppContext, const String &strMode, const String &strOwnership);
  virtual ~Extractor();

  // Set variable value
  void SetVar(const String &strVar, const String &strVal);

  // associate the value val to the variable var in the stringDict.
  void AddVar(const String &strVar, const String &strVal);

  // Test variable
  bool HasVar(const String &strVar);

  // Set silent mode
  void SetSilent(bool bSilent=true) { m_bSilentMode = bSilent; }

  // Specify output file
  void SetOutputFile(const String &strFile, bool bBinary);

  // Parsing and script executing
  void Execute();

  // ICounterVars
  bool GetValue(const String &strVar, int *piValue);

  // IValueEvaluator
  Variant Evaluate(const String &str);

#ifdef __JPEG_SUPPORT__
  /// Convert image embedded in a Nxextract DataBuf into jpeg format
  static void ValueToJpeg(DataBuf *pDataBuf, MemBufPtr ptrmbDest);
#endif

  /// Convert image embedded in a Nxextract DataBuf into bmp format
  static void ValueToBmp(DataBuf *pDataBuf, MemBufPtr ptrmbDest);

  /// Convert a numerical value for one type to another
  /// @param pSrc Adress of initial value
  /// @param eSrcType Type of initial value
  /// @param pDst Adress of final value
  /// @param eDstType Type of final value
  /// @return true if conversion was possible
  static bool ConvertBinaryValue(void *pSrc, DataBuf::Type eSrcType, void *pDst, TemplateToken::BinaryDataType eDstType);

  // Evaluate a variable and give its value if it has been stocked 
  // in the DictVar.
  virtual bool Evaluate(String *pstrVar);
};

#endif
