Contents
--------
*  Lisence
1) Version history
2) Known issues
3) Software needed
4) Environment variables
5) Compiling the vfat_controller

**** Lisence ****

The vfat_controller is part of Fec Software project.

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

Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland

******************

1) Version history

8/7/2009
- Fixed the filling of the VFAT and CCHIP identifiers in the panels

10/6/2009
- Fixed a bug: PIA channels and CCU addresses not updated in the Scans page when selecting a different FEC

22/5/2009  (chiptest version 1.11)
- Fixed a bug: Wrong VCal range shown for the low resolution s-curves in the summary file 

23/4/2009  (chiptest version 1.10)
- Introducted a version for the TTP chip testing procedure, included in summary file
- TTP firmware version added to summary file
- S-Bit acceptance limit changed from 5 % to 10 %

22/4/2009
- The S-bit checking updated (all VFATs individually) in the chip testing procedure, the TTP firmware version needed 0x211f (or newer).
- Mode for taking data with TTP using external trigger, the TTP firmware version needed 0x211f (or newer).
- Fine tuning of the PVSS datapoint names
- Check all the VFAT and CCHIP identifier while filling the devices to the panels.

7/4/2009
- Modifications related to the external TOTFED class
- Added options for setting the FED and TTCci slots from command line (use "-help" for details)
- The firmware versions of TOTFED and OptoRx are read during the initialization

6/4/2009
- Added a procedure for using VME readout with external trigger.
- Added a mode for using OptoRx data emulator (can be enabled from Makefile.settings).
- Added a option for using OptoRx data emulator with VME trigger (can be enabled from Makefile.settings).
- New procedure for acquiring and sending the detector biasing current and temperature values to PVSS system (RP only).
- Removed the "Power on" test from the scans (more practical to use the "Control" page)

11/3/2009
- VME readout included to the scans. (SLink still used as default, VME can be enabled from Makefile.settings)
- Redirected the TTCci messages to error output
- Disabled the filling of V and W options based on NP

19/2/2009
- Bug fixes to the coincidence panel

13/2/2009
- Removed the DAC Check and S-Curves from the trigger VFAT test procedure

12/2/2009
- Treasure Hunt procedure updated, the "Master" VFAT is no more fixed to certain position.
- Software accepts now also the "PIA reset entries" from XML files
- Removed the duplicated VFAT identifiers from panel
- Redirected the TTCci messages to "/dev/null"
- Removed extra messages from threshold scan (messages printed for every VFAT changed for 1 per step only)
- Added colors (red=error/warning, blue=info, cyan=debug) to print out messages. Messages coming from external parts are printed using default color.

10/12/2008
- Swapped the NP / Z relation to coincidence panel (NP=5 -> Z=0-15, NP=10 -> Z=0-7).

4/12/2008
- OptoRx reset call added to the scans' start up (needs an additional program, resetOptoRx, to be installed)  
- Added an option to disable the FEC initialization stuff while probing the I2C devices
- New option for configuring indivial register(s) for all the connected VFATs from the panel
- Added a tab for configuring the CCHIPs

6/11/2008
- User describes the i2c devices for the software by using an XML file
- The device type probing remains only for the VFAT, CCHIP, DCU chips
- Added option to save all i2c registers seen by the software to a file
- Added option to configure parameter(s) on all connected VFATs on one go
- Revised the error messages

29/10/2008
- Fixing some labels in the GUI
- Empty elog message also for non-RP hybrids in case no respond to I2C (was missing for some reason)  

17/10/2008
- Added an option for the trigger mezzanine testing

22/8/2008
- CTS/BE convertion factors changed for RP
- HV control for RP test with detector
- Control for CCU parallel port bits
- Changed the I2C device "identification procedure"
- New option "-detectortype" to force the detector type (not to ID autom.)

6/8/2008
- Some cleaning of the code

20/06/2008
- More relaxed acceptance limits for S-Curves (TTPTestConfig.txt)

16/06/2008
- SSID changed to PRESHOWER (No own mode for TOTEM yet)

12/6/2008
- Support for multiple rings and CCUs
- Removed use of $VFAT_CONTROLLER_HOME variable

6/6/2008
- Option for using TTCci trigger (enable from Makefile.settings)

3/6/2008
- Updated the directory structure for T1

29/5/2008
- Moved to use only one xml config for chip testing
- Fixed bug in polarity change (T1 related)

28/5/2008
- New directory structure for T1 and T2
- Added name to summary file

14/5/2008
- Changes needed after moving things to totem/generic

25/4/2008
- Dummy summary in case of I2C failure
- Added TestManager calls

11/4/2008
- First version of the complete chip testing procedure

  Features:
  * I2C, Data Packet, Mask, DAC, S-Bit, Monostable and Polarity Checks (+/-) and Pulse Scans (Low/High Res)
  * GPIB driven PS reading + other misc stuff, manual entrying also available
  * Bar code assignment
  * New directory structure for test results
  * Can now handle the power down while changing the hybrids
  * Chip testing now possible for indivial / selected chips also with RP
  * Loop option for chip testing

- Scrollable GUI
- New file "Makefile.settings" for compilation options

17/12/2007
- TTP based VFAT chip/hybrid testing part added (Not fully tested)
- Option for TTP based triggering and readout added to standard scan procedures (Not fully tested)

30/8/2007
- Compatibility with XDAQ 3.11
- Automatic chip detection for VFAT, CCHIP, DCU and FPGA
- Multiple VFAT update for all the scans
- New feature "Threasure Hunt"
- Separate Roman Pot CTS/BE reading
- DCU characterization update
- Retry I2C command sending

20/7/2007
- Exception handling corrected

16/7/2007 - First version of vfat_controller
- The SW was previously called VFATGUIDebugger


2) Known issues

- The software to do the "PC driven triggering" is not included (used together with Transition board).


3) Software needed

You need QT version between 3.0.5 and 3.x.x in order to compile it. Tested with version 3.3.3, which comes with standard SLC4 intallation. 


4) Environment variables

- Use the configure script located on the FecSoftware root
- Define variables requested (for more info type "configure --help")
- To compile with TTCci, configure $TTCSOFTWARE to point into your TTC Software installation
- To compile with VME readout, configure $TOTFED_ROOT to point into your TOTFED software installation 


5) Compiling the vfat_controller

cd FecSoftwareV3_0/ThirdParty/Totem/vfat_controller
make -f commands/Makefile

- Check commands/Makefile.settings for the different compilation options

**** OR ****

vfat_controller is also compiled with rest of TOTEM stuff

cd FecSoftwareV3_0
make totem

