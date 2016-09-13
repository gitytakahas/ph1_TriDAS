File  : DbCommonAccess
Date  : 04/02/2008
Author: F. Drouhin
Update:
  - Change the creation of the clob to use the variable DBMS_LOB.CALL

File  : FecAccess
Date  : 03/12/2007
Author: W. Bialas
Update:
  - Bug correction in FecAcess constructor if you use the VME 64x PNP. One of the parameter
    was not initialised properly.

File  : All description classes
Date  : 23/11/2007
Update:
  - All definition of parameter's names have been put in the class
  - All definition from ParameterDescription have been put in the class (ParameterDescription::INTEGER32) for example
  - The method deleteParameters have been removed and only the method is coming from deviceDescription

File  : TkIdVsHostname classes
Date  : 14/11/2007
Update:
  - Adding a class dedicated to the FED ID versus hostname in the project for the Strip Tracker.

File  : Description / Definition 
Date  : 14/11/2007
Update:
  - the enumeration and description of parameters should be put in the class itself to avoid
    enumeration with same name.

File  : DeviceFactory classes
Date  : 07/11/2007
Update:
  - All XML/DB classes have been changed in order to avoid copy of
  pointers.
  - Be really carefull when you delete the vector (deleteVector
  methods from Factory) since the delete is handled by the Factory.
  - The method remains as deleteVectorI that should be used if you
  create your own device and add it in a vector. The renainming of the
  method will be changed soon.
  - The code is in current evaluation with valgrind.
  - The exception message is now retreived with what for all classes
  including FecExceptionHandler and SQLException except for XERCES
  classes which not support it.

File  : DeviceFactory
Date  : 06/11/2007
Update:
  - All the vector returned by the DeviceFactory classes (concern only totem/tracker) should not be deleted since
    the class itself keep a reference on it.

File  : Most of the files that catch exceptions (including pixel,preshower,totem files)
Date  : 25/10/2007
Tests : valgrind
Update:
  - All catch block with exception get now a reference and not anymore a copy of the object
  - the method getMessage for the exceptions have been replaced by what (FecExceptionHandler and GenericExceptionHandler) except for XERCES

File  : FecAccess
Date  : 06/11/2007
Tests : To be done
Update:
  - Following the change for the inversed polarity, the FecAccess::createFecAccess have been changed to handle automatically the following options:
     * force acknowledge used (-noFack or -fack (default is -fack))
     * i2cSpeed used (-i2cSpeed [100 | 200 | 400 | 1000])
     * initialise or not the FEC (-noInitFec, -initFec (default is initFec)
     * initialise only the corresponding FEC if it is different from 0xFFFFFFFF (only for VME FEC) (-fec <>)
     * invert the clock polarity (-invrecvclk, false by default)
  - The FecAccess::createFecAccess method take now the following parameters (int argc, char **argv, int *cnt, bool init) where init initialise the ring before using it.
    The rest of the parameters (FecSlot, ForceAck, i2cSpeed) are now handled by this method and not anymore externally

File  : FecPciRingDevice, FecUsbRingDevice, FecVmeRingDevice, FecRingDevice
Date  : 16/10/2007
Tests : To be done
Update:
  - Remove in FecRingDevice the attributs fecInput and fecOutput, internalclock.
  - Add a new parameter in the classes which will invert the clock polarity in FecAccess and FecRingDevice and subsequent classes.
  - The inversion will be kept even if a reset is applied (firmware reload, crate reset, FEC reset, FEC reset B, FEC FMS reset)

File  : FecVmeRingDevice, CCSTrigger, FecAccess
Date  : 24/08/2007
Tests : 
Update:
  - Change CCSTrigger, FecVmeRingDevice, FecAccess for some changes related to Preshower 

File  : Makefile(s)
Date  : 22/08/2007
Tests : 
Update:
  - Makefile has been changed accordingly to 3.11
  - the include directories are only:
    * $(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include \
    * $(XDAQ_ROOT)/$(XDAQ_PLATFORM)/include/$(XDAQ_OS) \
  - the library directory is:
    * $(XDAQ_ROOT)/$(XDAQ_PLATFORM)/lib
  - All header files has been changed due to changes in header files from XDAQ 3.11

File  : FecVmeRingDevice
Date  : 27/06/2007
Tests : done
Update:
 - the getFirmwareVersion method that returns the mFec firmware version
   returns now only the 2 high digit ie. 0x1700 => 0x17

File  : FecAccess, FecVmeRingDevice
Date  : 26/06/2007
Tests : done
Update:
  - Adding a method getTriggerVersion in both classes to retreive the version of the trigger
    firmware 

File  : pllAccess
Date  : 26/06/2007
Tests : 
Update:
  - Problem in pllAccess::getPllValuesMultipleFrames if the value of delay fine or delay coarse for one PLL
    in the list are greater than 23 then an exception is raised that are not catched in this method is the 
    rest of the PLL are not configured.
  - Adding a catch of the exception in the method to let the rest of the PLL to be configured

File  : FecAccess
Date  : 19/06/2007
Tests : done
Update:
  - Adding method to empty FIFOs and to clear a ring error bit
    * clearFecCcuErrorRegisters
    * emptyFifoReturn
    * emptyFifoReceive
    * emptyFifoTransmit

File  : FecAccess
Date  : 11/06/2007
Tests : to be done
Update:
  - Adding numberOfRing in VME FecAccess constructors: FEC can have only few mFECs, each mFECs missing give a timeout on the 
    FEC VME that can be very long in order to avoid this problem, numberOfRing (default 8) is specified to scan only the ring involved:
    * Numbering schema given in the dat file is going from 0 to 7, top to bottom, if the numberOfRing is 5 then the scan will go from ring 0, 1, 2, 3, 4
    * Numbering schema given in the dat file is going from 8 to 1, top to bottom, if the numberOfRing is 5 then the scan will go from ring 4, 5, 6, 7, 8 in this order
  - Please remember that the top slot should be always set and the numberOfRing is working only if you put the mFECs in contigus way.

File  : FecAccessManager, pllAccess, pllDescription
Date  : 11/06/2007
Tests : done in general way so not PLL per PLL
Update:
  - PLL DAC parameter has been added for each PLL so you can apply a different value for each PLL
  - The PLL DAC is only usefull if you are using the PLL in cold (< -10 C)

File  : FecAccessManager
Date  : 25/04/2007
Tests : done
Update:
  - Download of DOH can be set or not in the device access manager

File  : FecAccess, cmdDescription
Date  : 25/04/2007
Tests : partially done
Update:
   - Adding the CCS trigger access to status and control register
     in FecAccess class.

File  : FecAccess, FecVmeRingDevice, DohAccess
Date  : 12/04/2007
Tests : done
Update:
   - Validation of the DOH calibraion in one setup of the Si. Tracker
   - FecAccess and FecVmeRingDevice has been changed to return the minimum and maximum ring 
     number (0 to 7 or 8 to 1, top to bottom). 

File  : DohAccess, FecAccessManager
Date  : 23/03/2007
Tests : done but still some problem during the procedure
Update:
   - Adding method for the DOH calibration both in DohAccess and FecAccessManager
   - Please note that when you call the DOH calibration method, the ring is reseted several times

File  : FecRingDevice, FecAccess
Date  : 13/03/2007
Tests : done but show one bug
Update:
  - Make a new method FecAccess::isChannelEnable
  - Bug correction in the Si Tk detection (include/FecDetectionUpload.h) that show a bug
     * The previous scan was done in no broadcast mode (ie. the scan of the CCUs was done 
       by scan each of the possible CCU address): this method seems to reset (or modify) the 
       CCU CRE. By changing this method to the broadcast mode, the CCU CRE remains the same
       but shows a bug/feature (?) of the method FecRingDevice::getCcuType
     * This method has been completed in order to see if the PIA channel are still enabled
       and raise an error if not. This error can be detected by the check of the enabled channel
       in the CCU or by reading back the value of the PIA since the FEC replies always address
       seen, data copied.
  - FecRingDevice::getCcuType bug:
     * The use of broadcast and non broadcast methods calls the method FecRingDevice::getCcuType 
       that will check if the CCU is a CCU 25 or an old CCU (check of the CRE => 8 words or 6 words).
       Anyway the CCU CRE read is not correct (always 0x1 or 0x0) and if you read back again by the method
       getCcuCRE shows the correct value. 
     * I compare the two method FecRingDevice::getCcuType and FecRingDevice::getCcuCRE (that cannot
       be called here since the CCU is not known the class FecRingDevice) shows no difference. Since
       the method is not used, it does not affect the use of the class. Anyway, the problem should be
       understand.

File  : DohAccess
Date  : 13/03/2007
Tests : not done
Update:
  - Add a calibration method for the DOH access. Method is based on a gain of 1, 
    then a calibration is done on the bias until the ring is lost.
  - This procedure shows some failures and should be investigated in the next coming weeks.
  - ProgramTest.exe -calibrateDOH call this method and investigation can be done on this
    method.

File  : all descriptions (totem, tracker, preshower)
Date  : 02/02/2007
Tests :
Update:
  - All the core of the methods has been moved to .cc file

File  : pllAccess
Date  : 30/01/2007
Tests : 
Update:
  - The cold PLL init procedure is based on the version 6 provided by P. Moreira
    (see the description in pllAccess::pllInit)
  - pllAccess::getPllColdInitVersion return a string with the PLL version

File  : FecVmeRingDevice, FecAccess
Date  : 30/01/2007
Tests : 
Update:
  - Following the modification of the uFecReload, the method fecHardReset is now
    link on FecVmeRingDevice::uFecReload(0x7) to reload all the firmware
  - Method FecAccess::fecHardReset (index, value) where value (only for VME FEC) is linked 
    to  FecVmeRingDevice::uFecReload(value)

File  : FecVmeRingDevice
Date  : 23/01/2007
Tests : done for VME version, not done for the reload of the firmware
Update:
  - FecAddressTable.dat has been updated for:
     => INT_MASK, INT_LEVEL, VME_VERSION_FW, RELOAD_FW
     => JTAG capability to be tested
  - Method uFecsReload has been changed to add a parameter (default = 0x7, all reload)
     => 0x1: mFEC reload
     => 0x2: Trigger FPGA
     => 0x4: VME FPGA

File  : pllAccess
Date  : 18/01/2007
Tests :
Update:
  - For debugging purpose the methods set/getCNTRL4 have been changed to set the CTRL2<5>=0.
    The original value of CTRL2 is set back after the change.
  - For debugging purpose the methods set/getCNTRL5 have been added and set the CTRL2<5>=1.
    The original value of CTRL2 is set back after the change.
  - The value of PLLDAQ in the CTRL3 can be a parameter of pllInit or setPllCheckGoingBitMultipleFrames

File  : FecRingDevice
Date  : 18/01/2007
Tests : 
Update:
  - Method checkFifoReceive has been updated to add a parameter to display or not on the console.
    The default parameter is set to display only if the ring is closed (that does not take care
    about possible change on the SR0 when it is blinking).

File  : XML classes
Date  : 09/01/2007
Tests : partially done, the version is set a beta version
Update:
  - Adding DOM parser in the XML class, a switch is existing in XMLFec to 
    come back to the SAX parser but the backward compatibility has not been yet tested

File  : dcuAccess
Date  : 08/12/2006
Tests : 
Update:
  - Add read/write to the registers of the DCU (CREG,SHREG,LREG,AREG)

File  : Preshower class for XML/Descriptions
Date  : 07/12/2006
Tests : done
Update:
  - esMbResetDescription class added
  - MemBufOutputSource / tscTypes.h

---------------------------------------------------- FecSoftwareV3_22_2

File  : All
Tag   : FecSoftwareV3_22_2
Date  : 07/12/2006
Tests : done
Update: Before upgrade of Preshower Software

File  : FecRingDevice, FecPciRingDevice, FecUsbRingDevice
Date  : 22/11/2006
Tests : done
Update: 
  - Following the problem of the timeout, the software has been updated to have 500ms (function of the read of SR0
    for the timeout of force ack. 
  - All the timeout has been updated in order to integrate a test on the CR0

File  : FecRingDevice
Date  : 22/11/2006
Tests : done
Update:
  - Following problem meet in a Tracker ring, the timeout assign and recalculate by the software has been removed
  - Measurement has been done on several PCs and we saw the following results (after 2 attempts):
    Pentium III (Coppermine): 
      Time to read 10000000 times the status register 0 of the FEC is 152 so 1.52e-05 per operation
      Time to write 10000000 times the control register 0 of the FEC is 142 so 1.42e-05 per operation
    Intel(R) Pentium(R) 4 CPU 3.00GHz: 
      Time to read 10000000 times the status register 0 of the FEC is 128 so 1.28e-05 per operation
      Time to write 10000000 times the control register 0 of the FEC is 137 so 1.37e-05 per operation
    2 x Intel(R) Xeon(TM) CPU 3.00GHz (1 U PC)
      Time to read 10000000 times the status register 0 of the FEC is 124 so 1.24e-05 per operation
      Time to write 10000000 times the control register 0 of the FEC is 120 so 1.2e-05 per operation
  - Following this result, the TIMEOUT is now fixed for 100 times x time to read a SR0 so something between
    1.24 ms and 1.42 ms
  - The re-calculation of the timeout can be done by enable the tag RECALCULATETIMEOUT in file FecRingDevice.cc

File  : FecDeviceFactory and PiaResetFactory
Date  : 24/10/2006
Tests : 
Update:
  - For upload in major version or upload in file, the devices uploaded to
    the database and the file are complete with the disabled devices

File  : CCU description, XML, database
Date  : 23/10/2006
Tests : 
Update:
  - Adding CCU database/file interface

File  : pllAccess
Date  : 23/10/2006
Tests : not tested
Update:
  - New PLL cold init version were introduced

File  : dcuDescription
Date  : 6/10/2006
Tests : compatible with previous version
        new version working fine
        tests should be done on database
Tag   :
Update:
  - dcuDescription include not the parameter enabled as all the devices
  - but a new parameter has been added to enable or disable it in the DCU readout

File  : deviceDescription and piaResetDescription
Date  : 5/10/2006
Tests : compatible with previous version
        new version working fine
        tests should be done on database
Tag   :
Update:
  - deviceDescription, piaResetDescription has been modified to integrate the tag enabled
  - the schema for XML validation has been changed accordingly (03/10/2006)

File  : pllAccess.cc
Date  : 03/10/2006
Tests : done on TIB
Tag   : FecSoftwareV3_20
Update:
  - PLL cold init was rewritten (both for mono and multi frames) with the following procedure (v3):
    *           ORG  V1   V3
    * 1 - write 0x80 0x80 0x80 to reg 0    
    * 2 - write 0x80 0xD0 0xC0 to reg 1    
    * 3 - write 0x84 0x84 0x84 to reg 0    
    * 4 - write 0xFF 0xFF 0xFF to reg 2    
    * 5 - write 0x00 0x00 0x00 to reg 3    
    * 6 - write 0x80 0x80 0x80 to reg 1    
    * 7 - write 0x80 0x80 0x00 to reg 0  

File  : FecPciDeviceDriver
Date  : 29/09/2006
Tests : done
Tag   : FecSoftwareV3_19_2
Update:
  - Autodection of the kernel version to be compatible for kernel 2.4 and kernel 2.6

File  : pllAccess.cc
Date  : 29/09/2006
Tests : done
Tag   : FecSoftwareV3_19_2
Update:
  - The first attempt in the PLL init is to reset it in normal way 
  - This change is compatible with the previous version except a check the GOING bit after
    the normal reset

File  : DbCommonAccess.cc
Date  : 25/09/2006
Tests : done
Tag   :
Update:
  - CrateController, FecSupervisor, DcuFilter upgrade
  - The database classes shared now the same connection on the database

-------------------- MIGRATION TO IT CVS
-------------------- THE CMS CVS is not more used for modification on it
-------------------- THE READ ACCESS WILL BE SOON UNAVAILABLE
-------------------- Version tagged as FecSoftware_V3_19_0

File   : CCUDescriptions.cc
Date   : 13/07/2006
Tests  : done
Tag    :
Update :
 - Adding redundancy in XML file

File   : APIAccess.cc
Date   : 13/07/2006
Tests  : done
Tag    :
Update :
  - Check and update the redundancy for TIB/TID people

File   : all files
Date   : 13/07/2006
Tests  : not done (need the latest version of HAL 03.14)
Tag    :
Update :
  - "Repolishing" all the name space in all files from FecSoftware.
  - With version of HAL 03.14, problem of link with ASCIIFileModuleMapper

File   : dcuDescription
Date   : 12/07/2006
Tests  : done
Tag    :
Update :
  - Adding default conversion for tracker DCU conversion factors

File   : FecRingDevice
Date   : 10/07/2006
Tests  : done
Tag    :
Update :
  - Correct a bug in FecRingDevice::getBlockFrames on the number of frames
    counted
  - Another bug is existing in case of mixing multiframes mode and mono-frame, 
    I'm trying to solve the problem but it does not affect the configuration for 
File   : dcuDescription, dcuAccess, FecAccessMananger
Date   : 03/07/2006
Tests  : done
Tag    : FecSoftwareV3_18_10
Update :
  - Add the DCU type in the dcuDescription and dcuAccess
  - During automatic detection the type is set in dcuDescption
  - During FecSupervisor/FecAccessManager the DCU type is also set

File   : XML and dcuDescription / dcuDefinition
Date   : 29/06/2006
Tests  : done
Tag    : FecSoftwareV3_18_7
Update :
  - Add a new parameter in the DCU to indicate the type (DCU on CCU or DCU on FEH)
  - The software remains compatible with the previous version

File   : FecRingDevice
Date   : 23/06/2006
Tests  : 
Tag    : FecSoftwareV3_18_6
Update :
   - Modify the size of the FIFO return for 1024 8 bits word.

File   : pllAccess
Date   : 07/06/2006
Tests  : 
Tag    : FecSoftwareV3_18_6
Update :
   - Update of the PLL access in multi-frame mode: pllAccess::setPllCheckGoingBitMultipleFrames
     Adding two lists, one contains the PLL in NOT nominal state before the PLL reset
     the second one contains the PLL in NOT nominal state after the PLL reset
   - The error is taking care in the FecAccessManager (for the Tracker at least)

File   : FecRingDevice
Date   : 29/05/2006
Tests  : done 
Tag    : FecSoftwareV3_18_3
Update : 
   - Bug correction in the FecRingDevice::setBlockFrames for VME FEC firmware < 0x1600

File   : XML
Date   : 29/05/2006
Tests  : done
Update :
   - Removing the reference to the version of XERCES
   - The following reference can be used but any refernce to xerces version in the
     FecSoftwareV3_0 is removed.
       util/XercesVersion.hpp:#define XERCES_VERSION_MAJOR 2
       util/XercesVersion.hpp:#define XERCES_VERSION_MINOR 7

File   : FecRingDevice
Date   : 26/05/2006
Tests  : done
Update :
   - adding the following method: FecRingDevice::fecRingResetFSM

File   : XML, Database access,
Date   : 23/05/2006
Tests  : done
Update :
   - All the classes for XML and database accesses has been changed for performance optimization

File   : FecRingDevice, FecAccess
Date   : 23/05/2006
Tests  : done
Update :
   - Change the Tracker DCU conversion factors
   - Add a method to reset the FSM of the FEC in FecRingDevice and FecAccess

File   : FecRingDevice, FecAccess
Date   : 25/04/2006
Tests  : Done on 1 tracker ROD
Update :
  - FecRingDevice::setBlockDevicesBltMode: the block mode is available on VME FEC for firmware >= 1.6
  - FecAccess::setBlockDevicesParallel: has been added in order to send frames on several rings in parallel
    For each ring, this method sort the frames, then fill the FIFO transmit and toggle the bit send
    Then
    For each ring, call the method FecRingDevice::getBlockFrames to receive, analyse the frames received
    The gain is relative to the time for the FIFO transmit running and FIFO receive procedures
    So when a FEC ring is running (hardware side), the process can send the frames to another ring
  - FecRingDevice::checkRing: has been added in order to check the SR0 and try to recover the ring 
  - FecRingDevice::getBlockFrames: has been added, this method complete the 
    FecAccess::setBlockDevicesParallel in order to receive, analyse and fill the itAccess data structure 

File   : TkDcuConversionFactors, MemBuffOuputSource
Date   : 11/04/2006
Tests  : done
Update :
  - Type of some parameters has been updated 
    (structureId and moduleId are now string and the detid is an unsigned long)

File   : FecVmeRingDevice, FecAccess
Date   : 07/04/2006
Tests  : done but the new VME64xDevice based methods are not working
Update :
  - In the plug and play method if a crate reset is performed then the plug and play must be redone
  - The method FecVmeRingDevice::fecHardReset and FecAccess::fecHardReset perform a reload of the firmware if the plug and play is done
  - A new method that explicitly reset the crate has been implement (FecAccess::crateReset)

File   : FecSupervisor, FecFactory, FecDeviceFactory, PiaResetFactory
Date   : 10/03/2006
Tests  : done
Update :
  - FecSupervisor and Fec<>Factory classes now is used to handle the FecHardwareId. For the PCI the slot of the FEC is used.

File   : FecVmeRingDevice, APIAccess
Date   : 10/03/2006
Tests  : done
Update  :
  - FecVmeRingDevice: add the getFifo and setFifo for all FIFOs in block mode
  - Add the test of the FIFO with block in FEC tests

File   : FecRingDevice, FecVmeRingDevice
Date   : 08/03/2006
Tests  : not finished
Update :
  - FecRingDevice: change the length of the FIFO due to a problem in the firmware FIFO size (768 -> 512)
  - FecVmeRingDevice: bug in the block transfer mode (corrected)

-------------------------------------------------------> Tag FecSoftwareV3_12_11

File   : Preshower description, DeviceFactory
Date   : 01/03/2006
Tests  : done
Update :
  - Remove the access header from description source
  - The DeviceFactory compiles with Fed9UUtils and Fed9UDeviceFactoryLib headers

File   : FecRingDevice
Date   : 28/02/2006
Tests  : done
Update :
  - Add longer timeout for fecScanRingBroadcast due to the long ring
  - Transform all the usleep in nanosleep and put a default value always on
    To disable it, you need to edit the file FecRingDevice.cc and then comment 
    the line #define TIMETOWAIT 10000000
  - The problems appears on the PCI FEC, need to be tested on the VME FEC 

File   : FecAccess, FecVmeRingDevice
Date   : 17/02/2006
Tests  : not done
Update :
  - Add the VME Block transfer in the code, switched off by default
  - To use it you need:
     * Update of the CAEN interface firmware (both PCI and VME)
     * FEC firmware version 1.5
     * HAL support > Version ver-03-09

File   : FecUsbDeviceDriver
Date   : 17/02/2006
Tests  : Not done
Update :
  - New release of the USB low level libraries

-------------------------------------------------------> Tag FecSoftwareV3_12_9

File   : FecRingDevice
Date   : 10/02/2006
Tests  : Done
Update :
  - Add a sleep in the fecScanRingBroadcast due to the increase of the performance
    of the computers
  - Add a timeout specific to FIFO Transmit running (writeFrame, setBlockDevices)

File   : FecRingDevice and Fec<Bus>RingDevice
Date   : 01/02/2006
Tests  : done
Update :
  - Add the firmware version as an attribut of the class
  - SR0 bit 0-15: is the status
    SR0 bit 16-31: number of words in the FIFO receive (firmware 1.4)
  - Due to the modification of the SR0 then the getFecRingSR0 is now 32 bits

File   : CCSTrigger
Date   : 11/01/2006
Tests  : done
Update :
  - Bug correction in the setSSID method

File   : FecAccess
Date   : 11/01/2006
Tests  : done
Update :
  - A new method FecAccess::scanFecRing ( fecHardwareId ) is added, only the FecRingDevice are created for the given FEC hardware ID

File   : CCSTrigger
Date   : 11/01/2006
Tests  : done
Update :
  - Bug correction in the constructor
  - Due to the different subdetectors used, the clear() method call is removed inside the constructor
  - The initialisation of the control register 0 depends of the SSID: for the time being for ECAL, the value is 0x60 (internal mode)
    For the other subdetectors, the value is 0B8 thanks to K. Kloukinas
  - Several methods to set and get the control and status register 0 has been added
  - The rest of the API is not changed

File   : CCSTrigger, APIConsoleDebugger
Date   : 10/01/2006
Tests  : not done
Update :
    - Correction in the config file for the VME FEC
    - CCSTrigger::setSSID and CCSTrigger::getSSID has been added
    - ProgramTest.exe has now an option to set and get the SSID

File   : FecSupervisor
Date   : 09/01/2006
Tests  : not done
Update :
   - Diagnositic system is now supported but need some changed to 
     completely handle it

File   : FecAccess, CCSTrigger
Date   : 09/01/2006
Tag    :
Tests  : not done
Update :
   - CCSTrigger class added
   - FecAccess manages now the CCSTrigger object

File   : FecAcess, FecAccessManager, <device>Access classes, FecProfiler
Date   : 21/12/2005
Tag    :
Tests  : not done
Update :
   - FecAccess::setBlockDevices methods are changed in order to include as output the list of 
     the exceptions during a download (std::list<FecExceptionHandler *>)
   - FecAccessManager::donwload/upload methods are changed in order to include as output the list of 
     the exceptions during a download/upload
   - <device>Access multiple frames methods are changed in order to include as output the list of 
     the exceptions
   - FecProfiler is modified in order to integrate the previous modifications
   - The FecSupervisor is not yet ported for that version but the integration of the TK diagnostic
     system is started and depending of the compilation (if diag libs are there or not) the display
     will be done through the diagnostic classes (diagbag class) or through the ErrorReportLogger

File   : All XML, HAL classes
Date   : 20/12/2005
Tag    : FecSoftwareV3_12_5
Tests  : not done
Update : 
   - Clean name space in header files. 
   - RULES: Do not add any using namespace in header files !
   - The FecSoftware is no more compatible with the version ver-03-06

File   : FecVmeRingDevice, FecAccess
Date   : 19/12/2005
Tag    : FecSoftwareV3_12_4
Tests  : not done, do not use it until test in 904
Update : 
   - Add the VME block transfer in the FecVmeRingDevice class
   - Add a method to enable or disable it in FecAccess (setFifoAccessMode)

File   : All classes (header files modification)
Date   : 16/12/2005
Tag    : 
Test   : done except for the GUIDebugger, TestControlLoop and Templates
Update :
   - Remove all the name space in the class header files
   - Clean the APIConsoleDebugger (printf replaced by cout/cerr)
   - Remove the compatibility with FecSoftware V1

File   : XML and Database classes, FecSupervisor, FecProfiler
Date   : 15/12/2005
Tag    :
Test   : not done
Update :
   - Database and XML parsing are based now on:
        * Partition name, FEC or FED hardware ID
        * Partition name and for FEC instance and class name
        * Partition name
   - Modify in consequences the FecSupervisor and FecProfiler to handle this new versions

File   : FecAccess
Date   : 01/12/2005
Tag    :
Test   : done
Update :
   - Add a method in FecAccess to retreive from an index the FEC hardware id: getFecHardwareId
   - Add a method in FecAccess to retreive from a FEC hardware id the index: getFecIndex. 
     For that method a new attribut is added in FecAccess: hash_map<std::string, keyType>fecHardwareIdMapIndex that is filled inside the VME methods in FecAccess

File   : FecAccessManager, FecSupervisor, DcuThread
Date   : 30/11/2005
Tag    :
Test   : done
Update :
   - FecAccessManager:
     * Add new method for the download of all APV or MUX or laserdriver or PLL from a device description: downloadValuesMultipleFrames ( ... )
     * Add new method for the downlaod of all PLL with an offset in the delay: downloadValuesMultipleFrames ( tscType8 delay ) ;
     * Add new method for the upload of the DCUs
     * Modify the method setColdPllReset to have the possibility to send the cold reset in multiple frames
   - Add new method in apvAccess to set only few registers in block transfer mode
   - Add new method in pllAcess to calculate the timing from coarse and fine delay and a method to return the frames when a delay is applied on it:
     * tscType8 getTimeValue ( ) ;
     * void getBlockWriteValues ( tscType8 delay, list<accessDeviceType> &vAccess )
   - The FecSupervisor integrate new messages for the block frames:
        // Multiple frames commands
        #define APV_SETALLVALUES         0x27  // Set all the APV with the description set in the message
        #define APV_ALLAPVCALIB          0x28  // Set all the APV with (CSEL, CRDV, LATENCY), description must be set in the message
        #define APV_ALLVPSP              0x29  // Set all the APV with VPSP, description must be set in the message
        #define LASERDRIVER_SETALLVALUES 0x30  // Set all the laserdriver with the description set in the message
        #define MUX_SETALLVALUES         0x31  // Set all the MUX with the description set in the message
        #define PLL_SETALLVALUES         0x32  // Set all the PLL with the description set in the message
        #define PLL_SETALLADDDELAY       0x33  // Add to all PLL, the delay set in the message
        #define PLL_ALLCOLDRESET         0x34  // Reset all the PLL with the cold procedure (same than the set cold reset in FecSupervisor)
        #define DCU_GETALLVALUES         0x35  // Retreive all the DCU values (message limited to 256kb)
   - The DcuThread use now the FecAccessManager directly to upload the DCUs (see previous methods)

File   : FecRingDevice
Date   : 29/11/2005
Tag    : 
Test   : Done
Update :
   - in the method setBlockDevices, if a device has an error on one register than the rest of the registers are not set.

File   : FecVmeRingDevice
Date   : 25/11/2005
Tag    : 
Test   : Done
Update :
   - The HAL item are now defined as offset and not absolute address, so the reference is the ring coupled with the VMEDevice
   - The items now is defined directly in the constructor to avoid time lost in reconstructing the HAL items (snprintf)

File   : FecVmeRingDevice
Date   : 23/11/2005 
Tag    : FecSoftwareV3_11_4
Test   : Done
Update :
   - Add the support for VME FEC on slot 21
   - Minor corrections in others classes for DEBUGMSGERROR

File   : apvAccess and muxAccess
Date   : 22/11/2005
Tag    :
Test   : Done
Update :
   - Correction in the frame to be sent for the multi-frames algorithm

File   : FecAccessManager, gohAccess, XML classes
Date   : 22/11/2005
Tag    : FecSoftwareV3_11_3
Test   : done
Update :
   - Add a new class for the gohAccess
   - XML schema
   - the gohDescription.cc is now added in CVS

File   : FecSupervisor, ErrorReportLogger
Date   : 16/11/2005
Tag    :
Test   :
Update :
   - Add an error report logger to avoid the multiplication of the messages LOG4C, console, etc.

File   : FecRingDevice
Date   : 14/11/2005
Tag    :
Test   :
Update :
   - Test on the channel on FecRingDevice::scanRingForI2CDevice is modified (minor change)

File   : FecSupervisor, FecAccessManager
Date   : 11/11/2005
Tag    : 
Test   : 
Update : 
   - Add an optional parameter in FecAccessManager::downloadValues and FecAccessManager::downloadValuesMultipleFrames to force the PLL reset init procedure
   - The parameter is now used in the FecSupervisor

File   : FecVmeRingDevice
Date   : 07/11/2005
Tag    : 
Test   :
Update :
   - In order to have the possibility to have several crates controlled by the same PC, the FecVmeRingDevice class is changed for:
      * busAdapter that becomes a hash_map of bus adapter
      * configurePlugNPlay method contains now a VMEDevice hash_map depending on the slot / crate
   - Following this change the keyType now contains one bit to have 

File   : FecRingDevice, pllAccess
Date   : 04/11/2005
Tag    : FecSoftwareV3_11_0
Test   : done
Update :
   - FecRingDevice timeout automatic adjustement:
      * Number of loop to wait before making a check of the FIFO transmit running
        The final timeout will be multiplied by 2 => it represents the time to send the frame when the toggle on the bit send is done until the FIFO transmit is empty
      * Number of loop to wait before making the read of the direct acknowledge
        The final timeout will be multiplied by 2 => it represents the time to wait the direct acknowledge so the time once the frame is sent until the time when the pending IRQ is arrived
      * Number of loop to wait before making a read frame or read force acknowledge
        The final timeout will be multiplied by 10 => it represents the time between the direct acknowledge is received until the the time when the data to FEC is arrived
   - pllAccess pllInit and pllReset time adjustement

File   : <device>Description
Date   : 03/11/2005
Tag    : FecSoftwareV3_10_3
Test   : done (valgrind)
Update :
  - Change the delete of the ParameterName in the description (delete done by static method contains in deviceDescription::deleteParameterNames)
  - Output of valgrind is:
==4913== 1 errors in context 1 of 2:
==4913== Syscall param ioctl(arg) contains uninitialised byte(s)
==4913==    at 0x1C0B17D4: ioctl (in /lib/tls/libc-2.3.2.so)
==4913==    by 0x1BE3795B: FecPciRingDevice::fecHardReset() (FecPciRingDevice.cc:500)
==4913==    by 0x1BDE6C8D: FecAccess::fecHardReset(unsigned) (FecAccess.cc:1120)
==4913==    by 0x8056727: uploadDetectedTrackerDevices(FecAccess*, FecAccessManager*, char*, char*, std::string, std::string) (FecProfiler.cc:850)
==4913==    by 0x805EFDE: main (FecProfiler.cc:2885)

==4913== 2 errors in context 2 of 2:
==4913== Source and destination overlap in memcpy(0x1C15A6B0, 0x1C15A6B0, 160)
==4913==    at 0x1B90083D: memcpy (mac_replace_strmem.c:394)
==4913==    by 0x1BAFCE2D: xercesc_2_3::XMLBuffer::set(unsigned short const*, unsigned) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BA79A4B: xercesc_2_3::IGXMLScanner::resolveSchemaGrammar(unsigned short const*, unsigned short const*) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BA7965C: xercesc_2_3::IGXMLScanner::scanRawAttrListforNameSpaces(xercesc_2_3::RefVectorOf<xercesc_2_3::KVStringPair> const*, int) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BA8073C: xercesc_2_3::IGXMLScanner::scanStartTagNS(bool&) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BA7E287: xercesc_2_3::IGXMLScanner::scanContent(bool) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BA7D34D: xercesc_2_3::IGXMLScanner::scanDocument(xercesc_2_3::InputSource const&) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BAAC3A3: xercesc_2_3::SAX2XMLReaderImpl::parse(xercesc_2_3::InputSource const&) (in /home/beaumont/xdaq/TrackerOnline/2005/TriDAS/daq/extern/xerces/linuxx86/lib/libxerces-c.so.23.0)
==4913==    by 0x1BE752BD: XMLFec::parseXMLBuffer() (XMLFec.cc:339)
==4913==    by 0x1BE77297: XMLFecDevice::getDevices() (XMLFecDevice.cc:242)
==4913==    by 0x1BE8A127: FecDeviceFactory::addFileName(std::string) (FecDeviceFactory.cc:122)
==4913==    by 0x1BE8D20B: FecFactory::addFileName(std::string) (FecFactory.cc:95)


File   : FecRingDevice
Date   : 03/11/2005
Tag    : no tag
Test   : done
Update : 
  - Bug correction for the management in the transaction number
  - Bug correction in the auto detection of the hardware: FecSupervisor & FecProfiler

File   : <device>Access
Date   : 02/11/2005
Tag    : FecSoftwareV3_10_2
Test   : done
Update :
  - Bug correction in FecProfiler
  - Add the fecHardwareId in the differents devices and update the FecAccessMananger

File   : FecFactory, PiaResetFactory, FecDeviceFactory
Date   : 27/10/2005
Tag    : No tag
Test   : not done
Update :
  - The FecHardwareId is a string. So a confusion exists in the methods related to the partition name.
  - The method names has been changed to have at the end the parameter passed, so for example getFecDeviceDescription becomes:
    * getFecDeviceDescription ( deviceVector ): for file
    * getFecDeviceDescriptionPartitionName ( deviceVector, std::string ): for partition name
    * getFecDeviceDescriptionFecHardwareId ( deviceVector, std::string ): for FEC hardware Id
    * getFecDeviceDescriptionInstance      ( deviceVector, unsigned int, std::string ): for instance and class name
  - Here the list of the methods that has been changed (suffix PartitionName, Instance, FecHardwareId is added):
    * FecDeviceFactory::getPartitionVersion
    * FecDeviceFactory::getFecDeviceDescription
    * FecDeviceFactory::setPartitionVersion
    * FecDeviceFactory::setFecDeviceDescriptions
    * FecDeviceFactory::getDcuDescriptions
    * PiaResetFactory::getPartitionVersion
    * PiaResetFactory::setPartitionVersion
    * PiaResetFactory::getPiaResetDescriptions
    * PiaResetFactory::setPiaResetDescriptions
    * FecFactory::getFecDevicePartitionVersion
    * FecFactory::getPiaResetPartitionVersion
    * FecFactory::setFecDevicePartitionVersionP
    * FecFactory::setPiaResetPartitionVersion
    * FecFactory::setFecDevicePiaDescriptions
  - For backward compatibility, the following methods are kept (only for partition name calls):
    * FecFactory::setFecDevicePartitionVersion calls FecFactory::setFecDevicePartitionVersionPartitionName
    * FecFactory::setPiaResetPartitionVersion calls FecFactory::setPiaResetPartitionVersionPartitionName
    * FecFactory::getFecDevicePartitionVersion calls FecFactory::getFecDevicePartitionVersionPartitionName
    * FecFactory::getPiaResetPartitionVersion calls FecFactory::getPiaResetPartitionVersionPartitionName
    * FecDeviceFactory::getFecDeviceDescriptions is the same than FecDeviceFactory::getFecDeviceDescriptionsPartitionName
    * FecDeviceFactory::setFecDeviceDescriptions is the same than FecDeviceFactory::setFecDeviceDescriptionsPartitionName
    * FecDeviceFactory::getDcuDescriptions is the same than FecDeviceFactory::getDcuDescriptionsPartitionName
    * PiaResetFactory::getPiaResetDescriptions is the same than PiaResetFactory::getPiaResetDescriptionsPartitionName
    * PiaResetFactory::setPiaResetDescriptions is the same than PiaResetFactory::setPiaResetDescriptionsPartitionName
 
File   : FecExceptionHandler
Date   : 24/10/2005
Tag    : FecSoftwareV3_9_3
Test   : done
Update :
  - display in the message the ring of the FEC used

File   : FecRingDevice class
Date   : 24/10/2005
Tag    : no tag
Test   : not done
Update :
  - add a new method to measure the timeout delay automatically, this method is not applyed for the time being

File   : FecAccess class
Date   : 24/10/2005
Tag    : no tag
Test   : not done
Update :
  - add a new method to get the FEC type managed by the FecAccess: getFecBusType ( )

File   : pllAccessNew
Date   : 24/10/2005
Tag    : not tag
Test   : not done
Update :
  - PLL init has been changed for the new procedure

File   : FecVmeRingDevice
Date   : 21/10/2005
Tag    :
Test   : done
Update :
  - Bug correction, the FecVmeRingDevice::setIRQ is not handle by the system for the time being so this method do nothing now.

File   : pllAccessNew
Date   : 14/10/2005, 19/10/2005, 20/10/2005
Tag    : not tag
Test   : not done
Update :
  - the pllAccess class must be changed according to remarks from S. Marchioro
  - A new file has been added but not tested and not used for the time being.

File   : FecRingDevice
Date   : 12/10/2005
Tag    : FecSoftwareV3_9_0
Test   : done
Update :
   - FecRingDevice::writeFrame: the loop to wait the direct acknowledge also wait on the DATA TO FEC not only on the pending IRQ
   - This modification is temporary and must be corrected in the hardware
   - This modification only work in i2c force acknowledge mode to avoid high modification in the software
     If the force acknowledge is not set, the software runs as before so waiting on pending IRQ (not on the data to FEC).
   - This modification only works on CCU 25 not on old CCUs

File   : dcuAccess
Date   : 12/10/2005
Tag    : no tag
Test   : 
Update :
   - Multiple frame read out are added in this class (dcuAccess::setDcuValuesMultipleFrames) and added to the FecAccessManager
   - The modification is not operational and it is not used for the time being

File   : FecRingDevice
Date   : 10/10/2005
Tag    : no tag
Test   : done
Update :
   - FecRingDevice::readFrame: the check of the size of the direct acknowledge is done before reading the rest of the frames (W. Beaumont)

File   : pllAccess, FecAccessManager
Date   : 07/10/2005
Tag    : no tag
Test   : done
Update :
   - Add a new method to send multiple frames for the PLL

File   : ccuDevice, CCUDescription 
Date   : 06/10/2005
Tag    : FecSoftwareV3_8_3
Test   : done
Update : 
   - ccuDevice class is now deprecated and replaced by CCUDescription
   - CCUDescription class integrate the possibility to specify the input/output from the configuration database
     with XML parameters described in the ccuDefinition.h file.

File   : TestControlLoop
Date   : 06/10/2005
Tag    :
Test   : done
Update :
   - Modification of the makefile and test on version 3

File   : FecRingDevice, FecAccessManager, PiaResetAccess, <device>Access
Date   : 05/10/2005
Tag    : 
Test   : tested on one module
Update : 
   - FecRingDevice::setBlockDevices updated to add the PIA resets in multiple frames
   - FecAccessManager::resetPiaModulesMultipleFrames addedfor PIA resets in multiple frames
   - PiaResetAccess:::resetPiaModulesMultipleFrame added for PIA resets in multiple frames
   - apvAccess::getBlockWriteValues added for multiple frames (only for write)
   - laserdriverAccess::getBlockWriteValues added for multiple frames (only for write)
   - muxAccess::getBlockWriteValues added for multiple frames (only for write)
   - philipsAccess::getBlockWriteValues added for multiple frames (only for write)
   - FecAccess::setBlockDevices updated to add the PIA resets in multiple frames

File   : FecVmeRingDevice
Date   : 05/10/2005
Tag    : 
Update : getFecHardwareId not tested
   - add the readout of the FEC hardware ID.
   - Now all the software must be adapted in order to request data from the DB through the FecHardwareId when a VME FEC is used

File   : FecAccess
Date   : 05/10/2005
Tag    : 
Test   : done
Update : 
    - FecAccess::setFecRingDevice updated for bug correction

File   : FecSupervisor
Date   : 03/10/2005
Tag    : FecSoftwareV3_7_8
Test   : done
Update :
   - FecSupervisor::setPiaReset bug correction

File   : FecAccessManager, FecAccess, FecRingDevice
Date   : 03/10/2005
Tag    : 
Test   : done
Update :
   - Change the methods for the multiple frames: FecRingDevice::setBlockDevices, FecAccessManager::downloadMultipleFrameValues, FecAccess::setBlockDevices

File   : FecRingDevice
Date   : 30/09/2005
Tag    :
Test   : done
Update :
   - Remove all the ulseep and the loop to wait the frames
   - The processor is always used in that case
   - Please note that in case of TIMEOUT, it must be configured depending of your system: FecRingDevice.cc file, TIMEOUT parameter

File   : FecRingDevice, <device>Access, FecAccessManager
Date   : 30/09/2005
Tag    : 
Test   : done
Update :
   - Implementation of the multiple frames methods
   - Add a new method setBlockDevices ( ) in <device>Access classes
   - Modification of the FecRingDevice, FecAccessMananger and FecAccess class
     to add the algorithm
   - Please note that the modification are not used for the time being

File   : FecRingDevice
Date   : 29/09/2005
Tag    : FecSoftwareV3_7_6
Test   : done
Update :
   -Improvements of the timeout in the FecRingDevice class

File   : FecRingRegisters & FecExceptionHandler
Date   : 28/09/2005
Tag    :
Test   : done
  - The FecExceptionHandler class will not anymore handle an object from FecRingRegisters a copy of the registers.
  - The registers are put and get from FecExceptionHandler through FecRingRegisters

File   : FecPciRingDevice
Date   : 27/09/2005
Tag    : FecSoftwareV3_7_5
Test   : done
Update :
   - Remove all the IRQ calls from that class => can be the solution for the US setup

File   : FecRingDevice
Date   : 27/09/2005
Tag    :
Test   : done
Update :
   - Change the writeFrame when a reconfiguration is done
   - Change the GUIDebugger to handle the fecReset method (changed to fecRingReset)

File   : FecDeviceFactory and PiaResetFactory
Date   : 21/09/2005
Tag    : FecSoftwareV3_6_2
Test   : done
Update :
  - The methods for FecDeviceFactory::getFecDeviceDescriptions, FecDeviceFactory::getDcuDescriptions, PiaResetFactory::getPiaResetDescription take the vector as a parameter of the method and not anymore by the return of the method. This new feature allows to make a persistent layer between the version and the related classes.

File   : FecRingDevice
Date   : 20/09/2005
Tag    : FecSoftware3_6_0 
Test   : done
Update :
  - The clear of the IRQ is done now on the beginning of the write frame when the check of the SR0 is done and in the readFrame method
  - Add the historic of the frame when an errors happends, the tag DEBUGFRAMES must be set in the FecRingDevice.cc file

File   : FecPciDeviceDriver
Date   : 20/09/2005
Tag    : No tag
Test   : done
Update :
  - Remove the IRQ manager from the FEC PCI device driver. This part is not anymore mandory for FecSoftwareV3_0

File   : FecRingDevice, apvAccess, FecAccess, FecAccessManager
Date   : 20/09/2005
Tag    : No Tag
Test   : Not done
Update :
  - Add methods for multiple frames but the methods are not enable
  - Must be debugged

File   : FecDeviceFactory and PiaResetFactory
Date   : 18/09/2005
Tag    : FecSoftwareV3_5_1
Test   : ok
Update :
  - Add a new method getFecDeviceDescriptions() and getPiaResetDescriptions() to retreive the devices from file
  - the default version values when you retreive from DB with Partition name or FEC hardware id or instance, className is
    now 0.0 so the current version from the database. These methods must be used with the database, for file use the previous one.

File   : FecUsbDeviceDriver, FecUsbRingDevice
Date   : 18/09/2005
Tag    : No Tag
Test   : not done
Update :
  - Update of the FecUsbDeviceDriver
  - FecUsbRingDevice::setFecRingCR0Real is removed

File   : FecRingDevice, FecAccess, PiaResetAccess
Date   : 16/09/2005
Tag    : FecSoftwareV3_5_0
Test   : done 
Update :
  - FecRingDevice::setInitPiaChannel just initialise the GCR of the PIA channel
  - Modify the PiaResetAccess to set the data direction register in the constructor

File   : FecRingDevice
Date   : 16/09/2005
Tag    : FecSoftwareV3_4_0
Test   : 
Update :
  - The method FecRingDevice::writeFrame has been updated to wait on the pending IRQ for the direct ack.
  - The method FecRingDevice::readFrame has been updated to wait on the data to FEC for the read answer or the force ack.
  - The timing in this two methods has been updated

File   : HashTable.h
Date   : 07/09/2005
Test   : done
Update :
  - The vector of deviceDescriptions and piaResetDescriptions has been removed from the HashTable.h and move to a new file deviceType.h

File   : FecRingDevice and FecPciDeviceDriver
Date   : 02/09/2005
Test   : done
Update :
  - The device driver and the FecRingDevice class manage now to check the FIFO transmit before set the SEND bit in the FEC CR0 (error 72)
  - FecSoftwareV1_0: in case of problem of the FIFO transmit bit in the FEC CR0, a dump will be put in the /var/log/messages
  - FecSoftwareV3_0: in case of problem of the FIFO transmit bit in the FEC CR0, the exception will contain the frame and the register
  - ASAP the error used by the FecSoftwareV3_0 must be the same the error code used in the FecSoftware (to be done)

File   : FecRingRegisters (new class), FecExceptionHandler, FecRingDevice
Date   : 02/09/2005
Test   : done
Update : 
  - FecRingRegisters class is added to store in case of error all the registers values (FEC,CCU,i2c and pia channel)
  - This class has statics methods for the display and the decoding of the registers
  - FecExceptionHandler has now two more optional parameters:
     * FecRingRegisters object
     * Frame array
  - FecRingDevice::getFecRingRegisters: to readout all the registers from FEC/CCU/i2c and pia channel
  - ProgramTest, FecSupervisor and FecProfiler through FecAccessManager class can display the values of the registers in case of error through the precompilation flags DEBUGGETREGISTERS
  - FecAccessManager is also configured to display the errors

File   : FecRingDevice class
Date   : 24/08/2005
Test   : done
Update :
  - Management of the transaction number is now done correctly with a busy and a timeout

File   : All DbAccess classes
Date   : 24/08/2005
Test   : done
Update :
  - Upload in nextMajor.0, currentMajor.0 or currentMajor.nextMinor

File   : FecRingDevice, FecVmeRingDevice, FecPciRingDevice, FecUsbRingDevice and FecInterfaceRingDevice
Date   : 19/08/1005
Test   : done
Update :
  - Inheritance between the different classes has been updated
  - FecVmeRingDevice, FecPciRingDevice, FecUsbRingDevice inherits from FecRingDevice
  - The class FecInterfaceRing is no more used

File   : dcuAccess, laserdriverAccess
Date   : 15/08/2005
Test   : done by W. Beaumont
Update :
  - Add the check of the SEU in dcuAccess and laserdriverAccess

File   : FecAccess, FecRingDevice, FecUsbRingDevice, FecInterfaceRingDevice, cmdDescription, tscTypes, FecErrorDescription, 
Date   : 12/07/2005
Test   : not done in V3, V2 on WB computer works OK.
Update : Integration of FecUsb classes, addition of array read/write to FEC fifos.

File   : XML classes, DB classes, 
Date   : 07/07/2005
Tests  : done but FEC hardware ID dev. not operational
Update : 
  * See the FecDeviceFatory and PiaResetFactory comments, now you can request the data from database through 3 parameters: 
    - IP, instance, className
    - FEC hardware ID
    - Partition name
  * Now the class DeviceFactory are no more dependent on the IP, instance, classname but when you request a parameter you need to set one the previous parameters
  * See API for more information, note the interface DeviceFactoryInterface was also changed.

File   : dcuAccess, pllAccess
Date   : 04/07/2005
Tests  : not done
Update :
  * dcuAccess to handle more carefully the reset of the DCU
  * pllAccess new method to read the getGoingStatus and getSeuStatus (W. Beaumont)

File   : database and XML
Date   : 04/07/2005
Tests  : done
Update :
  * Upgrade of all classes

File   : deviceDescription and deviceDefinition
Date   : 24/06/2005
Tests  : done
Update :
  * These two classes integrate now the FEC hardware id available on VME FEC
  * The factory/DB classes will be modified in order to integrate the possibility to retreive the data coming to a specific FEC depending on the FEC ID number 

File   : FecFactory PiaResetFactory FecDeviceFactory
Date   : 28/02/2005
Tests  : done
Update :
  * FecDeviceFactory is implemented to manage FEC devices
  * New class PiaResetFactory is implemented to manage PIA reset
  * New class FecFactory is implemented to manage both PIA and FEC devices
    It inherits from PiaResetFactory and FecDeviceFactory
  * All static methods implemented in FecDeviceFactory is moved to FecFactory.

File   : XML classes
Date   : 14/02/2005
Version: CVS
Test   : 
Update :
  * Since the 01/10/2004, the IP number, instance and class name is added to the <device>Description. 
    So the MemBufOuputSource class use now this parameters in place of the specification in the constructor. 
  * Same for the PIA reset

File   : FecSupervisor/APIXMLDebugger/GUIDebugger
Date   : 14/02/2005
Version: CVS
Test   : 
Update :
  * Address for DCU on CCU 0x78 is removed (TEC people)

File   : FecDevice
Date   : 11/02/2005
Version: CVS
Tests  : not done
Update :
  * The TAG SCANCCUATSTARTING is removed in order to avoid problem in the reconfiguration of the ring
  * Each CCU (as default parameter) is connected and initialised each time a CCU is seen

File   : DeviceFactory
Date   : 07/02/2005
Tests  : not done
Update :
  * DeviceFactory inherits from FecDeviceFactory

File   : FecAccess
Date   : 07/02/2005
Version: CVS
Tests  : not done
Update :
  * Add a new method to scan for i2c devices only for one ring

File   : FecDeviceFactory
Date   : 07/02/2005
Version: CVS
Tests  : not done
Update :
  * DeviceFactory calls is replaced by FecDeviceFactory
  * A new DeviceFactory class is implemented with the inheritance from the FecDeviceFactory, Fed9UDeviceFactory and a FedFecConnectionDeviceFactory (not already developped)
 
File   : FecDevce
Date   : 02/02/2005
Version: CVS
Tests  : done
Update :
  * The FecRelease method do 2 times the release

File   : FecAccess
Date   : 25/01/2005
Version: CVS
Tests  : done
Update :
  * Add new method for the getCcuList in order to retreive only the CCUs for a given FEC/Ring

File   : FecDevice / FecDeviceEmulation
Date   : 25/01/2005
Version: CVS
Tests  : done
Update :
  * A new exception handler and new error have been added in order to detect if a ring has several CCUs with the same addresses. 
  * FecDeviceEmulation was also updated.

File   : FecDeviceEmulation
Date   : 23/01/2005
Version: CVS
Tests  : done for some tests
Update : 
  * This class is used to simulate the anwser of the a FEC
  * Note all the tests have been done for example getCcuCRE but can be implemented

File   : XMLFedFecDevice
Date   : 20/01/2005
Version: CVS
Tests  : not done
Update :
  * Add a new class for the FEC/FED relation for the DeviceFactory class

File   : plxReset and fecReset
Date   : 20/01/2005
Version: CVS
Tests  : done
Update:
  * Add methods to empty the FIFO return and transmit
  * The reset can generate parasitic frame in the FIFO receive at leat
    so both reset now is implemented in order to empty all the FIFOs
  * A new call is done also to refresh the buffer used for the conversion
    between 32 bits (optical FEC) to 16 bits for the decoding of the frame.

File   : DbAccess
Version: CVS
Date   : 
Tests  : not done, compilation ok
Update :
  * Add all the stuff for the FED database interface

File   : FecDevice
Version: CVS
Tests  : done
Update :
  * Add a test for duplicate CCU addresses

File   : <device>Description
Date   : 
Version: CVS
Tests  : Problem in TrackerCommandSender messages, when method display is used
         a segmentation fault appears
Update : 
  * Add instance, classname, IP number in the XML description

File   : FecSupervisor
Version: CVS
Date   :
Tests  : Problem in TrackerCommandSender messages, see <deviec>Description 
         changes
Update :
  * Remove all old i2o messages and go to the message with the class 
    TrackerCommandSender and TrackerCommandListener

File   : DeviceFactory
Date   : 01/10/2004
Version: CVS
Tests  : Not finished, the database packages must be upgraded
Update :
  * Add a new class to retreive information concerning DCU from mainly the 
    database

File   : FecSoftware
Date   : 22/09/2004
Version: CVS
Tests  : Ok
Update :
  * A new class for access database or XML file has been introduced: 
    DeviceFactory class.
  * This class provides all tools for upload into file or database and tools
    to download from files or from database.
  * The FecProfiler use now this class
  * FecSupervisor will be ported with it soon

File   : FecDevice, FecAccess, APIConsoleDebugger, APIXMLDebugger
Date   : 13/09/2004
Version: CVS
Tests  : Ok
Update :
  * FecDevice::fecScanOrderCcu25 => change the size of the temporary frame
  * The method FecDevice::scanRingForI2CDevice check now the channel (if 
    different 0) to test the corresponding device. 
  * Same for FecAccess::scanRingForI2CDevice. 
  * Note that the APIXMLDebugger and APIConsoleDebugger are changed due to 
    the FecAccess, FecDevice::scanRingForI2CDevice

File   : FecSupervisor
Date   : 06/08/2004
Version: CVS
Tests  : Ok
Update :
  * Add the possibility to set with a new export parameters (i2cSpeed_) the values 
    of the i2c channels. Only new connections or after a reset will be affected by this change.

File   : XML and database files
Date   : 06/08/2004
Version: CVS
Tests  : ok
Updates:
  - use valgrind tools to remove all memory leak  

File   : description access classes
Date   : 12/07/2004
Version: CVS
Tests  : ok
Updates:
  - Add the chips for the preshower:
      * pace, kchip and delta
  - Modify the following files in order to take care about the previous 
    changes:
      * FecAccessManager class, MemParseHandlers class, FecDeviceMemParseHandlers class, HashTable.h, MemBufOutputSource class, MemParseHandlers class

File   : device description classes
Date   : 05/07/2004
Version: CVS
Tests  : Ok
Updates:
  - Add parameter names in order to have direct constructor between
    XML and the description of the devices
  - All descriptions have now two more methods:
     * constructor with parameter names (see class ParameterName)
     * static parameterDescriptionNameType *getParameterDescription )
  - A new class for the description of a parameter is added: ParameterDescription

File   : FecSupervisor
Date   : 06/04/2004
Version: CVS
Tests  : Ok
Updates:
  - Assuming that one FecSupervisor/Partition (trigger partition), 
    create new functions on database and add new export params (mostly for database):
      void createDatabaseAccess ( ) ;  => create a database access
      void getVersionPartition ( ) ;   => get a version for a partition
      void setVersionPartition ( ) ;   => set a version for a partition
  - add a DcuFilter in DcuThread (DCUFILTER precompilation flag must be known)
  - new i2o messages are ok (not all tested) but the old messages are not removed

File   : FecAccessManager, DcuThread, device descriptions
Date   : 02/06/2004
Version: CVS
Tests  : Ok
Updates:
  - Use now log4cplus messages both for FecAccessManager and DcuThread
  - All XDAQ_NOTE are removed and replaced by a cout/cerr given the fact
    that a DEBUGMSGERROR must be set.
  - descriptions of devices (APV, PLL, MUX, LD, DOH) have some constructors
    changed for optionnal parameters

File   : FecSupervisor
Date   : 02/06/2004
Version: CVS
Tests  : Ok
Updates:
  - New i2o messages are implemented
  - This change need files coming from the Tracker that are downloaded by
    the script on AFS:
       * TrackerCommandSender.h, TrackerCommandListener.h
       * xdaqTrackerDefs.h, xdaqTrackerFecDefs.h, xdaqTrackerCommand.h
       

File   : FecSupervisor
Date   : 26/05/2004
Version: CVS
Tests  : Ok
Updates:
  - Use now log4cplus messages
  - All XDAQ_NOTE are removed and replaced by a cout/cerr given the fact
    that a DEBUGMSGERROR must be set.

File   : XDAQ/Database
Date   : 10/05/2004
Version: CVS
Tests  : Ok
Updates: 
  - Add the management of the XDAQ instance id in DB classes and XML classes
  - DbAccess::setTid ( )

File   : deviceAccess class
Date   : 07/05/2004
Version: CVS
Tests  : Ok
Updates:
  - Remove a Tracker dependance coming from deviceAccess:getDeviceType

File   : XML classes
Date   : 03/05/2004
Version: CVS
Tests  : Ok
Updates:
  - Add class FecDeviceMemParseHandlers PiaResetMemParseHandlers
    to split devices and PIA reset
  - Update software for namespace and includes

File   : DeviceDescription, FecAccessManager
Date   : 21/04/2004
Version: CVS
Tests  : Ok
Updates:
  - The methods clone and display are now virtual and must be
    implemented in the description which inherits from 
    deviceDescription
  - The method FecAccessManager::downloadValue (deviceDescription)
    is removed to fit with the virtual methods of deviceDescription

File   : FecDevice
Date   : 14/04/2004
Version: CVS
Tests  : Ok
Updates:
  - Add the functionnality on the trigger (thanks to W. Bialas)

File   : XML and database access
Date   : 16/04/2004
Version: CVS
Tests  : Ok
Updates:
  - Add a DbFecAccess and a DbFedAccess class and change
    all XML files.
  - Modify the FecSupervisor, DcuThread for this new accesses

File   : FecDevice
Date   : 14/04/2004
Version: CVS
Tests  : Ok
Updates:
  - fecReset () add a timeout until the SR0 is correct

File   : FecDevice
Date   : 07/04/2004
Version: CVS
Tests  : Ok
Updates:
  - Add a reset on ring B FecDevice::fecResetRingB

File   : FecDevice
Date   : 24/03/2004
Version: CVS
Tests  : Ok
Updates:
  - Modify the FecDevice:getCcuList to return the CCU addresses in the correct
    order.

----------------- stable0204 available on 22/03/2004 ----------------------------------

File   : DbAccess, FecSupervisor
Date   : 11/03/2004
Version: stable0204
Tests  : Ok
Updates:
  - Use the new version of the DbAccess class that check dynamically the
    environment variables to find login/password and URL for the database:
      * DBCONFLOGIN, DBCONFPASSWD, DBCONFPATH
    If they are not existing then try to check if it is defined directly in
    the FecHeader.linux

----------------- stable0204 available on 29/02/2004 ----------------------------------

File   : FEC C++ API
Date   : 23/02/2004
Version: CVS
Tests  : ok
Updates:
  - Add  a new method in dcuAccess:setChannelEnabled to fix which channel must
    be read and which one must be ignored.

File   : FEC C++ API
Date   : 05/02/2004
Version: CVS
Tests  : ok 
Updates:
  - Add an exception in FecAccess:scanForFECs in case of one of the device
    driver does not have the same version with the C++ FEC software API
  - Re-numbered the errors of the FEC software for the FEC C++ API

File   : FEC C++ API
Date   : 03/02/2004
Version: CVS
Tests  : ok 
Updates:
  - Add a new methods for DohAccess::getValues in order to have the correct
    device type

File   : FEC C++ API
Date   : 28/01/2004
Version: CVS
Tests  : ok 
Updates:
  - Add a new constructor in class dcuDescription for empty DCU

File   : FEC C++ API
Date   : 28/01/2004
Version: CVS
Tests  : 
Updates:
  - Add 2 methods for the run number in DB Access
  - Not yet implemented in the FecSupervisor

File   : FEC C++ API and FecSupervisor
Date   : 27/01/2004
Version: CVS
Tests  : not tested and must be tested !
Updates:
  - Add a method for cold reset in pllAccess methods (pllAccess::pllInit)
  - Add a new parameter in the FecSupervisor to use the cold reset
  - Add the corresponding method in FecAccessManager

File   : FEC C++ API
Date   : 08/01/2004
Version: CVS
Tests  :
Updates:
  - Modification for the compabitity with version 2.0 of TriDAS.
  - The parser was modified and the IMS class is no more compatible.
  - XDAQ_NOTE also disapear due the new version of TriDAS
  - This version is only for transition and must be update for 
    the log4Cplus and new exceptions. Note also that the i2o message
    coming from the event builder must be also updated
  - The version is also compatible with the version 1.2 from TriDAS

File   : FEC C++ API
Date   : 19/12/2003
Version: CVS
Tests  :
Updates:
  - Add method for automatic database configuration
  - Reordering some commands in FecDevice class

File   : FEC C++ API
Date   : 17/12/2003
Version: CVS
Tests  :
Updates:
  - Add the support for Xerces 2.3
  - XMLFec::setDevices use the functionnalities of Xerces 2.3
  - XMLFec::setDevices ( XMLFecDevice, char *, boolean ) has been 
    removed

File   : FecDevice class
Date   : 10/12/2003
Version: CVS
Tests  : 
Updates:
  - Add in writeFrame method a check of the SR0

File   : FecHeader.linux
Date   : 08/12/2003
Version: greater than stable0703
Tests  : ok
Updates:
  - The FecHeader.linux integrate now the device driver definition
  - If will be now installed in FecSoftware/FecHeader.linux
  - The configuration (configure.sh, example of FecHeader.linux, oracle
    environment files) are now defined in FecSoftware/config

File   : FEC C++ API
Date   : 05/12/2003
Version: CVS
Tests  : 
Updates:
  - Add a new method to retreive the user counter of the device driver in
    FecDevice class (FecDevice::getUserCounter())

File   : FEC C++ API
Date   : 04/12/2003
Version: CVS
Tests  : 
Updates:
  - Methods are added in order to read a word into the FEC FIFOs (reveice,
    return and transmit)

File   : FEC C++ API
Date   : 04/12/2003
Version: CVS
Tests  : 
Updates:
  - The methods getFecRegister and setFecRegister have changed due the merge
    between the 16 bits device driver and the 32 bits device driver
  - The interface has not changed
  - Use preferably getFecCRX/getFecSRX and setFecCRX

File   : FEC C++ API
Date   : 01/12/2003
Version: CVS
Tests  : not done
Updates:
  - Add method to generate a new token (FEC release) in the FecAccess
    class (this methods already exists in FecDevice).

File   : FEC C++ API
Date   : 25/11/2003
Version: CVS
Tests  :
Updates:
  - Redefinition of types
  - DD_TYPE_FEC_DATAXX is defined now in the C++ API (tscTypes.h)
  - In the next future, all DD_TYPE_FEC_DATAXX will be replaced
    by new types based on __uXX in all C++ API files but the compatibility
    will stay due the definition in tscTypes.h
  - Perhaps one of the guideline of the software is to replace all your
    DD_TYPE_FEC_DATAXX by the new type. A new comment in this file will
    explain how to do it.

File   : FecSupervisor
Date   : 24/11/2003
Version: CVS
Tests  : Not performed
Updates:
  - Add a new i2o message for the DOH

File   : FEC C++ API
Date   : 17/11/2003
Version: CVS
Tests  : Not performed
Updates:
  - Add a new class DohAccess for the digital optical link that is 
    set step by step (see DohAccess.h for the value of the step).
  - The FecAccessManager class make the difference between the DOH
    and the laserdriver
  - Note that the XML is not change to set a doh declare a laserdriver
    and the software will make the difference on the i2c address

File   : FEC C++ API
Date   : 17/11/2003
Version: CVS
Tests  : Ok
Updates:
  - Add in FecDevice class a check of the version of the device driver
  - Add in FecDevice a timeout when the ring is scanned for CCU 
    (FecDevice::fecScanOrderCcu25). Add an error about code consistency.

File   : FEC C++ API
Date   : 12/11/2003
Version: CVS
Tests  : Ok
Updates:
  - Add a new constructor in MemBufOutputSource in order to dump PIA and
    device in files.

File   : ProgramTest, FecProfiler
Date   : 7/11/2003
Version: CVS
Tests  : Ok
Updates:
  - The file changes in ProgramTest is now log in 
    ThirdParty/APIConsoleDebugger/README.TXT
  - The file changes in FecProfiler is now log in 
    ThirdParty/APIXMLDebugger/README.TXT

File   : FEC C++ API
Date   : 06/11/2003
Version: CVS
Tests  : Ok
Updates:
  - FecAccess: add a new constructor that scans FEC and CCU (boolean)

File   : configure.sh
Date   : 31/10/2003
Version: CVS, stable0703
Tests  : Ok
Updates:
   - Bug correction when the XML file name to be downloaded (PIA
     or devices) are unknown, the FecSupervisor CVS version will
     not crashed. 
   - This bug is always available on version stable0703

File   : configure.sh
Date   : 31/10/2003
Version: stable1102, stable0703, CVS
Tests  : ok pour installation, the compilation works fine
Updates:
  - Modification to support XDAQ core 1.2 and 1.3
  - Updates and installation
  - for version stable1102 and XDAQ core 1.3, the script only compile 
    standalone programs

File   : configure.sh, Makefiles, ProgramTest, FecProfiler
Date   : 26/10/2003
Version: CVS
Tests  : Ok
Updates:
  - Add a new directory for ProgramTest in:
      ThridParty/APIConsoleDebugger with new Makefile
  - Add a new directory for FecProfiler in:
      ThridParty/APIXMLDebugger with new Makefile
  - Change the configure.sh for these new directories
  - This version keep the independancy from XDAQ for standalone programs
  - Update makefiles in order to use the same one for both XDAQ version 1.3 and 1.2

File   : configure.sh and Makefiles
Date   : 06/10/2003
Version: CVS
Tests  : not performed
Updates:
  - Add makefiles for the XDAQ core version 1.3 in FecSupervisor directory
  - Manage the XDAQ core version 1.3 with Makefiles in the script
  - The Makefiles for ProgramTest and FecProfiler are not yet developped

File   : FEC C++ API
Date   : 06/10/2003
Version: CVS
Tests  : Ok
Updates:
  - FecDevice bug correction due to misplaced #define

File   : FEC C++ API for database
Date   : 29/09/2003
Version: CVS
Tests  : Not performed on 29/09
Updates:
  - DbAccess interface for getDatabaseVersion
  - Add comments in some classes

File   : DcuThread and FecSupervisor
Date   : 26/09/2003
Version: CVS
Tests  : ok
Updates:
  - Add a suspended boolean to suspend the DcuThread
  - Synchronise the database and 
  - DcuThread updates for DCU database access
  - Add SOAP url for the destination of DcuThread SOAP message
  - It subsits errors when multiple database access is done

File   : FecSupervisor
Date   : 12/09/2003
Version: CVS
Tests  : 
Updates: 
  - Change the PIA error message when no pia are declared
  - Add into the Dcu Thread the possibility to send a SOAP message with
    DCU values

File   : ProgramTest
Date   : 11/09/2003
Version: CVS
Tests  : Alarms cannot be tested but software functionnalities is there
Updates: 
  - Separate in the ProgramTest the CCU alarms and the PIA interrupts

File   : Threads example
Date   : 11/09/2003
Version: CVS
Tests  :
Updates:
  - Remove the thread example (Threads class ThreadsSO class Threads.mak).

File   : FEC C++ API
Date   : 10/09/2003
Version: CVS
Tests  : Alarms cannot be tested but software functionnalities is there
Updates: 
  - Modify methods for CCU alarms in class FecDevice, FecAccess 
    (setCcuAlarmsEnable, waitForAnyCcuAlarms)
  - Modify methods for PIA interrupts in class FecDevice, FecAccess
    (setPiaEnableInterrupt, setPiaClearInterrupts)
  - The two previous functionnalities must be separated, it is not
    the same interrupts / alarms
  - The ProgramTest must be modified in the same direction to have
    CCU alarms in a method and PIA interrupts in other one.

File   : FecSupervisor, DcuThread
Date   : 09/09/2003
Version: CVS
Tests  :
Updates: Test the DCU thread in beam test, it seems to be ok
  - Remove a bug in the DCU thread SOAP message
  - Remove a bug in the DCU thread itself for the upload
  - Upload in the database or file is corrected
  - Modify the SOAP DCU message in order to send values even if
    the DCU thread are not running.
  - Add clear messages for upload.

File   : FEC C++ API
Date   : 08/09/2003
Version: CVS and stable703 (available)
Tests  : A bug stays in this version in the XML parser
Updates:
  - Bug correction for XMLFecPiaReset class.

File   : FecSupervisor
Date   : 08/09/2003
Version: CVS
Tests  : Ok
Updates:
  - Add in plugin the dependency with the dcu thread (just a display)

File   : FEC C++ API and ProgramTest
Date   : 29/08/2003
Version: CVS
Tests  : Alarms cannot be tested but software functionnalities is there
Updates: 
  - Add methods for CCU alarms in class FecDevice, FecAccess 
    (setCcuAlarmsEnable, waitForAnyCcuAlarms, setPiaClearInterrupts)
  - Add example in ProgramTest method waitCCUAlarms

File   : FEC C++ API
Date   : 19/08/2003
Version: stable0703 (available)
Tests  : done
Updates:
  - Bug correction for the memory channel in class FecDevice. Thanks to
    Michal Pietrusinski.

File   : FEC C++ API
Date   : 18/08/2003
Version: stable0703 (available)
Updates:
  - Correct a bug in the class FecDevice for multiple bytes memory access
    See FecDevice::writeIntoMemory. (Thanks to Michal Pietrusinski)

File   : ProgramTest
Date   : 18/08/2003
Version: stable0703 (available)
Updates:
  - The command -piaReset can be combined with -tracker or -device
  - add an options -piaChannel specific channel for pia
    Examples:
      ./ProgramTest -ccu 50 -piachannel 31 -piaReset -channel 10 -tracker

File   : configure.sh
Date   : 18/07/2003
Version: available on web site
Tests  : 
Updates:
  - Add GUIDebugger in the distribution for compilation
  - Only CVS version can be compiled with the GUIDebugger

File   : GUIDebugger
Date   : 18/07/2003
Version: CVS
Tests  : Memory / PIA but not I2C
Updates:
  - Add in ThirdParty/GUIDebugger the distribution of QT FEC debugger
  - Need QT 3.0

File   : FEC C++ API
Date   : 17/07/2003
Version: CVS
Tests  : Performed on optical FEC
Updates:
  - Add method deviceDescription::setAccessKey

----------------- stable0703 available on 10/07/2003 ----------------------------------
File   : FEC C++ API
Date   : 07/07/2003
Version:
Tests  : Performed on optical FEC
Updates:
  - Add method FecDevice::isChannelEnable

----------------- stable0703 available on 08/07/2003 ----------------------------------

File   : configure.sh
Date   : 08/07/2003
Version:
Updates:
  - Add an update command for versions (configure.sh update)
  - The script now build 3 directories:
      * bin for binaries with ProgramTest, debugger.bin, FecProfiler
      * lib for general libraries (libDeviceAccess.so, libfec_glue.a)
      * xdaqLib for XDAQ shared libraries (libFecSupervisor.so, libThreads.so)

File   : FEC C++ API
Date   : 07/07/2003
Version:
Tests  : Compilation ok with G++ 3.2.2
Updates:
  - Add the support for G++ 3.X

File   : ProgramTest
Date   : 03/07/2003
Version: 
Tests  : Test performed and ok for command -filecommand
Updates:
  - Quit the program on wrong parameters
  - Add a -reset function to reset PLX and FEC
  - Add the correct managment of the CCU frame size (two bytes length) for
    filecommand.
  - Add a size limitation on the memory channel (2^16 addresses) 

File   : configure.sh
Date   : 02/07/2003
Version: 
Updates: 
  - change some tests

----------------- stable0703 available on 01/07/2003 ----------------------------------

File   : FEC C++ API / ProgramTest
Date   : 19/06/2003
Version: 4.1 => unstable
Tests  : I2C / PIA channels / FecSupervisor / ProgramTest ok
         need to test the memory channel with the optical/electrical device driver
         need to test the optical device driver
Updates:
  - Add a new constructor in FecDevice in order to init or not the 
    FEC before using it.
  - Add a new constructor in FecAccess in order to init or not all
    the FEC devices opened (only used in the ProgramTest)
  - Modify the ProgramTest to retreive values for status without change
    the FEC

Package: FEC C++ API / FecSupervisor
Date   : 04/06/03
Version: 4.0 => unstable
Tests  : not fully tested
Updates:
  - add memoryAccess class and tools in FecAccess class
  - FecSupervisor:
      * DCUTHREAD implemented
      * Configure state:
          - Pia reset
          - if DCUTHREAD => create DCU thread (scan ring for CCUs, scan ring for DCU devices)
          - download
  - Remove DCUX tags in configure.sh and in the API

Package: FecDevice
Date   : 26/05/03
Version: 3.3 => stable
Tests  : not done
Updates:
  - Add methods to retreive errors and status fro, the device driver

Package: C++ API
Date   : 21/05/03
Version: 3.3 => stable
Tests  : ok
Updates:
  - Add getStatus () in the FecAccess / FecDevice class
  - Must be replaced by a list of status
  - Add the devicedriver status
  - Add the ignore error or manage error in the driver

Package: C++ API
Date   : 21/05/03
Version: 3.3 => stable
Tests  : ok
Updates:
  - Add the possibity to parse a XML file from http

Package: FEC C++ API
Date   : 17/05/03
Version: 4.0 => unstable
Tests  : not fully tested
Updates: 
  - g++ 3.0 compatible ("using namespace std;" added) => tested on g++ 3.0
    WARNING: Take care about distribution g++ 3.2.3 20030316 (Debian prerelease), 
    I have problem with std::hash_map that is included in ext/hash_map in this
    prerelease.
  - exceptions:
      * ccuChannelAccessError and fecHardAccessError are replaced FecExceptionHandler
      * Database exceptions and XML exceptions are also managed with FecExceptionHandler 
  - FecDevice: memory channel: added and ok (multiple read/write and single read/write)
  - SOAP DCU message updated and working
  - Add a validating parser but TAKE CARE if you use it:
	  * Add the rowset line with xsi lines.
	  * Order of the device, must be:
		  APV25, APVMUX, DCU, LASERDRIVER, PHILIPS, PLL
      See example in FecSupervisor/xml/test03.xml. The obsolete files test01.xml and 
      test02.xml are removed.

Package: configure.sh script
Date   : 17/05/03
Version: 2.1 => stable
Tests  : ok
Updates:
  - Add a DCU address hardcoded by hardware (see mfHeader.linux in 
    FecSupervisor/src/linux/x86)
  - Due to the new XDAQ web site, the script use the binary put in the FecControl 
    web site. If you want to use the orignal version, go to 
    http://xdaq.web.cern.ch/xdaq/Software/ and follow the link 
    Core Framework > Full Packaged Release > File:Full Packaged ReleaseV1.2.bin
  - Add a parameter for the validating parser (yes/no, default yes)
    
Package: ProgramTest
Date   : 17/05/03
Version: 3.3 => stable
Tests  : ok
Updates:
  - Add command "-scanring" to find all the CCUs on the FEC/RING specified
  - Add command "-scanringdevice" to find all the Tracker I2C devices. Note that you
    can change it, by editing the file and change the method testScanRingDevice
    with an array of the base address and the access mode.
  - Add command "-scanringedevice" to find all the I2C devices by scanning the 
    address (0x00 -> 0xFF) on each channel
  - All this command are now implemented in the FecAccess / FecDevice class, see
    FecDevice::scanRingForI2CDevice, FecAccess::scanRingForI2CDevice, FecAccess::scanForFECs

Package: FecSupervisor
Date   : 14/05/03
Version: 2.1 => stable
Tests  : ok
Updates:
  - Add run control information service (IMS) API
