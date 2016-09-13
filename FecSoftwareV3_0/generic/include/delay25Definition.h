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

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace,
Mulhouse-France
*/
#ifndef DELAY25DEFINITION_H
#define DELAY25DEFINITION_H

#ifndef PIXEL
#define PIXEL
#endif // PIXEL

// Internal control register offsets
#define CR0     0x00
#define CR1     0x01
#define CR2     0x02
#define CR3     0x03
#define CR4     0x04
#define GCR     0x05

// delay25 Access
#define DELAY25_DELAY  0x3F //bit 0:5 for CR0,CR1,CR2,CR4
#define DELAY25_ENABLE 0x40 //bit 6 for CR0,CR1,CR2,CR4

#define DELAY25_GCR_IDLL        0x40 //bit 6 of GCR
#define DELAY25_CLOCKFREQUENCY  0x03 //bit 0:1 of GCR


#endif
