/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include <sstream>  // Stringstream
#include <stdlib.h> // Standard

#include <qcombobox.h> // GUI
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qlabel.h>

#include "vfat_controller.h"    // SW Header

/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Control Panel                                                                     */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

// Methods to disable / enable fields

void vfat_controller::cr0Sel_pressed() {

  if (cr0Sel->isChecked()){
    cr0CalModeT->setDisabled(true) ;
    cr0CalPolT->setDisabled(true) ;
    cr0MSPolT->setDisabled(true) ;
    cr0TrigModeT->setDisabled(true) ;
    cr0RunT->setDisabled(true) ;
  }
  else {
    cr0CalModeT->setDisabled(false) ;
    cr0CalPolT->setDisabled(false) ;
    cr0MSPolT->setDisabled(false) ;
    cr0TrigModeT->setDisabled(false) ;
    cr0RunT->setDisabled(false) ;
  }
}

void vfat_controller::cr1Sel_pressed() {

  if (cr1Sel->isChecked()){
    cr1ReHitCTT->setDisabled(true) ;
    cr1LVDSPowerSaveT->setDisabled(true) ;
    cr1ProbeModeT->setDisabled(true) ;
    cr1DACSelT->setDisabled(true) ;
  }
  else {
    cr1ReHitCTT->setDisabled(false) ;
    cr1LVDSPowerSaveT->setDisabled(false) ;
    cr1ProbeModeT->setDisabled(false) ;
    cr1DACSelT->setDisabled(false) ;
  }
}

void vfat_controller::cr2Sel_pressed() {

  if (cr2Sel->isChecked()){
    cr2DigInSelT->setDisabled(true) ;
    cr2MSPulseLengthT->setDisabled(true) ;
    cr2HitCountSelT->setDisabled(true) ;
  }
  else {
    cr2DigInSelT->setDisabled(false) ;
    cr2MSPulseLengthT->setDisabled(false) ;
    cr2HitCountSelT->setDisabled(false) ;
  }
}

void vfat_controller::cr3Sel_pressed() {

  if (cr3Sel->isChecked()){
    cr3DFTestPatternT->setDisabled(true) ;
    cr3PbBGT->setDisabled(true) ;
    cr3TrimDACRangeT->setDisabled(true) ;
  }
  else {
    cr3DFTestPatternT->setDisabled(false) ;
    cr3PbBGT->setDisabled(false) ;
    cr3TrimDACRangeT->setDisabled(false) ;
  }
}

void vfat_controller::IPreampInSel_pressed() {

  if (IPreampInSel->isChecked()){ vfatIPreampInT->setDisabled(true) ; }
  else { vfatIPreampInT->setDisabled(false) ; }
}

void vfat_controller::IPreampFeedSel_pressed() {

  if (IPreampFeedSel->isChecked()){ vfatIPreampFeedT->setDisabled(true) ; }
  else { vfatIPreampFeedT->setDisabled(false) ; }
}

void vfat_controller::IPreampOutSel_pressed() {

  if (IPreampOutSel->isChecked()){ vfatIPreampOutT->setDisabled(true) ; }
  else { vfatIPreampOutT->setDisabled(false) ; }
}

void vfat_controller::IShaperSel_pressed() {

  if (IShaperSel->isChecked()){ vfatIShaperT->setDisabled(true) ; }
  else { vfatIShaperT->setDisabled(false) ; }
}

void vfat_controller::IShaperFeedSel_pressed() {

  if (IShaperFeedSel->isChecked()){ vfatIShaperFeedT->setDisabled(true) ; }
  else { vfatIShaperFeedT->setDisabled(false) ; }
}

void vfat_controller::ICompSel_pressed() {

  if (ICompSel->isChecked()){ vfatICompT->setDisabled(true) ; }
  else { vfatICompT->setDisabled(false) ; }
}

void vfat_controller::latencySel_pressed() {

  if (latencySel->isChecked()){ vfatLatencyT->setDisabled(true) ; }
  else { vfatLatencyT->setDisabled(false) ; }
}

void vfat_controller::VCalSel_pressed() {

  if (VCalSel->isChecked()){ vfatVCalT->setDisabled(true) ; }
  else { vfatVCalT->setDisabled(false) ; }
}

void vfat_controller::VThreshold1Sel_pressed() {

  if (VThreshold1Sel->isChecked()){ vfatVThreshold1T->setDisabled(true) ; }
  else { vfatVThreshold1T->setDisabled(false) ; }
}

void vfat_controller::VThreshold2Sel_pressed() {

  if (VThreshold2Sel->isChecked()){ vfatVThreshold2T->setDisabled(true) ; }
  else { vfatVThreshold2T->setDisabled(false) ; }
}

void vfat_controller::calPhaseSel_pressed() {

  if (calPhaseSel->isChecked()){ calPhaseT->setDisabled(true) ; }
  else { calPhaseT->setDisabled(false) ; }
}

void vfat_controller::chanRegSel_pressed() {

  if (chanRegSel->isChecked()){
    calAnalog->setDisabled(true) ;
    cal0->setDisabled(true) ;
    cal1->setDisabled(true) ;
    cal2->setDisabled(true) ;
    cal3->setDisabled(true) ;
    cal4->setDisabled(true) ;
    cal5->setDisabled(true) ;
    cal6->setDisabled(true) ;
    cal7->setDisabled(true) ;
    cal8->setDisabled(true) ;
    cal9->setDisabled(true) ;
    cal10->setDisabled(true) ;
    cal11->setDisabled(true) ;
    cal12->setDisabled(true) ;
    cal13->setDisabled(true) ;
    cal14->setDisabled(true) ;
    cal15->setDisabled(true) ;
    cal16->setDisabled(true) ;
    cal17->setDisabled(true) ;
    cal18->setDisabled(true) ;
    cal19->setDisabled(true) ;
    cal20->setDisabled(true) ;
    cal21->setDisabled(true) ;
    cal22->setDisabled(true) ;
    cal23->setDisabled(true) ;
    cal24->setDisabled(true) ;
    cal25->setDisabled(true) ;
    cal26->setDisabled(true) ;
    cal27->setDisabled(true) ;
    cal28->setDisabled(true) ;
    cal29->setDisabled(true) ;
    cal30->setDisabled(true) ;
    cal31->setDisabled(true) ;

    mask0->setDisabled(true) ;
    mask1->setDisabled(true) ;
    mask2->setDisabled(true) ;
    mask3->setDisabled(true) ;
    mask4->setDisabled(true) ;
    mask5->setDisabled(true) ;
    mask6->setDisabled(true) ;
    mask7->setDisabled(true) ;
    mask8->setDisabled(true) ;
    mask9->setDisabled(true) ;
    mask10->setDisabled(true) ;
    mask11->setDisabled(true) ;
    mask12->setDisabled(true) ;
    mask13->setDisabled(true) ;
    mask14->setDisabled(true) ;
    mask15->setDisabled(true) ;
    mask16->setDisabled(true) ;
    mask17->setDisabled(true) ;
    mask18->setDisabled(true) ;
    mask19->setDisabled(true) ;
    mask20->setDisabled(true) ;
    mask21->setDisabled(true) ;
    mask22->setDisabled(true) ;
    mask23->setDisabled(true) ;
    mask24->setDisabled(true) ;
    mask25->setDisabled(true) ;
    mask26->setDisabled(true) ;
    mask27->setDisabled(true) ;
    mask28->setDisabled(true) ;
    mask29->setDisabled(true) ;
    mask30->setDisabled(true) ;
    mask31->setDisabled(true) ;

    trimdac0->setDisabled(true) ;
    trimdac1->setDisabled(true) ;
    trimdac2->setDisabled(true) ;
    trimdac3->setDisabled(true) ;
    trimdac4->setDisabled(true) ;
    trimdac5->setDisabled(true) ;
    trimdac6->setDisabled(true) ;
    trimdac7->setDisabled(true) ;
    trimdac8->setDisabled(true) ;
    trimdac9->setDisabled(true) ;
    trimdac10->setDisabled(true) ;
    trimdac11->setDisabled(true) ;
    trimdac12->setDisabled(true) ;
    trimdac13->setDisabled(true) ;
    trimdac14->setDisabled(true) ;
    trimdac15->setDisabled(true) ;
    trimdac16->setDisabled(true) ;
    trimdac17->setDisabled(true) ;
    trimdac18->setDisabled(true) ;
    trimdac19->setDisabled(true) ;
    trimdac20->setDisabled(true) ;
    trimdac21->setDisabled(true) ;
    trimdac22->setDisabled(true) ;
    trimdac23->setDisabled(true) ;
    trimdac24->setDisabled(true) ;
    trimdac25->setDisabled(true) ;
    trimdac26->setDisabled(true) ;
    trimdac27->setDisabled(true) ;
    trimdac28->setDisabled(true) ;
    trimdac29->setDisabled(true) ;
    trimdac30->setDisabled(true) ;
    trimdac31->setDisabled(true) ;

    range1->setDisabled(true) ;
    range2->setDisabled(true) ;
    range3->setDisabled(true) ;
    range4->setDisabled(true) ;

    text1->setDisabled(true) ;
    text2->setDisabled(true) ;
    text3->setDisabled(true) ;
    text4->setDisabled(true) ;
    text5->setDisabled(true) ;
    text6->setDisabled(true) ;
    text7->setDisabled(true) ;
    text8->setDisabled(true) ;

    label->setDisabled(true) ;
    label0->setDisabled(true) ;
    label1->setDisabled(true) ;
    label2->setDisabled(true) ;
    label3->setDisabled(true) ;
    label4->setDisabled(true) ;
    label5->setDisabled(true) ;
    label6->setDisabled(true) ;
    label7->setDisabled(true) ;
    label8->setDisabled(true) ;
    label9->setDisabled(true) ;
    label10->setDisabled(true) ;
    label11->setDisabled(true) ;
    label12->setDisabled(true) ;
    label13->setDisabled(true) ;
    label14->setDisabled(true) ;
    label15->setDisabled(true) ;
    label16->setDisabled(true) ;
    label17->setDisabled(true) ;
    label18->setDisabled(true) ;
    label19->setDisabled(true) ;
    label20->setDisabled(true) ;
    label21->setDisabled(true) ;
    label22->setDisabled(true) ;
    label23->setDisabled(true) ;
    label24->setDisabled(true) ;
    label25->setDisabled(true) ;
    label26->setDisabled(true) ;
    label27->setDisabled(true) ;
    label28->setDisabled(true) ;
    label29->setDisabled(true) ;
    label30->setDisabled(true) ;
    label31->setDisabled(true) ;

  }
  else {

    calAnalog->setDisabled(false) ;
    cal0->setDisabled(false) ;
    cal1->setDisabled(false) ;
    cal2->setDisabled(false) ;
    cal3->setDisabled(false) ;
    cal4->setDisabled(false) ;
    cal5->setDisabled(false) ;
    cal6->setDisabled(false) ;
    cal7->setDisabled(false) ;
    cal8->setDisabled(false) ;
    cal9->setDisabled(false) ;
    cal10->setDisabled(false) ;
    cal11->setDisabled(false) ;
    cal12->setDisabled(false) ;
    cal13->setDisabled(false) ;
    cal14->setDisabled(false) ;
    cal15->setDisabled(false) ;
    cal16->setDisabled(false) ;
    cal17->setDisabled(false) ;
    cal18->setDisabled(false) ;
    cal19->setDisabled(false) ;
    cal20->setDisabled(false) ;
    cal21->setDisabled(false) ;
    cal22->setDisabled(false) ;
    cal23->setDisabled(false) ;
    cal24->setDisabled(false) ;
    cal25->setDisabled(false) ;
    cal26->setDisabled(false) ;
    cal27->setDisabled(false) ;
    cal28->setDisabled(false) ;
    cal29->setDisabled(false) ;
    cal30->setDisabled(false) ;
    cal31->setDisabled(false) ;

    mask0->setDisabled(false) ;
    mask1->setDisabled(false) ;
    mask2->setDisabled(false) ;
    mask3->setDisabled(false) ;
    mask4->setDisabled(false) ;
    mask5->setDisabled(false) ;
    mask6->setDisabled(false) ;
    mask7->setDisabled(false) ;
    mask8->setDisabled(false) ;
    mask9->setDisabled(false) ;
    mask10->setDisabled(false) ;
    mask11->setDisabled(false) ;
    mask12->setDisabled(false) ;
    mask13->setDisabled(false) ;
    mask14->setDisabled(false) ;
    mask15->setDisabled(false) ;
    mask16->setDisabled(false) ;
    mask17->setDisabled(false) ;
    mask18->setDisabled(false) ;
    mask19->setDisabled(false) ;
    mask20->setDisabled(false) ;
    mask21->setDisabled(false) ;
    mask22->setDisabled(false) ;
    mask23->setDisabled(false) ;
    mask24->setDisabled(false) ;
    mask25->setDisabled(false) ;
    mask26->setDisabled(false) ;
    mask27->setDisabled(false) ;
    mask28->setDisabled(false) ;
    mask29->setDisabled(false) ;
    mask30->setDisabled(false) ;
    mask31->setDisabled(false) ;

    trimdac0->setDisabled(false) ;
    trimdac1->setDisabled(false) ;
    trimdac2->setDisabled(false) ;
    trimdac3->setDisabled(false) ;
    trimdac4->setDisabled(false) ;
    trimdac5->setDisabled(false) ;
    trimdac6->setDisabled(false) ;
    trimdac7->setDisabled(false) ;
    trimdac8->setDisabled(false) ;
    trimdac9->setDisabled(false) ;
    trimdac10->setDisabled(false) ;
    trimdac11->setDisabled(false) ;
    trimdac12->setDisabled(false) ;
    trimdac13->setDisabled(false) ;
    trimdac14->setDisabled(false) ;
    trimdac15->setDisabled(false) ;
    trimdac16->setDisabled(false) ;
    trimdac17->setDisabled(false) ;
    trimdac18->setDisabled(false) ;
    trimdac19->setDisabled(false) ;
    trimdac20->setDisabled(false) ;
    trimdac21->setDisabled(false) ;
    trimdac22->setDisabled(false) ;
    trimdac23->setDisabled(false) ;
    trimdac24->setDisabled(false) ;
    trimdac25->setDisabled(false) ;
    trimdac26->setDisabled(false) ;
    trimdac27->setDisabled(false) ;
    trimdac28->setDisabled(false) ;
    trimdac29->setDisabled(false) ;
    trimdac30->setDisabled(false) ;
    trimdac31->setDisabled(false) ;

    range1->setDisabled(false) ;
    range2->setDisabled(false) ;
    range3->setDisabled(false) ;
    range4->setDisabled(false) ;

    text1->setDisabled(false) ;
    text2->setDisabled(false) ;
    text3->setDisabled(false) ;
    text4->setDisabled(false) ;
    text5->setDisabled(false) ;
    text6->setDisabled(false) ;
    text7->setDisabled(false) ;
    text8->setDisabled(false) ;

    label->setDisabled(false) ;
    label0->setDisabled(false) ;
    label1->setDisabled(false) ;
    label2->setDisabled(false) ;
    label3->setDisabled(false) ;
    label4->setDisabled(false) ;
    label5->setDisabled(false) ;
    label6->setDisabled(false) ;
    label7->setDisabled(false) ;
    label8->setDisabled(false) ;
    label9->setDisabled(false) ;
    label10->setDisabled(false) ;
    label11->setDisabled(false) ;
    label12->setDisabled(false) ;
    label13->setDisabled(false) ;
    label14->setDisabled(false) ;
    label15->setDisabled(false) ;
    label16->setDisabled(false) ;
    label17->setDisabled(false) ;
    label18->setDisabled(false) ;
    label19->setDisabled(false) ;
    label20->setDisabled(false) ;
    label21->setDisabled(false) ;
    label22->setDisabled(false) ;
    label23->setDisabled(false) ;
    label24->setDisabled(false) ;
    label25->setDisabled(false) ;
    label26->setDisabled(false) ;
    label27->setDisabled(false) ;
    label28->setDisabled(false) ;
    label29->setDisabled(false) ;
    label30->setDisabled(false) ;
    label31->setDisabled(false) ;
  }
}

void vfat_controller::selectAllReg_pressed() {

  if (!cr0Sel->isChecked()){
    cr0Sel_pressed() ;
    cr0Sel->setChecked(true) ;
  }
  if (!cr1Sel->isChecked()){
    cr1Sel_pressed() ;
    cr1Sel->setChecked(true) ;
  }
  if (!cr2Sel->isChecked()){
    cr2Sel_pressed() ;
    cr2Sel->setChecked(true) ;
  }
  if (!cr3Sel->isChecked()){
    cr3Sel_pressed() ;
    cr3Sel->setChecked(true) ;
  }
  if (!IPreampInSel->isChecked()){
    IPreampInSel_pressed() ;
    IPreampInSel->setChecked(true) ;
  }
  if (!IPreampFeedSel->isChecked()){
    IPreampFeedSel_pressed() ;
    IPreampFeedSel->setChecked(true) ;
  }
  if (!IPreampOutSel->isChecked()){
    IPreampOutSel_pressed() ;
    IPreampOutSel->setChecked(true) ;
  }
  if (!IShaperSel->isChecked()){
    IShaperSel_pressed() ;
    IShaperSel->setChecked(true) ;
  }
  if (!IShaperFeedSel->isChecked()){
    IShaperFeedSel_pressed() ;
    IShaperFeedSel->setChecked(true) ;
  }
  if (!ICompSel->isChecked()){
    ICompSel_pressed() ;
    ICompSel->setChecked(true) ;
  }
  if (!latencySel->isChecked()){
    latencySel_pressed() ;
    latencySel->setChecked(true) ;
  }
  if (!VCalSel->isChecked()){
    VCalSel_pressed() ;
    VCalSel->setChecked(true) ;
  }
  if (!VThreshold1Sel->isChecked()){
    VThreshold1Sel_pressed() ;
    VThreshold1Sel->setChecked(true) ;
  }
  if (!VThreshold2Sel->isChecked()){
    VThreshold2Sel_pressed() ;
    VThreshold2Sel->setChecked(true) ;
  }
  if (!calPhaseSel->isChecked()){
    calPhaseSel_pressed() ;
    calPhaseSel->setChecked(true) ;
  }
  if (!chanRegSel->isChecked()){  
    chanRegSel_pressed() ;
    chanRegSel->setChecked(true) ;
  }
}

void vfat_controller::selectNoneReg_pressed() {

  if (cr0Sel->isChecked()){
    cr0Sel_pressed() ;
    cr0Sel->setChecked(false) ;
  }
  if (cr1Sel->isChecked()){
    cr1Sel_pressed() ;
    cr1Sel->setChecked(false) ;
  }
  if (cr2Sel->isChecked()){
    cr2Sel_pressed() ;
    cr2Sel->setChecked(false) ;
  }
  if (cr3Sel->isChecked()){
    cr3Sel_pressed() ;
    cr3Sel->setChecked(false) ;
  }
  if (IPreampInSel->isChecked()){
    IPreampInSel_pressed() ;
    IPreampInSel->setChecked(false) ;
  }
  if (IPreampFeedSel->isChecked()){
    IPreampFeedSel_pressed() ;
    IPreampFeedSel->setChecked(false) ;
  }
  if (IPreampOutSel->isChecked()){
    IPreampOutSel_pressed() ;
    IPreampOutSel->setChecked(false) ;
  }
  if (IShaperSel->isChecked()){
    IShaperSel_pressed() ;
    IShaperSel->setChecked(false) ;
  }
  if (IShaperFeedSel->isChecked()){
    IShaperFeedSel_pressed() ;
    IShaperFeedSel->setChecked(false) ;
  }
  if (ICompSel->isChecked()){
    ICompSel_pressed() ;
    ICompSel->setChecked(false) ;
  }
  if (latencySel->isChecked()){
    latencySel_pressed() ;
    latencySel->setChecked(false) ;
  }
  if (VCalSel->isChecked()){
    VCalSel_pressed() ;
    VCalSel->setChecked(false) ;
  }
  if (VThreshold1Sel->isChecked()){
    VThreshold1Sel_pressed() ;
    VThreshold1Sel->setChecked(false) ;
  }
  if (VThreshold2Sel->isChecked()){
    VThreshold2Sel_pressed() ;
    VThreshold2Sel->setChecked(false) ;
  }
  if (calPhaseSel->isChecked()){
    calPhaseSel_pressed() ;
    calPhaseSel->setChecked(false) ;
  }
  if (chanRegSel->isChecked()){  
    chanRegSel_pressed() ;
    chanRegSel->setChecked(false) ;
  }
}

// Methods to check the field's input when focus is lost

void vfat_controller::vfatIPreampInT_lostFocus() {

  if (atoi(vfatIPreampInT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampInT->text())<=0){  
    if (atoi(vfatIPreampInT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatIPreampInT->setText("0") ;
      vfatIPreampInT->setFocus() ;
    }
    else { vfatIPreampInT->setText("0") ; }
  }
}

void vfat_controller::vfatIPreampFeedT_lostFocus() {

  if (atoi(vfatIPreampFeedT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampFeedT->text())<=0){  
    if (atoi(vfatIPreampFeedT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatIPreampFeedT->setText("0") ;
      vfatIPreampFeedT->setFocus() ;
    }
    else { vfatIPreampFeedT->setText("0") ; }
  }
}

void vfat_controller::vfatIPreampOutT_lostFocus() {

  if (atoi(vfatIPreampOutT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampOutT->text())<=0){  
    if (atoi(vfatIPreampOutT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatIPreampOutT->setText("0") ;
      vfatIPreampOutT->setFocus() ;
    }
    else { vfatIPreampOutT->setText("0") ; }
  }
}

void vfat_controller::vfatIShaperT_lostFocus() {

  if (atoi(vfatIShaperT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIShaperT->text())<=0){  
    if (atoi(vfatIShaperT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatIShaperT->setText("0") ;
      vfatIShaperT->setFocus() ;
    }
    else { vfatIShaperT->setText("0") ; }
  }
}

void vfat_controller::vfatIShaperFeedT_lostFocus() {

  if (atoi(vfatIShaperFeedT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIShaperFeedT->text())<=0){  
    if (atoi(vfatIShaperFeedT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatIShaperFeedT->setText("0") ;
      vfatIShaperFeedT->setFocus() ;
    }
    else { vfatIShaperFeedT->setText("0") ; }
  }
}

void vfat_controller::vfatICompT_lostFocus() {

  if (atoi(vfatICompT->text())>VFAT_REG_VALUE_MAX || atoi(vfatICompT->text())<=0){  
    if (atoi(vfatICompT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatICompT->setText("0") ;
      vfatICompT->setFocus() ;
    }
    else { vfatICompT->setText("0") ; }
  }
}

void vfat_controller::vfatLatencyT_lostFocus() {

  if (atoi(vfatLatencyT->text())>VFAT_REG_VALUE_MAX || atoi(vfatLatencyT->text())<=0){  
    if (atoi(vfatLatencyT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatLatencyT->setText("0") ;
      vfatLatencyT->setFocus() ;
    }
    else { vfatLatencyT->setText("0") ; }
  }
}

void vfat_controller::vfatVCalT_lostFocus() {

  if (atoi(vfatVCalT->text())>VFAT_REG_VALUE_MAX || atoi(vfatVCalT->text())<=0){  
    if (atoi(vfatVCalT->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatVCalT->setText("0") ;
      vfatVCalT->setFocus() ;
    }
    else { vfatVCalT->setText("0") ; }
  }
}

void vfat_controller::vfatVThreshold1T_lostFocus() {

  if (atoi(vfatVThreshold1T->text())>VFAT_REG_VALUE_MAX || atoi(vfatVThreshold1T->text())<=0){  
    if (atoi(vfatVThreshold1T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatVThreshold1T->setText("0") ;
      vfatVThreshold1T->setFocus() ;
    }
    else { vfatVThreshold1T->setText("0") ; }
  }
}

void vfat_controller::vfatVThreshold2T_lostFocus() {

  if (atoi(vfatVThreshold2T->text())>VFAT_REG_VALUE_MAX || atoi(vfatVThreshold2T->text())<=0){  
    if (atoi(vfatVThreshold2T->text())!=0){
      ErrorMessage("Value is not correct, must be 0-255.");
      vfatVThreshold2T->setText("0") ;
      vfatVThreshold2T->setFocus() ;
    }
    else { vfatVThreshold2T->setText("0") ; }
  }
}

bool vfat_controller::checkControlPanelValues(){

  if (atoi(vfatIPreampInT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampInT->text())<=0 || 
      atoi(vfatIPreampFeedT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampFeedT->text())<=0 ||
      atoi(vfatIPreampOutT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIPreampOutT->text())<=0 ||
      atoi(vfatIShaperT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIShaperT->text())<=0 ||
      atoi(vfatIShaperFeedT->text())>VFAT_REG_VALUE_MAX || atoi(vfatIShaperFeedT->text())<=0 || 
      atoi(vfatICompT->text())>VFAT_REG_VALUE_MAX || atoi(vfatICompT->text())<=0 ||
      atoi(vfatLatencyT->text())>VFAT_REG_VALUE_MAX || atoi(vfatLatencyT->text())<=0 ||
      atoi(vfatVCalT->text())>VFAT_REG_VALUE_MAX || atoi(vfatVCalT->text())<=0 ||
      atoi(vfatVThreshold1T->text())>VFAT_REG_VALUE_MAX || atoi(vfatVThreshold1T->text())<=0 || 
      atoi(vfatVThreshold2T->text())>VFAT_REG_VALUE_MAX || atoi(vfatVThreshold2T->text())<=0 ) {

    return false ;
  }

  return true ;
}

/** \brief Return the VFAT control register value corresponding the comboboxes
 * Return the VFAT control register value corresponding the comboboxes
 * \return the value
 */
uint vfat_controller::getCr0() {

  uint calmode, calpol, mspol, trigmode, run ;
  int iCr0Value = 0 ;

  calmode = cr0CalModeT->currentItem() ;
  calpol = cr0CalPolT->currentItem() ;
  mspol = cr0MSPolT->currentItem() ;
  trigmode = cr0TrigModeT->currentItem() ;
  run = cr0RunT->currentItem() ;

  if (calmode==1){ calmode = 64 ; }
  else if (calmode==2){ calmode = 128 ; }
  else if (calmode==3){ calmode = 192 ; }
  if (calpol==1){ calpol = 32 ; }
  if (mspol==1){ mspol = 16 ; }
  if (trigmode==1){ trigmode = 2 ; }
  else if (trigmode==2){ trigmode = 4 ; }
  else if (trigmode==3){ trigmode = 6 ; }
  else if (trigmode==4){ trigmode = 8 ; }

  iCr0Value = calmode + calpol + mspol + trigmode + run ;

  return iCr0Value ;
}

/** \brief Return the VFAT control register value corresponding the comboboxes
 * Return the VFAT control register value corresponding the comboboxes
 * \return the value
 */
uint vfat_controller::getCr1() {

  uint rehitct, lvdspowersave, probemode, dacsel ;
  int iCr1Value = 0 ;

  rehitct = cr1ReHitCTT->currentItem() ;
  lvdspowersave = cr1LVDSPowerSaveT->currentItem() ;
  probemode = cr1ProbeModeT->currentItem() ;
  dacsel = cr1DACSelT->currentItem() ;

  if (rehitct==1){ rehitct = 64 ; }
  else if (rehitct==2){ rehitct = 128 ; }
  else if (rehitct==3){ rehitct = 192 ; }
  if (lvdspowersave==1){ lvdspowersave = 32 ; }
  if (probemode==1){ probemode = 16 ; }

  iCr1Value = rehitct + lvdspowersave + probemode + dacsel ;

  return iCr1Value ;
}

/** \brief Return the VFAT control register value corresponding the comboboxes
 * Return the VFAT control register value corresponding the comboboxes
 * \return the value
 */
uint vfat_controller::getCr2() {

  uint diginsel, mspulselength, hitcountsel ;
  int iCr2Value = 0 ;

  diginsel = cr2DigInSelT->currentItem() ;
  mspulselength = cr2MSPulseLengthT->currentItem() ;
  hitcountsel = cr2HitCountSelT->currentItem() ;

  if (diginsel==1){ diginsel = 128 ; }
  if (mspulselength==1){ mspulselength = 16 ; }
  else if (mspulselength==2){ mspulselength = 32 ; }
  else if (mspulselength==3){ mspulselength = 48 ; }
  else if (mspulselength==4){ mspulselength = 64 ; }
  else if (mspulselength==5){ mspulselength = 80 ; }
  else if (mspulselength==6){ mspulselength = 96 ; }
  else if (mspulselength==7){ mspulselength = 112 ; }

  iCr2Value = diginsel + mspulselength + hitcountsel ;

  return iCr2Value ;
}

/** \brief Return the VFAT control register value corresponding the comboboxes
 * Return the VFAT control register value corresponding the comboboxes
 * \return the value
 */
uint vfat_controller::getCr3() {

  uint dftestpattern, pbbg, trimdacrange ;
  int iCr3Value = 0 ;

  dftestpattern = cr3DFTestPatternT->currentItem() ;
  pbbg = cr3PbBGT->currentItem() ;
  trimdacrange = cr3TrimDACRangeT->currentItem() ;

  if (dftestpattern==1){ dftestpattern=16 ; }
  if (pbbg==1){ pbbg=8 ; }

  iCr3Value = dftestpattern + pbbg + trimdacrange ;

  return iCr3Value ;
}

/** \brief Set control register's fields according to the input value
 * Set control register's fields according to the input value
 * \param ivalue - register value
 */
void vfat_controller::setCr0( uint ivalue ) {

  if (ivalue>=64){
    if(ivalue>=192){ 
      cr0CalModeT->setCurrentItem(3) ; 
      ivalue-=192 ;
    }
    else if(ivalue>=128){
      cr0CalModeT->setCurrentItem(2) ;
      ivalue-=128 ;
    }
    else if(ivalue>=64){
      cr0CalModeT->setCurrentItem(1) ;
      ivalue-=64 ;
    }
  }
  else{ cr0CalModeT->setCurrentItem(0) ; }
  if (ivalue>=32){
    cr0CalPolT->setCurrentItem(1) ; 
    ivalue-=32 ;
  }
  else{ cr0CalPolT->setCurrentItem(0) ; }
  if (ivalue>=16){
    cr0MSPolT->setCurrentItem(1) ; 
    ivalue-=16 ;
  }
  else{ cr0MSPolT->setCurrentItem(0) ; }
  if (ivalue>=2){
    if(ivalue>=8){ 
      cr0TrigModeT->setCurrentItem(4) ; 
      ivalue-=8 ;
    }
    else if(ivalue>=6){
      cr0TrigModeT->setCurrentItem(3) ;
      ivalue-=6 ;
    }
    else if(ivalue>=4){
      cr0TrigModeT->setCurrentItem(2) ;
      ivalue-=4 ;
    }
    else if(ivalue>=2){
      cr0TrigModeT->setCurrentItem(1) ;
      ivalue-=2 ;
    }
  }
  else{ cr0TrigModeT->setCurrentItem(0) ; }
  if (ivalue>=1){
    cr0RunT->setCurrentItem(1) ; 
    ivalue-=1 ;
  }
  else{ cr0RunT->setCurrentItem(0) ; } 
}

/** \brief Set control register's fields according to the input value
 * Set control register's fields according to the input value
 * \param ivalue - register value
 */
void vfat_controller::setCr1( uint ivalue ) {

  if (ivalue>=64){
    if(ivalue>=192){ 
      cr1ReHitCTT->setCurrentItem(3) ; 
      ivalue-=192 ;
    }
    else if(ivalue>=128){
      cr1ReHitCTT->setCurrentItem(2) ;
      ivalue-=128 ;
    }
    else if(ivalue>=64){
      cr1ReHitCTT->setCurrentItem(1) ;
      ivalue-=64 ;
    }
  }
  else{ cr1ReHitCTT->setCurrentItem(0) ; }
  if (ivalue>=32){
    cr1LVDSPowerSaveT->setCurrentItem(1) ; 
    ivalue-=32 ;
  }
  else{ cr1LVDSPowerSaveT->setCurrentItem(0) ; }
  if (ivalue>=16){
    cr1ProbeModeT->setCurrentItem(1) ; 
    ivalue-=16 ;
  }
  else{ cr1ProbeModeT->setCurrentItem(0) ; }
  if (ivalue>=1){
    if (ivalue>12){
      ivalue=12 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected DACSel Value." << SET_DEFAULT_COLORS << std::endl ;
    }
    cr1DACSelT->setCurrentItem(ivalue) ; 
    ivalue=0 ;
  }
  else{ cr1DACSelT->setCurrentItem(0) ; }     
}

/** \brief Set control register's fields according to the input value
 * Set control register's fields according to the input value
 * \param ivalue - register value
 */
void vfat_controller::setCr2( uint ivalue ) {

  if (ivalue>=128){
    cr2DigInSelT->setCurrentItem(1) ; 
    ivalue-=128 ;
  }
  else{ cr2DigInSelT->setCurrentItem(0) ; }
  if (ivalue>=16){
    if(ivalue>=112){ 
      cr2MSPulseLengthT->setCurrentItem(7) ; 
      ivalue-=112 ;
    }
    else if(ivalue>=96){
      cr2MSPulseLengthT->setCurrentItem(6) ;
      ivalue-=96 ;
    }
    else if(ivalue>=80){
      cr2MSPulseLengthT->setCurrentItem(5) ;
      ivalue-=80 ;
    }
    else if(ivalue>=64){
      cr2MSPulseLengthT->setCurrentItem(4) ;
      ivalue-=64 ;
    }
    else if(ivalue>=48){
      cr2MSPulseLengthT->setCurrentItem(3) ;
      ivalue-=48 ;
    }
    else if(ivalue>=32){
      cr2MSPulseLengthT->setCurrentItem(2) ;
      ivalue-=32 ;
    }
    else if(ivalue>=16){
      cr2MSPulseLengthT->setCurrentItem(1) ;
      ivalue-=16 ;
    }
  }
  else{ cr2MSPulseLengthT->setCurrentItem(0) ; }
  if (ivalue>=1){
    if (ivalue>8){
      ivalue=8 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected HitCountSel Value." << SET_DEFAULT_COLORS << std::endl ;
    }
    cr2HitCountSelT->setCurrentItem(ivalue) ; 
    ivalue=0 ;
  }
  else{ cr2HitCountSelT->setCurrentItem(0) ; }     
}

/** \brief Set control register's fields according to the input value
 * Set control register's fields according to the input value
 * \param ivalue - register value
 */
void vfat_controller::setCr3( uint ivalue ) {

  if (ivalue>=16){
    cr3DFTestPatternT->setCurrentItem(1) ; 
    ivalue-=16 ;
  }
  else{ cr3DFTestPatternT->setCurrentItem(0) ; }
  if (ivalue>=8){
    cr3PbBGT->setCurrentItem(1) ; 
    ivalue-=8 ;
  }
  else{ cr3PbBGT->setCurrentItem(0) ; }
  if (ivalue>=1){
    cr3TrimDACRangeT->setCurrentItem(ivalue) ; 
    ivalue=0 ;
  }
  else{ cr3TrimDACRangeT->setCurrentItem(0) ; }
}

/** \brief Return the VFAT CalPhase register value corresponding the combobox
 * Return the VFAT CalPhase register value corresponding the combobox
 * \return the value
 */
uint vfat_controller::getCalPhase() {

  uint calphase ;
  calphase = calPhaseT->currentItem() ;

  if (calphase==2){ calphase =VFAT_CALPHASE_90  ; }
  else if (calphase==3){ calphase = VFAT_CALPHASE_135 ; }
  else if (calphase==4){ calphase = VFAT_CALPHASE_180 ; }
  else if (calphase==5){ calphase = VFAT_CALPHASE_225 ; }
  else if (calphase==6){ calphase = VFAT_CALPHASE_270 ; }
  else if (calphase==7){ calphase = VFAT_CALPHASE_315 ; }

  return calphase ;
}

/** \brief Set CalPhase register's field according to the input value
 * Set CalPhase register's field according to the input value
 * \param ivalue - register value
 */
void vfat_controller::setCalPhase( uint ivalue ) {

  if (ivalue==127){ calPhaseT->setCurrentItem(7) ; }
  else if (ivalue==63){ calPhaseT->setCurrentItem(6) ; }
  else if (ivalue==31){ calPhaseT->setCurrentItem(5) ; }
  else if (ivalue==15){ calPhaseT->setCurrentItem(4) ; }
  else if (ivalue==7){ calPhaseT->setCurrentItem(3) ; }
  else if (ivalue==3){ calPhaseT->setCurrentItem(2) ; }
  else if (ivalue==1){ calPhaseT->setCurrentItem(1) ; }
  else if (ivalue==0){ calPhaseT->setCurrentItem(0) ; }
  else {  std::cout << SET_FORECOLOR_RED << std::dec << "Unexpected CalPhase Value." << ivalue << SET_DEFAULT_COLORS << std::endl ;}

}

// Channel Registers

/** \brief Fill the channel register fields according to the channel register value
 * Fill the channel register fields according to the channel register value
 */
void vfat_controller::setChannelRegisters( int page ){

  // Fill the channel register fields according to the channel register value

  std::stringstream svalue ; // trimdac value
  uint ivalue=0, i=0 ;           // interger value of current channel register

  // Change index according to the page to be shown
  if (page==1){ i = 1 ; }
  else if (page==2){ i = 33 ; }
  else if (page==3){ i = 65 ; }
  else if (page==4){ i = 97 ; }
  else { std::cout << SET_FORECOLOR_RED << "Channel range (" << page << ") incorrect" << SET_DEFAULT_COLORS << std::endl ; return ; }

  ivalue = getChReg(i) ;

  // Update analog channel calibration only with 1st group  
  if (i==1){
    if (ivalue>=128){
      calAnalog->setChecked(true) ;
      ivalue-=128 ;
    }
    else{ calAnalog->setChecked(false) ; }
  }

  if (ivalue>=64){
    cal0->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal0->setChecked(false) ; }
  if (ivalue>=32){
    mask0->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask0->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac0->setText(svalue.str()) ; 
  }
  else{ trimdac0->setText("0") ; }
  svalue.str("") ;
  ivalue = getChReg(i+1) ;
  if (ivalue>=64){
    cal1->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal1->setChecked(false) ; }
  if (ivalue>=32){
    mask1->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask1->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac1->setText(svalue.str()) ; 
  }
  else{ trimdac1->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+2) ;
  if (ivalue>=64){
    cal2->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal2->setChecked(false) ; }
  if (ivalue>=32){
    mask2->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask2->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac2->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac2->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+3) ;
  if (ivalue>=64){
    cal3->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal3->setChecked(false) ; }
  if (ivalue>=32){
    mask3->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask3->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac3->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac3->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+4) ;
  if (ivalue>=64){
    cal4->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal4->setChecked(false) ; }
  if (ivalue>=32){
    mask4->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask4->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac4->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac4->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+5) ;
  if (ivalue>=64){
    cal5->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal5->setChecked(false) ; }
  if (ivalue>=32){
    mask5->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask5->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac5->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac5->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+6) ;
  if (ivalue>=64){
    cal6->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal6->setChecked(false) ; }
  if (ivalue>=32){
    mask6->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask6->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac6->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac6->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+7) ;
  if (ivalue>=64){
    cal7->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal7->setChecked(false) ; }
  if (ivalue>=32){
    mask7->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask7->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac7->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac7->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+8) ;
  if (ivalue>=64){
    cal8->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal8->setChecked(false) ; }
  if (ivalue>=32){
    mask8->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask8->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac8->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac8->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+9) ;
  if (ivalue>=64){
    cal9->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal9->setChecked(false) ; }
  if (ivalue>=32){
    mask9->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask9->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac9->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac9->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+10) ;
  if (ivalue>=64){
    cal10->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal10->setChecked(false) ; }
  if (ivalue>=32){
    mask10->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask10->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac10->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac10->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+11) ;
  if (ivalue>=64){
    cal11->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal11->setChecked(false) ; }
  if (ivalue>=32){
    mask11->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask11->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac11->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac11->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+12) ;
  if (ivalue>=64){
    cal12->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal12->setChecked(false) ; }
  if (ivalue>=32){
    mask12->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask12->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac12->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac12->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+13) ;
  if (ivalue>=64){
    cal13->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal13->setChecked(false) ; }
  if (ivalue>=32){
    mask13->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask13->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac13->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac13->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+14) ;
  if (ivalue>=64){
    cal14->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal14->setChecked(false) ; }
  if (ivalue>=32){
    mask14->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask14->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac14->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac14->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+15) ;
  if (ivalue>=64){
    cal15->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal15->setChecked(false) ; }
  if (ivalue>=32){
    mask15->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask15->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac15->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac15->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+16) ;
  if (ivalue>=64){
    cal16->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal16->setChecked(false) ; }
  if (ivalue>=32){
    mask16->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask16->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac16->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac16->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+17) ;
  if (ivalue>=64){
    cal17->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal17->setChecked(false) ; }
  if (ivalue>=32){
    mask17->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask17->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac17->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac17->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+18) ;
  if (ivalue>=64){
    cal18->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal18->setChecked(false) ; }
  if (ivalue>=32){
    mask18->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask18->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac18->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac18->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+19) ;
  if (ivalue>=64){
    cal19->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal19->setChecked(false) ; }
  if (ivalue>=32){
    mask19->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask19->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac19->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac19->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+20) ;
  if (ivalue>=64){
    cal20->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal20->setChecked(false) ; }
  if (ivalue>=32){
    mask20->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask20->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac20->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac20->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+21) ;
  if (ivalue>=64){
    cal21->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal21->setChecked(false) ; }
  if (ivalue>=32){
    mask21->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask21->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac21->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac21->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+22) ;
  if (ivalue>=64){
    cal22->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal22->setChecked(false) ; }
  if (ivalue>=32){
    mask22->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask22->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac22->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac22->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+23) ;
  if (ivalue>=64){
    cal23->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal23->setChecked(false) ; }
  if (ivalue>=32){
    mask23->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask23->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac23->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac23->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+24) ;
  if (ivalue>=64){
    cal24->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal24->setChecked(false) ; }
  if (ivalue>=32){
    mask24->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask24->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac24->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac24->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+25) ;
  if (ivalue>=64){
    cal25->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal25->setChecked(false) ; }
  if (ivalue>=32){
    mask25->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask25->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac25->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac25->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+26) ;
  if (ivalue>=64){
    cal26->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal26->setChecked(false) ; }
  if (ivalue>=32){
    mask26->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask26->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac26->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac26->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+27) ;
  if (ivalue>=64){
    cal27->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal27->setChecked(false) ; }
  if (ivalue>=32){
    mask27->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask27->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac27->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac27->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+28) ;
  if (ivalue>=64){
    cal28->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal28->setChecked(false) ; }
  if (ivalue>=32){
    mask28->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask28->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac28->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac28->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+29) ;
  if (ivalue>=64){
    cal29->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal29->setChecked(false) ; }
  if (ivalue>=32){
    mask29->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask29->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac29->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac29->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+30) ;
  if (ivalue>=64){
    cal30->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal30->setChecked(false) ; }
  if (ivalue>=32){
    mask30->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask30->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac30->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac30->setText("0") ; }
  svalue.str("") ;

  ivalue = getChReg(i+31) ;
  if (ivalue>=64){
    cal31->setChecked(true) ; 
    ivalue-=64 ;
  }
  else{ cal31->setChecked(false) ; }
  if (ivalue>=32){
    mask31->setChecked(true) ; 
    ivalue-=32 ;
  }
  else{ mask31->setChecked(false) ; }
  if (ivalue>=1){
    if (ivalue>32){
      ivalue=32 ;
      std::cout << SET_FORECOLOR_RED << "Unexpected TrimDAC Value in Channel Register." << SET_DEFAULT_COLORS << std::endl ;
    }
    svalue << ivalue ;
    trimdac31->setText(svalue.str()) ; 
    ivalue=0 ;
  }
  else{ trimdac31->setText("0") ; }
  svalue.flush() ;
}

void vfat_controller::getChannelRegisters( int page ){
  
  uint chreg1=0, cal=0, mask=0, trimdac=0 ;
  int iValue=0, i=0 ;

  // Change index according to the page to be shown
  if (page==1){ i = 1 ; }
  else if (page==2){ i = 33 ; }
  else if (page==3){ i = 65 ; }
  else if (page==4){ i = 97 ; }
  else { std::cout << SET_FORECOLOR_RED << "Channel range (" << page << ") incorrect" << SET_DEFAULT_COLORS << std::endl ; return ; }

  if (cal0->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask0->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac0->text()) ;

  if(i==1 && calAnalog->isChecked()){ cal += 128 ; }
  
  iValue =  cal + mask + trimdac ;
  setChReg(i, iValue) ;

  iValue = 0 ;
  if (cal1->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask1->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac1->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+1, iValue) ;

  iValue = 0 ;
  if (cal2->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask2->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac2->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+2, iValue) ;

  iValue = 0 ;
  if (cal3->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask3->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac3->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+3, iValue) ;

  iValue = 0 ;
  if (cal4->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask4->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac4->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+4, iValue) ;

  iValue = 0 ;
  if (cal5->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask5->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac5->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+5, iValue) ;

  iValue = 0 ;
  if (cal6->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask6->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac6->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+6, iValue) ;

  iValue = 0 ;
  if (cal7->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask7->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac7->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+7, iValue) ;

  iValue = 0 ;
  if (cal8->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask8->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac8->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+8, iValue) ;

  iValue = 0 ;
  if (cal9->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask9->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac9->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+9, iValue) ;

  iValue = 0 ;
  if (cal10->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask10->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac10->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+10, iValue) ;

  iValue = 0 ;
  if (cal11->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask11->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac11->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+11, iValue) ;

  iValue = 0 ;
  if (cal12->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask12->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac12->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+12, iValue) ;

  iValue = 0 ;
  if (cal13->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask13->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac13->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+13, iValue) ;

  iValue = 0 ;
  if (cal14->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask14->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac14->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+14, iValue) ;

  iValue = 0 ;
  if (cal15->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask15->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac15->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+15, iValue) ;

  iValue = 0 ;
  if (cal16->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask16->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac16->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+16, iValue) ;

  iValue = 0 ;
  if (cal17->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask17->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac17->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+17, iValue) ;

  iValue = 0 ;
  if (cal18->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask18->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac18->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+18, iValue) ;

  iValue = 0 ;
  if (cal19->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask19->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac19->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+19, iValue) ;

  iValue = 0 ;
  if (cal20->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask20->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac20->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+20, iValue) ;

  iValue = 0 ;
  if (cal21->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask21->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac21->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+21, iValue) ;

  iValue = 0 ;
  if (cal22->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask22->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac22->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+22, iValue) ;

  iValue = 0 ;
  if (cal23->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask23->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac23->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+23, iValue) ;

  iValue = 0 ;
  if (cal24->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask24->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac24->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+24, iValue) ;

  iValue = 0 ;
  if (cal25->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask25->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac25->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+25, iValue) ;

  iValue = 0 ;
  if (cal26->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask26->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac26->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+26, iValue) ;

  iValue = 0 ;
  if (cal27->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask27->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac27->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+27, iValue) ;

  iValue = 0 ;
  if (cal28->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask28->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac28->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+28, iValue) ;

  iValue = 0 ;
  if (cal29->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask29->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac29->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+29, iValue) ;

  iValue = 0 ;
  if (cal30->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask30->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac30->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+30, iValue) ;

  iValue = 0 ;
  if (cal31->isChecked()){ cal = 64 ; }
  else { cal = 0 ; }
  if (mask31->isChecked()){ mask = 32 ; }
  else { mask = 0 ; }
  trimdac = atoi(trimdac31->text()) ;
  iValue = cal + mask + trimdac ;
  setChReg(i+31, iValue) ;
  
}

void vfat_controller::range1_pressed(){

  // Set the channel numbers to labels
  label0->setText("1") ;
  label1->setText("2") ;
  label2->setText("3") ;
  label3->setText("4") ;
  label4->setText("5") ;
  label5->setText("6") ;
  label6->setText("7") ;
  label7->setText("8") ;
  label8->setText("9") ;
  label9->setText("10") ;
  label10->setText("11") ;
  label11->setText("12") ;
  label12->setText("13") ;
  label13->setText("14") ;
  label14->setText("15") ;
  label15->setText("16") ;
  label16->setText("17") ;
  label17->setText("18") ;
  label18->setText("19") ;
  label19->setText("20") ;
  label20->setText("21") ;
  label21->setText("22") ;
  label22->setText("23") ;
  label23->setText("24") ;
  label24->setText("25") ;
  label25->setText("26") ;
  label26->setText("27") ;
  label27->setText("28") ;
  label28->setText("29") ;
  label29->setText("30") ;
  label30->setText("31") ;
  label31->setText("32") ;

  calAnalog->setDisabled(false) ;

  // Check TrimDAC values entered
  checkTrimDACs() ;

  // Read the fields and store values to channel register array
  getChannelRegisters(chanRegHis_) ;

  // Fill the fields according to the channel register value
  setChannelRegisters(1) ;
  chanRegHis_ = 1 ;
}


void vfat_controller::range2_pressed(){

  // Set the channel numbers to labels
  label0->setText("33") ;
  label1->setText("34") ;
  label2->setText("35") ;
  label3->setText("36") ;
  label4->setText("37") ;
  label5->setText("38") ;
  label6->setText("39") ;
  label7->setText("40") ;
  label8->setText("41") ;
  label9->setText("42") ;
  label10->setText("43") ;
  label11->setText("44") ;
  label12->setText("45") ;
  label13->setText("46") ;
  label14->setText("47") ;
  label15->setText("48") ;
  label16->setText("49") ;
  label17->setText("50") ;
  label18->setText("51") ;
  label19->setText("52") ;
  label20->setText("53") ;
  label21->setText("54") ;
  label22->setText("55") ;
  label23->setText("56") ;
  label24->setText("57") ;
  label25->setText("58") ;
  label26->setText("59") ;
  label27->setText("60") ;
  label28->setText("61") ;
  label29->setText("62") ;
  label30->setText("63") ;
  label31->setText("64") ;

  if (calAnalog->isEnabled()){ calAnalog->setDisabled(true) ; }

  // Check TrimDAC values entered
  checkTrimDACs() ;

  // Read the fields and store values to channel register array
  getChannelRegisters(chanRegHis_) ;

  // Fill the fields according to the channel register value
  setChannelRegisters(2) ;
  chanRegHis_ = 2 ;
}


void vfat_controller::range3_pressed(){

  // Set the channel numbers to labels
  label0->setText("65") ;
  label1->setText("66") ;
  label2->setText("67") ;
  label3->setText("68") ;
  label4->setText("69") ;
  label5->setText("70") ;
  label6->setText("71") ;
  label7->setText("72") ;
  label8->setText("73") ;
  label9->setText("74") ;
  label10->setText("75") ;
  label11->setText("76") ;
  label12->setText("77") ;
  label13->setText("78") ;
  label14->setText("79") ;
  label15->setText("80") ;
  label16->setText("81") ;
  label17->setText("82") ;
  label18->setText("83") ;
  label19->setText("84") ;
  label20->setText("85") ;
  label21->setText("86") ;
  label22->setText("87") ;
  label23->setText("88") ;
  label24->setText("89") ;
  label25->setText("90") ;
  label26->setText("91") ;
  label27->setText("92") ;
  label28->setText("93") ;
  label29->setText("94") ;
  label30->setText("95") ;
  label31->setText("96") ;

  if (calAnalog->isEnabled()){ calAnalog->setDisabled(true) ; }

  // Check TrimDAC values entered
  checkTrimDACs() ;

  // Read the fields and store values to channel register array
  getChannelRegisters(chanRegHis_) ;

  // Fill the fields according to the channel register value
  setChannelRegisters(3) ;
  chanRegHis_ = 3 ;
}

void vfat_controller::range4_pressed(){

  // Set the channel numbers to labels
  label0->setText("97") ;
  label1->setText("98") ;
  label2->setText("99") ;
  label3->setText("100") ;
  label4->setText("101") ;
  label5->setText("102") ;
  label6->setText("103") ;
  label7->setText("104") ;
  label8->setText("105") ;
  label9->setText("106") ;
  label10->setText("107") ;
  label11->setText("108") ;
  label12->setText("109") ;
  label13->setText("110") ;
  label14->setText("111") ;
  label15->setText("112") ;
  label16->setText("113") ;
  label17->setText("114") ;
  label18->setText("115") ;
  label19->setText("116") ;
  label20->setText("117") ;
  label21->setText("118") ;
  label22->setText("119") ;
  label23->setText("120") ;
  label24->setText("121") ;
  label25->setText("122") ;
  label26->setText("123") ;
  label27->setText("124") ;
  label28->setText("125") ;
  label29->setText("126") ;
  label30->setText("127") ;
  label31->setText("128") ;

  if (calAnalog->isEnabled()){ calAnalog->setDisabled(true) ; }

  // Check TrimDAC values entered
  checkTrimDACs() ;

  // Read the fields and store values to channel register array
  getChannelRegisters(chanRegHis_) ;

  // Fill the fields according to the channel register value
  setChannelRegisters(4) ;
  chanRegHis_ = 4 ;
}

void vfat_controller::checkTrimDACs(){

  if (atoi(trimdac0->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac0->text())<=0){  
    if (atoi(trimdac0->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac0->setText("0") ;
      trimdac0->setFocus() ;
    }
    else { trimdac0->setText("0") ; }
    
  }

  if (atoi(trimdac1->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac1->text())<=0){  
    if (atoi(trimdac1->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac1->setText("0") ;
      trimdac1->setFocus() ;
    }
    else { trimdac1->setText("0") ; }
    
  }

  if (atoi(trimdac2->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac2->text())<=0){  
    if (atoi(trimdac2->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac2->setText("0") ;
      trimdac2->setFocus() ;
    }
    else { trimdac2->setText("0") ; }
    
  }

  if (atoi(trimdac3->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac3->text())<=0){  
    if (atoi(trimdac3->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac3->setText("0") ;
      trimdac3->setFocus() ;
    }
    else { trimdac3->setText("0") ; }
    
  }

  if (atoi(trimdac4->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac4->text())<=0){  
    if (atoi(trimdac4->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac4->setText("0") ;
      trimdac4->setFocus() ;
    }
    else { trimdac4->setText("0") ; }
    
  }

  if (atoi(trimdac5->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac5->text())<=0){  
    if (atoi(trimdac5->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac5->setText("0") ;
      trimdac5->setFocus() ;
    }
    else { trimdac5->setText("0") ; }
    
  }

  if (atoi(trimdac6->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac6->text())<=0){  
    if (atoi(trimdac6->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac6->setText("0") ;
      trimdac6->setFocus() ;
    }
    else { trimdac6->setText("0") ; }
    
  }

  if (atoi(trimdac7->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac7->text())<=0){  
    if (atoi(trimdac7->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac7->setText("0") ;
      trimdac7->setFocus() ;
    }
    else { trimdac7->setText("0") ; }
    
  }

  if (atoi(trimdac8->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac8->text())<=0){  
    if (atoi(trimdac8->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac8->setText("0") ;
      trimdac8->setFocus() ;
    }
    else { trimdac8->setText("0") ; }
    
  }

  if (atoi(trimdac9->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac9->text())<=0){  
    if (atoi(trimdac9->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac9->setText("0") ;
      trimdac9->setFocus() ;
    }
    else { trimdac9->setText("0") ; }
    
  }

  if (atoi(trimdac10->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac10->text())<=0){  
    if (atoi(trimdac10->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac10->setText("0") ;
      trimdac10->setFocus() ;
    }
    else { trimdac10->setText("0") ; }
    
  }

  if (atoi(trimdac11->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac11->text())<=0){  
    if (atoi(trimdac11->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac11->setText("0") ;
      trimdac11->setFocus() ;
    }
    else { trimdac11->setText("0") ; }
    
  }

  if (atoi(trimdac12->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac12->text())<=0){  
    if (atoi(trimdac12->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac12->setText("0") ;
      trimdac12->setFocus() ;
    }
    else { trimdac12->setText("0") ; }
    
  }

  if (atoi(trimdac13->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac13->text())<=0){  
    if (atoi(trimdac13->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac13->setText("0") ;
      trimdac13->setFocus() ;
    }
    else { trimdac13->setText("0") ; }
    
  }

  if (atoi(trimdac14->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac14->text())<=0){  
    if (atoi(trimdac14->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac14->setText("0") ;
      trimdac14->setFocus() ;
    }
    else { trimdac14->setText("0") ; }
    
  }

  if (atoi(trimdac15->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac15->text())<=0){  
    if (atoi(trimdac15->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac15->setText("0") ;
      trimdac15->setFocus() ;
    }
    else { trimdac15->setText("0") ; }
    
  }

  if (atoi(trimdac16->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac16->text())<=0){  
    if (atoi(trimdac16->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac16->setText("0") ;
      trimdac16->setFocus() ;
    }
    else { trimdac16->setText("0") ; }
    
  }

  if (atoi(trimdac17->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac17->text())<=0){  
    if (atoi(trimdac17->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac17->setText("0") ;
      trimdac17->setFocus() ;
    }
    else { trimdac17->setText("0") ; }
    
  }

  if (atoi(trimdac18->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac18->text())<=0){  
    if (atoi(trimdac18->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac18->setText("0") ;
      trimdac18->setFocus() ;
    }
    else { trimdac18->setText("0") ; }
    
  }

  if (atoi(trimdac19->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac19->text())<=0){  
    if (atoi(trimdac19->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac19->setText("0") ;
      trimdac19->setFocus() ;
    }
    else { trimdac19->setText("0") ; }
    
  }

  if (atoi(trimdac20->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac20->text())<=0){  
    if (atoi(trimdac20->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac20->setText("0") ;
      trimdac20->setFocus() ;
    }
    else { trimdac20->setText("0") ; }
    
  }

  if (atoi(trimdac21->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac21->text())<=0){  
    if (atoi(trimdac21->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac21->setText("0") ;
      trimdac21->setFocus() ;
    }
    else { trimdac21->setText("0") ; }
    
  }

  if (atoi(trimdac22->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac22->text())<=0){  
    if (atoi(trimdac22->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac22->setText("0") ;
      trimdac22->setFocus() ;
    }
    else { trimdac22->setText("0") ; }
    
  }

  if (atoi(trimdac23->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac23->text())<=0){  
    if (atoi(trimdac23->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac23->setText("0") ;
      trimdac23->setFocus() ;
    }
    else { trimdac23->setText("0") ; }
    
  }

  if (atoi(trimdac24->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac24->text())<=0){  
    if (atoi(trimdac24->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac24->setText("0") ;
      trimdac24->setFocus() ;
    }
    else { trimdac24->setText("0") ; }
    
  }

  if (atoi(trimdac25->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac25->text())<=0){  
    if (atoi(trimdac25->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac25->setText("0") ;
      trimdac25->setFocus() ;
    }
    else { trimdac25->setText("0") ; }
    
  }

  if (atoi(trimdac26->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac26->text())<=0){  
    if (atoi(trimdac26->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac26->setText("0") ;
      trimdac26->setFocus() ;
    }
    else { trimdac26->setText("0") ; }
    
  }

  if (atoi(trimdac27->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac27->text())<=0){  
    if (atoi(trimdac27->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac27->setText("0") ;
      trimdac27->setFocus() ;
    }
    else { trimdac27->setText("0") ; }
    
  }

  if (atoi(trimdac28->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac28->text())<=0){  
    if (atoi(trimdac28->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac28->setText("0") ;
      trimdac28->setFocus() ;
    }
    else { trimdac28->setText("0") ; }
    
  }

  if (atoi(trimdac29->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac29->text())<=0){  
    if (atoi(trimdac29->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac29->setText("0") ;
      trimdac29->setFocus() ;
    }
    else { trimdac29->setText("0") ; }
    
  }

  if (atoi(trimdac30->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac30->text())<=0){  
    if (atoi(trimdac30->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac30->setText("0") ;
      trimdac30->setFocus() ;
    }
    else { trimdac30->setText("0") ; }
    
  }

  if (atoi(trimdac31->text())>=VFAT_TRIMDAC_MAX || atoi(trimdac31->text())<=0){  
    if (atoi(trimdac31->text())!=0){
      ErrorMessage("Value is not correct, must be 0-31.");
      trimdac31->setText("0") ;
      trimdac31->setFocus() ;
    }
    else { trimdac31->setText("0") ; }
  }
}

void vfat_controller::fecSlots_activated( int sel ){

  refreshCcuAddresses() ;
}

void vfat_controller::ccuAddresses_activated( int sel ){

  refreshI2cChannels() ;
}

void vfat_controller::i2cChannels_activated( int sel ){

  refreshDeviceAddresses() ;
}

void vfat_controller::deviceAddresses_activated( int sel ){

  readVfatId() ;
}

void vfat_controller::refreshCcuAddresses(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh CCUs" << SET_DEFAULT_COLORS << std::endl ;
#endif

  ccuAddresses->clear() ;
  i2cChannels->clear() ;
  deviceAddresses->clear() ;
  vfatChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (vfatAddresses_.size()>0 && sscanf (fecSlots->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;
   
    for (int i=0;i<vfatAddresses_.size();i++){

#ifdef DEBUG_VFAT_CONTROLLER
      //ss << "0x" << std::hex << (int)getFecKey(vfatAddresses_.at(i)) << std::dec ;
      ss << "FEC RING " << (int)getFecKey(vfatAddresses_.at(i)) << " " << (int)getRingKey(vfatAddresses_.at(i)) << SET_DEFAULT_COLORS << std::endl ;
      std::cout << SET_FORECOLOR_CYAN << ss.str() ;
#endif

      //std::cout << SET_FORECOLOR_CYAN << "FEC " << ss.str() << " / " << fecSlots->currentText().latin1() << SET_DEFAULT_COLORS << std::endl ;

      if(fecSlot==getFecKey(vfatAddresses_.at(i)) && ringSlot==getRingKey(vfatAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getCcuKey(vfatAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	bool addItem = true ;

	for(int j=0;j<ccuAddresses->count();j++){

	  if(ccuAddresses->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	}

	if(addItem){ ccuAddresses->insertItem(ss.str()) ; }

      }
    }
    ss.flush() ;

    if(ccuAddresses->count()>0){

      ccuAddresses->setCurrentItem(0) ; 
      refreshI2cChannels() ;
    }
  }
}

void vfat_controller::refreshI2cChannels(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh channels" << SET_DEFAULT_COLORS << std::endl ;
#endif

  i2cChannels->clear() ;
  deviceAddresses->clear() ;
  vfatChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (vfatAddresses_.size()>0 && sscanf (fecSlots->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;
   
    for (int i=0;i<vfatAddresses_.size();i++){

      ss.str("") ;
      ss << "0x" << std::hex << (int)getCcuKey(vfatAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif


#ifdef DEBUG_VFAT_CONTROLLER

      std::cout << SET_FORECOLOR_CYAN << ccuAddresses->currentText() << "==" << (QString)ss.str()
		<< " && " << fecSlot << "==" << getFecKey(vfatAddresses_.at(i))
		<< " && " << ringSlot << "==" << getRingKey(vfatAddresses_.at(i)) << SET_DEFAULT_COLORS << std::endl ;

#endif

      if(ccuAddresses->currentText()==(QString)ss.str() && fecSlot==getFecKey(vfatAddresses_.at(i)) && ringSlot==getRingKey(vfatAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getChannelKey(vfatAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "CH " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	bool addItem = true ;

	for(int j=0;j<i2cChannels->count();j++){

	  if(i2cChannels->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	}

	if(addItem){ i2cChannels->insertItem(ss.str()) ; }

      }
    }
      
    ss.flush() ;

    if(i2cChannels->count()){

      i2cChannels->setCurrentItem(0) ; 
      refreshDeviceAddresses() ;
    }
  }
}

void vfat_controller::refreshDeviceAddresses(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh devices" << SET_DEFAULT_COLORS << std::endl ;
#endif

  deviceAddresses->clear() ;
  vfatChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (vfatAddresses_.size()>0 && sscanf (fecSlots->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "------------------------------------------------------------------------" << std::endl ;
    std::cout << "Corresponding VFATS for channel selected:" << SET_DEFAULT_COLORS << std::endl ; 
#endif
   
    for (int i=0;i<vfatAddresses_.size();i++){

#ifdef DEBUG_VFAT_CONTROLLER
      keyType index = vfatAddresses_.at(i) ;

      std::cout << SET_FORECOLOR_CYAN << "FEC (" 
		<< std::hex << (int)getFecKey(index) << ","
		<< std::hex << (int)getRingKey(index) << ") CCU 0x" 
		<< std::hex << (int)getCcuKey(index) << " CHANNEL 0x" 
		<< std::hex << (int)getChannelKey(index) << " ADDRESS 0x" 
		<< std::hex << (int)getAddressKey(index) << SET_DEFAULT_COLORS << std::endl ;
#endif

      ss.str("") ;
      ss << "0x" << std::hex << (int)getCcuKey(vfatAddresses_.at(i)) << std::dec ;


#ifdef DEBUG_VFAT_CONTROLLER

      std::cout << SET_FORECOLOR_CYAN << ccuAddresses->currentText() << "==" << (QString)ss.str()
		<< " && " << fecSlot << "==" << getFecKey(vfatAddresses_.at(i))
		<< " && " << ringSlot << "==" << getRingKey(vfatAddresses_.at(i)) << SET_DEFAULT_COLORS << std::endl ;

#endif


      if(ccuAddresses->currentText()==(QString)ss.str() && fecSlot==getFecKey(vfatAddresses_.at(i)) && ringSlot==getRingKey(vfatAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getChannelKey(vfatAddresses_.at(i)) << std::dec ;

	if(i2cChannels->currentText()==(QString)ss.str()){

	  ss.str("") ;
	  ss << "0x" << std::hex << (int)getAddressKey(vfatAddresses_.at(i)) << std::dec ;

	  bool addItem = true ;

	  for(int j=0;j<deviceAddresses->count();j++){

	    if(deviceAddresses->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	  }

	  if(addItem){ deviceAddresses->insertItem(ss.str()) ; }
	}
      }
    }
#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "------------------------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
#endif
     
    ss.flush() ;

    if(deviceAddresses->count()){

      deviceAddresses->setCurrentItem(0) ; 
      readVfatId() ;
    }
  }
}
