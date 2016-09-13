/*
This file is part of Fec Software project.

author: Juha Petäjäjärvi
based on FecAccessManager by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef TOTEMFECACCESSMANAGER_H
#define TOTEMFECACCESSMANAGER_H

#include "TotemHashTable.h"
#include "FecAccessManager.h"
#include "vfatDescription.h"
#include "totemCChipDescription.h"
#include "totemBBDescription.h"

class TotemFecAccessManager : public FecAccessManager {

 protected:

  /** VFAT hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  vfatAccessedType vfatSet_ ;

  /** CCHIP hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  cchipAccessedType cchipSet_ ;

  /** TBB hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  tbbAccessedType tbbSet_ ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseVfat ( vfatDescription vfatDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseCChip ( totemCChipDescription cchipDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseTbb ( totemBBDescription tbbDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;


 public:

  /** \brief Initialisation
   */
  TotemFecAccessManager ( FecAccess *fecAccess, bool displayDebugMessage = false ) ;

  /** \brief Remove all the accesses
   */
  ~TotemFecAccessManager ( ) ;

  /** \brief Remove the connection for all the devices
   */
  void removeDevices ( ) ;

  /** \brief Remove the connection for one device type
   */
  void removeDevices ( enumDeviceType type ) ;

  /** \brief return all the accesses (map)
   */
  vfatAccessedType &getVfatAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  cchipAccessedType &getCchipAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  tbbAccessedType &getTbbAccesses ( ) ;

  /** \brief return an access from a hash table
   */
  deviceAccess *getAccess ( enumDeviceType deviceType, keyType index ) ;

  /** \brief set an access into an hash table
   */
  void setAccess ( deviceAccess *access ) throw (FecExceptionHandler) ;

  /** \brief download the values
   */
  unsigned int downloadValues ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset = false, bool dohSet = true) throw (FecExceptionHandler) ;

  /** \brief download the values into the hardware through block of frames
   */
  unsigned int downloadValuesMultipleFrames ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset = false, bool dohSet = true) throw (FecExceptionHandler) ;


  /** \brief upload all the values from the hardware
   */
  deviceVector *uploadValues ( std::list<FecExceptionHandler *> &errorList, bool comparison = false, bool dcuUpload = true, bool dohSet = true ) ;

  /** \brief upload all the values from the hardware through block of frames
   */
  deviceVector *uploadValuesMultipleFrames ( std::list<FecExceptionHandler *> &errorList, bool comparison = false, bool dcuUpload = true, bool dohSet = true ) ;

  unsigned int resetPia ( piaResetVector *vPiaReset, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) ;

} ;


#endif
