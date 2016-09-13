#ifndef CCUINFOPAGE_H
#define CCUINFOPAGE_H

#include "ModuleInfoPage.h"

#include <qlistbox.h>
#include <qstringlist.h>
#include <qmap.h>

class CCUInfoPage {
    private:
    int nbCcu;
    int checkBoxGroupValue;
    QValueVector<QString> ccuTable;
    QMap<QString, ModuleInfoPage *> moduleInfoPages;
    bool moduleConfig;
    
    public:
    CCUInfoPage() { 
	nbCcu = 0; 
	checkBoxGroupValue = 3;
	moduleConfig = false;
    }
        
    void setNbCCU(int nbC) { nbCcu = nbC; }
    void setCheckBoxGroup(int v) { checkBoxGroupValue = v; }
    void setCCUTable(QValueVector<QString> v) { ccuTable = v; }
    void updateModuleInfoPage(QString key, ModuleInfoPage *m) { moduleInfoPages[key] = m; }
    void setSameModuleConfig(bool config) { moduleConfig = config; }
    
    int getNbCCU() { return nbCcu; }
    int getCheckBoxGroup() { return checkBoxGroupValue; }
    QValueVector<QString> getCCUTable() { return ccuTable; }
    QListBox * getCCUList() {
	QListBox *ret = new QListBox();
	QStringList l;
	
	if(!ccuTable.empty()) {
	    for(QValueVector<QString>::iterator it = ccuTable.begin() ; it < ccuTable.end() ; it = it+4)
		l << *(it+1);
	
	    ret->insertStringList(l);
	}
	
	return ret;
    }
    ModuleInfoPage * getModuleInfoPage(QString key) { 
	if(moduleInfoPages.contains(key)) return *(moduleInfoPages.find(key));
	else return NULL;
    }
    bool sameModuleConfig() { return moduleConfig; }
};

#endif
