# if !defined( __grammap_h__ )
# define __grammap_h__

# include <namespace.h>
# include "mlmadefs.h"

namespace LIBMORPH_NAMESPACE
{

  //==============================================================================
  // Method:: mapCaseInfo
  // ������� ������ ���������� ����� �������� �����, �������� ��������������
  // �������� ����� (� ������ �����) � �������������� (� ������� �����).
  //==============================================================================
  inline  byte08_t  mapCaseInfo( word32_t info )
  {
    int   idcase = (info & gfFormMask) >> 12;

    return ( idcase > 3 ? idcase + 1 :
           ( idcase < 3 ? idcase :
           ( info & (afNotAlive << 16) ? 3 : 4 ) ) );
  }

  //==============================================================================
  // Method:: mapGenderOf
  // ������� ��������� ����� ���� � ����� ��� ����������� ��������������� ��������
  //==============================================================================
  inline  byte08_t  mapGenderOf( word32_t info )
  {
    return (byte08_t)( info & gfMultiple ? 3 : ((info & gfGendMask) >> 9) - 1 );
  }

  //==============================================================================
  // Method:: mapGendInfo
  // ������� ������ ����� ����-�������� ����������
  // ����������������� 7 ������� �� ������ ��� � ����� (6 ���������� + 1
  // �������������� ���.) � ����� ������� �����, 4 ���� * 8 = 32 ��������
  //==============================================================================
  inline  byte08_t  mapGendInfo( word32_t info )
  {
    return mapGenderOf( info ) * 8 + ( info & gfShortOne ? 7 : mapCaseInfo( info ) );
  }

  //==============================================================================
  // Method:: mapDefsInfo
  // ������� ������ ����� ����� ��������������� �� ��������������� �������� ��
  //==============================================================================
  inline byte08_t MapDefsInfo( word32_t info )
  {
    return ( info & gfAdverb   ? 65 :
           ( info & gfCompared ? 64 : ( info & gfRetForms ? 32 : 0 ) + mapGendInfo( info ) ) );
  }

  //==============================================================================
  // Method:: mapNounInfo
  // ������� ������ ����� ����� ���������������� �� ��� ��������������� ��������
  //==============================================================================
  inline byte08_t MapNounInfo( word32_t info )
  {
    return ( info & gfMultiple ? 10 : 0 ) + mapCaseInfo( info );
  }

  //==============================================================================
  // ������ ��������� ����� ��������� ������, ������� �������� ��������� ����
  // �� ������� � ������ ���������� ���� � ����� � ��������� 0..3
  //==============================================================================
  inline  byte08_t  mapVerbTant( word32_t info )
  {
    return (byte08_t)( ( info & gfRetForms ? 2 : 0 ) + ( info & gfMultiple ? 1 : 0 ) );
  }

  //==============================================================================
  // ������, ������������ ������ ����� � ���������� �� � �������� ����� 0..11
  // � ������ ��������� �� ������� �, ��������������, ������������
  //==============================================================================
  inline  byte08_t  mapPersonal( word32_t info )
  {
    return (byte08_t)(((info & gfVerbFace) >> 3) - 1) * 4 + mapVerbTant( info );
  }

  //==============================================================================
  // ������� ���� ���� ��������� ��������� � ��������� �� � �������� 0..63
  //==============================================================================
  inline  byte08_t  mapActvPart( word32_t info )
  {
    return (byte08_t)( info & gfRetForms ? 32 : 0 ) + mapGendInfo( info );
  }

  //==============================================================================
  // ������� ���� ��������� ��������� � ������������ � ��������� �� � ��������
  // ��������������� 0..97
  //==============================================================================
  inline  byte08_t  mapAllParts( word32_t info )
  {
    return (byte08_t)( (info & gfVerbForm) == vfVerbActive ? mapActvPart( info ) :
                   ( (info & gfVerbForm) == vfVerbPassiv ? 64 + mapGendInfo( info ) :
                     96 + ( info & gfRetForms ? 1 : 0 ) ) );
  }

  inline byte08_t MapVerbInfo( word32_t info )
  {
    switch ( info & gfVerbTime )
    {
    // ���������, ���� 0, ����� 2
      case vtInfinitiv:
        return ( info & gfRetForms ? 1 : 0 );
    // ���������, ���� 2, ����� 4
      case vtImperativ:
        return 2 + mapVerbTant( info );
    // ������� �����, ���� 6, ����� 12
      case vtFuture:
        return 6 + mapPersonal( info );
    // ��������� �����, ���� 18
      case vtPresent:
      // ������ �����, ���� 18, ����� 12
        if ( (info & gfVerbForm) == 0 ) return 18 + mapPersonal( info );
      // ��� ���������, ���� 30, ����� 98
          else return 30 + mapAllParts( info );
    // ��������� �����, ���� 128
      case vtPast:
      // ������ �����, ���� 128, ����� 8
        if ( (info & gfVerbForm) == 0 ) return 128 + ( info & gfRetForms ? 4 : 0 )
          + mapGenderOf( info );
      // ��� ���������, ���� 136, ����� 98
        else return 136 + mapAllParts( info );
      default:
        return 255;
    }
  }

  inline byte08_t MapWordInfo( byte08_t   wInfo,
                               word16_t   gInfo,
                               byte08_t   flags )
  {
    word32_t  dwinfo = (((word32_t)flags) << 16) | gInfo;
    return ( IsVerb( wInfo ) ? MapVerbInfo( dwinfo ) :
      ( IsAdjective( wInfo ) ? MapDefsInfo( dwinfo ) : MapNounInfo( dwinfo ) ) );
  }

  inline void     getCaseInfo( byte08_t   form,
                               word16_t&  info,
                               byte08_t&  flag )
  {
    flag = afAnimated | afNotAlive;
    if ( form >= 5 ) info |= (form - 1) << 12;
      else
    if ( form <= 2 ) info |= form << 12;
      else
    {
      info |= 3 << 12;  // ����������� �����
      if ( form == 3 ) flag = afNotAlive;
        else flag = afAnimated;
    }
  }

  inline bool getGendInfo( byte08_t  form,
                           word16_t& info,
                           byte08_t& flag )
  {
    flag = afAnimated | afNotAlive;
    if ( form >= 24 ) info |= gfMultiple;
      else info |= ((form / 8) + 1) << 9;
    if ( (form % 8) == 7 ) info |= gfShortOne;
      else getCaseInfo( form % 8, info, flag );
    return ( form < 32 );
  }

  inline word16_t getGenderOf( byte08_t form )
  {
    return ( form == 3 ? gfMultiple : (form + 1) << 9 );
  }

  inline word16_t getVerbTant( byte08_t form )
  {
    return ( form >= 2 ? gfRetForms : 0 ) | ( (form % 2) == 1 ? gfMultiple : 0 );
  }

  inline word16_t getPersonal( byte08_t form )
  {
    return ((form / 4 + 1) << 3) | getVerbTant( form % 4 );
  }

  inline  bool    getAllParts( byte08_t  form,
                               word16_t& info,
                               byte08_t& flag )
  {
    flag = afAnimated | afNotAlive;

    if ( form >= 96 ) info |= vfVerbDoing | ( (form % 2) == 1 ? gfRetForms : 0 );
      else
    if ( form >= 64 )
    {
      info |= vfVerbPassiv;
      return getGendInfo( form - 64, info, flag );
    }
      else
    {
      if ( form >= 32 ) info |= gfRetForms;
      info |= vfVerbActive;
      return getGendInfo( form % 32, info, flag );
    }
    return true;
  }

  inline  bool  GetDefsInfo( byte08_t   form,
                             word16_t&  info,
                             byte08_t&  flag )
  {
    if ( form <= 65 )
    {
      flag = afAnimated | afNotAlive;

      if ( form == 65 ) info = gfAdverb;
        else
      if ( form == 64 ) info = gfCompared;
        else
      {
        info = ( form >= 32 ? gfRetForms : 0 );
        return getGendInfo( form % 32, info, flag );
      }
      return true;
    }
    return false;
  }

  inline  bool  GetNounInfo( byte08_t  form,
                             word16_t& info,
                             byte08_t& flag )
  {
    if ( form >= 10 ) info = gfMultiple;
      else info = 0;
    getCaseInfo( form % 10, info, flag );
    return (form <= 8) || (form >= 10 && form <= 16);
  }

  inline  bool  GetVerbInfo( byte08_t   form,
                             word16_t&  info,
                             byte08_t&  flag )
  {
    flag = afAnimated | afNotAlive;
  // ���������
    if ( form < 2 ) info = vtInfinitiv | ( form == 1 ? gfRetForms : 0 );
      else
  // ���������
    if ( form < 6 ) info = vtImperativ | getVerbTant( form - 2 );
      else
  // ������� �����
    if ( form < 18 ) info = vtFuture | getPersonal( form - 6 );
      else
  // ��������� �����
    if ( form < 128 )
    {
      info = vtPresent;

    // ������ �����, ���� 18, ����� 12
      if ( form < 30 ) info |= getPersonal( form - 18 );
    // ��� ���������, ���� 30, ����� 98
        else return getAllParts( form - 30, info, flag );
    }
      else
  // ��������� �����
    {
      info = vtPast;

    // ��������� �������� ����
      if ( form >= 234 )
        return false;

    // ������ �����
      if ( form < 136 ) info |= ( form >= 132 ? gfRetForms : 0 ) | getGenderOf( form % 4 );
        else
    // ��� ���������
      return getAllParts( form - 136, info, flag );
    }
    return true;
  }

  inline bool GetWordInfo( byte08_t   wInfo,
                           byte08_t   nform,
                           word16_t&  gInfo,
                           byte08_t&  flags )
  {
  // on verbs, map formid to the verb form information
    if ( (wInfo & 0x3f) <= 6 )
      return GetVerbInfo( nform, gInfo, flags );

  // on real adjectives, map the complete definition information
    if ( (wInfo & 0x3f) == 25 || (wInfo & 0x3f) == 26 )
      return GetDefsInfo( nform, gInfo, flags );

  // check if has the gender
    if ( (wInfo & 0x3f) == 27 || (wInfo & 0x3f) == 28
      || (wInfo & 0x3f) == 34 || (wInfo & 0x3f) == 36
      || (wInfo & 0x3f) == 42 )
    {
      gInfo = 0;
      flags = 0;
      return getGendInfo( nform, gInfo, flags );
    }

  // else case only
    return GetNounInfo( nform, gInfo, flags );
  }

} // end namespace

# endif // __grammap_h__

