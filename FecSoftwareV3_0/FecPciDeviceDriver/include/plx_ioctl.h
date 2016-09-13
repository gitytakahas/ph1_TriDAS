/*
   FileName : 		PLX_IOCTL.H

   Content : 		IOCTL magic number choosen for PLX operations
			IOCTL commands requested by needed PLX operations
			Some common values used for PLX initialisation


   Used in : 		DRIVER

   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0
   
   Date of last modification : 03/05/2005

   Support : 		mail to : fec-support@ires.in2p3.fr
*/



/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3
*/


/* Begin Block - avoid multiple inclusions of this file */
#ifndef DD_PLXIOCTL_H
#define DD_PLXIOCTL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "defines.h"


/*!
choice of a magic number used for PLX IOCTL's commands
*/
#define DD_IOC_MAGIC_PLX 'm'


/*!
<b>DD_IOC_PLX_SET_ADDRESSES</b>
- Destination function : dd_0031_ioc_set_plx9080_addresses in file fastfec.c
- Can be called via function(s) : glue_plx_remap_base_addresses glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_PLX_SET_ADDRESSES _IO(DD_IOC_MAGIC_PLX,0)



/*!
<b>DD_IOC_WRITE_TO_PLX</b>
- Destination function : dd_external_write_to_plx in file fastfec.c
- Can be called via function(s) : glue_plx_set_value in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_WRITE_TO_PLX _IOW(DD_IOC_MAGIC_PLX,1,DD_PLX_ARRAY)




/*!
<b>DD_IOC_READ_FROM_PLX</b>
- Destination function : dd_external_read_from_plx in file fastfec.c
- Can be called via function(s) : glue_plx_get_value in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_READ_FROM_PLX _IOR(DD_IOC_MAGIC_PLX,2,DD_PLX_ARRAY)


/*!
<b>DD_IOC_PLX_HARD_RESET_MODULE</b>
- Destination function : dd_plx_hard_reset_module in file fastfec.c
- Can be called via function(s) : glue_plx_hard_reset in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_PLX_HARD_RESET_MODULE _IO(DD_IOC_MAGIC_PLX,3)


/*!
<b>DD_IOC_ENABLE_PLX_INTERRUPTS</b>
- Destination function : dd_ioc_enable_plx_interrupts in file fastfec.c
- Can be called via function(s) : glue_plx_enable_irqs in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_ENABLE_PLX_INTERRUPTS _IO(DD_IOC_MAGIC_PLX,4)



/*!
<b>DD_IOC_DISABLE_PLX_INTERRUPTS</b>
- Destination function : dd_ioc_disable_plx_interrupts in file fastfec.c
- Can be called via function(s) : glue_plx_disable_irqs in file glue.c
- See the public API documentation of the client function(s) in file glue.h for more details.
*/
#define DD_IOC_DISABLE_PLX_INTERRUPTS _IO(DD_IOC_MAGIC_PLX,5)



#ifdef __cplusplus
}
#endif



/* End Block - avoid multiple inclusions of this file */
#endif

