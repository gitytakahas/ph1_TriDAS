#ifndef MODULEINFOIMPL_H
#define MODULEINFOIMPL_H

#include "moduleinfo.h"
#include "GUISupervisor.h"

class ModuleInfoImpl : public ModuleInfo
{
    private:
    GUISupervisor *supervisor;
    CCUInfoPage *currentCCUInfoPage;
    QString currentFecRing;
    QString currentCCU;
    bool backClose; // for closeEvent management
    
    public:
    ModuleInfoImpl(QWidget *parent, GUISupervisor *guiSupervisor);
    void closeEvent(QCloseEvent *e);
    
    void clearPage();
    void back();
    void addModule();
    void deleteModule();
    void apv4();
    void apv6();
    void setDcuThreshold();
    void changeCCU();
    void changeFecRing();
    void warning();
    void clearModuleTable();
    void addTabEntry(QString i2cAddress, QString nbAPV, QString dcuHardId);
    int getSelectedCheckBox();
    void updateAPVNumber();
    QValueVector<QString> getModuleTable();
    void savePage(QString fecRingKey, QString ccuKey);
    void sameConfigForAll();
};
#endif
