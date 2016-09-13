# The one which should be done is marked TBD>
# This is the test that should be performed to validate the different software:

# ##########################################################################################################
# FEC
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fec -partition MORESLC4_7 -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
==8474== LEAK SUMMARY:
==8474==    definitely lost: 0 bytes in 0 blocks.
==8474==      possibly lost: 0 bytes in 0 blocks.
==8474==    still reachable: 823,283 bytes in 4,000 blocks.
==8474==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# Redundancy
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_MERSI_20080310 -redundancy 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_MERSI_20080310 -redundancy -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
==6198== LEAK SUMMARY:
==6198==    definitely lost: 0 bytes in 0 blocks.
==6198==      possibly lost: 0 bytes in 0 blocks.
==6198==    still reachable: 828,027 bytes in 3,930 blocks.
==6198==         suppressed: 0 bytes in 0 blocks.

valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_MERSI_20080310 -redundancy -uploadFile 2>/tmp/error.txt ; emacs /tmp/error.txt
==6202== LEAK SUMMARY:
==6202==    definitely lost: 2,944 bytes in 16 blocks.
==6202==      possibly lost: 0 bytes in 0 blocks.
==6202==    still reachable: 828,027 bytes in 3,930 blocks.
==6202==         suppressed: 0 bytes in 0 blocks.

TBD> valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition BADPARTITION -redundancy -uploadFile 2>/tmp/error.txt ; emacs /tmp/error.txt

# ##########################################################################################################
# det id in DbInterface
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_TKCC66 -detId 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_TKCC66 -detId -uploadFile 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -partition CRACK_TKCC66 -detId -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt

==7714== LEAK SUMMARY:
==7714==    definitely lost: 0 bytes in 0 blocks.
==7714==      possibly lost: 0 bytes in 0 blocks.
==7714==    still reachable: 828,545 bytes in 3,930 blocks.
==7714==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# FED
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fed -partition MORESLC4_7 -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fedId 55 -fed -partition MORESLC4_7 -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fedId 50 -fed -partition MORESLC4_7 -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fedId 50 -fed -partition RRRR -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -fed -partition RRRR -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt

# ##########################################################################################################
# Connection
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/DeviceDbInterface.exe -connection -partition MORESLC4_7 -uploadDB 2>/tmp/error.txt ; emacs /tmp/error.txt
==8598== LEAK SUMMARY:
==8598==    definitely lost: 0 bytes in 0 blocks.
==8598==      possibly lost: 0 bytes in 0 blocks.
==8598==    still reachable: 823,283 bytes in 4,000 blocks.
==8598==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# DCU conversion
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -output database -uploadDcuConversion ../../config/DcuConversionFactors.xml 2>/tmp/error.txt ; emacs /tmp/error.txt
==8617== LEAK SUMMARY:
==8617==    definitely lost: 0 bytes in 0 blocks.
==8617==      possibly lost: 0 bytes in 0 blocks.
==8617==    still reachable: 823,277 bytes in 4,000 blocks.
==8617==         suppressed: 0 bytes in 0 blocks.
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -output /tmp/dcuconversion.xml -uploadDcuConversion database 2>/tmp/error.txt ; emacs /tmp/error.txt
==7004== LEAK SUMMARY:
==7004==    definitely lost: 0 bytes in 0 blocks.
==7004==      possibly lost: 0 bytes in 0 blocks.
==7004==    still reachable: 823,419 bytes in 4,008 blocks.
==7004==         suppressed: 0 bytes in 0 blocks
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -output /tmp/dcuconversion.xml -partition COUCOU -uploadDcuConversion database 2>/tmp/error.txt ; emacs /tmp/error.txt
==7004== LEAK SUMMARY:
==7004==    definitely lost: 0 bytes in 0 blocks.
==7004==      possibly lost: 0 bytes in 0 blocks.
==7004==    still reachable: 823,419 bytes in 4,008 blocks.
==7004==         suppressed: 0 bytes in 0 blocks

# ##########################################################################################################
# DET ID: upload / download a wrong partition / download a real partition / download all
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input xml/AllDcuInfo.xml -output database -uploadDetId 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input database -output /tmp/detid.xml -uploadDetId RRR 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input database -output /tmp/detid.xml -uploadDetId MORESLC4_7 2>/tmp/error.txt ; emacs /tmp/error.txt
==17889== LEAK SUMMARY:
==17889==    definitely lost: 0 bytes in 0 blocks.
==17889==      possibly lost: 0 bytes in 0 blocks.
==17889==    still reachable: 823,617 bytes in 4,006 blocks.
==17889==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# CCU and ring
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -fecHardwareId 3020140B36C504 -partition MORESLC4_1 -ring 8 -download 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -fecHardwareId 3020140B36C504 -partition MORESLC4_7 -ring 8 -download 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -fecHardwareId 3020140B364 -partition MORESLC4_7 -ring 8 -download 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -fecHardwareId 3020140B364 -partition RRRR -ring 8 -download 2>/tmp/error.txt ; emacs /tmp/error.txt
==19509== LEAK SUMMARY:
==19509==    definitely lost: 0 bytes in 0 blocks.
==19509==      possibly lost: 0 bytes in 0 blocks.
==19509==    still reachable: 823,621 bytes in 4,006 blocks.
==19509==         suppressed: 0 bytes in 0 blocks.
TBD> bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -partition MORESLC4_1 -download
TBD> bin/linux/x86_slc4/CCUTemplateTest.exe -input database -output /tmp/coucou.xml -fecHardwareId 3020140B9CCE6E -ring 3 -downloadTKCC

# ##########################################################################################################
# Run: ok / unicity of the run failed / wrong partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition MORESLC4_7 -runNumber 4 -runMode PHYSIC -setRun 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition MORESLC4_7 -runNumber 4 -runMode PHYSIC -setRun 2>/tmp/error.txt ; emacs /tmp/error.txt
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition BLABLA -runNumber 5 -runMode PHYSIC -setRun 2>/tmp/error.txt ; emacs /tmp/error.txt
==21572== LEAK SUMMARY:
==21572==    definitely lost: 0 bytes in 0 blocks.
==21572==      possibly lost: 0 bytes in 0 blocks.
==21572==    still reachable: 823,616 bytes in 4,006 blocks.
==21572==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# Version / State methods / Run / PSU DCU map: display all partitions with all number of versions
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition ALL -displayVersion 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods: display the current state
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -displayCurrentState 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods: display the run for a given partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition MORESLC4_7 -displayRun 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods:  display the run for a given partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition FULLTRACKER -displayRun 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods:  display the run for a given partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition MORESLC4_5 -displayRun 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods: set the current state for a partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition MORESLC4_7 -setCurrentState 2>/tmp/error.txt ; emacs /tmp/error.txt

This error have been observed: http://forums.oracle.com/forums/thread.jspa?messageID=1493167

==14926== Invalid read of size 8
==14926==    at 0x543D0D8: (within /afs/cern.ch/project/oracle/linux/10203/lib/libnnz10.so)
==14926==  Address 0x5990E98 is 144 bytes inside a block of size 146 allocd
==14926==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==14926==    by 0x48D3205: nacomap (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48D2F7B: nacomsn (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48D00E2: na_client (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48CFB0E: naconnect (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48A8640: nsnadoconn (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48A7FC4: nsnaconn (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x487D7D6: nscall (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48B889B: niotns (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x4928F9F: nigcall (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x48BCE3B: osncon (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==14926==    by 0x4604CC7: kpuadef (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)

# Version / State methods: display all partitions
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -displayPartition 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods: display all state names
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -displayStateName 2>/tmp/error.txt ; emacs /tmp/error.txt
# Version / State methods: display all versions for all partitions
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -partition RRRR -displayVersion 2>/tmp/error.txt ; emacs /tmp/error.txt
==14965== LEAK SUMMARY:
==14965==    definitely lost: 0 bytes in 0 blocks.
==14965==      possibly lost: 0 bytes in 0 blocks.
==14965==    still reachable: 823,440 bytes in 4,000 blocks.
==14965==         suppressed: 0 bytes in 0 blocks.

# Display the version for a given run
TBD> bin/linux/x86_slc4/testVersionStateRun.exe -partition CRACK_TKCC56 -runNumber 36511 -displayRun
TBD> bin/linux/x86_slc4/testVersionStateRun.exe -runNumber 36511 -displayRun
TBD> bin/linux/x86_slc4/testVersionStateRun.exe -runNumber 36511 -displayPartition
TBD> bin/linux/x86_slc4/testVersionStateRun.exe -displayPartitionFromCurrentState

# ##########################################################################################################
# ID vs hostname: all data
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -idVsHostname 2>/tmp/error.txt ; emacs /tmp/error.txt
==24380== LEAK SUMMARY:
==24380==    definitely lost: 0 bytes in 0 blocks.
==24380==      possibly lost: 0 bytes in 0 blocks.
==24380==    still reachable: 822,407 bytes in 3,974 blocks.
==24380==         suppressed: 0 bytes in 0 blocks.
# ID vs hostname: data for a given hostname
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -idVsHostname -hostname vmepcs2b17-37 2>/tmp/error.txt ; emacs /tmp/error.txt
# ID vs hostnane: data for a bad hostname
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -idVsHostname -hostname BLBLA 2>/tmp/error.txt ; emacs /tmp/error.txt
==24370== Invalid read of size 8
==24370==    at 0x543CF98: (within /afs/cern.ch/project/oracle/linux/10203/lib/libnnz10.so)
==24370==  Address 0x5A5A648 is 0 bytes after a block of size 120 allocd
==24370==    at 0x4004BBE: operator new[](unsigned) (vg_replace_malloc.c:197)
==24370==    by 0x44353A1: oracle::occi::StatementImpl::do_setSQL(void*, unsigned, void*, unsigned) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24370==    by 0x4435A96: oracle::occi::StatementImpl::StatementImpl(oracle::occi::ConnectionImpl*, std::string const&, std::string const&) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24370==    by 0x4423241: oracle::occi::ConnectionImpl::createStatement(std::string const&) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24370==    by 0x5666915: DbTkIdVsHostnameAccess::getSubDetectorCrateNumber(std::string, std::string&, unsigned&, unsigned, unsigned) (DbTkIdVsHostnameAccess.cc:214)
==24370==    by 0x55F5B1C: TkIdVsHostnameFactory::getSubDetectorCrateNumberFromHostname(std::string, std::string&, unsigned&, unsigned, unsigned, bool) (TkIdVsHostnameFactory.cc:429)
==24370==    by 0x80523FB: main (testVersionStateRun.cc:407)
==6155== LEAK SUMMARY:
==6155==    definitely lost: 0 bytes in 0 blocks.
==6155==      possibly lost: 0 bytes in 0 blocks.
==6155==    still reachable: 822,113 bytes in 3,968 blocks.
==6155==         suppressed: 0 bytes in 0 blocks.
# ID vs hostname: data for a given hostname and slot
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -idVsHostname -hostname vmepcs2b17-37 -slot 5 2>/tmp/error.txt ; emacs /tmp/error.txt
# ID vs hostname: data for a given hostname and bad slot 
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -idVsHostname -hostname vmepcs2b17-37 -slot 58 2>/tmp/error.txt ; emacs /tmp/error.txt
==6155== LEAK SUMMARY:
==6155==    definitely lost: 0 bytes in 0 blocks.
==6155==      possibly lost: 0 bytes in 0 blocks.
==6155==    still reachable: 822,113 bytes in 3,968 blocks.
==6155==         suppressed: 0 bytes in 0 blocks.
==24138== Invalid read of size 8
==24138==    at 0x543CF98: (within /afs/cern.ch/project/oracle/linux/10203/lib/libnnz10.so)
==24138==  Address 0x5A57648 is 0 bytes after a block of size 120 alloc d
==24138==    at 0x4004BBE: operator new[](unsigned) (vg_replace_malloc.c:197)
==24138==    by 0x44353A1: oracle::occi::StatementImpl::do_setSQL(void*, unsigned, void*, unsigned) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24138==    by 0x4435A96: oracle::occi::StatementImpl::StatementImpl(oracle::occi::ConnectionImpl*, std::string const&, std::string const&) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24138==    by 0x4423241: oracle::occi::ConnectionImpl::createStatement(std::string const&) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==24138==    by 0x5665DB1: DbTkIdVsHostnameAccess::getSubDetectorCrateNumber(std::string, std::string&, unsigned&, unsigned, unsigned) (DbTkIdVsHostnameAccess.cc:214)
==24138==    by 0x55F4FB8: TkIdVsHostnameFactory::getSubDetectorCrateNumberFromHostname(std::string, std::string&, unsigned&, unsigned, unsigned, bool) (TkIdVsHostnameFactory.cc:431)
==24138==    by 0x80523FB: main (testVersionStateRun.cc:407)

# ID vs hostname: upload of data: from file to file
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input /exports/slc4/development/Database/Utils/TkFileIdVsHostname.xml -output /tmp/coucou.xml -uploadIdVsHostname 2>/tmp/error.txt ; emacs /tmp/error.txt
==24342== LEAK SUMMARY:
==24342==    definitely lost: 0 bytes in 0 blocks.
==24342==      possibly lost: 0 bytes in 0 blocks.
==24342==    still reachable: 822,399 bytes in 3,974 blocks.
==24342==         suppressed: 0 bytes in 0 blocks.

# ID vs hostname: upload of data: from file to DB
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input /exports/slc4/development/Database/Utils/TkFileIdVsHostname.xml -output database -uploadIdVsHostname 2>/tmp/error.txt ; emacs /tmp/error.txt
# ID vs hostname: upload of data: from DB to file
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input database -output /tmp/coucou.xml -uploadIdVsHostname 2>/tmp/error.txt ; emacs /tmp/error.txt
==24328== LEAK SUMMARY:
==24328==    definitely lost: 0 bytes in 0 blocks.
==24328==      possibly lost: 0 bytes in 0 blocks.
==24328==    still reachable: 822,399 bytes in 3,974 blocks.
==24328==         suppressed: 0 bytes in 0 blocks.
==24312== Invalid read of size 8
==24312==    at 0x543D0D8: (within /afs/cern.ch/project/oracle/linux/10203/lib/libnnz10.so)
==24312==  Address 0x5990F68 is 152 bytes inside a block of size 154 allocd
==24312==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==24312==    by 0x4005728: realloc (vg_replace_malloc.c:306)
==24312==    by 0x487E257: nscall1 (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x487D668: nscall (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x48B889B: niotns (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x4928F9F: nigcall (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x48BCE3B: osncon (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x4604CC7: kpuadef (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x471E390: upiini (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x46FF29B: upiah0 (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x460452D: kpuatch (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==24312==    by 0x46CDBEB: kpuspsessionget (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)

# ##########################################################################################################
# PSU/DCU map
# From text file to database
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input /exports/slc4/development/Database/Utils/PsuCrack.txt -output database -partition CRACK_TKCC58 -uploadDcuPsuMap 2>/tmp/error.txt ; emacs /tmp/error.txt
# Download, create fake data and upload it in database
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testVersionStateRun.exe -dcupsumap -partition CRACK_TKCC58 2>/tmp/error.txt ; emacs /tmp/error.txt
# From database to file
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input database -output /tmp/psuname.xml -partition CRACK_TKCC58 -uploadDcuPsuMap 2>/tmp/error.txt ; emacs /tmp/error.txt
# From xml file to database
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input /tmp/psuname.xml -output database -partition CRACK_TKCC58 -uploadDcuPsuMap 2>/tmp/error.txt ; emacs /tmp/error.txt
==15886== LEAK SUMMARY:
==15886==    definitely lost: 0 bytes in 0 blocks.
==15886==      possibly lost: 0 bytes in 0 blocks.
==15886==    still reachable: 824,389 bytes in 4,012 blocks.
==15886==         suppressed: 0 bytes in 0 blocks.

# ##########################################################################################################
# PSU names
# From database to file
TBD> valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/TkDcuFileDb.exe -input database -output /tmp/psuname.xml -partition CRACK_TKCC58 -uploadPsuName 2>/tmp/error.txt ; emacs /tmp/error.txt

# ##########################################################################################################
# UploadInputOutput: upload a new partition
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/UploadInputOutput.exe -input $CONFDBDROUHIN -output $CONFDBINT2RTEST -partition MORESLC4_7 -uploadPartition CRACK_3 2>/tmp/error.txt ; emacs /tmp/error.txt
# UploadInputOutput: upload a new partition with a name already existing
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/UploadInputOutput.exe -input $CONFDBDROUHIN -output $CONFDBINT2RTEST -partition MORESLC4_7 -uploadPartition CRACK_3 2>/tmp/error.txt ; emacs /tmp/error.txt
# UploadInputOutput: from file to database
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/UploadInputOutput.exe -input file -output $CONFDBDROUHIN -partition /home/fd/fec1111.xml -uploadFecPartition UNTEST1 2>/tmp/error.txt ; emacs /tmp/error.txt
# UploadInputOutput: from database to file
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/UploadInputOutput.exe -input $CONFDBDROUHIN -output file -partition UNTEST1 -uploadFecPartition /tmp/coucou.xml 2>/tmp/error.txt ; emacs /tmp/error.txt

==28951== LEAK SUMMARY:
==28951==    definitely lost: 0 bytes in 0 blocks.
==28951==      possibly lost: 148 bytes in 4 blocks.
==28951==    still reachable: 823,438 bytes in 4,000 blocks.
==28951==         suppressed: 0 bytes in 0 blocks.
 This error is due to the following command: std::string p = std::string(argv[i+1]) ; for example

# ##########################################################################################################
# PopulateDB

# ##########################################################################################################
# FecProfiler: download from database / upload to database / download from database to hardware that is not existing
TBD> 

# ##########################################################################################################
# Commissioning procedure

--------------------------------------------------------------

#!/bin/sh
XDAQ_PLATFORM=x86_slc4
echo $XDAQ_PLATFORM
#valgrind -- valgrind --suppressions=xml/suppression.txt --leak-check=full --leak-resolution=high

onlyDB=0
if [ -z "$1" ]; then 
  FREDTEST=FREDTEST 
else
  FREDTEST=$1
fi

if [ -z "$2" ]; then
  FREDTEST1=FREDTEST1
else
  FREDTEST1=$2
  onlyDB=1
fi

echo $onlyDB

# Devices

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -devices
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -devices
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml  -devices
read y
fi

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fecHardwareId $FREDTEST1 -devices
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -devices
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -devices
read y
fi


# PIA

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -pia
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -pia
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -pia
read y
fi

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fecHardwareId $FREDTEST1 -pia
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -pia
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -pia
read y
fi


# DCU

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -dcu
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -dcu
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -dcu
read y
fi

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fecHardwareId $FREDTEST1 -dcu
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -dcu
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -dcu
read y
fi

# # CCU

# time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fecHardwareId $FREDTEST1 -ring 0 -ccu
# read y
# time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file ccu.xml -fecHardwareId $FREDTEST1 -ring 0 -ccu
# read y
# time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file http://cmstkint07/~xdaqtk/xml/FecOneFullPartition.xml -fecHardwareId $FREDTEST1 -ring 0 -ccu
# read y

# # DCU conversion
# In the DCU conversion files, 14,18,21 parameters are found
# in the database only 14 and 21 are available

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -dcuconversion
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -dcuHardId 7601227 -dcuconversion
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file ../../FecSupervisor/xml/DcuConversionFactors.xml -dcuconversion
read y
fi

# DCU Info
# In the parameter 5 parameters can be set but only 4 are sent from the database (time of flight is missing)

time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -dcuinfo
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition ALL -dcuinfo
read y
if [ $onlyDB = "0" ]; then
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -file xml/AllDcuInfo.xml -dcuinfo
read y
fi

# State information
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -state
read y

# Versions
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fecversion
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -fedversion
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -piaversion
read y

# Run
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -run
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -runO2O
read y
time bin/linux/$XDAQ_PLATFORM/XMLFecParse.exe -database -partition $FREDTEST -runNumber 100 -run
read y


# VALGRIND:






# 
# Sebastien BEL - January 2008 - test of commissioning analysis API with valgrind
# 

echo "************************************************************************"
echo "Commissioning analysis test:: "
echo ""
echo "> Test 1: Download from file:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high bin/linux/x86_slc4/testAnalysis.exe -dd -i XMLFILE xml/testanalysis.xml 2> ./error.txt

==21396== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 36 from 1)
==21396== malloc/free: in use at exit: 0 bytes in 0 blocks.
==21396== malloc/free: 1,019,150 allocs, 1,019,151 frees, 51,368,544 bytes allocated.
==21396== For counts of detected errors, rerun with: -v
==21396== All heap blocks were freed -- no leaks are possible.


echo "************************************************************************"
echo ""
echo "> Test 2: Missing parameters:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dh -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 2> error.txt

Please set the CONFDB environemental variable as CONFDB=login/password@path
Exiting ...

==25624== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 36 from 1)
==25624== malloc/free: in use at exit: 0 bytes in 0 blocks.
==25624== malloc/free: 19 allocs, 20 frees, 2,423 bytes allocated.
==25624== For counts of detected errors, rerun with: -v
==25624== All heap blocks were freed -- no leaks are possible.



echo "************************************************************************"
echo ""
echo "> Test 3: Download history (run, versions) from DB:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dh -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 2> error.txt


Numberous errors related to oracle occi (well known memory leak):

..................

==25648==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==25648==    by 0x4F26A2B: lxldalc (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x4F26C4D: lxldlbb (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x4F2A5C1: lxlinit (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x4D53B36: kpummpin (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x4660C1F: kpuenvcr (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x472E999: OCIEnvCreate (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25648==    by 0x442C638: oracle::occi::Environment::createEnvironment(oracle::occi::Environment::Mode, void*, void* (*)(void*, unsigned), void* (*)(void*, void*, unsigned), void (*)(void*, void*)) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==25648==    by 0x5671009: DbCommonAccess::dbConnect(std::string, std::string, std::string, bool) (DbCommonAccess.cc:104)
==25648==    by 0x5670927: DbCommonAccess::DbCommonAccess(std::string, std::string, std::string, bool) (DbCommonAccess.cc:59)
==25648==    by 0x5678A51: DbAccess::DbAccess(std::string, std::string, std::string, bool) (DbAccess.cc:52)
==25648==    by 0x567D503: DbFecAccess::DbFecAccess(std::string, std::string, std::string, bool) (DbFecAccess.cc:42)

..................

==25648== LEAK SUMMARY:
==25648==    definitely lost: 0 bytes in 0 blocks.
==25648==      possibly lost: 0 bytes in 0 blocks.
==25648==    still reachable: 824,390 bytes in 4,012 blocks.
==25648==         suppressed: 0 bytes in 0 blocks.




echo "************************************************************************"
echo ""
echo "> Test 4: Download descriptions from DB:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dd -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 -v 2.0 2> error.txt


Numberous errors related to oracle occi (well known memory leak):

..................

==25652== 22,288 bytes in 1 blocks are still reachable in loss record 260 of 264
==25652==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==25652==    by 0x4F26A2B: lxldalc (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x4F26C4D: lxldlbb (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x4F2A5C1: lxlinit (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x4D53B36: kpummpin (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x4660C1F: kpuenvcr (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x472E999: OCIEnvCreate (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25652==    by 0x442C638: oracle::occi::Environment::createEnvironment(oracle::occi::Environment::Mode, void*, void* (*)(void*, unsigned), void* (*)(void*, void*, unsigned), void (*)(void*, void*)) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==25652==    by 0x5671009: DbCommonAccess::dbConnect(std::string, std::string, std::string, bool) (DbCommonAccess.cc:104)
==25652==    by 0x5670927: DbCommonAccess::DbCommonAccess(std::string, std::string, std::string, bool) (DbCommonAccess.cc:59)
==25652==    by 0x5678A51: DbAccess::DbAccess(std::string, std::string, std::string, bool) (DbAccess.cc:52)
==25652==    by 0x567D503: DbFecAccess::DbFecAccess(std::string, std::string, std::string, bool) (DbFecAccess.cc:42)

..................

==25652== 
==25652== LEAK SUMMARY:
==25652==    definitely lost: 0 bytes in 0 blocks.
==25652==      possibly lost: 0 bytes in 0 blocks.
==25652==    still reachable: 824,390 bytes in 4,012 blocks.
==25652==         suppressed: 0 bytes in 0 blocks.




echo "************************************************************************"
echo ""
echo "> Test 5: Download descriptions from DB and upload to file:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dd -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 -v 2.0 -o XMLFILE xml/TIBTOB_04072007_COPY2_APVLATENCY_2_0.xml 2> error.txt


Numberous errors related to oracle occi (well known memory leak):

..................

==25653== 22,288 bytes in 1 blocks are still reachable in loss record 260 of 264
==25653==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==25653==    by 0x4F26A2B: lxldalc (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x4F26C4D: lxldlbb (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x4F2A5C1: lxlinit (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x4D53B36: kpummpin (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x4660C1F: kpuenvcr (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x472E999: OCIEnvCreate (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25653==    by 0x442C638: oracle::occi::Environment::createEnvironment(oracle::occi::Environment::Mode, void*, void* (*)(void*, unsigned), void* (*)(void*, void*, unsigned), void (*)(void*, void*)) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==25653==    by 0x5671009: DbCommonAccess::dbConnect(std::string, std::string, std::string, bool) (DbCommonAccess.cc:104)
==25653==    by 0x5670927: DbCommonAccess::DbCommonAccess(std::string, std::string, std::string, bool) (DbCommonAccess.cc:59)
==25653==    by 0x5678A51: DbAccess::DbAccess(std::string, std::string, std::string, bool) (DbAccess.cc:52)
==25653==    by 0x567D503: DbFecAccess::DbFecAccess(std::string, std::string, std::string, bool) (DbFecAccess.cc:42)

..................

==25653== LEAK SUMMARY:
==25653==    definitely lost: 0 bytes in 0 blocks.
==25653==      possibly lost: 0 bytes in 0 blocks.
==25653==    still reachable: 824,390 bytes in 4,012 blocks.
==25653==         suppressed: 0 bytes in 0 blocks.




echo "************************************************************************"
echo ""
echo "> Test 6: Download descriptions from DB and upload to DB:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dd -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 -v 2.0 -o DATABASE 2> error.txt


Numberous errors related to oracle occi (well known memory leak):

..................

==25654== 22,288 bytes in 1 blocks are still reachable in loss record 260 of 264
==25654==    at 0x4004405: malloc (vg_replace_malloc.c:149)
==25654==    by 0x4F26A2B: lxldalc (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x4F26C4D: lxldlbb (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x4F2A5C1: lxlinit (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x4D53B36: kpummpin (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x4660C1F: kpuenvcr (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x472E999: OCIEnvCreate (in /afs/cern.ch/project/oracle/linux/10203/lib/libclntsh.so.10.1)
==25654==    by 0x442C638: oracle::occi::Environment::createEnvironment(oracle::occi::Environment::Mode, void*, void* (*)(void*, unsigned), void* (*)(void*, void*, unsigned), void (*)(void*, void*)) (in /afs/cern.ch/project/oracle/linux/10203/lib/libocci.so.10.1)
==25654==    by 0x5671009: DbCommonAccess::dbConnect(std::string, std::string, std::string, bool) (DbCommonAccess.cc:104)
==25654==    by 0x5670927: DbCommonAccess::DbCommonAccess(std::string, std::string, std::string, bool) (DbCommonAccess.cc:59)
==25654==    by 0x5678A51: DbAccess::DbAccess(std::string, std::string, std::string, bool) (DbAccess.cc:52)
==25654==    by 0x567D503: DbFecAccess::DbFecAccess(std::string, std::string, std::string, bool) (DbFecAccess.cc:42)

..................

==25654== LEAK SUMMARY:
==25654==    definitely lost: 0 bytes in 0 blocks.
==25654==      possibly lost: 0 bytes in 0 blocks.
==25654==    still reachable: 824,390 bytes in 4,012 blocks.
==25654==         suppressed: 0 bytes in 0 blocks.




echo "************************************************************************"
echo ""
echo "> Test 6: simulate a PL/SQL error: pkganalysisxml has to be removed BEFORE executing this command line:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dd -i DATABASE -t APVLATENCY -p TIBTOB_04072007_COPY2 -v 2.0 -o DATABASE 2> error.txt


ERROR: Error during the access to the database: cms_tracker_drouhin/chukrut2006@devdb10
Tue Jan 15 10:09:13 2008
XML parsing error
XMLCommissioningAnalysis::getAnalysisHistory2 -> Unable to retrieve analysis datas: ORA-06550: line 1, column 19:
PLS-00201: identifier 'PKGANALYSISXML.GETDOWNLOADANALYSISHISTORY' must be declared
ORA-06550: line 1, column 7:
PL/SQL: Statement ignored

PL/SQL Exception
ORA-01031: insufficient privileges
PkgIdVsHostnameXML.configureXMLClob;
Error Code: 1410

..................

Always the leak due to oracle occi environment code:


==26468== LEAK SUMMARY:
==26468==    definitely lost: 0 bytes in 0 blocks.
==26468==      possibly lost: 0 bytes in 0 blocks.
==26468==    still reachable: 824,390 bytes in 4,012 blocks.
==26468==         suppressed: 0 bytes in 0 blocks.




echo "************************************************************************"
echo ""
echo "> Test 7: retrieves local analysis versions by global version number - existing global version:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dv 127 -i DATABASE 2> error.txt

Test performed on april 2008. Changes occurs in common code and this is because we have an increasing of still reachable blocks
by comparing with previous tests. This is due to lazy oracle behaviour regarding the resources releasing

==9137== 
==9137== LEAK SUMMARY:
==9137==    definitely lost: 0 bytes in 0 blocks.
==9137==      possibly lost: 0 bytes in 0 blocks.
==9137==    still reachable: 830,544 bytes in 4,056 blocks.
==9137==         suppressed: 0 bytes in 0 blocks.


echo "************************************************************************"
echo ""
echo "> Test 8: retrieves local analysis versions by global version number - inexisting global version:"
echo ""
valgrind --suppressions=xml/suppression.txt --show-reachable=yes --leak-check=full --leak-resolution=high ./bin/linux/x86_slc4/testAnalysis.exe -dv 0 -i DATABASE 2> error.txt


==9263== LEAK SUMMARY:
==9263==    definitely lost: 0 bytes in 0 blocks.
==9263==      possibly lost: 0 bytes in 0 blocks.
==9263==    still reachable: 830,544 bytes in 4,056 blocks.
==9263==         suppressed: 0 bytes in 0 blocks.


