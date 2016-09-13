#include "SavePreferencesImpl.h"

#include <qlistbox.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qtextstream.h>
#include <qfile.h>

SavePreferencesImpl::SavePreferencesImpl(QWidget *parent, GUISupervisor *guiSupervisor) : SavePreferences(parent, "SavePreferences", true) {   
    supervisor = guiSupervisor;
    
    QDir d = QDir(supervisor->getPrefDir());
    if(d.exists()) {
      d.setNameFilter("*.xml");
      
      const QFileInfoList *list = d.entryInfoList();
      QFileInfoListIterator it( *list );
      QFileInfo *fi;
      
      while ( (fi = it.current()) != 0 ) {
	fileListBox->insertItem( tr( fi->fileName().latin1() ) );
	++it;
      }
    }
}

void SavePreferencesImpl::fileNameSelected() {
    QString name = fileListBox->currentText();
    name = name.left(name.length()-4);
    fileNameLineEdit->setText(name);
}

void SavePreferencesImpl::back() {
    close();
}

void SavePreferencesImpl::savePreferences() {
    generateXml(fileNameLineEdit->text());
    close();
}

void SavePreferencesImpl::generateXml(QString fileName) {
    QFile file(supervisor->getPrefDir() + fileName + ".xml");
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream stream( &file );
	
	// record of selected test
	stream << "<selectedTest = " << QString::number(GUISupervisor::testSelected) << ">" << endl;
	stream << "<specialI2CTest = " << QString::number(GUISupervisor::specialI2CTestSelected) << ">" << endl;
	// ------------------------------

	// record of nb loop on modules
	stream << "<nbLoopOnModules = " << QString::number(supervisor->getNbLoopOnModules()) << ">" << endl;
	// ------------------------------
	
	QValueVector<QString> selectedFecRing;
	QValueVector<QString> notSelectedFecRing;
	
	QValueVector<int> fecRing = supervisor->getFecRingSelected();
	for(QValueVector<int>::iterator it = fecRing.begin() ; it < fecRing.end() ; it = it + 3) {
	    int fecSlot = *it;
	    int ringSlot = *(it+1);
	    QString fecRingKey = QString::number(fecSlot) + "," + QString::number(ringSlot);
	    if(*(it+2) == 1) selectedFecRing.push_back(fecRingKey);
	    else notSelectedFecRing.push_back(fecRingKey);
	}
	
	// record of selected FecRing
	stream << "<FecRing detected>" << endl;
	stream << "   <selected>" << endl;
	for(QValueVector<QString>::iterator it = selectedFecRing.begin() ; it < selectedFecRing.end() ; it++) 	
	    stream << "      <FecRing = " << *it << ">" << endl;
	stream << "   </selected>" << endl;	
	//--------------------------------------
	// record of not selected FecRing
#ifdef TOBTEST
	QValueVector<QString>::iterator debut = notSelectedFecRing.begin();
	QValueVector<QString>::iterator end = notSelectedFecRing.end();
	if(debut != end) {
#else
	if(notSelectedFecRing.count() > 0) {
#endif
	    stream << "   <notSelected>" << endl;
	    for(QValueVector<QString>::iterator it = notSelectedFecRing.begin() ; it < notSelectedFecRing.end() ; it++) 	
		stream << "      <FecRing = " << *it << ">" << endl;
	    stream << "   </notSelected>" << endl;	
	}
	stream << "</FecRing detected>" << endl;
	//--------------------------------------
	
	// record dcu on ccu thr (if values have been entered)
	QStringList dcuOnCcuThr = supervisor->getDcuOnCcuThr();
	if(!dcuOnCcuThr.empty()) {
	    stream << "<DcuOnCcuThresholds>" << endl;
	    for(QStringList::iterator it = dcuOnCcuThr.begin() ; it != dcuOnCcuThr.end() ; ++it) {
		QStringList l = QStringList::split("/", *it);
		for(QStringList::iterator chIt = l.begin() ; chIt != l.end() ; ++chIt) {
		    stream << "   <channel " << *chIt << "/Min Value = ";
		    stream << *(++chIt) << "/Max Value = ";
		    stream << *(++chIt) << ">"<< endl;
		}
	    }
	    stream << "</DcuOnCcuThresholds>"<< endl;
	}
	// -------------------------------------
	
	// record dcu on module thr (if values have been entered)
	QStringList dcuOnModuleThr = supervisor->getDcuOnModuleThr();
	if(!dcuOnModuleThr.empty()) {
	    stream << "<DcuOnModuleThresholds>" << endl;
	    for(QStringList::iterator it = dcuOnModuleThr.begin() ; it != dcuOnModuleThr.end() ; ++it) {
		QStringList l = QStringList::split("/", *it);
		for(QStringList::iterator chIt = l.begin() ; chIt != l.end() ; ++chIt) {
		    stream << "   <channel " << *chIt << "/Min Value = ";
		    stream << *(++chIt) << "/Max Value = ";
		    stream << *(++chIt) << ">"<< endl;
		}
	    }
	    stream << "</DcuOnModuleThresholds>"<< endl;
	}
	// -------------------------------------
	
	// record of ccu info (for selected FecRing only)
	stream << "<CCUInfo>" << endl;
	for(QValueVector<QString>::iterator it = selectedFecRing.begin() ; it < selectedFecRing.end() ; it++) {
	    QString fecRingKey = *it;
	    CCUInfoPage *p = supervisor->getCCUInfoPage(fecRingKey);
	    if(p != NULL) {
		QValueVector<QString> ccuTable = p->getCCUTable();
		QString sameModuleConfig;
		if(p->sameModuleConfig()) sameModuleConfig = "true";
		else sameModuleConfig = "false";
	    
		stream << "   <FEC-Ring = " << fecRingKey << ">" << endl;
		stream << "      <NbCCU = " << p->getNbCCU() << ">" <<endl;
		stream << "      <DcuDistribution = " << p->getCheckBoxGroup() << ">" << endl;
		stream << "      <sameModuleConfig = " << sameModuleConfig << ">" << endl;
		for(QValueVector<QString>::iterator ccuTableIt = ccuTable.begin() ; ccuTableIt < ccuTable.end() ; ccuTableIt = ccuTableIt + 4) {
		    QString position = *ccuTableIt;
		    QString address = *(ccuTableIt+1);
		    QString dcu = *(ccuTableIt+2);
		    QString dcuHardId = *(ccuTableIt+3);
		    
		    stream << "      <CCU>" << endl;
		    stream << "         <position = " << position << ">" << endl;
		    stream << "         <address = " << address << ">" << endl;
		    stream << "         <dcu = " << dcu << ">" << endl;
		    stream << "         <hardId = " << dcuHardId << ">" << endl;
		    // record of modules info (for selected FecRing only)
		    stream << "         <ModulesInfo>" << endl;
		    ModuleInfoPage *m = p->getModuleInfoPage(address);
		    if(m != NULL) {
			stream << "            <NbModule = " << m->getNbModule() << ">" << endl;
			stream << "            <NbAPV = " << m->getCheckBoxGroup() << ">" << endl;
			QValueVector<QString> moduleTable = m->getModuleTable();
			for(QValueVector<QString>::iterator moduleTableIt = moduleTable.begin() ; moduleTableIt < moduleTable.end() ; moduleTableIt = moduleTableIt + 3) {
			    QString i2cAddress = *moduleTableIt;
			    QString nbAPV = *(moduleTableIt+1);
			    QString dcuHardId = *(moduleTableIt+2);
			    stream << "            <Module>" << endl;
			    stream << "               <i2cAddress = " << i2cAddress << ">" << endl;
			    stream << "               <nbAPV = " << nbAPV << ">" << endl;
			    stream << "               <DcuHardId = " << dcuHardId << ">" << endl;
			    stream << "            </Module>" << endl;
			}		    
		    }
		    stream << "         </ModulesInfo>" << endl;
		    // ----------------------------------------------------
		    stream << "      </CCU>" << endl;
		}
		stream << "   </FEC-Ring>" << endl;
	    }
	}	    
	stream << "</CCUInfo>" << endl;
	// ----------------------------------------------------
	file.close();
    }
}
