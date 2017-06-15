# include "grammap.h"
# include <mtc/stringmap.h>

using namespace mtc;

struct  graminfo
{
  unsigned  grmask;
  unsigned  grinfo;
  unsigned  mflags;
  unsigned  bflags;
};

static  stringmap<graminfo> gramMapper;

#define add_key( szkey, gmask, ginfo, fmask, flags )                                        \
  if ( gramMapper.Insert( szkey, { (unsigned)(gmask), (unsigned)(ginfo),                    \
                                   (unsigned)(fmask), (unsigned)(flags) } ) == nullptr )    \
    return ENOMEM;

bool  MapInfo( const char*  pszkey, unsigned& grinfo, unsigned& bflags )
{
  while ( *pszkey != '\0' )
  {
    const char*     pszorg;
    const graminfo* pginfo;
    
    for ( pszorg = pszkey; *pszkey != '\0' && *pszkey != '|'; ++pszkey )
      (void)NULL;
    assert( pszkey > pszorg && (*pszkey == '\0' || *pszkey == '|') );

    if ( (pginfo = gramMapper.Search( pszorg, pszkey - pszorg )) == NULL )
      return false;

    grinfo = (grinfo & pginfo->grmask) | pginfo->grinfo;
    bflags = (bflags & pginfo->mflags) | pginfo->bflags;

    if ( *pszkey == '|' )
      ++pszkey;
  }
  return true;
}

int   InitRus()
{
  gramMapper.DelAll();

  add_key( "�",         ~gfFormMask,  3 << 12, -1, afAnimated + afNotAlive )
  add_key( "��",        ~gfFormMask,  3 << 12, ~(afAnimated + afNotAlive), afNotAlive )
  add_key( "��",        ~gfFormMask,  3 << 12,  ~(afAnimated + afNotAlive),  afAnimated )
  add_key( "�",         ~gfFormMask,  2 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",         ~gfFormMask,  0,        -1,  afAnimated + afNotAlive )
  add_key( "�",         ~gfFormMask,  5 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�2",        ~gfFormMask,  7 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",         ~gfFormMask,  1 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�2",        ~gfFormMask,  6 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",         ~gfFormMask,  4 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "����� �",   0,            vtFuture,     -1, afAnimated + afNotAlive )
  add_key( "����� �",   0,            vtPresent,    -1, afAnimated + afNotAlive )
  add_key( "����� �",   0,            vtPast,       -1, afAnimated + afNotAlive )
  add_key( "��",        -1,           gfRetForms,   -1, 0 )
  add_key( "�����",     gfVerbTime,   vfVerbDoing,  -1, afAnimated + afNotAlive )
  add_key( "������",    gfVerbTime,   vfVerbActive, -1, afAnimated + afNotAlive )
  add_key( "�������",   -1,           0,            -1, afHardForm )
  add_key( "���������", 0,            vtInfinitiv,  0,            afAnimated + afNotAlive )
  add_key( "��",        gfVerbTime+gfVerbForm+gfGendMask+gfMultiple,   gfShortOne, -1, afAnimated + afNotAlive )
  add_key( "���� 1",    gfVerbTime,   vbFirstFace, -1, afAnimated + afNotAlive )
  add_key( "���� 2",    gfVerbTime,   vbSecondFace, -1, afAnimated + afNotAlive )
  add_key( "���� 3",    gfVerbTime,   vbThirdFace, -1, afAnimated + afNotAlive )
  add_key( "��",        0,            gfAdverb,     -1, afAnimated + afNotAlive )
  add_key( "�����",     0,            vtImperativ,  -1, afAnimated + afNotAlive )
  add_key( "����",      -1,           0,            -1, 0 )                      
  add_key( "���",       gfVerbTime|gfVerbForm|gfMultiple, 0,      -1, 0 )
  add_key( "��� �",     gfVerbTime|gfVerbForm, 2 << 9, -1, 0 )
  add_key( "��� �",     gfVerbTime|gfVerbForm, 1 << 9, -1, 0 )
  add_key( "��� �",     gfVerbTime|gfVerbForm, 3 << 9, -1, 0 )
  add_key( "����",      -1,           0,      -1, afJoiningC  )
  add_key( "��",        0,            gfCompared,   -1, afAnimated + afNotAlive )
  add_key( "�����",     gfVerbTime,   vfVerbPassiv, -1, afAnimated + afNotAlive )
  add_key( "�����",     gfVerbTime|gfVerbForm, 0, -1, afAnimated + afNotAlive )
  add_key( "����� �",   gfVerbTime|gfVerbForm, 0, -1, afAnimated + afNotAlive )
  add_key( "����� �",   gfVerbTime|gfVerbForm, gfMultiple, -1, afAnimated + afNotAlive )
  return 0;
}

int   InitUkr()
{
  gramMapper.DelAll();

  add_key( "act",        gfVerbTime,   vfVerbActive, -1, afAnimated + afNotAlive )
  add_key( "face 1",     gfVerbTime,   vbFirstFace, -1, afAnimated + afNotAlive )
  add_key( "face 2",     gfVerbTime,   vbSecondFace, -1, afAnimated + afNotAlive )
  add_key( "face 3",     gfVerbTime,   vbThirdFace, -1, afAnimated + afNotAlive )
  add_key( "fem",        gfVerbTime|gfVerbForm, 2 << 9, -1, 0 )
  add_key( "fut",        0,            vtFuture,     -1, afAnimated + afNotAlive )
  add_key( "ger",        gfVerbTime,   vfVerbDoing,  -1, afAnimated + afNotAlive )
  add_key( "imp",        0,            vtImperativ,  -1, afAnimated + afNotAlive )
  add_key( "inf",        0,            vtInfinitiv,  0,            afAnimated + afNotAlive )
  add_key( "msc",        gfVerbTime|gfVerbForm, 1 << 9, -1, 0 )
  add_key( "nwt",        gfVerbTime|gfVerbForm, 3 << 9, -1, 0 )
  add_key( "prs",        0,            vtPresent,    -1, afAnimated + afNotAlive )
  add_key( "pst",        0,            vtPast,       -1, afAnimated + afNotAlive )
  add_key( "psv",        gfVerbTime,   vfVerbPassiv, -1, afAnimated + afNotAlive )
  add_key( "sing",       gfVerbTime|gfVerbForm, 0, -1, afAnimated + afNotAlive )
  add_key( "plur",       gfVerbTime|gfVerbForm, gfMultiple, -1, afAnimated + afNotAlive )

  add_key( "�",          ~gfFormMask,  3 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "��",         ~gfFormMask,  3 << 12,  ~(afAnimated + afNotAlive),  afNotAlive )
  add_key( "��",         ~gfFormMask,  3 << 12,  ~(afAnimated + afNotAlive),  afAnimated )
  add_key( "�",          ~gfFormMask,  2 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",          ~gfFormMask,  6 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",          ~gfFormMask,  0,        -1,  afAnimated + afNotAlive )
  add_key( "�",          ~gfFormMask,  5 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",          ~gfFormMask,  1 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "�",          ~gfFormMask,  4 << 12,  -1,  afAnimated + afNotAlive )
  add_key( "��",         -1, gfRetForms,   -1, 0 )
  return 0;
}
