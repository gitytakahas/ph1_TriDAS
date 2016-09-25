#ifndef _PixelPOHBiasCalibration_h_
#define _PixelPOHBiasCalibration_h_

#include "PixelCalibrations/include/PixelCalibrationBase.h"

#include "toolbox/exception/Handler.h"
#include "toolbox/Event.h"

#include "CalibFormats/SiPixelObjects/interface/PixelChannel.h"
#include "PixelUtilities/PixelFEDDataTools/include/Moments.h"


class PixelPOHBiasCalibration : public PixelCalibrationBase {
 public:
  PixelPOHBiasCalibration(const PixelSupervisorConfiguration&, SOAPCommander*);

  void beginCalibration();
  virtual bool execute();
  void endCalibration();
  virtual std::vector<std::string> calibrated();
  void POHBiasLoop();

  bool ToggleChannels;
  bool CycleScopeChannels;
  bool DelayBeforeFirstTrigger;
  bool DelayEveryTrigger;
};

#endif
