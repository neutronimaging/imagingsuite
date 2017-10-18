//*****************************************************************************
// Synchrotron SOLEIL
//
// Autosized buffer
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

#ifndef __MEMBUF_H__
#define __MEMBUF_H__

namespace gdshare
{

// CRC computation
ulong Crc( const byte *pBuf, uint uiLen, ulong *pulInitValue = NULL );

//===========================================================================
// endian management
//===========================================================================

inline void InvertShort(short *pS)
{
  // AB -> BA
  char *p = (char*)pS;
  char c = *p;
  *p = *(p+1);
  *(p+1) = c;
}

inline void InvertLong(long *pL)
{
  // ABCD -> DCBA
  char *p = (char*)pL;
  char c = *p;
  *p = *(p+3);
  *(p+3) = c;
  c = *(p+1);
  *(p+1) = *(p+2);
  *(p+2) = c;
}
inline void InvertInt64(int64 *pi64)
{
  // ABCDEFGH -> HGFEDCBA
  char *p = (char*)pi64;
  char c = *p;
  *p = *(p+7);
  *(p+7) = c;
  c = *(p+1);
  *(p+1) = *(p+6);
  *(p+6) = c;
  c = *(p+5);
  *(p+2) = *(p+5);
  *(p+5) = c;
  c = *(p+4);
  *(p+4) = *(p+3);
  *(p+3) = c;
}

inline void InvertFloat(float *pF)
{
  // ABCD -> DCBA
  InvertLong((long*)pF);
}

inline void InvertDouble(double *pD)
{
  // ABCDEFGH -> HGFEDCBA
  char *p = (char*)pD;
  char c = *p;
  *p = *(p+7);
  *(p+7) = c;

  c = *(p+1);
  *(p+1) = *(p+6);
  *(p+6) = c;

  c = *(p+2);
  *(p+2) = *(p+5);
  *(p+5) = c;

  c = *(p+3);
  *(p+3) = *(p+4);
  *(p+4) = c;
}

//===================================================================
// Constantes
//===================================================================

// Re-allocation delta
#define CMEMBUF_MAXDELTA    8192

// Latest crypying version
#define REVCRYPT_LATEST_VERSION -1
#define VARLENGTH_CRYPT          2

#define OFFSET(type,field) \
  (int)&(((type*)NULL)->field)

//===========================================================================
// Class CMemBuf
//
// Auto-sized binary buffer with cryptographic capabilities
//===========================================================================
class CMemBuf : public MReferencable, public MMetadata
{
private:
  uint m_uiPos;             ///< Position (for reading)
  uint m_uiLen;             ///< Buffer size
  uint m_uiLenBuf;          ///< Allocated size
  char *m_pBuf;             ///< The buffer!
  bool m_bOwner;            ///< if true the instance own the buffer

  // Re-allocation
  void ReallocWithMargin(uint uiNewSize) ;

public:
  /// Constructor
  CMemBuf(uint uiLenBuf=0) ;
  CMemBuf(const CMemBuf& buf) ;
  /// Destructor
  ~CMemBuf() ;

  /// Copy operator
  CMemBuf& operator=(const CMemBuf& buf);

  // Comparaison operator
  bool operator==(const CMemBuf &mb) const;
  
  /// Attachment from a external buffer
  ///
  /// @param pBuf memory area to attach
  /// @param uiLen area size
  /// @param bOwner if true this instance will own the buffer
  ///
  void Attach(char* pBuf, uint uiLen, bool bOwner = false);

  /// Length
  uint Len() const  { return m_uiLen; }

  /// Is empty ?
  int IsEmpty() const { return m_uiLen==0; }

  // Buffer size
  uint BufLen() const { return m_uiLenBuf; }

  /// buffer pointer
  char* Buf() const   { return m_pBuf; }

  /// buffer pointer in *bytes* data type
  byte *Bytes() const { return (byte *)m_pBuf; }
   
  /// Position in buffer
  uint Pos() const    { return m_uiPos; }

  /// Set length
  void SetLen(uint ui);

  /// Reallocation
  void Realloc(uint uiNewLenBuf);

  /// Add a binary bloc
  void PutBloc(const void* p, uint uiNb);

  /// Get a binary bloc from the buffer
  int GetBloc(void* p, uint uiNb);

  /// Insert a binary bloc at given offset
  void InsertBloc(const void* p, uint uiNb, uint uiPos);

  /// Move a part of the buffer
  void MoveBloc(uint uiDst, uint uiSrc, uint uiSize);

  /// Set current read point to the begining
  void Rewind()  { m_uiPos = 0; }

  /// Set the current read point
  void SetPos(uint ui)
    {
//##      ASSERT(ui <= m_uiLen)
      m_uiPos = ui;
    }

  /// Reset without free buffer memory
  void Empty()
  {
    m_uiPos = 0 ;
    m_uiLen = 0 ;
  }

  /// Give buffer ownership to *this* instance
  void SetOwner(bool bOwner) { m_bOwner = bOwner; }

  /// Give buffer ownership to another instance
  int GiveOwnership(CMemBuf* pToHaveOwnership);

  /// Reset with memory freeing
  void Reset();

  /// Compute CRC
  ulong GetCrc() const;

  // Reading from file
  // @in pszFileName: file name
//  int LoadFile(const char *pszFileName);
  
  // Saving to a file
// int SaveToFile(const CFileName& fnFile, const char *pszMode= "w",
//    uint uiShareMode = CFILE_SHARE_COMPAT) const;

  /// Pointer to current position
  char *CurPointer() const  { return (m_pBuf + m_uiPos); }

  /// Stream methods
  CMemBuf& operator<<(char c);
  CMemBuf& operator>>(char &c);
  CMemBuf& operator<<(uchar uc);
  CMemBuf& operator>>(uchar &uc);
  CMemBuf& operator<<(short s);
  CMemBuf& operator>>(short &s);
  CMemBuf& operator<<(ushort us);
  CMemBuf& operator>>(ushort &us);
  CMemBuf& operator<<(long l);
  CMemBuf& operator>>(long &l);
  CMemBuf& operator<<(int64 i64);
  CMemBuf& operator>>(int64 &i64);
  CMemBuf& operator<<(ulong ul);
  CMemBuf& operator>>(ulong &ul);
  CMemBuf& operator<<(float f);
  CMemBuf& operator>>(float &f);
  CMemBuf& operator<<(double d);
  CMemBuf& operator>>(double &d);
  CMemBuf& operator<<(const char* psz);
  CMemBuf& operator<<(const string& string);
  CMemBuf& operator>>(string& string);
  CMemBuf& operator<<(const CMemBuf& membuf);

  // Get buffer content as hexadecimal string
  String HexString() const;

  /// Put a haxadecimal string into the buffer
  void PutHexString(const char *pszHex, int iLen = -1);
  
  /// Reversible crypting
  int ReversibleCrypt(const void* pXorKey=NULL, uint uiXorKeyLen=0,
                      const void* pRotKey=NULL, uint uiRotKeyLen=0,
                      short sVersion=REVCRYPT_LATEST_VERSION);

  /// Decrypting
  int ReversibleDecrypt(const void* pXorKey=NULL, uint uiXorKeyLen=0,
                        const void* pRotKey=NULL, uint uiRotKeyLen=0, 
                        short *psVersion=NULL);

  /// Do xor logical encoding
  void Xor(const void* pXor, uint uiXorLen);

  /// Binary shifting using shift key
  void BinRotLeft(const void* pRot, uint uiRotLen);
  void BinRotRight(const void* pRot, uint uiRotLen);

  /// Random key generation
  ///
  /// @param iKeyLen key length
  /// @param bInitRand initialize random serie
  /// @return crypted key
  static void KeyGen(CMemBuf *pmbKey, int iKeyLen, bool bInitRand=false);
};

// In order to rename CMemBuf type to MemBuf
typedef  CMemBuf MemBuf;

// Reference pointer to CMembuf type definition
typedef RefPtr<CMemBuf> MemBufPtr;

} // namespace soleil

#endif // __MEMBUF_H___
