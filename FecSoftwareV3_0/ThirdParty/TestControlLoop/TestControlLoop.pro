SOURCES += src/Main.cpp \
        src/TestConfigurationImpl.cpp \
        src/PSCurrentStoreImpl.cpp \
        src/TestResultsImpl.cpp \
        src/PowerUpStepsImpl.cpp \
        src/GUISupervisor.cpp \
        src/FecRingSelectionImpl.cpp \
        src/ModuleInfoImpl.cpp \
        src/LoadPreferencesImpl.cpp \
        src/SavePreferences.cpp \
        src/DCUThresholdsImpl.cpp \
        src/CCUInfoImpl.cpp \
        src/LoadResultsImpl.cpp \
        src/DirChoiceImpl.cpp
HEADERS += include/TestConfigurationImpl.h \
        include/PowerUpStepsImpl.h \
        include/SavePreferencesImpl.h \
        include/LoadPreferencesImpl.h \
        include/ModuleInfoImpl.h \
        include/PSCurrentStoreImpl.h \
        include/TestResultsImpl.h \
        include/GUISupervisor.h \
        include/FecRingSelectionImpl.h \
        include/DCUThresholdsImpl.h \
        include/CCUInfoPage.h \
        include/CCUInfoImpl.h \
        include/ModuleInfoPage.h \
        include/LoadResultsImpl.h \
        include/DirChoiceImpl.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS   = ccuinfo.ui \
        dcuthresholds.ui \
        loadpreferences.ui \
        moduleinfo.ui \
        powerupsteps.ui \
        pscurrentstore.ui \
        savepreferences.ui \
        testconfiguration.ui \
        testresults.ui \
        fecringselection.ui \
        loadresults.ui \
        dirchoice.ui
TEMPLATE	=app
CONFIG	+= qt warn_on exceptions debug
DEFINES	+= I2C100 BT1003 LINUX

INCLUDEPATH	+= ./include/ \
  ${ENV_CMS_TK_FEC_ROOT}/FecPciDeviceDriver/include/ \
  ${ENV_CMS_TK_FEC_ROOT}/FecUsbDeviceDriver/include/ \
  ${ENV_CMS_TK_FEC_ROOT}/generic/include \
  ${ENV_CMS_TK_FEC_ROOT}/ThirdParty/APIConsoleDebugger/include/ \
  ${ENV_CMS_TK_HAL_ROOT}/generic/include \
  ${ENV_CMS_TK_HAL_ROOT}/busAdapter/include \
  ${ENV_CMS_TK_HAL_ROOT}/utilities/include \
  ${ENV_CMS_TK_HAL_ROOT}/generic/include/linux \
  ${ENV_CMS_TK_SBS_ROOT}/include ${ENV_CMS_TK_CAEN_ROOT}/include \
  ${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
  ${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
  ${XDAQ_ROOT}/include \
  ${XDAQ_ROOT}/include/${XDAQ_OS} \

LIBS	+= \
  -L${ENV_CMS_TK_FEC_ROOT}/lib/ -lAPIFecVme \
  -L${ENV_CMS_TK_SBS_ROOT}/lib -lSBSLinuxX86BusAdapter -lbtp \
  -L${XDAQ_ROOT}/lib/ -L${XDAQ_ROOT}/${OS_PLATFORM}/lib/ -lGenericHAL \
  -lxerces-c \
  -L${ENV_CMS_TK_FEC_ROOT}/FecPciDeviceDriver -lfec_glue \
  -L${ENV_CMS_TK_FEC_ROOT}/FecUsbDeviceDriver/lib/linux/x86/ -lfec_usb_glue \
  -lpthread_nonshared

XDAQ_ROOT = $${XDAQ_ROOT}
LIBS += -L${ENV_CMS_TK_CAEN_ROOT}/lib -lCAENLinuxBusAdapter -lCAENVME 
INCLUDEPATH += ${XDAQ_ROOT}/daq/xcept/include
LIBS += -L${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib -lxcept

LANGUAGE	= C++
