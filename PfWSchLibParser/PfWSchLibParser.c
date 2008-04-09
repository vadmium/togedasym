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

// PfWSchLibparser.c : Defines the entry points for the library
//


#include <stdlib.h>
#include <string.h>

#include "PfWSchLibStruct.h"
#include "PfWSchLibParser.h"

#include "FlexBisonIface.h"

#ifdef _DEBUG
#include "dmalloc.h"
#endif

int (*gpFnNotifyCallback)(int,char*)=NULL;

/////////////////////////////////////////////////////////////////////////////
//  NotifyCallback - zwraca informacje o b≥Íedach w produkcji
//
int LibraryNotifyCallback(int nErrCode,char* message)
{
  if(gpFnNotifyCallback!=NULL)
    return gpFnNotifyCallback(nErrCode,message);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
//  PfWSchLibLoadFileEx - analizuje plik konfiguracyjny,
//  zapisuje dane do zewnÍtrznego wskaünika
//
int PfWSchLibLoadFileEx(const char *PfWSchLibFileName,PFWSCHLIBFILE **ppLocPfWSchLib)
{
  int result;
  if(*ppLocPfWSchLib!=NULL)
  {
    return -1;
  }
  result=ParsePfWSchLib((char*)PfWSchLibFileName,ppLocPfWSchLib);
  if (result!=0)
  {
    PfWSchLibFreeAllEx(ppLocPfWSchLib);
    return result;
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//  PfWSchLibLoadFileEx - kasuje dane
//
void PfWSchLibFreeAllEx(PFWSCHLIBFILE **ppLocPfWSchLib)
{
  int i,j,k,l;
  PFWSCHLIBFILE  *pLocPfWSchLib;
  if(ppLocPfWSchLib==NULL)
    return;
  pLocPfWSchLib=*ppLocPfWSchLib;
  if(pLocPfWSchLib==NULL)
    return;
  free(pLocPfWSchLib->LibraryName);
  if(pLocPfWSchLib->pFontTable!=NULL)
  {
    for(i=0;i<pLocPfWSchLib->pFontTable->pFonts->RealNumFonts;i++)
    {
      if(pLocPfWSchLib->pFontTable->pFonts->pFont[i]->Name!=NULL)
        free(pLocPfWSchLib->pFontTable->pFonts->pFont[i]->Name);
      free(pLocPfWSchLib->pFontTable->pFonts->pFont[i]);
    }
    free(pLocPfWSchLib->pFontTable->pFonts->pFont);
    free(pLocPfWSchLib->pFontTable->pFonts);
    free(pLocPfWSchLib->pFontTable);
  }
  if(pLocPfWSchLib->pLibV1!=NULL)
  {
    for(i=0;i<pLocPfWSchLib->pLibV1->pComps->RealNumComp;i++)
    {
      if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts!=NULL)
      {
        for(j=0;j<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->RealNumParts;j++)
        {
          if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal!=NULL)
          {
            if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr!=NULL)
            {
              for(k=0;k<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->NumPrimitives;k++)
              {
                if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]!=NULL)
                {
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->TextString!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->TextString);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->PinNr!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->PinNr);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->pPoints!=NULL)
                  {
                    for(l=0;l<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->pPoints->NumPoints;l++)
                    {
                      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->pPoints->pPoint[l]);
                    }
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->pPoints->pPoint);
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]->pPoints);
                  }
                  free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr[k]);
                }
              }
              free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal->pPr);
            }
            free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pNormal);
          }
          if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan!=NULL)
          {
            if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr!=NULL)
            {
              for(k=0;k<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->NumPrimitives;k++)
              {
                if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]!=NULL)
                {
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->TextString!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->TextString);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->PinNr!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->PinNr);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->pPoints!=NULL)
                  {
                    for(l=0;l<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->pPoints->NumPoints;l++)
                    {
                      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->pPoints->pPoint[l]);
                    }
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->pPoints->pPoint);
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]->pPoints);
                  }
                  free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr[k]);
                }
              }
              free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan->pPr);
            }
            free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pDeMorgan);
          }
          if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE!=NULL)
          {
            if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr!=NULL)
            {
              for(k=0;k<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->NumPrimitives;k++)
              {
                if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]!=NULL)
                {
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->TextString!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->TextString);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->PinNr!=NULL)
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->PinNr);
                  if(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->pPoints!=NULL)
                  {
                    for(l=0;l<pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->pPoints->NumPoints;l++)
                    {
                      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->pPoints->pPoint[l]);
                    }
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->pPoints->pPoint);
                    free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]->pPoints);
                  }
                  free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr[k]);
                }
              }
              free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE->pPr);
            }
            free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]->pIEEE);
          }
          free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp[j]);
        }
        free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts->pPartgrp);
        free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->pParts);
      }
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Description);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Footprint_1);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Footprint_2);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Footprint_3);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Footprint_4);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_1);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_2);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_3);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_4);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_5);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_6);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_7);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Field_8);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Default_Designator);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->Sheet_Part_Filename);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->ComponentName1);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]->ComponentName2);
      free(pLocPfWSchLib->pLibV1->pComps->pComp[i]);
    }
    free(pLocPfWSchLib->pLibV1->pComps->pComp);
    free(pLocPfWSchLib->pLibV1->pComps);
    free(pLocPfWSchLib->pLibV1);
  }
  if(pLocPfWSchLib->pLibV2!=NULL)
  {
    for(i=0;i<pLocPfWSchLib->pLibV2->pCompsV2->RealNumCompV2;i++)
    {
      if(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]!=NULL)
      {
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]-> COMPONENT_2);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_1);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_2);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_3);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_4);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_5);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_6);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_7);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_8);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_9);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_10);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_11);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_12);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_13);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_14);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_15);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]->PartField_16);
        free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2[i]);
      }
    }
    free(pLocPfWSchLib->pLibV2->pCompsV2->pCompV2);
    free(pLocPfWSchLib->pLibV2->pCompsV2);
    free(pLocPfWSchLib->pLibV2);
  }
  free(pLocPfWSchLib);
  pLocPfWSchLib=NULL;
  *ppLocPfWSchLib=NULL;
}

/////////////////////////////////////////////////////////////////////////////
//  PfWSchLibSetCallbackFunction - ustawia wskaünik na callback
//
void PfWSchLibSetCallbackFunction(int (*pFn)(int,char*))
{
  gpFnNotifyCallback=pFn;
}
