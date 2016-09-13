Summary
-------
1) Software needed
2) Environment variables
3) Compilation
   3.1) libDeviceAccess.so shared library
   3.2) Compiling the GUIDebugger

1) Software needed
You need QT version 3.0.5 in order to compile it.

2) Environment variables
Be sure that the environment variables are set to the correct path:
  - QTDIR: root directory of QT
  - LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib/
  - PATH=$PATH:$QTDIR/bin/
  - FECSOFTWARE_ROOT=$PREFIX/FecSoftware

You need also the libDeviceAccess.so shared library. To compile it,
use the FecProfiler makefile (cf. paragraph 3). Note that if you 
use the configure.sh script, the library is available on:
  - FecSoftware/FecSupervisor/src/linux/x86

The LD_LIBRARY_PATH for the libDeviceAccess.so must also be added:
  - LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/FecSoftware/FecSupervisor/src/linux/x86:$QTDIR/lib/
depend of the root directory of the FecSoftware.

The compilation use also the xerces library available on the TriDAS
core software or in the xerces web site (http://xml.apache.org/).
The tests are performed with the version 1.6 delivered with TriDAS
core software.

3) Compilation

3.1) libDeviceAccess.so

$> cd FecSoftware/FecSupervisor/src/linux/x86

3.2) Compiling the GUIDebugger
Once all variables are set:
$> cd FecSoftware/ThirdParty/GUIDebugger/src
$> qmake
$> make -f GUIDebugger.mak

The use of GUIDebugger.mak is mandatory because of rules for the FEC software.

Note that when you build the complete software, many warnings will be
displayed due to the version of xerces libraries that not completely
well defined. The next version (2.0) will avoid this problems.
Example of problems:
../../../../TriDAS/Auxiliary/xerceslinuxx86/include/util/NameIdPool.c: In method
`NameIdPoolEnumerator<TElem>::NameIdPoolEnumerator (const NameIdPoolEnumerator<TElem> &) [with TElem = DTDEntityDecl]':
../../../../TriDAS/Auxiliary/xerceslinuxx86/include/internal/XMLScanner.hpp:1108:   instantiated from here
../../../../TriDAS/Auxiliary/xerceslinuxx86/include/util/NameIdPool.c:358: warning: base class `class XMLEnumerator<DTDEntityDecl>' should be explicitly initialized in the copy constructor

