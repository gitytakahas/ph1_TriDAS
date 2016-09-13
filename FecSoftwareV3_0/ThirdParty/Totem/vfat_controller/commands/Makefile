#############################################################################
# Makefile for building: vfat_controller
#############################################################################

include ../../../FecHeader.linux
include ../../../config/FecRules.linux

include commands/Makefile.settings

include ${XDAQ_ROOT}/config/mfAutoconf.rules
BUILD_HOME=${FECSOFTWARE_ROOT}

# Add -I in front of every include directory
PackageGUIIncludeDirs = ${XDAQ_ROOT}/include \
			${XDAQ_ROOT}/${XDAQ_PLATFORM}/include \
			${XDAQ_ROOT}/${XDAQ_PLATFORM}/include/${XDAQ_OS} \
			${FECSOFTWARE_ROOT}/generic/include \
			${FECSOFTWARE_ROOT}/ThirdParty/Totem/generic/include \
			${BUSADAPTERINCLUDEDIR} \
			${ORACLE_INCLUDE}

IncludeGUIPaths = ${PackageGUIIncludeDirs:%=-I%} ${VFAT_CONTROLLER_INCS:%=-I%}


# Add -L in front of every library directory
LibraryGUIPaths=${XDAQ_ROOT}/lib \
		${XDAQ_ROOT}/${XDAQ_PLATFORM}/lib \
		${FECSOFTWARE_ROOT}/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
		${FECSOFTWARE_ROOT}/ThirdParty/Totem/generic/lib/${XDAQ_OS}/${XDAQ_PLATFORM} \
		${BUSADAPTERLIBDIR} \
		${ORACLE_LDDFLAGS} \
		${FED_LIBDIR}

DependentGUILibraryPaths	= ${LibraryGUIPaths:%=-L%} ${VFAT_CONTROLLER_LIBPATHS:%=-L%}


LibraryGUIList			= ${XERCESLIB} ${ORACLE_LIB} \
				  ${HALLIB} ${BUSADAPTERLIB} DeviceAccess \
				  DeviceDescriptions ${FED_LIB}	\
				  TotemDeviceAccess TotemDeviceDescriptions

DependentGUILibraryList		= ${LibraryGUIList:%=-l%} ${VFAT_CONTROLLER_LIBS:%=-l%}


####### Compiler, tools and options

CXX      = g++

CXXFLAGS = -g -DBT1003 -w -DQT_THREAD_SUPPORT -DREENTRANT \
	   ${FECSOFT_C++FLAGS} ${XMLPARSER_C++FLAGS} ${ORACLE_C++FLAGS} ${BUSADAPTERC++FLAG} ${VFAT_CONTROLLERC++FLAGS}\
	   -Wall -W -I/usr/include/freetype2 -O2 -DQT_NO_DEBUG -DBT1003 -DLINUX
INCPATH  = -I${QTDIR}/include -I./. -I./src/ui -I${QTDIR}/mkspecs/default ${IncludeGUIPaths}

LINK     = g++
LIBS     = ${SUBLIBS} -Wl,-rpath,${QTDIR}/lib -L${QTDIR}/lib -L/usr/X11R6/lib \
	   ${DependentGUILibraryPaths} ${DependentGUILibraryList} \
	   -lqt-mt -lXext -lX11 -lm
MOC      = ${QTDIR}/bin/moc
UIC      = ${QTDIR}/bin/uic
DEL_FILE = rm -f


####### Files

OBJECTS  = bin/main.o \
	   bin/0_Start.o \
	   bin/1_Control.o \
	   bin/1_Panel.o \
	   bin/2_Scans.o \
	   bin/2_Panel.o \
	   bin/4_Panel.o \
	   bin/4_Coincidence.o \
	   bin/VfatDialog.o \
           bin/VFATFrame.o \
           ${VFAT_CONTROLLER_OBJS}

UICDECLS = src/ui/VfatDialog.h
UICIMPLS = src/ui/VfatDialog.cpp

SRCMOC   = src/ui/moc_VfatDialog.cpp
OBJMOC   = bin/moc_VfatDialog.o

TARGET   = bin/vfat_controller

first: all
####### Implicit rules

.SUFFIXES: .c .cpp .cc .cxx .C

.cpp.o:
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o $@ $<

.cc.o:
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o $@ $<

.cxx.o:
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o $@ $<

.C.o:
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o $@ $<

.c.o:
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o $@ $<

####### Build rules

all: commands/Makefile ${TARGET}

${TARGET}:  ${UICDECLS} ${OBJECTS} ${OBJMOC} 
	${LINK} -o ${TARGET} ${OBJECTS} ${OBJMOC} ${LIBS}

${MOC}: 
	( cd ${QTDIR}/src/moc ; ${MAKE} }

mocclean:
	-${DEL_FILE} ${OBJMOC}
	-${DEL_FILE} ${SRCMOC}

uiclean:
	-${DEL_FILE} ${UICIMPLS} ${UICDECLS}

clean: mocclean uiclean
	-${DEL_FILE} ${OBJECTS} ${VFAT_CONTROLLER_OBJS}
	-${DEL_FILE} *~ core *.core


####### Sub-libraries

distclean: clean
	-${DEL_FILE} ${TARGET} 

####### Compile

bin/main.o: src/main.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/main.o src/main.cc

bin/0_Start.o: src/0_Start.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/0_Start.o src/0_Start.cc

bin/1_Control.o: src/1_Control.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/1_Control.o src/1_Control.cc

bin/1_Panel.o: src/1_Panel.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/1_Panel.o src/1_Panel.cc

bin/2_Scans.o: src/2_Scans.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/2_Scans.o src/2_Scans.cc

bin/2_Panel.o: src/2_Panel.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/2_Panel.o src/2_Panel.cc

bin/3_ChipTesting.o: src/3_ChipTesting.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/3_ChipTesting.o src/3_ChipTesting.cc

bin/3_Panel.o: src/3_Panel.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/3_Panel.o src/3_Panel.cc

bin/4_Panel.o: src/4_Panel.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/4_Panel.o src/4_Panel.cc

bin/4_Coincidence.o: src/4_Coincidence.cc src/vfat_controller.h src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/4_Coincidence.o src/4_Coincidence.cc

bin/GPIBDevice.o: src/GPIBDevice.cc
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/GPIBDevice.o src/GPIBDevice.cc

bin/VFATFrame.o: src/VFATFrame.cc
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/VFATFrame.o src/VFATFrame.cc

src/ui/VfatDialog.h: src/ui/VfatDialog.ui 
	${UIC} src/ui/VfatDialog.ui -o src/ui/VfatDialog.h

src/ui/VfatDialog.cpp: src/ui/VfatDialog.h src/ui/VfatDialog.ui 
	${UIC} src/ui/VfatDialog.ui -i src/ui/VfatDialog.h -o src/ui/VfatDialog.cpp

bin/VfatDialog.o: src/ui/VfatDialog.cpp src/ui/VfatDialog.ui src/ui/VfatDialog.h
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/VfatDialog.o src/ui/VfatDialog.cpp

bin/moc_VfatDialog.o: src/ui/moc_VfatDialog.cpp src/ui/VfatDialog.h 
	${CXX} -c ${CXXFLAGS} ${INCPATH} -o bin/moc_VfatDialog.o src/ui/moc_VfatDialog.cpp

src/ui/moc_VfatDialog.cpp: ${MOC} src/ui/VfatDialog.h
	${MOC} src/ui/VfatDialog.h -o src/ui/moc_VfatDialog.cpp

####### Install

install: all 

