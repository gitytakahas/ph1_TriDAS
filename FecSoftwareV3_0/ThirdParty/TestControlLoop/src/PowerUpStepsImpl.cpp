#include "PowerUpStepsImpl.h"

#include "PSCurrentStoreImpl.h"
#include "TestResultsImpl.h"

#include <qlabel.h>
#include <qpushbutton.h>

void PowerUpStepsImpl::nextStep() {
    switch(step) {
    case 1:
	step1Label->setText("<b>1/</b> <font color=\"#0000ff\">Switch on Control Power Supply</font>");
	step1Button->setEnabled(false);
	
	if(GUISupervisor::testSelected & psCurrentsRecordMask) {
	    step2Label->setText("<b>2/</b> <font color=\"#ff0000\">Store the Power Supply currents</font>");
	    PSCurrentStoreImpl *ps = new PSCurrentStoreImpl(this, supervisor);
	    ps->show();
	}
	else {
	    step3Button->setEnabled(true);
	    step3Label->setText("<b>3/</b> <font color=\"#ff0000\">Switch on Readout Power Supply</font>");
	    step++;
	}
	
	step++;
	break;
    case 2:	
	step3Button->setEnabled(true);
	step2Label->setText("<b>2/</b> <font color=\"#0000ff\">Store the Power Supply currents</font>");
	step3Label->setText("<b>3/</b> <font color=\"#ff0000\">Switch on Readout Power Supply</font>");
	step++;
	break;
    case 3:
	step3Label->setText("<b>3/</b> <font color=\"#0000ff\">Switch on Readout Power Supply</font>");
	step4Label->setText("<b>4/</b> <font color=\"#ff0000\">Start tests</font>");
	step3Button->setEnabled(false);
	step4Button->setEnabled(true);
	step++;
	break;
    case 4:
	step4Button->setEnabled(false);
	supervisor->powerIsUp();
	close();
	break;
    }
}
