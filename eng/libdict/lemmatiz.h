# if !defined( _lemmatiz_h_ )
# define _lemmatiz_h_

# include "mlmadefs.h"

namespace __libmorpheng__
{
  #if !defined( lemmatize_errors_defined )
    #define lemmatize_errors_defined
    #define LEMMBUFF_FAILED -1
    #define LIDSBUFF_FAILED -2
    #define GRAMBUFF_FAILED -3
    #define WORDBUFF_FAILED -4
  #endif

  typedef struct tagLemmInfo
  {
    char*     lpDest;   // ��������� �� ������ ���������� ���� �����
    word16_t  ccDest;   // ���������� ��������, ��������� � lpDest
    word32_t* lpLids;   // ��������� �� ������ ����������� �������
    word16_t  clLids;   // ���������� ���������, ��������� � lpLids
    char*     lpInfo;   // ��������� �� ������ �������� ���������������� ����
    word16_t  cbInfo;   // ���������� ����, ��������� � lpInfo
    short     failed;   // ����� ���������� �� ������������ ���������
    word16_t  fBuilt;   // ���������� ����������� ���������� ����
  } SLemmInfo;

  typedef struct tagMakeInfo
  {
    char*         lpDest;   // ��������� �� ������ ����������� ���� �����
    word16_t      ccDest;   // ���������� ��������, ��������� � lpDest
    byte08_t      idForm;   // ������������� �����
    word32_t      wrdLID;   // ����������� ����� ��������������� �����
    short         failed;   // ������� ������������ ��������� �������
    word16_t      fBuilt;   // ���������� ����
  } SMakeInfo;

  void  actLemmatize( const byte08_t* lpStem,
                      const byte08_t* lpWord,
                      SGramInfo*      grInfo,
                      int             cgInfo,
                      SScanPage&      rfScan );
  void  actBuildForm( const byte08_t* lpStem,
                      const byte08_t* lpWord,
                      SGramInfo*      grInfo,
                      int             cgInfo,
                      SScanPage&      rfScan );
  void  actListForms( const byte08_t* lpStem,
                      const byte08_t* lpWord,
                      SGramInfo*      grInfo,
                      int             cgInfo,
                      SScanPage&      rfScan );

  void  actGetWdType( const byte08_t* lpStem,
                      const byte08_t* lpWord,
                      SGramInfo*      grInfo,
                      int             cgInfo,
                      SScanPage&      rfScan );
  void  actGetClass(  const byte08_t* lpStem,
                      const byte08_t* lpWord,
                      SGramInfo*      grInfo,
                      int             cgInfo,
                      SScanPage&      rfScan );
} // end __libmorpheng__ namespace

# endif // _lemmatiz_h_
