
#include "LoadPreferencesImpl.h"

#include <qlistbox.h>
#include <qdir.h>
#include <qlineedit.h>

LoadPreferencesImpl::LoadPreferencesImpl(TestConfigurationImpl *p, GUISupervisor *guiSupervisor) : LoadPreferences(p, "LoadPreferences", true) {   
    supervisor = guiSupervisor;
    parent_ = p;

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

void LoadPreferencesImpl::fileNameSelected() {
    QString name = fileListBox->currentText();
    name = name.left(name.length()-4);
    fileNameLineEdit->setText(name);
}

void LoadPreferencesImpl::back() {
    close();
}

void LoadPreferencesImpl::loadPreferences() {
    readXml(fileNameLineEdit->text());
    close();
}

void LoadPreferencesImpl::readXml(QString fileName) {
    // reading...
    QStringList lines;
    QFile file( supervisor->getPrefDir() + fileName + ".xml" );
    if ( file.open( IO_ReadOnly ) ) {
	QTextStream stream( &file );
	QString line;
	while ( !stream.atEnd() ) {
	    line = stream.readLine(); // line of text excluding '\n'
	    lines += line;
	}
	file.close();
    }

    // -----------
    
    // interpreting.........
    QStringList::iterator it = lines.begin();
    // retrieve selected test
    QString str = *it;
    int length = str.length() - 17;
    GUISupervisor::testSelected = (str.mid(16, length)).toInt();

    str = *(++it);
    length = str.length() - 19;
    GUISupervisor::specialI2CTestSelected = (str.mid(18, length)).toInt();
    // ------
    // retrieve nb loop on modules
    str = *(++it);
    length = str.length() - 20;
    supervisor->setNbLoopOnModules((str.mid(19, length)).toInt());
    // ------
    // retrieve fecring detected
    int selectedStart = lines.findIndex("   <selected>");
    int selectedEnd  = lines.findIndex("   </selected>");
    int nbSelected = selectedEnd - (selectedStart + 1);
    QStringList selectedList = lines.grep("FecRing =");
    int inc = 0;
    QValueVector<int> v;
    for(QStringList::iterator s = selectedList.begin() ; s != selectedList.end() ; ++s) {
	int length = (*s).length() - 18;
	QString fecRingKey =  (*s).mid(17, length);
	int virgIndice = fecRingKey.find(',');
	
	if(inc < nbSelected) {
	    v.push_back(fecRingKey.left(virgIndice).toInt());
	    v.push_back(fecRingKey.right(virgIndice).toInt());
	    v.push_back(1);
	    inc++;
	}
	else {
	    v.push_back(fecRingKey.left(virgIndice).toInt());
	    v.push_back(fecRingKey.right(virgIndice).toInt());
	    v.push_back(0);
	}
    }
    supervisor->updateFecRingSelected(v);
    // ------
    
    // retrieve dcu thresholds
    int thrOnCcuStart = lines.findIndex("<DcuOnCcuThresholds>");
    int thrOnCcuEnd  = lines.findIndex("</DcuOnCcuThresholds>");
    int nbThrOnCcu = thrOnCcuEnd - (thrOnCcuStart + 1);
    QStringList thrList = lines.grep("channel");
    inc = 0;
    QStringList dcuOnCcuThr;
    QStringList dcuOnModuleThr;
    for(QStringList::iterator s = thrList.begin() ; s != thrList.end() ; ++s) {
	QStringList temp = QStringList::split("/", (*s).stripWhiteSpace());
	QString dcuThr;
	for(QStringList::iterator tempIt = temp.begin() ; tempIt != temp.end() ; ++tempIt) {
	    (*tempIt).stripWhiteSpace();
	    if((*tempIt).startsWith("<")) {
		dcuThr = (*tempIt).right(1) + "/";
	    }
	    else if((*tempIt).startsWith("Min")) {
		int length = (*tempIt).length() - 12;
		dcuThr += (*tempIt).right(length) + "/";
	    }
	    else { // starts with "Max"
		int length = (*tempIt).length() - 13;
		dcuThr += (*tempIt).mid(12, length);
	    }	    
	}
	
	if(inc < nbThrOnCcu) dcuOnCcuThr << dcuThr;
	else dcuOnModuleThr << dcuThr;
	
	inc++;
    }
    supervisor->setDcuOnCcuThr(dcuOnCcuThr);
    supervisor->setDcuOnModuleThr(dcuOnModuleThr);
    // ------
    
    // retrieve ccuinfo
    int ccuInfoStart = lines.findIndex("<CCUInfo>");
    for(int i = 0 ; i < ccuInfoStart+1 ; i++) lines.pop_front();
    int nbCcuInfoLines = lines.findIndex("</CCUInfo>");
    it = lines.begin();
    int nbCCUInfoRetrieved = 0;
    QString fecRingKey;
    QString ccuKey;
    CCUInfoPage *p = new CCUInfoPage();
    ModuleInfoPage *m = new ModuleInfoPage();
    QValueVector<QString> ccuTable;
    QValueVector<QString> moduleTable;
    for(int i = 0 ; i < nbCcuInfoLines ; i++) {
	QString line = (*it).stripWhiteSpace();
	if(line.startsWith("<FEC-Ring")) {
	    int length = line.length() - 13;
	    fecRingKey = line.mid(12, length);
	    p = new CCUInfoPage();
	}
	else if(line.startsWith("<NbCCU")) {
	    int length = line.length() - 10;
	    p->setNbCCU(line.mid(9, length).toInt());
	}
	else if(line.startsWith("<DcuDistribution")) {
	    p->setCheckBoxGroup(line.mid(19, 1).toInt());
	}
	else if(line.startsWith("<sameModuleConfig")) {
	    int length = line.length() - 21;
	    QString str = line.mid(20, length);
	    if(str == "true") p->setSameModuleConfig(true);
	    else p->setSameModuleConfig(false);
	}
	else if(line.startsWith("<position")) {
	    int length = line.length() - 13;
	    ccuTable.push_back(line.mid(12, length));
	}
	else if(line.startsWith("<address")) {
	    int length = line.length() - 12;
	    ccuKey = line.mid(11, length);
	    ccuTable.push_back(ccuKey);
	    m = new ModuleInfoPage();
	}
	else if(line.startsWith("<dcu")) {
	    int length = line.length() - 8;
	    ccuTable.push_back(line.mid(7, length));
	}	
	else if(line.startsWith("<hardId")) {
	    int length = line.length() - 11;
	    ccuTable.push_back(line.mid(10, length));
	}
	else if(line.startsWith("<NbModule")) {
	    int length = line.length() - 13;
	    m->setNbModule(line.mid(12, length).toInt());
	}
	else if(line.startsWith("<NbAPV")) {
	    m->setCheckBoxGroup(line.mid(9, 1).toInt());
	}
	else if(line.startsWith("<i2cAddress")) {
	    int length = line.length() - 15;
	    moduleTable.push_back(line.mid(14, length));
	}
	else if(line.startsWith("<nbAPV")) {
	    int length = line.length() - 10;
	    moduleTable.push_back(line.mid(9, length));
	}
	else if(line.startsWith("<DcuHardId")) {
	    int length = line.length() - 14;
	    moduleTable.push_back(line.mid(13, length));
	}
	else if(line.startsWith("</CCU")) {
	    m->setModuleTable(moduleTable);
	    p->updateModuleInfoPage(ccuKey, m);
	    moduleTable.clear();
	}
	else if(line.startsWith("</FEC-Ring")) {
	    p->setCCUTable(ccuTable);
	    supervisor->updateCCUInfoPage(fecRingKey, p);
	    ccuTable.clear();
	    nbCCUInfoRetrieved++;
	    if(nbCCUInfoRetrieved == nbSelected) i = nbCcuInfoLines;
	}	
	it++;
    }
    // ------
    // ----------------
    
    
    parent_->updateSelectedTestCheckBox();
}

