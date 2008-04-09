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
#include <stdio.h> 

#include "PfWSchLibStruct.h"
#include "FlexBisonIface.h"

#ifdef _DEBUG
#include "dmalloc.h"
#endif

#undef  ZeroMemory
#define ZeroMemory(p,n) memset((p),(0),(n))

#ifdef _PARSERDEBUG
FILE * bisonerrfileout;
#undef stderr
#define stderr bisonerrfileout
#endif

#ifndef YYPARSE_PARAM 
#define YYPARSE_PARAM *pPfWSchLib
#endif

#define pPfWSchLibCast ((PFWSCHLIBFILE*)*pPfWSchLib)

#ifdef _DEBUG
#undef YYERROR_VERBOSE
#define YYERROR_VERBOSE 1
#endif

extern char * PfWSchLibtext;

%}

/* basic */
%token EOL

/* keywords */
%token KWPTLIBINFO
%token KWORGANIZATION
%token KWEND
%token KWFONTTABLE
%token KWENDFONTTABLE

%token KWLIBRARY
%token KWENDOFLIBRARY
%token KWCOMPONENT
%token KWPART
%token KWENDCOMPONENT
%token KWENDOFNORMALPART
%token KWENDOFDEMORGANPART
%token KWENDOFIEEEPART
%token KWVERSION20

/* elements */
%token PRPOLYGON
%token PRPIN
%token PRLINE
%token PRROUNDRECTANGLE
%token PRRECTANGLE
%token PRPOLYLINE
%token PRLABEL
%token PRSYMBOL
%token PRELLIPTICALARC
%token PRELLIPSE
%token PRPIE
%token PRJUNCTION
%token PRARC
%token PRBEZIER
%token PRIMAGE
/* operators */

/* non keywords */
%token <chval>IDSTRING
%token <ival>IDNUMBER
%token <chval>IDLABEL
%token <flval>IDFLOAT
/* terminal token types */
/* token types */

/* non terminal token types */

%type <chval>optstring
%type <chval>optname2

%type <LIBRARY_V1type>library_v1
%type <FONTTABLEtype>fonttable
%type <FONTStype>optfontlines
%type <FONTStype>fontlines
%type <FONTtype>fontline
%type <COMPONENTStype>components
%type <COMPONENTtype>component
%type <PARTStype>parts
%type <PARTtype>partgroup
%type <PRIMITIVEStype>normalpart
%type <PRIMITIVEStype>demorganpart
%type <PRIMITIVEStype>ieeepart
%type <PRIMITIVEStype>optprimitives
%type <PRIMITIVEStype>primitives
%type <PRIMITIVEtype>primitive
%type <POINTStype>optpoints
%type <POINTStype>points
%type <POINTtype>point
%type <LIBRARY_V2type>library_v2
%type <COMPONENTS_V2type>optcomponents_v2
%type <COMPONENTS_V2type>components_v2
%type <COMPONENT_V2type>component_v2

/* bison options */

%pure_parser

%%

library: header
         library_v1 { pPfWSchLibCast->pLibV1=$2;}
         library_v2 { pPfWSchLibCast->pLibV2=$4;}
;
/* library header */
header: KWPTLIBINFO EOL
        IDNUMBER EOL 
        KWORGANIZATION EOL { LexEnableStrings();  }
        optstring EOL      { *pPfWSchLib=(PFWSCHLIBFILE*)malloc(sizeof(PFWSCHLIBFILE));
                             ZeroMemory(*pPfWSchLib,sizeof(PFWSCHLIBFILE));
                             pPfWSchLibCast->LibraryName=$8;
                             LexDisableStrings(); }
        EOL EOL EOL EOL EOL EOL EOL
        IDNUMBER IDNUMBER EOL
        KWEND EOL
        fonttable { pPfWSchLibCast->pFontTable=$23; }
;
fonttable: KWFONTTABLE EOL
           {
            $<FONTTABLEtype>$=(FONTTABLE*)malloc(sizeof(FONTTABLE));
            ZeroMemory($<FONTTABLEtype>$,sizeof(FONTTABLE));
           }
           IDNUMBER EOL
           optfontlines
           KWENDFONTTABLE EOL
           {
             $$=$<FONTTABLEtype>3;
             $$->NumFonts=$4;
             $$->pFonts=$6;
           }
;

optfontlines: /*empty*/ { $$=NULL; }
           | fontlines  { $$=$1;   }
;
fontlines: fontline
           {
             $$=(FONTS*)malloc(sizeof(FONTS));
             ZeroMemory($$,sizeof(FONTS));
             $$->pFont=(FONT**)realloc($$->pFont,(($$->RealNumFonts)+1)*sizeof(FONT*));
             $$->pFont[$$->RealNumFonts]=$1;
             $$->RealNumFonts++;
           }
         | fontlines fontline
           {
             $1->pFont=(FONT**)realloc($1->pFont,(($1->RealNumFonts)+1)*sizeof(FONT*));
             $1->pFont[$1->RealNumFonts]=$2;
             $1->RealNumFonts++;
             $$=$1
           }
;

fontline: IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER 
          {
            LexEnableStrings(); 
          }
          IDSTRING EOL
          {
            LexDisableStrings();
            $$=(FONT*)malloc(sizeof(FONT));
            ZeroMemory($$,sizeof(FONT));
            $$->Size=$1;
            $$->Name=$8;
          }
;
/* end of library header */

/* library_v1 */
library_v1: KWLIBRARY IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER
                      IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER
                      IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
                      IDNUMBER EOL
                      {
                        $<LIBRARY_V1type>$=(LIBRARY_V1*)malloc(sizeof(LIBRARY_V1));
                        ZeroMemory($<LIBRARY_V1type>$,sizeof(LIBRARY_V1));
                        $<LIBRARY_V1type>$->NumComponents=$18;
                      }
                      components
                      KWENDOFLIBRARY EOL
                      {
                        $$=$<LIBRARY_V1type>20;
                        $$->pComps=$21;
                      }
;
components: component
            {
              $$=(COMPONENTS*)malloc(sizeof(COMPONENTS));
              ZeroMemory($$,sizeof(COMPONENTS));
              $$->pComp=(COMPONENT**)realloc($$->pComp,(($$->RealNumComp)+1)*sizeof(COMPONENT*));
              $$->pComp[$$->RealNumComp]=$1;
              $$->RealNumComp++;
            }
          | components component
            {
              $1->pComp=(COMPONENT**)realloc($1->pComp,(($1->RealNumComp)+1)*sizeof(COMPONENT*));
              $1->pComp[$1->RealNumComp]=$2;
              $1->RealNumComp++;
              $$=$1;
            }
;
component: KWCOMPONENT EOL
           IDNUMBER EOL
           IDNUMBER EOL
           EOL           { LexEnableStrings();  }
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optstring EOL
           optname2
           parts
           KWENDCOMPONENT EOL
           {
             $$=(COMPONENT*)malloc(sizeof(COMPONENT));
             ZeroMemory($$,sizeof(COMPONENT));
             $$->NumParts=$3;
             $$->NumNames=$5;
             $$->Description=$9;
             $$->Footprint_1=$11;
             $$->Footprint_2=$13;
             $$->Footprint_3=$15;
             $$->Footprint_4=$17;
             $$->Field_1=$19;
             $$->Field_2=$21;
             $$->Field_3=$23;
             $$->Field_4=$25;
             $$->Field_5=$27;
             $$->Field_6=$29;
             $$->Field_7=$31;
             $$->Field_8=$33;
             $$->Default_Designator=$35; 
             $$->Sheet_Part_Filename=$37;
             $$->ComponentName1=$39;
             $$->ComponentName2=$41;
             $$->pParts=$42;
           }
;
optname2: /*empty*/ { LexDisableStrings(); $$=strdup(""); }
        | IDSTRING  { LexDisableStrings(); } EOL { $$=$1; }
;
parts: partgroup
       {
         $$=(PARTS*)malloc(sizeof(PARTS));
         ZeroMemory($$,sizeof(PARTS));
         $$->pPartgrp=(PART**)realloc($$->pPartgrp,(($$->RealNumParts)+1)*sizeof(PART*));
         $$->pPartgrp[$$->RealNumParts]=$1;
         $$->RealNumParts++;
       }
     | parts partgroup
       {
         $1->pPartgrp=(PART**)realloc($1->pPartgrp,(($1->RealNumParts)+1)*sizeof(PART*));
         $1->pPartgrp[$1->RealNumParts]=$2;
         $1->RealNumParts++;
         $$=$1;
       }
;
partgroup: KWPART EOL
           IDNUMBER IDNUMBER EOL
           normalpart
           demorganpart
           ieeepart
           {
             $$=(PART*)malloc(sizeof(PART));
             ZeroMemory($$,sizeof(PART));
             $$->pNormal=$6;
             $$->pDeMorgan=$7;
             $$->pIEEE=$8;
           }
;
normalpart: optprimitives
            KWENDOFNORMALPART EOL
            {
              $$=$1;
            }
;
demorganpart: KWENDOFDEMORGANPART EOL { $$=NULL; }
;
ieeepart: optprimitives
          KWENDOFIEEEPART EOL
          {
            $$=$1;
          }
;
optprimitives: /*empty*/   { $$=NULL; }
             | primitives  { $$=$1;   }
;
primitives: primitive
            {
              $$=(PRIMITIVES*)malloc(sizeof(PRIMITIVES));
              ZeroMemory($$,sizeof(PRIMITIVES));
              $$->pPr=(PRIMITIVE**)realloc($$->pPr,(($$->NumPrimitives)+1)*sizeof(PRIMITIVE*));
              $$->pPr[$$->NumPrimitives]=$1;
              $$->NumPrimitives++;
            }
          | primitives primitive
            {
              $1->pPr=(PRIMITIVE**)realloc($1->pPr,(($1->NumPrimitives)+1)*sizeof(PRIMITIVE*));
              $1->pPr[$1->NumPrimitives]=$2;
              $1->NumPrimitives++;
              $$=$1;
            }
;
primitive: PRRECTANGLE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Rectangle;
             $$->Selected=$9;
             $$->Filled=$10;
             $$->LineWidthOrRotation=$6;
             $$->BorderColorOrColor=$7;
             $$->FillColor=$8;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->X2orPinLength=$4;
             $$->Y2orPinAttributes=$5;
           }
         | PRELLIPSE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE)); 
             $$->Type=Ellipse;
             $$->Selected=$10;
             $$->Filled=$9;
             $$->LineWidthOrRotation=$6;
             $$->BorderColorOrColor=$7;
             $$->FillColor=$8;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->RadiusXorNrPoints=$4;
             $$->RadiusYorLineType=$5;
           }
         | PRARC IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDFLOAT IDFLOAT IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Arc;
             $$->Selected=$9;
             $$->LineWidthOrRotation=$5;
             $$->BorderColorOrColor=$8;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->RadiusXorNrPoints=$4;
             $$->StartAngle=$6;
             $$->EndAngle=$7;
           }
         | PRPIE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDFLOAT IDFLOAT IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Pie;
             $$->Selected=$11;
             $$->Filled=$10;
             $$->LineWidthOrRotation=$5;
             $$->BorderColorOrColor=$8;
             $$->FillColor=$9;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->RadiusXorNrPoints=$4;
             $$->StartAngle=$6;
             $$->EndAngle=$7;
           }
         | PRELLIPTICALARC IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDFLOAT IDFLOAT IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=EllipticalArc;
             $$->Selected=$10;
             $$->LineWidthOrRotation=$6;
             $$->BorderColorOrColor=$9;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->RadiusXorNrPoints=$4;
             $$->RadiusYorLineType=$5;
             $$->StartAngle=$7;
             $$->EndAngle=$8;
           }
         | PRROUNDRECTANGLE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=RoundRectangle;
             $$->Selected=$11;
             $$->Filled=$12;
             $$->LineWidthOrRotation=$8;
             $$->BorderColorOrColor=$9;
             $$->FillColor=$10;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->X2orPinLength=$4;
             $$->Y2orPinAttributes=$5;
             $$->RadiusXorNrPoints=$6;
             $$->RadiusYorLineType=$7;
           }
         | PRLABEL IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDLABEL EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Label;
             $$->Selected=$7;
             $$->LineWidthOrRotation=$4;
             $$->BorderColorOrColor=$5;
             $$->X1orCenterX=$3;
             $$->Y1orCenterY=$2;
             $$->RadiusXorNrPoints=$6;
             $$->TextString=$8;
           }
         | PRPIN IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDLABEL IDLABEL EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Pin;
             $$->BorderColorOrColor=$12;
             $$->X1orCenterX=$9;
             $$->Y1orCenterY=$10;
             $$->X2orPinLength=$8;
             $$->PinNr=$14;
             $$->TextString=$13;
             $$->Y2orPinAttributes=MK_PINATTR($2,$3,$4,$5,$6,$7,$11);
           }
         | PRIMAGE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDLABEL EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Image;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->X2orPinLength=$4;
             $$->Y2orPinAttributes=$5;
             $$->LineWidthOrRotation=$6;
             $$->BorderColorOrColor=$7;
             $$->Selected=$8;
             $$->Filled=$9;
             $$->RadiusXorNrPoints=$10;
             $$->TextString=$11;
           }
         | PRLINE IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Line;
             $$->X1orCenterX=$2;
             $$->Y1orCenterY=$3;
             $$->X2orPinLength=$4;
             $$->Y2orPinAttributes=$5;
             $$->LineWidthOrRotation=$6;
             $$->RadiusYorLineType=$7;
             $$->BorderColorOrColor=$8;
             $$->Selected=$9;
           }
         | PRSYMBOL numbers EOL
           {
             $$=NULL; PfWSchLibwarning("Unsupported primitive: Symbol");
           }
         | PRJUNCTION  numbers EOL
           {
             $$=NULL; PfWSchLibwarning("Unsupported primitive: Junction");
           }
         | PRPOLYLINE  IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER optpoints EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Polyline;
             $$->LineWidthOrRotation=$2;
             $$->RadiusYorLineType=$3;
             $$->BorderColorOrColor=$3;
             $$->Selected=$5;
             $$->RadiusXorNrPoints=$6;
             $$->X1orCenterX=$7;
             $$->Y1orCenterY=$8;
             $$->X2orPinLength=$9;
             $$->Y2orPinAttributes=$10;
             $$->pPoints=$11;
           }
         | PRPOLYGON IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER optpoints EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Polygon;
             $$->LineWidthOrRotation=$2;
             $$->BorderColorOrColor=$3;
             $$->FillColor=$4;
             $$->Filled=$5;
             $$->Selected=$6;
             $$->RadiusXorNrPoints=$7;
             $$->X1orCenterX=$8;
             $$->Y1orCenterY=$9;
             $$->X2orPinLength=$10;
             $$->Y2orPinAttributes=$11;
             $$->pPoints=$12;
           }
         | PRBEZIER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER IDNUMBER optpoints EOL
           {
             $$=(PRIMITIVE*)malloc(sizeof(PRIMITIVE));
             ZeroMemory($$,sizeof(PRIMITIVE));
             $$->Type=Bezier;
             $$->LineWidthOrRotation=$2;
             $$->BorderColorOrColor=$3;
             $$->Selected=$4;
             $$->RadiusXorNrPoints=$5;
             $$->X1orCenterX=$6;
             $$->Y1orCenterY=$7;
             $$->X2orPinLength=$8;
             $$->Y2orPinAttributes=$9;
             $$->pPoints=$10;
           }
;
optpoints: /*empty*/ { $$=NULL; }
         | points    { $$=$1;   }
;
points: point
        {
          $$=(POINTS*)malloc(sizeof(POINTS));
          ZeroMemory($$,sizeof(POINTS));
          $$->pPoint=(POINT**)realloc($$->pPoint,(($$->NumPoints)+1)*sizeof(POINT*));
          $$->pPoint[$$->NumPoints]=$1;
          $$->NumPoints++;
        }
      | points point
        {
          $1->pPoint=(POINT**)realloc($1->pPoint,(($1->NumPoints)+1)*sizeof(POINT*));
          $1->pPoint[$1->NumPoints]=$2;
          $1->NumPoints++;
          $$=$1;
        }
;
point: IDNUMBER IDNUMBER
       {
         $$=(POINT*)malloc(sizeof(POINT));
         ZeroMemory($$,sizeof(POINT));
         $$->x=$1;
         $$->y=$2;
       }
;

numbers: IDNUMBER
       | numbers IDNUMBER
;
/* end of temporary def*/

/* end of library_v1 */
/* library_v2 */
library_v2 : KWLIBRARY KWVERSION20 EOL
             {
               $<LIBRARY_V2type>$=(LIBRARY_V2*)malloc(sizeof(LIBRARY_V2));
               ZeroMemory($<LIBRARY_V2type>$,sizeof(LIBRARY_V2));
             }
             optcomponents_v2
             KWENDOFLIBRARY EOL
             {
               $$=$<LIBRARY_V2type>4;
               $$->pCompsV2=$5;
             }
;
optcomponents_v2: /*empty*/     { $$=NULL; }
                | components_v2 { $$=$1;   }
;
components_v2: component_v2
               {
                 $$=(COMPONENTS_V2*)malloc(sizeof(COMPONENTS_V2));
                 ZeroMemory($$,sizeof(COMPONENTS_V2));
                 $$->pCompV2=(COMPONENT_V2**)realloc($$->pCompV2,(($$->RealNumCompV2)+1)*sizeof(COMPONENT_V2*));
                 $$->pCompV2[$$->RealNumCompV2]=$1;
                 $$->RealNumCompV2++;
               }
             | components_v2 component_v2
               {
                 $1->pCompV2=(COMPONENT_V2**)realloc($1->pCompV2,(($1->RealNumCompV2)+1)*sizeof(COMPONENT_V2*));
                 $1->pCompV2[$1->RealNumCompV2]=$2;
                 $1->RealNumCompV2++;
                 $$=$1;
               }
;
component_v2: KWCOMPONENT EOL { LexEnableStrings(); }
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              optstring EOL
              KWENDCOMPONENT EOL
              {
                LexDisableStrings();
                $$=(COMPONENT_V2*)malloc(sizeof(COMPONENT_V2));
                ZeroMemory($$,sizeof(COMPONENT_V2));
                $$->COMPONENT_2=$4;
                $$->PartField_1=$6;
                $$->PartField_2=$8;
                $$->PartField_3=$10;
                $$->PartField_4=$12;
                $$->PartField_5=$14;
                $$->PartField_6=$16;
                $$->PartField_7=$18;
                $$->PartField_8=$20;
                $$->PartField_9=$22;
                $$->PartField_10=$24;
                $$->PartField_11=$26;
                $$->PartField_12=$28;
                $$->PartField_13=$30;
                $$->PartField_14=$32;
                $$->PartField_15=$34;
                $$->PartField_16=$36;
              }
;
/* end of library_v2 */
/* other groupings */
optstring: /*empty*/ { $$=strdup(""); }
          | IDSTRING { $$=$1; }
;

%%
#ifdef _PARSERDEBUG

int InitBisonForW32(char *DbgFileName)
{
  yydebug=1;
  bisonerrfileout=fopen(DbgFileName,"wt");
  if(bisonerrfileout)
    return 0;
  return 1;
}

int CloseBisonForW32(void)
{
  return fclose(bisonerrfileout);
}

void InitBisonForDebug(void)
{
  yydebug=1;
}

#endif

/*
 * Report an error situation discovered in a production
 *
 */

int PfWSchLibwarning(const char *s)
{
  char BisonTxtBuf[128];
  sprintf(BisonTxtBuf,"WARNING: %s, line: %d",s,NumLines+1);
  LibraryNotifyCallback(1,BisonTxtBuf);
  return 0;
}
int PfWSchLiberror(const char *s)
{
  char BisonTxtBuf[128];
  sprintf(BisonTxtBuf,"ERROR: %s, line: %d, found unexpected :%s",s,NumLines+1,PfWSchLibtext);
  LibraryNotifyCallback(-1,BisonTxtBuf);
  return 0;
}
