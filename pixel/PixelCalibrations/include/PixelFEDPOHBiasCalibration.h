#ifndef _PixelFEDPOHBiasCalibration_h_
#define _PixelFEDPOHBiasCalibration_h_

#include "toolbox/exception/Handler.h"
#include "toolbox/Event.h"

#include "PixelUtilities/PixelFEDDataTools/include/PixelMode.h"


#include "CalibFormats/SiPixelObjects/interface/PixelROCName.h"
#include "PixelCalibrations/include/PixelFEDCalibrationBase.h"
#include "PixelUtilities/PixelFEDDataTools/include/Moments.h"
#include "PixelUtilities/PixelFEDDataTools/include/PixelScanRecord.h"

#include <cstdint>
#include <fstream>

class TFile;
class TTree;
class TH1F;
//class TH2F;
//class TH3F;

class PixelFEDPOHBiasCalibration: public PixelFEDCalibrationBase {
 public:
  PixelFEDPOHBiasCalibration(const PixelFEDSupervisorConfiguration&, SOAPCommander*);

  virtual void initializeFED();
  virtual xoap::MessageReference beginCalibration(xoap::MessageReference msg);
  virtual xoap::MessageReference execute(xoap::MessageReference msg);
  virtual xoap::MessageReference endCalibration(xoap::MessageReference msg);

 private:
  void RetrieveData(unsigned int AOHBias, unsigned AOHGain);
  //void RetrieveData(unsigned int state);
  void Analyze();
  void CloseRootf();
  void BookEm(const TString& path);
  void FillEm(unsigned AOHBias, unsigned AOHGain, int fedid, int ch, int which);

  bool DumpFIFOs;
  //  std::vector<std::string> dacsToScan;

  TFile* rootf;
  TTree* tree;
  
  Int_t b_channel;
  Int_t b_fednumber;
  Int_t b_isTBM;
  Int_t b_AOHBias;
  Int_t b_AOHGain;



};

#endif
