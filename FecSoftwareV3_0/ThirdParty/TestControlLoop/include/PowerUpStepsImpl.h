#ifndef POWERUPSTEPSIMPL_H
#define POWERUPSTEPSIMPL_H

#include "powerupsteps.h"
#include "GUISupervisor.h"

class PowerUpStepsImpl : public PowerUpSteps
{
    private:
    GUISupervisor *supervisor;
    int step;
    
    public:
    void nextStep();
    
    PowerUpStepsImpl(GUISupervisor *guiSupervisor) : PowerUpSteps() { 
	supervisor = guiSupervisor;
	step =1;
    }
};
#endif
