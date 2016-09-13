/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/


#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h> 
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qthread.h>
#include <qpixmap.h>

#include "vfat_controller.h" // SW header


/* --------------------------------------------------------------------------------------------------- */
/*                                                                                                     */
/*                   Coincidence Panel                                                                 */
/*                                                                                                     */
/* --------------------------------------------------------------------------------------------------- */ 

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::controlSel_pressed() {

  if (controlSel->isChecked()){
    cr4Edge->setDisabled(true) ;
    cr0Run->setDisabled(true) ;
    cr0Sync->setDisabled(true) ;
    cr4MSPulseLength->setDisabled(true) ;
    cr3Inverted->setDisabled(true) ;
    cr3OutputGrouping->setDisabled(true) ;
    cr1Neighbors->setDisabled(true) ;
    cr1NP->setDisabled(true) ;
    cr1V->setDisabled(true) ;
    cr2W->setDisabled(true) ;
    cr3CoincidenceSel->setDisabled(true) ;
    cr2Z->setDisabled(true) ;
    cr3OutputCondition->setDisabled(true) ;
    cr0CTReset->setDisabled(true) ;
    cr4CounterSel->setDisabled(true) ;
  }
  else {
    cr4Edge->setDisabled(false) ;
    cr0Run->setDisabled(false) ;
    cr0Sync->setDisabled(false) ;
    cr4MSPulseLength->setDisabled(false) ;
    cr3Inverted->setDisabled(false) ;
    cr3OutputGrouping->setDisabled(false) ;
    cr1Neighbors->setDisabled(false) ;
    cr1NP->setDisabled(false) ;
    cr1V->setDisabled(false) ;
    cr2W->setDisabled(false) ;
    cr3CoincidenceSel->setDisabled(false) ;
    cr2Z->setDisabled(false) ;
    cr3OutputCondition->setDisabled(false) ;
    cr0CTReset->setDisabled(false) ;
    cr4CounterSel->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::counterSel_pressed() {

  if (counterSel->isChecked()){
    cchipCounterT->setDisabled(true) ;
    readCChipCounters->setDisabled(true) ;
  }
  else {
    cchipCounterT->setDisabled(false) ;
    readCChipCounters->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::impedanceSel_pressed() {

  if (impedanceSel->isChecked()){ impedance->setDisabled(true) ; }
  else { impedance->setDisabled(false) ; }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask0Sel_pressed() {

  if (cchip_mask0Sel->isChecked()){
    cchip_mask1->setDisabled(true) ;
    cchip_mask2->setDisabled(true) ;
    cchip_mask3->setDisabled(true) ;
    cchip_mask4->setDisabled(true) ;
    cchip_mask5->setDisabled(true) ;
    cchip_mask6->setDisabled(true) ;
    cchip_mask7->setDisabled(true) ;
    cchip_mask8->setDisabled(true) ;
  }
  else {
    cchip_mask1->setDisabled(false) ;
    cchip_mask2->setDisabled(false) ;
    cchip_mask3->setDisabled(false) ;
    cchip_mask4->setDisabled(false) ;
    cchip_mask5->setDisabled(false) ;
    cchip_mask6->setDisabled(false) ;
    cchip_mask7->setDisabled(false) ;
    cchip_mask8->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask1Sel_pressed() {

  if (cchip_mask1Sel->isChecked()){
    cchip_mask9->setDisabled(true) ;
    cchip_mask10->setDisabled(true) ;
    cchip_mask11->setDisabled(true) ;
    cchip_mask12->setDisabled(true) ;
    cchip_mask13->setDisabled(true) ;
    cchip_mask14->setDisabled(true) ;
    cchip_mask15->setDisabled(true) ;
    cchip_mask16->setDisabled(true) ;
  }
  else {
    cchip_mask9->setDisabled(false) ;
    cchip_mask10->setDisabled(false) ;
    cchip_mask11->setDisabled(false) ;
    cchip_mask12->setDisabled(false) ;
    cchip_mask13->setDisabled(false) ;
    cchip_mask14->setDisabled(false) ;
    cchip_mask15->setDisabled(false) ;
    cchip_mask16->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask2Sel_pressed() {

  if (cchip_mask2Sel->isChecked()){
    cchip_mask17->setDisabled(true) ;
    cchip_mask18->setDisabled(true) ;
    cchip_mask19->setDisabled(true) ;
    cchip_mask20->setDisabled(true) ;
    cchip_mask21->setDisabled(true) ;
    cchip_mask22->setDisabled(true) ;
    cchip_mask23->setDisabled(true) ;
    cchip_mask24->setDisabled(true) ;
  }
  else {
    cchip_mask17->setDisabled(false) ;
    cchip_mask18->setDisabled(false) ;
    cchip_mask19->setDisabled(false) ;
    cchip_mask20->setDisabled(false) ;
    cchip_mask21->setDisabled(false) ;
    cchip_mask22->setDisabled(false) ;
    cchip_mask23->setDisabled(false) ;
    cchip_mask24->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask3Sel_pressed() {

  if (cchip_mask3Sel->isChecked()){
    cchip_mask25->setDisabled(true) ;
    cchip_mask26->setDisabled(true) ;
    cchip_mask27->setDisabled(true) ;
    cchip_mask28->setDisabled(true) ;
    cchip_mask29->setDisabled(true) ;
    cchip_mask30->setDisabled(true) ;
    cchip_mask31->setDisabled(true) ;
    cchip_mask32->setDisabled(true) ;
  }
  else {
    cchip_mask25->setDisabled(false) ;
    cchip_mask26->setDisabled(false) ;
    cchip_mask27->setDisabled(false) ;
    cchip_mask28->setDisabled(false) ;
    cchip_mask29->setDisabled(false) ;
    cchip_mask30->setDisabled(false) ;
    cchip_mask31->setDisabled(false) ;
    cchip_mask32->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask4Sel_pressed() {

  if (cchip_mask4Sel->isChecked()){
    cchip_mask33->setDisabled(true) ;
    cchip_mask34->setDisabled(true) ;
    cchip_mask35->setDisabled(true) ;
    cchip_mask36->setDisabled(true) ;
    cchip_mask37->setDisabled(true) ;
    cchip_mask38->setDisabled(true) ;
    cchip_mask39->setDisabled(true) ;
    cchip_mask40->setDisabled(true) ;
  }
  else {
    cchip_mask33->setDisabled(false) ;
    cchip_mask34->setDisabled(false) ;
    cchip_mask35->setDisabled(false) ;
    cchip_mask36->setDisabled(false) ;
    cchip_mask37->setDisabled(false) ;
    cchip_mask38->setDisabled(false) ;
    cchip_mask39->setDisabled(false) ;
    cchip_mask40->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask5Sel_pressed() {

  if (cchip_mask5Sel->isChecked()){
    cchip_mask41->setDisabled(true) ;
    cchip_mask42->setDisabled(true) ;
    cchip_mask43->setDisabled(true) ;
    cchip_mask44->setDisabled(true) ;
    cchip_mask45->setDisabled(true) ;
    cchip_mask46->setDisabled(true) ;
    cchip_mask47->setDisabled(true) ;
    cchip_mask48->setDisabled(true) ;
  }
  else {
    cchip_mask41->setDisabled(false) ;
    cchip_mask42->setDisabled(false) ;
    cchip_mask43->setDisabled(false) ;
    cchip_mask44->setDisabled(false) ;
    cchip_mask45->setDisabled(false) ;
    cchip_mask46->setDisabled(false) ;
    cchip_mask47->setDisabled(false) ;
    cchip_mask48->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask6Sel_pressed() {

  if (cchip_mask6Sel->isChecked()){
    cchip_mask49->setDisabled(true) ;
    cchip_mask50->setDisabled(true) ;
    cchip_mask51->setDisabled(true) ;
    cchip_mask52->setDisabled(true) ;
    cchip_mask53->setDisabled(true) ;
    cchip_mask54->setDisabled(true) ;
    cchip_mask55->setDisabled(true) ;
    cchip_mask56->setDisabled(true) ;
  }
  else {
    cchip_mask49->setDisabled(false) ;
    cchip_mask50->setDisabled(false) ;
    cchip_mask51->setDisabled(false) ;
    cchip_mask52->setDisabled(false) ;
    cchip_mask53->setDisabled(false) ;
    cchip_mask54->setDisabled(false) ;
    cchip_mask55->setDisabled(false) ;
    cchip_mask56->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask7Sel_pressed() {

  if (cchip_mask7Sel->isChecked()){
    cchip_mask57->setDisabled(true) ;
    cchip_mask58->setDisabled(true) ;
    cchip_mask59->setDisabled(true) ;
    cchip_mask60->setDisabled(true) ;
    cchip_mask61->setDisabled(true) ;
    cchip_mask62->setDisabled(true) ;
    cchip_mask63->setDisabled(true) ;
    cchip_mask64->setDisabled(true) ;
  }
  else {
    cchip_mask57->setDisabled(false) ;
    cchip_mask58->setDisabled(false) ;
    cchip_mask59->setDisabled(false) ;
    cchip_mask60->setDisabled(false) ;
    cchip_mask61->setDisabled(false) ;
    cchip_mask62->setDisabled(false) ;
    cchip_mask63->setDisabled(false) ;
    cchip_mask64->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask8Sel_pressed() {

  if (cchip_mask8Sel->isChecked()){
    cchip_mask65->setDisabled(true) ;
    cchip_mask66->setDisabled(true) ;
    cchip_mask67->setDisabled(true) ;
    cchip_mask68->setDisabled(true) ;
    cchip_mask69->setDisabled(true) ;
    cchip_mask70->setDisabled(true) ;
    cchip_mask71->setDisabled(true) ;
    cchip_mask72->setDisabled(true) ;
  }
  else {
    cchip_mask65->setDisabled(false) ;
    cchip_mask66->setDisabled(false) ;
    cchip_mask67->setDisabled(false) ;
    cchip_mask68->setDisabled(false) ;
    cchip_mask69->setDisabled(false) ;
    cchip_mask70->setDisabled(false) ;
    cchip_mask71->setDisabled(false) ;
    cchip_mask72->setDisabled(false) ;
  }
}

/** \brief Disable / enable fields in the GUI
 * Disable / enable fields in the GUI
 */
void vfat_controller::cchip_mask9Sel_pressed() {

  if (cchip_mask9Sel->isChecked()){
    cchip_mask73->setDisabled(true) ;
    cchip_mask74->setDisabled(true) ;
    cchip_mask75->setDisabled(true) ;
    cchip_mask76->setDisabled(true) ;
    cchip_mask77->setDisabled(true) ;
    cchip_mask78->setDisabled(true) ;
    cchip_mask79->setDisabled(true) ;
    cchip_mask80->setDisabled(true) ;
  }
  else {
    cchip_mask73->setDisabled(false) ;
    cchip_mask74->setDisabled(false) ;
    cchip_mask75->setDisabled(false) ;
    cchip_mask76->setDisabled(false) ;
    cchip_mask77->setDisabled(false) ;
    cchip_mask78->setDisabled(false) ;
    cchip_mask79->setDisabled(false) ;
    cchip_mask80->setDisabled(false) ;
  }
}

/** \brief Check all the channels to be masked
 * Check all the channels to be masked
 */
void vfat_controller::cchip_maskAll_pressed() {

  cchip_mask1->setChecked(true) ;
  cchip_mask2->setChecked(true) ;
  cchip_mask3->setChecked(true) ;
  cchip_mask4->setChecked(true) ;
  cchip_mask5->setChecked(true) ;
  cchip_mask6->setChecked(true) ;
  cchip_mask7->setChecked(true) ;
  cchip_mask8->setChecked(true) ;
  cchip_mask9->setChecked(true) ;
  cchip_mask10->setChecked(true) ;
  cchip_mask11->setChecked(true) ;
  cchip_mask12->setChecked(true) ;
  cchip_mask13->setChecked(true) ;
  cchip_mask14->setChecked(true) ;
  cchip_mask15->setChecked(true) ;
  cchip_mask16->setChecked(true) ;
  cchip_mask17->setChecked(true) ;
  cchip_mask18->setChecked(true) ;
  cchip_mask19->setChecked(true) ;
  cchip_mask20->setChecked(true) ;
  cchip_mask21->setChecked(true) ;
  cchip_mask22->setChecked(true) ;
  cchip_mask23->setChecked(true) ;
  cchip_mask24->setChecked(true) ;
  cchip_mask25->setChecked(true) ;
  cchip_mask26->setChecked(true) ;
  cchip_mask27->setChecked(true) ;
  cchip_mask28->setChecked(true) ;
  cchip_mask29->setChecked(true) ;
  cchip_mask30->setChecked(true) ;
  cchip_mask31->setChecked(true) ;
  cchip_mask32->setChecked(true) ;
  cchip_mask33->setChecked(true) ;
  cchip_mask34->setChecked(true) ;
  cchip_mask35->setChecked(true) ;
  cchip_mask36->setChecked(true) ;
  cchip_mask37->setChecked(true) ;
  cchip_mask38->setChecked(true) ;
  cchip_mask39->setChecked(true) ;
  cchip_mask40->setChecked(true) ;
  cchip_mask41->setChecked(true) ;
  cchip_mask42->setChecked(true) ;
  cchip_mask43->setChecked(true) ;
  cchip_mask44->setChecked(true) ;
  cchip_mask45->setChecked(true) ;
  cchip_mask46->setChecked(true) ;
  cchip_mask47->setChecked(true) ;
  cchip_mask48->setChecked(true) ;
  cchip_mask49->setChecked(true) ;
  cchip_mask50->setChecked(true) ;
  cchip_mask51->setChecked(true) ;
  cchip_mask52->setChecked(true) ;
  cchip_mask53->setChecked(true) ;
  cchip_mask54->setChecked(true) ;
  cchip_mask55->setChecked(true) ;
  cchip_mask56->setChecked(true) ;
  cchip_mask57->setChecked(true) ;
  cchip_mask58->setChecked(true) ;
  cchip_mask59->setChecked(true) ;
  cchip_mask60->setChecked(true) ;
  cchip_mask61->setChecked(true) ;
  cchip_mask62->setChecked(true) ;
  cchip_mask63->setChecked(true) ;
  cchip_mask64->setChecked(true) ;
  cchip_mask65->setChecked(true) ;
  cchip_mask66->setChecked(true) ;
  cchip_mask67->setChecked(true) ;
  cchip_mask68->setChecked(true) ;
  cchip_mask69->setChecked(true) ;
  cchip_mask70->setChecked(true) ;
  cchip_mask71->setChecked(true) ;
  cchip_mask72->setChecked(true) ;
  cchip_mask73->setChecked(true) ;
  cchip_mask74->setChecked(true) ;
  cchip_mask75->setChecked(true) ;
  cchip_mask76->setChecked(true) ;
  cchip_mask77->setChecked(true) ;
  cchip_mask78->setChecked(true) ;
  cchip_mask79->setChecked(true) ;
  cchip_mask80->setChecked(true) ;
}

/** \brief Check all the channels not to be masked
 * Check all the channels not to be masked
 */
void vfat_controller::cchip_maskNone_pressed() {

  for (int i=0;i<=9;i++){ cchip_unmask(i) ; }
}

/** \brief Check one set of channels to be masked
 * Check one set of channels to be masked
 * \param index - set of channels
 */
void vfat_controller::cchip_unmask( int index ) {

  if(index==0){
    cchip_mask1->setChecked(false) ;
    cchip_mask2->setChecked(false) ;
    cchip_mask3->setChecked(false) ;
    cchip_mask4->setChecked(false) ;
    cchip_mask5->setChecked(false) ;
    cchip_mask6->setChecked(false) ;
    cchip_mask7->setChecked(false) ;
    cchip_mask8->setChecked(false) ;
  }
  if(index==1){
    cchip_mask9->setChecked(false) ;
    cchip_mask10->setChecked(false) ;
    cchip_mask11->setChecked(false) ;
    cchip_mask12->setChecked(false) ;
    cchip_mask13->setChecked(false) ;
    cchip_mask14->setChecked(false) ;
    cchip_mask15->setChecked(false) ;
    cchip_mask16->setChecked(false) ;
  }
  if(index==2){
    cchip_mask17->setChecked(false) ;
    cchip_mask18->setChecked(false) ;
    cchip_mask19->setChecked(false) ;
    cchip_mask20->setChecked(false) ;
    cchip_mask21->setChecked(false) ;
    cchip_mask22->setChecked(false) ;
    cchip_mask23->setChecked(false) ;
    cchip_mask24->setChecked(false) ;
  }
  if(index==3){
    cchip_mask25->setChecked(false) ;
    cchip_mask26->setChecked(false) ;
    cchip_mask27->setChecked(false) ;
    cchip_mask28->setChecked(false) ;
    cchip_mask29->setChecked(false) ;
    cchip_mask30->setChecked(false) ;
    cchip_mask31->setChecked(false) ;
    cchip_mask32->setChecked(false) ;
  }
  if(index==4){
    cchip_mask33->setChecked(false) ;
    cchip_mask34->setChecked(false) ;
    cchip_mask35->setChecked(false) ;
    cchip_mask36->setChecked(false) ;
    cchip_mask37->setChecked(false) ;
    cchip_mask38->setChecked(false) ;
    cchip_mask39->setChecked(false) ;
    cchip_mask40->setChecked(false) ;
  }
  if(index==5){
    cchip_mask41->setChecked(false) ;
    cchip_mask42->setChecked(false) ;
    cchip_mask43->setChecked(false) ;
    cchip_mask44->setChecked(false) ;
    cchip_mask45->setChecked(false) ;
    cchip_mask46->setChecked(false) ;
    cchip_mask47->setChecked(false) ;
    cchip_mask48->setChecked(false) ;
  }
  if(index==6){
    cchip_mask49->setChecked(false) ;
    cchip_mask50->setChecked(false) ;
    cchip_mask51->setChecked(false) ;
    cchip_mask52->setChecked(false) ;
    cchip_mask53->setChecked(false) ;
    cchip_mask54->setChecked(false) ;
    cchip_mask55->setChecked(false) ;
    cchip_mask56->setChecked(false) ;
  }
  if(index==7){
    cchip_mask57->setChecked(false) ;
    cchip_mask58->setChecked(false) ;
    cchip_mask59->setChecked(false) ;
    cchip_mask60->setChecked(false) ;
    cchip_mask61->setChecked(false) ;
    cchip_mask62->setChecked(false) ;
    cchip_mask63->setChecked(false) ;
    cchip_mask64->setChecked(false) ;
  }
  if(index==8){
    cchip_mask65->setChecked(false) ;
    cchip_mask66->setChecked(false) ;
    cchip_mask67->setChecked(false) ;
    cchip_mask68->setChecked(false) ;
    cchip_mask69->setChecked(false) ;
    cchip_mask70->setChecked(false) ;
    cchip_mask71->setChecked(false) ;
    cchip_mask72->setChecked(false) ;
  }
  if(index==9){
    cchip_mask73->setChecked(false) ;
    cchip_mask74->setChecked(false) ;
    cchip_mask75->setChecked(false) ;
    cchip_mask76->setChecked(false) ;
    cchip_mask77->setChecked(false) ;
    cchip_mask78->setChecked(false) ;
    cchip_mask79->setChecked(false) ;
    cchip_mask80->setChecked(false) ;
  }
}

/** \brief Check all the registers
 * Check all the registers
 */
void vfat_controller::selectAllReg2_pressed() {

  if (!controlSel->isChecked()){
    controlSel_pressed() ;
    controlSel->setChecked(true) ;
  }
  if (!counterSel->isChecked()){
    counterSel_pressed() ;
    counterSel->setChecked(true) ;
  }
  if (!impedanceSel->isChecked()){
    impedanceSel_pressed() ;
    impedanceSel->setChecked(true) ;
  }
  if (!cchip_mask0Sel->isChecked()){
    cchip_mask0Sel_pressed() ;
    cchip_mask0Sel->setChecked(true) ;
  }
  if (!cchip_mask1Sel->isChecked()){
    cchip_mask1Sel_pressed() ;
    cchip_mask1Sel->setChecked(true) ;
  }
  if (!cchip_mask2Sel->isChecked()){
    cchip_mask2Sel_pressed() ;
    cchip_mask2Sel->setChecked(true) ;
  }
  if (!cchip_mask3Sel->isChecked()){
    cchip_mask3Sel_pressed() ;
    cchip_mask3Sel->setChecked(true) ;
  }
  if (!cchip_mask4Sel->isChecked()){
    cchip_mask4Sel_pressed() ;
    cchip_mask4Sel->setChecked(true) ;
  }
  if (!cchip_mask5Sel->isChecked()){
    cchip_mask5Sel_pressed() ;
    cchip_mask5Sel->setChecked(true) ;
  }
  if (!cchip_mask6Sel->isChecked()){
    cchip_mask6Sel_pressed() ;
    cchip_mask6Sel->setChecked(true) ;
  }
  if (!cchip_mask7Sel->isChecked()){
    cchip_mask7Sel_pressed() ;
    cchip_mask7Sel->setChecked(true) ;
  }
  if (!cchip_mask8Sel->isChecked()){
    cchip_mask8Sel_pressed() ;
    cchip_mask8Sel->setChecked(true) ;
  }
  if (!cchip_mask9Sel->isChecked()){
    cchip_mask9Sel_pressed() ;
    cchip_mask9Sel->setChecked(true) ;
  }
}

/** \brief Uncheck all the registers
 * Uncheck all the registers
 */
void vfat_controller::selectNoneReg2_pressed() {

  if (controlSel->isChecked()){
    controlSel_pressed() ;
    controlSel->setChecked(false) ;
  }
  if (counterSel->isChecked()){
    counterSel_pressed() ;
    counterSel->setChecked(false) ;
  }
  if (impedanceSel->isChecked()){
    impedanceSel_pressed() ;
    impedanceSel->setChecked(false) ;
  }
  if (cchip_mask0Sel->isChecked()){
    cchip_mask0Sel_pressed() ;
    cchip_mask0Sel->setChecked(false) ;
  }
  if (cchip_mask1Sel->isChecked()){
    cchip_mask1Sel_pressed() ;
    cchip_mask1Sel->setChecked(false) ;
  }
  if (cchip_mask2Sel->isChecked()){
    cchip_mask2Sel_pressed() ;
    cchip_mask2Sel->setChecked(false) ;
  }
  if (cchip_mask3Sel->isChecked()){
    cchip_mask3Sel_pressed() ;
    cchip_mask3Sel->setChecked(false) ;
  }
  if (cchip_mask4Sel->isChecked()){
    cchip_mask4Sel_pressed() ;
    cchip_mask4Sel->setChecked(false) ;
  }
  if (cchip_mask5Sel->isChecked()){
    cchip_mask5Sel_pressed() ;
    cchip_mask5Sel->setChecked(false) ;
  }
  if (cchip_mask6Sel->isChecked()){
    cchip_mask6Sel_pressed() ;
    cchip_mask6Sel->setChecked(false) ;
  }
  if (cchip_mask7Sel->isChecked()){
    cchip_mask7Sel_pressed() ;
    cchip_mask7Sel->setChecked(false) ;
  }
  if (cchip_mask8Sel->isChecked()){
    cchip_mask8Sel_pressed() ;
    cchip_mask8Sel->setChecked(false) ;
  }
  if (cchip_mask9Sel->isChecked()){
    cchip_mask9Sel_pressed() ;
    cchip_mask9Sel->setChecked(false) ;
  }
}

/** \brief Change the grouping of the input and coincidences according to the number of plains.
 * Change the grouping of the input and coincidences according to the number of plains.
 * \param sel - value selected
 */
void vfat_controller::cr1NP_activated( int sel ) {

  /* TEMP DISABLE

  NOTE: THE V and W options changed also in the GUI 

  cr1V->clear() ;
  cr2W->clear() ;
  cr2Z->clear() ;

  */

  std::stringstream ss ;

  if(sel==0){

    /* TEMP DISABLE

    // Fill the V and W selections
    for (int i = 0 ; i <=5 ; i ++) {

      ss.str("");
      ss << i ;
      cr1V->insertItem (ss.str()) ;
      cr2W->insertItem (ss.str()) ;
    }

    // Fill the Z selection
    for (int i = 0 ; i <=15 ; i ++) {

      ss.str("");
      ss << i ;
      cr2Z->insertItem (ss.str()) ;
    }

    */

    // Set the colors for the grouping.
    cchip_labelMask1->setPaletteForegroundColor( QColor( 255, 0, 0 ) ) ;
    cchip_labelMask2->setPaletteForegroundColor( QColor( 0, 0, 255 ) ) ;
    cchip_labelMask5->setPaletteForegroundColor( QColor( 255, 0, 0) ) ;
    cchip_labelMask6->setPaletteForegroundColor( QColor( 0, 0, 255 ) ) ;
    cchip_labelMask9->setPaletteForegroundColor( QColor( 255, 0, 0 ) ) ;
  }
  else {

    /* TEMP DISABLE

    // Fill the V and W selections
    for (int i = 0 ; i <=10 ; i ++) {

      ss.str("") ;
      ss << i ;
      cr1V->insertItem (ss.str()) ;
      cr2W->insertItem (ss.str()) ;
    }

    // Fill te Z selection
    for (int i = 0 ; i <=8 ; i ++) {

      ss.str("") ;
      ss << i ;
      cr2Z->insertItem (ss.str()) ;
    }

    */
    
    // Set the colors for the grouping.
    cchip_labelMask1->setPaletteForegroundColor( QColor( 0, 0, 255 ) ) ;
    cchip_labelMask2->setPaletteForegroundColor( QColor( 255, 0, 0 ) ) ;
    cchip_labelMask5->setPaletteForegroundColor( QColor( 0, 0, 255 ) ) ;
    cchip_labelMask6->setPaletteForegroundColor( QColor( 255, 0, 0 ) ) ;
    cchip_labelMask9->setPaletteForegroundColor( QColor( 0, 0, 255 ) ) ;
  }

  ss.flush() ;
}

/** \brief Change the picture of coincidence selection and output condition.
 * Change the picture of coincidence selection and output condition.
 * \param sel - value selected
 */
void vfat_controller::cr3CoincidenceSel_activated( int sel ){

  int sel2 = cr3OutputCondition->currentItem() ;
  updateLogicPic(sel, sel2) ;
}

/** \brief Change the picture of coincidence selection and output condition.
 * Change the picture of coincidence selection and output condition.
 * \param sel - value selected
 */
void vfat_controller::cr3OutputCondition_activated( int sel ){

  int sel2 = cr3CoincidenceSel->currentItem() ;
  updateLogicPic(sel2, sel) ;
}

void vfat_controller::logicBlock_activated( int style ) {

  int sel = cr3CoincidenceSel->currentItem() ;
  int sel2 = cr3OutputCondition->currentItem() ;
  updateLogicPic(sel, sel2) ;
}

void vfat_controller::logicText_toggled( bool show ){

  int sel = cr3CoincidenceSel->currentItem() ;
  int sel2 = cr3OutputCondition->currentItem() ;
  updateLogicPic(sel, sel2) ;
}

void vfat_controller::updateLogicPic( int sel, int sel2 ) {

  int style = logicBlock->currentItem() ;

  if(sel==0){

    if(sel2==0){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/11a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/11b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("( V / NP <font color=\"#ff0000\">AND</font> W / NP ) <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\">NOT</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==1){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/12a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/12b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("( V / NP <font color=\"#ff0000\">AND</font> W / NP ) <font color=\"#ff0000\">AND</font> Z / 8 or 16") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==2){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/13a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/13b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( ( V / NP <font color=\"#ff0000\">AND</font> W / NP ) <font color=\"#ff0000\">AND</font>  (  <font color=\"#ff0000\">NOT</font> Z / 8 or 16 ) )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==3){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/14a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/14b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( ( V / NP <font color=\"#ff0000\">AND</font> W / NP ) <font color=\"#ff0000\">AND</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
  }
  else if(sel==1){

    if(sel2==0){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/21a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/21b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("( V / NP <font color=\"#ff0000\">OR</font> W / NP ) <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==1){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/22a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/22b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("( V / NP <font color=\"#ff0000\">OR</font> W / NP ) <font color=\"#ff0000\">AND</font> Z / 8 or 16") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==2){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/23a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/23b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( ( V / NP <font color=\"#ff0000\">OR</font> W /NP ) <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 ) )") ; }
      else{ conditionLabel->setText("") ; }

    }
    else if(sel2==3){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/24a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/24b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( ( V / NP <font color=\"#ff0000\">OR</font> W /NP ) <font color=\"#ff0000\">AND</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
  }

  if(sel==2){

    if(sel2==0){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/31a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/31b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("V / NP <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==1){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/32a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/32b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("V / NP <font color=\"#ff0000\">AND</font> Z / 8 or 16") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==2){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/33a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/33b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( V / NP <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 ) )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==3){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/34a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/34b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( V / NP <font color=\"#ff0000\">AND</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
  }
  else if(sel==3){

    if(sel2==0){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/41a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/41b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("W / NP <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==1){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/42a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/42b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("W / NP <font color=\"#ff0000\">AND</font> Z / 8 or 16") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==2){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/43a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/43b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( W / NP <font color=\"#ff0000\">AND</font> (  <font color=\"#ff0000\"> NOT</font> Z / 8 or 16 ) )") ; }
      else{ conditionLabel->setText("") ; }
    }
    else if(sel2==3){
      if (style==0){ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/a/44a.jpg")) ; }
      else{ logicPixmapLabel->setPixmap((QString)(vfat_controller_home_ + "/pic/b/44b.jpg")) ; }
      if(logicText->isChecked()){ conditionLabel->setText("<font color=\"#ff0000\">NOT</font> ( W / NP <font color=\"#ff0000\">AND</font> Z / 8 or 16 ) ") ; }
      else{ conditionLabel->setText("") ; }
    }
  }
}

/** \brief Get the value of channel register
 * Get the value of channel register
 * \return the control register value
 */
uint vfat_controller::getCChipCr0(){
  
  uint ct = cr0CTReset->currentItem() ;
  uint sync = cr0Sync->currentItem() ;
  uint run = cr0Run->currentItem() ;

  if (ct==1){ ct = 16 ; }
  else if (ct==2){ ct = 32 ; }
  else if (ct==3){ ct = 48 ; }
  if (sync==1){ sync = 2 ; }
  else if (sync==2){ sync = 4 ; }
  else if (sync==3){ sync = 6 ; }

  uint value = ct + sync + run ;

  return (value) ;
}

/** \brief Get the value of channel register
 * Get the value of channel register
 * \return the control register value
 */  
uint vfat_controller::getCChipCr1(){

  uint ov = cr1Neighbors->currentItem() ;
  uint np = cr1NP->currentItem() ;
  uint v = cr1V->currentItem() ;

  if (ov==1){ ov = 32 ; }
  else if (ov==2){ ov = 64 ; }
  else if (ov==3){ ov = 96 ; }
  else if (ov==4){ ov = 128 ; }
  else if (ov==5){ ov = 160 ; }
  else if (ov==6){ ov = 192 ; }
  else if (ov==7){ ov = 224 ; }
  if (np==1){ np = 16 ; }

  uint value = ov + np + v ;

  return (value) ;
}

/** \brief Get the value of channel register
 * Get the value of channel register
 * \return the control register value
 */
uint vfat_controller::getCChipCr2(){

  uint z = cr2Z->currentItem() ;
  uint w = cr2W->currentItem() ;

  if (z==1){ z = 16 ; }
  else if (z==2){ z = 32 ; }
  else if (z==3){ z = 48 ; }
  else if (z==4){ z = 64 ; }
  else if (z==5){ z = 80 ; }
  else if (z==6){ z = 96 ; }
  else if (z==7){ z = 112 ; }
  else if (z==8){ z = 128 ; }
  else if (z==9){ z = 144 ; }
  else if (z==10){ z = 160 ; }
  else if (z==11){ z = 176 ; }
  else if (z==12){ z = 192 ; }
  else if (z==13){ z = 208 ; }
  else if (z==14){ z = 224 ; }
  else if (z==15){ z = 240 ; }

  uint value = z + w  ;

  return (value) ;
}

/** \brief Get the value of channel register
 * Get the value of channel register
 * \return the control register value
 */
uint vfat_controller::getCChipCr3(){

  uint ao = cr3CoincidenceSel->currentItem() ;
  uint lo = cr3OutputCondition->currentItem() ;
  uint l1 = cr3Inverted->currentItem() ;
  uint o2 = cr3OutputGrouping->currentItem() ;

  if (ao==1){ ao = 64 ; }
  else if (ao==2){ ao = 128 ; }
  else if (ao==3){ ao = 192 ; }
  if (lo==1){ lo = 16 ; }
  else if (lo==2){ lo = 32 ; }
  else if (lo==3){ lo = 48 ; }
  if (l1==1){ l1 = 8 ; }

  uint value = ao + lo + l1 + o2 ;

  return (value) ;
}

/** \brief Get the value of channel register
 * Get the value of channel register
 * \return the control register value
 */
uint vfat_controller::getCChipCr4(){

  uint cl = cr4Edge->currentItem() ;
  uint t = cr4MSPulseLength->currentItem() ;
  uint co = cr4CounterSel->currentItem() ;

  if (cl==1){ cl = 128 ; }
  if (t==1){ t = 16 ; }
  else if (t==2){ t = 32 ; }
  else if (t==3){ t = 48 ; }
  else if (t==4){ t = 64 ; }
  else if (t==5){ t = 80 ; }
  else if (t==6){ t = 96 ; }
  else if (t==7){ t = 112 ; }

  uint value = cl + t + co  ;

  return (value) ;
}

/** \brief Get the value of impedance matching
 * Get the value of impedance matching
 * \return the impedance register value
 */
uint vfat_controller::getCChipRes(){

  uint res = impedance->currentItem() ;

  return (res) ;
}

/** \brief Get the value of mask register
 * Get the value of mask register
 * \param index - mask register
 * \return the mask register value
 */
uint vfat_controller::getCChipMask( int index ) {

  uint value = 0 ;

  if(index==0){
    if(cchip_mask1->isChecked()){ value+=128 ; }
    if(cchip_mask2->isChecked()){ value+=64 ; }
    if(cchip_mask3->isChecked()){ value+=32 ; }
    if(cchip_mask4->isChecked()){ value+=16 ; }
    if(cchip_mask5->isChecked()){ value+=8 ; }
    if(cchip_mask6->isChecked()){ value+=4 ; }
    if(cchip_mask7->isChecked()){ value+=2 ; }
    if(cchip_mask8->isChecked()){ value+=1 ; }
  }
  if(index==1){
    if(cchip_mask9->isChecked()){ value+=128 ; }
    if(cchip_mask10->isChecked()){ value+=64 ; }
    if(cchip_mask11->isChecked()){ value+=32 ; }
    if(cchip_mask12->isChecked()){ value+=16 ; }
    if(cchip_mask13->isChecked()){ value+=8 ; }
    if(cchip_mask14->isChecked()){ value+=4 ; }
    if(cchip_mask15->isChecked()){ value+=2 ; }
    if(cchip_mask16->isChecked()){ value+=1 ; }
  }
  if(index==2){
    if(cchip_mask17->isChecked()){ value+=128 ; }
    if(cchip_mask18->isChecked()){ value+=64 ; }
    if(cchip_mask19->isChecked()){ value+=32 ; }
    if(cchip_mask20->isChecked()){ value+=16 ; }
    if(cchip_mask21->isChecked()){ value+=8 ; }
    if(cchip_mask22->isChecked()){ value+=4 ; }
    if(cchip_mask23->isChecked()){ value+=2 ; }
    if(cchip_mask24->isChecked()){ value+=1 ; }
  }
  if(index==3){
    if(cchip_mask25->isChecked()){ value+=128 ; }
    if(cchip_mask26->isChecked()){ value+=64 ; }
    if(cchip_mask27->isChecked()){ value+=32 ; }
    if(cchip_mask28->isChecked()){ value+=16 ; }
    if(cchip_mask29->isChecked()){ value+=8 ; }
    if(cchip_mask30->isChecked()){ value+=4 ; }
    if(cchip_mask31->isChecked()){ value+=2 ; }
    if(cchip_mask32->isChecked()){ value+=1 ; }
  }
  if(index==4){
    if(cchip_mask33->isChecked()){ value+=128 ; }
    if(cchip_mask34->isChecked()){ value+=64 ; }
    if(cchip_mask35->isChecked()){ value+=32 ; }
    if(cchip_mask36->isChecked()){ value+=16 ; }
    if(cchip_mask37->isChecked()){ value+=8 ; }
    if(cchip_mask38->isChecked()){ value+=4 ; }
    if(cchip_mask39->isChecked()){ value+=2 ; }
    if(cchip_mask40->isChecked()){ value+=1 ; }

  }
  if(index==5){
    if(cchip_mask41->isChecked()){ value+=128 ; }
    if(cchip_mask42->isChecked()){ value+=64 ; }
    if(cchip_mask43->isChecked()){ value+=32 ; }
    if(cchip_mask44->isChecked()){ value+=16 ; }
    if(cchip_mask45->isChecked()){ value+=8 ; }
    if(cchip_mask46->isChecked()){ value+=4 ; }
    if(cchip_mask47->isChecked()){ value+=2 ; }
    if(cchip_mask48->isChecked()){ value+=1 ; }
  }
  if(index==6){
    if(cchip_mask49->isChecked()){ value+=128 ; }
    if(cchip_mask50->isChecked()){ value+=64 ; }
    if(cchip_mask51->isChecked()){ value+=32 ; }
    if(cchip_mask52->isChecked()){ value+=16 ; }
    if(cchip_mask53->isChecked()){ value+=8 ; }
    if(cchip_mask54->isChecked()){ value+=4 ; }
    if(cchip_mask55->isChecked()){ value+=2 ; }
    if(cchip_mask56->isChecked()){ value+=1 ; }

  }
  if(index==7){
    if(cchip_mask57->isChecked()){ value+=128 ; }
    if(cchip_mask58->isChecked()){ value+=64 ; }
    if(cchip_mask59->isChecked()){ value+=32 ; }
    if(cchip_mask60->isChecked()){ value+=16 ; }
    if(cchip_mask61->isChecked()){ value+=8 ; }
    if(cchip_mask62->isChecked()){ value+=4 ; }
    if(cchip_mask63->isChecked()){ value+=2 ; }
    if(cchip_mask64->isChecked()){ value+=1 ; }
  }
  if(index==8){
    if(cchip_mask65->isChecked()){ value+=128 ; }
    if(cchip_mask66->isChecked()){ value+=64 ; }
    if(cchip_mask67->isChecked()){ value+=32 ; }
    if(cchip_mask68->isChecked()){ value+=16 ; }
    if(cchip_mask69->isChecked()){ value+=8 ; }
    if(cchip_mask70->isChecked()){ value+=4 ; }
    if(cchip_mask71->isChecked()){ value+=2 ; }
    if(cchip_mask72->isChecked()){ value+=1 ; }
  }
  if(index==9){
    if(cchip_mask73->isChecked()){ value+=128 ; }
    if(cchip_mask74->isChecked()){ value+=64 ; }
    if(cchip_mask75->isChecked()){ value+=32 ; }
    if(cchip_mask76->isChecked()){ value+=16 ; }
    if(cchip_mask77->isChecked()){ value+=8 ; }
    if(cchip_mask78->isChecked()){ value+=4 ; }
    if(cchip_mask79->isChecked()){ value+=2 ; }
    if(cchip_mask80->isChecked()){ value+=1 ; }
  }
  return (value) ;
}

/** \brief Set the value of channel register
 * Set the value of channel register
 * \param value to be set
 */
void vfat_controller::setCChipCr0( uint iValue ){

  // All the bits are not used.
  if (iValue>=64){
    std::cout << SET_FORECOLOR_RED << "Unexpected Control Register 0 Value (0x" << std::hex << iValue << ") -> Masking Unused Bits..." << SET_DEFAULT_COLORS << std::endl ;
    if(iValue>=128){ iValue-=128 ; }
    if(iValue>=64){ iValue-=64 ; }
  }

  if (iValue>=16){
    if(iValue>=48){ 
      cr0CTReset->setCurrentItem(3) ; 
      iValue-=48 ;
    }
    else if(iValue>=32){
      cr0CTReset->setCurrentItem(2) ;
      iValue-=32 ;
    }
    else if(iValue>=16){
      cr0CTReset->setCurrentItem(1) ;
      iValue-=16 ;
    }
  }
  else{ cr0CTReset->setCurrentItem(0) ; }

  // All the bits are not used.
  if (iValue>=6){
    std::cout << SET_FORECOLOR_RED << "Unexpected Control Register 0 Value (0x" << std::hex << iValue << ") -> Masking Unused Bits..." << SET_DEFAULT_COLORS << std::endl ;
    if(iValue>=8){ iValue-=8 ; }
    if(iValue>=6){ iValue-=2 ; }
  }

  if (iValue>=2){
    if(iValue>=4){
      cr0Sync->setCurrentItem(2) ;
      iValue-=4 ;
    }
    else if(iValue>=2){
      cr0Sync->setCurrentItem(1) ;
      iValue-=2 ;
    }
  }
  else{ cr0Sync->setCurrentItem(0) ; }
  if (iValue>=1){
    cr0Run->setCurrentItem(1) ; 
    iValue-=1 ;
  }
  else{ cr0Run->setCurrentItem(0) ; }
}

/** \brief Set the value of channel register
 * Set the value of channel register
 * \param value to be set
 */
void vfat_controller::setCChipCr1( uint iValue ){

  if (iValue>=32){
    if(iValue>=224){ 
      cr1Neighbors->setCurrentItem(7) ; 
      iValue-=224 ;
    }
    else if(iValue>=192){
      cr1Neighbors->setCurrentItem(6) ;
      iValue-=192 ;
    }
    else if(iValue>=160){
      cr1Neighbors->setCurrentItem(5) ;
     iValue-=160 ;
    }
    if(iValue>=128){ 
      cr1Neighbors->setCurrentItem(4) ; 
      iValue-=128 ;
    }
    else if(iValue>=96){
      cr1Neighbors->setCurrentItem(3) ;
      iValue-=96 ;
    }
    else if(iValue>=64){
      cr1Neighbors->setCurrentItem(2) ;
      iValue-=64 ;
    }
    else if(iValue>=32){
      cr1Neighbors->setCurrentItem(1) ;
      iValue-=32 ;
    }
  }
  else{ cr1Neighbors->setCurrentItem(0) ; }

  if (iValue>=16){
    cr1NP->setCurrentItem(1) ; 
    iValue-=16 ;

    cr1NP_activated(1) ; // Update the fields
  }
  else{

    cr1NP->setCurrentItem(0) ;
    cr1NP_activated(0) ; // Update the fields
  }

  /* TEMP DISABLE

  // Check if the value is too much
  if((cr1NP->currentText()).toUInt()<iValue){

    sscanf(cr1NP->currentText(), "%d", &iValue) ;
    std::cout << SET_FORECOLOR_RED << "V value greater than NP, setting it equal to NP." << SET_DEFAULT_COLORS << std::endl ;
  }

  */

  cr1V->setCurrentItem(iValue) ;
}

/** \brief Set the value of channel register
 * Set the value of channel register
 * \param value to be set
 */
void vfat_controller::setCChipCr2( uint iValue ){

  if (iValue>=16){

    /* TEMP DISABLE

    // Check if the value is too much
    if((cr1NP->currentItem()==1) && ((iValue & 0xF0)> 0x70)){

      iValue &= 0x7F ;
      std::cout << SET_FORECOLOR_RED << "Z value greater than NP allows, setting it equal to the maximum." << SET_DEFAULT_COLORS << std::endl ;
    }

    */

    if(iValue>=240){ 
      cr2Z->setCurrentItem(15) ; 
      iValue-=240 ;
    }
    else if(iValue>=224){
      cr2Z->setCurrentItem(14) ;
      iValue-=224 ;
    }
    else if(iValue>=208){
      cr2Z->setCurrentItem(13) ;
      iValue-=208 ;
    }
    else if(iValue>=192){
      cr2Z->setCurrentItem(12) ;
      iValue-=192 ;
    }
    if(iValue>=176){ 
      cr2Z->setCurrentItem(11) ; 
      iValue-=176 ;
    }
    else if(iValue>=160){
      cr2Z->setCurrentItem(10) ;
      iValue-=160 ;
    }
    else if(iValue>=144){
      cr2Z->setCurrentItem(9) ;
      iValue-=144 ;
    }
    else if(iValue>=128){
      cr2Z->setCurrentItem(8) ;
      iValue-=128 ;
    }
    if(iValue>=112){ 
      cr2Z->setCurrentItem(7) ; 
      iValue-=112 ;
    }
    else if(iValue>=96){
      cr2Z->setCurrentItem(6) ;
      iValue-=96 ;
    }
    else if(iValue>=80){
      cr2Z->setCurrentItem(5) ;
      iValue-=80 ;
    }
    if(iValue>=64){ 
      cr2Z->setCurrentItem(4) ; 
      iValue-=64 ;
    }
    else if(iValue>=48){
      cr2Z->setCurrentItem(3) ;
      iValue-=48 ;
    }
    else if(iValue>=32){
      cr2Z->setCurrentItem(2) ;
      iValue-=32 ;
    }
    else if(iValue>=16){
      cr2Z->setCurrentItem(1) ;
      iValue-=16 ;
    }
  }
  else{ cr2Z->setCurrentItem(0) ; }

  /* TEMP DISABLE

  // Check if the value is too much
  if((cr1NP->currentText()).toUInt()<iValue){

    sscanf(cr1NP->currentText(), "%d", &iValue) ;
    std::cout << SET_FORECOLOR_RED << "W value greater than NP, setting it equal to NP." << SET_DEFAULT_COLORS << std::endl ;
  }

  */

  cr2W->setCurrentItem(iValue) ;
}

/** \brief Set the value of channel register
 * Set the value of channel register
 * \param value to be set
 */
void vfat_controller::setCChipCr3( uint iValue ){

  if (iValue>=64){
    if(iValue>=192){ 
      cr3CoincidenceSel->setCurrentItem(3) ; 
      iValue-=192 ;
    }
    else if(iValue>=128){
      cr3CoincidenceSel->setCurrentItem(2) ;
      iValue-=128 ;
    }
    else if(iValue>=64){
      cr3CoincidenceSel->setCurrentItem(1) ;
      iValue-=64 ;
    }
  }
  else{ cr3CoincidenceSel->setCurrentItem(0) ; }

  if (iValue>=16){
    if(iValue>=48){ 
      cr3OutputCondition->setCurrentItem(3) ; 
      iValue-=48 ;
    }
    else if(iValue>=32){
      cr3OutputCondition->setCurrentItem(2) ;
      iValue-=32 ;
    }
    else if(iValue>=16){
      cr3OutputCondition->setCurrentItem(1) ;
      iValue-=16 ;
    }
  }
  else{ cr3OutputCondition->setCurrentItem(0) ; }

  if (iValue>=8){
    cr3Inverted->setCurrentItem(1) ; 
    iValue-=8 ;
  }
  else{ cr3Inverted->setCurrentItem(0) ; }

  // All the bits are not used.
  if (iValue>4){
    std::cout << SET_FORECOLOR_RED << "Unexpected Control Register 3 Value (0x" << std::hex << iValue << ") -> Masking Unused Bits..." << SET_DEFAULT_COLORS << std::endl ;
    if (iValue=6){ iValue-=2 ; }
    if (iValue=5){ iValue-=1 ; } 
  }

  cr3OutputGrouping->setCurrentItem(iValue) ;
}

/** \brief Set the value of channel register
 * Set the value of channel register
 * \param value to be set
 */
void vfat_controller::setCChipCr4( uint iValue ){

  if (iValue>=128){
    cr4Edge->setCurrentItem(1) ; 
    iValue-=128 ;
  }
  else{ cr4Edge->setCurrentItem(0) ; }
  if (iValue>=16){
    if(iValue>=112){ 
      cr4MSPulseLength->setCurrentItem(7) ; 
      iValue-=112 ;
    }
    else if(iValue>=96){
      cr4MSPulseLength->setCurrentItem(6) ;
      iValue-=96 ;
    }
    else if(iValue>=80){
      cr4MSPulseLength->setCurrentItem(5) ;
      iValue-=80 ;
    }
    if(iValue>=64){ 
      cr4MSPulseLength->setCurrentItem(4) ; 
      iValue-=64 ;
    }
    else if(iValue>=48){
      cr4MSPulseLength->setCurrentItem(3) ;
      iValue-=48 ;
    }
    else if(iValue>=32){
      cr4MSPulseLength->setCurrentItem(2) ;
      iValue-=32 ;
    }
    else if(iValue>=16){
      cr4MSPulseLength->setCurrentItem(1) ;
      iValue-=16 ;
    }
  }
  else{ cr4MSPulseLength->setCurrentItem(0) ; }

  cr4CounterSel->setCurrentItem(iValue) ;
}

/** \brief Set the value of impedance matching
 * Set the value of impedance matching
 * \param value to be set
 */
void vfat_controller::setCChipRes( uint iValue ){

  // All the bits are not used.
  if (iValue>7){
    std::cout << SET_FORECOLOR_RED << "Unexpected Impedance Value (0x" << std::hex << iValue << ") -> Masking Unused Bits..." << SET_DEFAULT_COLORS << std::endl ;
    if(iValue>=128){ iValue-=128 ; }
    if(iValue>=64){ iValue-=64 ; }
    if(iValue>=32){ iValue-=32 ; }
    if(iValue>=16){ iValue-=16 ; }
    if(iValue>=8){ iValue-=8 ; }
  }
  impedance->setCurrentItem(iValue) ;
}

/** \brief Set the value of the mask register
 * Set the value of the mask register
 * \param index - mask register
 * \param iValue - value to be set
 */
void vfat_controller::setCChipMask( int index, uint iValue ) {

  cchip_unmask( index ) ;

  if (iValue>=128){
    if(index==0){ cchip_mask1->setChecked(true) ; }
    if(index==1){ cchip_mask9->setChecked(true) ; }
    if(index==2){ cchip_mask17->setChecked(true) ; }
    if(index==3){ cchip_mask25->setChecked(true) ; }
    if(index==4){ cchip_mask33->setChecked(true) ; }
    if(index==5){ cchip_mask41->setChecked(true) ; }
    if(index==6){ cchip_mask49->setChecked(true) ; }
    if(index==7){ cchip_mask57->setChecked(true) ; }
    if(index==8){ cchip_mask65->setChecked(true) ; }
    if(index==9){ cchip_mask73->setChecked(true) ; }
    iValue-=128 ;
  }
  if (iValue>=64){
    if(index==0){ cchip_mask2->setChecked(true) ; }
    if(index==1){ cchip_mask10->setChecked(true) ; }
    if(index==2){ cchip_mask18->setChecked(true) ; }
    if(index==3){ cchip_mask26->setChecked(true) ; }
    if(index==4){ cchip_mask34->setChecked(true) ; }
    if(index==5){ cchip_mask42->setChecked(true) ; }
    if(index==6){ cchip_mask50->setChecked(true) ; }
    if(index==7){ cchip_mask58->setChecked(true) ; }
    if(index==8){ cchip_mask66->setChecked(true) ; }
    if(index==9){ cchip_mask74->setChecked(true) ; }
    iValue-=64 ;
  }
  if (iValue>=32){
    if(index==0){ cchip_mask3->setChecked(true) ; }
    if(index==1){ cchip_mask11->setChecked(true) ; }
    if(index==2){ cchip_mask19->setChecked(true) ; }
    if(index==3){ cchip_mask27->setChecked(true) ; }
    if(index==4){ cchip_mask35->setChecked(true) ; }
    if(index==5){ cchip_mask43->setChecked(true) ; }
    if(index==6){ cchip_mask51->setChecked(true) ; }
    if(index==7){ cchip_mask59->setChecked(true) ; }
    if(index==8){ cchip_mask67->setChecked(true) ; }
    if(index==9){ cchip_mask75->setChecked(true) ; }
    iValue-=32 ;
  }
  if (iValue>=16){
    if(index==0){ cchip_mask4->setChecked(true) ; }
    if(index==1){ cchip_mask12->setChecked(true) ; }
    if(index==2){ cchip_mask20->setChecked(true) ; }
    if(index==3){ cchip_mask28->setChecked(true) ; }
    if(index==4){ cchip_mask36->setChecked(true) ; }
    if(index==5){ cchip_mask44->setChecked(true) ; }
    if(index==6){ cchip_mask52->setChecked(true) ; }
    if(index==7){ cchip_mask60->setChecked(true) ; }
    if(index==8){ cchip_mask68->setChecked(true) ; }
    if(index==9){ cchip_mask76->setChecked(true) ; }
    iValue-=16 ;
  }
  if (iValue>=8){
    if(index==0){ cchip_mask5->setChecked(true) ; } 
    if(index==1){ cchip_mask13->setChecked(true) ; }
    if(index==2){ cchip_mask21->setChecked(true) ; }
    if(index==3){ cchip_mask29->setChecked(true) ; }
    if(index==4){ cchip_mask37->setChecked(true) ; }
    if(index==5){ cchip_mask45->setChecked(true) ; }
    if(index==6){ cchip_mask53->setChecked(true) ; }
    if(index==7){ cchip_mask61->setChecked(true) ; }
    if(index==8){ cchip_mask69->setChecked(true) ; }
    if(index==9){ cchip_mask77->setChecked(true) ; }
    iValue-=8 ;
  }
  if (iValue>=4){
    if(index==0){ cchip_mask6->setChecked(true) ; }
    if(index==1){ cchip_mask14->setChecked(true) ; }
    if(index==2){ cchip_mask22->setChecked(true) ; }
    if(index==3){ cchip_mask30->setChecked(true) ; }
    if(index==4){ cchip_mask38->setChecked(true) ; }
    if(index==5){ cchip_mask46->setChecked(true) ; }
    if(index==6){ cchip_mask54->setChecked(true) ; }
    if(index==7){ cchip_mask62->setChecked(true) ; }
    if(index==8){ cchip_mask70->setChecked(true) ; }
    if(index==9){ cchip_mask78->setChecked(true) ; }
    iValue-=4 ;
  }
  if (iValue>=2){
    if(index==0){ cchip_mask7->setChecked(true) ; }
    if(index==1){ cchip_mask15->setChecked(true) ; }
    if(index==2){ cchip_mask23->setChecked(true) ; }
    if(index==3){ cchip_mask31->setChecked(true) ; }
    if(index==4){ cchip_mask39->setChecked(true) ; }
    if(index==5){ cchip_mask47->setChecked(true) ; }
    if(index==6){ cchip_mask55->setChecked(true) ; }
    if(index==7){ cchip_mask63->setChecked(true) ; }
    if(index==8){ cchip_mask71->setChecked(true) ; }
    if(index==9){ cchip_mask79->setChecked(true) ; }
    iValue-=2 ;
  }
  if (iValue>=1){
    if(index==0){ cchip_mask8->setChecked(true) ; }
    if(index==1){ cchip_mask16->setChecked(true) ; }
    if(index==2){ cchip_mask24->setChecked(true) ; }
    if(index==3){ cchip_mask32->setChecked(true) ; }
    if(index==4){ cchip_mask40->setChecked(true) ; }
    if(index==5){ cchip_mask48->setChecked(true) ; }
    if(index==6){ cchip_mask56->setChecked(true) ; }
    if(index==7){ cchip_mask64->setChecked(true) ; }
    if(index==8){ cchip_mask72->setChecked(true) ; }
    if(index==9){ cchip_mask80->setChecked(true) ; }
    iValue-=1 ;
  }
}

/** \brief Read cchipDescription values from file
 * Read cchipDescription values from file 
 */
void vfat_controller::loadSingleXML2_pressed(){

  std::cout << SET_FORECOLOR_BLUE << "Load description values from XML file" << SET_DEFAULT_COLORS << std::endl ;
  QString fn ;
       
  fn = QFileDialog::getOpenFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;

  singleFileName2->setText (fn) ;
   
  if (! fn.isEmpty ()) {
   
    try {

      TotemFecFactory fecFactory ;

      deviceVector vDevice ;
      fecFactory.getFecDeviceDescriptions(vDevice, fn.latin1()) ;

      for (deviceVector::iterator device = vDevice.begin() ; device != vDevice.end() ; device ++) {

        deviceDescription *deviced = *device ;
        totemCChipDescription *cchipDevice    = NULL ;

        // Find the correct item
        switch (deviced->getDeviceType()) {
        case CCHIP:

          cchipDevice = (totemCChipDescription *)deviced ;
          cchipDevice->display() ;

          setCChipCr0((uint)cchipDevice->getControl0()) ;
          setCChipCr1((uint)cchipDevice->getControl1()) ;
          setCChipCr2((uint)cchipDevice->getControl2()) ;
          setCChipCr3((uint)cchipDevice->getControl3()) ;
          setCChipCr4((uint)cchipDevice->getControl4()) ;
          setCChipRes((uint)cchipDevice->getRes()) ;

          setCChipMask(0,(uint)cchipDevice->getMask0()) ;
          setCChipMask(1,(uint)cchipDevice->getMask1()) ;
          setCChipMask(2,(uint)cchipDevice->getMask2()) ;
          setCChipMask(3,(uint)cchipDevice->getMask3()) ;
          setCChipMask(4,(uint)cchipDevice->getMask4()) ;
          setCChipMask(5,(uint)cchipDevice->getMask5()) ;
          setCChipMask(6,(uint)cchipDevice->getMask6()) ;
          setCChipMask(7,(uint)cchipDevice->getMask7()) ;
          setCChipMask(8,(uint)cchipDevice->getMask8()) ;
          setCChipMask(9,(uint)cchipDevice->getMask9()) ;
          setCChipRes((uint)cchipDevice->getRes()) ;
	  break ;

        default:

	  std::cout << SET_FORECOLOR_RED << "Non-CCHIP device found." << SET_DEFAULT_COLORS << std::endl ;
	}	    	    
      }	   
      //FecFactory::deleteVectorI (vDevice) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Error during the download from file", e) ; }      
  }
  else { ErrorMessage ("Please set the filename before loading it") ; }

}

/** \brief Write the fields' values to file
 * Write the fields' values to file
 */
void vfat_controller::saveSingleXML2_pressed(){

  std::cout << SET_FORECOLOR_BLUE << "Save description values to XML file" << SET_DEFAULT_COLORS << std::endl ;
  deviceVector vDevice ;
  totemCChipDescription *device = NULL ;

  device = new totemCChipDescription ( (tscType8)getCChipCr0(), (tscType8)getCChipCr1(), (tscType8)getCChipCr2(), (tscType8)getCChipCr3(), (tscType8)getCChipCr4(),
				       (tscType8)0x00, (tscType8)0x00, (tscType8)0x00, (tscType8)0x00, (tscType8)0x00,
				       (tscType8)getCChipMask(0), (tscType8)getCChipMask(1), (tscType8)getCChipMask(2), (tscType8)getCChipMask(3), (tscType8)getCChipMask(4),
				       (tscType8)getCChipMask(5), (tscType8)getCChipMask(6), (tscType8)getCChipMask(7), (tscType8)getCChipMask(8), (tscType8)getCChipMask(9), (tscType8)getCChipRes() ) ;
  device->display() ;
  vDevice.push_back(device) ;

  // Try to write
  QString fn = QFileDialog::getSaveFileName( QString::null, tr( "XML-Files (*.xml);;All Files (*)" ), this ) ;
  singleFileName2->setText (fn) ;
      
  if (!fn.isEmpty ()) {

    if (!fn.endsWith( ".xml" )){ fn.append(".xml") ; }
      
    try {

      TotemFecFactory fecFactory ;
      fecFactory.setFecDeviceDescriptions (vDevice, fn.latin1()) ;
    }
    catch (FecExceptionHandler &e) { ErrorMessage ("Error while saving to file", e) ; }
  }
  else { ErrorMessage ("Error: unable to save, the output buffer is not initialised") ; }

  FecFactory::deleteVectorI (vDevice) ;
}

void vfat_controller::fecSlots3_activated( int sel ){

  refreshCcuAddresses3() ;
}

void vfat_controller::ccuAddresses3_activated( int sel ){

  refreshI2cChannels3() ;
}

void vfat_controller::i2cChannels3_activated( int sel ){

  refreshDeviceAddresses3() ;
}

void vfat_controller::deviceAddresses3_activated( int sel ){

  readCChipId() ;
}

void vfat_controller::refreshCcuAddresses3(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh CCUs" << SET_DEFAULT_COLORS << std::endl ;
#endif

  ccuAddresses3->clear() ;
  i2cChannels3->clear() ;
  deviceAddresses3->clear() ;
  cchipChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (cchipAddresses_.size()>0 && sscanf (fecSlots3->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;
   
    for (int i=0;i<cchipAddresses_.size();i++){

#ifdef DEBUG_VFAT_CONTROLLER
      //ss << "0x" << std::hex << (int)getFecKey(cchipAddresses_.at(i)) << std::dec ;
      ss << "FEC RING " << (int)getFecKey(cchipAddresses_.at(i)) << " " << (int)getRingKey(cchipAddresses_.at(i)) << std::endl ;
      std::cout << SET_FORECOLOR_CYAN << SET_DEFAULT_COLORS << ss.str() ;
#endif

      //std::cout << "FEC " << ss.str() << " / " << fecSlots->currentText().latin1() << std::endl ;

      if(fecSlot==getFecKey(cchipAddresses_.at(i)) && ringSlot==getRingKey(cchipAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getCcuKey(cchipAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	bool addItem = true ;

	for(int j=0;j<ccuAddresses3->count();j++){

	  if(ccuAddresses3->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	}

	if(addItem){ ccuAddresses3->insertItem(ss.str()) ; }

      }
    }
    ss.flush() ;

    if(ccuAddresses3->count()>0){

      ccuAddresses3->setCurrentItem(0) ; 
      refreshI2cChannels3() ;
    }
  }
}

void vfat_controller::refreshI2cChannels3(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh channels" << SET_DEFAULT_COLORS << std::endl ;
#endif

  i2cChannels3->clear() ;
  deviceAddresses3->clear() ;
  cchipChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (cchipAddresses_.size()>0 && sscanf (fecSlots3->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;
   
    for (int i=0;i<cchipAddresses_.size();i++){

      ss.str("") ;
      ss << "0x" << std::hex << (int)getCcuKey(cchipAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

      if(ccuAddresses3->currentText()==(QString)ss.str() && fecSlot==getFecKey(cchipAddresses_.at(i)) && ringSlot==getRingKey(cchipAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getChannelKey(cchipAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
	std::cout << SET_FORECOLOR_CYAN << "CH " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

	bool addItem = true ;

	for(int j=0;j<i2cChannels3->count();j++){

	  if(i2cChannels3->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	}

	if(addItem){ i2cChannels3->insertItem(ss.str()) ; }

      }
    }
      
    ss.flush() ;

    if(i2cChannels3->count()){

      i2cChannels3->setCurrentItem(0) ; 
      refreshDeviceAddresses3() ;
    }
  }
}

void vfat_controller::refreshDeviceAddresses3(){

#ifdef DEBUG_VFAT_CONTROLLER
  std::cout << SET_FORECOLOR_CYAN << "---> refresh devices" << SET_DEFAULT_COLORS << std::endl ;
#endif

  deviceAddresses3->clear() ;
  cchipChipidT->clear() ;

  keyType fecSlot = 0, ringSlot = 0 ;

  if (cchipAddresses_.size()>0 && sscanf (fecSlots3->currentText(), "%d,%d", &fecSlot, &ringSlot) ){

    std::stringstream ss ;

#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "------------------------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
    std::cout << SET_FORECOLOR_CYAN << "Corresponding CCHIPS for channel selected:" << SET_DEFAULT_COLORS << std::endl ; 
#endif
   
    for (int i=0;i<cchipAddresses_.size();i++){

#ifdef DEBUG_VFAT_CONTROLLER
      keyType index = cchipAddresses_.at(i) ;

      std::cout << SET_FORECOLOR_CYAN << "FEC (" 
		<< std::hex << (int)getFecKey(index) << ","
		<< std::hex << (int)getRingKey(index) << ") CCU 0x" 
		<< std::hex << (int)getCcuKey(index) << " CHANNEL 0x" 
		<< std::hex << (int)getChannelKey(index) << " ADDRESS 0x" 
		<< std::hex << (int)getAddressKey(index) << SET_DEFAULT_COLORS << std::endl ;
#endif

      ss.str("") ;
      ss << "0x" << std::hex << (int)getCcuKey(cchipAddresses_.at(i)) << std::dec ;

#ifdef DEBUG_VFAT_CONTROLLER
      std::cout << SET_FORECOLOR_CYAN << "CCU " << ss.str() << SET_DEFAULT_COLORS << std::endl ;
#endif

      if(ccuAddresses3->currentText()==(QString)ss.str() && fecSlot==getFecKey(cchipAddresses_.at(i)) && ringSlot==getRingKey(cchipAddresses_.at(i))){

	ss.str("") ;
	ss << "0x" << std::hex << (int)getChannelKey(cchipAddresses_.at(i)) << std::dec ;

	if(i2cChannels3->currentText()==(QString)ss.str()){

	  ss.str("") ;
	  ss << "0x" << std::hex << (int)getAddressKey(cchipAddresses_.at(i)) << std::dec ;

	  bool addItem = true ;

	  for(int j=0;j<deviceAddresses3->count();j++){

	    if(deviceAddresses3->text(j)==(QString)ss.str()) { addItem=false ; break ; } 
	  }

	  if(addItem){ deviceAddresses3->insertItem(ss.str()) ; }      

	}
      }
    }
#ifdef DEBUG_VFAT_CONTROLLER
    std::cout << SET_FORECOLOR_CYAN << "------------------------------------------------------------------------" << SET_DEFAULT_COLORS << std::endl ;
#endif
     
    ss.flush() ;

    if(deviceAddresses3->count()){

      deviceAddresses3->setCurrentItem(0) ; 
      readCChipId() ;
    }
  }
}

