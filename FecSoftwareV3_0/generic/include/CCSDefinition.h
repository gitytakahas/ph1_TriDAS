/**
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
#ifndef CCSDEFINITION_H
#define CCSDEFINITION_H

  // For the SSID
#define SSIDOFFSET 12
#define SSIDMASK   0xF000

  // RESET TTCRx
#define TTCRXRESET 0x80

  // RESET QPLL
#define QPLLRESET 0x20

  // QPLL AUTORESTART
#define QPLLAUTORESTART 0x10

  // QPLL ERROR
#define STATUS0_QPLLERROR 0x4

  // QPLL LOCKED
#define STATUS0_QPLLLOCKED 0x2

  // TTCRx Ready
#define STATUS0_TTCRXREADY 0x1

#endif
