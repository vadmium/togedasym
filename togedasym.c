/* togedasym
 * converter from ascii exported Pro*el library parser to gEDA symbols
 * Copyright (C) 2008  Jacek Plucinski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 * this code has been write with gvim (http://www.vim.org),
 * big thanks for the big tool, my right hand
 */

#define FileVersion	"v.2008.04.09 alpha"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <getopt.h>
#include "PfWSchLibStruct.h"
#include "PfWSchLibParser.h"

#ifdef _DEBUG
#include "dmalloc.h"
#endif

#define TOFILE	// if TOFILE not define - don't create files (print all to stdout for tests)

#ifdef TOFILE	// open component file and write to this or print to stdout
#define OUTPUT	OutFile
#else
#define OUTPUT	stdout
#endif

//CONSTANTS
#define YES		     1
#define NO		     0
#define PI	     3.1415927
#define CompFileNameLength  30		// max number of characters for component file name
#define FixedPinLength	   300

// pin attributes mask used by parser
#define DotMask		0x0400
#define ClockMask	0x0200
#define TypeMask	0x01C0
#define HiddenMask	0x0020
#define NameVisibleMask	0x0010
#define NumbVisibleMask	0x0008
#define RotationMask	0x0007
// constants END

// DEFAULT CONVERT PARAMETERS
int FirstComp		    =    -1;	// first comp number to convert (default first in lib)
int LastComp		    =	 -1;	// last  comp number to convert (default last in lib)

unsigned char XYmultip      =	 20;    // Pro*el -> gEDA X Y multiplier (scale)
char	      SetLineWidth  =    NO;	// turn line width conversion ON or OFF (global)
unsigned char LineWidth0    =	  5;
unsigned char LineWidth1    =	 10;
unsigned char LineWidth2    =	 20;
unsigned char LineWidth3    =	 30;

// pins conversion parameters
char	      SetScalePins  =    NO;	// enable pins length to be scaled as graphics
unsigned int  DotSymDia	    =	100;	// dot symbol diameter
unsigned char DotSymLine    = 	 10;	// dot symbol line width
unsigned int  ClkSymWidth   =	 75;	// clock symbol width
unsigned int  ClkSymHigh    =	100;	// clock symbol high
unsigned char ClkSymLine    =	 10;	// clock symbol line width
int	      PinNumHoffset =   100;	// pin number horizontal offset
int	      PinNumVoffset =    50;	// pin number vertical offset
unsigned char PinNumTxtSize =	  8;	// pin number text size
int	      PinLabHoffset =   -50;
int	      PinLabVoffset =     0;
unsigned char PinLabTxtSize =	  8;	// pin label text size
int	      PinTypHoffset =   -50;
int	      PinTypVoffset =  -100;
unsigned char PinTypTxtSize =	  7;	// pin type text size
int	      PinSeqHoffset =   100;
int	      PinSeqVoffset =   -50;
unsigned char PinSeqTxtSize =	  7;	// pin sequence number text size

unsigned char ElArcAproxLev =	  4;	// quarter of number polygon edges aprox. ellipse for arc
unsigned char EllipAproxLev =	  6;	// quarter of number polygon edges aprox. ellipse
unsigned char ElliToCirTole =	  1;	// tolerance (0-100)% ellipse to circle approximation
unsigned char BezierLines   =	  7;	// number of lines aproximated Bezier curve

// parts attributes texts parameters
unsigned int  AttrHoffset   =   400;	// horizontal X coordinate for parts attributes draw
unsigned int  AttrVoffset   =   200;	// vertical offset between parts attributes texts
unsigned char AttrTxtSize   =	 10;	// parts attributes text size
char WrValue		    =	 NO;	// write extra "value=" attribute with comp. name
char WrFootpr2		    =	 NO;	// activate write component footprint 2 attribute
char WrFootpr3		    =	 NO;	//  --''--   -''-   --''--    --''--  3  --''--
char WrFootpr4		    =	 NO;	//  --''--   -''-   --''--    --''--  4  --''--
char WrField1		    =	 NO;	// activate write component field 1 attribute
char WrField2		    =	 NO;	//  --''--   -''-   --''--   -''- 2  --''--
char WrField3		    =	 NO;	//  --''--   -''-   --''--   -''- 3  --''--
char WrField4		    =	 NO;	//  --''--   -''-   --''--   -''- 4  --''--
char WrField5		    =	 NO;	//  --''--   -''-   --''--   -''- 5  --''--
char WrField6		    =	 NO;	//  --''--   -''-   --''--   -''- 6  --''--
char WrField7		    =	 NO;	//  --''--   -''-   --''--   -''- 7  --''--
char WrField8		    =	 NO;	//  --''--   -''-   --''--   -''- 8  --''--
char WrPrField1		    =    NO;	// activate write component part field  1 attribute
char WrPrField2		    =	 NO;	//  --''--   -''-   --''--  part  -''-  2  --''--
char WrPrField3		    =	 NO;	//  --''--   -''-   --''--  part  -''-  3  --''--
char WrPrField4		    =	 NO;	//  --''--   -''-   --''--  part  -''-  4  --''--
char WrPrField5		    =	 NO;	//  --''--   -''-   --''--  part  -''-  5  --''--
char WrPrField6		    =	 NO;	//  --''--   -''-   --''--  part  -''-  6  --''--
char WrPrField7		    =	 NO;	//  --''--   -''-   --''--  part  -''-  7  --''--
char WrPrField8		    =	 NO;	//  --''--   -''-   --''--  part  -''-  8  --''--
char WrPrField9		    =	 NO;	//  --''--   -''-   --''--  part  -''-  9  --''--
char WrPrField10	    =	 NO;	//  --''--   -''-   --''--  part  -''- 10  --''--
char WrPrField11	    =	 NO;	//  --''--   -''-   --''--  part  -''- 11  --''--
char WrPrField12	    =	 NO;	//  --''--   -''-   --''--  part  -''- 12  --''--
char WrPrField13	    =	 NO;	//  --''--   -''-   --''--  part  -''- 13  --''--
char WrPrField14	    =	 NO;	//  --''--   -''-   --''--  part  -''- 14  --''--
char WrPrField15	    =	 NO;	//  --''--   -''-   --''--  part  -''- 15  --''--
char WrPrField16	    =	 NO;	//  --''--   -''-   --''--  part  -''- 16  --''--
//default convert parameters END

char *ExtraAttr[20];	// saved pointers to extra attr. parameters (reset to 0 at start)

static char *ShortParameters = // short parameters definitions
"hl:f:e:ps:wr:";

static struct option LongParameter[] = // long parameters definitions
{
    {"help"		    ,0,0,'h'},
    {"list"		    ,0,0,'l'},
    {"first-component"	    ,1,0,'f'},
    {"last-component"	    ,1,0,'e'},
    {"scale"		    ,1,0,'s'},
    {"line-width"	    ,0,0,'w'},
    {"line-smallest"	    ,1,0,'A'},
    {"line-small"	    ,1,0,'B'},
    {"line-medium"	    ,1,0,'C'},
    {"line-large"	    ,1,0,'D'},

    {"pin-dot-diameter"	    ,1,0,'E'},
    {"pin-dot-line"	    ,1,0,'F'},
    {"pin-clk-width"	    ,1,0,'G'},
    {"pin-clk-high"	    ,1,0,'H'},
    {"pin-clk-line"	    ,1,0,'I'},
    {"pin-number-h-offset"  ,1,0,'J'},
    {"pin-number-v-offset"  ,1,0,'K'},
    {"pin-number-text-size" ,1,0,'L'},
    {"pin-label-h-offset"   ,1,0,'M'},
    {"pin-label-v-offset"   ,1,0,'N'},
    {"pin-label-text-size"  ,1,0,'O'},
    {"pin-type-h-offset"    ,1,0,'P'},
    {"pin-type-v-offset"    ,1,0,'Q'},
    {"pin-type-text-size"   ,1,0,'R'},
    {"pin-seq-h-offset"     ,1,0,'S'},
    {"pin-seq-v-offset"     ,1,0,'T'},
    {"pin-seq-text-size"    ,1,0,'U'},
    {"pin-length-scaled"    ,0,0,'p'},

    {"ellipticalarc-level"  ,1,0,'W'},
    {"ellipse-level"        ,1,0,'X'},
    {"ellipse-tolerance"    ,1,0,'Y'},
    {"bezier-lines"	    ,1,0,'t'},

    {"attribute-h-offset"   ,1,0,'Z'},
    {"attribute-v-offset"   ,1,0,'a'},
    {"attribute-text-size"  ,1,0,'b'},
    {"write-value"	    ,0,0,'c'},
    {"write-footprint"	    ,1,0,'d'},	    // select write footprint 2..4
    {"write-field"	    ,1,0,'j'},	    // select write filed 1..8
    {"write-part-field"	    ,1,0,'k'},	    // select write part field 1..16
    {"extra-attribute"	    ,1,0,'r'},	    // extra attribute to write
    {0,0,0,0}
};

PFWSCHLIBFILE * pPfWLib=NULL;  // Pro*el library file
FILE *OutFile;		    // gEDA file with component declaration
int MaxY;		    // max Y coordinate value (for places to draw symbol attributes)

const char // text
GedaVersion[] = // text; version write to symbol file
{
    "v 20031231 1"
},
    TxtHelp[] = // text; long help
{
    "SYNOPSIS\n"
	"\n"
	"    togedasym --help|-h\n"
	"\t print this help to standard out\n"
	"\n"
	"    togedasym --list|-l  <FILE>\n"
	"\t list components in Pro*el schema library file to standard out\n"
	"\n"
	"    togedasym [OPTION] ... <FILE>\n"
	"\t convert Pro*el schema symbols library file to gEDA format\n"
	"\n"
	"DESCRIPTION\n"
	"\n"
	"\t Convert Pro*el ascii exported schema symbol library to gEDA symbol.\n"
	"\t Primitives not serviced by converter are reported to stderr.\n"
	"\t Options aren't obligatory and are used only to change default values.\n"
	"\n"
	"OPTION\n"
	"\n"
	"--first-component=NUMBER, -f NUMBER\n"
	"\t First component number to convert. Use, if not all components\n"
	"\t should be converted. Before use, list library by -l options\n"
	"\t and select required range of components\n"
	"\t Default: 1 (first components in library)\n"
	"\n"
	"--last-component=NUMBER, -e NUMBER\n"
	"\t Last component number to convert. Use if not all components\n"
	"\t should be converted. Before use, list library by -l options\n"
	"\t and select required range of components\n"
	"\t Default: last components in library\n"
	"\n"
	"--scale=NUMBER, -s NUMBER\n"
	"\t Pro*el to gEDA X Y coordinations multiplier\n"
	"\t Default: 20\n"
	"\n"
	"--line-width, -w\n"
	"\t Globally turns on line width declaration in outputs parts.\n"
	"\t Lines will have width declared by other parameters.\n"
	"\t Default line width conversion is disabled -\n"
	"\t - elements have 0 line width.\n"
	"\n"
	"--line-smallest=NUMBER\n"
	"\t Sets line width for primitives with 'smallest' line width attribute.\n"
	"\t Should be used with --line-width option.\n"
	"\t Default: 5\n"
	"\n"
	"--line-small=NUMBER\n"
	"\t Sets line width for primitives with 'small' line width attribute.\n"
	"\t Should be used with --line-width option.\n"
	"\t Default: 10\n"
	"\n"
	"--line-medium=NUMBER\n"
	"\t Sets line width for primitives with 'medium' line width attribute.\n"
	"\t Should be used with --line-width option.\n"
	"\t Default: 20\n"
	"\n"
	"--line-large=NUMBER\n"
	"\t Sets line width for primitives with 'large' line width attribute.\n"
	"\t Should be use with --line-width option.\n"
	"\t Default: 30\n"
	"\n"
	"--pin-dot-diameter=NUMBER\n"
	"\t Sets pin dot symbol diameter.\n" 
	"\t Changing this parameter changes pin length.\n"
	"\t Default: 100\n"
	"\n"
	"--pin-dot-line=NUMBER\n"
	"\t Sets pin dot symbol line width.\n"
	"\t Should be used with --line-width option.\n"
	"\t Default: 10\n"
	"\n"
	"--pin-clk-width=NUMBER\n"
	"\t Sets pin clock symbol width. Default: 75\n"
	"\n"
	"--pin-clk-high=NUMBER\n"
	"\t Sets pin clock symbol height. Default: 100\n"
	"\n"
	"--pin-clk-line=NUMBER\n"
	"\t Sets pin clock symbol line width.\n"
	"\t Should be used with --line-width option.\n"
	"\t Default: 10\n"
	"\n"
	"--pin-number-h-offset=NUMBER\n"
	"\t Distance between text with pin number and pin start point\n"
	"\t in direction parallel to pin. Default: 100\n"
	"\n"
	"--pin-number-v-offset=NUMBER\n"
	"\t Distance between text with pin number and pin start point\n"
	"\t in direction perpendicular to pin. Default: 100\n"
	"\n"
	"--pin-number-text-size=NUMBER\n"
	"\t Size of text with pin number. Default: 8\n"
	"\n"
	"--pin-label-h-offset=NUMBER\n"
	"\t Distance between text with pin label and pin start point\n"
	"\t in direction parallel to pin. Default: -50\n"
	"\n"
	"--pin-label-v-offset=NUMBER\n"
	"\t Distance between text with pin label and pin start point\n"
	"\t in direction perpendicular to pin. Default: 0\n"
	"\n"
	"--pin-label-text-size=NUMBER\n"
	"\t Size of text with pin label. Default: 8\n"
	"\n"
	"--pin-type-h-offset=NUMBER\n"
	"\t Distance between text with pin type and pin start point\n"
	"\t in direction parallel to pin. Default: -50\n"
	"\n"
	"--pin-type-v-offset=NUMBER\n"
	"\t Distance between text with pin type and pin start point\n"
	"\t in direction perpendicular to pin. Default: -100\n"
	"\n"
	"--pin-type-text-size=NUMBER\n"
	"\t Size of text with pin type. Default: 7\n"
	"\n"
	"--pin-seq-h-offset=NUMBER\n"
	"\t Distance between text with pin sequence number and pin start point\n"
	"\t in direction parallel to pin. Default: 100\n"
	"\n"
	"--pin-seq-v-offset=NUMBER\n"
	"\t Distance between text with pin sequence number and pin start point\n"
	"\t in direction perpendicular to pin. Default: -50\n"
	"\n"
	"--pin-seq-text-size=NUMBER\n"
	"\t Size of text with pin sequence number. Default: 7\n"
	"\n"
	"--pin-length-scaled, -p\n"
	"\t Enable pin length to be scaled as graphics elements.\n"
	"\t This option is used when component has long pin number\n"
	"\t ('Emitter' for example). Default pin length has fixed value 300.\n"
	"\n"
	"--ellipticalarc-level=NUMBER\n"
	"\t Elliptical arcs are approximated by polygon.\n"
	"\t Parameter specifies quarter of number of polygon edges which\n"
       	"\t approximate ellipse. Higher values make elliptical arc smoother\n"
       	"\t but part 'heavier'. Default: 4\n"
	"\n"
	"--ellipse-level=NUMBER\n"
	"\t Ellipses are approximated by polygon.\n"
	"\t Parameter specifies quarter of number of polygon edges which\n"
       	"\t approximate ellipse. Higher values make ellipse smoother but part\n"
       	"\t 'heavier'. Default: 6\n"
	"\n"
	"--ellipse-tolerance=NUMBER\n"
	"\t Ellipses and eliptical arcs with equal diameters are always drawn\n"
       	"\t as circles. Ellipses and eliptical arcs with percentage difference\n"
       	"\t of diameters smaller than NUMBER will be drawn as circles too.\n"
       	"\t If you mistakenly draw circle as ellipse, or arc as eliptical arc,\n"
       	"\t this parameter will help you to correct this.\n"
	"\t To disable this function, set NUMBER to 0.\n"
	"\t Default: 1 (ellipses with 1\% diameters different will be drawn\n"
       	"\t as circle)\n"
	"\n"
	"--bezier-lines=NUMBER\n"
	"\t Bezier curves are approximated by lines.\n"
	"\t Parameter specifies number of lines which aproximate bezier curve.\n"
	"\t Higher values make curve smoother but part more 'heavier'.\n"
	"\t Default: 7\n"
	"\n"
	"--attribute-h-offset=NUMBER\n"
	"\t Sets horizontal (x) coordinate for text that contains part\n"
       	"\t attribues. After symbol translation in gschem, text's placement\n"
       	"\t should be corrected. Default: 400\n"
	"\n"
	"--attribute-v-offset=NUMBER\n"
	"\t Vertical (y) distance between texts that contain parts attributes\n"
       	"\t and vertical (y) distance between first text that contain part\n"
	"\t attribute and part. Default: 200\n"
	"\n"
	"--attribute-text-size=NUMBER\n"
	"\t Part attributes texts size.\n"
	"\t Default: 10\n"
	"\n"
	"--write-value\n"
	"\t Attribute \"device\" should be invisible in gEDA.\n"
	"\t This option enables to write additional visible text attribute\n"
       	"\t \"value=device\" which always shows component device type\n"
       	"\t and can by edited in gschem.\n"
	"\n"
	"--write-footprint=[2][:3][:4]\n"
	"\t Enables to write footprints 2, 3 or 4 texts attributes\n"
       	"\t in parts files. Footprint texts have hidden attribute.\n"
	"\t By default only footprint 1 is written (obligatory in gEDA).\n"
	"\t If footprint attribute has not value, text 'no value' appears\n"
	"\n"
	"--write-field=[1][:2] ... [:8]\n"
	"\t Enables to write field 1..8 texts attributes in parts files.\n"
	"\t By default no field text is written.\n"
	"\t If field attribute has no value, text 'no value' appears\n"
	"\n"
	"--write-part-field=[1][:2] ... [:16]\n"
	"\t Enables to write part field 1..16 texts attributes in parts files.\n"
	"\t By default no part field text is written.\n"
	"\t If part field attribute has no value, text 'no value' appears.\n"
	"\n"
	"--extra-attribute=[STRING], -r [STRING]\n"
	"\t Add additional hidden attribute to component,\n"
	"\t for example: -r author=jonny bravo\n"
	"\n"
},
    TxtNotDeclared[] = // info text; component attributes have to declare
{
    "not declared, declare please"
},
    TxtErrOptionsFirstLastComp[] =  // text; test for selected components numbers fail
{
    "options error: \n"
	" invalid first or last component number\n"
	" first component number should be lower then last\n"
	" and last component number should be lower then number\n"
	" component in library\n"
},
    TxtErrParamVal[] =	// text; incorrect fields select
{
    "incorrect parameter value between ':'"
};


int NotifyCallback(int nErrCode, char* message)
{
    fprintf(stderr, "%s\n", message);
}

void Exit(char ExitCode)
{
    PfWSchLibFreeAllEx(&pPfWLib);	    
    switch (ExitCode)
    {
	case  0  :
	    exit(0);
	case  1  : 
	    exit(1);
	case 'h' : 
	    fprintf(stdout, "\nPro*el to gEDA schema symbol converter %s\n\n%s",
		    FileVersion, TxtHelp);
	    exit(0);
	default:
	    exit(1);
    }
}

void ListLibComponents() // list all library components
{
    int i;
    fprintf(stdout, "\nLibrary name: %s\nNumber Components: %d\n\n",
	    pPfWLib->LibraryName, pPfWLib->pLibV1->pComps->RealNumComp);

    for(i = 0; i < pPfWLib->pLibV1->pComps->RealNumComp; i++)
    {
	fprintf(stdout, "%4d  parts:%2d  name: %s\n", i+1,
		pPfWLib->pLibV1->pComps->pComp[i]->pParts->RealNumParts,
		pPfWLib->pLibV1->pComps->pComp[i]->ComponentName1);
    }
}

int ParseFile(int argc, char* argv[]) // parse library file
{
    fprintf(stderr, "Parsing...\n");
    PfWSchLibSetCallbackFunction(NotifyCallback);
    if (PfWSchLibLoadFileEx(argv[argc-1],&pPfWLib))
    {
	fprintf(stderr, "Parse error, exit\n");
	return 1;
    }
    else
    {
	fprintf(stderr, "Parse OK\n");
	return 0;
    }
}

int TestParam(int LongIndex,  int Min, int Max) // test single parameter
{
    int ParValue = atoi(optarg);

    if (ParValue < Min)
    {
	fprintf(stderr,
		"PARAM ERROR: invalid value: --%s=%d\n"
		"  parameter should be greater then %d\n",
		LongParameter[LongIndex].name, ParValue, Min);
	return 1;
    }
    if (ParValue > Max)
    {
	fprintf(stderr,
		"PARAM ERROR: invalid value: --%s=%d\n"
		"  parameter should be lower then %d\n",
		LongParameter[LongIndex].name, ParValue, Max);
	return 1;
    }
    fprintf(stderr, "%s=%d\n", LongParameter[LongIndex].name, ParValue);
    return 0;
}

int GetOptParValue()
{
    /*	function read positive value from optarg
     *	  character by character
     *	return:
     *	    value from string have optarg start point or
     *	    -1 if no more value in optarg are to read
     *
     *	warning:
     *	    function destroy optarg pointer
     */
    char Str[] = {0,0,0}; // one of parameter value as string

    if (*optarg == ':') ++optarg;			// skip ":"
    if ((*optarg == '=') || (*optarg == 0)) return -1;	// it is end optarg

    Str[0] = *optarg;	    // read first value character
    ++optarg;		    // set pointer to second character
    if ((*optarg == '=') || (*optarg == 0) || (*optarg == ':'))
	return atoi(Str);
    else		    // it's second value character
	Str[1] = *optarg;
    ++optarg;		    // get next char (should be ":" or "=" or "0")
    if ((*optarg == '=') || (*optarg == 0) || (*optarg == ':'))
	return atoi(Str);

    // strange string in options
    fprintf(stderr,"%s\n", TxtErrParamVal);
    return -2;
}

int GetOptions(int argc, char* argv[])  // read option & set parameters
{
    int Option;		// short option
    int Arg = 0;	// option argument value
    int LongIndex;

    char **pExtraAttr = ExtraAttr; // pointer to extra attributes string

    if (argc<2) return -1;  // no parameters

    while ((Option = getopt_long(argc, argv,
		    ShortParameters, LongParameter, &LongIndex)) != -1)
    {
	/* search option LongIndex
	 * for short options getopt_long don't return LongIndex
	 * required to print information about option
	 */
	if (Option == '?')	// next used 'while' fuse
	{ fprintf(stderr, "invalid parameter\n"); return 1; }
	LongIndex = 0; while (LongParameter[LongIndex].val != Option) ++LongIndex;

	switch (Option)
	{
	    case 'h' :
		return 'h';	//  info to exit program
	    case 'l' :
		fprintf(stderr, "list components\n");
		return 'l';	
	    case 'f' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		FirstComp = atoi(optarg); break;
	    case 'e' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		LastComp = atoi(optarg); break;
	    case 's' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		XYmultip = atoi(optarg); break;
	    case 'w' :
		SetLineWidth = YES;
		fprintf(stderr, "set line width ON\n"); break;
	    case 'A' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		LineWidth0 = atoi(optarg); break;
	    case 'B' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		LineWidth1 = atoi(optarg); break;
	    case 'C' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		LineWidth2 = atoi(optarg); break;
	    case 'D' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		LineWidth3 = atoi(optarg); break;
	    case 'E' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		DotSymDia = atoi(optarg); break;
	    case 'F' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		DotSymLine = atoi(optarg); break;
	    case 'G' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		ClkSymWidth = atoi(optarg); break;
	    case 'H' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		ClkSymHigh = atoi(optarg); break;
	    case 'I' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		ClkSymLine = atoi(optarg); break;
	    case 'J' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinNumHoffset = atoi(optarg); break;
	    case 'K' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinNumVoffset = atoi(optarg); break;
	    case 'L' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		PinNumTxtSize = atoi(optarg); break;
	    case 'M' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinLabHoffset = atoi(optarg); break;
	    case 'N' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinLabVoffset = atoi(optarg); break;
	    case 'O' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		PinLabTxtSize = atoi(optarg); break;
	    case 'P' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinTypHoffset = atoi(optarg); break;
	    case 'Q' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinTypVoffset = atoi(optarg); break;
	    case 'R' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		PinTypTxtSize = atoi(optarg); break;
	    case 'S' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinSeqHoffset = atoi(optarg); break;
	    case 'T' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		PinSeqVoffset = atoi(optarg); break;
	    case 'U' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		PinSeqTxtSize = atoi(optarg); break;
	    case 'p' :
		SetScalePins = YES;
		fprintf(stderr, "scale length of pins ON\n"); break;
	    case 'W' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		ElArcAproxLev = atoi(optarg); break;
	    case 'X' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		EllipAproxLev = atoi(optarg); break;
	    case 'Y' :
		if (TestParam(LongIndex, 1, 100)) return 1;
		ElliToCirTole = atoi(optarg); break;
	    case 't' :
		if (TestParam(LongIndex, 1, 0xff)) return 1;
		BezierLines = atoi(optarg); break;
	    case 'Z' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		AttrHoffset = atoi(optarg); break;
	    case 'a' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		AttrVoffset = atoi(optarg); break;
	    case 'b' :
		if (TestParam(LongIndex, 1, 0xffff)) return 1;
		AttrTxtSize = atoi(optarg); break;
	    case 'c' :
		fprintf(stderr, "write value attribute ON\n");
		WrValue = YES; break;
	    case 'd' :
		while ((Arg = GetOptParValue()) != -1 )
		{
		    switch (Arg)
		    {
			case 2 : WrFootpr2 = YES; break;
			case 3 : WrFootpr3 = YES; break;
			case 4 : WrFootpr4 = YES; break;
			default : fprintf(stderr,
					  "PARAM ERROR: incorrect footprint number %d\n"
					  "  footprint should be 2 3 or 4 and separate by \":\"\n"
					  "  (footprint 1 always have to write to symbol -\n"
					  "   - it's obligatory in gEDA)\n", Arg);
				  return 1; // exit
		    }
		    fprintf(stderr, "footprint %d = ON\n", Arg);
		}
		break;
	    case 'j' :
		while ((Arg = GetOptParValue()) != -1 )
		{
		    switch (Arg)
		    {
			case 1 : WrField1 = YES; break;
			case 2 : WrField2 = YES; break;
			case 3 : WrField3 = YES; break;
			case 4 : WrField4 = YES; break;
			case 5 : WrField5 = YES; break;
			case 6 : WrField6 = YES; break;
			case 7 : WrField7 = YES; break;
			case 8 : WrField8 = YES; break;
			default : fprintf(stderr,
					  "PARAM ERROR: incorrect field number %d\n"
					  "  field numbers should be 1..8 and separate by \":\"\n", Arg);
				  return 1; // exit
		    }
		    fprintf(stderr, "field %d = ON\n", Arg);
		}
		break;
	    case 'k' :
		while ((Arg = GetOptParValue()) != -1 )
		{
		    switch (Arg)
		    {
			case  1 : WrPrField1  = YES; break;
			case  2 : WrPrField2  = YES; break;
			case  3 : WrPrField3  = YES; break;
			case  4 : WrPrField4  = YES; break;
			case  5 : WrPrField5  = YES; break;
			case  6 : WrPrField6  = YES; break;
			case  7 : WrPrField7  = YES; break;
			case  8 : WrPrField8  = YES; break;
			case  9 : WrPrField9  = YES; break;
			case 10 : WrPrField10 = YES; break;
			case 11 : WrPrField11 = YES; break;
			case 12 : WrPrField12 = YES; break;
			case 13 : WrPrField13 = YES; break;
			case 14 : WrPrField14 = YES; break;
			case 15 : WrPrField15 = YES; break;
			case 16 : WrPrField16 = YES; break;
			default : fprintf(stderr,
					  "PARAM ERROR: incorrect part field number %d\n"
					  "  part field numbers should be 1..16 and separate by \":\"\n", Arg);
				  return 1; // exit
		    }
		    fprintf(stderr, "part field %d = ON\n", Arg);
		}
		break;
	    case 'r' :
		*pExtraAttr = optarg;
		fprintf(stderr, "extra attributes: %s\n", *pExtraAttr);
	       	++pExtraAttr; break;
	}
    }
    return 0;
}

int ChooseLineWidth(PRIMITIVE * pPrShort) // shoes primitives line width
{
    int LineWidth;
    switch (pPrShort->LineWidthOrRotation)
    {
	case 0 : LineWidth=LineWidth0; break;
	case 1 : LineWidth=LineWidth1; break;
	case 2 : LineWidth=LineWidth2; break;
	case 3 : LineWidth=LineWidth3; break;
    }
    return LineWidth;
}

float CalcCirAngle(float Angle, float RadiusX, float RadiusY) // scale ellipse arc to circle arc
{
    /* scale ellipse arc (in degrees) to circle arc
     * and change value returned by arc tangent function
     * from -/+ PI/2 to value between 0 and 2PI
     *
     * ellipse equations:
     *	    http://en.wikipedia.org/wiki/Ellipse
     * below use equations:
     *	    tg(CirAngle) = (ScaleY/ScaleX) * tg(ElliAngle)
     */
    float ScaleYX = RadiusX/RadiusY;
    if      (Angle ==   0)
	return (0);
    else if (Angle ==  90)
	return (PI/2);
    else if (Angle == 180)
	return (PI);
    else if (Angle == 270)
	return (1.5*PI);
    else if ((Angle > 0) && (Angle < 90))
	return (atanf(ScaleYX * tanf(Angle*PI/180)));
    else if ((Angle > 90) && (Angle < 270))
	return (atanf(ScaleYX * tanf(Angle*PI/180)) + PI);
    else if ((Angle > 270) && (Angle < 360))
	return (atanf(ScaleYX * tanf(Angle*PI/180)) + 2*PI);
}

// DRAW COMPONENTS PRIMITIVES & ATTRIBUTES PROCEDURES
// ---- ---------- ---------- - ---------- ----------
int DrawRectangle(PRIMITIVE * pPrShort) // write Rectangle declaration to file
{
    /* gEDA box object fields:
     * field:	   value:
     * type	     B
     * x	
     * y	
     * width	
     * height	
     * color	     3
     * width	
     * capstyle	     0
     * dashtype	     0
     * dashlength   -1
     * dashspace    -1
     * filltype	     0
     * fillwidth    -1
     * angle1	    -1
     * pitch1	    -1
     * angle2	    -1
     * pitch2	    -1
     */

    int X = XYmultip*((pPrShort->X1orCenterX < pPrShort->X2orPinLength) ?
	    pPrShort->X1orCenterX : pPrShort->X2orPinLength);
    int Y = XYmultip*((pPrShort->Y1orCenterY < pPrShort->Y2orPinAttributes) ?
	    pPrShort->Y1orCenterY : pPrShort->Y2orPinAttributes);
    int Width  = XYmultip*abs(pPrShort->X1orCenterX - pPrShort->X2orPinLength);
    int Height = XYmultip*abs(pPrShort->Y1orCenterY - pPrShort->Y2orPinAttributes);
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;

    // write box
    fprintf(OUTPUT, "B %d %d %d %d 3 %d 0 0 -1 -1 0 -1 -1 -1 -1\n",
	    X, Y, Width, Height, LineWidth);
    // Y coordinate correction for write text attributes
    if (MaxY < (Y + Height)) MaxY = Y + Height;
    return 0;
}

int DrawEllipse(PRIMITIVE * pPrShort) // write Ellipse declaration to file
{

    /* ellipse is drawing as lines approximation in first quarter
     * and mirrored to 3 last quarters;
     * elementary angle for approximation is equal (PI/2)/EllipAproxLev
     */
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;
    int CenterX = XYmultip*(pPrShort->X1orCenterX);
    int CenterY = XYmultip*(pPrShort->Y1orCenterY);
    int RadiusX = XYmultip*(pPrShort->RadiusXorNrPoints);
    int RadiusY = XYmultip*(pPrShort->RadiusYorLineType);
    char AngleCounter;
    char SignX, SignY; // +1 or -1
    float Angle = PI/4/EllipAproxLev;	// start at 1/2 elementary angle
    float X1, Y1, X2, Y2; // line points coordinates

    // if ellipse is circle - draw circle & exit
    if (200*(RadiusX-RadiusY)/(RadiusX+RadiusY) <= ElliToCirTole)
    {
	fprintf(OUTPUT, "V %d %d %d 3 %d 0 0 -1 -1 0 -1 -1 -1 -1 -1\n",
		CenterX, CenterY, RadiusX, LineWidth);
	return 0;
    }

    // draw first line & mirror it
    // (first line can be mirrored only by X or only by Y axis)
    X1 = RadiusX * cosf(Angle);
    Y1 = RadiusY * sinf(Angle);
    for (SignX = -1; SignX < 2; SignX += 2)
	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		(int)(SignX*X1 + CenterX), (int)( SignX*Y1 + CenterY),
		(int)(SignX*X1 + CenterX), (int)(-SignX*Y1 + CenterY),
		LineWidth);
    X1 = RadiusX * cosf(PI/2-Angle);
    Y1 = RadiusY * sinf(PI/2-Angle);
    for (SignX = -1; SignX < 2; SignX += 2)
	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		(int)( SignX*X1 + CenterX), (int)(SignX*Y1 + CenterY),
		(int)(-SignX*X1 + CenterX), (int)(SignX*Y1 + CenterY),
		LineWidth);
    // draw lines up to PI/2 & mirror it
    for (AngleCounter=1; AngleCounter < EllipAproxLev; ++AngleCounter)
    {
	// calculate line coordinates
	X1 = RadiusX * cosf(Angle);
	Y1 = RadiusY * sinf(Angle);
	Angle += PI/2/EllipAproxLev;
	X2 = RadiusX * cosf(Angle);
	Y2 = RadiusY * sinf(Angle);

	for (SignX = -1; SignX < 2; SignX += 2)
	{
	    for (SignY = -1; SignY < 2; SignY += 2)
	    {
		// draw approximated ellipse lines
		fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
			(int)(SignX*X1 + CenterX), (int)(SignY*Y1 + CenterY),
			(int)(SignX*X2 + CenterX), (int)(SignY*Y2 + CenterY),
			LineWidth);
	    }
	}
    }
    // Y coordinate correction for write text attributes
    if (MaxY < (CenterY+RadiusY)) MaxY = CenterY+RadiusY;
    return 0;
}

int DrawArc(PRIMITIVE * pPrShort) // write Arc declaration to file
{
    /*
     *  gEDA arc object fields:
     *	    field:	    value:
     *	    type	    A
     *	    x
     *	    y	
     *	    radius	
     *	    startangle
     *	    sweepangle
     *	    color	    3
     *	    width	
     *	    capstyle	    0
     *	    dashtype	    0
     *	    dashlength	   -1
     *	    dashspace	   -1
     */

    int X = XYmultip*(pPrShort->X1orCenterX);
    int Y = XYmultip*(pPrShort->Y1orCenterY);
    int Radius = XYmultip*(pPrShort->RadiusXorNrPoints);
    int StartAngle = pPrShort->StartAngle;
    int EndAngle = pPrShort->EndAngle;
    int SweepAngle = (StartAngle < EndAngle) ?
	EndAngle - StartAngle : 360 + EndAngle - StartAngle;
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;

    fprintf(OUTPUT, "A %d %d %d %d %d 3 %d 0 0 -1 -1\n",
	    X, Y, Radius, StartAngle, SweepAngle, LineWidth);
    // Y coordinate correction for write text attributes
    if (MaxY < (Y+Radius)) MaxY = Y+Radius;
    return 0;
}

int DrawEllipticalArc(PRIMITIVE * pPrShort) // write EllipticalArc aprox. by lines
{
    /* elliptical arc is approximated by lines;
     * first, elliptical arcs are rescaled to circle,
     * next, arcs are approximated by lines
     * and rescaled down to elliptical arc
     */
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;
    int CenterX = XYmultip*(pPrShort->X1orCenterX);
    int CenterY = XYmultip*(pPrShort->Y1orCenterY);
    float RadiusX = XYmultip*(pPrShort->RadiusXorNrPoints);
    float RadiusY = XYmultip*(pPrShort->RadiusYorLineType);

    /* if elliptical arc is arc - draw arc & exit
     *   elliptical and arc have same parameters
     *   because of this DrawArc function is used
     */
    if (200*(RadiusX-RadiusY)/(RadiusX+RadiusY) <= ElliToCirTole)
	return DrawArc(pPrShort);

    float StartAngle = (pPrShort->StartAngle > 0) ? // in degrees
	pPrShort->StartAngle :
	pPrShort->StartAngle + 360;
    while (StartAngle >= 360) StartAngle-=360;

    float EndAngle = (pPrShort->EndAngle > 0) ? // in degrees
	pPrShort->EndAngle :
	pPrShort->EndAngle + 360; 	
    while (EndAngle >= 360) EndAngle-=360;

    // scale elliptical arcs to circle arcs (result in radians)
    float CirStartAngle =  CalcCirAngle(StartAngle, RadiusX, RadiusY);
    float CirEndAngle   =  CalcCirAngle(  EndAngle, RadiusX, RadiusY);

    if (CirStartAngle > CirEndAngle) CirEndAngle += 2*PI;

    // draw lines
    float Angle = CirStartAngle;
    float NextAngle = Angle + PI/(2*ElArcAproxLev);

    while (NextAngle < CirEndAngle)
    {
	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		(int)(CenterX + (RadiusX * cosf(Angle))),	// x1
		(int)(CenterY + (RadiusY * sinf(Angle))),	// y1
		(int)(CenterX + (RadiusX * cosf(NextAngle))),	// x2
		(int)(CenterY + (RadiusY * sinf(NextAngle))),	// y2
		LineWidth);
	Angle = NextAngle; NextAngle += PI/(2*ElArcAproxLev);
    }
    // draw last (or only one) line to end point
    fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
	    (int)(CenterX + (RadiusX * cosf(Angle))),	// x1
	    (int)(CenterY + (RadiusY * sinf(Angle))),	// y1
	    (int)(CenterX + (RadiusX * cosf(CirEndAngle))),	// x2
	    (int)(CenterY + (RadiusY * sinf(CirEndAngle))),	// y2
	    LineWidth);
    // Y coordinate correction for write text attributes
    if (MaxY < (CenterY+RadiusY)) MaxY = CenterY+RadiusY;
    return 0;
}

int DrawRoundRect(PRIMITIVE * pPrShort) // write round rectangle declaration to file
{
    // round rectangle is draw as lines & arc
    /* for round radius, smaller is taken
     * field: type x1 y1 x2 y2 color width capstyle dashstyle dashlength dashspace
     * value:	L		3	      0		0	 -1        -1	
     */
    char i;
    int Radius = XYmultip*((pPrShort->RadiusXorNrPoints < pPrShort->RadiusYorLineType) ?
	    pPrShort->RadiusXorNrPoints : pPrShort->RadiusYorLineType);
    int X1, Y1, X2, Y2; // x,y for lines & arcs
    int X1R, Y1R, X2R, Y2R; // x,y for arcs
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;

    // will be: X1,Y1 - lower left corner , X2,Y2 - upper right corner
    if (pPrShort->X1orCenterX < pPrShort->X2orPinLength)
    {
	X1 = XYmultip*(pPrShort->X1orCenterX);
	X2 = XYmultip*(pPrShort->X2orPinLength);
    }
    else
    {
	X1 = XYmultip*(pPrShort->X2orPinLength);
	X2 = XYmultip*(pPrShort->X1orCenterX);
    }
    if (pPrShort->Y1orCenterY < pPrShort->Y2orPinAttributes)
    {
	Y1 = XYmultip*(pPrShort->Y1orCenterY);
	Y2 = XYmultip*(pPrShort->Y2orPinAttributes);
    }
    else
    {
	Y1 = XYmultip*(pPrShort->Y2orPinAttributes);
	Y2 = XYmultip*(pPrShort->Y1orCenterY);
    }

    X1R = X1+Radius; X2R = X2-Radius;
    Y1R = Y1+Radius; Y2R = Y2-Radius;

    int Line[4][4] =	    // x1, y1, x2, y2 for lines
    { 	X1, Y1R,  X1, Y2R,
	X2, Y1R,  X2, Y2R,
	X1R, Y1, X2R,  Y1,
	X1R, Y2, X2R,  Y2
    };
    int Arc[4][3] =	    // x, y, start angle for arcs
    { 	X2R, Y2R,   0,
	X1R, Y2R,  90,
	X1R, Y1R, 180,
	X2R, Y1R, 270
    };
    for(i=0;i<4;i++)	// write lines
	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		Line[i][0], Line[i][1], Line[i][2], Line[i][3], LineWidth);
    for(i=0;i<4;i++)    // write arcs
	fprintf(OUTPUT, "A %d %d %d %d 90 3 %d 0 0 -1 -1\n",
		Arc[i][0], Arc[i][1], Radius, Arc[i][2], LineWidth);
    // Y coordinate correction for write text attributes
    if (MaxY < Y2) MaxY = Y2;
    return 0;
}

int WriteLabel(PRIMITIVE * pPrShort) // write label declaration to file
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		9
     *  size
     *  visibility	1
     *  show_name_value 1
     *  angle		
     *  alignment	0
     *  num_lines	1
     */

    int X = XYmultip*(pPrShort->X1orCenterX);
    int Y = XYmultip*(pPrShort->Y1orCenterY);
    int Angle = 90*pPrShort->LineWidthOrRotation;
    int FontNr = pPrShort->RadiusXorNrPoints;
    int Size = pPfWLib->pFontTable->pFonts->pFont[FontNr-1]->Size;

    fprintf(OUTPUT, "T %d %d 9 %d 1 1 %d 0 1\n", X, Y, Size, Angle);
    fprintf(OUTPUT, "%s\n",pPrShort->TextString);
    // Y coordinate correction for write text attributes
    if (MaxY < Y) MaxY = Y+100;
    return 0;
}

int WritePinNumber(PRIMITIVE * pPrShort) // write pin number declaration to file
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		5
     *  size
     *  visibility	
     *  show_name_value 1
     *  angle		
     *  alignment	
     *  num_lines	1
     */
    int X = XYmultip*pPrShort->X1orCenterX; // start pin X position
    int Y = XYmultip*pPrShort->Y1orCenterY; // start pin Y position
    int X1, Y1;	// text position
    char Align; int Angle;
    char Visible = (pPrShort->Y2orPinAttributes & NumbVisibleMask) ? 1 : 0;
    int DH = PinNumHoffset; // X1-X from parameters & rotation (corrected below)
    int DV = PinNumVoffset; // Y1-Y from parameters & rotation (corrected below)

    // calculate position, alignment, rotation
    switch (pPrShort->Y2orPinAttributes & RotationMask)
    {
	case 0 : X1 = X+DH; Y1 = Y+DV; Align = 0; Angle =  0; break; // rotate   0
	case 1 : X1 = X-DV; Y1 = Y+DH; Align = 0; Angle = 90; break; // rotate  90
	case 2 : X1 = X-DH; Y1 = Y+DV; Align = 6; Angle =  0; break; // rotate 180
	case 3 : X1 = X-DV; Y1 = Y-DH; Align = 6; Angle = 90; break; // rotate 270
    }
    fprintf(OUTPUT, "T %d %d 5 %d %d 1 %d %d 1\n",
	    X1, Y1, PinNumTxtSize, Visible, Angle, Align);
    fprintf(OUTPUT, "pinnumber=%s\n", pPrShort->PinNr);
    return 0;
}

int WritePinLabel(PRIMITIVE * pPrShort) // write pin label declaration to file
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		9
     *  size
     *  visibility	
     *  show_name_value 1
     *  angle		
     *  alignment	
     *  num_lines	1
     */
    int X = XYmultip*pPrShort->X1orCenterX; // start pin X position
    int Y = XYmultip*pPrShort->Y1orCenterY; // start pin Y position
    int X1, Y1;	// text position
    char Align; int Angle;
    char Visible = (pPrShort->Y2orPinAttributes & NameVisibleMask) ? 1 : 0;
    int DH = PinLabHoffset; // X1-X from parameters & rotation (corrected below)
    int DV = PinLabVoffset; // Y1-Y from parameters & rotation (corrected below)

    // calculate position, alignment, rotation
    switch (pPrShort->Y2orPinAttributes & RotationMask)
    {
	case 0 : X1 = X+DH; Y1 = Y+DV; Align = 7; Angle =  0; break; // rotate   0
	case 1 : X1 = X-DV; Y1 = Y+DH; Align = 7; Angle = 90; break; // rotate  90
	case 2 : X1 = X-DH; Y1 = Y+DV; Align = 1; Angle =  0; break; // rotate 180
	case 3 : X1 = X-DV; Y1 = Y-DH; Align = 1; Angle = 90; break; // rotate 270
    }
    fprintf(OUTPUT, "T %d %d 9 %d %d 1 %d %d 1\n",
	    X1, Y1, PinLabTxtSize, Visible, Angle, Align);
    fprintf(OUTPUT, "pinlabel=%s\n",pPrShort->TextString);
    return 0;
}

int WritePinType(PRIMITIVE * pPrShort) // write pin type declaration to file
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		5
     *  size
     *  visibility	0
     *  show_name_value 1
     *  angle		
     *  alignment	
     *  num_lines	1
     */
    int X = XYmultip*pPrShort->X1orCenterX; // start pin X position
    int Y = XYmultip*pPrShort->Y1orCenterY; // start pin Y position
    int X1, Y1;	// text position
    char Align; int Angle;
    int DH = PinTypHoffset; // X1-X from parameters & rotation (corrected below)
    int DV = PinTypVoffset; // Y1-Y from parameters & rotation (corrected below)
    char PinType[4];

    // calculate position, alignment, rotation
    switch (pPrShort->Y2orPinAttributes & RotationMask)
    {
	case 0 : X1 = X+DH; Y1 = Y+DV; Align = 7; Angle =  0; break; // rotate   0
	case 1 : X1 = X-DV; Y1 = Y+DH; Align = 7; Angle = 90; break; // rotate  90
	case 2 : X1 = X-DH; Y1 = Y+DV; Align = 1; Angle =  0; break; // rotate 180
	case 3 : X1 = X-DV; Y1 = Y-DH; Align = 1; Angle = 90; break; // rotate 270
    }
    // pin type Pro*el->gEDA conversion
    switch ((pPrShort->Y2orPinAttributes & TypeMask)>>6)
    {
	case 0 : strcpy(PinType,"in" ); break; // Input
	case 1 : strcpy(PinType,"io" ); break; // IO
	case 2 : strcpy(PinType,"out"); break; // Output
	case 3 : strcpy(PinType,"oc" ); break; // OpenCollector
	case 4 : strcpy(PinType,"pas"); break; // Passive
	case 5 : strcpy(PinType,"tri"); break; // HiZ
	case 6 : strcpy(PinType,"oe" ); break; // OpenEmitter
	case 7 : strcpy(PinType,"pwr"); break; // Power
	default :
		 fprintf(stderr, "invalid pin type in primitive attributes: 0x%0x\n",
			 pPrShort->Y2orPinAttributes); return 1;
    }
    fprintf(OUTPUT, "T %d %d 5 %d 0 1 %d %d 1\n",
	    X1, Y1, PinTypTxtSize, Angle, Align);
    fprintf(OUTPUT, "pintype=%s\n", PinType);
    return 0;
}

int WritePinSeq(PRIMITIVE * pPrShort, char PinSeq) // write pin sequence number
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		5
     *  size
     *  visibility	0
     *  show_name_value 1
     *  angle		
     *  alignment	
     *  num_lines	1
     */
    int X = XYmultip*pPrShort->X1orCenterX; // start pin X position
    int Y = XYmultip*pPrShort->Y1orCenterY; // start pin Y position
    int X1, Y1;	// text position
    char Align; int Angle;
    int DH = PinSeqHoffset; // X1-X from parameters & rotation (corrected below)
    int DV = PinSeqVoffset; // Y1-Y from parameters & rotation (corrected below)

    // calculate position, alignment, rotation
    switch (pPrShort->Y2orPinAttributes & RotationMask)
    {
	case 0 : X1 = X+DH; Y1 = Y+DV; Align = 2; Angle =  0; break; // rotate   0
	case 1 : X1 = X-DV; Y1 = Y+DH; Align = 2; Angle = 90; break; // rotate  90
	case 2 : X1 = X-DH; Y1 = Y+DV; Align = 8; Angle =  0; break; // rotate 180
	case 3 : X1 = X-DV; Y1 = Y-DH; Align = 8; Angle = 90; break; // rotate 270
    }
    fprintf(OUTPUT, "T %d %d 5 %d 0 1 %d %d 1\n",
	    X1, Y1, PinSeqTxtSize, Angle, Align);
    fprintf(OUTPUT, "pinseq=%d\n", PinSeq);
    return 0;
}

int DrawPin(PRIMITIVE * pPrShort, char PinSeq) // write Pin declaration to file
{
    /* about pins
     * --------------
     * gEDA pin object fields:
     *	    field: type x1 y1 x2 y2 color pintype whichend
     *	    value:  P		      1      0	    1 (second point active)
     *
     * pin attributes:
     *	    2^10*(dot symbol) + 2^9*(clock symbol) + 2^6*(type) + 2^5*(hidden) +
     *	    + 2^4*(name visible) + 2^3*(number visible) +2^0*(rotation)
     *
     * pintype gEDA values:
     *     in   Input
     *     out  Output
     *     io   Input/Output
     *     oc   Open collector
     *     oe   Open emitter
     *     pas  Passive
     *     tp   Totem pole
     *     tri  Tristate (high impedance)
     *     clk  Clock
     *     pwr  Power/Ground
     *
     * pin decl. example
     *	    P 0 300 300 300 1 0 0
     *	    {
     *	    T 200 350 5 8 1 1 0 6 1
     *	    pinnumber=8
     *	    T 200 250 5 8 0 1 0 8 1
     *	    pinseq=8
     *	    T 350 300 9 8 1 1 0 0 1
     *	    pinlabel=I7
     *	    T 350 300 5 8 0 1 0 2 1
     *	    pintype=in
     *	    }
     *
     *  dot circle definition:
     *	    field:	    value:
     *	    type	    V
     *	    x
     *	    y	
     *	    radius	    DotSymDia/2
     *	    color	    6
     *	    width	    DotSymLine
     *	    capstyle	    0
     *	    dashtype	    0
     *	    dashlength	   -1
     *	    dashspace	   -1
     *	    filltype	    0
     *	    fillwidth	   -1
     *	    angle1	   -1
     *	    pitch1	   -1
     *	    angle2	   -1
     *	    pitch2	   -1
     *
     *  clock lines definition:
     *	    field: type x1 y1 x2 y2 color width capstyle dashstyle dashlength dashspace
     *	    value:   L                3            0	      0	        -1        -1	
     */

    int x1 = XYmultip*pPrShort->X1orCenterX, y1 = XYmultip*pPrShort->Y1orCenterY;
    int PinLength;
    int x2, y2;
    int DotPlace = 0;	// pin length correction if dot symbol exist
    signed char i1;	// shoes + (0,90)  or - (180,270) operation
    signed char kx, ky;	// shoes x (0,180) or y ( 90,270) correction kx = !ky

    char * PinNumber = pPrShort->PinNr;
    char * PinLabel  = pPrShort->TextString;
    char Alignment;

    // draw pin line & dot | clock symbol
    switch (pPrShort->Y2orPinAttributes & RotationMask) // rotation determine draw way
    {
	case 0 : i1 =  1; kx = 1; ky = 0;    break; // rotate   0
	case 1 : i1 =  1; kx = 0; ky = 1;    break; // rotate  90
	case 2 : i1 = -1; kx = 1; ky = 0;    break; // rotate 180
	case 3 : i1 = -1; kx = 0; ky = 1;    break; // rotate 270
    }
    if (pPrShort->Y2orPinAttributes & DotMask) // draw dot symbol
    {
	fprintf(OUTPUT,"V %d %d %d 6 %d 0 0 -1 -1 0 -1 -1 -1 -1 -1\n",
		x1+kx*i1*DotSymDia/2, y1+ky*i1*DotSymDia/2, DotSymDia/2,
		(SetLineWidth) ? DotSymLine : 0);
	DotPlace = DotSymDia; // correct pin length
    }
    if (pPrShort->Y2orPinAttributes & ClockMask) // draw clock symbol
    {
	fprintf(OUTPUT,"L %d %d %d %d 3 %d 0 0 -1 -1\n",
		x1-kx*i1*ClkSymWidth, y1-ky*i1*ClkSymWidth,
		x1+ky*ClkSymHigh/2,  y1+kx*ClkSymHigh/2,
		(SetLineWidth) ? ClkSymLine : 0);
	fprintf(OUTPUT,"L %d %d %d %d 3 %d 0 0 -1 -1\n",
		x1-kx*i1*ClkSymWidth, y1-ky*i1*ClkSymWidth,
		x1-ky*ClkSymHigh/2,  y1-kx*ClkSymHigh/2,
		(SetLineWidth) ? ClkSymLine : 0);
    }

    PinLength = (SetScalePins) ?	    // select pin length
	XYmultip*pPrShort->X2orPinLength :  //   scale length as graphics
	FixedPinLength;			    //   fixed length
    x2 = x1+kx*i1*PinLength; x1 = x1+kx*i1*DotPlace;
    y2 = y1+ky*i1*PinLength; y1 = y1+ky*i1*DotPlace;

    fprintf(OUTPUT, "P %d %d %d %d 1 0 1\n", x1, y1, x2, y2 ); // draw pin line

    fprintf(OUTPUT, "{\n"); // open pin attributes declaration

    WritePinNumber(pPrShort);	    // write pin attributes
    WritePinLabel(pPrShort);	    //  --''--
    WritePinType(pPrShort);	    //  --''--
    WritePinSeq(pPrShort, PinSeq);  //  --''--

    fprintf(OUTPUT, "}\n"); // close pin attributes declaration

    // Y coordinate correction for write text attributes
    if (MaxY < y2) MaxY = y2;
    return 0;
}

int DrawImage(PRIMITIVE * pPrShort) // write image declaration to file (not embedded)
{
    /* gEDA picture object fields (taken from schematic file):
     * field: type x1 y1 width height angle ratio mirrored
     * value:  G                        0     0      0
     */
    int X = XYmultip*((pPrShort->X1orCenterX < pPrShort->X2orPinLength) ?
	    pPrShort->X1orCenterX : pPrShort->X2orPinLength);
    int Y = XYmultip*((pPrShort->Y1orCenterY < pPrShort->Y2orPinAttributes) ?
	    pPrShort->Y1orCenterY : pPrShort->Y2orPinAttributes);
    int Width  = XYmultip*abs(pPrShort->X1orCenterX - pPrShort->X2orPinLength);
    int Height = XYmultip*abs(pPrShort->Y1orCenterY - pPrShort->Y2orPinAttributes);

    fprintf(OUTPUT, "G %d %d %d %d 0 0 0\n", X, Y, Width, Height);
    fprintf(OUTPUT, "%s\n",pPrShort->TextString);
    if (pPrShort->Filled) DrawRectangle(pPrShort); // draw image border
    // Y coordinate correction for write text attributes
    if (MaxY < Y) MaxY = Y;
    return 0;
}

int DrawLine(PRIMITIVE * pPrShort) // write Line declaration to file
{
    // gEDA line object fields:
    // field:type x1 y1 x2 y2 color width capstyle dashstyle dashlength dashspace
    // value:  L		3	      0		0	 -1        -1	

    int x1 = XYmultip*(pPrShort->X1orCenterX);
    int y1 = XYmultip*(pPrShort->Y1orCenterY);
    int x2 = XYmultip*(pPrShort->X2orPinLength);
    int y2 = XYmultip*(pPrShort->Y2orPinAttributes);
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;

    fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n", x1, y1, x2, y2, LineWidth);
    // Y coordinate correction for write text attributes
    if (MaxY < y1) MaxY = y1; if (MaxY < y2) MaxY = y2;
    return 0;
}

int DrawPolygon(PRIMITIVE * pPrShort, char Closed) // write Polygon or Polyline
{
    /* Polygon or Polyline are draw as lines
     *
     * var. Closed=(1 for polygon | 0 for polyline)
     * line type for polyline is ignored
     *
     * gEDA line object fields:
     * field: type x1 y1 x2 y2 color width capstyle dashstyle dashlength dashspace
     * value:	L		3	      0		0	 -1        -1	
     */
    int x1 = XYmultip*(pPrShort->X1orCenterX);
    int y1 = XYmultip*(pPrShort->Y1orCenterY);
    int x2 = XYmultip*(pPrShort->X2orPinLength);
    int y2 = XYmultip*(pPrShort->Y2orPinAttributes);
    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;
    int PointNr;

    // Y coordinate correction for write text attributes
    if (MaxY < y1) MaxY = y1; if (MaxY < y2) MaxY = y2;

    // write first line
    fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
	    x1, y1, x2, y2, LineWidth);

    // write next lines
    if (pPrShort->pPoints == NULL) return 0; // no other lines
    for (PointNr = 0; PointNr < pPrShort->pPoints->NumPoints; PointNr++)
    {
	x1 = x2;
	x2 = XYmultip*(pPrShort->pPoints->pPoint[PointNr]->x);
	y1 = y2;
	y2 = XYmultip*(pPrShort->pPoints->pPoint[PointNr]->y);

	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		x1, y1, x2, y2, LineWidth);

	// Y coordinate correction for write text attributes
	if (MaxY < y1) MaxY = y1; if (MaxY < y2) MaxY = y2;
    }

    // write closing line
    if (Closed) // polygon only
    {
	x1 = XYmultip*(pPrShort->X1orCenterX);
	y1 = XYmultip*(pPrShort->Y1orCenterY);
	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		x1, y1, x2, y2, LineWidth);
    }
    return 0;
}

int DrawBezier(PRIMITIVE * pPrShort) // write Bezier curve
{
    /* Bezier are aproximated by lines
     *
     * Pro*el draw cubic curves type,
     *  for more then 4 points end point of curve
     *  is start point for next curve etc.
     *
     * http://en.wikipedia.org/wiki/B%C3%A9zier_curve 
     *	P = P0*(1-t)^3 + P1*3*t*(1-t)^2 + P2*3*t^2*(1-t) + P3*t^3
     *
     *	    P0..P3 - X or Y from control point; 0 <= t <= 1
     *	    P      - X or Y from curve point
     * ---------------------------------------------------------
     *  simplify:
     *	    T0 = (1-t)^3     \
     *	    T1 = 3*t*(1-t)^2  \  P = P0*T0 + P1*T1 + P2*T2 + P3*T3 
     *	    T2 = 3*t^2*(1-t)  /
     *	    T3 = t^3	     /
     *
     * gEDA line object fields:
     * field: type x1 y1 x2 y2 color width capstyle dashstyle dashlength dashspace
     * value:	L		3	      0		0	 -1        -1	
     */

    int LineWidth = (SetLineWidth) ? ChooseLineWidth(pPrShort) : 0;
    int x1,y1,x2,y2;			// x,y lines coordinates
    float T,Tm, T0,T1,T2,T3;		// simplify equations
    POINT P0, P1, P2, P3;		// Bezier control points
    int PtNr;				// current drawed point (0 for real 3th point)
    unsigned char LineNr;		// current drawed line

    if (pPrShort->RadiusXorNrPoints == 2) // it's line, draw line
	return DrawLine(pPrShort);
    
    // aproximate first Bezier curve
    P0.x = XYmultip*(pPrShort->X1orCenterX);
    P0.y = XYmultip*(pPrShort->Y1orCenterY);
    P1.x = XYmultip*(pPrShort->X2orPinLength);
    P1.y = XYmultip*(pPrShort->Y2orPinAttributes);
    PtNr = 0;
    P2.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr]->x);
    P2.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr]->y);
    PtNr = 1;
    P3.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr]->x);
    P3.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr]->y);

    x2 = P0.x; y2 = P0.y;
    for (LineNr = 1; LineNr <= BezierLines ; LineNr++)
    {
	T = (float)LineNr/(float)BezierLines;
	x1 = x2; y1 = y2;
	Tm = 1-T; T0 = Tm*Tm*Tm; T1 = 3*T*Tm*Tm; T2 = 3*T*T*Tm; T3 = T*T*T;
	x2 = (int)(P0.x*T0 + P1.x*T1 + P2.x*T2 + P3.x*T3);
	y2 = (int)(P0.y*T0 + P1.y*T1 + P2.y*T2 + P3.y*T3);

	// Y coordinate correction for write text attributes
	if (MaxY < y1) MaxY = y1; if (MaxY < y2) MaxY = y2;

	fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		x1, y1, x2, y2, LineWidth);
    }
    if (pPrShort->pPoints->NumPoints == 2)  // no more curves
	return 0;

    // aproximate next Bezier curves
    for (PtNr = 1; (PtNr+3) < pPrShort->pPoints->NumPoints; PtNr += 3)
    {
	P0.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr+0]->x);
	P0.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr+0]->y);
	P1.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr+1]->x);
	P1.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr+1]->y);
	P2.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr+2]->x);
	P2.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr+2]->y);
	P3.x = XYmultip*(pPrShort->pPoints->pPoint[PtNr+3]->x);
	P3.y = XYmultip*(pPrShort->pPoints->pPoint[PtNr+3]->y);
	
	// identical code as for 1st line
	x2 = P0.x; y2 = P0.y;
	for (LineNr = 1; LineNr <= BezierLines ; LineNr++)
	{
	    T = (float)LineNr/(float)BezierLines;
	    x1 = x2; y1 = y2;
	    Tm = 1-T; T0 = Tm*Tm*Tm; T1 = 3*T*Tm*Tm; T2 = 3*T*T*Tm; T3 = T*T*T;
	    x2 = (int)(P0.x*T0 + P1.x*T1 + P2.x*T2 + P3.x*T3);
	    y2 = (int)(P0.y*T0 + P1.y*T1 + P2.y*T2 + P3.y*T3);

	    // Y coordinate correction for write text attributes
	    if (MaxY < y1) MaxY = y1; if (MaxY < y2) MaxY = y2;

	    fprintf(OUTPUT, "L %d %d %d %d 3 %d 0 0 -1 -1\n",
		    x1, y1, x2, y2, LineWidth);
	}
    } 
    return 0;
}
// draw components primitives & attributes procedures END

void WriteCompAttrib(int CompNr, char SlotNr) // draw component attributes text
{
    /* gEDA text object fields:
     *  field:		value:
     *  type		T
     *  x
     *  y
     *  color		5 ( 8 for refdes)
     *  size
     *  visibility	0 ( 1 for refdes and value)
     *  show_name_value 0 ( 1 for refdes and value)
     *  angle		0
     *  alignment	0
     *  num_lines	1
     */


    int Y = MaxY;
    char Visible;
    char **pExtraAttr = ExtraAttr; // pointer to extra attributes string

    if (WrValue)	// write value
    {
	Y += AttrVoffset;
	fprintf(OUTPUT, "T %d %d 5 %d 1 1 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
	fprintf(OUTPUT, "value=%s\n", pPfWLib->pLibV1->pComps->pComp[CompNr]->ComponentName1);
    }

    // write device name (component name 1) (obligatory in gEDA)
    Y += AttrVoffset;
    fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
    fprintf(OUTPUT, "device=%s\n", pPfWLib->pLibV1->pComps->pComp[CompNr]->ComponentName1);

    // write default designator (obligatory in gEDA)
    Y += AttrVoffset;
    fprintf(OUTPUT, "T %d %d 8 %d 1 1 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
    (* pPfWLib->pLibV1->pComps->pComp[CompNr]->Default_Designator) ?
	fprintf(OUTPUT, "refdes=%s\n", pPfWLib->pLibV1->pComps->pComp[CompNr]->Default_Designator):
	fprintf(OUTPUT, "refdes=refdes %s\n", TxtNotDeclared);

    // write footprint (footprint 1) (obligatory in gEDA)
    Y += AttrVoffset;
    fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
    (* pPfWLib->pLibV1->pComps->pComp[CompNr]->Footprint_1) ?
	fprintf(OUTPUT, "footprint=%s\n", pPfWLib->pLibV1->pComps->pComp[CompNr]->Footprint_1):
	fprintf(OUTPUT, "footprint=%s\n", TxtNotDeclared);

    // write description (obligatory in gEDA)
    Y += AttrVoffset;
    fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
    (* pPfWLib->pLibV1->pComps->pComp[CompNr]->Description) ?
	fprintf(OUTPUT, "description=%s\n", pPfWLib->pLibV1->pComps->pComp[CompNr]->Description):
	fprintf(OUTPUT, "description=%s\n", TxtNotDeclared);

    // write numslots (obligatory in gEDA)
    Y += AttrVoffset;
    fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
    fprintf(OUTPUT, "numslots=%d\n", SlotNr);


#define	WrFootprint(NR)								    \
    if (WrFootpr##NR) {								    \
	Y += AttrVoffset;							    \
	fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);   \
	(* pPfWLib->pLibV1->pComps->pComp[CompNr]->Footprint_##NR) ?		    \
	fprintf(OUTPUT, "footprint%d=%s\n", NR,					    \
		pPfWLib->pLibV1->pComps->pComp[CompNr]->Footprint_##NR)	:	    \
	fprintf(OUTPUT, "footprint%d=%s\n", NR, TxtNotDeclared);}
#define	WrField(NR)								    \
    if (WrField##NR) {								    \
	Y += AttrVoffset;							    \
	fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);   \
	(* pPfWLib->pLibV1->pComps->pComp[CompNr]->Field_##NR) ?		    \
	fprintf(OUTPUT, "field%d=%s\n", NR,					    \
		pPfWLib->pLibV1->pComps->pComp[CompNr]->Field_##NR) :		    \
	fprintf(OUTPUT, "field%d=%s\n", NR, TxtNotDeclared);}
#define	WrPrField(NR)								    \
    if (WrPrField##NR) {							    \
	Y += AttrVoffset;							    \
	fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);   \
	(* pPfWLib->pLibV2->pCompsV2->pCompV2[CompNr]->PartField_##NR) ?	    \
	fprintf(OUTPUT, "partfield%d=%s\n", NR,					    \
		pPfWLib->pLibV2->pCompsV2->pCompV2[CompNr]->PartField_##NR):	    \
	fprintf(OUTPUT, "partfield%d=%s\n", NR, TxtNotDeclared);}

    WrFootprint(2); // write footprint 2
    WrFootprint(3); // write footprint 3
    WrFootprint(4); // write footprint 4
    WrField(1);	    // write field 1
    WrField(2);	    // write field 2
    WrField(3);     // write field 3
    WrField(4);     // write field 4
    WrField(5);     // write field 5
    WrField(6);     // write field 6
    WrField(7);     // write field 7
    WrField(8);     // write field 8
    WrPrField(1);   // write part field  1
    WrPrField(2);   // write part field  2
    WrPrField(3);   // write part field  3
    WrPrField(4);   // write part field  4
    WrPrField(5);   // write part field  5
    WrPrField(6);   // write part field  6
    WrPrField(7);   // write part field  7
    WrPrField(8);   // write part field  8
    WrPrField(9);   // write part field  9
    WrPrField(10);  // write part field 10
    WrPrField(11);  // write part field 11
    WrPrField(12);  // write part field 12
    WrPrField(13);  // write part field 13
    WrPrField(14);  // write part field 14
    WrPrField(15);  // write part field 15
    WrPrField(16);  // write part field 16

    while (*pExtraAttr != 0)	// write extra attributes
    {
	Y += AttrVoffset;
	fprintf(OUTPUT, "T %d %d 5 %d 0 0 0 0 1\n", AttrHoffset, Y, AttrTxtSize);
	fprintf(OUTPUT,"%s\n", *pExtraAttr);
	++pExtraAttr;
    }

    MaxY = Y; // update maximum used to write Y value
}

int WritePart2file(int CompNr, int PartNr) // write one part of component to file
{
    int PrimNr;		    // primitive number
    PRIMITIVE * pPrShort;   // short link to primitive
    char * CompName = pPfWLib->pLibV1->pComps->pComp[CompNr]->ComponentName1;
    char PinSeq = 0;	    // pin sequence number (incremented sequentially)
    char SlotNr;	    // real part number for multipart or 0
    char CompFileName[CompFileNameLength+9];	// symbol file name (name+_(1)+numslots(3)+ext(4)+null(1))
    const char CompFileExt[] = {".sym"};	// symbol file extension
    const char BadFileNameChar[] = {"'@\\/;:*?<>|[] "};
    char i,j;
    int RealNrParts = pPfWLib->pLibV1->pComps->pComp[CompNr]->pParts->RealNumParts;

    // remove incorrect characters from filename
    strncpy(CompFileName, CompName, CompFileNameLength);
    CompFileName[CompFileNameLength] = 0; // write "end string" character
    for (i = 0; CompFileName[i] != 0; ++i)
    {
	for (j = 0; BadFileNameChar[j] != 0; ++j)
	    if (CompFileName[i] == BadFileNameChar[j])
		CompFileName[i] = '_';
    }

    /* create new component file
     * if PartNr=0 file name will have no number after component name
     */
    if (RealNrParts == 1)   // one part only (none part number in name)
    {
	SlotNr = 0;
	sprintf(CompFileName, "%s%s", CompFileName, CompFileExt);
    }
    else		    // multipart 
    {       
	SlotNr = PartNr+1;
	if (RealNrParts < 10)	    // part number has 1 digit
	    sprintf(CompFileName, "%s_%01d%s", CompFileName, SlotNr, CompFileExt);
	else if (RealNrParts < 100) // part number has 2 digits
	    sprintf(CompFileName, "%s_%02d%s", CompFileName, SlotNr, CompFileExt);
	else			    // part number has 3 digits
	    sprintf(CompFileName, "%s_%03d%s", CompFileName, SlotNr, CompFileExt);
    }

    fprintf(stderr, "write component nr %3d: %s part: %d to file: %s\n",
	    CompNr+1, CompName, SlotNr, CompFileName);

#ifdef TOFILE
    OutFile = fopen(CompFileName, "w");	// create component file
    if (OutFile == 0)
    {
	fprintf(stderr, "FILE OPEN ERROR: %s\n", CompFileName);
	return 1;
    }
#endif

    // write header to component file
    fprintf(OUTPUT, "%s\n", GedaVersion);	

    // write primitives
    MaxY = 100;	// reset vertical coordinate for draw attributes
    for(PrimNr=0;PrimNr < pPfWLib->pLibV1->pComps->pComp[CompNr]->pParts->
	    pPartgrp[PartNr]->pNormal->NumPrimitives;PrimNr++)
    {
	pPrShort = pPfWLib->pLibV1->pComps->pComp[CompNr]->pParts->
	    pPartgrp[PartNr]->pNormal->pPr[PrimNr];
	if (pPrShort == NULL)	// invalid primitive (junction or symbol)
	{
	    fprintf(stderr,"  WARNING: unsuported primitive type\n");
	}
	else
	{
	    switch (pPrShort->Type) // select primitive type & write
	    {
		case Rectangle :	DrawRectangle(pPrShort);	break;
		case Ellipse :		DrawEllipse(pPrShort);		break;
		case Arc :		DrawArc(pPrShort);		break;
		case Pie :		DrawArc(pPrShort);		break; // draw as arc
		case EllipticalArc :	DrawEllipticalArc(pPrShort);	break;
		case RoundRectangle :   DrawRoundRect(pPrShort);	break;
		case Label :		WriteLabel(pPrShort);		break;
		case Pin :  ++PinSeq;	DrawPin(pPrShort, PinSeq);	break;
		case Image :		DrawImage(pPrShort);		break;
		case Line :		DrawLine(pPrShort);		break;
		case Polyline :		DrawPolygon(pPrShort, NO);	break;
		case Polygon :		DrawPolygon(pPrShort, YES);	break;
		case Bezier :		DrawBezier(pPrShort);		break;
		case Symbol :   fprintf(stderr, "  WARNING: symbol not supported\n");   break;
		case Junction : fprintf(stderr, "  WARNING: junction not supported\n"); break;
		case Invalid :  fprintf(stderr, "  WARNING: invalid primitive type\n"); break;
		default : fprintf(stderr, "  WARNING:",
				  " invalid primitive in component: %s part: %d\n",
				  pPfWLib->pLibV1->pComps->pComp[CompNr+1]->
				  ComponentName1, PartNr+1);
	    }
	}
    }
    WriteCompAttrib(CompNr, SlotNr);  // write component attributes

#ifdef TOFILE
    if (fclose(OutFile) != 0) // close component file
    {
	fprintf(stderr, "FILE CLOSE ERROR: %s\n", CompFileName);
	PfWSchLibFreeAllEx(&pPfWLib);
	return 1;
    }
#endif
    return 0;
}


int main(int argc,char* argv[]) // main
{
    char FileName[64];
    int CompNr, PartNr;
    int GetOptionsRet;

    /* set pointers to extra attribute to 0
     * value 0 in tables is signal if it is end of extra attributes parameters
     */
    memset(ExtraAttr, 0, sizeof(ExtraAttr));

    GetOptionsRet = GetOptions(argc, argv); // read options
    if (GetOptionsRet ==  1) Exit(1);	    // options error, exit
    if (GetOptionsRet == -1) 		    // no options, short help & exit
    {
	fprintf(stdout,
		"Pro*el to gEDA schema symbol converter %s\n"
		"  use %s [options] <PfWLibFile>\n"
		"  or  %s --help for help\n",
		FileVersion, argv[0],argv[0]);
	Exit(0);
    }
    if (GetOptionsRet == 'h') Exit('h');    // print help only & exit
    if (GetOptionsRet == 'l')		    // list library & exit
    {
	if (ParseFile(argc, argv) == 0) ListLibComponents();
       	Exit(0);
    }

    if (ParseFile(argc, argv)) Exit(1);    // parse file, if problem then exit

    // set first & last components numbers parameters
    if (FirstComp == -1) 	// no parameter for first component
	FirstComp = 1;
    if (LastComp == -1)		// no parameter for last component
	LastComp = pPfWLib->pLibV1->pComps->RealNumComp;
    if ((FirstComp < 1) || (FirstComp > LastComp) ||
	    (LastComp > pPfWLib->pLibV1->pComps->RealNumComp))
    {
	fprintf(stderr,"%s", TxtErrOptionsFirstLastComp);
	Exit(1);
    }

    --FirstComp; --LastComp; // tables start at 0 (no 1) correct this

    // write components to files
    for (CompNr = FirstComp; CompNr <= LastComp; ++CompNr)	// count component
    {
	for (PartNr = 0; PartNr < pPfWLib->pLibV1->pComps->	// count part
		pComp[CompNr]->pParts->RealNumParts; ++PartNr)
	    if (WritePart2file(CompNr, PartNr))
	    {
		fprintf(stderr, "ERROR write component: %d part: %d to file\n",
			CompNr+1, PartNr);
		Exit(1);
	    }
    }
    Exit(0);
} // main END
