TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= ${COPY} -p
COPY_DIR = ${COPY} -pR
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
CHK_FILE_EXISTS= test -f
MKDIR    = mkdir -p
QMAKE    = qmake
ECHO     = echo

all: TotemGenericMake vfat_controllerMake 

clean: TotemGenericMake_clean vfat_controller_clean 

vfat_controllerMake:
	( cd vfat_controller ; ${MAKE} -f commands/Makefile; cd ../.. )

vfat_controller_clean:
	( cd vfat_controller ; ${MAKE} -f commands/Makefile clean ; cd ../.. )

TotemGenericMake:
	( cd generic ; $(MAKE) Library=TotemDeviceAccess ; $(MAKE) Library=TotemDeviceDescriptions ; cd .. )

TotemGenericMake_clean:
	( cd generic ; $(MAKE) Library=TotemDeviceAccess clean ; $(MAKE) Library=TotemDeviceDescriptions clean ; cd .. )
install_complete:

install:

