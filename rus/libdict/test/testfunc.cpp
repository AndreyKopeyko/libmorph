/******************************************************************************

    libmorphrus - dictiorary-based morphological analyser for Russian.
    Copyright (C) 1994-2016 Andrew Kovalenko aka Keva

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    Contacts:
      email: keva@meta.ua, keva@rambler.ru
      Skype: big_keva
      Phone: +7(495)648-4058, +7(926)513-2991

******************************************************************************/
# include "../../include/mlma1049.h"
# include <string.h>
# include <assert.h>

# include "../../../../mtCommon/microtime.h"
# include <stdio.h>

extern "C"    // C API
{
  const char* TestLemmatize( const char*  szform, unsigned  dwsets, int nitems, ... );
}

void  CheckTemplate( const char* stempl, int nchars, const char* answer )
{
  IMlmaMb*  pmorph;
  char      szhelp[0x100];
  int       nitems;

  mlmaruLoadMbAPI( &pmorph );

  assert( (nitems = pmorph->CheckHelp( szhelp, 0x100, stempl, (size_t)-1 )) == nchars );
  assert( memcmp( szhelp, answer, nchars ) == 0 );
}

int   main( int argc, char* argv[] )
{
  FILE*       lpfile;
  char*       buffer;
  int         length;
  char*       buftop;
  char*       bufend;
  int         cycles;
  int         nvalid;
  double      tstart;
  IMlmaMb*    pmorph;
  int         nforms;
  int         nlemma;
  SLemmInfoA  lemmas[0x10];
  char        aforms[0x100];
  SGramInfo   agrams[0x40];

// test C API
  TestLemmatize( "������������", 0, 1, 1913, "����������" );
  TestLemmatize( "�������", 0, 3, 45384, "�������", 16500, "���������", 136174, "�������" );

  mlmaruLoadCpAPI( &pmorph, "utf8" );
    pmorph->SetLoCase( strcpy( aforms, "Комсомольск-на-Амуре" ), (size_t)-1 );
    pmorph->SetUpCase( strcpy( aforms, "комсомольск-на-амуре" ), (size_t)-1 );

    pmorph->CheckWord( "Комсомольск-на-Амуре", (size_t)-1, 0 );
    pmorph->CheckWord( "комсомольск-на-амуре", (size_t)-1, sfIgnoreCapitals );

    pmorph->Lemmatize( "киев", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );

    pmorph->BuildForm( aforms, 0x100, 61577, 5 );

    pmorph->FindForms( aforms, 0x100, "вобрать", (size_t)-1, 2 );
    pmorph->CheckHelp( aforms, 0x100, "просто?", (size_t)-1 );
  pmorph->Detach();

  mlmaruLoadMbAPI( &pmorph );
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
    pmorph->CheckWord( "�������", (size_t)-1, 0 );

    pmorph->CheckWord( "�������", (size_t)-1, 0 );

    pmorph->CheckWord( "��������", (size_t)-1, 0 );
  pmorph->Detach();

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

  nlemma = pmorph->Lemmatize( "�������", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "������������", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );

  nlemma = pmorph->Lemmatize( ",", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );

  nlemma = pmorph->Lemmatize( "���", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "���", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "���", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );

  nlemma = pmorph->Lemmatize( "�������", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "�����", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "�����", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );
  nlemma = pmorph->Lemmatize( "�����", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );
  nlemma = pmorph->Lemmatize( "������������-��-�����", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, sfIgnoreCapitals );
  nlemma = pmorph->Lemmatize( "�����-���������", (size_t)-1, lemmas, 0x10, aforms, 0x100, agrams, 0x40, 0 );

  nforms = pmorph->FindForms( aforms, 0x100, "�������", (size_t)-1, 6 );
  nforms = pmorph->FindForms( aforms, 0x100, "�����", (size_t)-1, 0 );
  nforms = pmorph->FindForms( aforms, 0x100, "�����", (size_t)-1, 0 );
  nforms = pmorph->FindForms( aforms, 0x100, "�������", (size_t)-1, 2 );
  nforms = pmorph->FindForms( aforms, 0x100, "���-������", (size_t)-1, 2 );

  nforms = pmorph->BuildForm( aforms, sizeof(aforms), 17110, 6 );

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
          if ( (nlemma = pmorph->Lemmatize( buffer, (size_t)-1, lemmas, 0x20, NULL, 0, NULL, 0, 0 )) <= 0 )
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
