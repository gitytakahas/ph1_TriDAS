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

#include <iostream>

#include "tscTypes.h"      // hash table of devices and PIA descriptions
#include "keyType.h"        // FEC/ring/CCU/channel/address are managed in this file (not a class)
#include "deviceType.h"

#include "FecExceptionHandler.h"    // exceptions
#include "FecFactory.h"
#include "FecDeviceFactory.h"
#include "TkRingDescription.h"

int main ( int argc, char **argv ) {

  if (argc<4) {
    std::cout << "Syntax: " << argv[0] << " ring.xml fecHardwareId ringId" << std::endl;
    return -1;
  }

  std::string inputFile, fecHardwareId;
  unsigned int ringId;
  FecDeviceFactory *deviceFactory = NULL ;
  TkRingDescription *myRing;
  

  inputFile = argv[1];
  fecHardwareId = argv[2];
  ringId = atoi(argv[3]);
  
  deviceFactory = new FecDeviceFactory ( ) ;
  myRing = deviceFactory->getFileRingDescription(inputFile, fecHardwareId, ringId);

  myRing->display(std::cout, true);
  std::cout << std::endl;
  
  if (myRing->hasDummyCcu()) {
    std::cout << "The ring has a dummy CCU";
    if (myRing->isDummyInverted()) {
      std::cout << " with ";
    } else {
      std::cout << " without ";
    }
    std::cout << "special inversion" << std::endl;
  } else {
    std::cout << "The ring has NO dummy CCU" << std::endl;
  }

  if (myRing->isReconfigurable()) {
    std::cout << "The ring is reconfigurable as asked" << std::endl;
    myRing->computeRedundancy();
  } else {
    std::cout << "The ring is NOT reconfigurable as asked for the following reason:" << std::endl;
    std::cout << myRing->getReconfigurationProblem() << std::endl;
  }

  myRing->display(std::cout, false, true);
  std::cout << std::endl;

  delete deviceFactory ;

  return 0 ;
}
