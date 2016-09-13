#include "GUISupervisor.h"

#include "TestConfigurationImpl.h"
#include "PowerUpStepsImpl.h"
#include "TestResultsImpl.h"
#include "APIAccess.h"

#include <qlistview.h>

#include "FecVmeRingDevice.h"

int GUISupervisor::testSelected = 0;
int GUISupervisor::specialI2CTestSelected = 0;

GUISupervisor::GUISupervisor( FecAccess *fecAcc ) {

  resDir = "ResultsFiles/";
  prefDir = "Preferences/";

  nbLoop_ = 1;

    // scan for fecs
    std::list<keyType> *fecSlotList;
    try {
      fecAccess = fecAcc ;
      fecSlotList = fecAccess->getFecList() ;
    }
    catch (FecExceptionHandler &e) { cout << e.what() << endl; }

    bool testPossible = true;

    if (fecSlotList != NULL) {
	for (std::list<keyType>::iterator p=fecSlotList->begin() ; p!=fecSlotList->end() ; p++) {
	    keyType index = *p ;
	    // plx and fec reset
	    try {
		fecAccess->fecHardReset(index);
		fecAccess->fecRingReset(index);
	    }
	    catch (FecExceptionHandler &e) { //cout << e.what() << endl; 
	    }
	    // check sr0 if not Ok ->emptyFifo
	    try {
		if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) emptyFIFOs ( fecAccess, index, true ) ;
		if (isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
		    okKeyTypes.push_back(index);
		    fecRingSelected.push_back(getFecKey(index));
		    fecRingSelected.push_back(getRingKey(index));
		    fecRingSelected.push_back(1); // default : fec-ring is selected
		}
	    }
	    catch (FecExceptionHandler &e) { cout << e.what() << endl; }
	}
    }
    else testPossible = false;
         
    // launch first window
    (new TestConfigurationImpl(this, testPossible))->show();
}

void GUISupervisor::endTestConfiguration() {
    if(testSelected > 0)
	(new PowerUpStepsImpl(this))->show();
    else exit(0);
}

void GUISupervisor::powerIsUp() {
    (new TestResultsImpl(this, fecAccess))->show();
}

void GUISupervisor::updateFecRingSelected(QValueVector<int> selectedFecRing) {
    fecRingSelected = selectedFecRing;
}

QValueVector<int> GUISupervisor::getFecRingSelected() {
    return fecRingSelected;
}

QValueVector<keyType> GUISupervisor::getOkKeyTypes() {
    return okKeyTypes;
}

CCUInfoPage * GUISupervisor::getCCUInfoPage(QString key) {
    if(ccuInfoPage.contains(key)) return *(ccuInfoPage.find(key));
    else return NULL;
}

void GUISupervisor::updateCCUInfoPage(QString key, CCUInfoPage *p) {
    ccuInfoPage[key] = p;
}

void GUISupervisor::recordPSCurrent(QString psCurrent) {
    psCurrents.push_back(psCurrent);
}

QValueVector<QString> GUISupervisor::getPSCurrent() {
    return psCurrents;
}

void GUISupervisor::setDcuOnCcuThr(QStringList l) {
    dcuOnCcuThr = l;
}

void GUISupervisor::setDcuOnModuleThr(QStringList l) {
    dcuOnModuleThr = l;
}

QStringList GUISupervisor::getDcuOnCcuThr() {
    return dcuOnCcuThr;
}

QStringList GUISupervisor::getDcuOnModuleThr() {
    return dcuOnModuleThr;
}

void GUISupervisor::uploadResults(std::string fileName) {
  (new TestResultsImpl(fileName))->show();
}

void GUISupervisor::setPrefDir(QString dirPath) {
  if(!dirPath.endsWith("/")) dirPath += "/";
  prefDir = dirPath;
}

void GUISupervisor::setResDir(QString dirPath) {
  if(!dirPath.endsWith("/")) dirPath += "/";
  resDir = dirPath;
}

QString GUISupervisor::getPrefDir() {
  return prefDir;
}

QString GUISupervisor::getResDir() {
  return resDir;
}

void GUISupervisor::setNbLoopOnModules(int nbLoop) {
  nbLoop_ = nbLoop;
}

int GUISupervisor::getNbLoopOnModules() {
  return nbLoop_;
}
