// FindFilesWithExt
// Distribué par http://www.developpez.com
// Auteur : Ricky81 ( http://ricky81.developpez.com )
//
//---------------------------------------------------------------------------
#include "FindFilesWithExt.h"
/*---------------------------------------------------------------------------
// Liste les fichiers d'extension spécifiée, dans le répertoire indiqué,
// et dans tous ses sous-répertoires si SousRep=true
---------------------------------------------------------------------------*/
void  __fastcall ListerFichiers(TStringList *liste, AnsiString NomRep,
   AnsiString extension, bool SousRep)
{
   TSearchRec Rec;
   int trouve;

   if (NomRep.SubString(NomRep.Length(),1) !="\\")
   {
      NomRep+="\\";
   }
   trouve = FindFirst(NomRep+"*."+extension, faAnyFile, Rec);
   while (trouve == 0)
   {
      liste->Add(NomRep+Rec.Name);
      if ((Rec.Attr && faDirectory >0) && (Rec.Name[1] != '.') && (SousRep == true))
      {
         ListerFichiers(liste, NomRep+Rec.Name, extension, true);
      }
      trouve = FindNext(Rec);
   }
   FindClose(Rec);
}
