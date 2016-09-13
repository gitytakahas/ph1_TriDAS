/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "FecExceptionHandler.h"    // exceptions
#include "DbTkDcuInfoAccess.h"

#include <fstream>

/**
 * Upload diag file to the database
 * -uploadDiagFile
 */
void uploadDiagFile ( DbTkDcuInfoAccess *dbAccess, std::string inputFile, std::string outputFile, bool display ) {

  // Open the file
  std::ifstream fichier(inputFile.c_str()) ;  
  
  // File existing ?
  if (!fichier) {
    std::cerr << "The file " << inputFile << " is not existing or empty" << std::endl ;
    return ;
  } 

  try {

    char coucou[1000] ;
    do {

      std::stringstream tobeuploaded ;
      tobeuploaded << "<ROWSET>" ;

      do {
	fichier.getline(coucou,1000) ;
	std::string completeLine = coucou ;
	
	if (!fichier.eof()) {
	  if (completeLine.find("<DATA>") != std::string::npos) {
	    tobeuploaded << "<RAWDIAGNOSTICERROR>" << std::endl ;
	  }
	  else {
	    if (completeLine.find("</DATA>") != std::string::npos) {
	      tobeuploaded << "</RAWDIAGNOSTICERROR>" << std::endl ;
	    }
	    else {
	      if (completeLine.find("<LEVEL>") != std::string::npos) {
		std::string::size_type ipass = completeLine.find("<LEVEL>");
		std::string::size_type ipath = completeLine.find("</LEVEL>");
		tobeuploaded << "\t<ERR_LEVEL>"
			     << completeLine.substr(ipass+7,ipath-ipass-7)
			   << "</ERR_LEVEL>"
			     << std::endl ;
	      }
	      else if (completeLine.find("<TRIGGER>") != std::string::npos) {
		std::string::size_type ipass = completeLine.find("<TRIGGER>");
		std::string::size_type ipath = completeLine.find("</TRIGGER>");
		tobeuploaded << "\t<ERR_TRIGGER>"
			     << completeLine.substr(ipass+7,ipath-ipass-7)
			     << "</ERR_TRIGGER>"
			     << std::endl ;
	      }
	      else {
		tobeuploaded << coucou << std::endl ;
	      }
	    }
	  }
	}
      }
      while ( (std::string(coucou) != "</DATA>") && (!fichier.eof()) ) ;

      tobeuploaded << "</ROWSET>" ;
      if (display) std::cout << tobeuploaded.str() << std::endl ;
      dbAccess->uploadDiagnoticXMLClob(tobeuploaded.str()) ;

      tobeuploaded.str("") ;


    } while (!fichier.eof()) ;
  }
  catch (FecExceptionHandler &e) {
    std::cerr << e.what() << std::endl ;
  }

  fichier.close() ;
}

/* ************************************************************************************************ */
/*                                                                                                  */
/*                                               Main                                               */
/*                                                                                                  */
/* ************************************************************************************************ */
int main ( int argc, char **argv ) {

  DbTkDcuInfoAccess *dbAccess = NULL ;
  std::string partitionName = "" ;
  bool display = false ;

  std::list<std::string> listFileName ;

  // *************************************************************************************************
  // Check the options
  int command = 0 ;
  std::string inputFile = "" ;
  std::string outputFile = "" ;
  for (int i = 1 ; i < argc ; i ++) {

    if (!strncasecmp(argv[i], "-input", strlen("-input"))) {

      i ++ ;
      if (i < argc) inputFile = std::string(argv[i]) ;
      else {
	std::cerr << "Error in usage, bad values for the option input, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp(argv[i], "-output", strlen("-output"))) {

      i ++ ;
      if (i < argc) outputFile = std::string(argv[i]) ;
      else {
	std::cerr << "Error in usage, bad values for the option output, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp (argv[i], "-partition", strlen("-partition"))) {

      i ++ ;
      if (i < argc) partitionName = argv[i] ;
      else {
	std::cerr << "Error in usage, bad values for the option partition, please type --help for help" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else if (!strncasecmp (argv[i], "-display", strlen("-display"))) {
      display = true ;
    }
    else if (!strncasecmp(argv[i], "-uploadDiagFile", strlen("-uploadDiagFile"))) command = 1 ;
    else if (!strncasecmp(argv[i], "-help", strlen("-help"))) command = 0 ;
    else if (!strncasecmp(argv[i], "--help", strlen("--help"))) command = 0 ;
    else std::cerr << "Invalid argument: " << argv[i] << std::endl ;
  }

  // ***************************************************************************************************
  // Create the database access
  // retreive the connection of the database through an environmental variable called CONFDB
  std::string login="nil", passwd="nil", path="nil" ;
  DbCommonAccess::getDbConfiguration (login, passwd, path) ;

  if (login != "nil" && passwd !="nil" && path != "nil") {

    try {
      // create the database access
      dbAccess = new DbTkDcuInfoAccess ( login, passwd, path ) ;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
  }
  else {
    std::cout << "Please specify a database" << std::endl ;
    return -1 ;
  }

  // *************************************************************************************************
  // Error in options or command
  int helpF = 0 ;
  if (command == 0) helpF = 1 ;
  if ((command == 1) && (inputFile == "") && (outputFile == "")) helpF = 2 ;

  if (helpF) {

    std::cerr << "Error in usage: wrong option or command (" << helpF << ")" << std::endl ;
    std::cerr << "\t" << argv[0] << " [-display] -input [file name | database] -output [file name | database] -uploadDiagFile" << std::endl ;
    std::cerr << "\t" << "-display: display buffer uploaded" << std::endl ;
    delete dbAccess ;
    exit (EXIT_FAILURE) ;
  }

  // *************************************************************************************************
  // Apply command
  switch (command) {
  case 1: // Upload diag file to database
    uploadDiagFile ( dbAccess, inputFile, outputFile, display ) ;
    break ;
  }

  if (dbAccess != NULL) delete dbAccess ;

  return 1 ;
}
