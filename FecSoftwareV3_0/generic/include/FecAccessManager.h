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
#ifndef FECACCESSMANAGER_H
#define FECACCESSMANAGER_H

#include "keyType.h"

#include "apvDescription.h"
#include "pllDescription.h"
#include "laserdriverDescription.h"
#include "muxDescription.h"
#include "philipsDescription.h"
#include "dcuDescription.h"
#include "deviceType.h"

#ifdef PRESHOWER
// For CMS Preshower
#include "deltaDescription.h"
#include "paceDescription.h"
#include "kchipDescription.h"
#endif // PRESHOWER

#ifdef TOTEM
#include "vfatDescription.h"
#include "totemCChipDescription.h"
#include "totemBBDescription.h"
#endif // TOTEM

#include "FecAccess.h"
#include "deviceAccess.h"
#include "HashTable.h"

/**
 * \class FecAccessManager
 * This class define and manage all the accesses.
 * It download all the values issued from vector<deviceDescription> into the hardware and upload it.
 * \version 1.0
 * \author Frederic Drouhin
 * \date November 2002
 * \warning All the hash table use a key defined in the file keyType.h:
 * \include keyType.h
 */
class FecAccessManager {

 protected:

  /** Concurrent access to the hardware via the FecAccess class
   */
  FecAccess *fecAccess_ ;

  /** APV hash table. 
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  apvAccessedType apvSet_ ;

  /** PLL hash table.  
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  pllAccessedType pllSet_ ;

  /** Laserdriver hash table.
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  laserdriverAccessedType laserdriverSet_ ;

  /** Doh hash table.
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  dohAccessedType dohSet_ ;

  /** MUX hash table.
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  muxAccessedType muxSet_ ;

  /** Philips hash table.
   * <p>STL Map for each device based on a key build in keyType.h file.
   * \warning This map is used to test the program.
   */
  philipsAccessedType philipsSet_ ;

  /** DCU hash table. 
   * <p>STL Map for each device based on a key build in keyType.h file.
   */
  dcuAccessedType dcuSet_ ;

  /** PIA channel hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  piaAccessedType piaSet_ ;

#ifdef PRESHOWER
  /** DELTA hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  deltaAccessedType deltaSet_ ;

  /** PACEAM hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  paceAccessedType paceSet_ ;

  /** KCHIP hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  kchipAccessedType kchipSet_ ;

  /** GOH hash table
   * <p>STL Map for each channel based on a key build in keyType.h file.
   */
  gohAccessedType gohSet_ ;
#endif // PRESHOWER

#ifdef TOTEM
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
#endif // TOTEM

  /** Number of error during the last operation
   */
  unsigned int lastOperationNumberErrors_ ;

  /** Max number of errors allowed before the download stopped
   * After this number the download stop
   */
  unsigned int maxErrorAllowed_ ;

  /** To stop download or upload
   */
  bool haltStateMachine_ ;

  /** Display or not the debug message
   */
  bool displayDebugMessage_ ;
  
  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseApv ( apvDescription apvDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parsePll ( pllDescription pllDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true, bool pllReset = false ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseMux ( muxDescription muxDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseLaserdriver ( laserdriverDescription laserdriverDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseDoh ( laserdriverDescription dohDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parsePhilips ( philipsDescription philipsDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseDcu ( dcuDescription dcuDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

#ifdef PRESHOWER
  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseDelta ( deltaDescription deltaDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parsePace ( paceDescription paceDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseKchip ( kchipDescription kchipDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;

  /** \brief Retreive the information from the DOM node of the database and
   * download the values into the hardware
   */
  unsigned int parseGoh ( gohDescription gohDevice, std::list<FecExceptionHandler *> &errorList, bool setIt = true ) ;
#endif // PRESHOWER

#ifdef TOTEM
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
#endif // TOTEM

 public:

  /** \brief Initialisation
   */
  FecAccessManager ( FecAccess *fecAccess, bool displayDebugMessage = false ) ;

  /** \brief Remove all the accesses
   */
  virtual ~FecAccessManager ( ) ;

  /** \brief Remove the connection for all the devices
   */
  virtual void removeDevices ( ) ;

  /** \brief set the display debug message
   */
  void setDisplayDebugMessage ( bool displayDebugMessage ) ;

  /** \brief return if the display debug message is on or off
   */
  bool getDisplayDebugMessage ( ) ;

  /** \brief return the FEC access object 
   */
  FecAccess *getFecAccess ( ) ;

  /** \brief Remove the connection for one device type
   */
  virtual void removeDevices ( enumDeviceType type ) ;

  /** \brief return an access from a hash table
   */
  virtual deviceAccess *getAccess ( enumDeviceType deviceType, keyType index ) ;

  /** \brief return an access to the PIA reset
   */
  PiaResetAccess *getPiaAccess ( keyType index ) ;

  /** \brief return all the accesses (map)
   */
  dcuAccessedType &getDcuAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  pllAccessedType &getPllAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  apvAccessedType &getApvAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  laserdriverAccessedType &getLaserdriverAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  laserdriverAccessedType &getAOHAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  dohAccessedType &getDOHAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  muxAccessedType &getApvMuxAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  philipsAccessedType &getPhilipsAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  piaAccessedType &getPiaResetAccesses ( ) ;

#ifdef PRESHOWER

  /** \brief return all the accesses (map)
   */
  deltaAccessedType &getDeltaAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  paceAccessedType &getPaceAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  kchipAccessedType &getKchipAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  gohAccessedType &getGohAccesses ( ) ;

#endif // PRESHOWER
#ifdef TOTEM

  /** \brief return all the accesses (map)
   */
  vfatAccessedType &getVfatAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  cchipAccessedType &getCchipAccesses ( ) ;

  /** \brief return all the accesses (map)
   */
  tbbAccessedType &getTbbAccesses ( ) ;

#endif // TOTEM

  /** \brief set an access into an hash table
   */
  virtual void setAccess ( deviceAccess *access ) throw (FecExceptionHandler) ;

  /** \brief download the values
   */
  virtual unsigned int downloadValues ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset = false, bool dohSet = true) throw (FecExceptionHandler) ;

  /** \brief calibrate the DOH (find for a specific gain the bias values)
   */
  unsigned int calibrateDOH ( deviceVector &dohDevices, std::list<FecExceptionHandler *> &errorList, unsigned char gain = 1 ) ;

  /** \brief download the values into the hardware through block of frames
   */
  virtual unsigned int downloadValuesMultipleFrames ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset = false, bool dohSet = true) throw (FecExceptionHandler) ;

  /** \brief Download the values only for certain APV registers on all APVs
   */
  unsigned int downloadValuesMultipleFrames ( apvDescription apvValues, 
					      std::list<FecExceptionHandler *> &errorList,
					      bool apvModeF = true, bool latencyF = true, bool muxGainF = true,
					      bool ipreF = true,    bool ipcascF = true,  bool ipsfF = true,
					      bool ishaF = true,    bool issfF = true,    bool ipspF = true,
					      bool imuxinF = true,  bool icalF = true,    bool ispareF = false,
					      bool vfpF = true,     bool vfsF = true,     bool vpspF = true,
					      bool cdrvF = true,    bool cselF = true,    bool apvErrorF = false ) ;

  /** \brief Download the values for all the laserdrivers (AOH)
   */
  unsigned int downloadValuesMultipleFrames ( laserdriverDescription laserdriverValues, std::list<FecExceptionHandler *> &errorList  ) ;

  /** \brief Download the values for all the MUX
   */
  unsigned int downloadValuesMultipleFrames ( muxDescription muxValues, std::list<FecExceptionHandler *> &errorList ) ;

  /** \brief Download the values for all the PLL
   */
  unsigned int downloadValuesMultipleFrames ( pllDescription pllValues, std::list<FecExceptionHandler *> &errorList) ;

  /** \brief Download the values for all the PLL by adding a specific time
   */
  unsigned int downloadValuesMultipleFrames ( tscType8 delay, std::list<FecExceptionHandler *> &errorList ) ;

  /** \brief upload all the values from the hardware
   */
  virtual deviceVector *uploadValues ( std::list<FecExceptionHandler *> &errorList, bool comparison = false, bool dcuUpload = true, bool dohSet = true ) ;

  /** \brief upload all the values from the hardware through block of frames
   */
  virtual deviceVector *uploadValuesMultipleFrames ( std::list<FecExceptionHandler *> &errorList, bool comparison = false, bool dcuUpload = true, bool dohSet = true ) ;

  /** \brief upload all the DCU values from the hardware
   */
  unsigned int uploadValues ( deviceVector &dcuDevice, std::list<FecExceptionHandler *> &errorList, bool dcuHardIdOnly = false ) ;

  /** \brief upload all the DCU values from the hardware through block of frames
   */
  unsigned int uploadValuesMultipleFrames ( deviceVector &dcuDevice, std::list<FecExceptionHandler *> &errorList, bool dcuHardIdOnly = false ) ;

  /** \brief to interrupt an operation
   */
  void setHalt ( bool halt ) ;

  /** \brief return the halt state
   */
  bool getHalt ( ) ;

  /** \brief set the maximum number of allowed error
   */
  void setMaxErrorAllowed ( unsigned int maxErrorAllowed ) ;

  /** \brief get the maximum number of allowed error
   */
  unsigned int getMaxErrorAllowed ( ) ;

  /** \brief return the number of errors during the lastoperation
   */
  unsigned int getLastErrorLastOperation() ;

  /** \brief reset all modules by using PIA channel
   */
  unsigned int resetPiaModules ( piaResetVector *vPiaReset, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) ;

  /** \brief reset all modules by using PIA channel
   */
  unsigned int resetPiaModulesMultipleFrames ( piaResetVector *vPiaReset,  std::list<FecExceptionHandler *> &errorList) throw (FecExceptionHandler) ;

  /** \brief cold reset for PLL
   */
  unsigned int setColdPllReset ( std::list<FecExceptionHandler *> &errorList, bool multiFrames = false ) ;
} ;


#endif
