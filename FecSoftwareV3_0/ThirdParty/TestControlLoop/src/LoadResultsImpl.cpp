
#include "LoadResultsImpl.h"

#include <qlistbox.h>
#include <qdir.h>
#include <qlineedit.h>

LoadResultsImpl::LoadResultsImpl(TestConfigurationImpl *p, GUISupervisor *guiSupervisor) : LoadResults(p, "LoadResults", true) {   
    supervisor = guiSupervisor;
    parent_ = p;
    
    QDir d = QDir(supervisor->getResDir());
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

void LoadResultsImpl::fileNameSelected() {
    QString name = fileListBox->currentText();
    name = name.left(name.length()-4);
    fileNameLineEdit->setText(name);
}

void LoadResultsImpl::back() {
    close();
}

void LoadResultsImpl::loadResults() {
  supervisor->uploadResults(fileNameLineEdit->text());
  close();
}


