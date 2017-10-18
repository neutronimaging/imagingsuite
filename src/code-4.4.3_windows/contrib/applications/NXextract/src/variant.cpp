//*****************************************************************************
// Synchrotron SOLEIL
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

#include "base.h"
#include "date.h"
#include "membuf.h"
#include "variant.h"

using namespace soleil;
//------------------------------------------------------------------------
// Variant::Variant
//------------------------------------------------------------------------
Variant::Variant()
{
  m_eType = NONE;
}
Variant::Variant(const Variant &v)
{
  m_mbValue = v.m_mbValue;
  m_eType = v.m_eType;
}
Variant::Variant(const char *pcsz)
{
  m_mbValue << pcsz;
  m_eType = Variant::STRING;
}
Variant::Variant(char c)
{
  m_mbValue << c;
  m_eType = Variant::CHAR;
}
Variant::Variant(uint8 uc)
{
  m_mbValue << uc;
  m_eType = Variant::UINT8;
}
Variant::Variant(short s)
{
  m_mbValue << s;
  m_eType = Variant::INT16;
}
Variant::Variant(ushort us)
{
  m_mbValue << us;
  m_eType = Variant::UINT16;
}
Variant::Variant(int i)
{
  switch( sizeof(int) )
  {
  case 4:
    m_mbValue << (long)i;
    m_eType = Variant::INT32;
    break;
  case 8:
    m_mbValue << (int64)i;
    m_eType = Variant::INT64;
    break;
  }
}
Variant::Variant(uint i)
{
  switch( sizeof(uint) )
  {
  case 4:
    m_mbValue << (ulong)i;
    m_eType = Variant::UINT32;
    break;
  case 8:
    m_mbValue << (int64)i;
    m_eType = Variant::INT64;
    break;
  }
}
Variant::Variant(long l)
{
  m_mbValue << l;
  m_eType = Variant::INT32;
}
Variant::Variant(ulong ul)
{
  m_mbValue << ul;
  m_eType = Variant::UINT32;
}
Variant::Variant(int64 i64)
{
  m_mbValue << i64;
  m_eType = Variant::INT64;
}
Variant::Variant(float f)
{
  m_mbValue << f;
  m_eType = Variant::FLOAT32;
}
Variant::Variant(double d)
{
  m_mbValue << d;
  m_eType = Variant::FLOAT64;
}
Variant::Variant(const string &str)
{
  m_mbValue << str;
  m_eType = Variant::STRING;
}
Variant::Variant(const Date &dt)
{
  m_mbValue << dt.RawValue();
  m_eType = Variant::DATE;
}

//------------------------------------------------------------------------
// Variant::operator=
//------------------------------------------------------------------------
Variant &Variant::operator=(const Variant &v)
{
  m_mbValue = v.m_mbValue;
  m_eType = v.m_eType;
  return *this;
}
Variant &Variant::operator=(const char *pcsz)
{
  m_mbValue.Empty();
  m_mbValue << pcsz;
  m_eType = Variant::STRING;
  return *this;
}
Variant &Variant::operator=(char c)
{
  m_mbValue.Empty();
  m_mbValue << c;
  m_eType = Variant::CHAR;
  return *this;
}
Variant &Variant::operator=(uint8 uc)
{
  m_mbValue.Empty();
  m_mbValue << uc;
  m_eType = Variant::UINT8;
  return *this;
}
Variant &Variant::operator=(short s)
{
  m_mbValue.Empty();
  m_mbValue << s;
  m_eType = Variant::INT16;
  return *this;
}
Variant &Variant::operator=(ushort us)
{
  m_mbValue.Empty();
  m_mbValue << us;
  m_eType = Variant::UINT16;
  return *this;
}
Variant &Variant::operator=(int i)
{
  m_mbValue.Empty();
  switch( sizeof(int) )
  {
  case 4:
    m_mbValue << (long)i;
    m_eType = Variant::INT32;
    break;
  case 8:
    m_mbValue << (int64)i;
    m_eType = Variant::INT64;
    break;
  }
  return *this;
}
Variant &Variant::operator=(uint i)
{
  m_mbValue.Empty();
  switch( sizeof(uint) )
  {
  case 4:
    m_mbValue << (ulong)i;
    m_eType = Variant::UINT32;
    break;
  case 8:
    m_mbValue << (int64)i;
    m_eType = Variant::INT64;
    break;
  }
  return *this;
}
Variant &Variant::operator=(long l)
{
  m_mbValue.Empty();
  m_mbValue << l;
  m_eType = Variant::INT32;
  return *this;
}
Variant &Variant::operator=(ulong ul)
{
  m_mbValue.Empty();
  m_mbValue << ul;
  m_eType = Variant::UINT32;
  return *this;
}
Variant &Variant::operator=(int64 i64)
{
  m_mbValue.Empty();
  m_mbValue << i64;
  m_eType = Variant::INT64;
  return *this;
}
Variant &Variant::operator=(float f)
{
  m_mbValue.Empty();
  m_mbValue << f;
  m_eType = Variant::FLOAT32;
  return *this;
}
Variant &Variant::operator=(double d)
{
  m_mbValue.Empty();
  m_mbValue << d;
  m_eType = Variant::FLOAT64;
  return *this;
}
Variant &Variant::operator=(const string &str)
{
  m_mbValue.Empty();
  m_mbValue << str;
  m_eType = Variant::STRING;
  return *this;
}
Variant &Variant::operator=(const Date &dt)
{
  m_mbValue.Empty();
  m_mbValue << dt.RawValue();
  m_eType = Variant::DATE;
  return *this;
}

//------------------------------------------------------------------------
// Variant::operator char()
//------------------------------------------------------------------------
Variant::operator char() const
{
  if( CHAR == m_eType )
    return *(char *)(m_mbValue.Buf());
  else
    throw BadCast("Cannot convert value to 'char'", "Bad cast", "Variant::operator char");
}

//------------------------------------------------------------------------
// Variant::operator uchar()
//------------------------------------------------------------------------
Variant::operator uchar() const
{
  if( UINT8 == m_eType )
    return *(uchar *)(m_mbValue.Buf());
  else
    throw BadCast("Cannot convert value to 'unsigned char'", "Bad cast", "Variant::operator char");
}

//------------------------------------------------------------------------
// Variant::operator short()
//------------------------------------------------------------------------
Variant::operator short() const
{
  switch( m_eType )
  {
    case CHAR:
      return short(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return short(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
    case UINT16:
      return *(short *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'short'", "Bad cast", "Variant::operator short");
  }
}

//------------------------------------------------------------------------
// Variant::operator ushort()
//------------------------------------------------------------------------
Variant::operator ushort() const
{
  switch( m_eType )
  {
    case CHAR:
      return ushort(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return ushort(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
    case UINT16:
      return *(ushort *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'unsigned short'", "Bad cast", "Variant::operator short");
  }
}

//------------------------------------------------------------------------
// Variant::operator int()
//------------------------------------------------------------------------
Variant::operator int() const
{
  switch( sizeof(int) )
  {
    case 4:
      switch( m_eType )
      {
        case CHAR:
          return int(*(char *)(m_mbValue.Buf()));
        case UINT8:
          return int(*(uint8 *)(m_mbValue.Buf()));
        case INT16:
          return int(*(short *)(m_mbValue.Buf()));
        case UINT16:
          return int(*(ushort *)(m_mbValue.Buf()));
        case INT32:
        case UINT32:
          return *(int *)(m_mbValue.Buf());
        default:
          throw BadCast("Cannot convert value to 'int'", "Bad cast", "Variant::operator int");
      }
    case 8:
      switch( m_eType )
      {
        case CHAR:
          return int(*(char *)(m_mbValue.Buf()));
        case UINT8:
          return int(*(uint8 *)(m_mbValue.Buf()));
        case INT16:
          return int(*(short *)(m_mbValue.Buf()));
        case UINT16:
          return int(*(ushort *)(m_mbValue.Buf()));
        case INT32:
          return int(*(long *)(m_mbValue.Buf()));
        case UINT32:
          return int(*(ulong *)(m_mbValue.Buf()));
        case INT64:
          return *(int *)(m_mbValue.Buf());
        default:
          throw BadCast("Cannot convert value to 'int'", "Bad cast", "Variant::operator int");
      }
  }
}

//------------------------------------------------------------------------
// Variant::operator int64()
//------------------------------------------------------------------------
Variant::operator int64() const
{
  switch( m_eType )
  {
    case CHAR:
      return int64(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return int64(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
      return int64(*(short *)(m_mbValue.Buf()));
    case UINT16:
      return int64(*(ushort *)(m_mbValue.Buf()));
    case INT32:
      return int64(*(long *)(m_mbValue.Buf()));
    case UINT32:
      return int64(*(ulong *)(m_mbValue.Buf()));
    case INT64:
      return *(int64 *)(m_mbValue.Buf());
    case DATE:
      return *(int64 *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'int64'", "Bad cast", "Variant::operator int64");
  }
}

//------------------------------------------------------------------------
// Variant::operator long()
//------------------------------------------------------------------------
Variant::operator long() const
{
  switch( m_eType )
  {
    case CHAR:
      return long(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return long(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
      return long(*(short *)(m_mbValue.Buf()));
    case UINT16:
      return long(*(ushort *)(m_mbValue.Buf()));
    case INT32:
    case UINT32:
      return *(long *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'long'", "Bad cast", "Variant::operator long");
  }
}

//------------------------------------------------------------------------
// Variant::operator ulong()
//------------------------------------------------------------------------
Variant::operator ulong() const
{
  switch( m_eType )
  {
    case CHAR:
      return ulong(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return ulong(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
      return ulong(*(short *)(m_mbValue.Buf()));
    case UINT16:
      return ulong(*(ushort *)(m_mbValue.Buf()));
    case INT32:
    case UINT32:
      return *(ulong *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'long'", "Bad cast", "Variant::operator long");
  }
}

//------------------------------------------------------------------------
// Variant::operator float()
//------------------------------------------------------------------------
Variant::operator float() const
{
  switch( m_eType )
  {
    case CHAR:
      return float(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return float(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
      return float(*(short *)(m_mbValue.Buf()));
    case UINT16:
      return float(*(ushort *)(m_mbValue.Buf()));
    case INT32:
      return float(*(long *)(m_mbValue.Buf()));
    case UINT32:
      return float(*(ulong *)(m_mbValue.Buf()));
    case INT64:
      return float(*(int64 *)(m_mbValue.Buf()));
    case FLOAT32:
      return *(float *)(m_mbValue.Buf());
    case FLOAT64:
      return double(*(float *)(m_mbValue.Buf()));
    default:
      throw BadCast("Cannot convert value to 'float'", "Bad cast", "Variant::operator float");
  }
}

//------------------------------------------------------------------------
// Variant::operator double()
//------------------------------------------------------------------------
Variant::operator double() const
{
  switch( m_eType )
  {
    case CHAR:
      return double(*(char *)(m_mbValue.Buf()));
    case UINT8:
      return double(*(uint8 *)(m_mbValue.Buf()));
    case INT16:
      return double(*(short *)(m_mbValue.Buf()));
    case UINT16:
      return double(*(ushort *)(m_mbValue.Buf()));
    case INT32:
      return double(*(long *)(m_mbValue.Buf()));
    case UINT32:
      return double(*(ulong *)(m_mbValue.Buf()));
    case INT64:
      return double(*(int64 *)(m_mbValue.Buf()));
    case FLOAT32:
      return double(*(float *)(m_mbValue.Buf()));
    case FLOAT64:
      return *(double *)(m_mbValue.Buf());
    default:
      throw BadCast("Cannot convert value to 'long'", "Bad cast", "Variant::operator long");
  }
}
//------------------------------------------------------------------------
// Variant::operator String()
//------------------------------------------------------------------------
Variant::operator String() const
{
  switch( m_eType )
  {
    case CHAR:
      return StrFormat("%c", *(char *)m_mbValue.Buf());
    case UINT8:
      return StrFormat("%ud", uint(*(char *)(m_mbValue.Buf())));
    case INT16:
      return StrFormat("%hd", *(short *)(m_mbValue.Buf()));
    case UINT16:
      return StrFormat("%uhd", *(ushort *)(m_mbValue.Buf()));
    case INT32:
      return StrFormat("%ld", *(long *)(m_mbValue.Buf()));
    case UINT32:
      return StrFormat("%uld", *(ulong *)(m_mbValue.Buf()));
    case INT64:
      return StrFormat("%uld", *(int64 *)(m_mbValue.Buf()));
    case FLOAT32:
      return StrFormat("%g", *(float *)(m_mbValue.Buf()));
    case FLOAT64:
      return StrFormat("%g", *(double *)(m_mbValue.Buf()));
    case STRING:
      return StrFormat("%s", m_mbValue.Buf());
    case DATE:
      {
        Date dt;
        dt.SetRawValue(*(int64 *)(m_mbValue.Buf()));
        return StrFormat("%s", PSZ(dt.ToISO8601()));
      }
    default:
      throw BadCast("Cannot convert value to 'long'", "Bad cast", "Variant::operator long");
  }
}
