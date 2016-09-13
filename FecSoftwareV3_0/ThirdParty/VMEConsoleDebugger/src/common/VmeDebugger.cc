#include <iostream>
//System includes
#include <stdio.h>

//Class definition includes
#include "FecVmeRingDevice.h"
#include "FecFunctions.h"



#define FECBOARD_ADDRESSTABLE "../../config/FecAddressTable.dat"

/* min-max CCU addresses */
#define MIN_CCU 0x01
#define MAX_CCU 0x7F

/* min-max I2C channels */
#define MIN_I2C_CH25	0x0010
#define MAX_I2C_CH25	0x001F

/* min-max I2C addresses */
#define MIN_I2C_ADR	0x01
#define MAX_I2C_ADR	0x7F


/* If not provided as a default value at program launch,
   the user must define on VME board (slot) he will work each time
   he launches a command. */ 
int checkslot(bool isFixed, int * slotNumber)
{
  if (!isFixed)
    {
      std::cout << "Which slot ? (0-20) : ";
      std::cin >> (*slotNumber);
      if ( (*slotNumber<0) || (*slotNumber>20) )
	{
	  std::cout << "Your VME slot number must be in [0..20]" << std::endl;
	  return -1;
	}

    }
  return 0;
}



/* If not provided as a default value at program launch,
   the user must define on which ring he will work each time
   he launches a command. */ 
int checkring(bool isFixed, int * ringNumber)
{
  if (!isFixed)
    {
      std::cout << "Which ring ? (0-7) : ";
      std::cin >> (*ringNumber);
      if ( (*ringNumber<0) || (*ringNumber>7) )
	{
	  std::cout << "Your Ring number must be in [0..7]" << std::endl;
	  return -1;
	}
    }
  return 0;
}




/* Transcription of SR0 bitfield */
void dbg_decode_status0(unsigned long param_status0)
{
  if (0x0001 & param_status0) fprintf(stdout,"\t fifo transmit running\n");
  if (0x0002 & param_status0) fprintf(stdout,"\t fifo receive running\n");
  //	if (DD_FEC_REC_HALF_F & param_status0) fprintf(stdout,"\t fifo receive half full\n");
  if (0x0008 & param_status0) fprintf(stdout,"\t fifo receive full\n");
  if (0x0010 & param_status0) fprintf(stdout,"\t fifo receive empty\n");
  //	if (DD_FEC_RET_HALF_F & param_status0) fprintf(stdout,"\t fifo return half full\n");
  if (0x0040 & param_status0) fprintf(stdout,"\t fifo return full\n");
  if (0x0080 & param_status0) fprintf(stdout,"\t fifo return empty\n");
  //	if (DD_FEC_TRA_HALF_F & param_status0) fprintf(stdout,"\t fifo transmit half full\n");
  if (0x0200 & param_status0) fprintf(stdout,"\t fifo transmit full\n");
  if (0x0400 & param_status0) fprintf(stdout,"\t fifo transmit empty\n");
  if (0x0800 & param_status0) fprintf(stdout,"\t Link initialized\n");
  if (0x1000 & param_status0) fprintf(stdout,"\t pending IRQ\n");
  if (0x2000 & param_status0) fprintf(stdout,"\t data to fec\n");
  if (0x4000 & param_status0) fprintf(stdout,"\t ttcrx ready\n");
}


//Let's define what we will need to access and control a board
typedef struct {
  unsigned long baseAddress;
  HAL::VME64xDevice * fecboard;
  FecVmeRingDevice * ringsList[8];
  bool isPresent;
} vmeBoard;


int main(int argc, char *argv[])
{
  //Define the crate as a collection of (potential) boards
  vmeBoard vmeCrate[21];

  //Tab list of bases addresses for slots 0..20 of the crate
  unsigned long baseAddressList[]={0x00080000, 0x00100000, 0x00180000, 0x00200000, 0x00280000, 0x00300000, 0x00380000, 0x00400000, 0x00480000, 0x00500000,
				   0x00580000, 0x00600000, 0x00680000, 0x00700000, 0x00780000, 0x00800000, 0x00880000, 0x00900000, 0x00980000, 0x00A00000, 0x00A80000};

	

  /* init ccu25 CTRL A frame, used for CCU25 detection */
  tscType32 frame_init_ctrlA[VME_MAX_FRAME_LENGTH] = {0xFF000400, 0x0100A000};
  //dest(0xFF) - src(0x00) - length(0x04) - channel(0x00)
  //tnum(0x01) - command(0x00) - data(0xA0) - padding(0x00)

  /* Frame used to READ BACK ccu25 CTRL A */
  tscType32 frame_readback_ctrlA[VME_MAX_FRAME_LENGTH] = {0xFF000300, 0x01100000};
  //dest(0xFF) - src(0x00) - length(0x03) - channel(0x00)
  //tnum(0x01) - command(0x10) - padding(0x00) - padding(0x00)

  /* Frame used to initialize ccu25 CTRL E */
  tscType32 frame_init_ctrlE[VME_MAX_FRAME_LENGTH] = {0xFF000600, 0x010400FF, 0xFF000000};
  //dest(0xFF) - src(0x00) - length(0x06) - channel(0x00)
  //tnum(0x01) - command(0x04) - data(0x00) - data(0xFF)
  //data(0xFF) - padding(0x00) - padding(0x00) - padding(0x00)


  /* Frame used to READ BACK ccu25 CTRL E */
  tscType32 frame_readback_ctrlE[VME_MAX_FRAME_LENGTH] = {0xFF000300, 0x01140000};
  //dest(0xFF) - src(0x00) - length(0x03) - channel(0x00)
  //tnum(0x01) - command(0x14) - padding(0x00) - padding(0x00)


  /* Frame used to Write I2C CRA on CCU25 (iniot I2C channels)*/
  tscType32 frame_init_i2c_ctrlA[VME_MAX_FRAME_LENGTH] = {0xFF000410, 0x01F04000};
  //dest(0xFF) - src(0x00) - length(0x04) - channel(0x10)
  //tnum(0x01) - command(0xF0) - data(0x40) - padding(0x00)

  /* Frame used to Read back the CCU25 I2C CRA */
  tscType32 frame_readback_i2c_ctrlA[VME_MAX_FRAME_LENGTH] = {0xFF000310, 0x01F10000};
  //dest(0xFF) - src(0x00) - length(0x03) - channel(0x10)
  //tnum(0x01) - command(0xF1) - padding(0x00) - padding(0x00)


  /* Frame used to detect I2C devices */
  tscType32 frame_detect_i2c[VME_MAX_FRAME_LENGTH] = {0xFF0004FF, 0x0101FF00};
  //dest(0xFF) - src(0x00) - length(0x04) - channel(0xFF)
  //tnum(0x01) - command(0x01) - i2cAddress(0xFF) - padding(0x00)



  /* Initialisation of the crate ; even if we do not know what we have on the crate slots,
     create an instance of a vme object to be able to go and probe the slot. */
  for (int i=0; i<21; i++)
    { 
      //create instance
      vmeCrate[i].baseAddress = baseAddressList[i];
      vmeCrate[i].fecboard = FecVmeRingDevice::configureHardBaseAddress( 0, FECBOARD_ADDRESSTABLE, vmeCrate[i].baseAddress, i, FecVmeRingDevice::STR_BUSADAPTER_NAME[SBS] );
      //probe
      (FecVmeRingDevice::detectVmeBoard(vmeCrate[i].fecboard, i+1) == 0) ? vmeCrate[i].isPresent = true : vmeCrate[i].isPresent = false;
    }


  /* If probing is successful, create the ring devices. */
  std::cout << std::endl << std::endl << "Probing Crate..." << std::endl;
  for (int i=0; i<21; i++)
    {
      if (vmeCrate[i].isPresent == true)
	{
	  std::cout << "SLOT " << i+1 << " :: VME board detected" << std::endl;
	  for (int j=0; j<8; j++)
	    {
	      vmeCrate[i].ringsList[j] = new FecVmeRingDevice(vmeCrate[i].fecboard, i+1, j);
	    }
	  std::cout << "     Rings 0 to 7 created for this board" << std::endl;
	}
      else std::cout << "SLOT " << i+1 << " :: ..." << std::endl;
    }
	


  int ringNumber, slotNumber;
  bool fixedRing = false;
  bool fixedSlot = false;


  //Check if user wants to run with default settings
  switch (argc)
    {

    case 2:
      slotNumber = atoi(argv[1]);
      if ( (slotNumber<0) || (slotNumber>20) )
	{
	  std::cout << "Your VME slot number must be in [0..20] ; Default slot number NOT setted." << std::endl;
	}
      else
	{
	  fixedSlot = true;
	  std::cout << "Default VME slot number ok ; You will work on VME slot : " << slotNumber << std::endl;
	}
      std::cout << "Default ring number not provided ; Default ring number NOT setted." << std::endl;

      break;


    case 3:
      slotNumber = atoi(argv[1]);
      if ( (slotNumber<0) || (slotNumber>20) )
	{
	  std::cout << "Your VME slot number must be in [0..20] ; Default slot number NOT setted." << std::endl;
	}
      else
	{
	  fixedSlot = true;
	  std::cout << "Default VME slot number ok ; You will work on VME slot : " << slotNumber << std::endl;
	}

      ringNumber = atoi(argv[2]);
      if ( (ringNumber<0) || (ringNumber>7) )
	{
	  std::cout << "Your ring number must be in [0..7] ; Default ring number NOT setted." << std::endl;
	}
      else
	{
	  fixedRing = true;
	  std::cout << "Default Ring number ok ; You will work on Ring : " << ringNumber << std::endl;
	}
      break;
			
    default:
      std::cout << "This program can be called as : Program.bin DEFAULT_VME_SLOT DEFAULT_FEC_RING to set default values." << std::endl;
      break;
    }
	


  bool loop = true;
  int option;
  unsigned long value16, value32;
  unsigned long watchdog = 0;
  //	int err;
	
  while ( loop )
    {
      std::cout << std::endl;
      std::cout << "0) End Program" << std::endl;
      std::cout << "1) Read CR0" << std::endl;
      std::cout << "2) Write CR0" << std::endl;
      //std::cout << "3) Read CR1" << std::endl;
      std::cout << "3) Write CR1" << std::endl;
      std::cout << "4) Read SR0" << std::endl;
      std::cout << "5) Read SR1" << std::endl;
      std::cout << "6) Write a word in Fifo Transmit" << std::endl;
      std::cout << "7) Read a word from Fifo Transmit" << std::endl;
      std::cout << "8) Write a word in Fifo Receive" << std::endl;
      std::cout << "9) Read a word from Fifo Receive" << std::endl;
      std::cout << "10) Write a word in Fifo Return" << std::endl;
      std::cout << "11) Read a word from Fifo Return" << std::endl;
      std::cout << "12) Write a frame on the ring" << std::endl;
      std::cout << "13) Read a frame from fifo receive" << std::endl;
      std::cout << "14) Reset Ring" << std::endl;
      std::cout << "15) Reset Crate" << std::endl;
      //std::cout << "16) Write a frame via toolbox" << std::endl;
      //std::cout << "17) Read a frame via toolbox" << std::endl;
      std::cout << "18) Quick scanRing - detects CCUs only" << std::endl;
      std::cout << "19) Full scanRing" << std::endl;
      std::cout << "20) set default VME Slot" << std::endl;
      std::cout << "21) set default Ring" << std::endl;
      std::cout << "22) unset default VME Slot" << std::endl;
      std::cout << "23) unset default Ring" << std::endl;
      std::cout << "24) display current default VME Slot" << std::endl;
      std::cout << "25) display current default Ring" << std::endl;
		
      std::cout << "Enter option : ";
      //std::cin >> option;
      fscanf(stdin, "%d", &option);

      switch (option)
	{
	  //Quit program
	case 0:
	  loop = false;
	  break;


	  //Read CRO
	case 1:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value16 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingCR0();
	  std::cout << "CR0 value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value16 << std::endl;
	  break;

			
	  //Write CR0
	case 2:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  std::cout << "Enter value (hex) : 0x";
	  std::cin >> std::hex >> value16;
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0((tscType16)value16);
	  std::cout << "I have wrote  : 0x" << std::hex << value16 << " in CR0 of slot " << slotNumber << ", ring " << ringNumber << std::endl;
	  break;



	  //Write CR1			
	case 3:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  std::cout << "Enter value (hex) : 0x";
	  std::cin >> std::hex >> value16;
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR1((tscType16)value16);
	  std::cout << "I have wrote  : 0x" << std::hex << value16 << " in CR1 of slot " << slotNumber << ", ring " << ringNumber << std::endl;
	  break;






	  //Read SR0
	case 4:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value16 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0();
	  std::cout << "SR0 value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value16 << std::endl;
	  dbg_decode_status0(value16);
	  break;



	  //Read SR1
	case 5:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value16 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR1();
	  std::cout << "SR1 value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value16 << std::endl;
	  break;


	  //Write to fifo transmit			
	case 6:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;

	  std::cout << "Enter 32 bits value (hex) : 0x";
	  std::cin >> std::hex >> value32;
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFifoTransmit((tscType32)value32);
	  std::cout << "I have wrote  : 0x" << std::hex << value32 << " in fifo transmit on slot " << slotNumber << ", ring " << ringNumber << std::endl;
	  break;


	  //Read from fifo transmit
	case 7:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value32 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFifoTransmit();
	  std::cout << "Fifo transmit value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value32 << std::endl;
	  break;


	  //Write to fifo receive
	case 8:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  std::cout << "Enter 32 bits value (hex) : 0x";
	  std::cin >> std::hex >> value32;
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFifoReceive((tscType32)value32);
	  std::cout << "I have wrote  : 0x" << std::hex << value32 << " in fifo receive on slot " << slotNumber << ", ring " << ringNumber << std::endl;
	  break;


	  //Read from fifo receive
	case 9:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value32 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFifoReceive();
	  std::cout << "Fifo receive value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value32 << std::endl;
	  break;


	  //Write to fifo return
	case 10:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  std::cout << "Enter 32 bits value (hex) : 0x";
	  std::cin >> std::hex >> value32;
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFifoReturn((tscType32)value32);
	  std::cout << "I have wrote  : 0x" << std::hex << value32 << " in fifo return on slot " << slotNumber << ", ring " << ringNumber << std::endl;

	  break;


	  //Read from fifo return
	case 11:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;
	  value32 = (unsigned long)vmeCrate[slotNumber-1].ringsList[ringNumber]->getFifoReturn();
	  std::cout << "Fifo return value read on Slot " << slotNumber << ", Ring " << ringNumber << " is : 0x" << std::hex << value32 << std::endl;
	  break;


	  //Write frame
	case 12:
	  tscType16 word3, word4;
	  int frame_length, frame32_length, i;
	  tscType32 frame[VME_MAX_FRAME_LENGTH];
	  //int lcl_err;
	  tscType16 cr0memory;


	  //Check if ring number was given at startup
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;


	  //Catch user input (frame to write) and
	  //write it in fifo transmit
	  std::cout << "How many 32 bits elements are in your frame ?";
	  std::cin >> frame_length;
	  if (frame_length > VME_MAX_FRAME_LENGTH)
	    {
	      std::cout << "Error : too long frame length." << std::endl;
	      break;
	    }
	  for (int i=0; i<frame_length ; i++)
	    {
	      std::cout << "Enter 32 bits value (hex) for element frame[" << i << "] : 0x";
	      std::cin >> std::hex >> frame[i];
	      vmeCrate[slotNumber-1].ringsList[ringNumber]->setFifoTransmit((tscType32)frame[i]);

	    }
				
	  //frame[0] = 0x44000400;
	  //frame[1] = 0x0100A000;
	  //Write frame on ring - toggle CR0
	  std::cout << "Writing frame on ring..." << std::endl;
	  cr0memory = vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingCR0();
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0(cr0memory | 3);
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0(cr0memory);

	  // now, wait for the direct ack ..

	  while ( (vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0() & DD_FEC_REC_EMPTY) && (watchdog++ < 100000) );

	  // No anwer
	  if (vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0() & DD_FEC_REC_EMPTY)
	    {
	      std::cout << "Timeout ; no direct ACK returned." << std::endl;
	      break;
	    }

	  //OK, we got an answer. Read word 1 and compute frame length
	  i = 0;
	  while (!(vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0() & DD_FEC_REC_EMPTY))
	    {
	      frame[i] = vmeCrate[slotNumber-1].ringsList[ringNumber]->getFifoReceive();
	      i++;
	    }
	  word3 = (tscType16)( (frame[0] >> 8) & 0xFF );
	  word4 = (tscType16)( frame[0] & 0xFF );
	  if (word3 & 0x0080)
	    {
	      frame_length = ( ((word3 & 0x007F)<<8) + word4 ) + 4;
	    }
	  else frame_length = word3 + 3;
	  frame32_length = (int)(frame_length / 4);
	  frame32_length++;
	  std::cout << "Direct ACK length is " << frame32_length << " 32 bits words." << std::endl;
	  for (i=0 ; i<frame32_length; i++)
	    {
	      std::cout << "received direct ack frame[" << i << "0] is : " << std::hex << frame[i] << std::endl;
	    }
				
	  //Clear Errors
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0(3);
	  break;


	  //Read frame
	case 13:
	  //tscType32 frame[20];
	  //tscType32 frame[VME_MAX_FRAME_LENGTH];
	  //int lcl_err;
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;

				
	  if (vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0() & DD_FEC_REC_EMPTY)
	    {
	      std::cout << "Fifo receive is empty ; no frame to read." << std::endl;
	      break;
	    }

	  //OK, we got an answer. Read word 1 and compute frame length
	  i = 0;
	  while (!(vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingSR0() & DD_FEC_REC_EMPTY))
	    {
	      frame[i] = vmeCrate[slotNumber-1].ringsList[ringNumber]->getFifoReceive();
	      i++;
	    }

	  word3 = (tscType16)( (frame[0] >> 8) & 0xFF );
	  word4 = (tscType16)( frame[0] & 0xFF );
	  if (word3 & 0x0080)
	    {
	      frame_length = ( ((word3 & 0x007F)<<8) + word4 ) + 4;
	    }
	  else frame_length = word3 + 3;
	  frame32_length = (int)(frame_length / 4);
	  frame32_length++;
	  std::cout << "Forced ACK or Data length is " << frame32_length << " 32 bits words." << std::endl;
	  for (i=0 ; i<frame32_length; i++)
	    {
	      std::cout << "received Data or Forced Ack frame[" << i << "0] is : " << std::hex << frame[i] << std::endl;
	    }
	  break;


	  //Reset ring
	case 14:
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;

	  value16 = vmeCrate[slotNumber-1].ringsList[ringNumber]->getFecRingCR0();
	  value16 = (value16 | 0x8000);
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0((tscType16)value16);
	  value16 = (value16 & 0x7FFF);
	  vmeCrate[slotNumber-1].ringsList[ringNumber]->setFecRingCR0((tscType16)value16);
	  break;
			

	  // Reset crate
	case 15:
	  //checkring(fixedRing, &ringNumber);
	  std::cout << "Resetting VME crate ; please wait few seconds..." << std::endl;
	  FecVmeRingDevice::crateReset( );
	  std::cout << "VME crate has been resetted." << std::endl;
	  break;


	  //Write a frame via the fec toolbox
	case 16:
	  //Check if ring number was given at startup
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;



	  //Catch user input (frame to write)
	  std::cout << "How many 32 bits elements are in your frame ?";
	  std::cin >> frame_length;
	  if (frame_length > VME_MAX_FRAME_LENGTH)
	    {
	      std::cout << "Error : too long frame length." << std::endl;
	      break;
	    }
	  for (int i=0; i<frame_length ; i++)
	    {
	      std::cout << "Enter 32 bits value (hex) for element frame[" << i << "] : 0x";
	      std::cin >> std::hex >> frame[i];
	    }
			
	  //Display frame for verification
	  for (int i=0; i<frame_length ; i++)
	    {
	      std::cout << "writing frame[" << i << "0] = : " << std::hex << frame[i] << std::endl;
	    }

	  //write frame
	  FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
				
	  //Display D-ACK frame for verification
	  for (int i=0; i<frame_length ; i++)
	    {
	      std::cout << "Received D-ACK frame[" << i << "0] = : " << std::hex << frame[i] << std::endl;
	    }
	  break;


	  //Read a frame via the fec toolbox
	case 17:
	  //Check if ring number was given at startup
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;


	  //read frame
	  FecFunctions::ffReadFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
				
	  //Display frame for verification
	  for (int i=0; i<frame_length ; i++)
	    {
	      std::cout << "Received F-ACK or DATA frame[" << i << "0] = : " << std::hex << frame[i] << std::endl;
	    }
	  break;




	  //Quick scanring ; detects only CCUs 
	case 18:
	  int ccu_counter;
	  tscType32 frame_head_mask;
	  tscType16 status;
				
	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;

	  fprintf(stdout,"Scanning ring for CCU's\n");
	  fprintf(stdout,"\tscanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
	  for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
	    {
	      //prepare frame header
	      frame[0] = frame_init_ctrlA[0] & 0x00FFFFFF;
	      //printf("frame[0] = frame_init_ctrlA[0] & 0x00FFFFFF --> 0x%x\n", (unsigned int)frame[0]);
	      frame_head_mask = ccu_counter;
	      //printf("frame_head_mask = ccu_counter; --> 0x%x\n", (unsigned int)frame_head_mask);					
	      frame_head_mask = frame_head_mask << 24;
	      //printf("frame_head_mask = frame_head_mask << 24; --> 0x%x\n", (unsigned int)frame_head_mask);					
	      frame[0] = frame[0] | frame_head_mask;
	      //printf("frame[0] = frame[0] | frame_head_mask; --> 0x%x\n", (unsigned int)frame[0]);

	      //prepare whole frame
	      fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
	      FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
	      for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
		{
		  frame[lcl_j] = frame_init_ctrlA[lcl_j];
		}
	      //DEBUG
	      /*
		for (int lcl_j=0 ; lcl_j<frame32_length ; lcl_j++)
		{
		printf("I will write in fifo transmit : 0x%x\n",(unsigned int)frame[lcl_j]);
		}
	      */
	      //write frame and read back ack
	      int lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
	      //FecFunctions::ffWriteFrame(frame, ringsOnBoard[ringNumber]);

	      if (lcl_err == DD_RETURN_OK)
		{
		  lcl_err = FecFunctions::ffGetFrameStatus( frame, &status );
		  //fprintf(stdout,"\nStatus is : 0x%x\n\n",status);
						
		  if ( (lcl_err == DD_RETURN_OK) && (status==0xb0) )
		    {
		      fprintf(stdout,"\n\t\tCCU number 0x%x detected.\n",ccu_counter);
		    }
		}

	    }
	  break;



	  //Full scanring
	case 19:

	  if (checkslot(fixedSlot, &slotNumber) != 0) break;
	  if (checkring(fixedRing, &ringNumber) != 0) break;

	  /* print header */
	  fprintf(stdout,"\n\nScanning ring for NEW CCU's - Devices are probed via READ method\n");
	  fprintf(stdout,"scanned CCU's range is [0x%x .. 0x%x]\n", MIN_CCU, MAX_CCU);
	  fprintf(stdout,"scanned i2c channels range is [0x%x .. 0x%x]\n", MIN_I2C_CH25, MAX_I2C_CH25);
	  fprintf(stdout,"scanned i2c addresses (per channel) range is [0x%x .. 0x%x]\n", MIN_I2C_ADR, MAX_I2C_ADR);


	  for (ccu_counter = MIN_CCU; ccu_counter <= MAX_CCU ; ccu_counter++)
	    {
	      //prepare frame header (set CCU)
	      frame[0] = frame_init_ctrlA[0] & 0x00FFFFFF;
	      frame_head_mask = ccu_counter;
	      frame_head_mask = frame_head_mask << 24;
	      frame[0] = frame[0] | frame_head_mask;
					
	      //prepare whole frame
	      fprintf(stdout,"Probing ccu 0x%x \r", ccu_counter);
	      FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
	      for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
		{
		  frame[lcl_j] = frame_init_ctrlA[lcl_j];
		}
	      //write frame and read back ack
	      int lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
	      tscType16 status;
	      FecFunctions::ffGetFrameStatus( frame, &status );
	      if ( (lcl_err == DD_RETURN_OK) && (status==0xb0) )
		{
		  //fprintf(stdout,"Reading back CCU25 CTRL-A\n");
		  fprintf(stdout,"\nCCU number 0x%x detected.\n",ccu_counter);

		  //prepare frame header (set CCU)
		  frame[0] = frame_readback_ctrlA[0] & 0x00FFFFFF;
		  frame_head_mask = ccu_counter;
		  frame_head_mask = frame_head_mask << 24;
		  frame[0] = frame[0] | frame_head_mask;
					
		  //prepare whole frame
		  FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
		  for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
		    {
		      frame[lcl_j] = frame_readback_ctrlA[lcl_j];
		    }
		  //write frame and read back ack
		  lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		  if (lcl_err == DD_RETURN_OK)
		    {
		      //fprintf(stdout,"Reading back CCU25 CTRL-A\n");
		      lcl_err = FecFunctions::ffReadFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		      if (lcl_err != DD_RETURN_OK)
			{
			  std::cout << "Error while reading back CCU CTRL-A" << std::endl;
			}
		      //else fprintf(stdout,"CTRLA really read back.\n");

		    }
		  else std::cout << "Error while initializing CCU CTRL-A" << std::endl;


		  //fprintf(stdout,"Initializing CCU25 CTRL-E\n");
		  //prepare frame header (set CCU)
		  frame[0] = frame_init_ctrlE[0] & 0x00FFFFFF;
		  frame_head_mask = ccu_counter;
		  frame_head_mask = frame_head_mask << 24;
		  frame[0] = frame[0] | frame_head_mask;
					
		  //prepare whole frame
		  FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
		  for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
		    {
		      frame[lcl_j] = frame_init_ctrlE[lcl_j];
		    }
		  //write frame and read back ack
		  lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		  if (lcl_err != DD_RETURN_OK)
		    {
		      std::cout << "Error while initializing CCU CTRL-E" << std::endl;
		    }


		  //fprintf(stdout,"Reading back CCU25 CTRL-E\n");
		  //prepare frame header (set CCU)
		  frame[0] = frame_readback_ctrlE[0] & 0x00FFFFFF;
		  frame_head_mask = ccu_counter;
		  frame_head_mask = frame_head_mask << 24;
		  frame[0] = frame[0] | frame_head_mask;
					
		  //prepare whole frame
		  FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
		  for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
		    {
		      frame[lcl_j] = frame_readback_ctrlE[lcl_j];
		    }
		  //write frame and read back ack
		  lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		  if (lcl_err == DD_RETURN_OK)
		    {
		      lcl_err = FecFunctions::ffReadFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		      if (lcl_err != DD_RETURN_OK)
			{
			  std::cout << "Error while reading back CCU CTRL-E" << std::endl;
			}
		      //else fprintf(stdout,"CTRL E really read back.\n");

		    }
		  else std::cout << "Error while trying to read back CCU CTRL-E" << std::endl;




		  //now, i2c channels are initialised and switched on ; let's try to probe them
		  for (int channel_counter = MIN_I2C_CH25; channel_counter <= MAX_I2C_CH25 ; channel_counter++)
		    {
		      //fprintf(stdout,"\tNow probing for i2c devices attached to channel 0x%x of CCU 0x%x ; be patient ...\n", channel_counter, ccu_counter);
		      //fprintf(stdout,"\tInitializing I2C channel CRA\n");
						
		      //prepare frame header (set CCU)
		      frame[0] = frame_init_i2c_ctrlA[0] & 0x00FFFFFF;
		      frame_head_mask = ccu_counter;
		      frame_head_mask = frame_head_mask << 24;
		      frame[0] = frame[0] | frame_head_mask;

		      //prepare frame header (set channel)
		      frame_head_mask = channel_counter & 0x000000FF;
		      frame[0] = (frame[0] & 0xFFFFFF00) | frame_head_mask;
					
		      //prepare whole frame
		      FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
		      for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
			{
			  frame[lcl_j] = frame_init_i2c_ctrlA[lcl_j];
			}
		      //write frame and read back ack
		      lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		      if (lcl_err != DD_RETURN_OK)
			{
			  std::cout << "Error while initializing CCU I2C CTRL-A register" << std::endl;
			}
							

		      //fprintf(stdout,"\tReading back I2C channel CRA\n");

		      //prepare frame header (set CCU)
		      frame[0] = frame_readback_i2c_ctrlA[0] & 0x00FFFFFF;
		      frame_head_mask = ccu_counter & 0xFF;
		      frame_head_mask = frame_head_mask << 24;
		      frame[0] = frame[0] | frame_head_mask;

		      //prepare frame header (set channel)
		      frame_head_mask = channel_counter & 0x000000FF;
		      frame[0] = (frame[0] & 0xFFFFFF00) | frame_head_mask;
					
		      //prepare whole frame
		      FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
		      for (int lcl_j=1 ; lcl_j<frame32_length ; lcl_j++)
			{
			  frame[lcl_j] = frame_readback_i2c_ctrlA[lcl_j];
			}
		      //write frame and read back ack
		      lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
		      if (lcl_err == DD_RETURN_OK)
			{
			  lcl_err = FecFunctions::ffReadFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
			  if (lcl_err != DD_RETURN_OK)
			    {
			      std::cout << "Error while reading back CCU I2C CTRL-A register" << std::endl;
			    }
			  //else fprintf(stdout,"CCU I2C CTRL A readback OK.\n");							
			}
		      else std::cout << "Error while trying to read back CCU I2C CTRL-A register" << std::endl;




		      // Now, scan for devices on this channel.
		      for (int i2c_counter = MIN_I2C_ADR; i2c_counter <= MAX_I2C_ADR ; i2c_counter++)
			{
			  //fprintf(stdout,"\t\tprobing i2c address 0x%x on channel 0x%x of CCU 0x%x ; be patient ...\r", i2c_counter, channel_counter, ccu_counter);
			  //Build stack for Normal Mode testing

			  //prepare frame header (set CCU)
			  frame[0] = frame_detect_i2c[0] & 0x00FFFFFF;
			  frame_head_mask = ccu_counter & 0x000000FF;
			  frame_head_mask = frame_head_mask << 24;
			  frame[0] = frame[0] | frame_head_mask;

			  //prepare frame header (set channel)
			  frame_head_mask = channel_counter & 0xFF;
			  frame[0] = (frame[0] & 0xFFFFFF00)| frame_head_mask;

			  //prepare frame header (set I2C address)
			  frame[1] = frame_detect_i2c[1] & 0xFFFF00FF;
			  frame_head_mask = i2c_counter & 0xFF;
			  frame_head_mask = frame_head_mask << 8;
			  frame[1] = frame[1] | frame_head_mask;
					
			  //prepare whole frame
			  FecFunctions::ffComputeFrame32Length( frame[0], &frame32_length );
			  for (int lcl_j=2 ; lcl_j<frame32_length ; lcl_j++)
			    {
			      frame[lcl_j] = frame_detect_i2c[lcl_j];
			    }
								
								
			  //write frame and read back ack
			  lcl_err = FecFunctions::ffWriteFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);

			  if (lcl_err == DD_RETURN_OK)
			    {
			      //std::cout << "Waiting for device addr. 0x%x data frame... " << std::hex << i2c_counter <<endl;

			      lcl_err = FecFunctions::ffReadFrame(frame, vmeCrate[slotNumber-1].ringsList[ringNumber]);
			      if (lcl_err == DD_RETURN_OK)
				{

				  tscType16 status, status1;
				  FecFunctions::ffGetFrameStatus(frame, &status);
				  FecFunctions::ffGetDeviceFrameStatus(frame, &status1);
				  if ((status == 0x80) && (status1 == 0x04))
				    {
				      fprintf(stdout,"\tI2C device detected at address 0x%x on channel 0x%x of CCU 0x%x        \n",i2c_counter, channel_counter, ccu_counter);
				    }
										
				}


			    }

			}
		    }
					
		}
	    }
	  fprintf(stdout,"END of Scan.\n");

	  break;


	case 20:
	  std::cout << "Which VME slot do you want to use as default ? (0-20) : ";
	  std::cin >> slotNumber;
	  if ( (slotNumber<0) || (slotNumber>20) )
	    {
	      std::cout << "Your VME slot number must be in [0..20] ; Default slot number NOT setted." << std::endl;
	    }
	  else
	    {
	      fixedSlot = true;
	      std::cout << "Default VME slot number ok ; You will work on VME slot : " << slotNumber << std::endl;
	    }
	  break;

	case 21:
	  std::cout << "Which Ring slot do you want to use as default ? (0-7) : ";
	  std::cin >> ringNumber;
	  if ( (ringNumber<0) || (ringNumber>7) )
	    {
	      std::cout << "Your ring number must be in [0..7] ; Default ring number NOT setted." << std::endl;
	    }
	  else
	    {
	      fixedRing = true;
	      std::cout << "Default ring number ok ; You will work on ring : " << ringNumber << std::endl;
	    }
	  break;

	case 22:
	  fixedSlot = false;
	  std::cout << "Default VME slot number resetted." << std::endl;
	  break;


	case 23:
	  fixedRing = false;
	  std::cout << "Default Ring number resetted." << std::endl;
	  break;



	case 24:
	  if (fixedSlot == false)
	    {
	      std::cout << "Default VME slot number unsetted." << std::endl;
	    }
	  else std::cout << "Current default VME slot number is : " << slotNumber << std::endl;
	  break;


	case 25:
	  if (fixedRing == false)
	    {
	      std::cout << "Default Ring number unsetted." << std::endl;
	    }
	  else std::cout << "Current default Ring number is : " << ringNumber << std::endl;
	  break;

	default:
	  std::cout << "You typed an invalid option number!" << std::endl;
	  break;
	}
    }



  for (int i=0; i<21; i++)
    if (vmeCrate[i].isPresent == true)
      for (int j=0; j<8; j++)
	free(vmeCrate[i].ringsList[j]);


  FecVmeRingDevice::closeFecVmeAccess( );

  return 0;
}


