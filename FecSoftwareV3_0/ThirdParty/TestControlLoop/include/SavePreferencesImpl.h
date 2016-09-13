#ifndef SAVEPREFERENCESIMPL_H
#define SAVEPREFERENCESIMPL_H

#include "savepreferences.h"
#include "GUISupervisor.h"

class SavePreferencesImpl : public SavePreferences
{
    private:
    GUISupervisor *supervisor;
    
    public:
    SavePreferencesImpl(QWidget *parent, GUISupervisor *guiSupervisor);
    
    void fileNameSelected();
    void back();
    void savePreferences();
    void generateXml(QString fileName);
};
#endif
