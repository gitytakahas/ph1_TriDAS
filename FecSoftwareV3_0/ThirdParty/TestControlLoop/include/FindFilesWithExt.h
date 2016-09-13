// FindFilesWithExt
// Distribué par http://www.developpez.com
// Auteur : Ricky81 ( http://ricky81.developpez.com )
//
//---------------------------------------------------------------------------
#ifndef FindFilesWithExtH
#define FindFilesWithExtH

#include <classes.hpp>
/*---------------------------------------------------------------------------
// Liste les fichiers d'extension spécifiée, dans le répertoire indiqué,
// et dans tous ses sous-répertoires si SousRep=true
---------------------------------------------------------------------------*/
void  __fastcall ListerFichiers(TStringList *liste, AnsiString NomRep,
   AnsiString extension, bool SousRep);

#endif
