# include "scandict.h"
# include "capsheme.h"
# include <string.h>
# include <assert.h>

namespace __libmorpheng__
{

  # define cmpmem( p1, p2, cb )  ( (cb) != 0 ? memcmp( (p1), (p2), (cb) ) : 0 )

  /************************************************************************/
  /* ������� ���������� ������ ����� ����� �� �������������� ��������     */
  /* ��������� � ���������� ���������� ���������������� ���������, �      */
  /* ����� ��������� ������ FlxInfo �������������� ����������� �� ����    */
  /* �����������.                                                         */
  /************************************************************************/
  int   FlexComp( word16_t        tfoffs,
                  const byte08_t* pszstr,
                  int             cchstr,
                  byte08_t*       pforms,
                  unsigned        dwsets,
                  const byte08_t* szpost )
  {
    char* fTable = (char*)fxTables + tfoffs;      // ������� ���������
    int   iCount = (unsigned char)*fTable++;      // ���������� ���������
    int   nitems = 0;                             // ���������� ��������������

    while ( iCount-- > 0 )
    {
      unsigned char idform = (unsigned char)*fTable++;
      const char*   szflex = (const char*)fxString + GetWord16( fTable );
      int           ccflex = *szflex++;
      int           cmplen = minimal( cchstr, ccflex );
        fTable += sizeof(word16_t);
      int           rescmp = cmplen > 0 ? memcmp( szflex, pszstr, cmplen ) : 0;

    // compare the string with element; if nothing more, return immidiately
      if ( cmplen > 0 && rescmp > 0 )
        break;

    // check if equal
      if ( cmplen < ccflex || rescmp < 0 )
        continue;

    // check complete lemmatization
      if ( cmplen < cchstr )
      {
        if ( szpost == NULL || ccflex + *szpost != cchstr )
          continue;
      }

    // check for postfix if present
      if ( szpost != NULL )
      {
        if ( *szpost != cchstr - ccflex || memcmp( pszstr + ccflex, szpost + 1, *szpost ) != 0 )
          continue;
      }

    // create grammatical description
      if ( pforms != NULL )
        *pforms++ = idform;
      ++nitems;
    }
    return nitems;
  }

  //=====================================================================
  // Method: ::CheckOnStems
  // ��� ����� � ������� pszStem ��������� �� ���������� ��������������
  // �������� ��������� �����.
  // ���������:
  //   stem - ��������� �� �������� ��������� ������;
  //   word - ��������� �� ����������� �����;
  //   size - ��� �����;
  //   lIdx - ����������� �����;
  //   info - ��������� �� ������ �������� ��������������;
  //   scan - ��������� �������
  // ������� ���������� 0, ���� ����� �� ���� �������������,
  //   �������� ������ ����, ���� ��������� ��������� ����������
  //   �������������� �������� � ������������� ��������, ���� ������������
  //   ������ ���� ������� ����������.
  //=====================================================================
  int  StemComp( const byte08_t*  stem,
                 const byte08_t*  word,
                 int              size,
                 SGramInfo*       info,
                 SScanPage&       scan )
  {
    const byte08_t* postText;         // ����������� �����
    const byte08_t* wordStem;         // �������� ��������������
    word16_t        idwdStem;         // ����� ������
    word16_t        stemInfo;
    byte08_t        flexlist[32];
    bool            getFirst;

  // ������� ����� �������� ������, ��������� ��������� �� ��������
  // �������������� � ������� ��� �����
    idwdStem = GetWord16( stem );
    stem += sizeof(word16_t);
    wordStem = GetInfoByID( idwdStem );

    stemInfo = GetWord16( wordStem );
    wordStem += sizeof(word16_t);

  // ����� ������� � ������� �����, ��� �������� �� ��������� �����
  // �������������, ���� ������������ ���������
    if ( (scan.nFlags & sfIgnoreCapitals) == 0 && !IsGoodSheme( scan.scheme,
      GetWordCaps( stemInfo ) ) )
        return 0;
    
  // ���� ����� ����� �������������, �������� ����� � ��� � ����������
  // ������, ���� ����� ����������� �� ������ �������� �� ��� �����.
    if ( (idwdStem & wfSuffix) != 0 )
    {
      byte08_t* lpfrag = GetFragByID( *stem++ );
      int       ccfrag = *lpfrag++;
      int       cccomp = ccfrag <= size ? ccfrag : size;
      int       rescmp = cccomp > 0 ? memcmp( word, lpfrag, cccomp ) : 0;
    
      if ( rescmp < 0 ) return -1;
        else
      if ( rescmp > 0 ) return 0;
        else
      if ( cccomp == ccfrag )
      {
        word += ccfrag;
        size -= ccfrag;
      }
        else
      {
        assert( ccfrag > size );
        return -1;
      }
    }

  // ����� ����� ����� �������������� ������������ ��������, �������
  // ������ ��������� �� ���������� ������. ���� ����, ������� ���
    postText = (idwdStem & wfPostSt) != 0 ? stem : NULL;

  // ����� �������������� 2 ������: ����� ����� ����������� � ������
  // �, ��������������, �� ����� ��������.
  // �� ������ ������ ������� ����������� ��� � ������ ������������ ����
  // ������ ����� ������ � ������������� �������
    if ( (stemInfo & wfFlexes) == 0 )
    {
    // ��� ������������ ���� � ���� ����� �������� size (����� ���������-
    // �������� ����� �����) ������ ���� 0. ���� ��� �� ���, �� ��������-
    // ������ ��� � ������ ������� ����� ����� ���������
      if ( size != 0 )
        return 0;

    // ������ ������������, ��� ����� ����� ������� - ��� ��������� ����-
    // ���������� ��������, ���� ��� ���� ����-������ �����
      if ( info != NULL )
      {
        info->wInfo = (byte08_t)(stemInfo & 0x7F);
        info->iForm = 0xFF;
        info->gInfo = ( (stemInfo & 0x3F) == 51 ? *(word16_t*)wordStem : 0 );
        info->other = 0;
        info++;
      }
      return 1;
    }

  // get plural stem flag - outside from loop
    getFirst = (scan.nFlags & sfStopAfterFirst) != 0;

  // ������ - ��������� ���������� ����. ������� ���������� �����
  // ������ ������ - ���������� ����� ��� ����������� � ������.
  // ��� ����� ���������� ���������, �� ��������� �� ���� �������
  // �����������, ��� ��� ��� ��������� � ��� ����� ����� ��������
  // ����� ����������.
    word16_t    tfOffs = GetWord16( wordStem );
    int         nForms = 0;
    int         nItems = FlexComp( tfOffs, word, size, flexlist, scan.nFlags, postText );
    byte08_t*   pstart;
    byte08_t*   pfinal;

  // ���� �� ������ �������������� �� ���������� �� ���������,
  // ��������� ������. ��� �� ���������� ������������, ����
  // ����� ������������ ������������� ��������� �����, � ��� ���
    if ( nItems != 0 )  pfinal = (pstart = flexlist) + nItems;
      else  return 0;      

  // if a simple spelling check, perform a simple test for presence
  // of plural forms in a forms list if the stem has the plural flag;
  // else simply return 1
    if ( getFirst )
      return 1;

  // this is not a spelling check; info != NULL (strongly checked),
  // store the string grammatical description data
    assert( info != NULL );

    for ( ; pstart < pfinal; pstart++, info++, nForms++ )
    {
      info->wInfo = (byte08_t)(stemInfo & 0x7F);
      info->iForm = *pstart;
      info->other = 0;
    }
    return nForms;
  }

  inline  bool  ScanList( const byte08_t* lplist,
                          const byte08_t* lpword,
                          int             ccword,
                          SScanPage&      rfscan,
                          FAction         action )
  {
    SGramInfo   grinfo[MAX_WORD_FORMS];
    int         nstems = *lplist++;     // get the number of stems
    SGramInfo*  pginfo = rfscan.nFlags & sfStopAfterFirst ? NULL : grinfo;
    bool        bfound = false;

    for ( bfound = false; nstems-- > 0; lplist += GetSkipCount( lplist ) )
    {
      int       nfound;

    // ��������� �������� �������������� �����
      nfound = StemComp( lplist, lpword, ccword, pginfo, rfscan );

    // check if nothing more in the list
      if ( nfound <  0 )
        break;

    // check if stem to ignore
      if ( nfound == 0 )
        continue;

    // else the stem is relevant; check if simple stemming
      if ( pginfo == NULL )
        return true;

    // ���������, ��� ����� �������
      if ( action != NULL )
        action( lplist, lpword, grinfo, nfound, rfscan );
      bfound = true;
    }
    return bfound;
  }

  //*************************************************************************
  // Method: ::ScanPage
  //   ������� ��������� �������� �������� ������� � ���������� true,
  //   ���� ������� ���-���� ������������.
  // ���������:
  //   page - ��������� �� ������ ����������� �������� �������;
  //   offs - ��������, � �������� �������� ������������;
  //   word - ��������� �� �����, ������� �����������;
  //   size - ����� ����� �����;
  //   scan - ������ �� ��������� ������, ������������ ��� �������;
  //   action - ����� ������, ����������� ��� �������������� (�. �. NULL)
  //*************************************************************************
  //=====================================================================
  // Method: ::ScanPage
  //   ������� ��������� �������� �������� ������� � ���������� true,
  // ���� ������� ���-���� ������������.
  // ���������:
  //   page - ��������� �� ������ ����������� �������� �������;
  //   offs - ��������, � �������� �������� ������������;
  //   word - ��������� �� �����, ������� �����������;
  //   size - ����� ����� �����;
  //   scan - ������ �� ��������� ������, ������������ ��� �������;
  //   action - ����� ������, ����������� ��� �������������� (�. �. NULL)
  //=====================================================================
  bool ScanPage( const byte08_t*  page,
                 word16_t         offs,
                 const byte08_t*  word,
                 int              size,
                 SScanPage&       rfScan,
                 FAction          action )
  {
    byte08_t* basePtr = (byte08_t*)page + offs + sizeof(word16_t);
    byte08_t* scanPtr = basePtr + (basePtr[0] & 0x7F);
    word16_t* offsPtr = (word16_t*)(basePtr + (basePtr[0] & 0x7F) * 3 - 1);
    bool      rSearch = false;
    bool      doFirst;

  // first check if the branch is degenerated
    if ( (*basePtr & 0x80) != 0 )
    {
      int   ccfrag = *basePtr++ & 0x7F;   // the fragment length

    // check if longer than the checked word
      if ( ccfrag > size )
        return false;

    // ���� �������� �� ������ � ������� �������������������
    // ��������, ����� ����� ���� ������ ����������� ������
      if ( ccfrag != 0 && memcmp( basePtr, word, ccfrag ) != 0 )
        return false;

    // call stem scanner
      return ScanList( basePtr + ccfrag + sizeof(word16_t),
        word + ccfrag, size - ccfrag, rfScan, action );
    }

  // doFirst calculation before loop excution
    doFirst = (rfScan.nFlags & sfStopAfterFirst) != 0;

    while ( scanPtr > basePtr )
    {
    // skip until the first character equal to searched one or empty
      while ( scanPtr > basePtr && *scanPtr != *word && *scanPtr != '\0' )
      {
        --scanPtr;
        --offsPtr;
      }

    // check if nothing to scan
      if ( scanPtr <= basePtr )
        return rSearch;

    // check if the scanner stopped on equal nested character
      if ( *scanPtr == '\0' )
      {
        rSearch |= ScanList( page + GetWord16( offsPtr ) + sizeof(word16_t),
          word, size, rfScan, action );
      }
        else
      {
        rSearch |= ScanPage( page, GetWord16( offsPtr ), word + 1, size - 1,
          rfScan, action );
      }

    // check if simple spellcheck
      if ( rSearch && doFirst )
        return rSearch;

    // move further scan
      --scanPtr;
      --offsPtr;
    }
    return rSearch;
  }

  //=====================================================================
  // Method:: ::ScanDictionary
  //=====================================================================
  bool  ScanDictionary( CPageMan& main, const char* word, int size,
    SScanPage& scan, FAction action )
  {
    bool            rsSearch = false;
    unsigned char** nextpage = NULL;
 
    while ( (nextpage = main.FindPage( (unsigned char)*word, nextpage )) != NULL )
    {
    // Check the word in a selected page
      if ( ScanPage( *nextpage, 0, (byte08_t*)word, size,
        scan, action ) )  rsSearch = true;
      else continue;

    // Check if one page scanning is completed and nothing more required
      if ( (scan.nFlags & sfStopAfterFirst) != 0 )
        break;
    }
    return rsSearch;
  }

  // ���� ��������� �������, ������� ��� ��������� � ������ �����
  // � ������� ����� �� ����� �����, � ��� ������������� �������

  //=====================================================================
  // Method: ::BuildStemDownUp
  // ��������� ����������� ����� �� ������ ����� � ��� �����, �����
  // ��������������� ����� ������ ����� � ���������� ����� ������
  //=====================================================================
  static int  BuildStemDownUp( const char*  page,
                               word16_t     offs,
                               word16_t     refs,
                               char*        buff )
  {
    word16_t  backref = GetWord16( page + offs );
    byte08_t* baseptr = (byte08_t*)page + offs + sizeof(word16_t);
    byte08_t* scanptr = baseptr + (baseptr[0] & ~0x80);
    word16_t* offsptr = (word16_t*)(baseptr + baseptr[0] * 3 - 1);
    int       clength = 0;

    if ( refs == 0xFFFF )
      return ( backref != 0xFFFF ? BuildStemDownUp( page, backref, offs, buff ) : 0 );

  // ������� ����������� 2 ������ - ��������������� ����������� ��������
  // � ���� ������
    if ( baseptr[0] & 0x80 )
    {
    // ���� �������� ��������������� ��������, ������� �������� ���
    // �� ������� ����� � ��������������� �������
      while ( scanptr > baseptr )
      {
        buff[clength++] = (char)scanptr[0];
        scanptr--;
      }
    }
      else
    {
    // � ������ ���� ������ ��������� ��� �������� � ������� ���,
    // ������� ��������� �� ������ ��������� ����, � �������
    // ������ ������
      while ( ( scanptr > baseptr ) && ( GetWord16( offsptr ) != refs ) )
      {
        scanptr--;
        offsptr--;
      }

      assert( scanptr > baseptr );

      if ( scanptr[0] != '\0' )
      {
        buff[0] = (char)scanptr[0];
        clength = 1;
      }
    }
  // � ������ ����������� ���� ��������� �� ��� ������
    if ( backref != 0xFFFF )
      clength += BuildStemDownUp( page, backref, offs, buff + clength );
    return clength;
  }

  //=====================================================================
  // Method: ::InvertString
  // ������� ������������ ������ ���������
  //=====================================================================
  static void InvertString( char* strtop, char* strend )
  {
    char chswap;

    while ( strend > strtop )
    {
      chswap = strtop[0];
      strtop[0] = strend[0];
      strend[0] = chswap;
      strtop++;
      strend--;
    }
  }

  //=====================================================================
  // Method: ::DirectPageJump
  // ��������� ������� action �� ����������� ������ ��������
  //=====================================================================
  bool  DirectPageJump( CLIDsMan&   lids,
                        CPageMan&   main,
                        word32_t    wlid,
                        SScanPage&  scan,
                        FAction     action )
  {
  // ����������, �� ����� �������� ��������� �������� �����������
    SLidsRef*       lppage;
    unsigned char*  lpdata;
    unsigned char   pageID;
    unsigned short  offset;
    char            szword[256];
    int             length;
    int             nstems;
    unsigned char*  lpscan;
    bool            search = false;

  // ��������� �������� ������� ������  
    if ( (lppage = lids.FindPage( wlid )) == NULL )
      return false;

  // ������� ����� �������� � �������� ������
    lpdata = lppage->pagedata + (wlid - GetWord32( &lppage->minvalue )) * 3;
    pageID = *lpdata++;
    offset = GetWord16( lpdata );

  // ���� ������������� �������� �� ��������� ��� �� ���������� ��������
  // �����������, ��������� ������
    if ( pageID == 0 || offset == 0 )
      return false;

  // ������ ��������� ������ ��������. �������� �� ������ �� ��������,
  // ��� ��� ���������, ��� ������� ��������� ��������������
    lpdata = main[pageID - 1];

  // ��������� ������, �� ������� ����� ������
    length = BuildStemDownUp( (const char*)lpdata, offset, 0xFFFF, szword );
    InvertString( szword, szword + length - 1 );

    scan.lpWord = szword;

    lpscan = lpdata + offset + sizeof(word16_t);
    nstems = *lpscan++;

    while ( nstems-- > 0 )
    {
    // Test next stem if there is same LID
      char*     lpStem = (char*)lpscan;
      word16_t  idInfo = GetWord16( lpscan );
      word32_t  wdlid = 0;

    // Skip reference to class
      lpscan += sizeof(word16_t);

    // Skip suffix if present
      if ( idInfo & wfSuffix )
        lpscan++;

    // Test if postfix presents
      if ( idInfo & wfPostSt )
        lpscan += lpscan[0] + 1;

    // Extract wdlid
      memcpy( &wdlid, lpscan, ((idInfo & 0x3000) >> 12) + 1 );
      Swap32( wdlid );

    // Skip wdlid
      lpscan += ((idInfo & 0x3000) >> 12) + 1;

    // Test same
      if ( wdlid != wlid )
        continue;

    // Call action to perform form creation
      if ( action != NULL )
      {
        action( (const byte08_t*)lpStem, (const byte08_t*)szword
          + length, NULL, 0, scan );
      }
      search = true;
    }

    return search;
  }

}
