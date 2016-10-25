#include "CalibFormats/SiPixelObjects/interface/PixelCalibConfiguration.h"
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
}


bool PixelPOHBiasCalibration::execute() {
  PixelCalibConfiguration* tempCalibObject = dynamic_cast<PixelCalibConfiguration*>(theCalibObject_);
  assert(tempCalibObject != 0);

  const bool firstOfPattern = event_ % tempCalibObject->nTriggersPerPattern() == 0;
  reportProgress(0.05);


  // Configure all TBMs and ROCs according to the PixelCalibConfiguration settings, but only when it's time for a new configuration.
  if (firstOfPattern) {
    commandToAllFECCrates("CalibRunning");
  }

  // should take this out
  commandToAllFEDCrates("JMTJunk");


  // Read out data from each FED.
  std::cout << "[DEBUG] nTriggersPerPattern = " << tempCalibObject->nTriggersPerPattern() << std::endl;
  std::cout << "[DEBUG] nTriggersTotal = " << tempCalibObject->nTriggersTotal() << std::endl;
  std::cout << "[DEBUG] event_ = " << event_ << std::endl;

  // Scan the AOH Bias 
  
  unsigned int AOHBiasMin = k_ScanMin_default;
  if ( tempCalibObject->parameterValue("ScanMin") != "" ) AOHBiasMin = atoi(tempCalibObject->parameterValue("ScanMin").c_str());

  unsigned int AOHBiasMax = k_ScanMax_default;;
  if ( tempCalibObject->parameterValue("ScanMax") != "" ) AOHBiasMax = atoi(tempCalibObject->parameterValue("ScanMax").c_str());

  unsigned int AOHBiasStepSize = k_ScanStepSize_default;
  if ( tempCalibObject->parameterValue("ScanStepSize") != "" ) AOHBiasStepSize = atoi(tempCalibObject->parameterValue("ScanStepSize").c_str());

  unsigned int AOHGainMin = k_GainMin_default;
  if ( tempCalibObject->parameterValue("GainMin") != "" ) AOHGainMin = atoi(tempCalibObject->parameterValue("GainMin").c_str());

  unsigned int AOHGainMax = k_GainMax_default;;
  if ( tempCalibObject->parameterValue("GainMax") != "" ) AOHGainMax = atoi(tempCalibObject->parameterValue("GainMax").c_str());

  unsigned int AOHGainStepSize = k_GainStepSize_default;
  if ( tempCalibObject->parameterValue("GainStepSize") != "" ) AOHGainStepSize = atoi(tempCalibObject->parameterValue("GainStepSize").c_str());

  unsigned int nTriggersPerPOHBias = k_nTriggersPerPOHBias_default;
  if ( tempCalibObject->parameterValue("nTriggersPerPOHBias") != "" ) nTriggersPerPOHBias = atoi(tempCalibObject->parameterValue("nTriggersPerPOHBias").c_str());

  std::cout << "[DEBUG] AOHBiasMin = " << AOHBiasMin << std::endl;
  std::cout << "[DEBUG] AOHBiasMax = " << AOHBiasMax << std::endl;
  std::cout << "[DEBUG] AOHBiasStepSize = " << AOHBiasStepSize << std::endl;
  std::cout << "[DEBUG] AOHGainMin = " << AOHGainMin << std::endl;
  std::cout << "[DEBUG] AOHGainMax = " << AOHGainMax << std::endl;
  std::cout << "[DEBUG] AOHGainStepSize = " << AOHGainStepSize << std::endl;
  std::cout << "[DEBUG] nTriggersPerPOHBias = " << nTriggersPerPOHBias << std::endl;

  
  for (unsigned int AOHBias = AOHBiasMin; AOHBias <= AOHBiasMax; AOHBias += AOHBiasStepSize){
      
    // Set AOH bias
    Attribute_Vector parametersToTKFEC(1);
    parametersToTKFEC[0].name_="AOHBias"; parametersToTKFEC[0].value_=itoa(AOHBias);
    commandToAllTKFECCrates("SetAOHBiasEnMass", parametersToTKFEC);

    std::cout << "[DEBUG] POH bias = " << AOHBias << std::endl;

    for (unsigned int AOHGain=AOHGainMin; AOHGain <= AOHGainMax; ++AOHGain){

      Attribute_Vector parametersToTKFEC_Gain(1);
      parametersToTKFEC_Gain[0].name_="AOHGain"; parametersToTKFEC_Gain[0].value_=itoa(AOHGain);
      commandToAllTKFECCrates("SetAOHGainEnMass", parametersToTKFEC_Gain);
      
      std::cout << "[DEBUG] POH gain = " << AOHGain << std::endl;

      for (unsigned int i_event=0; i_event < nTriggersPerPOHBias; ++i_event){

	// Send trigger to all TBMs and ROCs.
	sendTTCCalSync();
	
	Attribute_Vector parametersToFED(3);
	parametersToFED[0].name_ = "WhatToDo"; 
	parametersToFED[1].name_ = "AOHBias"; 
	parametersToFED[2].name_ = "AOHGain"; 
	parametersToFED[0].value_ = "RetrieveData";
	parametersToFED[1].value_ = itoa(AOHBias);
	parametersToFED[2].value_ = itoa(AOHGain);
	commandToAllFEDCrates("FEDCalibrations", parametersToFED);
      }

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

  //Yuta and Lea: added portcard writing
  //Write out the configs
  /*  for (std::map<std::string, std::map<unsigned int, unsigned int> >::iterator portCardName_itr = bias_values_by_portcard_and_aoh_new.begin(); portCardName_itr != bias_values_by_portcard_and_aoh_new.end(); portCardName_itr++){
    std::string portCardName = portCardName_itr->first;
    std::map<std::string, PixelPortCardConfig*>::iterator mapNamePortCard_itr = getmapNamePortCard()->find(portCardName);
    assert( mapNamePortCard_itr != getmapNamePortCard()->end());
    PixelPortCardConfig* thisPortCardConfig = mapNamePortCard_itr->second;
    for(std::map<unsigned int, unsigned int >::iterator AOHNumber_itr = portCardName_itr->second.begin(); AOHNumber_itr != portCardName_itr->second.end(); AOHNumber_itr++){
      unsigned int AOHNumber = AOHNumber_itr->first;
      unsigned int AOHBiasAddress = thisPortCardConfig->AOHBiasAddressFromAOHNumber(AOHNumber);
      thisPortCardConfig->setdeviceValues(AOHBiasAddress, bias_values_by_portcard_and_aoh_new[portCardName][AOHNumber]);
	}
    thisPortCardConfig->writeASCII(outputDir());
    cout << "Wrote the portcard config for port card: " << portCardName << endl;
    } */
  ///



}

std::vector<std::string> PixelPOHBiasCalibration::calibrated() {
  std::vector<std::string> tmp;
  tmp.push_back("portcard");
  return tmp;
}
