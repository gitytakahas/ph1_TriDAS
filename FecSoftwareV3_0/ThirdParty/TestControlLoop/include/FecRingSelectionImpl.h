#ifndef FECRINGSELECTIONIMPL_H
#define FECRINGSELECTIONIMPL_H

#include "fecringselection.h"

#include "GUISupervisor.h"

#include <qlayout.h>
#include <qwidget.h>
#include <qvaluevector.h>
#include <qcheckbox.h>

class FecRingSelectionImpl : public FecRingSelection {
               
    private:
    GUISupervisor *supervisor;
    
    QGridLayout *chBLayout;
    QGridLayout *mainLayout;
    QHBoxLayout *okButtonLayout;
    QPushButton *okButton;
    QValueVector<QCheckBox *> existingCheckBox;
    
    void addNewCheckBox(int fecSlot, int ringSlot, int selected, int row, int column);
        
    public:
    FecRingSelectionImpl(QWidget *parent, GUISupervisor *guiSupervisor);
    
    void fecRingSelected();
};

#endif
