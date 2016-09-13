#include "CCUInfoImpl.h"
#include "CCUInfoPage.h"
#include "DCUThresholdsImpl.h"

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qtable.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qvalidator.h>

CCUInfoImpl::CCUInfoImpl(QWidget *parent, GUISupervisor *guiSupervisor) : CCUInfo(parent, "CCUInfo", true) {
    supervisor = guiSupervisor;
    backClose = false;
    
    // set of regular expression
    QRegExp re1( "[0-9]{,2}" );
    QRegExpValidator *val1 = new QRegExpValidator( re1, 0 );
    nbCCULineEdit->setValidator(val1);
    
    QRegExp re2("(0x[0-9a-fA-F]|[0-9a-fA-F])[0-9a-fA-F]?");
    QRegExpValidator *val2 = new QRegExpValidator( re2, 0 );
    addressLineEdit->setValidator(val2);
    
        
    QValueVector<int> fecRingSelected = supervisor->getFecRingSelected();
    for(QValueVector<int>::iterator it = fecRingSelected.begin() ; it < fecRingSelected.end() ; it = it+3) {
	if(*(it+2) == 1) {
	    QString item = QString::number(*it);
	    item += QString(",");
	    item += QString::number(*(it+1));
	    fecRingCB->insertItem( tr( item ) );
	}
    }   
    
    // for possible use
    dcuCB->hide();
    
    currentFecRing ="";
    changePage();
}

void CCUInfoImpl::closeEvent(QCloseEvent *e) {
    e->accept();
    if(!backClose) back();
}

void CCUInfoImpl::clearPage()
{
    nbCCULineEdit->setText("0");
    noChB->animateClick();
    positionLineEdit->setText("");
    addressLineEdit->setText("");
    dcuHardIdLineEdit->setText("");
    clearCCUTable();
}


void CCUInfoImpl::setDcuThreshold()
{
    (new DCUThresholdsImpl(this, 1, supervisor))->show();
}


void CCUInfoImpl::changePage()
{
    if(currentFecRing != "") savePage(currentFecRing);
    
    currentFecRing = fecRingCB->currentText();

    clearPage();
    
    CCUInfoPage *p = supervisor->getCCUInfoPage(currentFecRing);
    
    if(p != NULL) {
	nbCCULineEdit->setText(QString::number(p->getNbCCU()));
	
	int v = p->getCheckBoxGroup();
	switch(v) {
	case 1:
	    allChB->animateClick();
	    break;
	case 2:
	    allExceptChB->animateClick();
	    break;
	case 3:
	    noChB->animateClick();
	    break;
	}
	
	clearCCUTable();
	QValueVector<QString> tab = p->getCCUTable();    
	QValueVector<QString>::iterator it = tab.begin();
#ifdef TOBTEST
	int nbElement = 0;
	for (QValueVector<QString>::iterator itt = tab.begin() ; itt < tab.end() ; itt++) nbElement++;
	int nbRow = nbElement/4;
#else
	int nbRow = tab.count()/4;
#endif
	for(int i = 0 ; i < nbRow ; i++) {
	    addTabEntry((*it), *(it+1), *(it+2), *(it+3));
	    it = it+4;
	}	
    }
}


void CCUInfoImpl::addCCU()
{
    QString position = positionLineEdit->text();
    QString address = addressLineEdit->text();
    QString dcuHardId = dcuHardIdLineEdit->text();
    QString dcu;
    
    if(position != "" && address != "") {
	int selected = getSelectedCheckBox();
	switch(selected) {
	case 1:
	    dcu = "yes";
	    break;
	case 2:
	    if(position != "dummy") dcu = "yes";
	    else dcu = "no";
	    break;
	case 3:
	    dcu = "no";
	    break;
	}
	if(! address.startsWith("0x")) address = "0x" + address;
	addTabEntry(position, address, dcu, dcuHardId);
    }
}


void CCUInfoImpl::deleteCCU()
{
    ccuTable->removeRow(ccuTable->currentRow());
}


void CCUInfoImpl::back()
{
    if(sameConfigChB->isChecked()) sameConfigForAll();
    else savePage(currentFecRing);
    
    backClose = true;
    close();
}

void CCUInfoImpl::allHaveDcu()
{
    allChB->setChecked(true);
    allExceptChB->setChecked(false);
    noChB->setChecked(false);
    updateDcuDistribution();
 }


void CCUInfoImpl::allExceptHaveDcu()
{
    allChB->setChecked(false);
    allExceptChB->setChecked(true);
    noChB->setChecked(false);
    
    updateDcuDistribution();
}


void CCUInfoImpl::noDcu()
{
    allChB->setChecked(false);
    allExceptChB->setChecked(false);
    noChB->setChecked(true);
    
    updateDcuDistribution();
}



void CCUInfoImpl::warning() {
    if(sameConfigChB->isChecked()) {
	fecRingCB->setEnabled(false);
	sameConfigForAll();
    }
    else fecRingCB->setEnabled(true);
}

int CCUInfoImpl::getSelectedCheckBox() {
    if(allChB->isChecked())  return 1;
    else if(allExceptChB->isChecked())  return 2;
    else return 3;    
}

void CCUInfoImpl::updateDcuDistribution()
{
    int selected = getSelectedCheckBox();
    QString dcu;
    
    if(selected == 1) dcu = "yes";
    else if(selected == 3) dcu = "no";
    
    for(int i = 0 ; i < ccuTable->numRows() ; i++) {
	if(selected == 2) {
	    if(ccuTable->text(i, 0) == "dummy") dcu = "no";
	    else dcu = "yes";
	}	
	ccuTable->setText(i, 2, dcu);
    }    
}

QValueVector<QString> CCUInfoImpl::getCCUTable() {
    QValueVector<QString> ret;
    for(int i = 0 ; i < ccuTable->numRows() ; i++)
	for (int j = 0 ; j < ccuTable->numCols() ; j++)
	    ret.push_back(ccuTable->text(i, j));
    
    return ret;
}

void CCUInfoImpl::savePage(QString key) {
    CCUInfoPage *p = supervisor->getCCUInfoPage(key);
    
    if(p == NULL) p = new CCUInfoPage();
    
    p->setNbCCU((nbCCULineEdit->text()).toInt());
    p->setCheckBoxGroup(getSelectedCheckBox());
    p->setCCUTable(getCCUTable());    
    
    supervisor->updateCCUInfoPage(key, p);
}

void CCUInfoImpl::sameConfigForAll()
{
    QListBox *l = fecRingCB->listBox();
    for(unsigned int i = 0 ; i < l->count() ; i++) savePage(l->text(i));
}


void CCUInfoImpl::addTabEntry(QString position, QString address, QString dcu, QString dcuHardId) {
    ccuTable->setNumRows(ccuTable->numRows() + 1);
    ccuTable->verticalHeader()->setLabel( ccuTable->numRows() - 1, tr( "" ) );
    ccuTable->setText(ccuTable->numRows() - 1, 0, position);
    ccuTable->setText(ccuTable->numRows() - 1, 1, address);
    ccuTable->setText(ccuTable->numRows() - 1, 2, dcu);
    ccuTable->setText(ccuTable->numRows() - 1, 3, dcuHardId);
    
    ccuTable->sortColumn(0, true, true);
}

void CCUInfoImpl::clearCCUTable() {
    int i = 0;
    while(i < ccuTable->numRows()) {
	ccuTable->removeRow(0);
    }
}
