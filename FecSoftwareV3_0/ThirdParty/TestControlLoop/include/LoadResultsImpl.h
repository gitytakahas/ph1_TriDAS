#ifndef LOADRESULTSIMPL_H
#define LOADRESULSTIMPL_H

#include "loadresults.h"
#include "GUISupervisor.h"
#include "TestConfigurationImpl.h"

class LoadResultsImpl : public LoadResults
{
    private:
    GUISupervisor *supervisor;
    TestConfigurationImpl *parent_;
    
    public:
    LoadResultsImpl(TestConfigurationImpl *p, GUISupervisor *guiSupervisor);
    
    void fileNameSelected();
    void back();
    void loadResults();
};
#endif
