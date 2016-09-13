/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#ifndef VFATCONTROLLER_H
#define VFATCONTROLLER_H

#include <qpixmap.h>
#include <qthread.h>
#include <qapplication.h>

#include "VfatDialog.h"
#include "Definitions.h"
#include "TTP_Definitions.h"
#include "VFATFrame.h"
  
#include "FecAccess.h"
#include "TotemFecAccessManager.h"
#include "TotemXMLFecDevice.h"
#include "TotemFecFactory.h"

#ifdef BUSUSBFEC
#include "fec_usb.h"
#endif

#ifdef USB_GPIB_PS
#include "GPIBDevice.h"
#endif

#ifdef TTCCI_TRIGGER
#include "MultiBusAdapter.hh"
#include "TTCci.hh"
#endif

#ifdef VMEREADOUT
#include "TOTFED.hh"
#include <fstream>    // Filestream
#endif

typedef unsigned int uint ;

class vfat_controller: public VfatDialog, public QThread {

 private:

  tscType8 chanReg[VFAT_CH_NUM_MAX + 1] ; // VFAT Channel Registers

  std::vector <keyType> vfatAddresses_ ;   // Addresses for VFATs found
  std::vector <keyType> dcuAddresses_ ;    // Addresses for DCUs found
  std::vector <keyType> cchipAddresses_ ;  // Addresses for CCHIPs found
  std::vector <vfatAccess *> testVfats_ ;  // VFATs to be tested
  std::vector <std::string> testVfatIds_ ; // Ids for VFATs to be tested

  std::vector <vfatDescription *> testVfatdValues_ ;  // VFAT values used while testing
  std::vector <vfatDescription> vfatd_start_ ;        // VFAT values before testing (to be returned back)

  std::vector <std::string> chipTestDirs_ ;         // Directories where the chip testing resuts are stored
  std::vector <std::string> chipTestI2cErrorDirs_ ; // Directories for VFAT not responding to I2C
  std::vector <std::string> chipTestBarcodes_ ;     // Barcodes for VFATs to be tested

  int chanRegHis_ ; // Channel register range history
 
  bool testInitOK_ ; // VFAT found when starting testing

  bool showConsole_ ; // Show console messages ?

  bool i2cError_ ;
  std::string failedTest_ ;

  //uint i2cSpeed_ ; // I2C speed

  // Leds
  QPixmap greenLed_ ;
  QPixmap redLed_ ;
  QPixmap blackLed_ ;

  vfatAccess *vfatAccess_ ;        // vfat access
  dcuAccess *dcuAccess_ ;          // dcu access
  totemCChipAccess *cchipAccess_ ; // cchip access
  FecAccess *fecAccess_ ;          // fec access

  vfatDescription *vfatd_ ; // vfat description

  TotemFecAccessManager *fecAccessManager_ ; // access manager

  std::string detectorType_ ;
  std::string deviceFile_ ;

  enumFecBusType fecType_ ;

  // FEC index (used during the access restore)
  keyType fecIndex_ ;

  std::string vfat_controller_home_ ;

#ifdef BUSUSBFEC
  // Crepe settings
  fecusbType32 daq_regs_[CRCOUNT] ;
  std::string ttpFirmwareVersion_ ;
#endif

#ifdef TTCCI_TRIGGER
  // Handle for the TTCci board
  ttc::TTCci::TTCci *ttcci_ ;
#endif

#ifdef VMEREADOUT

  // Handle for the TOTFED board
  TOTFED *totfed_ ;

  HAL::VMEAddressTableASCIIReader myASCIIReader_ ;
  HAL::VMEAddressTable myAddressTable_ ;

  HAL::CAENLinuxBusAdapter::CAENLinuxBusAdapter vmeBusAdapter_ ;

  std::ofstream vmeofstream_ ;
  uint totalBytesRead_ ;

  char vmeDataBuffer_[TOTFED_READOUT_BUFFER_SIZE] ;

#endif

  bool miniFrames_ ;
  bool simpleTrigger_ ;
  bool chipTesting_ ;
  bool shortDACScan_ ;
  bool sbitDebug_ ;
  bool abortChipTesting_ ;
  bool detectorPresent_ ;
  bool abort_ ;
  bool dataRun_ ;
  bool threadRunning_ ;

 public:

  /** \brief Create the default accesses to the panels
   */
  vfat_controller ( ) ;

  ~vfat_controller ( ) ;

  void helpAbout(); 

  void quitApplication();

  /** Display the errors coming from FecAccessManager download and upload operation
   */
  void displayErrorMessage ( std::string message, std::list<FecExceptionHandler *> &errorList ) ;

  /** Display a dialog box when an exception occurs
   */
  void ErrorMessage ( char *title, FecExceptionHandler e ) ;

  /** General error messages
   */
  void ErrorMessage (std::string msg ) ;

  void ErrorMessage ( const char *msg ) ;

  void ErrorMessage ( const char *msg, const char *msg1 ) ;

  void closeEvent ( QCloseEvent *e ) ;

  void cleanUp() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* General                                                                                             */
  /* --------------------------------------------------------------------------------------------------- */ 

  std::string IntToString(int t) ;

  std::string IntToHex(int t) ;

  int HexToInt(std::string hexStg) ;

  std::string getTimeInfo() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Ring                                                                                                */
  /* --------------------------------------------------------------------------------------------------- */

  void createFecAccess() ;

  void scanForFecs() ;

  void scanForCcus() ;

  void scanForVfats() ;

  void getChipType ( keyType ) ;

  void scanAll_pressed() ;

  void addAndConfigure_pressed() ;

  void writeI2cRegisters_pressed() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Control Panel                                                                                       */
  /* --------------------------------------------------------------------------------------------------- */ 

  void cr0Sel_pressed();

  void cr1Sel_pressed();

  void cr2Sel_pressed();

  void cr3Sel_pressed();

  void IPreampInSel_pressed();

  void IPreampFeedSel_pressed();

  void IPreampOutSel_pressed();

  void IShaperSel_pressed();

  void IShaperFeedSel_pressed();

  void ICompSel_pressed();

  void latencySel_pressed();

  void VCalSel_pressed();

  void VThreshold1Sel_pressed();

  void VThreshold2Sel_pressed();

  void calPhaseSel_pressed();
   
  void chanRegSel_pressed();

  void selectAllReg_pressed() ;

  void selectNoneReg_pressed() ;

  uint getCr0() ;
  
  uint getCr1() ;

  uint getCr2() ;

  uint getCr3() ;

  uint getCalPhase() ;
  
  void setCr0( uint ivalue ) ;

  void setCr1( uint ivalue ) ;

  void setCr2( uint ivalue ) ;

  void setCr3( uint ivalue ) ;

  void setCalPhase( uint ivalue ) ; 

  void vfatIPreampInT_lostFocus() ;

  void vfatIPreampFeedT_lostFocus() ;

  void vfatIPreampOutT_lostFocus() ;

  void vfatIShaperT_lostFocus() ;

  void vfatIShaperFeedT_lostFocus() ;

  void vfatICompT_lostFocus() ;

  void vfatLatencyT_lostFocus() ;

  void vfatVCalT_lostFocus() ;

  void vfatVThreshold1T_lostFocus() ;

  void vfatVThreshold2T_lostFocus() ;

  bool checkControlPanelValues() ;

  // Channel Registers

  void setChannelRegisters( int ) ;

  void getChannelRegisters( int ) ;

  void checkTrimDACs() ;
  
  void range1_pressed() ;

  void range2_pressed() ;

  void range3_pressed() ;

  void range4_pressed() ;

  void fecSlots_activated( int ) ;

  void ccuAddresses_activated( int ) ;

  void i2cChannels_activated( int ) ;

  void deviceAddresses_activated( int ) ;

  void refreshCcuAddresses() ;

  void refreshI2cChannels() ;

  void refreshDeviceAddresses() ;

  double getDcuTemperature() ;

  void readCurrentInfo_pressed() ;

  void pvssInfo_pressed() ;

  void updatePvssInfo() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Test Panel                                                                                          */
  /* --------------------------------------------------------------------------------------------------- */ 

  void getTestVfatDescription ( vfatDescription &vfatd, tscType8 ) ;

  void cloneTestVfatDescription ( vfatDescription &vfatd, vfatDescription *vfatD) ;

  void testSaveVfatValues() ;

  void testWriteBackVfatValues() ;

  std::string testI2CError( int testNumber, int vfatNumber ) ;

  void testPulseOneChannelSel_pressed() ;

  void testThresholdMeasurementSel_pressed() ;

  void testLatencyScanSel_pressed() ;

  void testPulseVCal1T_lostFocus() ;

  void testPulseVCal2T_lostFocus() ;

  void testThresholdVThreshold1T_lostFocus() ;

  void testThresholdVThreshold2T_lostFocus() ;

  void testThresholdVThreshold12T_lostFocus() ;

  void testLatencyScanLatency1T_lostFocus() ;

  void testLatencyScanLatency2T_lostFocus() ;

  bool testCheckValues() ;

  void fillVfatTestList() ;

  void testCleanUp() ;

  void displayDebugInfo() ;

  keyType getPiaKey ( bool initChannel=true ) ;

  uint getPPBDirections() ;

  void setPPBDirections( uint ivalue ) ;

  void readPPBDirections_pressed() ;

  void writePPBDirections_pressed() ;

  void readPPBData_pressed() ;

  void writePPBData_pressed() ;

  uint getPPBData() ;

  void setPPBData( uint ivalue ) ;

  void fecSlots2_activated( int ) ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Test Functions                                                                                      */
  /* --------------------------------------------------------------------------------------------------- */

  /**
   * This initializes VFAT test.
   */
  void initVfatTest() ;

  /** 
   * This method reads VFAT's chip ID.
   */
  std::string readVfatChipID() ;

  /** 
   * This method powers the VFAT on.
   */
  //std::string powerOnVfat () ;

  /** 
   * This method does the mapping according to the VFAT position.
   */
  std::string positionMappingVfat () ;

  /** 
   * This method tests the DACs on VFAT.
   */
  std::string testVfatDAC () ;

  /** 
   * This method pulses one channel, sets up latency and verifies data packet.
   */
  std::string pulseOneChannelVfat () ;

  /** 
   * This method checks dead channels.
   */
  std::string checkDeadChannelsVfat () ;

  /** 
   * This method measures thresholds.
   */
  std::string thresholdMeasurementVfat () ;

  /** 
   * This method does the latency scan.
   */
  std::string latencyScanVfat () ;

  /** 
   * This method writes defaults to chip.
   */
  void setDefaultsVfat () ;

  /** 
   * This method generates default tag, which includes the initial values
   */
  std::string getXMLTagInit() ;

  /** 
   * This method generates default tag, which includes the start values
   */
  std::string getXMLTagDefault ( deviceVector ) ;

  /** 
   * This method returns string containing full configuration for all vfats connected 
   */
  std::string getFullConfigInfo() ;

  /** 
   * This method saves the configuration info for all the vfats
   */
  void saveConfigFile() ;

  /** 
   * This method saves the Monitor configuration file for all vfats
   */
  void saveMonitorConfigFile( std::vector <std::string> ) ;

  /** 
   * This method starts the tests. See run method for details.
   */
  void testStart_pressed() ;

  void getDefaultDescription ( vfatDescription &vfatd, std::string file ) ;

  std::string getDifferences ( vfatDescription *vfatd, vfatDescription &vfatd_ref ) ;

  /* --------------------------------------------------------------------------------------------------- */
  /* VFAT                                                                                                */
  /* --------------------------------------------------------------------------------------------------- */

  /** \brief Return the value of the VFAT channel register corresponding to a VFAT register
   * Return the value of the VFAT channel register corresponding to a VFAT register
   * \param index - index
   * \return the value
   */
  tscType8 getChReg ( int index ) ;

  /** \brief set the value for register
   * Set the value for a VFAT description
   * \param index - index
   * \param value - value to be set
   */
  void setChReg ( int index, tscType8 value );

  /** \brief Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
   * Return the pointer of the VFAT channel registers corresponding to a VFAT channel registers' values
   * \return the pointer
   */
  tscType8 *getAllChReg () ;

  /** \brief set the values for all channel registers
   * Set the values for all channel registers
   * \param *value - pointer to values to be set
   */
  void setAllChReg ( tscType8 *value ) ;

  /** \brief set the same value for all channel registers
   * Set the same value for all channel registers
   * \param value to be set
   */
  void setAllChRegSame ( tscType8 value ) ;

  /** \brief Get complete address for panel's VFAT
   * Get complete address for panel's VFAT
   */
  keyType getPosition() ;

  void readVfatId() ;

  void findMultiXML() ;

  void writeMultiXMLValues() ;

  void readCounters_pressed() ;

  void readChip_pressed() ;

  void writeChip_pressed() ;

  void loadXML_pressed() ;

  void saveXML_pressed() ;

  void readMultiXML_pressed() ;

  void writeMultiXML_pressed() ;

  void readMultiXMLValues() ;

  void createDevices( deviceVector ) ;

  void configureDevices( deviceVector ) ;

  void resetPIA( piaResetVector ) ;
  
  /* --------------------------------------------------------------------------------------------------- */
  /* Thread                                                                                              */
  /* --------------------------------------------------------------------------------------------------- */ 

  /** \brief Thread's run method
   * Thread's run method where the actual testing is performed.
   */
  virtual void run();

#ifdef BUSUSBFEC

  /* --------------------------------------------------------------------------------------------------- */
  /* Chip Testing  -  Crepe Part                                                                         */
  /* --------------------------------------------------------------------------------------------------- */ 

  void setCrepeControl1( fecusbType32 value ) ;

  fecusbType32 getCrepeControl1() ;

  void setCrepeControl3( fecusbType32 value ) ;

  fecusbType32 getCrepeControl3() ;

  void setCrepeLat( fecusbType32 value ) ;

  fecusbType32 getCrepeLat() ;

  void setCrepeTriggerEnabledPeriod( fecusbType32 value ) ;

  fecusbType32 getCrepeTriggerEnabledPeriod() ;

  void setCrepeSettings( int option=0 ) ;

  void writeCrepeSettings( int device ) ;

  void readCrepeSettings( int device , fecusbType32 *readvalue) ;

  void writeCrepeTrigger( int device ) ;

  void writeFastCrepeTrigger( int device) ;

  void resetCrepeFifos( int device ) ;

  void startCrepeTrigger( int device ) ;

  void crepeMasterReset( int device ) ;

  void enableCrepeExtTrigger( int device ) ;

  void disableCrepeExtTrigger( int device ) ;

  void crepeDataTakingExtTrigger( int device ) ;

  void readAndWriteData( int device, std::string fileName ) ;

  void buildFramesFromBuffer( std::vector<Totem::VFATFrame> &data, unsigned *vfat_data, int vfat_words_read ) ;

  void readCrepeDataBuffers( int device, std::vector<Totem::VFATFrame> &data, std::vector<std::string> &dataFiles, bool checkFrames, int nFifo=0 ) ;

  void saveSbitInfo( int &total_sbits, unsigned int sectors_1[], unsigned int sectors_2[], unsigned int sectors_3[], unsigned int sectors_4[], int ch=0 ) ;

  void writeDataFile( std::vector<Totem::VFATFrame> &data, std::string fileName, bool append ) ;

  void readDataFile( std::vector<Totem::VFATFrame> &data, std::string fileName ) ;

  void printData( std::vector<Totem::VFATFrame> &data ) ;

  void updateLog() ;

  std::string createDirectory( std::string hybridid, int version, int position, std::string chipid ) ;

  //std::string createDirectory( std::string barcode, std::string id ) ;

  void createDirectories() ;

  void readConfigFile() ;

  void runMonitorAnalysis() ;

  void runTestManager() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Chip Testing Panel                                                                                  */
  /* --------------------------------------------------------------------------------------------------- */

  void clearBarcodeList_pressed() ;

  void startChipTest_pressed() ;

  void standardChipTesting_pressed() ;

  void fullI2Check_pressed() ;

  void advancedChipTesting_pressed() ;

  void tabWidget2_currentChanged( QWidget *qwidget ) ;

  void T1TypeSel_pressed() ;

  void T2TypeSel_pressed() ;

  void RPTypeSel_pressed() ;

  /* --------------------------------------------------------------------------------------------------- */
  /* Chip Testing Methods                                                                                */
  /* --------------------------------------------------------------------------------------------------- */ 

  void saveMonitorConfigFiles() ;

  std::string getTestSettings() ;

  void I2CScan() ;

  std::string testDataPacketCheck () ;

  std::string testVfatI2C () ;

  std::string maskCheck() ;

  std::string pulseScan() ;

#ifdef USB_GPIB_PS
  void powerOnHV() ;

  void powerOffHV() ;
#endif

#endif

#ifdef TTCCI_TRIGGER

  /* --------------------------------------------------------------------------------------------------- */
  /* TTCci Methods                                                                                       */
  /* --------------------------------------------------------------------------------------------------- */ 

  void findTTCciConfig_pressed() ;

  void testTrigger_pressed() ;

  void ttcci_configure( std::string fn ) ;

  void ttcci_executeSeq( std::string seq , int msleeptime=0 ) ;

  void ttcci_setIntTrigger() ;

  void ttcci_setExtTrigger() ;

#endif

#ifdef VMEREADOUT

  /* --------------------------------------------------------------------------------------------------- */
  /* TOTFED Methods                                                                                      */
  /* --------------------------------------------------------------------------------------------------- */ 

  void boardReset_pressed() ;

  void totfed_reset() ;

  void totfed_init() ;

  void totfed_configure( uint ) ;

  uint generateAndReadEvents( uint sequenceNumber, int nRepetitions, std::string seq="", int msleeptime=0 ) ;

  void dataTakingExtTrigger() ;

#endif

  /* --------------------------------------------------------------------------------------------------- */
  /* Coincidence Panel                                                                                   */
  /* --------------------------------------------------------------------------------------------------- */ 

  void fecSlots3_activated( int ) ;

  void ccuAddresses3_activated( int ) ;

  void i2cChannels3_activated( int ) ;

  void deviceAddresses3_activated( int ) ;

  void refreshCcuAddresses3() ;

  void refreshI2cChannels3() ;

  void refreshDeviceAddresses3() ;

  void scanAll2_pressed() ;

  keyType getCChipPosition() ;

  void readCChipId() ;

  void readCChip_pressed() ;

  void writeCChip_pressed() ;

  void readCChipCounters_pressed() ;

  void loadSingleXML2_pressed() ;

  void saveSingleXML2_pressed() ;

  void controlSel_pressed() ;

  void counterSel_pressed() ;

  void impedanceSel_pressed() ;

  void cchip_mask0Sel_pressed() ;

  void cchip_mask1Sel_pressed() ;

  void cchip_mask2Sel_pressed() ;

  void cchip_mask3Sel_pressed() ;

  void cchip_mask4Sel_pressed() ;

  void cchip_mask5Sel_pressed() ;

  void cchip_mask6Sel_pressed() ;

  void cchip_mask7Sel_pressed() ;

  void cchip_mask8Sel_pressed() ;

  void cchip_mask9Sel_pressed() ;

  void cchip_maskAll_pressed() ;

  void cchip_maskNone_pressed() ;

  void cchip_unmask( int ) ;

  void cr1NP_activated( int ) ;

  void cr3CoincidenceSel_activated( int ) ;

  void cr3OutputCondition_activated( int ) ;

  void logicBlock_activated( int ) ;

  void logicText_toggled( bool ) ;

  void updateLogicPic( int, int ) ;

  void selectAllReg2_pressed() ;

  void selectNoneReg2_pressed() ;
  
  uint getCChipCr0() ;
  
  uint getCChipCr1() ;

  uint getCChipCr2() ;

  uint getCChipCr3() ;

  uint getCChipCr4() ;

  uint getCChipRes() ;

  uint getCChipMask( int ) ;
 
  void setCChipCr0( uint ) ;

  void setCChipCr1( uint ) ;

  void setCChipCr2( uint ) ;

  void setCChipCr3( uint ) ;

  void setCChipCr4( uint ) ;

  void setCChipRes( uint ) ;

  void setCChipMask( int, uint ) ;

} ;

#endif
