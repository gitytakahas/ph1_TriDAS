This files explain the modifications only in FecProfiler. The changes
related to the FEC C++ API is given in FecSupervisor/README.TXT.
The changes of FecProfiler before the 07/11/2003 are given in 
FecSupervisor/README.TXT
---------------------------------------------------------------------
File   : XML and database access
Date   : 16/04/2004
Version: CVS
Tests  : Ok
Updates:
  - Add a DbFecAccess and a DbFedAccess class and change
    all XML files.
  - Modify the FecProfiler for this accesses

File   : FecProfiler
Date   : 11/03/2004
Version: CVS
Tests  : Ok
Updates:
  - Use the new version of the DbAccess class that check dynamically the
    environment variables to find login/password and URL for the database:
      * DBCONFLOGIN, DBCONFPASSWD, DBCONFPATH
    If they are not existing then try to check if it is defined directly in
    the FecHeader.linux

File   : FecProfiler
Date   : 17/12/2003
Version: CVS
Tests  : Ok
Updates:
  - Update for new XML parsing (see FecSupervisor/README.TXT)

File   : FecProfiler
Date   : 10/12/2003
Version: CVS
Tests  : Ok
Updates:
  - Add a new option (-outputfile=[file]) to specify the XML output file
  - Add several commands for the FecProfiler
  - -tracker for automatic detection of Tracker devices (can be extended to
    any devices) and write the device values into an XML buffer.
  - -database, not yet implemented
  - FecProfiler.exe [XML File Name] exists

File   : FecProfiler
Date   : 07/11/2003
Version: CVS
Tests  : Ok
Updates:
  - The library created are libDeviceAccess.so
    available on FecSoftware/lib or 
    FecSoftware/ThirdParty/APIXMLDebugger/lib/linux/x86.


