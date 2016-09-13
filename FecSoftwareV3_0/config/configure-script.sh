#!/bin/sh

# Use cases:

#  Help on configure
#    FecSoftwareV3_0 $> configure --help

#  If XDAQ_ROOT is set in environment variable, you do not need to define:
#         --with-xdaq-path=

#  XDAQ_OS and XDAQ_PLATFORM
#
#    - if XDAQ_OS and XDAQ_PLATFORM are set in environment variables, you do not need to define:
#        --with-xdaq-os=
#        --with-xdaq-platform=
#
#    - if they are not set then:
#       * XDAQ SLC3
#           --with-xdaq-os=linux 
#           --with-xdaq-platform=x86
#       * XDAQ RPM / SLC4
#           --with-xdaq-os=linux 
#           --with-xdaq-platform=x86_slc4

#  XDAQ source vs. RPM installation 
#
#    - sources
#          --disable-xdaq_rpmbuild
#    - RPM
#           --enable_xdaq_rpmbuild (or do not define, it's the default)


# Standalone applications
#      RPM compilation
#      No oracle support (do not need it for standalone applications)
#      No Tracker XDAQ support (Diagnostic system, FED, Tracker online supports)
#      No USB FEC compilation
#      No SBS interface
#      No PCI FEC compilation
#      CAEN interface support

./configure \
        --with-xdaq-path=/opt/xdaq \
        --with-hal-path=/opt/xdaq \
        --with-xerces-path=/exports/xdaq/xdaq-3.11/TriDAS/daq/extern/xerces/xerces-c-src_2_7_0 \
        --with-xdaq-os=linux \
        --with-xdaq-platform=x86_slc4 \
        --without-oracle-path \
        --without-fed9u-path \
        --without-trkonline-path \
        --without-diag-path \
        --disable-pcifec \
        --disable-usbfec \
        --with-caen-path=/usr/local/CAENVME/Linux

# Tracker configure usage

./configure \
        --with-xdaq-path=/opt/xdaq \
        --with-hal-path=/opt/xdaq \
        --with-xerces-path=/exports/xdaq/xdaq-3.11/TriDAS/daq/extern/xerces/xerces-c-src_2_7_0 \
        --with-xdaq-os=linux \
        --with-xdaq-platform=x86_slc4 \
        --without-oracle-path \
        --without-fed9u-path \
        --without-trkonline-path \
        --without-diag-path \
        --disable-pcifec \
        --disable-usbfec \
        --with-caen-path=/usr/local/CAENVME/Linux

# From cmstracker029
./configure --with-xdaq-path=/opt/xdaq --with-xdaq-os=linux --with-xdaq-platform=x86_slc4 --with-hal-path=/opt/xdaq/lib --with-oracle-path=/afs/cern.ch/project/oracle/@sys/10203 --with-xerces-path=/opt/xdaq/lib --with-fed9u-path=/exports/slc4/development/opt/trackerDAQ/lib --with-caen-path=/opt/xdaq/lib/ --with-trkonline-path=/exports/slc4/development/opt/trackerDAQ/lib/ --with-diag-path=/exports/slc4/development/opt/trackerDAQ/lib/

# From cmstracker029 for fPIC
./configure --with-xdaq-path=/opt/xdaq --with-xdaq-os=linux --with-xdaq-platform=x86_64 --with-hal-path=/opt/xdaq/lib --with-oracle-path=/afs/cern.ch/project/oracle/@sys/10203 --with-xerces-path=/opt/xdaq/lib --with-fed9u-path=/exports/slc4/development/opt/trackerDAQ/lib --with-caen-path=/opt/xdaq/lib/ --with-trkonline-path=/exports/slc4/development/opt/trackerDAQ/lib/ --with-diag-path=/exports/slc4/development/opt/trackerDAQ/lib/


