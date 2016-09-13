/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
  //#define DEBUGMSGERROR

#ifndef TKDCUCONVERTTIONFACTORS_H
#define TKDCUCONVERTTIONFACTORS_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"

#include "TkDcuInfo.h"
#include "dcuDescription.h"

/**
 * \class TkDcuConversionFactors
 * Interface for DCU on CCU or DCU on FEH
 * 
 * Information
 *
 * dcuHardId: DCU hardware ID as read by ProgramTest.exe
 * etDcuId: string with the 3 fields of the DCU hard id registers => this can be extracted from the DCU hard ID
 * subDetector: TIB, TID, TOB, TEC
 * dcuType: CCUM or FEH
 * stuctureId: ID of the structure (PETAL, ROD, STRING, DISK)
 * containerId: moduleId or ccuId: ID of the module
 *
 * Conversion data
 *
 * adcGain0: Gain of the ADC channel 0
 * adcOffset0: Offset of the ADC channel 0
 * adcInl0: Integral non linearity of ADC channel 0
 * adcInl0OW: Integral non linearity overflow flag of ADC channel 0 (true or false)
 * adcCal0: Calibration status flag for ADC channel 0 (true if correct, false if not)
 * 
 * i20: 20 uA current 
 * i10: 10 uA current
 * iCal: Calibraton status flag for i20 and i10 (true if correct, false if not)
 *
 * kDiv: Voltage divider ratio in FEH
 * kDivCal: Calibraton status flag for kDiv (true if correct, false if not)
 *
 * tsGain: Gain of the Integrated Temperature Sensor
 * tsOffset: Offset of the Integrated Temperature Sensor
 * tsCal: Calibraton status flag for the Integrated Temperature Sensor (true if correct, false if not)
 *
 * r68: 68 kOhm resistor in TOB CCUMs
 * r68Cal: Calibraton status flag for r68 (true if correct, false if not)
 *
 * adcGain2: Gain of the ADC channel 2
 * adcOffset2: Offset of the ADC channel 2
 * adcCal2: Calibration status flag for ADC channel 2 (true if correct, false if not)
 *
 * adcGain3: Gain of the ADC channel 3
 * adcOffset3: Offset of the ADC channel 3
 * adcCal3: Calibration status flag for ADC channel 3 (true if correct, false if not)
 *
 * For the TOB/TEC FEH
 *     0                 Temperature of each Silicon Sensor:  value = log((adcCountChannel0 - adc0Offset_) / (adc0Gain_ * i20_ * (RTH0 / 2)) 
 *                                                            tSi   = 1/(1/T0 + value/BETA) ;
 *                                                            adc0Cal_ AND iCal_             = FALSE     => ERROR
 *
 *     1                 V250 through Voltage divider (kDiv)  v250  = (adcCountChannel1 - adc0Offset_) / (adc0Gain_ * kDiv_)
 *                                                            adc0Cal_ AND kDivCal_          = FALSE     => ERROR
 *
 *     2                 V125                                 v125  = (adcCountChannel2 - adc0Offset_)/ adc0Gain_
 *                                                            adc0Cal_                       = FALSE     => ERROR
 * 
 *     3                 Leakage current (I Leak)             iLeak = (adcCountChannel3 - adc0Offset_)/ (adc0Gain_ * RLEAK) 
 *                                                            adc0Cal_                       = FALSE     => ERROR
 *
 *     4                 Temperature of each hybrid           value = log((adcCountChannel4 - adc0Offset_) / (adc0Gain_ * i10_ * RTH0)
 *                                                            tHyb  = 1/(1/T0 + value/BETA) ;
 *                                                            adc0Cal_ AND iCal_             = FALSE     => ERROR
 *
 *     5                 Not Used                
 *     6                 Not Used                
 *
 *     7                 Temperature of the DCU               tDcu  = (adcCountChannel7 - tsOffset) / tsGain_ + 25
 *                                                            adc0Cal_                       = FALSE     => ERROR
 *
 * For the TIB FEH
 *     0                 Temperature of each Silicon Sensor:  value = log((adcCountChannel0 - adc0Offset_) / (adc0Gain_ * i20_ * RTH0) 
 *                                                            tSi   = 1/(1/T0 + value/BETA) ;
 *                                                            adc0Cal_ AND iCal_             = FALSE     => ERROR
 *
 *     1                 V250 through Voltage divider (kDiv)  v250  = (adcCountChannel1 - adc0Offset_) / (adc0Gain_ * kDiv_)
 *                                                            adc0Cal_ AND kDivCal_          = FALSE     => ERROR
 *
 *     2                 V125                                 v125  = (adcCountChannel2 - adc0Offset_)/ adc0Gain_
 *                                                            adc0Cal_                       = FALSE     => ERROR
 * 
 *     3                 Leakage current (I Leak)             iLeak = (adcCountChannel3 - adc0Offset_)/ (adc0Gain_ * RLEAK) 
 *                                                            adc0Cal_                       = FALSE     => ERROR
 *
 *     4                 Temperature of each hybrid           value = log((adcCountChannel4 - adc0Offset_) / (adc0Gain_ * i10_ * RTH0)
 *                                                            tHyb  = 1/(1/T0 + value/BETA) ;
 *                                                            adc0Cal_ AND iCal_             = FALSE     => ERROR
 *
 *     5                 Not Used                
 *     6                 Not Used                
 *
 *     7                 Temperature of the DCU               tDcu  = (adcCountChannel7 - tsOffset) / tsGain_ + 25
 *                                                            adc0Cal_                       = FALSE     => ERROR
 *
 *
 * For TOB DCU on CCU
 *     0            Temperature of one of the silicon sensor: value = log((adcCountChannel0 - adc0Offset_) / (adc0Gain_ * i20_ * ((RTH0 * r68_) / (RTH0 + r68_)))) 
 *                                                            tSi   = 1/(1/T0 + value/BETA) ;
 *                                                            adc0Cal_ AND iCal_ AND r68Cal_ = FALSE     => ERROR
 *
 *     1            Not Used
 *
 *     2            V250                                    : v250  = (adcCountChannel2 - adc2Offset_)/ adc2Gain_
 *                                                            adc2Cal_                       = FALSE     => ERROR
 *
 *     3            V125                                    : v125  = (adcCountChannel3 - adc3Offset_)/ adc3Gain_
 *                                                            adc3Cal_                       = FALSE     => ERROR
 *
 *     4            Not Used
 *     5            Not Used
 *     6            Not Used
 *
 *     7            Temperature of the DCU                  : tDcu  = (adcCountChannel7 - tsOffset) / tsGain_ + 25
 *                                                            adc0Cal_                       = FALSE     => ERROR
 *
 * For TEC DCU on CCU
 *     0            Temperature of the input cooling pipe   : not yet defined
 *
 *     1            Temperature of the output cooling pipe  : not yet defined
 *
 *     2            Humidity sensor + (to be substracted with channel 3): not yet defined
 *
 *     3            Humidity sensor - (to be substracted with channel 2): not yet defined
 *
 *     4            Not Used
 *     5            Not Used
 *     6            Not Used
 *
 *     7            Temperature of the DCU
 *
 *
 * For TIB DCU on CCU
 *     0                 Not Used
 *     1                 Not Used
 *     2                 Not Used
 *     3                 Not Used
 *     4                 Not Used
 *     5                 Not Used
 *     6                 Not Used
 *     7                 Temperature of the DCU
 * 
 */
class TkDcuConversionFactors {

 private:

  /** Thermistor resistance at T0 = 25 C (in ohm)
   */
  //static const double RTH0    = 10000  ;
#define RTH0 10000

  /** Thermistor temperature coefficient (in K^-1)
   */
  //static const double BETA    = 3280   ;
#define BETA 3280

  /** Sensing resistor for the detector leakage current (in ohm)
   */
  //static const double RLEAK   = 681    ;
#define RLEAK 681

  /** Reference temperature
   */
#define TZEROCELSIUS 25
  
  /** Reference temperature T0 = 25 C (in K) 
   */
  //static const double T0      = 298.15 ;
#define T0 298.15

  /** Integrated temperature sensor gain (in ADC count / K)
   */
  //static const double GAINTS  = 8.901  ;
#define GAINTS 8.901

  /** Difference between kelvin and celcuis
   */
  //static const double DIFFKELCEL = 273.15 ;
#define DIFFKELCEL 273.15
  
  /** DCU Hard ID
   */
  tscType32 dcuHardId_ ;

  /** Sub detectors
   */
  std::string subDetector_ ;
  
  /** DCU type (DCU on CCU or DCU on FEH)
   */
  std::string dcuType_ ;

  /** Struture ID (Petal, ROD, String ID)
   */
  std::string structureId_ ;

  /** Module ID or CCU ID
   */
  std::string containerId_ ; 

  /** Detector ID for the offline
   */
  unsigned int detId_ ;

  // ----------------------------------------
  // -- Conversion factors
  // ----------------------------------------

  /**
   */
  double adcGain0_ ;

  /**
   */
  double adcOffset0_ ;

  /**
   */
  bool adcCal0_ ;

  /**
   */
  double adcInl0_ ;

  /**
   */
  bool adcInl0OW_ ;

  /**
   */
  double i20_ ;

  /**
   */
  double i10_ ;

  /**
   */
  bool iCal_ ;

  /** 
   */
  double kDiv_ ;

  /**
   */
  bool kDivCal_ ;

  /**
   */
  double tsGain_ ;

  /**
   */
  double tsOffset_ ;

  /**
   */
  bool tsCal_ ;

  /**
   */
  double r68_ ;

  /**
   */
  bool r68Cal_ ;

  /**
   */
  double adcGain2_ ;

  /**
   */
  double adcOffset2_ ;

  /**
   */
  bool adcCal2_ ;

  /**
   */
  double adcGain3_ ;
  
  /**
   */
  double adcOffset3_ ;

  /**
   */
  bool adcCal3_ ;

  /** DCU conversion: to avoid clone and copy of memory, the data are directly stored in values
   */
  //dcuDescription *dcuDescription_ ;

  /** DCU Hard ID from the DCU description
   */
  unsigned int dcuDescriptionDcuHardId_ ;

  /** DCU type
   */
  std::string dcuDescriptionDcuType_ ;

  /** Key from the DCU description
   */
  unsigned int dcuDescriptionKey_ ;

  /** Timestamp from the DCU description
   */
  unsigned int dcuDescriptionTimeStamp_ ;
      
  /** Channel values from the DCU description
   */
  unsigned int dcuDescriptionChannels_[MAXDCUCHANNELS] ;

  public:

  // TRUE or FALSE
#define STRTRUE  "T"
#define STRFALSE "F"

  /* Enumeration for the parameter's names
   */
  enum{DCUCONVERTHARDID, SUBDETECTOR, DCUTYPE,
	 STRUCTUREID, CONTAINERID, DETID,
	 ADCGAIN0, ADCOFFSET0, 
	 ADCINL0, ADCINL0OW, ADCCAL0, 
	 I20, I10, IDCUCAL, 
	 KDIV, KDIVCAL,
	 TSGAIN, TSOFFSET, TSCAL,
	 R68, R68CAL,
	 ADCGAIN2, ADCOFFSET2, ADCCAL2,
	 ADCGAIN3, ADCOFFSET3, ADCCAL3} ;

  /** List of parameter's names
   */
  static const char *DCUFACTORPARAMETERNAMES[ADCCAL3+1] ;
  
  /** Constructor
   */
  TkDcuConversionFactors ( tscType32 dcuHardId, std::string subDetector, std::string dcuType ) ;  
  /** Constructor for a DCU on CCU
   */
  
  /** Constructor for a DCU on FEH
   */

  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkDcuConversionFactors ( parameterDescriptionNameType parameterNames ) ;

  /** Nothing
   */
  ~TkDcuConversionFactors ( ) ;

  /** Set the description to be converted
   */
  void setDcuDescription ( dcuDescription *dcuDes )  ;
  
  /** Set the DCU hard ID
   */
  void setDcuHardId ( tscType32 dcuHardId ) ;

  /** Get the DCU hard ID
   */
  tscType32 getDcuHardId ( ) ;

  /** Set the sub detectors
   */
  void setSubDetector ( std::string subDetector ) ;

  /** Get the sub detectors
   */
  std::string getSubDetector ( ) ;

  /** Set the DCU type
   */
  void setDcuType ( std::string dcuType ) ;

  /** Get the DCU type
   */
  std::string getDcuType ( ) ;

  /** Set the structure ID
   */
  void setStructureId ( std::string structureId ) ;

  /** Get the structure ID
   */
  std::string getStructureId ( ) ;

  /** Set the module ID
   */
  void setContainerId ( std::string containerId ) ;

  /** Get the Container ID
   */
  std::string getContainerId ( ) ;

  /** Set the detector ID
   */
  void setDetId ( unsigned int detId ) ;

  /** Get the detector ID
   */
  unsigned int getDetId ( ) ;

  /** Set 
   */
  void setAdcGain0 ( double adcGain0 ) ;

  /** Set 
   */
  void setAdcOffset0 ( double adcOffset0 ) ;

  /** Set 
   */
  void setAdcCal0( bool adcCal0 ) ;

  /** Set
   */
  void setAdcInl0OW ( bool adcInl0OW ) ;

  /** Set
   */
  void setAdcInl0 ( double adcInl0 ) ;

  /** Set
   */
  void setI20( double i20 ) ;

  /** Set
   */
  void setI10( double i10 ) ;

  /** Set
   */
  void setICal( bool iCal ) ;

  /** Set
   */
  void setKDiv( double kDiv ) ;

  /** Set
   */
  void setKDivCal( bool kDivCal ) ;
	
  /** Set
   */
  void setTsGain( double tsGain ) ;

  /** Set
   */
  void setTsOffset( double tsOffset ) ;

  /** Set
   */
  void setTsCal( bool tsCal ) ;

  /** Set
   */
  void setR68 ( double r68 ) ;

  /** Set
   */
  void setR68Cal ( bool r68Cal ) ;

  /** Set
   */
  void setAdcGain2( double adcGain2 ) ;

  /** Set
   */
  void setAdcOffset2( double adcOffset2 ) ;

  /** Set
   */
  void setAdcCal2( bool adcCal2 ) ;

  /** Set
   */
  void setAdcGain3( double adcGain3 ) ;
  
  /** Set
   */
  void setAdcOffset3( double adcOffset3 ) ;

  /** Set 
   */
  void setAdcCal3 ( bool adcCal3 ) ;

  /** Get 
   */
  double getAdcGain0( ) ;

  /** Get 
   */
  double getAdcOffset0( ) ;

  /** Get 
   */
  bool getAdcCal0( ) ;

  /** Get
   */
  bool getAdcInl0OW ( ) ;

  /** Get
   */
  double getAdcInl0 ( ) ;
	
  /** Get
   */
  double getI20( ) ;

  /** Get
   */
  double getI10( ) ;

  /** Get
   */
  bool getICal( ) ;

  /** Get
   */
  double getKDiv( ) ;

  /** Get
   */
  bool getKDivCal( ) ;
	
  /** Get
   */
  double getTsGain( ) ;

  /** Get
   */
  double getTsOffset( ) ;

  /** Get
   */
  bool getTsCal( ) ;

  /** Get
   */
  double getR68 ( ) ;

  /** Set
   */
  bool getR68Cal ( ) ;

  /** Get
   */
  double getAdcGain2( ) ;

  /** Get
   */
  double getAdcOffset2( ) ;

  /** Get
   */
  bool getAdcCal2( ) ;

  /** Get
   */
  double getAdcGain3( ) ;

    /** Get
   */
  double getAdcOffset3( ) ;

  /** Get 
   */
  bool getAdcCal3 ( ) ;

  /** brief Clone TK DCU conversion factors description
   * \return the device description cloned
   */
  TkDcuConversionFactors *clone ( ) ;

  /** \brief Display the conversion factors
   * \param convertedValue - display the converted value with the DCU
   */
  void display ( bool convertedValue = false ) throw (std::string) ;

  /** \brief Display the conversion factors
   * \param convertedValue - display the converted value with the DCU
   */
  std::string getDisplay ( bool convertedValue = false ) throw (std::string) ;

  /** \brief Evaluate the silicon sensor temperature from the channel 0 for a FEH or a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return silicon sensor temperature
   * \warning return temperature are in degree celcuis
   */
  double getSiliconSensorTemperature ( bool &quality ) throw (std::string) ;

  /** \brief V250 from channel 1 for a FEH or from a channel 2 for a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return V250 in mV
   */
  double getV250 ( bool &quality ) throw (std::string) ;

  /** \brief V125 from channel 2 for a FEH or from channel 3 for a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return V125 in mV
   */
  double getV125 ( bool &quality ) throw (std::string) ;

  /** \brief Leakage current from channel 3 for a FEH, cannot be recovered for a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return Leakage current in uA
   */
  double getILeak ( bool &quality ) throw (std::string) ;

  /** \brief Temperature of each hybrid from channel 4 of FEH, cannnot be recovered for a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return Temperature of the corresponding hybrid
   * \warning return temperature are in degree celcuis
   */
  double getHybridTemperature ( bool &quality ) throw (std::string) ;
      
  /** \brief Temperature of the DCU from channel 7 for a FEH or from channel 7 for a DCU on CCU
   * \param quality - data quality (good or bad)
   * \return Temperature of the DCU
   */
  double getDcuTemperature ( bool &quality ) throw (std::string) ;

  /** \brief check if the difference between two DCUs and return true if it reach a certain level of %
   * \param dcu - dcu to be compare
   * \param level - level in % to raise true
   * \return true or false if one of the values is greater than the level
   * \exception if the DCU hard id is different
   * \warning the DCU should be already associated !
   */
  bool isPourcentageDifferent ( dcuDescription *dcu, double pourcent, std::string &dcuChannelChanged ) ;

  /** \brief return the corresponding subdetector from the det id
   * \return the string with TIB, TIB, TOB, TEC+, TEC- and an exception if the det id is not recognized
   */
  std::string getSubDetectorFromDetId ( ) throw (std::string) ;

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;
  
  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;

// Initialise of static variable
//const double TkDcuConversionFactors::RTH0       = 10000  ;
//const double TkDcuConversionFactors::BETA       = 3280   ;
//const double TkDcuConversionFactors::RLEAK      = 681    ;
//const double TkDcuConversionFactors::T0         = 298.15 ;
//const double TkDcuConversionFactors::GAINTS     = 8.901  ;
//const double TkDcuConversionFactors::DIFFKELCEL = 273.15 ;
// End of initialisation

} ;

#endif
