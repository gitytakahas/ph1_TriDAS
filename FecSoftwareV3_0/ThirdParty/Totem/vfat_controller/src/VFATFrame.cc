/****************************************************************************
*
* This is a part of TOTEM testbeam/monitoring software.
* Authors: 
*	Jan Kaspar (jan.kaspar@gmail.com) 
*	Leszek Grzanka
*    
* $Id: VFATFrame.cc,v 1.2 2008/04/11 09:35:59 jpetaeja Exp $
* $Revision: 1.2 $
* $Date: 2008/04/11 09:35:59 $
*
****************************************************************************/

#ifdef _MONITOR_
	#include "VFATFrame.h"
#else
	#include "TotemDataReadout/DataFormats/interface/VFATFrame.h"
#endif


namespace Totem {

//----------------------------------------------------------------------------------------------------

VFATFrame::VFATFrame(unsigned short *_data)
{
	if (_data) setData(_data);
	else memset(data, 0, 12 * sizeof(unsigned short));
}

//----------------------------------------------------------------------------------------------------

VFATFrame::~VFATFrame()
{
}


//----------------------------------------------------------------------------------------------------

bool VFATFrame::channelActive(unsigned char channel) const
{
	return ( data[1 + (channel / 16)] & (1 << (15 - channel % 16)) ) ? 1 : 0;
}

//----------------------------------------------------------------------------------------------------

std::vector<unsigned char> VFATFrame::getActiveChannels() const
{
	std::vector<unsigned char> channels;

	for (int i = 0; i < 8; i++) {
		// quick check
		if (!data[1 + i]) continue;

		// go throug bits
		unsigned short mask;
		char offset;
		for (mask = 1 << 15, offset = 15; mask; mask >>= 1, offset--) {
			if (data[1 + i] & mask) channels.push_back( i * 16 + offset );
		}
	}

	return channels;
}
//----------------------------------------------------------------------------------------------------

bool VFATFrame::checkCRC() const
{
	unsigned short int crc_fin = 0xffff;
	for (int i = 11; i >= 1; i--)
		crc_fin = crc_calc(crc_fin, data[i]);
	
	return (crc_fin == data[0]);
}

//----------------------------------------------------------------------------------------------------

unsigned short int VFATFrame::crc_calc(unsigned short int crc_in, unsigned short int dato)
{
 unsigned short int v = 0x0001;
 unsigned short int mask = 0x0001;    
 bool d=0;
 unsigned short int crc_temp = crc_in;
 unsigned char datalen = 16;

 for (int i=0; i<datalen; i++)
     {
          if (dato & v) d = 1; 
          else d = 0;
         
           if ((crc_temp & mask)^d) crc_temp = crc_temp>>1 ^ 0x8408;
           else crc_temp = crc_temp>>1;
          
          v<<=1;
     }

 return(crc_temp);
}

} // namespace
