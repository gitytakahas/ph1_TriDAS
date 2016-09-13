#ifndef GUISUPERVISOR_H
#define GUISUPERVISOR_H

#include <iostream>

using std::cout;
using std::endl;

#include <qvaluevector.h>
#include <qmap.h>
#include <qstring.h>

#include "CCUInfoPage.h"
#include "FecAccess.h"

#include "FecInterfaceRingDevice.h"

#define fecTestMask 		        0x1
#define ccuTestMask 		        0x10
#define redundancyTestMask  	        0x100
#define modulesDevicesTestMask 	        0x1000
#define dohCalibrationMask 	        0x10000
#define configurationTestMask 	        0x100000
#define psCurrentsRecordMask 	        0x1000000
#define i2cSpeedTestMask		0x10000000
#define i2cSpecialCommandTestMask	0x1

class GUISupervisor {
    private:
    QValueVector<int> fecRingSelected;
    QMap<QString, CCUInfoPage *> ccuInfoPage;
    QValueVector<QString> psCurrents;
    QStringList dcuOnCcuThr;
    QStringList dcuOnModuleThr;
    FecAccess *fecAccess;
    QValueVector<keyType> okKeyTypes;
    QString prefDir;    
    QString resDir;    
    int nbLoop_;

    public:
    static int testSelected;
    static int specialI2CTestSelected;

    GUISupervisor( FecAccess *fecAcc ) ;
    
    void endTestConfiguration();
    void powerIsUp();
    
    void updateFecRingSelected(QValueVector<int> selectedFecRing);
    QValueVector<int> getFecRingSelected();
    
    QValueVector<keyType> getOkKeyTypes();
    
    CCUInfoPage * getCCUInfoPage(QString key);
    void updateCCUInfoPage(QString key, CCUInfoPage *p);
    
    void recordPSCurrent(QString psCurrent);
    QValueVector<QString> getPSCurrent();
    
    void setDcuOnCcuThr(QStringList l);
    void setDcuOnModuleThr(QStringList l);
    
    QStringList getDcuOnCcuThr();
    QStringList getDcuOnModuleThr();
    
    void uploadResults(std::string fileName);

    void setPrefDir(QString dirPath);
    void setResDir(QString dirPath);
    QString getPrefDir();
    QString getResDir();    

    void setNbLoopOnModules(int nbLoop);
    int getNbLoopOnModules();
};
#endif
