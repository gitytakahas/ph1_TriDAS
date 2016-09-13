#ifndef _PixelFEDReadbackCalibration_h_
#define _PixelFEDReadbackCalibration_h_

#include "CalibFormats/SiPixelObjects/interface/PixelROCName.h"
#include "PixelCalibrations/include/PixelFEDCalibrationBase.h"
#include "PixelUtilities/PixelFEDDataTools/include/Moments.h"
#include "PixelUtilities/PixelFEDDataTools/include/PixelScanRecord.h"

#include <cstdint>
#include <fstream>

class TFile;
class TH1F;
class TH2F;
class TH3F;

class PixelFEDReadbackCalibration: public PixelFEDCalibrationBase {
 public:
  PixelFEDReadbackCalibration(const PixelFEDSupervisorConfiguration&, SOAPCommander*);

  virtual void initializeFED();
  virtual xoap::MessageReference beginCalibration(xoap::MessageReference msg);
  virtual xoap::MessageReference execute(xoap::MessageReference msg);
  virtual xoap::MessageReference endCalibration(xoap::MessageReference msg);

 private:
  void RetrieveData(unsigned int state);
  void Analyze();
  void CloseRootf();
  void BookEm(const TString& path);
  void ReadbackROCs(void);
   
  std::map<pos::PixelModuleName,pos::PixelDACSettings*> dacsettings_;  
  TFile* rootf;
  std::vector<std::string> dacsToScan;  
  unsigned lastDelayValue;
  bool isTBMPLLdelayScan;
  std::map<int,std::map<int,std::map<int,std::vector<int> > > > last_dacs;
  std::map<int,std::map<int,std::map<int,std::map<int,int> > > > values;
  std::map<int,std::map<int,std::vector<TH2F*> > > scansROCs;
  std::map<std::string,std::vector<TH2F*> > ROCsHistoSum;

   struct branch{
    float pass;
    char moduleName[38];
  };

};

#endif
