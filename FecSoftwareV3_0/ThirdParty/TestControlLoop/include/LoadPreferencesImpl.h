#ifndef LOADPREFERENCESIMPL_H
#define LOADPREFERENCESIMPL_H

#include "loadpreferences.h"
#include "GUISupervisor.h"
#include "TestConfigurationImpl.h"

class LoadPreferencesImpl : public LoadPreferences
{
    private:
    GUISupervisor *supervisor;
    TestConfigurationImpl *parent_;
    
    public:
    LoadPreferencesImpl(TestConfigurationImpl *p, GUISupervisor *guiSupervisor);
    
    void fileNameSelected();
    void back();
    void loadPreferences();
    void readXml(QString fileName);
};
#endif
