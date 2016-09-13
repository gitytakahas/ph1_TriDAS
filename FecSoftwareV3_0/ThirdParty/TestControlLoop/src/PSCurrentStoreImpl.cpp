#include "PSCurrentStoreImpl.h"

#include <qwidget.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qvalidator.h>

PSCurrentStoreImpl::PSCurrentStoreImpl(TestResultsImpl *parent, GUISupervisor *guiSupervisor) : PSCurrentStore() { 
    resultsParent = parent;
    powerParent = NULL;
    setCaption("PS Currents Store (after tests)");
    init(guiSupervisor);
}

PSCurrentStoreImpl::PSCurrentStoreImpl(PowerUpStepsImpl *parent, GUISupervisor *guiSupervisor) : PSCurrentStore(parent, "PS Currents Store (before power up)", true) { 
    powerParent = parent; 
    resultsParent = NULL;
    setCaption("PS Currents Store (before power up)");
    init(guiSupervisor);
}

void PSCurrentStoreImpl::nextStep() {
    // record os ps currents
    for(QValueVector<QLineEdit *>::iterator it = vLineEdit.begin() ; it < vLineEdit.end() ; it++) {
	supervisor->recordPSCurrent((*it)->text());
    }	
 
    if(caption() == "PS Currents Store (before power up)")
	powerParent->nextStep();
    else
	resultsParent->generateXml();
    okButtonClose = true;
    close();
}

void PSCurrentStoreImpl::closeEvent(QCloseEvent *e) {
    // if all fileds are filed, calls nextStep
    if(okButton->isEnabled()) {
	e->accept();
	if(!okButtonClose) {
	    nextStep();    
	}
    }
    else exit(1);    // else exit
}

void PSCurrentStoreImpl::addRecordLine(int fecSlot, int ringSlot, int numLine) {
    QLabel *textLabel = new QLabel( this, "textLabel" );
    QLineEdit *lineEdit = new QLineEdit( this, "lineEdit" );
    
    QRegExp re( "[0-9]+(,[0-9]+|)(A|mA)?" );
    QRegExpValidator *val = new QRegExpValidator( re, 0 );
    lineEdit->setValidator(val);
    
    textLabel->setText("FEC " + QString::number(fecSlot) + " Ring " + QString::number(ringSlot) + " Power Supply currents :");

    dynamicLayout->addWidget( textLabel, numLine, 0 );
    dynamicLayout->addWidget( lineEdit, numLine, 1 );
    
    connect( lineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( checkAllFields() ) );
    
    vLineEdit.push_back(lineEdit);
}

void PSCurrentStoreImpl::checkAllFields() {
    bool b = true;
    for(QValueVector<QLineEdit *>::iterator it = vLineEdit.begin() ; it < vLineEdit.end() ; it++) {
	if((*it)->text() == "") b = false;
    }
    if(b) okButton->setEnabled(true);
    else okButton->setEnabled(false);
}

void PSCurrentStoreImpl::init(GUISupervisor *guiSupervisor) {
    supervisor = guiSupervisor;
    okButtonClose = false;
    
    dynamicLayout = new QGridLayout( 0, 1, 1, 0, 6, "dynamicLayout"); 

    QValueVector<int> fecRingSelected = supervisor->getFecRingSelected();
    int numLine = 0;
    for (QValueVector<int>::iterator it = fecRingSelected.begin(); it < fecRingSelected.end() ; it = it+3) { // create a record line (a label + a lineEdit) for each selected fec-ring
	if(*(it+2) == 1) {
	    addRecordLine(*it, *(it+1), numLine);
	    numLine++;
	}
    }
    
    PSCurrentStoreLayout->addMultiCellLayout( dynamicLayout, 0, 0, 0, 2 );
    resize( QSize(380, 143).expandedTo(minimumSizeHint()) );
}
