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
  
  Copyright 2002 - 2003, Universite de Haute-Alsace, Mulhouse-France, Institut de Recherche Subatomiques de Strasborug
*/
#ifndef PARAMETERDESCRIPTION_H
#define PARAMETERDESCRIPTION_H

#include "tscTypes.h"
#include "stringConv.h"
#include "hashMapDefinition.h" // For hash_map

/** Class which map a name to a value, usually used to map a XML tag to a given value that can be converted through the methods provided here
 * and depending on the enumeration given in enumTscType
 */
class ParameterDescription {

 public:

  /** Enumeration
   */
  typedef enum{INTEGER8, INTEGER16, INTEGER32, REAL, STRING} enumTscType ;

  /** Nothing
   */
  ParameterDescription ( ) { 
    valConvert_ = NULL ;
  }

  /** Build a parameter
   * \param name - name of the parameter
   * \param typeN - type of the parameter
   * \param value_ - value_ of the parameter
   */
  ParameterDescription ( std::string name, enumTscType typeN, std::string value = "" ): 
  name_ (name), type_ (typeN), value_ (value) {
    valConvert_ = NULL ;
  }

  /** destructor
   */
  ~ParameterDescription ( ) {
    deleteValConvert() ;
  }

  /**
   */
  void setName ( std::string name ) {
    name_ = name ;
  }

  /**
   */
  std::string getName (  ) {
    return (name_) ;
  }

  /**
   */
  void setType ( enumTscType typeN ) {
    type_ = typeN ;
  }

  /**
   */
  enumTscType getType ( ) {
    return (type_) ;
  }

  /**
   */
  void setValue ( std::string value ) {

    value_ = value;

  }

  /**
   */
  std::string getValue ( ) {
    return (value_) ;
  }

  /**
   */
  void *getValueConverted ( ) {

    deleteValConvert() ;

    // Create the corresponding parameter
    setValConvert() ;

    return (valConvert_) ;
  }

  /**
   */
  unsigned long getTypeSize ( ) {

    unsigned long sizeN = 0 ;
    switch (type_) {
    case INTEGER8:
      sizeN = sizeof(tscType8) ;
      break ;
    case INTEGER16:
      sizeN = sizeof(tscType16) ;
      break ;
    case INTEGER32:
      sizeN = sizeof(tscType32) ;
      break ;
    case REAL:
      sizeN = sizeof(double) ;
      break ;
    case STRING:
      sizeN = value_.length() ;
      break ;
    }
    return sizeN ;
  }

 private:
  /** Name of the parameter
   */
  std::string name_ ;

  /** type of the corresponding parameter
   */
  enumTscType type_ ;

  /** value_ of the corresponding parameter
   */
  std::string value_ ;

  /** conversion parameter in 1 byte
   */
  void *valConvert_ ;

  /** delete the void * pointer
   */
  void deleteValConvert( ) {

    if (valConvert_ != NULL) {
      // Create the corresponding parameter
      tscType8 *val8 ;
      tscType16 *val16 ;
      tscType32 *val32 ;
      double *valDble ;
      std::string *str ;
      switch (type_) {
      case INTEGER8:
	val8 = (tscType8 *)valConvert_ ;
	delete val8 ;
	break ;
      case INTEGER16:
	val16 = (tscType16 *)valConvert_ ;
	delete val16 ;
	break ;
      case INTEGER32:
	val32 = (tscType32 *)valConvert_ ;
	delete val32 ;
	break ;
      case REAL:
	valDble = (double *)valConvert_ ;
	delete valDble ;
	break ;
      case STRING:
	str = (std::string *)valConvert_ ;
	delete str ;
	break ;
      }
    }
  }

  /** make the conversion depending of the type_ and the value_ and put it in valConvert
   */
  void setValConvert ( ) {

    //std::cout << "DEBUG: " << __func__ << " > value: " << value_ << std::endl;

    // Create the corresponding parameter
    tscType8 *val8 ;
    tscType16 *val16 ;
    tscType32 *val32 ;
    double *valDble ;
    std::string *valString ;

    switch (type_) {
    case INTEGER8:
      valConvert_ = new tscType8 ;
      val8 = (tscType8 *)valConvert_ ;
      *val8 = (tscType8)fromString<unsigned short>(value_) ;
      break ;
    case INTEGER16:
      valConvert_ = new tscType16 ;
      val16 = (tscType16 *)valConvert_ ;
      *val16 = fromString<unsigned short>(value_) ;
      break ;
    case INTEGER32:
      valConvert_ = new tscType32 ;
      val32 = (tscType32 *)valConvert_ ;
      *val32 = fromString<unsigned long>(value_) ;
      break ;
    case REAL:
      valConvert_ = new double ;
      valDble = (double *)valConvert_ ;
      *valDble = fromString<double>(value_) ;
      //float tmp = *valDble;
      //std::cout << "DEBUG: " << __func__ << " > valConvert: " << *valDble << " " << tmp << " " << ((float *)valConvert_)  << std::endl;
      break ;
    case STRING:
      valConvert_ = new std::string (value_) ;
      valString = (std::string *)valConvert_ ;
      break ;
    }
  }
};

// Hash map

//typedef Sgi::hash_map<const char *, ParameterDescription *, Sgi::hash<const char*>, eqstr> parameterDescriptionNameType ;
typedef Sgi::hash_map<const char *, ParameterDescription *, eqstr, eqstr> parameterDescriptionNameType ;

#endif
