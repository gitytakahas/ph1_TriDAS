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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef XDAQTRACKERDCUDEFS_H
#define XDAQTRACKERDCUDEFS_H

#ifndef VERS_FEC
#  define VERS_FEC 0x1
#endif

  // Send DCU data
#ifndef DCU_VALUES_SEND
#  define DCU_VALUES_SEND 0xFF
#endif

  // Registered the FECsupervisor to a DcuFilter
#ifndef DCUFILTER_REGISTER
#  define DCUFILTER_REGISTER 0xFE
#endif

  // Answer to a request for a connection
#ifndef DCUFILTER_OK
#   define DCUFILTER_OK 0x0
#endif

#ifndef DCUFILTER_NOK
#   define DCUFILTER_NOK 0x1
#endif

#ifndef DCUFILTERCLASSNAME
#  define DCUFILTERCLASSNAME "DcuFilter"
#endif


#endif
