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

#include "FecFactory.h"

int main ( int argc, char **argv ) {

  if (argc <= 3 || !strncmp(argv[1], "-help", strlen("-help"))) {
    
    std::cerr << "Error in usage " << argv[0] << " [Output File Name] [List of input File Name]" << std::endl ;
    std::cerr << "\tInstance are changed for each process, so the first input file will be instance 0, then the second file will instance 1, etc." << std::endl ;
    return -1 ;
  }
  
  deviceVector vDevices ;
  piaResetVector piaReset ;
  FecFactory fecFactory ;
  for (int i = 2 ; i < argc ; i ++) {

    // Retreive the vector from files for devices
    fecFactory.setInputFileName (argv[i]) ;
    deviceVector td, td1 ;
    fecFactory.getFecDeviceDescriptions ( td ) ;

    // Copy the vector of the files for devices
    FecFactory::vectorCopyI(td1, td) ;

    // Change the instance to 0 for the 1st, 1 for the second file, etc.
    // Add it to the previous one
    if (td1.size() > 0) {
      for ( deviceVector::iterator it = td1.begin() ; it != td1.end() ; it ++) {
	//(*it)->setInstance(i-2) ;
	vDevices.push_back((*it)) ;
      }
    }

    // Retreive the vector for the PIA
    piaResetVector tp, tp1 ;
    fecFactory.getPiaResetDescriptions ( tp ) ;

    // Copy the vector of the files for PIA resets
    FecFactory::vectorCopyI(tp1, tp) ;

    // Change the instance to 0 for the 1st, 1 for the second file, etc.
    // Add it to the previous one
    if (tp1.size() > 0) {
      for ( piaResetVector::iterator it = tp1.begin() ; it != tp1.end() ; it ++) {
	//(*it)->setInstance(i-2) ;
	piaReset.push_back((*it)) ;
      }
    }

    std::cout << "Found " << td1.size() << " devices and " << tp1.size() << " PIA rests, for the file " << argv[i] << " adding the instance " << (i-2) << std::endl ;
  }

  fecFactory.setOutputFileName (argv[1]) ;
  std::cout << "Upload " << vDevices.size() << " devices and " << piaReset.size()  << " PIA resets in file " << fecFactory.getOutputFileName() << std::endl ;

  fecFactory.setFecDevicePiaDescriptions (vDevices, piaReset) ;

  FecFactory::deleteVectorI (vDevices) ;
  FecFactory::deleteVectorI (piaReset) ;

  return 0 ;
}
