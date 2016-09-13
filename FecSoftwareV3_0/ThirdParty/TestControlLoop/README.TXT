This tools is used to test a complete FEC ring with: FEC, Ring, CCU, Channel, Modules, DCU on CCU, DOH and the redundancy.

A complete documentation is available on:
http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/slides/docs/FecSoftware/InterfaceForControlLoopTest.pdf

To compile this software, if you are using FecSoftware, just use the script configure.sh. If you are using FecSoftwareV2_0, please define following environement variable must be defined:
  * XDAQ_ROOT: TriDAS directory
  * FECSOFTWARE_ROOT: FecSoftwareV2_0 depend of the version you are compiling
  * HALDIR: Hardware access library (HAL) directory, normally in $XDAQ_ROOT/daq/hal
  * SBS_DIR: SBS directory (/usr/local/SBS/1003/v2p3p0/) if you are using this version
  * CAEN_DIR: CAEN directory (usr/local/CAEN/CAEN-VME/Linux/). This option is not mandatory till the update of ths file (02/06/2005).
