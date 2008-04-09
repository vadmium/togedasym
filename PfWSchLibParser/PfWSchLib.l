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

%{

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "PfWSchLibStruct.h"
#include "FlexBisonIface.h"
#include "PfWSchLib.y.h"

#ifdef _DEBUG
#include "dmalloc.h"
#endif

/* !!!!! All names redefined from yy to PfWSchLib */
#ifdef _PARSERDEBUG
FILE * flexerrfileout;
#undef stderr
#define stderr flexerrfileout
#endif

int NumLines=0;


#ifndef YY_DECL
#define YY_DECL int yylex ( YYSTYPE *pPfWSchLibllval )
#endif

%}

/* options */

%option never-interactive
%option noyywrap 
/* %option reentrant */

/* start conditions */

%s STRINGS

/* definitions */
/* basic */

DIGIT       [0-9]
CHARACTER   [a-z]
CAPITAL     [A-Z]

cr          \r
lf          \n
tab         \t
space       " "
minus       -
dot         "."

WS          [ \t]+
EOL         \r\n

number      {minus}?{DIGIT}+
float       {minus}?{DIGIT}+{dot}{DIGIT}+
string      ([^\t\r\n ]+)([^\r\n]*)
label       '[^\r\n']*'

%%

"\x50\x72\x6f\x74\x65\x6c for \x57\x69\x6e\x64\x6f\x77\x73 - Schematic Library Editor Ascii File Version 1.2 - 2.0" { return KWPTLIBINFO; }
"Organization"    { return KWORGANIZATION;      }
"End"             { return KWEND;               }
"[Font_Table]"    { return KWFONTTABLE;         }
"EndFont"         { return KWENDFONTTABLE;      }
"Library"         { return KWLIBRARY;           }
"EndLibrary"      { return KWENDOFLIBRARY;      }
"Component"       { return KWCOMPONENT;         }
"Part"            { return KWPART;              }
"EndNormalPart"   { return KWENDOFNORMALPART;   }
"EndDeMorganPart" { return KWENDOFDEMORGANPART; }
"EndIEEEPart"     { return KWENDOFIEEEPART;     }
"EndComponent"    { return KWENDCOMPONENT;      }
"Version 2.0"     { return KWVERSION20;         }

"Polygon"         { return PRPOLYGON;           }
"Pin"             { return PRPIN;               }
"Line"            { return PRLINE;              }
"RoundRectangle"  { return PRROUNDRECTANGLE;    }
"Rectangle"       { return PRRECTANGLE;         }
"Polyline"        { return PRPOLYLINE;          }
"Label"           { return PRLABEL;             }
"Symbol"          { return PRSYMBOL;            }
"EllipticalArc"   { return PRELLIPTICALARC;     }
"Ellipse"         { return PRELLIPSE;           }
"Pie"             { return PRPIE;               }
"Junction"        { return PRJUNCTION;          }
"Arc"             { return PRARC;               }
"Bezier"          { return PRBEZIER;            }
"Image"           { return PRIMAGE;             }

{label}    {
             pPfWSchLibllval->chval=(char*)malloc(strlen(PfWSchLibtext)-1);
             memcpy(pPfWSchLibllval->chval,PfWSchLibtext+1,strlen(PfWSchLibtext)-1);
             pPfWSchLibllval->chval[strlen(PfWSchLibtext)-2]=0;
             return IDLABEL;
           }

<STRINGS>{string} {
             pPfWSchLibllval->chval=(char*)malloc(strlen(PfWSchLibtext)+1);
             strcpy(pPfWSchLibllval->chval,PfWSchLibtext);
             return IDSTRING;
           }

{float}    {
             pPfWSchLibllval->flval=atof(PfWSchLibtext);
             return IDFLOAT;
           }

{number}   {
             pPfWSchLibllval->ival=atoi(PfWSchLibtext);
             return IDNUMBER;
           }

{EOL}      {
             NumLines++;
             return EOL;
           }

{WS}       { }

%%
/* .          { ECHO; } */



void LexEnableStrings(void)
{
  BEGIN(STRINGS);
}

void LexDisableStrings(void)
{
  BEGIN(INITIAL);
}

int ParsePfWSchLib(char *InFileName,PFWSCHLIBFILE ** pPfWSchLib)
{
  int nErrCode;
  char TxtBuf[48];
  PfWSchLibin=fopen(InFileName,"rb");
  if(PfWSchLibin==NULL)
  {
    sprintf(TxtBuf,"Error open file %s",InFileName);
    LibraryNotifyCallback(1,TxtBuf);
    return 1;
  }
  NumLines=0;
#ifdef _PARSERDEBUG
  flexerrfileout=fopen("flexdebug.out","wt");
  PfWSchLibout=fopen("flexout.out","wt");
  InitBisonForW32("bisondebug.out");
  InitBisonForDebug();
#endif
  nErrCode=PfWSchLibparse((void**)pPfWSchLib);
  yy_flush_buffer(YY_CURRENT_BUFFER);
  yy_start=0;
  yy_init=0;
  yy_delete_buffer(YY_CURRENT_BUFFER);
/*  free(YY_CURRENT_BUFFER); */
  free(yy_buffer_stack); 
/*  YY_CURRENT_BUFFER=0; */
/*  YY_CURRENT_BUFFER_LVALUE=0; */
  yy_buffer_stack=0;
#ifdef _PARSERDEBUG
  CloseBisonForW32();
  fclose(flexerrfileout);
  fclose(PfWSchLibout);
#endif
  fclose(PfWSchLibin);
  return nErrCode;
}
