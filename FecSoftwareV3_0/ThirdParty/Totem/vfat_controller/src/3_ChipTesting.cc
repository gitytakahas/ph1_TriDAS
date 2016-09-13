/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#ifdef BUSUSBFEC

#include <sstream>  // Stringstream

#include <qmessagebox.h> // GUI
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qdir.h>

#include "vfat_controller.h" // SW header


/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Crepe Methods                                                                     */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */

// NOTE: The TTP/CREPE firmware version MUST end with letter !!!
//       Otherwise, the wrong communication protocol is chosen...
//       See fec_usb_open() for details.

void vfat_controller::setCrepeControl1( fecusbType32 value ){

  daq_regs_[0] = value ;
}

fecusbType32 vfat_controller::getCrepeControl1(){

  return daq_regs_[0] ;

}

void vfat_controller::setCrepeControl3( fecusbType32 value ){

  daq_regs_[2] = value ;
}

fecusbType32 vfat_controller::getCrepeControl3(){

  return daq_regs_[2] ;
}

void vfat_controller::setCrepeLat( fecusbType32 value ){

  daq_regs_[5] = value ;
}

fecusbType32 vfat_controller::getCrepeLat(){

  return daq_regs_[5] ;
}

void vfat_controller::setCrepeTriggerEnabledPeriod( fecusbType32 value ){

  daq_regs_[6] = value ;
}

fecusbType32 vfat_controller::getCrepeTriggerEnabledPeriod(){

  return daq_regs_[6] ;
}

void vfat_controller::setCrepeSettings( int option ){

  // Default settings
  if(option==0){

    daq_regs_[0] = CREPE_CONTROL1 ;
    daq_regs_[1] = CREPE_CONTROL2 ;
    daq_regs_[2] = CREPE_CONTROL3 ;
    daq_regs_[3] = CREPE_FIRMWARE ;
    daq_regs_[4] = CREPE_STATUS ;
    daq_regs_[5] = CREPE_LAT ;
    daq_regs_[6] = CREPE_TRIGENABLE ; //CREPE_TRIGENABLE ; //0x00000F09 //0x00000FFF
    daq_regs_[7] = CREPE_TRIGDISABLE ;
    daq_regs_[8] = CREPE_BC0 ;
  }

  // Data packet check settings
  if(option==1){

    daq_regs_[0] = CREPE_CONTROL1_OPT1 ;
    daq_regs_[2] = CREPE_CONTROL3_OPT1 ;
    daq_regs_[5] = CREPE_LAT_OPT1 ;
    daq_regs_[6] = CREPE_TRIGENABLE_OPT1 ;
  }

  // Select the data input
  if(detectorType_ == "RP"){ daq_regs_[2] = daq_regs_[2] | CREPE_RPDATA_MASK ; }
  else{ daq_regs_[2] = daq_regs_[2] & CREPE_GEMDATA_MASK ; }

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Settings... " << detectorType_ << " CR3=" << std::hex << daq_regs_[2] << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif
}

void vfat_controller::writeCrepeSettings( int device ){

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Writing... " << detectorType_ << " CR3=" << std::hex << daq_regs_[2] << std::dec << SET_DEFAULT_COLORS << std::endl ;
#endif

  int ftStatus = FECUSB_RETURN_OK ;

  ftStatus = fec_usb_rw_block_dev(device,7,CREPE_BASE_ADDR,daq_regs_,CRCOUNT);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  // Read the pattern back to check
#ifdef DEBUGMODE

  fecusbType32 readvalue[CRCOUNT]; 
  for(int i=0;i<CRCOUNT-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(device,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  std::cout << SET_FORECOLOR_CYAN << "Content of local registers:" << SET_DEFAULT_COLORS << std::endl ;
  for(int j=0;j<CRCOUNT;j++) { printf("0x%04x 0x%08x \n",j+CREPE_BASE_ADDR,readvalue[j]) ; }
  printf("\n") ;

#endif

}

void vfat_controller::readCrepeSettings( int device , fecusbType32 *readvalue){

  int ftStatus = FECUSB_RETURN_OK ;

  ftStatus = fec_usb_rw_block_dev(device,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

}

void vfat_controller::writeCrepeTrigger( int device ){

  int ftStatus = FECUSB_RETURN_OK ;
  
  // Create crepe trigger pattern
  fecusbType32 trig_regs[TRIGPATTERNLENGTH_S] ;
  for(int i=0;i<TRIGPATTERNLENGTH_S-1;i++){ trig_regs[i] = SPACING_CAL ; }
  trig_regs[TRIGPATTERNLENGTH_S-1] = 0x0000000 ;
  
  // Write pattern to the crepe
  ftStatus = fec_usb_rw_block_dev(device,7,TRIGPATTERN_BASE,trig_regs,TRIGPATTERNLENGTH_S);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  // Read the pattern back to check
#ifdef DEBUGMODE

  fecusbType32 readvalue[TRIGPATTERNLENGTH_S]; 
  for(int i=0;i<TRIGPATTERNLENGTH_S-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(device,8,TRIGPATTERN_BASE,readvalue,TRIGPATTERNLENGTH_S) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  std::cout << SET_FORECOLOR_CYAN << "Trigger pattern:" << SET_DEFAULT_COLORS << std::endl ;
  for(int j=0;j<TRIGPATTERNLENGTH_S;j++) { printf("0x%04x 0x%08x \n",j+TRIGPATTERN_BASE,readvalue[j]) ; }
  printf("\n") ;

#endif

}

void vfat_controller::writeFastCrepeTrigger( int device){

  int ftStatus = FECUSB_RETURN_OK ;
 
  // Create crepe trigger pattern
  fecusbType32 trig_regs[TRIGPATTERNLENGTH_L] ;
  for(int i=0;i<TRIGPATTERNLENGTH_L-1;i++){ trig_regs[i] = SPACING_NOCAL ; }
  trig_regs[TRIGPATTERNLENGTH_L-1] = 0x0000000 ;

  // Write pattern to the crepe
  ftStatus = fec_usb_rw_block_dev(device,7,TRIGPATTERN_BASE,trig_regs,TRIGPATTERNLENGTH_L);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  // Read the pattern back to check
#ifdef DEBUGMODE

  fecusbType32 readvalue[TRIGPATTERNLENGTH_L]; 
  for(int i=0;i<TRIGPATTERNLENGTH_L-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(device,8,TRIGPATTERN_BASE,readvalue,TRIGPATTERNLENGTH_L) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  std::cout << SET_FORECOLOR_CYAN << "Trigger pattern:" << SET_DEFAULT_COLORS << std::endl ;
  for(int j=0;j<TRIGPATTERNLENGTH_L;j++) { printf("0x%04x 0x%08x \n",j+TRIGPATTERN_BASE,readvalue[j]) ; }
  printf("\n") ;

#endif

}

void vfat_controller::resetCrepeFifos( int device ){

  int ftStatus = FECUSB_RETURN_OK ;

  fecusbType32 reset_fifos[] = {
    0x00000100, // 0x0800013e
    0x08000020  // 0x0800003e
  } ;

  reset_fifos[1] = daq_regs_[2] ; // reset and return it to original status
 
  // Reset data buffers
  //std::cout << SET_FORECOLOR_CYAN << "Reseting data buffers... " ;
  ftStatus = fec_usb_rw_block_dev(0,5,CREPE_BASE_ADDR+2,reset_fifos,RESETPATTERNLENGTH);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << std::endl << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
  //else { std::cout << SET_FORECOLOR_CYAN << "OK" << SET_DEFAULT_COLORS << std::endl ; }

}

void vfat_controller::startCrepeTrigger( int device ){

  int ftStatus = FECUSB_RETURN_OK ;

  fecusbType32 go_with_triggers[] = {
    0x00001024, //1024
    0x00001004
  } ;

  if(simpleTrigger_){

    go_with_triggers[0] = 0x00001220 ;
    go_with_triggers[1] = 0x00001200 ;
  }

  // Start trigger sequence
  //std::cout << SET_FORECOLOR_CYAN << "Sending triggers... " ; 
  ftStatus = fec_usb_rw_block_dev(device,5,CREPE_BASE_ADDR,go_with_triggers,2);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << std::endl << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
  //else { std::cout << SET_FORECOLOR_CYAN << "OK" << SET_DEFAULT_COLORS << std::endl ; }
  
  //std::cout << SET_FORECOLOR_CYAN << "Wait for " << WAITTIME_MSEC << " ms... " << std::endl << SET_DEFAULT_COLORS << std::endl ; 
  //msleep(WAITTIME_MSEC) ;

}

void vfat_controller::crepeMasterReset( int device ){

  int ftStatus = FECUSB_RETURN_OK ;

  fecusbType32 master_reset[] = {
    0x00000010,
    0x00000000
  } ;


  // Start trigger sequence
  std::cout << SET_FORECOLOR_BLUE << "Master Reset... " ; 
  ftStatus = fec_usb_rw_block_dev(device,5,CREPE_BASE_ADDR,master_reset,2);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << std::endl << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
  else { std::cout << SET_FORECOLOR_BLUE << "OK" << SET_DEFAULT_COLORS << std::endl ; }
  
  //std::cout << SET_FORECOLOR_CYAN << "Wait for " << WAITTIME_MSEC << " ms... " << std::endl << SET_DEFAULT_COLORS << std::endl ; 
  //msleep(WAITTIME_MSEC) ;
}

void vfat_controller::enableCrepeExtTrigger( int device ){

  int ftStatus = FECUSB_RETURN_OK ;

  fecusbType32 go_with_triggers[] = {

    0x00000028,
    0x00000048
  } ;

  // Enable external trigger
  std::cout << SET_FORECOLOR_BLUE << "Enabling the external trigger." << SET_DEFAULT_COLORS << std::endl ; 
  ftStatus = fec_usb_rw_block_dev(device,5,CREPE_BASE_ADDR,go_with_triggers,1);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << std::endl << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

}

void vfat_controller::disableCrepeExtTrigger( int device ){

  int ftStatus = FECUSB_RETURN_OK ;

  fecusbType32 go_with_triggers[] = { 0x00001000 } ;

  // Disable external trigger
  std::cout << SET_FORECOLOR_BLUE << "Disabling the external trigger." << SET_DEFAULT_COLORS << std::endl ; 
  ftStatus = fec_usb_rw_block_dev(device,5,CREPE_BASE_ADDR,go_with_triggers,1);

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << std::endl << CREPE_WRITE_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

}

void vfat_controller::crepeDataTakingExtTrigger( int device ){

  bool abort = false ;
  std::string fn_data = vfat_controller_home_ + TTP_DATA_FILE ;

  std::cout << SET_FORECOLOR_BLUE << "WRITING DATA TO  " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;


  std::ofstream outputfilestream ;

  if (!outputfilestream.is_open()){

    outputfilestream.open(fn_data.c_str(), std::ios::binary ) ;
    outputfilestream << CREPE_HEADER ;
  }

  outputfilestream.close() ;


  writeCrepeSettings(0) ;
  resetCrepeFifos(0) ;

  enableCrepeExtTrigger(0) ;

 
  while(!abort_){

    // Read and write if data...
    readAndWriteData(0, fn_data.c_str()) ;
  }
  
  disableCrepeExtTrigger(0) ;
  std::cout << SET_FORECOLOR_BLUE << "\nWROTE DATA TO " << fn_data.c_str() << SET_DEFAULT_COLORS << std::endl ;

}

void vfat_controller::readAndWriteData( int device, std::string fileName ){


  // Data buffer readout

  fecusbType32 data_1[1024];
  fecusbType32 data_2[1024];
  fecusbType32 data_3[1024];
  fecusbType32 data_4[1024];

  std::vector<Totem::VFATFrame> frames_1 ;
  std::vector<Totem::VFATFrame> frames_2 ;
  std::vector<Totem::VFATFrame> frames_3 ;
  std::vector<Totem::VFATFrame> frames_4 ;

  fecusbType32 word_count = 0 ;
  int full_frames = 0 ;

  bool force_read = false ;
  int words_each_time = 1024 ;

  bool wrongNumberOfEvents = false ;

  int ftStatus = FECUSB_RETURN_OK ;


  // Read the status of the fifos

  fecusbType32 readvalue[CRCOUNT]; 
  for(int i=0;i<CRCOUNT-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(device,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }



  // Reading out the data buffers

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Reading 1st data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

  if ((readvalue[4] & 0x00000001) && !force_read ) {
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "1st fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
    return ;
#endif
  }
  else { 

    if (readvalue[4] & 0x00000002) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "1st fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
      return ;
#endif
    }
    else {

      //fecusbType32 words_to_read = 0 ; 
      ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1-1,&word_count,1) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "1st fifo contains " << word_count << " words" << SET_DEFAULT_COLORS << std::endl ;
#endif


      // Number of complete VFAT frames

      full_frames = floor(word_count / VFATPACKETLENGTH) ;
      word_count = full_frames*VFATPACKETLENGTH ;

      if(full_frames<=0){ return ; }

#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "READ " << word_count << " words from all fifos" << SET_DEFAULT_COLORS << std::endl ;
#endif

      if(showConsole_){ std::cout << SET_FORECOLOR_BLUE << "READ " << full_frames << " EVENT(S)" << SET_DEFAULT_COLORS << std::endl ; }

      if (force_read){ word_count = 1024 ; } 

      int burst_num = word_count  / words_each_time  ;
      int rest = word_count % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1,data_1+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }		  
      }	  
      if (rest) {

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1,data_1+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      }

      buildFramesFromBuffer( frames_1, (unsigned *)data_1, word_count ) ;
    } 
  }

  
#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Reading 2nd data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

  if ((readvalue[4] & 0x00000004) && !force_read ) {
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "2nd fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
  }
  else { 

    if (readvalue[4] & 0x00000008) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "2nd fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else {

      int burst_num = word_count  / words_each_time  ;
      int rest = word_count % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO2,data_2+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }		  
      }	  
      if (rest) {

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO2,data_2+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      }

      buildFramesFromBuffer( frames_2, (unsigned *)data_2, word_count ) ;
    } 
  }


#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Reading 3rd data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

  if ((readvalue[4] & 0x00000010) && !force_read ) {
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "3rd fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
  }
  else { 

    if (readvalue[4] & 0x00000020) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "3rd fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else {

      int burst_num = word_count  / words_each_time  ;
      int rest = word_count % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO3,data_3+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }		  
      }	  
      if (rest) {

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO3,data_3+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      }

      buildFramesFromBuffer( frames_3, (unsigned *)data_3, word_count ) ;
    } 
  }


#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Reading 4th data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

  if ((readvalue[4] & 0x00000040) && !force_read ) {
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "4th fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
  }
  else { 

    if (readvalue[4] & 0x00000080) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "4th fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else {

      int burst_num = word_count / words_each_time  ;
      int rest = word_count % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO4,data_4+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }		  
      }	  
      if (rest) {

	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO4,data_4+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      }

      buildFramesFromBuffer( frames_4, (unsigned *)data_4, word_count ) ;
    } 
  }




  std::ofstream outputfilestream ;

  if (!outputfilestream.is_open()){ outputfilestream.open(fileName.c_str(), std::ios::binary | std::ios::app) ; }
  

  if(outputfilestream.is_open()){

    Totem::VFATFrame frame ; 
    unsigned int ec = 0 ; 

    if(!frames_1.empty()){ ec = Totem::VFATFrame(frames_1.at(0)).getEC() ; }

    for(int i=0;i<full_frames;i++){

      if(ec>255){ ec = 0 ; }

      bool frameOK_1 = false, frameOK_2 = false, frameOK_3 = false, frameOK_4 = false ;
      unsigned short nFrames = 0 ;
      if(frames_1.size()>i && ec==Totem::VFATFrame(frames_1.at(i)).getEC()){ nFrames++ ; frameOK_1 = true ; }
      if(frames_2.size()>i && ec==Totem::VFATFrame(frames_2.at(i)).getEC()){ nFrames++ ; frameOK_2 = true ; }
      if(frames_3.size()>i && ec==Totem::VFATFrame(frames_3.at(i)).getEC()){ nFrames++ ; frameOK_3 = true ; }
      if(frames_4.size()>i && ec==Totem::VFATFrame(frames_4.at(i)).getEC()){ nFrames++ ; frameOK_4 = true ; }

      if(nFrames>0){ outputfilestream.write((char *)(&nFrames), sizeof(nFrames)) ; }

      //std::cout << "EC=" << ec << " nFrames=" << nFrames << std::endl ;


      if(frameOK_1){

	frame = frames_1.at(i) ;
	outputfilestream.write((char *)(&frame), sizeof(frame)) ;
      }
      if(frameOK_2){

	frame = frames_2.at(i) ;
	outputfilestream.write((char *)(&frame), sizeof(frame)) ;
      }
      if(frameOK_3){

	frame = frames_3.at(i) ;
	outputfilestream.write((char *)(&frame), sizeof(frame)) ;
      }
      if(frameOK_4){

	frame = frames_4.at(i) ;
	outputfilestream.write((char *)(&frame), sizeof(frame)) ;
      }

      ec++ ;
    }

    outputfilestream.close() ;
  }
}

void vfat_controller::buildFramesFromBuffer( std::vector<Totem::VFATFrame> &data, unsigned *vfat_data, int vfat_words_read ) {

  /*
  int numframes = 0 ;

  // Fill the VFAT frames if needed
  if(miniFrames_){

    for(int i=0;i<vfat_words_read;i++){

      if(i>=30){ printf("0x%04X ",vfat_data[i]) ; }
      if((i+1)%4==0 && i!=0){ printf("\n") ; }
    }

    numframes = vfat_words_read/MINIPACKETLENGTH ;

    //fecusbType32 vfat_data_fill[2048];
    //for(int i=0;i<2048;i++){ vfat_data_1[i] = 0x0000 ; }

    fecusbType32 vfat_data_fill[numframes*VFATPACKETLENGTH];
    for(int i=0;i<numframes*VFATPACKETLENGTH;i++){ vfat_data_fill[i] = 0x0000 ; }

    for(int i=0;i<numframes;i++){

      vfat_data_fill[i*VFATPACKETLENGTH] = vfat_data[i*MINIPACKETLENGTH] ;
      vfat_data_fill[i*VFATPACKETLENGTH+1] = vfat_data[i*MINIPACKETLENGTH+1] ;
      vfat_data_fill[i*VFATPACKETLENGTH+2] = vfat_data[i*MINIPACKETLENGTH+2] ;
      vfat_data_fill[i*VFATPACKETLENGTH+11] = vfat_data[i*MINIPACKETLENGTH+3] ;
    }

    for(int i=0;i<numframes*VFATPACKETLENGTH;i++){
      vfat_data[i] = vfat_data_fill[i] ;

    }

    //vfat_data = vfat_data_fill ;
  }
  else{

    numframes = vfat_words_read / VFATPACKETLENGTH ;
  }
  
  // Fill the vector
  for(int j=0;j<numframes;j++) {

    fecusbType16 tframe[VFATPACKETLENGTH] ; // 12

    for (int k=0;k<VFATPACKETLENGTH;k++){ tframe[k]=0x0000 ; }

    int datastart = VFATPACKETLENGTH * j ;

    for(int k=datastart, l=11;k<datastart+VFATPACKETLENGTH;k++,l--){

      tframe[l] = vfat_data[k] & 0xffff ;

      //if(miniFrames_){ tframe[l] = vfat_data_fill[k] & 0xffff ; }
      //else{ tframe[l] = vfat_data[k] & 0xffff ; }

      //fecusbType16 tmp = vfat_data_fill[k] & 0xffff ;
      //tframe[l] = tmp ;
    }

    Totem::VFATFrame frame((unsigned short*)tframe);
 
    if(tframe[0]!=0){ data.push_back(frame) ; }    
  }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << " ---> buildFramesFromBuffer(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

*/

  // Optional implementation
  int numframes = 0 ;

  // Fill the VFAT frames if needed
  if(miniFrames_){

#ifdef DEBUGMODE    
    /*
    for(int i=0;i<vfat_words_read;i++){

      printf("0x%04X ",vfat_data[i]) ;
      if((i+1)%4==0){ printf("\n") ; }
    }
    */

#endif   

    numframes = vfat_words_read/MINIPACKETLENGTH ;

    fecusbType32 vfat_data_fill[numframes*VFATPACKETLENGTH];
    for(int i=0;i<numframes*VFATPACKETLENGTH;i++){ vfat_data_fill[i] = 0x0000 ; }

    for(int i=0;i<numframes;i++){

      vfat_data_fill[i*VFATPACKETLENGTH] = vfat_data[i*MINIPACKETLENGTH] ;
      vfat_data_fill[i*VFATPACKETLENGTH+1] = vfat_data[i*MINIPACKETLENGTH+1] ;
      vfat_data_fill[i*VFATPACKETLENGTH+2] = vfat_data[i*MINIPACKETLENGTH+2] ;
      vfat_data_fill[i*VFATPACKETLENGTH+11] = vfat_data[i*MINIPACKETLENGTH+3] ;
    }

    // Fill the vector
    for(int j=0;j<numframes;j++) {

      fecusbType16 tframe[VFATPACKETLENGTH] ; // 12

      for (int k=0;k<VFATPACKETLENGTH;k++){ tframe[k]=0x0000 ; }

      int datastart = VFATPACKETLENGTH * j ;

      for(int k=datastart, l=11;k<datastart+VFATPACKETLENGTH;k++,l--){

	tframe[l] = vfat_data_fill[k] & 0xffff ;
      }

      Totem::VFATFrame frame((unsigned short*)tframe);
 
      if(tframe[0]!=0){ data.push_back(frame) ; }    
    }
  }
  else{

    numframes = vfat_words_read / VFATPACKETLENGTH ;

    // Fill the vector
    for(int j=0;j<numframes;j++) {

      fecusbType16 tframe[VFATPACKETLENGTH] ; // 12

      for (int k=0;k<VFATPACKETLENGTH;k++){ tframe[k]=0x0000 ; }

      int datastart = VFATPACKETLENGTH * j ;

      for(int k=datastart, l=11;k<datastart+VFATPACKETLENGTH;k++,l--){

	tframe[l] = vfat_data[k] & 0xffff ;
      }

      Totem::VFATFrame frame((unsigned short*)tframe);
 
      if(tframe[0]!=0){ data.push_back(frame) ; }    
    }
  }
  
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << " ---> buildFramesFromBuffer(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif
  
}

void vfat_controller::readCrepeDataBuffers( int device, std::vector<Totem::VFATFrame> &data, std::vector<std::string> &dataFiles, bool checkFrames, int nFifo ){


  // Data buffer readout

  fecusbType32 data_1[1024];
  fecusbType32 data_2[1024];
  fecusbType32 data_3[1024];
  fecusbType32 data_4[1024];

  std::vector<Totem::VFATFrame> frames_1 ;
  std::vector<Totem::VFATFrame> frames_2 ;
  std::vector<Totem::VFATFrame> frames_3 ;
  std::vector<Totem::VFATFrame> frames_4 ;

  int words_read_1 = 0 ; 
  int words_read_2 = 0 ;
  int words_read_3 = 0 ; 
  int words_read_4 = 0 ; 

  //fecusbType32 ev_data[256];
  //fecusbType32 bc_data[256];

  //int ev_words_read = 0 ; 
  //int bc_words_read = 0 ;

  bool readAll = true ;
  bool force_read = false ;
  int words_each_time = 1024 ;

  bool wrongNumberOfEvents = false ;

  int ftStatus = FECUSB_RETURN_OK ;

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "FIFO to read=" << nFifo << SET_DEFAULT_COLORS << std::endl ;
#endif

  if (nFifo==1 || nFifo==2 || nFifo==3 || nFifo==4 ) { readAll = false ; }

  // Read the status of the fifos

  fecusbType32 readvalue[CRCOUNT]; 
  for(int i=0;i<CRCOUNT-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(device,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }



  // Reading out the data buffers

  if(readAll || nFifo==1) {

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Reading 1st data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

    if ((readvalue[4] & 0x00000001) && !force_read ) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "1st fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ; 
#endif
    }
    else { 

      if (readvalue[4] & 0x00000002) {
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "1st fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
      }
      else {

	fecusbType32 words_to_read = 0 ; 
	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1-1,&words_to_read,1) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "1st fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
#endif

	if(!miniFrames_ && !simpleTrigger_ && words_to_read!=80*VFATPACKETLENGTH){
	  std::cout << SET_FORECOLOR_RED << "Wrong number of frames " << words_to_read << "/" << 80*VFATPACKETLENGTH << SET_DEFAULT_COLORS << std::endl ;
	  wrongNumberOfEvents = true ;
	}	

	if (force_read){ words_to_read = 1024 ; } 

	int burst_num = words_to_read  / words_each_time  ;
	int rest = words_to_read % words_each_time  ;

	for(int j=0;j<burst_num;j++) { 

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1,data_1+j*words_each_time,words_each_time) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_1 += words_each_time ; }		  
	}	  
	if (rest) {

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO1,data_1+burst_num*words_each_time,rest) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_1 += rest ; }
	}

	buildFramesFromBuffer( frames_1, (unsigned *)data_1, words_read_1 ) ;
      } 
    }
  }

  if(readAll || nFifo==2) {

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Reading 2nd data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

    if ((readvalue[4] & 0x00000004) && !force_read ) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "2nd fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else { 

      if (readvalue[4] & 0x00000008) {
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "2nd fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
      }
      else {

	fecusbType32 words_to_read = 0 ; 
	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO2-1,&words_to_read,1) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "2nd fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
#endif

	if(!miniFrames_ && !simpleTrigger_ && words_to_read!=80*VFATPACKETLENGTH){
	  std::cout << SET_FORECOLOR_RED << "Wrong number of frames " << words_to_read << "/" << 80*VFATPACKETLENGTH << SET_DEFAULT_COLORS << std::endl ;
	  wrongNumberOfEvents = true ;
	}

	if (force_read){ words_to_read = 1024 ; } 

	int burst_num = words_to_read  / words_each_time  ;
	int rest = words_to_read % words_each_time  ;

	for(int j=0;j<burst_num;j++) { 

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO2,data_2+j*words_each_time,words_each_time) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_2 += words_each_time ; }		  
	}	  
	if (rest) {

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO2,data_2+burst_num*words_each_time,rest) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_2 += rest ; }
	}

	buildFramesFromBuffer( frames_2, (unsigned *)data_2, words_read_2 ) ;
      } 
    }
  }

  if(readAll || nFifo==3) {

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Reading 3rd data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

    if ((readvalue[4] & 0x00000010) && !force_read ) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "3rd fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else { 

      if (readvalue[4] & 0x00000020) {
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "3rd fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
      }
      else {

	fecusbType32 words_to_read = 0 ; 
	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO3-1,&words_to_read,1) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "3rd fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
#endif

	if(!miniFrames_ && !simpleTrigger_ && words_to_read!=80*VFATPACKETLENGTH){
	  std::cout << SET_FORECOLOR_RED << "Wrong number of frames " << words_to_read << "/" << 80*VFATPACKETLENGTH << SET_DEFAULT_COLORS << std::endl ;
	  wrongNumberOfEvents = true ;
	}

	if (force_read){ words_to_read = 1024 ; } 

	int burst_num = words_to_read  / words_each_time  ;
	int rest = words_to_read % words_each_time  ;

	for(int j=0;j<burst_num;j++) { 

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO3,data_3+j*words_each_time,words_each_time) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_3 += words_each_time ; }		  
	}	  
	if (rest) {

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO3,data_3+burst_num*words_each_time,rest) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_3 += rest ; }
	}

	buildFramesFromBuffer( frames_3, (unsigned *)data_3, words_read_3 ) ;
      } 
    }
  }

  if(readAll || nFifo==4) {

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Reading 4th data fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

    if ((readvalue[4] & 0x00000040) && !force_read ) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "4th fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else { 

      if (readvalue[4] & 0x00000080) {
#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "4th fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
      }
      else {

	fecusbType32 words_to_read = 0 ; 
	ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO4-1,&words_to_read,1) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "4th fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
#endif

	if(!miniFrames_ && !simpleTrigger_ && words_to_read!=80*VFATPACKETLENGTH){
	  std::cout << SET_FORECOLOR_RED << "Wrong number of frames " << words_to_read << "/" << 80*VFATPACKETLENGTH << SET_DEFAULT_COLORS << std::endl ;
	  wrongNumberOfEvents = true ;
	}

	if (force_read){ words_to_read = 1024 ; } 

	int burst_num = words_to_read  / words_each_time  ;
	int rest = words_to_read % words_each_time  ;

	for(int j=0;j<burst_num;j++) { 

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO4,data_4+j*words_each_time,words_each_time) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_4 += words_each_time ; }		  
	}	  
	if (rest) {

	  ftStatus = fec_usb_rw_block_dev(device,6,CREPE_FIFO4,data_4+burst_num*words_each_time,rest) ;

	  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	  else { words_read_4 += rest ; }
	}

	buildFramesFromBuffer( frames_4, (unsigned *)data_4, words_read_4 ) ;
      } 
    }
  }

  // Save the data files directly here if filenames are specified 
  if(!dataFiles.empty()){

    for(int i=0;i<testVfats_.size();i++){

      unsigned int position = testVfats_.at(i)->getI2CAddress() ;

      // Select data FIFO corresponding the position
      if(detectorType_=="RP"){

	if(position==0x10){ writeDataFile(frames_1, dataFiles.at(i), true) ; }
	else if(position==0x20){ writeDataFile(frames_2, dataFiles.at(i), true) ; }
	else if(position==0x30){ writeDataFile(frames_3, dataFiles.at(i), true) ; }
	else if(position==0x40){ writeDataFile(frames_4, dataFiles.at(i), true) ; }
	else{ std::cout << SET_FORECOLOR_RED << "VFAT position " << std::hex << position << std::dec << " not expected, no frames written..." << SET_DEFAULT_COLORS << std::endl ; }
      }
      else{

	if(position==0x00){ writeDataFile(frames_1, dataFiles.at(i), true) ; }
	else if(position==0x10){ writeDataFile(frames_2, dataFiles.at(i), true) ; }
	else if(position==0x20){ writeDataFile(frames_3, dataFiles.at(i), true) ; }
	else if(position==0x30){ writeDataFile(frames_4, dataFiles.at(i), true) ; }
	else{ std::cout << SET_FORECOLOR_RED << "VFAT position " << std::hex << position << std::dec << " not expected, no frames written..." << SET_DEFAULT_COLORS << std::endl ; }
      }
    }

    //if (!frames_1.empty() && dataFiles.size()>=1){ writeDataFile(frames_1, dataFiles.at(0), true) ; }
    //if (!frames_2.empty() && dataFiles.size()>=2){ writeDataFile(frames_2, dataFiles.at(1), true) ; }
    //if (!frames_3.empty() && dataFiles.size()>=3){ writeDataFile(frames_3, dataFiles.at(2), true) ; }
    //if (!frames_4.empty() && dataFiles.size()>=4){ writeDataFile(frames_4, dataFiles.at(3), true) ; }
  }
  else{ // No filenames -> Pack the frames into one vector

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "datafiles empty... merge data to one data vector" << SET_DEFAULT_COLORS << std::endl ;
#endif

    int frames_read = 0 ;

    if (!frames_1.empty() && frames_read < frames_1.size()) { frames_read = frames_1.size() ; }
    if (!frames_2.empty() && frames_read < frames_2.size()) { frames_read = frames_2.size() ; }
    if (!frames_3.empty() && frames_read < frames_3.size()) { frames_read = frames_3.size() ; }
    if (!frames_4.empty() && frames_read < frames_4.size()) { frames_read = frames_4.size() ; }

    if(checkFrames && ((frames_read!=frames_1.size() && !frames_1.empty()) || (frames_read!=frames_2.size() && !frames_2.empty()) ||
		       (frames_read!=frames_3.size() && !frames_3.empty()) || (frames_read!=frames_4.size() && !frames_4.empty()))){

      std::cout << SET_FORECOLOR_RED << "NUMBER OF WORDS READ NOT THE SAME IN ALL FIFOS "
		<< frames_1.size() << "/" << frames_2.size() << "/" << frames_3.size() << "/" << frames_4.size() << SET_DEFAULT_COLORS << std::endl ;

      std::cout << SET_FORECOLOR_BLUE << "Trying to order the messed up VFAT frames, max=" << frames_read << SET_DEFAULT_COLORS << std::endl ;

      int missed1, missed2, missed3, missed4 = 0 ;

      // Merge all the data frames into one vector
      for(int i=0;i<frames_read;i++){

	if (i-missed1<frames_1.size() && i==Totem::VFATFrame(frames_1.at(i-missed1)).getEC()){ data.push_back(frames_1.at(i-missed1)) ; }
	else{ missed1++ ; }

	if (i-missed2<frames_2.size() && i==Totem::VFATFrame(frames_2.at(i-missed2)).getEC()){ data.push_back(frames_2.at(i-missed2)) ; }
	else{ missed2++ ; }

	if (i-missed3<frames_3.size() && i==Totem::VFATFrame(frames_3.at(i-missed3)).getEC()){ data.push_back(frames_3.at(i-missed3)) ; }
	else{ missed3++ ; }

	if (i-missed4<frames_4.size() && i==Totem::VFATFrame(frames_4.at(i-missed4)).getEC()){ data.push_back(frames_4.at(i-missed4)) ; }
	else{ missed4++ ; }

	if (i%79==0){ missed1=0 ; missed2=0 ; missed3=0 ; missed4=0 ; }  
      }
    }
    else{

      // Merge all the data frames into one vector
      for(int i=0;i<frames_read;i++){

	if (i<frames_1.size()){ data.push_back(frames_1.at(i)) ; }
	if (i<frames_2.size()){ data.push_back(frames_2.at(i)) ; }
	if (i<frames_3.size()){ data.push_back(frames_3.at(i)) ; }
	if (i<frames_4.size()){ data.push_back(frames_4.at(i)) ; }    
      }
    }
  }

  /*
  // Check the frames
  if(checkFrames){

    // Read BC and EC

    std::cout << SET_FORECOLOR_CYAN << "Reading EV fifo..." << SET_DEFAULT_COLORS << std::endl ;

    fecusbType32 words_to_read = 0 ; 
    ev_words_read = 0 ;

    //ftStatus = fec_usb_rw_block_dev(0,8,0x40e,&words_to_read,1) ;
    ftStatus = fec_usb_rw_block_dev(0,8,CREPE_BASE_ADDR+0xe,&words_to_read,1) ;

    if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

    std::cout << SET_FORECOLOR_BLUE << "EV fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;

    if (force_read){ words_to_read = 256 ; } 

    int burst_num = words_to_read  / words_each_time  ;
    int rest = words_to_read % words_each_time  ;

    for(int j=0;j<burst_num;j++) { 

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_EVFIFO,ev_data+j*words_each_time,words_each_time) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      else { ev_words_read += words_each_time ; }		  
    }	  
    if (rest) {

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_EVFIFO,ev_data+burst_num*words_each_time,rest) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      else { ev_words_read += rest ; }
    }

    std::cout << SET_FORECOLOR_CYAN << "Reading BC fifo..." << SET_DEFAULT_COLORS << std::endl ;

    words_to_read = 0 ; 
    bc_words_read = 0 ;

    ftStatus = fec_usb_rw_block_dev(0,8,CREPE_BASE_ADDR+0xf,&words_to_read,1) ;

    if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

    std::cout << SET_FORECOLOR_CYAN << "BC fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;

    if (force_read){ words_to_read = 256 ; } 

    burst_num = words_to_read  / words_each_time  ;
    rest = words_to_read % words_each_time  ;

    for(int j=0;j<burst_num;j++) { 

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_BCFIFO,bc_data+j*words_each_time,words_each_time) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      else { bc_words_read += words_each_time ; }		  
    }	  
    if (rest) {

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_BCFIFO,bc_data+burst_num*words_each_time,rest) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
      else { bc_words_read += rest ; }
    }
  }
  */


#ifdef ERRORLOGS

  if(wrongNumberOfEvents){

    std::ofstream outputfilestream ;
    std::ostringstream oss, oss2 ;

    //std::cout << SET_FORECOLOR_CYAN << "Data read from fifos" << SET_DEFAULT_COLORS << std::endl ; 

    int words_to_print = words_read_1 ;

    if (words_read_1 < words_read_2) { words_to_print = words_read_2 ; }
    if (words_to_print  < words_read_3) { words_to_print = words_read_3 ; }
    if (words_to_print  < words_read_4) { words_to_print = words_read_4 ; }



    if (words_to_print!=words_read_1 || words_to_print!=words_read_2 || words_to_print!=words_read_4 || words_to_print!=words_read_4){
	
      std::cout << SET_FORECOLOR_RED << "NUMBER OF WORDS READ NOT THE SAME IN ALL FIFOS" << SET_DEFAULT_COLORS << std::endl ;
    }

    if(words_to_print!=0){

      std::cout << SET_FORECOLOR_RED << "Error occured, see the error logs for details..." << SET_DEFAULT_COLORS << std::endl ;


      oss << std::endl ;
      oss << "ERROR on " << getTimeInfo() << std::endl ;
      oss2 << oss.str() ;

      oss << "####\t1st fifo\t2nd fifo\t3rd fifo\t4th fifo" << std::endl ;
      oss << "----\t--------\t--------\t--------\t--------" << std::endl ;

      for(int j=0;j<words_to_print;j++) {

	//if(j==0 || j==1 || j==78 || j==79 || j==80){
	//printf("%05d 0x%08X 0x%08X 0x%08X 0x%08X \n",j,data_1[j], data_2[j],data_3[j], data_4[j]  ) ;
	//}

	oss << std::setw(4) << std::setfill('0') << std::hex << j << "\t"
	    << std::setw(8) << std::setfill('0') << std::hex << data_1[j] << "\t" 
	    << std::setw(8) << std::setfill('0') << std::hex << data_2[j] << "\t" 
	    << std::setw(8) << std::setfill('0') << std::hex << data_3[j] << "\t" 
	    << std::setw(8) << std::setfill('0') << std::hex << data_4[j] << std::endl ;
      }

      oss << std::dec << std::endl ;
    }


    // Append to the ERROR file
    if (!outputfilestream.is_open()){ outputfilestream.open(ERRORDUMP_FIFOS, std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;  
    }



    words_to_print = 0 ;

    if (!frames_1.empty() && words_to_print < frames_1.size()) { words_to_print = frames_1.size() ; }
    if (!frames_2.empty() && words_to_print < frames_2.size()) { words_to_print = frames_2.size() ; }
    if (!frames_3.empty() && words_to_print < frames_3.size()) { words_to_print = frames_3.size() ; }
    if (!frames_4.empty() && words_to_print < frames_4.size()) { words_to_print = frames_4.size() ; }

    if(words_to_print!=0){

      Totem::VFATFrame frame ;

      oss2 << "####\tBC\tEN\tFLAGS\tFOOT\tCRC\tID" << std::endl ;
      oss2 << "----\t--\t--\t-----\t----\t---\t--" << std::endl ;

      for(unsigned int i=0;i<words_to_print;i++) {

	for(int j=0;j<4;j++){

	  if(j==0 && frames_1.size()>i){ frame = frames_1.at(i) ; }
	  else if(j==1 && frames_2.size()>i){ frame = frames_2.at(i) ; }
	  else if(j==2 && frames_3.size()>i){ frame = frames_3.at(i) ; }
	  else if(j==3 && frames_4.size()>i){ frame = frames_4.at(i) ; }
	  else{ break ; }

	  oss2 << std::hex << std::setw(4) << std::setfill('0') << i << "\t"
	       << std::setw(4) << std::setfill('0') << frame.getBC() << "\t"
	       << std::setw(4) << std::setfill('0') << frame.getEC() << "\t" 
	       << std::setw(4) << std::setfill('0') << frame.getFlags() << "\t" ;

	  if(frame.checkFootprint()){ oss2 << "OK\t" ; }
	  else{ oss2 << "FAIL\t" ; }

	  if(frame.checkCRC()){ oss2 << "OK\t" ; }
	  else{ oss2 << "FAIL\t" ; }

	  oss2 << std::setw(4) << std::setfill('0') << frame.getChipID() << std::endl ;
	}

	oss2 << std::dec << std::endl ;

      }

      // Append to the ERROR file
      if (!outputfilestream.is_open()){ outputfilestream.open(ERRORDUMP_FRAMES, std::ios::app) ; }
 
      if(outputfilestream.is_open()){

	outputfilestream << oss2.str() ;
	outputfilestream.close() ;  
      }

    }
  }

#endif

  /*
  printf("Event fifo:\n") ;
  for(int j=0;j<ev_words_read;j++) {
    printf("0x%08X \n",ev_data[j] ) ;
  }
  printf("\n") ;
	
  printf("BC fifo:\n") ;
  for(int j=0;j<bc_words_read;j++) {
    printf("0x%08X \n",bc_data[j] ) ;
  }
  printf("\n") ;
  */

#ifdef DEBUGMODE

  std::cout << SET_FORECOLOR_CYAN << "frame_1 size: " << frames_1.size() << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "frame_2 size: " << frames_2.size() << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "frame_3 size: " << frames_3.size() << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "frame_4 size: " << frames_4.size() << SET_DEFAULT_COLORS << std::endl ;
  std::cout << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << " ---> readCrepeDataBuffers(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
  std::cout << std::endl ;

#endif
  

}

void vfat_controller::saveSbitInfo( int &total_sbits, unsigned int sectors_1[], unsigned int sectors_2[], unsigned int sectors_3[], unsigned int sectors_4[], int ch){
  
  std::ofstream outputfilestream ;

  int ftStatus = FECUSB_RETURN_OK ;
  fecusbType32 s_data[256] = {0} ;
  fecusbType32 sbc_data[256] = {0};

  fecusbType32 words_to_read = 0 ;
  int s_words_read = 0 ;
  int sbc_words_read = 0 ;

  bool force_read = false ;
  int words_each_time = 1024 ;


  // Read the status of the fifos
  
  fecusbType32 readvalue[CRCOUNT]; 
  for(int i=0;i<CRCOUNT-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(0,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Content of local registers:" << SET_DEFAULT_COLORS << std::endl ;
  for(int j=0;j<CRCOUNT;j++) { printf("0x%04x 0x%08x \n",j+CREPE_BASE_ADDR,readvalue[j]) ; }
  printf("\n") ;
#endif

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }


  if ((readvalue[4] & 0x00001000) && !force_read ) {
#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "S-Bit fifo EMPTY, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
  }
  else {

    if ((readvalue[4] & 0x00002000) && !force_read) {
#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "S-Bit fifo FULL, not reading it..." << SET_DEFAULT_COLORS << std::endl ;
#endif
    }
    else {

      // Read the S-Bits

#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "Reading S-bit fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_BASE_ADDR+0x10,&words_to_read,1) ; // 0,8...

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

      if(sbitDebug_){

	std::cout << SET_FORECOLOR_CYAN << "S-bit fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
      }


      // Bug in FIFO (word count): Shows 0 words if full
      if (force_read || words_to_read==0){ words_to_read = 256 ; } 

      int burst_num = words_to_read  / words_each_time  ;
      int rest = words_to_read % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(0,6,CREPE_SFIFO,s_data+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	else { s_words_read += words_each_time ; }		  
      }	  
      if(rest){

	ftStatus = fec_usb_rw_block_dev(0,6,CREPE_SFIFO,s_data+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	else { s_words_read += rest ; }
      }

      // Read the S-Bit related BC

#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << "Reading S-bit BC fifo..." << SET_DEFAULT_COLORS << std::endl ;
#endif

      ftStatus = fec_usb_rw_block_dev(0,6,CREPE_BASE_ADDR+0x11,&words_to_read,1) ;

      if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

      if(sbitDebug_){

	std::cout << SET_FORECOLOR_CYAN << "S-bit BC fifo contains " << words_to_read << " words" << SET_DEFAULT_COLORS << std::endl ;
      }

     // Bug in FIFO (word count): Shows 0 words if full 
     if (force_read || words_to_read==0){ words_to_read = 256 ; } 

      burst_num = words_to_read  / words_each_time  ;
      rest = words_to_read % words_each_time  ;

      for(int j=0;j<burst_num;j++) { 

	ftStatus = fec_usb_rw_block_dev(0,6,CREPE_SBCFIFO,sbc_data+j*words_each_time,words_each_time) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	else { sbc_words_read += words_each_time ; }		  
      }	  
      if(rest){

	ftStatus = fec_usb_rw_block_dev(0,6,CREPE_SBCFIFO,sbc_data+burst_num*words_each_time,rest) ;

	if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }
	else { sbc_words_read += rest ; }
      }
    }
  }


#ifdef DEBUGMODE

  // Read the pattern back to check
  for(int i=0;i<CRCOUNT-1;i++){ readvalue[i] = 0x0000 ; }

  ftStatus = fec_usb_rw_block_dev(0,8,CREPE_BASE_ADDR,readvalue,CRCOUNT) ;

  if (ftStatus!=FECUSB_RETURN_OK){ std::cout << SET_FORECOLOR_RED << CREPE_READ_ERROR << ", status " << ftStatus << SET_DEFAULT_COLORS << std::endl ; }

  std::cout << SET_FORECOLOR_CYAN << "Content of local registers:" << SET_DEFAULT_COLORS << std::endl ;
  for(int j=0;j<CRCOUNT;j++) { printf("0x%04x 0x%08x \n",j+CREPE_BASE_ADDR,readvalue[j]) ; }
  printf("\n") ;

#endif


#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << " BC |  S-bit info" << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "--------------------------------" << SET_DEFAULT_COLORS << std::endl ;

  for(int j=0;j<s_words_read;j++) { printf("0x%08X 0x%08X\n",sbc_data[j], s_data[j] ) ; }

  std::cout << std::endl ;
#endif

  if(s_words_read!=sbc_words_read){ std::cout << SET_FORECOLOR_RED << "Number of entries in the S-bit fifo in the related BC fifo different (" << s_words_read << " / " << sbc_words_read << SET_DEFAULT_COLORS << std::endl ; }


  total_sbits+=s_words_read ;

  // Go through all sbit entries
  for(int j=0;j<s_words_read;j++){


    // RP
    if(detectorType_=="RP"){

      // S2
      if(ch==1 || ch==8){

	// 1st VFAT
	if((s_data[j] & 0x00000002) == 0x00000002){ sectors_1[2]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000020) == 0x00000020){ sectors_2[2]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00000200) == 0x00000200){ sectors_3[2]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x00002000) == 0x00002000){ sectors_4[2]++ ; }
      }

      // S3
      else if(ch==2 || ch==3){

	// 1st VFAT
	if((s_data[j] & 0x00000004) == 0x00000004){ sectors_1[3]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000040) == 0x00000040){ sectors_2[3]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00000400) == 0x00000400){ sectors_3[3]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x00004000) == 0x00004000){ sectors_4[3]++ ; }
      }

      // S1, S3
      else if(ch==4){

	// 1st VFAT
	if((s_data[j] & 0x00000001) == 0x00000001){ sectors_1[1]++ ; }
	if((s_data[j] & 0x00000004) == 0x00000004){ sectors_1[3]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000010) == 0x00000010){ sectors_2[1]++ ; }
	if((s_data[j] & 0x00000040) == 0x00000040){ sectors_2[3]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00000100) == 0x00000100){ sectors_3[1]++ ; }
	if((s_data[j] & 0x00000400) == 0x00000400){ sectors_3[3]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x00001000) == 0x00001000){ sectors_4[1]++ ; }
	if((s_data[j] & 0x00004000) == 0x00004000){ sectors_4[3]++ ; }
      }

      // S1, S4
      else if(ch==5 || ch==6){

	// 1st VFAT
	if((s_data[j] & 0x00000001) == 0x00000001){ sectors_1[1]++ ; }
	if((s_data[j] & 0x00000008) == 0x00000008){ sectors_1[4]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000010) == 0x00000010){ sectors_2[1]++ ; }
	if((s_data[j] & 0x00000080) == 0x00000080){ sectors_2[4]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00000100) == 0x00000100){ sectors_3[1]++ ; }
	if((s_data[j] & 0x00000800) == 0x00000800){ sectors_3[4]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x00001000) == 0x00001000){ sectors_4[1]++ ; }
	if((s_data[j] & 0x00008000) == 0x00008000){ sectors_4[4]++ ; }
      }

      // S2, S4
      else if(ch==7){

	// 1st VFAT
	if((s_data[j] & 0x00000002) == 0x00000002){ sectors_1[2]++ ; }
	if((s_data[j] & 0x00000008) == 0x00000008){ sectors_1[4]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000020) == 0x00000020){ sectors_2[2]++ ; }
	if((s_data[j] & 0x00000080) == 0x00000080){ sectors_2[4]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00000200) == 0x00000200){ sectors_3[2]++ ; }
	if((s_data[j] & 0x00000800) == 0x00000800){ sectors_3[4]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x00002000) == 0x00002000){ sectors_4[2]++ ; }
	if((s_data[j] & 0x00008000) == 0x00008000){ sectors_4[4]++ ; }
      }

    }

    // GEM
    else{

      // S2, S5, S8
      if(ch==1){

	// 1st VFAT
	if((s_data[j] & 0x00000002) == 0x00000002){ sectors_1[2]++ ; }
	if((s_data[j] & 0x00000010) == 0x00000010){ sectors_1[5]++ ; }
	if((s_data[j] & 0x00000080) == 0x00000080){ sectors_1[8]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000200) == 0x00000200){ sectors_2[2]++ ; }
	if((s_data[j] & 0x00001000) == 0x00001000){ sectors_2[5]++ ; }
	if((s_data[j] & 0x00008000) == 0x00008000){ sectors_2[8]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00020000) == 0x00020000){ sectors_3[2]++ ; }
	if((s_data[j] & 0x00100000) == 0x00100000){ sectors_3[5]++ ; }
	if((s_data[j] & 0x00800000) == 0x00800000){ sectors_3[8]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x02000000) == 0x02000000){ sectors_4[2]++ ; }
	if((s_data[j] & 0x10000000) == 0x10000000){ sectors_4[5]++ ; }
	if((s_data[j] & 0x80000000) == 0x80000000){ sectors_4[8]++ ; }
      }

      // S3, S5, S8
      else if(ch==2){

	// 1st VFAT
	if((s_data[j] & 0x00000004) == 0x00000004){ sectors_1[3]++ ; }
	if((s_data[j] & 0x00000010) == 0x00000010){ sectors_1[5]++ ; }
	if((s_data[j] & 0x00000080) == 0x00000080){ sectors_1[8]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000400) == 0x00000400){ sectors_2[3]++ ; }
	if((s_data[j] & 0x00001000) == 0x00001000){ sectors_2[5]++ ; }
	if((s_data[j] & 0x00008000) == 0x00008000){ sectors_2[8]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00040000) == 0x00040000){ sectors_3[3]++ ; }
	if((s_data[j] & 0x00100000) == 0x00100000){ sectors_3[5]++ ; }
	if((s_data[j] & 0x00800000) == 0x00800000){ sectors_3[8]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x04000000) == 0x04000000){ sectors_4[3]++ ; }
	if((s_data[j] & 0x10000000) == 0x10000000){ sectors_4[5]++ ; }
	if((s_data[j] & 0x80000000) == 0x80000000){ sectors_4[8]++ ; }

      }
      // S3, S6, S8
      else if(ch==3){

	// 1st VFAT
	if((s_data[j] & 0x00000004) == 0x00000004){ sectors_1[3]++ ; }
	if((s_data[j] & 0x00000020) == 0x00000020){ sectors_1[6]++ ; }
	if((s_data[j] & 0x00000080) == 0x00000080){ sectors_1[8]++ ; }

	// 2nd VFAT
	if((s_data[j] & 0x00000400) == 0x00000400){ sectors_2[3]++ ; }
	if((s_data[j] & 0x00002000) == 0x00002000){ sectors_2[6]++ ; }
	if((s_data[j] & 0x00008000) == 0x00008000){ sectors_2[8]++ ; }

	// 3rd VFAT
	if((s_data[j] & 0x00040000) == 0x00040000){ sectors_3[3]++ ; }
	if((s_data[j] & 0x00200000) == 0x00200000){ sectors_3[6]++ ; }
	if((s_data[j] & 0x00800000) == 0x00800000){ sectors_3[8]++ ; }

	// 4th VFAT
	if((s_data[j] & 0x04000000) == 0x04000000){ sectors_4[3]++ ; }
	if((s_data[j] & 0x20000000) == 0x20000000){ sectors_4[6]++ ; }
	if((s_data[j] & 0x80000000) == 0x80000000){ sectors_4[8]++ ; }

      }

      // S1, S3, S6
      else if(ch==4){

	// 1st VFAT
	if((s_data[j] & 0x00000001) == 0x00000001){ sectors_1[1]++ ; }
	if((s_data[j] & 0x00000004) == 0x00000004){ sectors_1[3]++ ; }
	if((s_data[j] & 0x00000020) == 0x00000020){ sectors_1[6]++ ; }


	// 2nd VFAT
	if((s_data[j] & 0x00000100) == 0x00000100){ sectors_2[1]++ ; }
	if((s_data[j] & 0x00000400) == 0x00000400){ sectors_2[3]++ ; }
	if((s_data[j] & 0x00002000) == 0x00002000){ sectors_2[6]++ ; }


	// 3rd VFAT
	if((s_data[j] & 0x00010000) == 0x00010000){ sectors_3[1]++ ; }
	if((s_data[j] & 0x00040000) == 0x00040000){ sectors_3[3]++ ; }
	if((s_data[j] & 0x00200000) == 0x00200000){ sectors_3[6]++ ; }


	// 4th VFAT
	if((s_data[j] & 0x01000000) == 0x01000000){ sectors_4[1]++ ; }
	if((s_data[j] & 0x04000000) == 0x04000000){ sectors_4[3]++ ; }
	if((s_data[j] & 0x20000000) == 0x20000000){ sectors_4[6]++ ; }
      }

      // S1, S4, S6
      else if(ch==5){

	// 1st VFAT
	if((s_data[j] & 0x00000001) == 0x00000001){ sectors_1[1]++ ; }
	if((s_data[j] & 0x00000008) == 0x00000008){ sectors_1[4]++ ; }
	if((s_data[j] & 0x00000020) == 0x00000020){ sectors_1[6]++ ; }


	// 2nd VFAT
	if((s_data[j] & 0x00000100) == 0x00000100){ sectors_2[1]++ ; }
	if((s_data[j] & 0x00000800) == 0x00000800){ sectors_2[4]++ ; }
	if((s_data[j] & 0x00002000) == 0x00002000){ sectors_2[6]++ ; }


	// 3rd VFAT
	if((s_data[j] & 0x00010000) == 0x00010000){ sectors_3[1]++ ; }
	if((s_data[j] & 0x00080000) == 0x00080000){ sectors_3[4]++ ; }
	if((s_data[j] & 0x00200000) == 0x00200000){ sectors_3[6]++ ; }


	// 4th VFAT
	if((s_data[j] & 0x01000000) == 0x01000000){ sectors_4[1]++ ; }
	if((s_data[j] & 0x08000000) == 0x08000000){ sectors_4[4]++ ; }
	if((s_data[j] & 0x20000000) == 0x20000000){ sectors_4[6]++ ; }
      }

      // S1, S4, S7
      else if(ch==6){

	// 1st VFAT
	if((s_data[j] & 0x00000001) == 0x00000001){ sectors_1[1]++ ; }
	if((s_data[j] & 0x00000008) == 0x00000008){ sectors_1[4]++ ; }
	if((s_data[j] & 0x00000040) == 0x00000040){ sectors_1[7]++ ; }


	// 2nd VFAT
	if((s_data[j] & 0x00000100) == 0x00000100){ sectors_2[1]++ ; }
	if((s_data[j] & 0x00000800) == 0x00000800){ sectors_2[4]++ ; }
	if((s_data[j] & 0x00004000) == 0x00004000){ sectors_2[7]++ ; }


	// 3rd VFAT
	if((s_data[j] & 0x00010000) == 0x00010000){ sectors_3[1]++ ; }
	if((s_data[j] & 0x00080000) == 0x00080000){ sectors_3[4]++ ; }
	if((s_data[j] & 0x00400000) == 0x00400000){ sectors_3[7]++ ; }


	// 4th VFAT
	if((s_data[j] & 0x01000000) == 0x01000000){ sectors_4[1]++ ; }
	if((s_data[j] & 0x08000000) == 0x08000000){ sectors_4[4]++ ; }
	if((s_data[j] & 0x40000000) == 0x40000000){ sectors_4[7]++ ; }
      }

      // S2, S4, S7
      else if(ch==7){

	// 1st VFAT
	if((s_data[j] & 0x00000002) == 0x00000002){ sectors_1[2]++ ; }
	if((s_data[j] & 0x00000008) == 0x00000008){ sectors_1[4]++ ; }
	if((s_data[j] & 0x00000040) == 0x00000040){ sectors_1[7]++ ; }


	// 2nd VFAT
	if((s_data[j] & 0x00000200) == 0x00000200){ sectors_2[2]++ ; }
	if((s_data[j] & 0x00000800) == 0x00000800){ sectors_2[4]++ ; }
	if((s_data[j] & 0x00004000) == 0x00004000){ sectors_2[7]++ ; }


	// 3rd VFAT
	if((s_data[j] & 0x00020000) == 0x00020000){ sectors_3[2]++ ; }
	if((s_data[j] & 0x00080000) == 0x00080000){ sectors_3[4]++ ; }
	if((s_data[j] & 0x00400000) == 0x00400000){ sectors_3[7]++ ; }


	// 4th VFAT
	if((s_data[j] & 0x02000000) == 0x02000000){ sectors_4[2]++ ; }
	if((s_data[j] & 0x08000000) == 0x08000000){ sectors_4[4]++ ; }
	if((s_data[j] & 0x40000000) == 0x40000000){ sectors_4[7]++ ; }
      }

      // S2, S5, S7
      else if(ch==8){

	// 1st VFAT
	if((s_data[j] & 0x00000002) == 0x00000002){ sectors_1[2]++ ; }
	if((s_data[j] & 0x00000010) == 0x00000010){ sectors_1[5]++ ; }
	if((s_data[j] & 0x00000040) == 0x00000040){ sectors_1[7]++ ; }


	// 2nd VFAT
	if((s_data[j] & 0x00000200) == 0x00000200){ sectors_2[2]++ ; }
	if((s_data[j] & 0x00001000) == 0x00001000){ sectors_2[5]++ ; }
	if((s_data[j] & 0x00004000) == 0x00004000){ sectors_2[7]++ ; }


	// 3rd VFAT
	if((s_data[j] & 0x00020000) == 0x00020000){ sectors_3[2]++ ; }
	if((s_data[j] & 0x00100000) == 0x00100000){ sectors_3[5]++ ; }
	if((s_data[j] & 0x00400000) == 0x00400000){ sectors_3[7]++ ; }


	// 4th VFAT
	if((s_data[j] & 0x02000000) == 0x02000000){ sectors_4[2]++ ; }
	if((s_data[j] & 0x10000000) == 0x10000000){ sectors_4[5]++ ; }
	if((s_data[j] & 0x40000000) == 0x40000000){ sectors_4[7]++ ; }
      }
    }
  }

  if(sbitDebug_){

    std::cout << SET_FORECOLOR_CYAN << " VFAT | S1 - S8 "<< SET_DEFAULT_COLORS << std::endl  ;
    std::cout << SET_FORECOLOR_CYAN << "--------------------------------" << SET_DEFAULT_COLORS << std::endl ;


    std::cout << SET_FORECOLOR_CYAN << "VFAT #0: " ;
    for(int j=1;j<=8;j++){ std::cout << SET_FORECOLOR_CYAN << sectors_1[j] << " " ; }
    std::cout << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_CYAN << "VFAT #1: " ;
    for(int j=1;j<=8;j++){ std::cout << SET_FORECOLOR_CYAN << sectors_2[j] << " " ; }
    std::cout << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_CYAN << "VFAT #2: " ;
    for(int j=1;j<=8;j++){ std::cout << SET_FORECOLOR_CYAN << sectors_3[j] << " " ; }
    std::cout << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_CYAN << "VFAT #3: " ;
    for(int j=1;j<=8;j++){ std::cout << sectors_4[j] << " " ; }
    std::cout << SET_DEFAULT_COLORS << std::endl ;

    std::cout << SET_FORECOLOR_CYAN << "SBIT ENTRIES READ: " << s_words_read << "\tTOTAL (SO FAR): " << total_sbits << SET_DEFAULT_COLORS << std::endl << std::endl ;


    // Append the S-Bit info into a file

    std::string fn_sbit_info = vfat_controller_home_ + CHIPTEST_DATA + TTP_SBIT_INFO ;

    std::cout << SET_FORECOLOR_CYAN << "Writing (append) s-bit debug info to " << fn_sbit_info.c_str() << SET_DEFAULT_COLORS << std::endl << std::endl ;

    if (!outputfilestream.is_open()){ outputfilestream.open(fn_sbit_info.c_str(), std::ios::app) ; }

    if(outputfilestream.is_open()){

      std::ostringstream oss ;

      for(int j=0;j<s_words_read;j++) {

	oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " 
	    << std::setw(8) << std::setfill('0') << std::hex << s_data[j] << std::endl ;
      } 

      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }
  }

 
    /*  Saving the S-Bits to file... no more done
    for(int i=0;i<files_sbits.size();i++){

      if (!outputfilestream.is_open()){ outputfilestream.open((files_sbits.at(i)).c_str(), std::ios::app) ; }

      if(outputfilestream.is_open()){

	std::ostringstream oss ;

	fecusbType32 sbits = 0 ;

	for(int j=0;j<s_words_read;j++) {

	  oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " 
	      << std::setw(8) << std::setfill('0') << std::hex << s_data[j] << std::endl ;

	//if(i==0){ sbits = s_data[j] & 0x03 ; }
	  //if(i==1){ sbits = s_data[j] & 0x03 ; }
	  //if(i==2){ sbits = s_data[j] & 0x03 ; }
	  //if(i==3){ sbits = s_data[j] & 0x03 ; }

	  //if(i==0){ oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " << std::setw(8) << sbits << std::endl ; }

	  
	  //if(i==0){ oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " << std::setw(2) << (s_data[j] & 0x03) << std::endl ; }
	  //if(i==1){ oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " << std::setw(2) << (s_data[j] & 0x0c) << std::endl ; }
	  //if(i==2){ oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " << std::setw(2) << (s_data[j] & 0x30) << std::endl ; }
	  //if(i==3){ oss << std::setw(8) << std::setfill('0') << std::hex << sbc_data[j] << " " << std::setw(2) << (s_data[j] & 0xc0) << std::endl ; }
	  
	} 

	outputfilestream << oss.str() ;
	outputfilestream.close() ;
      }
    }
    */
    
}

// Method to write VFAT frames into file. NOTE: 
void vfat_controller::writeDataFile( std::vector<Totem::VFATFrame> &data, std::string fileName, bool append ){

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << " ---> writeDataFile(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
  //printData(data) ;
#endif

  unsigned int ec1 = 0 ;
  unsigned int ec2 = 0 ;

  // Strict check, EC needs to increase by 1 or remain the same
  if(!miniFrames_ && !dataRun_){

    // Check the EC progress
    for(int i=0;i<data.size();i++){

      ec1 = Totem::VFATFrame(data.at(i)).getEC() ;

      //std::cout << SET_FORECOLOR_CYAN << "(" << ec1 << " " << ec2 << ") " ;

      if(ec2!=ec1 && ec2!=(ec1-1) && ec1!=0){

	std::cout << std::endl ;
	std::cout << SET_FORECOLOR_RED << "Wrong EC progress, frame #" << i << ", no data will be written..." << SET_DEFAULT_COLORS << std::endl ;
	std::cout << SET_FORECOLOR_RED << "EC read " << ec1 << "\tprevious " << ec2 << SET_DEFAULT_COLORS << std::endl ;
	return ;
      }

      ec2 = ec1 ;
    }
  }
  else{  // Loose checking, EC needs just to increase


    // Check the EC progress
    for(int i=0;i<data.size();i++){

      ec1 = Totem::VFATFrame(data.at(i)).getEC() ;

      //std::cout << SET_FORECOLOR_CYAN << "(" << ec1 << " " << ec2 << ") " ;

      if(ec2>ec1 && ec1!=0){

	printData(data) ;

	std::cout << std::endl ;
	std::cout << SET_FORECOLOR_RED << "Wrong EC progress, frame #" << i << ", aborting..." << SET_DEFAULT_COLORS << std::endl ;
	std::cout << SET_FORECOLOR_RED << ec1 << " is not greater than " << ec2 << SET_DEFAULT_COLORS << std::endl ;
	return ;
      }

      ec2 = ec1 ;
    }
  }
  
  
  //std::cout << SET_FORECOLOR_CYAN << "DOING THE WRITING NOW..." << SET_DEFAULT_COLORS << std::endl ;

  std::ofstream outputfilestream ;

  if (!outputfilestream.is_open()){

    if(!append){

      outputfilestream.open(fileName.c_str(), std::ios::binary ) ;
      outputfilestream << CREPE_HEADER ;
    }
    else{

      outputfilestream.open(fileName.c_str(), std::ios::binary | std::ios::app) ;
    }
  }

  if(outputfilestream.is_open()){

    Totem::VFATFrame frame ; 
    unsigned int i = 0 ;
    unsigned int ec = 0 ;

    /*  
    for(int s=0;s<data.size();s++){

      std::cout << SET_FORECOLOR_CYAN <<"s=" << s << " EC=" << Totem::VFATFrame(data.at(s)).getEC() << SET_DEFAULT_COLORS << std::endl ;

      if ((s==3 || s+1%4==0) && s>571){ sleep(2) ; std::cout << std::endl ; }
     }
    */  

    while(i<data.size()){

      unsigned short nFrames = 0 ;
      unsigned int j = i ;

      /*
	if(i>260){ sleep(3) ; }

	std::cout << SET_FORECOLOR_CYAN <<"i=" << i << " EC from data " << "[ "
	<< Totem::VFATFrame(data.at(i)).getEC() << " " ;
	std::cout << Totem::VFATFrame(data.at(i+1)).getEC() << " " ;
	std::cout << Totem::VFATFrame(data.at(i+2)).getEC() << " " ;
	std::cout << Totem::VFATFrame(data.at(i+3)).getEC() <<  " ]" << SET_DEFAULT_COLORS << std::endl ;
      */      


      while(j<i+4){

	if(j<data.size()){

	  if(ec==Totem::VFATFrame(data.at(j)).getEC()){ nFrames++ ; }
	}

	j++ ;
      }
            
      if(nFrames!=0){

	outputfilestream.write((char *)(&nFrames), sizeof(nFrames)) ; 
      
	for(unsigned int k=0;k<nFrames;k++){
       
	  frame = data.at(i+k) ;
	  outputfilestream.write((char *)(&frame), sizeof(frame)) ;
	}
      }
      

      if(nFrames!=0){

	i = i + nFrames ;
	ec++ ;
      }
      else{

	if(!miniFrames_){ ec=0 ; }
	else{ ec++ ; }

	//i++ ; // added Feb 21st
      }

#ifdef DEBUGMODE
      if(i%1000==0){ std::cout << SET_FORECOLOR_CYAN << " Frames saved, i=" << i << " ec=" << ec << " nFrames=" << nFrames << SET_DEFAULT_COLORS << std::endl ; }
#endif
    }	

    /*
    // Old simple saving...
    for(unsigned int i=0;i<data.size();i++){

      frame = data.at(i) ;
      outputfilestream.write((char *)(&frame), sizeof(frame)) ;
    }
    */
    //std::cout << std::endl ;

    if(data.size()%80!=0 && !data.empty() && !miniFrames_){ std::cout << SET_FORECOLOR_RED << data.size() << " frames saved (Not 80 frames / VFAT / step)" << SET_DEFAULT_COLORS << std::endl ; }

    outputfilestream.close() ;
  }
}

void vfat_controller::readDataFile( std::vector<Totem::VFATFrame> &data, std::string fileName ){

  Totem::VFATFrame frame ;
  std::ifstream inputfilestream ;

  if (!inputfilestream.is_open()){ inputfilestream.open(fileName.c_str(), std::ios::binary); }

  if(inputfilestream.is_open()){

    while (!inputfilestream.eof()){

      inputfilestream.read((char *)(&frame), sizeof(Totem::VFATFrame));
      data.push_back(frame) ;
    }

    inputfilestream.close() ;
  }

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << " ---> readDataFile(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

}

void vfat_controller::printData( std::vector<Totem::VFATFrame> &data ){

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << " ---> printData(), data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

  Totem::VFATFrame frame ;

  std::cout << SET_FORECOLOR_CYAN << "      VFAT data packets read            " << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "----------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "  BC     EN   FLAGS  FOOT  CRC     ID   " << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_CYAN << "----------------------------------------" << SET_DEFAULT_COLORS << std::endl ;

  for(unsigned int i=0;i<data.size();i++) {

    frame = data.at(i) ;

    printf("0x%04X ", frame.getBC()) ;
    printf("0x%04X ", frame.getEC()) ;
    printf("0x%04X ", frame.getFlags()) ;

    if(frame.checkFootprint()){ printf(" OK   ") ; }
    else { printf("FAIL  ") ; }

    if(frame.checkCRC()){ printf(" OK   ") ; }
    else { printf("FAIL  ") ; }

    printf("0x%04X ", frame.getChipID()) ;
    printf("\n") ;
  }

  printf("\n") ;
}

void vfat_controller::updateLog( ){

  std::ofstream outputfilestream ;

  if (!outputfilestream.is_open()){ outputfilestream.open(CHIPTEST_VFATLIST, std::ios::app) ; }

  if(outputfilestream.is_open()){

    for(unsigned int i=0;i<testVfatIds_.size();i++){ outputfilestream << testVfatIds_.at(i) << "\t" ; }

    if(testVfatIds_.size()<4){

      for(int i=0;i<(4-testVfatIds_.size());i++){ outputfilestream << "      \t" ; }
    }

    outputfilestream << getTimeInfo() ;
    outputfilestream.close() ;
  }
}

std::string vfat_controller::createDirectory( std::string hybridid, int version, int position, std::string chipid ){

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Create dir using following arguments: " << hybridid << " " << version << " " << position << " " << chipid << SET_DEFAULT_COLORS << std::endl ;
#endif

  QString vfat_dir = vfat_controller_home_ + CHIPTEST_DATA + hybridid ;

  const char* env = getenv("TTP_DATA") ;

  if(env!=0){ vfat_dir = (std::string)env + "/" + hybridid ; }
  else{

    if(advancedChipTesting->isChecked()){ std::cout << SET_FORECOLOR_BLUE << "$TTP_DATA NOT DEFINED, USE DEFAULT " << vfat_dir.latin1() << SET_DEFAULT_COLORS << std::endl ; }
    else{

      std::cout << SET_FORECOLOR_RED << "$TTP_DATA NOT DEFINED, ABORTING" << SET_DEFAULT_COLORS << std::endl ;
      abort_ = true ;
      return "" ;
    }
  }

  QDir vfat_ver_dir(vfat_dir) ;
  std::stringstream ss_path ;

  std::ofstream outputfilestream ;
  std::stringstream timestampfile ;

  // Get the time stamp ( the same used for data folder and timestap files )
  time_t rawtime ;
  struct tm * timeinfo ;
  char timestr [80] ;

  time(&rawtime) ;
  timeinfo = localtime ( &rawtime ) ;
  strftime (timestr,80,"%y%m%d%H%M",timeinfo) ;

  timestampfile << "timestamp." << timestr ;

  // Create the main directory for this VFAT if it doesn't exist
  // <hybrid type>.<last 4 digits from barcode>
  if(!vfat_ver_dir.exists()){

    if(!vfat_ver_dir.mkdir(vfat_dir)){

      std::cout << SET_FORECOLOR_RED << "Creation of " << vfat_dir.latin1() << " NOT OK" << SET_DEFAULT_COLORS << std::endl ;
      abort_ = true ;
      return "" ;
    }
  }
  else{

    // Remove timestampfile(s)
    QDir toberemoved = vfat_ver_dir ;
    toberemoved.setNameFilter("timestamp.*") ;
    QStringList strList = toberemoved.entryList() ;

    for( int i=0;i<strList.size();i++) { vfat_ver_dir.remove(strList[i], false) ; }
  }


  // Write timestampfile
  std::stringstream timestampdir ;
  timestampdir << vfat_ver_dir.absPath() << "/" << timestampfile.str() ;
  if (!outputfilestream.is_open()){ outputfilestream.open(timestampdir.str().c_str()) ; }

  if(outputfilestream.is_open()){

    outputfilestream << timestr ;
    outputfilestream.close() ;
  }

  // 1st sub directory <version>
  // 0->without detector, 1->with detector
  if(vfat_ver_dir.exists()){

    ss_path << vfat_ver_dir.absPath() << "/" << version << "/" ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Create the following version directory:" << std::endl << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Current structure before adding (" << vfat_ver_dir.count() <<" items):" << SET_DEFAULT_COLORS << std::endl ;
    for ( int i=0; i<vfat_ver_dir.count(); i++ ){ std::cout << SET_FORECOLOR_CYAN << vfat_ver_dir[i] << SET_DEFAULT_COLORS << std::endl ; }
#endif

    vfat_ver_dir = QDir(ss_path.str()) ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "DIR NOW: " << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(!vfat_ver_dir.exists()){

      // Create a new directory for the current run
      if(!vfat_ver_dir.mkdir(ss_path.str())){

	std::cout << SET_FORECOLOR_RED << "Creation of " << ss_path.str() << " NOT OK" << SET_DEFAULT_COLORS << std::endl ;
	abort_ = true ;
	return "" ;
      }
    }
  }


  // 2nd sub directory <position>
  // for RP 0-4
  if(vfat_ver_dir.exists()){

    ss_path << position << "/" ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Create the following version directory:" << std::endl << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Current structure before adding (" << vfat_ver_dir.count() <<" items):" << SET_DEFAULT_COLORS << std::endl ;
    for ( int i=0; i<vfat_ver_dir.count(); i++ ){ std::cout << SET_FORECOLOR_CYAN << vfat_ver_dir[i] << SET_DEFAULT_COLORS << std::endl ; }
#endif

    vfat_ver_dir = QDir(ss_path.str()) ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "DIR NOW: " << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(!vfat_ver_dir.exists()){

      // Create a new directory for the current run
      if(!vfat_ver_dir.mkdir(ss_path.str())){

	std::cout << SET_FORECOLOR_RED << "Creation of " << ss_path.str() << " NOT OK" << SET_DEFAULT_COLORS << std::endl ;
	abort_ = true ;
	return "" ;
      }
    }
  }


  // 3rd sub directory <chipid>
  if(vfat_ver_dir.exists()){

    ss_path << chipid << "/" ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Create the following version directory:" << std::endl << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Current structure before adding (" << vfat_ver_dir.count() <<" items):" << SET_DEFAULT_COLORS << std::endl ;
    for ( int i=0; i<vfat_ver_dir.count(); i++ ){ std::cout << SET_FORECOLOR_CYAN << vfat_ver_dir[i] << SET_DEFAULT_COLORS << std::endl ; }
#endif

    vfat_ver_dir = QDir(ss_path.str()) ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "DIR NOW: " << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(!vfat_ver_dir.exists()){

      // Create a new directory for the current run
      if(!vfat_ver_dir.mkdir(ss_path.str())){

	std::cout << SET_FORECOLOR_RED << "Creation of " << ss_path.str() << " NOT OK" << SET_DEFAULT_COLORS << std::endl ;
	abort_ = true ;
	return "" ;
      }
    }

    // Remove timestampfile(s)
    QDir toberemoved = vfat_ver_dir ;
    toberemoved.setNameFilter("timestamp.*") ;
    QStringList strList = toberemoved.entryList() ;

    for( int i=0;i<strList.size();i++) { vfat_ver_dir.remove(strList[i], false) ; }

    // Write timestampfile
    std::stringstream timestampdir ;
    timestampdir << ss_path.str() << timestampfile.str() ;
    if (!outputfilestream.is_open()){ outputfilestream.open(timestampdir.str().c_str()) ; }

    if(outputfilestream.is_open()){

      outputfilestream << timestr ;
      outputfilestream.close() ;
    }
  }


  // 4th sub directory <YYMMDDHHMM>
  if(vfat_ver_dir.exists()){

    ss_path << timestr << "/" ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Create the following version directory:" << std::endl << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Current structure before adding (" << vfat_ver_dir.count() <<" items):" << SET_DEFAULT_COLORS << std::endl ;
    for ( int i=0; i<vfat_ver_dir.count(); i++ ){ std::cout << SET_FORECOLOR_CYAN << vfat_ver_dir[i] << SET_DEFAULT_COLORS << std::endl ; }
#endif

    vfat_ver_dir = QDir(ss_path.str()) ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "DIR NOW: " << ss_path.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(!vfat_ver_dir.exists()){

      // Create a new directory for the current run
      if(!vfat_ver_dir.mkdir(ss_path.str())){

	std::cout << SET_FORECOLOR_RED << "Creation of " << ss_path.str() << " NOT OK" << SET_DEFAULT_COLORS << std::endl ;
	abort_ = true ;
	return "" ;
      }
    }

    return ss_path.str() ;
  }


  return "" ;
}

void vfat_controller::createDirectories(){

  chipTestDirs_.clear() ;

  QStringList list = QStringList::split("\n", barcodeList->text()) ;
  int version = 0 ;
  std::string prefix = "" ;

  bool chipsFound[4]={false} ;

  // 0 -> NO detector, 1 -> with detector 
  if(detectorPresent_){ version = 1 ; }

  if(detectorType_=="RP"){

    prefix = "RP." ;
    for( int i=0;i<testVfatIds_.size();i++) { chipTestBarcodes_.push_back(list[0].latin1()) ; }
  }
  else{

    if(T1TypeSel->isChecked()){

      prefix = "T1." ;
      if(StripTypeSel->isChecked()){ prefix += "pos." ; }
      else if(PadTypeSel->isChecked()){ prefix += "neg." ; }
      else if(TriggerTypeSel->isChecked()){ prefix += "tr." ; }

    }
    else if(T2TypeSel->isChecked()){ 

      prefix = "T2." ;
      if(StripTypeSel->isChecked()){ prefix += "strip." ; }
      else if(PadTypeSel->isChecked()){ prefix += "pad." ; }
      else if(TriggerTypeSel->isChecked()){ prefix += "tr." ; }
    }
    else if(RPTypeSel->isChecked()){ 

      prefix = "RP.tr." ;
    }

    //for( int i=0;i<list.size();i++) { chipTestBarcodes_.push_back(list[i].latin1()) ; }
  }

  // For all VFATs
  for( int i=0;i<testVfatIds_.size();i++) {

    std::string hybridid = prefix ;
    int position = 1 ;
    bool fail = false ;

    if(detectorType_=="RP"){

      // Hybrid id is the same for all the VFATs on the RP hybrid
      hybridid.append(list[0].right(4).latin1()) ;

      unsigned int addr = testVfats_.at(i)->getI2CAddress() ;

      // Get position corresponding to the address, mark chips found
      if(addr==0x10){ position = 1 ; chipsFound[0]=true ; }
      else if(addr==0x20){ position = 2 ; chipsFound[1]=true ; }
      else if(addr==0x30){ position = 3 ; chipsFound[2]=true ; }
      else if(addr==0x40){ position = 4 ; chipsFound[3]=true ; }
      else{ fail=true ; std::cout << SET_FORECOLOR_RED << "Incorrect address " << std::hex << addr << std::dec << ", index " << i << SET_DEFAULT_COLORS << std::endl ; }
    }
    else{

      unsigned int addr = testVfats_.at(i)->getI2CAddress() ;

      // Append the hybridid, mark chips found
      if(addr==0x00){

	if(list.size()>0){ hybridid.append(list[0].right(4).latin1()) ; chipTestBarcodes_.push_back(list[0].latin1()) ; }
	chipsFound[0]=true ;
      }
      else if(addr==0x10){

	if(list.size()>1){hybridid.append(list[1].right(4).latin1()) ; chipTestBarcodes_.push_back(list[1].latin1()) ; }
	chipsFound[1]=true ;
      }
      else if(addr==0x20){

	if(list.size()>2){hybridid.append(list[2].right(4).latin1()) ; chipTestBarcodes_.push_back(list[2].latin1()) ; }
	chipsFound[2]=true ;
      }
      else if(addr==0x30){

	if(list.size()>3){hybridid.append(list[3].right(4).latin1()) ; chipTestBarcodes_.push_back(list[3].latin1()) ; }
	chipsFound[3]=true ;
      }
      else{

	fail=true ;
	std::cout << SET_FORECOLOR_RED << "Incorrect address " << std::hex << addr << std::dec << ", index " << i << SET_DEFAULT_COLORS << std::endl ;
      }
    }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "HYBRID ID: " << hybridid << SET_DEFAULT_COLORS << std::endl ;
#endif

    if(!fail){ chipTestDirs_.push_back(createDirectory(hybridid, version, position, testVfatIds_.at(i).substr(2))) ; }
  }
 
  // Create empty dirs if needed
  if((detectorType_=="RP" && testVfats_.size()!=4) || (detectorType_!="RP" && testVfats_.size() != list.size())){

    int nDirs = list.size() ;

    if(detectorType_=="RP"){ nDirs = 4 ; }

    std::cout << SET_FORECOLOR_RED << "Not correct number of VFATs (found " << testVfats_.size() << ", expected " << nDirs << ")" <<SET_DEFAULT_COLORS << std::endl ;

    // Create empty dirs - add what's missing...
    for(int i=0;i<nDirs;i++){

      if(!chipsFound[i]){

	std::cout << SET_FORECOLOR_BLUE << "Create dir for missing VFAT #" << i+1 << SET_DEFAULT_COLORS << std::endl ;

	int position = 1 ;
	if(detectorType_=="RP"){ position = i+1 ; }

	std::string hybridid = prefix ;

	// Hybrid id is the same for all the VFATs on the RP hybrid
	if(detectorType_=="RP"){ hybridid.append(list[0].right(4).latin1()); }
	else{ hybridid.append(list[i].right(4).latin1()) ; }

	std::string dummySummary = createDirectory(hybridid, version, position, NOVFAT) ;
	chipTestI2cErrorDirs_.push_back(dummySummary) ;
	dummySummary += TTP_SUMMARY ;

	std::ofstream outputfilestream ;

	// Make the dummy summary file
	if (!outputfilestream.is_open()){ outputfilestream.open(dummySummary.c_str()) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << I2C_ERROR ;
	  outputfilestream.close() ;  
	}
      }
      /*
      if(testVfats_.size() > i){ addr = testVfats_.at(i)->getI2CAddress() ; }
      else{ std::cout << SET_FORECOLOR_RED << "Not correct number of VFATs (found " << testVfats_.size() << ", expected " << nDirs << ")" <<SET_DEFAULT_COLORS << std::endl ; }

      if(detectorType_=="RP"){ 

	// Map address against position 
	if(i==0 && addr==0x10){ position = 1 ; }
	else if(i==1 && addr==0x20){ position = 2 ; }
	else if(i==2 && addr==0x30){ position = 3 ; }
	else if(i==3 && addr==0x40){ position = 4 ; }
	else{ position = i+1 ; fail = true ; }
      }
      else{

	// Check if address corresponds to the index
	if(i==0 && addr!=0x00){ fail = true ; }
	else if(i==1 && addr!=0x10){ fail = true ; }
	else if(i==2 && addr!=0x20){ fail = true ; }
	else if(i==3 && addr!=0x30){ fail = true ; }
      }
      
      std::string id = "" ;
      if(!fail && testVfatIds_.size() > i){ id = testVfatIds_.at(i).substr(2) ; }
      else{ id = NOVFAT ; std::cout << SET_FORECOLOR_CYAN << "No id found for VFAT #" << i << ", use " << NOVFAT << SET_DEFAULT_COLORS << std::endl ; }

      std::string dir = createDirectory(hybridid, version, position, id) ;

      if(!fail){ chipTestDirs_.push_back(dir) ; }
      else{ std::cout << SET_FORECOLOR_CYAN << "VFAT address 0x" << std::hex << addr << std::dec << ", index " << i << " -> PROBLEM - Create empty dir only..." << SET_DEFAULT_COLORS << std::endl ; }  
      */

    }  
  }
  
 
  if(!abort_){ 

    std::cout << SET_FORECOLOR_BLUE << "-------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_BLUE << "Results will be stored to:" << SET_DEFAULT_COLORS << std::endl ;
    for(int i=0;i<chipTestDirs_.size();i++){ std::cout << SET_FORECOLOR_BLUE << chipTestDirs_.at(i) << SET_DEFAULT_COLORS << std::endl ; }
    std::cout << SET_FORECOLOR_BLUE << "-------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
  }
}

void vfat_controller::readConfigFile(){

  // try to load file
  std::string fileName = vfat_controller_home_ + CHIPTEST_PARAMFILE ;
  std::ifstream file;
  file.open(fileName.c_str());

  if(file.is_open()){

    while (!file.eof() && !file.fail()) {

      // Read line
      int pos = file.tellg() ;
      std::string line = "" ;

      getline(file, line) ;

      // Skip comments and empty lines
      if(line.size()>0 && line[0] != '#'){

	file.clear(file.goodbit) ;

	int pos2 = file.tellg() ;
	std:: string tag = "" ;
	uint value = 0 ;
       
	// Go back and try to read tag
	file.seekg(pos) ;
	file >> tag ;

	if (!file.fail()) {

	  // Read and set values
	  if (!tag.compare("VCal1")){ file >> value ; std::cout << SET_FORECOLOR_BLUE << "VCal1: " << value << SET_DEFAULT_COLORS << std::endl ; }
	  else if (!tag.compare("VCal2")){ file >> value ; std::cout << SET_FORECOLOR_BLUE << "VCal2: " << value << SET_DEFAULT_COLORS << std::endl ; }
	  else if (!tag.compare("VCalStep")){ file >> value ; std::cout << SET_FORECOLOR_BLUE << "VCalStep: " << value << SET_DEFAULT_COLORS << std::endl ; }
	  else if (!tag.compare("ErrorsAllowed")){ file >> value ; std::cout << SET_FORECOLOR_BLUE << "ErrorsAllowed: " << value << SET_DEFAULT_COLORS << std::endl ; }

	  else if (!tag.compare("Channels")){

	    do{

	      file >> value ;
	      if(!file.fail()){ std::cout << SET_FORECOLOR_BLUE << "value=" << value << SET_DEFAULT_COLORS << std::endl ; }
	    }
	    while(!file.fail()) ;

	    file.clear(file.goodbit) ;
	  }
	}
	else {

	  if (!file.eof()){ file.clear(file.goodbit) ; }

	  std::cout << SET_FORECOLOR_RED << "Unrecognized line: " <<  line.c_str() << SET_DEFAULT_COLORS << std::endl ;
	  file.seekg(pos2) ;
	}
      }
    }
    file.close() ;
  }
  else{ std::cout << SET_FORECOLOR_RED << "Couldn't open " << fileName.c_str() << " using default parameters..." << SET_DEFAULT_COLORS << std::endl ; }
}

void vfat_controller::runMonitorAnalysis(){

  for(int i=0;i<chipTestDirs_.size();i++){

    std::stringstream ss ;

    ss << vfat_controller_home_ << MONITORCALL_ANALYSIS << " "
       <<(chipTestDirs_.at(i)).c_str() ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ; 
#endif
    system((ss.str()).c_str()) ;
  
  }

  for(int i=0;i<chipTestDirs_.size();i++){

    std::stringstream ss ;
    ss << "cat " << chipTestDirs_.at(i) <<  TTP_SUMMARY ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    system((ss.str()).c_str()) ;
  }
}

void vfat_controller::runTestManager(){

  // TestManager need to be run from top data dir and argument dir needs to start with hybrid id
  for(int i=0;i<chipTestDirs_.size();i++){

    std::stringstream ss ;
    ss << "cd $TTP_DATA;" << TESTMANAGERCALL ;

    QStringList list = QStringList::split("/", chipTestDirs_.at(i)) ;

    if(list.size()>=5){

      for( int i=list.size()-5;i<list.size();i++) { ss << list[i].latin1() << "/" ; }
    }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    system((ss.str()).c_str()) ;
  }


  // Same for the VFATs not responding to I2c
  for(int i=0;i<chipTestI2cErrorDirs_.size();i++){

    std::stringstream ss ;
    ss << "cd $TTP_DATA;" << TESTMANAGERCALL ;

    QStringList list = QStringList::split("/", chipTestI2cErrorDirs_.at(i)) ;

    if(list.size()>=5){

      for( int i=list.size()-5;i<list.size();i++) { ss << list[i].latin1() << "/" ; }
    }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

    system((ss.str()).c_str()) ;
  }
}


/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Chip Testing Methods                                                              */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

void vfat_controller::saveMonitorConfigFiles(){

  std::ofstream outputfilestream ;

  for(int i=0;i<chipTestDirs_.size();i++){

    std::ostringstream oss ;
    std::string fn = chipTestDirs_.at(i) + CHIPTEST_MONITOR_CONFIG ;

    oss << "<top>" << std::endl
	<< "\t<test_vfat id=\"" << testVfatIds_.at(i) << "\" verbose=\"" << MONITOR_VERBOSE << "\" />" << std::endl
	<< "</top>" << std::endl ;

    if (!outputfilestream.is_open()){ outputfilestream.open(fn.c_str()) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }
  }
}


std::string vfat_controller::getTestSettings(){

  std::stringstream ss ;

  vfatDescription vfatd ;
  getDefaultDescription(vfatd, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS ) ;

  short int pulseThisChannel[VFAT_CH_NUM_MAX + 1] = {0} ;

  uint vcal1_hi = CHIPTEST_VCAL1_HI ;
  uint vcal2_hi = CHIPTEST_VCAL2_HI ;
  uint vcalstep_hi = CHIPTEST_VCALSTEP_HI ;
  uint vthreshold1 = CHIPTEST_VTHRESHOLD1 ;
  uint vthreshold2 = CHIPTEST_VTHRESHOLD2 ;

  uint vcal1 = CHIPTEST_VCAL1 ;
  uint vcal2 = CHIPTEST_VCAL2 ;
  uint vcalstep = CHIPTEST_VCALSTEP ;
  uint sbit_pass_limit = CHIPTEST_SBITPASSLIMIT ;

  // Read the parameters
  std::string fileName = vfat_controller_home_ + CHIPTEST_PARAMFILE ;
  std::ifstream file;
  file.open(fileName.c_str());

  if(file.is_open()){

    while (!file.eof() && !file.fail()) {

      // Read line
      int pos = file.tellg() ;
      std::string line = "" ;

      getline(file, line) ;

      // Skip comments and empty lines
      if(line.size()>0 && line[0] != '#'){

	file.clear(file.goodbit) ;

	int pos2 = file.tellg() ;
	std:: string tag = "" ;
	uint value = 0 ;
       
	// Go back and try to read tag
	file.seekg(pos) ;
	file >> tag ;

	if (!file.fail()) {

	  // Read and set values

	  if (!tag.compare("VCal1_hi")){ file >> vcal1_hi ; }
	  else if (!tag.compare("VCal2_hi")){ file >> vcal2_hi ; }
	  else if (!tag.compare("VCalStep_hi")){ file >> vcalstep_hi ; }
	  else if (!tag.compare("VThreshold1")){ file >> vthreshold1 ; }
	  else if (!tag.compare("VThreshold2")){ file >> vthreshold2 ; }

	  else if (!tag.compare("VCal1")){ file >> vcal1 ; }
	  else if (!tag.compare("VCal2")){ file >> vcal2 ; }
	  else if (!tag.compare("VCalStep")){ file >> vcalstep ; }
	  else if (!tag.compare("PassLimit")){ file >> sbit_pass_limit ; }

	  else if (!tag.compare("Channels")){

	    do{

	      file >> value ;
	      if(!file.fail() && (VFAT_CH_NUM_MAX>=value || value>0)){ pulseThisChannel[value] = VFAT_PULSE_FLAG ; }
	    }
	    while(!file.fail()) ;

	    file.clear(file.goodbit) ;
	  }
	}
	else {

	  if (!file.eof()){ file.clear(file.goodbit) ; }

	  std::cout << SET_FORECOLOR_RED << "Unrecognized line: " <<  line.c_str() << SET_DEFAULT_COLORS << std::endl ;
	  file.seekg(pos2) ;
	}
      }
    }

    file.close() ;

    if(VFAT_REG_VALUE_MAX<vcal1_hi || vcal1_hi<0){ vcal1_hi = CHIPTEST_VCAL1_HI ; }
    if(VFAT_REG_VALUE_MAX<vcal2_hi || vcal2_hi<0){ vcal2_hi = CHIPTEST_VCAL2_HI ;  }
    if(vcal1_hi>vcal2_hi){ vcal1_hi = CHIPTEST_VCAL1_HI ; vcal2_hi = CHIPTEST_VCAL2_HI ; }
    if(VFAT_REG_VALUE_MAX<vcalstep_hi || vcalstep_hi<0){ vcalstep_hi = CHIPTEST_VCALSTEP_HI ; }
    if(VFAT_REG_VALUE_MAX<vthreshold1 || vthreshold1<0){ vthreshold1 = CHIPTEST_VTHRESHOLD1 ; }
    if(VFAT_REG_VALUE_MAX<vthreshold2 || vthreshold2<0){ vthreshold2 = CHIPTEST_VTHRESHOLD2 ; }


    if(VFAT_REG_VALUE_MAX<vcal1 || vcal1<0){ vcal1 = CHIPTEST_VCAL1 ;}
    if(VFAT_REG_VALUE_MAX<vcal2 || vcal2<0){ vcal2 = CHIPTEST_VCAL2 ; }
    if(vcal1>vcal2){ vcal1 = CHIPTEST_VCAL1 ; vcal2 = CHIPTEST_VCAL2 ; }
    if(VFAT_REG_VALUE_MAX<vcalstep || vcalstep<0){ vcalstep = CHIPTEST_VCALSTEP ;}
    if(100<sbit_pass_limit || sbit_pass_limit<0){ sbit_pass_limit = CHIPTEST_SBITPASSLIMIT ; }
  }

  else{ std::cout << SET_FORECOLOR_RED << "Couldn't open " << fileName.c_str() << SET_DEFAULT_COLORS << std::endl ; }

  ss << "# begin: settings" << std::endl ;
  ss << "# General settings" << std::endl ;
  ss << "Lat:\t\t" << (unsigned long)vfatd.getLat() << std::endl ;
  ss << "Phase:\t\t" << (unsigned long)vfatd.getCalPhase() << std::endl ;
  ss << "VThreshold1:\t" << (unsigned long)vfatd.getVThreshold1() << std::endl ;
  ss << "VThreshold2:\t" << (unsigned long)vfatd.getVThreshold2() << std::endl << std::endl ;

  ss << "# Setting used for high resolution S-Curves" << std::endl ;
  ss << "VCal1_hi:\t" << vcal1_hi << std::endl ;
  ss << "VCal2_hi:\t" << vcal2_hi << std::endl ;
  ss << "VCalStep_hi:\t" << vcalstep_hi << std::endl ;
  ss << "VThreshold1_hi:\t" << vthreshold1 << std::endl ;
  ss << "VThreshold2_hi:\t" << vthreshold2 << std::endl ;
  ss << "Channels:\t" ;

  for(unsigned int a=1;a<=VFAT_CH_NUM_MAX;a++){

    if(pulseThisChannel[a] == VFAT_PULSE_FLAG){ ss << a << " " ; }
  }

  ss << std::endl << std::endl ;

  ss << "# Setting used for low resolution S-Curves" << std::endl ;
  ss << "VCal1:\t\t" << vcal1 << std::endl ;
  ss << "VCal2:\t\t" << vcal2<< std::endl ;
  ss << "VCalStep:\t" << vcalstep << std::endl ;
  ss << "# end: settings" << std::endl ;

  return (ss.str()) ;

}

void vfat_controller::I2CScan(){

  keyType fecSlot = 0, ringSlot = 0, index = 0, ccuAddress = 0x00, channel = 0x11 ;

  int okcount = 0, failcount = 0 , count = 0 ;

  // Display chip test panel's LED
  ledChipTest->setPixmap(blackLed_) ;
  
  if(testVfats_.size()==1 && fecSlots->count() && ccuAddresses->count()){

    writeCrepeSettings(0) ;

    if (sscanf (fecSlots->text(0), "%d,%d", &fecSlot, &ringSlot)) {

      FecRingDevice *fecRingDevice = NULL ;

      try {

	// Scan the ring for CCUs
	fecRingDevice = fecAccess_->getFecRingDevice ( buildFecRingKey(fecSlot,ringSlot) ) ;
	fecRingDevice->fecScanRingBroadcast ( ) ;

	// Scan the ring for each FECs and display the CCUs
	std::list<keyType> *ccuList = fecAccess_->getCcuList ( buildFecRingKey(fecSlot,ringSlot) ) ;

	if (ccuList != NULL ) {

	  std::list<keyType>::iterator p=ccuList->begin() ;
	  ccuAddress = getCcuKey((keyType)*p) ;
	}
	else { std::cout << SET_FORECOLOR_RED << "No CCU found on FEC" << SET_DEFAULT_COLORS << std::endl ; }  

      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "EXCEPTION WHILE TRYING TO FIND FEC AND CCU" << SET_DEFAULT_COLORS << std::endl ; }

      for(fecusbType32 gemAddress=0x00000000;gemAddress<=0x07000000;gemAddress+=0x01000000){


	// Change address on TTP
	daq_regs_[2] = (daq_regs_[2] & 0x10111111) | gemAddress ;
	writeCrepeSettings(0) ;

	for(keyType address = 0x00;address<=0x70;address+=0x10){

	  index = buildCompleteKey(fecSlot, ringSlot, ccuAddress, channel, address) ;
	  count++ ;

	  tscType8 value = 0x00 ;
	  int loop = 0 ;
	  bool exceptionThrown = false ;
	  do{

	    exceptionThrown = false ;
	    try{

	      loop++ ;
	      value = fecRingDevice->readi2cDevice(index+8) ; // Try to read chipID0
	    }
	    catch (FecExceptionHandler &e) { exceptionThrown = true ; }
	  }
	  while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	  if(exceptionThrown){

	    failcount++ ;
	    std::cout << SET_FORECOLOR_RED << "\tNo answer for " ;
	  }
	  else{

	    if((gemAddress >> 20) == address) { okcount++ ; }
	    std::cout << SET_FORECOLOR_BLUE << "\tAnswer (ID0=" << std::hex << (int)value << "for " ;
	  }

	  std::cout << "FEC (" 
		    << std::hex << (int)getFecKey(index) << ","
		    << std::hex << (int)getRingKey(index) << ") CCU 0x" 
		    << std::hex << (int)getCcuKey(index) << " CHANNEL 0x" 
		    << std::hex << (int)getChannelKey(index) << " ADDRESS 0x" 
		    << std::hex << (int)getAddressKey(index) << std::dec << SET_DEFAULT_COLORS << std::endl ;

	  if(address==0x70){ std::cout << std::endl ; }
	}
      }
    }  
  }
  else{

    std::cout << SET_FORECOLOR_RED << "Problem while starting the full I2C checking. Please, check the following points:" << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_RED << "The FEC and CCU are accessable on TTP" << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_RED << "There's only one GEM hybrid connected" << SET_DEFAULT_COLORS << std::endl ;
    ledChipTest->setPixmap(redLed_) ;
  }

  setCrepeSettings() ;
  writeCrepeSettings(0) ;

  std::cout << SET_FORECOLOR_BLUE << "Addresses correctly answered:" << okcount << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_BLUE << "Addresses not answered:" << failcount << SET_DEFAULT_COLORS << std::endl ;
  std::cout << SET_FORECOLOR_BLUE << "Total address checked:" << count << SET_DEFAULT_COLORS << std::endl ;

  if(okcount==8 && failcount==56 && count==64){ ledChipTest->setPixmap(greenLed_) ; }
  else{ ledChipTest->setPixmap(redLed_) ; }
}


/*
 * I2C functionality test
 */
std::string vfat_controller::testVfatI2C () {

  std::ostringstream results ;
  vfatDescription vfatd  ;

  std::ofstream outputfilestream ;
  //std::string fn = vfat_controller_home_ + VFAT_PATH_POS_MAPPING ;
  std::vector<Totem::VFATFrame> data ;

  std::vector<std::string> fileNames_data ;
  std::vector<std::string> fileNames_config ;
 
  std::vector<std::string> fileNames_summary ;

  // Append the complete absolute path
  //for(int i=0;i<chipTestDirs_.size();i++){ fileNames_data.push_back(chipTestDirs_.at(i) + TTP_DATA_TREASURE) ; }
  //for(int i=0;i<chipTestDirs_.size();i++){ fileNames_config.push_back(chipTestDirs_.at(i) + TTP_CONFIG_TREASURE) ; }
  for(int i=0;i<chipTestDirs_.size();i++){ fileNames_summary.push_back(chipTestDirs_.at(i) + TTP_SUMMARY) ; }

  // Create the files
  // for(int i=0;i<fileNames_data.size();i++){ writeDataFile(data, fileNames_data.at(i), false) ; }

  float ICommon = 0 ;

#ifdef USB_GPIB_PS

  try{

    GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    ICommon = gp.getCurrent() ;

    std::cout << SET_FORECOLOR_BLUE << "TOTAL SLEEP MODE CURRENT:\t" << floor((ICommon - TTP_CONSUMPTION) * 1000 + 0.5 ) << " mA (COMMON FOR " << vfatAddresses_.size() << " CHIPS)" << SET_DEFAULT_COLORS << std::endl ;

    
    /*
    GPIBDevice *gp = new GPIBDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp->getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    ICommon = gp->getCurrent() ;

    std::cout << SET_FORECOLOR_BLUE << "TOTAL SLEEP MODE CURRENT:\t" << floor((ICommon - TTP_CONSUMPTION) * 1000 + 0.5 ) << " mA (COMMON FOR " << vfatAddresses_.size() << " CHIPS)" << SET_DEFAULT_COLORS << std::endl ;
    //gp->release() ;
   
    delete gp ; 

    */

    /*
    GPIBDevice_ = new GPIBDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;
    msleep(100) ;
    //GPIBKey_ = GPIBDevice_->getDeviceKey() ;
    //std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << GPIBKey_ << SET_DEFAULT_COLORS << std::endl ;

    ICommon = GPIBDevice_->getCurrent() ;

    std::cout << SET_FORECOLOR_BLUE << "TOTAL SLEEP MODE CURRENT:\t" << floor((ICommon - TTP_CONSUMPTION) * 1000 + 0.5 ) << " mA (COMMON FOR " << vfatAddresses_.size() << " CHIPS)" << SET_DEFAULT_COLORS << std::endl ;
    GPIBDevice_->release() ;
    */
  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }

#else

  std::string current_str = "" ;

  std::cout << SET_FORECOLOR_BLUE << std::endl << "WRITE DOWN THE SLEEP MODE CURRENT (mA): " << SET_DEFAULT_COLORS << std::endl ;

  std::cin.clear() ;
  std::cin >> current_str ;

  ICommon = QString(current_str).toFloat() / 1000 ;

#endif

  char **argv = qApp->argv() ;

  //std::string TTPSerial(argv[qApp->argc()-1]) ;
  std::string TTPSerial = "TTPXXXXX" ;

  // Get the TTP serial
  for (int a = 0 ; a < qApp->argc() ; a++) {

    if (strcasecmp(argv[a],"-fechardwareid") == 0) {

      if ((a+1) < qApp->argc()) {

	a++ ;
	TTPSerial = std::string(argv[a]);
      }     
    } 
  }


  for(int i=0;i<fileNames_summary.size();i++){

    std::ostringstream oss ;
    char key[100] ;
    decodeKey(key,vfatAddresses_.at(i)) ;
    std::string chipid = "" ;

    oss << "--------------------------------------------------------------------------" << std::endl ;
    oss << "CHIPTEST: SUMMARY FILE" << std::endl ;
    oss << "--------------------------------------------------------------------------" << std::endl ;
    oss << "DATE:\t\t" << getTimeInfo() ;
    oss << "AUTHOR:\t\t" << personNameT->text().latin1() << std::endl ;
    oss << "BAR CODE:\t" << chipTestBarcodes_.at(i) << std::endl ;

    oss << "HYBRID TYPE:\t" ;

    if(detectorType_!="RP"){

      if(T1TypeSel->isChecked()){

	oss << detectorType_ << "\t(T1" ;
	if(StripTypeSel->isChecked()){ oss << "/POS)" ; }
	else if(PadTypeSel->isChecked()){ oss << "/NEG)" ; }
	else if(TriggerTypeSel->isChecked()){ oss << "/TR)" ; }
      }  
      else if(T2TypeSel->isChecked()){

	oss << detectorType_ << "\t(T2" ; 
	if(StripTypeSel->isChecked()){ oss << "/STRIP)" ; }
	else if(PadTypeSel->isChecked()){ oss << "/PAD)" ; }
	else if(TriggerTypeSel->isChecked()){ oss << "/TR)" ; }
      }
      else if(RPTypeSel->isChecked()){

	oss << "RP\t(TR)" ; // RP trigger mezzanine is plugged into GEM slot
      }
    }
    else{

      oss << detectorType_ ;
    }

    oss << std::endl ;

    oss << "CHIP ID:\t" << testVfatIds_.at(i) << std::endl ;

    if(testVfatIds_.at(i)!=CHIPID_NONSENCE){

      unsigned int chipid = HexToInt(testVfatIds_.at(i).substr(2)) ;

      oss << "WAFER NUMBER:\t0x" << std::hex << (chipid & 0x007F) << std::endl ;
      oss << "RETICLE NUMBER:\t0x" << ((chipid & 0xFE00) >> 9)<< std::endl ;
      oss << "CHIP NUMBER:\t0x" << ((chipid & 0x0180) >> 7) << std::dec << std::endl ;
    }

    oss << "PLATFORM:\t" << TTPSerial << std::endl ; 
    oss << "POSITION:\t" << key << std::endl << std::endl ;

    oss << "TEST PROCEDURE VERSION:\t" << CHIPTEST_VERSION << std::endl ;
    oss << "TTP FIRMWARE VERSION:\t" << ttpFirmwareVersion_ << std::endl ;
    oss << "--------------------------------------------------------------------------" << std::endl << std::endl << std::endl ;

    oss << getTestSettings() << std::endl ;

    oss << "# begin: test `I2C'" << std::endl ;
    oss << "SLEEP MODE CURRENT:\t" ;

    //oss << "SLEEP MODE CURRENT:\t" << floor((ICommon - TTP_CONSUMPTION)*1000 / vfatAddresses_.size()/10 + 0.5)*10 << " mA" << std::endl ;
    oss << floor((ICommon - TTP_CONSUMPTION)*1000 + 0.5 )<< " mA (common for " << vfatAddresses_.size() << ")" << std::endl ;


    // Append to the summary file
    if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;  
    }
  }


  vfatAccess_ = NULL ;
  vfatDescription *myvfat ;
  int confNumber = 1 ;

  bool testOK = true ;
  int loop = 0 ;
  bool exceptionThrown = false ;

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "VFAT I2C TEST:" << SET_DEFAULT_COLORS << std::endl ; }
  results <<  "VFAT I2C TEST:" << std::endl ;

  //getDefaultDescription(vfatd, vfat_controller_home_ + CHIPTEST_SETTINGS_MASK) ;

  getDefaultDescription(vfatd, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;
  for(int i=1;i<=VFAT_CH_NUM_MAX;i++){vfatd.setChanReg(i, VFAT_CALPULSE_MASKCH_MASK) ; }


  // 1st stage: write the default values
  for( int i=0;i<testVfats_.size();i++) {

    std::ostringstream oss ;
    std::cout << SET_FORECOLOR_BLUE << "Write defaults for VFAT #" << i+1 << SET_DEFAULT_COLORS << std::endl ;

    vfatAccess_ = testVfats_.at(i) ;
    loop = 0 ;

    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_->setVfatValues (vfatd) ; // Set the chip vfat to vfatd values
	myvfat = vfatAccess_->getVfatValues() ; // Get the chip vfat values
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      results << testI2CError(1, i) ;
      return (results.str()) ;
    }

    float ITmp = 0 ;

#ifdef USB_GPIB_PS

    try{


      GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

      std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

      ITmp = gp.getCurrent() ;

      std::cout << SET_FORECOLOR_BLUE << "VFAT #" << i+1 << " RUN MODE CURRENT:\t" << floor((ITmp - ICommon) * 1000 + 0.5) << " mA" << SET_DEFAULT_COLORS << std::endl ;



      /*
      GPIBDevice_ = new GPIBDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;
      GPIBKey_ = GPIBDevice_->getDeviceKey() ;
      std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << GPIBKey_ << SET_DEFAULT_COLORS << std::endl ;

      ITmp = GPIBDevice_->getCurrent() ;

      std::cout << SET_FORECOLOR_BLUE << "VFAT #" << i << " RUN MODE CURRENT:\t" << floor((ITmp - ICommon) * 1000 + 0.5) << " mA" << SET_DEFAULT_COLORS << std::endl ;

      GPIBDevice_->release() ;
    */

    }
    catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }

#else

    current_str = "" ;

    std::cout << SET_FORECOLOR_BLUE << std::endl << "WRITE DOWN THE RUN MODE CURRENT (mA): " << SET_DEFAULT_COLORS << std::endl ;

    std::cin.clear() ;
    std::cin >> current_str ;

    ITmp = QString(current_str).toFloat() / 1000 ;

#endif

    oss << "RUN MODE CURRENT:\t" << floor((ITmp - ICommon) * 1000 + 0.5) << " mA (diff. from SLEEP)" << std::endl ;
    ICommon = ITmp ;

    oss << "I2C CHECK:\t" ;

    if(*myvfat == vfatd) {

      std::string s = testVfatIds_.at(i) + "\tPASS" ;

      oss << "\tPASS" << std::endl ;
      if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << s << SET_DEFAULT_COLORS << std::endl ;  }
      results << s << std::endl ;

      // Append to the summary file
      if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
      if(outputfilestream.is_open()){

	outputfilestream << oss.str() ;
	outputfilestream.close() ;  
      }
    }
    else {

      std::string s = testVfatIds_.at(i) + "\tFAIL" ;

      oss << "\tFAIL" << std::endl ;
      oss << "FAILED REGS:\t\t" << getDifferences(myvfat, vfatd) << std::endl ;

      if(showConsole_) { std::cout << SET_FORECOLOR_RED << s << SET_DEFAULT_COLORS << std::endl ;  }
      results << s << std::endl ; ;
      ledChipTest->setPixmap(redLed_) ;


      // Append to the summary file
      if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
      if(outputfilestream.is_open()){

	outputfilestream << oss.str() ;
	outputfilestream.close() ;  
      }

      results << testI2CError(1, i) ;
      abortChipTesting_ = true ;
      //return (results.str()) ;
    }
  }
  
  delete myvfat ;

  
  writeCrepeSettings(0) ;
  writeCrepeTrigger(0) ;


  // 2nd stage: treasure hunt
  for( int i=0;i<testVfats_.size();i++) {

    std::ostringstream oss ;

    std::cout << SET_FORECOLOR_BLUE << "Treasure hunt, VFAT #" << i+1 << SET_DEFAULT_COLORS << std::endl ;

    //std::vector<Totem::VFATFrame> data ;
    unsigned int value = vfatd.getControl0() | VFAT_POWERON_MASK ;

    vfatAccess_ = testVfats_.at(i) ;
    loop = 0 ;

    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_->setVfatControl0(value) ; // RUN
      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      results << testI2CError(3, i) ;
      return (results.str()) ;
    }

    resetCrepeFifos(0) ;
    msleep(100) ;
    startCrepeTrigger(0) ;
    msleep(100) ;

    unsigned int position = testVfats_.at(i)->getI2CAddress() ;
    int index = 0 ;

    // Map position against data FIFO
    if(detectorType_=="RP"){

      if(position==0x10){ index = 1 ; }
      else if(position==0x20){ index = 2 ; }
      else if(position==0x30){ index = 3 ; }
      else if(position==0x40){ index = 4 ; }
      else{ std::cout << SET_FORECOLOR_RED << "VFAT position " << std::hex << position << std::dec << " not expected, aborting..." << SET_DEFAULT_COLORS << std::endl ; }
    }
    else{

      if(position==0x00){ index = 1 ; }
      else if(position==0x10){ index = 2 ; }
      else if(position==0x20){ index = 3 ; }
      else if(position==0x30){ index = 4 ; }
      else{ std::cout << SET_FORECOLOR_RED << "VFAT position " << std::hex << position << std::dec << " not expected, aborting..." << SET_DEFAULT_COLORS << std::endl ; }
    }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "POS: " << std::hex << position << "\tindex: " << std::dec << index << "\tsize: " << fileNames_data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

    readCrepeDataBuffers(0, data, fileNames_data, false, index) ; // fileNames_data is empty, i+1

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "Reading done..." << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Debugging... data size: " << data.size() << SET_DEFAULT_COLORS << std::endl ;
#endif


    unsigned int chipid = 0 ;

    oss << "TREASURE HUNT:\t" ;

    if(data.size()<=10){
      
      std::cout << SET_FORECOLOR_RED << "Incorrect number of frames read aborting, data vector size=" << data.size() << SET_DEFAULT_COLORS << std::endl ;

      oss << "\tFAIL" << std::endl ;
      ledChipTest->setPixmap(redLed_) ;
      abortChipTesting_ = true ;
    }
    else{

      chipid = Totem::VFATFrame(data.at(data.size()-10)).getChipID() ;

      //Write data file
      //writeDataFile(data, chipTestDirs_.at(i) + TTP_DATA_TREASURE, false) ;
      data.clear() ;

#ifdef DEBUGMODE
      std::cout << SET_FORECOLOR_CYAN << IntToHex(chipid & 0x00FF) << " " << testVfatIds_.at(i).substr(4,2) << SET_DEFAULT_COLORS << std::endl ;
      std::cout << SET_FORECOLOR_CYAN << IntToHex(chipid & 0x0F00).substr(0,1) << " " << testVfatIds_.at(i).substr(3,1) << SET_DEFAULT_COLORS << std::endl ;

      if(IntToHex(chipid & 0x00FF) != testVfatIds_.at(i).substr(4,2)){ std::cout << SET_FORECOLOR_CYAN << "8 LSB WRONG" << SET_DEFAULT_COLORS << std::endl ; }
      if(IntToHex(chipid & 0x0F00).substr(0,1) != testVfatIds_.at(i).substr(3,1)){ std::cout << SET_FORECOLOR_CYAN << "4 MSB WRONG" << SET_DEFAULT_COLORS << std::endl ; }
#endif

      if(IntToHex(chipid & 0x00FF)!= testVfatIds_.at(i).substr(4,2) ||
	 IntToHex(chipid & 0x0F00).substr(0,1) != testVfatIds_.at(i).substr(3,1)){

	std::cout << SET_FORECOLOR_RED << "CHIP ID MITCMATCH, VFAT #" << i+1<< " " << testVfatIds_.at(i) << " (I2C) / " << std::hex << chipid << " (DATA)" << SET_DEFAULT_COLORS << std::endl << std::dec ;

	oss << "\tFAIL" << std::endl ;

	ledChipTest->setPixmap(redLed_) ;
	abortChipTesting_ = true ;
	results << testI2CError(3, i) ;
	//return (results.str()) ;
      }
      else{

	oss << "\tPASS" << std::endl ;

#ifdef DEBUGMODE
	std::cout << SET_FORECOLOR_CYAN << "CHIP ID CHECK OK, VFAT #" << i+1 << " " << testVfatIds_.at(i) << " (I2C) / " << std::hex << chipid << " (DATA)" << SET_DEFAULT_COLORS << std::endl << std::dec ;
#endif
      }
    }

    oss << "# end: test `I2C'" << std::endl ;

    // Append to the summary file
    if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;  
    }


    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_->setVfatControl0(vfatd.getControl0()) ; // SLEEP

      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      results << testI2CError(1, i) ;
      return (results.str()) ;
    }
  }

  // Write data file
  //writeDataFile(data, TTP_DATA_TREASURE, false) ;


  // 3rd stage: put chips to run mode for following tests
  for( int i=0;i<testVfats_.size();i++) {

    unsigned int value = vfatd.getControl0() | VFAT_POWERON_MASK ;

    vfatAccess_ = testVfats_.at(i) ;
    loop = 0 ;

    do{

      exceptionThrown = false ;
      try{

	loop++ ;
	vfatAccess_->setVfatControl0(value) ; // RUN

      }
      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
    }
    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;

    if(exceptionThrown){

      results << testI2CError(1, i) ;
      return (results.str()) ;
    }
  }

  return (results.str()) ;
}


std::string vfat_controller::testDataPacketCheck () {

  std::ostringstream results ;
  results << "DATAPACKET CHECK:" << std::endl ;

  std::vector<Totem::VFATFrame> data ;
  std::vector<std::string> fileNames_data ;
  std::vector<std::string> fileNames_summary ;

  // Append the complete absolute path
  for(int i=0;i<chipTestDirs_.size();i++){ fileNames_data.push_back(chipTestDirs_.at(i) + TTP_DATA_DATAPACKET) ; }
  for(int i=0;i<chipTestDirs_.size();i++){ fileNames_summary.push_back(chipTestDirs_.at(i) + TTP_SUMMARY) ; }

  // Create the files
  for(int i=0;i<fileNames_data.size();i++){ writeDataFile(data, fileNames_data.at(i), false) ; }

  miniFrames_ = true ;
  simpleTrigger_ = true ;

  setCrepeSettings(1) ;

  writeCrepeSettings(0) ;
  writeFastCrepeTrigger(0) ;

  resetCrepeFifos(0) ;
  startCrepeTrigger(0) ;

  msleep(100) ;

  readCrepeDataBuffers(0, data, fileNames_data, false) ;
  //writeDataFile(data, TTP_DATA_DATAPACKET, false) ;

  miniFrames_ = false ;
  simpleTrigger_ = false ;

  setCrepeSettings() ;
  writeCrepeSettings(0) ;

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "Sizes (data/testDirs/summary): " << fileNames_data.size() << " " << chipTestDirs_.size() << " " << fileNames_summary.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

  for(int i=0;i<fileNames_data.size();i++){

    std::stringstream ss ;

    ss << DATAPACKET_COMMAND << " "
       <<(fileNames_data.at(i)).c_str() << " "
       << vfat_controller_home_ << CHIPTEST_DATAPACKET_REF << " "
       << (testVfatIds_.at(i)).c_str() << " "
       << (fileNames_summary.at(i)).c_str() ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ; 
#endif
   
    int status = system((ss.str()).c_str()) ;

    if(status!=25600/*DATAPACKET_OK*/){

      std::cout << SET_FORECOLOR_RED << "RETURN STATUS: " << std::dec << status/256 << SET_DEFAULT_COLORS << std::endl ;

      abortChipTesting_ = true ;
      results << testI2CError(9, i) ;
      return (results.str()) ;
    }   
  }

  return results.str() ;
}


std::string vfat_controller::maskCheck(){

  std::ostringstream results ;
  std::ofstream outputfilestream ;

  if(!chipTesting_){

    std::cout << SET_FORECOLOR_RED << "Software problem: ChipTesting flag false." << SET_DEFAULT_COLORS << std::endl ;
    return results.str() ;

  }

  //std::string fn_config_mask = vfat_controller_home_ + TTP_CONFIG_MASKCHECK ;
  //std::string fn_data_mask = vfat_controller_home_ + TTP_DATA_MASKCHECK ;
  std::vector<std::string> files_data_mask ;
  std::vector<std::string> files_config_mask ;
  std::vector<Totem::VFATFrame> data_mask ;

  std::vector<std::string> fileNames_summary ;
  for(int i=0;i<chipTestDirs_.size();i++){ fileNames_summary.push_back(chipTestDirs_.at(i) + TTP_SUMMARY) ; }

  int loop = 0 ;
  bool exceptionThrown = false ;


  writeCrepeSettings(0) ;
  writeCrepeTrigger(0) ;

  vfatAccess_ = NULL ;

  // Append the complete absolute path
  for(int i=0;i<chipTestDirs_.size();i++){ files_data_mask.push_back(chipTestDirs_.at(i) + TTP_DATA_MASKCHECK) ; }
  for(int i=0;i<chipTestDirs_.size();i++){ files_config_mask.push_back(chipTestDirs_.at(i) + TTP_CONFIG_MASKCHECK) ; }

#ifdef DEBUGMODE
  for(int i=0;i<files_data_mask.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data_mask.at(i) << SET_DEFAULT_COLORS << std::endl ; }
  for(int i=0;i<files_config_mask.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config_mask.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif

  // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
  for(int i=0;i<files_data_mask.size();i++){ writeDataFile(data_mask, files_data_mask.at(i), false) ; }

  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "MASK CHECK:" << SET_DEFAULT_COLORS << std::endl ; }
  results <<  "MASK CHECK:" << std::endl ;

  deviceVector vDevices_mask ;
  vfatDescription vfatDefaults_mask ;

  //std::string fn_mask = vfat_controller_home_ + CHIPTEST_SETTINGS_MASK ;
  //getDefaultDescription(vfatDefaults_mask, vfat_controller_home_ + CHIPTEST_SETTINGS_MASK) ;

  getDefaultDescription(vfatDefaults_mask, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;
  for(int i=1;i<=VFAT_CH_NUM_MAX;i++){vfatDefaults_mask.setChanReg(i, VFAT_CALPULSE_MASKCH_MASK) ; }

  vDevices_mask.push_back(&vfatDefaults_mask) ;

  // Build the configuration
  for( int j=0;j<testVfats_.size();j++) {

    std::ostringstream oss ;

    oss << "<?xml version=\"1.0\"?>" << std::endl ;
    oss << "<ROWSET>" << std::endl ;

    oss << getXMLTagDefault(vDevices_mask) ;

    oss << "<configuration number=\"" << 1 << "\">"  << std::endl ;

    oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
	<< "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) ;  // first 2 digits

    for(int k=1;k<=VFAT_CH_NUM_MAX;k++){

      oss << "\" chanreg" << (unsigned long)k << "=\"" << VFAT_CALPULSE_MASKCH_MASK ;
    }

    oss << "\" vcal=\"" << CHIPTEST_VCAL2 << "\" />" << std::endl ;

    oss << "</configuration>" << std::endl ;

    oss << "</ROWSET>" << std::endl ;


    if (!outputfilestream.is_open()){ outputfilestream.open((files_config_mask.at(j)).c_str(), std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;
    }    
  }

  resetCrepeFifos(0) ;
  msleep(100) ;
  startCrepeTrigger(0) ;
  readCrepeDataBuffers(0, data_mask, files_data_mask, true) ;


  // Disable ALL channels ( and activate the 1st set ??? )
  for(int k=1;k<=VFAT_CH_NUM_MAX;k++){

    uint value = 0x00 ;

    //if(k==1 || (k-1)%(VFAT_CH_NUM_MAX/parallelCH)==0){ value = VFAT_CALPULSE_MASK ; }


    // For every VFAT
    for( int j=0;j<testVfats_.size();j++) {

      vfatAccess_ = testVfats_.at(j) ;
      loop = 0 ;

      do{

	exceptionThrown = false ;
	try{

	  loop++ ;
	  vfatAccess_->setVfatChanReg(k, (tscType8)value) ;
	}
	catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
      }
      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
      if(exceptionThrown){

	results << testI2CError(5, j) ;
	return (results.str());
      }
    }
  }


  for(int i=0;i<fileNames_summary.size();i++){

    std::stringstream ss ;

    ss << "sh " << vfat_controller_home_ << MONITORCALL_MASK << " " << (chipTestDirs_.at(i)).c_str() ;

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << "RUN: " << ss.str() << SET_DEFAULT_COLORS << std::endl ; 
#endif
  
    int status = system((ss.str()).c_str()) ;
    ss.str("") ;

    if(status!=25600 && status!=0){ //DATAPACKET_OK 25600

      std::cout << SET_FORECOLOR_RED << "RETURN STATUS INCORRECT: " << std::dec << status/256 << SET_DEFAULT_COLORS << std::endl ;
      abortChipTesting_ = true ;
      results << testI2CError(10, i) ;
      return (results.str()) ;
    }    
  }

  return (results.str()) ;
}

/*
 * Pulse Scan for chip testing, checks also S-bits, MS length and pulse polarity
 */
std::string vfat_controller::pulseScan(){
 
  std::ostringstream results ;
  std::ofstream outputfilestream ;


  // Streams to reduce writing the number of writing operations
  std::ostringstream oss ;
  std::ostringstream oss_0 ; 
  std::ostringstream oss_1 ;
  std::ostringstream oss_2 ; 
  std::ostringstream oss_3 ;

  std::vector<std::string> files_data ;
  std::vector<std::string> files_config ;
  std::vector<Totem::VFATFrame> data ;
  //data.reserve(1*4*80*(CHIPTEST_VCAL2-CHIPTEST_VCAL1+1)/CHIPTEST_VCALSTEP) ;

  std::vector<std::string> files_data_hires ;
  std::vector<std::string> files_config_hires ;
  std::vector<Totem::VFATFrame> data_hires ;
  int confNumber_hires = 1 ;
  short int pulseThisChannel[VFAT_CH_NUM_MAX + 1] = {0} ;

  std::ostringstream oss_pol1 ;
  std::vector<std::string> files_data_pol1 ;
  std::vector<std::string> files_config_pol1 ;
  std::vector<Totem::VFATFrame> data_pol1 ;

  std::ostringstream oss_pol2 ;
  std::vector<std::string> files_data_pol2 ;
  std::vector<std::string> files_config_pol2 ;
  std::vector<Totem::VFATFrame> data_pol2 ;

  std::ostringstream oss_ms ;
  std::vector<std::string> files_data_ms ;
  std::vector<std::string> files_config_ms ;
  std::vector<Totem::VFATFrame> data_ms ;
  int confNumber_ms = 1 ;

  std::vector<std::string> files_sbits ;

  // index 0 = total s-bit counts for vfat, index 1-8 = S1-S8 
  unsigned int sectors_1[9]={0} ;
  unsigned int sectors_2[9]={0} ;
  unsigned int sectors_3[9]={0} ;
  unsigned int sectors_4[9]={0} ;
  int total_sbits = 0 ;

  bool testOK = true ;
  int confNumber = 1 ;
  bool completeTag = true ;

  uint vcal1, vcal2, vcal1_hi, vcal2_hi, vthreshold1, vthreshold2, chipid0, chipid1, sbit_pass_limit ;
  uint vcalstep = 1, vcalstep_hi = 1;

  int triggerCount = 0 ;

  int loop = 0 ;
  bool exceptionThrown = false ;


#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << "SIZE RESERVED FOR DATA VECTOR: " << data.capacity() << SET_DEFAULT_COLORS << std::endl ;
#endif

  if(chipTesting_){

    vcal1_hi = CHIPTEST_VCAL1_HI ;
    vcal2_hi = CHIPTEST_VCAL2_HI ;
    vcalstep_hi = CHIPTEST_VCALSTEP_HI ;
    vthreshold1 = CHIPTEST_VTHRESHOLD1 ;
    vthreshold2 = CHIPTEST_VTHRESHOLD2 ;

    vcal1 = CHIPTEST_VCAL1 ;
    vcal2 = CHIPTEST_VCAL2 ;
    vcalstep = CHIPTEST_VCALSTEP ;
    sbit_pass_limit = CHIPTEST_SBITPASSLIMIT ;

    // Read the parameters
    std::string fileName = vfat_controller_home_ + CHIPTEST_PARAMFILE ;
    std::ifstream file;
    file.open(fileName.c_str());

    if(file.is_open()){

      while (!file.eof() && !file.fail()) {

	// Read line
	int pos = file.tellg() ;
	std::string line = "" ;

	getline(file, line) ;

	// Skip comments and empty lines
	if(line.size()>0 && line[0] != '#'){

	  file.clear(file.goodbit) ;

	  int pos2 = file.tellg() ;
	  std:: string tag = "" ;
	  uint value = 0 ;
       
	  // Go back and try to read tag
	  file.seekg(pos) ;
	  file >> tag ;

	  if (!file.fail()) {

	    // Read and set values

	    if (!tag.compare("VCal1_hi")){ file >> vcal1_hi ; }
	    else if (!tag.compare("VCal2_hi")){ file >> vcal2_hi ; }
	    else if (!tag.compare("VCalStep_hi")){ file >> vcalstep_hi ; }
	    else if (!tag.compare("VThreshold1")){ file >> vthreshold1 ; }
	    else if (!tag.compare("VThreshold2")){ file >> vthreshold2 ; }

	    else if (!tag.compare("VCal1")){ file >> vcal1 ; }
	    else if (!tag.compare("VCal2")){ file >> vcal2 ; }
	    else if (!tag.compare("VCalStep")){ file >> vcalstep ; }
	    else if (!tag.compare("PassLimit")){ file >> sbit_pass_limit ; }

	    else if (!tag.compare("Channels")){

	      std::cout << SET_FORECOLOR_BLUE << "Channels selected:" ;

	      do{

		file >> value ;
		if(!file.fail() && (VFAT_CH_NUM_MAX>=value || value>0)){ pulseThisChannel[value] = VFAT_PULSE_FLAG ; std::cout << SET_FORECOLOR_BLUE << " " << value ; }
	      }
	      while(!file.fail()) ;

	      file.clear(file.goodbit) ;
	      std::cout << std::endl ;
	    }
	  }
	  else {

	    if (!file.eof()){ file.clear(file.goodbit) ; }

	    std::cout << SET_FORECOLOR_RED << "Unrecognized line: " <<  line.c_str() << SET_DEFAULT_COLORS << std::endl ;
	    file.seekg(pos2) ;
	  }
	}
      }
      file.close() ;

      if(VFAT_REG_VALUE_MAX<vcal1_hi || vcal1_hi<0){ vcal1_hi = CHIPTEST_VCAL1_HI ; std::cout << SET_FORECOLOR_BLUE << "VCAL 1 (HI) got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vcal2_hi || vcal2_hi<0){ vcal2_hi = CHIPTEST_VCAL2_HI ; std::cout << SET_FORECOLOR_BLUE << "VCAL 2 (HI) got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(vcal1_hi>vcal2_hi){ vcal1_hi = CHIPTEST_VCAL1_HI ; vcal2_hi = CHIPTEST_VCAL2_HI ;  std::cout << SET_FORECOLOR_BLUE << "VCal range (HI) start greater than end...using defaults instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vcalstep_hi || vcalstep_hi<0){ vcalstep_hi = CHIPTEST_VCALSTEP_HI ; std::cout << SET_FORECOLOR_BLUE << "VCal Step (HI) got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vthreshold1 || vthreshold1<0){ vthreshold1 = CHIPTEST_VTHRESHOLD1 ; std::cout << SET_FORECOLOR_BLUE << "VThreshold1 (HI) got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vthreshold2 || vthreshold2<0){ vthreshold2 = CHIPTEST_VTHRESHOLD2 ; std::cout << SET_FORECOLOR_BLUE << "VThreshold2 (HI) got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }


      if(VFAT_REG_VALUE_MAX<vcal1 || vcal1<0){ vcal1 = CHIPTEST_VCAL1 ; std::cout << SET_FORECOLOR_BLUE << "VCAL 1 got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vcal2 || vcal2<0){ vcal2 = CHIPTEST_VCAL2 ; std::cout << SET_FORECOLOR_BLUE << "VCAL 2 got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(vcal1>vcal2){ vcal1 = CHIPTEST_VCAL1 ; vcal2 = CHIPTEST_VCAL2 ;  std::cout << SET_FORECOLOR_BLUE << "VCal range start greater than end...using defaults instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(VFAT_REG_VALUE_MAX<vcalstep || vcalstep<0){ vcalstep = CHIPTEST_VCALSTEP ; std::cout << SET_FORECOLOR_BLUE << "VCal Step got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
      if(100<sbit_pass_limit || sbit_pass_limit<0){ sbit_pass_limit = CHIPTEST_SBITPASSLIMIT ; std::cout << SET_FORECOLOR_BLUE << "Pass limit for S-Bits got from file NOT OK...using default instead." << SET_DEFAULT_COLORS << std::endl ; }
    }
    else{ std::cout << SET_FORECOLOR_RED << "Couldn't open " << fileName.c_str() << " using default parameters..." << SET_DEFAULT_COLORS << std::endl ; }


    writeCrepeSettings(0) ;
    writeCrepeTrigger(0) ;

    vfatAccess_ = NULL ;
    vfatd_ = NULL ;
    vfatDescription device ;

    int parallelCH = CHIPTEST_PARALLEL_CH ;
    if(advancedChipTesting->isChecked()){ parallelCH = (parallelChannelsT->currentText()).toInt() ; }

#ifdef DEBUGMODE
    std::cout << SET_FORECOLOR_CYAN << parallelCH << " channels in parallel" << SET_DEFAULT_COLORS << std::endl ;
#endif

    bool triggerHybrid = false ;
    if(TriggerTypeSel->isChecked()){ triggerHybrid = true ; }


    bool SCurves_hires = false ;
    if(((advancedChipTesting->isChecked() && sCurvesHiSel->isChecked()) || standardChipTesting->isChecked()) && !triggerHybrid){ SCurves_hires = true ; }

    bool polarityCheck1 = false ;
    if((advancedChipTesting->isChecked() && pol1Sel->isChecked()) || standardChipTesting->isChecked()){ polarityCheck1 = true ; }

    bool SCurves_lowres = false ;
    if(((advancedChipTesting->isChecked() && sCurvesSel->isChecked()) || standardChipTesting->isChecked()) && !triggerHybrid){ SCurves_lowres = true ; }

    bool polarityCheck2 = false ;
    if((advancedChipTesting->isChecked() && pol2Sel->isChecked()) || (standardChipTesting->isChecked() /*&& detectorType_ != "RP"*/)){ polarityCheck2 = true ; }

    bool MSCheck = false ;
    if(((advancedChipTesting->isChecked() && MSSel->isChecked()) || standardChipTesting->isChecked()) && !triggerHybrid){ MSCheck = true ; }

    bool maskCheck = false ;
    if((advancedChipTesting->isChecked() && maskSel->isChecked()) || standardChipTesting->isChecked()){ maskCheck = true ; }

    oss << "<?xml version=\"1.0\"?>" << std::endl ;
    oss << "<ROWSET>" << std::endl ;

    if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << "PULSE TEST:" << SET_DEFAULT_COLORS << std::endl ; }
    results <<  "PULSE TEST:" << std::endl ;
   

    // Get starting values for the config file
    deviceVector vDevices ;
    vfatDescription vfatDefaults ;

    //std::string fn = vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS ;
    getDefaultDescription(vfatDefaults, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;

    vDevices.push_back(&vfatDefaults) ;
    //for( int j=0;j<testVfats_.size();j++) { vDevices.push_back(&vfatDefaults) ; }

    // Get the primary polarity 
    uint polarity = (((vfatDescription *)vDevices.at(0))->getControl0()) & 0x20 ;

    // CR0 value S-outputs disabled
    uint value_sbits_off = (((vfatDescription *)vDevices.at(0))->getControl0()) & 0xf1 ;

    // CR0 value S-outputs active
    uint value_sbits_on = ((vfatDescription *)vDevices.at(0))->getControl0() ;

    bool primaryPolPos = true ;
    if (polarity!=0){ primaryPolPos = false ; }

    if(primaryPolPos){ std::cout << SET_FORECOLOR_BLUE << "Using positive polarity for S-Curves (from file)" << SET_DEFAULT_COLORS << std::endl ; }
    else{ std::cout << SET_FORECOLOR_BLUE << "Using negative polarity for S-Curves (from file)" << SET_DEFAULT_COLORS << std::endl ; }


    // Hi-Res S-Curves
    if(SCurves_hires){

      // Append the complete absolute path
      for(int i=0;i<chipTestDirs_.size();i++){ files_data_hires.push_back(chipTestDirs_.at(i) + TTP_DATA_HIRESPULSESCAN) ; }
      for(int i=0;i<chipTestDirs_.size();i++){ files_config_hires.push_back(chipTestDirs_.at(i) + TTP_CONFIG_HIRESPULSESCAN) ; }
      //for(int i=0;i<chipTestDirs_.size();i++){ files_sbits.push_back(chipTestDirs_.at(i) + TTP_SBIT_INFO) ; }

#ifdef DEBUGMODE
      for(int i=0;i<files_data_hires.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data_hires.at(i) << SET_DEFAULT_COLORS << std::endl ; }
      for(int i=0;i<files_config_hires.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config_hires.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif

      // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
      for(int i=0;i<files_data_hires.size();i++){ writeDataFile(data_hires, files_data_hires.at(i), false) ; }

      //for(int i=0;i<chipTestDirs_.size();i++){ saveSbitInfo(files_sbits, false) ; }

      for(int j=0;j<files_config_hires.size();j++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_hires.at(j)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  if(j==0){ outputfilestream << oss.str() ; }
	  else if(j==1){ outputfilestream << oss.str() ; }
	  else if(j==2){ outputfilestream << oss.str() ; }
	  else if(j==3){ outputfilestream << oss.str() ; }

	  outputfilestream.close() ;
	}
      }
    }

    // Low-Res S-Curves
    if(SCurves_lowres){

      // Append the complete absolute path
      for(int i=0;i<chipTestDirs_.size();i++){ files_data.push_back(chipTestDirs_.at(i) + TTP_DATA_PULSESCAN) ; }
      for(int i=0;i<chipTestDirs_.size();i++){ files_config.push_back(chipTestDirs_.at(i) + TTP_CONFIG_PULSESCAN) ; }

      // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
      for(int i=0;i<files_data.size();i++){ writeDataFile(data, files_data.at(i), false) ; }

#ifdef DEBUGMODE
      for(int i=0;i<files_data.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data.at(i) << SET_DEFAULT_COLORS << std::endl ; }
      for(int i=0;i<files_config.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif
    }

    // Primary polarity files
    if(polarityCheck1){

      //if(detectorType_=="RP"){
      if(primaryPolPos){

	// Append the complete absolute path
	for(int i=0;i<chipTestDirs_.size();i++){ files_data_pol1.push_back(chipTestDirs_.at(i) + TTP_DATA_POSPOLARITY) ; }
	for(int i=0;i<chipTestDirs_.size();i++){ files_config_pol1.push_back(chipTestDirs_.at(i) + TTP_CONFIG_POSPOLARITY) ; }

      }
      else{

	// Append the complete absolute path
	for(int i=0;i<chipTestDirs_.size();i++){ files_data_pol1.push_back(chipTestDirs_.at(i) + TTP_DATA_NEGPOLARITY) ; }
	for(int i=0;i<chipTestDirs_.size();i++){ files_config_pol1.push_back(chipTestDirs_.at(i) + TTP_CONFIG_NEGPOLARITY) ; }
      }


#ifdef DEBUGMODE
      for(int i=0;i<files_data_pol1.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data_pol1.at(i) << SET_DEFAULT_COLORS << std::endl ; }
      for(int i=0;i<files_config_pol1.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config_pol1.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif

      // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
      for(int i=0;i<files_data_pol1.size();i++){ writeDataFile(data_pol1, files_data_pol1.at(i), false) ; }

      for(int j=0;j<files_config_pol1.size();j++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol1.at(j)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  if(j==0){ outputfilestream << oss.str() ; }
	  else if(j==1){ outputfilestream << oss.str() ; }
	  else if(j==2){ outputfilestream << oss.str() ; }
	  else if(j==3){ outputfilestream << oss.str() ; }

	  outputfilestream.close() ;
	}
      }
    }

    // Secondary polarity files
    if(polarityCheck2){

      if(primaryPolPos){

	// Append the complete absolute path
	for(int i=0;i<chipTestDirs_.size();i++){ files_data_pol2.push_back(chipTestDirs_.at(i) + TTP_DATA_NEGPOLARITY) ; }
	for(int i=0;i<chipTestDirs_.size();i++){ files_config_pol2.push_back(chipTestDirs_.at(i) + TTP_CONFIG_NEGPOLARITY) ; }

      }
      else{

	// Append the complete absolute path
	for(int i=0;i<chipTestDirs_.size();i++){ files_data_pol2.push_back(chipTestDirs_.at(i) + TTP_DATA_POSPOLARITY) ; }
	for(int i=0;i<chipTestDirs_.size();i++){ files_config_pol2.push_back(chipTestDirs_.at(i) + TTP_CONFIG_POSPOLARITY) ; }
      }

#ifdef DEBUGMODE
      for(int i=0;i<files_data_pol2.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data_pol2.at(i) << SET_DEFAULT_COLORS << std::endl ; }
      for(int i=0;i<files_config_pol2.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config_pol2.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif

      // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
      for(int i=0;i<files_data_pol2.size();i++){ writeDataFile(data_pol2, files_data_pol2.at(i), false) ; }

      for(int j=0;j<files_config_pol2.size();j++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol2.at(j)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  if(j==0){ outputfilestream << oss.str() ; }
	  else if(j==1){ outputfilestream << oss.str() ; }
	  else if(j==2){ outputfilestream << oss.str() ; }
	  else if(j==3){ outputfilestream << oss.str() ; }

	  outputfilestream.close() ;
	}
      }
    }

    // MS check files
    if(MSCheck){

      // Append the complete absolute path
      for(int i=0;i<chipTestDirs_.size();i++){ files_data_ms.push_back(chipTestDirs_.at(i) + TTP_DATA_MSCHECK) ; }
      for(int i=0;i<chipTestDirs_.size();i++){ files_config_ms.push_back(chipTestDirs_.at(i) + TTP_CONFIG_MSCHECK) ; }

#ifdef DEBUGMODE
      for(int i=0;i<files_data_ms.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_data_ms.at(i) << SET_DEFAULT_COLORS << std::endl ; }
      for(int i=0;i<files_config_ms.size();i++){ std::cout << SET_FORECOLOR_CYAN << files_config_ms.at(i) << SET_DEFAULT_COLORS << std::endl ; }
#endif

      // Create the data files with TTP header and config files with default tag/ Create the data files with TTP header and config files with default tag
      for(int i=0;i<files_data_ms.size();i++){ writeDataFile(data_ms, files_data_ms.at(i), false) ; }

      for(int j=0;j<files_config_ms.size();j++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_ms.at(j)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  if(j==0){ outputfilestream << oss.str() ; }
	  else if(j==1){ outputfilestream << oss.str() ; }
	  else if(j==2){ outputfilestream << oss.str() ; }
	  else if(j==3){ outputfilestream << oss.str() ; }

	  outputfilestream.close() ;
	}
      }
    }


    // Hi-Res S-Curves
    if(SCurves_hires){

      std::cout << SET_FORECOLOR_BLUE << "* Hi-Res S-Curves" << SET_DEFAULT_COLORS << std::endl ;

      oss.str("") ;

      // Get starting values for the config file
      deviceVector vDevices_hires ;
      vfatDescription vfatDefaults_hires ;

      getDefaultDescription(vfatDefaults_hires, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;

      vfatDefaults_hires.setVCal(vcal1_hi) ;
      vfatDefaults_hires.setVThreshold1(vthreshold1) ;
      vfatDefaults_hires.setVThreshold2(vthreshold2) ;

      vDevices_hires.push_back(&vfatDefaults_hires) ;

      // Default tag for Monitor
      oss << getXMLTagDefault(vDevices_hires) ;

      if(files_config_hires.size()>=1){ oss_0 << oss.str() ; }
      if(files_config_hires.size()>=2){ oss_1 << oss.str() ; }
      if(files_config_hires.size()>=3){ oss_2 << oss.str() ; }
      if(files_config_hires.size()>=4){ oss_3 << oss.str() ; }
      oss.str("") ;


      // Change test specific settings for every VFAT
      for( int j=0;j<testVfats_.size();j++) {

	vfatAccess_ = testVfats_.at(j) ;

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{

	    loop++ ;
	    vfatAccess_->setVfatVThreshold1(vthreshold1) ;       
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(6, j) ;
	  return (results.str());
	}

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{

	    loop++ ;
	    vfatAccess_->setVfatVThreshold2(vthreshold2) ;   
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(6, j) ;
	  return (results.str());
	}
      }



      // Scan through channels
      for(unsigned int a=1;a<=VFAT_CH_NUM_MAX;a++){

	if( pulseThisChannel[a] == VFAT_PULSE_FLAG ){

	  // Activate channel for every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    //uint chanregvalue = VFAT_CALPULSE_MASK ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatChanReg(a, VFAT_CALPULSE_MASK) ;          
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }
	 
	  
	  // Scan through VCAL range	  
	  for(unsigned int i=vcal1_hi;i<=vcal2_hi;i+=vcalstep_hi){

	    //oss << "<configuration number=\"" << confNumber_hires << "\">"  << std::endl ;

	    for( int j=0;j<testVfats_.size();j++) {

	      oss << "<configuration number=\"" << confNumber_hires << "\">"  << std::endl ;

	      vfatAccess_ = testVfats_.at(j) ;

	      uint chanregvalue = VFAT_CALPULSE_MASK ;
	      uint maskvalue = 0 , trimdacvalue = 0 ;
      
	      oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		  << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2))   // first 2 digits
		  << "\" chanreg" << (unsigned long)a << "=\"" << (unsigned long)chanregvalue
		  << "\" vcal=\"" << (unsigned long)i << "\" />" << std::endl ;

	      oss << "</configuration>" << std::endl ;

	      if(j==0){ oss_0 << oss.str() ; }
	      else if(j==1){ oss_1 << oss.str() ; }
	      else if(j==2){ oss_2 << oss.str() ; }
	      else if(j==3){ oss_3 << oss.str() ; }

	      oss.str("") ;

	      loop = 0 ;
	      do{

		exceptionThrown = false ;
		try{

		  loop++ ;
		  vfatAccess_->setVfatVCal((tscType8)i) ;	           
		}
		catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	      }
	      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	      if(exceptionThrown){

		results << testI2CError(5, j) ;
		return (results.str());
	      }
	      
	      if(showConsole_) {

		if(i==vcal1_hi && j==0){

		  std::cout << SET_FORECOLOR_BLUE << std::dec << "Channel " << a << "\t VCal " << vcal1_hi << "->" << vcal2_hi ;
		  
		  if(testPulseTrimDACSel->isChecked()){ std::cout << SET_FORECOLOR_BLUE << " / TrimDAC " << trimdacvalue ; }

		  if (maskvalue != 0) { std::cout << SET_FORECOLOR_BLUE << " *** MASKED ***" ; }
		  std::cout << SET_DEFAULT_COLORS << std::endl ;
		  
		}
	      }
	    }

	    confNumber_hires+=1 ;

	    resetCrepeFifos(0) ;

	    msleep(100) ;

	    startCrepeTrigger(0) ;
	    readCrepeDataBuffers(0, data_hires, files_data_hires, true) ; // Checking was not done

	    for(int j=0;j<files_config_hires.size();j++){

	      if (!outputfilestream.is_open()){ outputfilestream.open((files_config_hires.at(j)).c_str(), std::ios::app) ; }
 
	      if(outputfilestream.is_open()){

		if(j==0){ outputfilestream << oss_0.str() ; oss_0.str("") ; }
		else if(j==1){ outputfilestream << oss_1.str() ; oss_1.str("") ; }
		else if(j==2){ outputfilestream << oss_2.str() ; oss_2.str("") ; }
		else if(j==3){ outputfilestream << oss_3.str() ; oss_3.str("") ; }

		outputfilestream.close() ;
	      }
	    }
	  }

	  
	  // Disable channel for every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;

	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatChanReg(a, (tscType8)0x00) ;  // Set no cal pulse
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }
	}
      }



      // Restore the settings for every VFAT
      for( int j=0;j<testVfats_.size();j++) {

	vfatAccess_ = testVfats_.at(j) ;

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{

	    loop++ ;
 	    vfatAccess_->setVfatVThreshold1(((vfatDescription *)vDevices.at(0))->getVThreshold1()) ; // Restore the VT1      
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(6, j) ;
	  return (results.str());
	}

	loop = 0 ;
	do{

	  exceptionThrown = false ;
	  try{

	    loop++ ;
	    vfatAccess_->setVfatVThreshold2(((vfatDescription *)vDevices.at(0))->getVThreshold2()) ; // Restore the VT2      
	  }
	  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	}
	while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	if(exceptionThrown){

	  results << testI2CError(6, j) ;
	  return (results.str());
	}
      }



      std::cout << std::endl ;
      oss << "</ROWSET>" << std::endl ;

      for(int i=0;i<files_config_hires.size();i++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_hires.at(i)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	}
      }
    }

    if( SCurves_lowres || polarityCheck1 || polarityCheck2 || MSCheck ){
    
      oss.str("") ;
      oss << "<?xml version=\"1.0\"?>" << std::endl ;
      oss << "<ROWSET>" << std::endl ;

      // Default tag for Monitor
      oss << getXMLTagDefault(vDevices) ;

      if(files_config.size()>=1){ oss_0 << oss.str() ; }
      if(files_config.size()>=2){ oss_1 << oss.str() ; }
      if(files_config.size()>=3){ oss_2 << oss.str() ; }
      if(files_config.size()>=4){ oss_3 << oss.str() ; }
      oss.str("") ;


      // Scan through channels
      for(unsigned int a=1;a<=VFAT_CH_NUM_MAX/parallelCH;a++){
      //for(unsigned int a=1;a<=1;a++){

   
	// Activate channel(s)
	for(int k=0;k<parallelCH;k++){

	  uint jump = 0 ;
	  if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }

	  uint ch = a+(jump*k) ;

#ifdef DEBUGMODE
	  std::cout << SET_FORECOLOR_CYAN << "Activate CH #" << ch << SET_DEFAULT_COLORS << std::endl ; 
#endif

	  // For every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    //uint chanregvalue = VFAT_CALPULSE_MASK ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatChanReg(ch, VFAT_CALPULSE_MASK) ;//a	           
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }
	}
      


	// Low-Res S-Curves
	if(SCurves_lowres){

	  if(showConsole_) { std::cout << SET_FORECOLOR_BLUE << std::dec << parallelCH << " channel(s) \tVCal " << vcal1 << "->" << vcal2 << "\tSTEP #" << a << SET_DEFAULT_COLORS << std::endl ; }
	  std::cout << SET_FORECOLOR_BLUE << "* Low-Res S-Curves" << SET_DEFAULT_COLORS << std::endl ;
   
	  // Scan through VCAL range	  
	  for(unsigned int i=vcal1;i<=vcal2;i+=vcalstep){

	    //oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	    for( int j=0;j<testVfats_.size();j++) {

	      oss << "<configuration number=\"" << confNumber << "\">"  << std::endl ;

	      vfatAccess_ = testVfats_.at(j) ;

	      uint chanregvalue = VFAT_CALPULSE_MASK ;
	      uint maskvalue = 0 , trimdacvalue = 0 ;
      
	      oss << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		  << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) ;  // first 2 digits
	      //<< "\" chanreg" << (unsigned long)a << "=\"" << (unsigned long)chanregvalue

	      for(int k=0;k<parallelCH;k++){

		uint jump = 0 ;
		if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }
		uint ch = a+(jump*k) ;

		oss << "\" chanreg" << (unsigned long)ch << "=\"" << (unsigned long)chanregvalue ;
	      }

	      oss << "\" vcal=\"" << (unsigned long)i << "\" />" << std::endl ;
	      oss << "</configuration>" << std::endl ;

	      if(j==0){ oss_0 << oss.str() ; }
	      else if(j==1){ oss_1 << oss.str() ; }
	      else if(j==2){ oss_2 << oss.str() ; }
	      else if(j==3){ oss_3 << oss.str() ; }

	      oss.str("") ;

	      loop = 0 ;
	      do{

		exceptionThrown = false ;
		try{

		  loop++ ;
		  vfatAccess_->setVfatVCal((tscType8)i) ;	           
		}
		catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	      }
	      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	      if(exceptionThrown){

		results << testI2CError(5, j) ;
		return (results.str());
	      }
	      /*	      
	      if(showConsole_ && i==vcal1 && j==0) {

		std::cout << SET_FORECOLOR_BLUE << std::dec << parallelCH << " channel(s) \tVCal " << vcal1 << "->" << vcal2 << "\tSTEP #" << a << SET_DEFAULT_COLORS << std::endl ;		
	      }
	      */
	    }

	    //oss << "</configuration>" << std::endl ;
	    confNumber+=1 ;

	    resetCrepeFifos(0) ;
	    startCrepeTrigger(0) ;
	    readCrepeDataBuffers(0, data, files_data, true) ; // Checking was not done

	    for(int j=0;j<files_config.size();j++){

	      if (!outputfilestream.is_open()){ outputfilestream.open((files_config.at(j)).c_str(), std::ios::app) ; }
 
	      if(outputfilestream.is_open()){

		if(j==0){ outputfilestream << oss_0.str() ; oss_0.str("") ; }
		else if(j==1){ outputfilestream << oss_1.str() ; oss_1.str("") ; }
		else if(j==2){ outputfilestream << oss_2.str() ; oss_2.str("") ; }
		else if(j==3){ outputfilestream << oss_3.str() ; oss_3.str("") ; }

		outputfilestream.close() ;
	      }
	    }

	    triggerCount += 1 ;
	  }
	}


	// Change large test pulse for binary checks
	if( !SCurves_lowres && (polarityCheck1 || polarityCheck2 || MSCheck) ){

	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;

	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVCal(vcal2) ;  // Set the test pulse
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }
	}
   

	// Checking the primary polarity
	if(polarityCheck1){

	  std::cout << SET_FORECOLOR_BLUE << "* 1st Binary Check" << SET_DEFAULT_COLORS << std::endl ;

	  // Add default tag if the loop is going 1st time
	  if(a==1){

	    // Default tag for Monitor
	    oss_pol1 << getXMLTagDefault(vDevices) ;

	    if(files_config_pol1.size()>=1){ oss_0 << oss_pol1.str() ; }
	    if(files_config_pol1.size()>=2){ oss_1 << oss_pol1.str() ; }
	    if(files_config_pol1.size()>=3){ oss_2 << oss_pol1.str() ; }
	    if(files_config_pol1.size()>=4){ oss_3 << oss_pol1.str() ; }
	    oss_pol1.str("") ;
	  }


	  // For every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    oss_pol1 << "<configuration number=\"" << a << "\">"  << std::endl ;

	    vfatAccess_ = testVfats_.at(j) ;
	    //uint chanregvalue = VFAT_CALPULSE_MASK ;
      
	    oss_pol1 << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		     << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) ;  // first 2 digits

	    for(int k=0;k<parallelCH;k++){

	      uint jump = 0 ;
	      if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }
	      uint ch = a+(jump*k) ;

	      oss_pol1 << "\" chanreg" << (unsigned long)ch << "=\"" << VFAT_CALPULSE_MASK ;
	    }

	    oss_pol1 << "\" vcal=\"" << vcal2  << "\" />" << std::endl ; // VFAT_VCAL_TEST_VALUE

	    oss_pol1 << "</configuration>" << std::endl ;

	    if(j==0){ oss_0 << oss_pol1.str() ; }
	    else if(j==1){ oss_1 << oss_pol1.str() ; }
	    else if(j==2){ oss_2 << oss_pol1.str() ; }
	    else if(j==3){ oss_3 << oss_pol1.str() ; }

	    oss_pol1.str("") ;
	  }

	  resetCrepeFifos(0) ;
	  startCrepeTrigger(0) ;
	  readCrepeDataBuffers(0, data_pol1, files_data_pol1, true) ;

	  for(int j=0;j<files_config_pol1.size();j++){

	    if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol1.at(j)).c_str(), std::ios::app) ; }
 
	    if(outputfilestream.is_open()){

	      if(j==0){ outputfilestream << oss_0.str() ; oss_0.str("") ; }
	      else if(j==1){ outputfilestream << oss_1.str() ; oss_1.str("") ; }
	      else if(j==2){ outputfilestream << oss_2.str() ; oss_2.str("") ; }
	      else if(j==3){ outputfilestream << oss_3.str() ; oss_3.str("") ; }

	      outputfilestream.close() ;
	    }
	  }



	  // Disable S-outputs on all VFATs
	  for( int j=0;j<testVfats_.size();j++){

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;

	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl0(value_sbits_off) ; // Disable S-outputs
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }


	  // S-bits separately for every VFAT
	  for( int j=0;j<testVfats_.size();j++){

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl0(value_sbits_on) ; // Active S-outputs
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }

	    resetCrepeFifos(0) ;
	    startCrepeTrigger(0) ;

	    // Save S-bit info
	    saveSbitInfo(total_sbits, sectors_1, sectors_2, sectors_3, sectors_4, a) ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl0(value_sbits_off) ; // Disable S-outputs
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }

	}


	// Checking the secondary polarity
	if(polarityCheck2){

	  std::cout << SET_FORECOLOR_BLUE << "* 2nd Binary Check" << std::endl ;

	  // The settings are the same for all the chips
	  uint tmp = ((vfatDescription *)vDevices.at(0))->getControl0() ;
	  uint value = 0 ; //(tmp & 0xCF) | 0x30 ;

	  // Change neg. for RP and pos. for GEM
	  //if(detectorType_=="RP"){ value = (tmp & 0xCF) | 0x30 ; }
	  if(primaryPolPos){ value = (tmp & 0xCF) | 0x30 ; }
	  else{ value = tmp & 0xCF ; }


	  // Swap the threshold values
	  uint vt2 = ((vfatDescription *)vDevices.at(0))->getVThreshold1() ;
	  uint vt1 = ((vfatDescription *)vDevices.at(0))->getVThreshold2() ;

	  // Add default tag if the loop is going 1st time
	  if(a==1){

	    // Get starting values for the config file
	    deviceVector vDevices_pol2 ;
	    vfatDescription vfatDefaults_pol2 ;

	    //if(vDevices.size()>0){ vfatDefaults_pol2 = (vfatDescription)vDevices.at(0) ; }

	    getDefaultDescription(vfatDefaults_pol2, vfat_controller_home_ + CHIPTEST_DEFAULT_SETTINGS) ;

	    vfatDefaults_pol2.setControl0(value) ;
	    vfatDefaults_pol2.setVThreshold1(vt1) ;
	    vfatDefaults_pol2.setVThreshold2(vt2) ;

	    vDevices_pol2.push_back(&vfatDefaults_pol2) ;

	    //for( int j=0;j<testVfats_.size();j++) { vDevices_pol.push_back(&vfatDefaults_pol) ; }

	    // Default tag for Monitor
	    oss_pol2 << getXMLTagDefault(vDevices_pol2) ;

	    if(files_config_pol2.size()>=1){ oss_0 << oss_pol2.str() ; }
	    if(files_config_pol2.size()>=2){ oss_1 << oss_pol2.str() ; }
	    if(files_config_pol2.size()>=3){ oss_2 << oss_pol2.str() ; }
	    if(files_config_pol2.size()>=4){ oss_3 << oss_pol2.str() ; }
	    oss_pol2.str("") ;
	  }


	  // For every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    oss_pol2 << "<configuration number=\"" << a << "\">"  << std::endl ;

	    vfatAccess_ = testVfats_.at(j) ;
	    //uint chanregvalue = VFAT_CALPULSE_MASK ;
      
	    oss_pol2 << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		     << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) ;  // first 2 digits

	    for(int k=0;k<parallelCH;k++){

	      uint jump = 0 ;
	      if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }
	      uint ch = a+(jump*k) ;

	      oss_pol2 << "\" chanreg" << (unsigned long)ch << "=\"" << VFAT_CALPULSE_MASK ;
	    }

	    oss_pol2 << "\" vcal=\"" << vcal2 << "\" />" << std::endl ; // VFAT_VCAL_TEST_VALUE

	    oss_pol2 << "</configuration>" << std::endl ;

	    if(j==0){ oss_0 << oss_pol2.str() ; }
	    else if(j==1){ oss_1 << oss_pol2.str() ; }
	    else if(j==2){ oss_2 << oss_pol2.str() ; }
	    else if(j==3){ oss_3 << oss_pol2.str() ; }

	    oss_pol2.str("") ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl0(value) ; // Change the CalPulse and MS polarities      
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVThreshold1(vt1) ; // Set the new threshold        
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVThreshold2(vt2) ; // Set the new threshold        
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }
	  }


	  resetCrepeFifos(0) ;
	  startCrepeTrigger(0) ;
	  readCrepeDataBuffers(0, data_pol2, files_data_pol2, true) ;

	  for(int j=0;j<files_config_pol2.size();j++){

	    if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol2.at(j)).c_str(), std::ios::app) ; }
 
	    if(outputfilestream.is_open()){

	      if(j==0){ outputfilestream << oss_0.str() ; oss_0.str("") ; }
	      else if(j==1){ outputfilestream << oss_1.str() ; oss_1.str("") ; }
	      else if(j==2){ outputfilestream << oss_2.str() ; oss_2.str("") ; }
	      else if(j==3){ outputfilestream << oss_3.str() ; oss_3.str("") ; }

	      outputfilestream.close() ;
	    }
	  }


	  // For every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl0(((vfatDescription *)vDevices.at(0))->getControl0()) ; // Restore the CR0 (polarity)           
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVThreshold1(((vfatDescription *)vDevices.at(0))->getVThreshold1()) ; // Restore the threshold        
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatVThreshold2(((vfatDescription *)vDevices.at(0))->getVThreshold2()) ; // Restore the threshold        
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	  }
	  //	}
	}

	// Checking the monostable
	if(MSCheck){

	  std::cout << SET_FORECOLOR_BLUE << "* Monostable Check" << SET_DEFAULT_COLORS << std::endl ;

	  // Add default tag if the loop is going 1st time
	  if(a==1){

	    oss_ms << getXMLTagDefault(vDevices) ;
	    if(files_config_ms.size()>=1){ oss_0 << oss_ms.str() ; }
	    if(files_config_ms.size()>=2){ oss_1 << oss_ms.str() ; }
	    if(files_config_ms.size()>=3){ oss_2 << oss_ms.str() ; }
	    if(files_config_ms.size()>=4){ oss_3 << oss_ms.str() ; }
	    oss_ms.str("") ;
	  }

	  uint lat = ((vfatDescription *)vDevices.at(0))->getLat() ;
	  //uint value = ((vfatDescription *)vDevices.at(0))->getControl2() ;

	  // Scan through MS values
	  for(uint i=0x00;i<=CHIPTEST_MS_MAX;i+=CHIPTEST_MS_STEP){

#ifdef DEBUGMODE
	    //std::cout << SET_FORECOLOR_CYAN << "MS =  0x" << std::hex << i << std::dec << SET_DEFAULT_COLORS << std::endl ;
	    std::cout << SET_FORECOLOR_CYAN << "MS length = " <<  (i >> 4)+1 << " CLK" <<  SET_DEFAULT_COLORS << std::endl ;
#endif

	    // Change MS for every VFAT
	    for( int j=0;j<testVfats_.size();j++) {

	      vfatAccess_ = testVfats_.at(j) ;

	      loop = 0 ;
	      do{

		exceptionThrown = false ;
		try{

		  loop++ ;
		  vfatAccess_->setVfatControl2(i) ; // Change MS      
		}
		catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	      }
	      while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	      if(exceptionThrown){

		results << testI2CError(6, j) ;
		return (results.str());
	      }
	    }

	    // Scan through latency values
	    for(uint l=lat+1;l>=lat-(i >> 4)-1;l--){

#ifdef DEBUGMODE
	      std::cout << SET_FORECOLOR_CYAN << "LAT: " << l << SET_DEFAULT_COLORS << std::endl ;
#endif

	      // Change latency for every VFAT
	      for( int j=0;j<testVfats_.size();j++) {

		oss_ms << "<configuration number=\"" << confNumber_ms << "\">"  << std::endl ;

		vfatAccess_ = testVfats_.at(j) ;

		loop = 0 ;
		do{

		  exceptionThrown = false ;
		  try{

		    loop++ ;
		    vfatAccess_->setVfatLat(l) ; // Change latency      
		  }
		  catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
		}
		while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
		if(exceptionThrown){

		  results << testI2CError(6, j) ;
		  return (results.str());
		}
	    
		oss_ms << "\t<VFAT chipid0=\"" << HexToInt(testVfatIds_.at(j).substr(4,2)) // last 2 digits
		       << "\" chipid1=\"" << HexToInt(testVfatIds_.at(j).substr(2,2)) ;  // first 2 digits

		for(int k=0;k<parallelCH;k++){

		  uint jump = 0 ;
		  if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }
		  uint ch = a+(jump*k) ;

		  oss_ms << "\" chanreg" << (unsigned long)ch << "=\"" << VFAT_CALPULSE_MASK ;
		}

		oss_ms << "\" lat=\"" << (unsigned long)l
		       << "\" control2=\"" << (unsigned long)i << "\" />" << std::endl ;

		oss_ms << "</configuration>" << std::endl ;

		if(j==0){ oss_0 << oss_ms.str() ; }
		else if(j==1){ oss_1 << oss_ms.str() ; }
		else if(j==2){ oss_2 << oss_ms.str() ; }
		else if(j==3){ oss_3 << oss_ms.str() ; }

		oss_ms.str("") ;
	      }

	      //oss_ms << "</configuration>" << std::endl ;
	      confNumber_ms++ ;

	      resetCrepeFifos(0) ;
	      startCrepeTrigger(0) ;
	      readCrepeDataBuffers(0, data_ms, files_data_ms, true) ;

	      for(int j=0;j<files_config_ms.size();j++){

		if (!outputfilestream.is_open()){ outputfilestream.open((files_config_ms.at(j)).c_str(), std::ios::app) ; }
 
		if(outputfilestream.is_open()){

		  if(j==0){ outputfilestream << oss_0.str() ; oss_0.str("") ; }
		  else if(j==1){ outputfilestream << oss_1.str() ; oss_1.str("") ; }
		  else if(j==2){ outputfilestream << oss_2.str() ; oss_2.str("") ; }
		  else if(j==3){ outputfilestream << oss_3.str() ; oss_3.str("") ; }

		  outputfilestream.close() ;
		}
	      }
	    }

	  }



	  // Restore the settings for every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatControl2(((vfatDescription *)vDevices.at(0))->getControl2()) ; // Restore the CR2 (MS)           
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }

	    loop = 0 ;
	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatLat(((vfatDescription *)vDevices.at(0))->getLat()) ; // Restore the latency           
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(6, j) ;
	      return (results.str());
	    }
	  }
	}


	// Disable channel(s) and also restore the other settings
	for(int k=0;k<parallelCH;k++){

	  uint jump = 0 ;
	  if(parallelCH!=1){ jump = VFAT_CH_NUM_MAX/parallelCH ; }

	  uint ch = a+(jump*k) ;

#ifdef DEBUGMODE
	  std::cout << SET_FORECOLOR_CYAN << "Deactivate CH #" << ch << SET_DEFAULT_COLORS << std::endl ; 
#endif

	  // For every VFAT
	  for( int j=0;j<testVfats_.size();j++) {

	    vfatAccess_ = testVfats_.at(j) ;
	    loop = 0 ;

	    do{

	      exceptionThrown = false ;
	      try{

		loop++ ;
		vfatAccess_->setVfatChanReg(ch, (tscType8)0x00) ;  // Set no cal pulse
	      }
	      catch (FecExceptionHandler &e) { std::cout << SET_FORECOLOR_RED << "WARNING: I2C COMMAND FAILURE (" << loop << ")" << SET_DEFAULT_COLORS << std::endl ; exceptionThrown = true ; }
	    }
	    while(exceptionThrown && loop<VFAT_I2C_RETRIES) ;
  
	    if(exceptionThrown){

	      results << testI2CError(5, j) ;
	      return (results.str());
	    }
	  }
	}   
      }
    }

    
#ifdef DEBUG_VFAT_CONTROLLER      
    std::cout << SET_FORECOLOR_CYAN << std::dec << "Triggercommand given " << triggerCount << " times." << SET_DEFAULT_COLORS << std::endl ;
#endif      

    oss << "</ROWSET>" << std::endl ;

    for(int i=0;i<files_config.size();i++){

      if (!outputfilestream.is_open()){ outputfilestream.open((files_config.at(i)).c_str(), std::ios::app) ; }
 
      if(outputfilestream.is_open()){

	outputfilestream << oss.str() ;
	outputfilestream.close() ;
      }
    }


    if(polarityCheck1){

      std::vector<std::string> fileNames_summary ;
      for(int i=0;i<chipTestDirs_.size();i++){ fileNames_summary.push_back(chipTestDirs_.at(i) + TTP_SUMMARY) ; }

      float hits_expected = 1 * CHIPTEST_SBITEXPECTEDHITS ; // float is better for calculations...

      float minCount = sbit_pass_limit*hits_expected/100 ;
      float maxCount = (200-sbit_pass_limit)*hits_expected/100 ;
      float maxNumber = testVfats_.size()*CHIPTEST_SBITMAXHITS ;

      int nSectors = 8 ;
      if(detectorType_=="RP"){ nSectors = 4 ; }

#ifdef DEBUG_VFAT_CONTROLLER  
      std::cout << SET_FORECOLOR_CYAN << std::dec << "accept min=" << minCount << " accept max=" << maxCount << " max=" << maxNumber << SET_DEFAULT_COLORS << std::endl ;
#endif 

      for(int i=0;i<fileNames_summary.size();i++){

	std::ostringstream oss ;
	oss << std::endl << "# begin: test `S-bits'" << std::endl ;

	for(int j=1;j<=nSectors;j++){

	  oss << "S" << j << "\t" ;

	  if(i==0){

	    if(sectors_1[j]>= minCount && sectors_1[j]<= maxCount){ oss << "OK" ; }
	    else{ oss << "FAIL" ; }
	    oss << "\t" << sectors_1[j] / hits_expected * 100 << " %\t(" << sectors_1[j] << "/" << hits_expected << ")" ;

	    if(sectors_1[j]>= maxNumber){ oss << "\tALWAYS ON" << std::endl ; }
	    else if (sectors_1[j]==0){ oss << "\tALWAYS OFF" << std::endl ; }
	    else{ oss << std::endl ; }
	  }

	  if(i==1){

	    if(sectors_2[j]>= minCount && sectors_2[j]<= maxCount){ oss << "OK" ; }
	    else{ oss << "FAIL" ; }
	    oss << "\t" << sectors_2[j] / hits_expected * 100 << " %\t(" << sectors_2[j] << "/" << hits_expected << ")" ;

	    if(sectors_2[j]>= maxNumber){ oss << "\tALWAYS ON" << std::endl ; }
	    else if (sectors_2[j]==0){ oss << "\tALWAYS OFF" << std::endl ; }
	    else{ oss << std::endl ; }
	  }

	  if(i==2){

	    if(sectors_3[j]>= minCount && sectors_3[j]<= maxCount){ oss << "OK" ; }
	    else{ oss << "FAIL" ; }
	    oss << "\t" << sectors_3[j] / hits_expected * 100 << " %\t(" << sectors_3[j] << "/" << hits_expected << ")" ;

	    if(sectors_3[j]>= maxNumber){ oss << "\tALWAYS ON" << std::endl ; }
	    else if (sectors_3[j]==0){ oss << "\tALWAYS OFF" << std::endl ; }
	    else{ oss << std::endl ; }
	  }

	  if(i==3){

	    if(sectors_4[j]>= minCount && sectors_4[j]<= maxCount){ oss << "OK" ; }
	    else{ oss << "FAIL" ; }
	    oss << "\t" << sectors_4[j] / hits_expected * 100 << " %\t(" << sectors_4[j] << "/" << hits_expected << ")" ;

	    if(sectors_4[j]>= maxNumber){ oss << "\tALWAYS ON" << std::endl ; }
	    else if (sectors_4[j]==0){ oss << "\tALWAYS OFF" << std::endl ; }
	    else{ oss << std::endl ; }
	  }
	}

	oss << "# end: test `S-bits'" << std::endl << std::endl ;

	if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	}
      }

      for(int i=0;i<files_config_pol1.size();i++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol1.at(i)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	}
      }
    }


    if(polarityCheck2){

      for(int i=0;i<files_config_pol2.size();i++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_pol2.at(i)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	}
      }
    }

    if(MSCheck){

      for(int i=0;i<files_config_ms.size();i++){

	if (!outputfilestream.is_open()){ outputfilestream.open((files_config_ms.at(i)).c_str(), std::ios::app) ; }
 
	if(outputfilestream.is_open()){

	  outputfilestream << oss.str() ;
	  outputfilestream.close() ;
	}
      }
    }
  }   
  else { std::cout << SET_FORECOLOR_RED << "Software problem: ChipTesting flag false." << SET_DEFAULT_COLORS << std::endl ; }

  return (results.str()) ;
}

#ifdef USB_GPIB_PS
/*
 * Power on the HV 
 */
void vfat_controller::powerOnHV(){

  std::ofstream outputfilestream ;
  std::ostringstream oss ;

  std::vector<std::string> fileNames_summary ;

  // Append the complete absolute path
  for(int i=0;i<chipTestDirs_.size();i++){ fileNames_summary.push_back(chipTestDirs_.at(i) + TTP_SUMMARY) ; }

  std::cout << SET_FORECOLOR_BLUE << "RAMPING UP THE \"HV\"" << SET_DEFAULT_COLORS << std::endl ;

  // Set the current limit
  try{

    std::stringstream cmd ;
    cmd << "SENS:CURR:PROT:LEV " << GPIB_HV_CURRENT ;

    GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_HV_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    gp.write(cmd.str()) ;

  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }


  // Ramp up the HV
  try{

    int voltage = 0 ;

    GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_HV_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    do{

      voltage += GPIB_HV_VOLTAGE_STEP ;
      std::stringstream cmd ;
      cmd << "sour:volt:imm:ampl " << voltage << " \n" ;

      gp.write(cmd.str()) ;

      msleep(GPIB_HV_DELAY) ;
    }
    while(voltage<GPIB_HV_VOLTAGE) ;
  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }

  oss << std::endl << "# begin: test `Detector Biasing'" << std::endl ;
  oss << "Bias:\t" << GPIB_HV_VOLTAGE << " V" << std::endl ;

  // Get the correct DCU to read
  for(int i=0;i<dcuAddresses_.size();i++) {

    if(getChannelKey(dcuAddresses_.at(i)) == DCU_RPHYBRID_CH){

      dcuAccess_ = (dcuAccess *)fecAccessManager_->getAccess(DCU, dcuAddresses_.at(i)) ;
      break ;
    }
  }


  int beBits = (int)dcuAccess_->getDcuChannel((tscType8)DCU_RP_BE_CH, false) ;
  oss << "BE:\t" << beBits*BE_CONVERTION_FACTOR << " nA (" << std::dec << beBits << ")" << std::endl ;

  int ctsBits = (int)dcuAccess_->getDcuChannel((tscType8)DCU_RP_CTS_CH, false) ;
  oss << "CTS:\t" << ctsBits*CTS_CONVERTION_FACTOR << " µA (" << std::dec << ctsBits << ")" << std::endl ;


  // Read the total currect
  try{

    GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_HV_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    std::stringstream cmd ;
    cmd << "meas:curr?" ;

    QString answer = gp.read(cmd.str()) ;

    // The current is the 2nd value
    QStringList list = QStringList::split(",", answer) ;
    oss << "Total:\t" << list[1].latin1() << " A" << std::endl ;
  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }


  oss << "# end: test `Detector Biasing'" << std::endl << std::endl ;

  // Append to all the summary files
  for(int i=0;i<fileNames_summary.size();i++){

    if (!outputfilestream.is_open()){ outputfilestream.open((fileNames_summary.at(i)).c_str(), std::ios::app) ; }
 
    if(outputfilestream.is_open()){

      outputfilestream << oss.str() ;
      outputfilestream.close() ;  
    }
  }
}

/*
 * Power off the HV 
 */
void vfat_controller::powerOffHV(){

  std::cout << SET_FORECOLOR_BLUE << "RAMPING DOWN THE \"HV\"" << SET_DEFAULT_COLORS << std::endl ;

  // Ramp down the HV
  try{

    int voltage = GPIB_HV_VOLTAGE ;

    GPIBDevice gp( GPIB_BOARD_INDEX, GPIB_HV_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;

    std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gp.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;

    do{

      voltage -= GPIB_HV_VOLTAGE_STEP ;
      std::stringstream cmd ;
      cmd << "sour:volt:imm:ampl " << voltage << " \n" ;

      gp.write(cmd.str()) ;

      msleep(GPIB_HV_DELAY) ;
    }
    while(voltage>0) ;

  }
  catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }

}
#endif

#endif
