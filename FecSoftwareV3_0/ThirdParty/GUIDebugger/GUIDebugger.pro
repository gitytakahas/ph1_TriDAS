# ---------------------------------------------------------------------
# Root directory of the FecSoftware must be defined in FECSOFTWARE_ROOT
# note that this env. is defined in the fec_profile given by configure.sh
# export FECSOFTWARE_ROOT=../../..

#include ( ../../../FecHeader.linux )
#include ( ../../../config/FecRules.linux )


# Sources and headers
SOURCES	+= src/MainFecDebugger.cc src/FecDialogImpl.cc 
HEADERS	+= include/FecDialogImpl.h include/GroupCcuRedundancy.h

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

linux-g++:TMAKE_CXXFLAGS = -DBT1003 -w -DQT_THREAD_SUPPORT -DREENTRANT 

# ---------------------------------------------------------------------
# includes for this project: 
INCLUDEPATH += ./include

# ---------------------------------------------------------------------
# FEC device driver
INCLUDEPATH += $${FECSOFTWARE_ROOT}/DeviceDriver/includes /usr/include/kde
unix:LIBS += -L$${FECSOFTWARE_ROOT}/DeviceDriver -lfec_glue

# ---------------------------------------------------------------------
# FEC USB device driver
INCLUDEPATH += $${FECSOFTWARE_ROOT}/FecUsbDriver/include /usr/include/kde
unix:LIBS += -L$${FECSOFTWARE_ROOT}/FecUsbDriver/lib/linux/x86 -lfec_usb_glue

# ---------------------------------------------------------------------
# FEC API
INCLUDEPATH += $${FECSOFTWARE_ROOT}/FecSupervisor/include $${FECSOFTWARE_ROOT}/generic/include 
unix:LIBS += -L$${FECSOFTWARE_ROOT}/lib -lDeviceAccess

# ---------------------------------------------------------------------
# XERCES
INCLUDEPATH += $${XERCESCROOT}/include
unix:LIBS += -L$${XERCESCROOT}/lib -lxerces-c1_6_0

# ---------------------------------------------------------------------
# OTHERS
#INCLUDEPATH	 += ${QWTDIR}/include
#unix:LIBS	+= -L${QTDIR}/lib -lqwt -lkdeui

FORMS	= FecDialog.ui
TEMPLATE	= app
CONFIG	+= qt warn_on release
DBFILE	= 
LANGUAGE	= C++

