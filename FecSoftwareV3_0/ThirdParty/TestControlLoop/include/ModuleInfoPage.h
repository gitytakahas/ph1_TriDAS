#ifndef MODULEINFOPAGE_H
#define MODULEINFOPAGE_H

#include <qlistbox.h>
#include <qstringlist.h>

class ModuleInfoPage {
    private:
    int nbModule;
    int checkBoxGroupValue;
    QValueVector<QString> moduleTable;
    
    public:
    ModuleInfoPage() { 
	nbModule = 0; 
	checkBoxGroupValue = 0;
    }
        
    void setNbModule(int nbM) { nbModule = nbM; }
    void setCheckBoxGroup(int v) { checkBoxGroupValue = v; }
    void setModuleTable(QValueVector<QString> v) { moduleTable = v; }
    
    int getNbModule() { return nbModule; }
    int getCheckBoxGroup() { return checkBoxGroupValue; }
    QValueVector<QString> getModuleTable() { return moduleTable; }
    QListBox * getModuleList() {
	QListBox *ret = new QListBox();
	QStringList l;
	
	if(moduleTable.empty()) return NULL;
	else {
	    for(QValueVector<QString>::iterator it = moduleTable.begin() ; it < moduleTable.end() ; it = it+3)
		l << *it;
	
	    ret->insertStringList(l);
	    
	    return ret;
	}
    }
};

#endif
