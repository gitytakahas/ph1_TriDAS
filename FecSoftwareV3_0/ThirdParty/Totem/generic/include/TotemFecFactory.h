/*
This file is part of Fec Software project.

author: Juha Petäjäjärvi
based on FecFactory by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef TOTEMFECFACTORY_H
#define TOTEMFECFACTORY_H

#include "TotemMemBufOutputSource.h"
#include "TotemXMLFecDevice.h"
#include "FecFactory.h"

/** This class manage all FEC descriptions through the class FecDeviceFactory
 */
class TotemFecFactory: public FecFactory {

 private:

  /** FEC devices vector
   */
  deviceVector vFecDevices_ ;

  /** Ring description
   */
  tkringVector tkRingDescription_ ;

 public:

  /** \brief Build a FEC factory both for pia and devices and create a database access if needed
   */
  TotemFecFactory ( ) ;


  /** \brief Disconnect the database (if it is set)
   */  
  ~TotemFecFactory ( ) ;


  /** \brief retreive the descriptions from the file
   */
  void getFecDeviceDescriptions ( deviceVector &outVector, std::string filename ) throw (FecExceptionHandler ) ;

  /** \brief Write descriptions into file 
   */
  void setFecDeviceDescriptions ( deviceVector devices, std::string filename ) throw (FecExceptionHandler ) ;

} ;

#endif
