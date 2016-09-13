/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#ifdef BUSUSBFEC

#include <stdlib.h> // Standard

#include <qcheckbox.h>  // GUI
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qtextedit.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qbuttongroup.h>

#include "vfat_controller.h" // SW header


void vfat_controller::standardChipTesting_pressed(){

  advancedSettings->hide() ;
}

void vfat_controller::fullI2Check_pressed(){

  advancedSettings->hide() ;
}

void vfat_controller::advancedChipTesting_pressed(){

  advancedSettings->show() ;
}

void vfat_controller::clearBarcodeList_pressed(){

  barcodeList->setText("") ;
}

void vfat_controller::tabWidget2_currentChanged( QWidget *qwidget ){

  barcodeList->setFocus() ;
}

void vfat_controller::T1TypeSel_pressed(){

  hybridTypeGroup->setTitle("Hybrid Type / Polarity") ;
  StripTypeSel->setText("Positive") ;
  PadTypeSel->setText("Negative") ;
  StripTypeSel->show() ;
  PadTypeSel->show() ;
  TriggerTypeSel->setChecked(true) ;
}

void vfat_controller::T2TypeSel_pressed(){

  hybridTypeGroup->setTitle("Hybrid Type") ;
  StripTypeSel->setText("Strip") ;
  PadTypeSel->setText("Pad") ;
  StripTypeSel->show() ;
  PadTypeSel->show() ;
  TriggerTypeSel->setChecked(true) ;
}

void vfat_controller::RPTypeSel_pressed(){

  hybridTypeGroup->setTitle("Hybrid Type") ;
  StripTypeSel->hide() ;
  PadTypeSel->hide() ;
  TriggerTypeSel->setChecked(true) ;
}


/** Start the thread
 */
void vfat_controller::startChipTest_pressed(){

  chipTesting_ = true ;

#ifdef DEBUGMODE
  std::cout << SET_FORECOLOR_CYAN << vfatAddresses_.size() << " VFATs connected to the system in total... testVfats_.size()=" << testVfats_.size() << SET_DEFAULT_COLORS << std::endl ;
#endif

  // Ask user if testing plain hybrids ( need to know for directory structure...)
  switch( QMessageBox::question( this, tr(""), tr("By chance is there a detector present?"),
				tr("&Yes"), tr("&No"), QString::null, 1, -2 )){
  case 0:
    detectorPresent_ = true ;
    break ;

  case 1:
    detectorPresent_ = false ;
    break ;

  default:
    return ;
  }

  QStringList list = QStringList::split("\n", barcodeList->text()) ;

  // Check if barcodes are entered
  if(detectorType_=="RP"){

    if(list.size()!=1){

      QMessageBox::critical( this, tr(""), tr("The number of hybrids to test and number of barcodes doesn't match. Care to check?"), QMessageBox::Abort, QMessageBox::NoButton ) ;
      return ;
    }
  }
  else{

    if(!advancedChipTesting->isChecked()){

      if(list.size()!=4){

	// There's no 4 barcodes -> Ask user if this is on purpose
	switch( QMessageBox::question( this, tr(""), tr("There's no 4 barcodes entered. Is this on purpose?"),
				       tr("&Yes"), tr("&No"), QString::null, 1, -2 )){
	case 0:
	  break ;

	case 1:
	  return ;

	default:
	  return ;
	}
      }
    }
    else{

      int count = 0 ;

      for(int i=0;i<vfatList_test->count();i++){

	if (vfatList_test->isSelected(i)){ count++ ; }
      }

      if((vfatAddresses_.size()!=list.size()) && (advancedChipTesting->isChecked() && count!=list.size())){

	QMessageBox::critical( this, tr(""), tr("The number of hybrids to test and number of barcodes doesn't match. Care to check?"), QMessageBox::Abort, QMessageBox::NoButton ) ;
	return ;
      }
    }
  }

  // Abort if name is not given
  if(personNameT->text()=="" || personNameT->text()=="Your Name"){

    QMessageBox::critical( this, tr(""), tr("Fill in your name so that Paul can track you down..."), QMessageBox::Abort, QMessageBox::NoButton ) ;
    return ;
  }

  // Check if $TTP_DATA defined
  const char* env = getenv("TTP_DATA") ;

  if(env==0){ 

    if(!advancedChipTesting->isChecked()){
      std::cout << SET_FORECOLOR_RED << "$TTP_DATA NOT DEFINED, ABORTING" << SET_DEFAULT_COLORS << std::endl ;
      return ;
    }
  }


#ifdef BUSUSBFEC

  if(!advancedChipTesting->isChecked()){
    
#ifdef USB_GPIB_PS
#ifndef USB_GPIB_PS_NO_POWER_ON_OFF

    // Power on

    try{

      std::cout << SET_FORECOLOR_BLUE << "POWERING ON THE PS..." << SET_DEFAULT_COLORS << std::endl ;

      GPIBDevice gpibDevice( GPIB_BOARD_INDEX, GPIB_PRIMARY_ADDR, GPIB_SECONDARY_ADDR, GPIB_TIMEOUT, GPIB_LAST_BYTE_MODE, GPIB_END_STRING_MODE ) ;
      std::cout << SET_FORECOLOR_CYAN << "GPIB DEVICE KEY USED:" << gpibDevice.getDeviceKey() << SET_DEFAULT_COLORS << std::endl ;
      //gpibDevice.setup(GPIB_VOLTAGE, GPIB_CURRENT) ;
      gpibDevice.powerOn() ;
      //gpibDevice.release() ;
      sleep(5) ; // Wait the PS to power on
    }
    catch(...){ std::cout << SET_FORECOLOR_RED << "Problem while using GPIB." << SET_DEFAULT_COLORS << std::endl ; }

#endif 
#endif    
    

    // Check if we've FEC access, if not release USB device and create new FEC access from scratch
    try{

      std::cout << SET_FORECOLOR_CYAN << "FEC SR0: " << std::hex << fecAccess_->getFecRingSR0(fecIndex_) << std::dec << SET_DEFAULT_COLORS << std::endl ;
    }
    catch(FecExceptionHandler &e) {

      std::cout << SET_FORECOLOR_CYAN << "COULDN'T READ FEC REGISTER. TRY CREATING NEW FEC ACCESS..." << SET_DEFAULT_COLORS << std::endl ;

      // Close USB FEC Device
      int ftStatus = fec_usb_close(0) ;
      if(ftStatus!=FECUSB_RETURN_OK) {

	std::cout << SET_FORECOLOR_RED << "Problem while closing FEC USB device, status " << ftStatus << SET_DEFAULT_COLORS << std::endl ;
	return ;
      }
      
      createFecAccess() ;
    }

    crepeMasterReset(0) ;

    // Try once again...
    try{

      std::cout << SET_FORECOLOR_CYAN << "FEC SR0: " << std::hex << fecAccess_->getFecRingSR0(fecIndex_) << std::dec << SET_DEFAULT_COLORS << std::endl ;
    }
    catch(FecExceptionHandler &e) {

      std::cout << SET_FORECOLOR_RED << "CREATING FEC ACCESS FAILED. ABORTING..." << SET_DEFAULT_COLORS << std::endl ;
      return ;
    }
  }

#endif


  if(vfatAddresses_.size()){

    //Start thread if not already running, otherwise terminate
    if(startChipTest->text()=="Start"){

      std::cout << SET_FORECOLOR_BLUE << "TESTING STARTED" << SET_DEFAULT_COLORS << std::endl ;
      startChipTest->setText("Test Running...") ;
      this->start() ;
      this->wait(THREAD_WAIT_TIME) ;
    }
    else{

      std::cout << SET_FORECOLOR_RED << "TESTING ABORTED" << SET_DEFAULT_COLORS << std::endl ;
      msleep(1000) ;
      this->terminate() ;
      this->wait(THREAD_WAIT_TIME) ;
      startChipTest->setText("Start") ;
      testCleanUp() ;
    }
  }
  else { ErrorMessage ("NO VFATS FOUND -> ABORTING..." ) ; }
}

#endif
