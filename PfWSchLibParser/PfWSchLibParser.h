/*
    PfWSchLibParser - Parser for library files from P*tel, exported as ascii files
    Copyright (C) 2008  L.Kaczor

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

////// PfWSchLibLoad.h : exports from library PfWSchLibParser
////
//
#if !defined(__PFWSCHLIBPARSER_INCLUDE_FILE_L__)
#define __PFWSCHLIBPARSER_INCLUDE_FILE_L__

#ifdef __cplusplus
extern "C" 
{
#endif

/////////////////////////////////////////////////////////////////////////////
// PfWSchLibLoadFileEx - ³adowanie pliku PfWSchLib
//
// parametry:
//  nazwa pliku
//
// zwracana wartoœæ: 
//  0 - plik zosta³ odczytany bez b³êdów
// -1 - b³¹d w odczycie PfWSchLib
// 
int PfWSchLibLoadFileEx(const char *PfWSchLibFileName,PFWSCHLIBFILE **pLocPfWSchLib);

/////////////////////////////////////////////////////////////////////////////
// PfWSchLibFreeAllEx - zwalnia pamiêæ zaalokowan¹ na PfWSchLib
//
// parametry:
//  wskaŸnik do danych  
//
// zwracana wartoœæ: 
//  brak
// 
void PfWSchLibFreeAllEx(PFWSCHLIBFILE **pLocPfWSchLib);

/////////////////////////////////////////////////////////////////////////////
// PfWSchLibSetCallback - ustawienie wskaŸnika na funkcjê odbieraj¹c¹ 
// informacje o wydarzeniach i b³êdach zachodz¹cych w module PfWSchLibLoad.dll  
//
// parametry:
// pFn - wska¿nik funkcji wywo³ywanej przez bibliotekê PfWSchLibLoad.dll
// w momencie wysy³ania powiadomienia.
// Funkcja powinna byæ zadeklarowana w nastêpuj¹cy sposób:
// int NotifyCallback(int nErrCode,char* message);
//   paramrtry:
//   nErrCode - kod b³êdu zwracany przez PfWSchLibLoad.dll
//   obecnie zdefiniowane kody b³êdów to: 
//    0 - powiadomienie jest informacj¹ o przebiegu dzia³ania funkcji
//    1 - powiadomienie jest informacj¹ o b³êdzie, bie¿¹ca funkcja przerwa³a dzia³anie
//   message - wakaŸnik na ³añcuch znaków zawieraj¹cy treœæ powiadomienia.
//    W celu póŸniejszego u¿ycia ³aænuch wskazywany powinien zostaæ skopiowany, 
//    po wyjœciu z funkcji wskaŸnik nie jest ju¿ aktualny.
//   wartoœæ zwracana przez fukncjê powinna wynosiæ 1.
//
// zwracana wartoœæ:
//  funkcja nie zwraca wartoœci
//
void PfWSchLibSetCallbackFunction(int (*pFn)(int,char*));


#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////

#endif //__PFWSCHLIBPARSER_INCLUDE_FILE_L__
