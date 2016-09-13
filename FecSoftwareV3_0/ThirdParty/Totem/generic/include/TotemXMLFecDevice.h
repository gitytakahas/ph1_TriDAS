/*
This file is part of Fec Software project.

author: Juha Petäjäjärvi
based on XMLFecDevice by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France

*/

#ifndef TOTEMXMLFECDEVICE_H
#define TOTEMXMLFECDEVICE_H

#ifdef DATABASE
#include "DbFecAccess.h"
#endif

#include "XMLFecDevice.h"
#include "TotemMemBufOutputSource.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class TotemXMLFecDevice : public XMLFecDevice {

 protected:

  /** Parameter name's for the parsing of Philips
   */
  parameterDescriptionNameType *philipsParameterNames_ ;

  /** Parameter name's for the parsing of Vfat
   */
  parameterDescriptionNameType *vfatParameterNames_ ;

  /** Parameter name's for the parsing of cchip
   */
  parameterDescriptionNameType *totemCChipParameterNames_ ;

  /** Parameter name's for the parsing of totem BB
   */
  parameterDescriptionNameType *totemBBParameterNames_ ;

 public:

  /** \brief Default constructor
   */
  TotemXMLFecDevice () throw (FecExceptionHandler);

  /** \brief Constructor with xml buffer
   */
  TotemXMLFecDevice ( const XMLByte* xmlBuffer ) throw (FecExceptionHandler);

#ifdef DATABASE
  /** \brief Constructor whith database access
   */
  TotemXMLFecDevice ( DbFecAccess *dbAccess ) throw (FecExceptionHandler);
#endif

  /** \brief Constructor with file access
   */
  TotemXMLFecDevice ( std::string xmlFileName ) throw (FecExceptionHandler);
  
  /** \brief Deletes the TotemXMLFecDevice
   */
  ~TotemXMLFecDevice ();
  
  /** \brief initalise the parameter's names
   */
  void initParameterNames();

  /** \brief Writes the device vector parameter in a ASCII file
   */
  void setDevices ( deviceVector dVector, std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  void setDevices ( std::string outputFileName ) throw (FecExceptionHandler);

  /** \brief DOM parser for the FEC devices
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

} ;

#endif
