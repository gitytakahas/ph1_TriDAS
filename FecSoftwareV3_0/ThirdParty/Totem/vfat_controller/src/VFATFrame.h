/****************************************************************************
*
* This is a part of TOTEM testbeam/monitoring software.
* Authors: 
*   Jan Kaspar (jan.kaspar@gmail.com) 
*   Leszek Grzanka 
*    
* $Id: VFATFrame.h,v 1.1 2007/12/17 13:18:47 jpetaeja Exp $
* $Revision: 1.1 $
* $Date: 2007/12/17 13:18:47 $
*
****************************************************************************/

#ifndef _Totem_VFATFrame_h_
#define _Totem_VFATFrame_h_



#include <vector>

/**
\defgroup TotemReadout TOTEM Readout
\brief Classes for reading TOTEM data files.


\section Design
The classes are designed for the following concept.
\li A \b file is a sequence of events. Space between events may be filled with rubbish.
\li An \b event consists of a list of VFAT frames and additional information.

There might be several data formats. For each one there shall be a class reading corresponding data files. Common interface for those 
classes is defined in Totem::DataFile.

An event is represented by Totem::RawEvent class. As already said it includes collection of VFAT frames (class Totem::VFATFrameColl) and 
additional information.

\section RecommendedUsage Recommended usage
The following code opens a file (with one of the standard data formats), reads it event by event and for each event it prints ID of the
first VFAT frame in the event.
\code
Totem::DataFile* input = Totem::DataFile::OpenStandard("some file");
if (!input) ... error ...;

Totem::RawEvent event;

input->StartIndexing();
while (!input->MoveNext()) {
	input->GetEvent(event);
	const Totem::VFATFrame *f = event.frames.GetFrameByIndex(0);
	printf("%i\n", f->getChipID());
}
\endcode
The virtue of using Totem::DataFile::OpenStandard is that it first determines type of the file and then it creates instance of appropriate 
class (e.g. Totem::SlinkFile, Totem::TTPFile ...). The command <c>input->MoveNext()</c> finds the next event in the file and 
<c>input->GetEvent(event)</c> reads data (and performs necessary transformations) into \c event.

Strictly speaking, the <c>input->StartIndexing();</c> is not necessary. However, it is useful because when the loop finishes, the file is 
indexed. And hence one can use Totem::DataFile::MoveTo function. For an alternative way see the next example.

To access directly frame number 2 (numbered from 0), you can use (note the Totem::DataFile::MakeIndex method)
\code
Totem::DataFile* input = Totem::OpenDataFile("some file");
input->MakeIndex();
input->MoveTo(2);
...
\endcode
**/


namespace Totem {


/**
\ingroup TotemReadout
\brief VFAT frame class.

Corresponds to data from one frame (data from one VFAT). The buffer is organized as following (reversed
Figure 8 at page 23 of VFAT2 manual):
\verbatim
buffer index	content			size
---------------------------------------------------------------
	0			CRC				16 bits
	1->8		Channel data	128 bits, channel 0 first
	9			ChipID			4 dummy bits (1110) + 12 bits
	10			EC, Flags		4 dummy bits (1100) + 8, 4 bits 
	11			BC				4 dummy bits (1010) + 12 bits
\endverbatim
**/

class VFATFrame
{
	public:
		VFATFrame(unsigned short* _data = 0);					///< Constructor - copy data to private buffer
		VFATFrame(const VFATFrame& copy)						///  Copy constructor
			{ setData(copy.data); }

		~VFATFrame();											///< Destructor - does nothing, actually

		void setData(const unsigned short *data);				///< copy data
		const unsigned short* getData() const;					///< Returns pointer to Channel Data (16 bytes)

		unsigned short getBC() const;							///< Returns Bunch Crossing number (BC<11:0>)
		unsigned short getEC() const;							///< Returns Event Counter (EV<7:0>)
		unsigned short getFlags() const;						///< Returns flags
		unsigned short getChipID() const;						///< Returns ChipID (ChipID<11:0>)
		unsigned short getCRC() const;							///< Returns Check sum
		bool checkFootprint() const;							///< checks the dummy bits, returns true if they're as they should be
		bool checkCRC() const;									///< checks the CRC
		bool channelActive(unsigned char channel) const;		///< Checks if channel number 'channel' was active, returns positive number if it was active, 0 otherwise
		std::vector<unsigned char> getActiveChannels() const;	///<\brief Returns array of active channels
											 					///< it's MUCH MORE EFFICIENT than the previous method for events with low channel occupancy

	private:
		unsigned short data[12];								///< data buffer

		/// internaly used to check CRC
		static unsigned short int crc_calc (unsigned short int crc_in, unsigned short int dato);
};

//----------------------------------------------------------------------------------------------------

inline void VFATFrame::setData(const unsigned short *_data)
{
	memcpy(data, _data, 24);
}

//----------------------------------------------------------------------------------------------------

inline const unsigned short* VFATFrame::getData() const
{
	return data;
}

//----------------------------------------------------------------------------------------------------

inline unsigned short VFATFrame::getBC() const
{
	return data[11] & 0x0FFF;
}

//----------------------------------------------------------------------------------------------------

inline unsigned short VFATFrame::getEC() const
{
	return (data[10] & 0x0FF0) >> 4;
}

//----------------------------------------------------------------------------------------------------

inline unsigned short VFATFrame::getFlags() const
{
	return data[10] & 0x000F;
}

//----------------------------------------------------------------------------------------------------

inline unsigned short VFATFrame::getChipID() const
{
	return data[9] & 0x0FFF;
}

//----------------------------------------------------------------------------------------------------

inline bool VFATFrame::checkFootprint() const
{
	return ((data[9] & 0xF000) == 0xE000) && ((data[10] & 0xF000) == 0xC000) && ((data[11] & 0xF000) == 0xA000);
}

//----------------------------------------------------------------------------------------------------

inline unsigned short VFATFrame::getCRC() const
{
	return data[0];
}

} // namespace
#endif
