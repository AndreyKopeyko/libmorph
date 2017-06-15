# include "lresolve.h"
# include <libcodes/codes.h>
# include <tools/ftables.h>
# include <assert.h>

#if defined( _MSC_VER )
  #pragma warning( disable: 4237 )
#endif

//=====================================================================
// Method: GetRemark()
// ������� ���� ������ �� ������������ � �����������, ���������������
// ������� -nx-, ��� nx - ��������� ���������� ��������, � ����������
// ������ - ������.
//=====================================================================
static  char* GetRemark( char* lpdest, const char* string )
{
  const char* strTop;
  const char* strEnd;
  
  *lpdest = '\0';

  if ( (strEnd = strTop = strchr( string, '-' )) == nullptr )
    return lpdest;

  do ++strEnd;
    while ( *strEnd != '\0' && *strEnd != '-' && (unsigned char)*strEnd > 0x20 );

  if ( *strEnd == '-' )
    strncpy( lpdest, strTop + 1, strEnd - strTop - 1 )[strEnd - strTop - 1] = '\0';

  return lpdest;
}

//=========================================================================
// Method: GetCaseScale( ... )
//
// ������� ��������� �������� ����� �������� �� ������ ����������� � �����
//=========================================================================
static byte_t GetCaseScale( const char* string )
{
  static char caseString[] = "������";
  static char caseScales[] = "\x01\x02\x04\x08\x10\x20";
  byte_t      fetchValue = 0;
  const char* searchChar;

  if ( (string = strstr( string, "��:" )) != nullptr )
    for ( string += 3; *string != '\0' && (searchChar = strchr( caseString, *string )) != nullptr; ++string )
      fetchValue |= caseScales[searchChar - caseString];

  return fetchValue;
}

//=====================================================================
// Method: ExtractPostfix()
// ������� ��������� ������ ��������� �� ����������� � ����� � ������-
// ���� ����� ���������
//=====================================================================
inline  int   ExtractPostfix( char* szpost, const char* string )
{
  const char* strtop;
  const char* strend;

// ���������, ���� �� �������� � ������, ���������� �������
// ������� ���������� ����� ���������
  if ( (strtop = strstr( string, "post:" )) == NULL )
    return 0;
  for ( strtop += 5; *strtop != '\0' && (unsigned char)*strtop <= 0x20; strtop++ )
    (void)NULL;
  for ( strend = strtop; (unsigned char)*strend > 0x20; strend++ )
    (void)NULL;

// ����������� �����
  strncpy( szpost, strtop, strend - strtop )[strend - strtop] = '\0';
    return (int)(strend - strtop);
}

bool  ResolveClassInfo( const zarray<>& ztypes,
                        char*           pstems,
                        rusclassinfo&   clinfo,
                        const char*     sznorm, const char*   szdies, const char*   sztype, const char* zapart, const char*   szcomm,
                        const char*     ftable, CReferences&  findex, const char*   mtable, mixfiles&   mindex )
{
  char    ansiType[32];
  char    utf8Type[32];
  char    origType[32];
  char    szremark[32];
  int     mixIndex = 0;

// ����� ���������� ��� �����
  strcat( strcat( strcpy( ansiType, szdies ), " " ), sztype );
  strcat( strcat( strcpy( origType, sztype ), " " ), zapart );

  codepages::mbcstombcs( codepages::codepage_utf8, utf8Type, sizeof(utf8Type),
                         codepages::codepage_1251, ansiType );

  if ( (clinfo.wdinfo = (word16_t)(int32_t)ztypes[utf8Type]) == 0 )
    return false;

// ����� �������� �������� ���� �����, ����� �������� � ���������
// ������������ ����� ����, ���� ������� ����������� ����. � ����
// ������� �������� �� ������� ������ �� ������� ��������� ������
// ��������� ������ ��������� � ��������� � ������ ����������.
  if ( (clinfo.wdinfo & 0x3F) < 48 )
  {
    if ( (clinfo.tfoffs = findex.GetOffset( origType )) == 0 && strcmp( zapart, "0" ) != 0 )
      return false;
  }
    else
  clinfo.tfoffs = 0;

// ����������� �������� ����� ����� � �������� ��������, ���� �� ����
  for ( auto d = strcpy( pstems, sznorm ), s = pstems; (*d = *s++) != '\0'; )
    if ( *d != '=' )  ++d;

// �������� ��������
  pstems[strlen( sznorm ) - ExtractPostfix( clinfo.szpost, szcomm )] = '\0';

// ������� ����� �������� ����������� ����
  if ( strstr( szcomm, "[�������.]" ) != nullptr )
    clinfo.wdinfo |= wfExcellent;
  if ( strstr( szcomm, "{������.}" ) != nullptr )
    clinfo.wdinfo |= wfCountable;
  if ( strstr( szcomm, "{����.}" ) != nullptr )
    clinfo.wdinfo |= wfInformal;
  if ( strstr( szcomm, "{���.}" ) != nullptr )
    clinfo.wdinfo |= wfObscene;

// ��������� ��������� ������������ ����
  if ( clinfo.tfoffs == 0 )
  {
  // ���� ����� - �������, ������� �������� �����
    if ( (clinfo.wdinfo & 0x3F) == 51 )
      clinfo.tfoffs = GetCaseScale( zapart );
    clinfo.chrmin = clinfo.chrmax = '\0';
      return true;
  }

// � ������, ���� ������ �� ������� ��������� ���������, ���������
// ��������� ���������� ����� ��� ���������� ���������� ��������
// @ - ��� �������, ����� � ����������� ����� ���������� �����
// ����� �� �������������
// ������� �������� ��������, ��� �� ���������� �����������
// ���������, ��� ��� ������, ��������, � ���������������, ����-
// ��������� ������ � ������������ ������. ���� ���, �� ����-
// ����� ����������� ������� ����������� ����� ������.
  if ( strchr( pstems, '@' ) != NULL ) strchr( pstems, '@' )[0] = '\0';
    else
  {
  //   � ��������� ������ �������� ������ �� ���� ����� ��������-
  // ������ ���������� � ���������� ����� � ���������� ���������
  // ��� ����������. ��� ������, ������, ����������� ������.
  //   � ������ �������� (���� 1..6) ���������� ������ ��������
  // ��������� (� ��������� ���������� ��������). � ������ �����-
  // ��������� � ������ ����, ������������ �� �����, ������������
  // ������� �������� ���� � ���������� � ������������� ������.
    unsigned short* lpLevels;
    unsigned short  normInfo;

    switch ( clinfo.wdinfo & 0x3F )
    {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        normInfo = vtInfinitiv|gfRetForms;
        lpLevels = (unsigned short*)verbLevels;
        break;
      case 25:
      case 26:
      case 27:
      case 28:
      case 34:
      case 36:
      case 42:
      case 52:
        normInfo = 1 << 9;    // ������� ��� ��� ��������������
        if ( strcmp( pstems + strlen( pstems ) - 2, "��" ) == 0 )
          normInfo |= gfRetForms;
        lpLevels = (unsigned short*)nounLevels;
        break;
      default:
        normInfo = 0;
        lpLevels = (unsigned short*)nounLevels;
        break;
    }
  // ���������� ������������� �����, ���� ��� ����
    if ( clinfo.wdinfo & wfMultiple )
      normInfo |= gfMultiple;

  // �������� ��������� ���������� �����
    if ( !StripDefault( pstems, normInfo, clinfo.tfoffs, lpLevels, 0, ftable ) && ( strchr( zapart, ':' ) == NULL ) )
      return false;
  }

// ���������������� ����������� � ������������ ������� �� ������� ���������
  clinfo.chrmin = GetMinLetter( ftable, clinfo.tfoffs );
  clinfo.chrmax = GetMaxLetter( ftable, clinfo.tfoffs );

// ����� ���������� ��������� ���������� ����� ����������� �����-
// ������ �� ������� �����������.
  clinfo.mtoffs = mindex.GetMix( clinfo.wdinfo, pstems, origType, GetRemark( szremark, szcomm ) );

// ���� ����������� ������������, �� ����������� ������ �������
// ����������� � ������, �. �. ��� ������������� ���������� �����.
  if ( clinfo.mtoffs != 0 )
  {
    clinfo.chrmin = GetMinLetter( mtable, clinfo.mtoffs, clinfo.chrmin );
    clinfo.chrmax = GetMaxLetter( mtable, clinfo.mtoffs, clinfo.chrmax );

  // ���������, ��� �� ������ �������� ���� �����������
  // �������� ����������� ������ �������
    if ( (clinfo.wdinfo & 0x3F) <= 6 )
    {
      switch ( zapart[0] )
      {
        case '6':
        case '9':
          mixIndex = 1;
          break;
        case '5':
          if ( memcmp( zapart, "5c/c", 4 ) == 0 || memcmp( zapart, "5*c/c", 5 ) == 0 )
            mixIndex = 1;
          break;
        case '1':
          switch ( zapart[1] )
          {
            case '0': // 10
            case '4': // 14
              mixIndex = 1;
              break;
            case '1': // 11
              mixIndex = 2;
              break;
          }
          break;
      }
    }
    pstems[strlen( pstems ) - (0x0f & *GetDefText( mtable, clinfo.mtoffs ))] = '\0';
  }
  if ( mixIndex != 0 )
    clinfo.wdinfo |= (mixIndex << 8);
  return true;
}

#if defined( _MSC_VER )
  #pragma warning( default: 4237 )
#endif
