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

// testprogram for PfWSchLibParser

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "PfWSchLibStruct.h"
#include "PfWSchLibParser.h"

#ifdef _DEBUG
#include "dmalloc.h"
#endif

int NotifyCallback(int nErrCode,char* message)
{
  fprintf(stderr,"%s\n",message);
}

int main(int argc,char* argv[])
{
  int i;
  char FileName[64];
  PFWSCHLIBFILE * pPfWSchLib=NULL;
  FILE *OutFile;
  if(argc<2)
  {
    printf("U¿ycie %s <PfWSchLibFile>\n",argv[0]);
    return 1;
  }
  printf("Parsing...\n");
  PfWSchLibSetCallbackFunction(NotifyCallback);
  printf("Parse 1\n");
  if(PfWSchLibLoadFileEx(argv[1],&pPfWSchLib)==0)
  {
    printf("Parse OK\n");
  }
  else
  {
    printf("Parse error\n");
    return 1;
  }
  PfWSchLibFreeAllEx(&pPfWSchLib);
  printf("Parse 2\n");
  if(PfWSchLibLoadFileEx(argv[1],&pPfWSchLib)==0)
  {
    printf("Parse OK\n");
  }
  else
  {
    printf("Parse error\n");
    return 1;
  }
  printf("Library name: %s\n",pPfWSchLib->LibraryName);
  printf("NumComponents: %d\n",pPfWSchLib->pLibV1->NumComponents);
  printf("RealNumComponents: %d\n",pPfWSchLib->pLibV1->pComps->RealNumComp);
  for(i=0;i<pPfWSchLib->pLibV1->pComps->RealNumComp;i++)
  {
    printf("Component: %d, name: %s \n",i+1,pPfWSchLib->pLibV1->pComps->pComp[i]->ComponentName1);

  }
  PfWSchLibFreeAllEx(&pPfWSchLib);
  return 0;
}
