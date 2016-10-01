#include "CalibFormats/SiPixelObjects/interface/PixelCalibConfiguration.h"
//#include "CalibFormats/SiPixelObjects/interface/PixelDACNames.h"
#include "CalibFormats/SiPixelObjects/interface/PixelPortCardSettingNames.h"
#include "PixelCalibrations/include/PixelPOHBiasCalibration.h"
#include "PixelCalibrations/include/PixelPOHBiasCalibrationParameters.h"

#include <toolbox/convertstring.h>

using namespace pos;
using namespace std;
using namespace PixelPOHBiasCalibrationParameters;
PixelPOHBiasCalibration::PixelPOHBiasCalibration(const PixelSupervisorConfiguration & tempConfiguration, SOAPCommander* mySOAPCmdr)
  : PixelCalibrationBase(tempConfiguration, *mySOAPCmdr)
{
  std::cout << "Greetings from the PixelPOHBiasCalibration copy constructor." << std::endl;
}

void PixelPOHBiasCalibration::beginCalibration() {
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  // Check that PixelCalibConfiguration settings make sense.
	
  
}

bool PixelPOHBiasCalibration::execute() {
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  const bool firstOfPattern = event_ % tempCalibObject->nTriggersPerPattern() == 0;
  const unsigned state = event_/(tempCalibObject->nTriggersPerPattern());
  reportProgress(0.05);



  // Configure all TBMs and ROCs according to the PixelCalibConfiguration settings, but only when it's time for a new configuration.
  if (firstOfPattern) {
    commandToAllFECCrates("CalibRunning");
  }

  // should take this out
  commandToAllFEDCrates("JMTJunk");

  // Send trigger to all TBMs and ROCs.
  //  sendTTCCalSync();

  // Read out data from each FED.

  std::cout << "[DEBUG] status = " << state << std::endl;
  std::cout << "[DEBUG] nTriggersPerPattern = " << tempCalibObject->nTriggersPerPattern() << std::endl;
  std::cout << "[DEBUG] nTriggersTotal = " << tempCalibObject->nTriggersTotal() << std::endl;
  std::cout << "[DEBUG] event_ = " << event_ << std::endl;

  // From here, scan the AOH Bias 

  //  PixelCalibConfiguration* tempCalibObject = dynamic_cast <PixelCalibConfiguration*> (theCalibObject_);
  //  assert(tempCalibObject!=0);
  
  unsigned int AOHBiasMin = k_ScanMin_default;
  if ( tempCalibObject->parameterValue("ScanMin") != "" ) AOHBiasMin = atoi(tempCalibObject->parameterValue("ScanMin").c_str());
  unsigned int AOHBiasMax = k_ScanMax_default;;
  if ( tempCalibObject->parameterValue("ScanMax") != "" ) AOHBiasMax = atoi(tempCalibObject->parameterValue("ScanMax").c_str());
  unsigned int AOHBiasStepSize = k_ScanStepSize_default;
  if ( tempCalibObject->parameterValue("ScanStepSize") != "" ) AOHBiasStepSize = atoi(tempCalibObject->parameterValue("ScanStepSize").c_str());

  unsigned int nTriggersPerPOHBias = k_nTriggersPerPOHBias_default;
  if ( tempCalibObject->parameterValue("nTriggersPerPOHBias") != "" ) nTriggersPerPOHBias = atoi(tempCalibObject->parameterValue("nTriggersPerPOHBias").c_str());

  std::cout << "[DEBUG] AOHBiasMin = " << AOHBiasMin << std::endl;
  std::cout << "[DEBUG] AOHBiasMax = " << AOHBiasMax << std::endl;
  std::cout << "[DEBUG] AOHBiasStepSize = " << AOHBiasStepSize << std::endl;
  std::cout << "[DEBUG] nTriggersPerPOHBias = " << nTriggersPerPOHBias << std::endl;

  
  const std::set<unsigned int> fedcrates=tempCalibObject->getFEDCrates(theNameTranslation_, theFEDConfiguration_);
  const std::set<unsigned int> TKFECcrates=tempCalibObject->getTKFECCrates(thePortcardMap_, *getmapNamePortCard(), theTKFECConfiguration_);
  
  for (unsigned int AOHBias = AOHBiasMin; AOHBias <= AOHBiasMax; AOHBias += AOHBiasStepSize){
      
    // Set AOH bias
    Attribute_Vector parametersToTKFEC(1);
    parametersToTKFEC[0].name_="AOHBias"; parametersToTKFEC[0].value_=itoa(AOHBias);
    commandToAllTKFECCrates("SetAOHBiasEnMass", parametersToTKFEC);

    std::cout << "[DEBUG] POH bias = " << AOHBias << std::endl;
      
    //    unsigned int Ntriggers=tempCalibObject->nTriggersPerPattern();
    //    std::cout << "[DEBUG] nTriggersPerPattern = " << Ntriggers << std::endl;

    for (unsigned int i_event=0; i_event < nTriggersPerPOHBias; ++i_event){

      //      Send trigger to all TBMs and ROCs.
      sendTTCCalSync();

      Attribute_Vector parametersToFED(2);
      parametersToFED[0].name_ = "WhatToDo"; 
      parametersToFED[1].name_ = "StateNum";
      parametersToFED[2].name_ = "AOHBias"; 
      parametersToFED[0].value_ = "RetrieveData";
      parametersToFED[1].value_ = itoa(state);
      parametersToFED[2].value_ = itoa(AOHBias);
      commandToAllFEDCrates("FEDCalibrations", parametersToFED);
    }
      
  } // end of loop over AOHBias values

  

  return event_ + 1 < tempCalibObject->nTriggersTotal();
}

void PixelPOHBiasCalibration::endCalibration() {
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);
  assert(event_ == tempCalibObject->nTriggersTotal());
	
  Attribute_Vector parametersToFED(2);
  parametersToFED[0].name_ = "WhatToDo"; parametersToFED[0].value_ = "Analyze";
  parametersToFED[1].name_ = "StateNum"; parametersToFED[1].value_ = "0";
  commandToAllFEDCrates("FEDCalibrations", parametersToFED);

}

std::vector<std::string> PixelPOHBiasCalibration::calibrated() {
  std::vector<std::string> tmp;
  return tmp;
}



//Attribute_Vector PixelPOHBiasCalibration::SetupParametersToFED() const
//{
//	Attribute_Vector parametersToFED(k_NumVars);
//	parametersToFED[k_WhatToDo].name_="WhatToDo";
//
//	parametersToFED[k_POHBias].name_="POHBias";                         
//	parametersToFED[k_POHBias].value_="unset";
//
////	parametersToFED[k_FEDNumber].name_="FEDNumber"; 
////	parametersToFED[k_FEDNumber].value_="unset";
////
////	parametersToFED[k_FEDChannel].name_="FEDChannel"; 
////	parametersToFED[k_FEDChannel].value_="unset";
//	
//	return parametersToFED;
//}



//void PixelPOHBiasCalibration::POHBiasLoop()
//{
//}

//void PixelPOHBiasCalibration::triggeringLoop(Attribute_Vector parametersToFED, std::set<unsigned int> fedcrates, unsigned int Ntriggers){
//  for (unsigned int i_event=0;i_event<Ntriggers;++i_event){
//
//    // Send trigger to all TBMs and ROCs.
//    sendTTCCalSync();
//    
//    // Read out data from each FED.
//    // This is not replaced with commandToAllFEDCrates("AOHBias", parametersToFED) because we may not be doing all FED crates.
//    for (std::set<unsigned int>::iterator ifedcrate=fedcrates.begin();ifedcrate!=fedcrates.end();++ifedcrate){
//      if (Send(PixelFEDSupervisors_[(*ifedcrate)], "FEDCalibrations", parametersToFED)!="AOHBiasDone"){
//	diagService_->reportError("AOHBias in FED crate # " + stringF((*ifedcrate)) + " could not be done!",DIAGWARN);
//      }
//    }
//  }
//}
