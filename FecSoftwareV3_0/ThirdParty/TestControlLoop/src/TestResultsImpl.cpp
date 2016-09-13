// Fred, recherche ton prenom dans ce src pour trouver les commentqires concernant le test i2c special

#include <string>
#include <fstream>
#include <iostream>

#include "FecErrorDescription.h"

#include "TestResultsImpl.h"
#include "GUISupervisor.h"

#include "PSCurrentStoreImpl.h"
#include "APIAccess.h"

#include <qlistview.h>
#include <qimage.h>
#include <qfile.h>
#include <qpushbutton.h>

#include <time.h>

#define APV20 		0
#define APV21 		1
#define APV22 		2
#define APV23 		3
#define APV24 		4
#define APV25 		5
#define DCU   		6
#define LASERDRIVER	7
#define MUX		8
#define PLL 		9

static const char* const greenLed_data[] = { 
"36 37 7 1",
"a c #00ff00",
"b c #35ff35",
"c c #6aff6a",
"d c #9fff9f",
"# c #aaaaaa",
"e c #d4ffd4",
". c #e6e6e6",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"................######..............",
"..............##aaaaaa##............",
".............#aaaabaaaaa#...........",
"............#aabbccbbbaaa#..........",
"...........#aabccdcccbbbaa#.........",
"...........#abcddeddccbbaa#.........",
"..........#aabcdeeeddccbbaa#........",
"..........#aacdeeeeddccbbaa#........",
"..........#abccdeeeddccbbaa#........",
"..........#aabcddddddccbbaa#........",
"..........#aabccddddcccbbaa#........",
"..........#aabccccccccbbbaa#........",
"...........#aabccccccbbbaa#.........",
"...........#aabbbbbbbbbbaa#.........",
"............#aaabbbbbbaaa#..........",
".............#aaaaaaaaaa#...........",
"..............##aaaaaa##............",
"................######..............",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"...................................."};

static const char* const orangeLed_data[] = { 
"36 39 7 1",
"# c #aaaaaa",
". c #e6e6e6",
"a c #ffaa00",
"b c #ffbc35",
"c c #ffcd6a",
"d c #ffdf9f",
"e c #fff1d4",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"................######..............",
"..............##aaaaaa##............",
".............#aaaabaaaaa#...........",
"............#aabbccbbbaaa#..........",
"...........#aabccdcccbbbaa#.........",
"...........#abcddeddccbbaa#.........",
"..........#aabcdeeeddccbbaa#........",
"..........#aacdeeeeddccbbaa#........",
"..........#abccdeeeddccbbaa#........",
"..........#aabcddddddccbbaa#........",
"..........#aabccddddcccbbaa#........",
"..........#aabccccccccbbbaa#........",
"...........#aabccccccbbbaa#.........",
"...........#aabbbbbbbbbbaa#.........",
"............#aaabbbbbbaaa#..........",
".............#aaaaaaaaaa#...........",
"..............##aaaaaa##............",
"................######..............",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"....................................",
"...................................."};

static const char* const redLed_data[] = { 
"37 37 7 1",
"# c #aaaaaa",
". c #e6e6e6",
"a c #ff0000",
"b c #ff3535",
"c c #ff6a6a",
"d c #ff9f9f",
"e c #ffd4d4",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
"................######...............",
"..............##aaaaaa##.............",
".............#aaaabaaaaa#............",
"............#aabbccbbbaaa#...........",
"...........#aabccdcccbbbaa#..........",
"...........#abcddeddccbbaa#..........",
"..........#aabcdeeeddccbbaa#.........",
"..........#aacdeeeeddccbbaa#.........",
"..........#abccdeeeddccbbaa#.........",
"..........#aabcddddddccbbaa#.........",
"..........#aabccddddcccbbaa#.........",
"..........#aabccccccccbbbaa#.........",
"...........#aabccccccbbbaa#..........",
"...........#aabbbbbbbbbbaa#..........",
"............#aaabbbbbbaaa#...........",
".............#aaaaaaaaaa#............",
"..............##aaaaaa##.............",
"................######...............",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
".....................................",
"....................................."};

TestResultsImpl::TestResultsImpl(std::string fileName): redLed( (const char **) redLed_data ), greenLed( (const char **) greenLed_data ), orangeLed( (const char **) orangeLed_data ) {
  uploadXml(fileName);
  finishButton->setText("Close");
}

TestResultsImpl::TestResultsImpl(GUISupervisor *guiSupervisor, 
			         FecAccess *fecAccess_) : TestResults(), redLed( (const char **) redLed_data ), greenLed( (const char **) greenLed_data ), orangeLed( (const char **) orangeLed_data ) { 
    supervisor = guiSupervisor; 
    fecAccess = fecAccess_; 
    
    anErrorOccured = false;

    setNoGetchar(true);
    
    doTest();
}

void TestResultsImpl::closeEvent(QCloseEvent *e) {
    e->accept();
    close();
}

void TestResultsImpl::finish() {
  if(finishButton->text() != "Close") {
    if((GUISupervisor::testSelected & psCurrentsRecordMask) && !anErrorOccured)// if all tests ok and ps current record required
      (new PSCurrentStoreImpl(this, supervisor))->show();
    // else direct generation of xml file -> method of results class    
    else generateXml();
  }
    
  close();
}

void TestResultsImpl::doTest() {
    fecRingSelected = supervisor->getFecRingSelected();
    okKeyTypes = supervisor->getOkKeyTypes();
    
    int i = 0;
 
    for(QValueVector<int>::iterator it = fecRingSelected.begin() ; it < fecRingSelected.end() ; it = it+3) {	
	if(*(it+2) == 1) { // fecRing is selected 
	    keyType index = okKeyTypes[i];
	    tscType16 fecSlot = getFecKey(index);
	    tscType16 ringSlot = getRingKey(index);
	    currentFecRingItem = new QListViewItem(resultsView);
	    currentFecRingItem->setOpen(false);
	    QString label = "FEC " + QString::number(fecSlot) + " Ring " + QString::number(ringSlot);
	    currentFecRingItem->setText( 0, tr(label) ); 
	    currentFecRingItem->setPixmap( 1, greenLed ); // green led for the fec
		
	    // fec Test
	    if(GUISupervisor::testSelected & fecTestMask) {
		fecTest(index);
	    }
		
	    int dcuTest;
	    bool test = true;
	    std::list<keyType> *ccuList = NULL;
		
	    // Add ccu item if necessary
	    if(GUISupervisor::testSelected & ccuTestMask || 
	       GUISupervisor::testSelected & redundancyTestMask ||
	       GUISupervisor::testSelected & modulesDevicesTestMask || 
	       GUISupervisor::testSelected & dohCalibrationMask ||
	       GUISupervisor::testSelected & configurationTestMask ||
	       GUISupervisor::specialI2CTestSelected & i2cSpecialCommandTestMask) {
		
		try {
		    ccuList = fecAccess->getCcuList(index);
		}
		catch (FecExceptionHandler &e) {
		    if(e.getErrorCode() == TSCFEC_CCUADDRESSDUPLICATEDERROR) {
			setRingError("Duplicated address of CCU found -> cannot performed tests on Ccu and modules");
		    }	
		}	
		    
		if(ccuList != NULL) {
		    // check the agreement between the ccuList and user specification
		    dcuTest = checkCcuAgreement(index, ccuList);
		    
		    // ccu registers Test
		    for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
			keyType ccuIndex = *p;
			tscType16 ccuAddress = getCcuKey(ccuIndex);
			createCcuItem(QString::number(ccuAddress, 16));
		    }
		    currentFecRingItem->sortChildItems(0, true);
		}
		else {	// no CCU detected
		    delete ccuList;
		    test = false;
		    setFecRingInfo("Warning, no CCU detected -> no test on CCU, Modules/Devices performed");
		}
	    }
	    
	    if(test) {
		moduleList = NULL;
		dcuOnCcuList = NULL;
		dohOnCcuList = NULL;
	    
		// ccu Test
		if(GUISupervisor::testSelected & ccuTestMask) {
		    // ring is closed ?
		    try {
			if(! fecAccess->getFecRingSR0(index) & FEC_SR0_LINKINITIALIZED) {
			    setRingError("Ring not closed");
			}		    
		    }
		    catch(FecExceptionHandler &e) { }

		    // ccu registers Test
		    for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
		      setCurrentCcuItem(getCcuKey(*p));
			ccuTest(*p);
		    }		    
		}
				
		// redundancy Test
		if(GUISupervisor::testSelected & redundancyTestMask) {
		    redundancyTest(index, dcuTest);
		    if(GUISupervisor::testSelected & ccuTestMask && currentDummyIndex != 0) testDummyCcuRegisters(index);
		}
	    
		if(GUISupervisor::testSelected & modulesDevicesTestMask || 
		   GUISupervisor::specialI2CTestSelected & i2cSpecialCommandTestMask || // detect modules/devices for display of other test
		   GUISupervisor::testSelected & configurationTestMask) {
		    piaResetOnCcu(ccuList);
		    modulesDevicesTest(index);
		}
	    
		if(GUISupervisor::testSelected & dohCalibrationMask) dohCalibration(); // not yet implemented

		if(GUISupervisor::specialI2CTestSelected & i2cSpecialCommandTestMask) {
		  // Fred, active ici le specialI2CTest
		  if(moduleList != NULL) {
		    for(std::list<keyType>::iterator it = moduleList->begin() ; it != moduleList->end() ; it++) ;//specialI2CTest(*it); // not yet implemented
		  }
		}
	    
		if(GUISupervisor::testSelected & configurationTestMask) {
		    if(!(GUISupervisor::testSelected & modulesDevicesTestMask)) {
			piaResetOnCcu(ccuList); 
			modulesDevicesTest(index);
		    }
		    
		    if(moduleList != NULL) {
			for(std::list<keyType>::iterator it = moduleList->begin() ; it != moduleList->end() ; it++) configurationTest(*it, 1); // 1 is for Module test
		    }
		    if(dcuOnCcuList != NULL) {
			for(std::list<keyType>::iterator it = dcuOnCcuList->begin() ; it != dcuOnCcuList->end() ; it++) configurationTest(*it, 2); // 2 is for DCU on CCUM test
		    }
		    if(dohOnCcuList != NULL) {
			for(std::list<keyType>::iterator it = dohOnCcuList->begin() ; it != dohOnCcuList->end() ; it++) configurationTest(*it, 3); // 3 is for DOH on CCUM test
		    }
		}		
	    }
	}
	i++;
    }    
}

void TestResultsImpl::fecTest(keyType index) {
    // test of CR0
    try {
	fecAccess->setFecRingCR0(index, FEC_CR0_ENABLEFEC);
    }
    catch (FecExceptionHandler &e) { 
	setFecError("Problem occured while writing on CR0");
    }
    try {
	if(! fecAccess->getFecRingCR0(index) & FEC_CR0_ENABLEFEC) {
	    setFecError("Wrong value read on CR0");
	}
    }
    catch (FecExceptionHandler &e) {
	setFecError("Problem occured while reading CR0");
    }	    
    
    // test of CR1
    try {
	fecAccess->setFecRingCR1(index,  FEC_CR1_CLEARIRQ & FEC_CR1_CLEARERRORS);
    }
    catch (FecExceptionHandler &e) {
	setFecError("Problem occured while writing on CR1");
    }
    
    // test of SR0
    try {
	if(! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
	    setFecError("Wrong value read on SR0");
	}
    }
    catch (FecExceptionHandler &e) {
	setFecError("Problem occured while reading SR0");
    }
    
    // test of SR1
    try {
	if(fecAccess->getFecRingSR1(index) != 0x0) {
	    setFecError("Wrong value read on SR1");
	}	
    }
    catch (FecExceptionHandler &e) {
	setFecError("Problem occured while reading SR1");
    }
}

void TestResultsImpl::ccuTest(keyType ccuIndex) {
    // Test of CRA
    try {
	fecAccess->setCcuCRA(ccuIndex, CCU_CRA_CLRE);
    }	
    catch (FecExceptionHandler &e) { 
	setCcuError("Problem occured while writing on CRA");
    }		    
    
    try {
	if(fecAccess->getCcuCRA(ccuIndex) != 0x0) {
	    setCcuError("Wrong value read on CRA");
	}
    }
    catch (FecExceptionHandler &e) {
	setCcuError("Problem occured while reading CRA");
    }
    
    // Test of SRA
    try {
	if(fecAccess->getCcuSRA(ccuIndex) != 0x0) {
	    setCcuError("Wrong value read on SRA");
	}
    }
    catch (FecExceptionHandler &e) {
	setCcuError("Problem occured while reading SRA");
    }
    
    // Test of CRE
    try {
      fecAccess->setCcuCRE(ccuIndex, 0x0);
    }	
    catch (FecExceptionHandler &e) { 
	setCcuError("Problem occured while writing on CRE");
    }	    
    
    try {
	if(fecAccess->getCcuCRE(ccuIndex) != 0x0) {
	    setCcuError("Wrong value read on CRE");
	}
    }
    catch (FecExceptionHandler &e) {
	setCcuError("Problem occured while reading CRE");
    }
    
    // Test of SRE
    try {
	if(fecAccess->getCcuSRE(ccuIndex) != 0x0) {
	    setCcuError("Wrong value read on SRE");
	}
    }
    catch (FecExceptionHandler &e) {
	setCcuError("Problem occured while reading SRE");
    }   
}

void TestResultsImpl::redundancyTest(keyType index, int dcuTest) {
    // bypass all CCU and check if the ring is closed
    std::vector<std::string> results;
    try {
      results = autoTrackerRedundancyRing ( fecAccess, getFecKey(index), getRingKey(index), 10, dcuTest, 1, 0) ;
    }
    catch(FecExceptionHandler &e) {
	cout << e.what() << endl;
    }
  
    QString ccuAccessing;
    currentDummyIndex = 0;
    
    for(std::vector<std::string>::iterator it = results.begin() ; it < results.end() ; it++) {
#ifdef TOBTEST
	QString str((*it).c_str());
#else
	QString str(*it);
#endif
	
	str = str.stripWhiteSpace();
		
	if(str.startsWith("<CCUDummy")) { // add the dummy ccu item to the results
	    currentCcuItem = new QListViewItem(currentFecRingItem);
	    currentCcuItem->setOpen(false);
	    QString dummyAddress = str.mid(14, str.length()-15);
	    QString label = "Dummy CCU 0x" + dummyAddress;
	    currentCcuItem->setText( 0, tr(label) ); 
	    currentCcuItem->setPixmap( 1, greenLed );
	    currentDummyIndex = index | setCcuKey(dummyAddress.toInt(0, 16)); // dummy address needed to perform registers test on this ccu
	}
	else if(str.startsWith("<CCUAddress")) {
	    tscType16 ccuAddress = (str.mid(16,str.length()-17)).toInt(0,16);
	    setCurrentCcuItem(ccuAddress);
	}
	else if(str.startsWith("<Bypass")) {
	    QString res = str.mid(8, str.length()-17);
	    if(res == "Nok") 	setCcuError("Cannot be bypassed");
	}
	else if(str.startsWith("<InputB")) {
	    QString res = str.mid(8, str.length()-17);
	    if(res == "Nok") setCcuError("Link to Fec on Input B not operational");
	}
	else if(str.startsWith("<AccessTo")) {
	    ccuAccessing = str.mid(22, str.length() - 23);
	}
	else if(str.startsWith("<AccessCcu")) {
	    QString res = str.mid(11, str.length()-23);
	    if(res == "Nok") setCcuError("Failure while accessing Ccu "+ ccuAccessing + " after bypassing this Ccu");
	}
	else if(str.startsWith("<AccessDcu")) {
	    QString res = str.mid(11, str.length()-23);
	    if(res == "Nok") 	setCcuError("Failure while accessing Dcu on Ccu "+ ccuAccessing + " after bypassing this Ccu");
	}
    }
    
    if(currentDummyIndex == 0) setRingError("Dummy CCU not found");
}

void TestResultsImpl::modulesDevicesTest(keyType index) {

  bool i2cSpeed = false;
  if(GUISupervisor::testSelected & i2cSpeedTestMask) i2cSpeed = true;
  std::vector<std::string> results = testScanTrackerDevice ( fecAccess, getFecKey(index), getRingKey(index), 1, 0, i2cSpeed ); 


    QString currentModuleAddress;
  
    for(std::vector<std::string>::iterator it = results.begin() ; it < results.end() ; it++) {
#ifdef TOBTEST
	QString str((*it).c_str());
#else
	QString str(*it);
#endif
	
	str = str.stripWhiteSpace();
	
	if(str.startsWith("<CCU")) {
	    tscType16 ccuAddress = str.mid(7, str.length() - 8).toInt(0, 16); 
	    
	    setCurrentCcuItem(ccuAddress);
	    
	}
	else if(str.startsWith("<Module")) {
	    currentModuleAddress = str.mid(10, str.length()-11);
	    createModuleItem("Module on i2c channel 0x" + currentModuleAddress);

	    // store module detected on corresponding CCU
	    if(moduleList == NULL) moduleList = new std::list<keyType>;
	    moduleList->push_back(buildCompleteKey(getFecKey(index), 
					          getRingKey(index), 
					          (currentCcuItem->text(0)).mid(6).toInt(0,16), 
					          currentModuleAddress.toInt(0,16), 
					          0));
	}
	else if(str.startsWith("<APV")) {
	    if(str.endsWith("missing>")) {
		int apvIndex;
		
		if(str.mid(5, 4) == "0x20") 		apvIndex = APV20;
		else if(str.mid(5, 4) == "0x21") 	apvIndex = APV21;
		else if(str.mid(5, 4) == "0x22") 	apvIndex = APV22;
		else if(str.mid(5, 4) == "0x23") 	apvIndex = APV23;
		else if(str.mid(5, 4) == "0x24") 	apvIndex = APV24;
		else 				apvIndex = APV25;

		setDeviceError(apvIndex);
	    }	
	    else if (str.endsWith("23 not detected>")) { // ends with "not detected>"
		int nbAPVWaited = checkNbApv(index, currentModuleAddress);
		switch(nbAPVWaited) {
		case 0:
		    setOnly4APV(true);
		    break;
		case 4:
		    setOnly4APV(false);
		    break;
		case 6:
		    setDeviceError(APV22);
		    setDeviceError(APV23);
		    break;
		}		
	    }
	}
	else if(str.startsWith("<MUX")) {
	    setDeviceError(MUX);
	}
	else if(str.startsWith("<PLL")) {
	    setDeviceError(PLL);
	}
	else if(str.startsWith("<AOH")) {
	    setDeviceError(LASERDRIVER);
	}
	else if(str.startsWith("<MDCU")) {
	    setDeviceError(DCU);   
	}
	else if(str.startsWith("<DCU")) {
	    int startChannelIndex = str.find('o');
	    QString dcuAddress = str.mid(5, startChannelIndex-6);
	    QString channel = str.mid(startChannelIndex+11, str.length()-(startChannelIndex+12));

	    createModuleItem("DCU " + dcuAddress + " on channel " + channel, false);
	    
	    // store DCU detected on corresponding CCU
	    if(dcuOnCcuList == NULL) dcuOnCcuList = new std::list<keyType>;
	    dcuOnCcuList->push_back(buildCompleteKey(getFecKey(index), 
						     getRingKey(index), 
						     (currentCcuItem->text(0)).mid(6).toInt(0,16), 
						     channel.mid(2).toInt(0,16),
						     dcuAddress.mid(2).toInt(0,16)));
	}
	else if(str.startsWith("<DOH")) {
	    int startChannelIndex = str.find('o');
	    QString dohAddress = str.mid(5, startChannelIndex-6);
	    QString channel = str.mid(startChannelIndex+11, str.length()-(startChannelIndex+12));
	    createModuleItem("DOH " + dohAddress + " on channel " + channel, false);
	    
	    // store DOH detected on corresponding CCU
	    if(dohOnCcuList == NULL) dohOnCcuList = new std::list<keyType>;
	    dohOnCcuList->push_back(buildCompleteKey(getFecKey(index), 
						     getRingKey(index), 
						     (currentCcuItem->text(0)).mid(6).toInt(0,16), 
						     channel.mid(2).toInt(0,16),
						     dohAddress.mid(2).toInt(0,16)));
	}
	else if(str.startsWith("<i2cSpeed = ")) { // for dcu or doh on CCU
	    if(str.endsWith("error>")) { // error at 100kHz
		setModuleError("i2c speed error : Inconsistency error at 100 kHz => cannot download/upload it");
	    }	
	    else {
		setModuleInfo("i2c speed supported = " + str.mid(12, str.length()-13));
	    }	    
	}
	// i2c speed modules
	else if(str.startsWith("<i2cSpeed APV")) {
	    int apvIndex;
	    
	    if	(str.mid(14, 4) == "0x20") 	apvIndex = APV20;
	    else if	(str.mid(14, 4) == "0x21") 	apvIndex = APV21;
	    else if	(str.mid(14, 4) == "0x22") 	apvIndex = APV22;
	    else if	(str.mid(14, 4) == "0x23") 	apvIndex = APV23;
	    else if	(str.mid(14, 4) == "0x24") 	apvIndex = APV24;
	    else 				apvIndex = APV25;
	    
	    setDeviceError(apvIndex, "i2cSpeed");   
	}
	else if(str.startsWith("<i2cSpeed MUX")) {
	    setDeviceError(MUX, "i2cSpeed");
	}
	else if(str.startsWith("<i2cSpeed PLL")) {
	    setDeviceError(PLL, "i2cSpeed");
	}
	else if(str.startsWith("<i2cSpeed LASERDRIVER")) {
	    setDeviceError(LASERDRIVER, "i2cSpeed");   
	}
	else if(str.startsWith("<i2cSpeed DCU")) {
	    setDeviceError(DCU, "i2cSpeed");
	}
    }
 
    checkDcuOnCcu(index, dcuOnCcuList);
    checkModuleAgreement(index, moduleList);
}

void TestResultsImpl::dohCalibration() {
    
}

void TestResultsImpl::specialI2CTest() {
  /*************************************
   * Fred,
   *
   * Etat du display a ce moment du deroulement du test :
   *  les CCU et modules existants sont crees
   *
   * Lorsque tu veux ajouter un message d'erreur our d'info, utilise les methodes suivantes:
   * - erreur: setModuleError, setDeviceError (les tag pour specifier quel device l'erreur concerne sont definis au debut de ce fichier)
   * - info: setModuleInfo, setDeviceInfo
   * suivant le niveau ou tu veux ajouter une erreur/info (l'erreur est propagee automatiquement au niveau superieur pour la mise ajour de la led)
   *
   * Il faut que tu te bases sur la methode testTrackerDevices 
   * en ce qui concerne la facon de retourner les messages de resultats
   *
   * Pour l'interpretation des messages, bases toi sur la methode configurationTest
   *
   * Si ca va vraiment pas, tu as toujours mon num de tel...
  **************************************/
}

void TestResultsImpl::configurationTest(keyType channelIndex, int typeOfDevice) {
    std::vector<std::string> results;

    unsigned int v_size;

    int nbLoop = supervisor->getNbLoopOnModules();
    
    switch(typeOfDevice) {
    case 1: // for Module
      // test 6 apv
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  true,  // sixApvsF
			  false, // fourApvsF
			  false, // apvmuxF
			  false, // dcuF
			  false, // laserdriverF
			  false, // dohF
			  false, // pllF
			  nbLoop, 0,
			  results);

      if(v_size+2+nbLoop > results.size()) { // test if an exception occured before test
	results.push_back("6APV Error");
	results.push_back("</Module>");
	
	// test 4 apv
	v_size = results.size();
	testTrackerDevices (fecAccess,  
			    getFecKey(channelIndex), 
			    getRingKey(channelIndex),
			    getCcuKey(channelIndex),
			    getChannelKey(channelIndex),
			    false, // sixApvsF
			    true,  // fourApvsF
			    false, // apvmuxF
			    false, // dcuF
			    false, // laserdriverF
			    false, // dohF
			    false, // pllF
			    nbLoop, 0,
			    results);

	if(v_size+2+nbLoop > results.size()) {
	  results.push_back("4APV Error");
	  results.push_back("</Module>");
	}
      }

      // test apvmux
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  false, // fourApvsF
			  true,  // apvmuxF
			  false, // dcuF
			  false, // laserdriverF
			  false, // dohF
			  false, // pllF
			  nbLoop, 0,
			  results);

      if(v_size+2+nbLoop > results.size()) {
	results.push_back("MUX Error");
	results.push_back("</Module>");
      }      

      // test dcu
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  true,  // fourApvsF ----> to specify that it's a dcu on a module (and not on ccum)
			  false, // apvmuxF
			  true,  // dcuF
			  false, // laserdriverF
			  false, // dohF
			  false, // pllF
			  nbLoop, 0,
			  results);

      if(v_size+2+12*nbLoop > results.size()) {
	results.push_back("<DCU Error>");
	results.push_back("</Module>");
      }      

      // test laserdriver
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  false, // fourApvsF
			  false, // apvmuxF
			  false, // dcuF
			  true,  // laserdriverF
			  false, // dohF
			  false, // pllF
			  nbLoop, 0,
			  results);

      if(v_size+2+nbLoop > results.size()) {
	results.push_back("LASERDRIVER Error");
	results.push_back("</Module>");
      }      

      // test pll
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  false, // fourApvsF
			  false, // apvmuxF
			  false, // dcuF
			  false, // laserdriverF
			  false, // dohF
			  true,  // pllF
			  nbLoop, 0,
			  results);

      if(v_size+2+nbLoop > results.size()) {
	results.push_back("PLL Error");
	results.push_back("</Module>");
      }      

	break;
    case 2: // for DCU on CCUM
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  false, // fourApvsF
			  false, // apvmuxF
			  true,  // dcuF
			  false, // laserdriverF
			  false, // dohF
			  false,  // pllF
			  nbLoop, 0,
			  results);

      if(v_size+(12*nbLoop) > results.size()) {
	results.push_back("<DCUCCU Error>");
      }

	break;
    case 3: // for DOH on CCUM
      v_size = results.size();
      testTrackerDevices (fecAccess,  
			  getFecKey(channelIndex), 
			  getRingKey(channelIndex),
			  getCcuKey(channelIndex),
			  getChannelKey(channelIndex),
			  false, // sixApvsF
			  false, // fourApvsF
			  false, // apvmuxF
			  false, // dcuF
			  false, // laserdriverF
			  true,  // dohF
			  false,  // pllF
			  nbLoop, 0,
			  results);

      if(v_size+(2*nbLoop) > results.size()) {
	results.push_back("DOH Error");
      }

	break;
    }
    
    if(results.size() > 0) {    
      // For loop, count how many failure occured 
      /* already declared
	 APV20 		0
	 APV21 		1
	 APV22 		2
	 APV23 		3
	 APV24 		4
	 APV25 		5
	 LASERDRIVER	7
	 MUX		8
	 PLL 		9
      */
      int DCUCH0 = 10;      
      int DCUCH1 = 11;      
      int DCUCH2 = 12;      
      int DCUCH3 = 13;      
      int DCUCH4 = 14;      
      int DCUCH5 = 15;      
      int DCUCH6 = 16;      
      int DCUCH7 = 17;      
      int DCUCCUCH0 = 18;      
      int DCUCCUCH1 = 19;      
      int DCUCCUCH2 = 20;      
      int DCUCCUCH3 = 21;      
      int DCUCCUCH4 = 22;      
      int DCUCCUCH5 = 23;      
      int DCUCCUCH6 = 24;      
      int DCUCCUCH7 = 25;      
     
      int DOH = 6;
      
      int failures[26];
      QValueVector<QString> dcuHardId;
      QValueVector<QString> dcuCcuHardId;

      bool dcuccu = false;

      for(int i = 0 ; i < 26 ; i++) failures[i] = 0;

	tscType16 ccuAddress = getCcuKey(channelIndex);
	QString moduleAddress = "0x" + QString::number(getChannelKey(channelIndex), 16);

	setCurrentCcuItem(ccuAddress);

	QValueVector<QString> dcuValues;

	for(std::vector<std::string>::iterator it = results.begin() ; it < results.end() ; it++) {
#ifdef TOBTEST
	    QString str((*it).c_str());
#else
	    QString str(*it);
#endif    
	
	    str = str.stripWhiteSpace();
	    
	    if(str.startsWith("<Channel")) {
		QString text = str.mid(8, 1) + "/";
		text = text + str.mid(12, str.length()-13);
		dcuValues.push_back(text);	
	    }
	    else if(str.startsWith("<DcuHardId")) {
	      if(dcuccu) dcuCcuHardId.push_back(str.mid(11, str.length()-23));
	      else dcuHardId.push_back(str.mid(11, str.length()-23));
	    }
	    else if(str.startsWith("<DCUCCU>")) {		
	      dcuccu = true;
	      dcuValues.clear();
	    }
	    else if(str.startsWith("</DCUCCU>")) {
		QStringList thrList = supervisor->getDcuOnCcuThr();
		for(QStringList::iterator it = thrList.begin() ; it != thrList.end() ; ++it) {
		    QStringList l = QStringList::split("/", *it);
		    for(QStringList::iterator chIt = l.begin() ; chIt != l.end() ; ++chIt) {
			QString channelNum = (*chIt);
			double minThr = (*(++chIt)).toDouble();
			double maxThr = (*(++chIt)).toDouble();
			
			for(QValueVector<QString>::iterator valIt = dcuValues.begin() ; valIt < dcuValues.end() ; ++valIt) {
			    QString s = *valIt;
			    if(s.startsWith(channelNum)) {
				valIt = dcuValues.end();
				double val = (s.mid(2)).toDouble();
				
				if(val < minThr || val > maxThr) failures[channelNum.toInt()+18] = failures[channelNum.toInt()+18]+1;
			    }
			}
		    }
		}
	    }
	    else if(str.startsWith("DOH") && str.endsWith("Error")) {		
	      failures[DOH] = failures[DOH]+1;
	    }
	    else if(str.startsWith("<Module")) {
		currentModuleItem = currentCcuItem->firstChild();
		bool found = false;
		while(!found) {
		    if((currentModuleItem->text(0)).startsWith("Module")) {
			if(moduleAddress != (currentModuleItem->text(0)).mid(22))
			    currentModuleItem = currentModuleItem->itemBelow();
			else found = true;
		    }
		    else currentModuleItem = currentModuleItem->itemBelow();
		}
	    }
	    else if(str.startsWith("APV")) {
		if 	(str.mid(4, 4) == "0x20") 	failures[APV20] = failures[APV20]+1;
		else if	(str.mid(4, 4) == "0x21") 	failures[APV21] = failures[APV21]+1;
		else if	(str.mid(4, 4) == "0x22") 	failures[APV22] = failures[APV22]+1;
		else if	(str.mid(4, 4) == "0x23") 	failures[APV23] = failures[APV23]+1;
		else if	(str.mid(4, 4) == "0x24") 	failures[APV24] = failures[APV24]+1;
		else 				failures[APV25] = failures[APV25]+1;
	    }
	    else if(str.startsWith("<DCU>")) {
	      dcuccu = false;
	      dcuValues.clear();
	    }
	    else if(str.startsWith("</DCU>")) {
		QStringList thrList = supervisor->getDcuOnModuleThr();
		for(QStringList::iterator it = thrList.begin() ; it != thrList.end() ; ++it) {
		    QStringList l = QStringList::split("/", *it);
		    for(QStringList::iterator chIt = l.begin() ; chIt != l.end() ; ++chIt) {
			QString channelNum = (*chIt);
			double minThr = (*(++chIt)).toDouble();
			double maxThr = (*(++chIt)).toDouble();
			
			for(QValueVector<QString>::iterator valIt = dcuValues.begin() ; valIt < dcuValues.end() ; ++valIt) {
			    QString s = *valIt;
			    if(s.startsWith(channelNum)) {
				valIt = dcuValues.end();
				double val = (s.mid(2)).toDouble();

				if(val < minThr || val > maxThr) failures[channelNum.toInt()+10] = failures[channelNum.toInt()+10]+1;
			    }
			}
		    }
		}
	    }
	    else if(str.startsWith("LASERDRIVER")) {
	      failures[LASERDRIVER] = failures[LASERDRIVER]+1;
	    }
	    else if(str.startsWith("MUX")) {
	      failures[MUX] = failures[MUX]+1;
	    }
	    else if(str.startsWith("PLL")) {
	      failures[PLL] = failures[PLL]+1;
	    }
	}

	// Modules
	// APVs
	if(failures[APV20] > 0) setDeviceError(APV20, QString::number(failures[APV20]) + " error(s) while configuration");
	if(failures[APV21] > 0) setDeviceError(APV21, QString::number(failures[APV21]) + " error(s) while configuration");
	if(failures[APV22] > 0) setDeviceError(APV22, QString::number(failures[APV22]) + " error(s) while configuration");
	if(failures[APV23] > 0) setDeviceError(APV23, QString::number(failures[APV23]) + " error(s) while configuration");
	if(failures[APV24] > 0) setDeviceError(APV24, QString::number(failures[APV24]) + " error(s) while configuration");
	if(failures[APV25] > 0) setDeviceError(APV25, QString::number(failures[APV25]) + " error(s) while configuration");	
	// LaserDriver
	if(failures[LASERDRIVER] > 0) setDeviceError(LASERDRIVER, QString::number(failures[LASERDRIVER]) + " error(s) while configuration");	
	// Mux
	if(failures[MUX] > 0) setDeviceError(MUX, QString::number(failures[MUX]) + " error(s) while configuration");	
	// PLL
	if(failures[PLL] > 0) setDeviceError(PLL, QString::number(failures[PLL]) + " error(s) while configuration");	
	// Dcu
	// Hard Id
	if(!dcuHardId.isEmpty()) {
	  QString hardId = doublon(dcuHardId);
	
	  if(hardId != "") { // 1 hard id found -> no error
	    setDeviceInfo(DCU, "Hard Id = " + hardId);
	    checkDcuModuleHardId(channelIndex, hardId);
	  }
	  else { // more than 1 hard id found -> error
	    setDeviceError(DCU, "Different Hard Id found");
	  }
	}

	// Channel Values
	if(failures[DCUCH0] > 0) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH0]) + " times not between specified thresholds");
	if(failures[DCUCH1] > 1) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH1]) + " times not between specified thresholds");
	if(failures[DCUCH2] > 2) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH2]) + " times not between specified thresholds");
	if(failures[DCUCH3] > 3) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH3]) + " times not between specified thresholds");
	if(failures[DCUCH4] > 4) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH4]) + " times not between specified thresholds");
	if(failures[DCUCH5] > 5) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH5]) + " times not between specified thresholds");
	if(failures[DCUCH6] > 6) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH6]) + " times not between specified thresholds");
	if(failures[DCUCH7] > 7) setDeviceError(DCU, "Value on channel 0x" + QString::number(failures[DCUCH7]) + " times not between specified thresholds");

	// DCU on CCU
	// Hard Id
	if(!dcuCcuHardId.isEmpty()) {
	  currentModuleItem = currentCcuItem->firstChild();
	  bool found = false;
	  while(!found) {
	    if((currentModuleItem->text(0)).startsWith("DCU")) {
	      int startChannelIndex = (currentModuleItem->text(0)).find('o');
	      if(moduleAddress != (currentModuleItem->text(0)).mid(startChannelIndex + 11))
		currentModuleItem = currentModuleItem->itemBelow();
	      else found = true;
	    }
	    else currentModuleItem = currentModuleItem->itemBelow();
	  }
	  
	  QString hardId = doublon(dcuCcuHardId);
	  
	  if(hardId != "") { // 1 hard id found -> no error
	    setModuleInfo("Hard Id = " + hardId);
	    checkDcuCcuHardId(channelIndex, hardId);
	  }
	  else { // more than 1 hard id found -> error
	    setModuleError("Different Hard Id found");
	  }
	}
	  
	// Channel Values
	if(failures[DCUCCUCH0] > 0) setModuleError("Value on channel 0x0" + QString::number(failures[DCUCCUCH0]) + " times not between specified thresholds");
	if(failures[DCUCCUCH1] > 1) setModuleError("Value on channel 0x1" + QString::number(failures[DCUCCUCH1]) + " times not between specified thresholds");
	if(failures[DCUCCUCH2] > 2) setModuleError("Value on channel 0x2" + QString::number(failures[DCUCCUCH2]) + " times not between specified thresholds");
	if(failures[DCUCCUCH3] > 3) setModuleError("Value on channel 0x3" + QString::number(failures[DCUCCUCH3]) + " times not between specified thresholds");
	if(failures[DCUCCUCH4] > 4) setModuleError("Value on channel 0x4" + QString::number(failures[DCUCCUCH4]) + " times not between specified thresholds");
	if(failures[DCUCCUCH5] > 5) setModuleError("Value on channel 0x5" + QString::number(failures[DCUCCUCH5]) + " times not between specified thresholds");
	if(failures[DCUCCUCH6] > 6) setModuleError("Value on channel 0x6" + QString::number(failures[DCUCCUCH6]) + " times not between specified thresholds");
	if(failures[DCUCCUCH7] > 7) setModuleError("Value on channel 0x7" + QString::number(failures[DCUCCUCH7]) + " times not between specified thresholds");
    
	// DOH
	if(failures[DOH] > 0) {
	  currentModuleItem = currentCcuItem->firstChild();
	  bool found = false;
	  while(!found) {
	    if((currentModuleItem->text(0)).startsWith("DOH")) {
	      int startChannelIndex = (currentModuleItem->text(0)).find('o');
	      if(moduleAddress != (currentModuleItem->text(0)).mid(startChannelIndex+11))
		currentModuleItem = currentModuleItem->itemBelow();
	      else found = true;
	    }
	    else currentModuleItem = currentModuleItem->itemBelow();
	  }
	  
	  setModuleError(QString::number(failures[DOH]) + " error(s) while configuration");
	}
    }
}

void TestResultsImpl::testDummyCcuRegisters(keyType index) {
    setCurrentCcuItem(0, true);
    
      tscType16 fecAddress = getFecKey(index);
      
      unsigned int timeout = 10 ;
      do {
	// Make a reset
	fecAccess->fecHardReset (index) ;
	fecAccess->fecRingReset (index) ;
	if (! isFecSR0Correct(fecAccess->getFecRingSR0(index))) {
	  emptyFIFOs ( fecAccess, setFecKey(fecAddress), true ) ;
	}
	
	timeout -- ;
      }
      while (! isFecSR0Correct(fecAccess->getFecRingSR0(index)) && timeout > 0) ;
      
      // Find all the CCUs
      std::list<keyType> *ccuList = NULL ;
      try {
	// Scan the ring for each FECs and display the CCUs
	ccuList = fecAccess->getCcuList ( index ) ;
	
	if (ccuList != NULL) {
	  // Store all the addresses
	  keyType ccuAddresses[127] ;
	  unsigned int nbCcu = 0 ;
	  
	  for (std::list<keyType>::iterator p=ccuList->begin();p!=ccuList->end();p++) {
	    keyType index = *p ;
	    ccuAddresses[nbCcu++] = index ;
	  }
	  
	  // Delete = already store in the table of FEC device class
	  delete ccuList ;
	  
	  // disable the IRQ
	  fecAccess->getFecRingDevice(index)->setIRQ(false) ;
	  
	  // Put the dummy Ccu in the ring
	  fecAccess->setCcuCRC (ccuAddresses[nbCcu-1], CCU_CRC_SSP) ;
	  fecAccess->setFecRingCR0 ( ccuAddresses[nbCcu-1], FEC_CR0_SELSERIN ) ;
	  
	  // Check if the dummy CCU is in the ring
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) {
	    emptyFIFOs ( fecAccess, ccuAddresses[nbCcu-1] ) ;
	  }
	  if (! isFecSR0Correct(fecAccess->getFecRingSR0(ccuAddresses[nbCcu-1]))) { 
	    setCcuError("Failure while trying to test registers");
	  }
	  else {		
	    ccuList = fecAccess->getCcuList(index);
	    ccuTest(currentDummyIndex);
	  }
	}
      }
      catch (FecExceptionHandler &e) { 
	setCcuError("Hard access error while trying to test registers");
      }
}

void TestResultsImpl::piaResetOnCcu(std::list<keyType> *ccuList) {
    unsigned int piaResetValue = 0xFF ;
    unsigned long delayActiveReset = 10 ;
    unsigned long intervalDelayReset = 10000 ;
    tscType16 firstPiaChannel = 0x30;
 
    for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	keyType index = *p;
	QString result(testPIAResetfunctions (       fecAccess, 
					       getFecKey(index), 
					       getRingKey(index),
					       getCcuKey(index),
					       firstPiaChannel,
					       piaResetValue,
					       delayActiveReset,
					       intervalDelayReset, 1, 0 ));
	
	if(result.endsWith("error")) {
	    setCurrentCcuItem(getCcuKey(index));
	    setCcuError(result);
	}
    }
}

void TestResultsImpl::checkDcuModuleHardId(keyType channelIndex, QString dcuHardId) {
  CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(channelIndex)) + "," + QString::number(getRingKey(channelIndex)));
  if(p != NULL) {
    ModuleInfoPage *m = p->getModuleInfoPage((currentCcuItem->text(0)).mid(4));
    if(m != NULL) {
      QValueVector<QString> moduleTable = m->getModuleTable();
      cout << "moduletable size = " << moduleTable.size() << endl;
      for(unsigned int i = 0 ; i < moduleTable.size() ; i = i+3) {
	cout << "i = " << i << endl;
	cout << "moduletable[i] = " << moduleTable[i] << endl;
	cout << "moduletable[i+1] = " << moduleTable[i+1] << endl;
	cout << "moduletable[i+2] = " << moduleTable[i+2] << endl;
	QString infoModuleAddress = moduleTable[i];
	tscType16 moduleAddress;
	if(infoModuleAddress.startsWith("0x")) 
	  moduleAddress = infoModuleAddress.mid(2).toInt(0,16);
	else 
	  moduleAddress = infoModuleAddress.toInt(0,16);

	if(moduleAddress == getChannelKey(channelIndex)) {
	    cout << "av comparaison" << endl;
	    cout << "dcuHardid = " << dcuHardId << endl;
	    cout << "moduleTabelId = " << moduleTable[i+2] << endl;
	    if(dcuHardId != moduleTable[i+2]) {// error
	      cout << "av set device error" << endl;
	      cout << "Didn't found " << moduleTable[i+2] << " for Dcu Hard Id" << endl;
	      setDeviceError(DCU, "Didn't found " + moduleTable[i+2] + " for Dcu Hard Id");
	      cout << "ap set device error" << endl;
	    }
	    i = moduleTable.size();
	}
	
      }
    }
  }
}

void TestResultsImpl::checkDcuCcuHardId(keyType channelIndex, QString dcuHardId) {
  CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(channelIndex)) + "," + QString::number(getRingKey(channelIndex)));
  if(p != NULL) {
    QValueVector<QString> ccuTable = p->getCCUTable();
    for(unsigned int i = 0 ; i < ccuTable.size() ; i = i+4) {
      tscType16 ccuAddress;
      if(ccuTable[i+1].contains("0x")) ccuAddress = (ccuTable[i+1]).mid(2).toInt(0,16);
      else ccuAddress = ccuTable[i+1].toInt(0, 16);

      if(ccuAddress == getCcuKey(channelIndex)) {
	if(dcuHardId != ccuTable[i+3]) // error
	  setModuleError("Didn't found " + ccuTable[i+3] + " for Dcu Hard Id");

	i = ccuTable.size();
      }
    }
  }
}

int TestResultsImpl::checkNbApv(keyType fecRingIndex, QString moduleAddress) {
    CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(fecRingIndex)) + "," + QString::number(getRingKey(fecRingIndex)));
    if(p != NULL) {
	ModuleInfoPage *m = p->getModuleInfoPage((currentCcuItem->text(0)).mid(4));
	if(m != NULL) {
	    QValueVector<QString> moduleTable = m->getModuleTable();
	    for(QValueVector<QString>::iterator it = moduleTable.begin() ; it < moduleTable.end() ; it = it+3) {
		QString infoModuleAddress = *it;
		if(infoModuleAddress.startsWith("0x")) {
		    if(infoModuleAddress == ("0x"+moduleAddress)) {
			return (*(it+1)).toInt();
		    }
		}
		else {
		    if(infoModuleAddress == moduleAddress) {
			return (*(it+1)).toInt();
		    }
		}
	    }
	}
    }
   
    return 0;    
}

void TestResultsImpl::checkDcuOnCcu(keyType fecRingIndex, std::list<keyType> *dcuOnCcuList) {
    CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(fecRingIndex)) + "," + QString::number(getRingKey(fecRingIndex)));
    
    if(p != NULL) {
	QValueVector<QString> ccuTable = p->getCCUTable();
	for(unsigned int i = 0 ; i < ccuTable.size() ; i = i+4) {
	    tscType16 ccuAddress;
	    if(ccuTable[i+1].contains("0x")) ccuAddress = (ccuTable[i+1]).mid(2).toInt(0,16);
	    else ccuAddress = ccuTable[i+1].toInt(0, 16);

	    if(ccuTable[i+2] == "yes") {
		bool error = false;
		if(dcuOnCcuList != NULL) {
		    bool found = false;
		    for(std::list<keyType>::iterator dcuIt = dcuOnCcuList->begin() ; dcuIt != dcuOnCcuList->end() ; dcuIt++) {
			keyType dcuIndex = *dcuIt;
			tscType16 ccuAddr = getCcuKey(dcuIndex);
			
			if(ccuAddr == ccuAddress) {
			    found = true;
			    dcuIt = dcuOnCcuList->end();
			    dcuIt--;
			}
		    }			
		    if(!found) error = true;
		}
		else error = true;
		
		if(error) { // error : no dcu on ccu found
		    setCurrentCcuItem(ccuAddress);
		    
		    if(currentCcuItem != 0) {
			setCcuError("No Dcu on Ccu found");
		    }
		}
	    }
	}
    }
}

int TestResultsImpl::checkCcuAgreement(keyType fecRingIndex, std::list<keyType> *ccuList) {
    CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(fecRingIndex)) + "," + QString::number(getRingKey(fecRingIndex)));
    
    if(p != NULL) {
	// check nb ccu found
	int nbCcuUser = p->getNbCCU();
	int nbCcuSoft = ccuList->size();
	if(nbCcuUser != 0 && nbCcuUser != nbCcuSoft) {
	    setRingError("Wait " + QString::number(nbCcuUser) + " but found " + QString::number(nbCcuSoft) + " CCU");
	}
    
	// check addresses
	QValueVector<QString> ccuTable = p->getCCUTable();
	std::list<keyType>::iterator it = ccuList->begin();
	unsigned int i = 0;
	int lastPosition = 1;
	uint nbCcu = 1;
	while (i < ccuTable.size()) {
	    int position = ccuTable[i].toInt();
	    int ccuAddressUser;
	    QString addr = ccuTable[i+1];
	    if(addr.contains("0x")) ccuAddressUser = (ccuTable[i+1]).mid(2).toInt(0,16);
	    else ccuAddressUser = ccuTable[i+1].toInt(0, 16);
	    for(int j = lastPosition ; j < position ; j++) it++;
	    lastPosition = position;
	    int ccuAddressSoft = getCcuKey(*it);
	    
	    if(nbCcu <= ccuList->size()) {	
		if(ccuAddressUser != ccuAddressSoft) {
		    setRingError("Wait address 0x" + QString::number(ccuAddressUser,16) + " for CCU at position " + QString::number(position) + " but found address 0x" + QString::number(ccuAddressSoft,16));
		}	
		i = i+4;
		nbCcu++;
	    }
	    else {
		i = ccuTable.size();
	    }	    
	}
	return p->getCheckBoxGroup();
    }
    else return 3;
}

void TestResultsImpl::checkModuleAgreement(keyType fecRingIndex, std::list<keyType> *moduleList) {
    CCUInfoPage *p = supervisor->getCCUInfoPage(QString::number(getFecKey(fecRingIndex)) + "," + QString::number(getRingKey(fecRingIndex)));
 
    if(p != NULL) {
	QValueVector<QString> ccuTable = p->getCCUTable();
	for(QValueVector<QString>::iterator it = ccuTable.begin() ; it < ccuTable.end() ; it = it + 4) {
	    QString ccuAddress = *(it+1);
	    
	    setCurrentCcuItem(ccuAddress.mid(2).toInt(0,16));
	    
	    if(currentCcuItem != 0) {
		ModuleInfoPage *m = p->getModuleInfoPage(ccuAddress);
	    
		if(m != NULL) {
		    // check nbModule
		    int nbModuleUser = m->getNbModule();
		    int nbModuleSoft = 0;
		    if(moduleList != NULL) {
			for(std::list<keyType>::iterator moduleIt = moduleList->begin() ; moduleIt != moduleList->end() ; moduleIt++) {
			    tscType16 ccuAddr = getCcuKey(*moduleIt);
			    if(ccuAddr == ccuAddress.mid(2).toInt(0,16)) nbModuleSoft++;
			}
		    }
		
		    if(nbModuleUser != nbModuleSoft && nbModuleUser != 0) {
			setCcuError("Wait " + QString::number(nbModuleUser) + " but found " + QString::number(nbModuleSoft) + " Module(s)");
		    }
		
		    // check module adresses		
		    QValueVector<QString> moduleTable = m->getModuleTable();
		    for(QValueVector<QString>::iterator mIt = moduleTable.begin() ; mIt < moduleTable.end() ; mIt = mIt + 3) {
			tscType16 moduleAddress = (*mIt).mid(2).toInt(0, 16);
			bool error = false;
			if(moduleList != NULL) {
			    bool found = false;
			    for(std::list<keyType>::iterator moduleIt = moduleList->begin() ; moduleIt != moduleList->end() ; moduleIt++) {
				keyType moduleIndex = *moduleIt;
				tscType16 moduleAddr = getChannelKey(moduleIndex);
				tscType16 ccuAddr = getCcuKey(moduleIndex);
				if(moduleAddr == moduleAddress && ccuAddr == ccuAddress.mid(2).toInt(0,16)) {
				    found = true;
				    moduleIt = moduleList->end();
				    moduleIt--;
				}
			    }			
			    if(!found) error = true;
			}
			else error = true;
		    
			if(error) { // error : no dcu on ccu found
			    setCcuError("Module on i2c channel 0x" + QString::number(moduleAddress, 16) + " not found");
			}
		    }				
		}	    
	    }
	}		
    }
}

void TestResultsImpl::createCcuItem(QString ccuAddress) {
    currentCcuItem = new QListViewItem(currentFecRingItem);
    currentCcuItem->setOpen(false);
    QString label = "CCU 0x" + ccuAddress;
    currentCcuItem->setText( 0, tr(label) ); 
    currentCcuItem->setPixmap( 1, greenLed );
}

void TestResultsImpl::createModuleItem(QString moduleText, bool module) {
    currentModuleItem = new QListViewItem(currentCcuItem);
    currentModuleItem->setOpen(false);
    QString label = moduleText;
    currentModuleItem->setText( 0, tr(label) ); 
    currentModuleItem->setPixmap( 1, greenLed );
    
    if(module) {
	createDeviceItem("DCU 0x0");
	createDeviceItem("APV 0x20");
	createDeviceItem("APV 0x21");
	createDeviceItem("APV 0x22");
	createDeviceItem("APV 0x23");
	createDeviceItem("APV 0x24");
	createDeviceItem("APV 0x25");
	createDeviceItem("PLL 0x43");
	createDeviceItem("MUX 0x44");
	createDeviceItem("LaserDriver 0x60");
	
	currentModuleItem->sortChildItems(0, true);
    }       
}

void TestResultsImpl::createDeviceItem(QString deviceText) {
    currentDeviceItem = new QListViewItem(currentModuleItem);
    currentDeviceItem->setOpen(false);
    QString label = deviceText;
    currentDeviceItem->setText( 0, tr(label) ); 
    currentDeviceItem->setPixmap( 1, greenLed );
}

void TestResultsImpl::setCurrentCcuItem(tscType16 ccuAddress, bool dummy) {
     currentCcuItem = currentFecRingItem->firstChild();
 
     if(dummy) {
	 while(currentCcuItem != 0 && !((currentCcuItem->text(0)).startsWith("Dummy"))) {
	     currentCcuItem = currentCcuItem->itemBelow();
	 }
     }
     else {
	 while(currentCcuItem != 0 && ccuAddress != (currentCcuItem->text(0)).mid(6).toInt(0,16)) {
	     currentCcuItem = currentCcuItem->itemBelow();
	 }
     }
}

void TestResultsImpl::setFecError(QString errMessage) {
    currentFecRingItem->setPixmap( 1, redLed );
    QString text = currentFecRingItem->text(2);
    if(text != "") text += ", ";
    currentFecRingItem->setText( 2, tr( text + "Fec : " + errMessage ) );

    anErrorOccured = true;
}

void TestResultsImpl::setRingError(QString errMessage) {
    currentFecRingItem->setPixmap( 1, redLed );
    if(errMessage != "") {
	QString text = currentFecRingItem->text(2);
	if(text != "") text += ", ";
	currentFecRingItem->setText( 2, tr( text + "Ring : " + errMessage ) );
    }
    anErrorOccured = true;
}

void TestResultsImpl::setCcuError(QString errMessage) {
    currentCcuItem->setPixmap( 1, redLed );
    if(errMessage != "") {
	QString text = currentCcuItem->text(2);
	if(text != "") text += ", ";
	currentCcuItem->setText( 2, tr( text + errMessage ) );
    }       
    setRingError();
}

void TestResultsImpl::setModuleError(QString errMessage) {
     currentModuleItem->setPixmap( 1, redLed );
     if(errMessage != "") {
	 QString text = currentModuleItem->text(2);
	 if(text != "") text += ", ";
	 currentModuleItem->setText( 2, tr( text + errMessage ) );
     }
     setCcuError();
}

void TestResultsImpl::setDeviceError(int deviceIndex, QString errMessage) {

    currentDeviceItem = currentModuleItem->firstChild(); // currentDeviceItem set on first apv item

    int neg = 0;
    if(currentModuleItem->childCount() != 10) neg = 2;

    for(int i = 0 ; i < deviceIndex-neg ; i++) {
	currentDeviceItem = currentDeviceItem->itemBelow(); // set currentDeviceItem on the good device item
    }

    if(errMessage == "i2cSpeed") {
	errMessage = "i2c speed error : Inconsistency error at 100 kHz => cannot download/upload it";
    }
    else if(errMessage == "") {    
	switch(deviceIndex) {
	case DCU:
	    errMessage = "DCU missing";
	    break;
	case APV20:	    
	case APV21:	    
	case APV22:	    
	case APV23:
	case APV24:	    
	case APV25:
	    errMessage = "APV missing";
	    break;
	case PLL:
	    errMessage = "PLL missing";
	    break;
	case MUX:
	    errMessage = "MUX missing";
	    break;
	case LASERDRIVER:
	    errMessage = "LASERDRIVER missing";
	    break;
	}	
    }

    currentDeviceItem->setPixmap( 1, redLed );
    QString text = currentDeviceItem->text(2);
    if(text != "") text += ", ";
    currentDeviceItem->setText( 2, tr( text + errMessage ) );
    setModuleError();
}

void TestResultsImpl::setFecRingInfo(QString infoMessage) {
    QString text = currentFecRingItem->text(2);
    if(text != "") text += ", ";
    currentFecRingItem->setText(2, tr(text + "Info : " + infoMessage));
    QImage currentIm = (currentFecRingItem->pixmap(1))->convertToImage();
    if(currentIm != redLed.convertToImage())
	currentFecRingItem->setPixmap(1, orangeLed);
}

void TestResultsImpl::setModuleInfo(QString infoMessage) {
    QString text = currentModuleItem->text(2);
    if(text != "") text += ", ";
    currentModuleItem->setText( 2, tr( text + "Info : " + infoMessage ) );
}

void TestResultsImpl::setDeviceInfo(int deviceIndex, QString infoMessage) {
    currentDeviceItem = currentModuleItem->firstChild(); // currentDeviceItem set on first apv item
    int neg = 0;
    if(currentModuleItem->childCount() != 10) neg = 2;
    for(int i = 0 ; i < deviceIndex-neg ; i++) {
	currentDeviceItem = currentDeviceItem->itemBelow(); // set currentDeviceItem on the good device item
    }

    QString text = currentDeviceItem->text(2);
    if(text != "") text += ", ";
    currentDeviceItem->setText( 2, tr( text + "Info : " + infoMessage ) );
}

void TestResultsImpl::setOnly4APV(bool undefined) {
    currentDeviceItem = currentModuleItem->firstChild(); // currentDeviceItem set on 1st APV item
    currentDeviceItem = currentDeviceItem->itemBelow(); // currentDeviceItem set on 2nd APV item
    currentDeviceItem = currentDeviceItem->itemBelow(); // currentDeviceItem set on 3rd APV item
    
    if(undefined) { // inform that only 4 APV have been detected
	bool okForOrangeLed = false;
	QImage currentIm;
	
	QString text = currentDeviceItem->text(2);
	if(text != "") text += ", ";
	currentDeviceItem->setText( 2, tr( text + "Warning : not detected") );
	currentIm = (currentDeviceItem->pixmap(1))->convertToImage();
	if(redLed.convertToImage() != currentIm) { // set the led to orange
	    okForOrangeLed = true;
	    currentDeviceItem->setPixmap( 1, orangeLed );
	}
	
	currentDeviceItem = currentDeviceItem->itemBelow(); // currentDeviceItem set on 4th APV item
	text = currentDeviceItem->text(2);
	if(text != "") text += ", ";
	currentDeviceItem->setText( 2, tr( text + "Warning : not detected") );
	if(okForOrangeLed) { // set the led to orange
	    currentDeviceItem->setPixmap( 1, orangeLed );
	}
	
	currentIm = (currentModuleItem->pixmap(1))->convertToImage();
	if(redLed.convertToImage() != currentIm) currentModuleItem->setPixmap(1, orangeLed);
	currentIm = (currentCcuItem->pixmap(1))->convertToImage();
	if(redLed.convertToImage() != currentIm) currentCcuItem->setPixmap(1, orangeLed);
	currentIm = (currentFecRingItem->pixmap(1))->convertToImage();
	if(redLed.convertToImage() != currentIm) currentFecRingItem->setPixmap(1, orangeLed);	
    }
    else { // disable APV 0x22 and 0x23 because only 4 APV are awaited
	QListViewItem *secondAPVToDelete = currentDeviceItem->itemBelow();
	delete(secondAPVToDelete);
	delete(currentDeviceItem);
    }
} 

void TestResultsImpl::generateXml() {
    time_t timestamp;
    struct tm *t;
    
    /* read of date and time */
    timestamp = time (NULL);
    t = gmtime(&timestamp);
    char temp_time[16];
    strftime (temp_time, 16, "%m%d%Y_%H%M%S", t);

    QFile file(supervisor->getResDir() + QString(temp_time) + ".xml");
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream stream( &file );
 
	QValueVector<QString> psCurrents;
	int nbFecRing;
	if((GUISupervisor::testSelected & psCurrentsRecordMask) && !anErrorOccured) {
	    psCurrents = supervisor->getPSCurrent();
#ifdef TOBTEST
	    int nbElement = 0;
	    for (QValueVector<QString>::iterator itt = psCurrents.begin() ; itt < psCurrents.end() ; itt++) nbElement++;
	    nbFecRing = nbElement/2;
#else
	    nbFecRing = psCurrents.count()/2;
#endif
	}
	
	int indFecRing = 0;
	
	// header of the xml file
	stream << "<?xml version=\"1.0\"?>" << endl;
	stream << "<!-- TOB control loop test result file -->" << endl;
	stream << "<group detector=\"TOB\" operator=\"ControlLoopTester v1.0\" datetime=\"" << timestamp << "\">" << endl;
	
	currentFecRingItem = resultsView->firstChild();
	
	do {
	    currentFecRingItem->setOpen(false);
	    QString fecRingLabel = currentFecRingItem->text(0);
	    int startRingIndex = fecRingLabel.find('R', 5);
	    QString fecSlot = fecRingLabel.mid(4, startRingIndex-5);
	    QString ringSlot = fecRingLabel.right(1);
	
	    QStringList fecRingResults = QStringList::split(",", currentFecRingItem->text(2));
	    
	    // Fec results
	    stream << "   <fec slot=\"" << fecSlot << "\">" << endl;
	    
	    // PS currents
	    if((GUISupervisor::testSelected & psCurrentsRecordMask) && !anErrorOccured) {
		stream << "      <!-- PowerSupply currents record -->" << endl;
		stream << "      <ps currents before>"<< endl;
		stream << "         <value =\"" << psCurrents[indFecRing] << "\">" << endl;
		stream << "      </ps currents before>"<< endl;
		stream << "      <ps currents after>"<< endl;
		stream << "         <value =\"" << psCurrents[indFecRing+ nbFecRing] << "\">" << endl;
		stream << "      </ps currents after>"<< endl;	    
		
		indFecRing++;
	    }	    
	    
	    stream << "      <!-- Test FEC registers (read/write) -->" << endl;
	    for(QStringList::iterator it = fecRingResults.begin() ; it != fecRingResults.end() ; ++it) {
		QString res = *it;
		res = res.stripWhiteSpace();
		if(res.startsWith("Fec")) { // fec result
		    stream << "      <failure reason=\"" << res.mid(6) << "\" transaction=\"fec test\">" << endl;
		}
	    }
	  
	    // Ring results
	    stream << "      <ring slot=\"" << ringSlot << "\">" << endl;
	    for(QStringList::iterator it = fecRingResults.begin() ; it != fecRingResults.end() ; ++it) {
		QString res = *it;
		res = res.stripWhiteSpace();
		if(res.startsWith("Ring")) { // fec result
		    res = res.mid(7);
		    if(res.startsWith("Duplicated")) 			
			stream << "         <failure reason=\"" << res << "\" transaction=\"ccu detection\">" << endl;
		    else if(res.startsWith("Ring")) 
			stream << "         <failure reason=\"" << res << "\" transaction=\"ccu test\">" << endl;
		    else if(res.startsWith("Dummy")) 
			stream << "         <failure reason=\"" << res << "\" transaction=\"redundancy test\">" << endl;
		    else if(res.startsWith("Wait address")) 
			stream << "         <failure reason=\"" << res << "\" transaction=\"ccu address comparison\">" << endl;
		    else if(res.startsWith("Wait")) 
			stream << "         <failure reason=\"" << res << "\" transaction=\"ccu number comparison\">" << endl;		
		}
		else if(res.startsWith("Info")) {
		    stream << "         <info message=\"" << res.mid(7) << "\">" << endl;
		}
	    }
	 
	    // CCU results
	    bool nextFecRing = false;
	    currentCcuItem = currentFecRingItem->firstChild();
	    if(currentCcuItem != 0 && (currentCcuItem->text(0).startsWith("CCU") || currentCcuItem->text(0).startsWith("Dummy"))) {
	      do {
		currentCcuItem->setOpen(false);
		QString ccuAddress;
		if(currentCcuItem->text(0).startsWith("CCU"))
		  ccuAddress = (currentCcuItem->text(0)).mid(4);
		else
		  ccuAddress= currentCcuItem->text(0);
		QStringList ccuResults = QStringList::split(",", currentCcuItem->text(2));
		
		stream << "         <ccu address=\"" << ccuAddress << "\">" << endl;
		stream << "            <!-- Test CCU registers (read/write) -->" << endl;
		
		for(QStringList::iterator it = ccuResults.begin() ; it != ccuResults.end() ; ++it) {
		  QString res = *it;
		  res = res.stripWhiteSpace();
		  if(res.startsWith("Cannot")) 			
		    stream << "            <failure reason=\"" << res << "\" transaction=\"redundancy test\">" << endl;
		  else if(res.startsWith("Failure while accessing")) 
		    stream << "            <failure reason=\"" << res << "\" transaction=\"redundancy test\">" << endl;
		  else if(res.startsWith("Link"))
		    stream << "            <failure reason=\"" << res << "\" transaction=\"redundancy test\">" << endl;
		  else if(res.startsWith("Pia")) 
		    stream << "            <failure reason=\"" << res << "\" transaction=\"device detection\">" << endl;
		  else if(res.startsWith("No")) 
		    stream << "            <failure reason=\"" << res << "\" transaction=\"dcu on ccu comparison\">" << endl;
		  else if(res.startsWith("Wait")) 
		    stream << "            <failure reason=\"" << res << "\" transaction=\"module number comparison\">" << endl;
		  else if(res.startsWith("Module")) 
		    stream << "            <failure reason=\"" << res << "\" transaction=\"module address comparison\">" << endl;
		  else
		    stream << "            <failure reason=\"" << res << "\" transaction=\"ccu test\">" << endl;
		}
		
		// Module results
		currentModuleItem = currentCcuItem->firstChild();
		bool nextCcu = false;
		if(currentModuleItem != 0) {
		  QString label = currentModuleItem->text(0);
		  if(label.startsWith("Module") || label.startsWith("DCU") || label.startsWith("DOH")) {
		    do {
		      currentModuleItem->setOpen(false);
		      QString channel;
		      QString moduleLabel = currentModuleItem->text(0);

		      QStringList moduleResults = QStringList::split(",", currentModuleItem->text(2));
		      
		      if(moduleLabel.startsWith("D")) {
			int startChannelIndex = moduleLabel.find('o');
			QString moduleAddress = moduleLabel.mid(4, startChannelIndex-5);
			channel = moduleLabel.mid(startChannelIndex+11);
	
			stream << "            <channel port=\"" << channel << "\">" << endl;
			
			if(moduleLabel.startsWith("DCU")) { // DCU
			  // for the dcu hard id
			  QString dcuKey = "0";
			  for(QStringList::iterator it = moduleResults.begin() ; it != moduleResults.end() ; ++it) {
			    QString res = (*it).stripWhiteSpace();
			    if(res.startsWith("Info : Hard")) dcuKey = (res.mid(17));
			  }
			  
			  stream << "               <dcu address=\"" << moduleAddress << "\" key=\"" << dcuKey << "\">" << endl;
			  
			  for(QStringList::iterator it = moduleResults.begin() ; it != moduleResults.end() ; ++it) {
			    QString res = *it;
			    res = res.stripWhiteSpace();
			    if(res.startsWith("i2c")) 			
			      stream << "                  <failure reason=\"" << res << "\" transaction=\"device test\">" << endl;
			    else if(res.startsWith("Info : i2c")) 
			      stream << "                  <info message=\"" << res.mid(7) << "\">" << endl;
			    else if(!res.startsWith("Info"))
			      stream << "                  <failure reason=\"" << res << "\" transaction=\"config test\">" << endl;
			  }
			  stream << "               </dcu>" << endl;
			}
			else { // DOH
			  stream << "               <doh address=\"" << moduleAddress << "\">" << endl;
			  
			  for(QStringList::iterator it = moduleResults.begin() ; it != moduleResults.end() ; ++it) {
			    QString res = *it;
			    res = res.stripWhiteSpace();
			    if(res.startsWith("i2c")) 			
			      stream << "                  <failure reason=\"" << res << "\" transaction=\"device test\">" << endl;
			    else if(res.startsWith("Info")) 
			      stream << "                  <info message=\"" << res.mid(7) << "\">" << endl;
			    else
			      stream << "                  <failure reason=\"" << res << "\" transaction=\"config test\">" << endl;
			  }
			  stream << "               </doh>" << endl;
			}
		      }
		      else { // Module
			channel = moduleLabel.mid(22);
			
			stream << "            <channel port=\"" << channel << "\">" << endl;

			for(QStringList::iterator it = moduleResults.begin() ; it != moduleResults.end() ; ++it) {
			  QString res = *it;
			  res = res.stripWhiteSpace();
			  if(res.startsWith("Info")) 
			    stream << "               <info message=\"" << res.mid(7) << "\">" << endl;
			}
			
			currentDeviceItem = currentModuleItem->firstChild();
			bool nextModule = false;
			if(currentDeviceItem != 0) {
			  QString devLabel = currentDeviceItem->text(0);
			  if(devLabel.startsWith("APV")   ||
			     devLabel.startsWith("DCU")   ||
			     devLabel.startsWith("Laser") ||
			     devLabel.startsWith("MUX")   ||
			     devLabel.startsWith("PLL")) {
			    do  {
			      currentDeviceItem->setOpen(false);
			      QString deviceLabel = currentDeviceItem->text(0);
			      
			      if(deviceLabel.startsWith("APV")) {
				QString apvAddress = deviceLabel.mid(4);
				stream << "               <apv address=\"" << apvAddress << "\">" << endl;
			      }
			      else if(deviceLabel.startsWith("MUX")) {
				QString muxAddress = deviceLabel.mid(4);
				stream << "               <mux address=\"" << muxAddress << "\">" << endl;
			      }
			      else if(deviceLabel.startsWith("PLL")) {
				QString pllAddress = deviceLabel.mid(4);
				stream << "               <pll address=\"" << pllAddress << "\">" << endl;
			      }
			      else if(deviceLabel.startsWith("Laser")) {
				QString aohAddress = deviceLabel.mid(12);
				stream << "               <aoh address=\"" << aohAddress << "\">" << endl;
			      }
			      else if(deviceLabel.startsWith("DCU")) {
				// for the dcu hard id
				QString dcuKey = "0";
				QStringList deviceResults = QStringList::split(",", currentDeviceItem->text(2));
				for(QStringList::iterator it = deviceResults.begin() ; it != deviceResults.end() ; ++it) {
				  QString res = (*it).stripWhiteSpace();
				  if(res.startsWith("Info : Hard")) dcuKey = (res.mid(17));
				}

				QString dcuAddress = deviceLabel.mid(4);
				stream << "               <dcu address=\"" << dcuAddress << "\" key=\"" << dcuKey << "\">" << endl;
			      }
			      
			      QStringList deviceResults = QStringList::split(",", currentDeviceItem->text(2));
			      for(QStringList::iterator it = deviceResults.begin() ; it != deviceResults.end() ; ++it) {
				QString res = *it;
				res = res.stripWhiteSpace();
				if(res.startsWith("i2c")) 			
				  stream << "                  <failure reason=\"" << res << "\" transaction=\"device test\">" << endl;
				else if(res.endsWith("missing")) 
				  stream << "                  <failure reason=\"" << res << "\" transaction=\"device test\">" << endl;
				/*else if(res.startsWith("Info")) 
				  stream << "                  <info message=\"" << res.mid(7) << "\">" << endl;*/
				else if(!res.startsWith("Info")) 
				  stream << "                  <failure reason=\"" << res << "\" transaction=\"configuration test\">" << endl;
			      }
			      
			      if(deviceLabel.startsWith("APV")) 		stream << "               </apv>" << endl;
			      else if(deviceLabel.startsWith("MUX")) 	stream << "               </mux>" << endl;
			      else if(deviceLabel.startsWith("PLL")) 	stream << "               </pll>" << endl;
			      else if(deviceLabel.startsWith("Laser"))	stream << "               </aoh>" << endl;
			      else if(deviceLabel.startsWith("DCU")) 	stream << "               </dcu>" << endl;
			      
			      currentDeviceItem = currentDeviceItem->itemBelow();
			      if(currentDeviceItem != 0) {
				if( (currentDeviceItem->text(0)).startsWith("Module") 
				    || (currentDeviceItem->text(0)).startsWith("DOH") 
				    || ((currentDeviceItem->text(0)).startsWith("DCU") && (currentDeviceItem->text(0)).length() > 10)
				    || (currentDeviceItem->text(0)).startsWith("CCU")
				    || (currentDeviceItem->text(0)).startsWith("FEC") )
				  nextModule = true;
			      }
			      else nextModule = true;
			    }
			    while (!nextModule);
			  }
			}
		      }
		      
		      stream << "            </channel>" << endl;
		    
		      currentModuleItem = currentModuleItem->itemBelow();
		  
		      if(currentModuleItem != 0) {
			if( (currentModuleItem->text(0)).startsWith("CCU") || (currentModuleItem->text(0)).startsWith("FEC") ) 
			  nextCcu = true;	
		      }
		      else nextCcu = true;
		    }
		    while (!nextCcu);
		  }
		}
	      
		stream << "         </ccu>" << endl;
		
		currentCcuItem = currentCcuItem->itemBelow();
		if(currentCcuItem != 0) {
		    if(currentCcuItem->text(0).startsWith("FEC")) nextFecRing = true;
		}
		else nextFecRing = true;
	      }
	      while (!nextFecRing);
	    }
	    
	    stream << "      </ring>" << endl;
	    stream << "   </fec>" << endl;
	    
	    currentFecRingItem = currentFecRingItem->itemBelow();
	}
	while (currentFecRingItem != 0);
	
	stream << "</group>" << endl;
	
	file.close();
    }
    else {
	cout << "------------------------------------------------------" << endl;
	cout << "| Error : cannot create the output file |" << endl;
	cout << "------------------------------------------------------" << endl;
	
	close();
    }
}

void TestResultsImpl::uploadXml(std::string fileName) {
  // le constructeur de ifstream permet d'ouvrir un fichier en lecture
  fileName = "ResultsFiles/" + fileName + ".xml";
  std::ifstream fichier(fileName.c_str());
  
  if (fichier) { // ce test échoue si le fichier n'est pas ouvert
    std::string ligne; // variable contenant chaque ligne lue
    
    int component = 0; // indicate wether a fec (1), a ring (2), a ccu (3), a channel (4) or a device (5) is detected
    int device; // indicate which device is detected (0 -> apv20, 1->apv21 2->apv22 3->apv23 4->apv24 5->apv25 6->dcu 7->laserdriver 8->mux 9->pll)
    bool notCreated = false;
    QString channel = "";
    
    // cette boucle s'arrête dès qu'une erreur de lecture survient
    while ( std::getline( fichier, ligne ) ) {
      // afficher la ligne à l'écran
      //cout << ligne << endl;
      
      QString str(ligne.c_str());
      str = str.stripWhiteSpace();
      //cout << str.latin1() << endl;

      if(component == 4 && notCreated) {
	if(str.startsWith("<doh")) {
	  QString dohAddress = str.mid(14, str.length()-16);
	  createModuleItem("DOH " + dohAddress + " on channel " + channel, false);
	}
	else if(str.startsWith("<dcu")) {
	  int end = str.findRev('=') - 19;
	  QString dcuAddress = str.mid(14, end);
	  createModuleItem("DCU " + dcuAddress + " on channel " + channel, false);

	  int start = str.findRev('=');
	  QString hardId = str.mid(start+1, str.findRev('"')-start);
	  setModuleInfo("Hard Id = " + hardId);
	}
	else { // (str.startsWith("<apv")) 
	  component = 5; // for device
	  createModuleItem("Module on i2c channel " + channel);
	}

	notCreated = false;
      }
      
      if(str.startsWith("<fec slot")) { // fec detection
	component = 1; // for fec
	
	currentFecRingItem = new QListViewItem(resultsView);
	currentFecRingItem->setOpen(false);
	QString label = "FEC " + str.mid(11, str.length()-13);
	currentFecRingItem->setText( 0, tr(label) ); 
	currentFecRingItem->setPixmap( 1, greenLed ); // green led for the fec
      }
      else if(str.startsWith("<ring slot")) { // ring detection
	component = 2; // for ring
	
	QString label = currentFecRingItem->text(0);
	label += " Ring " + str.mid(12, str.length()-14);
	currentFecRingItem->setText( 0, tr(label) ); 
      }
      else if(str.startsWith("<ccu address")) { // ccu detection
	component = 3; // for ccu

	if((str.mid(14)).startsWith("Dummy")) {
	  currentCcuItem = new QListViewItem(currentFecRingItem);
	  currentCcuItem->setOpen(false);
	  QString label = str.mid(14, str.length()-16);
	  currentCcuItem->setText( 0, tr(label) ); 
	  currentCcuItem->setPixmap( 1, greenLed );
	}
	else
	  createCcuItem(str.mid(16, str.length()-18));
      }
      else if(str.startsWith("<channel port")) { // channel detection
	component = 4; // for channel
	notCreated = true;

	channel = str.mid(15, str.length()-17);
      }      
      else if(str.startsWith("<apv")) {
	if(str.endsWith("0\">")) 
	  device = 0;
	else if(str.endsWith("1\">")) 
	  device = 1;
	else if(str.endsWith("2\">")) 
	  device = 2;
	else if(str.endsWith("3\">")) 
	  device = 3;
	else if(str.endsWith("4\">")) 
	  device = 4;
	else // if(str.endsWith("5\">")) 
	  device = 5;
      }
      else if(str.startsWith("<dcu") && component == 5) {
	device = 6;
	int start = str.findRev('=');
	QString hardId = str.mid(start+1, str.findRev('"')-start);
	setDeviceInfo(DCU, "Hard Id = " + hardId);
      }
      else if(str.startsWith("<aoh")) {
	device = 7;
      }
      else if(str.startsWith("<mux")) {
	device = 8;
      }
      else if(str.startsWith("<pll")) {
	device = 9;
      }
      else if(str.startsWith("<failure")) {
	int lastInd;
	switch(component) {
	case 1: // fec error
	  setFecError(str.mid(17, str.length()-42));
	  break;
	case 2: // ring error
	  lastInd = str.findRev('=');
	  setRingError(str.mid(17, lastInd+1-31));
	  break;
	case 3: // ccu error
	  lastInd = str.findRev('=');
	  setCcuError(str.mid(17, lastInd+1-31));
	  break;
	case 4: // channel error
	  setModuleError(str.mid(17, lastInd+1-31));
	  break;
	case 5:
	  QString errMessage;

	  lastInd = str.findRev('=');
	  str = str.mid(17, lastInd+1-31);

	  if(str.startsWith("Warning")) setOnly4APV(true);
	  else {

	    if(str.startsWith("i2c")) errMessage = "i2cSpeed";
	    else if(str.endsWith("missing")) errMessage = "";
	    else errMessage = str;
	    
	    switch(device) {
	    case 0:
	      setDeviceError(APV20, errMessage);
	      break;
	    case 1:
	      setDeviceError(APV21, errMessage);
	      break;
	    case 2:
	      setDeviceError(APV22, errMessage);
	      break;
	    case 3:
	      setDeviceError(APV23, errMessage);
	      break;
	    case 4:
	      setDeviceError(APV24, errMessage);
	      break;
	    case 5:
	      setDeviceError(APV25, errMessage);
	      break;
	    case 6:
	      setDeviceError(DCU, errMessage);
	      break;
	    case 7:
	      setDeviceError(LASERDRIVER, errMessage);
	      break;
	    case 8:
	      setDeviceError(MUX, errMessage);
	      break;
	    case 9:
	      setDeviceError(PLL, errMessage);
	      break;
	    }
	  }
	  break;
	}
      }
      else if(str.startsWith("<info")) {
	str = str.mid(15, str.length()-17);
	switch(component) {
	case 2:
	  setFecRingInfo(str);
	  break;
	case 4:
	case 5:
	  setModuleInfo(str);
	  break;
	}
      }
    }
  }
  else {
    cout << "------------------------------------------------------" << endl;
    cout << "| Error : cannot create the output file |" << endl;
    cout << "------------------------------------------------------" << endl;
    
    close();
  }
}

QString TestResultsImpl::doublon(QValueVector<QString> vec) {
  QString res = vec[0];

  for(unsigned int i = 1 ; i < vec.size() ; i++) {
    QString temp = vec[i];
    if(temp.compare(res) != 0) {
      res = "";
      i = vec.size();
    }
  }

  return res;
}
