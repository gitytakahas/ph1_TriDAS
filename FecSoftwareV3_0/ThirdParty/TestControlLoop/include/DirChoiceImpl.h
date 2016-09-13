#ifndef DIRCHOICEIMPL_H
#define DIRCHOICEIMPL_H

#include "dirchoice.h"
#include "GUISupervisor.h"

#include <qstring.h>

class DirChoiceImpl : public DirChoice {
    private:
    GUISupervisor *supervisor;
        
    public:
    DirChoiceImpl(QWidget *parent, GUISupervisor *guiSupervisor, QString title);
    
    void dirSelected();
};

#endif
