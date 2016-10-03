#include "CalibFormats/SiPixelObjects/interface/PixelCalibConfiguration.h"
//#include "CalibFormats/SiPixelObjects/interface/PixelDACNames.h"
#include "CalibFormats/SiPixelObjects/interface/PixelPortCardSettingNames.h"
#include "CalibFormats/SiPixelObjects/interface/PixelPortcardMap.h"
#include "PixelCalibrations/include/PixelFEDPOHBiasCalibration.h"
#include "PixelCalibrations/include/PixelPOHBiasCalibration.h"
#include "PixelCalibrations/include/PixelCalibrationBase.h"
#include "PixelConfigDBInterface/include/PixelConfigInterface.h"
#include "PixelUtilities/PixelFEDDataTools/include/PixelFEDDataTypes.h"
#include "PixelUtilities/PixelFEDDataTools/include/ErrorFIFODecoder.h"
#include "PixelUtilities/PixelFEDDataTools/include/ColRowAddrDecoder.h"
#include "PixelUtilities/PixelFEDDataTools/include/DigScopeDecoder.h"
#include "PixelUtilities/PixelFEDDataTools/include/DigTransDecoder.h"
#include "PixelUtilities/PixelFEDDataTools/include/FIFO3Decoder.h"
#include "PixelUtilities/PixelRootUtilities/include/PixelRootDirectoryMaker.h"
#include "PixelUtilities/PixelFEDDataTools/include/DigFIFO1Decoder.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include <iomanip>
#include <algorithm>

using namespace pos;

///////////////////////////////////////////////////////////////////////////////////////////////
PixelFEDPOHBiasCalibration::PixelFEDPOHBiasCalibration(const PixelFEDSupervisorConfiguration & tempConfiguration, SOAPCommander* mySOAPCmdr)
  : PixelFEDCalibrationBase(tempConfiguration,*mySOAPCmdr), rootf(0)
{
  std::cout << "In PixelFEDPOHBiasCalibration copy ctor()" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::initializeFED() {
  setFEDModeAndControlRegister(0x8, 0x30010);
  printIfSlinkHeaderMessedup_off();
  sendResets();
  
}

///////////////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference PixelFEDPOHBiasCalibration::beginCalibration(xoap::MessageReference msg) {
  std::cout << "In PixelFEDPOHBiasCalibration::beginCalibration()" << std::endl;

  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  tempCalibObject->writeASCII(outputDir());
  DumpFIFOs = tempCalibObject->parameterValue("DumpFIFOs") == "yes";

  std::cout << "[DEBUG] OutputDir = " << outputDir() << ", dumpFIFOs = " << DumpFIFOs << std::endl;

  setFIFO1Mode();//jen

  BookEm("");

  xoap::MessageReference reply = MakeSOAPMessageReference("BeginCalibrationDone");
  return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference PixelFEDPOHBiasCalibration::execute(xoap::MessageReference msg) {
  Attribute_Vector parameters(3);
  parameters[0].name_ = "WhatToDo";
  parameters[1].name_ = "AOHBias";
  parameters[2].name_ = "AOHGain";
  Receive(msg, parameters);

  const unsigned AOHBias = atoi(parameters[1].value_.c_str());
  const unsigned AOHGain = atoi(parameters[2].value_.c_str());

  if (parameters[0].value_ == "RetrieveData")
    RetrieveData(AOHBias, AOHGain);
  else if (parameters[0].value_ == "Analyze")
    Analyze();
  else {
    cout << "ERROR: PixelFEDPOHBiasCalibration::execute() does not understand the WhatToDo command, "<< parameters[0].value_ <<", sent to it.\n";
    assert(0);
  }

  xoap::MessageReference reply = MakeSOAPMessageReference("FEDCalibrationsDone");
  return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////
xoap::MessageReference PixelFEDPOHBiasCalibration::endCalibration(xoap::MessageReference msg) {
  
  std::cout << "In PixelFEDPOHBiasCalibration::endCalibration()" << std::endl;
  xoap::MessageReference reply = MakeSOAPMessageReference("EndCalibrationDone");
  return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::RetrieveData(unsigned AOHBias, unsigned AOHGain) {

  std::cout << "Enter retrieve data @ event = " << event_ << std::endl;

  PixelCalibConfiguration* tempCalibObject = dynamic_cast <PixelCalibConfiguration*> (theCalibObject_);
  assert(tempCalibObject!=0);

  const std::vector<std::pair<unsigned, std::vector<unsigned> > >& fedsAndChannels = tempCalibObject->fedCardsAndChannels(crate_, theNameTranslation_, theFEDConfiguration_, theDetectorConfiguration_);

  for (unsigned ifed = 0; ifed < fedsAndChannels.size(); ++ifed) {

    const unsigned fednumber = fedsAndChannels[ifed].first;
    const unsigned long vmeBaseAddress = theFEDConfiguration_->VMEBaseAddressFromFEDNumber(fednumber);
    PixelFEDInterface* iFED = FEDInterface_[vmeBaseAddress];
    const int MaxChans = 37;    
    uint32_t bufferFifo1[MaxChans][1024];
    int statusFifo1[MaxChans] = {0};

    std::cout << "[DEBUG] ifed = " << ifed << std::endl;
    std::cout << "[DEBUG] fednumber = " << fednumber << std::endl;
    std::cout << "[DEBUG] vmeBaseAddress = " << vmeBaseAddress << std::endl;

    for( unsigned int ch = 0; ch < fedsAndChannels[ifed].second.size(); ch++ ){
      std::cout << "[DEBUG ] channel = " << fedsAndChannels[ifed].second[ch] << std::endl;
      statusFifo1[ch] = iFED->drainFifo1(fedsAndChannels[ifed].second[ch], bufferFifo1[ch], 1024);
      std::cout << "[DEBUG] statusFifo1 = "  << statusFifo1[ch] << std::endl;
    }
     
    for( unsigned int ch = 0; ch < fedsAndChannels[ifed].second.size(); ch++ ){

      int channel = (fedsAndChannels[ifed].second)[ch];
      bool found_TBMA = false;

      if (statusFifo1[ch] > 0) {

	std::cout << "[DEBUG] inside Fifo1 = " << channel <<std::endl;
        DigFIFO1Decoder theFIFO1Decoder(bufferFifo1[ch],statusFifo1[ch]);
	std::cout << "[DEBUG] after Fifo1 definition globalChannel = " << (int)theFIFO1Decoder.globalChannel() << " " << channel <<std::endl;

        if( (int)theFIFO1Decoder.globalChannel() == channel ){
	  found_TBMA = theFIFO1Decoder.foundTBM();
	
	  if( DumpFIFOs ){
	    std::cout << "-----------------------------------" << std::endl;
	    std::cout << "Contents of FIFO 1 for channel " << channel << " (status = " << statusFifo1[ch] << ")" << std::endl;
	    std::cout << "-----------------------------------" << std::endl;
	    theFIFO1Decoder.printToStream(std::cout);
	  }
	}
	
      }

      std::cout << "[DEBUG] found_TBMA = " << found_TBMA << std::endl;
      FillEm(AOHBias, AOHGain, fednumber, channel, found_TBMA);

    }// end loop on channels
      
      
      
  }//end loop on feds
  
  event_++;
  sendResets();
  
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::Analyze() {

  std::cout << "[DEBUG] entering Analyze function @ event_ = " << event_ << std::endl;
//  int ntriggers = event_-1;
//  if (dacsToScan.size() == 0){
//
//    for( std::map<int,std::map<int,std::vector<TH1F*> > >::iterator it1 = ntrigsTBM.begin(); it1 != ntrigsTBM.end(); ++it1 ){
//     for( std::map<int,std::vector<TH1F*> >::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2 ){
//      for(unsigned int i = 0; i < it2->second.size(); ++i ) it2->second[i]->Scale(1./ntriggers);
//     }
//    }
//    CloseRootf();
//    return;
//  }
//
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  const std::vector<std::pair<unsigned, std::vector<unsigned> > >& fedsAndChannels = tempCalibObject->fedCardsAndChannels(crate_, theNameTranslation_, theFEDConfiguration_, theDetectorConfiguration_);



  for (unsigned ifed = 0; ifed < fedsAndChannels.size(); ++ifed) {

    const unsigned fednumber = fedsAndChannels[ifed].first;

    for( unsigned int ch = 0; ch < fedsAndChannels[ifed].second.size(); ch++ ){

      int channel = (fedsAndChannels[ifed].second)[ch];
      
      std::cout << "[DEBUG] " << fednumber << " " << channel << std::endl;
    }
  }


  CloseRootf();

}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::CloseRootf() {
  if (rootf) {
    rootf->Write();
    rootf->Close();
    delete rootf;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::BookEm(const TString& path) {

//  ntrigsTBM.clear();
//  scansTBM.clear();
//  TBMsHistoSum.clear();
//
  TString root_fn;
  if (path == "")
    root_fn.Form("%s/POHBias.root", outputDir().c_str());
  else
    root_fn.Form("%s/POHBias_%s.root", outputDir().c_str(), path.Data());
  cout << "writing histograms to file " << root_fn << endl;
  CloseRootf();
  rootf = new TFile(root_fn, "create");
  assert(rootf->IsOpen());

  tree = new TTree("tree", "tree");

  tree->Branch("channel",&b_channel,"channel/I");
  tree->Branch("fednumber",&b_fednumber,"fednumber/I");
  tree->Branch("isTBM",&b_isTBM,"isTBM/I");
  tree->Branch("AOHBias",&b_AOHBias,"AOHBias/I");
  tree->Branch("AOHGain",&b_AOHGain,"AOHGain/I");
    

  

//
//  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
//  assert(tempCalibObject != 0);
//  const std::vector<std::pair<unsigned, std::vector<unsigned> > >& fedsAndChannels = tempCalibObject->fedCardsAndChannels(crate_, theNameTranslation_, theFEDConfiguration_, theDetectorConfiguration_);
//
//  PixelRootDirectoryMaker rootDirs(fedsAndChannels,gDirectory);
//
//  for (unsigned ifed = 0; ifed < fedsAndChannels.size(); ++ifed) {
//
//   std::map<int,std::vector<TH1F*> > chTBMmap;
//   std::map<int,std::vector<TH2F*> > chTBMmap2D;
//   for( unsigned int ch = 0; ch < fedsAndChannels[ifed].second.size(); ch++ ){
//
//    rootDirs.cdDirectory(fedsAndChannels[ifed].first, (fedsAndChannels[ifed].second)[ch]);
//
//    if (dacsToScan.size() == 0){
//
//     TString hname; hname.Form("Ch%i",(fedsAndChannels[ifed].second)[ch]);
//     std::vector<TH1F*> histosTBM;
//     TH1F* h_TBM_nDecodes = new TH1F(hname+"_TBM_nDecodes", hname+"_TBM_nDecodes", 2, 0, 2 );
//     histosTBM.push_back(h_TBM_nDecodes);
//     TH1F* h_nROCHeaders = new TH1F(hname+"_nROCHeaders", hname+"_nROCHeaders", 9, 0, 9 );
//     histosTBM.push_back(h_nROCHeaders);  
//     chTBMmap[(fedsAndChannels[ifed].second)[ch]] = histosTBM;
//  
//    }// end book histos for zero dacsToScan case
//
//    if(dacsToScan.size() == 1){
//
//      TString hname; hname.Form("Ch%i",(fedsAndChannels[ifed].second)[ch]);
//      std::vector<TH2F*> histosTBM;
//      TH2F* h_TBM_nDecodes = new TH2F(hname+"_TBM_nDecodes", hname+"_TBM_nDecodes", 8, 0, 8, 8, 0, 8 );
//      //h_TBM_nDecodes->SetXTitle("400 MHz phase");
//      //h_TBM_nDecodes->SetYTitle("160 MHz phase");
//      histosTBM.push_back(h_TBM_nDecodes);
//      TH2F* h_nROCHeaders = new TH2F(hname+"_nROCHeaders", hname+"_nROCHeaders", 8, 0, 8, 8, 0, 8 );
//      //h_nROCHeaders->SetXTitle("400 MHz phase");
//      //h_nROCHeaders->SetYTitle("160 MHz phase");
//      histosTBM.push_back(h_nROCHeaders);     
//      chTBMmap2D[(fedsAndChannels[ifed].second)[ch]] = histosTBM;
//  
//    }// end book histos for 1 dacsToScan case (TBMPLL scan)
//
//   }//close loop on channels
//
//   if (dacsToScan.size() == 0) ntrigsTBM[fedsAndChannels[ifed].first] = chTBMmap;
//   if (dacsToScan.size() == 1) scansTBM[fedsAndChannels[ifed].first] = chTBMmap2D;
//
//  }//close loop on feds
//
//  rootf->cd();
//
//  //book histos with sum of channels
//  if (dacsToScan.size() == 1){
//
//   std::set<PixelChannel> names = theNameTranslation_->getChannels(); 
//   PixelRootDirectoryMaker rootDirsModules(names,gDirectory);
//
//   for(std::set<PixelChannel>::iterator PixelChannel_itr=names.begin(), PixelChannel_itr_end = names.end();PixelChannel_itr!=PixelChannel_itr_end; ++PixelChannel_itr){
//
//    rootDirsModules.cdDirectory((*PixelChannel_itr));
//    std::string moduleName = (*PixelChannel_itr).modulename();
//    std::vector<TH2F*> histosTBM;
//    TString hname(moduleName);
//    TH2F* h_TBM_nDecodes = new TH2F(hname+"_nTBMDecodes", hname+"_nTBMDecodes", 8, 0, 8, 8, 0, 8 );
//    //h_TBM_nDecodes->SetXTitle("400 MHz phase");
//    //h_TBM_nDecodes->SetYTitle("160 MHz phase");
//    histosTBM.push_back(h_TBM_nDecodes);
//    TH2F* h_nROCHeaders = new TH2F(hname+"_nROCHeaders", hname+"_nROCHeaders", 8, 0, 8, 8, 0, 8 );
//    //h_nROCHeaders->SetXTitle("400 MHz phase");
//    //h_nROCHeaders->SetYTitle("160 MHz phase");
//    histosTBM.push_back(h_nROCHeaders);   
//    TBMsHistoSum[moduleName] = histosTBM;
//    
//   }
//  }//end booking sum histo
//
//  rootf->cd(0);

}

///////////////////////////////////////////////////////////////////////////////////////////////
void PixelFEDPOHBiasCalibration::FillEm(unsigned AOHBias, unsigned AOHGain, int fedid, int ch, int which) {
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  std::cout << "[DEBUG] FilEm = " << " "<< AOHBias << " " << fedid << " " << ch << " " << which << std::endl;
  
  //  if (event_==0) return;

  b_channel = ch;
  b_fednumber = fedid;
  b_isTBM = which;
  b_AOHBias = AOHBias;
  b_AOHGain = AOHGain;
  tree->Fill();

}
