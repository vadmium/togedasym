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

// PfWSchLibStruct.h : typedefs of structutes used to store parsed
//                    PfW Sch Library
//
//////////////////////////////////////////////////////////////////////

#if !defined(__PFWSCHLIB_STRUCT__INCLUDE_FILE_L__)
#define __PFWSCHLIB_STRUCT__INCLUDE_FILE_L__

enum PrType
{
  Invalid,
  Rectangle,
  Ellipse,
  Arc,
  Pie,
  EllipticalArc,
  RoundRectangle,
  Label,
  Pin,
  Image,
  Line,
  Polyline,
  Polygon,
  Bezier,

  Symbol,
  Junction
};

#define BOOL   unsigned int
#define RGB    unsigned int
#define SHORT  short int
#define FLOAT  double
#define BYTE   char

#define MK_PINATTR(d,c,t,h,n,u,r) (((d)<<10)|((c)<<9)|((t)<<6)|((h)<<5)|((n)<<4)|((u)<<3)|(r))

typedef struct tPOINT
{
  SHORT x;
  SHORT y;
}POINT;

typedef struct tPOINTS
{
  int NumPoints;
  POINT **pPoint;
}POINTS;


typedef struct tPRIMITIVE
{
  enum  PrType Type;
  BOOL  Selected;
  BOOL  Filled;
  BYTE  LineWidthOrRotation;
  RGB   BorderColorOrColor;
  RGB   FillColor;
  SHORT X1orCenterX;
  SHORT Y1orCenterY;
  SHORT X2orPinLength;
  SHORT Y2orPinAttributes;
  SHORT RadiusXorNrPoints;
  SHORT RadiusYorLineType;
  FLOAT StartAngle;
  FLOAT EndAngle;
  char* TextString;
  char* PinNr;
  POINTS *pPoints; /* for Polyline,Polygon,Bezier */
}PRIMITIVE;

/* je¿eli *pPr[x]==NULL to znaczy, ¿e primitive nie obs³ugiwanego typu (Junction lub symbol) */
typedef struct tPRIMITIVES
{
  int NumPrimitives;
  PRIMITIVE **pPr;
}PRIMITIVES;

typedef struct tPART
{
  PRIMITIVES *pNormal;
  PRIMITIVES *pDeMorgan;
  PRIMITIVES *pIEEE;
}PART;

typedef struct tPARTS
{
  int RealNumParts;
  PART **pPartgrp;
}PARTS;

typedef struct tCOMPONENT
{
  int NumParts;			// (number of parts)
  int NumNames;			// (number of dif. names for this component)
  char *Description;	// (short part description ex: 2xEXOR )
  char *Footprint_1;	// (footprint 1)
  char *Footprint_2;	// (footprint 2)
  char *Footprint_3;	// (footprint 3)
  char *Footprint_4;	// (footprint 4)
  char *Field_1;		//  /
  char *Field_2;		//  |  (library fields 1..8
  char *Field_3;		//  |   for component,
  char *Field_4;		//  |   no changed in schematic
  char *Field_5;		//  |
  char *Field_6;		//  |   for example: creator
  char *Field_7;		//  |   licence GPL, etc.)
  char *Field_8;		//
  char *Default_Designator;  // (ex: U?, IC?, R?, C?, etc.)
  char *Sheet_Part_Filename; // (I don't use it for component)
  char *ComponentName1;      //(name 1 for component)
  char *ComponentName2;      //(name 2 for component)
  PARTS * pParts;
}COMPONENT;

typedef struct tCOMPONENTS
{
  int RealNumComp;
  COMPONENT **pComp;
}COMPONENTS;


typedef struct tFONT
{
  int Size;
  char * Name;
}FONT;

typedef struct tFONTS
{
  int RealNumFonts;
  FONT **pFont;
}FONTS;

typedef struct tFONTTABLE
{
  int NumFonts;
  FONTS *pFonts;
}FONTTABLE;

typedef struct tLIBRARY_V1
{
  int NumComponents;
  COMPONENTS *pComps;
}LIBRARY_V1;

typedef struct tCOMPONENT_V2
{
  char * COMPONENT_2;    //   (name of component - one only for multiname component)
  char * PartField_1;   //  /
  char * PartField_2;   // |
  char * PartField_3;   // |
  char * PartField_4;   // |
  char * PartField_5;   // |
  char * PartField_6;   // |
  char * PartField_7;   // |  (name for Part Field 1 .. 16
  char * PartField_8;   // |   - example: price, company, etc.)
  char * PartField_9;   // |
  char * PartField_10;  // |
  char * PartField_11;  // |
  char * PartField_12;  // |
  char * PartField_13;  // |
  char * PartField_14;  // |
  char * PartField_15;  // |
  char * PartField_16;  //
}COMPONENT_V2;

typedef struct tCOMPONENTS_V2
{
  int RealNumCompV2;
  COMPONENT_V2 **pCompV2;
}COMPONENTS_V2;

typedef struct tLIBRARY_V2
{
  COMPONENTS_V2 *pCompsV2;
}LIBRARY_V2;


typedef struct tPFWSCHLIBFILE
{
  char *LibraryName;
  FONTTABLE  *pFontTable;
  LIBRARY_V1 *pLibV1;
  LIBRARY_V2 *pLibV2;
}PFWSCHLIBFILE;

#endif // !defined(__PFWSCHLIB_STRUCT__INCLUDE_FILE_L__)
