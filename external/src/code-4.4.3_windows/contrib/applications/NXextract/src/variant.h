//*****************************************************************************
// Synchrotron SOLEIL
//
// 'Variant' class
//
// Creation : 26/11/2009
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

#ifndef __VARIANT_H__
#define __VARIANT_H__

#ifndef __BASE_H__
  #include "base.h"
#endif

namespace gdshare
{
//=============================================================================
/// Handling various types in a single class
//=============================================================================
class Variant
{
public:
  /// Value type
  enum EType
  {
    NONE = 0, CHAR, UINT8, INT16, INT32, INT64, UINT16, UINT32, FLOAT32, FLOAT64, STRING, DATE
  };

  /// Bad cast Exception
  class BadCast: public Exception
  {
  protected:
    const char *ErrorTitle() { return "Bad cast: "; }
  public:
    BadCast(const char *pcszError=NULL, const char *pcszReason=NULL, const char *pcszMethod=NULL):
    Exception(pcszError, pcszReason, pcszMethod) { }
  };

private:
  MemBuf m_mbValue;
  EType  m_eType;

public:
  EType Type() const { return m_eType; }

  Variant();
  Variant(const Variant &v);
  Variant(char c);
  Variant(uchar uc);
  Variant(short s);
  Variant(ushort us);
  Variant(long l);
  Variant(ulong ul);
  Variant(int i);
  Variant(uint ui);
  Variant(int64 i64);
  Variant(float f);
  Variant(double d);
  Variant(const string &str);
  Variant(const char *pcsz);
  Variant(const Date &dt);

  Variant &operator=(char c);
  Variant &operator=(uchar uc);
  Variant &operator=(short s);
  Variant &operator=(ushort us);
  Variant &operator=(long l);
  Variant &operator=(ulong l);
  Variant &operator=(int i);
  Variant &operator=(uint ui);
  Variant &operator=(int64 i64);
  Variant &operator=(float f);
  Variant &operator=(double d);
  Variant &operator=(const char *pcsz);
  Variant &operator=(const string &str);
  Variant &operator=(const Date &dt);
  Variant &operator=(const Variant &v);

  //@{ Comparison operator: YET to be implmented
  Variant &operator==(char c) const;
  Variant &operator==(uchar uc) const;
  Variant &operator==(short s) const;
  Variant &operator==(ushort us) const;
  Variant &operator==(long l) const;
  Variant &operator==(ulong l) const;
  Variant &operator==(int i) const;
  Variant &operator==(uint ui) const;
  Variant &operator==(int64 i64) const;
  Variant &operator==(float f) const;
  Variant &operator==(double d) const;
  Variant &operator==(const char *pcsz) const;
  Variant &operator==(const string &str) const;
  Variant &operator==(const Date &dt) const;
  Variant &operator==(const Variant &v) const;

  //@{ Implicite conversions
  operator char() const;
  operator uchar() const;
  operator short() const;
  operator ushort() const;
  operator int() const;
  operator int64() const;
  operator long() const;
  operator ulong() const;
  operator float() const;
  operator double() const;
  operator String() const;
  operator Date() const;
  //@}

  //@{ Explicite conversions: YET to be implmented
  char   ToChar() const;
  uint8  ToUInt8() const;
  short  ToShort() const;
  ushort ToUShort() const;
  long   ToLong() const;
  ulong  ToULong() const;
  int    ToInt() const;
  int64  ToInt64() const;
  float  ToFloat() const;
  double ToDouble() const;
  String ToString() const;
  Date   ToDate() const;
  //@}
};

/// Reference pointer to variant
typedef RefPtr<Variant> VariantPtr;

} // namespace soleil

#endif