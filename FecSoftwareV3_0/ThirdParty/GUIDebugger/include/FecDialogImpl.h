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
#ifndef FECDIALOGIMPL_H
#define FECDIALOGIMPL_H
  
#include <qthread.h>
#include <qpixmap.h>  

#include "FecDialog.h"
  
#include "keyType.h"
#include "FecAccess.h"
#include "FecAccessManager.h"
#include "PiaResetAccess.h"
#include "piaResetDescription.h"

#include "XMLFecDevice.h"
#include "XMLFecPiaReset.h"
  
#ifdef DATABASEFACTORY
#include "FecFactory.h"
#endif

#include "GroupCcuRedundancy.h"

#ifndef MEMORYCHANNELNUMBER
#define MEMORYCHANNELNUMBER 0x40
#endif

#define FECSOFTWAREV2_0

class FecDialogImpl: public FecDialog, public QThread {

 private:
  /** Green led
   */
  QPixmap greenLed_ ;

  /** Orange led
   */
  QPixmap orangeLed_ ;

  /** Red led
   */
  QPixmap redLed_ ;

  /** Black led
   */
  QPixmap blackLed_ ;

  /** Access to the hardware
   */
  FecAccess *fecAccess_ ;

  /** Access device manager
   */
  FecAccessManager *fecAccessManager_ ;

#ifdef DATABASEFACTORY
  /** access to XML file or database
   */
  FecFactory *fecFactory_ ;
#endif

  /** all the redundancy button InputA, InputB, OutputA, OutputB
   */
  Sgi::hash_map<keyType, GroupCcuRedundancy *> ccuRedundancyInOut_ ;

  /** IP address
   */
  std::string supervisorIp_ ;

  /** For database access, partition name
   */
  std::string partitionName_ ;

  /** For database access, FEC hardware ID
   */
  std::string fecHardwareId_ ;

  /** Order of the CCUs, only used for the redundancy
   */
#define CONSTMAXFEC 160 
  keyType ccuOrder_[CONSTMAXFEC][MAXCCU+1] ;
  unsigned int indexCcuOrder_ ;

  /** Just do display the order of the CCU in a label
   */
  bool displayCcuOrder_ ;

 public:

  /** \brief Create the default accesses to the panels
   */
  FecDialogImpl ( FecAccess *fecAccess, std::string partitionName, std::string fecHardwareId ) ;

  /**
   */
  ~FecDialogImpl ( ) ;

  /**
   */
  void helpAbout(); 

  /** Quit or not to quit that is the question
   */
  void quitTout();

  /** Display the errors coming from FecAccessManager download and upload operation
   */
  void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList ) ;

  /**
   */
  void ErrorMessage ( const char *msg ) ;
  /**
   */
  void ErrorMessage ( const char *msg, const char *msg1 ) ;
  /** Display a dialog box when an exception occurs
   */
  void ErrorMessage ( char *title, FecExceptionHandler e ) ;

  /** Disable or enable the IRQ in the driver
   */
  void ddIRQEnableDisable() ;

  /**
   */
  void scanForFecs() ;

  /**
   */
  void fecPlxReset();

  /**
   */
  void fecSoftReset();

  /**
   */
  void fecDisableReceiveFec() ;

  /**
   */
  void fecReleaseFec() ;

  /**
   */
  void fecClearErrorsCR1() ;

  /**
   */
  void fecReadCR0() ;

  /** Call the next method
   */
  void fecWriteCR0_ () ;

  /**
   */
  void fecWriteCR0();

  /**
   */
  void fecWriteCR0Bit();

  /**
   */
  void fecReadCR1();

  /**
   */
  void fecWriteCR1();

  /**
   */
  void fecWriteCR1Bit();

  /**
   */
  void fecReadSR0();

  /**
   */
  void fecReadSR1();

  /**
   */
  void fecReadDDStatus();

  /**
   */
  void fecEnableCounters() ;

  /**
   */
  void fecResetErrorCounter() ;

  /**
   */
  void fecReadAllRegisters();

  /**
   */
  void fecClearAll() ;

  /**
   */
  void scanForCcus() ;

  /**
   */
  void ccuSelected() ;

  /**
   */
  void ccuWriteCRA();

  /**
   */
  void ccuWriteCRABit();

  /**
   */
  void ccuReadCRA() ;

  /**
   */
  void ccuWriteCRB();

  /**
   */
  void ccuWriteCRBBit();

  /**
   */
  void ccuReadCRB() ;

  /**
   */
  void ccuWriteCRC();

  /**
   */
  void ccuWriteCRCBit();

  /**
   */
  void ccuReadCRC() ;

  /**
   */
  void ccuWriteCRD();

  /**
   */
  void ccuReadCRD() ;

  /**
   */
  void ccuWriteCRE();

  /**
   */
  void ccuWriteCREBit();

  /**
   */
  void ccuReadCRE() ;

  /**
   */
  void ccuReadSRA() ;

  /**
   */
  void ccuReadSR() ;

  /**
   */
  void ccuReadSRE() ;

  /**
   */
  void ccuReadAll() ;

  /**
   */
  void ccuClearAll() ;

  /** Change the FEC slots for all combo boxes
   */
  void fecSlotsChange ( int index ) ;

  /** For modification of the FEC slots
   * fecSlots
   */
  void fecSlotsModify ( ) ;

  /** For modification of the FEC slots
   * ccuFecSlots
   */
  void fecSlotsCcuModify ( ) ;

  /** For modification of the FEC slots
   * i2cFecSlots
   */
  void fecSlotsI2cModify ( ) ;

  /** For modification of the FEC slots
   * memoryFecSlots
   */
  void fecSlotsMemoryModify ( ) ;

  /** For modification of the FEC slots
   * piaFecSlots
   */
  void fecSlotsPiaModify ( ) ;

  /** For modification of the FEC slots
   * fecSlotsCcuRedundancy
   */
  void fecSlotsRedundancyModify ( ) ;

  /**
   */
  void scanFecsCcusDevices() ;

  /**
   */
  void ccuEnableAllChannels() ;

  /**
   */
  keyType getI2CKey( bool ) ;

  /**
   */
  void i2cSetModeNormal() ;

  /**
   */
  void i2cSetModeExtended() ;

  /**
   */
  void i2cSetModeRal() ;

  /**
   */
  void i2cSetMaskAnd();

  /**
   */
  void i2cSetMaskOr();

  /**
   */
  void i2cSetMaskXor();

  /**
   */
  void i2cIsChannelEnable () ;

  /**
   */
  void i2cEnableChannel() ;

  /**
   */
  void i2cWriteCRA();

  /**
   */
  void i2cReadCRA();

  /**
   */
  void i2cSetSpeed100();

  /**
   */
  void i2cSetSpeed200();

  /**
   */
  void i2cSetSpeed400();

  /**
   */
  void i2cSetSpeed1000();

  /**
   */
  void i2cWriteCRABit();

  /**
   */
  void i2cResetChannel();

  /**
   */
  void i2cReadSRA();

  /**
   */
  void i2cReadSR();

  /**
   */
  void i2cReadI2c() ;

  /**
   */
  void i2cWriteI2c() ;

  /**
   */
  void i2cReadModifyWrite();

  /**
   */
  void displayTrackerModulesNSpeed(std::list<keyType> *) ;

  /**
   */
  void scanForI2CDevices() ;

  /**
   */
  void i2cReadAll() ;

  /**
   */
  void i2cClearAll() ;

  /**
   */
  keyType getMemoryKey( bool ) ;

  /**
   */
  void memoryIsChannelEnable() ;

  /**
   */
  void memoryEnableChannel() ;

  /**
   */
  void memoryWriteCRA() ;
  
  /**
   */
  void memoryReadCRA() ;
  
  /**
   */
  void memoryWriteCRABit() ;
  
  /**
   */
  void memorySetSpeed1 () ;
  
  /**
   */
  void memorySetSpeed4 () ;
  
  /**
   */
  void memorySetSpeed10 () ;

  /**
   */
  void memorySetSpeed20 () ;
  
  /**
   */
  void memoryResetChannel() ;
  
  /**
   */
  void memoryReadWindowRegisters() ;
  
  /**
   */
  void memoryWriteWindowRegisters() ;
  
  /**
   */
  void memoryReadSR() ;

  /**
   */
  void memorySetMaskAnd() ;

  /**
   */
  void memorySetMaskOr() ;

  /**
   */
  void memorySetMaskXor() ;

  /**
   */
  void memoryReadModifyWrite() ;

  /**
   */
  void memoryReadMaskReg() ;

  /**
   */
  void memoryReadSB() ;

  /**
   */
  void memoryWriteSB() ;

  /**
   */
  void memoryReadMB() ;

  /**
   */
  void memoryWriteMB() ;

  /**
   */
  void memoryWriteCompareMB() ;

  /**
   */
  void memoryReadAll() ;
  
  /**
   */
  void memoryClearAll() ;
  
  /**
   */
  keyType getPiaKey( bool ) ;

  /**
   */
  void piaIsChannelEnable () ;

  /**
   */
  void piaEnableChannel() ;

  /**
   */
  void piaResetChannel();

  /**
   */
  void piaWriteGCR();

  /**
   */
  void piaWriteGCRBit();

  /**
   */
  void piaReadGCR();

  /**
   */
  void piaReadSR();

  /**
   */
  void piaWriteDDRP() ;

  /**
   */
  void piaReadDDRP() ;

  /**
   */
  void piaWriteDDRPBit() ;

  /**
   */
  void piaDDRSwitchOutputInput() ;

  /**
   */
  void piaDataRead();

  /**
   */
  void piaDataWrite();

  /**
   */
  void piaReadAll();

  /**
   */
  void piaClearAll();
  
  /**
   */
  apvDescription *buildApvDescription() ;

  /**
   */
  muxDescription *buildMuxDescription () ;

  /**
   */
  pllDescription *buildPllDescription () ;

  /**
   */
  laserdriverDescription *buildLaserdriverDescription () ;

  /**
   */
  void scanForTrackerDevices();

  /**
   */
  void trackerSetAllDevicesSelected();

  /**
   */
  void trackerListDeviceDetectedChanged();

  /**
   */
  void trackerSetDefaultValues();

  /**
   */
  void trackerWriteAll();

  /**
   */
  void trackerReadAll();

  /**
   */
  void trackerReadAll(apvDescription *oldApv, muxDescription *oldMux,
                      pllDescription *oldPll, 
                      laserdriverDescription *oldLaserdriver,
		      laserdriverDescription *oldDoh);

  /** \brief Clear all fields from the frame Tracker
   */
  void trackerClearAll();

  /**
   */
  void trackerFecEnableCounters();

  /** 
   */
  void trackerAddPiaKey ( ) ;

  /** 
   */
  void trackerDeletePiaKey ( ) ;

  /**
   */
  void trackerWritePiaReset();

  /** 
   */
  void trackerSetAllPiaSelected() ;

  /** 
   */
  void trackerWritePiaSelected();

  /** \brief Add all PIA channels for all CCUs for PIA Reset
   */
  void trackerPiaResetAddAllCCUKey();

  /** \brief build a PIA reset vector for all PIA setted in PIA Tracker window
   */
  void trackerRetreivePiaDescription ( piaResetVector *vPiaReset ) ;

  /** \brief If the database is not set, then the toggle file must stay checked
   */
  void trackerFileToggle();

  /** \brief If the database is not set, then the toggle file must stay checked
   */
  void trackerDatabaseToggle();

  /** \brief make the selection for the partition/state
   */
  void trackerSelectPartition ( ) ;

  /** \brief remove a device from the list
   */
  void trackerDeleteDeviceList() ;

  /** \brief Find a file name and put it on a text field
   */
  void trackerFindXMLFile() ;

  /** \brief find a file name and use it to upload values in it
   */
  void trackerFindSaveXMLFile() ;

  /**
   */
  void trackerLoadXML() ;

  /**
   */
  void trackerSaveXML() ;

  /**
   */
  void trackerSetListXML ( piaResetVector vPiaReset, deviceVector vDevice ) ;

  /* --------------------------------------------------------------------------------------------------- */
  /* CCU redundancy part                                                                                 */
  /* --------------------------------------------------------------------------------------------------- */

  /** Save in file
   */
  void ccuRedundanceSaveCCU ( ) ;

  /** Load file
   */
  void ccuRedundancyLoadCCU ( ) ;

  /** Display all the FEC/CCU found
   */
  void ccuRedundancyCache ( ) ;

  /** Reconfigure the FEC depending of the check box input/output
   */
  void fecRedundancy () ;

  /** Add a new CCU in the list
   */
  void ccuRedundancyAddCcuHand ( ) ;

  /** Create a group box for the corresponding ccu
   */
  void createNewCcuGroupBox ( keyType index ) ;


  /** Delete all the CCUs in the CCU redundancy table and delete also the hash-map
   */
  void ccuRedundancyClearCCU() ;

  /** Clear all the values after a reset (PLX or FEC)
   */
  void redundancyClearAll() ;

  /** Make an automatic check to test the reconfiguration of each CCU
   * This solution need to know what is the CCU addresses and the order of the CCU in the ring
   */
  void ccuRedundancyTestRingB() ;

  /** Try to empty all FIFOs in order to retreive a correct status
   */
  void emptyFIFOs ( FecAccess *fecAccess, keyType index, bool display=false ) ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Thread                                                                                              */
  /* --------------------------------------------------------------------------------------------------- */ 

  /** \brief start the thread, see the run method
   */
  void startThread() ;

  /** \brief
   */
  virtual void run();

  /* --------------------------------------------------------------------------------------------------- */
  /* FIFO                                                                                                */
  /* --------------------------------------------------------------------------------------------------- */ 
  /** Read a word in the FEC FIFO receive
   */
  void fecReadFifoRec() ;

  /** Read a word in the FEC FIFO return
   */
  void fecReadFifoRet() ;

  /** Read a word in the FEC FIFO transmit
   */
  void fecReadFifoTra() ;

  /** Write a word in the FEC FIFO receive
   */
  void fecWriteFifoRec() ;

  /** Write a word in the FEC FIFO return
   */
  void fecWriteFifoRet() ;

  /** Write a word in the FEC FIFO transmit
   */
  void fecWriteFifoTra() ;

  /** Empty the FEC FIFO receive
   */
  void fecEmptyFifoRec() ;

  /** Empty the FEC FIFO return
   */
  void fecEmptyFifoRet() ;

  /** Empty the FEC FIFO transmit
   */
  void fecEmptyFifoTra() ;

  /** \brief Catch the close event method
   */
  void closeEvent (QCloseEvent *e) ;

  /** static method to retreive the Tracker type device
   */
  static enumDeviceType getDeviceType ( keyType index ) { 

    enumDeviceType deviceType = APV25 ;

    switch (getAddressKey(index)) {
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:     // ------------- APV
      deviceType = APV25 ;
      break ;
    case 0x43:     // ------------ MUX
      deviceType = APVMUX ;
      break ;
    case 0x0:     // ------------- DCU
      deviceType = DCU ;
      break ;
    case 0x70:     // ------------- DOH
      deviceType = DOH ;
      break ;
    case 0x60:    // -------------- Laserdriver
      deviceType = LASERDRIVER ;
      break ;
    case 0x44:    // ------------- PLL
      deviceType = PLL ;
      break ;
    }

    return (deviceType) ; 
  }
} ;

#endif
