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
#ifndef CONNECTIONDESCRIPTION_H
#define CONNECTIONDESCRIPTION_H

#include <string>

#include "keyType.h"
#include "deviceDescription.h"

/** Class for connection description
 */
class ConnectionDescription {

 private:

  /** FED crate ID
   */
  unsigned int fedCrateId_ ;

  /** FED slot
   */
  unsigned int fedSlot_ ;

  /** FED software ID
   */
  unsigned int fedId_ ;

  /** FED channel
   */
  unsigned int fedChannel_ ;

  /** VME FEC has identification number inside the hardware.
   */
  char fecHardwareId_[100] ;

  /** FEC crate slot
   */
  unsigned int fecCrateId_ ;

  /** FEC Slot
   */
  unsigned int fecSlot_ ;

  /** ring slot
   */
  unsigned int ringSlot_ ;

  /** CCU address
   */
  unsigned int ccuAddress_ ;

  /** I2C Channel
   */
  unsigned int i2cChannel_ ;

  /** APV address
   */
  unsigned int apvAddress_ ;

  /** DCU hardware ID
   */
  unsigned int dcuHardId_ ;

  /** Enabled connection
   */
  bool enabled_ ;

  /** DET ID for the given connection
   * \warning this information is coming the DCU info factory or directly from the DB
   */
  unsigned int detId_ ;

  /** Number of APVS for this module
   * \warning this information is coming the DCU info factory or directly from the DB
   */
  unsigned int nApvs_ ;

  /** Fiber length between the AOH and the FED in cm
   * \warning this information is coming the DCU info factory or directly from the DB
   */
  double fiberLength_ ;

 public:

  /** Enumeration to access the list of parameter's names
   */
  enum DeviceEnumType {FEDCRATEID,FEDSLOT,FEDID,FEDCHANNEL,FECHARDWAREID,FECCRATEID,FECSLOT,FECRING,CCUADDRESS,CCUCHANNEL,APVADDRESS,DCUHARDID,ENABLED} ;

  /** Parameter's names
   */
  static const char *CONNECTIONPARAMETERNAMES[ENABLED+1] ; 
  
  /** Constructor for ConnectionDescription
   * \param fedId - FED software ID
   * \param fedChannel -  FED channel
   * \param fecCrateId - crate Id
   * \param fecSlot - FEC Slot
   * \param ringSlot - ring slot
   * \param ccuAddress - CCU address
   * \param i2cChannel - I2C Channel
   * \param apvAddress - I2C Channel
   * \param dcuHardId - DCU hardware ID
   * \param enabled - Enabled connection
   */
  ConnectionDescription ( unsigned int fedId = 0, unsigned int fedChannel = 0, 
			  std::string fecHardwareId = "",
			  unsigned int fecCrateId = 0, unsigned int fecSlot = 0, 
			  unsigned int ringSlot = 0, unsigned int ccuAddress = 0, 
			  unsigned int i2cChannel = 0, unsigned int apvAddress = 0,
			  unsigned int dcuHardId = 0, bool enabled = true,
			  unsigned int fedCrateId = 0, unsigned int fedSlot = 0 ) ;

  /** Constructor for ConnectionDescription with the parameter names and values
   * \param parameterNames - description of the parameters (fec, ring, ccu, channel, address)
   */
  ConnectionDescription ( parameterDescriptionNameType parameterNames ) ;

  /** Get CRATE ID
   * \return CRATE Slot
   */
  inline unsigned int getFedCrateId ( ) { return fedCrateId_; }

  /** Set CRATE ID
   * \param fedCrateId - CRATE ID
   */
  inline void setFedCrateId ( unsigned int fedCrateId ) { fedCrateId_ = fedCrateId ; }

  /** Get CRATE ID
   * \return CRATE Slot
   */
  inline unsigned int getFedCrateSlot ( ) {

    //#warning "getFedCrateSlot DEPRECATED> use getFedCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getFedCrateId" << std::endl ;
    return fedCrateId_;
  }

  /** Set CRATE ID
   * \param fedCrateId - CRATE ID
   */
  inline void setFedCrateSlot ( unsigned int fedCrateId ) {

    //#warning "setFedCrateSlot DEPRECATED> use setFedCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use setFedCrateId" << std::endl ;
    fedCrateId_ = fedCrateId ;
  }

  /** Get FED Slot
   * \return FED Slot
   */
  inline unsigned int getFedSlot ( ) { return fedSlot_; }

  /** Set FED Slot
   * \param fedSlot - FED Slot
   */
  void setFedSlot ( unsigned int fedSlot ) { fedSlot_ = fedSlot ; }

  /** Get FED software ID
   * \return FED software ID
   */
  inline unsigned int getFedId ( ) { return fedId_; }

  /** Set FED software ID
   * \param fedId - FED software ID
   */
  inline void setFedId ( unsigned int fedId ) { fedId_ = fedId ; }

  /** Get FED channel
   * \return FED channel
   */
  inline unsigned int getFedChannel ( ) { return fedChannel_ ; }

  /** Set FED channel
   * \param fedChannel -  FED channel
   */
  inline void setFedChannel ( unsigned int fedChannel ) { fedChannel_ = fedChannel ; }

  /** \brief Set the FEC hardware identification number
   */
  inline void setFecHardwareId ( std::string fecHardwareId ) { strncpy (fecHardwareId_, fecHardwareId.c_str(), 100) ; }

  /** \brief return the FEC hardware identification number
   */
  inline std::string getFecHardwareId ( ) { return fecHardwareId_ ; }

  /** Get FEC Slot
   * \return FEC Slot
   */
  inline unsigned int getFecSlot ( ) { return fecSlot_; }

  /** Set FEC Slot
   * \param fecSlot - FEC Slot
   */
  void setFecSlot ( unsigned int fecSlot ) { fecSlot_ = fecSlot ; }

  /** Get CRATE ID
   * \return CRATE ID
   */
  inline unsigned int getFecCrateId ( ) { return fecCrateId_; }

  /** Set CRATE ID
   * \param fecCrateId - CRATE ID
   */
  void setFecCrateId ( unsigned int fecCrateId ) { fecCrateId_ = fecCrateId ; }

  /** Get CRATE ID
   * \return CRATE ID
   */
  inline unsigned int getFecCrateSlot ( ) {

    //#warning "getFecCrateSlot DEPRECATED> use getFecCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use getFecCrateId" << std::endl ;
    return fecCrateId_;
  }

  /** Set CRATE ID
   * \param fecCrateId - CRATE ID
   */
  inline void setFecCrateSlot ( unsigned int fecCrateId ) {

    //#warning "setFecCrateSlot DEPRECATED> use setFecCrateId instead"
    std::cerr << "--------------------> " << __PRETTY_FUNCTION__ << " DEPRECATED> use setFecCrateId" << std::endl ;
    fecCrateId_ = fecCrateId ;
  }

  /** Get ring slot
   * \return ring slot
   */
  inline unsigned int getRingSlot ( ) { return ringSlot_ ; }

  /** Set ring slot
   * \param ringSlot - ring slot
   */
  void setRingSlot ( unsigned int ringSlot ) { ringSlot_ = ringSlot ; }

  /** Get CCU address
   * \return CCU address
   */
  inline unsigned int getCcuAddress ( ) { return ccuAddress_; }

  /** Set CCU address
   * \param ccuAddress - CCU address
   */
  void setCcuAddress ( unsigned int ccuAddress ) { ccuAddress_ = ccuAddress ; }

  /** Get I2C Channel
   * \return I2C Channel
   */
  inline unsigned int getI2cChannel ( ) { return i2cChannel_ ; }

  /** Set I2C Channel
   * \param i2cChannel - I2C Channel
   */
  void setI2cChannel ( unsigned int i2cChannel ) { i2cChannel_ = i2cChannel ; }

  /** Get APV address
   * \return APV Address
   */
  inline unsigned int getApvAddress ( ) { return apvAddress_ ; }

  /** Set APV address
   * \param apvChannel - APV Address
   */
  void setApvAddress ( unsigned int apvAddress ) { apvAddress_ = apvAddress ; }

  /** Get DCU hardware ID 
   * \return DCU hardware ID
   */
  inline unsigned int getDcuHardId ( ) { return dcuHardId_ ; }

  /** Set DCU hardware ID
   * \param dcuHardId - DCU hardware ID
   */
  void setDcuHardId ( unsigned int dcuHardId ) { dcuHardId_ = dcuHardId ; }

  /** Get Enabled connection
   * \return Enabled connection
   */
  inline bool getEnabled ( ) { return enabled_ ; }

  /** Is the connection enabled
   */
  inline bool isEnabled ( ) { return enabled_ ; }

  /** Set Enabled connection
   * \param enabled - Enabled connection
   */
  void setEnabled ( bool enabled ) { enabled_ = enabled ; }

  /** Get key of the FEC position
   * \return index of the FEC position
   */
  inline keyType getKey ( ) { return buildCompleteKey(fecSlot_,ringSlot_,ccuAddress_,i2cChannel_,apvAddress_) ; }

  /** Set key of the FEC position
   * \param index - index of the FEC position
   */
  inline void setKey ( keyType index ) {
    fecSlot_ = getFecKey(index) ;
    ringSlot_ = getRingKey(index) ;
    ccuAddress_ = getCcuKey(index) ;
    i2cChannel_ = getChannelKey(index) ;
    apvAddress_ = getAddressKey(index) ; 
  }

  /** Get method for the det id
   * \return the det id
   */
  inline unsigned int getDetId ( ) {
    return detId_ ;
  }

  /** Get method for the number of APVs
   * \return the number of APVs
   */
  inline unsigned int getNumberOfApvs ( ) {
    return nApvs_ ;
  }

  /** Get method for the fiber length
   * \return the fiber length
   */
  inline double getFiberLength ( ) {
    return fiberLength_ ;
  }

  /** Set the det id for this connection
   * \param detId - det id
   */
  inline void setDetId ( unsigned int detId ) {
    detId_ = detId ;
  }
  
  /** Set the number of APVs for this connection
   * /param nApvs - number of APVs
   */
  inline void setNumberOfAvps ( unsigned int nApvs ) {
    nApvs_ = nApvs ;
  }

  /** Set the fiber length
   * \param fiberLength - fiber length
   */
  inline void setFiberLength ( double fiberLength ) {
    fiberLength_ = fiberLength ;
  }

  /** \brief display a connection description
   */
  void display ( ) ;

  /** \brief compare two connections
   */
  bool operator== ( ConnectionDescription &connection ) ;

  /** \brief sort a vector of connection descriptions
   */
  bool sortByKey ( ConnectionDescription *r1, ConnectionDescription *r2 ) ;

  /** \brief sort a vector of connection descriptions
   */
  bool sortByDcuHardId ( ConnectionDescription *r1, ConnectionDescription *r2 ) ;

  /** \brief clone the connection
   */
  ConnectionDescription *clone ( ) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name and its content
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;
};

#endif

