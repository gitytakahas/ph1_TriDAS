#ifndef _PixelReadbackCalibration_h_
#define _PixelReadbackCalibration_h_

#include "PixelCalibrations/include/PixelCalibrationBase.h"

class PixelReadbackCalibration : public PixelCalibrationBase {
 public:
  PixelReadbackCalibration(const PixelSupervisorConfiguration&, SOAPCommander*);

  void beginCalibration();
  virtual bool execute();
  void endCalibration();
  virtual std::vector<std::string> calibrated();

  /*bool ToggleChannels;
  bool CycleScopeChannels;
  bool DelayBeforeFirstTrigger;
  bool DelayEveryTrigger;*/
};

#endif
