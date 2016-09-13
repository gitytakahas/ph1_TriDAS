#ifndef TESTRESULTSIMPL_H
#define TESTRESULTSIMPL_H

#include "testresults.h"
#include "GUISupervisor.h"

#include <qtextstream.h>

class TestResultsImpl : public TestResults
{
    private:
    GUISupervisor *supervisor;
    FecAccess *fecAccess;
    
    QValueVector<int> fecRingSelected;
    QValueVector<keyType> okKeyTypes;
    
    QPixmap redLed;
    QPixmap greenLed;
    QPixmap orangeLed;
    
    QListViewItem *currentFecRingItem;
    QListViewItem *currentCcuItem;
    keyType currentDummyIndex;
    QListViewItem *currentModuleItem;
    QListViewItem *currentDeviceItem;
    std::list<keyType> *moduleList;
    std::list<keyType> *dcuOnCcuList;
    std::list<keyType> *dohOnCcuList;
    
    bool anErrorOccured; // to know if ps currents have to be asked at the end
    
    public:
    TestResultsImpl(std::string fileName);
    TestResultsImpl(GUISupervisor *guiSupervisor, FecAccess *fecAccess_);
    
    void closeEvent(QCloseEvent *e);
    void finish();
    
    void doTest();
    
    void fecTest(keyType index);
    void ccuTest(keyType ccuIndex);
    void redundancyTest(keyType index, int dcuTest);
    void modulesDevicesTest(keyType index);
    void dohCalibration();
    void specialI2CTest();
    void configurationTest(keyType channelIndex, int typeOfDevice);    
    
    void testDummyCcuRegisters(keyType index);
    
    void piaResetOnCcu(std::list<keyType> *ccuList);
    
    void checkDcuModuleHardId(keyType channelIndex, QString dcuHardId);
    void checkDcuCcuHardId(keyType channelIndex, QString dcuHardId);
    int checkNbApv(keyType fecRingIndex, QString moduleAddress);
    void checkDcuOnCcu(keyType fecRingIndex, std::list<keyType> *dcuOnCcuList);
    int checkCcuAgreement(keyType fecRingIndex, std::list<keyType> *ccuList);    
    void checkModuleAgreement(keyType fecRingIndex, std::list<keyType> *moduleList);
        
    void createCcuItem(QString ccuAddress);
    void createModuleItem(QString moduleText, bool module = true);
    void createDeviceItem(QString deviceText);
    
    void setCurrentCcuItem(tscType16 ccuAddress, bool dummy = false);
    
    void setFecError(QString errMessage = "");
    void setRingError(QString errMessage = "");
    void setCcuError(QString errMessage = "");
    void setModuleError(QString errMessage = "");
    void setDeviceError(int deviceIndex, QString errMessage = "");
    void setFecRingInfo(QString infoMessage);
    void setModuleInfo(QString infoMessage);
    void setDeviceInfo(int deviceIndex, QString infoMessage);
    void setOnly4APV(bool undefined);
    
    void generateXml();
    void uploadXml(std::string fileName);

    QString doublon(QValueVector<QString> tab);
};
#endif
