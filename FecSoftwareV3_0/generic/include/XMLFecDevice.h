/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLFECDEVICE_H
#define XMLFECDEVICE_H

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "XMLFec.h"
//#include "FecDeviceMemParseHandlers.h"
#include "MemBufOutputSource.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class XMLFecDevice : public XMLFec {
 protected:
  //
  // private attributes
  //
  /**
   * Vector of deviceDescriptions
   */
  deviceVector dVector_;

  /** Parameter name's for the parsing of APV
   */
  parameterDescriptionNameType *apvParameterNames_ ;

  /** Parameter name's for the parsing of APV Mux
   */
  parameterDescriptionNameType *muxParameterNames_ ;

  /** Parameter name's for the parsing of DCU
   */
  parameterDescriptionNameType *dcuParameterNames_ ;

  /** Parameter name's for the parsing of laserdriver
   */
  parameterDescriptionNameType *laserdriverParameterNames_ ;

  /** Parameter name's for the parsing of PLL
   */
  parameterDescriptionNameType *pllParameterNames_ ;

#ifdef PRESHOWER
  /** Parameter name's for the parsing of delta
   */
  parameterDescriptionNameType *deltaParameterNames_ ;

  /** Parameter name's for the parsing of pace
   */
  parameterDescriptionNameType *paceParameterNames_ ;

  /** Parameter name's for the parsing of KCHIP
   */
  parameterDescriptionNameType *kchipParameterNames_ ;

  /** Parameter name's for the parsing of GOH
   */
  parameterDescriptionNameType *gohParameterNames_ ;
#endif
#ifdef TOTEM
  /** Parameter name's for the parsing of Vfat
   */
  parameterDescriptionNameType *vfatParameterNames_ ;

  /** Parameter name's for the parsing of cchip
   */
  parameterDescriptionNameType *totemCChipParameterNames_ ;

  /** Parameter name's for the parsing of totem BB
   */
  parameterDescriptionNameType *totemBBParameterNames_ ;
#endif

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLFecDevice () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  XMLFecDevice ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor whith database access
   */
  XMLFecDevice ( DbFecAccess *dbAccess ) throw (FecExceptionHandler);
#endif

  /** \brief Constructor with file access
   */
  XMLFecDevice ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the XMLFecDevice
   */
  virtual ~XMLFecDevice ();

  /** \brief initialise the parameter's names
   */
  virtual void initParameterNames ( ) ;

  /** \brief DOM parser for the FEC devices
   */
  virtual unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

  /** \brief Gets a pointer on the device vector private attribute
   */
  deviceVector getDeviceVector ( );

  /** \brief Sets the device vector private attribute
   */
  void setDeviceVector (deviceVector) throw (FecExceptionHandler);

  /** \brief Parse the inputSource and gets a pointer on the device vector private attribute
   */
  deviceVector getDevices ( ) throw (FecExceptionHandler);

  /** \brief clear the vector of devices
   */
  void clearVector();

#ifdef DATABASE
  /** \brief Gets a pointer on the device vector private attribute from database
   */
  deviceVector getDevices (std::string partitionName) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  deviceVector getDevices (std::string partitionName, unsigned int versionMajor, unsigned int versionMinor, unsigned int pMaskVersionMajor, unsigned int pMaskVersionMinor ) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  deviceVector getDevices (std::string partitionName, std::string fecHardId ) throw (FecExceptionHandler);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  deviceVector getDevices (std::string partitionName, std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler);
#endif

  /** \brief Writes the device vector parameter in a ASCII file
   */
  virtual void setDevices ( deviceVector dVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  virtual void setDevices ( std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Displays the contents of the device vector attribute
   */
  void display() throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Sends the parameter value from device vector to the database
   */
  void setDevices ( deviceVector dVector, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (FecExceptionHandler);

  /** \brief Sends the parameter value from device vector to the database
   */
  void setDevices ( std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId ) throw (FecExceptionHandler);

  /** \brief Sends the parameter value from device vector to the database
   */
  void setDevices ( deviceVector dVector, std::string partitionName, unsigned int versionUpdate ) throw (FecExceptionHandler);

  /** \brief Sends the parameter value from device vector to the database
   */
  void setDevices ( std::string partitionName, unsigned int versionUpdate ) throw (FecExceptionHandler);

  /** \brief Configures the database
   */
  unsigned int dbConfigure(std::string partitionName, boolean newPartition = true) throw (FecExceptionHandler);

  /** \brief Configures the database
   */
  //  unsigned int newDbConfigure(std::string partitionName, boolean newPartition = true) throw (FecExceptionHandler);
#endif

} ;

#endif
