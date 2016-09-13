#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(DiagSystem, development, lgross@cern.ch)

echo '';
echo 'DiagSystem configuration: '; 
echo '---------------------------------------------------------';

fromwhere=;

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


AC_ARG_WITH(fec-path, 
            [AC_HELP_STRING([--with-fec-path],[specify the path to FecSoftware])],  
            [fec_root=$withval], 
            []) 
AS_IF([test "x$fec_root" == x], 
      AS_IF([test "x$ENV_CMS_TK_FEC_ROOT" != x], 
            [fec_root=$ENV_CMS_TK_FEC_ROOT; echo "ENV_CMS_TK_FEC_ROOT: $ENV_CMS_TK_FEC_ROOT";], 
	    [echo " "; 
             echo "************************************************************************************";
	     echo "FecSystem libraries are required. Please set the FecSoftware path using --with-fec-path=<path>"; 
	     echo "or using the ENV_CMS_TK_FEC_ROOT environment variable.";
	     echo "************************************************************************************"; 
             echo" "; exit; 
	   ])
    )	


# -------------------------------------------------------------------------------------------
#
#    Optional Settings 
#
# -------------------------------------------------------------------------------------------

xdaq_rpmbuild=
AC_ARG_ENABLE(xdaq_rpmbuild, 
              [AC_HELP_STRING([--enable-xdaq_rpmbuild],[specify if building against an XDAQ rpm. (default yes)])],  
              [xdaq_rpmbuild=$enableval; fromwhere="cmd-line"; ], 
              [xdaq_rpmbuild=yes; fromwhere="default"; ]) 
echo "enable XDAQ rpmbuild : $xdaq_rpmbuild (from $fromwhere) "
AS_IF([test "x$xdaq_rpmbuild" == xno], 
      [xdaq_rpmbuild='';],
      [xdaq_rpmbuild=$xdaq_rpmbuild;])


use_trackeronline_db=
AC_ARG_ENABLE(use_trackeronline_db, 
              [AC_HELP_STRING([--enable-use_trackeronline_db],[specify if logreader uses online tracker db . (default no)])],  
              [use_trackeronline_db=$enableval; fromwhere="cmd-line";], 
              [use_trackeronline_db=no; fromwhere="default";]) 
echo "enable Logreader using online tracker DB : $use_trackeronline_db (from $fromwhere) "


compile_for_tracker=
AC_ARG_ENABLE(compile_for_tracker, 
              [AC_HELP_STRING([--enable-compile_for_tracker],[specify if software must be compiled specifically for tracker. (default no)])],  
              [compile_for_tracker=$enableval; fromwhere="cmd-line";], 
              [compile_for_tracker=no; fromwhere="default";]) 
echo "compile diagnostic software for Tracker : $compile_for_tracker (from $fromwhere) "



compile_for_pixels=
AC_ARG_ENABLE(compile_for_pixels, 
              [AC_HELP_STRING([--enable-compile_for_pixels],[specify if software must be compiled specifically for pixels. (default no)])],  
              [compile_for_pixels=$enableval; fromwhere="cmd-line";], 
              [compile_for_pixels=no; fromwhere="default";]) 
echo "compile diagnostic software for Pixels : $compile_for_pixels (from $fromwhere) "





compile_for_generic=
AC_ARG_ENABLE(compile_for_generic, 
              [AC_HELP_STRING([--enable-compile_for_generic],[no sub-detector specific compilation options will be used. (default yes)])],  
              [compile_for_generic=$enableval; fromwhere="cmd-line";], 
              [compile_for_generic=no; fromwhere="default";]) 
echo "compile diagnostic software in generic mode : $compile_for_generic (from $fromwhere) "



parse_tracker_xtra_buffer=
AC_ARG_ENABLE(parse_tracker_xtra_buffer, 
              [AC_HELP_STRING([--enable-parse_tracker_xtra_buffer],[specify if extra buffer content must be interpreted online. (default no)])],  
              [parse_tracker_xtra_buffer=$enableval; fromwhere="cmd-line";], 
              [parse_tracker_xtra_buffer=no; fromwhere="default";]) 
echo "parse and interprete online Tracker extra buffer : $parse_tracker_xtra_buffer (from $fromwhere) "



store_logs_to_trackerdb=
AC_ARG_ENABLE(store_logs_to_trackerdb, 
              [AC_HELP_STRING([--enable-store_logs_to_trackerdb],[Store online error logs into Tracker Error DB. (default no)])],  
              [store_logs_to_trackerdb=$enableval; fromwhere="cmd-line";], 
              [store_logs_to_trackerdb=no; fromwhere="default";]) 
echo "Store online error logs into Tracker Error DB : $store_logs_to_trackerdb (from $fromwhere) "

store_logs_to_pixelsdb=
AC_ARG_ENABLE(store_logs_to_pixelsdb, 
              [AC_HELP_STRING([--enable-store_logs_to_pixelsdb],[Store online error logs into Pixels Error DB. (default no)])],  
              [store_logs_to_pixelsdb=$enableval; fromwhere="cmd-line";], 
              [store_logs_to_pixelsdb=no; fromwhere="default";]) 
echo "Store online error logs into Pixels Error DB : $store_logs_to_pixelsdb (from $fromwhere) "




gwtapplets_root=;
AC_ARG_WITH(gwtapplets-path, 
            [AC_HELP_STRING([--with-gwtapplets-path],[specify the symbolic name (i.e. the aliasName in xdaq default.profile file) to your GWT applets repository])],  
            [gwtapplets_root=$withval; fromwhere="cmd-line";], 
            []) 
AS_IF([test "x$gwtapplets_root" == x],
      [gwtapplets_root=gwtapplets;],
      [])
AS_IF([test "x$gwtapplets_root" != x],
      [echo "GWT root repository : $gwtapplets_root (from $fromwhere)";],
      [])


use_gwtapplet_fireitems_viewer=
AC_ARG_ENABLE(use_gwtapplet_fireitems_viewer, 
              [AC_HELP_STRING([--enable-use_gwtapplet_fireitems_viewer],[Use gwt applet to centrally manage fireitems. (default no)])],  
              [use_gwtapplet_fireitems_viewer=$enableval; fromwhere="cmd-line";], 
              [use_gwtapplet_fireitems_viewer=no; fromwhere="default";]) 
echo "Use gwt applet to centrally manage fireitems : $use_gwtapplet_fireitems_viewer (from $fromwhere) "


compilewith_package_infos=
AC_ARG_ENABLE(compilewith_package_infos, 
              [AC_HELP_STRING([--enable-compilewith_package_infos],[Compile modules with embedded xdaq packages infos. (default yes)])],  
              [compilewith_package_infos=$enableval; fromwhere="cmd-line";], 
              [compilewith_package_infos=yes; fromwhere="default";]) 
echo "Compile modules with embedded xdaq packages infos : $compilewith_package_infos (from $fromwhere) "

# -------------------------------------------------------------------------------------------
#
#    Now, substitute
#
# -------------------------------------------------------------------------------------------

AC_SUBST(diag_home, $PWD)
AC_SUBST(xdaq_root, $xdaq_root)
AC_SUBST(xdaq_os, $xdaq_os)
AC_SUBST(xdaq_platform, $xdaq_platform)
AC_SUBST(xdaq_rpmbuild, $xdaq_rpmbuild)
AC_SUBST(fec_root, $fec_root)
AC_SUBST(use_trackeronline_db, $use_trackeronline_db)
AC_SUBST(compile_for_tracker, $compile_for_tracker)
AC_SUBST(compile_for_pixels, $compile_for_pixels)
AC_SUBST(parse_tracker_xtra_buffer, $parse_tracker_xtra_buffer)
AC_SUBST(gwtapplets_root, $gwtapplets_root)
AC_SUBST(compile_for_generic, $compile_for_generic)
AC_SUBST(store_logs_to_trackerdb, $store_logs_to_trackerdb)
AC_SUBST(store_logs_to_pixelsdb, $store_logs_to_pixelsdb)
AC_SUBST(use_gwtapplet_fireitems_viewer, $use_gwtapplet_fireitems_viewer)
AC_SUBST(compilewith_package_infos, $compilewith_package_infos)
echo '---------------------------------------------------------';
AC_CONFIG_FILES([DiagHeader.linux:DiagHeader.in])
AC_CONFIG_FILES([toolbox/reportErrorReturnType.h:reportErrorReturnType.in])
AC_OUTPUT
echo '---------------------------------------------------------';
echo '';
echo '';
echo '';
echo '';



