#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FecSoftwareV3_0, development, Frederic.Drouhin@cern.ch)


echo '';
echo 'FecSoftwareV3_0 configuration: '; 
echo '---------------------------------------------------------';

fromwhere=;

# -------------------------------------------------------------------------------------------
#
#    Optional Settings 
#
# -------------------------------------------------------------------------------------------

xdaq_rpmbuild=;
AC_ARG_ENABLE(xdaq_rpmbuild, 
              [AC_HELP_STRING([--enable-xdaq_rpmbuild],[specify if building against an XDAQ rpm. (default yes)])],  
              [xdaq_rpmbuild=$enableval; fromwhere="cmd-line"; ], 
              [xdaq_rpmbuild=yes; fromwhere="default";]) 
echo "enable XDAQ rpmbuild : $xdaq_rpmbuild (from $fromwhere) "
AS_IF([test "x$xdaq_rpmbuild" == xno], 
      [xdaq_rpmbuild='';],
      [xdaq_rpmbuild=$xdaq_rpmbuild;])


usbfec_enabled=;fecusb_targets=;fecusb_clean=;
AC_ARG_ENABLE(usbfec,   
	      [AC_HELP_STRING([--enable-usbfec],[optional: enable usb functionality (default no)])], 
	      [usbfec_enabled=$enableval; fromwhere="cmd-line";], 
              []) 
AS_IF([test "x$usbfec_enabled" == x], 
      [AS_IF([test "x$USBFEC_COMPILE" != x], 
          [usbfec_enabled=$USBFEC_COMPILE; fromwhere="environment";], 
   	  [usbfec_enabled=no; fromwhere="default"; ]
      )],
      [usbfec_enabled=$usbfec_enabled;])
AS_IF([test "x$usbfec_enabled" != xno], 
      [AC_SUBST([fecusb_targets], 'FecLibFtdi FecUsbDriver') ], 
      [AC_SUBST([fecusb_targets], '') 
     ])
AS_IF([test "x$usbfec_compile" != xno], 
      [AC_SUBST([fecusb_clean], 'FecLibFtdi_clean FecUsbDriver_clean') ], 
      [AC_SUBST([fecusb_clean], '') 
      ])
echo "enable USBFec        : $usbfec_enabled (from $fromwhere) "


pcifec_enabled=;
fecpci_targets=;fecpci_tktargets=;
fecpci_clean=;fecpci_tkclean=;
AC_ARG_ENABLE(pcifec,   
	      [AC_HELP_STRING([--enable-pcifec],[optional: enable pci functionality (default no)])], 
	      [pcifec_enabled=$enableval; fromwhere="cmd-line";], 
              []) 
AS_IF([test "x$pcifec_enabled" == x], 
      [AS_IF([test "x$PCIFEC_COMPILE" != x], 
          [pcifec_enabled=$PCIFEC_COMPILE; fromwhere="environment"; ], 
   	  [pcifec_enabled=no; fromwhere="default"; ]
      )],
      [pcifec_enabled=$pcifec_enabled; ])
AS_IF([test "x$pcifec_enabled" != xno], 
      [AC_SUBST([fecpci_targets], 'FecDeviceDriver') AC_SUBST([fecpci_tktargets], 'PciConsoleDebugger FecDeviceDriver')], 
      [AC_SUBST([fecpci_targets], '') AC_SUBST([fecpci_tktargets], '') 
     ])
AS_IF([test "x$pcifec_compile" != xno], 
      [AC_SUBST([fecpci_clean], 'FecDeviceDriver_clean') AC_SUBST([fecpci_tkclean], 'FecDeviceDriver_clean') ], 
      [AC_SUBST([fecpci_clean], '')  AC_SUBST([fecpci_tkclean], '') 
      ])
echo "enable PCIFec        : $pcifec_enabled (from $fromwhere) "


sbs_enabled=;
AC_ARG_ENABLE(sbs,
              [AC_HELP_STRING([--enable-sbs],[specify to compile SBS support. (default no)])],  
              [sbs_enabled=$enableval; fromwhere="cmd-line";  ], 
              [sbs_enabled=no; fromwhere="default";]) 
echo "enable SBS           : $sbs_enabled (from $fromwhere) "
AS_IF([test "x$sbs_enabled" == xyes], 
      [sbs_enabled=1;], 
      [sbs_enabled=0;] 
      )



# -------------------------------------------------------------------------------------------
#
#    Required Settings 
#
# -------------------------------------------------------------------------------------------

xdaq_root=;
AC_ARG_WITH(xdaq-path, 
            [AC_HELP_STRING([--with-xdaq-path],[specify the path to your XDAQ installation or set $XDAQ_ROOT])],  
            [xdaq_root=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$xdaq_root" == xno],
      [xdaq_root=;],
      [])
AS_IF([test "x$xdaq_root" == x], 
       AS_IF([test "x$XDAQ_ROOT" != x], 
             [xdaq_root=$XDAQ_ROOT;  fromwhere="environment";], 
             [echo ''; 
              echo "************************************************************************************";
  	      echo "XDAQ libraries are required. Please set the xdaq path using --with-xdaq-path=<path>"; 
	      echo "or using the XDAQ_ROOT environment variable."; 
	      echo "************************************************************************************"; 
              echo ''; exit; 
            ])
     )	
echo "XDAQ root            : $xdaq_root (from $fromwhere)"; 

xdaq_os=;
AC_ARG_WITH(xdaq-os, 
            [AC_HELP_STRING([--with-xdaq-os],[specify your os or set $XDAQ_OS (default linux)])],  
            [xdaq_os=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$xdaq_os" == xno],
      [xdaq_os=;],
      [])
AS_IF([test "x$xdaq_os" == x], 
       AS_IF([test "x$XDAQ_OS" != x], 
             [xdaq_os=$XDAQ_OS; fromwhere="environment";], 
             [echo ''; 
              echo "************************************************************************************";
              echo "XDAQ OS must be specified. Please set the xdaq os using --with-xdaq-os=<os>"; 
	      echo "or using the XDAQ_OS environment variable."; 
	      echo "************************************************************************************"; 
              echo ''; exit; 
            ]) 
     )	
echo "XDAQ OS              : $xdaq_os (from $fromwhere)" 

xdaq_platform=;
AC_ARG_WITH(xdaq-platform, 
            [AC_HELP_STRING([--with-xdaq-platform],[specify your platform or set $XDAQ_PLATFORM (default x86_slc4)])],  
            [xdaq_platform=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$xdaq_platform" == xno],
      [xdaq_platform=;],
      [])
AS_IF([test "x$xdaq_platform" == x], 
       AS_IF([test "x$XDAQ_PLATFORM" != x], 
             [xdaq_platform=$XDAQ_PLATFORM; fromwhere="environment";], 
             [echo ''; 
              echo "************************************************************************************";
	      echo "XDAQ PLATFORM must be specified. Please set the xdaq platform using --with-xdaq-platform=<ptfrm>"; 
 	      echo "or using the XDAQ_PLATFORM environment variable."; 
	      echo "************************************************************************************"; 
              echo ''; exit; 
           ]) 
    )	
echo "XDAQ platform        : $xdaq_platform (from $fromwhere)" 


hal_root=;
AC_ARG_WITH(hal-path, 
            [AC_HELP_STRING([--with-hal-path],[specify the path to HAL or set $ENV_CMS_TK_HAL_ROOT])],  
            [hal_root=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$hal_root" == xno],
      [hal_root=;],
      [])
AS_IF([test "x$hal_root" == x], 
      AS_IF([test "x$ENV_CMS_TK_HAL_ROOT" != x], 
            [hal_root=$ENV_CMS_TK_HAL_ROOT; fromwhere="environment";], 
	    [echo ''; 
             echo "************************************************************************************";
	     echo "HAL libraries are required. Please set the HAL path using --with-hal-path=<path>"; 
	     echo "or using the ENV_CMS_TK_HAL_ROOT environment variable.";
	     echo "************************************************************************************"; 
             echo ''; exit; 
	   ])
    )	
echo "HAL root             : $hal_root (from $fromwhere) "



oracle_root=;
oracle_envvar=;
AC_ARG_WITH(oracle-path, 
            [AC_HELP_STRING([--with-oracle-path],[specify the path to Oracle or set $ENV_ORACLE_HOME])],  
	    [oracle_root=$withval;fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$oracle_root" == x], 
      AS_IF([test "x$ENV_ORACLE_HOME" != x], 
             [oracle_root=$ENV_ORACLE_HOME; fromwhere="environment";], 
	     [echo ''; 
              echo "************************************************************************************";
	      echo "Oracle libraries are required. Please set the oracle path using --with-oracle-path=<path>"; 
	      echo "or using the ENV_ORACLE_HOME environment variable";
	      echo "************************************************************************************"; 
              echo ''; exit; 
            ])
     )
echo "Oracle home          : $oracle_root (from $fromwhere) "
AS_IF([test "x$oracle_root" == xno],
      [oracle_root=; set_oracle_envvar=;],
      [oracle_envvar='ENV_CMS_TK_ORACLE_HOME=';])



xerces_root=;
AC_ARG_WITH(xerces-path, 
            [AC_HELP_STRING([--with-xerces-path],[specify the path to Xerces or set $XERCESCROOT])],  
            [xerces_root=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$xerces_root" == x], 
      AS_IF([test "x$XERCESCROOT" != x], 
            [xerces_root=$XERCESCROOT; fromwhere="environment";],  
	    [echo ''; 
             echo "************************************************************************************";
	     echo "Xerces libraries are required. Please set the xerces path using --with-xerces-path=<path>"; 
	     echo "or using the XERCESCROOT environment variable.";
	     echo "************************************************************************************"; 
             echo ''; exit; 
	   ])
    )	
AS_IF([test "x$xerces_root" == xno],
      [xerces_root=;],
      [])
echo "Xerces-c root        : $xerces_root (from $fromwhere) "




fed9u_root=;fed9u_envvar=;
AC_ARG_WITH(fed9u-path, 
            [AC_HELP_STRING([--with-fed9u-path],[specify the path to Fed9USoftware or set $ENV_CMS_TK_FED9U_ROOT])],  
            [fed9u_root=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$fed9u_root" == x], 
      AS_IF([test "x$ENV_CMS_TK_FED9U_ROOT" != x], 
            [fed9u_root=$ENV_CMS_TK_FED9U_ROOT; fromwhere="environment";], 
	    [echo ''; 
             echo '************************************************************************************';
	     echo 'Fed9U libraries are required. Please set the fed9u path using --with-fed9u-path=<path>'; 
	     echo 'or using the ENV_CMS_TK_FED9U_ROOT environment variable.';
	     echo '************************************************************************************'; 
             echo ''; exit; 
	   ])
    )	
AS_IF([test "x$fed9u_root" == xno],
      [fed9u_root=; fed9u_envvar=;],
      [fed9u_envvar='ENV_CMS_TK_FED9U_ROOT='; ])
echo "Fed9USoftware  path  : $fed9u_root (from $fromwhere) "


caen_root=;
AC_ARG_WITH(caen-path, 
            [AC_HELP_STRING([--with-caen-path],[specify the CAEN path or set $ENV_CMS_TK_CAEN_ROOT])],  
            [caen_root=$withval;fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$caen_root" == xno],
      [caen_root=;],
      [])
AS_IF([test "x$caen_root" == x], 
      AS_IF([test "x$ENV_CMS_TK_CAEN_ROOT" != x], 
            [caen_root=$ENV_CMS_TK_CAEN_ROOT; fromwhere="environment";], 
            [echo ''; 
             echo "************************************************************************************";
	     echo "CAEN libraries are required. Please set the CAEN path using --with-caen-path=<path>"; 
	     echo "or using the ENV_CMS_TK_CAEN_ROOT environment variable.";
	     echo "************************************************************************************"; 
             echo ''; exit; 
	   ])
   )	
echo "CAEN software root   : $caen_root (from $fromwhere) "



sbs_root=;
AS_IF([test "x$sbs_enabled" == x1],
      [AC_ARG_WITH(sbs-path, 
                   [AC_HELP_STRING([--with-sbs-path],[specify the SBS path or set $ENV_CMS_TK_SBS_ROOT])],  
                   [sbs_root=$withval; fromwhere="cmd-line";], 
                   []) 
       AS_IF([test "x$sbs_root" == xno],
             [sbs_root=;],
             [])
       AS_IF([test "x$sbs_root" == x], 
             AS_IF([test "x$ENV_CMS_TK_SBS_ROOT" != x], 
                   [sbs_root=$ENV_CMS_TK_SBS_ROOT; fromwhere="environment";], 
	           [echo ''; 
                    echo "************************************************************************************";
	            echo "You have enabled SBS support but did not specify a path.  Please set the SBS path"
                    echo "using --with-sbs-path=<path> or using the ENV_CMS_TK_SBS_ROOT environment variable."
	            echo "************************************************************************************"; 
                    echo ''; exit;
	          ])
              ) 
      echo "SBS software root    : $sbs_root (from $fromwhere) ";
      ],
    [])



trk_online=;tk_common=;tk_xdaq=;icutils_root=;
tkcommon_envvar=;tkxdaq_envvar=;icutils_envvar=;
AC_ARG_WITH(trkonline-path, 
            [AC_HELP_STRING([--with-trkonline-path],[specify the path to TrackerOnline or set $ENV_CMS_TK_ONLINE_ROOT])],  
            [trk_online=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$trk_online" == x], 
      AS_IF([test "x$ENV_CMS_TK_ONLINE_ROOT" != x], 
            [trk_online=$ENV_CMS_TK_ONLINE_ROOT; fromwhere="environment";], 
	    [echo ''; 
             echo '************************************************************************************';
	     echo 'Libraries within TrackerOnline are required. Please set the path using --with-trkonline-path=<path>'; 
	     echo 'or using the ENV_CMS_TK_ONLINE_ROOT environment variable.';
	     echo '************************************************************************************'; 
             echo ''; exit; 
	   ])
   )	
AS_IF([test "x$trk_online" == xno],
      [trk_online=; trk_common=; trk_xdaq=; trkonline_envvar=;tkcommon_envvar=;tkxdaq_envvar=;],
      [tk_common="$trk_online/2005/TrackerCommon"; 
       tk_xdaq="$trk_online/2005/TrackerXdaq";
       icutils_root="$tk_common/ICUtils";	
       tkcommon_envvar='ENV_CMS_TK_COMMON=';
       tkxdaq_envvar='ENV_CMS_TK_XDAQ=';
       icutils_envvar='ENV_CMS_TK_ICUTILS=';
      ]
     )
echo "TrackerOnline path   : $trk_online (from $fromwhere) "

diag_root=;
diag_envvar=;
AC_ARG_WITH(diag-path, 
            [AC_HELP_STRING([--with-diag-path],[specify the path to DiagSystem $ENV_CMS_TK_DIAG_ROOT])],  
            [diag_root=$withval;fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$diag_root" == x], 
      AS_IF([test "x$ENV_CMS_TK_DIAG_ROOT" != x], 
            [diag_root=$ENV_CMS_TK_DIAG_ROOT; fromwhere="environment";], 
	    [echo ''; 
             echo '************************************************************************************';
	     echo 'DiagSystem libraries are required. Please set the DiagSystem path using --with-diag-path=<path>'; 
	     echo 'or using the ENV_CMS_TK_DIAG_ROOT environment variable.';
	     echo '************************************************************************************'; 
             echo ''; exit; 
	   ])
    )	
AS_IF([test "x$diag_root" == xno],
      [diag_root=; diag_envvar=;],
      [diag_envvar='ENV_CMS_TK_DIAG_ROOT=';])
echo "DiagSystem path      : $diag_root (from $fromwhere) "



# -------------------------------------------------------------------------------------------
#
#    Now, substitute
#
# -------------------------------------------------------------------------------------------

AC_SUBST(tk_base, $PWD/.. )
AC_SUBST(fec_root, $PWD)
AC_SUBST(xdaq_root, $xdaq_root)
AC_SUBST(xdaq_base, $xdaq_root)
AC_SUBST(xdaq_os, $xdaq_os)
AC_SUBST(xdaq_platform, $xdaq_platform)
AC_SUBST(xdaq_rpmbuild, $xdaq_rpmbuild)
AC_SUBST(trk_online, $trk_online)
AC_SUBST(tk_common, $tk_common)
AC_SUBST(tk_xdaq, $tk_xdaq)
AC_SUBST(icutils_root, $icutils_root)
AC_SUBST(hardware_root, $trk_online/2005/hardware)
AC_SUBST(diag_root, $diag_root)
AC_SUBST(fecpci_targets, $fecpci_targets)
AC_SUBST(fecpci_targets, $fecpci_tktargets)
AC_SUBST(fecpci_clean, $fecpci_clean)
AC_SUBST(fecpci_clean, $fecpci_tkclean)
AC_SUBST(hal_root, $hal_root)
AC_SUBST(oracle_root, $oracle_root)
AC_SUBST(xerces_root, $xerces_root)
AC_SUBST(fed9u_root, $fed9u_root)
AC_SUBST(caen_root, $caen_root)
AC_SUBST(sbs_root, $sbs_root)
AC_SUBST(sbs_enabled, $sbs_enabled)
AC_SUBST(usbfec_enabled, $usbfec_enabled)
AC_SUBST(pcifec_enabled, $pcifec_enabled)
AC_SUBST(fecusb_targets, $fecusb_targets)
AC_SUBST(fecusb_clean, $fecusb_clean)
#AC_SUBST(hal_version, `cat $hal_root/thisVersion.txt`)

AC_SUBST(fed9u_envvar, $fed9u_envvar)
AC_SUBST(diag_envvar, $diag_envvar)
AC_SUBST(oracle_envvar, $oracle_envvar)
AC_SUBST(tkcommon_envvar, $tkcommon_envvar)
AC_SUBST(tkxdaq_envvar, $tkxdaq_envvar)
AC_SUBST(icutils_envvar, $icutils_envvar)
echo '---------------------------------------------------------';
AC_CONFIG_FILES([FecHeader.linux:FecHeader.in])
AC_CONFIG_FILES([Makefile:Makefile.in])
AC_OUTPUT
echo '---------------------------------------------------------';
echo '';
echo '';
echo '';
echo '';
