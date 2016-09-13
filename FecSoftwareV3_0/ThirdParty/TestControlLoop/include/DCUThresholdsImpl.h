#ifndef DCUTHRESHOLDSIMPL_H
#define DCUTHRESHOLDSIMPL_H

#include "dcuthresholds.h"
#include "GUISupervisor.h"

class DCUThresholdsImpl : public DCUThresholds {
    private:
    GUISupervisor *supervisor;
    
    public:
    DCUThresholdsImpl(QWidget *parent, int type, GUISupervisor *guiSupervisor);
    
    void clearPage();
    void saveThr();
};

#endif
