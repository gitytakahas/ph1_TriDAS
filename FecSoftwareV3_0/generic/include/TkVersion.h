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

#ifndef TKVERSION_H
#define TKVERSION_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"

#include "TkVersionDefinition.h"


/**
 * \class TkVersion
 * Interface for versions
 * 
 *
 *
 */
class TkVersion {

 private:

  std::string versionName_; 
  std::string versionDate_; 
  unsigned int versionMajorId_; 
  unsigned int versionMinorId_;
  std::string comments_;  
  int registered_;

  public:
  
  /** Constructor
   */
  TkVersion ( std::string versionName, std::string versionDate, unsigned int versionMajorId, unsigned int versionMinorId, int registered, std::string comments ):
    versionName_(versionName),
    versionDate_(versionDate),
    versionMajorId_(versionMajorId),
    versionMinorId_(versionMinorId),
    comments_(comments),
    registered_(registered)
    {
    }

  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkVersion ( parameterDescriptionNameType parameterNames ):
    versionName_(""),
    versionDate_(""),
    versionMajorId_(0),
    versionMinorId_(0),
    comments_(""),
    registered_(-1)
    {
      setVersionName((std::string)parameterNames[TKVERSIONPARAMETERNAMES[VERSIONNAME]]->getValue());
      setVersionDate((std::string)parameterNames[TKVERSIONPARAMETERNAMES[VERSIONDATE]]->getValue());
      setVersionMajorId(*((int*) parameterNames[TKVERSIONPARAMETERNAMES[VERSIONMAJORID]]->getValueConverted()));
      setVersionMinorId(*((int*)parameterNames[TKVERSIONPARAMETERNAMES[VERSIONMINORID]]->getValueConverted()));
      setComments((std::string)parameterNames[TKVERSIONPARAMETERNAMES[COMMENTS]]->getValue());
      setRegistered(*((int*)parameterNames[TKVERSIONPARAMETERNAMES[VERSIONREGISTERED]]->getValueConverted()));
  }
    
 

  /** Get the version name
   */
  std::string getVersionName ( ) {
    return (versionName_) ;
  }

  /**Set the version name
   */
  void setVersionName ( std::string n) {
    versionName_ = n ;
  }

  /** Get the version creation date
   */
  std::string getVersionDate ( ) {
    return (versionDate_) ;
  }

  /** Set the version date
   */
  void setVersionDate ( std::string n) {
    versionDate_ = n ;
  }

  /** Get the versionMajorId
   */
  unsigned int getVersionMajorId ( ) {
    return (versionMajorId_) ;
  }

  /** Set the versionMajorId
   */
  void setVersionMajorId ( unsigned int v ) {
    versionMajorId_ = v ;
  }

  /** Get the versionMinorId
   */
  unsigned int getVersionMinorId ( ) {
    return (versionMinorId_) ;
  }

  /** Set the versionMinorId
   */
  void setVersionMinorId ( unsigned int v ) {
    versionMinorId_ = v ;
  }

  /** Get the comment
   */
  std::string getComments ( ) {
    return (comments_) ;
  }

  /**Set the comments
   */
  void setComments ( std::string n) {
    comments_ = n ;
  }

  /** Get the registered value
   */
  int getRegistered ( ) {
    return (registered_) ;
  }

  /** Set the registered value
   */
  void setRegistered ( int r ) {
    registered_ = r ;
  }



  /** brief Clone TK DCU info description
   * \return the device description cloned
   */
  TkVersion *clone (){
    
    TkVersion *version = new TkVersion( versionName_,versionDate_, versionMajorId_, versionMinorId_, registered_, comments_);
    return (version) ;
  }

  /** \brief Display the Dcu Info
   */
  void display ( ) {

    std::cout<< "Version Name : "<<versionName_<< std::endl ;
    std::cout<< "Version Date : "<<versionDate_<< std::endl ;
    std::cout<< "Major version : "<<versionMajorId_<< std::endl ;
    std::cout<< "Minor version : "<<versionMinorId_<< std::endl ;
    std::cout<< "Registered : "<<registered_<< std::endl ;
    std::cout<< "Comments : "<<comments_<< std::endl ;

  }

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) {
    
    parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
    
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONNAME]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[VERSIONNAME], ParameterDescription::STRING) ;
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONDATE]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[VERSIONDATE], ParameterDescription::STRING) ;
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONMAJORID]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[VERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONMINORID]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[VERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKVERSIONPARAMETERNAMES[COMMENTS]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[COMMENTS], ParameterDescription::STRING) ;
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONREGISTERED]] = new ParameterDescription(TKVERSIONPARAMETERNAMES[VERSIONREGISTERED], ParameterDescription::INTEGER32) ;
    // Set default values in case some informations are missing in the XML
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONNAME]]->setValue("NONE");
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONDATE]]->setValue("NONE");
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKVERSIONPARAMETERNAMES[COMMENTS]]->setValue("NONE");
    (*parameterNames)[TKVERSIONPARAMETERNAMES[VERSIONREGISTERED]]->setValue("-1");
    
    return parameterNames ;
  }

  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) {
    deviceDescription::deleteParameterNames ( parameterNames ) ;
  }

} ;

#endif
