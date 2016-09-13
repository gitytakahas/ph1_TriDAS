#ifndef CCUINFOIMPL_H
#define CCUINFOIMPL_H

#include "ccuinfo.h"
#include "GUISupervisor.h"

class CCUInfoImpl : public CCUInfo
{
    private:
    GUISupervisor *supervisor;
    QString currentFecRing;
    bool backClose; // for closeEvent management
    
    public:    
    CCUInfoImpl(QWidget *parent, GUISupervisor *guiSupervisor);
    
    void closeEvent(QCloseEvent *e);
    
    void clearPage();
    void deleteCCU();
    void setDcuThreshold();
    void changePage();
    void addCCU();
    void back();
    void allHaveDcu();
    void allExceptHaveDcu();
    void noDcu();
    void warning();
    int getSelectedCheckBox();
    void updateDcuDistribution();
    QValueVector<QString> getCCUTable();
    void savePage(QString key);
    void sameConfigForAll();
    void addTabEntry(QString position, QString address, QString dcu, QString dcuHardId);
    void clearCCUTable();
};
#endif
