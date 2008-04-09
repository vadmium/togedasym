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

// BisonParser.h: Global variables and definitions for parser
//
//////////////////////////////////////////////////////////////////////

#if !defined  __PFWSCHLIB_BISON_PARSER_H_INCLUDE_FILE___
#define __PFWSCHLIB_BISON_PARSER_H_INCLUDE_FILE___

typedef union
{
  int ival;
  char *chval;
  double flval;
  LIBRARY_V1    * LIBRARY_V1type;
  FONTTABLE     * FONTTABLEtype;
  FONTS         * FONTStype;
  FONT          * FONTtype;
  COMPONENTS    * COMPONENTStype;
  COMPONENT     * COMPONENTtype;
  PARTS         * PARTStype;
  PART          * PARTtype;
  PRIMITIVES    * PRIMITIVEStype;
  PRIMITIVE     * PRIMITIVEtype;
  POINTS        * POINTStype;
  POINT         * POINTtype;

  LIBRARY_V2    * LIBRARY_V2type;
  COMPONENTS_V2 * COMPONENTS_V2type;
  COMPONENT_V2  * COMPONENT_V2type;
  
} YYSTYPE;

#define YYSTYPE_IS_DECLARED 1

extern int NumLines;

int LibraryNotifyCallback(int nErrCode,char* message);

int PfWSchLibwarning(const char *s);
int PfWSchLiberror(const char* s);
int PfWSchLiblex( YYSTYPE *pPfWSchLibllval );

int PfWSchLibparse(void **pPfWSchLib);

int ParsePfWSchLib(char *InFileName,PFWSCHLIBFILE ** pPfWSchLib);
void InitBisonForDebug(void);
int InitBisonForW32(char *DbgFileName);
int CloseBisonForW32(void);
void LexEnableStrings(void);
void LexDisableStrings(void);

#endif // __PFWSCHLIB_BISON_PARSER_H_INCLUDE_FILE___E___
