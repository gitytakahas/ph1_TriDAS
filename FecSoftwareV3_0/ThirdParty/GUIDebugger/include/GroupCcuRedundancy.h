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
#ifndef GroupCcuRedundancy_H
#define GroupCcuRedundancy_H

  // MAX CCU number per line
#define MAXCCULINE 4
  // Minimum length between 2 group of buttons
#define PIXELCC    5

#include <iostream.h>

  // CCU redundancy
#include <qbuttongroup.h> 
#include <qgroupbox.h> 
#include <qlayout.h>
#include <qpoint.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qlistbox.h>

#include "tscTypes.h"
#include "keyType.h"
#include "FecAccess.h"
#include "FecExceptionHandler.h"

class GroupCcuRedundancy: public QGroupBox {

  // For QT connect methods
  Q_OBJECT

 private:

  /** Access to the hardware
   */
  FecAccess *fecAccess_ ;

  /** Layout
   */
  QGridLayout *ccuRedundancyCCUGroupLayout_ ;

  /** Group of the button for the CCU input
   */
  QButtonGroup *inputButtonGroup_ ;

  /** Layout of the input button group
   */
  QVBoxLayout *inputButtonGroup_Layout ;

  /** Group of the button for the CCU output
   */
  QButtonGroup *outputButtonGroup_ ;

  /** Layout of the output button group
   */
  QVBoxLayout *outputButtonGroup_Layout ;

  /** Input A
   */
  QRadioButton *ccuRedundancyCcuOutputAButton_ ;

  /** Input B
   */
  QRadioButton *ccuRedundancyCcuOutputBButton_ ;

  /** Output A
   */
  QRadioButton *ccuRedundancyCcuInputAButton_ ;

  /** Output B
   */
  QRadioButton *ccuRedundancyCcuInputBButton_ ;

  /** Index of the CCU
   */
  keyType index_ ;

  /** Error message in a list
   */
  QListBox *errorTextEdit_ ;

 public:

  /** Create a group box for the redundancy
   * index - index of the CCU
   * pos - original position of the FEC output
   * position - CCU number
   * parent - parent
   * name - name
   */
  GroupCcuRedundancy ( FecAccess *fecAccess, keyType index, QPoint pos, int position, QListBox *errorTextEdit, QWidget * parent = 0, const char * name = 0 ):
    QGroupBox (parent, name) {

    // -----------------------------------------------------------------
    // Error message inside
    errorTextEdit_ = errorTextEdit ;

    // ----------------------------------------------
    fecAccess_ = fecAccess ;
    index_ = index ;

    // ----------------------------------------------
    setGeometry( QRect( 150, 20, 153, 114 ) );
    setColumnLayout(0, Qt::Vertical );
    layout()->setSpacing( 6 );
    layout()->setMargin( 11 );
    ccuRedundancyCCUGroupLayout_ = new QGridLayout( layout() );
    setAlignment( Qt::AlignTop );
    
    char msg[80] ;
    sprintf (msg, "CCU 0x%X", getCcuKey(index)) ;
    setTitle( msg ) ;
    
    // ----------------------------------------------
    inputButtonGroup_ = new QButtonGroup( this, "inputButtonGroup_" );
    inputButtonGroup_->setColumnLayout(0, Qt::Vertical );
    inputButtonGroup_Layout = new QVBoxLayout( inputButtonGroup_->layout() );
    inputButtonGroup_Layout->setAlignment( Qt::AlignTop );
    ccuRedundancyCCUGroupLayout_->addWidget( inputButtonGroup_, 0, 0 );
    inputButtonGroup_->setTitle( tr( "In" ) );

    outputButtonGroup_ = new QButtonGroup( this, "outputButtonGroup_" );
    outputButtonGroup_->setColumnLayout(0, Qt::Vertical );
    outputButtonGroup_Layout = new QVBoxLayout( outputButtonGroup_->layout() );
    outputButtonGroup_Layout->setAlignment( Qt::AlignTop );
    ccuRedundancyCCUGroupLayout_->addWidget( outputButtonGroup_, 0, 1 );
    outputButtonGroup_->setTitle( tr( "Out" ) );

    // ---------------------------------------------
    ccuRedundancyCcuInputAButton_ = new QRadioButton(inputButtonGroup_, "ccuRedundancyCcuInputAButton" );
    ccuRedundancyCcuInputAButton_->setChecked( TRUE );
    ccuRedundancyCcuInputAButton_->setText( "A" );
    inputButtonGroup_Layout->addWidget( ccuRedundancyCcuInputAButton_ ) ;

    ccuRedundancyCcuInputBButton_ = new QRadioButton( inputButtonGroup_, "ccuRedundancyCcuInputBButton" );
    ccuRedundancyCcuInputBButton_->setText( "B" );
    inputButtonGroup_Layout->addWidget( ccuRedundancyCcuInputBButton_ ) ;

    // ---------------------------------------------
    ccuRedundancyCcuOutputAButton_ = new QRadioButton( outputButtonGroup_, "ccuRedundancyCcuOutputAButton" );
    ccuRedundancyCcuOutputAButton_->setChecked( TRUE );
    ccuRedundancyCcuOutputAButton_->setText( "A" );
    outputButtonGroup_Layout->addWidget( ccuRedundancyCcuOutputAButton_ ) ;

    ccuRedundancyCcuOutputBButton_ = new QRadioButton( outputButtonGroup_, "ccuRedundancyCcuOutputBButton" );
    ccuRedundancyCcuOutputBButton_->setText( "B" );
    outputButtonGroup_Layout->addWidget( ccuRedundancyCcuOutputBButton_ ) ;

    // -----------------------------------------------------------------
    try {
      // Find the value of the CRC in order to put the correct settings
      uint CRC = fecAccess_->getCcuCRC (index) ;
      if (CRC & 0x1) {
	ccuRedundancyCcuInputAButton_->setChecked( FALSE );
	ccuRedundancyCcuInputBButton_->setChecked( TRUE );
      }
      else {
	ccuRedundancyCcuInputAButton_->setChecked( TRUE );
	ccuRedundancyCcuInputBButton_->setChecked( FALSE );
      }
      if (CRC & 0x2) {
	ccuRedundancyCcuOutputAButton_->setChecked( FALSE );
	ccuRedundancyCcuOutputBButton_->setChecked( TRUE );
      }
      else {
	ccuRedundancyCcuOutputAButton_->setChecked( TRUE );
	ccuRedundancyCcuOutputBButton_->setChecked( FALSE );
      }
    }
    catch (FecExceptionHandler &e) {

      char msg[80] ;
      sprintf (msg, "Reconfiguration of the input for CCU 0x%x", getCcuKey(index_)) ;
      ErrorMessage (msg, e) ;
    }

    // -----------------------------------------------
    // Change of the input or output
    connect( ccuRedundancyCcuInputAButton_,  SIGNAL( clicked() ), this, SLOT( redundancyChangeInput( ) ) );
    connect( ccuRedundancyCcuInputBButton_,  SIGNAL( clicked() ), this, SLOT( redundancyChangeInput( ) ) );
    connect( ccuRedundancyCcuOutputAButton_, SIGNAL( clicked() ), this, SLOT( redundancyChangeOutput( ) ) );
    connect( ccuRedundancyCcuOutputBButton_, SIGNAL( clicked() ), this, SLOT( redundancyChangeOutput( ) ) );

    // -----------------------------------------------
    // Move to correct place => max MAXCCU CCU after go to a new line
    int ccuN = position ;
    int ccuL = (int)(ccuN / MAXCCULINE) ; 
    int ccuC = ccuN % MAXCCULINE ;

    //if (ccuL == 0) pos.setX (pos.x() + width()) ; // + PIXELCC * 3) ;

    pos.setX(pos.x()+ PIXELCC + ccuC*width() + PIXELCC) ;
    pos.setY(pos.y()+ ccuL*height()) ;
    //if (ccuL != 0) pos.setX(pos.x()+width() + PIXELCC) ;
    move(pos) ;
  }


  /** Return the index of the current CCU
   * \return index - index of the CCU
   */
  keyType getIndex ( ) {

    return index_ ;
  }
  

  /** Set the value depending on the CRC given
   */
  void setCcuCRC ( uint CRC ) {

    if (CRC & CCU_CRC_ALTIN) {
      //ccuRedundancyCcuInputAButton_->setChecked( FALSE );
      ccuRedundancyCcuInputBButton_->setChecked( TRUE );
    }
    else {
      ccuRedundancyCcuInputAButton_->setChecked( TRUE );
      //ccuRedundancyCcuInputBButton_->setChecked( FALSE );
    }

    if (CRC & CCU_CRC_SSP) {
      //ccuRedundancyCcuOutputAButton_->setChecked( FALSE );
      ccuRedundancyCcuOutputBButton_->setChecked( TRUE );
    }
    else {
      ccuRedundancyCcuOutputAButton_->setChecked( TRUE );
      //ccuRedundancyCcuOutputBButton_->setChecked( FALSE );
    }
  }

  /** Descructor
   */
  ~GroupCcuRedundancy ( ) {

    delete ccuRedundancyCcuInputAButton_ ;
    delete ccuRedundancyCcuInputBButton_ ;
    delete ccuRedundancyCcuOutputAButton_ ;
    delete ccuRedundancyCcuOutputBButton_ ;
    delete ccuRedundancyCCUGroupLayout_ ;

    delete inputButtonGroup_Layout ;
    delete inputButtonGroup_ ;
    delete outputButtonGroup_Layout ;
    delete outputButtonGroup_ ;
  }

  /** After a PLX reset or a FEC reset
   */
  void resetMethod ( ) {

    ccuRedundancyCcuInputAButton_->setChecked(true) ;
    ccuRedundancyCcuInputBButton_->setChecked(false) ;
    ccuRedundancyCcuOutputAButton_->setChecked(true) ;
    ccuRedundancyCcuOutputBButton_->setChecked(false) ;
  }

  /** Build the value depends on the radio button
   * \see documentation on the CCU 25 about the control register C
   *   bit | Name  | Function
   *    0  | Altin | 1 => input B
   *                 0 => input A
   *    1  | SSP   | 1 => output B
   *                 0 => output A
   */
  tscType16 buildValue( ) {

    tscType16 value = 0 ;

    if (ccuRedundancyCcuInputBButton_->isChecked()) value |= 0x1 ;
    if (ccuRedundancyCcuOutputBButton_->isChecked()) value |= 0x2 ;

    return value ;
  }

  /**
   */
  void ErrorMessage (char *title, FecExceptionHandler e ) {

    char msg[1000] ;
    if (errorTextEdit_ != NULL) {

      sprintf (msg, "%s: %s", title, e.what().c_str()) ;
      errorTextEdit_->insertItem(msg) ;
    }
    else {
      char msg[1000] ;
      sprintf (msg, "<center><h4><font color=red>%s</font></h4></center><center><h4>%s</h4></center><p>Error Code %ld<br>", 
	       title, e.getErrorMessage().c_str(), e.getErrorCode()) ;
    
      char msg1[80] ;
      if (e.getPositionGiven()) {
	
	decodeKey ( msg1, e.getHardPosition() ) ;
	strcat (msg, "Position: ") ;
	strcat (msg, msg1) ;
	strcat (msg, "</p>") ;
      }

      QMessageBox::critical( parentWidget(), "FecDialog -- Exception", msg ) ;    
    }
  }

public slots:
  /** Change the input
   */
  void redundancyChangeInput( ) {

    try {

      fecAccess_->setCcuCRC (index_, buildValue()) ;
      //fecAccess_->setCcuCRC (index_, CCU_CRC_ALTIN) ;
    }
    catch (FecExceptionHandler &e) {

      char msg[80] ;
      sprintf (msg, "Reconfiguration of the input for CCU 0x%x", getCcuKey(index_)) ;
      ErrorMessage (msg, e) ;
    }
  }
  
  /** Change the output
   */
  void redundancyChangeOutput( ) {

    try {
      fecAccess_->setCcuCRC (index_, buildValue()) ;
    }
    catch (FecExceptionHandler &e) {

      char msg[80] ;
      sprintf (msg, "Reconfiguration of the output for CCU 0x%x", getCcuKey(index_)) ;
      ErrorMessage (msg, e) ;
    }    
  }

} ;

#endif
