# include "../include/mlma1049.h"
# include "lemmatiz.h"
# include <string.h>
# include <assert.h>

using namespace LIBMORPH_NAMESPACE;

# include "../../../mtCommon/microtime.h"
# include <stdio.h>
//# include <tools/sweets.h>

void  CheckTemplate( const char* stempl, int nchars, const char* answer )
{
  IMlmaMb*  pmorph;
  char      szhelp[0x100];
  int       nitems;

  mlmaruLoadMbAPI( &pmorph );

  assert( (nitems = pmorph->CheckHelp( szhelp, 0x100, stempl, (unsigned)-1 )) == nchars );
  assert( memcmp( szhelp, answer, nchars ) == 0 );
}

int   main( int argc, char* argv[] )
{
  FILE*     lpfile;
  char*     buffer;
  int       length;
  char*     buftop;
  char*     bufend;
  int       cycles;
  int       nvalid;
  double    tstart;
  IMlmaMb*  pmorph;
  int       nlexid;
  int       idform;
  int       nforms;
  int       nlemma;

  mlmaruLoadMbAPI( &pmorph );
/*
  pmorph->CheckWord( "�", 1, 0 );
  pmorph->CheckWord( "��", 2, 0 );
  pmorph->CheckWord( "��", 2, 0 );
  pmorph->CheckWord( "��", 2, 0 );
  pmorph->CheckWord( "����", 4, 0 );
  pmorph->CheckWord( "������", 6, 0 );
  pmorph->CheckWord( "������", 6, 0 );
  pmorph->CheckWord( "������", 6, sfIgnoreCapitals );
  pmorph->CheckWord( "���", 3, 0 );
  pmorph->CheckWord( "���", 3, 0 );
  pmorph->CheckWord( "���", 3, 0 );
  pmorph->CheckWord( "���", 3, sfIgnoreCapitals );
  pmorph->CheckWord( "�������", (unsigned)-1, 0 );

  pmorph->CheckWord( "�������", (unsigned)-1, 0 );

  pmorph->CheckWord( "��������", (unsigned)-1, 0 );
*/
  SLemmInfoA  lemmas[0x10];
  char        aforms[0x100];
  SGramInfo   agrams[0x40];

  CheckTemplate( "?�����������", 1, "�" );
  CheckTemplate( "�?����������", 1, "�" );
  CheckTemplate( "��?���������", 1, "�" );
  CheckTemplate( "���?��������", 1, "�" );
  CheckTemplate( "����?�������", 1, "�" );
  CheckTemplate( "�����?������", 1, "�" );
  CheckTemplate( "������?�����", 1, "�" );
  CheckTemplate( "�������?����", 2, "��" );
  CheckTemplate( "��������?���", 1, "�" );
  CheckTemplate( "���������?��", 1, "�" );
  CheckTemplate( "����������?�", 1, "�" );
  CheckTemplate( "�����������?", 1, "�" );
  CheckTemplate( "������������?", 1, "\0" );

  pmorph->Lemmatize( ",", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );

  pmorph->Lemmatize( "���", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  pmorph->Lemmatize( "���", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  pmorph->Lemmatize( "���", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );

  pmorph->Lemmatize( "�������", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  pmorph->Lemmatize( "�����", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  pmorph->Lemmatize( "�����", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );
  pmorph->Lemmatize( "�����", (unsigned)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );

  pmorph->FindForms( aforms, 0x100, "�������", (unsigned)-1, 6 );
  pmorph->FindForms( aforms, 0x100, "�����", (unsigned)-1, 0 );
  pmorph->FindForms( aforms, 0x100, "�����", (unsigned)-1, 0 );
  pmorph->FindForms( aforms, 0x100, "�������", (unsigned)-1, 2 );

  pmorph->BuildForm( aforms, sizeof(aforms), 17110, 6 );

  pmorph->BuildForm( aforms, sizeof(aforms), 128, (unsigned char)-1 );
/*
  int lastid = -1;

  for ( nlexid = 0; nlexid < 256000; ++nlexid )
    for ( idform = -1; idform < 255; ++idform )
      if ( (nforms = pmorph->BuildForm( buffer = aforms, 0x100, nlexid, (unsigned char)idform )) > 0 )
      {
        if ( lastid != nlexid )
          fprintf( stderr, "%d\n", lastid = nlexid );

        for ( ; nforms-- > 0; buffer += strlen(buffer) + 1 )
          if ( (nlemma = pmorph->Lemmatize( buffer, (unsigned)-1, lemmas, 0x20, NULL, 0, NULL, 0, 0 )) <= 0 )
          {
            printf( "Could not lemmatize \'%s\'!\n", buffer );
          }
            else
          {
            while ( nlemma > 0 && lemmas[nlemma - 1].nlexid != nlexid )
              --nlemma;
            if ( nlemma == 0 )
              printf( "string \'%s\' is not mapped to nlexid=%d!\n", buffer, nlexid );
          }
      }

  return 0;
*/
  if ( (lpfile = fopen( "example.txt", "rb" )) != NULL )  fseek( lpfile, 0, SEEK_END );
    else  return -1;
  if ( (buffer = (char*)malloc( length = ftell( lpfile ) )) != NULL )  fseek( lpfile, 0, SEEK_SET );
    else  return -2;
  if ( fread( buffer, 1, length, lpfile ) == length ) fclose( lpfile );
    else  return -3;

  tstart = millitime();
  cycles = nvalid = 0;

  for ( int i = 0; i < 2; ++i )
  for ( bufend = (buftop = buffer) + length; buftop < bufend; )
  {
    char* wrdtop;

    while ( buftop < bufend && (unsigned char)*buftop <= 0x20 )
      ++buftop;
    if ( (unsigned char)*buftop < (unsigned char)'�' )
    {
      ++buftop;
      continue;
    }

    for ( wrdtop = buftop; buftop < bufend && (unsigned char)*buftop >= (unsigned char)'�'; ++buftop )
      (void)NULL;
/*
    {
      libmorph::file lpfile = fopen( "qq.txt", "at" );
        for ( const char* p = wrdtop; p < buftop; ++p ) fputc( *p, lpfile );
      fprintf( lpfile, "\n" );
    }

    if ( !(buftop - wrdtop == 6 && memcmp( wrdtop, "������", 6 ) == 0) )
      continue;
*/
    if ( pmorph->Lemmatize( (const char*)wrdtop, buftop - wrdtop, lemmas, 0x20, NULL, 0x100, agrams, 0x40, sfIgnoreCapitals ) > 0 )
      ++nvalid;
    else
    {
      while ( wrdtop < buftop ) fputc( *wrdtop++, stdout );
        fprintf( stdout, "\n" );
    }
    ++cycles;
  }

  printf( "%d WPS\n", (unsigned)(cycles / (millitime() - tstart)) );

	return 0;
}
