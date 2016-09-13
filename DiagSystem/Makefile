#FileName : 		Makefile
#Content :		Makefile for Tracker Diagnostic System
#Programmer : 		Laurent GROSS
#Version : 		TDS 4.0
#Last modification date	19/12/2006
#Support : 		mail to : laurent.gross@ires.in2p3.fr
#online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem

# KH
include DiagHeader.linux
LIB_DEST ?= tools/lib
LIB_FILES = modules/*/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/*.so \
	tools/*/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)/*.so


TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY) -p -f
COPY_DIR = $(COPY) -pR
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
CHK_FILE_EXISTS= test -f
MKDIR    = mkdir -p
QMAKE    = qmake
TOUCH    = touch

display:
	@echo
	@echo MAKE OPTION ARE
	@echo ---------------
	@echo
	@echo make all / make clean
	@echo ---------------------
	@echo Compiles/Cleans up diagnostic system tools and modules.
	@echo
	@echo make tools / make tools_clean
	@echo -----------------------------
	@echo Compiles/Cleans up diagnostic system tools.
	@echo
	@echo make modules / make modules_clean
	@echo ---------------------------------
	@echo Compiles/Cleans up diagnostic system modules.
	@echo

install: install_libs

#all: tools modules install_libs
all: tools modules install_libs

clean: tools_clean modules_clean libs_clean

tools: soapdiagappender xmldiagappender diagbag

tools_clean: diagbag_clean soapdiagappender_clean xmldiagappender_clean

#modules: DummySupervisor DummySupervisorDiagLvlOne ErrorDispatcher FecSupervisorDiagLvlOne DiagSentinelErrorsGrabber FFApplication LogReader
modules: DummySupervisor GlobalErrorDispatcher AjaxLogReader ErrorDbUploader #AjaxLogReaderWithFileServer

#modules_clean: DummySupervisor_clean DummySupervisorDiagLvlOne_clean ErrorDispatcher_clean FecSupervisorDiagLvlOne_clean DiagSentinelErrorsGrabber_clean FFApplication_clean LogReader_clean
modules_clean: DummySupervisor_clean GlobalErrorDispatcher_clean AjaxLogReader_clean ErrorDbUploader_clean #AjaxLogReaderWithFileServer_clean

diagbag: 
	( cd tools/diagbag ; ./compile ; cd - )

diagbag_clean:
	( cd tools/diagbag ; ./clean ; cd - )

soapdiagappender:
	( cd tools/soapdiagappender ; $(MAKE) ; cd - )

soapdiagappender_clean:
	( cd tools/soapdiagappender ; $(MAKE) clean ; cd - )

xmldiagappender:
	( cd tools/xmldiagappender ; $(MAKE) ; cd - )

xmldiagappender_clean:
	( cd tools/xmldiagappender ; $(MAKE) clean ; cd - )

DummySupervisor: tools
	( cd modules/DummySupervisor ; $(MAKE) ; cd - )

DummySupervisor_clean:
	( cd modules/DummySupervisor ; $(MAKE) clean ; cd - )

DummySupervisorDiagLvlOne:
	( cd modules/DummySupervisorDiagLvlOne ; $(MAKE) ; cd - )

DummySupervisorDiagLvlOne_clean:
	( cd modules/DummySupervisorDiagLvlOne ; $(MAKE) clean ; cd - )

LocalErrorDispatcher: tools
	( cd modules/LocalErrorDispatcher ; $(MAKE) ; cd - )

LocalErrorDispatcher_clean:
	( cd modules/LocalErrorDispatcher ; $(MAKE) clean ; cd - )

GlobalErrorDispatcher: tools
	( cd modules/GlobalErrorDispatcher ; $(MAKE) ; cd - )

GlobalErrorDispatcher_clean:
	( cd modules/GlobalErrorDispatcher ; $(MAKE) clean ; cd - )

LogReader: tools
	( cd modules/LogReader ; $(MAKE) ; cd - )

LogReader_clean:
	( cd modules/LogReader ; $(MAKE) clean ; cd - )

AjaxLogReader: tools
	( cd modules/AjaxLogReader ; $(MAKE) ; cd - )

AjaxLogReader_clean:
	( cd modules/AjaxLogReader ; $(MAKE) clean ; cd - )

AjaxLogReaderWithFileServer: tools
	( cd modules/AjaxLogReaderWithFileServer ; $(MAKE) ; cd - )

AjaxLogReaderWithFileServer_clean:
	( cd modules/AjaxLogReaderWithFileServer ; $(MAKE) clean ; cd - )

FecSupervisorDiagLvlOne:
	( cd modules/FecSupervisorDiagLvlOne ; $(MAKE) ; cd - )

FecSupervisorDiagLvlOne_clean:
	( cd modules/FecSupervisorDiagLvlOne ; $(MAKE) clean ; cd - )

DiagSentinelErrorsGrabber:
	( cd modules/DiagSentinelErrorsGrabber ; $(MAKE) ; cd - )

DiagSentinelErrorsGrabber_clean:
	( cd modules/DiagSentinelErrorsGrabber ; $(MAKE) clean ; cd - )

FFApplication:
	( cd modules/FFApplication ; $(MAKE) ; cd - )

FFApplication_clean:
	( cd modules/FFApplication ; $(MAKE) clean ; cd - )


ReconfigurationModule: tools
	( cd modules/ReconfigurationModule ; $(MAKE) ; cd - )

ReconfigurationModule_clean:
	( cd modules/ReconfigurationModule ; $(MAKE) clean ; cd - )

ErrorDbUploader: tools
	( cd modules/ErrorDbUploader ; $(MAKE) ; cd - )

ErrorDbUploader_clean:
	( cd modules/ErrorDbUploader ; $(MAKE) clean ; cd - )


# KH modified
install_libs:
	if [ ! -e lib ] ; \
	then ${MKDIR} tools/lib; \
	fi; \
	cp -rf $(LIB_FILES) $(LIB_DEST)

libs_clean:
	(rm -f tools/lib/*.so;)
