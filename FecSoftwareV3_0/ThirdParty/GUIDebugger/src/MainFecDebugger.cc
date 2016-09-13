/****************************************************************************
**
** Copyright (C) 1992-2000 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "string.h"

#include <qapplication.h>

#include "FecDialogImpl.h"

void help ( char **argv ) {

  std::cerr << argv[0] << ": error in usage, please specify bus type " << std::endl ;
  std::cerr << argv[0] << " [-pci | -vmesbs | -vmecaenpci | -vmecaenusb | -usb]" << std::endl ;
  std::cerr << "\t" << "The next optional parameter for database use only, you need to specify the partition name and, or the XDAQ instance or the FEC hardware ID" << std::endl ;
  std::cerr << "\t\t" << "[-p partition name]" << std::endl ;
  std::cerr << "\t\t" << "[-h FEC hardware ID]" << std::endl ;
}

int main( int argc, char **argv ) {

  if ((argc <= 1) || (!strncmp(argv[1],"-help",5))) {
    help (argv) ;
    return -1 ;
  }

  QApplication a( argc, argv );

  bool forceAck = true ;
  tscType16 i2cSpeed = 100 ;
  int cpt = 1 ;

  try {
    FecAccess *fecAccess = FecAccess::createFecAccess ( argc, argv, &cpt, false) ;
  
    if (!fecAccess) {
      std::cerr << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
      exit (EXIT_FAILURE) ; ; 
    }

    std::string partitionName = "nil" ;
    std::string fecHardwareId = "0" ;

    // Check the args of the methods
    for (int i = cpt ; i < argc ; i ++) {

      if (argv[i][0] == '-') {
	switch (argv[i][1]) {
	case 'f': // FEC hardware ID
	  i ++ ;
	  if (i >= argc) {
	    std::cerr << "Error in usage for instance, please specify a FEC hardware ID" << std::endl ;
	    help(argv) ;
	    exit (-1) ;
	  }
	  fecHardwareId = std::string(argv[i]) ;
	  break ;
	case 'p': // partition name
	  i ++ ;
	  if (i >= argc) {
	    std::cerr << "Error in usage for instance, please specify a FEC hardware ID" << std::endl ;
	    help(argv) ;
	    exit (-1) ;
	  }
	  partitionName = std::string(argv[i]) ;
	  break ; 
	}
      }
    }

    if (fecHardwareId == "0" && partitionName =="nil") {
      std::cerr << "Warning: no database access is possible with these parameters" << std::endl ;
    }
    
    //std::cout << "FEC bus type = " << fecBusType << " / " << FECVME << " / " << FECPCI << std::endl ;
    //std::cout << "Bus adapter = " << busAdapter << std::endl ;
    //std::cout << "VME file name = " << vmeFileName << std::endl ;
    //std::cout << "Partition name = " << partitionName << std::endl ;
    //std::cout << "FEC hardware ID = " << fecHardwareId << std::endl ;

    FecDialogImpl *control = new FecDialogImpl(fecAccess, partitionName, fecHardwareId);

    a.setMainWidget( control );
    control->show();
    return a.exec();
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "Error in creating the hardware access: " << e.what() << std::endl ;
  }

  return 0 ;
}
