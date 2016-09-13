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
*/

//#define FF_DEBUG
#define WAITANSWERWATCHDOG	15

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;

#include <unistd.h> // for usleep

#include "FecFunctions.h"
//#include "FecVmeRingDevice.h"

/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/*
Constructor ::
Still have to consider what will be needed as constructor parameters
*/
FecFunctions::FecFunctions ( void ) throw ( FecExceptionHandler ) 
{
}


/*
Destructor ::
Standard implementation
*/
FecFunctions::~FecFunctions ( void ) throw ( FecExceptionHandler )
{
}


int FecFunctions::ffGetFrameStatus( tscType32 *frame, tscType16 *status ) throw ( FecExceptionHandler )
{
tscType16 word1, word2;
int frame_length;
int statusOffset;
tscType32 statusWord;


	word1 = (tscType16)( (frame[0] >> 8) & 0xFF );
	word2 = (tscType16)( frame[0] & 0xFF );
	if (word1 & 0x0080)
	{
		frame_length = ( ((word1 & 0x007F)<<8) + word2 ) + 4;
	}
	else frame_length = word1 + 3;

	if ((frame_length % 4) == 0)
	{
		statusWord = frame[(frame_length/4)+1];
		statusOffset = 1;
	}
	else
	{
		statusWord = frame[frame_length/4];
		statusOffset = (frame_length % 4)+1;	
	}
	(*status) = (tscType16)((statusWord >> ((4-statusOffset)*8)) & 0xFF) ;

	#ifdef FF_DEBUG
		cout << "I see a status of : " << hex << (*status) << endl;
	#endif
	
	return 0;
}


int FecFunctions::ffGetDeviceFrameStatus( tscType32 *frame, tscType16 *status ) throw ( FecExceptionHandler )
{
tscType16 word1, word2;
int frame_length;

tscType32 statusWord;


	word1 = (tscType16)( (frame[0] >> 8) & 0xFF );
	word2 = (tscType16)( frame[0] & 0xFF );
	if (word1 & 0x0080)
	{
		frame_length = ( ((word1 & 0x007F)<<8) + word2 ) + 4;
	}
	else frame_length = word1 + 3;

	statusWord = frame[(frame_length/4)];
		
//cout << "my status word is : " << hex << statusWord << endl;

	//(*status) = (tscType16)((statusWord >> ((frame_length % 4)*8)) & 0xFF) ;
	(*status) = (tscType16)(statusWord & (0x0000FF00)) >> 8 ;

	#ifdef FF_DEBUG
		cout << "I see a device frame status of : " << hex << (*status) << endl;
	#endif
	
	return 0;
}



int FecFunctions::ffComputeFrame32Length( tscType32 frame_word, int *frame32_length ) throw ( FecExceptionHandler )
{
tscType16 word1, word2;
int frame_length;

	word1 = (tscType16)( (frame_word >> 8) & 0xFF );
	word2 = (tscType16)( frame_word & 0xFF );
	if (word1 & 0x0080)
	{
		frame_length = ( ((word1 & 0x007F)<<8) + word2 ) + 4;
	}
	else frame_length = word1 + 3;
	(*frame32_length) = (int)(frame_length / 4);
	(*frame32_length)++;
	return 0;
}





int FecFunctions::ffReadFrame( tscType32 *frameToRead, FecVmeRingDevice * whereToRead ) throw ( FecExceptionHandler )
{
int watchdog;
int i;
int frame32_length;


	// Wait for data to arrive in fifo ..
	watchdog = 0;
	while ( (whereToRead->getFecRingSR0() & DD_FEC_REC_EMPTY) && (watchdog++ < WAITANSWERWATCHDOG) );

	// No anwer
	if (whereToRead->getFecRingSR0() & DD_FEC_REC_EMPTY)
	{
		#ifdef FF_DEBUG
			cout << "Timeout ; no forced ACK or data returned." << endl;
		#endif
		return -1;
	}

/*
//Base method for fifo received readout
	//OK, we got an answer. Read fifo and compute received frame length
	i = 0;
	while (!(whereToRead->getFecRingSR0() & DD_FEC_REC_EMPTY))
	{
		frameToRead[i] = whereToRead->getFifoReceive();
		i++;
	}
	
	//computed received frame length
	ffComputeFrame32Length(frameToRead[0], &frame32_length);

	//Check frame sanity
	if (frame32_length > VME_MAX_FRAME_LENGTH)
	{
		#ifdef FF_DEBUG
		cout << "Received frame is too long : " << frame32_length << " elements." << endl;
		cout << "Max allowed frame length is VME_MAX_FRAME_LENGTH = " << VME_MAX_FRAME_LENGTH << endl;
		#endif
		return -1;
	}
//End of Base method for fifo received readout
*/




//other method, this is to try ...

	//OK, we got an answer. Read fifo receive first word
	frameToRead[0] = whereToRead->getFifoReceive();

	//computed received frame length
	ffComputeFrame32Length(frameToRead[0], &frame32_length);

	//Check frame sanity
	if (frame32_length > VME_MAX_FRAME_LENGTH)
	{
		#ifdef FF_DEBUG
		cout << "Received frame is too long : " << frame32_length << " elements." << endl;
		cout << "Max allowed frame length is VME_MAX_FRAME_LENGTH = " << VME_MAX_FRAME_LENGTH << endl;
		#endif
		return -1;
	}

	//Now, read frame till the end
	for (i=1; i<frame32_length ; i++)
	{
		frameToRead[i] = whereToRead->getFifoReceive();
	}

//End of other Method



	#ifdef FF_DEBUG
		cout << "Received frame length is " << frame32_length << " 32 bits words." << endl;
		for (i=0 ; i<frame32_length; i++)
		{
			cout << "Received data or f-acked frame[" << i << "0] is : " << hex << frameToRead[i] << endl;
		}
	#endif
	
	//Clear Errors
	whereToRead->setFecRingCR1(3);
	
return 0;

}









int FecFunctions::ffWriteFrame( tscType32 *frameToWrite, FecVmeRingDevice * whereToWrite ) throw ( FecExceptionHandler )
{
int i;
int frame32_length;
	

	//computed sent frame length
	ffComputeFrame32Length(frameToWrite[0], &frame32_length);

	#ifdef FF_DEBUG
		cout << "Frame to write length is " << frame32_length << " 32 bits words." << endl;
	#endif
	
	//Check frame sanity
	if (frame32_length > VME_MAX_FRAME_LENGTH)
	{
		#ifdef FF_DEBUG
		cout << "Frame to write is too long." << endl;
		cout << "Max allowed frame length is VME_MAX_FRAME_LENGTH = " << VME_MAX_FRAME_LENGTH << endl;
		#endif
		return -1;
	}

	//Write frame in fifo transmit
	for (i=0; i<frame32_length ; i++)
	{
		whereToWrite->setFifoTransmit((tscType32)frameToWrite[i]);
		#ifdef FF_DEBUG
			cout << "wrote frame[" << i << "0] = : " << hex << frameToWrite[i] << " in fifo transmit." << endl;
		#endif

	}

	//Send frame on ring
	tscType16 cr0memory = whereToWrite->getFecRingCR0();
	whereToWrite->setFecRingCR0(cr0memory | 3);
	whereToWrite->setFecRingCR0(cr0memory);


	int lcl_err = ffReadFrame( frameToWrite, whereToWrite );
	
return lcl_err;
return 0;

}







