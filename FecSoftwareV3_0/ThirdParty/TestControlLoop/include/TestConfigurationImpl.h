#ifndef TESTCONFIGURATIONIMPL_H
#define TESTCONFIGURATIONIMPL_H

#include "testconfiguration.h"
#include "GUISupervisor.h"

class TestConfigurationImpl : public TestConfiguration
{
    private:
    GUISupervisor *supervisor;
    
    void launchPowerUpStep();
    void launchCCUInfo();
    void launchModuleInfo();
    void loadPref();
    void savePref();
    void fecSelection();
    void uploadResults();
        
    int getSelectedTest();
    
    public:
    TestConfigurationImpl(GUISupervisor *guiSupervisor, bool testPossible);
    
    void updateSelectedTestCheckBox();
    void prefDirChoice();
    void resDirChoice();    
    void configTestChanged();

    void updateSupervisor();
};
#endif

