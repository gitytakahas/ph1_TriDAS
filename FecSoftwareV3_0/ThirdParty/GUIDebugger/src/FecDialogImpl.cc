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

// Penser a changer les noms des canaux en fonction du type de CCU
// Loop pas encore implementer ...
// Pas d'upload de PIA dans la base de donnees

#include <fstream>

#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>

// Led
#include "PixMapLed.h"

// For time
#include <sys/time.h>

// To retreive the IP address of the current machine
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Type
#include "tscTypes.h"

#include "FecDialogImpl.h"

#include "FecVmeRingDevice.h"

//#define DEBUGMSGERROR

//using namespace std; 
//using namespace Sgi; 

typedef unsigned int uint ;

/** Constructor: build a FEC access
 */
FecDialogImpl::FecDialogImpl ( FecAccess *fecAccess, std::string partitionName, std::string fecHardwareId ):
 
  greenLed_((const char **)greenLed_data),
  orangeLed_((const char **)orangeLed_data),
  redLed_  ((const char **)redLed_data),
  blackLed_  ((const char **)blackLed_data),
  partitionName_(partitionName),
  fecHardwareId_(fecHardwareId) {

  // Create a FEC access
  fecAccess_ = fecAccess ;

  // Access manager
  fecAccessManager_ = new FecAccessManager ( fecAccess_ ) ;

#ifdef DATABASEFACTORY
  std::string login="nil", passwd="nil", path="nil" ;
  DbFecAccess::getDbConfiguration (login, passwd, path) ;

  if (login == "nil") {
    fecFactory_ = NULL ;
  }
  else {
    try {
      cout << "Create an access to the database "
	   << login << "@" << path << endl ;
      fecFactory_ = new FecFactory (login,passwd,path) ;

      trackerUseDatabase->setEnabled     (true) ;
      trackerDatabaseVersion->setEnabled (true) ;
      textLabelPartitionName->setEnabled (true) ;
      trackerPartitionName->setEnabled   (true) ;
      textLabelStructureName->setEnabled (true) ;
      trackerStructureName->setEnabled   (true) ;
    }
    catch (FecExceptionHandler &e) {

      char msg[1000] ;
      sprintf (msg, "Unable to connect the database %s@%s", login.c_str(), path.c_str()) ;
      ErrorMessage (msg, e) ;

      fecFactory_ = NULL ;
      //cerr << "*********** ERROR *************" << endl ; 
      //cerr << "Unable to connect the database " << login << "@" << path << endl ;
      //cerr << e.what() << endl ; 
      //cerr << "*******************************" << endl ;
    }
    catch (oracle::occi::SQLException &e) {
    
      char msg[1000] ;
      sprintf (msg, "Unable to connect the database %s@%s", login.c_str(), path.c_str()) ;
      ErrorMessage (msg, e.what()) ;

      fecFactory_ = NULL ;
      //cerr << "*********** ERROR *************" << endl ; 
      //cerr << "Unable to connect the database " << login << "@" << path << endl ;
      //cerr << e.what() << endl ; 
      //cerr << "*******************************" << endl ;
    }
  }
#endif

#ifdef DATABASEFACTORY
  try {
    if (fecFactory_ == NULL) {
      fecFactory_ = new FecFactory ( ) ;
      fecFactory_->setOutputFileName ( "output.xml" ) ;
    }
  }
  catch (FecExceptionHandler &e) {
    
    ErrorMessage ("Unable to create output for the devices", e) ;
  }
#endif
    
  // hide the redundancy part
  CCURedundancyFecInputGroup->hide() ;
  CCURedundancyFecOutputGroup->hide() ;

  // Nothing is displayed in the CCU order until a reset 
  displayCcuOrder_ = false ;

  // Display LED
  LinkInitThreadLinkLed2->setPixmap(blackLed_) ;

  // For FEC software V2_0 no device driver counters are available
  DeviceDriver->hide() ;
}

/** Destructor: disable all errors counting
 */
FecDialogImpl::~FecDialogImpl ( ) {

  try {
    delete fecAccessManager_ ;
    delete fecAccess_ ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Cannot delete hardware accesses", e) ; 
    //cerr << e.what() << endl ; 
  }

#ifdef DATABASEFACTORY
  delete fecFactory_ ;
#endif

  // for each qtCcuRedundancy => reset
  for (Sgi::hash_map<keyType, GroupCcuRedundancy *>::iterator p=ccuRedundancyInOut_.begin();p!=ccuRedundancyInOut_.end();p++) {

    GroupCcuRedundancy *qtCcuRedundancy = p->second ;
    delete qtCcuRedundancy ;
  }
  ccuRedundancyInOut_.clear() ;
}

/**
 */
void FecDialogImpl::helpAbout() {
  QMessageBox::about( this, "FecDialog -- About",
                      "<center><h1><font color=blue>FEC Debugger</font></h1></center>"
                      "<center><p><i>FEC debugger Graphic User Interface - Beta Version</i></p></center>"
                      "<center><p><i>Frédéric Drouhin - 2003</i></p></center>"
                      "<center><p><i>Email: fec-support@ires.in2p3.fr</i></p></center>"
                      );
}

/**
 */
void FecDialogImpl::quitTout() {

  close () ;
}

/** Display the errors coming from FecAccessManager download and upload operation
 */
void FecDialogImpl::displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList ) {

  if (errorList.size() == 0) return ;

  Sgi::hash_map<keyType, bool> errorDevice ; // in order to avoid multiple display of errors
  std::list<FecExceptionHandler *> deleteException ; // to delete the exception

  // simple message
  std::cout << message << ": found " << errorList.size() << " errors" << std::endl ;

  for (std::list<FecExceptionHandler *>::iterator p = errorList.begin() ; p != errorList.end() ; p ++) {

    FecExceptionHandler *e = *p ;

    if (e->getPositionGiven()) {

      if (!errorDevice[e->getHardPosition()]) {

	errorDevice[e->getHardPosition()] = true ;

	std::cerr << "******************************** ERROR *************************" << std::endl ;
	std::cerr << e->what() << std::endl ;
	std::cerr << "****************************************************************" << std::endl ;

	// Add to be deleted
	deleteException.push_back (e) ;	
      }
    }
    else {
      
      std::cerr << "******************************** ERROR *************************" << std::endl ;
      std::cerr << e->what() << std::endl ;
      std::cerr << "****************************************************************" << std::endl ;
      
      // Add to be deleted
      deleteException.push_back (e) ;	
    }
  }

  // Delete the exception (not redundant)
  for (std::list<FecExceptionHandler *>::iterator p = deleteException.begin() ; p != deleteException.end() ; p ++) {
    delete *p ;
  }
    
  // No more error
  errorList.clear() ;
}

/**
 */
void FecDialogImpl::ErrorMessage (const char *msg ) {

  char msg1[1000] ;
  sprintf (msg1, "<h4><font color=red>%s</font></h4>", msg) ; 
  QMessageBox::warning( this, "FecDialog Debugger -- Error", msg1 ) ;
}

/**
 */
void FecDialogImpl::ErrorMessage (const char *msg, const char *msg1 ) {

  char msg2[1000] ;
  sprintf (msg2, "<center><h4><font color=red>%s</font></h4></center><center><h4>%s</h4></center>", msg, msg1) ; 
  QMessageBox::warning( this, "FecDialog Debugger -- Error", msg2 ) ;
}

/**
 */
void FecDialogImpl::ErrorMessage (char *title, FecExceptionHandler e ) {

  char msg[1000] ;
  sprintf (msg, "<center><h4><font color=red>%s</font></h4></center><center><h4>%s</h4></center><p>Error Code %ld<br>", 
           title, e.getErrorMessage().c_str(), e.getErrorCode()) ;
  
  char msg1[80] ;
  if (e.getPositionGiven()) {

    decodeKey ( msg1, e.getHardPosition() ) ;
    strcat (msg, "Position: ") ;
    strcat (msg, msg1) ;
    strcat (msg, "</p>") ;
  }

  QMessageBox::critical( this, "FecDialog -- Exception", msg ) ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   FECS methods                                                      */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Disable the IRQ in the device driver level
 */
void FecDialogImpl::ddIRQEnableDisable ( ) {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      if (fecDisableIRQ->text() == "Disable IRQ") {

	fecAccess_->getFecRingDevice(buildFecRingKey(slot,ringSlot))->setIRQ(false) ;
	fecDisableIRQ->setText("Enable IRQ") ;
      }
      else {

	fecAccess_->getFecRingDevice(buildFecRingKey(slot,ringSlot))->setIRQ(true) ;
	fecDisableIRQ->setText("Disable IRQ") ;
      }

      fecReadAllRegisters( ) ;
      fecReadDDStatus( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during a FEC release (CR1)", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::scanForFecs() {

  fecSlots->clear() ;
  fecSlotsCcuRedundancy->clear() ;
  ccuFecSlots->clear() ;
  i2cFecSlots->clear() ;
  memoryFecSlots->clear() ;
  piaFecSlots->clear() ;
  //triggerFecSlots->clear() ;
  //jtagFecSlots->clear() ;
  trackerPiaFecSlots->clear() ;

  try {
    // Scan the PCI slot for device driver loaded
    // std::list<keyType> *fecSlotList = fecAccess_->scanForFECs (0, 4) ;
    std::list<keyType> *fecSlotList = fecAccess_->getFecList() ;

    if (fecSlotList != NULL) {

      for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
      
	keyType index = *p ;
      
	char msg[80] ;
	sprintf (msg, "%d,%d", getFecKey(index),getRingKey(index)) ;
	fecSlots->insertItem( msg );
	fecSlotsCcuRedundancy->insertItem( msg );
	ccuFecSlots->insertItem( msg );
	i2cFecSlots->insertItem( msg );
	memoryFecSlots->insertItem( msg );
	piaFecSlots->insertItem( msg );
	trackerPiaFecSlots->insertItem( msg );
	//triggerFecSlots->insertItem( msg );
	//jtagFecSlots->insertItem( msg );
      }
    }
    else {

      ErrorMessage ("No FEC device driver detected", "Please check if a FEC is available and install the FEC device driver." ) ;
      
      // ERROR
      // cout << "No FEC device driver detected" << endl ;
    }
  }
  catch (FecExceptionHandler &e) {

    ErrorMessage ("Error on the version of the device driver", e) ;
  }
}

/**
 */
void FecDialogImpl::fecPlxReset() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot,&ringSlot)) {

      fecAccess_->fecHardReset ( buildFecRingKey(slot,ringSlot) ) ;
      fecAccessManager_->removeDevices () ;

      // Clear all the tables
      fecClearAll() ;
      ccuClearAll() ;
      i2cClearAll() ;
      memoryClearAll() ;
      piaClearAll() ;
      //triggerClearAll() ;
      //jtagClearAll() ;
      trackerClearAll();
      redundancyClearAll() ;

      fecReadAllRegisters( ) ;

      // If a CCU scan is performed after the reset then a display is done with the CCU order
      displayCcuOrder_ = true ;
    }
    else {
      
      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during a PLX reset", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecSoftReset() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      fecAccess_->fecRingReset ( buildFecRingKey(slot,ringSlot) ) ;
      fecAccessManager_->removeDevices () ;

      // Clear all the tables
      fecClearAll() ;
      ccuClearAll() ;
      i2cClearAll() ;
      memoryClearAll() ;
      piaClearAll() ;
      //triggerClearAll() ;
      //jtagClearAll() ;
      trackerClearAll();
      redundancyClearAll() ;

      fecReadAllRegisters( ) ;

      // If a CCU scan is performed after the reset then a display is done with the CCU order
      displayCcuOrder_ = true ;
    }
    else {
      
      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during a FEC reset", e) ; // cerr << e.what() << endl ;
  }
}

/** Disable the FEC receive
 */
void FecDialogImpl::fecDisableReceiveFec() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot,&ringSlot)) {

      if (fecDisableReceive->text() == "Disable Receive") {
	fecAccess_->setFecRingCR0 ( buildFecRingKey(slot,ringSlot), fecAccess_->getFecRingCR0 ( buildFecRingKey(slot,ringSlot)) | FEC_CR0_DISABLERECEIVE ) ;

	fecDisableReceive->setText("Enable Receive") ;
      }
      else {
	fecAccess_->setFecRingCR0 ( buildFecRingKey(slot,ringSlot), fecAccess_->getFecRingCR0 ( buildFecRingKey(slot,ringSlot)) & ~FEC_CR0_DISABLERECEIVE ) ;

	fecDisableReceive->setText("Disable Receive") ;
      }

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during a FEC release (CR1)", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecReleaseFec() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      fecAccess_->setFecRingCR1 ( buildFecRingKey(slot,ringSlot), FEC_CR1_RELEASEFEC ) ;

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during a FEC release (CR1)", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecClearErrorsCR1() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      fecAccess_->setFecRingCR1 ( buildFecRingKey(slot,ringSlot), FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS ) ;

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error during the clear of errors (CR1)", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecReadCR0( ) {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
      
      uint fecCR0 = 0, timeout = 0 ;
      while (fecCR0 == 0 && timeout < 10) {
        fecCR0 = fecAccess_->getFecRingCR0 (buildFecRingKey(slot,ringSlot)) ;
        timeout ++ ;
      }

      char msg[80] ;
      sprintf (msg, "0x%04X", fecCR0) ;
      fecCR0InputLine->setText (msg) ;
      fecCR0Bis->setText (msg) ;
      
      if (fecCR0 & FEC_CR0_ENABLEFEC) fecCR0enableFEC->setChecked (true) ;
      else fecCR0enableFEC->setChecked (false) ;
      if (fecCR0 & FEC_CR0_SEND) fecCR0Send->setChecked (true) ;
      else fecCR0Send->setChecked (false) ;
      if (fecCR0 & FEC_CR0_XTALCLOCK) fecCR0InternalClock->setChecked (true) ;
      else fecCR0InternalClock->setChecked (false) ;
      if (fecCR0 & FEC_CR0_SELSEROUT) fecCR0SelSerOut->setChecked (true) ;
      else fecCR0SelSerOut->setChecked (false) ;
      if (fecCR0 & FEC_CR0_SELSERIN) fecCR0SelSerIn->setChecked (true) ;
      else fecCR0SelSerIn->setChecked (false) ;
      if (fecCR0 & FEC_CR0_RESETTCRX) fecCR0ResetTTCRx->setChecked (true) ;
      else fecCR0ResetTTCRx->setChecked (false) ;
      if (fecCR0 & FEC_CR0_POLARITY) fecCR0InvertPolarity->setChecked (true) ;
      else fecCR0InvertPolarity->setChecked (false) ;
      if (fecCR0 & FEC_CR0_DISABLERECEIVE) {
	fecCR0DisableRec->setChecked (true) ;
	fecDisableReceive->setText("Enable Receive") ;
      }
      else {
	fecCR0DisableRec->setChecked (false) ;
	fecDisableReceive->setText("Disable Receive") ;
      }
      if (fecCR0 & FEC_CR0_RESETFSMFEC) fecCR0ResetFSM->setChecked (true) ;
      else fecCR0ResetFSM->setChecked (false) ;
      if (fecCR0 & FEC_CR0_RESETRINGB) fecCR0ResetB->setChecked (true) ;
      else fecCR0ResetB->setChecked (false) ;
      if (fecCR0 & FEC_CR0_RESETOUT) fecCR0ResetOut->setChecked (true) ;
      else fecCR0ResetOut->setChecked (false) ;
      
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC CR0 is retrieved", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecWriteCR0_() {

  fecCR0InputLine->setText (fecCR0Bis->text()) ;
  fecWriteCR0() ;
}

/**
 */
void FecDialogImpl::fecWriteCR0() {

  try {

    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint value ;
      if (sscanf (fecCR0InputLine->text(), "%x", &value)) {
      
        fecAccess_->setFecRingCR0 (buildFecRingKey(slot,ringSlot), value) ;

	if ( (value & FEC_CR0_RESETOUT) || (value & FEC_CR0_RESETRINGB) ) {
	  // Reset the redundancy command
	  // Clear all the tables
	  fecClearAll() ;
	  ccuClearAll() ;
	  i2cClearAll() ;
	  memoryClearAll() ;
	  piaClearAll() ;
	  //triggerClearAll() ;
	  //jtagClearAll() ;
	  trackerClearAll();
	  redundancyClearAll() ;
	  //fecReadAllRegisters( ) ;
	  // If a CCU scan is performed after the reset then a display is done with the CCU order
	  displayCcuOrder_ = true ;
	}
      }
      else {

        ErrorMessage ("Wrong value in the FEC CR0 text field") ;

        // ERROR
        // cout << "Wrong value in the FEC CR0 text field" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }

    fecReadCR0 ( ) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC CR0 is written", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecWriteCR0Bit() {

  uint fecCR0 = 0 ;

  if (fecCR0enableFEC->isChecked ()) fecCR0 |= FEC_CR0_ENABLEFEC ;
  if (fecCR0Send->isChecked ()) fecCR0 |= FEC_CR0_SEND ;
  if (fecCR0InternalClock->isChecked ()) fecCR0 |= FEC_CR0_XTALCLOCK ;
  if (fecCR0SelSerOut->isChecked ()) fecCR0 |= FEC_CR0_SELSEROUT ;
  if (fecCR0SelSerIn->isChecked ()) fecCR0 |= FEC_CR0_SELSERIN ;
  if (fecCR0ResetTTCRx->isChecked ()) fecCR0 |= FEC_CR0_RESETTCRX ;
  if (fecCR0InvertPolarity->isChecked ()) fecCR0 |= FEC_CR0_POLARITY ;
  if (fecCR0DisableRec->isChecked ()) fecCR0 |= FEC_CR0_DISABLERECEIVE ;
  if (fecCR0ResetFSM->isChecked()) fecCR0 |= FEC_CR0_RESETFSMFEC ;
  if (fecCR0ResetB->isChecked ()) fecCR0 |= FEC_CR0_RESETRINGB ;
  if (fecCR0ResetOut->isChecked ()) fecCR0 |= FEC_CR0_RESETOUT ;

  char msg[80] ;
  sprintf (msg, "0x%04X", fecCR0) ;
  fecCR0InputLine->setText(msg) ;

  fecWriteCR0 ( ) ;
}

/**
 */
void FecDialogImpl::fecReadCR1() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
      
      uint fecCR1 = 0, timeout = 0 ;

      while (fecCR1 == 0 && timeout < 10) {
        fecCR1 = fecAccess_->getFecRingCR1 (buildFecRingKey(slot,ringSlot)) ;
        timeout ++ ;
      }

      char msg[80] ;
      sprintf (msg, "0x%04X", fecCR1) ;
      fecCR1InputLine->setText (msg) ;
      
      if (fecCR1 & FEC_CR1_CLEARIRQ) fecCR1ClearIrq->setChecked (true) ;
      else fecCR1ClearIrq->setChecked (false) ;
      if (fecCR1 & FEC_CR1_CLEARERRORS) fecCR1ClearErrors->setChecked (true) ;
      else fecCR1ClearErrors->setChecked (false) ;
      if (fecCR1 & FEC_CR1_RELEASEFEC) fecCR1Release->setChecked (true) ;
      else fecCR1Release->setChecked (false) ;
      
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC CR1 is retreived", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecWriteCR1() {
  try {

    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint value ;
      if (sscanf (fecCR1InputLine->text(), "%x", &value)) {
      
        fecAccess_->setFecRingCR1 (buildFecRingKey(slot,ringSlot), value) ;
      }
      else {

        ErrorMessage ("Wrong value in the FEC CR1 text field") ;
        // ERROR
        // cout << "Wrong value in the FEC CR1 text field" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }

    // Not read back value is write only
    //fecReadCR1 ( ) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC CR1 is written", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecWriteCR1Bit() {

  uint fecCR1 = 0 ;

  if (fecCR1ClearIrq->isChecked ()) fecCR1 |= FEC_CR1_CLEARIRQ ;
  if (fecCR1ClearErrors->isChecked ()) fecCR1 |= FEC_CR1_CLEARERRORS ;
  if (fecCR1Release->isChecked ()) fecCR1 |= FEC_CR1_RELEASEFEC ;

  char msg[80] ;
  sprintf (msg, "0x%04X", fecCR1) ;
  fecCR1InputLine->setText(msg) ;

  fecWriteCR1 ( ) ;
}

/**
 */
void FecDialogImpl::fecReadSR0() {

  try {

    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
      
      uint fecSR0 = 0, timeout = 0 ;

      while (fecSR0 == 0 && timeout < 10) {
        fecSR0 = fecAccess_->getFecRingSR0 (buildFecRingKey(slot,ringSlot)) ;
        timeout ++ ;
      }
      char msg[80] ;
      sprintf (msg, "0x%04X", fecSR0) ;
      fecSR0InputLine->setText (msg) ;
      fecSR0Bis->setText (msg) ;
      fecSR0Thread->setText (msg) ;
      
      if (fecSR0 & FEC_SR0_TRARUN) fecSR0TraRunning->setChecked (true) ;
      else fecSR0TraRunning->setChecked (false) ;
      if (fecSR0 & FEC_SR0_RECRUN) fecSR0RecRunning->setChecked (true) ;
      else fecSR0RecRunning->setChecked (false) ;
      if (fecSR0 & FEC_SR0_RECFULL) fecSR0RecFull->setChecked (true) ;
      else fecSR0RecFull->setChecked (false) ;
      if (fecSR0 & FEC_SR0_RECEMPTY) fecSR0RecEmpty->setChecked (true) ;
      else fecSR0RecEmpty->setChecked (false) ;
      if (fecSR0 & FEC_SR0_RETFULL) fecSR0RetFull->setChecked (true) ;
      else fecSR0RetFull->setChecked (false) ;
      if (fecSR0 & FEC_SR0_RETEMPTY) fecSR0RetEmpty->setChecked (true) ;
      else fecSR0RetEmpty->setChecked (false) ;
      if (fecSR0 & FEC_SR0_TRAFULL) fecSR0TraFull->setChecked (true) ;
      else fecSR0TraFull->setChecked (false) ;
      if (fecSR0 & FEC_SR0_TRAEMPTY) fecSR0TraEmpty->setChecked (true) ;
      else fecSR0TraEmpty->setChecked (false) ;
      if (fecSR0 & FEC_SR0_LINKINITIALIZED) {
	fecSR0LinkInit->setChecked (true) ;
	//LinkInitThreadLinkLed->setColor(Qt::green) ;
	LinkInitThreadLinkLed2->setPixmap(greenLed_) ;
      }
      else {
	fecSR0LinkInit->setChecked (false) ;
	//LinkInitThreadLinkLed->setColor(Qt::red) ;
	LinkInitThreadLinkLed2->setPixmap(redLed_) ;
      }
      if (fecSR0 & FEC_SR0_PENDINGIRQ) fecSR0PendingIrq->setChecked (true) ;
      else fecSR0PendingIrq->setChecked (false) ;
      if (fecSR0 & FEC_SR0_DATATOFEC) fecSR0DataToFec->setChecked (true) ;
      else fecSR0DataToFec->setChecked (false) ;
      if (fecSR0 & FEC_SR0_TTCRXREADY) fecSR0TTCRx->setChecked (true) ;
      else fecSR0TTCRx->setChecked (false) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC SR0 is retreived", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecReadSR1() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
      
      uint fecSR1 = fecAccess_->getFecRingSR1 (buildFecRingKey(slot,ringSlot)) ;
      char msg[80] ;
      sprintf (msg, "0x%04X", fecSR1) ;
      fecSR1InputLine->setText (msg) ;
      fecSR1Bis->setText (msg) ;
      
      if (fecSR1 & FEC_SR1_ILLDATA) fecSR1IllData->setChecked (true) ;
      else fecSR1IllData->setChecked (false) ;
      if (fecSR1 & FEC_SR1_ILLSEQ) fecCR1IllSeq->setChecked (true) ;
      else fecCR1IllSeq->setChecked (false) ;
      if (fecSR1 & FEC_SR1_CRCERROR) fecSR1CRCError->setChecked (true) ;
      else fecSR1CRCError->setChecked (false) ;
      if (fecSR1 & FEC_SR1_DATACOPIED) fecSR1DataCopied->setChecked (true) ;
      else fecSR1DataCopied->setChecked (false) ;
      if (fecSR1 & FEC_SR1_ADDRSEEN) fecSR1AddrSeen->setChecked (true) ;
      else fecSR1AddrSeen->setChecked (false) ;
      if (fecSR1 & FEC_SR1_ERROR) fecSR1Error->setChecked (true) ;
      else fecSR1Error->setChecked (false) ;
      if (fecSR1 & FEC_SR1_TIMEOUT) fecSR1Timeout->setChecked (true) ;
      else fecSR1Timeout->setChecked (false) ;
      if (fecSR1 & FEC_SR1_CLOCKERROR) fecSR1ClockError->setChecked (true) ;
      else fecSR1ClockError->setChecked (false) ;
      
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC SR1 is retreived", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::fecReadDDStatus() {

#ifndef FECSOFTWAREV2_0
  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) { 

      // Check all the status for the FECs
      // If you want only the status use the method FecAccess::getDeviceDriverStatus
      // or FecAccess::getDeviceDriverStatus (index) 
      tscType32 errorCounter[DD_NB_OF_ERRORS_TO_SPY+3] ;
      fecAccess_->getErrorCounters ( buildFecRingKey(slot,ringSlot), errorCounter ) ;

      if (isErrorCountingEnabled(errorCounter[0]))
	fecEnableErrorCounter->setChecked(true) ;
      else
	fecEnableErrorCounter->setChecked(false) ;

      if (isIrqOn( errorCounter[0])) {
	fecIRQEnableButton->setChecked(true) ;
	ccuRedundancyIRQEnableButton->setChecked(true) ;
	//fecDisableIRQ->setText("Enable IRQ") ;
      }
      else {
	fecIRQEnableButton->setChecked(false) ;
	ccuRedundancyIRQEnableButton->setChecked(false) ;
	//fecDisableIRQ->setText("Disable IRQ") ;
      }

      char msg[80] ; 
      sprintf (msg, "0x%04X", errorCounter[0]) ;
      fecDDStatus->setText (msg) ;
      trackerFecDDStatus->setText (msg) ;
      sprintf (msg, "%d", errorCounter[1]) ;
      fecLongFrame->setText (msg) ;
      trackerFecLongFrame->setText (msg) ;
      sprintf (msg, "%d", errorCounter[2]) ;
      fecShortFrame->setText (msg) ;
      trackerFecShortFrame->setText (msg) ;
      sprintf (msg, "%d", errorCounter[3]) ;
      fecBadTransaction->setText (msg) ;
      trackerFecBadTransaction->setText (msg) ;
      sprintf (msg, "%d", errorCounter[4]) ;
      fecWarningDevice->setText (msg); 
      trackerFecWarningDevice->setText (msg) ;
      sprintf (msg, "%d", errorCounter[5]) ;
      fecNbPlxReset->setText (msg) ;
      trackerFecNbPlxReset->setText (msg) ;
      sprintf (msg, "%d", errorCounter[6]) ;
      fecNbFecReset->setText (msg) ;
      trackerFecNbFecReset->setText (msg) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC device driver status is retreived", e) ; // cerr << e.what() << endl ;
  }
#endif
}

/**
 */
void FecDialogImpl::fecEnableCounters() {

#ifndef FECSOFTWAREV2_0
  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) { 

      trackerFecEnableErrorCounter->setChecked(!fecEnableErrorCounter->isChecked()) ;

      fecAccess_->setErrorCounting (buildFecRingKey(slot,ringSlot), !fecEnableErrorCounter->isChecked()) ; 

      fecReadDDStatus() ;
    }
    else {

      fecEnableErrorCounter->setChecked (false) ;

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 
          
    fecEnableErrorCounter->setChecked (true) ;

    // ERROR 
    ErrorMessage ("Error when the FEC device driver counters are enabled", e) ; // cerr << e.what() << endl ;
  }
#endif
}

/**
 */
void FecDialogImpl::fecResetErrorCounter() {

#ifndef FECSOFTWAREV2_0
  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) { 

      fecAccess_->resetErrorCounters (buildFecRingKey(slot,ringSlot)) ;

      fecReadDDStatus() ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC device driver counters are reseted", e) ; // cerr << e.what() << endl ;
  }
#endif
}

/**
 */
void FecDialogImpl::fecReadAllRegisters() {

  fecReadCR0( ) ;
  fecReadCR1( ) ;
  fecReadSR0( ) ;
  fecReadSR1( ) ;
  fecReadDDStatus ( ) ;
}

/** After a PLX reset, clear all values needed
 */
void FecDialogImpl::fecClearAll() {

  fecReadAllRegisters() ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   CCUS methods                                                      */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/**
 */
void FecDialogImpl::scanForCcus() {

  ccuAddresses->clear() ;
  ccuRedundancyAddresses->clear() ;
  i2cCcuAddresses->clear() ;
  memoryCcuAddresses->clear() ;
  piaCcuAddresses->clear() ;
  //triggerCcuAddresses->clear() ;
  //jtagCcuAddresses->clear() ;
  trackerPiaCcuAddresses->clear() ;

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      // Scan the ring for CCUs
      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( buildFecRingKey(slot,ringSlot) ) ;
      fecRingDevice->fecScanRingBroadcast ( ) ;

      // Scan the ring for each FECs and display the CCUs
      std::list<keyType> *ccuList = fecAccess_->getCcuList ( buildFecRingKey(slot,ringSlot) ) ;

      // display the order if it is set
      QString ccuDisplay = "No CCU found" ;

      if (ccuList != NULL) {

	bool firstCCU = true ;

        for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
      
          keyType index = *p ;
          if (getFecKey(index) == slot) {

            char msg[80] ;
            sprintf (msg, "0x%02X", getCcuKey(index)) ;
            ccuAddresses->insertItem( msg );
            ccuRedundancyAddresses->insertItem( msg );
            i2cCcuAddresses->insertItem( msg );
            memoryCcuAddresses->insertItem( msg );
            piaCcuAddresses->insertItem( msg );
            //triggerCcuAddresses->insertItem( msg );
            //jtagCcuAddresses->insertItem( msg );
            trackerPiaCcuAddresses->insertItem( msg );

	    // build the string to be displayed
	    if (displayCcuOrder_) {
	      if (firstCCU) {
		ccuDisplay = msg ;
		firstCCU = false ;
	      }
	      else
		ccuDisplay = ccuDisplay + " -> " + msg ;
	    }
          }
        }

        //ccuSelected() ;
      }
      else {

        ErrorMessage ("No CCU found on FECs") ;

        // ERROR
        // cout << "No CCU found on FECs" << endl ;
      }  

      if (displayCcuOrder_) {
	ccuOrderLabel->setText(ccuDisplay) ;
	displayCcuOrder_ = false ; // do not display it until next reset
      }

      // Delete = already store in the table of FEC device class
      delete ccuList ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Scan ring for CCU failed", e) ; // cerr << e.what() << endl ;      
  }

  // Set if the current CCU is a CCU25
  ccuSelected() ;
}

/**
 */
void FecDialogImpl::ccuSelected() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        // Clear ALL
        ccuClearAll() ;
        i2cClearAll() ;
        memoryClearAll() ;
        piaClearAll() ;
        //triggerClearAll() ;
        //jtagClearAll() ;

        // CCU 25 ?
        FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( buildFecRingKey(slot,ringSlot) ) ;
        tscType32 CRE ;
	if (fecRingDevice->getCcuType ( buildCompleteKey(slot,ringSlot,ccuAddress,0,0), &CRE )) {

          ccuIsACcu25->setChecked (true) ;
          i2cCcuIsACcu25->setChecked (true) ;
          memoryCcuIsACcu25->setChecked (true) ;
          piaCcuIsACcu25->setChecked (true) ;
          //triggerCcuIsACcu25->setChecked (true) ;
          //jtagCcuIsACcu25->setChecked (true) ;

          // Clear all the channels number
          i2cChannels->clear() ;
          for (uint i = 0x10 ; i < 0x20 ; i ++) {
            char msg[80] ;
            sprintf (msg, "%d", i) ;
            i2cChannels->insertItem ( msg ) ;
          }

          uint channel = 0x10 ;
          if ( fecRingDevice->isChannelEnabled(buildCompleteKey(slot,ringSlot,ccuAddress,channel,0))) 
            i2cEnable->setChecked(true) ;
          else
            i2cEnable->setChecked(false) ;

          // Clear all the channels number
          piaChannels->clear() ;
          trackerPiaChannels->clear() ;
          for (uint i = 0x30 ; i < 0x34 ; i ++) {
            char msg[80] ;
            sprintf (msg, "%d", i) ;
            piaChannels->insertItem ( msg ) ;
            trackerPiaChannels->insertItem ( msg ) ;
          }

          if ( fecRingDevice->isChannelEnabled(buildCompleteKey(slot,ringSlot,ccuAddress,0x40,0))) 
            memoryEnable->setChecked(true) ;
          else
            memoryEnable->setChecked(false) ;

          channel = 0x30 ;
          if ( fecRingDevice->isChannelEnabled(buildCompleteKey(slot,ringSlot,ccuAddress,channel,0))) 
            piaEnable->setChecked(true) ;
          else
            piaEnable->setChecked(false) ;
        }
        else {

	  char msg[80] ;
	  sprintf (msg, "CCU 0x%X", ccuAddress) ;
	  ErrorMessage ("The CCU found is an old CCU", msg) ;

          ccuIsACcu25->setChecked (false) ;
          i2cCcuIsACcu25->setChecked (false) ;
          memoryCcuIsACcu25->setChecked (false) ;
          piaCcuIsACcu25->setChecked (true) ;
          //triggerCcuIsACcu25->setChecked (true) ;
          //jtagCcuIsACcu25->setChecked (true) ;

          for (int i = 0x0 ; i < 0x10 ; i ++) {
            char msg[80] ;
            sprintf (msg, "%d", i) ;
            i2cChannels->insertItem ( msg ) ;
          }

          for (int i = 0x0 ; i < 0x10 ; i ++) {
            char msg[80] ;
            sprintf (msg, "%d", i) ;
            i2cChannels->insertItem ( msg ) ;
          }
        }

        i2cChannels->setCurrentItem(0) ;
        piaChannels->setCurrentItem(0) ;
        trackerPiaChannels->setCurrentItem(0) ;
      }
      else {

        ErrorMessage ("Error the ccu address enter is not correct or empty", "Please use scan CCUs before use read/write.") ;

        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }

    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("CCU type, operation failed", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRA() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        uint value ;
        if (sscanf (ccuCRA->text(), "%x", &value)) {

          keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
          fecAccess_->setCcuCRA (index, value) ;    
        }
        else {

          ErrorMessage ("Wrong value in the CCU CRA text field") ;
          // ERROR
          // cout << "Wrong value in the CCU CRA text field" << endl ;
        }

        ccuReadCRA() ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the CCU CRA", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRABit() {

  uint CRA = 0 ;

  if (ccuExternalReset->isChecked ()) CRA |= CCU_CRA_EXTRES ;
  if (ccuClearError->isChecked ()) CRA |= CCU_CRA_CLRE ;
  if (ccuResetAllChannels->isChecked ()) CRA |= CCU_CRA_RES ;

  char msg[80] ;
  sprintf (msg, "0x%04X", CRA) ;
  ccuCRA->setText(msg) ;

  ccuWriteCRA ( ) ;
}

/**
 */
void FecDialogImpl::ccuReadCRA() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint CRA = fecAccess_->getCcuCRA (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", CRA) ;
        ccuCRA->setText (msg) ;

        if (CRA & CCU_CRA_EXTRES) ccuExternalReset->setChecked (true) ;
        else ccuExternalReset->setChecked (false) ;
        if (CRA & CCU_CRA_CLRE) ccuClearError->setChecked (true) ;
        else ccuClearError->setChecked (false) ;
        if (CRA & CCU_CRA_RES) ccuResetAllChannels->setChecked (true) ;
        else ccuResetAllChannels->setChecked (false) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU CRA", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRB() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        uint value ;
        if (sscanf (ccuCRB->text(), "%x", &value)) {

          keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
          fecAccess_->setCcuCRB (index, value) ;    

        }
        else {

          ErrorMessage ("Wrong value in the CCU CRB text field") ;
          // ERROR
          // cout << "Wrong value in the CCU CRB text field" << endl ;
        }

        ccuReadCRB() ;
      }
      else {
        
        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the CCU CRB", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRBBit() {

  uint CRB = 0 ;

  // ????????????????????????????????????????????????????????
  // ???????????????????????????????????????????????????????
  // if (ccuExternalReset->isChecked ()) CRB |= 0x1 ;
  // if (ccuClearError->isChecked ()) CRB |= 0x2 ;
  // if (ccuResetAllChannels->isChecked ()) CRB |= 0x8 ;
  // ????????????????????????????????????????????????????????
  // ???????????????????????????????????????????????????????

  char msg[80] ;
  sprintf (msg, "0x%04X", CRB) ;
  ccuCRB->setText(msg) ;

  ccuWriteCRB ( ) ;
}

/**
 */
void FecDialogImpl::ccuReadCRB() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint CRB = fecAccess_->getCcuCRB (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", CRB) ;
        ccuCRB->setText (msg) ;

        if (CRB & 0x1) ccuEnableAlarm1->setChecked (true) ;
        else ccuEnableAlarm1->setChecked (false) ;
        if (CRB & 0x2) ccuEnableAlarm2->setChecked (true) ;
        else ccuEnableAlarm2->setChecked (false) ;
        if (CRB & 0x4) ccuEnableAlarm3->setChecked (true) ;
        else ccuEnableAlarm3->setChecked (false) ;
        if (CRB & 0x8) ccuEnableAlarm4->setChecked (true) ;
        else ccuEnableAlarm4->setChecked (false) ;
        switch (CRB & 0x30) {
        case 0x0: 
          ccuNoRety->setChecked (true) ;
          ccuRetryOnce->setChecked (false) ;
          ccuRetryTwice->setChecked (false) ;
          ccuRetryFourth->setChecked (false) ;
          break ;
        case 0x1:
          ccuNoRety->setChecked (false) ;
          ccuRetryOnce->setChecked (true) ;
          ccuRetryTwice->setChecked (false) ;
          ccuRetryFourth->setChecked (false) ;
          break ;
        case 0x2:
          ccuNoRety->setChecked (false) ;
          ccuRetryOnce->setChecked (false) ;
          ccuRetryTwice->setChecked (true) ;
          ccuRetryFourth->setChecked (false) ;
          break ;
        case 0x3:
          ccuNoRety->setChecked (false) ;
          ccuRetryOnce->setChecked (false) ;
          ccuRetryTwice->setChecked (false) ;
          ccuRetryFourth->setChecked (true) ;
          break ;
        }
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU CRB", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRC() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        uint value ;
        if (sscanf (ccuCRC->text(), "%x", &value)) {

          keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
          fecAccess_->setCcuCRC (index, value) ;    

        }
        else {

          ErrorMessage ("Wrong value in the CCU CRC text field") ;
          // ERROR
          // cout << "Wrong value in the CCU CRC text field" << endl ;
        }

        ccuReadCRC() ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the CCU CRC", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRCBit() {

  uint CRC = 0 ;

  if (ccuSelectInputB->isChecked ()) CRC |= 0x1 ;
  if (ccuSelectOuputB->isChecked ()) CRC |= 0x2 ;

  char msg[80] ;
  sprintf (msg, "0x%04X", CRC) ;
  ccuCRC->setText(msg) ;

  ccuWriteCRC ( ) ;
}

/**
 */
void FecDialogImpl::ccuReadCRC() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint CRC = fecAccess_->getCcuCRC (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", CRC) ;
        ccuCRC->setText (msg) ;

        if (CRC & 0x1) {

          ccuSRCInputA->setChecked (false) ;
          ccuSRCInputB->setChecked (true) ;

          ccuSelectInputB->setChecked (true) ;
        }
        else {

          ccuSRCInputA->setChecked (true) ;
          ccuSRCInputB->setChecked (false) ;

          ccuSelectInputB->setChecked (false) ;
        }

        if (CRC & 0x2) ccuSelectOuputB->setChecked (true) ;
        else ccuSelectOuputB->setChecked (false) ;

        

      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU CRC", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRD() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        uint value ;
        if (sscanf (ccuCRD->text(), "%x", &value)) {

          keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
          fecAccess_->setCcuCRD (index, value) ;    

        }
        else {

          ErrorMessage ("Wrong value in the CCU CRD text field") ;
          // ERROR
          // cout << "Wrong value in the CCU CRD text field" << endl ;
        }

        ccuReadCRD() ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the CCU CRD", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuReadCRD() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint CRD = fecAccess_->getCcuCRD (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", CRD) ;
        ccuCRD->setText (msg) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU CRD", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCRE() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        uint value ;
        if (sscanf (ccuCRE->text(), "%x", &value)) {

          keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
          fecAccess_->setCcuCRE (index, value) ;
        }
        else {
          
          ErrorMessage ("Wrong value in the CCU CRE text field") ;
          // ERROR
          // cout << "Wrong value in the CCU CRE text field" << endl ;
        }

        i2cIsChannelEnable() ;
        memoryIsChannelEnable() ;
        piaIsChannelEnable() ;
        //jtagIsChannelEnable();
        //triggerIsChannelEnable() ;

        ccuReadCRE() ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the CCU CRE", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuWriteCREBit() {

  uint CRE = 0 ;

  if (ccuCREI2C1->isChecked ()) CRE |= 0x1 ;
  if (ccuCREI2C2->isChecked ()) CRE |= 0x2 ;
  if (ccuCREI2C3->isChecked ()) CRE |= 0x4 ;
  if (ccuCREI2C4->isChecked ()) CRE |= 0x8 ;
  if (ccuCREI2C5->isChecked ()) CRE |= 0x10 ;
  if (ccuCREI2C6->isChecked ()) CRE |= 0x20 ;
  if (ccuCREI2C7->isChecked ()) CRE |= 0x40 ;
  if (ccuCREI2C8->isChecked ()) CRE |= 0x80 ;
  if (ccuCREI2C9->isChecked ()) CRE |= 0x100 ;
  if (ccuCREI2C10->isChecked ()) CRE |= 0x200 ;
  if (ccuCREI2C11->isChecked ()) CRE |= 0x400 ;
  if (ccuCREI2C12->isChecked ()) CRE |= 0x800 ;
  if (ccuCREI2C13->isChecked ()) CRE |= 0x1000 ;
  if (ccuCREI2C14->isChecked ()) CRE |= 0x2000 ;
  if (ccuCREI2C15->isChecked ()) CRE |= 0x4000 ;
  if (ccuCREI2C16->isChecked ()) CRE |= 0x8000 ;
  if (ccuCREPIA1->isChecked ()) CRE |= 0x10000 ;
  if (ccuCREPIA2->isChecked ()) CRE |= 0x20000 ;
  if (ccuCREPIA3->isChecked ()) CRE |= 0x40000 ;
  if (ccuCRETrigger->isChecked ()) CRE |= 0x80000 ;
  if (ccuCREJTAG->isChecked ()) CRE |= 0x100000 ;
  if (ccuCREMemory->isChecked ()) CRE |= 0x200000 ;

  char msg[80] ;
  sprintf (msg, "0x%06X", CRE) ;
  ccuCRE->setText(msg) ;

  ccuWriteCRE ( ) ;
}

/**
 */
void FecDialogImpl::ccuReadCRE() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint CRE = fecAccess_->getCcuCRE (index) ;

        char msg[80] ;
        sprintf (msg, "0x%06X", CRE) ;
        ccuCRE->setText (msg) ;

        if (CRE & 0x1) ccuCREI2C1->setChecked (true) ;
        else ccuCREI2C1->setChecked (false) ;
       if (CRE & 0x2) ccuCREI2C2->setChecked (true) ;
        else ccuCREI2C2->setChecked (false) ;
       if (CRE & 0x4) ccuCREI2C3->setChecked (true) ;
        else ccuCREI2C3->setChecked (false) ;
       if (CRE & 0x8) ccuCREI2C4->setChecked (true) ;
        else ccuCREI2C4->setChecked (false) ;
       if (CRE & 0x10) ccuCREI2C5->setChecked (true) ;
        else ccuCREI2C5->setChecked (false) ;
       if (CRE & 0x20) ccuCREI2C6->setChecked (true) ;
        else ccuCREI2C6->setChecked (false) ;
       if (CRE & 0x40) ccuCREI2C7->setChecked (true) ;
        else ccuCREI2C7->setChecked (false) ;
       if (CRE & 0x80) ccuCREI2C8->setChecked (true) ;
        else ccuCREI2C8->setChecked (false) ;
       if (CRE & 0x100) ccuCREI2C9->setChecked (true) ;
        else ccuCREI2C9->setChecked (false) ;
       if (CRE & 0x200) ccuCREI2C10->setChecked (true) ;
        else ccuCREI2C10->setChecked (false) ;
       if (CRE & 0x400) ccuCREI2C11->setChecked (true) ;
        else ccuCREI2C11->setChecked (false) ;
       if (CRE & 0x800) ccuCREI2C12->setChecked (true) ;
        else ccuCREI2C12->setChecked (false) ;
       if (CRE & 0x1000) ccuCREI2C13->setChecked (true) ;
        else ccuCREI2C13->setChecked (false) ;
       if (CRE & 0x2000) ccuCREI2C14->setChecked (true) ;
        else ccuCREI2C14->setChecked (false) ;
       if (CRE & 0x4000) ccuCREI2C15->setChecked (true) ;
        else ccuCREI2C15->setChecked (false) ;
       if (CRE & 0x8000) ccuCREI2C16->setChecked (true) ;
        else ccuCREI2C16->setChecked (false) ;
       if (CRE & 0x10000) ccuCREPIA1->setChecked (true) ;
        else ccuCREPIA1->setChecked (false) ;
       if (CRE & 0x20000) ccuCREPIA2->setChecked (true) ;
        else ccuCREPIA2->setChecked (false) ;
       if (CRE & 0x40000) ccuCREPIA3->setChecked (true) ;
        else ccuCREPIA3->setChecked (false) ;
       if (CRE & 0x80000) ccuCRETrigger->setChecked (true) ;
        else ccuCRETrigger->setChecked (false) ;
       if (CRE & 0x100000) ccuCREJTAG->setChecked (true) ;
        else ccuCREJTAG->setChecked (false) ;
       if (CRE & 0x200000) ccuCREMemory->setChecked (true) ;
        else ccuCREMemory->setChecked (false) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU CRE", e) ; // cerr << e.what() << endl ;      
  }
}


/**
 */
void FecDialogImpl::ccuReadSRA() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRA = fecAccess_->getCcuSRA (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", SRA) ;
        ccuSRA->setText (msg) ;

        if (SRA & 0x1) ccuSRACRCError->setChecked (true) ;
        else ccuSRACRCError->setChecked (false) ;
        if (SRA & 0x2) ccuSRAInternalError->setChecked (true) ;
        else ccuSRAInternalError->setChecked (false) ;
        if (SRA & 0x4) ccuSRAAlarmInputActive->setChecked (true) ;
        else ccuSRAAlarmInputActive->setChecked (false) ;
        if (SRA & 0x8) ccuSRACCUParityError->setChecked (true) ;
        else ccuSRACCUParityError->setChecked (false) ;
        if (SRA & 0x10) ccuSRAChannelParityError->setChecked (true) ;
        else ccuSRAChannelParityError->setChecked (false) ;
        if (SRA & 0x20) ccuSRAIllegalSequence->setChecked (true) ;
        else ccuSRAIllegalSequence->setChecked (false) ;
        if (SRA & 0x40) ccuSRAInvalidCommand->setChecked (true) ;
        else ccuSRAInvalidCommand->setChecked (false) ;
        if (SRA & 0x80) ccuSRAGlobalError->setChecked (true) ;
        else ccuSRAGlobalError->setChecked (false) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU SRA", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuReadSR() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        // SRB
        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRB = fecAccess_->getCcuSRB (index) ;
        char msg[80] ;
        sprintf (msg, "0x%04X", SRB) ;
        ccuSRB->setText (msg) ;

        // SRC
        index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRC = fecAccess_->getCcuSRC (index) ;
        if (SRC & 0x1) {
          ccuSRCInputA->setChecked (false) ;
          ccuSRCInputB->setChecked (true) ;
        }
        else {
          ccuSRCInputA->setChecked (true) ;
          ccuSRCInputB->setChecked (false) ;
        }

        // SRD
        index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRD = fecAccess_->getCcuSRD (index) ;
        sprintf (msg, "0x%04X", SRD) ;
        ccuSRD->setText (msg) ;

        // SRF
        index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRF = fecAccess_->getCcuSRD (index) ;
        sprintf (msg, "0x%04X", SRF) ;
        ccuSRF->setText (msg) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read a CCU status register", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::ccuReadSRE() {

  try {
    uint slot, ringSlot; 
    if (ccuFecSlots->count() && sscanf (ccuFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint ccuAddress ;
      if (ccuAddresses->count() && sscanf (ccuAddresses->currentText(), "%x", &ccuAddress)) {

        keyType index = buildCompleteKey(slot,ringSlot,ccuAddress,0,0) ;
        uint SRE = fecAccess_->getCcuSRE (index) ;

        char msg[80] ;
        sprintf (msg, "0x%04X", SRE) ;
        ccuSRE->setText (msg) ;

        if (SRE & 0x1) ccuSREI2C1->setChecked (true) ;
        else ccuSREI2C1->setChecked (false) ;
        if (SRE & 0x2) ccuSREI2C2->setChecked (true) ;
        else ccuSREI2C2->setChecked (false) ;
        if (SRE & 0x4) ccuSREI2C3->setChecked (true) ;
        else ccuSREI2C3->setChecked (false) ;
        if (SRE & 0x8) ccuSREI2C4->setChecked (true) ;
        else ccuSREI2C4->setChecked (false) ;
        if (SRE & 0x10) ccuSREI2C5->setChecked (true) ;
        else ccuSREI2C5->setChecked (false) ;
        if (SRE & 0x20) ccuSREI2C6->setChecked (true) ;
        else ccuSREI2C6->setChecked (false) ;
        if (SRE & 0x40) ccuSREI2C7->setChecked (true) ;
        else ccuSREI2C7->setChecked (false) ;
        if (SRE & 0x80) ccuSREI2C8->setChecked (true) ;
        else ccuSREI2C8->setChecked (false) ;
        if (SRE & 0x100) ccuSREI2C9->setChecked (true) ;
        else ccuSREI2C9->setChecked (false) ;
        if (SRE & 0x200) ccuSREI2C10->setChecked (true) ;
        else ccuSREI2C10->setChecked (false) ;
        if (SRE & 0x400) ccuSREI2C11->setChecked (true) ;
        else ccuSREI2C11->setChecked (false) ;
        if (SRE & 0x800) ccuSREI2C12->setChecked (true) ;
        else ccuSREI2C12->setChecked (false) ;
        if (SRE & 0x1000) ccuSREI2C13->setChecked (true) ;
        else ccuSREI2C13->setChecked (false) ;
        if (SRE & 0x2000) ccuSREI2C14->setChecked (true) ;
        else ccuSREI2C14->setChecked (false) ;
        if (SRE & 0x4000) ccuSREI2C15->setChecked (true) ;
        else ccuSREI2C15->setChecked (false) ;
        if (SRE & 0x8000) ccuSREI2C16->setChecked (true) ;
        else ccuSREI2C16->setChecked (false) ;
      }
      else {

        ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
        // ERROR
        // cout << "Error the ccu address enter is not correct or empty" << endl ;
        // cout << "Please use scan CCUs before use read/write" << endl ;
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the CCU SRE", e) ; // cerr << e.what() << endl ;      
  }
}

/**
 */
void FecDialogImpl::scanFecsCcusDevices () {

  scanForFecs() ;
  scanForCcus() ;
  //scanForI2CDevices() ;
}

/**
 */
void FecDialogImpl::ccuEnableAllChannels() {

  ccuCRE->setText ("0x3FFFFF") ;
  ccuWriteCRE() ;
}

/**
 */
void FecDialogImpl::ccuReadAll() {

  ccuReadCRA() ;
  ccuReadCRB() ;
  ccuReadCRC() ;
  ccuReadCRD() ;
  ccuReadCRE() ;
  ccuReadSRA() ;
  ccuReadSR () ;
  ccuReadSRE() ;
}

/** After a reset, clear all the tables needed
 */
void FecDialogImpl::ccuClearAll() {

  // The CCU continues to be alive so the ccuAddresses is not cleared
  // if (ccuAddresses->count()) ccuReadAll() ;
}

/** Modify the FEC slot in all combo boxes
 */
void FecDialogImpl::fecSlotsChange ( int index ) {

  fecSlots->setCurrentItem(index) ;
  ccuFecSlots->setCurrentItem(index) ;
  i2cFecSlots->setCurrentItem(index) ;
  memoryFecSlots->setCurrentItem(index) ;
  piaFecSlots->setCurrentItem(index) ;
  fecSlotsCcuRedundancy->setCurrentItem(index) ;
}

/** For modification of the FEC slots
 * fecSlots
 */
void FecDialogImpl::fecSlotsModify ( ) {
 
  fecSlotsChange (fecSlots->currentItem ()) ;
}

/** For modification of the FEC slots
 * ccuFecSlots
 */
void FecDialogImpl::fecSlotsCcuModify ( ) {
  
  fecSlotsChange (ccuFecSlots->currentItem ()) ;
}

/** For modification of the FEC slots
 * i2cFecSlots
 */
void FecDialogImpl::fecSlotsI2cModify ( ) {
  
  fecSlotsChange (i2cFecSlots->currentItem ()) ;
}

/** For modification of the FEC slots
 * memoryFecSlots
 */
void FecDialogImpl::fecSlotsMemoryModify ( ) {
  
  fecSlotsChange (memoryFecSlots->currentItem ()) ;
}

/** For modification of the FEC slots
 * piaFecSlots
 */
void FecDialogImpl::fecSlotsPiaModify ( ) {
  
  fecSlotsChange (piaFecSlots->currentItem ()) ;
}

/** For modification of the FEC slots
 * fecSlotsCcuRedundancy
 */
void FecDialogImpl::fecSlotsRedundancyModify ( ) {
  
  fecSlotsChange (fecSlotsCcuRedundancy->currentItem ()) ;
}


/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   I2C channel methods                                               */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Try to retreive the i2C key and check if the channel is enable or not
 * \param checkInit - check if the channel is initialised
 * \return index of the channel
 * \exception FecExceptionHandler
 */
keyType FecDialogImpl::getI2CKey ( bool checkInit ) {

  keyType index = 0 ;

  uint slot, ringSlot; 
  if (i2cFecSlots->count() && sscanf (i2cFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
    
    uint ccuAddress ;
    if (i2cCcuAddresses->count() && sscanf (i2cCcuAddresses->currentText(), "%x", &ccuAddress)) {
      
      uint channel ;
      if (i2cChannels->count() && sscanf (i2cChannels->currentText(), "%d", &channel)) {
        
        index = buildCompleteKey(slot,ringSlot,ccuAddress,channel,0) ;
        FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
        if (fecRingDevice->isChannelEnabled(index)) i2cEnable->setChecked(true) ;
        else i2cEnable->setChecked(false) ;

        if (checkInit) {
          if (! i2cEnable->isChecked()) {
            index = 0 ;

            ErrorMessage ("The channel is not enable, please enable the channel before performed any operation") ;
            // ERROR
            // cout << "The channel is not enable, please enable the channel before performed any operation" << endl ;
          }
        }
      }
      else {
          
        ErrorMessage ("Error the channel number enter is not correct or empty") ;
        // ERROR
        // cout << "Error the channel number enter is not correct or empty" << endl ;
      }
    }
    else {
      
      ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
      // ERROR
      // cout << "Error the ccu address enter is not correct or empty" << endl ;
      // cout << "Please use scan CCUs before use read/write" << endl ;
    }
  }
  else {

    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

    // ERROR
    // cout << "Error the slot enter is not correct or empty" << endl ;
    // cout << "Please scan FECs before use read/write" << endl ;
  }

  return (index) ;
}

/**
 */
void FecDialogImpl::i2cSetModeExtended() {

  i2cNormalMode->setChecked(false) ;
  //i2cExtendedMode->setChecked(true) ;
  i2cRalMode->setChecked(false) ;
}
/**
 */
void FecDialogImpl::i2cSetModeNormal() {

  //i2cNormalMode->setChecked(true) ;
  i2cExtendedMode->setChecked(false) ;
  i2cRalMode->setChecked(false) ;
}
/**
 */
void FecDialogImpl::i2cSetModeRal() {

  i2cNormalMode->setChecked(false) ;
  i2cExtendedMode->setChecked(false) ;
  //i2cRalMode->setChecked(true) ;
}

/**
 */
void FecDialogImpl::i2cSetMaskAnd() {

  //i2cAndOperation->setChecked(true) ;
  i2cOrOperation->setChecked(false) ;
  i2cXorOperation->setChecked(false) ;
}

/**
 */
void FecDialogImpl::i2cSetMaskOr() {

  i2cAndOperation->setChecked(false) ;
  //i2cOrOperation->setChecked(true) ;
  i2cXorOperation->setChecked(false) ;
}

/**
 */
void FecDialogImpl::i2cSetMaskXor() {

  i2cAndOperation->setChecked(false) ;
  i2cOrOperation->setChecked(false) ;
  //i2cXorOperation->setChecked(true) ;
}

/**
 */
void FecDialogImpl::i2cIsChannelEnable() {

  try {

    getI2CKey(false) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to know if the channel is enable or not", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cEnableChannel() {

  try {

    bool init = i2cEnable->isChecked() ;
    keyType index = getI2CKey(false) ;

    if (index != 0) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( index ) ;
      
      fecRingDevice->setChannelEnable (index, init) ;
      i2cEnable->setChecked(init) ;
    }
    else
      i2cEnable->setChecked(false) ;
  }
  catch (FecExceptionHandler &e) { 
    
    i2cEnable->setChecked(false) ;
    
    // ERROR 
    ErrorMessage ("Unable to enable an i2c channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cWriteCRA() {

  try {

    keyType index = getI2CKey(true) ;

    if (index != 0) {    

      i2cEnable->setChecked(true) ;
      uint value ;
      if (sscanf (i2cCRA->text(), "%x", &value)) {
          
        fecAccess_->seti2cChannelCRA (index, value) ;
      }
      else {
         
        ErrorMessage ("Wrong value in the I2C CRA text field") ;
        // ERROR
        // cout << "Wrong value in the I2C CRA text field" << endl ;
      }
        
      i2cReadCRA() ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the i2c CRA", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cReadCRA() {

  try {

    keyType index = getI2CKey(true) ;
    
    if (index != 0) {
      
      uint CRA = fecAccess_->geti2cChannelCRA (index) ;

      char msg[80] ;
      sprintf (msg, "0x%04X", CRA) ;
      i2cCRA->setText (msg) ;

      switch (CRA & 0x3) {
        
      case 0x0:
        i2cSpeed100->setChecked(true) ;
        i2cSpeed200->setChecked(false) ;
        i2cSpeed400->setChecked(false) ;
        i2cSpeed1000->setChecked(false) ;
        break ;
      case 0x1:
        i2cSpeed100->setChecked(false) ;
        i2cSpeed200->setChecked(true) ;
        i2cSpeed400->setChecked(false) ;
        i2cSpeed1000->setChecked(false) ;
        break ;
      case 0x2:
        i2cSpeed100->setChecked(false) ;
        i2cSpeed200->setChecked(false) ;
        i2cSpeed400->setChecked(true) ;
        i2cSpeed1000->setChecked(false) ;
        break;
      case 0x3:
        i2cSpeed100->setChecked(false) ;
        i2cSpeed200->setChecked(false) ;
        i2cSpeed400->setChecked(false) ;
        i2cSpeed1000->setChecked(true) ;
        break ;
      }

      if (CRA & 0x20) 
        i2cEnableBroadCast->setChecked(true) ;
      else 
        i2cEnableBroadCast->setChecked(false) ;
      
      if (CRA & 0x40) 
        i2cForceAcknowledge->setChecked(true) ;
      else 
        i2cForceAcknowledge->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the i2c CRA", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cWriteCRABit() {

  uint CRA = 0 ;
  if (i2cSpeed100->isChecked()) CRA = 0 ;
  else
    if (i2cSpeed200->isChecked()) CRA = 1 ;
    else
      if (i2cSpeed400->isChecked()) CRA = 2 ;
      else
        if (i2cSpeed1000->isChecked()) CRA = 3 ;
  

  if (i2cEnableBroadCast->isChecked()) CRA |= 0x20 ;
  if (i2cForceAcknowledge->isChecked()) CRA |= 0x40 ;

  char msg[80] ;
  sprintf (msg, "0x%04X", CRA) ;
  i2cCRA->setText (msg) ;

  i2cWriteCRA() ;
}

/**
 */
void FecDialogImpl::i2cSetSpeed100 () {

  bool error = true ;
  uint speed = 0 ; // 100 KHz

  try {

    keyType index = getI2CKey(true) ;
    
    if (index != 0) {

      uint CRA = fecAccess_->geti2cChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;

      fecAccess_->seti2cChannelCRA (index, CRA) ;
      
      i2cReadCRA() ;

      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the i2c speed (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    i2cSpeed100->setChecked(true) ;
  else
    i2cSpeed100->setChecked(false) ;
  
  i2cSpeed200->setChecked(false) ;
  i2cSpeed400->setChecked(false) ;
  i2cSpeed1000->setChecked(false) ;
}


/**
 */
void FecDialogImpl::i2cSetSpeed200 () {

  bool error = true ;
  uint speed = 1 ; // 200 KHz

  try {

    keyType index = getI2CKey(true) ;

    if (index != 0) {

      uint CRA = fecAccess_->geti2cChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;

      fecAccess_->seti2cChannelCRA (index, CRA) ;
      
      i2cReadCRA() ;
      
      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the i2c speed (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    i2cSpeed200->setChecked(true) ;
  else
    i2cSpeed200->setChecked(false) ;
  
  i2cSpeed100->setChecked(false) ;
  i2cSpeed400->setChecked(false) ;
  i2cSpeed1000->setChecked(false) ;
}


/**
 */
void FecDialogImpl::i2cSetSpeed400 () {

  bool error = true ;
  uint speed = 2 ; // 400 KHz

  try {

    keyType index = getI2CKey(true) ;

    if (index != 0) {

      uint CRA = fecAccess_->geti2cChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;

      fecAccess_->seti2cChannelCRA (index, CRA) ;
      
      i2cReadCRA() ;

      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the i2c speed (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    i2cSpeed400->setChecked(true) ;
  else
    i2cSpeed400->setChecked(false) ;

  i2cSpeed100->setChecked(false) ;  
  i2cSpeed200->setChecked(false) ;
  i2cSpeed1000->setChecked(false) ;
}


/**
 */
void FecDialogImpl::i2cSetSpeed1000 () {

  bool error = true ;
  uint speed = 3 ; // 1000 KHz

  try {

    keyType index = getI2CKey(true) ;
    
    if (index != 0) {

      uint CRA = fecAccess_->geti2cChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;

      fecAccess_->seti2cChannelCRA (index, CRA) ;
      
      i2cReadCRA() ;
      
      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the i2c speed (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    i2cSpeed1000->setChecked(true) ;
  else
    i2cSpeed1000->setChecked(false) ;

  i2cSpeed100->setChecked(false) ;  
  i2cSpeed200->setChecked(false) ;
  i2cSpeed400->setChecked(false) ;
}

/**
 */
void FecDialogImpl::i2cResetChannel() {

  try {

    keyType index = getI2CKey(true) ;

    if (index != 0) { 
      fecAccess_->i2cChannelReset (index) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to reset an i2c channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cReadSRA() {

  try {

    keyType index = getI2CKey(true) ;
    
    if (index != 0) { 

      uint SRA = fecAccess_->geti2cChannelSRA (index) ;

      char msg[80] ;
      sprintf (msg, "0x%04X", SRA) ;
      i2cSRA->setText (msg) ;

      if (SRA & 0x4) 
        i2cLastTrSuccessfull->setChecked(true) ;
      else 
        i2cLastTrSuccessfull->setChecked(false) ;
      
      if (SRA & 0x8) 
        i2cLow->setChecked(true) ;
      else 
        i2cLow->setChecked(false) ;
      
      if (SRA & 0x20) 
        i2cInvalidCommand->setChecked(true) ;
      else 
        i2cInvalidCommand->setChecked(false) ;
      
      if (SRA & 0x40) 
        i2cLastOperationNotAck->setChecked(true) ;
      else 
        i2cLastOperationNotAck->setChecked(false) ;

      if (SRA & 0x80) 
        i2cGlobalError->setChecked(true) ;
      else 
        i2cGlobalError->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the i2c channel SRA", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cReadSR() {

  try {

    keyType index = getI2CKey(true) ;
    
    if (index != 0) {

      char msg[80] ;

      uint SRB = fecAccess_->geti2cChannelSRB (index) ;
      sprintf (msg, "0x%04X", SRB) ;
      i2cSRB->setText (msg) ;
      
      uint SRC = fecAccess_->geti2cChannelSRC (index) ;
      sprintf (msg, "0x%04X", SRC) ;
      i2cSRC->setText (msg) ;
      
      uint SRD = fecAccess_->geti2cChannelSRD (index) ;
      sprintf (msg, "0x%04X", SRD) ;
      i2cSRD->setText (msg) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read an i2c channel status register", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cReadI2c() {

  bool init = false ;
  keyType index = 0 ;

  try {

    index = getI2CKey(true) ;

    if (index != 0) {

      uint address ;
      if (sscanf (i2cAddressAccess->text(), "%x", &address)) {

        index |= setAddressKey(address) ;
        uint value = 0 ;

        if (i2cNormalMode->isChecked()) {

          // Create an access
          fecAccess_->addi2cAccess (index,PHILIPS,MODE_SHARE) ;
          init = true ;
          
          // Check the offset
          uint offset ;
          if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) offset = 0 ;
          // Check the shift
          uint decal ;
          if (sscanf (i2cShiftAccess->text(), "%x", &decal)) decal = 0 ;

          if (offset == 0 && decal == 0)
            value = fecAccess_->read ( index ) ;
          else
            if (decal == 0)
              value = fecAccess_->readOffset ( index, offset ) ;
            else
              value = fecAccess_->readOffset ( index, offset, decal ) ;
        }
        else if ( i2cExtendedMode->isChecked() ) {

          // Create an access
          fecAccess_->addi2cAccess (index,FOREXTENDED,MODE_SHARE) ;
          init = true ;

          // Check the offset
          uint offset ;
          if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) {
            
            value = fecAccess_->read ( index, offset ) ;
          }
          else {
            
            ErrorMessage ("Offset for i2c extended read is not correct") ;
            // ERROR
            // cout << "Offset for i2c extended read is not correct" << endl ;
          }
        }
        else if ( i2cRalMode->isChecked() ) {

          // Create an access
          fecAccess_->addi2cAccess (index,APV25,MODE_SHARE) ;
          init = true ;

          // Check the offset
          uint offset ;
          if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) {
            
            value = fecAccess_->read ( index, offset ) ;
          }
          else {
            
            ErrorMessage ("Offset for i2c RAL read is not correct") ;
            // ERROR
            // cout << "Offset for i2c RAL read is not correct" << endl ;
          }
            }
        else {
          
          ErrorMessage ("No mode is specified") ;
          // ERROR
          // cout << "No mode is specified" << endl ;
        }

        char msg[80] ;
        sprintf (msg, "0x%04X", value) ;
        i2cDataAccess->setText (msg) ;

        if (i2cAndOperation->isChecked() ||
            i2cOrOperation->isChecked()  ||
            i2cXorOperation->isChecked() ) {

          // ??????????????????????????????????????????????????
          // Read the mask registers
          value = 0 ;
          sprintf (msg, "0x%04X", value) ;
          i2cDataAccess->setText (msg) ;
          cout << "Read mask register Not implemented" << endl ;
        }
      }
      else {

        ErrorMessage ("I2C address is not correct") ;
        // ERROR
        // cout << "I2C address is not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 
    
    // ERROR 
    ErrorMessage ("Unable to read an i2c device", e) ; // cerr << e.what() << endl ;
  }

  // Remove the access
  try {
    if (init) fecAccess_->removei2cAccess (index) ;
  }
  catch (FecExceptionHandler &e) { }
}

/**
 */
void FecDialogImpl::i2cWriteI2c() {

  bool init = false ;
  keyType index = 0 ;

  try {
    
    index = getI2CKey(true) ;

    if (index != 0) {

      uint address ;
      if (sscanf (i2cAddressAccess->text(), "%x", &address)) {

        index |= setAddressKey(address) ;
        uint value = 0 ;

        if (sscanf (i2cDataAccess->text(), "%x", &value)) {

          if (i2cNormalMode->isChecked()) {

            // Create an access
            fecAccess_->addi2cAccess (index,PHILIPS,MODE_SHARE) ;
            init = true ;

            // Check the offset
            uint offset ;
            if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) offset = 0 ;
            // Check the shift
            uint decal ;
            if (sscanf (i2cShiftAccess->text(), "%x", &decal)) decal = 0 ;
            
            if (offset == 0 && decal == 0)
              fecAccess_->write ( index, value ) ;
            else
              if (decal == 0)
                fecAccess_->writeOffset ( index, offset, value ) ;
              else
                fecAccess_->writeOffset ( index, offset, decal, value ) ;
          }
          else if (i2cExtendedMode->isChecked()) {

            // Create an access
            fecAccess_->addi2cAccess (index,FOREXTENDED,MODE_SHARE) ;
            init = true ;

            // Check the offset
            uint offset ;
            if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) {
                
              fecAccess_->write ( index, offset, value ) ;
            }
            else {
              
              ErrorMessage ("Offset for i2c extended write is not correct") ;
              // ERROR
              // cout << "Offset for i2c extended write is not correct" << endl ;
            }
          }
          else if (i2cRalMode->isChecked()) {

            // Create an access
            fecAccess_->addi2cAccess (index,APV25,MODE_SHARE) ;
            init = true ;

            // Check the offset
            uint offset ;
            if (sscanf (i2cOffsetAccess->text(), "%x", &offset)) {
              
              fecAccess_->write ( index, offset, value ) ;
            }
            else {
              
              ErrorMessage ("Offset for i2c RAL write is not correct") ;
              // ERROR
              // cout << "Offset for i2c RAL write is not correct" << endl ;
            }
          }
          else {
            
            ErrorMessage ("No mode are specified") ;
            // ERROR
            // cout << "No mode are specified" << endl ;
          }

        }
        else {
          
          ErrorMessage ("I2C data is not correct") ;
          // ERROR
          // cout << "I2C data is not correct" << endl ;
        } 
      }
      else {

        ErrorMessage ("I2C address is not correct") ;
        // ERROR
        // cout << "I2C address is not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write to an i2c device", e) ; // cerr << e.what() << endl ;
  }

  // Remove the access
  try {
    if (init) fecAccess_->removei2cAccess (index) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to remove an i2c access", e) ; // cerr << e.what() << endl ; 
  }
}

/**
 */
void FecDialogImpl::i2cReadModifyWrite() {

  bool init = false ;
  keyType index = 0 ;

  try {

    index = getI2CKey(true) ;

    if (index != 0) {

      uint address ;
      if (sscanf (i2cAddressAccess->text(), "%x", &address)) {

        index |= setAddressKey(address) ;

        cout << "Read modify write is Not implemented" << endl ;

        // ???????????????????????????????????????
        // Write the mask register
        uint mask ;
        if (sscanf (i2cLogMaskRegister->text(), "%x", &mask)) {
          
          // Write it
          // Not implemend

          // Create an access
          //fecAccess_->addi2cAccess (buildCompleteKey(slot,ringSlot,ccuAddress,channel,address),PHILIPS,MODE_SHARE) ;
          //index = buildCompleteKey(slot,ringSlot,ccuAddress,channel,address) ;
          
          uint value = 0 ;
          if (i2cNormalMode->isChecked()) {
            
          }
          else if (i2cExtendedMode->isChecked()) {

          }
          else if (i2cRalMode->isChecked() ) {
            
          }

          char msg[80] ;
          sprintf (msg, "0x%04X", value) ;
          i2cDataAccess->setText (msg) ;
        }
        else {

          ErrorMessage ("Mask register is not correct") ;
          // ERROR
          // cout << "Mask register is not correct" << endl ;
        }
      }
      else {
        
        ErrorMessage ("I2C address is not correct") ;
        // ERROR
        // cout << "I2C address is not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read and write (RMW) to an i2c device", e) ; // cerr << e.what() << endl ;
  }

  // Remove the access
  try {
    if (init) fecAccess_->removei2cAccess (index) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to remove an i2c access", e) ; // cerr << e.what() << endl ; 
  }
}

/**
 */
void FecDialogImpl::scanForI2CDevices() {

  try {
    uint slot, ringSlot; 
    if (i2cFecSlots->count() && sscanf (i2cFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      i2cListDeviceDetected->clear() ;

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;

      std::list<keyType> *deviceList = fecRingDevice->scanRingForI2CDevice ( ) ;

      if (deviceList != NULL) {

        for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {
      
          keyType index = *p ;
          
          char msg[100] ;
          decodeKey (msg, index) ;

          i2cListDeviceDetected->insertItem (msg) ;
        }
      }
      else {

        ErrorMessage ("No device found on the CCU and channels") ;
        // ERROR
        // cout << "No device found on the CCU and channels" << endl ;
      }

#ifdef COUCOU
      // Display the module and the i2c speed for each module
      displayTrackerModulesNSpeed (deviceList) ;
#endif

      delete deviceList ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Operation failed: detection of i2c device", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::i2cReadAll() {

  i2cReadCRA() ;
  i2cReadSRA() ;
  i2cReadSR() ;
}

/**
 */
void FecDialogImpl::i2cClearAll() {

  //i2cReadAll() ;
  i2cEnable->setChecked(false) ;
  i2cListDeviceDetected->clear() ;
  i2cCRA->setText ("0x0000") ;
  i2cSpeed100->setChecked(false) ;
  i2cSpeed200->setChecked(false) ;
  i2cSpeed400->setChecked(false) ;
  i2cSpeed1000->setChecked(false) ;
  i2cEnableBroadCast->setChecked(false) ;
  i2cForceAcknowledge->setChecked(false) ;
  i2cSRB->setText ("0x0000") ;
  i2cSRC->setText ("0x0000") ;
  i2cSRD->setText ("0x0000") ;
  i2cSRA->setText ("0x0000") ;
  i2cLastTrSuccessfull->setChecked(false) ;
  i2cLow->setChecked(false) ;
  i2cInvalidCommand->setChecked(false) ;
  i2cLastOperationNotAck->setChecked(false) ;
  i2cGlobalError->setChecked(false) ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   Memory channel methods                                            */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Try to retreive the memory key and check if the channel is enable or not
 * \param checkInit - check if the channel is initialised
 * \return index of the channel
 * \exception FecExceptionHandler
 */
keyType FecDialogImpl::getMemoryKey ( bool checkInit ) {

  keyType index = 0 ;

  uint slot, ringSlot; 
  if (memoryFecSlots->count() && sscanf (memoryFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
      
    uint ccuAddress ;
    if (memoryCcuAddresses->count() && sscanf (memoryCcuAddresses->currentText(), "%x", &ccuAddress)) {
      
      uint channel = MEMORYCHANNELNUMBER ;

      index = buildCompleteKey(slot,ringSlot,ccuAddress,channel,0) ;

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      if (fecRingDevice->isChannelEnabled(buildCompleteKey(slot,ringSlot,ccuAddress,channel,0)))
        memoryEnable->setChecked(true) ;
      else
        memoryEnable->setChecked(false) ;

      if (checkInit) {
        
        if (! memoryEnable->isChecked()) {
          
          index = 0 ;

          ErrorMessage ("The channel is not enable", "Please enable the channel before performed any operation") ;
          // ERROR
          //cout << "The channel is not enable, please enable the channel before performed any operation" << endl ;
        }
      }
    }
    else {
      
      ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
      // ERROR
      // cout << "Error the ccu address enter is not correct or empty" << endl ;
      // cout << "Please use scan CCUs before use read/write" << endl ;
    }
  }
  else {

    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

    // ERROR
    // cout << "Error the slot enter is not correct or empty" << endl ;
    // cout << "Please scan FECs before use read/write" << endl ;
  }

  return (index) ;
}

/**
 */
void FecDialogImpl::memoryIsChannelEnable() {

  try {

    getMemoryKey(false) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to check if the memory channel is enable", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryEnableChannel() {

  try {

    bool init = memoryEnable->isChecked() ;
    keyType index = getMemoryKey(false) ;

    if (index != 0) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( index ) ;
          
      fecRingDevice->setChannelEnable (index, init) ;
      memoryEnable->setChecked(init) ;
    }
  }
  catch (FecExceptionHandler &e) { 
    
    memoryEnable->setChecked(false) ;
    
    // ERROR 
    ErrorMessage ("Unable to enable the memory channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryWriteCRA() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint value ;
      if (sscanf (memoryCRA->text(), "%x", &value)) {

        fecAccess_->setMemoryChannelCRA (index, value) ;
      
        memoryReadCRA() ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the memory channel CRA", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryReadCRA() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {    

      uint CRA = fecAccess_->getMemoryChannelCRA (index) ;

      char msg[80] ;
      sprintf (msg, "0x%04X", CRA) ;
      memoryCRA->setText (msg) ;

      switch (CRA & 0x3) {
        
      case 0x0:
        memorySpeed1->setChecked(true) ;
        memorySpeed4->setChecked(false) ;
        memorySpeed10->setChecked(false) ;
        memorySpeed20->setChecked(false) ;
        break ;
      case 0x1:
        memorySpeed1->setChecked(false) ;
        memorySpeed4->setChecked(true) ;
        memorySpeed10->setChecked(false) ;
        memorySpeed20->setChecked(false) ;
        break ;
      case 0x2:
        memorySpeed1->setChecked(false) ;
        memorySpeed4->setChecked(false) ;
        memorySpeed10->setChecked(true) ;
        memorySpeed20->setChecked(false) ;
        break;
      case 0x3:
        memorySpeed1->setChecked(false) ;
        memorySpeed4->setChecked(false) ;
        memorySpeed10->setChecked(false) ;
        memorySpeed20->setChecked(true) ;
        break ;
      }
      
      if (CRA & 0x04) 
        memoryWin1Enable->setChecked(true) ;
      else 
        memoryWin1Enable->setChecked(false) ;
      
      if (CRA & 0x08) 
        memoryWin2Enable->setChecked(true) ;
      else 
        memoryWin2Enable->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the memory channel CRA", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryWriteCRABit() {

  uint CRA = 0 ;
  if (memorySpeed1->isChecked()) CRA = 0 ;
  else
    if (memorySpeed4->isChecked()) CRA = 1 ;
    else
      if (memorySpeed10->isChecked()) CRA = 2 ;
      else
        if (memorySpeed20->isChecked()) CRA = 3 ;
  

  if (memoryWin1Enable->isChecked()) CRA |= 0x04 ;
  if (memoryWin2Enable->isChecked()) CRA |= 0x08 ;

  char msg[80] ;
  sprintf (msg, "0x%04X", CRA) ;
  memoryCRA->setText (msg) ;

  memoryWriteCRA() ;
}

/**
 */
void FecDialogImpl::memorySetSpeed1 () {

  bool error = true ;
  uint speed = 0 ; // 1 MHz

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint CRA = fecAccess_->getMemoryChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;

      fecAccess_->setMemoryChannelCRA (index, CRA) ;
      
      memoryReadCRA() ;

      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the speed of the memory channel (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    memorySpeed1->setChecked(true) ;
  else
    memorySpeed1->setChecked(false) ;
  
  memorySpeed4->setChecked(false) ;
  memorySpeed10->setChecked(false) ;
  memorySpeed20->setChecked(false) ;
}


/**
 */
void FecDialogImpl::memorySetSpeed4 () {

  bool error = true ;
  uint speed = 1 ; // 4 MHz

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint CRA = fecAccess_->getMemoryChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;
      
      fecAccess_->setMemoryChannelCRA (index, CRA) ;
      
      memoryReadCRA() ;
      
      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the speed of the memory channel (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    memorySpeed4->setChecked(true) ;
  else
    memorySpeed4->setChecked(false) ;
  
  memorySpeed1->setChecked(false) ;
  memorySpeed10->setChecked(false) ;
  memorySpeed20->setChecked(false) ;
}


/**
 */
void FecDialogImpl::memorySetSpeed10 () {

  bool error = true ;
  uint speed = 2 ; // 10 MHz

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint CRA = fecAccess_->getMemoryChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;
      
      fecAccess_->setMemoryChannelCRA (index, CRA) ;

      memoryReadCRA() ;

      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the speed of the memory channel (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    memorySpeed10->setChecked(true) ;
  else
    memorySpeed10->setChecked(false) ;

  memorySpeed1->setChecked(false) ;  
  memorySpeed4->setChecked(false) ;
  memorySpeed20->setChecked(false) ;
}


/**
 */
void FecDialogImpl::memorySetSpeed20 () {

  bool error = true ;
  uint speed = 3 ; // 20 MHz

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint CRA = fecAccess_->getMemoryChannelCRA (index) ;
          
      CRA &= 0xFFFC ;
      CRA |= speed ;
      
      fecAccess_->setMemoryChannelCRA (index, CRA) ;
      
      memoryReadCRA() ;
      
      error = false ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to set the speed of the memory channel (CRA)", e) ; // cerr << e.what() << endl ;
  }

  if (error) 
    memorySpeed20->setChecked(true) ;
  else
    memorySpeed20->setChecked(false) ;

  memorySpeed1->setChecked(false) ;  
  memorySpeed4->setChecked(false) ;
  memorySpeed10->setChecked(false) ;
}

/**
 */
void FecDialogImpl::memoryResetChannel() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      fecAccess_->memoryChannelReset (index) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to reset the memory channel", e) ; // cerr << e.what() << endl ; 
  }
}

/**
 */
void FecDialogImpl::memoryReadWindowRegisters() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      char msg[80] ;
      uint value = 0 ;

      value = fecAccess_->getMemoryChannelWin1LReg (index) ;
      sprintf (msg, "0x%04X", value) ;
      memoryWin1L->setText (msg) ;

      value = fecAccess_->getMemoryChannelWin1HReg (index) ;
      sprintf (msg, "0x%04X", value) ;
      memoryWin1H->setText (msg) ;
      
      value = fecAccess_->getMemoryChannelWin2LReg (index) ;
      sprintf (msg, "0x%04X", value) ;
      memoryWin2L->setText (msg) ;

      value = fecAccess_->getMemoryChannelWin2HReg (index) ;
      sprintf (msg, "0x%04X", value) ;
      memoryWin2H->setText (msg) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to reset the memory channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryWriteWindowRegisters() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint value = 0 ;
      
      if (sscanf (memoryWin1L->text(), "%x", &value))
        fecAccess_->setMemoryChannelWin1LReg (index, value) ;
      
      if (sscanf (memoryWin1H->text(), "%x", &value))
        fecAccess_->setMemoryChannelWin1HReg (index, value) ;
      
      if (sscanf (memoryWin2L->text(), "%x", &value))
        fecAccess_->setMemoryChannelWin2LReg (index, value) ;
      
      if (sscanf (memoryWin2H->text(), "%x", &value))
        fecAccess_->setMemoryChannelWin2HReg (index, value) ;
      
      memoryReadWindowRegisters() ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the memory window registers", e) ; // cerr << e.what() << endl ;
  }
}


/**
 */
void FecDialogImpl::memoryReadSR() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint SR = fecAccess_->getMemoryChannelStatus (index) ;
          
      char msg[80] ;
      sprintf (msg, "0x%04X", SR) ;
      memorySR->setText (msg) ;
        
      if (SR & 0x20) 
        memoryInvalidCommand->setChecked(true) ;
      else 
        memoryInvalidCommand->setChecked(false) ;
        
      if (SR & 0x40) 
        memoryInvalidAddress->setChecked(true) ;
      else 
        memoryInvalidAddress->setChecked(false) ;
      
      if (SR & 0x80) 
        memoryGlobalError->setChecked(true) ;
      else 
        memoryGlobalError->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read a memory status register", e) ; // cerr << e.what() << endl ;
  }
}


/**
 */
void FecDialogImpl::memorySetMaskAnd() {

  //memoryAndOperation->setChecked(true) ;
  memoryOrOperation->setChecked(false) ;
  memoryXorOperation->setChecked(false) ;
}

/**
 */
void FecDialogImpl::memorySetMaskOr() {

  memoryAndOperation->setChecked(false) ;
  //memoryOrOperation->setChecked(true) ;
  memoryXorOperation->setChecked(false) ;
}

/**
 */
void FecDialogImpl::memorySetMaskXor() {

  memoryAndOperation->setChecked(false) ;
  memoryOrOperation->setChecked(false) ;
  //memoryXorOperation->setChecked(true) ;
}

/**
 */
void FecDialogImpl::memoryReadModifyWrite () {

  bool added = false ;
  keyType index = 0 ;

  try {

    index = getMemoryKey(true) ;
    if (index != 0) {

      // Add the access
      fecAccess_->addMemoryAccess (index, MODE_SHARE) ;
      added = true ;

      uint mask = 0, AH = 0, AL = 0 ;
      if (sscanf (memoryMask->text(), "%x", &mask)) {

        // Write the mask
        // fecAccess_->setMemoryChannelMaskReg ( index, mask ) ; => done in method write

        if ( (sscanf (memoryAL->text(), "%x", &AL)) && (sscanf (memoryAH->text(), "%x", &AH)) ) {
          
          logicalOperationType op = CMD_OR ;
          if ((memoryOrOperation->isChecked()) || 
              (memoryAndOperation->isChecked()) || 
              (memoryXorOperation->isChecked())) {
            
            if (memoryOrOperation->isChecked())  op =  CMD_OR  ;
            if (memoryAndOperation->isChecked()) op =  CMD_AND ;
            if (memoryXorOperation->isChecked()) op =  CMD_XOR ;
            
            // Apply read modify write
            fecAccess_->write (index, AL, AH, op, mask) ;

            // Remove the access
            if (added) {
              try {
                fecAccess_->removeMemoryAccess(index) ;
                added = false ;
              }
              catch (FecExceptionHandler &e) { }
            }

            // Read back value
            memoryReadSB() ;
          }
          else {

            ErrorMessage ("Select a logical operation to be performed") ;
            // ERROR
            // cout << "Select a logical operation to be performed" << endl ;
          }
        }
        else {

          ErrorMessage ("Value for AL/AH are not correct") ;
          // ERROR
          // cout << "Value for AL/AH are not correct" << endl ;
        }
      }
      else {
        
        ErrorMessage ("Value for the mask is not correct") ;
        // ERROR
        // cout << "Value for the mask is not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to perform a read and write operation (RMW) on the memory channel", e) ; // cerr << e.what() << endl ;
  }

  if (added) {
    try {
      fecAccess_->removeMemoryAccess(index) ;
    }
    catch (FecExceptionHandler &e) { }
  }
}

/**
 */
void FecDialogImpl::memoryReadMaskReg() {

  try {

    keyType index = getMemoryKey(true) ;
    if (index != 0) {

      uint mask = fecAccess_->getMemoryChannelMaskReg(index) ;

      char msg[80] ;
      sprintf (msg, "0x%02X", mask) ;
      memoryMask->setText (msg) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the memory mask register", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::memoryReadSB() {

  bool added = false ;
  keyType index = 0 ;

  try {

    index = getMemoryKey(true) ;
    if (index != 0) {

      // Add the access
      fecAccess_->addMemoryAccess (index, MODE_SHARE) ;
      added = true ;

      uint AH = 0, AL = 0 ;
      if ( (sscanf (memoryAL->text(), "%x", &AL)) && (sscanf (memoryAH->text(), "%x", &AH)) ) {
        
        // Apply read
        uint value = fecAccess_->read (index, AL, AH) ;

        char msg[80] ;
        sprintf (msg, "0x%02X", value) ;
        memoryData->setText (msg) ;
      }
      else {
        
        ErrorMessage ("Values for AH/AL are not correct") ;
        // ERROR
        // cout << "Values for AH/AL are not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read a byte from the memory", e) ; // cerr << e.what() << endl ;
  }

  if (added) {
    try {

      fecAccess_->removeMemoryAccess(index) ;
    }
    catch (FecExceptionHandler &e) { }
  }
}


/**
 */
void FecDialogImpl::memoryWriteSB() {

  bool added = false ;
  keyType index = 0 ;

  try {

    index = getMemoryKey(true) ;
    if (index != 0) {

      // Add the access
      fecAccess_->addMemoryAccess (index, MODE_SHARE) ;
      added = true ;

      uint AH = 0, AL = 0 ;
      if ( (sscanf (memoryAL->text(), "%x", &AL)) && (sscanf (memoryAH->text(), "%x", &AH)) ) {

        uint value = 0 ;
        if (sscanf (memoryData->text(), "%x", &value)) {
          
          // Apply write
          fecAccess_->write (index, AL, AH, value) ;
        }
        else {
          
          ErrorMessage ("Data is not correct") ;
          // ERROR
          // cout << "Data is not correct" << endl ;
        }
      }
      else {
        
        ErrorMessage ("Values for AH/AL are not correct") ;
        // ERROR
        // cout << "Values for AH/AL are not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write a byte to the memory", e) ; // cerr << e.what() << endl ;
  }

  if (added) {
    try {

      fecAccess_->removeMemoryAccess(index) ;
    }
    catch (FecExceptionHandler &e) { }
  }
}

/**
 */
void FecDialogImpl::memoryReadMB() {

  bool added = false ;
  keyType index = 0 ;
  tscType8 *values = NULL ;

  try {

    memoryDisplayList->clear(); 

    index = getMemoryKey(true) ;
    if (index != 0) {

      // Add the access
      fecAccess_->addMemoryAccess (index, MODE_SHARE) ;
      added = true ;

      uint AH = 0, AL = 0 ;
      if ( (sscanf (memoryAL->text(), "%x", &AL)) && (sscanf (memoryAH->text(), "%x", &AH)) ) {
        
        uint sizeToRead = 0 ;
        if ( (sscanf (memorySize->text(), "%d", &sizeToRead)) && (sizeToRead > 0) && (sizeToRead < 65536) ) {

          values = new tscType8[sizeToRead] ;

          // Apply read
          struct timeval time1, time2;
          struct timezone zone1, zone2;
          gettimeofday(&time1, &zone1);

          fecAccess_->read (index, AL, AH, sizeToRead, values) ;

          gettimeofday(&time2, &zone2);
          double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
          double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
          double timesec = timesec2 - timesec1 ;
          double perf = (double) sizeToRead ;
          perf /= (1024*1024*timesec) ;

          char msg[80] ;
          sprintf (msg, "%f s", timesec) ;
          memoryTime->setText (msg) ;
          sprintf (msg, "%f Mb/s", perf) ;
          memoryBandwith->setText (msg) ;

          if (memoryDisplayOnList->isChecked()) {
            for (uint i = 0 ; i < sizeToRead ; i ++) {

              sprintf (msg, "%d - 0x%02X", i, values[i]) ;
              memoryDisplayList->insertItem (msg) ;
            }
          }
        }
        else {

          ErrorMessage ("The size enter is not correct, must be 0 < x < 65536") ;
          // ERROR
          // cout << "The size enter is not correct, must be 0 < x < 65536" << endl ;
        }
      }
      else {

        ErrorMessage ("Value for AL/AH are not correct") ;
        // ERROR
        // cout << "Value for AH/AL are not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read several bytes from the memory (multiple bytes mode)", e) ; // cerr << e.what() << endl ;
  }

  if (added) {
    try {

      fecAccess_->removeMemoryAccess(index) ;
    }
    catch (FecExceptionHandler &e) { }
  }

  if (values != NULL) delete values ;
}


/**
 */
void FecDialogImpl::memoryWriteMB() {

  bool added = false ;
  keyType index = 0 ;
  tscType8 *values = NULL ;

  try {

    index = getMemoryKey(true) ;
    if (index != 0) {

      // Add the access
      fecAccess_->addMemoryAccess (index, MODE_SHARE) ;
      added = true ;

      uint AH = 0, AL = 0 ;
      if ( (sscanf (memoryAL->text(), "%x", &AL)) && (sscanf (memoryAH->text(), "%x", &AH)) ) {

        uint sizeToRead = 0 ;
        if ((sscanf (memorySize->text(), "%d", &sizeToRead)) && (sizeToRead > 0) && (sizeToRead < 65536)) {

          values = new tscType8[sizeToRead] ;
          bool error = false ;
          if (memoryFillSameData->isChecked()) {

            uint value = 0 ;
            if (sscanf (memoryData->text(), "%x", &value)) 
              for (uint i = 0 ; i < sizeToRead ; i ++) values[i] = value % 256 ;
            else {

              error = true ;

              ErrorMessage ("Data is not correct") ;
              // ERROR
              // cout << "Data is not correct" << endl ;
            }
          }
          else
            for (uint i = 0 ; i < sizeToRead ; i ++) values[i] = (i+1) % 256 ;

          if (! error) {

            // Apply read
            struct timeval time1, time2;
            struct timezone zone1, zone2;
            gettimeofday(&time1, &zone1);

            fecAccess_->write (index, AL, AH, values, sizeToRead) ;
            
            gettimeofday(&time2, &zone2);
            double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
            double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
            double timesec = timesec2 - timesec1 ;
            double perf = (double) sizeToRead ;
            perf /= (1024*1024*timesec) ;

            char msg[80] ;
            sprintf (msg, "%f s", timesec) ;
            memoryTime->setText (msg) ;
            sprintf (msg, "%f Mb/s", perf) ;
            memoryBandwith->setText (msg) ;
          }
        }
        else {
          
          ErrorMessage ("The size enter is not correct, must be 0 < x < 65536") ;
          // ERROR
          // cout << "The size enter is not correct, must be 0 < x < 65536" << endl ;
        }
      }
      else {
        
        ErrorMessage ("Values for AH/AL are not correct") ;
        // ERROR
        // cout << "Values for AH/AL are not correct" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write several bytes to the memory (multiple bytes mode)", e) ; // cerr << e.what() << endl; 
  }

  if (added) {
    try {

      fecAccess_->removeMemoryAccess(index) ;
    }
    catch (FecExceptionHandler &e) { }
  }

  if (values != NULL) delete values ;
}

/**
 */
void FecDialogImpl::memoryWriteCompareMB() {

  memoryDisplayOnList->setChecked(true) ;

  memoryWriteMB() ;
  memoryReadMB() ;

  uint sizeToRead = 0 ;
  if ((sscanf (memorySize->text(), "%d", &sizeToRead)) && (sizeToRead > 0) && (sizeToRead < 65536)) {

    bool error = false ;
    uint value = 0 ;
    if (memoryFillSameData->isChecked())
      if (sscanf (memoryData->text(), "%x", &value)) value = value % 256 ;
      else error = true ;

    if (!error) {

      if (sizeToRead != memoryDisplayList->count()) {

        ErrorMessage ("Problem on size, so the values compared are not the same => continue ...") ;
        // ERROR
        // cout << "Problem on size, so the values compared are not the same => continue ..." << endl ;
      }

      error = false ;
      for (uint i = 0 ; i < sizeToRead && i < memoryDisplayList->count() && !error ; i ++) {
        
        uint valueR = 0, valueC = 0, indexT ;
        if (sscanf (memoryDisplayList->text(i), "%d - %x", &indexT, &valueR)) {
          
          if (memoryFillSameData->isChecked()) valueC = value ;
          else valueC = (i+1) % 256 ;
          
          if (valueR != valueC) {
            
            char msg[80] ;
            sprintf (msg, "Incoherent value found at index %d (%d is different with %d)", i, valueR, valueC) ;
            ErrorMessage (msg) ;
            // ERROR
            // cout << "Incoherent value found at index " << i << " (" << valueR << " is different with " << valueC << ")" << endl ;
            memoryDisplayList->setCurrentItem(i) ;
            error = true ;
          }
        }
        else {
          
          char msg[80] ;
          sprintf (msg, "Incoherent value found in the list: %s", memoryDisplayList->text(i).latin1()) ;
          ErrorMessage (msg) ;
          // ERROR
          // cout << "Incoherent value found in the list: " << memoryDisplayList->text(i) << endl ;
        }
      }
        
      if (! error) {
        
        ErrorMessage ("No difference found") ;
        // ERROR / INFORMATION
        // cout << "No differences found" << endl ;
      }
    }
  }
}

/**
 */
void FecDialogImpl::memoryReadAll() {

  memoryReadCRA() ;
  memoryReadWindowRegisters() ;
  memoryReadSR() ;
}

/**
 */
void FecDialogImpl::memoryClearAll() {

  //memoryReadAll() ;
  memoryEnable->setChecked(false) ;
  memoryCRA->setText ("0x0000") ;
  memorySpeed1->setChecked(false) ;
  memorySpeed4->setChecked(false) ;
  memorySpeed10->setChecked(false) ;
  memorySpeed20->setChecked(false) ;
  memoryWin1Enable->setChecked(false) ;
  memoryWin1Enable->setChecked(false) ;
  memorySR->setText ("0x0000") ;
  memoryInvalidCommand->setChecked(false) ;
  memoryInvalidAddress->setChecked(false) ;
  memoryGlobalError->setChecked(false) ;
  memoryWin1L->setText ("0x0000") ;
  memoryWin1H->setText ("0x0000") ;
  memoryWin2L->setText ("0x0000") ;
  memoryWin1H->setText ("0x0000") ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   PIA channel methods                                               */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Try to retreive the PIA key and check if the channel is enable or not
 * \param checkInit - check if the channel is initialised
 * \return index of the channel
 * \exception FecExceptionHandler
 */
keyType FecDialogImpl::getPiaKey ( bool checkInit ) {

  keyType index = 0 ;

  uint slot, ringSlot; 
  if (piaFecSlots->count() && sscanf (piaFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
    
    uint ccuAddress ;
    if (piaCcuAddresses->count() && sscanf (piaCcuAddresses->currentText(), "%x", &ccuAddress)) {
      
      uint channel ;
      if (piaChannels->count() && sscanf (piaChannels->currentText(), "%d", &channel)) {
        
        index = buildCompleteKey(slot,ringSlot,ccuAddress,channel,0) ;
        FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
        if (fecRingDevice->isChannelEnabled(index)) piaEnable->setChecked(true) ;
        else piaEnable->setChecked(false) ;

        if (checkInit) {
          if (! piaEnable->isChecked()) {
            index = 0 ;

            ErrorMessage ("The channel is not enable", "Please enable the channel before performed any operation") ;
            // ERROR
            // cout << "The channel is not enable, please enable the channel before performed any operation" << endl ;
          }
        }
      }
      else {
          
        ErrorMessage ("Error the channel number enter is not correct or empty") ;
        // ERROR
        // cout << "Error the channel number enter is not correct or empty" << endl ;
      }
    }
    else {
      
      ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
      // ERROR
      // cout << "Error the ccu address enter is not correct or empty" << endl ;
      // cout << "Please use scan CCUs before use read/write" << endl ;
    }
  }
  else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

    // ERROR
    // cout << "Error the slot enter is not correct or empty" << endl ;
    // cout << "Please scan FECs before use read/write" << endl ;
  }

  return (index) ;
}

/**
 */
void FecDialogImpl::piaIsChannelEnable () {

  try {

    getPiaKey(false) ;
  }
  catch (FecExceptionHandler &e) { 
    
    // ERROR 
    ErrorMessage ("Unable to determine if a PIA channel is enable", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaEnableChannel() {

  try {
    
    bool init = piaEnable->isChecked() ;
    keyType index = getPiaKey(false) ;

    if (index != 0) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( index ) ;
      
      fecRingDevice->setChannelEnable (index, init) ;
      piaEnable->setChecked(init) ;
    }
    else
      piaEnable->setChecked(false) ;
  }
  catch (FecExceptionHandler &e) { 
    
    piaEnable->setChecked(false) ;
    
    // ERROR 
    ErrorMessage ("Unable to enable a PIA channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaResetChannel() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 
      fecAccess_->piaChannelReset (index) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to reset a PIA channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaWriteGCR() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 

      uint value ;
      if (sscanf (piaGCR->text(), "%x", &value)) {
          
        fecAccess_->setPiaChannelGCR (index, value) ;
      }
      else {
        
        ErrorMessage ("Wrong value in the general control register text field") ;
        // ERROR
        // cout << "Wrong value in the general control register text field" << endl ;
      }
        
      piaReadGCR() ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the PIA GCR", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaWriteGCRBit(){ 

  uint GCR = 0 ;

  if (piaWidth1000->isChecked()) GCR |= 0x0 ;
  else if (piaWidth500->isChecked()) GCR |= 0x1 ;
  else if (piaWidth200->isChecked()) GCR |= 0x2 ; 
  else if (piaWidth100->isChecked()) GCR |= 0x3 ;

  if (piaEnp->isChecked()) GCR |= 0x04 ;
  if (piaStrb->isChecked()) GCR |= 0x08 ;
  if (piaStroutp->isChecked()) GCR |= 0x10 ;
  if (piaEnintA->isChecked()) GCR |= 0x20 ;
  if (piaStrinp->isChecked()) GCR |= 0x40 ;

  char msg[80] ;
  sprintf (msg, "0x%04X", GCR) ;
  piaGCR->setText (msg) ;

  piaWriteGCR() ;
}

/**
 */
void FecDialogImpl::piaReadGCR() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) {
      
      uint GCR = fecAccess_->getPiaChannelGCR (index) ;

      char msg[80] ;
      sprintf (msg, "0x%04X", GCR) ;
      piaGCR->setText (msg) ;

      switch (GCR & 0x3) {
        
      case 0x0:
        piaWidth1000->setChecked(true) ;
        piaWidth500->setChecked(false) ;
        piaWidth200->setChecked(false) ;
        piaWidth100->setChecked(false) ;
        break ;
      case 0x1:
        piaWidth1000->setChecked(false) ;
        piaWidth500->setChecked(true) ;
        piaWidth200->setChecked(false) ;
        piaWidth100->setChecked(false) ;
        break ;
      case 0x2:
        piaWidth1000->setChecked(false) ;
        piaWidth500->setChecked(false) ;
        piaWidth200->setChecked(true) ;
        piaWidth100->setChecked(false) ;
        break;
      case 0x3:
        piaWidth1000->setChecked(false) ;
        piaWidth500->setChecked(false) ;
        piaWidth200->setChecked(false) ;
        piaWidth100->setChecked(true) ;
        break ;
      }

      if (GCR & 0x04) 
        piaEnp->setChecked(true) ;
      else 
        piaEnp->setChecked(false) ;

      if (GCR & 0x08) 
        piaStrb->setChecked(true) ;
      else 
        piaStrb->setChecked(false) ;
      
      if (GCR & 0x10) 
        piaStroutp->setChecked(true) ;
      else 
        piaStroutp->setChecked(false) ;

      if (GCR & 0x20)
        piaEnintA->setChecked(true) ;
      else
        piaEnintA->setChecked(false) ;

      if (GCR & 0x40)
        piaStrinp->setChecked(true) ;
      else
        piaStrinp->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the PIA GCR", e) ; // cerr << e.what() << endl ;
  }  
}

/**
 */
void FecDialogImpl::piaReadSR() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 

      uint SR = fecAccess_->getPiaChannelStatus (index) ;

      char msg[80] ;
      sprintf (msg, "0x%04X", SR) ;
      piaSR->setText (msg) ;

      if (SR & 0x1) 
        piaInt->setChecked(true) ;
      else 
        piaInt->setChecked(false) ;
      
      if (SR & 0x20) 
        piaInvcom->setChecked(true) ;
      else 
        piaInvcom->setChecked(false) ;

      if (SR & 0x80) 
        piaGlobalError->setChecked(true) ;
      else 
        piaGlobalError->setChecked(false) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the PIA status register", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaWriteDDRP() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 

      uint value ;
      if (sscanf (piaDDRP->text(), "%x", &value)) {

        fecAccess_->setPiaChannelDDR(index, value) ;
      }
      else {

        ErrorMessage ("Wrong value in the general control register text field") ;
        // ERROR
        // cout << "Wrong value in the general control register text field" << endl ;
      }

      piaReadDDRP() ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write the PIA DDR", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaReadDDRP() { 

  try {

    keyType index = getPiaKey(true) ;
    
    if (index != 0) { 

      uint value = fecAccess_->getPiaChannelDDR(index) ;
      char msg[80] ;
      sprintf (msg, "0x%02X", value) ;
      piaDDRP->setText(msg) ;

      if (value & 0x01) {
        piaDDRBit0O->setChecked(true) ;
        piaDDRBit0I->setChecked(false) ;
      }
      else {
        piaDDRBit0O->setChecked(false) ;
        piaDDRBit0I->setChecked(true) ;
      }

      if (value & 0x02) {
        piaDDRBit1O->setChecked(true) ;
        piaDDRBit1I->setChecked(false) ;
      }
      else {
        piaDDRBit1O->setChecked(false) ;
        piaDDRBit1I->setChecked(true) ;
      }

      if (value & 0x04) {
        piaDDRBit2O->setChecked(true) ;
        piaDDRBit2I->setChecked(false) ;
      }
      else {
        piaDDRBit2O->setChecked(false) ;
        piaDDRBit2I->setChecked(true) ;
      }

      if (value & 0x08) {
        piaDDRBit3O->setChecked(true) ;
        piaDDRBit3I->setChecked(false) ;
      }
      else {
        piaDDRBit3O->setChecked(false) ;
        piaDDRBit3I->setChecked(true) ;
      }

      if (value & 0x10) {
        piaDDRBit4O->setChecked(true) ;
        piaDDRBit4I->setChecked(false) ;
      }
      else {
        piaDDRBit4O->setChecked(false) ;
        piaDDRBit4I->setChecked(true) ;
      }

      if (value & 0x20) {
        piaDDRBit5O->setChecked(true) ;
        piaDDRBit5I->setChecked(false) ;
      }
      else {
        piaDDRBit5O->setChecked(false) ;
        piaDDRBit5I->setChecked(true) ;
      }

      if (value & 0x40) {
        piaDDRBit6O->setChecked(true) ;
        piaDDRBit6I->setChecked(false) ;
      }
      else {
        piaDDRBit6O->setChecked(false) ;
        piaDDRBit6I->setChecked(true) ;
      }
        
      if (value & 0x80) {
        piaDDRBit7O->setChecked(true) ;
        piaDDRBit7I->setChecked(false) ;
      }
      else {
        piaDDRBit7O->setChecked(false) ;
        piaDDRBit7I->setChecked(true) ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read the PIA DDR", e) ; // cerr << e.what() << endl  ;
  }
}

/**
 */
void FecDialogImpl::piaWriteDDRPBit() { 

  uint value = 0 ;

  if (piaDDRBit0O->isChecked()) value |= 0x1 ;
  if (piaDDRBit1O->isChecked()) value |= 0x2 ;
  if (piaDDRBit2O->isChecked()) value |= 0x4 ;
  if (piaDDRBit3O->isChecked()) value |= 0x8 ;
  if (piaDDRBit4O->isChecked()) value |= 0x10 ;
  if (piaDDRBit5O->isChecked()) value |= 0x20 ;
  if (piaDDRBit6O->isChecked()) value |= 0x40 ;
  if (piaDDRBit7O->isChecked()) value |= 0x80 ;

  char msg[80] ;
  sprintf (msg, "0x%02X", value) ;
  piaDDRP->setText(msg) ;

  piaWriteDDRP() ;
}

/**
 */
void FecDialogImpl::piaDDRSwitchOutputInput() {

  piaDDRBit0I->setChecked(true) ;
  piaDDRBit1I->setChecked(true) ;
  piaDDRBit2I->setChecked(true) ;
  piaDDRBit3I->setChecked(true) ;
  piaDDRBit4I->setChecked(true) ;
  piaDDRBit4I->setChecked(true) ;
  piaDDRBit5I->setChecked(true) ;
  piaDDRBit6I->setChecked(true) ;
  piaDDRBit7I->setChecked(true) ;

  piaWriteDDRPBit() ;
}

/**
 */
void FecDialogImpl::piaDataRead(){ 

  bool init = false ;
  keyType index = 0 ;

  try {
    
    index = getPiaKey(true) ;
    
    if (index != 0) {

      // Create an access
      fecAccess_->addPiaAccess (index,MODE_SHARE) ;
      init = true ;

      // Write it
      uint value = fecAccess_->getPiaChannelDataReg(index) ;

      char msg[80] ;
      sprintf (msg, "0x%02X", value) ;
      piaDataRegister->setText(msg) ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to read a data from a PIA channel", e) ; // cerr << e.what() << endl ;
  }

  // Remove the access
  try {
    if (init) fecAccess_->removei2cAccess (index) ;
  }
  catch (FecExceptionHandler &e) { 
    
    // ERROR 
    ErrorMessage ("Unable to read a data from the PIA channel", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::piaDataWrite(){ 

  bool init = false ;
  keyType index = 0 ;

  try {
    
    index = getPiaKey(true) ;
    
    if (index != 0) {

      uint value ;
      if (sscanf (piaDataRegister->text(), "%x", &value)) {

        if (piaEnableOperationBit->isChecked()) {

          uint duration = 0, sleepT = 0 ;
          if ( sscanf (piaDuration->text(), "%x", &duration) && 
               sscanf (piaSleepTime->text(), "%x", &sleepT) ) {

            PiaResetAccess piaResetAccess (fecAccess_, index, 0xFF) ;
            piaResetDescription piaD ( index, duration, sleepT, value ) ;
            piaResetAccess.setValues (piaD) ;
          }
          else {

            ErrorMessage ("PIA duration and/or sleep time are not correct", "Please put right values") ;
            // ERROR
            // cout << "PIA duration and/or sleep time are not correct" << endl ;
            // cout << "Please put right values" << endl ;
          }
        }
        else {
              
          // Create an access
          fecAccess_->addPiaAccess (index,MODE_SHARE) ;
          init = true ;
          
          // Write it
          fecAccess_->setPiaChannelDataReg(index, value) ;

          //if (init) {
          //  fecAccess_->removei2cAccess (index) ;
          //  init = false ;
          //}
          //piaDataRead() ;
        }
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Unable to write a data into the PIA channel", e) ; // cerr << e.what() << endl ;
  }

  // Remove the access
  try {
    if (init) fecAccess_->removei2cAccess (index) ;
  }
  catch (FecExceptionHandler &e) { 
    
    // ERROR 
    ErrorMessage ("Unable to remove an i2c access", e) ; // cerr << e.what() << endl ; 
  }
}

/**
 */
void FecDialogImpl::piaReadAll() {

  piaReadGCR() ;
  piaReadSR() ;
  piaReadDDRP() ;
  piaDataRead() ;
}

/**
 */
void FecDialogImpl::piaClearAll() { 

  //memoryReadAll() ;
  piaEnable->setChecked(false) ;
  piaGCR->setText ("0x0000") ;
  piaWidth1000->setChecked(false) ;
  piaWidth500->setChecked(false) ;
  piaWidth200->setChecked(false) ;
  piaWidth100->setChecked(false) ;
  piaEnp->setChecked(false) ;
  piaStrb->setChecked(false) ;
  piaStroutp->setChecked(false) ;
  piaEnintA->setChecked(false) ;
  piaStrinp->setChecked(false) ;

  memorySR->setText ("0x0000") ;
  piaGlobalError->setChecked(false) ;
  piaInt->setChecked(false) ;
  piaInvcom->setChecked(false) ;

  piaDDRP->setText ("0x00") ;
  piaDataRegister->setText ("0x00") ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   Tracker methods                                                   */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/**
 * \warning do not forget to delete the pointer returned
 */
apvDescription *FecDialogImpl::buildApvDescription () {

  apvDescription *device = NULL ;

  uint apvMode, latency, muxGain, ipre, ipcasc, ipsf,
   isha, issf, ipsp, imuxin, ical, ispare, vfp, vfs, 
    vpsp, cdrv, csel, apvError ;

  if ( sscanf (apvModeT->text(), "%x", &apvMode) && 
       sscanf (apvLatencyT->text(), "%x", &latency) &&
       sscanf (apvMuxGainT->text(), "%x", &muxGain) &&
       sscanf (apvIpreT->text(), "%x", &ipre) &&
       sscanf (apvIpcascT->text(), "%x", &ipcasc) &&
       sscanf (apvIpsfT->text(), "%x", &ipsf) &&
       sscanf (apvIshaT->text(), "%x", &isha) &&
       sscanf (apvIssfT->text(), "%x", &issf) &&
       sscanf (apvIpspT->text(), "%x", &ipsp) &&
       sscanf (apvImuxinT->text(), "%x", &imuxin) &&
       sscanf (apvIcalT->text(), "%x", &ical) &&
       sscanf (apvIspareT->text(), "%x", &ispare) &&
       sscanf (apvVfpT->text(), "%x", &vfp) &&
       sscanf (apvVfsT->text(), "%x", &vfs) &&
       sscanf (apvVpspT->text(), "%x", &vpsp) &&
       sscanf (apvCdrvT->text(), "%x", &cdrv) &&
       sscanf (apvCselT->text(), "%x", &csel) &&
       sscanf (apvErrorT->text(), "%x", &apvError) ) {

    device = new apvDescription ( (tscType8)apvMode, (tscType8)latency, 
				  (tscType8)muxGain, (tscType8)ipre, 
				  (tscType8)ipcasc, (tscType8)ipsf,
				  (tscType8)isha, (tscType8)issf, 
				  (tscType8)ipsp, (tscType8)imuxin, 
				  (tscType8)ical, (tscType8)ispare, 
				  (tscType8)vfp, (tscType8)vfs, 
				  (tscType8)vpsp, (tscType8)cdrv, 
				  (tscType8)csel, (tscType8)apvError) ;
  }
  else {
    
    ErrorMessage ("One or several APV parameters are not correct") ;
    // ERROR
    // cout << "One or several APV parameters are not correct" << endl ;
  }

  return device ;
}

/**
 * \warning do not forget to delete the pointer returned
 */
muxDescription *FecDialogImpl::buildMuxDescription () {

  muxDescription *device = NULL ;

  uint resistor ;

  if ( sscanf (apvMuxResistorT->text(), "%x", &resistor) )
    device = new muxDescription ( resistor ) ;
  else {
    
    ErrorMessage ("MUX parameter is not correct") ;
    // ERROR
    // cout << "MUX parameter is not correct" << endl ;
  }

  return device ;
}

/**
 * \warning do not forget to delete the pointer returned
 */
pllDescription *FecDialogImpl::buildPllDescription () {

  pllDescription *device = NULL ;

  uint delayFine = 0, delayCoarse = 0 ;

  if ( sscanf (pllDelayFineT->text(), "%x", &delayFine) &&
       sscanf (pllDelayCoarseT->text(), "%x", &delayCoarse) )
    device = new pllDescription ( delayFine, delayCoarse ) ;
  else {
    
    ErrorMessage ("One or both PLL parameters are not correct") ;
    // ERROR
    // cout << "One or both PLL parameters are not correct" << endl ;
  }

  return device ;
}

/**
 * \warning do not forget to delete the pointer returned
 */
laserdriverDescription *FecDialogImpl::buildLaserdriverDescription () {

  laserdriverDescription *device = NULL ;

  unsigned char gainC[3] = {0}, biasC[3] = {0} ;
  uint gain[3] = {0}, bias[3] = {0} ;

  if ( sscanf (laserdriverGain1T->text(), "%x", &gain[0]) &&
       sscanf (laserdriverGain2T->text(), "%x", &gain[1]) &&
       sscanf (laserdriverGain3T->text(), "%x", &gain[2]) &&
       sscanf (laserdriverBias1T->text(), "%x", &bias[0]) &&
       sscanf (laserdriverBias2T->text(), "%x", &bias[1]) &&
       sscanf (laserdriverBias3T->text(), "%x", &bias[2]) ) {

    for (int i = 0 ; i < 3 ; i ++) {

      gainC[i] = (unsigned char)gain[i] ;
      biasC[i] = (unsigned char)bias[i] ;
    }

    device = new laserdriverDescription ( gainC, biasC ) ;
  }
  else {
    
    ErrorMessage ("One or several laserdriver parameters are not correct") ;
    // ERROR
    // cout << "One or several laserdriver parameters are not correct" << endl ;
  }

  return device ;
}

/**
 */
void FecDialogImpl::scanForTrackerDevices() {

  trackerListDeviceDetected->clear() ;
  trackerListHybridDetected->clear() ;
  trackerAllDevices->setChecked(false);

  try {

    if (fecSlots->count()) {

      // For each FEC
      for (int i = 0 ; i < fecSlots->count() ; i ++) {

        uint slot, ringSlot ;
        if (sscanf (fecSlots->text(i), "%d,%d", &slot, &ringSlot)) {

          FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;

          // Build a table with all values
          int sizeValues = 11 ;
	  keyType
            deviceValues[11][2] = { 
              {DCUADDRESS, NORMALMODE}, // DCU
              {0x20, RALMODE   }, // APV
              {0x21, RALMODE   }, // APV
              {0x22, RALMODE   }, // APV
              {0x23, RALMODE   }, // APV
              {0x24, RALMODE   }, // APV
              {0x25, RALMODE   }, // APV
              {0x43, RALMODE   }, // MUX
              {0x44, NORMALMODE}, // PLL
              {0x60, NORMALMODE}, // Laserdriver AOH
              {0x70, NORMALMODE}  // Laserdriver DOH
            } ;

          std::list<keyType> *deviceList = fecRingDevice->scanRingForI2CDevice ( (keyType *)deviceValues, sizeValues ) ;
	  Sgi::hash_map<keyType, bool *> trackerModule ;

          if (deviceList != NULL) {
	    
	    bool *module ;

            for (std::list<keyType>::iterator p=deviceList->begin();p!=deviceList->end();p++) {

              keyType index = *p ;

	      // Count the device on a module
	      keyType indexChannel = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),getChannelKey(index),0) ;
	      if (trackerModule.find(indexChannel) == trackerModule.end()) {
		trackerModule[indexChannel] = new bool[12] ;
		module = trackerModule[indexChannel] ;
		for (int i = 0 ; i < 12 ; i ++) module[i] = false ;
	      }
	      else module = trackerModule[indexChannel] ;
          
              char msg[100], msg1[200] ;
              decodeKey (msg, index) ;

	      switch (getAddressKey(index)) {
	      case 0x78:
		sprintf (msg1, "0x%X - DCU/CCU 0x%02X/0x%02X - %s", 
			 index, getCcuKey(index), getChannelKey(index), msg) ;
		module[11] = true ;
		break ;
	      case DCUADDRESS:
		sprintf (msg1, "0x%X - DCU 0x%02X/0x%02X - %s", 
			 index, getCcuKey(index), getChannelKey(index), msg) ;
		module[10] = true ;
		break ;
	      case 0x20:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[0] = true ;
		break ;
	      case 0x21:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[1] = true ;
		break ;
	      case 0x22:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[2] = true; 
		break ;
	      case 0x23:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[3] = true ;
		break ;
	      case 0x24:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[4] = true; 
		break ;
	      case 0x25:
		sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[5] = true ;
		break ;
	      case 0x43:
                sprintf (msg1, "0x%X - MUX 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[6] = true ;
		break ;
	      case 0x44:
                sprintf (msg1, "0x%X - PLL 0x%02X/0x%02X - %s", 
                         index, getCcuKey(index), getChannelKey(index), msg) ;
		module[7] = true ;
		break ;
	      case 0x60:
		sprintf (msg1, "0x%X - AOH 0x%02X/0x%02X - %s", 
			 index, getCcuKey(index), getChannelKey(index), msg);
		module[8] = true ;
		break ;
	      case 0x70:
		sprintf (msg1, "0x%X - DOH 0x%02X/0x%02X - %s", 
			 index, getCcuKey(index), getChannelKey(index), msg);
		module[9] = true ;
		break ;
	      }
              trackerListDeviceDetected->insertItem (msg1) ;
            }

            delete deviceList ;

	    int error = 0 ;
	    trackerErrorNumber->setText ( "0" ) ;
	    for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) {

	      keyType indexF = p->first ;
	      module = p->second ;
	      if (module != NULL) {
	  
		// 4 APVs
		bool apv4F = module[0] && module[1] && module[4] && module[5] ;
		bool pllF  = module[7] ;
		bool muxF  = module[6] ;
		bool aohF  = module[8] ;
		bool dohF  = module[9] ;
		bool dcu0F  = module[10] ;
		bool dcu78F = module[11] ;

		bool apv6F  = false ;
		if (module[2] || module[3]) apv6F = true ;
		
		// Is it a module
		if (apv6F || apv4F || pllF || muxF || aohF) {
		  char msgM[100] ;
		  snprintf (msgM, 100, "Module: FEC %d Ring %d CCU 0x%x Channel 0x%x", getFecKey(indexF),  getRingKey(indexF), getCcuKey(indexF),  getChannelKey(indexF)) ;
		  trackerListHybridDetected->insertItem (msgM) ;
		  int indexM = trackerListHybridDetected->count()-1 ;
		  //cout << "Found a module on FEC " << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << hex << getCcuKey(indexF) << " channel 0x" << hex << getChannelKey(indexF) << endl ;
		  
		  // Check each part
		  bool errorM = false ;
		  if (!module[0]) {
		    //cerr << "\t" << "Miss an APV at address 0x20" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x20") ;
		    errorM = true ;
		  }
		  if (!module[1]) {
		    //cerr << "\t" << "Miss an APV at address 0x21" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x21") ;
		    errorM = true ;
		  }
		  if (apv6F && !module[2]) { 
		    //cerr << "\t" << "Miss an APV at address 0x22" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x22") ;
		    errorM = true ;
		  }
		  if (apv6F && !module[3]) { 
		     //cerr << "\t" << "Miss an APV at address 0x23" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x23") ;
		    errorM = true ;
		  }
		  if (!module[4]) { 
		     //cerr << "\t" << "Miss an APV at address 0x24" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x24") ;
		    errorM = true ;
		  }
		  if (!module[5]) { 
		     //cerr << "\t" << "Miss an APV at address 0x25" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an APV at address 0x25") ;
		    errorM = true ;
		  }
		  if (!muxF) { 
		     //cerr << "\t" << "Miss a  MUX at address 0x43" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss a  MUX at address 0x43") ;
		    errorM = true ;
		  }
		  if (!pllF) { 
		     //cerr << "\t" << "Miss a  PLL at address 0x44" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss a  PLL at address 0x44") ;
		    errorM = true ;
		  }
		  if (!aohF) { 
		     //cerr << "\t" << "Miss an AOH at address 0x60" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss an AOH at address 0x60") ;
		    errorM = true ;
		  }
		  if (!dcu0F) { 
		     //cerr << "\t" << "Miss a  DCU at address 0x0" << endl ;
		    trackerListHybridDetected->insertItem ("\tMiss a  DCU at address 0x0") ;
		    errorM = true ;
		  }
		  if (errorM) {
		    trackerListHybridDetected->setSelected(indexM, true) ;
		    error ++ ;
		  }

		  // Display the number of errors
		  char msg[80] ;
		  sprintf (msg, "%d", error) ;
		  trackerErrorNumber->setText ( msg ) ;
		}
		else {
		  // Is it a DCU on CCU
		  if ( (dcu0F || dcu78F) && !apv6F && !apv4F && !pllF && !muxF && !aohF ) {
		    char msgM[100] ;
		    snprintf (msgM, 100, "DCU/CCU: FEC %d Ring %d CCU 0x%x Channel 0x%x", getFecKey(indexF),  getRingKey(indexF), getCcuKey(indexF),  getChannelKey(indexF)) ;
		    trackerListHybridDetected->insertItem (msgM) ;
		    //cout << "Found a DCU on CCU on FEC " << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << hex << getCcuKey(indexF) << " channel 0x" << hex << getChannelKey(indexF) << endl ;
		  }
		  // Is it a DOH
		  if (dohF) {
		    char msgM[100] ;
		    snprintf (msgM, 100, "DOH: FEC %d Ring %d CCU 0x%x Channel 0x%x", getFecKey(indexF),  getRingKey(indexF), getCcuKey(indexF),  getChannelKey(indexF)) ;
		    trackerListHybridDetected->insertItem (msgM) ;
		    //cout << "Found a DOH on FEC " << getFecKey(indexF) << " Ring " << getRingKey(indexF) << " CCU 0x" << hex << getCcuKey(indexF) << " channel 0x" << hex << getChannelKey(indexF) << endl ;
		  }
		}
	      }
	    }

	    for (Sgi::hash_map<keyType, bool *>::iterator p=trackerModule.begin();p!=trackerModule.end();p++) {
	      delete[] p->second ;
	    }
	    trackerModule.clear() ;
	  }
	  else {
            
	    char msg[80] ;
	    sprintf (msg, "No device found for the FEC %d", slot) ;
	    ErrorMessage (msg) ;
	    // ERROR
	    // cout << "No device found for the FEC " << slot << endl ;
	  }
	}
        else {
	  
          char msg[100] ; 
          sprintf (msg, "Inconsistency FEC slot: %s", fecSlots->text(i).latin1()) ;
          ErrorMessage (msg, "Please report to the developper list (fec-support@ires.in2p3.fr)") ;

          // ERROR
          // cout << "Inconsistency FEC slot: " << fecSlots->text(i) << endl ;
          // cout << "Please report to the developper list (fec-support@ires.in2p3.fr)" << endl ;
        }
      }
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Operation failed: unable to scan the ring for i2c devices", e) ; // cerr << e.what() << endl ;
  }
}

/**
 */
void FecDialogImpl::trackerWriteAll() {

  if (trackerListDeviceDetected->count() <= 0) {

    ErrorMessage ("No device to be downloaded/write", "Please detect Tracker devices or load a file") ;
    // ERROR
    // cout << "No device to be downloaded/write" << endl ;
    // cout << "Please detect Tracker devices or load a file" << endl ;

    return ;
  }
  else {

    bool sel = false ;
    for (uint i = 0 ; i < trackerListDeviceDetected->count() && !sel; i ++) {
      
      if (trackerListDeviceDetected->isSelected(i)) sel = true ;
    }

    if (!sel) {

      ErrorMessage ("No device to be downloaded/write", "Please select item to be downlaoded") ;
      // ERROR
      // cout << "No device to be downloaded/write" << endl ;
      // cout << "Please select item to be downlaoded" << endl ;
      return ;
    }
  }

  // Retreive all the values
  apvDescription *apvD = buildApvDescription () ;
  if (apvD == NULL) {
    
    ErrorMessage ("APV could not be downloaded") ;
    // ERROR
    // cout << "APV could not be downloaded" << endl ;
  }

  muxDescription *muxD = buildMuxDescription () ;
  if (muxD == NULL) {

    ErrorMessage ("MUX could not be downloaded") ;
    // ERROR
    // cout << "MUX could not be downloaded" << endl ;
  }

  pllDescription *pllD = buildPllDescription () ;
  if (pllD == NULL) {

    ErrorMessage ("PLL could not be downloaded") ;
    // ERROR
    // cout << "PLL could not be downloaded" << endl ;
  }

  laserdriverDescription *laserdriverD = buildLaserdriverDescription () ;
  if (laserdriverD == NULL) {

    ErrorMessage ("LASERDRIVER could not be downloaded") ;
    // ERROR
    // cout << "LASERDRIVER could not be downloaded" << endl ;
  }

  laserdriverDescription *dohD = buildLaserdriverDescription () ;
  if (dohD == NULL) {

    ErrorMessage ("DOH could not be downloaded") ;
    // ERROR
    // cout << "DOH could not be downloaded" << endl ;
  }
  //else {

    //tscType8 bias[MAXLASERDRIVERCHANNELS] = {24, 24, 24} ;
    //dohD->setBias ( bias ) ;
  //}

  if ( (apvD != NULL) && (muxD != NULL) && 
       (pllD != NULL) && (laserdriverD != NULL) &&
       (dohD != NULL) ) {

    // The final device description
    deviceVector vDevice ;
    apvDescription *apvD1 = NULL ;
    muxDescription *muxD1 = NULL ;
    pllDescription *pllD1 = NULL ;
    laserdriverDescription *laserdriverD1 = NULL ;
    laserdriverDescription *dohD1 = NULL ;
    dcuDescription *dcuD1 = NULL ;

    for ( uint i = 0 ; i < trackerListDeviceDetected->count() ; i ++ ) {

      keyType index = 0 ;
      if ( (trackerListDeviceDetected->isSelected(i) || 
            trackerAllDevices->isChecked() ) &&
           sscanf (trackerListDeviceDetected->text(i), "%x", &index) ) {

        enumDeviceType deviceType = getDeviceType (index) ;
        switch (deviceType) {
        case APV25:
          apvD1 = apvD->clone() ;
          apvD1->setAccessKey (index) ;
          // Add to the list to be downloaded
          vDevice.push_back(apvD1) ;
          break ;
        case PLL:
          pllD1 = pllD->clone() ;
          pllD1->setAccessKey (index) ;
          // Add to the list to be downloaded
          vDevice.push_back(pllD1) ;
          break ;
	case DOH:
	  // Information
	  ErrorMessage ("DOH bias will be set to 24") ;

          dohD1 = dohD->clone() ;
          dohD1->setAccessKey (index) ;
          // Add to the list to be downloaded
          vDevice.push_back(dohD1) ;
          break ;
        case LASERDRIVER:
          laserdriverD1 = laserdriverD->clone() ;
          laserdriverD1->setAccessKey (index) ;
          // Add to the list to be downloaded
          vDevice.push_back(laserdriverD1) ;
          break ;
        case APVMUX:
          muxD1 = muxD->clone() ;
          muxD1->setAccessKey (index) ;
          // Add to the list to be downloaded
          vDevice.push_back(muxD1) ;
          break ;
        case DCU:
          // No write can be performed on DCU so the description 
          dcuD1 = new dcuDescription ( index, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ) ;
          // Add to the list to be downloaded
          vDevice.push_back(dcuD1) ;          
          break ;
        default:
          char msg[100], msg1[200] ;
          decodeKey (msg, index) ;

          sprintf (msg1, "Unknown device at address 0x%04X (%s)", getAddressKey(index), msg) ;
          ErrorMessage (msg1) ;
          // ERROR
          // cout << "Unknown device at address " << getAddressKey(index) << " (" << msg << ")" << endl ;
        }
      }
    }

    if (vDevice.size() > 0) {

      // console display
      // FecFactory::display(vDevice) ;

      try {

	std::list<FecExceptionHandler *> errorList  ;
        uint error = fecAccessManager_->downloadValues ( &vDevice, errorList ) ;
	displayErrorMessage ("Error during the download", errorList) ;

        if (error != 0) {

          char msg[80] ;
          sprintf (msg, "%d", error) ;
          trackerErrorNumber->setText ( msg ) ;          
          ErrorMessage ("One or several errors occurs during the download") ;
          // ERROR
          // cout << "One or several errors occurs during the download" << endl ;
        }
        //else {

        //  ErrorMessage ("No error during the download") ;
        //}
        if (trackerDoComparison->isChecked()) 
          trackerReadAll(apvD, muxD, pllD, laserdriverD, dohD) ;
      }
      catch (FecExceptionHandler &e) {

        // ERROR
	ErrorMessage ("Unable to download devices", e) ; // cerr << e.what() << endl ;
      }
    }
    else {

      ErrorMessage ("No device found to be uploaded", "Please download/write values and upload/read it") ;
      
      // ERROR
      // cout << "No device found to be uploaded" << endl ;
      // cout << "Please download/write values and upload/write it" << endl ;
    }

    // Delete all
    delete apvD ; 
    delete apvD1 ;
    delete muxD ; 
    delete muxD1 ;
    delete pllD ; 
    delete pllD1 ;
    delete laserdriverD ; 
    delete laserdriverD1 ;
    delete dohD ; delete dohD1 ;

    vDevice.clear() ;
  }
}

/**
 */
void FecDialogImpl::trackerSetAllDevicesSelected() {

  if (!trackerAllDevices->isChecked())
    for (uint i = 0 ; i < trackerListDeviceDetected->count() ; i ++) {

      //if (! trackerListDeviceDetected->isSelected(i))
      trackerListDeviceDetected->setSelected (i, true) ;
    }
  else {
    trackerListDeviceDetected->clearSelection() ;
    trackerAllDevices->setChecked(false);
  }
}

/**
 */
void FecDialogImpl::trackerSetDefaultValues() {

  // APV
  apvModeT->setText("0x2B") ;
  apvLatencyT->setText("0x64") ;
  apvMuxGainT->setText("0x04") ;
  apvIpreT->setText("0x73") ;
  apvIpcascT->setText("0x3C") ;
  apvIpsfT->setText("0x32") ;
  apvIshaT->setText("0x32") ;
  apvIssfT->setText("0x32") ;
  apvIpspT->setText("0x50") ;
  apvImuxinT->setText("0x32") ;
  apvIcalT->setText("0x50") ;
  apvIspareT->setText("0x00") ;
  apvVfpT->setText("0x43") ;
  apvVfsT->setText("0x43") ;
  apvVpspT->setText("0x14") ;
  apvCdrvT->setText("0xFB") ;
  apvCselT->setText("0xFE") ;
  apvErrorT->setText("0x00") ;

  // PLL
  pllDelayFineT->setText("0x06") ;
  pllDelayCoarseT->setText("0x01") ;

  // MUX
  apvMuxResistorT->setText("0xFF") ;

  // Laserdriver
  laserdriverGain1T->setText("0x02") ;
  laserdriverGain2T->setText("0x02") ;
  laserdriverGain3T->setText("0x02") ;
  laserdriverBias1T->setText("0x12") ;
  laserdriverBias2T->setText("0x12") ;
  laserdriverBias3T->setText("0x12") ;

  // DCU
  dcuHardIdT->setText("0x000000") ;
  dcuChannel1T->setText("0") ;
  dcuChannel2T->setText("0") ;
  dcuChannel3T->setText("0") ;
  dcuChannel4T->setText("0") ;
  dcuChannel5T->setText("0") ;
  dcuChannel6T->setText("0") ;
  dcuChannel7T->setText("0") ;
  dcuChannel8T->setText("0") ;
}

/**
 */
void FecDialogImpl::trackerListDeviceDetectedChanged() {

  trackerAllDevices->setChecked(false);
}

/**
 */
void FecDialogImpl::trackerReadAll() {

  apvDescription *oldApv = NULL ;
  muxDescription *oldMux = NULL ;
  pllDescription *oldPll = NULL ;
  laserdriverDescription *oldLaserdriver = NULL ;
  laserdriverDescription *oldDoh = NULL ;

  if (trackerDoComparison->isChecked()) {

    oldApv = buildApvDescription() ;
    oldMux = buildMuxDescription() ;
    oldPll = buildPllDescription() ;
    oldLaserdriver = buildLaserdriverDescription() ;
    oldDoh = buildLaserdriverDescription() ;
    tscType8 bias[MAXLASERDRIVERCHANNELS] = {24, 24, 24} ;
    oldDoh->setBias ( bias ) ;
  }

  trackerReadAll (oldApv, oldMux, oldPll, oldLaserdriver, oldDoh) ;

  // Delete all
  if (oldApv != NULL) delete oldApv ;
  if (oldMux != NULL) delete oldMux ;
  if (oldPll != NULL) delete oldPll ;
  if (oldLaserdriver != NULL) delete oldLaserdriver ;
  if (oldDoh != NULL) delete oldDoh ;
}

/**
 * for comparison
 */
void FecDialogImpl::trackerReadAll( apvDescription *oldApv, muxDescription *oldMux,
                                    pllDescription *oldPll, 
                                    laserdriverDescription *oldLaserdriver,
				    laserdriverDescription *oldDoh ) {

  deviceVector *vDevice = NULL ;

  try {

    std::list<FecExceptionHandler *> errorList  ;
    vDevice = fecAccessManager_->uploadValues (errorList, false) ; //trackerDoComparison->isChecked()) ;
    displayErrorMessage ("Error during the upload", errorList) ;

    // console display
    // fecAccessManager_->displayDeviceVector(vDevice) ;
  }
  catch (FecExceptionHandler &e) {

    // ERROR
    ErrorMessage ("Unable to upload devices", e) ; // cerr << e.what() << endl ;
  }

  if ( (vDevice != NULL) && (!vDevice->empty()) ) {

    for (uint it = 0 ; it < trackerListDeviceDetected->count() ; it ++) {

      keyType index = 0 ;
      if (sscanf (trackerListDeviceDetected->text(it), "%x", &index)) {

        unsigned int error = 0 ;
        // For each device => access it
        for (deviceVector::iterator device = vDevice->begin() ; device != vDevice->end() ; device ++) {

          deviceDescription *deviced = *device ;

          if ( trackerAllDevices->isChecked() ||
               ( trackerListDeviceDetected->isSelected(it) &&
                 index == deviced->getKey() ) ) {

            pllDescription         *pllDevice     = NULL ;
            apvDescription         *apvDevice     = NULL ;
            laserdriverDescription *laserdriverDevice = NULL ;
            laserdriverDescription *dohDevice     = NULL ;
            muxDescription         *muxDevice     = NULL ;
            dcuDescription         *dcuDevice     = NULL ;

            // Find the correct item => change the color to rigth value
            char msg[80] ;
            switch (deviced->getDeviceType()) {
            case APV25:

              apvDevice = (apvDescription *)deviced ;

              // Do the comparison
              if (trackerDoComparison->isChecked()) {
                oldApv->setAccessKey (index) ;
                bool sel = false ;
                if (*oldApv == *apvDevice) {

                  error += 1 ;
                  sel = true ;
                }
                trackerListDeviceDetected->setSelected(it, sel) ;
              }

              // Set the values on text
              sprintf (msg, "0x%02X", (uint)apvDevice->getApvMode()) ;
              apvModeT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getLatency()) ;
              apvLatencyT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getMuxGain()) ;
              apvMuxGainT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIpre()) ;
              apvIpreT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIpcasc()) ;
              apvIpcascT->setText (msg);
              sprintf (msg, "0x%02X", (uint)apvDevice->getIpsf()) ;
              apvIpsfT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIsha()) ;
              apvIshaT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIssf()) ;
              apvIssfT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIpsp()) ;
              apvIpspT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getImuxin()) ;
              apvImuxinT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIcal()) ;
              apvIcalT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getIspare()) ;
              apvIspareT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getVfp()) ;
              apvVfpT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getVfs()) ;
              apvVfsT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getVpsp()) ;
              apvVpspT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getCdrv()) ;
              apvCdrvT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getCsel()) ;
              apvCselT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)apvDevice->getApvError()) ;
              apvErrorT->setText (msg) ;
              break ;
            case PLL:
              pllDevice = (pllDescription *)deviced ;

              // Do the comparison
              if (trackerDoComparison->isChecked()) {
                oldPll->setAccessKey (index) ;
                bool sel = false ;
                if (*oldPll == *pllDevice) {

                  error += 1 ;
                  sel = true ;
                }
                trackerListDeviceDetected->setSelected(it, sel) ;
              }

              sprintf (msg, "0x%02X", (uint)pllDevice->getDelayFine()) ;
              pllDelayFineT->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)pllDevice->getDelayCoarse()) ;
              pllDelayCoarseT->setText (msg) ;
              break ;
	    case DOH:
	      dohDevice = (laserdriverDescription *)deviced ;
              // Do the comparison
              if (trackerDoComparison->isChecked()) {
                oldDoh->setAccessKey (index) ;
                bool sel = false ;
                if (*oldDoh == *dohDevice) {

                  error += 1 ;
                  sel = true ;
                }
                trackerListDeviceDetected->setSelected(it, sel) ;
              }
	      break ;
            case LASERDRIVER:
              laserdriverDevice = (laserdriverDescription *)deviced ;

              // Do the comparison
              if (trackerDoComparison->isChecked()) {
                oldLaserdriver->setAccessKey (index) ;
                bool sel = false ;
                if (*oldLaserdriver == *laserdriverDevice) {

                  error += 1 ;
                  sel = true ;
                }
                trackerListDeviceDetected->setSelected(it, sel) ;
              }

              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getGain0()) ;
              laserdriverGain1T->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getGain1()) ;
              laserdriverGain2T->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getGain2()) ;
              laserdriverGain3T->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getBias0()) ;
              laserdriverBias1T->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getBias1()) ;
              laserdriverBias2T->setText (msg) ;
              sprintf (msg, "0x%02X", (uint)laserdriverDevice->getBias2()) ;
              laserdriverBias3T->setText (msg) ;
              break ;
            case APVMUX:
              muxDevice = (muxDescription *)deviced ;

              // Do the comparison
              if (trackerDoComparison->isChecked()) {
                oldApv->setAccessKey (index) ;
                bool sel = false ;
                if (*oldMux == *muxDevice) {

                  error += 1 ;
                  sel = true ;
                }
                trackerListDeviceDetected->setSelected(it, sel) ;
              }

              sprintf (msg, "0x%02X", (uint)muxDevice->getResistor()) ;
              apvMuxResistorT->setText (msg) ;
              break ;
            case DCU:
              dcuDevice = (dcuDescription *)deviced ;
              sprintf (msg, "0x%02X", (uint)dcuDevice->getDcuHardId()) ;
              dcuHardIdT->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel0()) ;
              dcuChannel1T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel1()) ;
              dcuChannel2T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel2()) ;
              dcuChannel3T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel3()) ;
              dcuChannel4T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel4()) ;
              dcuChannel5T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel5()) ;
              dcuChannel6T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel6()) ;
              dcuChannel7T->setText (msg) ;
              sprintf (msg, "%d", (uint)dcuDevice->getDcuChannel7()) ;
              dcuChannel8T->setText (msg) ;
              break ;
            default:
              decodeKey (msg, deviced->getKey()) ;

              char msg1[200] ;
              decodeKey (msg, index) ;
              sprintf (msg1, "Unknown device, the address does not match with any Tracker device (%s)", msg) ;
              ErrorMessage (msg1) ;
              // ERROR
              // cout << "Unknown device: " << deviced->getDeviceType() << "(" << msg << ")" << endl ;
            }
          }
        }
      }
      else {

        char msg[100] ; 
        sprintf (msg, "Inconsistency index value: %s", trackerListDeviceDetected->text(it).latin1()) ;
        ErrorMessage (msg, "Please report to the developper list (fec-support@ires.in2p3.fr)") ;

        // ERROR
        // cout << "Inconsistency index value: " << trackerListDeviceDetected->text(it) << endl ;
        // cout << "Please report to the developper list (fec-support@ires.in2p3.fr)" << endl ;
      }
    }

    for (deviceVector::iterator device = vDevice->begin() ; 
         device != vDevice->end() ; device ++) delete *device ;
    vDevice->clear() ;
  }
  else {

    ErrorMessage ("No device found to be uploaded", "Please select device and download/write values first and finally upload/read it") ;

    // ERROR
    // cout << "No device found to be uploaded" << endl ;
    // cout << "Please download/write values and upload/write it" << endl ;
  }

  if (vDevice != NULL) delete vDevice ;
}

/** \brief Clear the list of PIA
 */
void FecDialogImpl::trackerClearAll() {

  trackerListDeviceDetected->clear() ;
  trackerListHybridDetected->clear() ;
  trackerListPiaReset->clear() ;
  trackerAllDevices->setChecked(false);
}

/**
 */
void FecDialogImpl::trackerFecEnableCounters() {

#ifndef FECSOFTWAREV2_0
  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      fecEnableErrorCounter->setChecked(!trackerFecEnableErrorCounter->isChecked()) ;

      fecAccess_->setErrorCounting (buildFecRingKey(slot,ringSlot), !trackerFecEnableErrorCounter->isChecked()) ; 

      fecReadDDStatus() ;
    }
    else {

      fecEnableErrorCounter->setChecked (false) ;

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 
          
    fecEnableErrorCounter->setChecked (true) ;

    // ERROR 
    ErrorMessage ("Unable to enable the device driver counters", e) ; // cerr << e.what() << endl ;
  }
#endif
}

/** 
 */
void FecDialogImpl::trackerAddPiaKey ( ) {

  keyType index = 0 ;

  uint slot, ringSlot; 
  if (trackerPiaFecSlots->count() && sscanf (trackerPiaFecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
    
    uint ccuAddress ;
    if (trackerPiaCcuAddresses->count() && sscanf (trackerPiaCcuAddresses->currentText(), "%x", &ccuAddress)) {
      
      uint channel ;
      if (trackerPiaChannels->count() && sscanf (trackerPiaChannels->currentText(), "%d", &channel)) {
        
        index = buildCompleteKey(slot,ringSlot,ccuAddress,channel,0) ;

        bool alAdded = false ;
        for (uint i = 0 ; i < trackerListPiaReset->count() && !alAdded; i ++) {
          
          keyType index1 = 0 ;
          if (sscanf (trackerListPiaReset->text(i), "%x", &index1) && (index == index1)) alAdded = true ;
        }

        if (!alAdded) {

          char msg100[100] ;
          sprintf (msg100, "0x%X - FEC 0x%X CCU 0x%02X Channel %d", index, slot, ccuAddress, channel) ;
          trackerListPiaReset->insertItem (msg100);
        }
      }
      else {
          
        ErrorMessage ("Error the channel number enter is not correct or empty") ;
        // ERROR
        // cout << "Error the channel number enter is not correct or empty" << endl ;
      }
    }
    else {
      
      ErrorMessage ("Error the CCU address enter is not correct or empty", "Please use scan CCUs before use read/write." ) ;
      // ERROR
      // cout << "Error the ccu address enter is not correct or empty" << endl ;
      // cout << "Please use scan CCUs before use read/write" << endl ;
    }
  }
  else {

    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

    // ERROR
    // cout << "Error the slot enter is not correct or empty" << endl ;
    // cout << "Please scan FECs before use read/write" << endl ;
  }
}

/** 
 */
void FecDialogImpl::trackerDeletePiaKey ( ) {

  for (uint i = 0 ; i < trackerListPiaReset->count(); i ++) {

    if (trackerListPiaReset->isSelected(i)) trackerListPiaReset->removeItem(i) ;
  }
}


/** Take all CCUs and add all PIA channels
 */
void FecDialogImpl::trackerPiaResetAddAllCCUKey() {

  keyType index = 0 ;
  uint slot, ringSlot, ccuAddress, piaChannel ; 

  if (!trackerPiaFecSlots->count() ||
      !trackerPiaCcuAddresses->count() ||
      !trackerPiaChannels->count()) {

    ErrorMessage ("No FEC or no CCU or no channels are declared, cannot be add something") ;
    // ERROR
    // cout << "Error: No FEC or no CCU or no channels are declared, cannot be add something" << endl ;

    return ;
  }

  // for each FEC slot
  for ( uint slotIn = 0 ; slotIn < trackerPiaFecSlots->count() ; slotIn ++ ) {

    if (sscanf (trackerPiaFecSlots->text(slotIn), "%d,%d", &slot, &ringSlot)) {

      // for each CCU
      for ( uint ccuIn = 0 ; ccuIn < trackerPiaCcuAddresses->count() ; ccuIn ++ ) {

	if (sscanf (trackerPiaCcuAddresses->text(ccuIn), "%x", &ccuAddress)) {

	  // For each PIA channels
	  for ( uint piaIn = 0 ; piaIn < trackerPiaChannels->count() ; piaIn ++ ) {

	    if (sscanf (trackerPiaChannels->text(piaIn), "%x", &piaChannel)) {

	      // Add this one to the list
	      index = buildCompleteKey(slot,ringSlot,ccuAddress,piaChannel,0) ;
	      bool alAdded = false ;
	      for (uint i = 0 ; i < trackerListPiaReset->count() && !alAdded; i ++) {
          
		keyType index1 = 0 ;
		if (sscanf (trackerListPiaReset->text(i), "%x", &index1) && (index == index1)) alAdded = true ;
	      }

	      if (!alAdded) {

		char msg100[100] ;
		sprintf (msg100, "0x%X - FEC 0x%X CCU 0x%02X Channel %d", index, slot, ccuAddress, piaChannel) ;
		trackerListPiaReset->insertItem (msg100);
	      }
	    }
	  }
	}
      }
    }
  }
}

/**
 */
void FecDialogImpl::trackerWritePiaReset () {

  // Build a description of pia reset
  piaResetVector vPiaReset ;

  for (uint i = 0 ; i < trackerListPiaReset->count() ; i ++) {

    keyType index = 0 ;
    if (sscanf (trackerListPiaReset->text(i), "%x", &index)) {

      uint duration = 0, sleepT = 0, mask = 0 ;
      if ( sscanf (trackerPiaDuration->text(), "%x", &duration) && 
           sscanf (trackerPiaSleepTime->text(), "%x", &sleepT) &&
           sscanf (trackerPiaDataRegister->text(), "%x", &mask) ) {

        vPiaReset.push_back(new piaResetDescription(index, duration, sleepT, mask));
      }
      else {

        ErrorMessage ("PIA duration and/or sleep time are not correct", "Please put right values") ;

        // ERROR
        // cout << "PIA duration and/or sleep time are not correct" << endl ;
        // cout << "Please put right values" << endl ;
      }
    }
    else {

      char msg[100] ; 
      sprintf (msg, "Inconsistency index value: %s", trackerListPiaReset->text(i).latin1()) ;
      ErrorMessage (msg, "Please report to the developper list (fec-support@ires.in2p3.fr)") ;

      // ERROR
      // cout << "Inconsistency index value: " << trackerListPiaReset->text(i) << endl ;
      // cout << "Please report to the developper list (fec-support@ires.in2p3.fr)" << endl ;
    }
  }  

  if (! vPiaReset.empty()) {
    try {

      uint loop = 1 ;
      if (sscanf (trackerLoopNumber->text(), "%d", &loop) && (loop > 0)) {

        for (uint i = 0 ; i < loop ; i ++) {
	  std::list<FecExceptionHandler *> errorList  ;
          fecAccessManager_->resetPiaModules (&vPiaReset,errorList) ;
	  displayErrorMessage ("Error during the PIA reset", errorList) ;
        }
      }
      else {

        ErrorMessage ("Please enter a correct value in the loop field") ;
        // ERROR
        // cout << "Please enter a correct value in the loop field" << endl ;
      }
    }
    catch (FecExceptionHandler &e) { 

      // ERROR 
      ErrorMessage ("Unable to reset modules through PIA", e) ; // cerr << e.what() << endl ;
    }
  }
  else {

    ErrorMessage ("No PIA reset defined", "Please add PIA reset.") ;
    // ERROR
    // cout << "No PIA reset defined" << endl ;
    // cout << "Please add PIA reset" << endl ;
  }
}

/** \brief If the database is not set, then the toggle file must stay checked
 */
void FecDialogImpl::trackerFileToggle() {

  trackerUseDatabase->setChecked(false) ;
  trackerUseFile->setChecked(true) ;
}

/** \brief If the database is not set, then the toggle file must stay checked
 */
void FecDialogImpl::trackerDatabaseToggle() {

  trackerUseFile->setChecked(false) ;  
  trackerUseDatabase->setChecked(true) ;  
}

/** \brief delete a device from the list
 */
void FecDialogImpl::trackerDeleteDeviceList() {

  for (uint i = 0 ; i < trackerListDeviceDetected->count(); i ++) {

    if (trackerListDeviceDetected->isSelected(i)) {

      trackerListDeviceDetected->removeItem(i) ;
      i -- ;
    }
  }
}

/** \brief State to be configured for each partition (selected in the box)
 */
void FecDialogImpl::trackerSelectPartition ( ) {

  //ErrorMessage ("Coucou") ;
}  

/** \brief Select all PIA devices in the the list
 */
void FecDialogImpl::trackerSetAllPiaSelected() {

  for (uint i = 0 ; i < trackerListPiaReset->count() ; i ++) {

    //if (! trackerListPiaReset->isSelected(i))
    trackerListPiaReset->setSelected (i, true) ;
  }
}

/** \brief Write each PIA value with the corresponding delay
 */
void FecDialogImpl::trackerWritePiaSelected() {

  if (trackerListPiaReset->count() <= 0) {

    ErrorMessage ("No device to be downloaded/write", "Please detect Tracker devices or load a file") ;
    // ERROR
    // cout << "No device to be downloaded/write" << endl ;
    // cout << "Please detect Tracker devices or load a file" << endl ;

    return ;
  }
  else {

    bool sel = false ;
    for (uint i = 0 ; i < trackerListPiaReset->count() && !sel; i ++) {
      
      if (trackerListPiaReset->isSelected(i)) sel = true ;
    }

    if (!sel) {

      ErrorMessage ("No PIA definition to be downloaded/write", "Please select item to be downlaoded") ;
      // ERROR
      // cout << "No device to be downloaded/write" << endl ;
      // cout << "Please select item to be downlaoded" << endl ;
      return ;
    }
  }


  // Build a description of pia reset
  piaResetVector vPiaReset ;

  for (uint i = 0 ; i < trackerListPiaReset->count() ; i ++) {

    keyType index = 0 ;
    if (trackerListPiaReset->isSelected(i) &&
        sscanf (trackerListPiaReset->text(i), "%x", &index)) {

      uint duration = 0, sleepT = 0, mask = 0 ;
      if ( sscanf (trackerPiaDuration->text(), "%x", &duration) && 
           sscanf (trackerPiaSleepTime->text(), "%x", &sleepT) &&
           sscanf (trackerPiaDataRegister->text(), "%x", &mask) ) {

        vPiaReset.push_back(new piaResetDescription(index, duration, sleepT, mask));
      }
      else {

        ErrorMessage ("PIA duration and/or sleep time are not correct", "Please put right values") ;

        // ERROR
        // cout << "PIA duration and/or sleep time are not correct" << endl ;
        // cout << "Please put right values" << endl ;
      }
    }
    else {

      char msg[100] ; 
      sprintf (msg, "Inconsistency index value: %s", trackerListPiaReset->text(i).latin1()) ;
      ErrorMessage (msg, "Please report to the developper list (fec-support@ires.in2p3.fr)") ;

      // ERROR
      // cout << "Inconsistency index value: " << trackerListPiaReset->text(i) << endl ;
      // cout << "Please report to the developper list (fec-support@ires.in2p3.fr)" << endl ;
    }
  }  

  if (! vPiaReset.empty()) {
    try {

      uint loop = 1 ;
      if (sscanf (trackerLoopNumber->text(), "%d", &loop) && (loop > 0)) {

        for (uint i = 0 ; i < loop ; i ++) {
	  std::list<FecExceptionHandler *> errorList ;
          fecAccessManager_->resetPiaModules (&vPiaReset, errorList) ;
	  displayErrorMessage ("Error during the PIA reset", errorList) ;
        }
      }
      else {

        ErrorMessage ("Please enter a correct value in the loop field") ;
        // ERROR
        // cout << "Please enter a correct value in the loop field" << endl ;
      }
    }
    catch (FecExceptionHandler &e) { 

      // ERROR 
      ErrorMessage ("Unable to reset modules through PIA", e) ; // cerr << e.what() << endl ;
    }
  }
  else {

    ErrorMessage ("No PIA reset defined", "Please add PIA reset.") ;
    // ERROR
    // cout << "No PIA reset defined" << endl ;
    // cout << "Please add PIA reset" << endl ;
  }
}

/** \brief Find a file and load it to the differents lists (devices + PIA)
 */
void FecDialogImpl::trackerFindXMLFile() {

  // Try to read file
  QString fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
  trackerFileName->setText (fn) ;

  // File checked
  if (! fn.isEmpty()) trackerUseFile->setChecked(true) ;

  // Load it
  trackerLoadXML ( ) ;
}

/** \brief Find a file to be saved
 */
void FecDialogImpl::trackerFindSaveXMLFile() {

  // Try to save file
  QString fn = QFileDialog::getSaveFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
  trackerFileName->setText (fn) ;
}

/** \brief Save the upload devices in database or file depends of the check box (file/database)
 * If the database is set: upload in database in a new version
 * If the file is set upload the devices and pia in a file
 */
void FecDialogImpl::trackerSaveXML() {

#ifdef DATABASEFACTORY
  deviceVector *vDevice = NULL ;
    
  // Retreive all the devices from the parsing class
  std::list<FecExceptionHandler *> errorList  ;
  vDevice = fecAccessManager_->uploadValues ( errorList, false ) ;
  displayErrorMessage ("Error during the upload", errorList) ;
    
  // Pia reset vector
  piaResetVector *vPiaReset = new piaResetVector() ;
  trackerRetreivePiaDescription ( vPiaReset ) ;
  
  if (vDevice->empty()) {
      
    ErrorMessage ("The devices saved in XML file comes from the upload set. So download and upload values") ;      
  }
  else {
      
    if (vPiaReset->empty()) {
      
      ErrorMessage ("Warning: No pia reset selected, continue without") ;
    }
      
    if (! trackerUseFile->isChecked()) {

#ifdef DATABASEFACTORY
      // Database connection
      try {
	// Remove the timestamp
	char partition[100] ;
	strncpy (partition, trackerPartitionName->text().latin1(), 100) ;

	// tokenize it
	char *str = strtok (partition, "_") ;
	trackerPartitionName->setText(str) ;

	// Retreive the name of the partition
	long timestamp = time(NULL) ;
	if (trackerPartitionName->text().isEmpty ()) {
	  snprintf (partition, 100, "partition_%d", timestamp) ;
	}
	else {
	  char partition1[100] ;
	  strncpy (partition1, trackerPartitionName->text().latin1(), 80) ;
	  snprintf (partition, 100, "%s_%d", partition1, timestamp) ;
	}

	trackerPartitionName->setText(partition) ;

	// Remove the timestamp
	char structure[100] ;
	strncpy (structure, trackerStructureName->text().latin1(), 100) ;

	// tokenize it
	str = strtok (structure, "_") ;
	trackerStructureName->setText(str) ;

	// Retreive the name of the partition
	if (trackerStructureName->text().isEmpty ()) {
	  snprintf (structure, 100, "structure_%d", timestamp) ;
	}
	else {
	  char structure1[100] ;
	  strncpy (structure1, trackerStructureName->text().latin1(), 80) ;
	  snprintf (structure, 100, "%s_%d", structure1, timestamp) ;
	}

	trackerStructureName->setText(structure) ;
	
	// XML upload in database
	unsigned int major, minor, piaMajor, piaMinor ;
	fecFactory_->createPartition (*vDevice, *vPiaReset, &major, &minor, partition) ;
	char msg[1000] ;
	sprintf (msg, "Upload a new version %d.%d in the database", major, minor) ;
	ErrorMessage (msg) ;

	// Reload it
	trackerLoadXML ( ) ;
      }
      catch (FecExceptionHandler &e) {
	  
	char msg[1000] ;
	sprintf (msg, "Unable to upload values inside the database %s@%s", fecFactory_->getDatabaseAccess()->getDbLogin().c_str(), fecFactory_->getDatabaseAccess()->getDbPath().c_str()) ;
	ErrorMessage (msg) ;
	
	cerr << "*********** ERROR *************" << endl ; 
	cerr << "Unable to upload values inside the database " << fecFactory_->getDatabaseAccess()->getDbLogin() << "@" << fecFactory_->getDatabaseAccess()->getDbPath() << endl ;
	cerr << e.what() << endl ; 
	cerr << "*******************************" << endl ;
      }
#else
      ErrorMessage ("Could not upload in database, the database is not set", "Incoherent software problem, please report to the list") ;
#endif
	
    }
    else {

      trackerFindSaveXMLFile() ;
      QString fn = trackerFileName->text() ;
      
      if (! fn.isEmpty ()) {
	
	char str[200] ;
	sprintf (str, "%s", fn.latin1()) ;
	
	try {
	  fecFactory_->setOutputFileName (fn.latin1()) ;
	  fecFactory_->setFecDevicePiaDescriptions (*vDevice, *vPiaReset) ;
	}
	catch (FecExceptionHandler &e) { 
	  
	  // ERROR 
	  ErrorMessage ("Error during the upload in file", e) ; // cerr << e.what() << endl ; 
	}
      }
      else {
	    
	ErrorMessage ("Error: unable to perform an upload, the ouput buffer is not initialised") ;
      }
    }
  }
#endif
}

/** \brief Load the devices and PIA values from database or file
 */
void FecDialogImpl::trackerLoadXML() {

#ifdef DATABASEFACTORY  
  if (trackerUseFile->isChecked()) {

    QString fn = trackerFileName->text() ;
    if (fn.isEmpty ()) {
      
      QString fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
      trackerFileName->setText (fn) ;
    }
    fn = trackerFileName->text() ;
    
    if (! fn.isEmpty ()) {
      
      try {


	// Parse a file for PIA reset and for devices
	fecFactory_->addFileName (fn.latin1()) ;
	
	// Load it from file
	deviceVector vDevice ;
	fecFactory_->getFecDeviceDescriptions(vDevice) ;
	piaResetVector piaVector ;
	fecFactory_->getPiaResetDescriptions(piaVector) ;
	trackerSetListXML(piaVector, vDevice) ;

	// Set the name
	trackerFileName->setText (fn) ;
      }
      catch (FecExceptionHandler &e) {
    
	ErrorMessage ("Error during the download from database", e) ;
	//cerr << "*********** ERROR ********************************" << endl ; 
	//cerr << e.what()  << endl ;
	//cerr << "**************************************************" << endl ;
      }      

    }
    else {

      ErrorMessage ("Please set or find a filename before loading it") ;

      // ERROR
      // cout << "Please set or find a filename before loading it" << endl ;
    }
  }
#ifdef DATABASEFACTORY
  else
    if (trackerUseDatabase->isChecked()) {

      try {
	// Database access to retreive the device values
	// Database access allowed
	// Create the parser and retreive the different devices

	trackerDatabaseVersion->clear() ;

	if (partitionName_ != "nil") {
	  // Get the current version
	  unsigned int versionMajor, versionMinor, partitionNumber ;
	  fecFactory_->getFecDevicePartitionVersion(partitionName_, &versionMajor, &versionMinor, &partitionNumber) ;
    
	  cout << "Access the database and try to retreive values: " << endl ; 
	  char msg[100] ;
	  sprintf (msg, "Partition %d: Version %d.%d", partitionNumber, versionMajor, versionMinor) ;
	  trackerDatabaseVersion->insertItem(msg) ;

	  deviceVector vDevice ;
	  piaResetVector vPiaReset ;

	  if (fecHardwareId_ != "0")
	    fecFactory_->getFecDeviceDescriptions(partitionName_,fecHardwareId_,vDevice) ;
	  else 
	    fecFactory_->getFecDeviceDescriptions(partitionName_,vDevice) ;
	  
	  if (fecHardwareId_ != "0")
	    fecFactory_->getPiaResetDescriptions(partitionName_,fecHardwareId_,vPiaReset) ;
	  else 
	    fecFactory_->getPiaResetDescriptions(partitionName_,vPiaReset) ;
	  
	    trackerSetListXML(vPiaReset,vDevice) ;
	}
	else {
	  ErrorMessage ("Error cannot access data from database", "To use the database you need to specify at least the partition name, type GUIDebugger -help for more information") ;
	}
      }
      catch (FecExceptionHandler &e) {
    
	ErrorMessage ("Error during the download from database", e) ;
	cerr << "*********** ERROR ********************************" << endl ; 
	cerr << e.what()  << endl ;
	cerr << "**************************************************" << endl ;
      }
      catch (oracle::occi::SQLException &e) {

	ErrorMessage ("Error during the download from database", e.what()) ;
	cerr << "*********** ERROR ********************************" << endl ; 
	cerr << e.what()  << endl ;
	cerr << "**************************************************" << endl ;
      }
    }
#endif

#endif
  
}

/** \brief Add the different values from the vectors into the lists of PIA and devices
 * \param vPiaReset - vector of PIA reset
 * \param vDevice - vector of descriptions
 */
void FecDialogImpl::trackerSetListXML ( piaResetVector vPiaReset, deviceVector vDevice ) {

  // Clear the previous lists
  trackerListPiaReset->clear() ;
  trackerListDeviceDetected->clear() ;
  trackerListHybridDetected->clear() ;
  trackerAllDevices->setChecked(false) ;

  // Create the access and reset the modules
  if ( ! vPiaReset.empty() ) {
    
    // For each value => access it
    for (piaResetVector::iterator piaResetIt = vPiaReset.begin() ; 
	 piaResetIt != vPiaReset.end() ; piaResetIt ++) {  
      
      piaResetDescription *piaResetDes = *piaResetIt ;
	
      keyType index = piaResetDes->getKey() ;
      char msg[100] ;
      sprintf (msg, "0x%X - FEC 0x%X CCU 0x%02X Channel %d", 
	       index, getFecKey(index), 
	       getCcuKey(index), getChannelKey(index)) ;
      trackerListPiaReset->insertItem (msg) ;        
      
      sprintf (msg, "0x%02X", (uint)piaResetDes->getMask()) ;
      trackerPiaDataRegister->setText (msg) ;
      sprintf (msg, "%d", (uint)piaResetDes->getDelayActiveReset()) ;
      trackerPiaDuration->setText (msg) ;
      sprintf (msg, "%d", (uint)piaResetDes->getIntervalDelayReset()) ;
      trackerPiaSleepTime->setText (msg) ;
    }
  }
  else {
    
    ErrorMessage ("No PIA reset tags found") ;
    // ERROR
    // cout << "-------------- No PIA reset tags found in file " << fn << endl ;
  }
  
  // Create the access and reset the modules
  if ( ! vDevice.empty() ) {

    // For each device => access it
    for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {
      
      deviceDescription *deviced = *device ;
	
      keyType index = deviced->getKey() ;
      char msg[100], msg1[200] ;
      decodeKey (msg, index) ;
      enumDeviceType deviceType = getDeviceType (index) ;
      switch (deviceType) {
      case APV25:
	sprintf (msg1, "0x%X - APV 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      case APVMUX:
	sprintf (msg1, "0x%X - MUX 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      case PLL:
	sprintf (msg1, "0x%X - PLL 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      case DCU:
	sprintf (msg1, "0x%X - DCU 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      case LASERDRIVER:
	sprintf (msg1, "0x%X - AOH 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      case DOH:
	sprintf (msg1, "0x%X - DOH 0x%02X/0x%02X - %s", 
		 index, getCcuKey(index), getChannelKey(index), msg) ;
	break ;
      }
      trackerListDeviceDetected->insertItem (msg1) ;
    }
  }
  else {
    ErrorMessage ("No devices found") ;
    // ERROR
    // cout << "-------------- No PIA reset tags found in file " << fn << endl ;
  }
}

/** \brief Display all PIA in the list 
 * \param vPiaReset - vector of PIA reset to be displayed in the list
 */
void FecDialogImpl::trackerRetreivePiaDescription ( piaResetVector *vPiaReset ) {

  // Build a description of pia reset
  for (uint i = 0 ; i < trackerListPiaReset->count() ; i ++) {

    keyType index = 0 ;
    if (sscanf (trackerListPiaReset->text(i), "%x", &index)) {

      uint duration = 0, sleepT = 0, mask = 0 ;
      if ( sscanf (trackerPiaDuration->text(), "%x", &duration) && 
           sscanf (trackerPiaSleepTime->text(), "%x", &sleepT) &&
           sscanf (trackerPiaDataRegister->text(), "%x", &mask) ) {
        
        vPiaReset->push_back(new piaResetDescription(index, duration, sleepT, mask));
      }
      else {

        ErrorMessage ("PIA duration and/or sleep time are not correct", "Please put right values") ;

        // ERROR
        // cout << "PIA duration and/or sleep time are not correct" << endl ;
        // cout << "Please put right values" << endl ;
      }
    }
    else {

      char msg[100] ; 
      sprintf (msg, "Inconsistency index value: %s", trackerListPiaReset->text(i).latin1()) ;
      ErrorMessage (msg, "Please report to the developper list (fec-support@ires.in2p3.fr)") ;

      // ERROR
      // cout << "Inconsistency index value: " << trackerListPiaReset->text(i) << endl ;
      // cout << "Please report to the developper list (fec-support@ires.in2p3.fr)" << endl ;
    }
  }  
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   CCU redundancy                                                   */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Save in file the list of the CCU
 */
void FecDialogImpl::ccuRedundanceSaveCCU() {

  QString fileName = "/tmp/ccu.txt" ;
  // Try to save file
  QString fn = QFileDialog::getSaveFileName( fileName, tr( "All Files (*)" ), this ) ;
  if (fn.isEmpty()) return ;
  fileName = fn ;

  std::cout << "Save file in " << fileName << std::endl ;

  std::ofstream fileCcu(fileName.ascii());
  if (!fileCcu.is_open()) {
    std::string msg = "Could not open the file " + fileName ;
    ErrorMessage ("CCU file problem", msg.c_str()) ;
  }
  else if (ccuRedundancyAddresses->count()) {
    
    uint ccuAddress = 0 ;
    for (uint i = 0 ; i < ccuRedundancyAddresses->count() ; i ++) {
      if (sscanf (ccuRedundancyAddresses->text(i), "%x", &ccuAddress)) {
	fileCcu << std::hex << "0x" << ccuAddress << " ";
      }
    }
    fileCcu << std::endl ;
    fileCcu.close() ;
  }
  else ErrorMessage ("CCU file problem", "No CCU declares, not saving") ;
}

/** Load file with the list of the CCU
 */
void FecDialogImpl::ccuRedundancyLoadCCU ( ) {

  QString fileName = "/tmp/ccu.txt" ;
  // Try to load file
  QString fn = QFileDialog::getOpenFileName( fileName, tr( "All Files (*)" ), this ) ;
  // File checked
  if (fn.isEmpty()) return ;
  fileName = fn ;

  std::cout << "Open file " << fileName << std::endl ;

  std::ifstream fileCcu(fileName.ascii());
  if (!fileCcu.is_open()) {
    std::string msg = "Could not open the file " + fileName ;
    ErrorMessage ("CCU file problem", msg.c_str()) ;
  }
  else {
    ccuRedundancyClearCCU();

    ccuAddresses->clear() ;
    ccuRedundancyAddresses->clear() ;
    i2cCcuAddresses->clear() ;
    memoryCcuAddresses->clear() ;
    piaCcuAddresses->clear() ;
    //triggerCcuAddresses->clear() ;
    //jtagCcuAddresses->clear() ;
    trackerPiaCcuAddresses->clear() ;

    int ccuAddressCreated[MAXCCU+1] = {0} ;
    unsigned int ccuAddress = 0 ;
    while (!fileCcu.eof()) {
      fileCcu >> std::hex >> ccuAddress ;
      if (!fileCcu.eof()) {
	if (!ccuAddressCreated[ccuAddress] && (ccuAddress != 0)) {
	  ccuAddressCreated[ccuAddress] = 1 ;
	  char msg[80] ;
	  sprintf (msg, "0x%02X", ccuAddress) ;
	  ccuAddresses->insertItem( msg );
	  ccuRedundancyAddresses->insertItem( msg );
	  i2cCcuAddresses->insertItem( msg );
	  memoryCcuAddresses->insertItem( msg );
	  piaCcuAddresses->insertItem( msg );
	  //triggerCcuAddresses->insertItem( msg );
	  //jtagCcuAddresses->insertItem( msg );
	  trackerPiaCcuAddresses->insertItem( msg );
	}
      }
    }

    fileCcu.close() ;
    ccuRedundancyCache() ;
  }
}

/** Try to hide and show a group box
 */
void FecDialogImpl::ccuRedundancyCache ( ) {

//   if (CCURedundancyCacheButton->text() == "Cache") {
//     CCURedundancyAddCCU->hide() ;
//     CCURedundancyCacheButton->setText("Montre") ;
//   }
//   else {
//     CCURedundancyAddCCU->show() ;
//     CCURedundancyCacheButton->setText("Cache") ;
//   }
//   // FEC Input
//   CCURedundancyFecOutputGroup->show() ;

//   for (int i = 1 ; i <= 36 ; i ++) {
//     createNewCcuGroupBox (buildCompleteKey(0,0,i,0,0)) ;
//   }

//   // FEC output
//   CCURedundancyFecInputGroup->show() ;

  uint slot, ringSlot; 
  if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
    
    char msg[80] ;
    //sprintf (msg, "FEC %d/0 Input", slot) ;
    //CCURedundancyFecInputGroup->setTitle(msg) ;
    //sprintf (msg, "FEC %d/0 Output", slot) ;
    //CCURedundancyFecOutputGroup->setTitle(msg) ;

    // FEC Input
    CCURedundancyFecOutputGroup->show() ;
    
    // for each CCU
    uint ccuAddress ;
    for ( uint ccuIn = 0 ; ccuIn < trackerPiaCcuAddresses->count() ; ccuIn ++ ) {
      
      if (sscanf (trackerPiaCcuAddresses->text(ccuIn), "%x", &ccuAddress)) {

	//std::cout << "Add the CCU 0x" << std::hex << ccuAddress << std::endl ;
	createNewCcuGroupBox (buildCompleteKey(slot,ringSlot,ccuAddress,0,0)) ;
      }
    }

    // FEC output
    CCURedundancyFecInputGroup->show() ;
  }
  else {
    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 
  }
}

/** Change the value of the control register 0 depending of the check concerning the check button
 */
void FecDialogImpl::fecRedundancy() {

  try {

    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      uint fecCR0 = fecAccess_->getFecRingCR0 (buildFecRingKey(slot,ringSlot)) ;
      fecCR0 &= 0xFFE7 ;
      if (CCURedundancyFecInputAButton->isChecked  ()) fecCR0 |= 0x0000 ;
      if (CCURedundancyFecInputBButton->isChecked  ()) fecCR0 |= FEC_CR0_SELSERIN ;
      if (CCURedundancyFecOutputAButton->isChecked ()) fecCR0 |= 0x0000 ;
      if (CCURedundancyFecOutputBButton->isChecked ()) fecCR0 |= FEC_CR0_SELSEROUT ;

      char msg[80] ;
      sprintf (msg, "0x%04X", fecCR0) ;
      fecCR0InputLine->setText(msg) ;

      fecWriteCR0 ( ) ;
    }
    else {

     ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }

    fecReadCR0 ( ) ;
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC CR0 is written", e) ; // cerr << e.what() << endl ;
  }
}

/** Add a CCU by hand
 */
void FecDialogImpl::ccuRedundancyAddCcuHand ( ) {

  uint slot, ringSlot; 
  if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

    uint ccuAddress ;
    if (sscanf (CCURedundancyCCUHandCCUAddress->text(), "%x", &ccuAddress)) {

      if (ccuAddress != 0) {
	createNewCcuGroupBox (buildCompleteKey(slot,ringSlot,ccuAddress,0,0)) ;

	// Add to all comb box from all tabs
	char msg[80] ;
	sprintf (msg, "0x%02X", ccuAddress) ;
	ccuAddresses->insertItem( msg );
	ccuRedundancyAddresses->insertItem( msg );
	i2cCcuAddresses->insertItem( msg );
	memoryCcuAddresses->insertItem( msg );
	piaCcuAddresses->insertItem( msg );
	//triggerCcuAddresses->insertItem( msg );
	//jtagCcuAddresses->insertItem( msg );
	trackerPiaCcuAddresses->insertItem( msg );
      }

      CCURedundancyCCUHandCCUAddress->setText ("0x") ;
    }
    else {
      ErrorMessage ("Error the ccu address enter is not correct or empty", "Please set the address before adding.") ;
    }
  }
  else {
    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 
  }
}

/** Create a new CCU group box
 */
void FecDialogImpl::createNewCcuGroupBox ( keyType index ) {

  if (ccuRedundancyInOut_.find(index) == ccuRedundancyInOut_.end()) {

    ccuOrder_[getFecKey(index)][indexCcuOrder_] = index ;
    char msg[80] ;
    sprintf (msg, "ccuRedundancyCCUGroup%x", getCcuKey(index)) ;

    QPoint posOrig = CCURedundancyFecOutputGroup->pos() ;
    posOrig.setX(posOrig.x() + CCURedundancyFecOutputGroup->width()) ;
    GroupCcuRedundancy *qtCcuRedundancy = new GroupCcuRedundancy ( fecAccess_, index, 
								   posOrig, 
								   ccuRedundancyInOut_.size(), 
								   ccuRedundancyErrorText, 
								   tab_7, msg) ;
    qtCcuRedundancy->show() ;

    // Move the FEC input (following the position of the last CCU)
    QPoint pos = qtCcuRedundancy->pos() ;
    pos.setX(CCURedundancyFecOutputGroup->x() +
	     CCURedundancyFecOutputGroup->width()+
	     MAXCCULINE * (qtCcuRedundancy->width()+PIXELCC) ) ;
    CCURedundancyFecInputGroup->move (pos) ;

    // Add to the table
    ccuRedundancyInOut_[index] = qtCcuRedundancy ;
  }
}

/** Clear what is needed after a PLX or FEC reset
 */
void FecDialogImpl::ccuRedundancyClearCCU() {
  
  CCURedundancyFecOutputGroup->hide() ;
  CCURedundancyFecInputGroup->hide() ;

  // for each qtCcuRedundancy => reset
  for (Sgi::hash_map<keyType, GroupCcuRedundancy *>::iterator p=ccuRedundancyInOut_.begin();p!=ccuRedundancyInOut_.end();p++) {

    if (p->second != NULL) {
      GroupCcuRedundancy *qtCcuRedundancy = p->second ;
      qtCcuRedundancy->hide() ;
      delete qtCcuRedundancy ;
    }
  }
  ccuRedundancyInOut_.clear() ;

  // clear all the CCU addresses
  ccuAddresses->clear() ;
  ccuRedundancyAddresses->clear() ;
  i2cCcuAddresses->clear() ;
  memoryCcuAddresses->clear() ;
  piaCcuAddresses->clear() ;
  //triggerCcuAddresses->clear() ;
  //jtagCcuAddresses->clear() ;
  trackerPiaCcuAddresses->clear() ;
}

 /** Delete all CCUs in the CCU redundancy
  */
void FecDialogImpl::redundancyClearAll() {

  CCURedundancyFecInputAButton->setChecked(true) ;
  CCURedundancyFecInputBButton->setChecked(false) ;
  CCURedundancyFecOutputAButton->setChecked(true) ;
  CCURedundancyFecOutputBButton->setChecked(false) ;

  // for each qtCcuRedundancy => reset
  for (Sgi::hash_map<keyType, GroupCcuRedundancy *>::iterator p=ccuRedundancyInOut_.begin();p!=ccuRedundancyInOut_.end();p++) {

    GroupCcuRedundancy *qtCcuRedundancy = p->second ;
    qtCcuRedundancy->resetMethod() ;
  }
}

/** Make an automatic check to test the reconfiguration of each CCU
 * This solution need to know what is the CCU addresses and the order of the CCU in the ring
 */
void FecDialogImpl::ccuRedundancyTestRingB ( ) {

  char msg[80] ;

  // Reset all the rings
  fecPlxReset() ;
  fecSoftReset() ;
  ccuRedundancyClearCCU () ;

  // Make a new scanring and find all the CCUs
  scanForCcus ( ) ;
  
  // Check if the CCU Group exists
  if (ccuRedundancyInOut_.size() == 0) // Display all the group CCU
    ccuRedundancyCache ( ) ;

  if (ccuRedundancyInOut_.size() == 0) {
    ErrorMessage ("Cannot test the redundancy", "No CCU found") ;
    return ;
  }

  // find the FEC slot
  tscType16 fecAddress, ringAddress ;
  if (!fecSlots->count() || !sscanf (fecSlots->currentText(), "%d,%d", &fecAddress,&ringAddress)) {

    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 
    return ;
  }

  // Wait for the message
  ErrorMessage ("Redundancy test", "Click on Ok to start the test") ;

  unsigned int timeout = 10 ;
  do {

    // Make a reset
    fecAccess_->fecHardReset (buildFecRingKey(fecAddress,ringAddress)) ;
    fecAccess_->fecRingReset (buildFecRingKey(fecAddress,ringAddress)) ;
    if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) {

#ifdef LOGMESSAGE
      cout << "PARASITIC: After reset => SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << endl ;
#endif
      emptyFIFOs ( fecAccess_, buildFecRingKey(fecAddress,ringAddress) ) ;
    }
    redundancyClearAll() ;

#ifdef LOGMESSAGE
    cout << "PLX and FEC reset Performed (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif

    timeout -- ;

    if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) usleep (100) ;
  }
  while (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) && timeout > 0) ;
    
  // Display the time
  struct timeval time1, time2 ;
  struct timezone zone1, zone2 ;
  gettimeofday(&time1, &zone1);
  
  // Find all the CCUs
  std::list<keyType> *ccuList = NULL ;
  try {

    // Scan the ring for each FECs and display the CCUs
    ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
      
    if (ccuList != NULL) {
	
      // Store all the addresses
      keyType ccuAddresses[127] ;
      unsigned int nbCcu = 0 ;

#ifdef LOGMESSAGE
      cout << "After Reset: CCU found " ;
#endif

      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	  
	keyType index = *p ;
	ccuAddresses[nbCcu++] = index ;


#ifdef LOGMESSAGE
	cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
      }

#ifdef LOGMESSAGE
	cout << endl ;
#endif

	// Delete = already store in the table of FEC device class
	delete ccuList ;

	// disable the IRQ
	fecAccess_->getFecRingDevice(buildFecRingKey(fecAddress,ringAddress))->setIRQ(false) ;

	// Find the dummy CCU
	bool error = false, dummyCcu = false ;
	fecAccess_->setCcuCRC (ccuAddresses[nbCcu-1], CCU_CRC_SSP) ;
	fecAccess_->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;

	ccuRedundancyInOut_[ccuAddresses[nbCcu-1]]->setCcuCRC (CCU_CRC_SSP) ;
	CCURedundancyFecInputBButton->setChecked(true) ;

	// Check if a dummy CCU exists
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1]))) {

	  emptyFIFOs ( fecAccess_, ccuAddresses[nbCcu-1] ) ;
	}
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1]))) { 

#ifdef LOGMESSAGE
	  cout << "ERROR: Dummy CCU, cannot find it, SR0 = 0x" << hex << fecAccess_->getFecRingSR0(ccuAddresses[0]) << endl ;
#endif
	  sprintf (msg, "Dummy CCU, cannot find it, SR0 = 0x%x", fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	  ccuRedundancyErrorText->insertItem (msg) ;
	}
	else {

	  // Scan the ring for each FECs and display the CCUs
	  ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
      
	  if (ccuList != NULL) {

#ifdef LOGMESSAGE
	    cout << "Dummy CCU: CCU found " ;
#endif

	    nbCcu = 0 ;
	    for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	  
	      keyType index = *p ;
	      ccuAddresses[nbCcu++] = index ;

#ifdef LOGMESSAGE
	      cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
	    }

#ifdef LOGMESSAGE
	    cout << endl ;
	    cout << "Dummy CCU address found 0x" << hex << getCcuKey(ccuAddresses[nbCcu-1]) 
		 << " (SR0 = 0x" << hex << fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1]) << ")"
		 << endl ;
#endif

	    sprintf (msg, "Dummy CCU address found 0x%x (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-1]), fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	    
	    dummyCcu = true ;

	    // Read during a certain amount of time the CCU and the DCU
	    //readDcuCcuMinutes (fecAccess_, fecAddress, ccuList) ;

	    // Delete the list of the CCUs
	    delete ccuList ;

	    // Display in the tab
	    createNewCcuGroupBox (ccuAddresses[nbCcu-1]) ;
	  }
	  else {

#ifdef LOGMESSAGE
	    cout << "ERROR: No CCU found after the check of the dummy CCU, (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif

	    sprintf (msg, "ERROR: No CCU found after the check of the dummy CCU, (SR0 = 0x%x)", fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	  }
	}
	
	// Reset the Ring A and B
	fecAccess_->fecHardReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingResetB ( buildFecRingKey(fecAddress,ringAddress) ) ;
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) {
							
#ifdef LOGMESSAGE
	  cout << "PARASITIC: After reset => SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << endl ;
#endif
	  emptyFIFOs ( fecAccess_, buildFecRingKey(fecAddress,ringAddress) ) ;
	}
	redundancyClearAll() ;	

	// disable the IRQ
	fecAccess_->getFecRingDevice(buildFecRingKey(fecAddress,ringAddress))->setIRQ(false) ;

#ifdef LOGMESSAGE
	cout << "PLX and FEC reset Performed (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif

	// Bypass the 1st CCU
	fecAccess_->setFecRingCR0 ( ccuAddresses[0], FEC_CR0_SELSEROUT ) ;
	fecAccess_->setCcuCRC ( ccuAddresses[1], CCU_CRC_ALTIN) ;

	CCURedundancyFecOutputBButton->setChecked(true) ;
	ccuRedundancyInOut_[ccuAddresses[1]]->setCcuCRC (CCU_CRC_ALTIN) ;

	fecAccess_->fecRingRelease ( buildFecRingKey(fecAddress,ringAddress) ) ;
  
	// If the status is not correct then
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[0])))
	  emptyFIFOs ( fecAccess_, ccuAddresses[0] ) ;

	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[0]))) { 

#ifdef LOGMESSAGE
	  cout << "ERROR when I bypass the 1st CCU 0x" << hex << getCcuKey(ccuAddresses[0]) << ", SR0 = 0x" <<  fecAccess_->getFecRingSR0(ccuAddresses[0]) << endl ; 
#endif

	  sprintf (msg, "ERROR when I bypass the 1st CCU 0x%x (SR0 = 0x%x)", getCcuKey(ccuAddresses[0]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	  ccuRedundancyErrorText->insertItem (msg) ;

	  error = true ;
	}
	else {

#ifdef LOGMESSAGE
	  cout << "Bypass the 1st CCU 0x" << hex << getCcuKey(ccuAddresses[0]) << " Ok (SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[0]) << ")" << endl ;
#endif

	  sprintf (msg, "Bypass the 1st CCU 0x%x Ok (SR0 = 0x%x)", getCcuKey(ccuAddresses[0]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	  ccuRedundancyErrorText->insertItem (msg) ;
	  
	  // Scan the ring for each FECs and display the CCUs
	  ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  
	  if (ccuList != NULL) {

#ifdef LOGMESSAGE
	    cout << "Bypass of the 1st CCU: CCU found " ;
#endif

	    for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
	      keyType index = *p ;
	      
#ifdef LOGMESSAGE
	      // Log Message
	      cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
	    }
	
#ifdef LOGMESSAGE    
	    cout << endl ;
#endif

	    // Read during a certain amount of time the CCU and the DCU
	    //readDcuCcuMinutes (fecAccess_, fecAddress, ccuList) ;

	    // Delete = already store in the table of FEC device class
	    delete ccuList ;
	  }
	  else {

#ifdef LOGMESSAGE
	    cout << "ERROR: No CCU found  after the bypassed of the 1st CCU 0x"  << hex << getCcuKey(ccuAddresses[0]) << "(SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif

	    sprintf (msg, "ERROR: No CCU found  after the bypassed of the 1st CCU 0x%x, (SR0 = 0x%x)", getCcuKey(ccuAddresses[0]), fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	  }
	}

	// Reset the Ring A and B
	fecAccess_->fecHardReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingResetB ( buildFecRingKey(fecAddress,ringAddress) ) ;
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) {

#ifdef LOGMESSAGE
	  cout << "PARASITIC: After reset => SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << endl ;
#endif
	  emptyFIFOs ( fecAccess_, buildFecRingKey(fecAddress,ringAddress) ) ;	
	}
	redundancyClearAll() ;

#ifdef LOGMESSAGE
	cout << "PLX and FEC reset Performed (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif

	// disable the IRQ
	fecAccess_->getFecRingDevice(buildFecRingKey(fecAddress,ringAddress))->setIRQ(false) ;
	
	// Bypass the other CCU except the last one
	uint numberCCU = nbCcu - 1 ;
	if (dummyCcu) numberCCU -= 1 ;

	for (uint ccuI = 1 ; ccuI < numberCCU ; ccuI ++) {

	  fecAccess_->setCcuCRC (ccuAddresses[ccuI-1], CCU_CRC_SSP) ;
	  ccuRedundancyInOut_[ccuAddresses[ccuI-1]]->setCcuCRC (CCU_CRC_SSP) ;
	  fecAccess_->setCcuCRC (ccuAddresses[ccuI+1], CCU_CRC_ALTIN) ;
	  ccuRedundancyInOut_[ccuAddresses[ccuI+1]]->setCcuCRC (CCU_CRC_ALTIN) ;
	  fecAccess_->fecRingRelease ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  
	  // If the status is not correct then
	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[ccuI])))
	    emptyFIFOs ( fecAccess_, ccuAddresses[ccuI] ) ;

	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[ccuI]))) { 

#ifdef LOGMESSAGE
	    cout << "ERROR when I bypass the " << dec << ccuI+1 << "th CCU 0x" << hex << getCcuKey(ccuAddresses[ccuI])
		 << ", SR0 = 0x" << hex << fecAccess_->getFecRingSR0(ccuAddresses[ccuI]) << endl ;
#endif

	    sprintf (msg, "ERROR when I bypass the %dth CCU 0x%x (SR0 = 0x%x)", ccuI+1, getCcuKey(ccuAddresses[ccuI]), fecAccess_->getFecRingSR0(ccuAddresses[ccuI])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	    error = true ;
	  }
	  else {
	
#ifdef LOGMESSAGE    
	    cout << "Bypass of the " << dec << ccuI+1 << "th CCU 0x" << hex << getCcuKey(ccuAddresses[ccuI]) << " Ok (SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[0]) << ")" << endl ;
#endif
	    sprintf (msg, "Bypass of the %dth CCU 0x%x Ok (SR0 = 0x%x)", ccuI+1, getCcuKey(ccuAddresses[ccuI]), fecAccess_->getFecRingSR0(ccuAddresses[ccuI])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;

	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
      
	    if (ccuList != NULL) {

#ifdef LOGMESSAGE
	      cout << "Bypass of the " << dec << ccuI+1 << "th CCU 0x" << hex << getCcuKey(ccuAddresses[ccuI]) << ": CCU found " ;
#endif

	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	      
		keyType index = *p ;

#ifdef LOGMESSAGE
		cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
	      }

#ifdef LOGMESSAGE
	      cout << endl ;
#endif

	      // Read during a certain amount of time the CCU and the DCU
	      //readDcuCcuMinutes (fecAccess_, fecAddress, ccuList) ;

	      // Delete = already store in the table of FEC device class
	      delete ccuList ;
	    }
	    else {
	      
#ifdef LOGMESSAGE
	      cout << "ERROR: No CCU found  after the bypassed of the "  << dec << ccuI+1 << "th CCU 0x" << hex << getCcuKey(ccuAddresses[ccuI]) << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
	      
	      sprintf (msg, "ERROR: No CCU found  after the bypassed of the %dth CCU 0x%x, (SR0 = 0x%x)", ccuI+1, getCcuKey(ccuAddresses[ccuI]), fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) ;
	      ccuRedundancyErrorText->insertItem (msg) ;
	    }
	  }

	  // Reset the Ring A and B
	  fecAccess_->fecHardReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  fecAccess_->fecRingReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  fecAccess_->fecRingResetB ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) {

#ifdef LOGMESSAGE
	    cout << "PARASITIC: After reset => SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << endl ;
#endif
	    emptyFIFOs ( fecAccess_, buildFecRingKey(fecAddress,ringAddress) ) ;
	  }
	  redundancyClearAll() ;

	  // disable the IRQ
	  fecAccess_->getFecRingDevice(buildFecRingKey(fecAddress,ringAddress))->setIRQ(false) ;

#ifdef LOGMESSAGE
	  cout << "PLX and FEC reset Performed (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
	}

	// Bypass the last CCU
	if (dummyCcu) {

	  fecAccess_->setCcuCRC (ccuAddresses[nbCcu-3], CCU_CRC_SSP) ;
	  ccuRedundancyInOut_[ccuAddresses[nbCcu-3]]->setCcuCRC (CCU_CRC_SSP) ;
	  fecAccess_->setCcuCRC (ccuAddresses[nbCcu-1], CCU_CRC_ALTIN) ;
	  ccuRedundancyInOut_[ccuAddresses[nbCcu-1]]->setCcuCRC (CCU_CRC_ALTIN) ;
	  fecAccess_->fecRingRelease ( buildFecRingKey(fecAddress,ringAddress) ) ;
	  fecAccess_->setFecRingCR0 ( buildFecRingKey(fecAddress,ringAddress), FEC_CR0_SELSERIN ) ;
	  CCURedundancyFecInputBButton->setChecked(true) ;

	  // If the status is not correct then
	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-2])))
	    emptyFIFOs ( fecAccess_, ccuAddresses[nbCcu-2] ) ;

	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-2]))) { 

#ifdef LOGMESSAGE
	    cout << "ERROR when I bypass the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-2])
		 << " with the dummy CCU, SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-2]) << endl ;
#endif

	    sprintf (msg, "ERROR when I bypass the last CCU 0x%x Ok with the dummy CCU (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-2]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	    error = true ;
	  }
	  else {

#ifdef LOGMESSAGE
	    cout << "Bypass of the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-2]) << " Ok with the dummy CCU, (SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[0]) << ")" << endl ;
#endif
	    sprintf (msg, "Bypass of the last CCU 0x%x Ok with the dummy CCU (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-2]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;

	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
	    
	    if (ccuList != NULL) {
	      
#ifdef LOGMESSAGE
	      cout << "Bypass of the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-2]) << " CCU found " ;
#endif

	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
		
		keyType index = *p ;
		
#ifdef LOGMESSAGE
		cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
	      }

	      // Log Message
	      cout << endl ;

	      // Read during a certain amount of time the CCU and the DCU
	      //readDcuCcuMinutes (fecAccess_, fecAddress, ccuList) ;

	      // Delete = already store in the table of FEC device class
	      delete ccuList ;
	    }
	    else {
	      
#ifdef LOGMESSAGE
	      cout << "ERROR: No CCU found after the bypassed of the last CCU 0x"  << hex << getCcuKey(ccuAddresses[nbCcu-2]) << " with dummy CCU" << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
	      
	      sprintf (msg, "ERROR: No CCU found after the bypassed of the last CCU 0x%x, (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-2]), fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) ;
	      ccuRedundancyErrorText->insertItem (msg) ;
	    }
	  }
	}
	else {

	  // Bypass the last CCU without the dummy CCU
	  fecAccess_->setCcuCRC (ccuAddresses[nbCcu-2], CCU_CRC_SSP) ;
	  ccuRedundancyInOut_[ccuAddresses[nbCcu-2]]->setCcuCRC (CCU_CRC_SSP) ;
	  fecAccess_->setFecRingCR0 ( buildFecRingKey(fecAddress,ringAddress), FEC_CR0_SELSERIN ) ;
	  CCURedundancyFecInputBButton->setChecked(true) ;

	  // If the status is not correct then
	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1])))
	    emptyFIFOs ( fecAccess_, ccuAddresses[nbCcu-1] ) ;

	  if (! isFecSR0Correct(fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-1]))) { 

#ifdef LOGMESSAGE
	    cout << "ERROR when I bypass the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-1]) 
		 << " without the dummy CCU, SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[nbCcu-2]) << endl ;
#endif
	    sprintf (msg, "ERROR when I bypass the last CCU 0x%x Ok without the dummy CCU (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-2]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;
	    error = true ;
	  }
	  else {

#ifdef LOGMESSAGE
	    cout << "Bypass of the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-1]) << " Ok without the dummy CCU, (SR0 = 0x" << fecAccess_->getFecRingSR0(ccuAddresses[0]) << ")" << endl ;
#endif
	    sprintf (msg, "Bypass of the last CCU 0x%x Ok without the dummy CCU (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-2]), fecAccess_->getFecRingSR0(ccuAddresses[0])) ;
	    ccuRedundancyErrorText->insertItem (msg) ;

	    // Scan the ring for each FECs and display the CCUs
	    ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecAddress,ringAddress) ) ;
	    
	    if (ccuList != NULL) {
	      
#ifdef LOGMESSAGE
	      cout << "Bypass of the last CCU 0x" << hex << getCcuKey(ccuAddresses[nbCcu-2]) << " CCU found " ;
#endif

	      for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
		
		keyType index = *p ;
		
#ifdef LOGMESSAGE
		cout << hex << "0x" << getCcuKey(index) << " " ;
#endif
	      }

	      // Log Message
	      cout << endl ;

	      // Read during a certain amount of time the CCU and the DCU
	      //readDcuCcuMinutes (fecAccess_, fecAddress, ccuList) ;

	      // Delete = already store in the table of FEC device class
	      delete ccuList ;
	    }
	    else {
	      
#ifdef LOGMESSAGE
	      cout << "ERROR: No CCU found after the bypassed of the last CCU 0x"  << hex << getCcuKey(ccuAddresses[nbCcu-1]) << " without dummy CCU" << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
	      
	      sprintf (msg, "ERROR: No CCU found after the bypassed of the last CCU 0x%x, (SR0 = 0x%x)", getCcuKey(ccuAddresses[nbCcu-1]), fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress))) ;
	      ccuRedundancyErrorText->insertItem (msg) ;
	    }
	  }
	}

	// Display the final message
	if (error) {

	  ErrorMessage ("Redundancy", "Error when the CCUs are bypassed, see the Error Messages just below") ;
	  ccuRedundancyErrorText->insertItem ("Cannot use the redundancy") ;

#ifdef LOGMESSAGE
	  cout << "Redundancy: Error when the CCUs are bypassed, Cannot use the redundancy" << endl ;
#endif
	}
	else {
	  
	  ErrorMessage ("Redundancy Working", "Ring B is ok, see the Status just below for more information") ;
	  ccuRedundancyErrorText->insertItem ("Redundancy OK") ;

#ifdef LOGMESSAGE
	  cout << "Redundancy: Ring B is ok" << endl ;
#endif
	}

	// Reset the Ring A and B
	fecAccess_->fecHardReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingReset ( buildFecRingKey(fecAddress,ringAddress) ) ;
	fecAccess_->fecRingResetB ( buildFecRingKey(fecAddress,ringAddress) ) ;
	if (! isFecSR0Correct(fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)))) {

#ifdef LOGMESSAGE
	  cout << "PARASITIC: After reset => SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << endl ;
#endif
	  emptyFIFOs ( fecAccess_, buildFecRingKey(fecAddress,ringAddress) ) ;
	}
	redundancyClearAll() ;

#ifdef LOGMESSAGE
	cout << "PLX and FEC reset Performed (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
	
	// enable the IRQ
	fecAccess_->getFecRingDevice(buildFecRingKey(fecAddress,ringAddress))->setIRQ(true) ;
      }
      else {

#ifdef LOGMESSAGE
	cout << "ERROR: No CCU found on the ring, (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
      }
    }
    catch (FecExceptionHandler &e) { 
      
#ifdef LOGMESSAGE
      cerr << "*********** ERROR ********************************" << endl ; 
      cerr << "An error occurs during hardware access" << endl ;
      cerr << e.what()  << endl ;
      cerr << "**************************************************" << endl ;
#endif

      ErrorMessage ("Redundancy problem check", e) ;
    }

    // Display the time
    gettimeofday(&time2, &zone2);
    double timesec1 = time1.tv_sec + time1.tv_usec * 0.000001 ;
    double timesec2 = time2.tv_sec + time2.tv_usec * 0.000001 ;
    double timesec = timesec2 - timesec1 ;
#ifdef LOGMESSAGE
    cout << "--------------------------------> test stop and takes " << dec << timesec << " s (SR0 = 0x" 
	 << hex << fecAccess_->getFecRingSR0(buildFecRingKey(fecAddress,ringAddress)) << ")" << endl ;
#endif
}

/** This method try to recover the ring until the status register 0 is ok
 */
void FecDialogImpl::emptyFIFOs ( FecAccess *fecAccess_, keyType index, bool display ) {

  //cout << "Start emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;

  try {
    
    // Start the scanning of the ring for each FEC device
    FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice ( index ) ;

    // Is the status is correct
    if (! isFecSR0Correct(fecAccess_->getFecRingSR0(index))) {

      uint fecSR0 = fecAccess_->getFecRingSR0(index) ;

      //cout << "      emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;
      fecSR0 =  fecAccess_->getFecRingSR0(index) ;

      // FIFO receive
      if (! (fecSR0 & FEC_SR0_RECEMPTY)) {
        
        // cout << "FIFO receive not empty" << endl ;
        tscType32 value = 0 ;
        tscType16 FecSR0 ; 
	
        int loopCnt = 0 ; 
        
        while (
               (!((FecSR0=fecAccess_->getFecRingSR0(index)) & FEC_SR0_RECEMPTY) || (FecSR0 & FEC_SR0_RECRUN)) 
               && (loopCnt < fecRingDevice->getReceiveFifoDepth())
               && (value != 0xFFFF) ) {
	  

	  if (display) 
	    cout << "FIFO receive word(" << dec << loopCnt << ") = " 
		 <<  fecRingDevice->getFifoReceive()
		 << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(index)
		 << ")" << endl ;
	  else
	    fecRingDevice->getFifoReceive() ;
          
          loopCnt++; 
          
        } // end of while loop 
      }
      
      // Clear the errors and display the status
      // fecAccess_->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      //cout << "      emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;
      fecSR0 =  fecAccess_->getFecRingSR0(index) ;

      // FIFO return
      if (! (fecSR0 & FEC_SR0_RETEMPTY)) {

	// cout << "FIFO return not empty" << endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 
	fecRingDevice->emptyFifoReturn() ;

// 	int loopCnt = 0 ; 
// 	while (
// 	       (!((FecSR0=fecAccess_->getFecRingSR0(index)) & FEC_SR0_RETEMPTY)) 
// 	       && (loopCnt < fecRingDevice->getReturnFifoDepth())
// 	       && (value != 0xFF) ) {

// 	  if (display) 
// 	    cout << "FIFO return word(" << dec << loopCnt << ") = " 
// 		 <<  (int)fecRingDevice->getFifoReturn()
// 		 << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(index)
// 		 << ")" << endl ;
// 	  else
// 	    fecRingDevice->getFifoReturn() ;
          
// 	  loopCnt++; 
// 	} // end of while loop 
      }

      // Clear the errors and display the status
      // fecAccess_->setFecRingCR1 (index, FEC_CR1_CLEARIRQ | FEC_CR1_CLEARERRORS) ;

      //cout << "      emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;
      fecSR0 =  fecAccess_->getFecRingSR0(index) ;

      // FIFO transmit
      if (! (fecSR0 & FEC_SR0_TRAEMPTY)) {

	// cout << "FIFO transmit not empty" << endl ;
	tscType32 value = 0 ;
	tscType16 FecSR0 ; 

	int loopCnt = 0 ; 

	while (
	       (!((FecSR0=fecAccess_->getFecRingSR0(index)) & FEC_SR0_TRAEMPTY) || (FecSR0 & FEC_SR0_TRARUN)) 
	       && (loopCnt < fecRingDevice->getTransmitFifoDepth())
	       && (value != 0xFFFF) ) {

	  if (display) 
	    cout << "FIFO transmit word(" << dec << loopCnt << ") = " 
		 <<  fecRingDevice->getFifoTransmit()
		 << " (SR0 = 0x" << hex <<  fecAccess_->getFecRingSR0(index)
		 << ")" << endl ;
	  else
	    fecRingDevice->getFifoTransmit() ;
     
	  loopCnt++; 
	} // end of while loop 
      }

      //cout << "      emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;
      fecSR0 =  fecAccess_->getFecRingSR0(index) ;

      // Pending IRQ
      if (fecSR0 & FEC_SR0_PENDINGIRQ) {

	//cout << "setFecRingCR1 (" << hex << (FEC_CR1_CLEARIRQ) << ")" << endl ;
	fecAccess_->setFecRingCR1 (index, FEC_CR1_CLEARIRQ) ;
      }
    }
  }
  catch (FecExceptionHandler &e) {
    
    cerr << "*********** ERROR ********************************" << endl ; 
    cerr << "An error occurs during hardware access" << endl ;
    cerr << e.what()  << endl ;
    cerr << "**************************************************" << endl ;
  }

  if (display)
    cout << "Stop emptyFIFOs: SR0 = 0x" << hex << fecAccess_->getFecRingSR0(index) << endl ;
}

/* ----------------------------------------------------------------------------------- */
/*                                                                                     */
/*                                                                                     */
/*                   Thread methods                                                    */
/*                                                                                     */
/*                                                                                     */
/* ----------------------------------------------------------------------------------- */

/** Start the thread and change the button name
 */
void FecDialogImpl::startThread ( ) {

  uint slot, ringSlot ;
  if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

    if (StartThreadButton->text() == "Stop Thread") {
      StartThreadButton->setText ("Start Thread") ;
    }
    else {
      StartThreadButton->setText ("Stop Thread") ;
      this->start() ;
    }
  }
  else {

    ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

    // ERROR
    // cout << "Error the slot enter is not correct or empty" << endl ;
    // cout << "Please scan FECs before use read/write" << endl ;
  }
}

/** Thread method, read the SR0 and display it in a field, display also through a LED the link
 * initialise
 */
void FecDialogImpl::run() {

  StartThreadButton->setText ("Stop Thread") ;
  try {
    while (StartThreadButton->text() == "Stop Thread") {

      uint slot, ringSlot; 
      if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {
	
	uint fecSR0 = 0, timeout = 0 ;
	
	while (fecSR0 == 0 && timeout < 10) {
	  fecSR0 = fecAccess_->getFecRingSR0 (buildFecRingKey(slot,ringSlot)) ;
	  timeout ++ ;
	}
	if (fecSR0 & FEC_SR0_TRARUN) fecSR0TraRunning->setChecked (true) ;
	else fecSR0TraRunning->setChecked (false) ;
	if (fecSR0 & FEC_SR0_RECRUN) fecSR0RecRunning->setChecked (true) ;
	else fecSR0RecRunning->setChecked (false) ;
	if (fecSR0 & FEC_SR0_RECFULL) fecSR0RecFull->setChecked (true) ;
	else fecSR0RecFull->setChecked (false) ;
	if (fecSR0 & FEC_SR0_RECEMPTY) fecSR0RecEmpty->setChecked (true) ;
	else fecSR0RecEmpty->setChecked (false) ;
	if (fecSR0 & FEC_SR0_RETFULL) fecSR0RetFull->setChecked (true) ;
	else fecSR0RetFull->setChecked (false) ;
	if (fecSR0 & FEC_SR0_RETEMPTY) fecSR0RetEmpty->setChecked (true) ;
	else fecSR0RetEmpty->setChecked (false) ;
	if (fecSR0 & FEC_SR0_TRAFULL) fecSR0TraFull->setChecked (true) ;
	else fecSR0TraFull->setChecked (false) ;
	if (fecSR0 & FEC_SR0_TRAEMPTY) fecSR0TraEmpty->setChecked (true) ;
	else fecSR0TraEmpty->setChecked (false) ;
	
	if (fecSR0 & FEC_SR0_LINKINITIALIZED) {
	    fecSR0LinkInit->setChecked (true) ;
	    //LinkInitThreadLinkLed->setColor(Qt::green) ;
	    LinkInitThreadLinkLed2->setPixmap(greenLed_) ;
	}
	else {
	  fecSR0LinkInit->setChecked (false) ;
	  //LinkInitThreadLinkLed->setColor(Qt::red) ;
	  LinkInitThreadLinkLed2->setPixmap(redLed_) ;
	}
	if (fecSR0 & FEC_SR0_PENDINGIRQ) fecSR0PendingIrq->setChecked (true) ;
	else fecSR0PendingIrq->setChecked (false) ;
	if (fecSR0 & FEC_SR0_DATATOFEC) fecSR0DataToFec->setChecked (true) ;
	else fecSR0DataToFec->setChecked (false) ;
	if (fecSR0 & FEC_SR0_TTCRXREADY) fecSR0TTCRx->setChecked (true) ;
	else fecSR0TTCRx->setChecked (false) ;
	
	char msg[80] ;
	sprintf (msg, "0x%04X", fecSR0) ;
	fecSR0InputLine->setText (msg) ;
	fecSR0Thread->setText (msg) ;
	fecSR0Bis->setText (msg) ;
	
	sleep (2) ;
      }
      else {
	
	ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 
	
	// ERROR
	// cout << "Error the slot enter is not correct or empty" << endl ;
	// cout << "Please scan FECs before use read/write" << endl ;
      }
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FEC SR0 is retrieved, thread stopped", e) ; // cerr << e.what() << endl ;
  }

  StartThreadButton->setText ("Start Thread") ;
}

/* --------------------------------------------------------------------------------------------------- */
/* FIFO                                                                                                */
/* --------------------------------------------------------------------------------------------------- */ 
/** Read a word in the FEC FIFO receive
 */
void FecDialogImpl::fecReadFifoRec() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      char msg[80] ;
      sprintf (msg, "0x%X", fecRingDevice->getFifoReceive ( )) ;
      fecFifoRecWord->setText(msg) ;
  
      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO receive is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/** Read a word in the FEC FIFO return
 */
void FecDialogImpl::fecReadFifoRet() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      char msg[80] ;
      sprintf (msg, "0x%X", fecRingDevice->getFifoReturn ( )) ;
      fecFifoRetWord->setText(msg) ;
  
      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO receive is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/** Read a word in the FEC FIFO transmit
 */
void FecDialogImpl::fecReadFifoTra() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      char msg[80] ;
      sprintf (msg, "0x%X", fecRingDevice->getFifoTransmit ( )) ;
      fecFifoTraWord->setText(msg) ;
  
      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO receive is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/** Write a word in the FEC FIFO receive
 */
void FecDialogImpl::fecWriteFifoRec() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      ulong value ;
      if (sscanf (fecFifoRecWord->text(), "%x", &value)) {

	FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
	fecRingDevice->setFifoReceive (value) ;
      }
      else 
	ErrorMessage ("Bad value in the field to be written") ;	
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when I try to write in the FIFO receive", e) ; // cerr << e.what() << endl ;
  }
}

/** Write a word in the FEC FIFO return
 */
void FecDialogImpl::fecWriteFifoRet() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      ulong value ;
      if (sscanf (fecFifoRecWord->text(), "%x", &value)) {

	FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
	fecRingDevice->setFifoReturn (value) ;
      }
      else 
	ErrorMessage ("Bad value in the field to be written") ;	
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when I try to write in the FIFO receive", e) ; // cerr << e.what() << endl ;
  }
}

/** Write a word in the FEC FIFO transmit
 */
void FecDialogImpl::fecWriteFifoTra() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      ulong value ;
      if (sscanf (fecFifoRecWord->text(), "%x", &value)) {

	FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
	fecRingDevice->setFifoTransmit (value) ;
      }
      else 
	ErrorMessage ("Bad value in the field to be written") ;	
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when I try to write in the FIFO receive", e) ; // cerr << e.what() << endl ;
  }
}

/** Empty the FEC FIFO receive
 */
void FecDialogImpl::fecEmptyFifoRec() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      fecRingDevice->emptyFifoReceive() ;

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO receive is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/** Empty the FEC FIFO return
 */
void FecDialogImpl::fecEmptyFifoRet() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      fecRingDevice->emptyFifoReturn (  ) ;

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO return is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/** Empty the FEC FIFO transmit
 */
void FecDialogImpl::fecEmptyFifoTra() {

  try {
    uint slot, ringSlot; 
    if (fecSlots->count() && sscanf (fecSlots->currentText(), "%d,%d", &slot, &ringSlot)) {

      FecRingDevice *fecRingDevice = fecAccess_->getFecRingDevice (buildFecRingKey(slot,ringSlot)) ;
      fecRingDevice->emptyFifoTransmit (  ) ;

      fecReadAllRegisters( ) ;
    }
    else {

      ErrorMessage ("The slot enter is not correct or empty", "Please scan FECs and CCUs before use read/write." ) ; 

      // ERROR
      // cout << "Error the slot enter is not correct or empty" << endl ;
      // cout << "Please scan FECs before use read/write" << endl ;
    }
  }
  catch (FecExceptionHandler &e) { 

    // ERROR 
    ErrorMessage ("Error when the FIFO transmit is emptyied", e) ; // cerr << e.what() << endl ;
  }
}

/* --------------------------------------------------------------------------------------------------- */
/* 
/* --------------------------------------------------------------------------------------------------- */ 

/** To catch the close event method in order to do the same as quit button
 */
void FecDialogImpl::closeEvent (QCloseEvent *e) {

  QMessageBox mb( "Quit Application",
		  "Do you want to quit the application ?",
		  QMessageBox::NoIcon,
		  QMessageBox::Yes,
		  QMessageBox::No | QMessageBox::Escape | QMessageBox::Default,
		  QMessageBox::NoButton );

  if ( mb.exec() == QMessageBox::Yes ) {

    try {
      delete fecAccessManager_ ;
    }
    catch (FecExceptionHandler &e) { 
      ErrorMessage ("Unable to delete the access manager") ;
      //cerr << e.what() << endl ; 
    }
    try {
      delete fecAccess_ ;
    }
    catch (FecExceptionHandler &e) { 
      ErrorMessage ("Unable to delete the hardware accesses", e) ; 
      //cerr << e.what() << endl ; 
    }

#ifdef DATABASEFACTORY
    delete fecFactory_ ;
#endif

    e->accept() ;
    close() ;
  }
}
 
