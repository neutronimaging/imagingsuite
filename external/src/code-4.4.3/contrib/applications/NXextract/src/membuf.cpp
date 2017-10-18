//*****************************************************************************
// Synchrotron SOLEIL
//
// Creation : 20/03/2006
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
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>

#include "membuf.h"

using namespace soleil;

//----------------------------------------------------------------------------
// Constantes
//----------------------------------------------------------------------------

#define CMemBuf_MAXDELTA    1048576

// Buffer allocation min size
#define MEMBUF_MINSIZE   16


//-------------------------------------------------------------------
// Conversion hexadecimal to decimal
//-------------------------------------------------------------------
char HexToDec (char ch)
{
  if (ch >= '0' && ch <= '9')
    return (char)(ch - '0');
  if (ch >= 'A' && ch <= 'F')
    return (char)(ch - 'A' + 10);
  if (ch >= 'a' && ch <= 'f')
    return (char)(ch - 'a' + 10);

  return 0;
}

//===================================================================
// Class CMemBuf
//===================================================================

//-------------------------------------------------------------------
// CMemBuf::CMemBuf
//-------------------------------------------------------------------
CMemBuf::CMemBuf(uint uiLenBuf)
 : m_uiLenBuf(uiLenBuf),m_bOwner(true)
{
  
  if( m_uiLenBuf )
  {
    m_pBuf = new char[m_uiLenBuf];
  }
  else
    m_pBuf = NULL;
  m_uiLen = 0;
  m_uiPos = 0;
}

//-------------------------------------------------------------------
// CMemBuf::CMemBuf
//-------------------------------------------------------------------
CMemBuf::CMemBuf(const CMemBuf& buf)
  : m_uiLen(buf.Len()), m_bOwner(true)
{
  if( m_uiLen != 0 )
  {
    m_uiLenBuf = m_uiLen;
    m_pBuf = new char[m_uiLenBuf];
    memcpy(m_pBuf, buf.Buf(), m_uiLen);
  }
  else
  { 
    // Empty buffer
    m_uiLenBuf = 0;
    m_pBuf = NULL;
  }
  m_uiPos = 0;
}

//-------------------------------------------------------------------
// CMemBuf::~CMemBuf
//-------------------------------------------------------------------
CMemBuf::~CMemBuf()
{
  if( (m_uiLenBuf != 0) && m_bOwner )
    delete [] m_pBuf;
}

//-------------------------------------------------------------------
// CMemBuf::operator=
//-------------------------------------------------------------------
CMemBuf& CMemBuf::operator=(const CMemBuf& buf)
{
  if( !m_bOwner )
    Reset();
  uint uiNb = buf.Len();
  if( uiNb > m_uiLenBuf )
  {
    // pas assez de place : realloue
    if( m_uiLenBuf != 0 )
      delete [] m_pBuf;

    m_uiLenBuf = uiNb;
    m_pBuf = new char[m_uiLenBuf];
  }
  memcpy(m_pBuf, buf.Buf(), uiNb);
  m_uiLen = uiNb;
  m_uiPos = buf.Pos();
  return *this;
}

//-------------------------------------------------------------------
// CMemBuf::operator==
//-------------------------------------------------------------------
bool CMemBuf::operator==(const CMemBuf &mb) const
{
  if ( m_uiLen == mb.Len() )
    return !memcmp(m_pBuf, mb.Buf(), m_uiLen);
  return false;
}

//-------------------------------------------------------------------
// CMemBuf::ReallocWithMargin
//-------------------------------------------------------------------
void CMemBuf::ReallocWithMargin(uint uiNewSize)
{
  // Min size
  if( uiNewSize < MEMBUF_MINSIZE )
    uiNewSize = MEMBUF_MINSIZE;

  // Not enough space : The buffer grow with CMEMBUF_MAXDELTA bytes max
  uint uiDelta = uiNewSize / 2;
  if (uiDelta > CMEMBUF_MAXDELTA)
    uiDelta = CMEMBUF_MAXDELTA;

  uint uiNewLenBuf = uiNewSize + uiDelta;
  Realloc(uiNewLenBuf);
}

//-------------------------------------------------------------------
// CMemBuf::Realloc
//-------------------------------------------------------------------
void CMemBuf::Realloc(uint uiNewLenBuf)
{
  if (uiNewLenBuf < m_uiLen)
  {
    // Wished size smaller than current one!
//##    ASSERT_FAILURE;
    return;
  }
  char *pNewBuf = new char[uiNewLenBuf];
  if( m_pBuf )
    memcpy(pNewBuf, m_pBuf, m_uiLen);
  if( m_uiLenBuf != 0 && m_pBuf && m_bOwner )
    delete [] m_pBuf;
  m_pBuf     = pNewBuf;
  m_uiLenBuf = uiNewLenBuf;
}

//-------------------------------------------------------------------
// CMemBuf::Reset
//-------------------------------------------------------------------
void CMemBuf::Reset()
{
  if (m_uiLenBuf != 0)
  {
    if( m_bOwner )
      delete [] m_pBuf;
    m_uiPos = 0;
    m_uiLen = 0;
    m_uiLenBuf = 0;
    m_pBuf = NULL;
    m_bOwner = true;
  }
}

//-------------------------------------------------------------------
// CMemBuf::SetLen
//-------------------------------------------------------------------
void CMemBuf::SetLen(uint uiNb)
{
  if( uiNb > m_uiLenBuf )
    // Not enough space : re-alloc
    Realloc(uiNb);
  m_uiLen = uiNb;
  m_uiPos = 0;
}

//-------------------------------------------------------------------
// CMemBuf::PutBloc
//-------------------------------------------------------------------
void CMemBuf::PutBloc(const void* p, uint uiNb)
{
//##  ASSERT( (ulong)m_uiLen + (ulong)uiNb <= (ulong)MAX_ALLOC_NEW)
  uint uiTotalLen = m_uiLen + uiNb;
  if( uiTotalLen > m_uiLenBuf )
  {
    // Not enough space : re-alloc with margin
    ReallocWithMargin(uiTotalLen);
  }
  memcpy(m_pBuf+m_uiLen, p, uiNb);
  m_uiLen = uiTotalLen;
}

//-------------------------------------------------------------------
// CMemBuf::InsertBloc
//-------------------------------------------------------------------
void CMemBuf::InsertBloc(const void* p, uint uiNb, uint uiInsPos)
{
//##  ASSERT(uiInsPos <= m_uiLen)
//##  ASSERT( (ulong)m_uiLen + (ulong)uiNb <= (ulong)MAX_ALLOC_NEW)
  uint uiTotalLen = m_uiLen + uiNb;
  if( uiTotalLen > m_uiLenBuf )
  {
    // Not enough space : re-alloc with margin
    ReallocWithMargin(uiTotalLen);
  }

  // Not enough space
  memmove(m_pBuf+uiInsPos+uiNb, m_pBuf+uiInsPos, m_uiLen-uiInsPos);
  memcpy(m_pBuf+uiInsPos, p, uiNb);
  m_uiLen = uiTotalLen;
}

//-------------------------------------------------------------------
// CMemBuf::GetBloc
//-------------------------------------------------------------------
int CMemBuf::GetBloc(void* p, uint uiNb)
{
  if( m_uiLen - m_uiPos < uiNb )
  {
    // Plus assez de place
    //##ASSERT_FAILURE
    return 1;
  }

  memcpy(p, m_pBuf+m_uiPos, uiNb);
  m_uiPos += uiNb;
  return 0;
}

//-------------------------------------------------------------------
// CMemBuf::MoveBloc
//-------------------------------------------------------------------
void CMemBuf::MoveBloc(uint uiDst, uint uiSrc, uint uiSize)
{
//##  ASSERT( (uiDst+uiSize) <= m_uiLen );
//##  ASSERT( (uiSrc+uiSize) <= m_uiLen );
  memmove(m_pBuf + uiDst, m_pBuf + uiSrc, uiSize);
}

//-------------------------------------------------------------------
// CRC computing
//-------------------------------------------------------------------
// Table des CRC-32 (engendree par un utilitaire, on s'en doute !)
ulong crc_32_tab[] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};


//***************************************************************************
// @func API de calcul de CRC 32 bits.
// Deux utilisations possibles de cette API : 
//  soit on laisse pulInitValue a NULL, auquel cas elle calcule le crc
//  global du buffer. Soit pulInitValue n'est pas NULL, et cela permet de
//  calcul un crc global sur un ensemble de buffers. Par exemple si p1 et
//  p2 sont deux pointeurs vers des buffers, il faut ecrire :
//    ulong ulCrc = 0xFFFFFFFFL;
//    ulCrc = Crc( p1, uiP1Len, &ulCrc );
//    ulCrc = Crc( p2, uiP2Len, &ulCrc );
//    ulCrc = ulCrc ^ 0xFFFFFFFFL;
//***************************************************************************
ulong soleil::Crc( const byte *pBuf, uint uiLen, ulong *pulInitValue )
{
  ulong ulVal;
  if( pulInitValue != NULL )
    ulVal = *pulInitValue;
  else
    ulVal = 0L;

  if( uiLen == 0 )
    return ulVal;
 
  if( pulInitValue == NULL )
    ulVal = 0xffffffffL;

  // Main loop : under Visual C++, use assembly language
  #if defined(_MSC_VER)
      _asm 
      {
        mov   ecx, uiLen
        jecxz AsmEnd

        push  esi
        mov   esi, pBuf
        mov   ebx, ulVal
        AsmLoop:

        lodsb
        xor   al, bl
        movzx eax, al
        mov   eax, Dword Ptr [crc_32_tab + eax*4]

        shr   ebx, 8
        xor   ebx, eax

        loop AsmLoop
        mov   ulVal, ebx
        pop   esi

        AsmEnd:
      }
  #else
    while( uiLen-- )
      ulVal = crc_32_tab[((int)ulVal ^ (*pBuf++)) & 0xff] ^ (ulVal >> 8);
  #endif

  if( pulInitValue == NULL )
    ulVal = ulVal ^ 0xffffffffL;
  return ulVal;
}

//-------------------------------------------------------------------
// CMemBuf::GetCrc
//-------------------------------------------------------------------
ulong CMemBuf::GetCrc() const
{
  return soleil::Crc( (byte *)m_pBuf, m_uiLen );
}

//-------------------------------------------------------------------
// CMemBuf::LoadFile
//-------------------------------------------------------------------
/*
int CMemBuf::LoadFile(const char *pszFile)
{
  int rc = 0;
  CFileEx f (pszFile);

  rc = f.Open ("r");
  if (rc)
    return rc;

  long lSize = f.Size ();
  SetLen (lSize);
  rc = f.ReadBloc (Buf(), lSize);
  return rc;
}

//---------------------------------------------------------------------------
// CMemBuf::SaveToFile
//---------------------------------------------------------------------------
int CMemBuf::SaveToFile(const CFileName& fnFile, const char *pszMode,
  uint uiShareMode) const
{
  int iRc = 0;
  CFileEx f(fnFile);
  iRc = f.Open( pszMode, uiShareMode );
  if( iRc )
    return iRc;

  iRc = f.WriteBloc( *this );
  if( iRc )
    return iRc;

  iRc = f.Close();
  if( iRc )
    return iRc;

  return 0;
}
*/

//---------------------------------------------------------------------------
// CMemBuf::Attach
//---------------------------------------------------------------------------
void CMemBuf::Attach(char* pBuf,uint uiLen,bool bOwner)
{
  Reset();
  m_uiLen = uiLen;
  m_uiLenBuf = uiLen;
  m_pBuf = pBuf;
  m_bOwner = bOwner;
}

//---------------------------------------------------------------------------
// CMemBuf::GiveOwnership
//---------------------------------------------------------------------------
int CMemBuf::GiveOwnership(CMemBuf* pToHaveOwnership)
{
  if( !m_bOwner )
  {
//##    ASSERT_FAILURE;
    return -1;
  }
  pToHaveOwnership->Attach( m_pBuf, m_uiLen, true );
  SetOwner(false);
  return 0;
}

//-------------------------------------------------------------------
// Stream oparators
//-------------------------------------------------------------------
CMemBuf& CMemBuf::operator<<(char c)
{
  PutBloc(&c, sizeof(c)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(char &c)
{
  GetBloc(&c, sizeof(c));           
  return *this;
}

CMemBuf& CMemBuf::operator<<(uchar uc)
{
  PutBloc(&uc, sizeof(uc)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(uchar &uc)
{
  GetBloc(&uc, sizeof(uc));           
  return *this;
}


CMemBuf& CMemBuf::operator<<(short s)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertShort(&s);
  #endif
  PutBloc(&s, sizeof(s)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(short &s)
{
  GetBloc(&s, sizeof(s));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertShort(&s);
  #endif
  return *this;
}

CMemBuf& CMemBuf::operator<<(ushort us)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertShort((short*)&us);
  #endif
  PutBloc(&us, sizeof(us)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(ushort &us)
{
  GetBloc(&us, sizeof(us));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertShort((short*)&us);
  #endif
  return *this;
}

CMemBuf& CMemBuf::operator<<(long l)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertLong(&l);
  #endif
  PutBloc(&l, sizeof(l)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(long &l)
{
  GetBloc(&l, sizeof(l));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertLong(&l);
  #endif
  return *this;
}
CMemBuf& CMemBuf::operator<<(ulong ul)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertLong((long*)&ul);
  #endif
  PutBloc(&ul, sizeof(ul)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(ulong &ul)
{
  GetBloc(&ul, sizeof(ul));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertLong((long*)&ul);
  #endif
  return *this;
}
CMemBuf& CMemBuf::operator>>(int64 &i64)
{
  GetBloc(&i64, sizeof(int64));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertInt64(&i64);
  #endif
  return *this;
}
CMemBuf& CMemBuf::operator<<(int64 i64)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertInt64((long*)&ul);
  #endif
  PutBloc(&i64, sizeof(i64)); 
  return *this;
}

CMemBuf& CMemBuf::operator<<(float f)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertFloat(&f);
  #endif
  PutBloc(&f, sizeof(f)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(float &f)
{
  GetBloc(&f, sizeof(f));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertFloat(&f);
  #endif
  return *this;
}

CMemBuf& CMemBuf::operator<<(double d)
{
  #ifdef __MOTOROLA_ENDIAN__
    InvertDouble(&d);
  #endif
  PutBloc(&d, sizeof(d)); 
  return *this;
}
CMemBuf& CMemBuf::operator>>(double &d)
{
  GetBloc(&d, sizeof(d));           
  #ifdef __MOTOROLA_ENDIAN__
    InvertDouble(&d);
  #endif
  return *this;
}

CMemBuf& CMemBuf::operator<<(const char* psz)
{
  PutBloc(psz, strlen(psz)+1);
  return *this;
}

CMemBuf& CMemBuf::operator<<(const string& string)
{
  PutBloc(string.c_str(), string.size()+1);
  return *this;
}

CMemBuf& CMemBuf::operator>>(string& string)
{
  if ( m_pBuf != NULL )
  {
    uint uiNb = strlen(CurPointer());
    string.reserve(uiNb);
    string.append(CurPointer(), uiNb);
  }
  return *this;
}

CMemBuf& CMemBuf::operator<<(const CMemBuf& membuf)
{
  PutBloc( membuf.Buf(), membuf.Len() );
  return *this;
}

//---------------------------------------------------------------------------
// CMemBuf::HexString
//---------------------------------------------------------------------------
String CMemBuf::HexString() const
{
  static const char *pszHexa = "0123456789ABCDEF";
  ostringstream oss;
  String str;
  str.reserve(2 * m_uiLen);
  for (uint ui = 0; ui < m_uiLen; ui++)
    oss << (pszHexa[(uchar)m_pBuf[ui] >> 4]) 
        << (pszHexa[(uchar)m_pBuf[ui] & 0x0F]);

  return oss.str();
}

//---------------------------------------------------------------------------
// CMemBuf::PutHexString
//---------------------------------------------------------------------------
void CMemBuf::PutHexString(const char *pszHex, int iLen)
{
  if( iLen == -1 )
    iLen = strlen(pszHex);
//##  ASSERT(!(iLen & 1)) // Only even value are valid
  
  for (int i = 0; i < iLen; i+=2)
    *this << (char)((char)(HexToDec(pszHex[i]) << 4) + HexToDec(pszHex[i+1]));
}

//---------------------------------------------------------------------------
// CMemBuf::ReversibleCrypt
//---------------------------------------------------------------------------
#define REVCRYPT_MAGIC 0x20544212

#define PSW_HEADER "CmjBlurp"
#define PSW_FOOTER "ZnortGnap"
#define DEFAULT_XOR_KEY "ÏgTy7è_iuljkR4-)9qsEê-|à@-nU02;#(&z&eâ)=3aZ#bTù$<?Y@#)pT$*£Ë%')àeQ4~{^>|ç_èFp9/+-§H"
#define DEFAULT_ROT_KEY "T;#Tù$|p9/+=à*gTy@è_iz3$sÏ)pE2%<_')àaZun.0èlj#bâekR4?YË#H(&£{^>|ê)9-Q4~@ZçqFô*1"

uchar BinRotLeft(uchar chOctet, uint uiRotCoeff );
uchar BinRotRight(uchar chOctet, uint uiRotCoeff );

int CMemBuf::ReversibleCrypt(const void* pXorKey, uint uiXorKeyLen,
                             const void* pRotKey, uint uiRotKeyLen,
                             short sVersion)
{
  // !!! Attention, si version vaut REVCRYPT_LATEST_VERSION, utiliser la 
  // dernière version de cryptage
  if (sVersion==REVCRYPT_LATEST_VERSION)
    sVersion = 2;

  static bool s_bsRandCalled = false;
  if( !s_bsRandCalled )
  {
    CCurrentDate dtCur;
    srand ((uint)((dtCur.Sec()*1000)));
    s_bsRandCalled = true;
  }
	
  switch (sVersion)
  {
    case 2:
    { // Add a variable length header and footer
      short sl1 = (short)(((double)(rand()) * strlen(PSW_HEADER))/RAND_MAX);
      short sl2 = (short)(((double)(rand()) * strlen(PSW_FOOTER))/RAND_MAX);

      // Header
      InsertBloc(PSW_HEADER,sl1, 0);
      #ifdef __MOTOROLA_ENDIAN__
        InvertShort(&sl1);
      #endif
      InsertBloc(&sl1, sizeof(short), 0);
      // Footer
      PutBloc(PSW_FOOTER,sl2);
      operator<<((short)sl2);
      operator<<((long)GetCrc());
    }
    case 0:
    case 1:
    {
      // Compute a key
      short asKey[4] = { 0, 0, 0, 0 };
      asKey[0] = (short)(rand() & 0x7FFF);
      asKey[1] = (short)(rand() & 0x7FFF);
      asKey[2] = (short)(rand() & 0x7FFF);
      asKey[3] = (short)(rand() & 0x7FFF);
      // Crypt using computed key
      Xor(asKey, sizeof(asKey));
      BinRotLeft(asKey, sizeof(asKey));
      // Add key at the end of the buffer
      PutBloc(asKey, sizeof(asKey));
      // Crypt using given keys
      if( pXorKey )
        Xor(pXorKey, uiXorKeyLen);
      else
        Xor(DEFAULT_XOR_KEY, strlen(DEFAULT_XOR_KEY));
      if( pRotKey )
        BinRotLeft(pRotKey, uiRotKeyLen);
      else
        BinRotLeft(DEFAULT_ROT_KEY, strlen(DEFAULT_ROT_KEY));
      break;
    }
  }

  if (sVersion>0)
  {
    operator <<((long)REVCRYPT_MAGIC);
    operator <<(sVersion);
  }
  return 0;
}

//---------------------------------------------------------------------------
// CMemBuf::ReversibleDecrypt
//---------------------------------------------------------------------------
int CMemBuf::ReversibleDecrypt(const void* pXorKey, uint uiXorKeyLen,
                                        const void* pRotKey, uint uiRotKeyLen,
                                        short *psVersion)
{
  if (Len()==0)
    return 0;

  // Get crypting version
  long lMagic;
  short sVersion=0;
  ulong ulMinSize = sizeof(sVersion)+sizeof(lMagic);

  if (Len()<ulMinSize)
    return -1;

  ulong ulPos = Len()-ulMinSize;
  SetPos(ulPos);
  operator >> (lMagic);
  if (lMagic==REVCRYPT_MAGIC)
  {
    // Magic is found, crypting version is in next short value
    operator >> (sVersion);
    Rewind();
    SetLen(ulPos); // Cut buffer
  }
  if (psVersion)
    *psVersion = sVersion;

  // Decrypt
  switch (sVersion)
  {
    case 2:
    case 0:
    case 1:
    {
      short asKey[4] = { 0, 0, 0, 0 };
      if (Len()>=sizeof(asKey))
      {
        // Decrypt using given keys
        if( pRotKey )
          BinRotRight(pRotKey, uiRotKeyLen);
        else
          BinRotRight(DEFAULT_ROT_KEY, strlen(DEFAULT_ROT_KEY));
        if( pXorKey )
          Xor(pXorKey, uiXorKeyLen);
        else
          Xor(DEFAULT_XOR_KEY, strlen(DEFAULT_XOR_KEY));
        // Get computed key
        SetPos(Len()-sizeof(asKey));
        GetBloc(asKey, sizeof(asKey));
        Rewind();
        SetLen(Len()-sizeof(asKey));
        // Decrypt using computed key
        BinRotRight(asKey, sizeof(asKey));
        Xor(asKey, sizeof(asKey));
      }
      if( sVersion == 2 )
      {
        // Get CRC value
        long lStoredCRC;
        memcpy(&lStoredCRC, Buf() + Len() - sizeof(long), sizeof(long));
        #ifdef __MOTOROLA_ENDIAN__
          // Invert long value
          InvertLong(&lStoredCRC);
        #endif
        SetLen(Len() - sizeof(long));
        // Gete buffer CRC for comparison
        long lCRC = GetCrc();
        if( lCRC != lStoredCRC )
          return -1;

        // Remove header
        short sGarbageLength;
        operator>>(sGarbageLength);
        memmove(Buf(), Buf() + Pos() + sGarbageLength, Len() - Pos() - sGarbageLength);
        // La nouvelle longueur est : la longueur de donnees copiees
        SetLen(Len()-Pos()-sGarbageLength);

        // Remove footer
        memcpy(&sGarbageLength, Buf() + Len() - sizeof(short), sizeof(short));
        #ifdef __MOTOROLA_ENDIAN__
          // Invert long value
          InvertShort(&sGarbageLength);
        #endif
        SetLen(Len() - sizeof(short) - sGarbageLength);
        Rewind();
      }
    }

  }
  return 0;
}

//------------------------------------------------------------------------
// CMemBuf::Xor
//------------------------------------------------------------------------
void CMemBuf::Xor(const void* pXor, uint uiXorLen)
{   
  char *pcXor = (char *)pXor;
  char *pHuge = m_pBuf;
  uint uiXor = 0;

  for (ulong ul = 0; ul < Len(); ul++ )
  {     
    pHuge[ul] ^= pcXor[uiXor ];
    uiXor++;  
    if ( uiXor >= uiXorLen)
      uiXor = 0;
  }
}

//------------------------------------------------------------------------
// CMemBuf::BinRotLeft
//------------------------------------------------------------------------
void CMemBuf::BinRotLeft(const void* pRot, uint uiRotLen)
{
  char *pcRot = (char *)pRot;
  char *pHuge = m_pBuf;

  uint uiRot = 0;
  for ( ulong ul = 0; ul<Len(); ul++ )
  {
    pHuge[ul] = ::BinRotLeft(pHuge[ul], pcRot[uiRot]&7);
    uiRot++;
    if ( uiRot >= uiRotLen)
      uiRot = 0;
  }
}

//------------------------------------------------------------------------
// CMemBuf::BinRotRight
//------------------------------------------------------------------------
void CMemBuf::BinRotRight(const void* pRot, uint uiRotLen)
{
  char *pcRot = (char *)pRot;
  char *pHuge = m_pBuf;

  uint uiRot = 0;
  for ( ulong ul = 0; ul<Len(); ul++ )
  {
    pHuge[ul] = ::BinRotRight(pHuge[ul], pcRot[uiRot]&7);
    uiRot++;
    if ( uiRot >= uiRotLen)
      uiRot = 0;
  }
}

//------------------------------------------------------------------------
// Binary shifting
//------------------------------------------------------------------------
uchar BinRotLeft(uchar chOctet, uint uiRotCoeff )
{
  uchar chLeftShift = (uchar ) (chOctet << uiRotCoeff);
  uchar chRightShift = (uchar ) (chOctet >> (8 - uiRotCoeff));
  return (uchar ) (chLeftShift | chRightShift); 
}

//------------------------------------------------------------------------
// Binary shifting
//------------------------------------------------------------------------
uchar BinRotRight(uchar chOctet, uint uiRotCoeff )
{
  uchar chRightShift = (uchar ) (chOctet >> uiRotCoeff);
  uchar chLeftShift  = (uchar ) (chOctet << (8 - uiRotCoeff));
  return (uchar ) (chLeftShift | chRightShift); 
}

//------------------------------------------------------------------------
// Binary shifting
//------------------------------------------------------------------------
void CMemBuf::KeyGen(CMemBuf *pmbCryptedKey, int iKeyLen, bool bInitRand)
{
  if( bInitRand )
  {
    CCurrentDate dtCur;
    srand ((uint)((dtCur.Sec()*1000)));
  }

  CMemBuf mbKey;
  for(int i = 0; i < iKeyLen; i++ )
  {
    uchar uc = rand() & 0xFF;
    mbKey << uc;
  }
  mbKey.ReversibleCrypt();

  // Give crypted key to caller
  *pmbCryptedKey = mbKey;
}

//=============================================================================
//
// String methods
//
//=============================================================================

//---------------------------------------------------------------------------
// String::Crypt
//---------------------------------------------------------------------------
void String::Crypt(const char* pszXorKey, const char* pszRotKey)
{
  CMemBuf membuf;
	membuf << *this;
  membuf.ReversibleCrypt(pszXorKey, pszXorKey ? strlen(pszXorKey) : 0, pszRotKey, pszRotKey ? strlen(pszRotKey) : 0);
	*this = membuf.HexString();
}

//---------------------------------------------------------------------------
// String::Decrypt
//---------------------------------------------------------------------------
void String::Decrypt(const char* pszXorKey, const char* pszRotKey)
{
  CMemBuf membuf;
  membuf.PutHexString(c_str(), size());
  membuf.ReversibleDecrypt(pszXorKey, pszXorKey ? strlen(pszXorKey) : 0, pszRotKey, pszRotKey ? strlen(pszRotKey) : 0);
  erase();
  membuf >> *this;
}

