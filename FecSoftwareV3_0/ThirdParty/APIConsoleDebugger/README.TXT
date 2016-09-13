This files explain the modifications only in ProgramTest. The changes
related to the FEC C++ API is given in FecSupervisor/README.TXT.
The changes of ProgramTest before the 07/11/2003 are given in 
FecSupervisor/README.TXT
---------------------------------------------------------------------

File   : ProgramTest
Date   : 07/04/04
Version: CVS
Tests  : Ok
Updates:
  - Remove the command -redundancy
  - Add a loop on the reset command (-reset)

File   : ProgramTest
Date   : 24/03/04
Version: CVS
Tests  : Ok
Updates:
  - Add a command to bypass one CCU (-fec [FEC] -ring [RING] -bypassed CCU

File   : ProgramTest
Date   : 27/02/04
Version: CVS
Tests  : Ok
Updates:
  - Change the command -scanringdevice with -scantrackerdevice
  - Change the command -scanring with -scanccu

File   : APIAccess.cc
Date   : 27/01/2003
Version: CVS
Tests  :
Updates:
  - Add some socket commands

File   : ProgramTest.cc APIAccess.cc
Date   : 12/12/2003
Version: CVS
Tests  :
Updates:
  - Add a method to check open a socket and depend of the string sent, a 
    command is applied.
  - This method is mostly using by Christian Paillard for debugging test.
  - The 19/12/2003, new commands was added

File   : ProgramTest.cc APIAccess.cc
Date   : 04/12/2003
Version: CVS
Tests  :
Updates:
  - Add a method to check the parasitic frame receive by the FEC
  - Add a new option: -parasitic

File   : ProgramTest.cc APIAccess.cc
Date   : 01/12/2003
Version: CVS
Tests  :
Updates:
  - Add a method to change the input/ouput for FEC and CCU (testRedundancyRing)
  - Add a new option: -redundancy FEC-Input-Output CCUAddress-Input-Ouput ...

File   : ProgramTest
Date   : 21/11/2003
Version: CVS
Tests  : Ok
Updates:
  - Manage the Digital Optical Hybrid (DOH)

File   : ProgramTest
Date   : 19/11/2003
Version: CVS
Tests  : Ok
Updates:
  - The command -reset display the FEC status register 0

File   : ProgramTest
Date   : 12/11/2003
Version: CVS
Tests  : Ok
Updates:
  - Add a maximum of errors before the program stop in command -tracker 
    for loop command

File   : ProgramTest
Date   : 07/11/2003
Version: CVS
Tests  : Ok
Updates:
  - Separate the ProgramTest in two parts, one library and the main
  - The library created are libHardwareAccess.so and libHardwareAccess.a
    available on FecSoftware/lib or 
    FecSoftware/ThirdParty/APIConsoleDebugger/lib/linux/x86.


