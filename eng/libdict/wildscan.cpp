#include "wildscan.h"

namespace __libmorpheng__
{

  //=====================================================================
  // Meth: InsertChar
  // ������� ��������� � ��������������� ������ �������� string ������
  // length ������ symbol � ���������� ����� ����� ������������ ������
  //=====================================================================
  inline  int   InsertChar( byte08_t* string,
                            int       length,
                            byte08_t  symbol )
  {
    int     chrpos = 0;

    while ( ( chrpos < length ) && ( string[chrpos] < symbol ) )
      chrpos++;
    if ( chrpos == length ) string[length++] = symbol;
      else
    if ( string[chrpos] != symbol )
    {
      memmove( string + chrpos + 1, string + chrpos, length - chrpos );
      string[chrpos] = symbol;
      length++;
    }
    return length;
  }

  inline  bool  IsWildChar( byte08_t c )
  {
    return ( c == '*' ) || ( c == '?' );
  }

  inline  bool  IsWildMask( const byte08_t* string, int size )
  {
    return ( memchr( string, '*', size ) != NULL )
        || ( memchr( string, '?', size ) != NULL );
  }

  inline  int   WildOffset( const byte08_t* string, int size )
  {
    int  offset = 0;

    while ( ( offset < size ) && !IsWildChar( string[offset] ) )
      offset++;
    return offset;
  }

  typedef struct tagWCFlexInfo
  {
    byte08_t iform;
    byte08_t wType;
    byte08_t wChar;
  } SWCFlexInfo;

  static int  WildFlexComp( word16_t        tfOffs,
                            const byte08_t* lpWord,
                            int             ccWord,
                            SWCFlexInfo*    lpInfo )
  {
    const byte08_t* ptable = fxTables + tfOffs;
    int             fcount = *ptable++;
    int             nBuilt = 0;
    int             ccPure = WildOffset( lpWord, ccWord );

  // ��������� ��� ��������� �������
    while ( fcount-- > 0 )
    {
      byte08_t        idform = *ptable++;
      const byte08_t* tfText = (const byte08_t*)fxString + GetWord16( ptable );
      int             ccText = tfText[0];
      int             ccComp = minimal( ccText, ccPure );
      int             rsComp;

    // �������� ��������� �� ������� ������� ���������
      ptable += sizeof(word16_t);

    // ��������� ����� ������� ��������, ����� ����� ��� � ��� ��
    // ����� ��������� ������ ���������.
      rsComp = ( ccComp > 0 ? memcmp( tfText + 1, lpWord, ccComp ) : 0 );
      if ( ccComp > 0 )
      {
        if ( rsComp > 0 ) return nBuilt;
          else
        if ( rsComp < 0 ) continue;
      }

      lpInfo->iform = idform;

    // ����, ���� ����� ���������� ��������� ���������.
    // wildcard � ����� ����� ������� �� ���� ��������, � �����
    // �� �������. ������� ����� ��������, ����� wildcard ��
    // �������� �� ���� ��������.
      if ( ccComp == ccText )
      {
      // � ��� ����� �����, ������� �������� �� ������ ��������,
      // ��� �����������. ����� ������� ������ ��������� ��� �������
      // � ������������ ������� ���������, �� ����������� ����, ���
      // ��������, ��� ����� ����� �������� ������ ���� ����� �����
      // ���������.
      // ����� ��������������� ��� ������:
      // - ���� ����� ����� ����� ��������� � ����� ����� ���������,
      //   � �������� �� ����� ������������ ������ �� ��������� �������,
      //   �� ������� ���������������� �������� � ������� �������
      //   � ������� �������;
      // - � ��������� ������� ���� �������� ���������� ������ ������.
        if ( ccPure == ccText && ccWord == ccPure + 1 && IsWildChar( lpWord[ccPure] ) )
        {
        // ���� ��� ������������ ������ �� ������� ���������� ������,
        // ������������ �������� �� ������ ����� ����� ����� ���������
          lpInfo->wType = lpWord[ccPure];
          lpInfo->wChar = 0;
          lpInfo++;
          nBuilt++;
        }
        continue;
      }

    // ����, ��������� � ��� ����� ��������, ��� wildcard ����� ��
    // ������������� �������� ���������. ����� ����� �������,
    // ��� �� ��������� � ����� �� �������� ccPure == ccComp
    // ����� ����� ������� ���������������� ��� ��������:
    // - *;
    // - ?.
    // ��� ��������� ������ ����������, ��� ��� ����� �����
    // ��������� ����� �� �������������.
      if ( lpWord[ccComp] == '*' )
      {
        lpInfo->wType = '*';
        lpInfo->wChar = tfText[ccComp + 1];
        lpInfo++;
        nBuilt++;
        continue;
      }
        else
    // � ��� ��� - �������. �������������� ���� �������� ���� ������,
    // ����� �������� ����� ��������� �����.
      {
      // ���� ��� ������������ ����� �� ������� ���������� ������,
      // �� ���������������� ���������� ���� ��������, ����� ?
        if ( ( ccComp < ccText - 1 ) && ( memcmp( tfText + 2 + ccComp,
          lpWord + ccComp + 1, ccText - ccComp - 1 ) != 0 ) )
        continue;
         
        if ( ccText == ccWord )
        {
          lpInfo->wType = '?';
          lpInfo->wChar = tfText[ccComp + 1];
          lpInfo++;
          nBuilt++;
        }
      }
    }
    return nBuilt;
  }

  inline  bool  HasSomeForms( word16_t        tfOffs,
                              const byte08_t* lpWord,
                              int             ccWord )
  {
    return tfOffs == 0 ? ccWord == 0 : FlexComp( tfOffs, lpWord, ccWord, NULL,
      sfStopAfterFirst, NULL ) != 0;
  }

  static int  WildStemComp( const byte08_t* lpStem,
                            const byte08_t* lpWord,
                            int             ccWord,
                            byte08_t*       lpDest,
                            int&            ccDest )
  {
    word16_t  idStem = GetWord16( lpStem );
    byte08_t* lpInfo = GetInfoByID( idStem );
    word16_t  wbInfo = GetWord16( lpInfo );
    word16_t  tfOffs = 0;
    int       ccPure = WildOffset( lpWord, ccWord );

  // ������� ������ �������� �����������
    lpStem += sizeof(word16_t);
    lpInfo += sizeof(word16_t);
    if ( wbInfo & wfFlexes )
      tfOffs = GetWord16( lpInfo );

  // ���������, ��� �� ��������������.
  // ���� ������������� ����, ������� ���������, �� �������� ��
  // * ��� ? �� ����. � ������ ��������� ������� ����������
  // ��� � ����������� ��������� ��������.
    if ( idStem & wfSuffix )
    {
      byte08_t* lpFrag = GetFragByID( *lpStem++ );
      int       ccFrag = lpFrag[0];
      int       ccComp = minimal( ccFrag, ccPure );
      int       rsComp = ( ccComp > 0 ? memcmp( lpWord, lpFrag + 1, ccComp ) : 0 );

    // ����� ������� �������� ������������ ����� � ����������� ����������
      if ( ccComp > 0 )
      {
        if ( rsComp < 0 ) return -1;
          else
        if ( rsComp > 0 ) return 0;
      }
    // ���� wildcard �� �������� �� �������������, ��������� ���
    // �� �������� � ������� �������� �������
      if ( ccComp == ccFrag )
      {
        lpWord += ccFrag;
        ccWord -= ccFrag;
        ccPure -= ccFrag;
      }
        else
    // � ��������� ������ ������� ���������� 2 ��������:
    // � ������ ��������� - ������ �������� � �������� ������
    // ������, ������� ����� �� ����� ���� ���������.
      if ( lpWord[ccComp] == '*' )
      {
        ccDest = InsertChar( lpDest, ccDest, lpFrag[ccComp + 1] );
        return 0;
      }
        else
    // � ��������� ������ � ������� ccComp ����� '?'. ����� ���
    // ���������� ���� ������ ����������, �����:
    // - ��������� ����� �������������� ��������� �� ������
    //   ����� �������;
    // - ��� ��������� ������ ��������������� �� ������������
    //   � �� ������� ���������.
      {
      // ������� �������� � ������������� ���������� �����
        if ( ( ccComp < ccFrag - 1 ) && ( ccWord >= ccFrag ) && ( memcmp( lpWord + ccComp + 1,
          lpFrag + ccComp + 2, ccFrag - ccComp - 1 ) != 0 ) )
        return 0;
      // ������ ��������� (����� ���������) ����� ����� ������ ��������-
      // ������� �� ���������� � ������������ (���� ����)
        if ( ( tfOffs == 0 ) && ( ccWord != ccFrag ) )
          return 0;

        if ( !HasSomeForms( tfOffs, lpWord + ccFrag, ccWord - ccFrag ) )
          return 0;
        ccDest = InsertChar( lpDest, ccDest, lpFrag[ccComp + 1] );
      }
    }
  // ����, ����� ������������� ����������. ������ ������ �����
  // ��� ������� ����������� � ���������.
  // ������ ������������ ���� - ����� �������. ������������,
  // ��� ����� ������������� - ��� ��������� ����� �����.
    if ( tfOffs == 0 )
    {
      if ( ( ccWord == 1 ) && IsWildChar( lpWord[0] ) )
        ccDest = InsertChar( lpDest, ccDest, 0 );
      return 0;
    }
  // ����� - ���������� �����. ���������� ������ ��� �����������.
  // ������� ������� ������ ������ ��������� � ����������������
  // ��� �������, ������� ��� ����� �������.
  // ��������� ��� ��������� �������� ��������� ������
    SWCFlexInfo wfInfo[32];
    int         nItems = WildFlexComp( tfOffs, lpWord, ccWord, wfInfo );

    while ( nItems-- > 0 )
      ccDest = InsertChar( lpDest, ccDest, wfInfo[nItems].wChar );
    return 0;
  }

  static int  WildScanPage( const byte08_t* lpPage,
                            word16_t        offset,
                            const byte08_t* lpWord,
                            int             ccWord,
                            byte08_t*       lpDest,
                            int             length )
  {
    byte08_t* scanTop = (byte08_t*)lpPage + offset + sizeof(word16_t);
    byte08_t* scanPtr = scanTop + scanTop[0];
    word16_t* offsPtr = (word16_t*)(scanTop + scanTop[0] * 3 - 1);
    int       pureLen = WildOffset( lpWord, ccWord );

    if ( scanTop[0] & 0x80 )
    {
      int   ccFrag = scanTop[0] & 0x7F;
      int   ccComp = minimal( ccFrag, pureLen );
      int   rsComp = ( ccComp > 0 ? memcmp( scanTop + 1, lpWord, ccComp ) : 0 );

    // ���� ���� ������������ ����� � ������, �� ��������� ��������
    // ������ ������� ���������
      if ( ( ccComp != 0 ) && ( rsComp != 0 ) )
        return length;

    // ���������, �������� �� �������� wildcard ��� �� ��������
      if ( ccComp == ccFrag )
      {
      // �������� �� �������� �� wildcard. � ���� ������ ����
      // ������� ������ ������
        byte08_t* lpStem = scanTop + ccFrag + 1 + sizeof(word16_t);
        int       cStems = *lpStem++;
    
        while ( cStems-- > 0 )
        {
          if ( WildStemComp( lpStem, lpWord + ccFrag, ccWord - ccFrag,
            lpDest, length ) < 0 )
          break;
          lpStem += GetSkipCount( lpStem );
        }
      }
        else
      {
        byte08_t* lpStem;
        int       cStems;

      // �������� �������� wildcard
      // ���� ���� wildcard - *, ������ �������� ��������� ������,
      // ��� ��� ������ �� ����� ������� ���� ����������
        if ( lpWord[pureLen] == '*' )
          return InsertChar( lpDest, length, scanTop[pureLen + 1] );

      // ���� ���� wildcard - ?, ������ ���� ���������� � ���������
      // ������ ��������� � ���������� ���������.
      // ���������� �� ������� � ��� ���� (��. ����, �� ccComp),
      // ��������� ���� ���������� ����� ����
        if ( ( pureLen < ccFrag - 1 ) && ( memcmp( lpWord + ccComp + 1,
          scanTop + ccComp + 2, ccFrag - ccComp - 1 ) != 0 ) )
        return length;

      // ������� ������ � �������� �������������� ���������� ����� �����
      // �� ����������
        lpStem = scanTop + ccFrag + 1 + sizeof(word16_t);
        cStems = *lpStem++;

        while ( cStems-- > 0 )
        {
          SGramInfo grInfo[32];
          SScanPage rfscan;

          rfscan.nFlags = sfIgnoreCapitals|sfStopAfterFirst;

          if ( StemComp( lpStem, lpWord + ccFrag, ccWord - ccFrag,
            grInfo, rfscan ) )
          break;
          lpStem += GetSkipCount( lpStem );
        }
        if ( cStems >= 0 )
          length = InsertChar( lpDest, length, scanTop[ccComp + 1] );
      }
      return length;
    }

    if ( lpWord[0] == '*' )
    {
      while ( ( scanPtr > scanTop ) && ( scanPtr[0] != 0 ) )
      {
        length = InsertChar( lpDest, length, scanPtr[0] );
        scanPtr--;
        offsPtr--;
      }
      if ( ( scanPtr > scanTop ) && ( scanPtr[0] == 0 ) )
      {
        byte08_t* lpStem = (byte08_t*)lpPage + GetWord16( offsPtr )
          + sizeof(word16_t);
        int       cStems = *lpStem++;
      
        while ( cStems-- > 0 )
        {
          if ( WildStemComp( lpStem, lpWord, ccWord, lpDest, length ) < 0 )
            break;
          lpStem += GetSkipCount( lpStem );
        }
      }

      return length;
    }
      else
    if ( lpWord[0] == '?' )
    {
      SScanPage rfScan;

      rfScan.nFlags = sfStopAfterFirst|sfIgnoreCapitals;

      while ( ( scanPtr > scanTop ) && ( scanPtr[0] != 0 ) )
      {
        if ( ScanPage( lpPage, GetWord16( offsPtr ), lpWord + 1, ccWord - 1,
          rfScan, NULL ) )
        length = InsertChar( lpDest, length, scanPtr[0] );
        scanPtr--;
        offsPtr--;
      }
      if ( ( scanPtr > scanTop ) && ( scanPtr[0] == 0 ) )
      {
        byte08_t* lpStem = (byte08_t*)lpPage + GetWord16( offsPtr )
          + sizeof(word16_t);
        int       cStems = *lpStem++;
      
        while ( cStems-- > 0 )
        {
          if ( WildStemComp( lpStem, lpWord, ccWord, lpDest, length ) < 0 )
            break;
          lpStem += GetSkipCount( lpStem );
        }
      }
      return length;
    }
      else
    {
      while ( scanPtr > scanTop )
      {
        while ( ( scanPtr > scanTop ) && ( scanPtr[0] != lpWord[0] )
          && ( scanPtr[0] != 0 ) )
        {
          scanPtr--;
          offsPtr--;
        }
        if ( scanPtr == scanTop )
          return length;
        if ( scanPtr[0] == lpWord[0] )
        {
          length = WildScanPage( lpPage, GetWord16( offsPtr ), lpWord + 1, ccWord - 1,
            lpDest, length );
        }
          else
        {
          byte08_t* lpStem = (byte08_t*)lpPage + GetWord16( offsPtr )
            + sizeof(word16_t);
          int       cStems = *lpStem++;

          while ( cStems-- > 0 )
          {
            if ( WildStemComp( lpStem, lpWord, ccWord, lpDest, length ) < 0 )
              break;
            lpStem += GetSkipCount( lpStem );
          }
        }
        scanPtr--;
        offsPtr--;
      }
      return length;
    }
  }

  //=====================================================================
  // Method:: ::WildScanDictionary
  //=====================================================================
  int  WildScanDictionary( CPageMan&        main,
                           const byte08_t*  word,
                           byte08_t*        lpDest )
  {
    unsigned char*  lplast = NULL;
    unsigned char*  lppage;
    unsigned        uindex;
    int             ccdest = 0;
    int             ccword = strlen( (char*)word );
    int             ofwild = WildOffset( word, ccword );

  // ��� ������ ������� ����������, ����� ����� ��������� wildcard.
  // ���� ��� �������� �� ��������, ����� ������� 0
    if ( !IsWildMask( word, ccword ) )
      return 0;

  // ���������, ��������� �� ����� ����������� �����
    if ( word[ofwild] == '*' )
      ccword = ofwild + 1;

    for ( uindex = 0; uindex < main.GetCount(); uindex++ )
    {
      lppage = main[uindex];

      if ( lppage == NULL || lppage == lplast )
        continue;
      ccdest = WildScanPage( lplast = lppage, 0, (byte08_t*)word, ccword,
        lpDest, ccdest );
    }
    return ccdest;
  }

}
