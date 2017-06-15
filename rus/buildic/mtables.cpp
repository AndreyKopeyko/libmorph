# include "mtables.h"

// CMixTable

// �������������� ���� ����������� � ������ ����������
//{ ��            - ����� ��������� ��������� ����� "��"          }
//{ ��            - ����� ��������� ��������� ����� ������� + �   }
//{ �             - ������� - �, �, �, �, �                       }
//{ �             - ������� �, �, �                               }
//{ �             - ������ �                                      }
//{ �             - ������ �                                      }
//{ �             - ��� ���������� "�������" �������              }

//=======================================================================
// Method:: MapMixType
// ���������� ��� ����������� �� ����������� ������
//=======================================================================
static int  MapMixType( const char* strmixtype )
{
  const struct
  {
    const char* typeSt;
    int         typeId;
  } typeList[] =
  {
    { "��", 1 },
    { "�",  5 },
    { "�",  3 },
    { "��", 0 },
    { "�",  6 },
    { "�",  2 },
    { "�",  4 }
  };

  for ( int i = 0; i < (int)(sizeof(typeList) / sizeof(typeList[0])); i++ )
  {
    int   cmpres = strcmp( strmixtype, typeList[i].typeSt );

    if ( cmpres > 0 )
      continue;
    if ( cmpres < 0 )
      break;
    return typeList[i].typeId;
  }
  return -1;
}

inline  bool  StemHasTail( const char* stem, const char* tail )
{
  size_t  ccStem = strlen( stem );
  size_t  ccTail = 0x0f & *tail++;

  return ccTail <= ccStem && memcmp( stem + ccStem - ccTail, tail, ccTail ) == 0;
}
         
unsigned  mixtable::GetMix( unsigned short type, const char* stem, const char* rems )
{
  const mixclass* p;

  for ( p = begin(); p < end(); ++p )
  {
  // ���������, ���� �� ���������� ����� ����� � ������������ �� ���������
    if ( !StemHasTail( stem, GetDefText( tables, p->offset ) ) )
      continue;

  // ���� ����� �������� ��������, ����������� ����������� �������
  // ����������� ������������ �����������
    if ( (type & 0x001F) >= 1 && (type & 0x001F) <= 6 )
    {
    // ������ �������������, ���� ��������� ����������� ��� ��� -
    // ������� ������, � ���� ���� ���������� ������ ������� ��-
    // ��������� � ������ ����������� �����
      if ( strcmp( p->szcond, rems ) != 0 && strcmp( p->szcond, "�" ) != 0 )
        continue;

      return p->offset;
    }
      else
    {
      size_t  ccStem = strlen( stem );

      switch ( MapMixType( p->szcond ) )
      {
      /* �� */
        case 0:
          if ( ccStem < 3 )
            break;
          if ( memcmp( stem + ccStem - 3, "��", 2 ) != 0 )
            break;
          return p->offset;
      /* �� */
        case 1:
          if ( ccStem < 3 )
            break;
          if ( stem[ccStem - 2] != '�' )
            break;
          if ( strchr( "���������", stem[ccStem - 3] ) == NULL )
            break;
          return p->offset;
      /* �  */
        case 2:
          if ( strchr( "�����", stem[ccStem - 2] ) == NULL )
            break;
          return p->offset;
      /* �  */
        case 3:
          if ( strchr( "���", stem[ccStem - 2] ) == NULL )
            break;
          return p->offset;
      /* �  */
        case 4:
          if ( stem[ccStem - 2] != '�' )
            break;
          return p->offset;
      /* �  */
        case 5:
          if ( stem[ccStem - 2] != '�' )
            break;
      /* �  */
        case 6:
          return p->offset;
        default:
          continue;
      }
    }
  }
  return 0;
}

// mixfiles

unsigned  mixfiles::GetMix( unsigned short type, const char* stem, const char* ztyp, const char* rems )
{
  int*  tabpos;

  if ( (tabpos = tableref.Search( ztyp )) != NULL )
    return (*this)[(size_t)*tabpos].GetMix( type, stem, rems );

  return 0;
}
