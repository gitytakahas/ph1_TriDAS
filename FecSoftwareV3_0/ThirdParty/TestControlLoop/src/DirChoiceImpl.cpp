#include "DirChoiceImpl.h"

#include <qlineedit.h>
#include <qdir.h>

DirChoiceImpl::DirChoiceImpl(QWidget *parent, GUISupervisor *guiSupervisor, QString title) : DirChoice(parent, "DirChoice", true) {
    supervisor = guiSupervisor;
    
    setCaption(title);

    dirChoiceLineEdit->setText("Enter the complete path (or nothing to reset to default)");
    dirChoiceLineEdit->selectAll();
}

void DirChoiceImpl::dirSelected() {
  QString path = dirChoiceLineEdit->text();

  if(path == "") {
    if(caption().startsWith("Pref"))
      supervisor->setPrefDir("Preferences/");
    else
      supervisor->setResDir("ResultsFiles/");

    close();
  }
  else {
    QDir d = QDir(path);
    if(d.exists()) {
      if(caption().startsWith("Pref"))
	supervisor->setPrefDir(dirChoiceLineEdit->text());
      else
	supervisor->setResDir(dirChoiceLineEdit->text());

      close();
    }
    else {
      dirChoiceLineEdit->setText("Dir doesn't exist !");
      dirChoiceLineEdit->selectAll();
    }
  }
}
