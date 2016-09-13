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
#ifndef ERRORCODEDEFINITION_H
#define ERRORCODEDEFINITION_H

// In this file take place all the definition of the system 
// and the severity error
// Each subsystem must be implement a "System"ErrorDescription
// with the error allowed and the definition of the subsystem
// Each error must be attached to System/SubSystem/Severity/ErrorCode
// path
// This file is for the time being attached to the Tracker but you
// can define more system and more severity error if needed. The
// FEC group will added it in the repository.

/** Error definition
 */
typedef unsigned long errorType ;

// -------------------------------------------------------------
// Error severity
#define FATALERRORCODE         0x0  // Fatal error: no possibility to continue
#define CRITICALERRORCODE      0x1  // An hardware error occurs: grave error, action should be taken
#define ERRORCODE              0x2  // An error occurs: error all the functions should raised this error
#define WARNINGCODE            0x3  // Warning
#define INFORMATIONCODE        0x4  // Information

// -------------------------------------------------------------
// Range error code
#define MAXDDERRORCODE            199  // Between 0 and 199
#define MAXFECSOFTWAREERRORCODE   799  // Between 200 and 799
#define MAXFECSUPERVISORERRORCODE 999  // Between 800 and 999
#define MAXDATABASEERRORCODE     1199  // Between 1000 and 1199
#define MAXXMLERRORCODE          1499  // Between 1200 and 1499
#define SOFTWAREERRORCODE        1600  // 1600

// -------------------------------------------------------------
// System definition

// -------------------------------------------------------------------------------------
// -------------------------------- Low level errors

// Cannot open the device driver
// index - FEC slot
#define TSCFEC_BADCOMPATIBILITY 201
#define TSCFEC_BADCOMPATIBILITY_MSG (std::string)"Bad compabitlity in the FEC software"

// FEC device driver not opened (or close) and somebody try to called a method
// or any error that is related to board unreachable
// index - FEC slot
#define TSCFEC_FECDDNOTOPEN 202
#define TSCFEC_FECDDNOTOPEN_MSG (std::string)"The FEC device driver is not opened and a method is called"

// FEC status register 0 is not correct
// index - FEC slot
// SR0 - FEC status register 0
#define TSCFEC_SR0NOTNOMINAL 210
#define TSCFEC_SR0NOTNOMINAL_MSG (std::string)"The FEC status register 0 is not correct"

// Invalid parameter in index 
// index - FEC, Ring, CCU, channel (if it's an i2c access, the error returned if TSCFEC_I2CREGISTERACCESS)
// value - valid parameter
#define TSCFEC_FECPARAMETERNOTMANAGED 221
#define TSCFEC_FECPARAMETERNOTMANAGED_MSG (std::string)"Invalid parameter in index"

// FEC register access error (index). If index if just a FEC, it is a FEC register error. If index is FEC,RING,CCU, it is a CCU register access. If index is FEC,RING,CCU,CHANNEL, it is a channel register access. The message must also specified if the access (read/write).
// index - FEC, Ring, CCU, channel (if it's an i2c access, the error returned if TSCFEC_I2CREGISTERACCESS)
#define TSCFEC_REGISTERACCESS 222
#define TSCFEC_REGISTERACCESS_MSG (std::string)"Register access error"

// Unable to perform an operation on the FEC/Ring/CCU ...
// index - FEC, Ring, CCU, channel - depend of the error level
#define TSCFEC_UNABLETOPERFORMOPERATION 223
#define TSCFEC_UNABLETOPERFORMOPERATION_MSG (std::string)"Unable to perform operation"

// Invalid operation given (index, operation)
// index - FEC, Ring, CCU, channel (if it's an i2c access, the error returned if TSCFEC_I2CREGISTERACCESS)
// operation - invalid operation
#define TSCFEC_INVALIDOPERATION 224
#define TSCFEC_INVALIDOPERATION_MSG (std::string)"Invalid operation given"

// FIFO cannot be emptied
#define TSCFEC_FIFONOTEMPTIED 225
#define TSCFEC_FIFONOTEMPTIED_MSG (std::string)"FIFO cannot be emptied"

// I2C channel access error for a device register (index). The message must specified the access (read/write) and the mode (RAL, EXTENDED, NORMAL).
// index - FEC, Ring, CCU, channel, address of the device
// #define TSCFEC_I2CREGISTERACCESS 226
// #define TSCFEC_I2CREGISTERACCESS_MSG (std::string)"Cannot access a device on i2c bus"

// Problem in the coherence of a frame (bad transaction number, too much data)
#define TSCFEC_PROBLEMINFRAME 227
#define TSCFEC_PROBLEMINFRAME_MSG (std::string)"Frame is corrupted"

// Bad direct acknowledge
#define TSCFEC_BADDIRECTACKNOWLEDGE 251
#define TSCFEC_BADDIRECTACKNOWLEDGE_MSG (std::string)"Invalid direct acknowledge received after sending a frame"

// Force acknowledge or read answer problem
#define TSCFEC_BADFACKORREADANS 252
#define TSCFEC_BADFACKORREADANS_MSG (std::string)"Invalid force acknowledge or read answer"

// Problem in the CCU addresses, duplicate addresses found
#define TSCFEC_CCUADDRESSDUPLICATEDERROR 260
#define TSCFEC_CCUADDRESSDUPLICATED_MSG (std::string)"Several CCU have the same addresses"

// Problem in accessing the USB hardware 
#define TSCFEC_USBACCESSPROBLEM 300
#define TSCFEC_USBACCESSPROBLEM_MSG (std::string)"Problem in accessing the FEC using USB bus"

// Problem in accessing the USB hardware 
#define TSCFEC_VMEACCESSPROBLEM 300
#define TSCFEC_VMEACCESSPROBLEM_MSG (std::string)"Problem in accessing the FEC using VME bus"

// -----------------------------------------------------------------------------------------------------
// ------------------------------------ FecSupervisor problems for trigger and hardware access

// Problem with the FecSupervisor: Problem of creation of the FecAccessManager
#define XDAQFEC_HARDWAREACCESSERROR 800
#define XDAQFEC_HARDWAREACCESSERROR_MSG (std::string)"Hardware access was not created, no command can be applied"

// Problem with the FecSupervisor: Did not find the files for the VME FEC
#define XDAQFEC_VMEFILEMISSING 801
#define XDAQFEC_VMEFILEMISSING_MSG (std::string)"Did not find the VME file name"

// Problem with the FecSupervisor: No FEC bus type was specified
#define XDAQFEC_NOFECBUSTYPE 802
#define XDAQFEC_NOFECBUSTYPE_MSG (std::string)"Please specify a FEC bus type"

// Problem with the FecSupervisor: No VME FEC board detected on this setup
#define XDAQFEC_NOFECDETECTED 803
#define XDAQFEC_NOFECDETECTED_MSG (std::string)"No VME FEC board detected on this setup"

// Problem with the FecSupervisor: Trigger system not operational
// QPLL in error
#define XDAQFEC_QPLLERROR     804
#define XDAQFEC_QPLLERROR_MSG (std::string)"QPLL Error" ;

// QPLL not locked
#define XDAQFEC_QPLLNOTLOCKED 805
#define XDAQFEC_QPLLNOTLOCKED_MSG (std::string)"QPLL not locked" ;

// TTCRx not ready
#define XDAQFEC_TTCRXNOTREADY 806
#define XDAQFEC_TTCRXNOTREADY_MSG (std::string)"TTCRx not ready" ;

// Error in comparison
#define XDAQFEC_ERRORINCOMPARISON 807
#define XDAQFEC_ERRORINCOMPARISON_MSG (std::string)"Error in comparison"

// Error in the PLL mode
#define XDAQFEC_PLLOPERATIONAL 808
#define XDAQFEC_PLLOPERATIONAL_MSG (std::string)"Cannot set the PLL in operational mode"

// Invalid operation given 
#define XDAQFEC_INVALIDOPERATION 810
#define XDAQFEC_INVALIDOPERATION_MSG (std::string)"Invalid operation given"

// -----------------------------------------------------------------------------------------------------
// ------------------------------------ HAL problems

// Hal subsystem errors
#define HAL_NOSUCHITEMEXCEPTION 900
#define HAL_NOSUCHITEMEXCEPTION_MSG "Bad item name for the access (HAL::NoSuchItemException)"

#define HAL_BUSADAPTEREXCEPTION 901
#define HAL_BUSADAPTEREXCEPTION_MSG "Hardware problem access (HAL::BusAdapterException)"

#define HAL_ILLEGALOPERATIONEXCEPTION 902
#define HAL_ILLEGALOPERATIONEXCEPTION_MSG "Illegal operation in HAL (HAL::IllegalOperationException)"

#define HAL_VERIFYEXCEPTION 903
#define HAL_VERIFYEXCEPTION_MSG "The value wrote is different than the value read (HAL::VerifyException)"

#define HAL_MASKBOUNDARYEXCEPTION 904
#define HAL_MASKBOUNDARYEXCEPTION_MSG "The value Wrote is greater than the mask set in the configuration (HAL::MaskBoundaryException)"

#define HAL_ADDRESSOUTOFLIMITSEXCEPTION 905
#define HAL_ADDRESSOUTOFLIMITSEXCEPTION_MSG "Offset outside of the limits (AddressOutOfLimitsException)"

// -----------------------------------------------------------------------------------------------------
// ------------------------------------------ DB errors 1000 -> 1200

// no data available
#define DB_NOTCONNECTED 1000
#define DB_NOTCONNECTED_MSG (std::string)"Database is not connected or support is not compiled"

// no version available
#define DB_NOVERSIONAVAILABLE 1001
#define DB_NOVERSIONAVAILABLE_MSG (std::string)"No version available"

// several versions exist for the same partition / data
#define DB_SEVERALVERSIONSERROR 1002
#define DB_SEVERALVERSIONSERROR_MSG (std::string)"Several versions exist for the same partition / data"

// PL/SQL procedure called failed
#define DB_PLSQLCALLFAILED 1003
#define DB_PLSQLCALLFAILED_MSG (std::string)"PL/SQL called failed, statement execution result does not correspond to what it is expected"

// PL/SQL exception raised
#define DB_PLSQLEXCEPTIONRAISED 1004
#define DB_PLSQLEXCEPTIONRAISED_MSG (std::string)"PL/SQL exception raised"

// Invalid operation given 
#define DB_INVALIDOPERATION 1005
#define DB_INVALIDOPERATION_MSG (std::string)"Invalid operation given"

// No data in the database
#define DB_NODATAAVAILABLE 1006
#define DB_NODATAAVAILABLE_MSG (std::string)"No data available in database"

// -----------------------------------------------------------------------------------------------------
// ------------------------------------------ XML errors 1200 -> 1499

// Invalid operation given 
#define XML_INVALIDOPERATION 1200
#define XML_INVALIDOPERATION_MSG (std::string)"Invalid operation given"

// XML parsing error
#define XML_PARSINGERROR 1201
#define XML_PARSINGERROR_MSG (std::string)"Error during XML parsing"

// XML parsing error for SAX
#define XML_SAXEXCEPTION 1220 
#define XML_SAXEXCEPTION_MSG (std::string)"Error during XML parsing, SAX exception raised"

// XML parsing error for SAX
#define XML_DOMEXCEPTION 1221 
#define XML_DOMEXCEPTION_MSG (std::string)"Error during XML parsing, DOM exception raised"

// XML parsing error for SAX
#define XML_XMLEXCEPTION 1222
#define XML_XMLEXCEPTION_MSG (std::string)"Error during XML parsing, XML exception raised"

// XML parsing error for SAX
#define XML_BUFFEREMPTY 1223
#define XML_BUFFEREMPTY_MSG (std::string)"XML buffer empty"

// XML allocation problem
#define XML_ALLOCATIONPROBLEM 1224
#define XML_ALLOCATIONPROBLEM_MSG (std::string)"Allocation problem"

// Invalid PSU name
#define XML_INVALIDPSUNAME 1210
#define XML_INVALIDPSUNAME_MSG (std::string)"Invalid PSU name"

// Invalid ring file
#define XML_INVALIDFILE 1211
#define XML_INVALIDFILE_MSG (std::string)"XML file provided is not correct"

// -----------------------------------------------------------------------------------------------------
// ------------------------------------------ Software issues 1500

// Problem in software
#define CODECONSISTENCYERROR 1500
#define CODECONSISTENCYERROR_MSG (std::string)"Software problem"

// No data available
#define NODATAAVAILABLE 1501
#define NODATAAVAILABLE_MSG (std::string)"No description available"

// Problem in file
#define FILEPROBLEMERROR 1502
#define FILEPROBLEMERROR_MSG (std::string)"File access problem"

// Duplicated information
#define DUPLICATEDINFORMATION 1503
#define DUPLICATEDINFORMATION_MSG (std::string)"Information duplicated"

// Too much errors during upload or download 
#define TOOMUCHERROR 1504
#define TOOMUCHERROR_MSG (std::string("Too much errors") ;

// ---------------------------------------------------
// XML_NOFECDEVICEFOUND a remplacer par NODATAAVAILABLE
// TSCFEC_CODECONSISTENCYERROR a remplacer par CODECONSISTENCYERROR
// FATALERRORCODE a remplacer par SOFTWAREERRORCODE
// TSCFEC_FECPARAMETERNOTMANAGED n'a pas de sens a voir
// TSCFEC_I2CREGISTERACCESS / TSCFEC_REGISTERACCESS

#endif
