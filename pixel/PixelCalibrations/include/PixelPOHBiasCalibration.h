#ifndef _PixelPOHBiasCalibration_h_
#define _PixelPOHBiasCalibration_h_

#include "PixelCalibrations/include/PixelCalibrationBase.h"

#include "toolbox/exception/Handler.h"
#include "toolbox/Event.h"

#include "CalibFormats/SiPixelObjects/interface/PixelChannel.h"
#include "PixelUtilities/PixelFEDDataTools/include/Moments.h"

class TFile;
class TTree;
class TH1F;
class TH2F;
class TString;
class TInt;

#include <cstdint>
#include <fstream>


class PixelPOHBiasCalibration : public PixelCalibrationBase {
 public:
  PixelPOHBiasCalibration(const PixelSupervisorConfiguration&, SOAPCommander*);

  void beginCalibration();
  virtual bool execute();
  void endCalibration();
  virtual std::vector<std::string> calibrated();
  //  bool ToggleChannels;
//  bool CycleScopeChannels;
  //  bool DelayBeforeFirstTrigger;
  //  bool DelayEveryTrigger;
  //  Attribute_Vector SetupParametersToFED() const; 

  //  std::map<std::string, std::map<unsigned, unsigned>> bias_values_by_portcard_and_aoh_new;

  void Analyze();
  void CloseRootf();
  void BookEm(const TString& path);
  //void BookEm(string path);
  
  TFile* rootf;
  TTree* tree;
  
  int b_channel;
  int b_fednumber;
  int b_AOHGain;
  int b_isPass;
  int b_isStable;
  int b_isFirstPlateau;

  //  std::map<std::string, std::map<unsigned, unsigned>> bias_values_by_portcard_and_aoh_new;
};

#endif
