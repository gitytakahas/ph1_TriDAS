#include "DCUThresholdsImpl.h"

#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qvalidator.h>

DCUThresholdsImpl::DCUThresholdsImpl(QWidget *parent, int type, GUISupervisor *guiSupervisor) : DCUThresholds(parent, "DCUThresholds", true) {
    supervisor = guiSupervisor;
    
    // set of regular expression
    QRegExp re( "[0-9]+(,[0-9]+|)" );
    QRegExpValidator *val = new QRegExpValidator( re, 0 );
    ch0MinLineEdit->setValidator(val);
    ch1MinLineEdit->setValidator(val);
    ch2MinLineEdit->setValidator(val);
    ch3MinLineEdit->setValidator(val);
    ch4MinLineEdit->setValidator(val);
    ch5MinLineEdit->setValidator(val);
    ch6MinLineEdit->setValidator(val);
    ch7MinLineEdit->setValidator(val);
    ch0MaxLineEdit->setValidator(val);
    ch1MaxLineEdit->setValidator(val);
    ch2MaxLineEdit->setValidator(val);
    ch3MaxLineEdit->setValidator(val);
    ch4MaxLineEdit->setValidator(val);
    ch5MaxLineEdit->setValidator(val);
    ch6MaxLineEdit->setValidator(val);
    ch7MaxLineEdit->setValidator(val);
    

    
    QStringList list;
    switch(type) {
    case 1:
	setCaption("DCU (on CCU) Thresholds");
	list = supervisor->getDcuOnCcuThr();
	break;
    case 2:
	setCaption("DCU (on modules) Thresholds");
	list = supervisor->getDcuOnModuleThr();
	break;
    }
    
    if(!list.empty()) {
	for(QStringList::iterator it = list.begin() ; it != list.end() ; ++it) {
	    QStringList l = QStringList::split("/", *it);
	    for(QStringList::iterator chIt = l.begin() ; chIt != l.end() ; ++chIt) {
		int channelNum = (*chIt).toInt();
		switch(channelNum) {
		case 0:
		    ch0YButton->setChecked(true);
		    chIt++;
		    ch0MinLineEdit->setText(*chIt);
		    chIt++;
		    ch0MaxLineEdit->setText(*chIt);
		    break;
		case 1:
		    ch1YButton->setChecked(true);
		    chIt++;
		    ch1MinLineEdit->setText(*chIt);
		    chIt++;
		    ch1MaxLineEdit->setText(*chIt);
		    break;
		case 2:
		    ch2YButton->setChecked(true);
		    chIt++;
		    ch2MinLineEdit->setText(*chIt);
		    chIt++;
		    ch2MaxLineEdit->setText(*chIt);
		    break;
		case 3:
		    ch3YButton->setChecked(true);
		    chIt++;
		    ch3MinLineEdit->setText(*chIt);
		    chIt++;
		    ch3MaxLineEdit->setText(*chIt);
		    break;
		case 4:
		    ch4YButton->setChecked(true);
		    chIt++;
		    ch4MinLineEdit->setText(*chIt);
		    chIt++;
		    ch4MaxLineEdit->setText(*chIt);
		    break;
		case 5:
		    ch5YButton->setChecked(true);
		    chIt++;
		    ch5MinLineEdit->setText(*chIt);
		    chIt++;
		    ch5MaxLineEdit->setText(*chIt);
		    break;
		case 6:
		    ch6YButton->setChecked(true);
		    chIt++;
		    ch6MinLineEdit->setText(*chIt);
		    chIt++;
		    ch6MaxLineEdit->setText(*chIt);
		    break;
		case 7:
		    ch7YButton->setChecked(true);
		    chIt++;
		    ch7MinLineEdit->setText(*chIt);
		    chIt++;
		    ch7MaxLineEdit->setText(*chIt);
		    break;
		}
	    }
	}
    }
}	

void DCUThresholdsImpl::clearPage() {
    // Reset all Min values
    ch0MinLineEdit->setText("0");
    ch1MinLineEdit->setText("0");
    ch2MinLineEdit->setText("0");
    ch3MinLineEdit->setText("0");
    ch4MinLineEdit->setText("0");
    ch5MinLineEdit->setText("0");
    ch6MinLineEdit->setText("0");
    ch7MinLineEdit->setText("0");
    // Reset all Max values
    ch0MaxLineEdit->setText("4095");
    ch1MaxLineEdit->setText("4095");
    ch2MaxLineEdit->setText("4095");
    ch3MaxLineEdit->setText("4095");
    ch4MaxLineEdit->setText("4095");
    ch5MaxLineEdit->setText("4095");
    ch6MaxLineEdit->setText("4095");
    ch7MaxLineEdit->setText("4095");
    // Reset all button grp
    if(!ch0NButton->isChecked()) ch0NButton->setChecked(true);
    if(!ch1NButton->isChecked()) ch1NButton->setChecked(true);
    if(!ch2NButton->isChecked()) ch2NButton->setChecked(true);
    if(!ch3NButton->isChecked()) ch3NButton->setChecked(true);
    if(!ch4NButton->isChecked()) ch4NButton->setChecked(true);
    if(!ch5NButton->isChecked()) ch5NButton->setChecked(true);
    if(!ch6NButton->isChecked()) ch6NButton->setChecked(true);
    if(!ch7NButton->isChecked()) ch7NButton->setChecked(true);
}


void DCUThresholdsImpl::saveThr() {
    QStringList list;
    if(ch0YButton->isChecked()) 
	list << "0/" + ch0MinLineEdit->text() + "/" + ch0MaxLineEdit->text();
    if(ch1YButton->isChecked()) 
	list << "1/" + ch1MinLineEdit->text() + "/" + ch1MaxLineEdit->text();
    if(ch2YButton->isChecked()) 
	list << "2/" + ch2MinLineEdit->text() + "/" + ch2MaxLineEdit->text();
    if(ch3YButton->isChecked()) 
	list << "3/" + ch3MinLineEdit->text() + "/" + ch3MaxLineEdit->text();
    if(ch4YButton->isChecked()) 
	list << "4/" + ch4MinLineEdit->text() + "/" + ch4MaxLineEdit->text();
    if(ch5YButton->isChecked()) 
	list << "5/" + ch5MinLineEdit->text() + "/" + ch5MaxLineEdit->text();
    if(ch6YButton->isChecked()) 
	list << "6/" + ch6MinLineEdit->text() + "/" + ch6MaxLineEdit->text();
    if(ch7YButton->isChecked()) 
	list << "7/" + ch7MinLineEdit->text() + "/" + ch7MaxLineEdit->text();
    
    if(caption() == "DCU (on CCU) Thresholds")
	supervisor->setDcuOnCcuThr(list);
    else
	supervisor->setDcuOnModuleThr(list);
    
    close();
}


