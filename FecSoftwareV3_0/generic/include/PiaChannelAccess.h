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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
  Mulhouse-France
*/
#ifndef PIACHANNELACCESS_H
#define PIACHANNELACCESS_H

#include "tscTypes.h"

#include "keyType.h"

#include "FecAccess.h"

/**
 * \class PiaChannelAccess
 * This class manage the key for a given specification on PIA channels. 
 * It is also an interface for the PIA accesses (reset)
 * \author Frederic Drouhin
 * \date Frebrary 2003
 * \brief Specifiation for PIA channels
 */
class PiaChannelAccess {

 protected:

  /** Access to the FEC hardware, the hardware access is done throught
   * the FecAccess class in order to manage the concurrent access to the devices
   * channels, rings, and FECs.
   */
  FecAccess *accessToFec_ ;

  /** key access
   */
  keyType accessKey_ ;

  /** Name of the device ... Fec:ring:ccu:channel:address
   */
  char decodedKey_[MAXCHARDECODEKEY] ;

  /** FEC hardware id
   */
  std::string fecHardwareId_ ;

  /** Crate slot
   */
  unsigned int crateId_ ;

  /** Private method to be called by the constructor
   * \param fec - hardware access to the fec
   * \param key - key that contains the path to the current device
   * \exception FecExceptionHandler
   */
  void initializeDeviceAccess ( FecAccess *fec, 
                                keyType key ) {

    accessToFec_ = fec ;

    accessKey_ = accessToFec_->addPiaAccess ( key, MODE_SHARE) ;

    decodeKey (decodedKey_, accessKey_) ;
  }

 public:

  /** \brief Constructor to store the access for this device (use when the database is not accessible)
   * \param fec - fec hardware access
   * \param Fec_slot - FEC slot
   * \param Ring_slot - Ring slot
   * \param Ccu_slot - CCU address
   * \param channelId - CCU channel
   * \exception FecExceptionHandler
   */
  PiaChannelAccess ( FecAccess *fec, 
                     tscType16 fecSlot,
                     tscType16 ringSlot,
                     tscType16 ccuAddress,
                     tscType16 piaChannel ) {

    initializeDeviceAccess (fec, buildCompleteKey (fecSlot, ringSlot, ccuAddress, piaChannel, 0x0)) ;
          
  }

  /** \brief Constructor to store the access for this device (use when the database is not accessible)
   * \param fec - fec hardware access
   * \param key - key of the channel
   * \exception FecExceptionHandler
   */
  PiaChannelAccess ( FecAccess *fec, 
                 keyType key ) {

    initializeDeviceAccess (fec, key) ;
  }


  /** \brief Remove the access
   * \exception FecExceptionHandler
   */
  ~PiaChannelAccess ( ) {

    accessToFec_->removePiaAccess ( accessKey_ ) ;

  }

  /** \brief return the name in term of FEC:Ring:CCU:Channel:Address
   * \return name of the device
   */
  char *getDecodedKey ( ) {

    return (decodedKey_) ;
  }

  /** \brief set the FEC hardware ID
   */
  void setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) {

    fecHardwareId_ = fecHardwareId ;
    crateId_ = crateId ;
  }

  /** \brief set the FEC hardware ID
   */
  std::string getFecHardwareId ( ) {

    return (fecHardwareId_) ;
  }

  /** \brief set the FEC hardware ID
   */
  void setCrateId ( tscType16 crateId ) {

    crateId_ = crateId ;
  }

  /** \brief set the FEC hardware ID
   */
  tscType16 getCrateId ( ) {

    return (crateId_) ;
  }

  /** \brief Get a key in order to retreive this device
   * \return Unique key for this device
   */
  keyType getKey ( ) {

    return (accessKey_) ;
  }

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getFecSlot ( ) {return (getFecKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getRingSlot ( ) {return (getRingKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getCcuAddress ( ) {return (getCcuKey(accessKey_)) ;}

  /** \brief Tools for the database, return the value corresponding to the function
   * \return Value set for this attribut
   */
  tscType16 getChannelId ( ) {return (getChannelKey(accessKey_)) ;}

};

#endif
