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
#ifndef TKRINGDESCRIPTION_H
#define TKRINGDESCRIPTION_H
#include <string.h>

#include <list>
#include <algorithm> 

#include "keyType.h"
#include "FecExceptionHandler.h"
#include "ParameterDescription.h"
#include "CCUDescription.h"

/**
 * \class TkRingDescription
 * This class reflect the software point of view of the configuration of a Ring. No hardware access are done in this class.
 * \author Frederic Drouhin, Stefano Mersi
 * \date June 2007 
 * \brief This class give the current value for a ring
 * \warning This class do not access the hardware. Is created in order to keep
 * in memory the current state of a ring 
 * The attributs fecHardwareId_ are used only to retreive the information from the database
 */


// TODO: implement the = operator

class TkRingDescription {

 private: 

  /** VME FEC has identification number inside the hardware.
   */
  char fecHardwareId_[100] ;

  /** Crate ID
   */
  tscType16 crateId_ ;

  /** Address of the Ring 
   */
  keyType key_ ;

  /** This is to flag when a CCU is disabled
   *  because of a failure or any other reason
   */
  bool enabled_;

  /** This is a flag signaling (if true) that the
   *  output from the ring is standard (no dummy ccu needed)
   *  that corresponds to the FEC standard (A) input.
   *  If it's false then the B ring output is used.
   */
  bool outputAUsed_;

  /** This is a flag signaling (if true) that the
   *  input from the ring is standard (no dummy ccu needed)
   *  that corresponds to the FEC standard (A) output 
   *  If it's false then the B ring input is used.
   */
  bool inputAUsed_;

  /** Dummy CCU description
   */
  CCUDescription* dummyCcu_ ;

  /** Vector of CCUs
   */
  //ccuVector ccuVector_ ; cannot be used to the description in deviceType.h
  std::vector<CCUDescription *> ccuVector_ ;

  /** Return the dummy CCU
   */
  CCUDescription* getDummyCcu() ;

  /** Clear the vector of CCUs
   */
  void clearCcuVector ( ) ;

  /** The reason why the redundancy is not applicable
   *  if it is actually not applicable
   */
  std::string whyNotReconfigurable_;

 public:

  /** Enumeration of list of name of parameter
   */
  enum{INPUTAUSED, OUTPUTAUSED};

  /** Parameter's names
   */
  static const char *RINGPARAMETERNAMES[OUTPUTAUSED+1] ;

  /** Number of parameters
   */
  static const unsigned int RINGNUMBEROFPARAMETERS ; // = 7;

  /** Set all the channel to not initialise
   * \brief Constructor that create a Ring
   */
  TkRingDescription ( tscType16 crateId, keyType key, bool enabled, bool inAused, bool outAused) ;

  /** \brief Create a CCU description from XML buffer
   * \param parameterNames - list of parameter
   * \see ParameterDescription class
   * \warning an exception is raised if one the parameter is not correct
   */
  TkRingDescription ( parameterDescriptionNameType parameterNames ) ;

  /** Copy constructor defined here explicitely and should be due to the vector of pointer on CCUs
   */
  TkRingDescription ( TkRingDescription &sourceRing ) ;
  
  /** Destructor, destroy the vector of CCUs
   */
  ~TkRingDescription ( ) ;


  /** \brief Sets the CCUDescription vector
   */
  void setCcuVector (std::vector<CCUDescription *> &newCcuVector) ;

  /* \brief retrives the (supposedly ordered) vector of
   * CCUDescription in the ring
   */
  std::vector<CCUDescription *>* getCcuVector( ) ;

  /* \brief checks if the dummy ccu connection is of TIB inverted type
   */
  bool isDummyInverted( ) ;

  /* \brief checks if the dummy ccu is present
   */
  bool hasDummyCcu ( ) {if (dummyCcu_!=NULL) return true; return false;};

  /** \brief Clone a tkring description
   * \return the tkring cloned
   */
  TkRingDescription *clone ( ) ;

  /** \brief Return the current enabled parameter 
   * \return enabled
   */
  bool getEnabled ( ) ;

  /** Return the address of the tkring 
   * \return tkring address
   */
  keyType getKey ( ) ;

  /** \brief return the crate ID
   * \return the crate ID
   */
  tscType16 getCrateId ( ) ;

  /** \brief return the FEC slot
   * \return the FEC slot
   */
  inline tscType16 getFecSlot ( ) { return getFecKey(key_) ; }

  /** \brief return the ring
   * \return the ring
   */
  inline tscType16 getRingSlot ( ) { return getRingKey(key_) ; }

  /** \brief set the crate ID
   * \param crateId - ID of the crate to be set
   */
  void setCrateId ( tscType16 crateId ) ;

  /** \brief Set the address of the ring 
   * \param address - start from 0x1 to 0x7f
   */
  void setKey ( keyType key ) ;

  /** \brief Set the value of Enabled 
   * \param enabled (boolean)
   */
  void setEnabled ( bool enabled ) ;

 /** \brief Set the value of InputAUsed 
   * \param used (boolean) = true
   */
  void setInputAUsed ( bool used = true ) ;

 /** \brief Set the value of outputAUsed 
   * \param used (boolean) = true
   */
  void setOutputAUsed ( bool used = true ) ;

 /** \brief Set the value of InputAUsed 
   * opposite to the given value
   * \param used (boolean) = true
   */
  void setInputBUsed ( bool used = true ) ;

 /** \brief Set the value of outputAUsed 
   * opposite to the given value
   * \param used (boolean) = true
   */
  void setOutputBUsed ( bool used = true ) ;

 /** \brief Returns the value of InputAUsed 
   */
  bool getInputAUsed () ;

 /** \brief Returns the value of OutputAUsed 
   */
  bool getOutputAUsed () ;

 /** \brief Returns the opposite of the value of InputAUsed 
   */
  bool getInputBUsed () ;

 /** \brief Returns the opposite of the value of OutputAUsed 
   */
  bool getOutputBUsed () ;

  /** \brief number of CCUs in the ring
   */
  unsigned int getNumberOfCcus() ;

  /** \brief Tells if the current configuration is possible
   */
  bool isReconfigurable() ;

  /** \brief Computes the reconfiguration path if possible
   *  otherwise it returns false
   */
  bool computeRedundancy();

  /** \brief Tells for what reason the ring was not reconfigurable
   */
  std::string getReconfigurationProblem ()  { return whyNotReconfigurable_ ; };

  // ***********************************************************************************************
  // FEC hardware ID
  // ***********************************************************************************************

  /** \brief Set the FEC hardware identification number
   */
  void setFecHardwareId ( std::string fecHardwareId, tscType16 crateId ) ;

  /** \brief return the FEC hardware identification number
   */
  std::string getFecHardwareId ( ) ;
 

  // ***********************************************************************************************
  // Display
  // ***********************************************************************************************

  /** Display the ring into a stream
   */
  void display ( std::ostream &flux, bool all = false , bool redundancyDetails = false) ;

  // ***********************************************************************************************
  // Static method
  // ***********************************************************************************************


  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) ;

  /** \brief Delete a list of parameter name but only its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) ;
};

// Vector of TkRingDescriptions
typedef std::vector<TkRingDescription *> tkringVector ;

#endif
