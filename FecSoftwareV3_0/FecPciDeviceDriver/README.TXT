INPUT FOR 09/10/2006
--------------------
- Warning emitted in /var/log/message about timers not
initialised do not occurs anymore

INPUT FOR 29/09/2006
--------------------
- FEC Driver can now be compiled under kernel 2.4.X or 2.6.X
- Make commands have not changed and are backwards compatibles
- If compiling under a 2.6.X system, check in Makefile that the path to
the 2.6.X kernel sources is correct before typing Make. Search for the line
"KERNELDIR ?=" in Makefile to know where to change this information

INPUT FOR 02/09/2005
--------------------
- Interrupts management has changed.
- IF compiled with the DD_ENABLE_IRQMANAGER flag, the driver will acts as usual.
- IF compiled without the DD_ENABLE_IRQMANAGER flag, therefore if no interrupt routine is installed,
all calls related to interrupt management and especially calls related to write into the 
PLX INTERRUPT register will return DD_RETURN_OK, but will not touch anymore to the PLX register.
This, for avoid PC freezing as seen in US during month 09/2005.
In the case where no interrupt manager is compiled with the driver, only one access to the
PLX IRQ register is performed at load time in order to swith off the PLX IRQ line. This is the only
access performed to the PLX INTERRUPT register.


INPUT FOR 02/09/2005
--------------------
- Update of the device driver in order to check the FIFO transmit before any sending

INPUT FOR 25/05/2005
------------------
- Release tested and validated on CERN benches
- The script INSTALL can NOT BE launched by hand anymore.
  You MUST use the command -make load- to load the driver on your system.


INPUT FOR 03/05/2005
------------------
-First unofficial release of the FEC driver version "Unified-6.0"
-The driver can now handle both electrical and optical fecs, and
auto-detect at load time which type of fec it has to manage.
-Corrected one bug in the install script, with awk messing the kernel
when trying to extract the major of the driver but where no driver is loaded.

-Added a glue function glue_fec_get_driver_type().
A returned value equal to 0 means that the driver is
associated to an electrical FEC
A returned value equal to 1 means that the driver is
associated to an optical FEC

-No more need of the file version.h, at least for the driver.
As the current version of FecSoftware still needs it for
driver versionning, I will keep it in the distro 6.0 for
back compatibility purposes.

-Added a configuration file driver.conf for the driver
specs (minor, major,device name, module name, fec number).
The file driver_conf.h is generated at make time from this
configuration file.

-The Makefile still holds the hooks "electrical" and "optical"
as make commands, for back compatibility with FecSupervisor purposes.

INPUT FOR 02/05/2005
------------------
-Corrected bug in the PCI devices parsing function. Now,
trying to load an electrical driver on a system equiped with
no fec or an optical fec, and opposite, should not freeze the
computer anymore.

INPUT FOR 17/01/05
------------------
- Added the address definition of registers SOURCe and VERSION in the file
define.h (defined as DD_FEC_SOURCE_OFFSET and DD_FEC_VERSION_OFFSET)
- Added the functions glue_fec_get_source_register and
glue_fec_get_version_register in glue.c and glue.h

INPUT FOR 03/01/05
------------------
CVS version switched back to latest stable version.
This, until the irq_flag problem is solved.



INPUT FOR 15/06/04
------------------
- A patch has been applied to the interrupt function in order to take care 
of some errors which can appear (physically) on FEC boards (sometimes, when
turning off the interrupts on the PLX chip, the PLX9080 BAR2/IRQREGISTER will
be turned to off but irq's will still be taken into account).
You must therefore use ONLY the glue functions glue_plx_enable_irqs() and
glue_plx_disable_irqs() to play with interrupts, and NOT program directly
the PLX base interrupt register.

- Makefile has been modified. The syntax is now :
make help (default) / make electrical / make optical / make install.

- install file has been modified. The compiled device driver fecpmc00.o 
is now copied into /lib/modules.... directory, where it stands as the default
module to load when invoked.

- The version of the Driver itself has not been modified (it is still Unified
5.0.1) du to compatibility requirements with the current FecSupervisor version, 
but the version of the files headers has been upgraded to Unified-5.1.0.


INPUT FOR 09/02/04
------------------
File includes/version.h has been modified as follows :
if the driver is compiled for 16 bits fecs, the tag will be unified-x.yy-16
if the driver is compiled for 16 bits fecs, the tag will be unified-x.yy-32


INPUT FOR 12/12/03
------------------
Correction of a bug affecting long frames R/W on optical
fecs : Computation of DD_MAX_MSG_LENGTH_32 has been updated
in file includes/defines.h
The type of fifo to use (16 or 32 bits) now defaults to 16
bits if not specified, and have no longer to be strictly explicit.

INPUT FOR 08/12/03
------------------
File: FecDriverHeader.linux
Configuration file FecDriverHeader.linux removed.
The new installation file are defined in FecSoftware/FecHeader.linux that
is used commonly by all FecSoftware programs.
A new file is also used, the FecRules.linux for all rules depending on the
definition of the FecHeader.linux

INPUT FOR 01/12/03
------------------
File compat.h removed ; no backwards compatibility vs. stable0703 will be
requested for next stable release.

Configuration file FecDriverHeader.linux added. Users can choose their
compilation option via this file (Optical/Electrical, etc...).

All references to the old define #COMPAT_0703 has been removed in the
whole source code and includes.

INPUT FOR 18/11/03
------------------
Definition of DD_FEC_FIFO_DATA in file includes/compat.h has been commented
(was always defined as 16 bits in case OLD_COMPAT option was on)

Definition of DD_FEC_FIFO_DATA in file includes/datatypes.h has been moved
out of the precompilation brackets #ifndef OLD_COMPAT

That way, even if the OLD_COMPAT option is validated, the fifos type (16/32 bits)
remains acessible for each functions using the DD_FEC_FIFO_DATA type and is not forced
to 16 bits whatever the real fifo's size is.


INPUT FOR 18/11/03
------------------
Package: FEC Device Driver

Date of creation of this file  : 18/11/03

Reference WebSite : http://fecdriver.shorturl.com

Version: 5.0.0 => First upload of a unified driver on CVS
		The "official" version name for this driver is : Unified-5.0

		Basically, this driver compiles for electrical fecs (16 bits) or
		optical fecs (32 bits), user have just one line to comment/uncomment
		before compilation.

		API do not change when accessing a 16 bits or a 32 bits FEC, except
		for direct hardware readout (direct readout from fifos for example)

Documentation : Obsolote, no up to date documentation available yet, but
		nearly all functions described on http://fecdriver.shorturl.com
		should be usable as-is with this driver.
		The only functions which will for sure create problems are 
		direct readouts from fifos, when working with a 32 bits FEC.
		

Compared to the previous version (3.0.4) of the Device Driver, differences are :

	- The following files has been added to the CVS tree :
		- DeviceDriver/README.TXT
			-> current file
		- DeviceDriver/includes/compat.h
			-> allows a compatibility between software release stable0703 and the
			current unified driver
			-> Compatibility is turned ON by default ; edit the file to turn it OFF,
			instructions are embedded into the file.
		- DeviceDriver/includes/version.h
			-> Mainly, allows users to choose between an optical FEC or an electrical FEC.
			Only one line have to be commented/uncommented to make this choice.
			See inside the file for deeper details, instructions are embedded.
			-> The default parameters are setted for an ELECTRICAL FEC, using MAJOR 127

	- Major version 4.x.x has been used for developpement of beta and validated versions of a
	device driver dedicated to optical FECs.
	No developpements majored 4.x.x will be released, all functionnalities developped for optical FECs
	has been integrated into 5.x.x serie.


	- Due to potential changes of hardware plateform, new data types have been defined. 
	The old datatypes have been splitted into new sets of data types, with more granularity, and
	they have been renamed. The file DeviceDriver/includes/compat.h allows to use the old naming
	convention and old datatypes, in order to preserve backcompatibility with previous releases of
	client programs.

	- The source code of the core device driver and of the glue layer has been fully rewritten
	in order to support these changes (32 bits support, new data types and naming conventions).

