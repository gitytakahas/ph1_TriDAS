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
#include <iostream>

using std::cout;
using std::endl;

#include <stdio.h>    // fopen snprintf
#include <string.h>   // strcmp

#include "FecVmeRingDevice.h"

#include "APIAccess.h"

#define DEFAULTSOCKETPORT 1300

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       main functions                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 */
FecAccess *createFecAccess ( int argc, char **argv, int *cnt, unsigned int fecAddress ) {

  FecAccess *fecAccess = FecAccess::createFecAccess ( argc, argv, cnt, false ) ;

  if (!fecAccess) {
    std::cerr << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
    exit (EXIT_FAILURE) ; ; 
  }

  setFecType (fecAccess->getFecBusType()) ;

  return (fecAccess) ;
}

/** Display the help
 */
void helpMe ( char *programName ) {

      printf ( "NAME\n") ;
      printf ( "\t%s - tests for all FEC / CCU / I2C / PIA / MEMORY functionnalities\n", programName) ;

      printf ( "\nSYNOPSIS\n") ;
      printf ( "\t%s [ OPTIONS ] COMMAND [ ARGUMENTS ]\n", programName) ;
#ifdef FECSOFTWAREV2_0
      printf ( "\n\tList of the options:\n\n") ;
      std::cerr << "\t[-pci]\tPCI FEC (default value)" << std::endl ;
      std::cerr << "\t[-vmesbs | -vmecaenpci | -vmecaenusb] [filename]\tVME FEC" << std::endl ;
      std::cerr << "\t\tfilename: configuration file for the VME FEC" << std::endl ;
      std::cerr << "\t\t          by default the file used is FecSoftwareV2_0/ThirdParty/VMEConsoleDebugger/config/FecAddressTable.dat" << std::endl ;
      std::cerr << "\t[-usb]\tUSB FEC" << std::endl ;
      std::cerr << std::endl ;
#endif
      printf ( "\n\tList of the commands:\n\n") ;
      printf ( "\t%s -help\n", programName) ;
      printf ( "\t%s\n", programName) ;
      printf ( "\t%s -ssid [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | TOTEM | RESERVED2]\n", programName) ;
      printf ( "\t%s [-noBroadcast] -scanccu\n", programName) ;
      printf ( "\t%s [-noBroadcast] -status\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] [ccu x] -statusCCU\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -reset\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -resetA\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -resetB\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] -uFecReload\n", programName) ;
      printf ( "\t%s  -crateReset\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] -testCrateReset\n", programName) ;
      printf ( "\t%s  -createPnpFecHardwareList\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -fectest\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] -fectemp\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -recover\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -spuriousreset\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -parasitic\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -scantrackerdevice\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -scantrackerdcu\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -scanringedevice\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -redundancy [<FEC-Input-Ouput>] [<CCU address-Input-Ouput>]\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] -autoredundancy\n", programName) ;
      printf ( "\t%s [-loop [n[-t]]] [-fec x] [-ring x] [-channel x] -autoTrackerRedundancy [<DCU read mode>]\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -bypassed [CCU address]\n", programName) ;
      printf ( "\t%s [-errorCounter [file]] [-loop [n[-t]]] [-fec x] [-ring x] [-ccu x] [-piaChannel x] -pia\n", programName) ;
      printf ( "\t%s [-errorCounter [file]] [-loop [n[-t]] [-fec x] [-ring x] [-ccu x] [-piaChannel x] -piaReset [value-delayActiveReset-intervalDelayReset]\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] [-ccu x] [-channel x] -memory\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] [-ccu x] [-channel x] -memoryPerf [number]\n", programName) ;
      printf ( "\t%s [-fack] [-loop [n[-t]] [-Tmode t] [-fec x] [-ring x] [-ccu x] [-channel x[-xx]] [-deviceAddress x] -i2c\n", programName) ;
      printf ( "\t%s [-errorCounter [file]] [-loop [n]] [-noFack] [-fec x] [-ring x] [-ccu x] [-channel x[-x]] -device <list of device type>\n", programName) ;      
      printf ( "\t%s [-errorCounter [file]] [-loop [n]] [-noFack] [-fec x] [-ring x] [-ccu x] [-channel x[-x]] -trackeri2ctest <list of device type>\n", programName) ;
      
      printf ( "\t%s [-fec x] [-ring x] [-ccu x] [-channel x[-x]] -calibrateDOH\n", programName) ;

      printf ( "\t%s [-fec x] [-ring x] [-ccu x] [-channel x[-x]] -pllCheckLock\n", programName) ;

      printf ( "\t%s [-fec x] [-ring x] [-ccu x] [-channel x[-x]] -calibrateI2CSpeed\n", programName) ;

      printf ( "\t%s [-fec x] [-ring x] -fileCommand file\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -socket [port]\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -key index\n", programName) ;
      printf ( "\t%s [-fec x] [-ring x] -SR0 sr0\n", programName) ;
      printf ( "\t%s [-invrecvclk]\t Force the inversion of the polarity of receiver clock\n",programName) ;

      printf ( "\t%s -fecver\tDisplay version registers of all FECs\n",programName) ;
      printf ( "\t%s -fectrigger [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | RESERVED1 | RESERVED2]\n",programName) ;
      printf ( "\t%s -resetCCSTTCRx [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | RESERVED1 | RESERVED2]\n",programName) ;

      printf ( "\nDESCRIPTION\n") ;
      printf ( "\tAll the values specified by x/xx must be given in hexadecimal (with or not\n\t\"0x\" before).\n") ;
      printf ( "\tFor the others values, there must be specied as a radix 10 integer value.\n") ;
      printf ( "\tThe character case of the options/commands are not important.\n") ;
      printf ( "\tPolicy:\t[] means optionnal parameters.\n") ;

      printf ( "\nOPTIONS\n") ;
      printf ( "\n\tList of the options:\n\n") ;
      printf ( "\t[-fec x]\tSpecify the FEC slot in hexadecimal (default = 0x0)\n") ;
      printf ( "\t[-ring x]\tSpecify the ring slot in hexadecimal (default = 0x0)\n") ;
      printf ( "\t[-ccu x]\tSpecify the CCU address in hexadecimal (default = 0x7F)\n") ;
      printf ( "\t[-piaChannel x]\tSpecify the PIA channel in hexadecimal (default = 0x30)\n") ;
      printf ( "\t[-channel x]\tSpecify the channel in hexadecimal (default = 0x1B)\n") ;
      printf ( "\t[-channel x-xx]\tSpecify a range of channels in hexadecimal\n") ;
      printf ( "\t[-deviceAddress x]\n\t\t\tSpecify the I2C address in hexadecimal (default = 0x20)\n") ;
      printf ( "\t[-Tmode t]\tSpecify the device type: APV, MUX, PLL, DCU, LASERDRIVER, DOH\n") ;
      printf ( "\t[-fack]\t\tForce acknowledge used (default)\n") ;
      printf ( "\t[-noFack]\tDo not use the force acknowledge. Note that some requests\n\t\t\tdue to the FEC bandwith can have problems.\n") ;
      printf ( "\t[-loop]\t\tThe program will made an infinite loop (see below)\n") ;
      printf ( "\t[-loop n]\tMake n loop. If the value is negative or no value are\n\t\t\tspecified, the program will made an infinite loop.\n") ;
      printf ( "\t[-loop n-t]\tSame than the previous command but wait between each\n\t\t\tcommand t microseconds.\n") ;
      printf ( "\t[-errorCounter [file]]\n\t\t\tDisplay error counters and FEC/CCU status with the error.\n\t\t\tIf a file is specified, dump all the messages to the file.\n\t\t\tIf no file are specified the output is on stderr\n") ;
      printf ("\t[-nogetchar]\n\t\t\tNo pause during the command\n") ;

      printf ( "\nCOMMANDS\n") ;

      printf ( "\tWith no command\n\t\tOpen the device driver slot 0 and display\n") ;
      printf ( "\t\tthe control register 0 and the device driver status for all the FECs available on the system\n") ;
      printf ( "\t\tNo option nor command are needed\n") ;
      printf ( "\n") ;

      printf ( "\t-help\n\t\tDisplay this help.\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\t-ssid [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | TOTEM | RESERVED2]\n") ;
      printf ( "\t\tSet the SSID for the corresponding system\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\t-fectrigger [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | TOTEM | RESERVED2]\n") ;
      printf ( "\t\tSet the SSID for the corresponding system and display all the registers in a table\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\t-fectrigger [RESERVED0 | TRACKER | ECAL | PRESHOWER | PIXELS | RPC | TOTEM | RESERVED2]\n") ;
      printf ( "\t\tSet the SSID for the corresponding system and display all the registers in a table\n") ;
      printf ( "\t\tand toggle the re-sync on the bit 7 on CCS CR0\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\t-reset\n\t\tHard and FEC reset.\n") ;
      printf ( "\t\tIf you use the loop the command only reset the FEC\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]] [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-resetA\n\t\tFEC reset on ring A.\n") ;
      printf ( "\t\tIf you use the loop the command only reset the FEC\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]] [-fec x] [-ring x]\n") ;
      printf ( "\n") ;
      
      printf ( "\t-resetB\n\t\tFEC reset on ring B.\n") ;
      printf ( "\t\tIf you use the loop the command only reset the FEC\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]] [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-uFecReload\n\t\tReload the firmwares on the different FPGA (mFEC, trigger FPGA, VME FPGA)\n") ;
      printf ( "\t\tOptions: [-fec x]\n") ;
      printf ( "\n") ;

      printf ( "\t-crateReset\n\t\tReset the FEC crate and reload the mFEC firmware.\n") ;
      printf ( "\n") ;

      printf ( "\t-testCrateReset\n\t\tReset the FEC crate.\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]]\n") ;
      printf ( "\n") ;

      std::cout << "\t-createPnpFecHardwareList\n\t\tCreate a file with the FEC hardware in order to have the pnp working," 
		<< "please append this file at the official file in " 
		<< ((getenv("ENV_TRACKER_DAQ")==NULL) ? "FecSoftwareV3_0" : getenv("ENV_TRACKER_DAQ")) 
		<< "/config/FecHardwareIdList.dat" << std::endl ;


      printf ( "\t-fectest\n\t\tMake some test on the FEC.\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]]\n") ;
      printf ( "\n") ;

      printf ( "\t-fectemp\n\t\tRead the two sensors from the VME FEC.\n") ;
      printf ( "\t\tOptions: [-loop [n[-t]]] [-fec x]\n") ;
      printf ( "\n") ;

      printf ( "\t-scanccu\n\t\tFind all the device driver loaded for FECs\n") ;
      printf ( "\t\tScan the ring for each FEC to find all the CCUs.\n") ;
      printf ( "\t\tIf -noBroadcast is used (false by default), the CCU broadcast mode is not used.\n") ;
      printf ( "\t\tOptions: [-noBroadcast]\n") ;
      printf ( "\n"); 

      printf ( "\t-status\n\t\tFind all the device driver loaded for FECs\n") ;
      printf ( "\t\tDisplay all the FEC control and status registers\n") ;
      printf ( "\t\tDisplay all the device driver status\n") ;
      printf ( "\t\tScan the ring for each FEC to find all the CCUs.\n") ;
      printf ( "\t\tDisplay all the CCC control and status registers\n") ;
      printf ( "\t\tIf -noBroadcast is used (false by default), the CCU broadcast mode is not used.\n") ;
      printf ( "\t\tOptions: [-noBroadcast]\n") ;
      printf ( "\n") ;

      printf ( "\t-statusCCU\n\t\tDisplay registers for a given FEC / Ring / CCU\n") ;
      printf ( "\t\tDisplay the FEC control and status registers\n") ;
      printf ( "\t\tDisplay the CCU control and status registers\n") ;
      printf ( "\t\t[-fec x] [-ring x] [-ccu x]\n") ;
      printf ( "\n") ;

      printf ( "\t-calibrateDOH\n\t\tSet the DOH bias channel per channel until the limit of the value\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x] [-ccu x] [-channel x] [-deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-pllCheckLock\n\t\tCheck all the PLL (automatic detection) and display the results including the LOCK status including reset if needed\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x] [-ccu x] [-channel x] [-deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-calibrateI2CSpeed\n\t\tCheck for each speed of the i2c channel the access to each devices") ;
      printf ( "\t\tDisplay the differences when the speed is increased\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x] [-ccu x] [-channel x] [-deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-recover\n\t\tTry to recover the ring\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-spuriousreset\n\t\tCheck the spurious reset on the ring\n") ;
      printf ( "\t\terrors, the FIFO values is there are not empty\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-parasitic\n\t\tCheck the FEC status register 0 and display\n") ;
      printf ( "\t\terrors, the FIFO values is there are not empty\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-scantrackerdevice\n\t\tFor a FEC, find all the CCUs on the ring\n") ;
      printf ( "\t\tFind all the I2C Tracker devices connected\n") ;
      printf ( "\t\tDevice available: APVs (all poss.), MUX, PLL, DCU, LASERDRIVER, DOH\n") ;
      printf ( "\t\tNote that the program try to read the base address\n\t\t(in the right mode) but not all the device registers are accessed.\n") ;      
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-scantrackerdcu\n\t\tFor a FEC, find all the CCUs on the ring\n") ;
      printf ( "\t\tFind all the DCU devices connected\n") ;
      printf ( "\t\tNote that the program try to read the base address\n\t\t(in the right mode) but not all the device registers are accessed.\n") ;      
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-scanringedevice\n\t\tFor a FEC, find all the CCUs on the ring\n") ;
      printf ( "\t\tFind all the I2C registers connected (address from 0x00 to 0x7F)\n") ;
      printf ( "\t\tNote that the program try to read from the address\n\t\t(in i2c normal mode) but not all the device registers are accessed.\n") ;      
      printf ( "\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-waitCCUAlarms\n\t\tWait for CCU alarms.\n") ;
      printf ( "\t\tWait for any alarm on CCU. The PIA interrupt must be hardware\n") ;
      printf ( "\t\treliable.\n") ;
      printf ( "\t\tThis program is blocked until a ^C is done.\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-loop [n[-t]]] [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-redundancy [<FEC-Input-Ouput>] [<CCU address-Input-Ouput>]\n\t\tTest ring A-B for the FEC and/or CCU specified\n") ;
      printf ( "\t\tUse only with CCU 25, <CCU address> is the CCU address, and <Input>\n") ;
      printf ( "\t\tor <Output> can be A or B\n") ;
      printf ( "\t\tOption: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-autoredundancy\n\t\tTest ring A-B for the all the CCUs found in the ring\n") ;
      printf ( "\t\tOption: [-loop [n[-t]]] [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-autoTrackerRedundancy [DCU read more]\n\t\tTest ring A-B for the all the CCUs found in the ring with the Tracker definition of the\n") ;
      printf ( "\t\t dummy CCU\n") ;
      cout << "\t\t<DCU read mode> is are:" << std::endl ;
      cout << "\t\t\t1: if you want to read all DCUs (default)" << std::endl ;
      cout << "\t\t\t2: if you want to read all DCUs except the dummy CCU" << std::endl ;
      cout << "\t\t\t3: if you don't want to read the DCUs" << std::endl ;
      cout << "\t\tThe DCU channel can be specified at this point with option -channel" << std::endl ;
      printf ( "\t\tOption: [-loop [n[-t]]] [-fec x] [-ring x] [-channel]\n") ;
      printf ( "\n") ;

      printf ( "\t-bypassed [CCU address]\n\t\tBypass one CCU\n") ;
      printf ( "\t\tUse only with CCU 25, <CCU address> is the CCU address\n") ;
      printf ( "\t\tOption: [-fec x] [-ring x]\n") ;
      printf ( "\t\tBug: only work with one CCU address for the moment\n") ;
      printf ( "\n") ;

      printf ( "\t-pia\n\t\tTest the PIA channel\n") ;
      printf ( "\t\tUse only with CCU 25, channels can be 0x30 to 0x33\n\t\tfor 8 bits long register.\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-loop [n[-t]]] [-fec x] [-ring x]\n\t\t\t [-ccu x] [-piaChannel x]\n") ;
      printf ( "\n") ;

      printf ( "\t-piaReset [value-delayActiveReset-intervalDelayReset]\n\t\tTest the PIA reset.\n\t\tSet the value given bit per bit, wait for delayActiveReset,\n\t\tSet 0xFF and wait for intervalDelayReset (default 0xFF-10-10000).\n") ;
      printf ( "\t\tThe value must be given in hexadecimal and the delayActiveReset and\n\t\tintervalDelayReset must be given in microseconds (decimal)\n") ;
      printf ( "\t\tUse only with CCU 25, channels can be 0x30 to 0x33\n\t\tfor 8 bits long register.\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-loop [n[-t]]] [-fec x] [-ring x]\n\t\t\t [-ccu x] [-piaChannel x]\n") ;
      printf ( "\n") ;

      printf ( "\t-waitPiaInterrupt [PIA channel]\n\t\tWait for PIA interrupts.\n") ;
      printf ( "\t\tWait for any alarm on CCU. The PIA interrupt must be hardware\n") ;
      printf ( "\t\treliable.\n") ;
      printf ( "\t\tIf the PIA channel is not specified, then all PIA channels are used.\n") ;
      printf ( "\t\tThis program is blocked until a ^C is done.\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-loop [n[-t]]] [-fec x] [-ring x]\n") ;

      printf ( "\t-memory\n\t\tTest the memory channel\n") ;
      printf ( "\t\tUse only with CCU 25, channel can be only 0x40 (set by default)\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x] [-ccu x] [-channel x]\n") ;
      printf ( "\n") ;

      printf ( "\t-memoryPerf [number of accesses]\n\t\tTest the performance for the memory channel in\n\t\tmultiple read and multiple write, display the bandwith\n") ;
      printf ( "\t\tThe number of accesses to be done can be specified (<= 65535)\n") ;
      printf ( "\t\tUse only with CCU 25, channel can be only 0x40 (set by default)\n") ;
      printf ( "\t\tOptions: [-fec x] [-ring x] [-ccu x] [-channel x]\n") ;
      printf ( "\n") ;

      printf ( "\t-i2c\n\t\tTest the i2c write/read\n\t\test the access with the normal mode by write and read the values\n\t\t0x0F to the address given (default address 0x0)\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-noFack] [-loop [n[-t]]] [-Tmode t]\n\t\t\t [-fec x] [-ring x] [-ccu x] [-channel x] -[deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-device <list of device type>\n") ;
      printf ( "\t\tWrite and read back the values in all registers of the given\n\t\tdevice type.\n") ;
      printf ( "\t\tDevice type: APV, APV6 (all poss.), MUX, PLL, DCU, laserdriver, DOH, VFAT, CCHIP, TBB\n") ;
      printf ( "\t\tOptions: [-noFack] [-fec x] [-ring x] [-channel x[-xx]]\n") ;
      printf ( "\t\tNote that a range of channels can be specified\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-noFack] [-loop [n[-t]]] [-fec x]\n\t\t\t [-ring x] [-ccu x] [-channel x] -[deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-tracker <list of device type>\n") ;
      printf ( "\t\tWrite and read back the values in all registers of the given\n\t\tdevice type.\n") ;
      printf ( "\t\tDevice type: apv, apv6 (all poss.), mux, pll, dcu, laserdriver, DOH\n") ;
      printf ( "\t\tOptions: [-noFack] [-fec x] [-ring x] [-channel x[-xx]]\n") ;
      printf ( "\t\tNote that a range of channels can be specified\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-noFack] [-loop [n[-t]]] [-fec x]\n\t\t\t [-ring x] [-ccu x] [-channel x] -[deviceAddress x]\n") ;
      printf ( "\n") ;

      printf ( "\t-trackeri2ctest <list of device type>\n") ;
      printf ( "\t\tSame feature than command -tracker but display error counter\n") ;
      printf ( "\t\tDevice type: apv, apv6 (all poss.), mux, pll, dcu, laserdriver, DOH\n") ;
      printf ( "\t\tOptions: [-noFack] [-fec x] [-ring x] [-channel x[-xx]]\n") ;
      printf ( "\t\tNote that a range of channels can be specified\n") ;
      printf ( "\t\tOptions: [-errorCounter [file]] [-noFack] [-loop [n[-t]]] [-fec x]\n\t\t\t [-ring x] [-ccu x] [-channel x] -[deviceAddress x]\n") ;
      printf ( "\n") ;


      printf ( "\t-fileCommand file\n\t\tSubmit a file of frames (FEC/CCU frames) to a given FEC\n") ;
      printf ( "\t\tPossible tags in the file are:\n\n") ;
      printf ( "\t\t- noDisplay: Do not display any message\n") ;
      printf ( "\t\t- noWait: Do not wait after each frame\n") ;
      printf ( "\t\t- noClearErrors: by default, when an error occurs, a clear\n\t\t  of the FEC control register are done (CR1 = 0x3 and\n\t\t  CR0 = 0x1). If this command is set the clear is not performed\n") ;
      printf ( "\t\t- Frame: <Dest Src Len channel transaction command data ...>\n\t\t  to send a frame over the ring. The direct ack is read back\n\t\t  and display. Note that the transaction is fixed by the device\n\t\t  driver so the value given is not important (for value 0xFE,\n\t\t  see next command).\n") ;
      printf ( "\t\t- Frame: <Dest Src Len channel 0xFE command data ...> to send\n\t\t  a frame over the ring and read back the direct ack and the\n\t\t  answer of the frame\n") ;
      printf ( "\t\t- w FecRegister Value: write the given FEC register to the\n\t\t  value given (FEC register are CR0, CR1, SR0, SR1).\n") ;
      printf ( "\t\t- r FecRegister: read the given FEC register and display it\n\t\t  (FEC register are CR0, CR1, SR0, SR1).\n") ;
      printf ( "\n\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-socket [port]\n\t\tOpen a server socket and wait for commands coming\n") ;
      printf ( "\tfrom the socket\n") ;
      printf ( "\tIf no port are specified the default port used is %d\n", DEFAULTSOCKETPORT ) ;
      printf ( "\t\tPossible tags in the file are:\n\n") ;
      printf (" \t\t- s: read the status and control register from FEC (SR0, SR1, CR0, CR1)\n") ;
      printf (" \t\t- f: write the FEC control register (CR0, CR1)\n") ;
      printf ( "\t\t- Frame: <Dest Src Len channel transaction command data ...>\n\t\t  to send a frame over the ring. The direct ack is read back\n\t\t  and display. Note that the transaction is fixed by the device\n\t\t  driver so the value given is not important (for value 0xFE,\n\t\t  see next command).\n") ;
      printf ( "\t\t- Frame: <Dest Src Len channel 0xFE command data ...> to send\n\t\t  a frame over the ring and read back the direct ack and the\n\t\t  answer of the frame\n") ;
      printf ( "\n\t\tOptions: [-fec x] [-ring x]\n") ;
      printf ( "\n") ;

      printf ( "\t-key index\n\t\tTest a key value and display the FEC/RING/CCU/Channel/Address path\n") ;
      printf ( "\t\tThe index must be given in hexadecimal.\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\t-SR0 sr0\n\t\tDisplay in more understandable FEC a status from the FEC\n") ;
      printf ( "\t\tThe SR0 must be given in hexadecimal.\n") ;
      printf ( "\t\tNo option is needed\n") ;
      printf ( "\n") ;

      printf ( "\nDIAGNOSTICS\n\n") ;
      printf ( "When an error occurs, an C++ exceptions is thrown and displayed.\n") ;
      printf ( "Differents errors can arrived, depend of the command.\n") ;
      printf ( "Most of the access are i2c accesses and you can have error on FEC access,\nCCU access, channel access or device access. So take care about the address that\nyou give.") ;
      printf ( "\nPlease fill free to signal errors/bug to fec-support@ires.in2p3.fr.\n") ;
      printf ( "You can also submit modifications/adds to the same address.\n") ;
      printf ( "\nA complete description of the software (device driver, C++ API, examples)\nare available on:\n\thttp://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl\n") ;

      printf ( "\nBUGS\n\n") ;

      printf ( "\nLICENCE\n\n") ;
      printf ( "This file is part of Fec Software project.\n\n") ;

      printf ( "Fec Software is free software; you can redistribute it and/or modify\n") ;
      printf ( "it under the terms of the GNU General Public License as published by\n") ;
      printf ( "the Free Software Foundation; either version 2 of the License, or\n") ;
      printf ( "(at your option) any later version.\n\n") ;
      
      printf ( "Fec Software is distributed in the hope that it will be useful,\n") ;
      printf ( "but WITHOUT ANY WARRANTY; without even the implied warranty of\n") ;
      printf ( "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n") ;
      printf ( "GNU General Public License for more details.\n") ;
      
      printf ( "You should have received a copy of the GNU General Public License\n") ;
      printf ( "along with Fec Software; if not, write to the Free Software\n") ;
      printf ( "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\n") ;
      
      printf ( "Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France\n") ;
}

/** \brief Main function
 */
int main ( int argc, char *argv[] ) {

  /** Download all
   */
  FecAccess *fecAccess = NULL ;

  /**
   */
  char filename[80] = "" ;

  /** Default settings
   */
  unsigned int fecAddress = 0xFFFFFFFF ;
  unsigned int ringAddress = 0x0 ;
  unsigned int ccuAddress = 0x7F;

  unsigned int channelAddress[2] = {0x1B,0x1B} ;
  unsigned int memoryChannel = 0x40 ;
  unsigned int piaChannel = 0x30 ;
  unsigned int deviceAddress = 0x0 ;
  unsigned long numberOfValues ;
  enumDeviceType modeType = PHILIPS ;

  unsigned int i2cSpeed = 100 ;

  unsigned int piaResetValue = 0xFF ;
  unsigned long delayActiveReset = 10 ;
  unsigned long intervalDelayReset = 10000 ;

  // Redundancy and bypassed check
  unsigned int ccuAddresses[128][3] ;

  // For the different types of reconfiguration tests
  unsigned int nbccu = 3 ;

  // To loop on a command
  long loop = 1 ;
  unsigned long tms  = 0 ;  // wait tms microseconds
  int port = DEFAULTSOCKETPORT ;

  char ssidSystem[80] = "TRACKER" ;
  std::string pnpFileName = "/tmp/FecHardwareIdList_partial.txt" ;

  /** Command
   */
  bool ssid                  = false ;
  bool redundancy            = false ;
  bool autoredundancy        = false ;
  bool autotrackerredundancy = false ;
  bool bypassed              = false ;
  bool scanccu               = false ;
  bool apvlasertest          = false ;
  bool lasergain             = false ;
  bool parasitic             = false ;
  bool spuriousreset         = false ;
  bool recover               = false ;
  bool calibrateDOHF         = false ;
  bool pllCheckLockF         = false ;
  bool calibrateI2CSpeedF    = false ;
  bool scantrackerdevice     = false ;
  bool scantrackerdcu        = false ;
  bool scanringedevice       = false ;
  bool displaystatus         = false ;
  bool displaystatusCCU      = false ; 
  bool pia                   = false ;
  bool piaReset              = false ;
  bool memory                = false ;
  bool memoryPerf            = false ;
  bool i2c                   = false ;
  bool device                = false ;
  bool tracker               = false ;
  bool trackeri2ctest        = false ;
  bool key                   = false ;
  bool boolSR0               = false ;
  bool justatest             = false ;
  bool help                  = false ;
  bool byFileCommand         = false ;
  bool bySocketCommand       = false ;
  bool resetAll              = false ;
  bool resetA                = false ;
  bool resetB                = false ;
  bool uFecReloadF           = false ;
  bool crateResetF           = false ;
  bool testCrateResetF       = false ;
  bool createPnpFile         = false ;
  bool fectest               = false ;
  bool fectemp               = false ;
  bool waitCCUAlarmsAll      = false ;
  bool waitPiaInterruptB     = false ;
  bool invertReceiverClk     = false ; 
  bool fecVersion            = false ;
  bool fecTrigger            = false ;
  bool resetCCSTTCRx         = false ;
  keyType myKey = 0 ;

  /** Options
   */
  bool apv              = false ;
  bool apv6             = false ;
  bool pll              = false ;
  bool mux              = false ;
  bool dcu              = false ;
  bool philips          = false ;
  bool laserdriver      = false ;
  bool doh              = false ;

#ifdef TOTEM
  bool vfat             = false ;
  bool cchip            = false ;
  bool tbb              = false ;
#endif

  bool i2creg           = false ;
  bool i2cregvalue      = false ;
  unsigned int value    = 0x0 ;

  bool channel          = false ;
  bool address          = false ;
  bool mode_type        = false ;
  bool access_mode      = false ;
  bool fack             = true  ;
  bool noBroadcast      = false ;

  // Test the DCU in the automatic reconfiguration
  int dcuTest           = 2 ; // Test all DCus

  /**
   */
  bool error            = false ;

  // Error messages
  char errorString[100] ; 

  // For the execution of the test functions
  bool status           = false ; 
  char functionOrder[100][100] ; 
  int order = 0 ;

  // Check if the help is asked
  if ((argc > 1) && ((strcasecmp (argv[1],"--help") == 0)||(strcasecmp (argv[1],"-help") == 0))) { // If the help has been called
    helpMe( argv[0] ) ;
    exit (EXIT_SUCCESS) ;
  }

  // For the recovery of the parameters
  int cnt = 1 ;
  for (int i = cnt ; i < argc ; i ++) {

    if (strcasecmp (argv[i],"-fec") == 0) { // If a new fecAddress has been set

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%d",&fecAddress) || (argv[i+1][0] == '-')) { // Check and init the new fecAddress value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the FEC, must be an integer") ;
        error = true ; 
        status = false ;
      } 
      else {
        status = true ; // No error during this setting
        i ++ ;
      }
    } 
    else if (strcasecmp (argv[i],"-ring") == 0) { // If a new ringAddress has been set

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&ringAddress) || (argv[i+1][0] == '-')) { // Check and init the new ringAddress value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the ring, must be an integer") ;
        error = true ;
        status = false ;
      }
      else {
	status = true ; // No error during this setting
	i ++ ;
      }
    } 
    else if (strcasecmp (argv[i],"-ccu") == 0) { // If a new ccuAddress has been set
      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&ccuAddress) || (argv[i+1][0] == '-')) { // Check and init the new ccuAddress value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the CCU address, must be an integer") ;
        error = true ;
        status = false ;
      }
      else {
	status = true ; // No error during this setting
	i ++;
      }
    }
    else if (strcasecmp (argv[i],"-piaChannel") == 0) { // If a new piaChannel has been set

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&piaChannel) || (argv[i+1][0] == '-')) { // Check and init the new ccuAddress value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the PIA channel, must be an integer") ;
        error = true ;
        status = false ;
      }
      else {
	status = true ; // No error during this setting
	i ++;
      }
    }
    else if (strcasecmp (argv[i],"-i2cSpeed") == 0) { // I2C speed

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%u",&i2cSpeed) || (argv[i+1][0] == '-')) { // Check and init the new ccuAddress value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the i2c speed, must be an integer and possible values are <100, 200, 400, 1000>") ;
        error = true ;
        status = false ;
      }
      else {
	if ( (i2cSpeed != 100) && (i2cSpeed != 200) && (i2cSpeed != 400) && (i2cSpeed != 1000) ) {

	  // Set all error flags and init error message
	  snprintf (errorString, 100, "Wrong value for the i2c speed, must be an integer and possible values are <100, 200, 400, 1000>") ;
	  error = true ;
	  status = false ;
	}
	else {
	  status = true ; // No error during this setting
	  i ++;
	}
      }
    }
    else if (strcasecmp (argv[i],"-channel") == 0) { // If a new channelAddress has been set      

      error = true ;

      if ( (argc <= (i+1)) || (argv[i+1][0] == '-')) {

        snprintf (errorString, 100, "Wrong value for the channel, must be an integer or a range of channel separated by '-'") ;
      }
      else {
        // A range of channel
        char cmd[100] ;
        snprintf (cmd, 100, "%s", argv[i+1]) ;
        char *str1 = strtok (cmd      , "-") ;
        char *str2 = strtok (NULL, "-") ;
        char *str3 = strtok (NULL, "-") ;

        if ( (str3 == NULL) && (str1 != NULL) && (str2 != NULL) &&
             sscanf(str1,"%x",&channelAddress[0]) &&
             sscanf(str2,"%x",&channelAddress[1]) ) error = false ;
      
        if (error) {

          if (!sscanf(argv[i+1],"%x",&channelAddress[0])) { // Check and init the new channelAddress value 

            // Set all error flags and init error message
            snprintf (errorString, 100, "Wrong value for the channel, must be an integer or a range of channel separated by '-'") ;
            error = true ;
            status = false ;
          }
          else {
            channelAddress[1] = channelAddress[0] ;
            error = false ;
          }
        }
        
        i ++ ;
      }
 
      if (! error) {
        status = true ; // No error during this setting
        channel = true ; // Set the flag in order to execute at least the default test program
      }
    }
    else if (strcasecmp (argv[i],"-deviceAddress") == 0) { // If a new deviceAddress has been set

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&deviceAddress) || (argv[i+1][0] == '-')) { // Check and init the new deviceAddress value 
        // Set all error flags and init error message  	
        snprintf (errorString, 100, "Wrong value for the device address, must be an integer") ;
        error = true ;
        status = false ;
      }
      else { 
        status = true ; // No error during this setting
        address = true ; // Set the flag in order to execute at least the default test function   
        i ++;
      }
    }

    else if (strcasecmp (argv[i],"-value") == 0) { // If a new value has been set

      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&value) || (argv[i+1][0] == '-')) { // Check and init the new value 
        // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value to be written, must be an integer") ;
        error = true ;
        status = false ;
      }
      else { 
        status = true ; // No error during this setting
        i2cregvalue = true ; // Set the flag in order to execute at least the default test function   
        i ++ ;
      }
    }

    else if (strcasecmp (argv[i],"-loop") == 0) { // If a loop is define

      if ( argc <= (i+1) ) {

        loop = -1 ;
        tms = 0 ;
      }
      else {
        switch (sscanf(argv[i+1],"%ld-%lu",&loop, &tms)) {
        case 0: 
          loop = -1 ;
          tms = 0 ;
          break ;
          
        case 1:
        case 2:
          i ++ ;
          break ;
        }
      }

      status = true ; // No error during this setting
    }

    else if (strcasecmp (argv[i],"-Tmode") == 0) { // If a new modeType has been set
      mode_type = true ; // Set the flag in order to execute at least the default test function
      status = true ;

      // Check and init the new mode type 
      if      (strncasecmp(argv[i+1],"PLL", 3)      == 0) modeType = PLL ;
      else if (strncasecmp(argv[i+1],"LASERDRIVER", 11) == 0) modeType = LASERDRIVER ;
      else if (strncasecmp(argv[i+1],"DOH", 3)      == 0) modeType = DOH ;
      else if (strncasecmp(argv[i+1],"LAS", 3)      == 0) modeType = LASERDRIVER ;
      else if (strncasecmp(argv[i+1],"DCU", 3)      == 0) modeType = DCU ;
      else if (strncasecmp(argv[i+1],"PHILIPS", 7)  == 0) modeType = PHILIPS ;
      else if (strncasecmp(argv[i+1],"PHILLIPS", 8) == 0) modeType = PHILIPS ;
      else if (strncasecmp(argv[i+1],"MUX", 3)      == 0) modeType = APVMUX ;
      else if (strncasecmp(argv[i+1],"APV_MUX", 7)  == 0) modeType = APVMUX ;
      else if (strncasecmp(argv[i+1],"APVMUX", 6)   == 0) modeType = APVMUX ;
      else if (strncasecmp(argv[i+1],"APV25", 5)    == 0) modeType = APV25 ;
      else { // Set all error flags and init error message
        snprintf (errorString, 100, "Wrong value for the type of device, use --help or -help to know the correct parameters") ;
        error = true ;
        status = false ;
      }

      i ++;
    }

    else if (strcasecmp (argv[i],"-i2c") == 0) { // If the pia test has to be done
      status = true ; // No error during this setting
      i2c = true ; // Set the flag in order to execute the appropriate test function
      
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "i2c") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-i2creg") == 0) {     
      status = true ; // No error during this setting
      i2creg = true ; // Set the flag in order to execute the appropriate test function
      
      
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "i2creg") ;
      order++ ;
      
    }

    else if (strcasecmp (argv[i],"-pia") == 0) { // If the pia test has to be done
      status = true ; // No error during this setting
      pia = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "pia") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-piaReset") == 0) { // If the pia test has to be done

      if ( argc <= (i+1) ) {

        piaResetValue = 0xFF ;
        delayActiveReset = 10 ;
        intervalDelayReset = 10000 ;
      }
      else {

	int val = sscanf(argv[i+1],"%x-%lu-%lu", 
			 &piaResetValue, 
			 &delayActiveReset, 
			 &intervalDelayReset) ;
	if (argv[i+1][0] == '-') val = 0 ;

        switch (val) {
        case 0:
          piaResetValue = 0xFF ;
          delayActiveReset = 10 ;
          intervalDelayReset = 10000 ;
	  break ;
        case 1:
          delayActiveReset = 10 ;
          intervalDelayReset = 10000 ;
	  i += 1 ;
	  break ;
        case 2:
          intervalDelayReset = 10000 ;
	  i += 2 ;
	  break ;
        case 3:
	  i += 3 ;
          break ;
        }
      }

      status = true ; // No error during this setting
      piaReset = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "piaReset") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-memory") == 0) { // If the pia test has to be done
      status = true ; // No error during this setting
      memory = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "memory") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-memoryPerf") == 0) { // Memory access measurement

      numberOfValues = 65535 ;
      if ( (argc > (i+1)) && !sscanf(argv[i+1],"%lu",&numberOfValues) ) { 

        // Set all error flags and init error message  
        snprintf (errorString, 100, "Wrong value for the number of values to be accessed, must be an integer") ;
        error = true ;
        status = false ;
      }
      else {

        status = true ; // No error during this setting
        memoryPerf = true ; // Set the flag in order to execute the appropriate test function
        // Set the table to execute the test functions in the order defined by the user
        strcpy (functionOrder[order], "memoryPerf") ;
        order++;
        i ++ ;
      }
    }

    else if (strcasecmp (argv[i],"-key") == 0) { // If the key test has to be done
      status = true ; // No error during this setting
      key = true ; // Set the flag in order to execute the appropriate test function

      if ( (argc <= (i+1)) || !sscanf(argv[i+1],"%x",&myKey)) { // Check and init the new deviceAddress value 
        // Set all error flags and init error message  	
        snprintf (errorString, 100, "An index must be given") ;
        error = true ;
        status = false ;
      }
      else { 
        status = true ; // No error during this setting

        // Set the table to execute the test functions in the order defined by the user
        strcpy (functionOrder[order], "key") ;
        order++;
        i ++;
      }
    }

    else if (strcasecmp (argv[i],"-SR0") == 0) { // If the key test has to be done
      status = true ; // No error during this setting
      boolSR0 = true ; // Set the flag in order to execute the appropriate test function

      if ( (argc <= (i+1)) || !sscanf(argv[i+1],"%x",&myKey)) { // Check and init the new deviceAddress value 
        // Set all error flags and init error message  	
        snprintf (errorString, 100, "An index must be given") ;
        error = true ;
        status = false ;
      }
      else { 
        status = true ; // No error during this setting

        // Set the table to execute the test functions in the order defined by the user
        strcpy (functionOrder[order], "SR0") ;
        order++;
        i ++;
      }
    }

    // -------------------------------------------------------------------------------------> Just a Test
    else if (strcasecmp (argv[i],"-justatest") == 0) { // If the key test has to be done
      status = true ; // No error during this setting
      justatest = true ; // Set the flag in order to execute the appropriate test function

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "justatest") ;
      order++;
      i ++;
    }

    else if (strcasecmp (argv[i],"-fileCommand") == 0) { // If the key test has to be done
      snprintf (filename, 80, "%s", argv[i+1]) ;
      status = true ; // No error during this setting
      byFileCommand = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "fileCommand") ;
      order++;
      i++;
    }
    else if (strcasecmp (argv[i],"-socket") == 0) { // If the key test has to be done
      status = true ; // No error during this setting
      bySocketCommand = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "socket") ;
      order++;

      if (i+1 < argc) {
	if (sscanf (argv[i+1], "%d", &port) == 1) {
	  i++;
	}
	else {
	  snprintf (errorString, 100, "Bad port value, must be an integer, use --help or -help to know the correct parameters") ;
	  i = argc ;
	  error = true ;
	  status = false ;
	}
      }
    }
    else if (strcasecmp(argv[i],"-ssid") == 0) { // SSID settings

      if (i+1 < argc) {

	i++;
	strncpy (ssidSystem, argv[i], 80) ;
	status = true ;
	strcpy (functionOrder[order], "ssid") ;
	order ++ ;
	ssid = true ;
      }
      else {
	snprintf (errorString,100,"You need to specify which sub system you want to use") ;
	i = argc ;
	error = true ;
	status = false ;
      }
    }
    else if (strcasecmp (argv[i],"-noBroadcast") == 0) { // If the force acknowledge has to be set
      status = true ; // No error during this setting
      noBroadcast = true ;
    }
    else if (strcasecmp (argv[i],"-fack") == 0) { // If the force acknowledge has to be set
      status = true ; // No error during this setting
      fack = true ;
    }
    else if ( (strcasecmp (argv[i],"-errorCounter") == 0) || (strcasecmp (argv[i],"-errorCounting") == 0) ) {

      status = true ; // No error during this setting
      setErrorCounterFlag (true) ; // Enable error

      if ((argc >= (i+1)) && (argv[i+1][0] != '-')) {

        // File name
        FILE *chan = fopen (argv[i+1], "a") ;
        if (chan == NULL) { 

          printf ("Cannot open the file %s\n", argv[i+1]) ;
          setStdchan ( stderr ) ;
        }
        else {
          setStdchan ( chan ) ;
        }

        i ++ ;
      } 
      else setStdchan ( stderr ) ;

      if (getStdchan() != stderr)
        printf ("Set the output of error counter to file %s\n", argv[i]) ;
    }
    else if (strcasecmp (argv[i],"-noGetchar") == 0) { // If the force acknowledge has to be set

      setNoGetchar (true) ;
    }
    else if (strcasecmp (argv[i],"-nofack") == 0) { // If the force acknowledge has to be set
      status = true ; // No error during this setting
      fack = false ;
    }
    else if (strcasecmp (argv[i],"-autoredundancy") == 0) { // Ring A - B

      status = true ;
      autoredundancy = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "autoredundancy") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-autotrackerredundancy") == 0) { // Ring A - B

      dcuTest = 1 ; // Read all DCUs on all CCUs
      if ( (argc > (i+1)) && !sscanf(argv[i+1],"%d",&dcuTest) ) { 

        // Set all error flags and init error message  
        snprintf (errorString, 100, "Wrong value for the DCU mode value, must be 1, 2 or 3") ;
        error = true ;
        status = false ;
      }
      else {

	status = true ;
	autotrackerredundancy = true ;
	// Set the table to execute the test functions in the order defined by the user
	strcpy (functionOrder[order], "autotrackerredundancy") ;
        order++;
        i ++ ;
      }
    }

    else if (strcasecmp (argv[i],"-redundancy") == 0) { // Ring A - B

      status = true ;
      redundancy = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "redundancy") ;
      order++;

      nbccu = 0 ;
      for ( i++ ; i < argc ; i++) {

	if (! strncasecmp(argv[i],"FEC",3)) {
	  ccuAddresses[nbccu][0] = 0x0 ;
	  char input, output ;
	  if (sscanf (&argv[i][4], "%c-%c\n", &input, &output) == 2) {
	    if (input == 'A') ccuAddresses[nbccu][1] = 0 ;
	    else
	      if (input == 'B') ccuAddresses[nbccu][1] = 1 ;
	      else
		error = true ;

	    if (output == 'A') ccuAddresses[nbccu][2] = 0 ;
	    else
	      if (output == 'B') ccuAddresses[nbccu][2] = 1 ;
	      else
		error = true ;
	  }
	  else error = true ;

	  nbccu ++ ;
	}
	else {

	  char input, output ;
	  if (sscanf (argv[i], "%x-%c-%c\n", &ccuAddresses[nbccu][0], &input, &output) == 3) {
	    if (input == 'A') ccuAddresses[nbccu][1] = 0 ;
	    else
	      if (input == 'B') ccuAddresses[nbccu][1] = 1 ;
	      else
		error = true ;

	    if (output == 'A') ccuAddresses[nbccu][2] = 0 ;
	    else
	      if (output == 'B') ccuAddresses[nbccu][2] = 1 ;
	      else
		error = true ;
	  }
	  else error = true ;

	  nbccu ++ ;
	}

	if (error) {
	  snprintf (errorString, 100, "Wrong value for redundancy, use --help or -help to know the correct parameters") ;
	  i = argc ;
	}
#ifdef DEBUGMSGERROR
	else {
	  printf ("%d %d %d\n", 
		  ccuAddresses[nbccu-1][0], 
		  ccuAddresses[nbccu-1][1], 
		  ccuAddresses[nbccu-1][2] ) ;
	}
#endif
      }
    }

    else if (strcasecmp (argv[i],"-bypassed") == 0) { // Ring A - B

      status = true ;
      bypassed = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "bypassed") ;
      order++;

      nbccu = 0 ;
      i ++ ;

      if ( (i == argc) || (sscanf (argv[i], "%x", &ccuAddresses[nbccu][0]) != 1) )
        ccuAddresses[nbccu][0] = 0 ;
      else nbccu ++ ;

      if (error) {
        snprintf (errorString, 100, "Wrong value for bypassed, use --help or -help to know the correct parameters") ;
        i = argc ;
      }
    }
    else if ( (strcasecmp (argv[i],"-scanring") == 0) || (strcasecmp (argv[i],"-scanccu") == 0) ) {
      scanccu = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "scanccu") ;
      order++;
    }
    else if ( (strcasecmp (argv[i],"-apvlasertest") == 0) || (strcasecmp (argv[i],"-scanccu") == 0) ) {
      apvlasertest = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "apvlasertest") ;
      order++;
    }
    else if ( (strcasecmp (argv[i],"-lasergain") == 0) || (strcasecmp (argv[i],"-scanccu") == 0) ) {
      lasergain = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "lasergain") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-scantrackerdevice") == 0) {
      scantrackerdevice = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "scantrackerdevice") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-scantrackerdcu") == 0) {
      scantrackerdcu = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "scantrackerdcu") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-scanringedevice") == 0) {
      scanringedevice = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "scanringedevice") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-status") == 0) {
      displaystatus = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "status") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-statusCCU") == 0) {
      displaystatusCCU = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "statusCCU") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-parasitic") == 0) {
      parasitic = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "parasitic") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-spuriousreset") == 0) {
      spuriousreset = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "spuriousreset") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-calibrateDOH") == 0) {
      calibrateDOHF = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "calibrateDOH") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-pllCheckLock") == 0) {
      pllCheckLockF = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "pllCheckLock") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-calibrateI2CSpeed") == 0) {
      calibrateI2CSpeedF = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "calibrateI2CSpeed") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-recover") == 0) {
      recover = true ;
      status = true ;

      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "recover") ;
      order++;
    }
    else if (strcasecmp (argv[i],"-device") == 0) { // If device tests have to be done

      for (int j = (i+1) ; j < argc ; j++, i++ ) {
        device = true ;
        status = true ; // No error during this setting

        if (strcasecmp (argv[j],"apv") == 0) { 
          apv = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "apv") ;
          order++;
        }
        else if (strcasecmp (argv[j],"6apvs") == 0) {
          apv6 = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "6apvs") ;
          order++;
        }
        else if (strcasecmp (argv[j],"pll") == 0) {
          pll = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "pll") ;
          order++;
        }
        else if (strcasecmp (argv[j],"mux") == 0) {
          mux = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "mux") ;
          order++;
        }
        else if (strcasecmp (argv[j],"dcu") == 0) {
          dcu = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "dcu") ;
          order++;
	  deviceAddress = DCUADDRESS ;
        }
        else if (strcasecmp (argv[j],"philips") == 0) {
          philips = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "philips") ;
          order++;
        }
        else if (strcasecmp (argv[j],"laserdriver") == 0) {
          laserdriver = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "laserdriver") ;
          order++;
        }
        else if (strcasecmp (argv[j],"doh") == 0) {
          doh = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "doh") ;
          order++;
        }
#ifdef TOTEM
	else if (strcasecmp (argv[j],"vfat") == 0) { 
          vfat = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "vfat") ;
          order++;
        }
	else if (strcasecmp (argv[j],"cchip") == 0) { 
          cchip = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "cchip") ;
          order++;
        }
	else if (strcasecmp (argv[j],"tbb") == 0) { 
          tbb = true ;
          // Set the table to execute the test functions required
          strcpy (functionOrder[order], "tbb") ;
          order++;
        }
#endif
        else { // Set all error flags and init error message
          snprintf (errorString, 100, "Wrong parameter for the device, use --help or -help to know the correct parameters") ;
          error = true ;
          status = false ;
          device = false ;
        }
      }
    }
    else if (strcasecmp (argv[i],"-tracker") == 0) { // If device tests have to be done

      for (int j = (i+1) ; j < argc ; j++, i++ ) {
        status = true ; // No error during this setting

        if (strcasecmp (argv[j],"apv") == 0) { 
          apv = true ;
        }
        else if (strcasecmp (argv[j],"6apvs") == 0) {
          apv6 = true ;
        }
        else if (strcasecmp (argv[j],"pll") == 0) {
          pll = true ;
        }
        else if (strcasecmp (argv[j],"mux") == 0) {
          mux = true ;
        }
        else if (strcasecmp (argv[j],"dcu") == 0) {
          dcu = true ;
        }
        else if (strcasecmp (argv[j],"philips") == 0) {
          philips = true ;
        }
        else if (strcasecmp (argv[j],"laserdriver") == 0) {
          laserdriver = true ;
        }
        else if (strcasecmp (argv[j],"doh") == 0) {
          doh = true ;
        }
        else { // Set all error flags and init error message
          snprintf (errorString, 100, "Wrong parameter for the tracker, use --help or -help to know the correct parameters") ;
          error = true ;
          status = false ;
        }
      }

      if (status && !error && ( apv || apv6 || pll || mux || dcu || philips || laserdriver || doh )) {

        tracker = true ;
        // Set the table to execute the test functions required
        strcpy (functionOrder[order], "tracker") ;
        order++;        
      }
      else {

        snprintf (errorString, 100, "Wrong parameter for the tracker, use --help or -help to know the correct parameters") ;
      }
    }

    else if (strcasecmp (argv[i],"-trackeri2ctest") == 0) { // If device tests have to be done

      for (int j = (i+1) ; j < argc ; j++, i++ ) {
        status = true ; // No error during this setting
	
        if (strcasecmp (argv[j],"apv") == 0) { 
          apv = true ;
        }
        else if (strcasecmp (argv[j],"6apvs") == 0) {
          apv6 = true ;
        }
        else if (strcasecmp (argv[j],"pll") == 0) {
          pll = true ;
        }
        else if (strcasecmp (argv[j],"mux") == 0) {
          mux = true ;
        }
        else if (strcasecmp (argv[j],"dcu") == 0) {
          dcu = true ;
        }
        else if (strcasecmp (argv[j],"philips") == 0) {
          philips = true ;
        }
        else if (strcasecmp (argv[j],"laserdriver") == 0) {
          laserdriver = true ;
        }
        else if (strcasecmp (argv[j],"doh") == 0) {
          doh = true ;
        }
        else { // Set all error flags and init error message
          snprintf (errorString, 100, "Wrong parameter for the tracker, use --help or -help to know the correct parameters") ;
          error = true ;
          status = false ;
        }
      }

      if (status && !error && ( apv || apv6 || pll || mux || dcu || philips || laserdriver || doh )) {

        trackeri2ctest = true ;
        // Set the table to execute the test functions required
        strcpy (functionOrder[order], "trackeri2ctest") ;
        order++;        
      }
      else {

        snprintf (errorString, 100, "Wrong parameter for the tracker i2c test, use --help or -help to know the correct parameters") ;
      }
    }

    else if (strcasecmp (argv[i],"-fectest") == 0) { // If the help has been called

      status  = true ; // No error during this setting
      fectest = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "fectest") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-fectemp") == 0) { // If the help has been called

      status  = true ; // No error during this setting
      fectemp = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "fectemp") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-reset") == 0) { // If the help has been called

      status = true ; // No error during this setting
      resetAll = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "reset") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-resetB") == 0) { // If the help has been called

      status = true ; // No error during this setting
      resetB = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "resetB") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-resetA") == 0) { // If the help has been called

      status = true ; // No error during this setting
      resetA = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "resetA") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-uFecReload") == 0) { // If the help has been called

      status = true ; // No error during this setting
      uFecReloadF = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "uFecReload") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-crateReset") == 0) { // If the help has been called

      status      = true ; // No error during this setting
      crateResetF = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "crateReset") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-testCrateReset") == 0) { // If the help has been called

      status      = true ; // No error during this setting
      crateResetF = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "crateReset") ;
      order++;
      testCrateResetF = true ;
    }

    else if (strcasecmp (argv[i],"-createPnpFecHardwareList") == 0) { // If the help has been called

      status        = true ; // No error during this setting
      createPnpFile = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "createFecPnpFile") ;
      order++;
    }


    else if (strcasecmp (argv[i],"-invrecvclk") == 0) { // If the help has been called

      status = true ; // No error during this setting
      invertReceiverClk = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "invrecvclk") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-fecver") == 0) { // If the help has been called

      status = true ; // No error during this setting
      fecVersion = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "fecver") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-fectrigger") == 0) { // If the help has been called

      fecTrigger = true ; // Set the flag in order to execute the appropriate test function
      if (i+1 < argc) {

	i++;
	strncpy (ssidSystem, argv[i], 80) ;
	status = true ;
	strcpy (functionOrder[order], "fectrigger") ;
	order ++ ;
	fecTrigger = true ;
      }
      else {
	snprintf (errorString,100,"You need to specify which sub system you want to use") ;
	i = argc ;
	error = true ;
	status = false ;
      }
    }

    else if (strcasecmp (argv[i],"-resetCCSTTCRx") == 0) { // If the help has been called

      if (i+1 < argc) {

	i++;
	strncpy (ssidSystem, argv[i], 80) ;
	status = true ;
	strcpy (functionOrder[order], "resetCCSTTCRx") ;
	order ++ ;
        resetCCSTTCRx = true ;
      }
      else {
	snprintf (errorString,100,"You need to specify which sub system you want to use") ;
	i = argc ;
	error = true ;
	status = false ;
      }
    }

    else if (strcasecmp (argv[i],"-waitCCUAlarms") == 0) { // If the help has been called

      status = true ; // No error during this setting
      waitCCUAlarmsAll = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "waitCCUAlarms") ;
      order++;
    }

    else if (strcasecmp (argv[i],"-waitPiaInterrupt") == 0) { // If the help has been called

      // Check which PIA channel will be used
      if ( (argc > (i+1)) && (argv[i+1][0] != '-') && (sscanf(argv[i+1],"%x",&piaChannel)) ) i ++ ;
      else piaChannel = 0 ;

      status = true ; // No error during this setting
      waitPiaInterruptB = true ; // Set the flag in order to execute the appropriate test function
      // Set the table to execute the test functions in the order defined by the user
      strcpy (functionOrder[order], "waitPiaInterrupt") ;
      order++;
    }

    else if ((strcasecmp (argv[i],"--help") == 0)||(strcasecmp (argv[i],"-help") == 0)) { // If the help has been called
      help = true ;
      helpMe( argv[0] ) ;
    }

    else { // Set all error flags and init error message

      if ( strncmp(argv[i], "-pci", strlen("-pci")) && strncmp(argv[i], "-usb", strlen("-usb")) && strncmp(argv[i], "-vmecaenpci", strlen("-vmecaenpci")) && strncmp(argv[i], "-vmesbs", strlen("-vmesbs")) && strncmp(argv[i], "-vmecaenusb", strlen("-vmecaenusb")) ) {
	snprintf (errorString, 100, "Wrong parameter \"%s\", use --help or -help to know the correct parameters", argv[i]) ;
	error = true ;
      }
    }
  }

  if (apv && apv6) { // Check if these two concurrent test functions haven't been required 
    snprintf (errorString, 100, "Wrong parameter for the device, apv and 6apvs options can't be used at the same time") ;
    error = true ;
    status = false ;
  }

  if ((device == false) && (!order) && (channel || address || mode_type || access_mode)) { // Check if the default test function for access a device has to be done
    strcpy (functionOrder[order], "default") ;
    order++;
  }
  
  if (error) { // show the possible error message
    printf ( "Error in usage: %s\n",errorString) ;
  } 

  else if (help == false) { // Sum up which test functions have been called

    printf ("Make the configuration for\n") ;
    if (resetAll) printf ("  - reset all FECs\n") ;
    if (resetA) printf ("  - reset all FECs on ring A\n") ;
    if (resetB) printf ("  - reset all FECs on ring B\n") ;
    if (uFecReloadF) printf ("  - reload the firmwares on the corresponding FEC (mFEC, trigger FPGA, VME FPGA)\n") ;
    if (crateResetF && !testCrateResetF) printf ("  - reset the crate\n") ;
    if (testCrateResetF) printf ("  - test the crate reset / firmware reload\n") ;
    if (createPnpFile) std::cout << "  - Create a file in " << pnpFileName << " with the FEC hardware in order to have the pnp working," 
				 << "please append this file at the official file in " 
				 << ((getenv("ENV_TRACKER_DAQ")==NULL) ? "FecSoftwareV3_0" : getenv("ENV_TRACKER_DAQ")) 
				 << "/config/FecHardwareIdList.dat" << std::endl ;
    if (fectest) printf ("  - test the FEC and the rings\n") ;
    if (fectemp) printf ("  - read the 2 temperatures sensors from the VME FEC\n") ;
    if (waitCCUAlarmsAll) printf ("  - wait for any CCU alarms for FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (waitPiaInterruptB) printf ("  - wait for any PIA interrupt for FEC %d ring %d, all CCU, channel %d (if 0 then all channels are used)\n", fecAddress, ringAddress, piaChannel) ;
    if (redundancy) printf ("  - test of ring reconfiguration on FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (autoredundancy) printf ("  - automatic test of ring reconfiguration on FEC %d ring %d only use when CCUs are on ring A (see cmd -autoTrackerRedundancy)\n", fecAddress, ringAddress) ;
    if (autotrackerredundancy) printf ("  - automatic test of ring reconfiguration on FEC %d ring %d for the Tracker\n", fecAddress, ringAddress) ;
    if (bypassed) printf ("  - test of ring reconfiguration on FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (scanccu) printf ("  - scan the ring for CCU on FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (apvlasertest) printf ("  - make tests with AOH / APV on the FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
    if (recover) printf ("  - Try to recover a specific ring\n") ;
    if (calibrateDOHF) printf ("  - calibrate the DOH channel per channel until the correct value\n") ;
    if (pllCheckLockF) printf ("  - check the locking state of all PLL that can be detected\n") ;
    if (calibrateI2CSpeedF) printf ("  - calibrate the i2c speed for each device connected to the ring\n") ;
    if (parasitic) printf ("  - check FEC SR0 for parasitic frame for FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (spuriousreset) printf ("  - check FEC SR0 for spurious reset for FEC %d ring %d\n", fecAddress, ringAddress) ;
    if (displaystatus) printf ("  - Display status for device driver / FEC / CCU\n") ;
    if (displaystatusCCU) printf ("  - Display status for device driver a given FEC and CCU\n") ;
    if (scantrackerdevice) printf ("  - scan the ring for CCUs and Tracker devices\n") ;
    if (scantrackerdcu) printf ("  - scan the ring for CCUs and Tracker DCU devices\n") ;
    if (scanringedevice) printf ("  - scan the ring for CCUs and devices (0x0 -> 0x7F)\n") ;
    if (pia)      printf ("  - the test of the pia channel access for FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, piaChannel); 
    if (piaReset) printf ("  - the test of the pia reset functionnalities for value 0x%x on FEC %d ring %d CCU 0x%x channel %d\n", piaResetValue, fecAddress, ringAddress, ccuAddress, piaChannel) ;
    if (memory)   printf ("  - the test of the memory channel access on on FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, piaChannel) ;
    if (memoryPerf) printf ("  - Memory channel performances measurements in multiple byte access on FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, piaChannel) ;
    if (key)      printf ("  - the test of the key creation\n") ;
    if (boolSR0)      printf ("  - printout the value of the SR0 in more understandable setence\n") ;
    if (byFileCommand) printf ("  - the file %s\n", filename) ;
    if (bySocketCommand) printf ("  - the socket on port %d\n", port) ;
    if (ssid) printf ("  - set the SSID %s\n", ssidSystem) ;
    if (i2c) printf ("  - access a specific register\n"); 
    if (i2creg) printf ("  - access a specific register\n") ;
    if (device) {

      if (channelAddress[0] == channelAddress[1])
	printf ("  - for the device on FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
      else
	printf ("  - for the device on FEC %d ring %d CCU 0x%x channel %d to %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0], channelAddress[1]) ;
      if (apv)         printf ("      * APV") ;
      if (apv6)        printf ("      * 6 APVs") ;
      if (pll)         printf ("      * PLL") ;
      if (mux)         printf ("      * MUX") ;
      if (dcu)         printf ("      * DCU") ;
      if (philips)     printf ("      * Philips") ;
      if (laserdriver) printf ("      * Laserdriver") ;
      if (doh)         printf ("      * DOH") ;
#ifdef TOTEM
      if (vfat)        printf ("      * VFAT (Totem chip)") ;
      if (cchip)       printf ("      * CCHIP (Totem chip)") ;
      if (tbb)         printf ("      * TBB (Totem board)") ;
#endif
      printf ("\n") ;
    }
    if (tracker) {
      if (channelAddress[0] == channelAddress[1])
	printf ("  - for the Tracker device on FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
      else
	printf ("  - for the Tracker device on FEC %d ring %d CCU 0x%x channel %d to %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0], channelAddress[1]) ;
      if (apv)         printf ("      * APV") ;
      if (apv6)        printf ("      * 6 APVs") ;
      if (pll)         printf ("      * PLL") ;
      if (mux)         printf ("      * MUX") ;
      if (dcu)         printf ("      * DCU") ;
      if (philips)     printf ("      * Philips") ;
      if (laserdriver) printf ("      * Laserdriver") ;
      if (doh)         printf ("      * DOH") ;
      printf ("\n") ;
    }
    if (trackeri2ctest) {
      if (channelAddress[0] == channelAddress[1])
	printf ("  - for the Tracker device on FEC %d ring %d CCU 0x%x channel %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
      else
	printf ("  - for the Tracker device on FEC %d ring %d CCU 0x%x channel %d to %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0], channelAddress[1]) ;
      if (apv)         printf ("      * APV") ;
      if (apv6)        printf ("      * 6 APVs") ;
      if (pll)         printf ("      * PLL") ;
      if (mux)         printf ("      * MUX") ;
      if (dcu)         printf ("      * DCU") ;
      if (philips)     printf ("      * Philips") ;
      if (laserdriver) printf ("      * Laserdriver") ;
      if (doh)         printf ("      * DOH") ;
      printf ("\n") ;
    }

    if (invertReceiverClk) printf("  - invert polarity of receiver clock. \n"); 

    if (fecVersion) printf ("  - display the firmware version of the FEC.\n") ;

    if (resetCCSTTCRx) std::cout << "  - toggle CCS trigger on FEC " << std::dec << (unsigned int)fecAddress << std::endl ;
    if (fecTrigger) std::cout <<    "  - display the firmware version of the FEC and trigger values" << std::endl ;

    if (order == 0) printf ("  - default : get Status Register 0 for all FECs\n") ; // %d ring %d\n", fecAddress, ringAddress) ;  
     
    if ((device || tracker || trackeri2ctest) && fack) printf ("    and with the force acknowledge set\n") ;
    if (noBroadcast) printf ("    The CCU broadcast frame is not used\n") ;

    if (! getNoGetchar()) {
      fprintf (stderr, "Press <Enter> to continue ...") ;
      getchar() ;
    }

    // Enable the device driver counters
    if (getErrorCounterFlag()) {
      fprintf (getStdchan(), "------------------------------------ Error counting start\ntimestamp=%ld\n", time(NULL)) ;
    }

    // Create the FEC Access
    try {
      fecAccess = createFecAccess ( argc, argv, &cnt, fecAddress ) ;
    }
    catch (FecExceptionHandler &e) {
      
      cout << "------------ Exception ----------" << std::endl ;
      cout << e.what()  << std::endl ;
      cout << "---------------------------------" << std::endl ;
      exit (EXIT_FAILURE) ;
    }

    // Set the options for the FecAccess
    fecAccess->setForceAcknowledge (fack) ;
    fecAccess->seti2cChannelSpeed (i2cSpeed) ;

    try {
      // Execute the required test functions
      if (order == 0) {
        printf ("------------------------------------------------------------------------------\n") ;
        printf ("default : get status register 0 of all FECs\n") ;
        printf ("------------------------------------------------------------------------------\n") ;
        readFecRegisterSR0(fecAccess, fecAddress, ringAddress, loop, tms) ;
      }
      else {

        for(int i = 0 ; i < order ; i++) {

          if (strcasecmp (functionOrder[i],"i2c") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("default : open a device\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testI2CDevice (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], deviceAddress, modeType, loop, tms) ;
          }
#ifdef TOTEM
	  else if (strcasecmp (functionOrder[i],"i2creg") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("default : open a register\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testI2CRegister (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], deviceAddress, value,  modeType, loop, tms) ;
          }
#endif
          else if (strcasecmp (functionOrder[i],"pia") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("test pia functions\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testPIAfunctions(fecAccess, fecAddress, ringAddress, ccuAddress, piaChannel, loop, tms) ;
            //testPIAfunctions(fecAddress, ringAddress, ccuAddress, piaChannel, loop, tms) ;
          } 
	  else if (strcasecmp (functionOrder[i], "ssid") == 0) {
	    
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Set the SSID for all the FECs in crate\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            setSSID (fecAccess, ssidSystem) ;
	  }
          else if (strcasecmp (functionOrder[i],"reset") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Reset PLXs and FECs\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            resetPlxFec ( fecAccess, fecAddress, ringAddress, loop, tms ) ;
          }
          else if (strcasecmp (functionOrder[i],"uFecReload") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Reload the firmwares on the corresponding FEC (mFEC, trigger FPGA, VME FPGA)\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            uFecReload ( fecAccess, fecAddress ) ;
          }

          else if (strcasecmp (functionOrder[i],"crateReset") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Reset PLXs and FECs\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            crateReset ( fecAccess, testCrateResetF, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"createFecPnpFile") == 0) {

            std::cout << "-------------------------------------------------------------------------------" << std::endl;
	    std::cout << "Create the plug and play file" << std::endl ;
            std::cout << "-------------------------------------------------------------------------------" << std::endl;
            createPlugAndPlayFile ( fecAccess, pnpFileName ) ;
          }

          else if (strcasecmp (functionOrder[i],"fectest") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Test the FEC\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            fecTest (fecAccess, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"fectemp") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Read temperatures of the VME FEC\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            fecTempTest (fecAccess, fecAddress, loop, tms ) ;
          }


	  else if (strcasecmp (argv[i],"-invrecvclk") == 0) { // If the help has been called
	    
	    status = true ; // No error during this setting
	    invertReceiverClk = true ; // Set the flag in order to execute the appropriate test function
	    // Set the table to execute the test functions in the order defined by the user
	    strcpy (functionOrder[order], "invrecvclk") ;
	    order++;
	  }
	  
	  else if (strcasecmp (argv[i],"-fecver") == 0) { // If the help has been called
	    
	    status = true ; // No error during this setting
	    fecVersion = true ; // Set the flag in order to execute the appropriate test function
	    // Set the table to execute the test functions in the order defined by the user
	    strcpy (functionOrder[order], "fecver") ;
	    order++;
	  }

          else if (strcasecmp (functionOrder[i],"resetB") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Reset ring B on all FECs\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            resetRingB ( fecAccess, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"resetA") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Reset ring A on all FECs\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            resetRingA ( fecAccess, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"waitCCUAlarms") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Wait for any CCU alarms, this program is blocked until a ^C.\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            waitCCUAlarms ( fecAccess, fecAddress, ringAddress, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"waitPiaInterrupt") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Wait for any PIA interrupts, this program is blocked until a ^C.\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            waitPiaInterrupt ( fecAccess, fecAddress, ringAddress, piaChannel, loop, tms ) ;
          }

          else if (strcasecmp (functionOrder[i],"piaReset") == 0) {

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("test pia reset functions\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testPIAResetfunctions (fecAccess, fecAddress, ringAddress, ccuAddress, piaChannel, 
                                   piaResetValue, delayActiveReset, intervalDelayReset, loop, tms) ;
          } 
          else if (strcasecmp (functionOrder[i],"memory") == 0) {

            if (!channel) {
              channelAddress[0] = memoryChannel ;
              channel = false ;
            }

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("test memory functions\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testMemoryFunctions(fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
          } 
          else if (strcasecmp (functionOrder[i],"memoryPerf") == 0) {

            if (!channel) {
              channelAddress[0] = memoryChannel ;
              channel = false ;
            }

            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Memory channel measurement\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testMemoryFunctions(fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], numberOfValues) ;
          } 
          else if (strcasecmp (functionOrder[i],"key") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("test key creation\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testKey (myKey,0xFFFFFFFF) ;
          } 
          else if (strcasecmp (functionOrder[i],"SR0") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("SR0 display sentence\n") ;
            printf ("-------------------------------------------------------------------------------\n") ;
            testKey (0xFFFFFFFF,myKey) ;
          } 
          else if (strcasecmp (functionOrder[i],"justatest") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Just a test on FEC %d ring %d CCU 0x%x Channel %d\n", fecAddress, ringAddress, ccuAddress, channelAddress[0]) ;
            printf ("-------------------------------------------------------------------------------\n") ;
            justATest (fecAccess, buildCompleteKey(fecAddress, ringAddress, ccuAddress, channelAddress[0], deviceAddress)) ;
          } 
          else if (strcasecmp (functionOrder[i],"fileCommand") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Read frame from the file %s and send it onto the FEC ring\n", filename) ;
            printf ("-------------------------------------------------------------------------------\n") ;
            readFile (fecAccess, filename, fecAddress, ringAddress) ;
          }
          else if (strcasecmp (functionOrder[i],"socket") == 0) {
            printf ("-------------------------------------------------------------------------------\n") ;
            printf ("Read frame from the a socket on port %d and send it onto the FEC ring\n", port) ;
            printf ("-------------------------------------------------------------------------------\n") ;
	    socketCommand (fecAccess, port, fecAddress, ringAddress) ;
          }
	  else if (strcasecmp (functionOrder[i],"invrecvclk") == 0) {
	    
	    printf ("-------------------------------------------------------------------------------\n") ;
	    printf ("Clock inversion\n") ;
	    printf ("-------------------------------------------------------------------------------\n") ;
	    toggleReceiverClockPolarity ( fecAccess, loop, tms ) ;
	  }
	  else if (strcasecmp (functionOrder[i],"fecver") == 0) {
	    
	    printf ("-------------------------------------------------------------------------------\n") ;
	    printf ("Display firmware version on all FECs\n") ;
	    printf ("-------------------------------------------------------------------------------\n") ;
	    displayFecVersion ( fecAccess, loop, tms ) ;
	  }
	  else if (strcasecmp (functionOrder[i],"fectrigger") == 0) {
	    
	    printf ("-------------------------------------------------------------------------------\n") ;
	    printf ("Display firmware version on all FECs\n") ;
	    printf ("-------------------------------------------------------------------------------\n") ;
	    displayFecVersion ( fecAccess, loop, tms, true, ssidSystem, resetCCSTTCRx ) ;
	  }
	  else if (strcasecmp (functionOrder[i],"resetCCSTTCRx") == 0) {
	    
	    printf ("-------------------------------------------------------------------------------\n") ;
	    printf ("Reset TTCRx on CCS on the given slot\n") ;
	    printf ("-------------------------------------------------------------------------------\n") ;
	    commandResetCCSTTCRx ( fecAccess, fecAddress, ssidSystem, loop, tms ) ;
	  }
          else if (strcasecmp (functionOrder[i],"scanccu") == 0) {
	    
            testScanCCU (fecAccess, fecAddress, ringAddress, noBroadcast) ;
          }
          else if (strcasecmp (functionOrder[i],"apvlasertest") == 0) {

            testApvLaser (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], loop, tms) ;
          }
          else if (strcasecmp (functionOrder[i],"lasergain") == 0) {

            testLaserGain (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], loop, tms) ;
          }
          else if (strcasecmp (functionOrder[i],"recover") == 0) {

            testRecover (fecAccess, fecAddress, ringAddress) ;
          }
          else if (strcasecmp (functionOrder[i],"calibrateDOH") == 0) {

            calibrateDoh (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], 0x70) ;
          }
          else if (strcasecmp (functionOrder[i],"pllCheckLock") == 0) {

            pllLockCheck (fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], 0x44, loop, tms) ;
          }
          else if (strcasecmp (functionOrder[i],"calibrateI2CSpeed") == 0) {
      
            testScanTrackerDevice (fecAccess, fecAddress, ringAddress, loop, tms, false, true) ;
          }
          else if (strcasecmp (functionOrder[i],"parasitic") == 0) {

            checkParasiticFrame (fecAccess, fecAddress, ringAddress) ;
          }
          else if (strcasecmp (functionOrder[i],"spuriousreset") == 0) {

            checkSpuriousReset (fecAccess, fecAddress, ringAddress) ;
          }
          else if (strcasecmp (functionOrder[i],"scantrackerdevice") == 0) {

            testScanTrackerDevice (fecAccess, fecAddress, ringAddress, loop, tms, false, false) ;
          }
          else if (strcasecmp (functionOrder[i],"scantrackerdcu") == 0) {

            testScanTrackerDevice (fecAccess, fecAddress, ringAddress, loop, tms, true, false) ;
          }
          else if (strcasecmp (functionOrder[i],"scanringedevice") == 0) {

            testScanRingEDevice (fecAccess, fecAddress, ringAddress, noBroadcast) ;
          }
          else if (strcasecmp (functionOrder[i],"status") == 0) {

            testStatus (fecAccess, noBroadcast) ;
          }
          else if (strcasecmp (functionOrder[i],"statusCCU") == 0) {

            testCCU (fecAccess, fecAddress, ringAddress, ccuAddress, loop, tms) ;
          }
          else if (strcasecmp (functionOrder[i],"redundancy") == 0) {

	    testRedundancyRing ( fecAccess, fecAddress, ringAddress, ccuAddresses, nbccu) ;
	  }
          else if (strcasecmp (functionOrder[i],"autoredundancy") == 0) {

	    autoRedundancyRing ( fecAccess, fecAddress, ringAddress, channelAddress[0], dcuTest, loop, tms) ;
	    //autoTrackerRedundancyRing ( fecAccess, fecAddress, ringAddress, channelAddress[0], dcuTest, loop, tms) ;
	  }
          else if (strcasecmp (functionOrder[i],"autotrackerredundancy") == 0) {

	    autoTrackerRedundancyRing ( fecAccess, fecAddress, ringAddress, channelAddress[0], dcuTest, loop, tms) ;
	  }
	  else if (strcasecmp (functionOrder[i],"bypassed") == 0) {

            ccuBypassed ( fecAccess, fecAddress, ringAddress, ccuAddresses[0][0]) ;
          }
          else if (strcasecmp (functionOrder[i],"tracker") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {

              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Test the devices enter for channel %d\n", channel) ;

              if (!(testTrackerDevices(fecAccess, fecAddress, ringAddress, ccuAddress, channel, apv6, apv, mux, dcu, laserdriver, doh, pll, loop, tms))) printf ("\n\nTracker devices set\n\n") ; 
              else printf ("\n\nError during the setting of the Tracker devices\n\n") ;
            }

            apv6 = apv = mux = dcu = laserdriver = pll = doh = false ;
          }
          else if (strcasecmp (functionOrder[i],"trackeri2ctest") == 0) {

	    if (!(testTrackerDevicesI2CTest(fecAccess, fecAddress, ringAddress, ccuAddress, channelAddress[0], channelAddress[1], apv6, apv, mux, dcu, laserdriver, doh, pll, loop, tms))) printf ("\n\nTracker devices set\n\n") ; 
	    else printf ("\n\nError during the setting of the Tracker devices\n\n") ;
	    
            apv6 = apv = mux = dcu = laserdriver = pll = doh = false ;
          }
	  
          else if (strcasecmp (functionOrder[i],"philips") == 0) {

	    if (!address) deviceAddress = 0x20 ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the philips chip (Address = 0x%x) for channel %d\n", deviceAddress, channel) ; 

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testPhilips(fecAccess, fecAddress, ringAddress, ccuAddress, channel, 0x20, loop, tms))) printf ("\n\nPhilips chip set\n\n") ; 
              else printf ("\n\nError during the setting of the philips\n\n") ; 
            }
          }

          else if (strcasecmp (functionOrder[i],"mux") == 0) {

	    if (!address) deviceAddress = 0x43 ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the MUX (Address = 0x%x) for channel %d\n", deviceAddress, channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testMux(fecAccess, fecAddress, ringAddress, ccuAddress, channel, 0x43, loop, tms))) printf ("\n\nMUX set\n\n") ; 
              else printf ("\n\nError during the setting of the MUX\n\n") ;
            }
          }

          else if (strcasecmp (functionOrder[i],"apv") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the APVs (Adresses = 0x20, 0x21, 0x24, 0x25) for channel %d\n", channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(test4Apvs(fecAccess, fecAddress, ringAddress, ccuAddress, channel, 0x20, 0x21, 0x24, 0x25, loop, tms))) printf ("\n\nApvs set\n\n") ; 
              else printf ("\n\nError during the setting of the APVs\n\n") ;
            }
          }
          else if (strcasecmp (functionOrder[i],"6apvs") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the APVs (Addresses = 0x20,0x21,0x22,0x23,0x24,0x25) for channel %d\n", channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if(!(test6Apvs(fecAccess, fecAddress, ringAddress, ccuAddress, channel, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, loop, tms))) printf ("\n\nApvs set\n\n") ; 
              else printf ("\n\nError during the setting of the APVs\n\n") ;
            }
          }

          else if (strcasecmp (functionOrder[i],"pll") == 0) {

	    if (!address) deviceAddress = 0x44 ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the PLL (Address = 0x%x) for channel %d\n", deviceAddress, channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testPll(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nPll set\n\n") ; 
              else printf ("\n\nError during the setting of the PLL\n\n") ; 
            }
          }

          else if(strcasecmp (functionOrder[i],"laserdriver") == 0) {

	    if (!address) deviceAddress = 0x60 ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the laserdriver (Address = 0x%x) for channel %d\n", deviceAddress, channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testLaserdriver(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nLaserdriver set\n\n") ; 
              else printf ("\n\nError during the setting of the laserdriver\n\n") ; 
            }
          }

          else if(strcasecmp (functionOrder[i],"doh") == 0) {

	    if (!address) deviceAddress = 0x70 ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the DOH (Address = 0x%x) for channel %d\n", deviceAddress, channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testDoh(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nDOH set\n\n") ; 
              else printf ("\n\nError during the setting of the DOH\n\n") ; 
            }
          }
	  else if (strcasecmp (functionOrder[i],"dcu") == 0) {

	    if (!address) deviceAddress = DCUADDRESS ;

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the dcu (Address = 0x%x) for channel %d\n", deviceAddress, channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (! (testDcu(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\ndcu set\n\n") ; 
              else printf ("\n\nError during the setting of the DCU\n\n") ;  
            }
          }
#ifdef TOTEM
          else if (strcasecmp (functionOrder[i],"vfat") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the VFAT for channel %d\n", channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testVFAT(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nVFAT(s) set\n\n") ; 
              else printf ("\n\nError during the setting of the VFAT(s)\n\n") ;
            }
          }
          else if (strcasecmp (functionOrder[i],"cchip") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the CCHIP for channel %d\n", channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testTotemCChip(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nCCHIP(s) set\n\n") ; 
              else printf ("\n\nError during the setting of the CCHIP(s)\n\n") ;
            }
          }
          else if (strcasecmp (functionOrder[i],"tbb") == 0) {

            for (unsigned int channel = channelAddress[0] ; channel <= channelAddress[1] ; channel ++) {
              printf ("-------------------------------------------------------------------------------\n") ;
              printf ("Set the TBB for channel %d\n", channel) ;

              if (! getNoGetchar()) {
                cout << "Press <Enter> to continue" ;
                getchar();
              }

              if (!(testTotemBB(fecAccess, fecAddress, ringAddress, ccuAddress, channel, deviceAddress, loop, tms))) printf ("\n\nTBB(s) set\n\n") ; 
              else printf ("\n\nError during the setting of the TBB(s)\n\n") ;
            }
          }
#endif
        }
      }
    }
    catch (FecExceptionHandler &e) {
      
      cout << "------------ Exception ----------" << std::endl ;
      cout << e.what()  << std::endl ;
      cout << "---------------------------------" << std::endl ;
      exit (EXIT_FAILURE) ;
    }
  }

  if (fecAccess != NULL) {

    // Enable the device driver counters
    if (getErrorCounterFlag()) {

#ifdef FECSOFTWAREV1_0
      try {
        fecAccess->setErrorCounting (false) ;
        //displayStatus ( NULL, 0, fecAccess, getStdchan() ) ;
      }
      catch (FecExceptionHandler &e) {}
#endif
      
      fprintf (getStdchan(), "------------------------------------ Error counting stop\ntimestamp=%ld\n------------------------------------ End of Program\n", time(NULL)) ;
    }

    try {
      delete fecAccess ;
    }
    catch (FecExceptionHandler &e) {

      std::cerr << "------------ Exception ----------" << std::endl ;
      std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
      std::cerr << "---------------------------------" << std::endl ;
    }
  }

  exit (0) ;

}

