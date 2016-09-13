/*
This file is part of Fec Software project.

author: Juha Petäjäjärvi
based on FecAccessManager by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "TotemFecAccessManager.h"

/** Inialising the attributs of the class
 */
TotemFecAccessManager::TotemFecAccessManager ( FecAccess *fecAccess, bool displayDebugMessage ) : FecAccessManager ( fecAccess, displayDebugMessage ){

  vfatSet_.clear ( ) ;
  cchipSet_.clear ( ) ;
  tbbSet_.clear ( ) ;
}

/** Destroy each access store in the different hash tables and destroy all the
 * map created
 */
TotemFecAccessManager::~TotemFecAccessManager ( ) {

  removeDevices ( ) ;
}

/** Destroy each access store in the different hash tables for all the device
 * that was initialised. 
 */
void TotemFecAccessManager::removeDevices ( ) {

  removeDevices (APV25) ;
  removeDevices (PLL) ;
  removeDevices (LASERDRIVER) ;
  removeDevices (DOH) ;
  removeDevices (APVMUX) ;
  removeDevices (PHILIPS) ;
  removeDevices (DCU) ;
  removeDevices (PIARESET) ;

#ifdef PRESHOWER
  removeDevices (DELTA) ;
  removeDevices (PACE) ;
  removeDevices (KCHIP) ;
  removeDevices (GOH) ;
#endif // PRESHOWER

  removeDevices (VFAT) ;
  removeDevices (CCHIP) ;
  removeDevices (TBB) ;
}

/** Destroy each access store in the different hash tables for all the device
 * that was initialised. 
 * \param type - device type to be removed
 */
void TotemFecAccessManager::removeDevices ( enumDeviceType type ) {

  // Destroy all the device connected
  if (type == APV25) {
    for (apvAccessedType::iterator p=apvSet_.begin();p!=apvSet_.end();p++) {

      // Retreive the device connected
      apvAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (APV25)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear APV table
    apvSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == PLL) {
    for (pllAccessedType::iterator p=pllSet_.begin();p!=pllSet_.end();p++) {
      
      // Retreive the device connected
      pllAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (PLL)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PLL table
    pllSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == LASERDRIVER) {
    for (laserdriverAccessedType::iterator p=laserdriverSet_.begin();p!=laserdriverSet_.end();p++) {
      
      // Retreive the device connected
      laserdriverAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (LASERDRIVER)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear LASERDRIVER table
    laserdriverSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == DOH) {
    for (dohAccessedType::iterator p=dohSet_.begin();p!=dohSet_.end();p++) {
      
      // Retreive the device connected
      DohAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (DOH)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DOH table
    dohSet_.clear ( ) ;

    return ;
  }

  // Destroy all the device connected
  if (type == APVMUX) {
    for (muxAccessedType::iterator p=muxSet_.begin();p!=muxSet_.end();p++) {

      // Retreive the device connected
      muxAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (MUX)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear APVMUX table
    muxSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == PHILIPS) {
    for (philipsAccessedType::iterator p=philipsSet_.begin();p!=philipsSet_.end();p++) {

      // Retreive the device connected
      philipsAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;        

        std::cout << "Problem in TotemFecAccessManager::removeDevices (PHILIPS)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PHILIPS table
    philipsSet_.clear ( )     ;

    return ;
  }

  // Destroy all the device connected
  if (type == DCU) {
    for (dcuAccessedType::iterator p=dcuSet_.begin();p!=dcuSet_.end();p++) {
  
      // Retreive the device connected
      dcuAccess *device = p->second ;
      
      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;        

        std::cout << "Problem in TotemFecAccessManager::removeDevices (DCU) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DCU table
    dcuSet_.clear  ( )        ;

    return ;
  }

  if (type == PIARESET) {

    // Destroy all the device connected
    for (piaAccessedType::iterator p=piaSet_.begin();p!=piaSet_.end();p++) {

      // Retreive the device connected
      PiaResetAccess *pia = p->second ;

      try {
        // Delete it
        delete pia ;
      }
      catch (FecExceptionHandler &e) {
        
        char msg[80] ;
        decodeKey (msg, pia->getKey()) ;

        std::cout << "Problem in TotemFecAccessManager::removeDevices (PIA) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PIA table
    piaSet_.clear  ( )        ;

    return ;
  }

#ifdef PRESHOWER
  // Destroy all the device connected
  if (type == DELTA) {
    for (deltaAccessedType::iterator p=deltaSet_.begin();p!=deltaSet_.end();p++) {

      // Retreive the device connected
      deltaAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (DELTA)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear DELTA table
    deltaSet_.clear  ( )        ;

    return ;
  }

// Destroy all the device connected
  if (type == PACE) {
    for (paceAccessedType::iterator p=paceSet_.begin();p!=paceSet_.end();p++) {

      // Retreive the device connected
      paceAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (PACE)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear PACE table
    paceSet_.clear  ( )        ;

    return ;
  }

// Destroy all the device connected
  if (type == KCHIP) {
    for (kchipAccessedType::iterator p=kchipSet_.begin();p!=kchipSet_.end();p++) {

      // Retreive the device connected
      kchipAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (KCHIP)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear KCHIP table
    kchipSet_.clear  ( )        ;
    return ;
  }
// Destroy all the device connected
  if (type == GOH) {
    for (gohAccessedType::iterator p=gohSet_.begin();p!=gohSet_.end();p++) {

      // Retreive the device connected
      gohAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (GOH)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear KCHIP table
    gohSet_.clear  ( ) ;
    return ;
  }

#endif // PRESHOWER

  // Destroy all the device connected
  if (type == VFAT) {
    for (vfatAccessedType::iterator p=vfatSet_.begin();p!=vfatSet_.end();p++) {

      // Retreive the device connected
      vfatAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (VFAT)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear VFAT table
    vfatSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == CCHIP) {
    for (cchipAccessedType::iterator p=cchipSet_.begin();p!=cchipSet_.end();p++) {

      // Retreive the device connected
      totemCChipAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (CCHIP)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear CCHIP table
    cchipSet_.clear  ( )        ;

    return ;
  }

  // Destroy all the device connected
  if (type == TBB) {
    for (tbbAccessedType::iterator p=tbbSet_.begin();p!=tbbSet_.end();p++) {

      // Retreive the device connected
      totemBBAccess *device = p->second ;

      try {
        // Delete it
        delete device ;
      }
      catch (FecExceptionHandler &e) {

        char msg[80] ;
        decodeKey (msg, device->getKey()) ;
        
        std::cout << "Problem in TotemFecAccessManager::removeDevices (TBB)) when I delete " << msg << std::endl ;
	std::cout << e.what() << std::endl ;
      }
    }

    // Clear CCHIP table
    tbbSet_.clear  ( )        ;

    return ;
  }
}

/** 
 * \param deviceType - device type
 * \param index - key of the device connected
 * \return access of the device (it is the real access and not a copy, do not delete it !)
 * \exception FecExceptionHandler
 * \warning Only access that inherits from deviceAccess class can be used in this methods
 */
deviceAccess *TotemFecAccessManager::getAccess ( enumDeviceType deviceType, keyType index ) {
 
  deviceAccess *device = NULL ;

  if (deviceType == APV25) {
    // Try to find the Apv in the map
    apvAccessedType::iterator apvmap = apvSet_.find(index) ;
    if (apvmap == apvSet_.end()) device = NULL ;
    else device = apvmap->second ;
  } 
  
  else if (deviceType == PLL) {
    // Try to find the Pll in the map
    pllAccessedType::iterator pllmap = pllSet_.find(index) ;
    if (pllmap == pllSet_.end()) device = NULL ;
    else device = pllmap->second ;
  } 

  else if (deviceType == LASERDRIVER) {
    // Try to find the Laserdriver in the map
    laserdriverAccessedType::iterator laserdrivermap = laserdriverSet_.find(index) ;
    if (laserdrivermap == laserdriverSet_.end()) device = NULL ;
    else device = laserdrivermap->second ;
  } 

  else if (deviceType == DOH) {
    // Try to find the Doh in the map
    dohAccessedType::iterator dohmap = dohSet_.find(index) ;
    if (dohmap == dohSet_.end()) device = NULL ;
    else device = dohmap->second ;
  } 
  
  else if (deviceType == APVMUX) {
    // Try to find the Mux in the map
    muxAccessedType::iterator muxmap = muxSet_.find(index) ;
    if (muxmap == muxSet_.end()) device = NULL ;
    else device = muxmap->second ;
  } 

  else if (deviceType == DCU) {
    // Try to find the Dcu in the map
    dcuAccessedType::iterator dcumap = dcuSet_.find(index) ;
    if (dcumap == dcuSet_.end()) device = NULL ;
    else device = dcumap->second ;

  } 

  else if (deviceType == PHILIPS) {
    // Try to find the Philips in the map
    philipsAccessedType::iterator philipsmap = philipsSet_.find(index) ;
    if (philipsmap == philipsSet_.end()) device = NULL ;
    else device = philipsmap->second ;
  }

#ifdef PRESHOWER
  else if (deviceType == DELTA) {
    // Try to find the Delta in the map
    deltaAccessedType::iterator deltamap = deltaSet_.find(index) ;
    if (deltamap == deltaSet_.end()) device = NULL ;
    else device = deltamap->second ;
  }

  else if (deviceType == PACE) {
    // Try to find the PaceAM in the map
    paceAccessedType::iterator pacemap = paceSet_.find(index) ;
    if (pacemap == paceSet_.end()) device = NULL ;
    else device = pacemap->second ;
  }

  else if (deviceType == KCHIP) {
    // Try to find the Kchip in the map
    kchipAccessedType::iterator kchipmap = kchipSet_.find(index) ;
    if (kchipmap == kchipSet_.end()) device = NULL ;
    else device = kchipmap->second ;
  }

  else if (deviceType == GOH) {
    // Try to find the Kchip in the map
    gohAccessedType::iterator gohmap = gohSet_.find(index) ;
    if (gohmap == gohSet_.end()) device = NULL ;
    else device = gohmap->second ;
  }
#endif // PRESHOWER

  else if (deviceType == VFAT) {
    // Try to find the Vfat in the map
    vfatAccessedType::iterator vfatmap = vfatSet_.find(index) ;
    if (vfatmap == vfatSet_.end()) device = NULL ;
    else device = vfatmap->second ;
  }

  else if (deviceType == CCHIP) {
    // Try to find the CChip in the map
    cchipAccessedType::iterator cchipmap = cchipSet_.find(index) ;
    if (cchipmap == cchipSet_.end()) device = NULL ;
    else device = cchipmap->second ;
  }

  else if (deviceType == TBB) {
    // Try to find the Tbb in the map
    tbbAccessedType::iterator tbbmap = tbbSet_.find(index) ;
    if (tbbmap == tbbSet_.end()) device = NULL ;
    else device = tbbmap->second ;
  }

  // Note that the PIA access cannot be returned in this method
  return (device) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
vfatAccessedType &TotemFecAccessManager::getVfatAccesses ( ) {
 
  return (vfatSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
cchipAccessedType &TotemFecAccessManager::getCchipAccesses ( ) {
 
  return (cchipSet_) ;
}

/** 
 * \return all the accesses from the map (it is the real accesses and not a copy, do not delete it !)
 * \warning Very Important: the access returned is the real access and not a copy, do not delete it !
 */
tbbAccessedType &TotemFecAccessManager::getTbbAccesses ( ) {
 
  return (tbbSet_) ;
}

/**
 * \param access - device access class
 * \exception FecExceptionHandler
 */
void TotemFecAccessManager::setAccess ( deviceAccess *access ) throw (FecExceptionHandler) {

  keyType index = access->getKey() ;
  enumDeviceType deviceType = access->getDeviceType() ;

  if (deviceType == APV25) {
    // Try to find the Apv in the map
    apvAccessedType::iterator apvmap = apvSet_.find(index) ;
    if (apvmap == apvSet_.end()) {
      
      // Add the access to the map
      apvSet_[index] = (apvAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (APV) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 
  
  else if (deviceType == PLL) {
    // Try to find the Pll in the map
    pllAccessedType::iterator pllmap = pllSet_.find(index) ;
    if (pllmap == pllSet_.end()) {
      
      // Add the access to the map
      pllSet_[index] = (pllAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (PLL) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == LASERDRIVER) {
    // Try to find the Laserdriver in the map
    laserdriverAccessedType::iterator laserdrivermap = laserdriverSet_.find(index) ;
    if (laserdrivermap == laserdriverSet_.end()) {

      // Add the access to the map
      laserdriverSet_[index] =  (laserdriverAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (AOH) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == DOH) {
    // Try to find the Doh in the map
    dohAccessedType::iterator dohmap = dohSet_.find(index) ;
    if (dohmap == dohSet_.end()) {

      // Add the access to the map
      dohSet_[index] =  (DohAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (DOH) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == APVMUX) {
    // Try to find the Mux in the map
    muxAccessedType::iterator muxmap = muxSet_.find(index) ;
    if (muxmap == muxSet_.end()) {

      // Add the access to the map
      muxSet_[index] = (muxAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (APV MUX) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 
  
  else if (deviceType == DCU) {
    // Try to find the Dcu in the map
    dcuAccessedType::iterator dcumap = dcuSet_.find(index) ;
    if (dcumap == dcuSet_.end()) {

      // Add the access to the map
      dcuSet_[index] = (dcuAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (DCU) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == PHILIPS) {
    // Try to find the Philips in the map
    philipsAccessedType::iterator philipsmap = philipsSet_.find(index) ;
    if (philipsmap == philipsSet_.end()) {
      
      // Add the access to the map
      philipsSet_[index] = (philipsAccess *)access ;
    }
    else {
      
      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (PHILIPS) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

#ifdef PRESHOWER
  else if (deviceType == DELTA) {
    // Try to find the Delta in the map
    deltaAccessedType::iterator deltamap = deltaSet_.find(index) ;
    if (deltamap == deltaSet_.end()) {
      
      // Add the access to the map
      deltaSet_[index] = (deltaAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (DELTA) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == PACE) {
    // Try to find the PaceAM in the map
    paceAccessedType::iterator pacemap = paceSet_.find(index) ;
    if (pacemap == paceSet_.end()) {
      
      // Add the access to the map
      paceSet_[index] = (paceAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (PACEAM) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == KCHIP) {
    // Try to find the Kchip in the map
    kchipAccessedType::iterator kchipmap = kchipSet_.find(index) ;
    if (kchipmap == kchipSet_.end()) {
      
      // Add the access to the map
      kchipSet_[index] = (kchipAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (KCHIP) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 

  else if (deviceType == GOH) {
    // Try to find the GOH in the map
    gohAccessedType::iterator gohmap = gohSet_.find(index) ;
    if (gohmap == gohSet_.end()) {
      
      // Add the access to the map
      gohSet_[index] = (gohAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (GOH) already connected",
					      ERRORCODE,
					      index) ;
    }
  } 
#endif // PRESHOWER

  else if (deviceType == VFAT) {
    // Try to find the Vfat in the map
    vfatAccessedType::iterator vfatmap = vfatSet_.find(index) ;
    if (vfatmap == vfatSet_.end()) {
      
      // Add the access to the map
      vfatSet_[index] = (vfatAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (VFAT) already connected",
					      ERRORCODE,
					      index) ;
    }
  }

  else if (deviceType == CCHIP) {
    // Try to find the Cchip in the map
    cchipAccessedType::iterator cchipmap = cchipSet_.find(index) ;
    if (cchipmap == cchipSet_.end()) {
      
      // Add the access to the map
      cchipSet_[index] = (totemCChipAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (CCHIP) already connected",
					      ERRORCODE,
					      index) ;
    }
  }

  else if (deviceType == TBB) {
    // Try to find the Tbb in the map
    tbbAccessedType::iterator tbbmap = tbbSet_.find(index) ;
    if (tbbmap == tbbSet_.end()) {
      
      // Add the access to the map
      tbbSet_[index] = (totemBBAccess *)access ;
    }
    else {

      RAISEFECEXCEPTIONHANDLER_HARDPOSITION ( XDAQFEC_INVALIDOPERATION,
					      "device (TBB) already connected",
					      ERRORCODE,
					      index) ;
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER_INFOSUP ( XDAQFEC_INVALIDOPERATION,
				       "device type unknown",
				       ERRORCODE,
				       index,
				       "device type", deviceType) ;
  }
}

/**
 * \param vDevice - a vector of devices (description)
 * \param errorList - list of exceptions during the download
 * \param pllReset - force the PLL reset at the starting (default false)
 * \param dohSet - set the DOH (default true)
 * \return error number
 * \exception FecHandlerException
 */
unsigned int TotemFecAccessManager::downloadValues ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset, bool dohSet ) throw (FecExceptionHandler) {

  if (displayDebugMessage_)
    std::cout << std::dec << vDevice->size() << " devices to be downloaded" << std::endl ;

  // A message
  char msg[MAXCHARDECODEKEY] ;

  // Number of errors
  unsigned int error = 0 ;
  
  if ( (vDevice != NULL) && (!vDevice->empty()) ) {

    // For each device => access it
    for (deviceVector::iterator device = vDevice->begin() ; (device != vDevice->end()) && (! haltStateMachine_) && ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); device ++) {

      deviceDescription *deviced = *device ;

      switch (deviced->getDeviceType()) {
      case APV25: {
	apvDescription *apvDevice = (apvDescription *)deviced ;
	error += parseApv ( *apvDevice, errorList ) ;
	break ;
      }
      case PLL: {
	pllDescription *pllDevice = (pllDescription *)deviced ;
	error += parsePll ( *pllDevice, errorList, true, pllReset ) ;
	break ;
      }
      case DOH:
      case LASERDRIVER: {
	laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	// Address 0x60 for laserdriver
	// Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	if (laserdriverDevice->getAddress() == DOHI2CADDRESS)
	  error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	else
	  error += parseLaserdriver ( *laserdriverDevice, errorList ) ;
	break ;
      }
      case APVMUX: {
	muxDescription *muxDevice = (muxDescription *)deviced ;
	error += parseMux ( *muxDevice, errorList ) ;
	break ;
      }
      case PHILIPS: {
	philipsDescription *philipsDevice = (philipsDescription *)deviced ;
	error += parsePhilips ( *philipsDevice, errorList ) ;
	break ;
      }
      case DCU: {
	dcuDescription *dcuDevice = (dcuDescription *)deviced ;
	error += parseDcu ( *dcuDevice, errorList ) ;
	break ;
      }
#ifdef PRESHOWER
      case DELTA: {
	deltaDescription *deltaDevice = (deltaDescription *)deviced ;
	error += parseDelta ( *deltaDevice, errorList ) ;
	break ;
      }
      case PACE: {
	paceDescription *paceDevice = (paceDescription *)deviced ;
	error += parsePace ( *paceDevice, errorList ) ;
	break ;
      }
      case KCHIP: {
	kchipDescription *kchipDevice = (kchipDescription *)deviced ;
	error += parseKchip ( *kchipDevice, errorList ) ;
	break ;
      }
      case GOH: {
	gohDescription *gohDevice = (gohDescription *)deviced ;
	error += parseGoh ( *gohDevice, errorList ) ;
	break ;
      }
#endif // PRESHOWER
	  
      case VFAT: {
	vfatDescription *vfatDevice = (vfatDescription *)deviced ;
	error += parseVfat ( *vfatDevice, errorList ) ;
	break ;
      }

      case CCHIP: {
	totemCChipDescription *cchipDevice = (totemCChipDescription *)deviced ;
	error += parseCChip ( *cchipDevice, errorList ) ;
	break ;
      }

      case TBB: {
	totemBBDescription *tbbDevice = (totemBBDescription *)deviced ;
	error += parseTbb ( *tbbDevice, errorList ) ;
	break ;
      }
	  
      default:
	decodeKey (msg, deviced->getKey()) ;
	std::cerr << "Unknown device: 0x" << std::hex << deviced->getDeviceType() << " (" << msg << ")" << std::endl ;
      }
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER (NODATAAVAILABLE,
			      "no device found to be downloaded",
			      ERRORCODE) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/**
 * \param vDevice - a vector of devices (description)
 * \param errorList - list of exceptions during the download
 * \param pllReset - force the PLL reset at the starting (default false)
 * \param dohSet - set the DOH (default true)
 * \return error number
 * \exception FecHandlerException
 */
unsigned int TotemFecAccessManager::downloadValuesMultipleFrames ( deviceVector *vDevice, std::list<FecExceptionHandler *> &errorList, bool pllReset, bool dohSet ) throw (FecExceptionHandler) {

  if (displayDebugMessage_) 
    std::cout << "TotemFecAccessManager::downloadValuesMultipleFrames: " << std::dec << vDevice->size() << " devices to be downloaded" << std::endl ;
  
  // A message
  char msg[MAXCHARDECODEKEY] ;

  // Number of errors
  unsigned int error = 0 ;

  // vector of PLL to send the frames
  deviceVector pllVector ;
  
  if ( (vDevice != NULL) && (!vDevice->empty()) ) {

    // hash_map of list of device frames
    accessDeviceTypeListMap vAccessDevices ;

    // For each device => access it
    for (deviceVector::iterator device = vDevice->begin() ; (device != vDevice->end()) && (! haltStateMachine_) && (error < maxErrorAllowed_); device ++) {

      deviceDescription *deviced = *device ;
      keyType indexFecRing = getFecRingKey ((*device)->getKey()) ;

      switch (deviced->getDeviceType()) {
      case APV25: {
	apvDescription *apvDevice = (apvDescription *)deviced ;
	unsigned int err = parseApv ( *apvDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  apvSet_[apvDevice->getKey()]->getBlockWriteValues(*apvDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PLL: {
	pllDescription *pllDevice = (pllDescription *)deviced ;
	unsigned err = parsePll ( *pllDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  pllSet_[pllDevice->getKey()]->getBlockWriteValues(*pllDevice, vAccessDevices[indexFecRing]) ;
	break ;
      }
      case DOH: {
	// Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	break ;
      }
      case LASERDRIVER: {
	if (getAddressKey(deviced->getKey()) == DOHI2CADDRESS) {
	  // Address DOHI2CADDRESS for DOH (defined in DohAccess.h)
	  laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	  error += parseDoh ( *laserdriverDevice, errorList, dohSet ) ;
	}
	else {
	  // Address 0x60 for laserdriver
	  laserdriverDescription *laserdriverDevice = (laserdriverDescription *)deviced ;
	  unsigned int err = parseLaserdriver ( *laserdriverDevice, errorList, false ) ;
	  if (!err) // Each accesses is classified by ring
	    laserdriverSet_[laserdriverDevice->getKey()]->getBlockWriteValues(*laserdriverDevice, vAccessDevices[indexFecRing]) ;
	  else error += err ;
	}
	break ;
      }
      case APVMUX: {
	muxDescription *muxDevice = (muxDescription *)deviced ;
	unsigned int err = parseMux ( *muxDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  muxSet_[muxDevice->getKey()]->getBlockWriteValues(*muxDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PHILIPS: {
	philipsDescription *philipsDevice = (philipsDescription *)deviced ;
	unsigned int err = parsePhilips ( *philipsDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  philipsSet_[philipsDevice->getKey()]->getBlockWriteValues(*philipsDevice, vAccessDevices[indexFecRing]) ;
	break ;
      }
      case DCU: {
	dcuDescription *dcuDevice = (dcuDescription *)deviced ;
	error += parseDcu ( *dcuDevice, errorList, false ) ;
	break ;
      }
#ifdef PRESHOWER
      case DELTA: {
	deltaDescription *deltaDevice = (deltaDescription *)deviced ;
	unsigned int err = parseDelta ( *deltaDevice, errorList, false ) ;
	if (!err) // Each accesses is classified by ring
	  deltaSet_[deltaDevice->getKey()]->getBlockWriteValues(*deltaDevice, vAccessDevices[indexFecRing]) ;
	else error += err ;
	break ;
      }
      case PACE: {
	paceDescription *paceDevice = (paceDescription *)deviced ;
	error += parsePace ( *paceDevice, errorList, true ) ;
	break ;
      }
      case KCHIP: {
	kchipDescription *kchipDevice = (kchipDescription *)deviced ;
	error += parseKchip ( *kchipDevice, errorList, true ) ;
	break ;
      }
      case GOH: {
	gohDescription *gohDevice = (gohDescription *)deviced ;
	error += parseGoh ( *gohDevice, errorList, true ) ;
	break ;
      }
#endif // PRESHOWER
	  
      case VFAT: {
	vfatDescription *vfatDevice = (vfatDescription *)deviced ;
	error += parseVfat ( *vfatDevice, errorList, true ) ;
	break ;
      }

      case CCHIP: {
	totemCChipDescription *cchipDevice = (totemCChipDescription *)deviced ;
	error += parseCChip ( *cchipDevice, errorList, true ) ;
	break ;
      }

      case TBB: {
	totemBBDescription *tbbDevice = (totemBBDescription *)deviced ;
	error += parseTbb ( *tbbDevice, errorList, true ) ;
	break ;
      }
	  
      default:
	decodeKey (msg, deviced->getKey()) ;
	std::cerr << "Unknown device: 0x" << std::hex << deviced->getDeviceType() << " (" << msg << ")" << std::endl ;
      }
    }
    // ---------------------------------------------------------------------------------------------------------------------------
    // Apply a reset on the PLL if needed
    if (pllSet_.size() > 0) {

      std::list<keyType> pllErrorBefore, pllErrorAfter ;
      bool errorGoingBit = false ;
      pllAccess::setPllCheckGoingBitMultipleFrames ( *fecAccess_, errorList, pllSet_, 
      						     &errorGoingBit, pllErrorBefore, pllErrorAfter,
      //                                             ^is a going bit is not correct ?
      						     pllReset, true ) ;
      //                                             ^ Check the GOING bit to see if the reset is necessary
      //                                                       ^ Normal reset (true = Cold reset)
      //if (pllErrorAfter.size() == 0) std::cout << "All PLL locked" << std::endl ;

      for (std::list<keyType>::iterator it = pllErrorAfter.begin() ; it != pllErrorAfter.end() ; it ++) {

	FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_PLLOPERATIONAL,
								      XDAQFEC_PLLOPERATIONAL_MSG,
								      ERRORCODE,
								      *it) ;
	errorList.push_back(e) ;
      }
    }

    // ---------------------------------------------------------------------------------------------------------------------------
    // Make the download and decode the errors
    error += fecAccess_->setBlockDevices( vAccessDevices, errorList ) ;

    // Read out the DCU for tests
    //deviceVector dcuVector ;
    //dcuAccess::getDcuValuesMultipleFrames ( *fecAccess_, dcuSet_, dcuVector) ;
    //FecFactory::display (dcuVector) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  return (error) ;
}

/** 
 * \param comparison - do the comparison between what you write is what your read
 * \param dcuUpload - upload the DCU (by default true)
 * \return all the values in a vector of description, NULL is the upload is interrupted
 * \warning delete the vector once you have finished with it
 * \warning if the comparison is set, only the differences will be stored in the database 
 * or file
 */
deviceVector *TotemFecAccessManager::uploadValues ( std::list<FecExceptionHandler *> &errorList, bool comparison, bool dcuUpload, bool dohSet ) {

  // Retreive all the devices from the parsing class
  deviceVector *vDevice = new deviceVector ( ) ;
  
  unsigned int error = 0 ;
  unsigned int totalVal = 0 ;

  // Add all the devices
  for (apvAccessedType::iterator p=apvSet_.begin();(p!=apvSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    apvAccess *device = p->second ;

    apvDescription *apvDevice = NULL ;
    try {
      // Retreive the values
      apvDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	apvDescription *apvOrig = (apvDescription *)device->getDownloadedValues () ;
        if ( *apvOrig != *apvDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( apvDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV",
						 WARNINGCODE,
						 apvDevice->getKey()) ;
        }
	else
	  delete apvDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( apvDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an APV device"
      errorList.push_back (new FecExceptionHandler(e)) ;
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (muxAccessedType::iterator p=muxSet_.begin();(p!=muxSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    muxAccess *device = p->second ;

    muxDescription *muxDevice = NULL ;
    try {
      // Retreive the values
      muxDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	muxDescription *muxOrig = (muxDescription *)device->getDownloadedValues () ;
        if ( *muxOrig != *muxDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( muxDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV MUX",
						 WARNINGCODE,
						 muxDevice->getKey()) ;
        }
	else delete muxDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( muxDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of a MUX device"
      errorList.push_back (new FecExceptionHandler(e)) ;

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  if (dcuUpload) {
    // Add all the devices
    for (dcuAccessedType::iterator p=dcuSet_.begin();(p!=dcuSet_.end()) && (! haltStateMachine_);p++) {

      // Retreive the device connected
      dcuAccess *device = p->second ;

      dcuDescription *dcuDevice = NULL ;
      
      try {
	// Retreive the values
	dcuDevice = device->getValues() ;
	
	// Insert it in the vector vDevice
	vDevice->push_back ( dcuDevice ) ;
	
	totalVal += 1 ;
      }
      catch (FecExceptionHandler &e) {

	error ++ ;

	// "Error during the upload of a DCU device"
	errorList.push_back (new FecExceptionHandler(e)) ;	
	
#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }
  //#endif

  // Add all the devices
  for (laserdriverAccessedType::iterator p=laserdriverSet_.begin();(p!=laserdriverSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    laserdriverAccess *device = p->second ;

    laserdriverDescription *laserdriverDevice = NULL ;
    try {
      // Retreive the values
      laserdriverDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *laserdriverOrig = (laserdriverDescription *)device->getDownloadedValues () ;
        if ( *laserdriverOrig != *laserdriverDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( laserdriverDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the AOH",
						 WARNINGCODE,
						 laserdriverDevice->getKey()) ;
        }
	else delete laserdriverDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( laserdriverDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of a laserdriver device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (dohAccessedType::iterator p=dohSet_.begin();(p!=dohSet_.end()) && (! haltStateMachine_) && (dohSet);p++) {

    // Retreive the device connected
    DohAccess *device = p->second ;

    laserdriverDescription *dohDevice = NULL ;
    try {
      // Retreive the values
      dohDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *dohOrig = (laserdriverDescription *)device->getDownloadedValues () ;
        if ( *dohOrig != *dohDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( dohDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the DOH",
						 WARNINGCODE,
						 dohDevice->getKey()) ;

        }
	else delete dohDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( dohDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an DOH device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (philipsAccessedType::iterator p=philipsSet_.begin();(p!=philipsSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    philipsAccess *device = p->second ;

    philipsDescription *philipsDevice = NULL ;
    try {
      // Retreive the values
      philipsDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	philipsDescription *philipsOrig = (philipsDescription *)device->getDownloadedValues () ;
        if ( *philipsOrig != *philipsDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( philipsDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Philips",
						 WARNINGCODE,
						 philipsDevice->getKey()) ;

        }
	else delete philipsDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( philipsDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Philips device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (pllAccessedType::iterator p=pllSet_.begin();(p!=pllSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    pllAccess *device = p->second ;

    pllDescription *pllDevice = NULL ;
    try {
      // Retreive the values
      pllDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	pllDescription *pllOrig = (pllDescription *)device->getDownloadedValues () ;
        if ( *pllOrig != *pllDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( pllDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the PLL",
						 WARNINGCODE,
						 pllDevice->getKey()) ;
        }
	else delete pllDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( pllDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an PLL device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

#ifdef PRESHOWER
  // Add all the devices
  for (deltaAccessedType::iterator p=deltaSet_.begin();(p!=deltaSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    deltaAccess *device = p->second ;

    deltaDescription *deltaDevice = NULL ;
    try {
      // Retreive the values
      deltaDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	deltaDescription *deltaOrig = (deltaDescription *)device->getDownloadedValues () ;
        if ( *deltaOrig != *deltaDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( deltaDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Delta",
						 WARNINGCODE,
						 deltaDevice->getKey()) ;
        }
	else delete deltaDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( deltaDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an delta device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#  ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }


  // Add all the devices
  for (paceAccessedType::iterator p=paceSet_.begin();(p!=paceSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    paceAccess *device = p->second ;

    paceDescription *paceDevice = NULL ;
    try {
      // Retreive the values
      paceDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	paceDescription *paceOrig = (paceDescription *)device->getDownloadedValues () ;
        if ( *paceOrig != *paceDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( paceDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the PaceAM",
						 WARNINGCODE,
						 paceDevice->getKey()) ;
        }
	else delete paceDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( paceDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Pace device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (kchipAccessedType::iterator p=kchipSet_.begin();(p!=kchipSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    kchipAccess *device = p->second ;

    kchipDescription *kchipDevice = NULL ;
    try {
      // Retreive the values
      kchipDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	kchipDescription *kchipOrig = (kchipDescription *)device->getDownloadedValues () ;
        if ( *kchipOrig != *kchipDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( kchipDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Kchip",
						 WARNINGCODE,
						 kchipDevice->getKey()) ;
        }
	else delete kchipDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( kchipDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an Kchip device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }


  // TEMPORARY HACK by JUHA --> UPDATE WHEN THE OFFICIAL RELEASE IS AVAILABLE


  // Add all the devices
  for (gohAccessedType::iterator p=gohSet_.begin();(p!=gohSet_.end()) && (! haltStateMachine_);p++) {


    // Retreive the device connected
    gohAccess *device = p->second ;

    gohDescription *gohDevice = NULL ;
    try {
      // Retreive the values
      gohDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	gohDescription *gohOrig = (gohDescription *)device->getDownloadedValues () ;
        if ( *gohOrig != *gohDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( gohDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Goh",
						 WARNINGCODE,
						 gohDevice->getKey()) ;
        }
	else delete gohDevice ;
      }
      else {

        // Insert it in the vector vDevice
        vDevice->push_back ( gohDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an goh device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

#endif // PRESHOWER

  // Add all the devices
  for (vfatAccessedType::iterator p=vfatSet_.begin();(p!=vfatSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    vfatAccess *device = p->second ;

    vfatDescription *vfatDevice = NULL ;
    try {
      // Retreive the values
      vfatDevice = device->getVfatValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	vfatDescription *vfatOrig = (vfatDescription *)device->getDownloadedValues () ;
        if ( *vfatOrig != *vfatDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( vfatDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Vfat",
						 WARNINGCODE,
						 vfatDevice->getKey()) ;
        }
	else delete vfatDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( vfatDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an vfat device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (cchipAccessedType::iterator p=cchipSet_.begin();(p!=cchipSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    totemCChipAccess *device = p->second ;

    totemCChipDescription *cchipDevice = NULL ;
    try {
      // Retreive the values
      cchipDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	totemCChipDescription *cchipOrig = (totemCChipDescription *)device->getDownloadedValues () ;
        if ( *cchipOrig != *cchipDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( cchipDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the CChip",
						 WARNINGCODE,
						 cchipDevice->getKey()) ;
        }
	else delete cchipDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( cchipDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an cchip device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Add all the devices
  for (tbbAccessedType::iterator p=tbbSet_.begin();(p!=tbbSet_.end()) && (! haltStateMachine_);p++) {

    // Retreive the device connected
    totemBBAccess *device = p->second ;

    totemBBDescription *tbbDevice = NULL ;
    try {
      // Retreive the values
      tbbDevice = device->getValues() ;
      totalVal += 1 ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	totemBBDescription *tbbOrig = (totemBBDescription *)device->getDownloadedValues () ;
        if ( *tbbOrig != *tbbDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( tbbDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the TBB",
						 WARNINGCODE,
						 tbbDevice->getKey()) ;
        }
	else delete tbbDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( tbbDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an vfat device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Check and update the database or a file ...
  if (totalVal == 0) {

    error ++ ;
  }
  else if (displayDebugMessage_) {
    // Debug reporting
    std::string debugReport = "Found " + toString(totalVal) + " devices for all the modules" ;
    std::cout << debugReport << std::endl ;
  }

  lastOperationNumberErrors_ = error ;

  return (vDevice) ;
}

/** 
 * \param comparison - do the comparison between what you write is what your read
 * \param dcuUpload - upload the DCU (by default true)
 * \return all the values in a vector of description, NULL is the upload is interrupted
 * \warning delete the vector once you have finished with it
 * \warning if the comparison is set, only the differences will be stored in the database 
 * or file
 */
deviceVector *TotemFecAccessManager::uploadValuesMultipleFrames ( std::list<FecExceptionHandler *> &errorList, bool comparison, bool dcuUpload, bool dohSet ) {
  
  // Retreive all the devices from the parsing class
  deviceVector *vDevice = new deviceVector ( ) ;
  
  unsigned int error = 0 ;
  unsigned int totalVal = 0 ;

  // --------------------------------------------------------------- APV
  // Find all the values for the APV
  try {
    deviceVector apvVector ;
    error += apvAccess::getApvValuesMultipleFrames (*fecAccess_, apvSet_, apvVector, errorList )  ;
    totalVal += apvVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator apvIt = apvVector.begin() ; apvIt != apvVector.end() ; apvIt ++) {

      apvDescription *apvDevice = (apvDescription *)(*apvIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  apvDescription *apvOrig = (apvDescription *)apvSet_[apvDevice->getKey()]->getDownloadedValues () ;

	  if ( *apvOrig != *apvDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( apvDevice ) ;
	    
 	    //apvOrig->displayDifferences (*apvDevice) ;
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						   XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV",
						   WARNINGCODE,
						   apvDevice->getKey()) ;
	  }
	  else
	    delete apvDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of an APV device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( apvDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an APV device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- MUX
  // Find all the values for the MUX
  try {
    deviceVector muxVector ;
    error += muxAccess::getMuxValuesMultipleFrames (*fecAccess_, muxSet_, muxVector, errorList )  ;
    totalVal += muxVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator muxIt = muxVector.begin() ; muxIt != muxVector.end() ; muxIt ++) {

      muxDescription *muxDevice = (muxDescription *)(*muxIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  muxDescription *muxOrig = (muxDescription *)muxSet_[muxDevice->getKey()]->getDownloadedValues () ;

	  if ( *muxOrig != *muxDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( muxDevice ) ;
 	    muxOrig->displayDifferences (*muxDevice) ;	 
	    	  
	    // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the APV MUX",
						 WARNINGCODE,
						 muxDevice->getKey() ) ;
	  }
	  else
	    delete muxDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of an MUX device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( muxDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an MUX device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- PLL
  // Find all the values for the PLL
  try {
    deviceVector pllVector ;
    error += pllAccess::getPllValuesMultipleFrames (*fecAccess_, pllSet_, pllVector, errorList )  ;
    totalVal += pllVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator pllIt = pllVector.begin() ; pllIt != pllVector.end() ; pllIt ++) {

      pllDescription *pllDevice = (pllDescription *)(*pllIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  pllDescription *pllOrig = (pllDescription *)pllSet_[pllDevice->getKey()]->getDownloadedValues () ;

	  if ( *pllOrig != *pllDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( pllDevice ) ;

 	    //pllOrig->displayDifferences (*pllDevice) ;	 

	    // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the PLL",
						 WARNINGCODE,
						 pllDevice->getKey() ) ;

	  }
	  else
	    delete pllDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a PLL device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( pllDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a PLL device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- LASERDRIVER
  // Find all the values for the LASERDRIVER
  try {
    deviceVector laserdriverVector ;
    error += laserdriverAccess::getLaserdriverValuesMultipleFrames (*fecAccess_, laserdriverSet_, laserdriverVector, errorList )  ;
    totalVal += laserdriverVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator laserdriverIt = laserdriverVector.begin() ; laserdriverIt != laserdriverVector.end() ; laserdriverIt ++) {

      laserdriverDescription *laserdriverDevice = (laserdriverDescription *)(*laserdriverIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  laserdriverDescription *laserdriverOrig = (laserdriverDescription *)laserdriverSet_[laserdriverDevice->getKey()]->getDownloadedValues () ;

	  if ( *laserdriverOrig != *laserdriverDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( laserdriverDevice ) ;
	    	  
	    // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the AOH",
						 WARNINGCODE,
						 laserdriverDevice->getKey() ) ;

	  }
	  else
	    delete laserdriverDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a laserdriver device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( laserdriverDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a laserdriver device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- DOH
  // Find all the values for the DOH
  for (dohAccessedType::iterator p=dohSet_.begin();(p!=dohSet_.end()) && (! haltStateMachine_) && (dohSet);p++) {

    // Retreive the device connected
    DohAccess *device = p->second ;

    laserdriverDescription *dohDevice = NULL ;
    try {
      // Retreive the values
      dohDevice = device->getValues() ;

      // Do the comparison
      if (comparison) {

        //                     Old Values                     != Values Uploaded
	laserdriverDescription *dohOrig = (laserdriverDescription *)device->getDownloadedValues () ;

#ifdef DEBUGMSGERROR
	std::cout << "Uploading DOH " << std::endl ;
	dohDevice->display() ;
	if ( *dohOrig != *dohDevice ) std::cout << "Differences" << std::endl;
	else std::cout << "No differences" << std::endl ;
#endif

        if ( *dohOrig != *dohDevice ) {

          // Insert it in the vector vDevice
          vDevice->push_back ( dohDevice ) ;

          // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the DOH",
						 WARNINGCODE,
						 device->getKey()) ;
        }
	else delete dohDevice ;
      }
      else {
        // Insert it in the vector vDevice
        vDevice->push_back ( dohDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      error ++ ;

      // "Error during the upload of an DOH device"
      errorList.push_back (new FecExceptionHandler(e)) ; 

#ifdef DEBUGGETREGISTERS
      // Check the registers of the FEC and CCU and display it
      if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
      // Original frame
      if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
      if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
      if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
    }
  }

  try {
    deviceVector laserdriverVector ;
    error += laserdriverAccess::getLaserdriverValuesMultipleFrames (*fecAccess_, laserdriverSet_, laserdriverVector, errorList )  ;
    totalVal += laserdriverVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator laserdriverIt = laserdriverVector.begin() ; laserdriverIt != laserdriverVector.end() ; laserdriverIt ++) {

      laserdriverDescription *laserdriverDevice = (laserdriverDescription *)(*laserdriverIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  laserdriverDescription *laserdriverOrig = (laserdriverDescription *)laserdriverSet_[laserdriverDevice->getKey()]->getDownloadedValues () ;

	  if ( *laserdriverOrig != *laserdriverDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( laserdriverDevice ) ;
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						   XDAQFEC_ERRORINCOMPARISON_MSG + " for the DOH",
						   WARNINGCODE,
						   laserdriverDevice->getKey() ) ;
	  }
	  else
	    delete laserdriverDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a laserdriver device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( laserdriverDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a laserdriver device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // --------------------------------------------------------------- Philips
  // Find all the values for the PHILIPS
  try {
    deviceVector philipsVector ;
    error += philipsAccess::getPhilipsValuesMultipleFrames (*fecAccess_, philipsSet_, philipsVector, errorList )  ;
    totalVal += philipsVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator philipsIt = philipsVector.begin() ; philipsIt != philipsVector.end() ; philipsIt ++) {

      philipsDescription *philipsDevice = (philipsDescription *)(*philipsIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  philipsDescription *philipsOrig = (philipsDescription *)philipsSet_[philipsDevice->getKey()]->getDownloadedValues () ;

	  if ( *philipsOrig != *philipsDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( philipsDevice ) ;	    
	  
	    // Error in comparison
	  RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						 XDAQFEC_ERRORINCOMPARISON_MSG + " for the Philips",
						 WARNINGCODE,
						 philipsDevice->getKey() ) ;
	  }
	  else
	    delete philipsDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a Philips device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( philipsDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of a Philips device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // ------------------------------------------------------------- DCU
  if (dcuUpload) {

    // Read out the DCU for tests
    try {
      deviceVector dcuVector ;
      error += dcuAccess::getDcuValuesMultipleFrames (*fecAccess_, dcuSet_, dcuVector, errorList )  ;
      totalVal += dcuVector.size() ;
      
      // Analyse the answer for comparison
      for (deviceVector::iterator dcuIt = dcuVector.begin() ; dcuIt != dcuVector.end() ; dcuIt ++) {
	
      dcuDescription *dcuDevice = (dcuDescription *)(*dcuIt) ;
      
      // Add it to the vector
      vDevice->push_back ( dcuDevice ) ;
      }
    }
    catch (FecExceptionHandler &e) {

      // "Error during the upload of a DCU device"
      errorList.push_back (new FecExceptionHandler(e)) ;
    }
  }

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // ------------------------------------------------------------- PRESHOWER
#ifdef PRESHOWER
  // --------------------------------------------------------------- DELTA
  // Find all the values for the delta chip
  try {
    deviceVector deltaVector ;
    error += deltaAccess::getDeltaValuesMultipleFrames (*fecAccess_, deltaSet_, deltaVector, errorList )  ;
    totalVal += deltaVector.size() ;

    // Analyse the answer for comparison
    for (deviceVector::iterator deltaIt = deltaVector.begin() ; deltaIt != deltaVector.end() ; deltaIt ++) {

      deltaDescription *deltaDevice = (deltaDescription *)(*deltaIt) ;

      if (comparison) {

	try {
	  // Old Values != Values Uploaded
	  deltaDescription *deltaOrig = (deltaDescription *)deltaSet_[deltaDevice->getKey()]->getDownloadedValues () ;

	  if ( *deltaOrig != *deltaDevice ) {
	    
	    // Insert it in the vector vDevice
	    vDevice->push_back ( deltaDevice ) ;
	    
 	    //deltaOrig->displayDifferences (*deltaDevice) ;
	    	  
	    // Error in comparison
	    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (XDAQFEC_ERRORINCOMPARISON,
						   XDAQFEC_ERRORINCOMPARISON_MSG + " for the DELTA",
						   WARNINGCODE,
						   deltaDevice->getKey() ) ;
	  }
	  else
	    delete deltaDevice ;
	}
	catch (FecExceptionHandler &e) {

	  // "Error during the upload of a delta device"
	  errorList.push_back (new FecExceptionHandler(e)) ;
	}
      }
      else { // no comparison put all values inside the vector
	// Insert it in the vector vDevice
        vDevice->push_back ( deltaDevice ) ;
      }      
    }
  }
  catch (FecExceptionHandler &e) {

    // "Error during the upload of an APV device"
    errorList.push_back (new FecExceptionHandler(e)) ;
  }
#endif

  // Test if halt command was sent
  if (haltStateMachine_) {
    haltStateMachine_ = false ;
    return (NULL) ;
  }

  // Number of operations
  lastOperationNumberErrors_ = error ;

  // Just to display
  //FecFactory::display (*vDevice) ;

  return (vDevice) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an vfatDescription that is sent via to the vfatAccess class. Note
 * that if the VFAT does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param vfatDevice - VFAT device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int TotemFecAccessManager::parseVfat ( vfatDescription vfatDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Vfat 0x" << std::hex << (uint)vfatDevice.getFecSlot()
		<< ":0x" << (uint)vfatDevice.getRingSlot()
		<< ":0x" << (uint)vfatDevice.getCcuAddress()
		<< ":0x" << (uint)vfatDevice.getChannel()
		<< ":0x" << (uint)vfatDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  try {

    vfatAccess *vfat ;

    // Try to find the Vfat in the map
    keyType myKey = vfatDevice.getKey() ;
    vfatAccessedType::iterator vfatmap = vfatSet_.find(myKey) ;
    if (vfatmap == vfatSet_.end()) {

      // Create the access
      vfat = new vfatAccess (fecAccess_, myKey) ;
      vfat->setFecHardwareId (vfatDevice.getFecHardwareId(),vfatDevice.getCrateId()) ;

      // Add the access to the map
      vfatSet_[vfat->getKey()] = vfat ;
    }
    else {
      
      vfat = vfatmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Vfat: set the values ..." << std::endl ;

      vfat->setVfatValues (vfatDevice) ;
      ok = true ;
    }
    else vfat->setVfatDescription (vfatDevice) ;
  }
  catch (FecExceptionHandler &e) {

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Vfat: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (new FecExceptionHandler(e)) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an totemCChipDescription that is sent via to the totemCChipAccess class. Note
 * that if the CCHIP does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param cchipDevice - CCHIP device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int TotemFecAccessManager::parseCChip ( totemCChipDescription cchipDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Cchip 0x" << std::hex << (uint)cchipDevice.getFecSlot()
		<< ":0x" << (uint)cchipDevice.getRingSlot()
		<< ":0x" << (uint)cchipDevice.getCcuAddress()
		<< ":0x" << (uint)cchipDevice.getChannel()
		<< ":0x" << (uint)cchipDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  try {

    totemCChipAccess *cchip ;

    // Try to find the Cchip in the map
    keyType myKey = cchipDevice.getKey() ;
    cchipAccessedType::iterator cchipmap = cchipSet_.find(myKey) ;
    if (cchipmap == cchipSet_.end()) {

      // Create the access
      cchip = new totemCChipAccess (fecAccess_, myKey) ;
      cchip->setFecHardwareId (cchipDevice.getFecHardwareId(),cchipDevice.getCrateId()) ;

      // Add the access to the map
      cchipSet_[cchip->getKey()] = cchip ;
    }
    else {
      
      cchip = cchipmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Cchip: set the values ..." << std::endl ;

      cchip->setValues (cchipDevice) ;
      ok = true ;
    }
    else cchip->setDescription (cchipDevice) ;
  }
  catch (FecExceptionHandler &e) {

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Cchip: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (new FecExceptionHandler(e)) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}

/** Retreive values from a device. 
 * This method take the different values
 * and create an totemBBDescription that is sent via to the totemBBAccess class. Note
 * that if the TBB does not exists in the corresponding hash table, a new one
 * is created and store in the table.
 * \param tbbDevice - TBB device with all informations
 * \param setIt - set the PLL after the connection (default true)
 * \return If an exception is done, an error is returned.
 * \exception FecExceptionHandler
 * \warning All the exceptions are catched in this method
 */
unsigned int TotemFecAccessManager::parseTbb ( totemBBDescription tbbDevice, std::list<FecExceptionHandler *> &errorList, bool setIt ) {

  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::ostringstream debugReport ;
    debugReport << "Tbb 0x" << std::hex << (uint)tbbDevice.getFecSlot()
		<< ":0x" << (uint)tbbDevice.getRingSlot()
		<< ":0x" << (uint)tbbDevice.getCcuAddress()
		<< ":0x" << (uint)tbbDevice.getChannel()
		<< ":0x" << (uint)tbbDevice.getAddress() ;
    std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
  }

  try {

    totemBBAccess *tbb ;

    // Try to find the Tbb in the map
    keyType myKey = tbbDevice.getKey() ;
    tbbAccessedType::iterator tbbmap = tbbSet_.find(myKey) ;
    if (tbbmap == tbbSet_.end()) {

      // Create the access
      tbb = new totemBBAccess (fecAccess_, myKey) ;
      tbb->setFecHardwareId (tbbDevice.getFecHardwareId(),tbbDevice.getCrateId()) ;

      // Add the access to the map
      tbbSet_[tbb->getKey()] = tbb ;
    }
    else {
      
      tbb = tbbmap->second ;
    }

    //unsigned int ret = 0 ;
    bool ok = false ;
    //char clearMsg[1000] ;

    // Set the values in the hardware
    if (setIt) {

      // Display
      if (displayDebugMessage_)
	std::cout << "DEBUG Information: " << "Tbb: set the values ..." << std::endl ;

      tbb->setValues (tbbDevice) ;
      ok = true ;
    }
    else tbb->setDescription (tbbDevice) ;
  }
  catch (FecExceptionHandler &e) {

    error = 1 ;
    
    // Error display
    if (displayDebugMessage_) {
      std::cerr << "****************************** ERROR ***************************" << std::endl ;
      std::cerr << "Error during a download of a Tbb: " << e.what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
    }
    
    // Error report
    errorList.push_back (new FecExceptionHandler(e)) ;

#ifdef DEBUGGETREGISTERS
    // Check the registers of the FEC and CCU and display it
    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
    // Original frame
    if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
    if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
    if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
  }

  return (error) ;
}


/**
 * Take a vector of PIA reset and reset all modules given by this vector
 * \param vPiaReset - vector of PIA reset description
 * \warning This function should overwrite the FecAccessManager::resetPiaModules after defining it as a virtual !!!
 */
unsigned int TotemFecAccessManager::resetPia ( piaResetVector *vPiaReset, std::list<FecExceptionHandler *> &errorList ) throw (FecExceptionHandler) {

  // Number of errors
  unsigned int error = 0 ;

  if (displayDebugMessage_) {
    std::string str = toString(vPiaReset->size()) + " PIA reset found" ;
    std::cout << "DEBUG Information: " << str << std::endl ;
  }
  
  if ( (vPiaReset != NULL) && (!vPiaReset->empty()) ) {

    // For each value => access it
    for (piaResetVector::iterator piaResetIt = vPiaReset->begin() ; (piaResetIt != vPiaReset->end()) && (! haltStateMachine_) && ( (error < maxErrorAllowed_) || (maxErrorAllowed_ == 0) ); piaResetIt ++) {  

      piaResetDescription *piaResetDes = *piaResetIt ;

      if (displayDebugMessage_) {
	std::ostringstream debugReport ;
	debugReport << "PIA reset for: 0x" << std::hex << (uint)piaResetDes->getFecSlot()
		    << ":0x" << (uint)piaResetDes->getRingSlot()
		    << ":0x" << (uint)piaResetDes->getCcuAddress()
		    << ":0x" << (uint)piaResetDes->getChannel()
		    << ":0x" << (uint)piaResetDes->getAddress()
		    << std::endl << "\t" << "Mask: " << std::dec << (uint)piaResetDes->getMask()
		    << std::endl << "\t" << "Delay between the write and read: " << (uint)piaResetDes->getDelayActiveReset()
		    << std::endl << "\t" << "Delay after the operation: " << (uint)piaResetDes->getIntervalDelayReset() ;
        std::cout << "DEBUG Information: " << debugReport.str() << std::endl ;
      }

      try {

        PiaResetAccess *piaReset ;

        // Try to find the access in the map
        keyType myKey = piaResetDes->getKey() ;
        piaAccessedType::iterator piamap = piaSet_.find(myKey) ;
        if (piamap == piaSet_.end()) {

          // Create the access
          piaReset = new PiaResetAccess ( fecAccess_, myKey, PIAINITIALVALUE) ;
	  piaReset->setFecHardwareId (piaResetDes->getFecHardwareId(),piaResetDes->getCrateId()) ;

          // Add access to the map
          piaSet_[piaReset->getKey()] = piaReset ;
        }
        else {
          piaReset = piamap->second ;
        }

	// Apply the mask
	tscType8 uval = PIAINITIALVALUE & ~piaResetDes->getMask() ;

	if (displayDebugMessage_) {

	  std::cout << std::hex << "WRITE: Initial & ~mask = " << PIAINITIALVALUE << " & " << (int)~piaResetDes->getMask() << " = " << (int)uval << std::dec << std::endl ;
	}

	fecAccess_->write ( piaReset->getKey(), uval ) ;

	if (displayDebugMessage_) {

	  // Wait delay
	  usleep ((unsigned long)piaResetDes->getDelayActiveReset()) ;

	  std::cout << std::hex << "WROTE (READ BACK): 0x" << (int)uval << " (0x" <<  (int)fecAccess_->read(piaReset->getKey()) << ")" << std::dec << std::endl ;
	}


	// Wait delay
	usleep ((unsigned long)piaResetDes->getIntervalDelayReset()) ;
      }
      catch (FecExceptionHandler &e) {
        
        error ++ ;

	// Error display
	if (displayDebugMessage_) {
	  std::cerr << "****************************** ERROR ***************************" << std::endl ;
	  std::cerr << "Error during reset through PIA/PIO reset: " << e.what() << std::endl ;
	  std::cerr << "****************************************************************" << std::endl ;
	}
	
	// Error report
	errorList.push_back (e.clone()) ;

#ifdef DEBUGGETREGISTERS
	// Check the registers of the FEC and CCU and display it
	if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	// Original frame
	if (e.getRequestFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getRequestFrame()) << std::endl ;
	if (e.getDirectAckFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDirectAckFrame()) << std::endl ;
	if (e.getAnswerFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getAnswerFrame()) << std::endl ;
#endif
      }
    }
  }
  else {

    RAISEFECEXCEPTIONHANDLER ( NODATAAVAILABLE,
			       "No PIA reset values in the vector",
			       ERRORCODE ) ;
  }

  haltStateMachine_ = false ;

  lastOperationNumberErrors_ = error ;

  // Adding an error if we raised the maximum number of errors
  if ( (maxErrorAllowed_ > 0) && (error >= maxErrorAllowed_) ) {
    FecExceptionHandler *e = NEWFECEXCEPTIONHANDLER ( TOOMUCHERROR,
						      "Maximum number of errors (" + toString(maxErrorAllowed_) + ") reached",
						      CRITICALERRORCODE) ;
    errorList.push_back(e) ;
  }


  return (error) ;
}
