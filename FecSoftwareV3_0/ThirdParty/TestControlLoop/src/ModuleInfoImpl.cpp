#include "ModuleInfoImpl.h"
#include "DCUThresholdsImpl.h"

#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qtable.h>
#include <qregexp.h>
#include <qvalidator.h>

ModuleInfoImpl::ModuleInfoImpl(QWidget *parent, GUISupervisor *guiSupervisor) : ModuleInfo(parent, "ModuleInfo", true) {
    supervisor = guiSupervisor;
    currentCCUInfoPage = NULL;
    backClose = false;
    
    // set of regular expression
    QRegExp re1( "[0-9]{,2}" );
    QRegExpValidator *val1 = new QRegExpValidator( re1, 0 );
    nbModuleLineEdit->setValidator(val1);
    
    QRegExp re2("(0x[0-9a-fA-F]|[0-9a-fA-F])[0-9a-fA-F]?");
    QRegExpValidator *val2 = new QRegExpValidator( re2, 0 );
    i2cAddressLineEdit->setValidator(val2);
    
    QRegExp re3( "[46]{1}" );
    QRegExpValidator *val3 = new QRegExpValidator( re3, 0 );
    nbAPVLineEdit->setValidator(val3);
    
    QValueVector<int> fecRingSelected = supervisor->getFecRingSelected();
    for(QValueVector<int>::iterator it = fecRingSelected.begin() ; it < fecRingSelected.end() ; it = it+3) {
	if(*(it+2) == 1) {
	    QString item = QString::number(*it);
	    item += QString(",");
	    item += QString::number(*(it+1));
	    fecRingCB->insertItem( tr( item ) );
	}
    }   
    
    currentFecRing = "";
    currentCCU = "";
    
    changeFecRing();
}

void ModuleInfoImpl::closeEvent(QCloseEvent *e) {
    e->accept();
    if(!backClose) back();
}

void ModuleInfoImpl::clearPage()
{
    nbModuleLineEdit->setText("0");
    APV4ChB->setChecked(false);
    APV6ChB->setChecked(false);
    i2cAddressLineEdit->setText("");
    nbAPVLineEdit->setText("");
    dcuHardIdLineEdit->setText("");
    clearModuleTable();
}

void ModuleInfoImpl::back() {
    if(currentCCUInfoPage != NULL) {
	if(sameConfigChB->isChecked()) sameConfigForAll();
	else savePage(currentFecRing, currentCCU);
    }
    
    backClose = true;
    close();
}

void ModuleInfoImpl::addModule()
{
    QString i2cAddress = i2cAddressLineEdit->text();
    QString nbAPV;
    int selected = getSelectedCheckBox();
    switch(selected) {
    case 0:
	nbAPV = nbAPVLineEdit->text();
	break;
    case 1:
	nbAPV = "4";
	break;
    case 2:
	nbAPV = "6";
	break;
    }
    QString dcuHardId = dcuHardIdLineEdit->text();    
    
    if(i2cAddress != "" && nbAPV != "") {
	if(! i2cAddress.startsWith("0x")) i2cAddress = "0x" + i2cAddress;
	addTabEntry(i2cAddress, nbAPV, dcuHardId);
    }
}

void ModuleInfoImpl::deleteModule()
{
    moduleTable->removeRow(moduleTable->currentRow());
}

void ModuleInfoImpl::apv4()
{
    if(APV4ChB->isChecked()) {
	APV6ChB->setChecked(false);
	nbAPVLineEdit->setEnabled(false);
	updateAPVNumber();
    }
    else nbAPVLineEdit->setEnabled(true);
    
    if(sameConfigChB->isChecked()) sameConfigForAll();
}

void ModuleInfoImpl::apv6()
{
    if(APV6ChB->isChecked()) {
	APV4ChB->setChecked(false);
	nbAPVLineEdit->setEnabled(false);
	updateAPVNumber();
    }
    else nbAPVLineEdit->setEnabled(true);
    
    if(sameConfigChB->isChecked()) sameConfigForAll();
}

void ModuleInfoImpl::setDcuThreshold()
{
    (new DCUThresholdsImpl(this, 2, supervisor))->show();
}

void ModuleInfoImpl::changeCCU()
{
    if(currentCCU != "") savePage(currentFecRing, currentCCU);
    
    currentCCU = ccuCB->currentText();
    
    if(currentCCU != "") {
	ModuleInfoPage *m = currentCCUInfoPage->getModuleInfoPage(currentCCU);
	
	if(m != NULL) {
	    nbModuleLineEdit->setText(QString::number(m->getNbModule()));
	
	    int v = m->getCheckBoxGroup();

	    switch(v) {
	    case 0:
		APV4ChB->setChecked(false);
		APV6ChB->setChecked(false);
		nbAPVLineEdit->setEnabled(true);
		break;
	    case 1:
		APV4ChB->setChecked(true);
		apv4();
		break;
	    case 2:
		APV6ChB->setChecked(true);
		apv6();
		break;
	    }
	
	    clearModuleTable();
	    QValueVector<QString> tab = m->getModuleTable();    
	    QValueVector<QString>::iterator it = tab.begin();
#ifdef TOBTEST
	    int nbElement = 0;
	    for (QValueVector<QString>::iterator itt = tab.begin() ; itt < tab.end() ; itt++) nbElement++;
	    int nbRow = nbElement/3;
#else
	    int nbRow = tab.count()/3;
#endif
	    for(int i = 0 ; i < nbRow ; i++) {
		addTabEntry((*it), *(it+1), *(it+2));
		it = it+3;
	    }
	}
    }
}

void ModuleInfoImpl::changeFecRing()
{
    if(currentFecRing != "" && currentCCU != "") savePage(currentFecRing, currentCCU);
    
    currentCCU = "";
    currentFecRing = fecRingCB->currentText();

    clearPage();
    currentCCUInfoPage = supervisor->getCCUInfoPage(fecRingCB->currentText());
    if(currentCCUInfoPage != NULL) {
	ccuCB->setListBox(currentCCUInfoPage->getCCUList());    
	if(currentCCUInfoPage->sameModuleConfig()) {
	    sameConfigChB->setChecked(true);
	    ccuCB->setEnabled(false);
	}
	else {
	    sameConfigChB->setChecked(false);
	    ccuCB->setEnabled(true);
	}
	changeCCU();
    }
    else {
	QListBox *temp = new QListBox();
	ccuCB->setListBox(temp);   
    }
}

void ModuleInfoImpl::warning()
{
    if(sameConfigChB->isChecked()) {
	ccuCB->setEnabled(false);
    }
    else ccuCB->setEnabled(true);
    
    sameConfigForAll();
}

void ModuleInfoImpl::clearModuleTable() {
    int i = 0;
    while(i < moduleTable->numRows()) {
	moduleTable->removeRow(0);
    }
}

void ModuleInfoImpl::addTabEntry(QString i2cAddress, QString nbAPV, QString dcuHardId) {
    moduleTable->setNumRows(moduleTable->numRows() + 1);
    moduleTable->verticalHeader()->setLabel(moduleTable->numRows() - 1, tr( "" ) );
    moduleTable->setText(moduleTable->numRows() - 1, 0, i2cAddress);
    moduleTable->setText(moduleTable->numRows() - 1, 1, nbAPV);
    moduleTable->setText(moduleTable->numRows() - 1, 2, dcuHardId);
    
    moduleTable->sortColumn(0, true, true);
}

int ModuleInfoImpl::getSelectedCheckBox() {
    if(APV4ChB->isChecked())  return 1;
    else if(APV6ChB->isChecked())  return 2;
    else return 0;    
}

void ModuleInfoImpl::updateAPVNumber()
{
    int selected = getSelectedCheckBox();
    if(selected > 0) {
	QString nbAPV;
    
	if(selected == 1) nbAPV = "4";
	else nbAPV = "6";
    
	for(int i = 0 ; i < moduleTable->numRows() ; i++)
	    moduleTable->setText(i, 1, nbAPV);	
    }
}

QValueVector<QString> ModuleInfoImpl::getModuleTable() {
    QValueVector<QString> ret;
    for(int i = 0 ; i < moduleTable->numRows() ; i++)
	for (int j = 0 ; j < moduleTable->numCols() ; j++)
	    ret.push_back(moduleTable->text(i, j));
    
    return ret;
}

void ModuleInfoImpl::savePage(QString fecRingKey, QString ccuKey) {
    ModuleInfoPage *m = currentCCUInfoPage->getModuleInfoPage(ccuKey);
    if(m == NULL) m = new ModuleInfoPage();
    
    m->setNbModule((nbModuleLineEdit->text()).toInt());
    m->setCheckBoxGroup(getSelectedCheckBox());
    m->setModuleTable(getModuleTable());    
    
    currentCCUInfoPage->setSameModuleConfig(sameConfigChB->isChecked());
    
    currentCCUInfoPage->updateModuleInfoPage(ccuKey, m);
    supervisor->updateCCUInfoPage(fecRingKey, currentCCUInfoPage);
}

void ModuleInfoImpl::sameConfigForAll()
{
    if(currentCCUInfoPage != NULL) {
	QListBox *l = ccuCB->listBox();
	for(unsigned int i = 0 ; i < l->count() ; i++) savePage(currentFecRing, l->text(i));
    }
}
