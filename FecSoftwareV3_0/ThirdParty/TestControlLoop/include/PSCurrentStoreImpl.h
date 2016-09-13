#ifndef PSCURRENTSTOREIMPL_H
#define PSCURRENTSTOREIMPL_H

#include "PowerUpStepsImpl.h"
#include "TestResultsImpl.h"
#include "pscurrentstore.h"
#include "GUISupervisor.h"

#include <qevent.h>
#include <qlineedit.h>

class PSCurrentStoreImpl : public PSCurrentStore
{
    private:
    GUISupervisor *supervisor;
    TestResultsImpl *resultsParent;
    PowerUpStepsImpl *powerParent;    
    QGridLayout *dynamicLayout;
    QValueVector<QLineEdit *> vLineEdit;
    bool okButtonClose; // for closeEvent management
    
    void addRecordLine(int fecSlot, int ringSlot, int numLine);
    void init(GUISupervisor *guiSupervisor);
    
    public:
    void nextStep();
    void checkAllFields();
    void closeEvent(QCloseEvent *e);
    
    PSCurrentStoreImpl(TestResultsImpl *parnet, GUISupervisor *guiSupervisor);
    PSCurrentStoreImpl(PowerUpStepsImpl *parent, GUISupervisor *guiSupervisor);
};
#endif
