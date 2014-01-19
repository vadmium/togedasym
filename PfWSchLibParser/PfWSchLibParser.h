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
// PfWSchLibLoadFileEx - �adowanie pliku PfWSchLib
//
// parametry:
//  nazwa pliku
//
// zwracana warto��: 
//  0 - plik zosta� odczytany bez b��d�w
// -1 - b��d w odczycie PfWSchLib
// 
int PfWSchLibLoadFileEx(const char *PfWSchLibFileName,PFWSCHLIBFILE **pLocPfWSchLib);

/////////////////////////////////////////////////////////////////////////////
// PfWSchLibFreeAllEx - zwalnia pami�� zaalokowan� na PfWSchLib
//
// parametry:
//  wska�nik do danych  
//
// zwracana warto��: 
//  brak
// 
void PfWSchLibFreeAllEx(PFWSCHLIBFILE **pLocPfWSchLib);

/////////////////////////////////////////////////////////////////////////////
// PfWSchLibSetCallback - ustawienie wska�nika na funkcj� odbieraj�c� 
// informacje o wydarzeniach i b��dach zachodz�cych w module PfWSchLibLoad.dll  
//
// parametry:
// pFn - wska�nik funkcji wywo�ywanej przez bibliotek� PfWSchLibLoad.dll
// w momencie wysy�ania powiadomienia.
// Funkcja powinna by� zadeklarowana w nast�puj�cy spos�b:
// int NotifyCallback(int nErrCode,char* message);
//   paramrtry:
//   nErrCode - kod b��du zwracany przez PfWSchLibLoad.dll
//   obecnie zdefiniowane kody b��d�w to: 
//    0 - powiadomienie jest informacj� o przebiegu dzia�ania funkcji
//    1 - powiadomienie jest informacj� o b��dzie, bie��ca funkcja przerwa�a dzia�anie
//   message - waka�nik na �a�cuch znak�w zawieraj�cy tre�� powiadomienia.
//    W celu p�niejszego u�ycia �a�nuch wskazywany powinien zosta� skopiowany, 
//    po wyj�ciu z funkcji wska�nik nie jest ju� aktualny.
//   warto�� zwracana przez fukncj� powinna wynosi� 1.
//
// zwracana warto��:
//  funkcja nie zwraca warto�ci
//
void PfWSchLibSetCallbackFunction(int (*pFn)(int,char*));


#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////

#endif //__PFWSCHLIBPARSER_INCLUDE_FILE_L__
