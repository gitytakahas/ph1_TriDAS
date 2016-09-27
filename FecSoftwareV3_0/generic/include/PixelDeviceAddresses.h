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
#ifndef PIXELDEVICEADDRESSES_H
#define PIXELDEVICEADDRESSES_H

#ifndef PIXEL
#define PIXEL
#endif // PIXEL

const unsigned int dcudeviceAddress = 0x50;
const unsigned int plldeviceAddress = 0x40;
const unsigned int dohdeviceAddress = 0x70;
#ifdef PIXEL
const unsigned int delay25deviceAddress = 0x60;
#endif // PIXEL 

const unsigned int aoh1AdeviceAddress = 0x08;
const unsigned int aoh1BdeviceAddress = 0x0C;
const unsigned int aoh2AdeviceAddress = 0x10;
const unsigned int aoh2BdeviceAddress = 0x14;
const unsigned int aoh3AdeviceAddress = 0x18;
const unsigned int aoh3BdeviceAddress = 0x1C;
const unsigned int aoh4AdeviceAddress = 0x20;
const unsigned int aoh4BdeviceAddress = 0x24;

const unsigned int poh1deviceAddress = 0x0;
const unsigned int poh2deviceAddress = 0x08;
const unsigned int poh3deviceAddress = 0x10;
const unsigned int poh4deviceAddress = 0x18;
const unsigned int poh5deviceAddress = 0x20;
const unsigned int poh6deviceAddress = 0x28;
const unsigned int poh7deviceAddress = 0x30;

const unsigned int moduleAddress = 0x08;

const unsigned int PixelDelay25FrequencyMode = 40;

#endif



